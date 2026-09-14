#ifndef ANS_PASSTHROUGH_HPP
#define ANS_PASSTHROUGH_HPP

#include "ans_passthrough_types.hpp"
#include "fmod_common.h"
#include "fmod_dsp.h"

namespace ans_passthrough
{
    // PARAMETERS

    enum DSP_PARAMETER_INDEX {
        PARAM_CUTOFF = 0,
        PARAM_INT,
        PARAM_INT_ENUM,
        PARAM_BOOL,
        PARAM_DATA,
        /**
         * Add more parameters here ...
         */
        PARAM_COUNT
    };

    static FMOD_DSP_PARAMETER_DESC param_cutoff;
    static FMOD_DSP_PARAMETER_DESC param_int;
    static FMOD_DSP_PARAMETER_DESC param_int_enum;
    static FMOD_DSP_PARAMETER_DESC param_bool;
    static FMOD_DSP_PARAMETER_DESC param_data;

    /* Add more parameters here ...*/

    inline FMOD_DSP_PARAMETER_DESC* param_description[PARAM_COUNT] {
        &param_cutoff,
        &param_int,
        &param_int_enum,
        &param_bool,
        &param_data,

        /* Add more parameters here ...*/
    };

    // USER DATA

    inline ANS_DSP_UserData userdata{"User Data", 0.0f, 1, true};

    // CALLBACKS

    FMOD_RESULT F_CALL dsp_create(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_release(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_reset(FMOD_DSP_STATE* dsp_state);

    FMOD_RESULT F_CALL dsp_read(FMOD_DSP_STATE* dsp_state, float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int* out_channels);
    FMOD_RESULT F_CALL dsp_process(FMOD_DSP_STATE* dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY* in_buffer_array, FMOD_DSP_BUFFER_ARRAY* out_buffer_array, FMOD_BOOL inputs_idle, FMOD_DSP_PROCESS_OPERATION operation);
    FMOD_RESULT F_CALL dsp_set_position(FMOD_DSP_STATE* dsp_state, unsigned int position);

    FMOD_RESULT F_CALL dsp_set_param_float(FMOD_DSP_STATE* dsp_state, int index, float value);
    FMOD_RESULT F_CALL dsp_set_param_int(FMOD_DSP_STATE* dsp_state, int index, int value);
    FMOD_RESULT F_CALL dsp_set_param_bool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL value);
    FMOD_RESULT F_CALL dsp_set_param_data(FMOD_DSP_STATE* dsp_state, int index, void* data, unsigned int length);

    FMOD_RESULT F_CALL dsp_get_param_float(FMOD_DSP_STATE* dsp_state, int index, float* value, char* value_str);
    FMOD_RESULT F_CALL dsp_get_param_int(FMOD_DSP_STATE* dsp_state, int index, int* value, char* value_str);
    FMOD_RESULT F_CALL dsp_get_param_bool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL* value, char* value_str);
    FMOD_RESULT F_CALL dsp_get_param_data(FMOD_DSP_STATE* dsp_state, int index, void** value, unsigned int* length, char *value_str);

    FMOD_RESULT F_CALL dsp_should_process(FMOD_DSP_STATE* dsp_state, FMOD_BOOL inputs_idle, unsigned int length, FMOD_CHANNELMASK in_mask, int in_channels, FMOD_SPEAKERMODE speaker_mode);

    FMOD_RESULT F_CALL dsp_sys_register(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_deregister(FMOD_DSP_STATE* dsp_state);
    FMOD_RESULT F_CALL dsp_sys_mix(FMOD_DSP_STATE* dsp_state, int stage);
}

#endif
