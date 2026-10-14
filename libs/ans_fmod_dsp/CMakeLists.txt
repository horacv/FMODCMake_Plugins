project(ans_fmod_dsp)

set(ANS_FMOD_DSP_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
add_library(${PROJECT_NAME} STATIC
        ans_fmod_dsp.hpp
        ans_fmod_dsp.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC "${ANS_FMOD_DSP_INCLUDE_PATH}")
target_link_libraries(${PROJECT_NAME} PUBLIC fmod_core)
