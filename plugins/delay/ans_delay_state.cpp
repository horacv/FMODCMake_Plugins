#include "ans_delay_state.hpp"

#include "ans_dsp_common.hpp"

namespace ans_delay
{
    void ANS_DSP_State::reset(const unsigned int buffer_size, const int samplerate_current)
    {
        m_data.reset(buffer_size, samplerate_current);
    }

    void ANS_DSP_State::set_delay_ms(const float delay_ms)
    {
        m_data.m_delay_ms_target = std::clamp(delay_ms, m_data.m_defaults.m_delay_ms_min, m_data.m_defaults.m_delay_ms_max);
    }

    float ANS_DSP_State::get_delay_ms() const
    {
        return m_data.m_delay_ms_smoothed;
    }

    void ANS_DSP_State::set_wet_gain_db(const float db)
    {
        const float gain_db_min = m_data.m_defaults.m_wet_gain_db_min;
        const float gain_db_max = m_data.m_defaults.m_wet_gain_db_max;
        m_data.m_wet_gain_lin_target = ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max));
    }

    float ANS_DSP_State::get_wet_gain_db() const
    {
        return ans_dsp::linear_to_db(m_data.m_wet_gain_lin_smoothed);
    }

    void ANS_DSP_State::set_dry_gain_db(const float db)
    {
        const float gain_db_min = m_data.m_defaults.m_dry_gain_db_min;
        const float gain_db_max = m_data.m_defaults.m_dry_gain_db_max;
        m_data.m_dry_gain_lin_target = ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max));
    }

    float ANS_DSP_State::get_dry_gain_db() const
    {
        return ans_dsp::linear_to_db(m_data.m_dry_gain_lin_smoothed);
    }

    void ANS_DSP_State::set_feedback_percent(const float value)
    {
        const float feedback_percent_min = m_data.m_defaults.m_feedback_percent_min;
        const float feedback_percent_max = m_data.m_defaults.m_feedback_percent_max;
        m_data.m_feedback_normalized_target.store(std::clamp(value, feedback_percent_min, feedback_percent_max) * 0.01f);
    }

    float ANS_DSP_State::get_feedback_percent() const
    {
        return m_data.m_feedback_normalized_smoothed * 100.0f;
    }

    void ANS_DSP_State::start_fading_out()
    {
        m_data.playback_state = ANS_DSP_ProcessingState::FadingOut;
    }

    FMOD_RESULT ANS_DSP_State::process(const float* in_buffer, float* out_buffer, const unsigned int buffer_size, const int in_channels, const int out_channels)
    {
        if (!(in_buffer && out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        // Buffers are interleaved [L,R,L,R,L,R...], so to get all possible samples to process, multiply the buffer size by the minimum of the input and output channels.

        const int num_channels = std::clamp(std::min(in_channels, out_channels), 0, FMOD_MAX_CHANNEL_WIDTH);
        assert(num_channels == std::min(in_channels, out_channels) && "channel count exceeds FMOD_MAX_CHANNEL_WIDTH");

        if (m_data.m_channel_count_current != num_channels)
        {
            // Number of channels has changed, so we need to reset the delay buffer and read/write indices.
            m_data.m_channel_count_current = num_channels;
            m_data.reset_delay_buffer();
            m_data.update_delay_smoothed_samples();
            m_data.reposition_read_index();
        }

        // Feedback Smoothing - Once per block is enough
        ans_dsp::interpolate_with_coefficient(m_data.m_feedback_normalized_target.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_feedback_normalized_smoothed);

        // Delay Smoothing - Once per block is enough
        // If the parameter was interpolated, re-calculate the read index
        if (ans_dsp::interpolate_with_coefficient(m_data.m_delay_ms_target.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_delay_ms_smoothed))
        {
            m_data.update_delay_smoothed_samples();
            m_data.reposition_read_index();
        }

        unsigned int samples_to_process = buffer_size * num_channels;
        float loudest_delay_sample_in_buffer = 0.f;

        while (samples_to_process--)
        {
            /*
             * y[n] = x[n - D] + fb * y[n - D]
             *
             * Pirkle, W. C. (2019). Designing Audio Effect Plugins in C++: For AAX, AU,
             * and VST3 With DSP Theory (2nd ed.), Ch. 14 "Delay Effects and Circular
             * Buffers," §14.1–14.2 (feedback DDL derivation, Eq. 14.8), Routledge.
            */

            const float input = *in_buffer++;
            const float delay = m_data.m_delay_buffer[m_data.m_delay_buffer_read_index];

            // Gain Smoothing - Interpolate once per sample.
            constexpr auto GAIN_INTERP_TOLERANCE = 0.001f;
            // WET
            ans_dsp::interpolate_with_coefficient(m_data.m_wet_gain_lin_target.load(std::memory_order_relaxed),
                m_data.m_smoothing_coefficient, m_data.m_wet_gain_lin_smoothed, GAIN_INTERP_TOLERANCE);
            // DRY
            ans_dsp::interpolate_with_coefficient(m_data.m_dry_gain_lin_target.load(std::memory_order_relaxed),
                m_data.m_smoothing_coefficient, m_data.m_dry_gain_lin_smoothed, GAIN_INTERP_TOLERANCE);

            // Wet/dry mix of the direct and delayed signals scaled by their gain parameters.
            const float input_scaled = input * m_data.m_dry_gain_lin_smoothed;
            const float delay_scaled = delay * m_data.m_wet_gain_lin_smoothed;
            *out_buffer++ = ans_dsp::flush_subnormal_to_zero(input_scaled + delay_scaled);

            // Here is where we write to the circular buffer "tape".
            m_data.m_delay_buffer[m_data.m_delay_buffer_write_index] = input + m_data.m_feedback_normalized_smoothed * delay;
            // Advance the delay buffer indexes - Move the "tape" one position.
            m_data.advance_delay_buffer_indexes();

            // Cache the loudest sample in the delay buffer to know when to stop processing when fading out.
            loudest_delay_sample_in_buffer = std::max(std::fabs(delay), std::fabs(loudest_delay_sample_in_buffer));
        }

        if (is_fading_out())
        {
            const bool is_below_threshold = loudest_delay_sample_in_buffer < ans_dsp::db_to_linear(m_data.m_defaults.m_min_threshold_db);
            m_data.playback_state = is_below_threshold ? ANS_DSP_ProcessingState::NotActive : ANS_DSP_ProcessingState::FadingOut;
        }

        return FMOD_OK;
    }
}
