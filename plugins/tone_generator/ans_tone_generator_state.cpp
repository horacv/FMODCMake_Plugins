#include "ans_tone_generator_state.hpp"

#include "ans_dsp_common.hpp"
#include "ans_dsp_noise.hpp"
#include "ans_dsp_oscillator.hpp"
#include "ans_dsp_smoothing.hpp"
#include <algorithm>

namespace ans_tone_generator
{
    void ANS_DSP_State::set_freq_Hz(const float value)
    {
        m_data.m_freq_Hz_current.store(value, std::memory_order_relaxed);
    }

    void ANS_DSP_State::set_gain_db(const float db)
    {
        m_data.m_gain_lin_current.store(ans_dsp::db_to_linear(db), std::memory_order_relaxed);
    }

    void ANS_DSP_State::set_osc_type(const OSC_TYPE type)
    {
        m_data.m_param_oscillator_type.store(type, std::memory_order_relaxed);
    }

    float ANS_DSP_State::get_freq_Hz() const
    {
        return m_data.m_freq_Hz_current.load(std::memory_order_relaxed);
    }

    float ANS_DSP_State::get_gain_db() const
    {
        return ans_dsp::linear_to_db(m_data.m_gain_lin_current.load(std::memory_order_relaxed));
    }

    OSC_TYPE ANS_DSP_State::get_osc_type() const
    {
        return m_data.m_param_oscillator_type.load(std::memory_order_relaxed);
    }

    FMOD_RESULT ANS_DSP_State::generate(float* out_buffer, const unsigned int buffer_size, const int out_channels)
    {
        if (!(out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (ans_dsp::interpolate_with_coefficient(m_data.m_freq_Hz_current.load(std::memory_order_relaxed), m_data.m_freq_smoothing_coefficient, m_data.m_freq_Hz_smoothed))
        {
            m_data.m_increment.store(ans_dsp::get_phase_increment(m_data.m_freq_Hz_smoothed, m_data.m_defaults.samplerate), std::memory_order_relaxed);
        }

        const int num_channels = std::clamp(out_channels, 0, FMOD_MAX_CHANNEL_WIDTH);
        const OSC_TYPE osc = m_data.m_param_oscillator_type.load(std::memory_order_relaxed);

        // Buffers are interleaved [L,R,L,R,L,R...]
        for (unsigned int buffer_index = 0; buffer_index < buffer_size; ++buffer_index)
        {
            const double output = [&]
            {
                const auto gain = m_data.m_gain_lin_smoothed;
                switch (osc)
                {
                    case SINE:
                        return ans_dsp::generate_sine(m_data.m_counter, gain);
                    case SAW:
                        return ans_dsp::generate_saw(m_data.m_counter, gain);
                    case TRIANGLE:
                        return ans_dsp::generate_triangle(m_data.m_counter, gain);
                    case SQUARE:
                        return ans_dsp::generate_square(m_data.m_counter, gain);
                    case NOISE:
                        return ans_dsp::uint32_bipolar(m_data.m_noise_state) * gain;
                    default:
                        break;
                }
                return 0.0;
            }();

            for (unsigned int channel_index = 0; channel_index < num_channels; ++channel_index)
            {
                // Gain Smoothing - Interpolate once per sample.
                constexpr auto GAIN_INTERP_TOLERANCE = 0.001f;
                ans_dsp::interpolate_with_coefficient(m_data.m_gain_lin_current.load(std::memory_order_relaxed),
                    m_data.m_gain_smoothing_coefficient, m_data.m_gain_lin_smoothed, GAIN_INTERP_TOLERANCE);

                const unsigned int sample = buffer_index * num_channels + channel_index;
                out_buffer[sample] = ans_dsp::flush_subnormal_to_zero(static_cast<float>(output));
            }

            if (osc == NOISE)
                m_data.m_noise_state = ans_dsp::generate_noise_xorshift32(m_data.m_noise_state);
            else
                m_data.m_counter = ans_dsp::modulo_counter_advance_wrap(m_data.m_counter, m_data.m_increment.load(std::memory_order_relaxed));
        }
        return FMOD_OK;
    }
}
