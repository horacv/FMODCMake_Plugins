#include "ans_gain.hpp"

#include "ans_fmod_dsp.hpp"
#include "ans_gain_state.hpp"

#define FMOD_PLUGIN_NAME "ANS Gain"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 1
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

using namespace ans_fmod_dsp;

namespace ans_gain
{
    // DEFAULTS

    static constexpr auto PARAM_GAIN_NAME = "Gain";
    static constexpr auto PARAM_GAIN_LABEL = "dB";
    static constexpr auto PARAM_GAIN_DESC = "Gain in dB. -80 to 10. Default = 0";
    static float param_gain_mapping_values[] = { -80.f, -50.f, -30.f, -10.f, 10.f };
    static float param_gain_mapping_scale[] = { 0, 2, 4, 7, 11 };

    static constexpr float PARAM_GAIN_DB_DEFAULT = 0.0f;
    static constexpr float PARAM_GAIN_DB_MAX = 10.0f;
    static constexpr float PARAM_GAIN_DB_MIN = -80.0f;

    static constexpr float SMOOTHING_TIME_MS = 5.8f;

    // PLUGIN DESCRIPTION

    static FMOD_DSP_DESCRIPTION fmod_plugin_description
    {
        .pluginsdkversion = FMOD_PLUGIN_SDK_VERSION,
        .name = FMOD_PLUGIN_NAME,
        .version = FMOD_PLUGIN_VERSION,
        .numinputbuffers = FMOD_PLUGIN_BUFFERS_IN_NUM,
        .numoutputbuffers = FMOD_PLUGIN_BUFFERS_OUT_NUM,
        .create = dsp_create,
        .release = dsp_release,
        .reset = dsp_reset,
        .read = nullptr, // Disabled legacy 'read' callback in favor of 'process'
        .process = dsp_process,
        .setposition = nullptr,
        .numparameters = PARAM_COUNT,
        .paramdesc = param_description,
        .setparameterfloat = dsp_set_param_float,
        .setparameterint = nullptr,
        .setparameterbool = nullptr,
        .setparameterdata = nullptr,
        .getparameterfloat = dsp_get_param_float,
        .getparameterint = nullptr,
        .getparameterbool = nullptr,
        .getparameterdata = nullptr,
        .shouldiprocess = nullptr, // Disabled legacy 'read' callback in favor of 'process'
        .userdata = nullptr,
        .sys_register = dsp_sys_register,
        .sys_deregister = dsp_sys_deregister,
        .sys_mix = dsp_sys_mix,
    };

    // DSP INTERFACE

    extern "C" {
        F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
        {
            init_param_desc_float_with_mapping(param_gain, PARAM_GAIN_NAME, PARAM_GAIN_LABEL,
                PARAM_GAIN_DESC, PARAM_GAIN_DB_DEFAULT, param_gain_mapping_values, param_gain_mapping_scale);

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};
        init_data.m_gain_db_default = PARAM_GAIN_DB_DEFAULT;
        init_data.m_gain_db_max = PARAM_GAIN_DB_MAX;
        init_data.m_gain_db_min = PARAM_GAIN_DB_MIN;
        init_data.m_smoothing_time_ms = SMOOTHING_TIME_MS;

        unsigned int buffer_size; FMOD_DSP_GETBLOCKSIZE(dsp_state, &buffer_size);
        init_data.buffer_size = buffer_size;
        int samplerate; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate);
        init_data.m_samplerate = samplerate;

        ANS_DSP_State* state = nullptr;
        return alloc_create_dsp_state<ANS_DSP_State>(dsp_state, state, init_data);
    }

    FMOD_RESULT F_CALL dsp_release(FMOD_DSP_STATE* dsp_state)
    {
        return free_destroy_dsp_state<ANS_DSP_State>(dsp_state);
    }

    FMOD_RESULT F_CALL dsp_reset(FMOD_DSP_STATE* dsp_state)
    {
        if (auto* state = get_dsp_state<ANS_DSP_State>(dsp_state))
        {
            int samplerate; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate);
            state->reset(samplerate);
            return FMOD_OK;
        }
        return FMOD_ERR_INVALID_HANDLE;
    }

    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int buffer_size,
        const FMOD_DSP_BUFFER_ARRAY* in_buffer_array, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL inputs_idle, FMOD_DSP_PROCESS_OPERATION operation)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (operation == FMOD_DSP_PROCESS_QUERY)
        {
            if (in_buffer_array && out_buffer_array)
            {
                out_buffer_array->buffernumchannels[FMOD_MAIN_BUFFER_INDEX] = in_buffer_array->buffernumchannels[FMOD_MAIN_BUFFER_INDEX];
                out_buffer_array->speakermode = in_buffer_array->speakermode;
            }

            return inputs_idle ? FMOD_ERR_DSP_DONTPROCESS : FMOD_OK;
        }

        if (in_buffer_array && out_buffer_array)
        {
            return state->process(in_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], out_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], buffer_size,
                *in_buffer_array->buffernumchannels, *out_buffer_array->buffernumchannels) ? FMOD_OK : FMOD_ERR_DSP_DONTPROCESS;
        }

        return FMOD_ERR_DSP_DONTPROCESS;
    }

    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        int samplerate_current;
        FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate_current);

        if (index == PARAM_GAIN)
        {
            state->set_gain_db(value);
            return FMOD_OK;
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_get_param_float(FMOD_DSP_STATE* dsp_state, int index, float* value, char* value_str)
    {
        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_GAIN)
        {
            *value = state->get_gain_db();
            return write_float_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_sys_register(FMOD_DSP_STATE* dsp_state)
    {
        return FMOD_OK;
    }

    FMOD_RESULT F_CALL dsp_sys_deregister(FMOD_DSP_STATE* dsp_state)
    {
        return FMOD_OK;
    }

    FMOD_RESULT F_CALL dsp_sys_mix(FMOD_DSP_STATE* dsp_state, int stage)
    {
        // stage == 0 , before all dsps are processed/mixed, this callback is called once for this type.
        // stage == 1 , after all dsps are processed/mixed, this callback is called once for this type.
        return FMOD_OK;
    }
}

#undef FMOD_PLUGIN_NAME
#undef FMOD_PLUGIN_VERSION
#undef FMOD_PLUGIN_BUFFERS_IN_NUM
#undef FMOD_PLUGIN_BUFFERS_OUT_NUM
#undef FMOD_MAIN_BUFFER_INDEX