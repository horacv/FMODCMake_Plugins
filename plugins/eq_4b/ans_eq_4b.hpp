#ifndef ANS_EQ_4B_HPP
#define ANS_EQ_4B_HPP

#include "fmod_common.h"
#include "fmod_dsp.h"

namespace ans_eq_4b
{
    // PARAMETERS

    enum DSP_PARAMETER_INDEX {
        // LOW BAND
        PARAM_L_FREQ = 0,
        PARAM_L_TYPE,
        PARAM_L_GAIN,
        PARAM_L_Q,

        // LOW MID BAND
        PARAM_LM_FREQ,
        PARAM_LM_GAIN,
        PARAM_LM_Q,

        // HIGH MID BAND
        PARAM_HM_FREQ,
        PARAM_HM_GAIN,
        PARAM_HM_Q,

        // HIGH BAND
        PARAM_H_FREQ,
        PARAM_H_Q,
        PARAM_H_GAIN,
        PARAM_H_TYPE,
        
        PARAM_COUNT
    };

    // LOW BAND
    static FMOD_DSP_PARAMETER_DESC param_l_freq;
    static FMOD_DSP_PARAMETER_DESC param_l_type;
    static FMOD_DSP_PARAMETER_DESC param_l_gain;
    static FMOD_DSP_PARAMETER_DESC param_l_q;

    // LOW MID BAND
    static FMOD_DSP_PARAMETER_DESC param_lm_freq;
    static FMOD_DSP_PARAMETER_DESC param_lm_gain;
    static FMOD_DSP_PARAMETER_DESC param_lm_q;

    // HIGH MID BAND
    static FMOD_DSP_PARAMETER_DESC param_hm_freq;
    static FMOD_DSP_PARAMETER_DESC param_hm_gain;
    static FMOD_DSP_PARAMETER_DESC param_hm_q;

    // HIGH BAND
    static FMOD_DSP_PARAMETER_DESC param_h_freq;
    static FMOD_DSP_PARAMETER_DESC param_h_q;
    static FMOD_DSP_PARAMETER_DESC param_h_gain;
    static FMOD_DSP_PARAMETER_DESC param_h_type;

    inline FMOD_DSP_PARAMETER_DESC* param_description[PARAM_COUNT] {
        // LOW BAND
        &param_l_freq,
        &param_l_type,
        &param_l_gain,
        &param_l_q,

        // LOW MID BAND
        &param_lm_freq,
        &param_lm_gain,
        &param_lm_q,

        // HIGH MID BAND
        &param_hm_freq,
        &param_hm_gain,
        &param_hm_q,

        // HIGH BAND
        &param_h_freq,
        &param_h_q,
        &param_h_gain,
        &param_h_type,
    };

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_release(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_reset(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int buffer_size, const FMOD_DSP_BUFFER_ARRAY* in_buffer_array, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL inputs_idle, FMOD_DSP_PROCESS_OPERATION operation);
    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value);
    FMOD_RESULT F_CALL dsp_set_param_int(FMOD_DSP_STATE* dsp_state, int index, int value);
    FMOD_RESULT F_CALL dsp_get_param_float(FMOD_DSP_STATE* dsp_state, int index, float* value, char* value_str);
    FMOD_RESULT F_CALL dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str);
    FMOD_RESULT F_CALL dsp_sys_register(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_deregister(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_mix(FMOD_DSP_STATE* dsp_state, int stage);
}

#endif
