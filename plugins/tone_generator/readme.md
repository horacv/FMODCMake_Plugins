### Tone Generator Plugin

This directory contains an FMOD DSP plugin that generates various test tones (Sine, Saw, Triangle, Square) and white noise.

#### File Descriptions

- **`ans_tone_generator.cpp` / `ans_tone_generator.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_tone_generator_state.cpp` / `ans_tone_generator_state.hpp`**: Manages the plugin instance state and performs the signal generation.
- **`ans_tone_generator_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_tone_generator.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
