#ifndef ANS_DELAY_TYPES_HPP
#define ANS_DELAY_TYPES_HPP

#include "ans_dsp_common.hpp"
#include "ans_dsp_smoothing.hpp"
#include "fmod_common.h"
#include <atomic>
#include <vector>

namespace ans_delay
{
    enum class ANS_DSP_ProcessingState
    {
        Active,    // Receiving real input, processing normally
        FadingOut, // Input stopped, playing decaying tail
        NotActive  // Tail fully decayed, nothing left to output
    };

    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_delay_ms_default;
        float m_delay_ms_max;
        float m_delay_ms_min;

        float m_wet_gain_db_default;
        float m_wet_gain_db_max;
        float m_wet_gain_db_min;

        float m_dry_gain_db_default;
        float m_dry_gain_db_max;
        float m_dry_gain_db_min;

        float m_feedback_percent_default;
        float m_feedback_percent_max;
        float m_feedback_percent_min;

        float m_min_threshold_db;
        float m_smoothing_time_ms;

        int samplerate;
        unsigned int buffer_size;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_delay_ms_target(m_defaults.m_delay_ms_default)
            , m_delay_ms_smoothed(m_delay_ms_target)
            , m_wet_gain_lin_target(ans_dsp::db_to_linear(m_defaults.m_wet_gain_db_default))
            , m_wet_gain_lin_smoothed(m_wet_gain_lin_target)
            , m_dry_gain_lin_target(ans_dsp::db_to_linear(m_defaults.m_dry_gain_db_default))
            , m_dry_gain_lin_smoothed(m_dry_gain_lin_target)
            , m_feedback_normalized_target(m_defaults.m_feedback_percent_default)
            , m_feedback_normalized_smoothed(m_feedback_normalized_target)
            , m_channel_count_current(2)
            , m_delay_smoothed_samples(ans_dsp::ms_to_samples(m_delay_ms_smoothed, static_cast<float>(m_defaults.samplerate)) * m_channel_count_current)
            , m_delay_buffer_write_index(m_delay_smoothed_samples)
            , m_delay_buffer_read_index(0)
            , m_smoothing_coefficient(0)
        {
            calculate_smoothing_coefficient();
        }

        ANS_DSP_Init_Data m_defaults;

        std::atomic<float> m_delay_ms_target;
        float m_delay_ms_smoothed;
        std::atomic<float> m_wet_gain_lin_target;
        float m_wet_gain_lin_smoothed;
        std::atomic<float> m_dry_gain_lin_target;
        float m_dry_gain_lin_smoothed;
        std::atomic<float> m_feedback_normalized_target;
        float m_feedback_normalized_smoothed;

        int m_channel_count_current;
        unsigned int m_delay_smoothed_samples;

        unsigned int m_delay_buffer_write_index;
        unsigned int m_delay_buffer_read_index;

        ANS_DSP_ProcessingState playback_state = ANS_DSP_ProcessingState::NotActive;
        float m_smoothing_coefficient;

        std::vector<float> m_delay_buffer;

        void reset(const unsigned int buffer_size, const int samplerate)
        {
            m_defaults.samplerate = samplerate;
            m_defaults.buffer_size = buffer_size;

            m_delay_ms_smoothed = m_delay_ms_target.load();
            m_wet_gain_lin_smoothed = m_wet_gain_lin_target.load();
            m_dry_gain_lin_smoothed = m_dry_gain_lin_target.load();
            m_feedback_normalized_smoothed = m_feedback_normalized_target.load();

            update_delay_smoothed_samples();
            m_delay_buffer_write_index = m_delay_smoothed_samples;
            playback_state = ANS_DSP_ProcessingState::NotActive;

            m_delay_buffer.clear();
            const unsigned int max_delay_samples = ans_dsp::ms_to_samples(m_defaults.m_delay_ms_max, static_cast<float>(m_defaults.samplerate));
            // Allocate n number of samples based on the maximum supported delay and the maximum supported FMOD channel count.
            // Only allocate memory in reset(). NEVER do this in process()!
            m_delay_buffer.resize(max_delay_samples * FMOD_MAX_CHANNEL_WIDTH, 0.0f);

            calculate_smoothing_coefficient();
            reposition_read_index();
        }

        void calculate_smoothing_coefficient()
        {
            const float buffer_duration_ms = ans_dsp::samples_to_ms(m_defaults.buffer_size, static_cast<float>(m_defaults.samplerate));
            m_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(buffer_duration_ms, m_defaults.m_smoothing_time_ms);
        }

        void reset_delay_buffer()
        {
            std::ranges::fill(m_delay_buffer.begin(), m_delay_buffer.end(), 0.0f);
        }

        void reposition_read_index()
        {
            const unsigned int delay_buffer_size = m_delay_buffer.size();
            assert(delay_buffer_size != 0 && "Delay buffer size must be greater than zero. Set the buffer size before using it.");
            m_delay_buffer_read_index = (m_delay_buffer_write_index - m_delay_smoothed_samples + delay_buffer_size) % delay_buffer_size;
        }

        void advance_delay_buffer_indexes()
        {
            const unsigned int delay_buffer_size = m_delay_buffer.size();

            m_delay_buffer_write_index++;
            if (m_delay_buffer_write_index >= delay_buffer_size)
            {
                m_delay_buffer_write_index -= delay_buffer_size;
            }

            m_delay_buffer_read_index++;
            if (m_delay_buffer_read_index >= delay_buffer_size)
            {
                m_delay_buffer_read_index -= delay_buffer_size;
            }
        }

        void update_delay_smoothed_samples()
        {
            m_delay_smoothed_samples = ans_dsp::ms_to_samples(m_delay_ms_smoothed, static_cast<float>(m_defaults.samplerate)) * m_channel_count_current;
        }
    };
}

#endif
