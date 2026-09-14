#include  "ans_eq_4b.hpp"

#include "ans_dsp_common.hpp"
#include "ans_eq_4b_state.hpp"
#include "ans_fmod_dsp.hpp"

#define FMOD_PLUGIN_NAME "ANS EQ 4B"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 1
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

using namespace ans_fmod_dsp;

namespace ans_eq_4b
{
    // DEFAULTS

    // LOW BAND
    static constexpr auto PARAM_L_FREQ_NAME = "Freq. L";
    static constexpr auto PARAM_L_FREQ_LABEL = "Hz";
    static constexpr auto PARAM_L_FREQ_DESC = "Cutoff in Hz. 10Hz to 400Hz. Default = 10kHz";
    static constexpr float PARAM_L_FREQ_HZ_DEFAULT = 10.f;
    static constexpr float PARAM_L_FREQ_HZ_MAX = 500.f;
    static constexpr float PARAM_L_FREQ_HZ_MIN = 10.f;

    static constexpr auto PARAM_L_TYPE_NAME = "Type L";
    static constexpr auto PARAM_L_TYPE_LABEL = "";
    static constexpr auto PARAM_L_TYPE_DESC = "High pass or Low shelf";
    static const char* PARAM_L_TYPE_VALUES[2] = { "HPF", "Shelf" };
    static constexpr FILTER_TYPE PARAM_L_TYPE_DEFAULT = LOW_OR_HIGH_PASS;

    static constexpr auto PARAM_L_GAIN_NAME = "Gain L";
    static constexpr auto PARAM_L_GAIN_LABEL = "dB";
    static constexpr auto PARAM_L_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_L_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_L_GAIN_DB_MAX = 30.f;
    static constexpr float PARAM_L_GAIN_DB_MIN = -30.f;

    static constexpr auto PARAM_L_Q_NAME = "Q L";
    static constexpr auto PARAM_L_Q_LABEL = "";
    static constexpr auto PARAM_L_Q_DESC = "Q. 0.1 to 10. Default = 0.7";
    static constexpr float PARAM_L_Q_DEFAULT = 0.7071f;
    static constexpr float PARAM_L_Q_MAX = 10.f;
    static constexpr float PARAM_L_Q_MIN = 0.1f;

    // LOW MID BAND
    static constexpr auto PARAM_LM_FREQ_NAME = "Freq. LM";
    static constexpr auto PARAM_LM_FREQ_LABEL = "Hz";
    static constexpr auto PARAM_LM_FREQ_DESC = "Center frequency in Hz";
    static constexpr float PARAM_LM_FREQ_HZ_DEFAULT = 1000.f;
    static constexpr float PARAM_LM_FREQ_HZ_MAX = 2000.f;
    static constexpr float PARAM_LM_FREQ_HZ_MIN = 100.f;

    static constexpr auto PARAM_LM_GAIN_NAME = "Gain LM";
    static constexpr auto PARAM_LM_GAIN_LABEL = "dB";
    static constexpr auto PARAM_LM_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_LM_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_LM_GAIN_DB_MAX = 30.f;
    static constexpr float PARAM_LM_GAIN_DB_MIN = -30.f;

    static constexpr auto PARAM_LM_Q_NAME = "Q LM";
    static constexpr auto PARAM_LM_Q_LABEL = "";
    static constexpr auto PARAM_LM_Q_DESC = "Q. 0.1 to 10. Default = 0.7";
    static constexpr float PARAM_LM_Q_DEFAULT = 0.7071f;
    static constexpr float PARAM_LM_Q_MAX = 10.f;
    static constexpr float PARAM_LM_Q_MIN = 0.1f;

    // HIGH MID BAND
    static constexpr auto PARAM_HM_FREQ_NAME = "Freq. HM";
    static constexpr auto PARAM_HM_FREQ_LABEL = "Hz";
    static constexpr auto PARAM_HM_FREQ_DESC = "Center frequency in Hz";
    static constexpr float PARAM_HM_FREQ_HZ_DEFAULT = 4000.f;
    static constexpr float PARAM_HM_FREQ_HZ_MAX = 8000.f;
    static constexpr float PARAM_HM_FREQ_HZ_MIN = 500.f;

    static constexpr auto PARAM_HM_GAIN_NAME = "Gain HM";
    static constexpr auto PARAM_HM_GAIN_LABEL = "dB";
    static constexpr auto PARAM_HM_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_HM_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_HM_GAIN_DB_MAX = 30.f;
    static constexpr float PARAM_HM_GAIN_DB_MIN = -30.f;

    static constexpr auto PARAM_HM_Q_NAME = "Q HM";
    static constexpr auto PARAM_HM_Q_LABEL = "";
    static constexpr auto PARAM_HM_Q_DESC = "Q. 0.1 to 10. Default = 0.7";
    static constexpr float PARAM_HM_Q_DEFAULT = 0.7071f;
    static constexpr float PARAM_HM_Q_MAX = 10.f;
    static constexpr float PARAM_HM_Q_MIN = 0.1f;

