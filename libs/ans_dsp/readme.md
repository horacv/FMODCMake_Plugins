### ANS DSP Library

Common DSP helper functions. This is a header-only CMake `INTERFACE` library (`ans_dsp`) with only C++ STD library dependencies.

| Header | Contents |
|---|---|
| `ans_dsp_common.hpp` | dB/linear conversions, sample/ms conversions, unipolar/bipolar mapping, angular frequency, denormal flushing |
| `ans_dsp_filters_biquad.hpp` | `BiquadCoefficients` + RBJ cookbook coefficients (LPF, HPF, low shelf, high shelf, peaking EQ) |
| `ans_dsp_filters_one_pole.hpp` | One-pole coefficient formulas |
| `ans_dsp_noise.hpp` | xorshift32 PRNG for white noise |
| `ans_dsp_oscillator.hpp` | Phase accumulator, naive and PolyBLEP-corrected oscillators |
| `ans_dsp_smoothing.hpp` | Parameter smoothing helpers |