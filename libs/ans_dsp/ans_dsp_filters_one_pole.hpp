#ifndef ANS_DSP_FILTERS_ONE_POLE_HPP
#define ANS_DSP_FILTERS_ONE_POLE_HPP

#include "ans_dsp_common.hpp"

namespace ans_dsp
{
    // https://dsp.stackexchange.com/questions/54086/single-pole-iir-low-pass-filter-which-is-the-correct-formula-for-the-decay-coe

    /**
     * Computes the one-pole filter coefficient (α) for a target cutoff frequency,
     * using the exact bilinear-transform formula (accurate all the way up to Nyquist).
     *
     * @param cutoff_Hz The cutoff frequency in Hz for the filter. The frequency should lie within a valid range as determined
     *                 by the sampling rate and internal clamping mechanism.
     * @param samplerate_Hz The sampling rate in Hz. Must be high enough to accommodate the frequency range being processed.
     * @return The computed bilinear transformation coefficient (α).
     * @note Formula: α = 2y / (y + 1), where y = tan(ωc / 2), and ωc is the angular cutoff frequency.
     */
    inline float alpha_from_Hz_bilinear(const float cutoff_Hz, const float samplerate_Hz)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // y = tan(ωc / 2)
        const float y = tan(wc * 0.5f);

        // α = 2y / (y + 1)
        return 2.0f * y / (y + 1.0f);
    }

    /**
     * Computes the one-pole filter coefficient (α) for a target cutoff frequency,
     * using an exact mathematical formula based on the angular cutoff frequency.
     *
     * @param cutoff_Hz The cutoff frequency in Hz for the filter. The frequency should lie within a valid range
     *                 as determined by the sampling rate and internal clamping mechanism.
     * @param samplerate_Hz The sampling rate in Hz. Must be high enough to support the frequency range being processed.
     * @return The computed filter coefficient (α) derived using an exact formula.
     * @note Formula: α = -y + sqrt(y^2 + 2y), where y = 1 - cos(ωc), and ωc is the angular cutoff frequency.
     */
    inline float alpha_from_Hz_exact(const float cutoff_Hz, const float samplerate_Hz)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // y = 1 - cos(ωc)
        const float y = 1 - cos(wc);

        // α = -y + sqrt(y^2 + 2y)
        return -y + sqrtf(powf(y, 2) + 2 * y);
    }
}
#endif
