#ifndef ANS_HPF_1P_STATE_HPP
#define ANS_HPF_1P_STATE_HPP

#include "ans_hpf_1p_types.hpp"
#include "fmod_common.h"

namespace ans_hpf_1p
{
    class ANS_DSP_State
    {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data& init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset();
        void set_cutoff_Hz(float value);
        [[nodiscard]] float get_cutoff_Hz() const { return m_data.m_cutoff_Hz_current; }
        FMOD_RESULT process(const float* in_buffer, float* out_buffer,
            unsigned int buffer_size, int samplerate,  int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
    };
}

#endif
