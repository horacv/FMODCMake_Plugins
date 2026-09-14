### Gain Plugin

This directory contains an FMOD DSP plugin that applies a gain multiplier to the input signal.

#### File Descriptions

- **`ans_gain.cpp` / `ans_gain.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_gain_state.cpp` / `ans_gain_state.hpp`**: Manages the plugin instance state and performs the signal processing (gain application).
- **`ans_gain_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_gain.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
