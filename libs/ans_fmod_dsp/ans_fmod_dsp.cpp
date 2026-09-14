#include "ans_fmod_dsp.hpp"

#include <cstdio>

namespace ans_fmod_dsp
{
    void init_param_desc_float(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name,
        const std::string_view label, const char* description, const float min, const float max, const float default_value)
    {
        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_FLOAT;
        std::memcpy(param.name,  name.data() , sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description = description;
        param.floatdesc.min = min;
        param.floatdesc.max = max;
        param.floatdesc.defaultval = default_value;
        param.floatdesc.mapping.type = FMOD_DSP_PARAMETER_FLOAT_MAPPING_TYPE_AUTO;
    }

    void init_param_desc_int(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name, const std::string_view label,
        const char* description, const int min, const int max, const int default_value,
        const bool goes_to_inf, const char* const* value_names)
    {
        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_INT;
        std::memcpy(param.name,  name.data() , sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description = description;
        param.intdesc.min = min;
        param.intdesc.max = max;
        param.intdesc.defaultval = default_value;
        param.intdesc.goestoinf = goes_to_inf;
        param.intdesc.valuenames = value_names;
    }

    void init_param_desc_bool(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name,
        const std::string_view label, const char* description, const bool default_value, const char* const* value_names)
    {
        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_BOOL;
        std::memcpy(param.name,  name.data() , sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description = description;
        param.booldesc.defaultval = default_value;
        param.booldesc.valuenames = value_names;
    }

    void init_param_desc_data(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name,
        const std::string_view label, const char* description, const FMOD_DSP_PARAMETER_DATA_TYPE data_type)
    {
        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_DATA;
        std::memcpy(param.name,  name.data(), sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description = description;
        param.datadesc.datatype = data_type;
    }

    FMOD_RESULT write_bool_value_string(char* buffer, const bool value)
    {
        if (buffer == nullptr)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
        return std::snprintf(buffer, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, value ? "True" : "False") < 0
            ? FMOD_ERR_INVALID_PARAM
            : FMOD_OK;
    }

    FMOD_RESULT write_float_value_string(char* buffer, const float value)
    {
        if (buffer == nullptr)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
        return std::snprintf(buffer, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%f", value) < 0
            ? FMOD_ERR_INVALID_PARAM
            : FMOD_OK;
    }

    FMOD_RESULT write_int_value_string(char* buffer, const int value)
    {
        if (buffer == nullptr)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
        return std::snprintf(buffer, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", value) < 0
            ? FMOD_ERR_INVALID_PARAM
            : FMOD_OK;
    }
}