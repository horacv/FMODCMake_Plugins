#include "ans_hpf_2p_state.hpp"

#include "ans_dsp_filters_biquad.hpp"

using namespace ans_dsp;

namespace ans_hpf_2p
{
    void ANS_DSP_State::reset(const unsigned int buffer_size, const unsigned int samplerate)
    {
        m_data.reset(buffer_size, samplerate);
    }

    void ANS_DSP_State::set_cutoff_Hz(const float value)
    {
        const float cutoff_Hz_min = m_data.m_defaults.m_cutoff_Hz_min;
        const float cutoff_Hz_max = m_data.m_defaults.m_cutoff_Hz_max;
        m_data.m_cutoff_Hz_current.store(std::clamp(value, cutoff_Hz_min, cutoff_Hz_max), std::memory_order_relaxed);
    }

    void ANS_DSP_State::set_Q(const float value)
    {
        const float Q_min = m_data.m_defaults.m_Q_min;
        const float Q_max = m_data.m_defaults.m_Q_max;
        m_data.m_Q_current.store(std::clamp(value, Q_min, Q_max), std::memory_order_relaxed);
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

        // Parameter smoothing
        interpolate_parameters(m_data);

        // Biquad filter coefficients: https://www.w3.org/TR/audio-eq-cookbook/#formulae
        // TDF-II formula: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html

        // Normalize the coefficients first
        const auto [a0, a1, a2, b0, b1, b2] =
            hpf_filter_coefficients(m_data.m_cutoff_Hz_smoothed, static_cast<float>(samplerate), m_data.m_Q_smoothed).normalized();

        // Buffers are interleaved [L,R,L,R,L,R...]
        for (unsigned int buffer_index = 0; buffer_index < buffer_size; ++buffer_index)
        {
            for (unsigned int channel_index = 0; channel_index < num_channels; ++channel_index)
            {
                const unsigned int sample = buffer_index * num_channels + channel_index;

                // x[n] (Input)
                const float input = in_buffer[sample];

                // y[n] (Output) = x[n-1] + |b0| · x
                const float output = m_data.state_1[channel_index] + b0 * input;

                // x[n-1] = y[n-1] + |b1| · x[n] - |a1| · y[n]
                m_data.state_1[channel_index] = flush_subnormal_to_zero(m_data.state_2[channel_index] + b1 * input - a1 * output);

                // y[n-1] = |b2| · x[n] - |a2| · y[n]
                m_data.state_2[channel_index] = flush_subnormal_to_zero(b2 * input - a2 * output);

                out_buffer[sample] = output;
            }
        }

        return FMOD_OK;
    }

    void ANS_DSP_State::interpolate_parameters(ANS_DSP_State_Data& data)
    {
        if (const float cutoff_Hz_target = data.m_cutoff_Hz_current.load(std::memory_order_relaxed);
            data.m_cutoff_Hz_smoothed != cutoff_Hz_target)
        {
            data.m_cutoff_Hz_smoothed += (cutoff_Hz_target - data.m_cutoff_Hz_smoothed) * (1.0f - data.m_smoothing_coefficient);
        }

        if (const float Q_target = data.m_Q_current.load(std::memory_order_relaxed);
            data.m_Q_smoothed != Q_target)
        {
            data.m_Q_smoothed += (Q_target - data.m_Q_smoothed)  * (1.0f - data.m_smoothing_coefficient);
        }
    }
}
