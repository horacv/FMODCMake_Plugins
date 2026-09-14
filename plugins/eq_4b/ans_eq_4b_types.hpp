#ifndef ANS_EQ_4B_TYPES_HPP
#define ANS_EQ_4B_TYPES_HPP

#include "ans_dsp_common.hpp"
#include "fmod_common.h"
#include <array>
#include <atomic>

#include "ans_dsp_smoothing.hpp"

namespace ans_eq_4b
{
    enum BAND
    {
        LOW,
        LOW_MID,
        HIGH_MID,
        HIGH
    };

    enum FILTER_TYPE
    {
        LOW_OR_HIGH_PASS = 0,
        SHELF,

        MAX,
    };
    
    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        // LOW BAND
        FILTER_TYPE m_l_filter_type_default;
        float m_l_Q_default;
        float m_l_Q_max;
        float m_l_Q_min;
        float m_l_freq_Hz_default;
        float m_l_freq_Hz_max;
        float m_l_freq_Hz_min;
        float m_l_gain_db_default;
        float m_l_gain_db_max;
        float m_l_gain_db_min;

        // LOW MID BAND
        float m_lm_Q_default;
        float m_lm_Q_max;
        float m_lm_Q_min;
        float m_lm_freq_Hz_default;
        float m_lm_freq_Hz_max;
        float m_lm_freq_Hz_min;
        float m_lm_gain_db_default;
        float m_lm_gain_db_max;
        float m_lm_gain_db_min;

        // HIGH MID BAND
        float m_hm_Q_default;
        float m_hm_Q_max;
        float m_hm_Q_min;
        float m_hm_freq_Hz_default;
        float m_hm_freq_Hz_max;
        float m_hm_freq_Hz_min;
        float m_hm_gain_db_default;
        float m_hm_gain_db_max;
        float m_hm_gain_db_min;

        // HIGH BAND
        FILTER_TYPE m_h_filter_type_default;
        float m_h_Q_default;
        float m_h_Q_max;
        float m_h_Q_min;
        float m_h_freq_Hz_default;
        float m_h_freq_Hz_max;
        float m_h_freq_Hz_min;
        float m_h_gain_db_default;
        float m_h_gain_db_max;
        float m_h_gain_db_min;

        float m_smoothing_time_ms;
        unsigned int buffer_size;
        int samplerate;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_l_filter_type_current(init_data.m_l_filter_type_default)
            , m_l_gain_lin_current(ans_dsp::db_to_linear(m_defaults.m_l_gain_db_default))
            , m_lm_gain_lin_current(ans_dsp::db_to_linear(m_defaults.m_lm_gain_db_default))
            , m_hm_gain_lin_current(ans_dsp::db_to_linear(m_defaults.m_hm_gain_db_default))
            , m_h_filter_type_current(init_data.m_h_filter_type_default)
            , m_h_gain_lin_current(ans_dsp::db_to_linear(m_defaults.m_h_gain_db_default))
        {
            // LOW BAND
            m_defaults.m_l_Q_default = std::clamp(init_data.m_l_Q_default, init_data.m_l_Q_min, init_data.m_l_Q_max);
            m_defaults.m_l_freq_Hz_default = std::clamp(init_data.m_l_freq_Hz_default,init_data.m_l_freq_Hz_min, init_data.m_l_freq_Hz_max);
            m_l_Q_current = m_defaults.m_l_Q_default;
            m_l_Q_smoothed = m_defaults.m_l_Q_default;
            m_l_freq_Hz_current = m_defaults.m_l_freq_Hz_default;
            m_l_freq_Hz_smoothed = m_defaults.m_l_freq_Hz_default;

            // LOW MID BAND
            m_defaults.m_lm_Q_default = std::clamp(init_data.m_lm_Q_default, init_data.m_lm_Q_min, init_data.m_lm_Q_max);
            m_defaults.m_lm_freq_Hz_default = std::clamp(init_data.m_lm_freq_Hz_default,init_data.m_lm_freq_Hz_min, init_data.m_lm_freq_Hz_max);
            m_lm_Q_current = m_defaults.m_lm_Q_default;
            m_lm_Q_smoothed = m_defaults.m_lm_Q_default;
            m_lm_freq_Hz_current = m_defaults.m_lm_freq_Hz_default;
            m_lm_freq_Hz_smoothed = m_defaults.m_lm_freq_Hz_default;

            // HIGH MID BAND
            m_defaults.m_hm_Q_default = std::clamp(init_data.m_hm_Q_default, init_data.m_hm_Q_min, init_data.m_hm_Q_max);
            m_defaults.m_hm_freq_Hz_default = std::clamp(init_data.m_hm_freq_Hz_default,init_data.m_hm_freq_Hz_min, init_data.m_hm_freq_Hz_max);
            m_hm_Q_current = m_defaults.m_hm_Q_default;
            m_hm_Q_smoothed = m_defaults.m_hm_Q_default;
            m_hm_freq_Hz_current = m_defaults.m_hm_freq_Hz_default;
            m_hm_freq_Hz_smoothed = m_defaults.m_hm_freq_Hz_default;

            // HIGH BAND
            m_defaults.m_h_Q_default = std::clamp(init_data.m_h_Q_default, init_data.m_h_Q_min, init_data.m_h_Q_max);
            m_defaults.m_h_freq_Hz_default = std::clamp(init_data.m_h_freq_Hz_default,init_data.m_h_freq_Hz_min, init_data.m_h_freq_Hz_max);
            m_h_Q_current = m_defaults.m_h_Q_default;
            m_h_Q_smoothed = m_defaults.m_h_Q_default;
            m_h_freq_Hz_current = m_defaults.m_h_freq_Hz_default;
            m_h_freq_Hz_smoothed = m_defaults.m_h_freq_Hz_default;

            const float buffer_duration_ms = ans_dsp::samples_to_ms(init_data.buffer_size, static_cast<float>(init_data.samplerate));
            m_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(buffer_duration_ms, m_defaults.m_smoothing_time_ms);
        }

