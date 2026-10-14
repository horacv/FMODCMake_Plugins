### ANS FMOD DSP Library

Common FMOD DSP helper functions and utilities, built as a `STATIC` library (`ans_fmod_dsp`). It provides:

- `create_dsp_state<T>` / `destroy_dsp_state<T>` / `get_dsp_state<T>`: per-instance state stored in `FMOD_DSP_STATE::plugindata`.
- `init_param_desc_float`, `init_param_desc_float_with_mapping`, `init_param_desc_int`, `init_param_desc_int_enumerated`, `init_param_desc_bool`, `init_param_desc_data`: parameter descriptor initialization.
- `write_*_value_string`: formatting of parameter value strings.
