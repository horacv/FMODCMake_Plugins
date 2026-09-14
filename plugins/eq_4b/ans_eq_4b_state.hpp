#ifndef ANS_EQ_4B_STATE_HPP
#define ANS_EQ_4B_STATE_HPP

#include "fmod_common.h"
#include "ans_eq_4b_types.hpp"

namespace ans_eq_4b
{
    class ANS_DSP_State
    {
    public:
        explicit ANS_DSP_State(const ANS_DSP_Init_Data& init_data) : m_data(init_data) {}
        ~ANS_DSP_State() = default;

        void reset(unsigned int buffer_size, unsigned int samplerate);
        void set_freq_Hz(BAND band, float value);
        void set_filter_type(BAND band, FILTER_TYPE type);
        void set_gain_db(BAND band, float db);
        void set_Q(BAND band, float value);
        [[nodiscard]] float get_freq_Hz(BAND band) const;
        [[nodiscard]] FILTER_TYPE get_filter_type(BAND band) const;
        [[nodiscard]] float get_gain_db(BAND band) const;
        [[nodiscard]] float get_Q(BAND band) const;
        FMOD_RESULT process(const float* in_buffer, float* out_buffer,
            unsigned int buffer_size, int samplerate,  int in_channels, int out_channels);

    private:
        // Defaults
        ANS_DSP_State_Data m_data;
    };
}

#endif
