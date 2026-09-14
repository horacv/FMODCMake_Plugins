### EQ 4B Plugin

This directory contains the reference implementation of an FMOD DSP plugin that implements a 4-band equalizer.

#### File Descriptions

- **`ans_eq_4b.cpp` / `ans_eq_4b.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_eq_4b_state.cpp` / `ans_eq_4b_state.hpp`**: Manages the plugin instance state and performs the signal processing.
- **`ans_eq_4b_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_eq_4b.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
