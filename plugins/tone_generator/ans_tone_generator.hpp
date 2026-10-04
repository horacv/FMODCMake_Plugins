#ifndef ANS_TONE_GENERATOR_HPP
#define ANS_TONE_GENERATOR_HPP

#include "fmod_common.h"
#include "fmod_dsp.h"

namespace ans_tone_generator
{
    // PARAMETERS

    enum DSP_PARAMETER_INDEX {
        PARAM_FREQ = 0,
        PARAM_GAIN,
        PARAM_OSC_TYPE,

        PARAM_COUNT
    };

    static FMOD_DSP_PARAMETER_DESC param_freq;
    static FMOD_DSP_PARAMETER_DESC param_gain;
    static FMOD_DSP_PARAMETER_DESC param_osc_type;
    inline FMOD_DSP_PARAMETER_DESC* param_description[PARAM_COUNT] {
        &param_freq,
        &param_gain,
        &param_osc_type,
    };

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_release(FMOD_DSP_STATE* dsp_state);

    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY*, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL, FMOD_DSP_PROCESS_OPERATION operation);

    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value);
    FMOD_RESULT F_CALL dsp_set_param_int(FMOD_DSP_STATE* dsp_state, int index, int value);

    FMOD_RESULT F_CALL dsp_get_param_float(FMOD_DSP_STATE* dsp_state, int index, float* value, char* value_str);
    FMOD_RESULT F_CALL dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str);
}

#endif
