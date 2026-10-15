#ifndef ANS_DELAY_STATE_HPP
#define ANS_DELAY_STATE_HPP

#include "ans_delay_types.hpp"
#include "fmod_common.h"

namespace ans_delay
{
    class ANS_DSP_State
    {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data& init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset(unsigned int buffer_size, int samplerate_current);
        void set_delay_ms(float delay_ms);
        [[nodiscard]] float get_delay_ms() const;
        void set_wet_gain_db(float db);
        [[nodiscard]] float get_wet_gain_db() const;
        void set_dry_gain_db(float db);
        [[nodiscard]] float get_dry_gain_db() const;
        void set_feedback_percent(float value);
        [[nodiscard]] float get_feedback_percent() const;

        unsigned int get_tail_length_samples() const;

        FMOD_RESULT process(const float* in_buffer, float* out_buffer, unsigned int buffer_size, int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };

}
#endif
