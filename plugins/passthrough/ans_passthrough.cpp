#include "ans_passthrough.hpp"

#include "ans_fmod_dsp.hpp"
#include "ans_passthrough_state.hpp"
#include <cstdio>

#define FMOD_PLUGIN_NAME "ANS Passthrough"
#define FMOD_PLUGIN_VERSION 0x00000100
#define FMOD_PLUGIN_BUFFERS_IN_NUM 1
#define FMOD_PLUGIN_BUFFERS_OUT_NUM 1
#define FMOD_MAIN_BUFFER_INDEX 0

namespace ans_passthrough
{
    // DEFAULTS

    // Float param
    static constexpr auto PARAM_FLOAT_NAME = "Float Param";
    static constexpr auto PARAM_FLOAT_LABEL = "%";
    static constexpr auto PARAM_FLOAT_DESC = "Float parameter in %. 0 to 100. Default = 0";
    static constexpr float PARAM_FLOAT_DEFAULT = 0.f;
    static constexpr float PARAM_FLOAT_MAX = 100.0f;
    static constexpr float PARAM_FLOAT_MIN = PARAM_FLOAT_DEFAULT;

    // Int param
    static constexpr auto PARAM_INT_NAME = "Int Param";
    static constexpr auto PARAM_INT_LABEL = "-Units";
    static constexpr auto PARAM_INT_DESC = "Int parameter in Units. 0 to 100. Default = 0";
    static constexpr bool PARAM_INT_INF = false;
    static constexpr int PARAM_INT_DEFAULT = 0;
    static constexpr int PARAM_INT_MAX = 100;
    static constexpr int PARAM_INT_MIN = 0;

    // Int enumerated param
    static constexpr auto PARAM_ENUM_NAME = "Int Enum Param";
    static constexpr auto PARAM_ENUM_LABEL = "";
    static constexpr auto PARAM_ENUM_DESC = "Int enum parameter";
    static constexpr int PARAM_ENUM_DEFAULT = 0;
    static const char* int_enum_param_values[3] = { "Opt 1", "Opt 2", "Opt 3" };

    // Bool param
    static constexpr auto PARAM_BOOL_NAME = "Bool Param";
    static constexpr auto PARAM_BOOL_LABEL = "";
    static constexpr auto PARAM_BOOL_DESC = "Bool parameter";
    static constexpr bool PARAM_BOOL_DEFAULT = false;
    static const char* bool_param_values[2] = { "Active", "Inactive" };

    // Custom data param
    static constexpr auto PARAM_CUSTOM_NAME = "User Data";
    static constexpr auto PARAM_CUSTOM_LABEL = "Custom Data";
    static constexpr auto PARAM_CUSTOM_DESC = "Custom data parameter";
    static constexpr int PARAM_CUSTOM_INT_DEFAULT = 0;

    static bool ans_passthrough_registered = false;

    using namespace ans_fmod_dsp;

    // USER DATA

