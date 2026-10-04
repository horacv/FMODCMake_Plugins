#ifndef ANS_TONE_GENERATOR_STATE_HPP
#define ANS_TONE_GENERATOR_STATE_HPP

#include "ans_tone_generator_types.hpp"
#include "fmod_common.h"

namespace ans_tone_generator
{
    class ANS_DSP_State {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data &init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void set_freq_Hz(float value);
        void set_gain_db(float db);
        void set_osc_type(OSC_TYPE type);

        [[nodiscard]] float get_freq_Hz() const;
        [[nodiscard]] float get_gain_db() const;
        [[nodiscard]] OSC_TYPE get_osc_type() const;

        FMOD_RESULT generate(float* out_buffer, unsigned int buffer_size, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };
}

#endif
