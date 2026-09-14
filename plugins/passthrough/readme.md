### Passthrough Plugin

This directory contains the reference implementation of an FMOD DSP plugin that passes the input signal through to the output without modification.\
This plugin serves as the boilerplate for creating custom FMOD DSP plugins.

#### File Descriptions

- **`ans_passthrough.cpp` / `ans_passthrough.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_passthrough_state.cpp` / `ans_passthrough_state.hpp`**: Manages the plugin instance state and performs the signal processing.
- **`ans_passthrough_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_passthrough.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
