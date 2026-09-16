#ifndef ANS_GAIN_TYPES_HPP
#define ANS_GAIN_TYPES_HPP

#include "ans_dsp_common.hpp"

namespace ans_gain
{
    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_gain_db_default;
        float m_gain_db_max;
        float m_gain_db_min;
        float m_smoothing_time_ms;

        unsigned int m_samplerate;
        unsigned int buffer_size;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_gain_lin_current(ans_dsp::db_to_linear(m_defaults.m_gain_db_default))
            , m_gain_lin_target(m_gain_lin_current)
            , m_samples_to_target(0)
        {}

        ANS_DSP_Init_Data m_defaults;

        float m_gain_lin_current;
        float m_gain_lin_target;
        unsigned int m_samples_to_target;

        void reset(const int samplerate)
        {
            m_defaults.m_samplerate = samplerate;
            m_gain_lin_current = m_gain_lin_target;
            m_samples_to_target = 0;
        }
    };
}

#endif
