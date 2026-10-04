#include "ans_tone_generator.hpp"

#include "ans_fmod_dsp.hpp"
#include "ans_tone_generator_state.hpp"

#define FMOD_PLUGIN_NAME "ANS Tone Generator"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 0
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

using namespace ans_fmod_dsp;

namespace ans_tone_generator
{
    // DEFAULTS

    static constexpr auto PARAM_FREQ_NAME = "Freq.";
    static constexpr auto PARAM_FREQ_LABEL = "Hz";
    static constexpr auto PARAM_FREQ_DESC = "Frequency in Hz. 20 to 20000. Default = 440";
    static constexpr float PARAM_FREQ_HZ_DEFAULT = 440.f;
    static float param_frequency_mapping_values[] = { 20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f };
    static float param_frequency_mapping_scale[]  = { 0.f, 1.32f, 2.32f, 3.32f, 4.64f, 5.64f, 6.64f, 7.97f, 8.97f, 9.97f };

    static constexpr auto PARAM_GAIN_NAME = "Gain";
    static constexpr auto PARAM_GAIN_LABEL = "dB";
    static constexpr auto PARAM_GAIN_DESC = "Gain in dB. -80 to 10. Default = 0";
    static constexpr float PARAM_GAIN_DB_DEFAULT = -6.f;
    static constexpr float PARAM_GAIN_DB_MAX = 10.f;
    static constexpr float PARAM_GAIN_DB_MIN = -80.f;

    static constexpr auto PARAM_OSC_TYPE_NAME = "Osc Type";
    static constexpr auto PARAM_OSC_TYPE_LABEL = "";
    static constexpr auto PARAM_OSC_TYPE_DESC = "Waveform type";
    static const char* PARAM_OSC_TYPE_VALUES[5] = { "Sine", "Square", "Saw", "Triangle", "Noise" };
    static constexpr OSC_TYPE PARAM_OSC_TYPE_DEFAULT = SAW;

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
        .reset = nullptr,
        .read = nullptr,
        .process = dsp_process, // In a plugin with no input, this function 'generates' instead.
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
        .shouldiprocess = nullptr,
        .userdata = nullptr,
        .sys_register = nullptr,
        .sys_deregister = nullptr,
        .sys_mix = nullptr,
    };

    extern "C" {
        F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
        {
            init_param_desc_float_with_mapping(param_freq, PARAM_FREQ_NAME, PARAM_FREQ_LABEL,
                PARAM_FREQ_DESC, PARAM_FREQ_HZ_DEFAULT, param_frequency_mapping_values, param_frequency_mapping_scale);
            init_param_desc_float(param_gain, PARAM_GAIN_NAME, PARAM_GAIN_LABEL,
                PARAM_GAIN_DESC, PARAM_GAIN_DB_MIN, PARAM_GAIN_DB_MAX, PARAM_GAIN_DB_DEFAULT);
            init_param_desc_int_enumerated(param_osc_type, PARAM_OSC_TYPE_NAME, PARAM_OSC_TYPE_LABEL,
                PARAM_OSC_TYPE_DESC, PARAM_OSC_TYPE_DEFAULT, PARAM_OSC_TYPE_VALUES);

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};
        init_data.m_freq_Hz_default = PARAM_FREQ_HZ_DEFAULT;
        init_data.m_gain_default = PARAM_GAIN_DB_DEFAULT;
        init_data.m_oscillator_type_default = PARAM_OSC_TYPE_DEFAULT;

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

    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int length,
        const FMOD_DSP_BUFFER_ARRAY*, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL, FMOD_DSP_PROCESS_OPERATION operation)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (operation == FMOD_DSP_PROCESS_QUERY)
        {
            if (out_buffer_array)
            {
                out_buffer_array->buffernumchannels[FMOD_MAIN_BUFFER_INDEX] = 1;
                out_buffer_array->speakermode = FMOD_SPEAKERMODE_MONO;
            }

            return FMOD_OK;
        }

        if (out_buffer_array)
        {
            return state->generate(out_buffer_array->buffers[FMOD_MAIN_BUFFER_INDEX], length,
                out_buffer_array->buffernumchannels[FMOD_MAIN_BUFFER_INDEX]);
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

        if (index == PARAM_FREQ)
        {
            state->set_freq_Hz(value);
            return FMOD_OK;
        }
        if (index == PARAM_GAIN)
        {
            state->set_gain_db(value);
            return FMOD_OK;
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_set_param_int(FMOD_DSP_STATE* dsp_state, int index, int value)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_OSC_TYPE)
        {
            state->set_osc_type(static_cast<OSC_TYPE>(value));
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

        if (index == PARAM_FREQ)
        {
            *value = state->get_freq_Hz();
            return write_float_value_string(value_str, *value);
        }
        if (index == PARAM_GAIN)
        {
            *value = state->get_gain_db();
            return write_float_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str)
    {
        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_OSC_TYPE)
        {
            *value = state->get_osc_type();
            return write_int_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }
}

#undef FMOD_PLUGIN_NAME
#undef FMOD_PLUGIN_VERSION
#undef FMOD_PLUGIN_BUFFERS_IN_NUM
#undef FMOD_PLUGIN_BUFFERS_OUT_NUM
#undef FMOD_MAIN_BUFFER_INDEX