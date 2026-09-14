### Delay Plugin

This directory contains an FMOD DSP plugin that applies a delay to the input signal.

#### File Descriptions

- **`ans_delay.cpp` / `ans_delay.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_delay_state.cpp` / `ans_delay_state.hpp`**: Manages the plugin instance state and performs the signal processing.
- **`ans_delay_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_delay.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
