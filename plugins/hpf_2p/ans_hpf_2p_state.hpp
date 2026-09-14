#ifndef ANS_HPF_2P_STATE_HPP
#define ANS_HPF_2P_STATE_HPP

#include "ans_hpf_2p_types.hpp"
#include "fmod_common.h"

namespace ans_hpf_2p
{
    class ANS_DSP_State
    {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data& init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset(unsigned int buffer_size, unsigned int samplerate);
        void set_cutoff_Hz(float value);
        void set_Q(float value);
        [[nodiscard]] float get_cutoff_Hz() const { return m_data.m_cutoff_Hz_current.load(); }
        [[nodiscard]] float get_Q() const { return m_data.m_Q_current.load(); }
        FMOD_RESULT process(const float* in_buffer, float* out_buffer,
            unsigned int buffer_size, int samplerate,  int in_channels, int out_channels);

    private:
        ANS_DSP_State_Data m_data;
        static void interpolate_parameters(ANS_DSP_State_Data& data);
    };
}

#endif