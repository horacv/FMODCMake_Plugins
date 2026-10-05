#ifndef FMOD_PLUGINS_ANS_DSP_OSCILLATOR_HPP
#define FMOD_PLUGINS_ANS_DSP_OSCILLATOR_HPP

#include <algorithm>
#include <cassert>
#include <numbers>

namespace ans_dsp
{
    /**
     * Calculates the phase increment for a given frequency and sample rate.
     *
     * This method ensures the calculated phase increment is clamped to a valid range
     * based on the Nyquist frequency and a safety factor to avoid aliasing. The phase
     * increment determines the step size for advancing the phase of an oscillator per
     * sample in digital signal processing.
     *
     * References:
     * W. Pirkle, "Designing Audio Effect Plugins in C++", 2nd ed., Routledge, 2019.
     * Ch. 13.1, LFO Algorithms: modulo counter, eq. 13.2
     *
     * @param freq_Hz The frequency in Hertz (Hz) for which the phase increment is calculated.
     *                It is clamped to a range between 0 Hz and the Nyquist limit of the sample rate.
     * @param samplerate_Hz The sample rate in Hertz (Hz) at which the system operates.
     *                      Must be a positive, non-zero value.
     * @return The phase increment as a double, representing the fraction of the sample rate
     *         that corresponds to the given frequency.
     * @note Formula: inc = fo / fs
     */
    inline double get_phase_increment(const float freq_Hz, const unsigned int samplerate_Hz)
    {
        if (samplerate_Hz == 0)
        {
            assert(!"samplerate must be positive");
            return 0.f;
        }

        // Stay just below Nyquist: inc < 0.5 guarantees a single subtraction always wraps the counter.
        constexpr float nyquist_safety_factor = 0.99f;
        const float upper_limit_Hz = static_cast<float>(samplerate_Hz) * 0.5f * nyquist_safety_factor;
        const double clamped_freq_Hz = std::clamp(freq_Hz, 0.f, upper_limit_Hz);

        return clamped_freq_Hz / static_cast<double>(samplerate_Hz);
    }

    /**
     * Advances a modulo counter by a specified increment while ensuring both the counter
     * and increment are clamped to valid ranges, and wraps the result within the range [0.0, 1.0).
     *
     * This function is typically used in digital signal processing to manage phase counters
     * for oscillators, ensuring that the counter remains within a normalized range suitable
     * for generating periodic waveforms.
     *
     * @param counter The current value of the modulo counter. It must be within the range [0.0, 1.0).
     *                If it falls outside this range, it will be clamped to fit within.
     * @param increment The increment value to add to the counter. It must be in the range [0.0, 0.5).
     *                  Values outside this range will be clamped accordingly.
     * @return The updated counter value after applying the increment, wrapped into the range [0.0, 1.0).
     */
    inline double modulo_counter_advance_wrap(const double counter, const double increment)
    {
        double counter_clamped = counter;
        if (counter < 0.0 || counter >= 1.0)
        {
            assert(!"counter must be between 0.0 and less than 1.0");
            counter_clamped = std::clamp(counter, 0.0, 1.0);
        }

        double increment_clamped = increment;
        if (increment < 0.0 || increment >= 0.5)
        {
            assert(!"increment must be between 0.0 and less than 0.5");
            increment_clamped = std::clamp(increment, 0.0, 0.5);
        }
        const double next = counter_clamped + increment_clamped;

        return next >= 1.0 ? next - 1.0 : next;
    }

    /**
     * PolyBLEP (polynomial band-limited step) correction for a jump of height 2 at counter 0.
     * Returns a value to combine with the naive waveform: subtract for a falling jump
     * (+1 to -1), add for a rising jump (-1 to +1). Nonzero only within one sample of the jump.
     *
     * References:
     *   V. Valimaki, A. Huovilainen, "Antialiasing Oscillators in Subtractive Synthesis",
     *     IEEE Signal Processing Magazine, vol. 24, no. 2, 2007.
     *   https://www.martin-finke.de/articles/audio-plugins-018-polyblep-oscillator/
     *   https://github.com/martinfinke/PolyBLEP/blob/master/PolyBLEP.cpp
     *
     * @param counter   Position in the cycle, [0, 1). For a jump elsewhere, shift it to 0 first.
     * @param increment Phase increment per sample, [0, 0.5).
     *
     */
    inline double poly_blep(const double counter, const double increment)
    {
        // First sample after the jump.
        if (counter < increment)
        {
            const double t = counter / increment; // distance after the jump, in samples: [0, 1)
            return t + t - t * t - 1.0;
        }

        // Last sample before the jump.
        if (counter > 1.0 - increment)
        {
            const double t = (counter - 1.0) / increment; // distance before the jump: (-1, 0)
            return t * t + t + t + 1.0;
        }

        return 0.0;
    }

