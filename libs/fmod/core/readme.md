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
│   ├── fmod_common.h
│   ├── fmod_dsp.h
│   ├── fmod_dsp_effects.h
│   ├── fmod_errors.h
│   └── fmod_output.h
└── lib/                    # Platform-specific binaries
    ├── linux_x64/          # libfmod.so / libfmodL.so
    ├── mac/                # libfmod.dylib / libfmodL.dylib
    ├── win_x64/            # fmod.dll + fmod_vc.lib / fmodL.dll + fmodL_vc.lib
    └── ...                 # Other platforms as needed
```

## Location

Files are located in the FMOD installation under `api/core/`:
- **Windows**: `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core`
- **macOS**: Inside the FMOD Studio API installer `.dmg` file
- **Linux**: Inside the FMOD Studio API `.tar.gz` file
