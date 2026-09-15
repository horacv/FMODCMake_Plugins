### Plugins Directory

This directory contains FMOD DSP plugins. Each plugin is a self-contained
subdirectory with its own `CMakeLists.txt`, built as a shared library
(`.dll`/`.so`/`.dylib`) that FMOD loads at runtime via `System::loadPlugin`.

`plugins/passthrough` is the reference implementation — copy its layout when
adding a new plugin.

#### Layout of a plugin directory

```
plugins/<name>/
├── CMakeLists.txt            # SHARED library target
├── <name>.cpp / .hpp         # FMODGetDSPDescription() entry point + DSP callback table
├── <name>_state.cpp / .hpp   # Plugin's per-instance state and signal processing
├── <name>.plugin.js          # FMOD Studio plugin description (UI, parameter bindings)
├── <name>_types.hpp          # Data and types used by the DSP state
└── resources/                # Assets referenced by the .plugin.js UI (icons, etc.)
```

#### Conventions

- **Entry point:** the plugin exports `extern "C" F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()`,
  which initializes parameter descriptors and returns the static
  `FMOD_DSP_DESCRIPTION` describing the plugin's callback table.
- **Per-instance state** is allocated via `FMOD_DSP_ALLOC`/`FMOD_DSP_FREE`
  (see `alloc_dsp_state`/`get_dsp_state` in `libs/ans_fmod_dsp`), 
  and attached to `FMOD_DSP_STATE::plugindata`.
- **Shared helpers** for parameter descriptors and DSP state plumbing live in
  `libs/ans_fmod_dsp`; general-purpose and shared DSP math helpers live in
  `libs/ans_dsp`. New plugins should link both, plus `fmod_core`, as in
  `plugins/passthrough/CMakeLists.txt`.
- **CMake target:** link `ans_dsp`, `ans_fmod_dsp`, and `fmod_core`, then copy
  the plugin's `<name>.plugin.js` and `resources/` next to the built binary
  as a post-build step (see `plugins/passthrough/CMakeLists.txt` for the
  `add_custom_command(... POST_BUILD ...)` pattern).
- **Register the new plugin** by adding `add_subdirectory(<name>)` to
  `plugins/CMakeLists.txt`.

For Python scripts that download/install the FMOD API itself, see
`tools/readme.md`.
