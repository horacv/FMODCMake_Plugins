project(ans_dsp)

set(ANS_DSP_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(ans_dsp INTERFACE
        ans_dsp_common.hpp
        ans_dsp_filters_one_pole.hpp
        ans_dsp_filters_biquad.hpp
        ans_dsp_smoothing.hpp)

target_include_directories(ans_dsp INTERFACE "${ANS_DSP_INCLUDE_PATH}")

