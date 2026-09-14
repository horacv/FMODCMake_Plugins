# Refer to  https://www.fmod.com/docs/2.03/api/studio-api-getting-started.html
# and https://www.fmod.com/docs/2.03/api/platforms.html

###
## 1 - Define and add the FMOD Core libraries
###

project(fmod_core)

###
## 2 - Set the include paths to find the header files
###

set(FMOD_CORE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core/inc")
add_library(fmod_core SHARED IMPORTED GLOBAL "${FMOD_CORE_INCLUDE_PATH}/fmod.hpp")

###
## 3 - Set the Logging libraries for the Debug config
###

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FMOD_LIB_NAME "fmodL")
    set(FMOD_STUDIO_LIB_NAME "fmodstudioL")
else()
    set(FMOD_LIB_NAME "fmod")
    set(FMOD_STUDIO_LIB_NAME "fmodstudio")
endif()

###
## 4 - Set the location of the dynamic libraries
###

# Define the path of the dynamic libraries for each platform
# Add more target platforms here with an elseif ()
if (WIN32 AND MSVC)
    set_property(TARGET fmod_core PROPERTY IMPORTED_LOCATION
            "${CMAKE_CURRENT_SOURCE_DIR}/core/lib/win_x64/${FMOD_LIB_NAME}.dll")
    set_property(TARGET fmod_core PROPERTY IMPORTED_IMPLIB
            "${CMAKE_CURRENT_SOURCE_DIR}/core/lib/win_x64/${FMOD_LIB_NAME}_vc.lib")
elseif (APPLE AND NOT IOS)
    set_property(TARGET fmod_core PROPERTY IMPORTED_LOCATION
            "${CMAKE_CURRENT_SOURCE_DIR}/core/lib/mac/lib${FMOD_LIB_NAME}.dylib")
elseif (LINUX)
    set_property(TARGET fmod_core PROPERTY IMPORTED_LOCATION
            "${CMAKE_CURRENT_SOURCE_DIR}/core/lib/linux_x64/lib${FMOD_LIB_NAME}.so")
endif ()

###
## 5 - Prepare FMOD for linking with the plugins
###

set_property(TARGET fmod_core PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${FMOD_CORE_INCLUDE_PATH}")