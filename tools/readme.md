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
- **api_version**: e.g., `20312` (representing version 2.03.12).
- **--delete-installer**: (Optional) Deletes the downloaded installer after successful extraction.

**Example:**
```bash
python setup_fmod_api.py user@example.com password123 windows 20312 --delete-installer
```

#### `get_fmod_api.py`
Downloads the FMOD Studio API installer only.
```bash
python get_fmod_api.py <username> <password> <platform> <api_version>
```

#### `install_fmod_api.py`
Extracts and copies FMOD API files from a local installer to the project's `libs/fmod` directory.
```bash
python install_fmod_api.py <installer_file> [--delete-installer]
```
