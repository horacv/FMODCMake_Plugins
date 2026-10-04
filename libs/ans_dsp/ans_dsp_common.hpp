#ifndef ANS_DSP_COMMON_HPP
#define ANS_DSP_COMMON_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

namespace ans_dsp
{
    /**
     * Converts a decibel (dB) value to a linear scale.
     *
     * @param dB The value in decibels to be converted.
     * @return The corresponding value in linear scale.
     * @note Formula: 10^(dB · 0.05) or 10^(dB / 20)
     */
    [[nodiscard]] inline float db_to_linear(const float dB)
    {
        return powf(10, dB * 0.05f);
    }

    /**
     * Converts a decibel (dB) value to the square root of its linear equivalent.
     * Used for the Audio EQ Cookbook's `A` coefficient in peaking/shelving filters.
     *
     * @param dB The value in decibels to be converted.
     * @return The corresponding value in linear scale -> sqrt(db_to_linear(dB)), computed directly via a halved exponent.
     * @note Formula: 10^(dB · 0.025) or 10^(dB / 40)
     * @link https://www.w3.org/TR/audio-eq-cookbook/#formulae
     */
    [[nodiscard]] inline float db_to_linear_sqrt(const float dB)
    {
        return powf(10, dB * 0.025f);
    }

    /**
     * Converts a linear value to a decibel (dB) scale.
     *
     * @param value The value in linear scale to be converted.
     * @return The corresponding value in decibels.
     * @note Formula: 20 * log10(value)
     */
    [[nodiscard]] inline float linear_to_db(const float value)
    {
        return log10f(value) * 20;
    }

    /**
     * Converts a time value from milliseconds to seconds.
     *
     * @param ms The time value in milliseconds to be converted.
     * @return The equivalent time value in seconds.
     */
    constexpr float ms_to_seconds(const float ms)
    {
        return ms * 0.001f;
    }

    /**
     * Converts a time value from seconds to milliseconds.
     *
     * @param seconds The time value in seconds to be converted.
     * @return The equivalent time value in milliseconds.
     */
    constexpr float seconds_to_ms(const float seconds)
    {
        return seconds * 1000.0f;
    }

    /**
     * Converts a time value from seconds to samples.
     *
     * @param seconds The time value in seconds to be converted.
     * @param samplerate The sampling rate in Hz.
     * @return The equivalent time value in samples.
     */
    inline unsigned int seconds_to_samples(const float seconds, const float samplerate)
    {
        return static_cast<unsigned int>(std::round(seconds * samplerate));
    }

    /**
     * Converts a time value from milliseconds to samples.
     *
     * @param ms The time value in milliseconds to be converted.
     * @param samplerate The sampling rate in Hz.
     * @return The equivalent time value in samples.
     */
    inline unsigned int ms_to_samples(const float ms, const float samplerate)
    {
        return static_cast<unsigned int>(std::round(ms_to_seconds(ms) * samplerate));
    }

    /**
     * Converts a time value from samples to seconds.
     *
     * @param samples The time value in samples to be converted.
     * @param samplerate The sampling rate in Hz.
     * @return The equivalent time value in seconds.
     */
    inline float samples_to_seconds(const unsigned int samples, const float samplerate)
    {
        return samplerate != 0 ? static_cast<float>(samples) / samplerate : 0.0f;
    }

    /**
     * Converts a time value from samples to milliseconds.
     *
     * @param samples The time value in samples to be converted.
     * @param samplerate The sampling rate in Hz.
     * @return The equivalent time value in milliseconds.
     */
    inline float samples_to_ms(const unsigned int samples, const float samplerate)
    {
        return seconds_to_ms(samples_to_seconds(samples, samplerate));
    }

    /**
     * Maps a unipolar value [0, 1] to bipolar [-1, 1]: 0 -> -1, 0.5 -> 0, 1 -> 1.
     * Linear and unclamped; inputs outside [0, 1] map proportionally.
     * Inverse of bipolar_to_unipolar.
     *
     * @param value The input value in the unipolar range [0, 1].
     * @return The corresponding value in the bipolar range [-1, 1].
     */
    inline double unipolar_to_bipolar(const double value)
    {
        return value * 2.0 - 1.0;
    }

