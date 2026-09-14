#ifndef ANS_LPF_1P_HPP
#define ANS_LPF_1P_HPP

#include "fmod_common.h"
#include "fmod_dsp.h"

namespace ans_lpf_1p
{
    // PARAMETERS

    enum DSP_PARAMETER_INDEX {
        PARAM_CUTOFF = 0,
        PARAM_COUNT
    };

    static FMOD_DSP_PARAMETER_DESC param_cutoff;
    inline FMOD_DSP_PARAMETER_DESC* param_description[PARAM_COUNT] {
        &param_cutoff,
    };

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_release(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_reset(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int buffer_size, const FMOD_DSP_BUFFER_ARRAY* in_buffer_array, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL inputs_idle, FMOD_DSP_PROCESS_OPERATION operation);
    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value);
    FMOD_RESULT F_CALL dsp_get_param_float(FMOD_DSP_STATE* dsp_state, int index, float* value, char* value_str);
    FMOD_RESULT F_CALL dsp_sys_register(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_deregister(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_mix(FMOD_DSP_STATE* dsp_state, int stage);
}

#endif
