#ifndef ANS_DSP_FILTERS_BIQUAD_HPP
#define ANS_DSP_FILTERS_BIQUAD_HPP

#include "ans_dsp_common.hpp"

namespace ans_dsp
{
    constexpr auto MAX_Q = 20.f;
    constexpr auto MIN_Q = 0.1f;

    // Biquad filter coefficients: https://www.w3.org/TR/audio-eq-cookbook/#formulae
    struct BiquadCoefficients
    {
        float a0; float a1; float a2; float b0; float b1; float b2;

        [[nodiscard]] BiquadCoefficients normalized() const
        {
            assert(a0 != 0.0f && "Division by 0: Coefficient a0 must not be zero");
            return { .a0 = a0, .a1 = a1 / a0, .a2 = a2 / a0, .b0 = b0 / a0, .b1 = b1 / a0, .b2 = b2 / a0 };
        }
    };

    /**
     * Calculates the biquad filter coefficients for a low-shelf filter.
     *
     * The method computes the coefficients based on the specified cutoff frequency, sample rate,
     * gain (in decibels), and quality factor (Q). These coefficients can be used in digital
     * signal processing to apply a low-shelf filter, which boosts or attenuates frequencies below
     * the cutoff frequency.
     *
     * @param cutoff_Hz The cutoff frequency of the low-shelf filter, in Hertz.
     * @param samplerate_Hz The sample rate of the processed audio signal, in Hertz.
     * @param gain_db The gain applied to frequencies below the cutoff, in decibels.
     * @param q The quality factor of the filter. Default value is 0.7071f. This parameter
     *          affects the slope of the transition at the cutoff frequency.
     * @return A BiquadCoefficients structure containing the calculated coefficients: a0, a1, a2, b0, b1, and b2.
     */
    inline BiquadCoefficients low_shelf_coefficients(const float cutoff_Hz, const float samplerate_Hz, const float gain_db, const float q = 0.7071f)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // α = sin(ωc) / (2 · Q)
        const float alpha = sin(wc) / (2 * std::clamp(q, MIN_Q, MAX_Q));

        // A = sqrt(10^(gain_db / 40))
        const float A = db_to_linear_sqrt(gain_db);

        // a0 = (A + 1) + (A − 1) · cos(ωc) + 2 · sqrt(A) · α
        const float a0 = A + 1 + (A - 1) * cos(wc) + 2 * sqrt(A) * alpha;

        // a1 = −2 · ((A − 1) + (A + 1) · cos(ωc))
        const float a1 = -2 * (A - 1 + (A + 1) * cos(wc));

        // a2 = (A + 1) + (A − 1) · cos(ωc) - 2 · sqrt(A) · α
        const float a2 = A + 1 + (A - 1) * cos(wc) - 2 * sqrt(A) * alpha;

        // b0 = A · ((A + 1) - (A − 1) · cos(ωc) + 2 · sqrt(A) · α)
        const float b0 = A * (A + 1 - (A - 1) * cos(wc) + 2 * sqrt(A) * alpha);

        // b1 = 2 · A · ((A - 1) - (A + 1) · cos(ωc))
        const float b1 = 2 * A * (A - 1 - (A + 1) * cos(wc));

        // b2 = A · ((A + 1) - (A − 1) · cos(ωc) - 2 · sqrt(A) · α)
        const float b2 = A * (A + 1 - (A - 1) * cos(wc) - 2 * sqrt(A) * alpha);

