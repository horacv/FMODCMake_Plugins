### Tools Directory

This directory contains Python scripts to automate the downloading and installation of required libraries (FMOD) for the project.

#### Prerequisites

- **Python 3.x**
- **Requests library**: Install via `pip install requests`
- **7-Zip (Windows only)**: Required for FMOD API extraction. Ensure `7z` is in your PATH or installed at the default location.

---

### FMOD Studio API Scripts

#### `setup_fmod_api.py` (Recommended)
Downloads and installs the FMOD Studio API in one step.

```bash
python setup_fmod_api.py <username> <password> <platform> <api_version> [--delete-installer]
```
- **username/password**: Your FMOD account credentials.
- **platform**: `windows`, `linux`, or `mac`.
- **api_version**: e.g., `20400` (representing version 2.04.00).
- **--delete-installer**: (Optional) Deletes the downloaded installer after successful extraction.

**Example:**
```bash
python setup_fmod_api.py user@example.com password123 windows 20400 --delete-installer
```

#### `get_fmod_api.py`
Downloads the FMOD Studio API installer only.
```bash
python get_fmod_api.py <username> <password> <platform> <api_version>
```

#### `install_fmod_api.py`
Extracts the **Core** API files (`api/core/inc` and `api/core/lib`) from a local installer and copies them into the project's `libs/fmod/core` directory. The platform is detected from the installer file name.
```bash
python install_fmod_api.py <installer_file> [--delete-installer]
```

> The downloaded package is the "FMOD Studio API" installer, but only the **Core** API is installed. Neither CMake nor the plugins use the Studio API.