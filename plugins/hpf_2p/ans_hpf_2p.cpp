#include  "ans_hpf_2p.hpp"

#include "ans_dsp_common.hpp"
#include "ans_fmod_dsp.hpp"
#include "ans_hpf_2p_state.hpp"

#define FMOD_PLUGIN_NAME "ANS HPF 2P"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 1
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

using namespace ans_fmod_dsp;

namespace ans_hpf_2p
{
    // DEFAULTS

    static constexpr auto PARAM_CUTOFF_NAME = "Cutoff";
    static constexpr auto PARAM_CUTOFF_LABEL = "Hz";
    static constexpr auto PARAM_CUTOFF_DESC = "Cutoff in Hz. 10Hz to 22kHz. Default = 2kHz";
    static constexpr float PARAM_CUTOFF_HZ_DEFAULT = 2000.f;
    static constexpr float PARAM_CUTOFF_HZ_MAX = 22000.f;
    static constexpr float PARAM_CUTOFF_HZ_MIN = 10.f;

    static constexpr auto PARAM_Q_NAME = "Q";
    static constexpr auto PARAM_Q_LABEL = "";
    static constexpr auto PARAM_Q_DESC = "Q. 0.1 to 10. Default = 0.7";
    static constexpr float PARAM_Q_DEFAULT = 0.7071f;
    static constexpr float PARAM_Q_MAX = 10.f;
    static constexpr float PARAM_Q_MIN = 0.1f;

    static constexpr float PARAM_SMOOTHING_TIME_MS = 5.8f;

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
            init_param_desc_float(param_cutoff, PARAM_CUTOFF_NAME, PARAM_CUTOFF_LABEL,
                PARAM_CUTOFF_DESC, PARAM_CUTOFF_HZ_MIN, PARAM_CUTOFF_HZ_MAX, PARAM_CUTOFF_HZ_DEFAULT);

            init_param_desc_float(param_Q, PARAM_Q_NAME, PARAM_Q_LABEL,
                PARAM_Q_DESC, PARAM_Q_MIN, PARAM_Q_MAX, PARAM_Q_DEFAULT);

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};
        init_data.m_Q_default = PARAM_Q_DEFAULT;
        init_data.m_Q_max = PARAM_Q_MAX;
        init_data.m_Q_min = PARAM_Q_MIN;
        init_data.m_cutoff_Hz_default = PARAM_CUTOFF_HZ_DEFAULT;
        init_data.m_cutoff_Hz_max = PARAM_CUTOFF_HZ_MAX;
        init_data.m_cutoff_Hz_min = PARAM_CUTOFF_HZ_MIN;
        init_data.m_smoothing_time_ms = PARAM_SMOOTHING_TIME_MS;

        unsigned int buffer_size_current; FMOD_DSP_GETBLOCKSIZE(dsp_state, &buffer_size_current);
        int samplerate_current; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate_current);
        init_data.buffer_size = buffer_size_current;
        init_data.samplerate = samplerate_current;

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
            unsigned int buffer_size_current; FMOD_DSP_GETBLOCKSIZE(dsp_state, &buffer_size_current);
            int samplerate_current; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate_current);
            state->reset(buffer_size_current, samplerate_current);
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
            int samplerate_current; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate_current);
            return state->process(in_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], out_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], buffer_size,
                samplerate_current, *in_buffer_array->buffernumchannels, *out_buffer_array->buffernumchannels);
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

        if (index == PARAM_CUTOFF)
        {
            state->set_cutoff_Hz(value);
            return FMOD_OK;
        }
        if (index == PARAM_Q)
        {
            state->set_Q(value);
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

        if (index == PARAM_CUTOFF)
        {
            *value = state->get_cutoff_Hz();
            return write_float_value_string(value_str, *value);
        }
        if (index == PARAM_Q)
        {
            *value = state->get_Q();
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