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
        void set_active(const bool bActive) { m_data.playback_state = bActive ? ANS_DSP_ProcessingState::Active : ANS_DSP_ProcessingState::NotActive; }
        void start_fading_out();
        [[nodiscard]] bool is_fading_out() const { return m_data.playback_state == ANS_DSP_ProcessingState::FadingOut; }
        [[nodiscard]] bool is_active() const { return m_data.playback_state != ANS_DSP_ProcessingState::NotActive; }
        FMOD_RESULT process(const float* in_buffer, float* out_buffer, unsigned int buffer_size, int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };

}
#endif
