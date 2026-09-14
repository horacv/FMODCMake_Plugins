#include "ans_eq_4b_state.hpp"

#include "ans_dsp_filters_biquad.hpp"

namespace ans_eq_4b
{
    void ANS_DSP_State::reset(const unsigned int buffer_size, const unsigned int samplerate)
    {
        m_data.reset(buffer_size, samplerate);
    }

    void ANS_DSP_State::set_freq_Hz(BAND band, const float value)
    {
        switch (band)
        {
            case LOW:
                m_data.m_l_freq_Hz_current.store(std::clamp(value, m_data.m_defaults.m_l_freq_Hz_min, m_data.m_defaults.m_l_freq_Hz_max), std::memory_order_relaxed);
                break;
            case LOW_MID:
                m_data.m_lm_freq_Hz_current.store(std::clamp(value, m_data.m_defaults.m_lm_freq_Hz_min, m_data.m_defaults.m_lm_freq_Hz_max), std::memory_order_relaxed);
                break;
            case HIGH_MID:
                m_data.m_hm_freq_Hz_current.store(std::clamp(value, m_data.m_defaults.m_hm_freq_Hz_min, m_data.m_defaults.m_hm_freq_Hz_max), std::memory_order_relaxed);
                break;
            case HIGH:
                m_data.m_h_freq_Hz_current.store(std::clamp(value, m_data.m_defaults.m_h_freq_Hz_min, m_data.m_defaults.m_h_freq_Hz_max), std::memory_order_relaxed);
                break;
            default:
                break;
        }
    }

    void ANS_DSP_State::set_filter_type(const BAND band, const FILTER_TYPE type)
    {
        switch (band)
        {
            case LOW:
                m_data.m_l_filter_type_current = type;
                break;
            case HIGH:
                m_data.m_h_filter_type_current = type;
                break;
            default:
                break;
        }
    }