    // HIGH BAND
    static constexpr auto PARAM_H_FREQ_NAME = "Freq. H";
    static constexpr auto PARAM_H_FREQ_LABEL = "Hz";
    static constexpr auto PARAM_H_FREQ_DESC = "Cutoff in Hz. 10Hz to 400Hz. Default = 10kHz";
    static constexpr float PARAM_H_FREQ_HZ_DEFAULT = 22000.f;
    static constexpr float PARAM_H_FREQ_HZ_MAX = 22000.f;
    static constexpr float PARAM_H_FREQ_HZ_MIN = 2000.f;

    static constexpr auto PARAM_H_TYPE_NAME = "Type H";
    static constexpr auto PARAM_H_TYPE_LABEL = "";
    static constexpr auto PARAM_H_TYPE_DESC = "Low pass or High shelf";
    static const char* PARAM_H_TYPE_VALUES[2] = { "LPF", "Shelf" };
    static constexpr FILTER_TYPE PARAM_H_TYPE_DEFAULT = LOW_OR_HIGH_PASS;

    static constexpr auto PARAM_H_GAIN_NAME = "Gain H";
    static constexpr auto PARAM_H_GAIN_LABEL = "dB";
    static constexpr auto PARAM_H_GAIN_DESC = "Gain in dB";
    static constexpr float PARAM_H_GAIN_DB_DEFAULT = 0.f;
    static constexpr float PARAM_H_GAIN_DB_MAX = 30.f;
    static constexpr float PARAM_H_GAIN_DB_MIN = -30.f;

    static constexpr auto PARAM_H_Q_NAME = "Q H";
    static constexpr auto PARAM_H_Q_LABEL = "";
    static constexpr auto PARAM_H_Q_DESC = "Q. 0.1 to 10. Default = 0.7";
    static constexpr float PARAM_H_Q_DEFAULT = 0.7071f;
    static constexpr float PARAM_H_Q_MAX = 10.f;
    static constexpr float PARAM_H_Q_MIN = 0.1f;

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
        .setparameterint = dsp_set_param_int,
        .setparameterbool = nullptr,
        .setparameterdata = nullptr,
        .getparameterfloat = dsp_get_param_float,
        .getparameterint = dsp_get_param_int,
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
            // LOW BAND
            init_param_desc_float(param_l_freq, PARAM_L_FREQ_NAME, PARAM_L_FREQ_LABEL,
                PARAM_L_FREQ_DESC, PARAM_L_FREQ_HZ_MIN, PARAM_L_FREQ_HZ_MAX, PARAM_L_FREQ_HZ_DEFAULT);
            init_param_desc_int_enumerated(param_l_type, PARAM_L_TYPE_NAME, PARAM_L_TYPE_LABEL,
                PARAM_L_TYPE_DESC, PARAM_L_TYPE_DEFAULT, PARAM_L_TYPE_VALUES);
            init_param_desc_float(param_l_gain, PARAM_L_GAIN_NAME, PARAM_L_GAIN_LABEL,
                PARAM_L_GAIN_DESC, PARAM_L_GAIN_DB_MIN, PARAM_L_GAIN_DB_MAX, PARAM_L_GAIN_DB_DEFAULT);
            init_param_desc_float(param_l_q, PARAM_L_Q_NAME, PARAM_L_Q_LABEL,
                PARAM_L_Q_DESC, PARAM_L_Q_MIN, PARAM_L_Q_MAX, PARAM_L_Q_DEFAULT);

            // LOW MID BAND
            init_param_desc_float(param_lm_freq, PARAM_LM_FREQ_NAME, PARAM_LM_FREQ_LABEL,
                PARAM_LM_FREQ_DESC, PARAM_LM_FREQ_HZ_MIN, PARAM_LM_FREQ_HZ_MAX, PARAM_LM_FREQ_HZ_DEFAULT);
            init_param_desc_float(param_lm_gain, PARAM_LM_GAIN_NAME, PARAM_LM_GAIN_LABEL,
                PARAM_LM_GAIN_DESC, PARAM_LM_GAIN_DB_MIN, PARAM_LM_GAIN_DB_MAX, PARAM_LM_GAIN_DB_DEFAULT);
            init_param_desc_float(param_lm_q, PARAM_LM_Q_NAME, PARAM_LM_Q_LABEL,
                PARAM_LM_Q_DESC, PARAM_LM_Q_MIN, PARAM_LM_Q_MAX, PARAM_LM_Q_DEFAULT);

