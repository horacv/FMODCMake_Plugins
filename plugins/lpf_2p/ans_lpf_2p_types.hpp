#ifndef ANS_LPF_2P_TYPES_HPP
#define ANS_LPF_2P_TYPES_HPP

#include "ans_dsp_common.hpp"
#include "ans_dsp_smoothing.hpp"
#include "fmod_common.h"
#include <array>
#include <atomic>

namespace ans_lpf_2p
{
    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_Q_default;
        float m_Q_max;
        float m_Q_min;
        float m_cutoff_Hz_default;
        float m_cutoff_Hz_max;
        float m_cutoff_Hz_min;
        float m_smoothing_time_ms;
        
        unsigned int buffer_size;
        unsigned int samplerate;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_smoothing_coefficient(0)
        {
            m_Q_current = m_defaults.m_Q_default;
            m_Q_smoothed = m_defaults.m_Q_default;
            m_cutoff_Hz_current = m_defaults.m_cutoff_Hz_default;
            m_cutoff_Hz_smoothed = m_defaults.m_cutoff_Hz_default;
            m_defaults.m_Q_default = std::clamp(init_data.m_Q_default, init_data.m_Q_min, init_data.m_Q_max);
            m_defaults.m_cutoff_Hz_default = std::clamp(init_data.m_cutoff_Hz_default,init_data.m_cutoff_Hz_min, init_data.m_cutoff_Hz_max);
        }

        ANS_DSP_Init_Data m_defaults;

        float m_Q_smoothed;
        float m_cutoff_Hz_smoothed;
        /** Using atomics here because both the QT UI thread and the Mixer thread update these values */
        std::atomic<float> m_Q_current {};
        std::atomic<float> m_cutoff_Hz_current {};
        // x[n-1]: input sample per channel: One sample minus current
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> state_1 {};
        // y[n-1]: output sample per channel: One sample minus current
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> state_2 {};

        float m_smoothing_coefficient;

        void reset(const unsigned int buffer_size, const unsigned int samplerate)
        {
            m_defaults.buffer_size = buffer_size;
            m_defaults.samplerate = samplerate;

            m_Q_smoothed = m_Q_current.load(std::memory_order_relaxed);
            m_cutoff_Hz_smoothed = m_cutoff_Hz_current.load(std::memory_order_relaxed);
            state_1.fill(0.f);
            state_2.fill(0.f);
        }

        void calculate_smoothing_coefficient()
        {
            const float buffer_duration_ms = ans_dsp::samples_to_ms(m_defaults.buffer_size, static_cast<float>(m_defaults.samplerate));
            m_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(buffer_duration_ms, m_defaults.m_smoothing_time_ms);
        }
    };
}

#endif
