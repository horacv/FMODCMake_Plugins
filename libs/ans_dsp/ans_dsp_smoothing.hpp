#ifndef ANS_DSP_SMOOTHING_HPP
#define ANS_DSP_SMOOTHING_HPP

#include <cmath>
#include "ans_dsp_common.hpp"

namespace ans_dsp
{
    /**
     * Computes a smoothing coefficient given a time step and a smoothing time in seconds.
     *
     * This function calculates a coefficient that can be used for smoothing signals or
     * values over time. The calculation uses an exponential decay function based on
     * the provided time step and smoothing time.
     *
     * If the smoothing time is zero, the method returns a coefficient of 0.0.
     *
     * @param time_step_seconds The time interval in seconds for each step.
     * @param smoothing_time_seconds The smoothing time constant in seconds.
     * @return The smoothing coefficient as a float. Returns 0.0 if smoothing_time_seconds is 0.0.
     */
    inline float smoothing_coefficient_from_seconds(const float time_step_seconds, const float smoothing_time_seconds)
    {
        return smoothing_time_seconds != 0.0f ? std::exp(-time_step_seconds / smoothing_time_seconds) : 0.0f;
    }

    /**
     * Computes a smoothing coefficient given a time step and a smoothing time in milliseconds.
     *
     * This function calculates a coefficient that can be used for smoothing signals or
     * values over time. The calculation uses an exponential decay function based on
     * the provided time step and smoothing time in milliseconds.
     *
     * If the smoothing time is zero, the method returns a coefficient of 0.0.
     *
     * @param time_step_ms The time interval in milliseconds for each step.
     * @param smoothing_time_ms The smoothing time constant in milliseconds.
     * @return The smoothing coefficient as a float. Returns 0.0 if smoothing_time_ms is 0.0.
     */
    inline float smoothing_coefficient_from_ms(const float time_step_ms, const float smoothing_time_ms)
    {
        return smoothing_coefficient_from_seconds(ms_to_seconds(time_step_ms), ms_to_seconds(smoothing_time_ms));
    }

    /**
     * Smoothly interpolates a value towards a target using a smoothing coefficient.
     *
     * This method updates the current smoothed value (`inout_smoothed`) by applying
     * an interpolation step towards the target value (`in_target`), controlled by
     * the smoothing coefficient (`in_smoothing_coefficient`). The interpolation also
     * respects a given tolerance to handle cases where the smoothed value is close
     * enough to the target.
     *
     * If the target value equals the current smoothed value, the method exits early.
     * Similarly, if the difference between the target and the smoothed value is
     * within the specified tolerance, the smoothed value is directly set to the target.
     *
     * @param in_target The target value to interpolate towards.
     * @param in_smoothing_coefficient The smoothing coefficient, a value between 0.0 and 1.0,
     *                                  where higher values result in slower smoothing.
     * @param inout_smoothed A reference to the current smoothed value, which is updated
     *                       by this method.
     * @param tolerance The tolerance for considering the smoothed value to be close
     *                  enough to the target. Defaults to 0.1.
     * @return True if the smoothed value was updated, false otherwise.
     */
    inline bool interpolate_with_coefficient(const float in_target, const float in_smoothing_coefficient, float& inout_smoothed, const float tolerance = 0.1f)
    {
        if (in_target == inout_smoothed)
        {
            return false;
        }
        if (std::abs(in_target - inout_smoothed) <= tolerance)
        {
            inout_smoothed = in_target;
            return false;
        }
        inout_smoothed += (in_target - inout_smoothed) * (1.0f - in_smoothing_coefficient);
        return true;
    }
}

#endif
