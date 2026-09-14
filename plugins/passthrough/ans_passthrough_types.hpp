#ifndef ANS_PASSTHROUGH_TYPES_HPP
#define ANS_PASSTHROUGH_TYPES_HPP

namespace ans_passthrough
{
    struct ANS_DSP_UserData {
        ANS_DSP_UserData(const char* name, float value, unsigned int id, bool enabled);

        char name[256];
        unsigned int id;
        float value;
        bool enabled;
    };

    struct ANS_DSP_CustomData {
        int value;
    };

    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_param_float_default;
        int m_param_int_default;
        int m_param_int_enum_default;
        bool m_param_bool_default;
        ANS_DSP_CustomData m_param_data_default;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_param_float_current(init_data.m_param_float_default)
            , m_param_int_current(init_data.m_param_int_default)
            , m_param_int_enum_current(init_data.m_param_int_enum_default)
            , m_param_bool_current(init_data.m_param_bool_default)
            , m_param_data_current(init_data.m_param_data_default)
            , m_position(0) {
        }

        ANS_DSP_Init_Data m_defaults;

        float m_param_float_current;
        int m_param_int_current;
        int m_param_int_enum_current;
        bool m_param_bool_current;
        ANS_DSP_CustomData m_param_data_current;
        unsigned int m_position;

        void reset()
        {
            m_param_float_current = m_defaults.m_param_float_default;
            m_param_int_current = m_defaults.m_param_int_default;
            m_param_int_enum_current = m_defaults.m_param_int_enum_default;
            m_param_bool_current = m_defaults.m_param_bool_default;
            m_param_data_current.value = m_defaults.m_param_data_default.value;
        }
    };
}

#endif
