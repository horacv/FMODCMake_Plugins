### LPF 2P Plugin
 
This directory contains the reference implementation of an FMOD DSP plugin that implements a 2-pole low-pass filter.
 
#### File Descriptions
 
- **`ans_lpf_2p.cpp` / `ans_lpf_2p.hpp`**: Defines the `FMOD_DSP_DESCRIPTION` and the necessary interface functions to register the plugin with FMOD.
- **`ans_lpf_2p_state.cpp` / `ans_lpf_2p_state.hpp`**: Manages the plugin instance state and performs the signal processing.
- **`ans_lpf_2p_types.hpp`**: Defines the plugin parameter types and structures.
- **`ans_lpf_2p.plugin.js`**: FMOD Studio plugin definition file, used for UI configuration and parameter mapping.
- **`resources/`**: Contains assets for the plugin UI, such as `ans_logo.png` or other custom data.
- **`CMakeLists.txt`**: Build configuration for the plugin as a shared library.
