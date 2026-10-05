#ifndef ANS_TONE_GENERATOR_TYPES_HPP
#define ANS_TONE_GENERATOR_TYPES_HPP

#include "ans_dsp_common.hpp"
#include "ans_dsp_oscillator.hpp"
#include "ans_dsp_smoothing.hpp"
#include <atomic>

namespace ans_tone_generator
{
    enum OSC_TYPE
    {
        SINE = 0,
        SQUARE,
        SAW,
        TRIANGLE,
        NOISE,

        MAX,
    };

    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_freq_Hz_default;
        float m_gain_default;
        OSC_TYPE m_oscillator_type_default;

        float m_smoothing_time_ms;

        int samplerate;
        unsigned int buffer_size;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_freq_Hz_current(init_data.m_freq_Hz_default)
            , m_freq_Hz_smoothed(m_freq_Hz_current)
            , m_gain_lin_current(ans_dsp::db_to_linear(init_data.m_gain_default))
            , m_gain_lin_smoothed(0.f)
            , m_param_oscillator_type(init_data.m_oscillator_type_default)
            , m_freq_smoothing_coefficient(0.f)
            , m_gain_smoothing_coefficient(0.f)
            , m_phase_increment(ans_dsp::get_phase_increment(m_defaults.m_freq_Hz_default, m_defaults.samplerate))
            , m_counter(0.0)
        {
            const float buffer_duration_ms = ans_dsp::samples_to_ms(m_defaults.buffer_size, static_cast<float>(m_defaults.samplerate));
            m_freq_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(buffer_duration_ms, m_defaults.m_smoothing_time_ms);

            const float sample_duration_ms = ans_dsp::samples_to_ms(1u, static_cast<float>(m_defaults.samplerate));
            m_gain_smoothing_coefficient = ans_dsp::smoothing_coefficient_from_ms(sample_duration_ms, m_defaults.m_smoothing_time_ms);
        }

        ANS_DSP_Init_Data m_defaults;

        std::atomic<float> m_freq_Hz_current;
        float m_freq_Hz_smoothed;
        std::atomic<float> m_gain_lin_current;
        float m_gain_lin_smoothed;
        std::atomic<OSC_TYPE> m_param_oscillator_type;
        float m_freq_smoothing_coefficient;
        float m_gain_smoothing_coefficient;

        double m_phase_increment;
        uint32_t m_noise_state = 0x9E3779B9; // Seed
        double m_counter;
    };
}

#endif
