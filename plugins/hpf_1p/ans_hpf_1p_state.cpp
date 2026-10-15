#include "ans_hpf_1p_state.hpp"

#include "ans_dsp_common.hpp"
#include "ans_dsp_filters_one_pole.hpp"
#include <algorithm>

namespace ans_hpf_1p
{
    void ANS_DSP_State::reset(const int samplerate)
    {
        m_data.reset(samplerate);
    }

    void ANS_DSP_State::set_cutoff_Hz(const float value)
    {
        const float cutoff_Hz_min = m_data.m_defaults.m_cutoff_Hz_min;
        const float cutoff_Hz_max = m_data.m_defaults.m_cutoff_Hz_max;
        m_data.m_cutoff_Hz_current.store(std::clamp(value, cutoff_Hz_min, cutoff_Hz_max), std::memory_order::relaxed);
    }

    FMOD_RESULT ANS_DSP_State::process(const float* in_buffer, float* out_buffer, const unsigned int buffer_size, const int in_channels, const int out_channels)
    {
        if (!(in_buffer && out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        const unsigned int num_channels = std::clamp(std::min(in_channels, out_channels), 0, FMOD_MAX_CHANNEL_WIDTH);
        assert(num_channels == std::min(in_channels, out_channels) && "channel count exceeds FMOD_MAX_CHANNEL_WIDTH");

        const float alpha = ans_dsp::alpha_from_Hz_bilinear(m_data.m_cutoff_Hz_current.load(std::memory_order::relaxed), static_cast<float>(m_data.m_defaults.m_samplerate));

        // Buffers are interleaved [L,R,L,R,L,R...]
        for (unsigned int buffer_index = 0; buffer_index < buffer_size; ++buffer_index)
        {
            for (unsigned int channel_index = 0; channel_index < num_channels; ++channel_index)
            {
                const unsigned int sample = buffer_index * num_channels + channel_index;

                const float input = in_buffer[sample];

                // lp[n] = lp[n-1] + α·(x[n] - lp[n-1])
                const float out_lpf = ans_dsp::flush_subnormal_to_zero(m_data.m_lpf_prev[channel_index] + alpha * (input  - m_data.m_lpf_prev[channel_index]));
                m_data.m_lpf_prev[channel_index] = out_lpf;

                // y[n] = x[n] - lp[n]
                out_buffer[sample] = input - out_lpf;
            }
        }

        return FMOD_OK;
    }
}