    /**
     * Maps a bipolar value [-1, 1] to unipolar [0, 1]: -1 -> 0, 0 -> 0.5, 1 -> 1.
     * Linear and unclamped; inputs outside [-1, 1] map proportionally.
     * Inverse of unipolar_to_bipolar.
     *
     * @param value The bipolar value to be converted.
     * @return The corresponding unipolar value.
     */
    inline double bipolar_to_unipolar(const double value)
    {
        return value * 0.5 + 0.5;
    }

    /**
     * Converts a 32-bit unsigned integer state value to a unipolar double in the range [0, 1).
     *
     * The method maps the input `state` uniformly across the unipolar range by dividing
     * it by the total number of possible 32-bit unsigned integer values.
     *
     * @param state The 32-bit unsigned integer value to be converted.
     * @return A double value representing the unipolar equivalent of the input,
     *         in the range [0, 1).
     */
    inline double uint32_unipolar(const uint32_t state)
    {
        constexpr double uint32_value_count = static_cast<double>(std::numeric_limits<uint32_t>::max()) + 1.0;
        return state / uint32_value_count;
    }

    /**
     * Converts a 32-bit unsigned integer state value to a bipolar double in the range [-1, 1).
     *
     * The method first transforms the input `state` into a unipolar double value in the range [0, 1)
     * using `uint32_unipolar`. It then maps the unipolar value to a bipolar range by applying
     * `unipolar_to_bipolar`.
     *
     * @param state The 32-bit unsigned integer value to be converted.
     * @return A double value representing the bipolar equivalent of the input,
     *         in the range [-1, 1).
     */
    inline double uint32_bipolar(const uint32_t state)
    {
        return unipolar_to_bipolar(uint32_unipolar(state));
    }

    /**
     * Computes the angular cutoff frequency in radians per second, given a linear frequency in Hz and a sampling rate.
     *
     * The provided frequency is clamped to a safe range determined by a lower limit of 10 Hz and an upper limit slightly below half the sample rate
     * to avoid a numerical singularity near π.
     *
     * @param cutoff_Hz The frequency in Hz to be converted to angular frequency. It will be clamped to a safe range.
     * @param samplerate_Hz The sampling rate in Hz. Must be high enough to accommodate the clamping range.
     * @return The angular frequency in radians per second.
     * @note Formula: ωc = 2π · fc / fs;
     */
    constexpr float angular_cutoff_from_Hz(const float cutoff_Hz, const float samplerate_Hz)
    {
        // Keeps the cutoff a little below the highest frequency this math can handle (numerical singularity while approaching π).
        constexpr float nyquist_safety_factor = 0.99f;

        // An arbitrary lower limit below human hearing.
        constexpr float lower_limit_Hz = 10.0f;

        // An upper limit slightly below samplerate / 2 to avoid a numerical singularity near π.
        const float upper_limit_Hz = samplerate_Hz * 0.5f * nyquist_safety_factor;

        if (lower_limit_Hz > upper_limit_Hz) [[unlikely]]
        {
            assert(!"samplerate too low for clamp range");
            return 0.f;
        }

        const float freqHz_clamped = std::clamp(cutoff_Hz, lower_limit_Hz, upper_limit_Hz);

        // ωc = 2π · fc / fs;
        return 2.0f * std::numbers::pi_v<float> * freqHz_clamped / samplerate_Hz;
    }

    /**
     * Flushes subnormal floating-point values to zero.
     * A subnormal value is considered as one that falls below a threshold
     * equivalent to the decibel-to-linear conversion of a predefined minimum dB value.
     *
     * @param value The floating-point value to check and adjust.
     * @return Zero if the absolute value of the input is subnormal; otherwise, returns the original value.
     * @note Checks for a minimum constant of -300 dBFS, which is far below any real signal or noise floor.
     */
    inline float flush_subnormal_to_zero(const float value)
    {
        // https://www.youtube.com/watch?v=y-NOz94ZEOA&t=12s
        // https://en.wikipedia.org/wiki/Subnormal_number
        constexpr float MIN_DB_THRESHOLD = -300.0f;
        return std::fabs(value) < db_to_linear(MIN_DB_THRESHOLD) ? 0.0f : value;
    }
}

#endif
