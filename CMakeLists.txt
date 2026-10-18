cmake_minimum_required(VERSION 3.30)
project(fmod-plugins LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(libs)
add_subdirectory(plugins)
add_subdirectory(tests)