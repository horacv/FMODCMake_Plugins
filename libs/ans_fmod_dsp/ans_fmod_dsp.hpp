#ifndef ANS_FMOD_DSP_HPP
#define ANS_FMOD_DSP_HPP

#include "fmod.hpp"
#include <cassert>
#include <cstring>
#include <memory>
#include <string_view>

namespace ans_fmod_dsp
{
    /**
     * Helper functions for initializing FMOD DSP parameters and managing DSP state.
     */

    /**
     * Allocates memory for a DSP state structure of type T, constructs it in place,
     * and stores it in dsp_state->plugindata.
     *
     * @param dsp_state A pointer to the FMOD_DSP_STATE structure associated with the DSP.
     * @param out_state Receives the constructed state pointer on success, or nullptr on failure.
     * @param args Arguments forwarded to T's constructor.
     * @return FMOD_OK on success, FMOD_ERR_UNINITIALIZED if allocation failed.
     */
    template <typename T, typename... Args>
    [[nodiscard]] FMOD_RESULT alloc_create_dsp_state(FMOD_DSP_STATE* dsp_state, T*& out_state, Args&&... args)
    {
        assert(dsp_state != nullptr);
        assert(dsp_state->plugindata == nullptr);

        void* memory = FMOD_DSP_ALLOC(dsp_state, sizeof(T));
        if (!memory) [[unlikely]]
        {
            out_state = nullptr;
            dsp_state->plugindata = nullptr;
            return FMOD_ERR_UNINITIALIZED;
        }

        out_state = std::construct_at(static_cast<T*>(memory), std::forward<Args>(args)...);
        dsp_state->plugindata = out_state;
        return FMOD_OK;
    }

    /**
     * Destroys and frees the DSP state structure of type T stored in dsp_state->plugindata.
     *
     * @param dsp_state A pointer to the FMOD_DSP_STATE structure associated with the DSP.
     * @return FMOD_OK on success, FMOD_ERR_UNINITIALIZED if plugindata was already null.
     */
    template <typename T>
    FMOD_RESULT free_destroy_dsp_state(FMOD_DSP_STATE* dsp_state)
    {
        assert(dsp_state != nullptr);

        T* state = static_cast<T*>(dsp_state->plugindata);
        if (!state)
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        dsp_state->plugindata = nullptr;
        std::destroy_at(state);
        FMOD_DSP_FREE(dsp_state, state);
        return FMOD_OK;
    }

    /**
     * Retrieves a pointer to the DSP state structure of type T associated with the given FMOD_DSP_STATE.
     *
     * @param dsp_state A pointer to the FMOD_DSP_STATE structure associated with the DSP.
     * @return A pointer to the DSP state structure of type T.
     */
    template <typename T>
    T* get_dsp_state(const FMOD_DSP_STATE* dsp_state)
    {
        assert(dsp_state != nullptr);
        return static_cast<T*>(dsp_state->plugindata);
    }