    /**
     * Generates a sawtooth waveform value based on the provided counter and scalar.
     *
     * This function computes the value of a normalized unipolar sawtooth waveform,
     * converts it to a bipolar representation, and scales the result by the given scalar.
     *
     * @param counter The current value of the phase counter, expected to be in the
     *                normalized range [0.0, 1.0). Values outside this range may produce
     *                undefined behavior.
     * @param scalar An optional scaling factor to modulate the waveform amplitude.
     *               Defaults to 1.0.
     * @return The scaled bipolar sawtooth waveform value as a double.
     */
    inline double generate_saw(const double counter, const float scalar = 1.f)
    {
        return unipolar_to_bipolar(counter) * scalar;
    }

    /**
     * Generates a sawtooth waveform with PolyBLEP (polynomial band-limited step) correction to reduce aliasing.
     *
     * Smooths the jump at counter 0 over about one sample on each side.
     *
     * @param counter   Position in the cycle, [0, 1).
     * @param increment Phase increment per sample, [0, 0.5). Must match the counter's step.
     * @param scalar    Amplitude, applied after the correction.
     */
    inline double generate_saw_corrected(const double counter, const double increment, const float scalar = 1.f)
    {
        return (generate_saw(counter) - poly_blep(counter, increment)) * scalar;
    }

    /**
     * Generates a sine waveform value based on the provided counter and scalar.
     *
     * This function computes the value of a sine waveform using an intermediate sawtooth waveform
     * as the phase input. The sawtooth waveform is scaled by π, the sine of the resulting angle is
     * calculated, and the final result is scaled by the given scalar.
     *
     * @param counter The current value of the phase counter, expected to be in the normalized range [0.0, 1.0).
     *                Values outside this range may produce undefined behavior.
     * @param scalar An optional scaling factor to modulate the waveform amplitude. Defaults to 1.0.
     * @return The scaled sine waveform value as a double.
     */
    inline double generate_sine(const double counter, const float scalar = 1.f)
    {
        const double angle = generate_saw(counter) * std::numbers::pi_v<double>;
        return std::sin(angle) * scalar;
    }

    /**
     * Generates a triangle waveform value based on the provided counter and scalar.
     *
     * This function computes a triangle waveform by first generating a normalized
     * sawtooth waveform value and converting it to the corresponding triangle waveform.
     * The result is then scaled by the given scalar value to adjust the amplitude.
     *
     * References:
     *   W. Pirkle, "Designing Audio Effect Plugins in C++", 2nd ed., Routledge, 2019.
     *     Ch. 13.1, LFO Algorithms: eq. 13.1, triangle = 2 * |bipolar saw| - 1.
     *   "Triangle wave", Wikipedia: triangle as the absolute value of a shifted sawtooth.
     *     https://en.wikipedia.org/wiki/Triangle_wave
     *
     * @param counter The current value of the phase counter, expected to be in the normalized
     *                range [0.0, 1.0). Values outside this range may produce undefined behavior.
     * @param scalar An optional scaling factor to modulate the waveform amplitude. Defaults to 1.0.
     * @return The scaled triangle waveform value as a double.
     * @note Formula: triangle = 2 * |bipolar saw| - 1.
     */
    inline double generate_triangle(const double counter, const float scalar = 1.f)
    {
        const double saw = generate_saw(counter);
        return (2 * std::abs(saw) - 1) * scalar;
    }

    /**
     * Generates a square waveform based on the input phase counter and an optional amplitude scalar.
     *
     * This method produces a square wave by evaluating a comparator on the output of
     * a sawtooth waveform generator. Positive values produce 1.0 * scalar, and negative
     * values produce -1.0 * scalar.
     *
     * @param counter The phase counter input, typically within the range [0.0, 1.0).
     *                This represents the phase progression of the waveform.
     * @param scalar An optional amplitude scaling factor. Default value is 1.0.
     *               Used to scale the output waveform's amplitude.
     * @return The square wave value as a double, scaled by the amplitude scalar.
     */
    inline double generate_square(const double counter, const float scalar = 1.f)
    {
        const double saw = generate_saw(counter);
        return (saw >= 0.0 ? 1.0 : -1.0) * scalar;
    }

    /**
     * Generates a square waveform with PolyBLEP (polynomial band-limited step) correction to reduce aliasing.
     * Smooths both jumps: the falling edge at counter 0 and the rising edge at 0.5.
     *
     * @param counter   Position in the cycle, [0, 1).
     * @param increment Phase increment per sample, [0, 0.5). Must match the counter's step.
     * @param scalar    Amplitude, applied after the corrections.
     */
    inline double generate_square_corrected(const double counter, const double increment, const float scalar = 1.f)
    {
        // poly_blep corrects a jump at counter 0. Shift by half a cycle so the rising edge at 0.5 lands at 0.
        const double counter_shifted = counter >= 0.5 ? counter - 0.5 : counter + 0.5;
        return (generate_square(counter) - poly_blep(counter, increment) + poly_blep(counter_shifted, increment)) * scalar;
    }
}
#endif
