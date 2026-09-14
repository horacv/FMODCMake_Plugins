project(ans_lpf_2p DESCRIPTION "2-pole Low-Pass Filter")

add_library(ans_lpf_2p SHARED
        ans_lpf_2p.cpp
        ans_lpf_2p.hpp
        ans_lpf_2p_state.cpp
        ans_lpf_2p_state.hpp
        ans_lpf_2p_types.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ans_dsp)
target_link_libraries(${PROJECT_NAME} PRIVATE ans_fmod_dsp)
target_link_libraries(${PROJECT_NAME} PRIVATE fmod_core)

# Copy plugin description *.plugin.js file to build folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.plugin.js"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.plugin.js")

# Copy all resource files to build folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")