    /**
     * Initializes an integer parameter descriptor for an FMOD DSP plugin with enumerated values.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param default_value The default integer value for the parameter when the DSP is initialized.
     * @param value_names An array of null-terminated strings representing the names of the enumerated integer states.
     */
    template <std::size_t EnumCount>
    void init_param_desc_int_enumerated(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name,
        const std::string_view label, const char* description, const int default_value, const char* const (&value_names)[EnumCount])
    {
        static_assert(EnumCount > 0);

        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_INT;
        std::memcpy(param.name,  name.data(), sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description  = description;
        param.intdesc.min = 0;
        param.intdesc.max = static_cast<int>(EnumCount - 1);
        param.intdesc.defaultval = default_value;
        param.intdesc.goestoinf  = false;
        param.intdesc.valuenames = value_names;
    }

    /**
     * Initializes a floating-point parameter descriptor for an FMOD DSP plugin with a piecewise linear mapping.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param default_value The default value for the parameter when the DSP is initialized.
     * @param values An array of float values representing the mapped values.
     * @param positions An array of float values representing the positions of the mapped values.
     */
    template <std::size_t ValueCount>
    void init_param_desc_float_with_mapping(FMOD_DSP_PARAMETER_DESC& param, const std::string_view name,
        const std::string_view label, const char* description, const float default_value,
        float (&values)[ValueCount],
        float (&positions)[ValueCount])
    {
        static_assert(ValueCount > 0);

        param = {};
        param.type = FMOD_DSP_PARAMETER_TYPE_FLOAT;
        std::memcpy(param.name,  name.data(), sizeof(param.name) - 1);
        std::memcpy(param.label, label.data(), sizeof(param.label) - 1);
        param.description  = description;
        param.floatdesc.min = values[0];
        param.floatdesc.max = values[ValueCount - 1];
        param.floatdesc.defaultval = default_value;
        param.floatdesc.mapping.type = FMOD_DSP_PARAMETER_FLOAT_MAPPING_TYPE_PIECEWISE_LINEAR;
        param.floatdesc.mapping.piecewiselinearmapping.numpoints = static_cast<int>(ValueCount);
        param.floatdesc.mapping.piecewiselinearmapping.pointparamvalues = values;
        param.floatdesc.mapping.piecewiselinearmapping.pointpositions = positions;
    }

    /**
     * Initializes a floating-point parameter descriptor for an FMOD DSP plugin.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param min The minimum allowable value for the parameter.
     * @param max The maximum allowable value for the parameter.
     * @param default_value The default value for the parameter when the DSP is initialized.
     */
    void init_param_desc_float(FMOD_DSP_PARAMETER_DESC& param, std::string_view name,
        std::string_view label, const char* description, float min, float max, float default_value);

    /**
     * Initializes an integer parameter descriptor for an FMOD DSP plugin.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param min The minimum allowable value for the parameter.
     * @param max The maximum allowable value for the parameter.
     * @param default_value The default value for the parameter when the DSP is initialized.
     * @param goes_to_inf A boolean indicating whether the parameter can represent infinite values.
     * @param value_names An array of null-terminated strings representing the names for each enumerated value, or nullptr if not applicable.
     */
    void init_param_desc_int(FMOD_DSP_PARAMETER_DESC& param, std::string_view name, std::string_view label,
        const char* description, int min, int max, int default_value, bool goes_to_inf,const char* const* value_names);

    /**
     * Initializes a boolean parameter descriptor for an FMOD DSP plugin.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param default_value The default boolean value for the parameter when the DSP is initialized.
     * @param value_names An array of null-terminated strings representing the names of the boolean states (e.g., "Off" and "On").
     */
    void init_param_desc_bool(FMOD_DSP_PARAMETER_DESC& param, std::string_view name,
        std::string_view label, const char* description, bool default_value, const char* const* value_names);

    /**
     * Initializes a data parameter descriptor for an FMOD DSP plugin.
     *
     * @param param A reference to the FMOD_DSP_PARAMETER_DESC structure to initialize.
     * @param name The name of the parameter as a string view.
     * @param label The label for the parameter, typically used in UI elements, as a string view.
     * @param description A pointer to a null-terminated string containing a description of the parameter.
     * @param data_type The type of data represented by the parameter, specified as an FMOD_DSP_PARAMETER_DATA_TYPE value.
     */
    void init_param_desc_data(FMOD_DSP_PARAMETER_DESC& param, std::string_view name,
        std::string_view label, const char* description, FMOD_DSP_PARAMETER_DATA_TYPE data_type);

    /**
     * Converts a boolean value into a string representation and writes it into the provided buffer.
     *
     * @param buffer A pointer to a character buffer where the resulting string ("True" or "False") will be written.
     *               The buffer must have a capacity of at least FMOD_DSP_GETPARAM_VALUESTR_LENGTH characters.
     * @param value The boolean value to convert. If true, "True" is written to the buffer;
     *              if false, "False" is written.
     */
    FMOD_RESULT write_bool_value_string(char* buffer, bool value);

    /**
     * Writes the string representation of a floating-point value to a buffer.
     *
     * @param buffer A pointer to the character buffer where the string representation of the value will be written.
     *               The buffer must be large enough to hold the resulting string, including the null terminator.
     * @param value The floating-point value to convert to a string.
     */
    FMOD_RESULT write_float_value_string(char* buffer, float value);

    /**
     * Converts an integer value into a string and writes it into the provided buffer.
     *
     * @param buffer A pointer to a character array where the string representation of the integer will be written.
     *               The buffer should have a size of at least FMOD_DSP_GETPARAM_VALUESTR_LENGTH.
     * @param value The integer value to convert to a string.
     */
    FMOD_RESULT write_int_value_string(char* buffer, int value);
}

#endif