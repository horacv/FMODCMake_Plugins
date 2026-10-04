#ifndef FMOD_PLUGINS_ANS_DSP_NOISE_HPP
#define FMOD_PLUGINS_ANS_DSP_NOISE_HPP

#include <cassert>

namespace ans_dsp
{
    /**
     * Generates a pseudo-random number using the xorshift32 algorithm.
     *
     * The xorshift32 algorithm is a simple pseudorandom number generator
     * that performs bitwise XOR and bit shifts on the input state to produce
     * the next state. The input state must not be zero.
     *
     * @param state The initial state of the xorshift32 generator. Must not be zero.
     * @return The next state in the xorshift32 sequence.
     */
    inline uint32_t generate_noise_xorshift32(uint32_t state)
    {
        if (state == 0u)
        {
            assert(!"generate noise xorshift32: state must not be zero");
            return 1u;
        }

        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;

        return state;
    }
}
#endif
