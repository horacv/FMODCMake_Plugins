#include "ans_passthrough_state.hpp"

#include <algorithm>
#include <cstring>

namespace ans_passthrough
{
    void ANS_DSP_State::reset()
    {
        m_data.reset();
    }

    FMOD_RESULT ANS_DSP_State::process(const float* in_buffer, float* out_buffer, const unsigned int buffer_size, const int in_channels, const int out_channels)
    {
        if (!(in_buffer && out_buffer && out_channels))
        {
            return FMOD_ERR_INVALID_HANDLE;
        }
        const unsigned int num_samples = buffer_size * std::min(in_channels, out_channels);
        const unsigned int num_bytes = num_samples * sizeof(float);

        // Copy samples from the input buffer to the output buffer - A simple passthrough with no processing.
        return std::memcpy(out_buffer, in_buffer, num_bytes) == nullptr ? FMOD_ERR_INVALID_HANDLE : FMOD_OK;
    }
}