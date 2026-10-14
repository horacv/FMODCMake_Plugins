# Refer to https://www.fmod.com/docs/2.03/api/studio-api-getting-started.html
# and https://www.fmod.com/docs/2.03/api/platforms.html

###
## 1 - Define the FMOD Core imported library
###

project(fmod_core)

set(FMOD_CORE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core/inc")
set(FMOD_CORE_LIB_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/core/lib")

add_library(fmod_core SHARED IMPORTED GLOBAL)

###
## 2 - Set the library locations
##     Debug uses the logging library (fmodL), every other config uses fmod
###

# Add more target platforms here with an elseif ()
if (WIN32 AND MSVC)
    set_target_properties(fmod_core PROPERTIES
            IMPORTED_LOCATION       "${FMOD_CORE_LIB_PATH}/win_x64/fmod.dll"
            IMPORTED_LOCATION_DEBUG "${FMOD_CORE_LIB_PATH}/win_x64/fmodL.dll"
            IMPORTED_IMPLIB         "${FMOD_CORE_LIB_PATH}/win_x64/fmod_vc.lib"
            IMPORTED_IMPLIB_DEBUG   "${FMOD_CORE_LIB_PATH}/win_x64/fmodL_vc.lib"
    )
elseif (APPLE AND NOT IOS)
    set_target_properties(fmod_core PROPERTIES
            IMPORTED_LOCATION       "${FMOD_CORE_LIB_PATH}/mac/libfmod.dylib"
            IMPORTED_LOCATION_DEBUG "${FMOD_CORE_LIB_PATH}/mac/libfmodL.dylib"
    )
elseif (LINUX)
    set_target_properties(fmod_core PROPERTIES
            IMPORTED_LOCATION       "${FMOD_CORE_LIB_PATH}/linux_x64/libfmod.so"
            IMPORTED_LOCATION_DEBUG "${FMOD_CORE_LIB_PATH}/linux_x64/libfmodL.so"
    )
else ()
    message(FATAL_ERROR "fmod_core: undefined platform")
endif ()

###
## 3 - Prepare FMOD for linking with the plugins
###

set_target_properties(fmod_core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FMOD_CORE_INCLUDE_PATH}")