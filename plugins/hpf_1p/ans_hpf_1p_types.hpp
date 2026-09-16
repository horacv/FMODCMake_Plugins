#ifndef ANS_HPF_1P_TYPES_HPP
#define ANS_HPF_1P_TYPES_HPP

#include "fmod_common.h"
#include <array>

namespace ans_hpf_1p
{
    // Defaults and initialization values
    struct ANS_DSP_Init_Data {
        float m_cutoff_Hz_default;
        float m_cutoff_Hz_max;
        float m_cutoff_Hz_min;
    };

    // The current state of the dsp effect
    struct ANS_DSP_State_Data {
        explicit ANS_DSP_State_Data(const ANS_DSP_Init_Data& init_data)
            : m_defaults(init_data)
            , m_cutoff_Hz_current(std::clamp(init_data.m_cutoff_Hz_default,init_data.m_cutoff_Hz_min, init_data.m_cutoff_Hz_max))
        {}

        ANS_DSP_Init_Data m_defaults;

        // y[n-1]: previous low passed sample per channel
        std::array<float, FMOD_MAX_CHANNEL_WIDTH> lpf_prev {};
        std::atomic<float> m_cutoff_Hz_current;

        void reset()
        {
            lpf_prev.fill(0);
        }
    };
}

#endif
