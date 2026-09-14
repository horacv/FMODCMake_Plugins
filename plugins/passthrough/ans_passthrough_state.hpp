#ifndef ANS_PASSTHROUGH_STATE_HPP
#define ANS_PASSTHROUGH_STATE_HPP

#include "ans_passthrough_types.hpp"
#include "fmod_common.h"

namespace ans_passthrough
{
    class ANS_DSP_State {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data &init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset();

        void set_param_float(const float value) { m_data.m_param_float_current = value; }
        void set_param_int(const int value) { m_data.m_param_int_current = value; }
        void set_param_int_enum(const int value) { m_data.m_param_int_enum_current = value; }
        void set_param_bool(const bool value) { m_data.m_param_bool_current = value; }
        void set_param_data(const ANS_DSP_CustomData& value) { m_data.m_param_data_current = value; }

        [[nodiscard]] float get_param_float() const { return m_data.m_param_float_current; }
        [[nodiscard]] int get_param_int() const { return m_data.m_param_int_current; }
        [[nodiscard]] int get_param_int_enum() const { return m_data.m_param_int_enum_current; }
        [[nodiscard]] bool get_param_bool() const { return m_data.m_param_bool_current; }
        [[nodiscard]] const ANS_DSP_CustomData* get_param_data() const { return &m_data.m_param_data_current; }

        void set_position(const unsigned int position) { m_data.m_position = position; }
        [[nodiscard]] unsigned int get_position() const { return m_data.m_position; }

        FMOD_RESULT process(const float* in_buffer, float* out_buffer, unsigned int buffer_size, int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };
}

#endif
