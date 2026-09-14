#ifndef ANS_GAIN_STATE_HPP
#define ANS_GAIN_STATE_HPP

#include "ans_gain_types.hpp"
#include "fmod_common.h"

namespace ans_gain
{
    class ANS_DSP_State
    {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data& init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset(int samplerate_current);
        void set_gain_db(float db);
        [[nodiscard]] float get_gain_db() const;
        FMOD_RESULT process(const float* in_buffer, float* out_buffer, unsigned int buffer_size, int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };

}
#endif
