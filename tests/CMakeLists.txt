add_executable(tests
        libs/doctest.h
        tests_ans_dsp_common.cpp
        tests_main.cpp)

target_link_libraries(tests ans_dsp)