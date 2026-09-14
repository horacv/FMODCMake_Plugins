#include "ans_delay.hpp"

#include "ans_delay_state.hpp"
#include "ans_fmod_dsp.hpp"

#define FMOD_PLUGIN_NAME "ANS Delay"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 1
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

using namespace ans_fmod_dsp;

namespace ans_delay
{
    // DEFAULTS

    static constexpr auto PARAM_DELAY_NAME = "Delay";
    static constexpr auto PARAM_DELAY_LABEL = "ms";
    static constexpr auto PARAM_DELAY_DESC = "Delay in ms. 1 to 5 sec. Default = 500ms";
    static constexpr float PARAM_DELAY_DEFAULT = 500.0f;
    static constexpr float PARAM_DELAY_MAX = 5000.0f;
    static constexpr float PARAM_DELAY_MIN = 1.0f;

    static constexpr auto PARAM_WET_GAIN_NAME = "Gain Wet";
    static constexpr auto PARAM_WET_GAIN_LABEL = "dB";
    static constexpr auto PARAM_WET_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_WET_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_WET_GAIN_DB_MAX = 10.f;
    static constexpr float PARAM_WET_GAIN_DB_MIN = -80.f;

    static constexpr auto PARAM_DRY_GAIN_NAME = "Gain Dry";
    static constexpr auto PARAM_DRY_GAIN_LABEL = "dB";
    static constexpr auto PARAM_DRY_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_DRY_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_DRY_GAIN_DB_MAX = 10.f;
    static constexpr float PARAM_DRY_GAIN_DB_MIN = -80.f;

    static constexpr auto PARAM_FEEDBACK_NAME = "Feedback";
    static constexpr auto PARAM_FEEDBACK_LABEL = "%";
    static constexpr auto PARAM_FEEDBACK_DESC = "Feedback";
    static constexpr float PARAM_FEEDBACK_PERCENT_DEFAULT = 50.0f;
    static constexpr float PARAM_FEEDBACK_PERCENT_MAX = 100.f;
    static constexpr float PARAM_FEEDBACK_PERCENT_MIN = 0.f;
    
    static constexpr float MIN_THRESHOLD_DB = -96.f;
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
            init_param_desc_float(param_delay, PARAM_DELAY_NAME, PARAM_DELAY_LABEL,
                PARAM_DELAY_DESC, PARAM_DELAY_MIN, PARAM_DELAY_MAX, PARAM_DELAY_DEFAULT);
            init_param_desc_float(param_wet_gain, PARAM_WET_GAIN_NAME, PARAM_WET_GAIN_LABEL,
                PARAM_WET_GAIN_DESC, PARAM_WET_GAIN_DB_MIN, PARAM_WET_GAIN_DB_MAX, PARAM_WET_GAIN_DB_DEFAULT);
            init_param_desc_float(param_dry_gain, PARAM_DRY_GAIN_NAME, PARAM_DRY_GAIN_LABEL,
                PARAM_DRY_GAIN_DESC, PARAM_DRY_GAIN_DB_MIN, PARAM_DRY_GAIN_DB_MAX, PARAM_DRY_GAIN_DB_DEFAULT);
            init_param_desc_float(param_feedback, PARAM_FEEDBACK_NAME, PARAM_FEEDBACK_LABEL,
                PARAM_FEEDBACK_DESC, PARAM_FEEDBACK_PERCENT_MIN, PARAM_FEEDBACK_PERCENT_MAX, PARAM_FEEDBACK_PERCENT_DEFAULT);

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};
        init_data.m_delay_ms_default = PARAM_DELAY_DEFAULT;
        init_data.m_delay_ms_max = PARAM_DELAY_MAX;
        init_data.m_delay_ms_min = PARAM_DELAY_MIN;
        
        init_data.m_dry_gain_db_default = PARAM_DRY_GAIN_DB_DEFAULT;
        init_data.m_dry_gain_db_max = PARAM_DRY_GAIN_DB_MAX;
        init_data.m_dry_gain_db_min = PARAM_DRY_GAIN_DB_MIN;

        init_data.m_wet_gain_db_default = PARAM_WET_GAIN_DB_DEFAULT;
        init_data.m_wet_gain_db_max = PARAM_WET_GAIN_DB_MAX;
        init_data.m_wet_gain_db_min = PARAM_WET_GAIN_DB_MIN;

        init_data.m_feedback_percent_default = PARAM_FEEDBACK_PERCENT_DEFAULT;
        init_data.m_feedback_percent_max = PARAM_FEEDBACK_PERCENT_MAX;
        init_data.m_feedback_percent_min = PARAM_FEEDBACK_PERCENT_MIN;

        init_data.m_min_threshold_db = MIN_THRESHOLD_DB;
        init_data.m_smoothing_time_ms = SMOOTHING_TIME_MS;

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
            unsigned int buffer_size; FMOD_DSP_GETBLOCKSIZE(dsp_state, &buffer_size);
            int samplerate; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate);
            state->reset(buffer_size, samplerate);
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

            if (inputs_idle)
            {
                if (state->is_active())
                {
                    if (!state->is_fading_out())
                    {
                        state->start_fading_out();
                    }
                    return FMOD_OK;
                }
                return FMOD_ERR_DSP_DONTPROCESS;
            }

            if (!state->is_active())
            {
                state->set_active(true);
            }
            return FMOD_OK;
        }

        if (in_buffer_array && out_buffer_array)
        {
            return state->process(in_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], out_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], buffer_size,
                *in_buffer_array->buffernumchannels, *out_buffer_array->buffernumchannels);
        }

        return FMOD_OK;
    }

    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        switch (index)
        {
            case PARAM_DELAY:
                state->set_delay_ms(value);
                return FMOD_OK;
            case PARAM_WET_GAIN:
                state->set_wet_gain_db(value);
                return FMOD_OK;
            case PARAM_DRY_GAIN:
                state->set_dry_gain_db(value);
                return FMOD_OK;
            case PARAM_FEEDBACK:
                state->set_feedback_percent(value);
                return FMOD_OK;
            default:
                break;
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

        switch (index)
        {
            case PARAM_DELAY:
                *value = state->get_delay_ms();
                return write_float_value_string(value_str, *value);
            case PARAM_WET_GAIN:
                *value = state->get_wet_gain_db();
                return write_float_value_string(value_str, *value);
            case PARAM_DRY_GAIN:
                *value = state->get_dry_gain_db();
                return write_float_value_string(value_str, *value);
            case PARAM_FEEDBACK:
                *value = state->get_feedback_percent();
                return write_float_value_string(value_str, *value);
            default:
                break;
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