    void ANS_DSP_State::set_gain_db(const BAND band, const float db)
    {
        switch (band)
        {
            case LOW:
                {
                    const float gain_db_min = m_data.m_defaults.m_l_gain_db_min;
                    const float gain_db_max = m_data.m_defaults.m_l_gain_db_max;
                    m_data.m_l_gain_lin_current.store(ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max)));
                }
                break;
            case LOW_MID:
                {
                    const float gain_db_min = m_data.m_defaults.m_l_gain_db_min;
                    const float gain_db_max = m_data.m_defaults.m_l_gain_db_max;
                    m_data.m_lm_gain_lin_current.store(ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max)));
                }
                break;
            case HIGH_MID:
                {
                    const float gain_db_min = m_data.m_defaults.m_hm_gain_db_min;
                    const float gain_db_max = m_data.m_defaults.m_hm_gain_db_max;
                    m_data.m_hm_gain_lin_current.store(ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max)));
                }
            break;
            case HIGH:
                {
                    const float gain_db_min = m_data.m_defaults.m_h_gain_db_min;
                    const float gain_db_max = m_data.m_defaults.m_h_gain_db_max;
                    m_data.m_h_gain_lin_current.store(ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max)));
                }
                break;
            default:
                break;
        }
    }

    void ANS_DSP_State::set_Q(const BAND band, const float value)
    {
        switch (band)
        {
            case LOW:
                m_data.m_l_Q_current.store(std::clamp(value, m_data.m_defaults.m_l_Q_min, m_data.m_defaults.m_l_Q_max), std::memory_order_relaxed);
                break;
            case LOW_MID:
                m_data.m_lm_Q_current.store(std::clamp(value, m_data.m_defaults.m_lm_Q_min, m_data.m_defaults.m_lm_Q_max), std::memory_order_relaxed);
                break;
            case HIGH_MID:
                m_data.m_hm_Q_current.store(std::clamp(value, m_data.m_defaults.m_hm_Q_min, m_data.m_defaults.m_hm_Q_max), std::memory_order_relaxed);
                break;
            case HIGH:
                m_data.m_h_Q_current.store(std::clamp(value, m_data.m_defaults.m_h_Q_min, m_data.m_defaults.m_h_Q_max), std::memory_order_relaxed);
                break;
            default:
                break;
        }
    }

    float ANS_DSP_State::get_freq_Hz(BAND band) const
    {
        switch (band)
        {
            case LOW:
                return m_data.m_l_freq_Hz_current.load(std::memory_order_relaxed);
            case LOW_MID:
                return m_data.m_lm_freq_Hz_current.load(std::memory_order_relaxed);
            case HIGH_MID:
                return m_data.m_hm_freq_Hz_current.load(std::memory_order_relaxed);
            case HIGH:
                return m_data.m_h_freq_Hz_current.load(std::memory_order_relaxed);
            default:
                return 0.0f;
        }
    }

    FILTER_TYPE ANS_DSP_State::get_filter_type(const BAND band) const
    {
        switch (band)
        {
            case LOW:
                return m_data.m_l_filter_type_current;
                break;
            case HIGH:
                return m_data.m_h_filter_type_current;
                break;
            default:
                return MAX;
                break;
        }
    }

    float ANS_DSP_State::get_gain_db(const BAND band) const
    {
        switch (band)
        {
            case LOW:
                return ans_dsp::linear_to_db(m_data.m_l_gain_lin_current.load());
                break;
            case LOW_MID:
                return ans_dsp::linear_to_db(m_data.m_lm_gain_lin_current.load());
                break;
            case HIGH_MID:
                return ans_dsp::linear_to_db(m_data.m_hm_gain_lin_current.load());
                break;
            case HIGH:
                return ans_dsp::linear_to_db(m_data.m_h_gain_lin_current.load());
                break;
            default:
                return MAX;
                break;
        }
    }

    float ANS_DSP_State::get_Q(const BAND band) const
    {
        switch (band)
        {
            case LOW:
                return m_data.m_l_Q_current.load(std::memory_order_relaxed);
                break;
            case LOW_MID:
                return m_data.m_lm_Q_current.load(std::memory_order_relaxed);
                break;
            case HIGH_MID:
                return m_data.m_hm_Q_current.load(std::memory_order_relaxed);
                break;
            case HIGH:
                return m_data.m_h_Q_current.load(std::memory_order_relaxed);
                break;
            default:
                return 0.0f;
                break;
        }
    }

    FMOD_RESULT ANS_DSP_State::process(const float* in_buffer, float* out_buffer,
        const unsigned int buffer_size, const int samplerate, const int in_channels, const int out_channels)
    {
        if (!(in_buffer && out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        const int num_channels = std::clamp(std::min(in_channels, out_channels), 0, FMOD_MAX_CHANNEL_WIDTH);
        assert(num_channels == std::min(in_channels, out_channels) && "channel count exceeds FMOD_MAX_CHANNEL_WIDTH");

        // Biquad filter coefficients: https://www.w3.org/TR/audio-eq-cookbook/#formulae
        // TDF-II formula: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
        // Coefficient calculations + parameter smoothing.

        // LOW BAND

        ans_dsp::interpolate_with_coefficient(m_data.m_l_freq_Hz_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_l_freq_Hz_smoothed);
        ans_dsp::interpolate_with_coefficient(m_data.m_l_Q_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_l_Q_smoothed);
        const auto [l_a0, l_a1, l_a2, l_b0, l_b1, l_b2] = get_filter_type(LOW) == LOW_OR_HIGH_PASS
            ? ans_dsp::hpf_filter_coefficients(m_data.m_l_freq_Hz_smoothed, static_cast<float>(samplerate), m_data.m_l_Q_smoothed).normalized()
            : ans_dsp::low_shelf_coefficients(m_data.m_l_freq_Hz_smoothed, static_cast<float>(samplerate), get_gain_db(LOW), m_data.m_l_Q_smoothed).normalized();

        // LOW MID BAND
        ans_dsp::interpolate_with_coefficient(m_data.m_lm_freq_Hz_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_lm_freq_Hz_smoothed);
        ans_dsp::interpolate_with_coefficient(m_data.m_lm_Q_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_lm_Q_smoothed);
        const auto [lm_a0, lm_a1, lm_a2, lm_b0, lm_b1, lm_b2] =
            ans_dsp::peaking_eq_coefficients(m_data.m_lm_freq_Hz_smoothed, static_cast<float>(samplerate), get_gain_db(LOW_MID), m_data.m_lm_Q_smoothed).normalized();

        // HIGH MID BAND
        ans_dsp::interpolate_with_coefficient(m_data.m_hm_freq_Hz_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_hm_freq_Hz_smoothed);
        ans_dsp::interpolate_with_coefficient(m_data.m_hm_Q_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_hm_Q_smoothed);
        const auto [hm_a0, hm_a1, hm_a2, hm_b0, hm_b1, hm_b2] =
            ans_dsp::peaking_eq_coefficients(m_data.m_hm_freq_Hz_smoothed, static_cast<float>(samplerate), get_gain_db(HIGH_MID), m_data.m_hm_Q_smoothed).normalized();

        // HIGH BAND
        ans_dsp::interpolate_with_coefficient(m_data.m_h_freq_Hz_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_h_freq_Hz_smoothed);
        ans_dsp::interpolate_with_coefficient(m_data.m_h_Q_current.load(std::memory_order_relaxed), m_data.m_smoothing_coefficient, m_data.m_h_Q_smoothed);
        const auto [h_a0, h_a1, h_a2, h_b0, h_b1, h_b2] = get_filter_type(HIGH) == LOW_OR_HIGH_PASS
            ? ans_dsp::lpf_filter_coefficients(m_data.m_h_freq_Hz_smoothed, static_cast<float>(samplerate), m_data.m_h_Q_smoothed).normalized()
            : ans_dsp::high_shelf_coefficients(m_data.m_h_freq_Hz_smoothed, static_cast<float>(samplerate), get_gain_db(HIGH), m_data.m_h_Q_smoothed).normalized();

        // Buffers are interleaved [L,R,L,R,L,R...]
        for (unsigned int buffer_index = 0; buffer_index < buffer_size; ++buffer_index)
        {
            for (unsigned int channel_index = 0; channel_index < num_channels; ++channel_index)
            {
                const unsigned int sample = buffer_index * num_channels + channel_index;

                // This is a cascaded biquad filter. One band output feeds into the next band:
                // input -> low -> low-mid -> high-mid -> high -> output

                /**
                 * Formulas:
                 * x[n] (Input)
                 * y[n] (Output) = x[n-1] + |b0| · x
                 * x[n-1] (state_1) = y[n-1] + |b1| · x[n] - |a1| · y[n]
                 * y[n-1] (state_2) = |b2| · x[n] - |a2| · y[n]
                 */

                float input = 0;
                float output = 0;

                // LOW BAND
                input = in_buffer[sample];
                output = m_data.m_l_state_1[channel_index] + l_b0 * input;
                m_data.m_l_state_1[channel_index] = ans_dsp::flush_subnormal_to_zero(m_data.m_l_state_2[channel_index] + l_b1 * input - l_a1 * output);
                m_data.m_l_state_2[channel_index] = ans_dsp::flush_subnormal_to_zero(l_b2 * input - l_a2 * output);

                // LOW MID BAND
                input = output;
                output = m_data.m_lm_state_1[channel_index] + lm_b0 * input;
                m_data.m_lm_state_1[channel_index] = ans_dsp::flush_subnormal_to_zero(m_data.m_lm_state_2[channel_index] + lm_b1 * input - lm_a1 * output);
                m_data.m_lm_state_2[channel_index] = ans_dsp::flush_subnormal_to_zero(lm_b2 * input - lm_a2 * output);

                // HIGH MID BAND
                input = output;
                output = m_data.m_hm_state_1[channel_index] + hm_b0 * input;
                m_data.m_hm_state_1[channel_index] = ans_dsp::flush_subnormal_to_zero(m_data.m_hm_state_2[channel_index] + hm_b1 * input - hm_a1 * output);
                m_data.m_hm_state_2[channel_index] = ans_dsp::flush_subnormal_to_zero(hm_b2 * input - hm_a2 * output);

                // HIGH BAND
                input = output;
                output = m_data.m_h_state_1[channel_index] + h_b0 * input ;
                m_data.m_h_state_1[channel_index] = ans_dsp::flush_subnormal_to_zero(m_data.m_h_state_2[channel_index] + h_b1 * input - h_a1 * output);
                m_data.m_h_state_2[channel_index] = ans_dsp::flush_subnormal_to_zero(h_b2 * input - h_a2 * output);

                out_buffer[sample] = output;
            }
        }

        return FMOD_OK;
    }
}