        ANS_DSP_Init_Data m_defaults;

        /**
         * Using atomics here because both the QT UI thread and the Mixer thread update these values
         * x[n-1] (state_1): input sample per channel: One sample minus current
         * y[n-1] (state_2): output sample per channel: One sample minus current
         */

        // LOW BAND
        FILTER_TYPE m_l_filter_type_current;
        float m_l_Q_smoothed;
        float m_l_freq_Hz_smoothed;
        std::atomic<float> m_l_Q_current {};
        std::atomic<float> m_l_freq_Hz_current {};
        std::atomic<float> m_l_gain_lin_current {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_l_state_1 {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_l_state_2 {};

        // LOW MID BAND
        float m_lm_Q_smoothed;
        float m_lm_freq_Hz_smoothed;
        std::atomic<float> m_lm_Q_current {};
        std::atomic<float> m_lm_freq_Hz_current {};
        std::atomic<float> m_lm_gain_lin_current {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_lm_state_1 {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_lm_state_2 {};

        // HIGH MID BAND
        float m_hm_Q_smoothed;
        float m_hm_freq_Hz_smoothed;
        std::atomic<float> m_hm_Q_current {};
        std::atomic<float> m_hm_freq_Hz_current {};
        std::atomic<float> m_hm_gain_lin_current {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_hm_state_1 {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_hm_state_2 {};

        // HIGH BAND
        FILTER_TYPE m_h_filter_type_current;
        float m_h_Q_smoothed;
        float m_h_freq_Hz_smoothed;
        std::atomic<float> m_h_Q_current {};
        std::atomic<float> m_h_freq_Hz_current {};
        std::atomic<float> m_h_gain_lin_current {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_h_state_1 {};
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> m_h_state_2 {};

        float m_smoothing_coefficient;

        void reset(const unsigned int buffer_size, const unsigned int samplerate)
        {
            // LOW BAND
            m_l_Q_smoothed = m_l_Q_current.load(std::memory_order_relaxed);
            m_l_freq_Hz_smoothed = m_l_freq_Hz_current.load(std::memory_order_relaxed);
            m_l_state_1.fill(0.f);
            m_l_state_2.fill(0.f);

            // LOW MID BAND
            m_lm_Q_smoothed = m_lm_Q_current.load(std::memory_order_relaxed);
            m_lm_freq_Hz_smoothed = m_lm_freq_Hz_current.load(std::memory_order_relaxed);
            m_lm_state_1.fill(0.f);
            m_lm_state_2.fill(0.f);

            // HIGH MID BAND
            m_hm_Q_smoothed = m_hm_Q_current.load(std::memory_order_relaxed);
            m_hm_freq_Hz_smoothed = m_hm_freq_Hz_current.load(std::memory_order_relaxed);
            m_hm_state_1.fill(0.f);
            m_hm_state_2.fill(0.f);

            // HIGH BAND
            m_h_Q_smoothed = m_h_Q_current.load(std::memory_order_relaxed);
            m_h_freq_Hz_smoothed = m_h_freq_Hz_current.load(std::memory_order_relaxed);
            m_h_state_1.fill(0.f);
            m_h_state_2.fill(0.f);

            const float buffer_duration_ms = ans_dsp::samples_to_ms(buffer_size, static_cast<float>(samplerate));
            m_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(buffer_duration_ms, m_defaults.m_smoothing_time_ms);
        }
    };
}

#endif
