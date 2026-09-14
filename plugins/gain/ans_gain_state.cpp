#include "ans_gain_state.hpp"

#include "ans_dsp_common.hpp"
#include <algorithm>
#include <cmath>

namespace ans_gain
{
    void ANS_DSP_State::reset(const int samplerate_current)
    {
        m_data.reset(samplerate_current);
    }

    void ANS_DSP_State::set_gain_db(const float db)
    {
        const float gain_db_min = m_data.m_defaults.m_gain_db_min;
        const float gain_db_max = m_data.m_defaults.m_gain_db_max;
        const float ramp_time_seconds = m_data.m_defaults.m_ramp_time_seconds;
        m_data.m_gain_lin_target = ans_dsp::db_to_linear(std::clamp(db, gain_db_min, gain_db_max));
        m_data.m_samples_to_target = static_cast<int>(std::round(static_cast<float>(m_data.m_samplerate) * ramp_time_seconds));
    }

    float ANS_DSP_State::get_gain_db() const
    {
        return ans_dsp::linear_to_db(m_data.m_gain_lin_current);
    }

    FMOD_RESULT ANS_DSP_State::process(const float* in_buffer, float* out_buffer, const unsigned int buffer_size, const int in_channels, const int out_channels)
    {
        if (!(in_buffer && out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        // Buffers are interleaved [L,R,L,R,L,R...], so to get all possible samples to process, multiply the buffer size by the minimum of the input and output channels.

        unsigned int samples_to_process = buffer_size * std::min(in_channels, out_channels);
        float smoothed_gain = m_data.m_gain_lin_current;

        // delta = distance to target gain remaining / samples remaining, recomputed every remaining sample.
        // Only snap to target once the ramp has actually completed. This can span multiple process calls.

        if (m_data.m_samples_to_target)
        {
            const float delta = (m_data.m_gain_lin_target - smoothed_gain) / static_cast<float>(m_data.m_samples_to_target);
            while (samples_to_process && m_data.m_samples_to_target)
            {
                smoothed_gain += delta;
                *out_buffer++ = *in_buffer++ * smoothed_gain;
                samples_to_process--;
                m_data.m_samples_to_target--;
            }
            if (!m_data.m_samples_to_target)
            {
                smoothed_gain = m_data.m_gain_lin_target;
            }
        }

        // No ramping left. Set the remaining samples to the current gain

        while (samples_to_process--)
        {
            *out_buffer++ = *in_buffer++ * smoothed_gain;
        }

        m_data.m_gain_lin_current = smoothed_gain;
        return FMOD_OK;
    }
}