    ANS_DSP_UserData::ANS_DSP_UserData(
        const char* name,
        const float value,
        const unsigned int id,
        const bool enabled)
        : name(""), id(id), value(value),enabled(enabled)
    {
        std::snprintf(this->name, sizeof(this->name), "%s", name);
    }

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
        .read = dsp_read, // This callback only works if 'process' is not provided.
        .process = dsp_process,
        .setposition = dsp_set_position,
        .numparameters = PARAM_COUNT,
        .paramdesc = param_description,
        .setparameterfloat = dsp_set_param_float,
        .setparameterint = dsp_set_param_int,
        .setparameterbool = dsp_set_param_bool,
        .setparameterdata = dsp_set_param_data,
        .getparameterfloat = dsp_get_param_float,
        .getparameterint = dsp_get_param_int,
        .getparameterbool = dsp_get_param_bool,
        .getparameterdata = dsp_get_param_data,
        .shouldiprocess = dsp_should_process, // This callback only works in conjunction with the 'read'.
        .userdata = &userdata,
        .sys_register = dsp_sys_register,
        .sys_deregister = dsp_sys_deregister,
        .sys_mix = dsp_sys_mix,
    };

    extern "C" {
        F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
        {
            init_param_desc_float(param_cutoff, PARAM_FLOAT_NAME, PARAM_FLOAT_LABEL, PARAM_FLOAT_DESC, PARAM_FLOAT_MIN, PARAM_FLOAT_MAX, PARAM_FLOAT_DEFAULT);
            init_param_desc_int(param_int, PARAM_INT_NAME, PARAM_INT_LABEL, PARAM_INT_DESC, PARAM_INT_MIN, PARAM_INT_MAX, PARAM_INT_DEFAULT, PARAM_INT_INF, nullptr);
            init_param_desc_int_enumerated(param_int_enum, PARAM_ENUM_NAME, PARAM_ENUM_LABEL, PARAM_ENUM_DESC, PARAM_ENUM_DEFAULT, int_enum_param_values);
            init_param_desc_bool(param_bool, PARAM_BOOL_NAME, PARAM_BOOL_LABEL, PARAM_BOOL_DESC, PARAM_BOOL_DEFAULT, bool_param_values);
            init_param_desc_data(param_data, PARAM_CUSTOM_NAME, PARAM_CUSTOM_LABEL, PARAM_CUSTOM_DESC, FMOD_DSP_PARAMETER_DATA_TYPE_USER);

            /* Initialize more parameters here ...*/

            return &fmod_plugin_description;
        }
    }

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state)
    {
        ANS_DSP_Init_Data init_data = {};
        init_data.m_param_bool_default = PARAM_BOOL_DEFAULT;
        init_data.m_param_data_default.value = PARAM_CUSTOM_INT_DEFAULT;
        init_data.m_param_float_default = PARAM_FLOAT_DEFAULT;
        init_data.m_param_int_default = PARAM_INT_DEFAULT;
        init_data.m_param_int_enum_default = PARAM_ENUM_DEFAULT;

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
            state->reset();
            return FMOD_OK;
        }
        return FMOD_ERR_INVALID_HANDLE;
    }

    FMOD_RESULT F_CALL dsp_read(FMOD_DSP_STATE* dsp_state,
        float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int* out_channels)
    {
        if (auto* state = get_dsp_state<ANS_DSP_State>(dsp_state))
        {
            return state->process(in_buffer, out_buffer, length, in_channels, *out_channels) ? FMOD_OK : FMOD_ERR_DSP_DONTPROCESS;
        }
        return FMOD_ERR_INVALID_HANDLE;
    }

    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int length,
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
            return state->process(in_buffer_array->buffers[0], out_buffer_array->buffers[0], length,
                *in_buffer_array->buffernumchannels, *out_buffer_array->buffernumchannels) ? FMOD_OK : FMOD_ERR_DSP_DONTPROCESS;
        }

        return FMOD_ERR_DSP_DONTPROCESS;
    }

    FMOD_RESULT F_CALL dsp_set_position(FMOD_DSP_STATE* dsp_state, unsigned int position)
    {
        if (auto* state = get_dsp_state<ANS_DSP_State>(dsp_state))
        {
            state->set_position(position);
            return FMOD_OK;
        }
        return FMOD_ERR_INVALID_HANDLE;
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
            state->set_param_float(value);
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

        if (index == PARAM_INT)
        {
            state->set_param_int(value);
            return FMOD_OK;
        }
        if (index == PARAM_INT_ENUM)
        {
            state->set_param_int_enum(value);
            return FMOD_OK;
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_set_param_bool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL value)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_BOOL)
        {
            state->set_param_bool(value);
            return FMOD_OK;
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_set_param_data(FMOD_DSP_STATE* dsp_state, int index, void* data, unsigned int length)
    {
        auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_DATA)
        {
            if (sizeof(ANS_DSP_CustomData) != length)
            {
                return FMOD_ERR_FILE_BAD;
            }
            state->set_param_data(*static_cast<ANS_DSP_CustomData*>(data));
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
            *value = state->get_param_float();
            return write_float_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str)
    {
        using namespace ans_fmod_dsp;

        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_INT)
        {
            *value = state->get_param_int();
            return write_int_value_string(value_str, *value);
        }
        if (index == PARAM_INT_ENUM)
        {
            *value = state->get_param_int_enum();
            return write_int_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_get_param_bool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL* value, char* value_str)
    {
        using namespace ans_fmod_dsp;

        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_BOOL)
        {
            *value = state->get_param_bool();
            return write_bool_value_string(value_str, *value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_get_param_data(FMOD_DSP_STATE* dsp_state, int index, void** value, unsigned int* length, char* value_str)
    {
        using namespace ans_fmod_dsp;

        const auto* state = get_dsp_state<ANS_DSP_State>(dsp_state);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (index == PARAM_DATA)
        {
            *value =  const_cast<ANS_DSP_CustomData*>(state->get_param_data());
            *length = sizeof(ANS_DSP_CustomData);
            return write_int_value_string(value_str, state->get_param_data()->value);
        }

        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL dsp_should_process(FMOD_DSP_STATE* dsp_state, FMOD_BOOL inputs_idle,
        unsigned int length, FMOD_CHANNELMASK in_mask, int in_channels, FMOD_SPEAKERMODE speaker_mode)
    {
        return inputs_idle ? FMOD_ERR_DSP_DONTPROCESS : FMOD_OK;
    }

    FMOD_RESULT F_CALL dsp_sys_register(FMOD_DSP_STATE* dsp_state)
    {
        ans_passthrough_registered = true;
        return FMOD_OK;
    }

    FMOD_RESULT F_CALL dsp_sys_deregister(FMOD_DSP_STATE* dsp_state)
    {
        ans_passthrough_registered = false;
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