            // HIGH MID BAND
            init_param_desc_float(param_hm_freq, PARAM_HM_FREQ_NAME, PARAM_HM_FREQ_LABEL,
                PARAM_HM_FREQ_DESC, PARAM_HM_FREQ_HZ_MIN, PARAM_HM_FREQ_HZ_MAX, PARAM_HM_FREQ_HZ_DEFAULT);
            init_param_desc_float(param_hm_gain, PARAM_HM_GAIN_NAME, PARAM_HM_GAIN_LABEL,
                PARAM_HM_GAIN_DESC, PARAM_HM_GAIN_DB_MIN, PARAM_HM_GAIN_DB_MAX, PARAM_HM_GAIN_DB_DEFAULT);
            init_param_desc_float(param_hm_q, PARAM_HM_Q_NAME, PARAM_HM_Q_LABEL,
                PARAM_HM_Q_DESC, PARAM_HM_Q_MIN, PARAM_HM_Q_MAX, PARAM_HM_Q_DEFAULT);

            // HIGH BAND
            init_param_desc_float(param_h_freq, PARAM_H_FREQ_NAME, PARAM_H_FREQ_LABEL,
                    PARAM_H_FREQ_DESC, PARAM_H_FREQ_HZ_MIN, PARAM_H_FREQ_HZ_MAX, PARAM_H_FREQ_HZ_DEFAULT);
            init_param_desc_float(param_h_q, PARAM_H_Q_NAME, PARAM_H_Q_LABEL,
                PARAM_H_Q_DESC, PARAM_H_Q_MIN, PARAM_H_Q_MAX, PARAM_H_Q_DEFAULT);
            init_param_desc_float(param_h_gain, PARAM_H_GAIN_NAME, PARAM_H_GAIN_LABEL,
                PARAM_H_GAIN_DESC, PARAM_H_GAIN_DB_MIN, PARAM_H_GAIN_DB_MAX, PARAM_H_GAIN_DB_DEFAULT);
            init_param_desc_int_enumerated(param_h_type, PARAM_H_TYPE_NAME, PARAM_H_TYPE_LABEL,
                PARAM_H_TYPE_DESC, PARAM_H_TYPE_DEFAULT, PARAM_H_TYPE_VALUES);

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};

        // LOW BAND
        init_data.m_l_Q_default = PARAM_L_Q_DEFAULT;
        init_data.m_l_Q_max = PARAM_L_Q_MAX;
        init_data.m_l_Q_min = PARAM_L_Q_MIN;
        init_data.m_l_filter_type_default = PARAM_L_TYPE_DEFAULT;
        init_data.m_l_freq_Hz_default = PARAM_L_FREQ_HZ_DEFAULT;
        init_data.m_l_freq_Hz_max = PARAM_L_FREQ_HZ_MAX;
        init_data.m_l_freq_Hz_min = PARAM_L_FREQ_HZ_MIN;
        init_data.m_l_gain_db_default = PARAM_L_GAIN_DB_DEFAULT;
        init_data.m_l_gain_db_max = PARAM_L_GAIN_DB_MAX;
        init_data.m_l_gain_db_min = PARAM_L_GAIN_DB_MIN;

        // LOW MID BAND
        init_data.m_lm_Q_default = PARAM_LM_Q_DEFAULT;
        init_data.m_lm_Q_max = PARAM_LM_Q_MAX;
        init_data.m_lm_Q_min = PARAM_LM_Q_MIN;
        init_data.m_lm_freq_Hz_default = PARAM_LM_FREQ_HZ_DEFAULT;
        init_data.m_lm_freq_Hz_max = PARAM_LM_FREQ_HZ_MAX;
        init_data.m_lm_freq_Hz_min = PARAM_LM_FREQ_HZ_MIN;
        init_data.m_lm_gain_db_default = PARAM_LM_GAIN_DB_DEFAULT;
        init_data.m_lm_gain_db_max = PARAM_LM_GAIN_DB_MAX;
        init_data.m_lm_gain_db_min = PARAM_LM_GAIN_DB_MIN;

        // HIGH MID BAND
        init_data.m_hm_Q_default = PARAM_HM_Q_DEFAULT;
        init_data.m_hm_Q_max = PARAM_HM_Q_MAX;
        init_data.m_hm_Q_min = PARAM_HM_Q_MIN;
        init_data.m_hm_freq_Hz_default = PARAM_HM_FREQ_HZ_DEFAULT;
        init_data.m_hm_freq_Hz_max = PARAM_HM_FREQ_HZ_MAX;
        init_data.m_hm_freq_Hz_min = PARAM_HM_FREQ_HZ_MIN;
        init_data.m_hm_gain_db_default = PARAM_HM_GAIN_DB_DEFAULT;
        init_data.m_hm_gain_db_max = PARAM_HM_GAIN_DB_MAX;
        init_data.m_hm_gain_db_min = PARAM_HM_GAIN_DB_MIN;

        // HIGH BAND
        init_data.m_h_Q_default = PARAM_H_Q_DEFAULT;
        init_data.m_h_Q_max = PARAM_H_Q_MAX;
        init_data.m_h_Q_min = PARAM_H_Q_MIN;
        init_data.m_h_filter_type_default = PARAM_H_TYPE_DEFAULT;
        init_data.m_h_freq_Hz_default = PARAM_H_FREQ_HZ_DEFAULT;
        init_data.m_h_freq_Hz_max = PARAM_H_FREQ_HZ_MAX;
        init_data.m_h_freq_Hz_min = PARAM_H_FREQ_HZ_MIN;
        init_data.m_h_gain_db_default = PARAM_H_GAIN_DB_DEFAULT;
        init_data.m_h_gain_db_max = PARAM_H_GAIN_DB_MAX;
        init_data.m_h_gain_db_min = PARAM_H_GAIN_DB_MIN;

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

        int samplerate; FMOD_DSP_GETSAMPLERATE(dsp_state, &samplerate);

        switch (index) {
            case PARAM_L_FREQ:
                state->set_freq_Hz(LOW, value);
                return FMOD_OK;
                break;
            case PARAM_L_GAIN:
                state->set_gain_db(LOW, value);
                return FMOD_OK;
                break;
            case PARAM_L_Q:
                state->set_Q(LOW, value);
                return FMOD_OK;
                break;
            case PARAM_LM_FREQ:
                state->set_freq_Hz(LOW_MID, value);
                return FMOD_OK;
                break;
            case PARAM_LM_GAIN:
                state->set_gain_db(LOW_MID, value);
                return FMOD_OK;
                break;
            case PARAM_LM_Q:
                state->set_Q(LOW_MID, value);
                return FMOD_OK;
                break;
            case PARAM_HM_FREQ:
                state->set_freq_Hz(HIGH_MID, value);
                return FMOD_OK;
                break;
            case PARAM_HM_GAIN:
                state->set_gain_db(HIGH_MID, value);
                return FMOD_OK;
                break;
            case PARAM_HM_Q:
                state->set_Q(HIGH_MID, value);
                return FMOD_OK;
                break;
            case PARAM_H_FREQ:
                state->set_freq_Hz(HIGH, value);
                return FMOD_OK;
                break;
            case PARAM_H_GAIN:
                state->set_gain_db(HIGH, value);
                return FMOD_OK;
                break;
            case PARAM_H_Q:
                state->set_Q(HIGH, value);
                return FMOD_OK;
                break;
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
    }

    FMOD_RESULT dsp_set_param_int(FMOD_DSP_STATE* dsp_state, int index, int value)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_L_TYPE) {
            state->set_filter_type(LOW, static_cast<FILTER_TYPE>(value));
            return FMOD_OK;
        }
        if (index == PARAM_H_TYPE) {
            state->set_filter_type(HIGH, static_cast<FILTER_TYPE>(value));
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

        switch (index) {
            case PARAM_L_FREQ:
                *value = state->get_freq_Hz(LOW);
                return FMOD_OK;
                break;
            case PARAM_L_GAIN:
                *value = state->get_gain_db(LOW);
                return FMOD_OK;
                break;
            case PARAM_L_Q:
                *value = state->get_Q(LOW);
                return FMOD_OK;
                break;
            case PARAM_LM_FREQ:
                *value = state->get_freq_Hz(LOW_MID);
                return FMOD_OK;
                break;
            case PARAM_LM_GAIN:
                *value = state->get_gain_db(LOW_MID);
                return FMOD_OK;
                break;
            case PARAM_LM_Q:
                *value = state->get_Q(LOW_MID);
                return FMOD_OK;
                break;
            case PARAM_HM_FREQ:
                *value = state->get_freq_Hz(HIGH_MID);
                return FMOD_OK;
                break;
            case PARAM_HM_GAIN:
                *value = state->get_gain_db(HIGH_MID);
                return FMOD_OK;
                break;
            case PARAM_HM_Q:
                *value = state->get_Q(HIGH_MID);
                return FMOD_OK;
                break;
            case PARAM_H_FREQ:
                *value = state->get_freq_Hz(HIGH);
                return FMOD_OK;
                break;
            case PARAM_H_GAIN:
                *value = state->get_gain_db(HIGH);
                return FMOD_OK;
                break;
            case PARAM_H_Q:
                *value = state->get_Q(HIGH);
                return FMOD_OK;
                break;
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
        
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str)
    {
        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_L_TYPE) {
            *value = state->get_filter_type(LOW);
            return write_int_value_string(value_str, *value);
        }
        if (index == PARAM_H_TYPE) {
            *value = state->get_filter_type(HIGH);
            return write_int_value_string(value_str, *value);
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