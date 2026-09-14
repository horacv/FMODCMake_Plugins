# FMOD Core API

This directory contains the FMOD Core API files (low-level audio engine).

## Setup Instructions

1. Copy header files from the FMOD installation to the `inc/` folder
2. Copy platform-specific binaries to the appropriate subdirectory in `lib/`

## Directory Structure

```
core/
├── inc/                    # FMOD Core header files
│   ├── fmod.h
│   ├── fmod.hpp
│   ├── fmod_codec.h
│   ├── fmod_dsp.h
│   ├── fmod_errors.h
│   └── fmod_output.h
└── lib/                    # Platform-specific binaries
    ├── apple/                  
    ├── linux_x64/             
    ├── win_x64/ 
    └── ...                 # Other platforms as needed
```

## Location

Files are located in the FMOD installation under `api/core/`:
- **Windows**: `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core`
- **macOS**: Inside the FMOD Studio API installer `.dmg` file
- **Linux**: Inside the FMOD Studio API `.tar` file
