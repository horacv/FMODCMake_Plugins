add_executable(tests
        libs/doctest.h
        tests_ans_dsp_common.cpp
        tests_main.cpp)

target_link_libraries(tests ans_dsp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs")
include(doctest)

doctest_discover_tests(tests)