        return {.a0 = a0, .a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    /**
     * Calculates the biquad filter coefficients for a low-pass filter (LPF).
     *
     * The method computes the coefficients based on the specified cutoff frequency, sample rate, and quality factor (Q).
     * The coefficients can be used in digital signal processing for filtering signals.
     *
     * @param cutoff_Hz The cutoff frequency of the low-pass filter, in Hertz.
     * @param samplerate_Hz The sample rate of the processed audio signal, in Hertz.
     * @param q The quality factor of the filter. Default value is 0.7071f. This parameter affects the bandwidth of the filter.
     * @return A BiquadCoefficients structure containing the calculated coefficients: a0, a1, a2, b0, b1, and b2.
     */
    inline BiquadCoefficients lpf_filter_coefficients(const float cutoff_Hz, const float samplerate_Hz, const float q = 0.7071f)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // α = sin(ωc) / (2 · Q)
        const float alpha = sin(wc) / (2 * std::clamp(q, MIN_Q, MAX_Q));

        // a0 = 1 + α
        const float a0 = 1.0f + alpha;

        // a1 = -2 · cos(ωc)
        const float a1 = -2.0f * cos(wc);

        // a2 = 1 - α
        const float a2 = 1.0f - alpha;

        // b1 = 1 - cos(ωc)
        const float b1 = 1.0f - cos(wc);

        // b0 = (1 - cos(ωc)) / 2
        const float b0 = b1 / 2.0f;

        // b2 = (1 - cos(ωc)) / 2
        const float b2 = b0;

        return {.a0 = a0, .a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    /**
     * Calculates the biquad filter coefficients for a high-shelf filter.
     *
     * The method computes the coefficients based on the specified cutoff frequency, sample rate,
     * gain (in decibels), and quality factor (Q). These coefficients can be used in digital
     * signal processing to apply a high-shelf filter, which boosts or attenuates frequencies above
     * the cutoff frequency.
     *
     * @param cutoff_Hz The cutoff frequency of the high-shelf filter, in Hertz.
     * @param samplerate_Hz The sample rate of the processed audio signal, in Hertz.
     * @param gain_db The gain applied to frequencies above the cutoff, in decibels.
     * @param q The quality factor of the filter. Default value is 0.7071f. This parameter
     *          affects the slope of the transition at the cutoff frequency.
     * @return A BiquadCoefficients structure containing the calculated coefficients: a0, a1, a2, b0, b1, and b2.
     */
    inline BiquadCoefficients high_shelf_coefficients(const float cutoff_Hz, const float samplerate_Hz, const float gain_db, const float q = 0.7071f)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // α = sin(ωc) / (2 · Q)
        const float alpha = sin(wc) / (2 * std::clamp(q, MIN_Q, MAX_Q));

        // A = sqrt(10^(gain_db / 20))
        const float A = db_to_linear_sqrt(gain_db);

        // a0 = (A + 1) - (A − 1) · cos(ωc) + 2 · sqrt(A) · α
        const float a0 = A + 1 - (A - 1) * cos(wc) + 2 * sqrt(A) * alpha;

        // a1 = 2 · ((A − 1) + (A - 1) · cos(ωc))
        const float a1 = 2 * (A - 1 - (A + 1) * cos(wc));

        // a2 = (A + 1) - (A − 1) · cos(ωc) - 2 · sqrt(A) · α
        const float a2 = A + 1 - (A - 1) * cos(wc) - 2 * sqrt(A) * alpha;

        // b0 = A · ((A + 1) + (A − 1) · cos(ωc) + 2 · sqrt(A) · α)
        const float b0 = A * (A + 1 + (A - 1) * cos(wc) + 2 * sqrt(A) * alpha);

        // b1 = -2 · A · ((A - 1) - (A + 1) · cos(ωc))
        const float b1 = -2 * A * (A - 1 + (A + 1) * cos(wc));

        // b2 = A · ((A + 1) + (A − 1) · cos(ωc) - 2 · sqrt(A) · α)
        const float b2 = A * (A + 1 + (A - 1) * cos(wc) - 2 * sqrt(A) * alpha);

        return {.a0 = a0, .a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    /**
     * Calculates the biquad filter coefficients for a high-pass filter (HPF).
     *
     * The method computes the coefficients based on the specified cutoff frequency, sample rate, and quality factor (Q).
     * The coefficients can be used in digital signal processing for filtering signals.
     *
     * @param cutoff_Hz The cutoff frequency of the high-pass filter, in Hertz.
     * @param samplerate_Hz The sample rate of the processed audio signal, in Hertz.
     * @param q The quality factor of the filter. Default value is 0.7071f. This parameter affects the bandwidth of the filter.
     * @return A BiquadCoefficients structure containing the calculated coefficients: a0, a1, a2, b0, b1, and b2.
     */
    inline BiquadCoefficients hpf_filter_coefficients(const float cutoff_Hz, const float samplerate_Hz, const float q = 0.7071f)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(cutoff_Hz, samplerate_Hz);

        // α = sin(ωc) / (2 · Q)
        const float alpha = sin(wc) / (2 * std::clamp(q, MIN_Q, MAX_Q));

        // a0 = 1 + α
        const float a0 = 1.0f + alpha;

        // a1 = -2 · cos(ωc)
        const float a1 = -2.0f * cos(wc);

        // a2 = 1 - α
        const float a2 = 1.0f - alpha;

        // b0 = (1 + cos(ωc)) / 2
        const float b0 = (1 + cos(wc)) / 2.0f;

        // b1 = -(1 + cos(ωc))
        const float b1 = -(1.0f + cos(wc));

        // b2 = (1 + cos(ωc)) / 2
        const float b2 = b0;

        return {.a0 = a0, .a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    /**
     * Calculates the biquad filter coefficients for a peaking equalizer (EQ) filter.
     *
     * This method computes the filter coefficients based on the specified cutoff frequency, sample rate, gain in decibels,
     * and quality factor (Q). The coefficients are used in digital signal processing for shaping the frequency response of
     * an audio signal around a given frequency.
     *
     * @param center_Hz The center frequency of the peaking EQ filter, in Hertz.
     * @param samplerate_Hz The sample rate of the processed audio signal, in Hertz.
     * @param gain_db The gain applied at the center frequency, in decibels.
     * @param q The quality factor of the filter. Default value is 0.7071f. This parameter controls the bandwidth of the filter.
     * @return A BiquadCoefficients structure containing the calculated coefficients: a0, a1, a2, b0, b1, and b2.
     */
    inline BiquadCoefficients peaking_eq_coefficients(const float center_Hz, const float samplerate_Hz, const float gain_db, const float q = 0.7071f)
    {
        // ωc = 2π · fc / fs;
        const float wc = angular_cutoff_from_Hz(center_Hz, samplerate_Hz);

        // α = sin(ωc) / (2 · Q)
        const float alpha = sin(wc) / (2 * std::clamp(q, MIN_Q, MAX_Q));

        // A = sqrt(10^(gain_db / 20))
        const float A = db_to_linear_sqrt(gain_db);

        // a0 = 1 + α / A
        const float a0 = 1.0f + alpha / A;

        // a1 = -2 · cos(ωc)
        const float a1 = -2.0f * cos(wc);

        // a2 = 1 - α / A
        const float a2 = 1.0f - alpha / A;

        // b0 = 1 + a · A
        const float b0 = 1.0f + alpha * A;

        // b1 = -2 · cos(ωc)
        const float b1 = a1;

        // b2 = 1 - a · A
        const float b2 = 1.0f - alpha * A;

        return {.a0 = a0, .a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }
}

#endif
