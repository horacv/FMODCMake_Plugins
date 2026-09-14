"""
FMOD Core API Installer for CI/CD
Extracts FMOD installer, copies Core API files to the project, and cleans up.

Only the Core API is installed; FMOD Studio API files are not needed by this
project and are skipped entirely.
"""

import sys
import subprocess
import shutil
import tempfile
from pathlib import Path

# Project directories
PROJECT_ROOT = Path(__file__).parent.parent
FMOD_LIB_DIR = PROJECT_ROOT / "libs" / "fmod"

# Platform-specific configuration
PLATFORM_CONFIG = {
    'windows': {
        'lib_subdir': 'win_x64',
        'api_structure': {
            'core_inc_dir': 'api/core/inc',
            'core_lib_dir': 'api/core/lib/x64'
        }
    },
    'linux': {
        'lib_subdir': 'linux_x64',
        'api_structure': {
            'core_inc_dir': 'api/core/inc',
            'core_lib_dir': 'api/core/lib/x86_64'
        }
    },
    'mac': {
        'lib_subdir': 'mac',
        'api_structure': {
            'core_inc_dir': 'api/core/inc',
            'core_lib_dir': 'api/core/lib'
        }
    }
}


def detect_platform(installer_path):
    """Detect platform from installer filename."""
    name = installer_path.name.lower()
    if 'win' in name and name.endswith('.exe'):
        return 'windows'
    elif 'linux' in name and name.endswith('.tar.gz'):
        return 'linux'
    elif 'mac' in name and name.endswith('.dmg'):
        return 'mac'
    else:
        raise ValueError(f"Cannot detect platform from installer: {installer_path.name}")


def extract_windows_exe(installer_path, temp_dir):
    """Extract Windows .exe installer using available methods."""
    print("Extracting Windows installer...")

    # Try 7zip first (most common in CI environments)
    seven_zip_paths = [
        '7z',
        r'C:\Program Files\7-Zip\7z.exe',
        r'C:\Program Files (x86)\7-Zip\7z.exe',
    ]

    for seven_zip in seven_zip_paths:
        try:
            subprocess.run(
                [seven_zip, 'x', str(installer_path), f'-o{temp_dir}', '-y'],
                check=True,
                capture_output=True,
                text=True
            )
            print("✓ Extracted with 7zip")
            return
        except (FileNotFoundError, subprocess.CalledProcessError) as e:
            continue

    # Error message with helpful information
    raise RuntimeError(
        "Cannot extract Windows installer. Please install 7-Zip:\n"
        "  1. Download from: https://www.7-zip.org/download.html\n"
        "  2. Install to default location\n"
        "  3. Re-run this script\n\n"
        "For CI/CD, install 7-Zip in your pipeline:\n"
        "  winget install -e --id 7zip.7zip\n"
        "  choco install 7zip  (Chocolatey)\n"
        "  or use GitHub Actions: actions/setup-7zip"
    )


def extract_linux_tar(installer_path, temp_dir):
    """Extract Linux .tar.gz archive."""
    print("Extracting Linux tar...")
    shutil.unpack_archive(str(installer_path), temp_dir)
    print("✓ Extracted tar")


def extract_macos_dmg(installer_path, temp_dir):
    """Extract macOS .dmg installer."""
    print("Extracting macOS DMG...")

    # Mount the DMG
    mount_point = temp_dir / "dmg_mount"
    mount_point.mkdir(exist_ok=True)

    subprocess.run(
        ['hdiutil', 'attach', str(installer_path), '-mountpoint', str(mount_point), '-nobrowse'],
        check=True
    )

    try:
        # Copy contents
        for item in mount_point.iterdir():
            if item.is_dir():
                shutil.copytree(item, temp_dir / item.name, dirs_exist_ok=True)
            else:
                shutil.copy2(item, temp_dir / item.name)
    finally:
        # Unmount
        subprocess.run(['hdiutil', 'detach', str(mount_point)], check=False)

    print("✓ Extracted DMG")


def copy_api_files(temp_dir, platform):
    """Copy FMOD Core API files from temp directory to project."""
    print("\nCopying FMOD Core API files to project...")

    config = PLATFORM_CONFIG[platform]
    api_structure = config['api_structure']

    # Find the actual API directory in temp (may be nested)
    api_dirs = list(temp_dir.rglob('api'))
    if not api_dirs:
        # Try alternate structure
        api_dirs = [temp_dir]

    source_root = api_dirs[0].parent if api_dirs else temp_dir

    # Copy core headers
    src = source_root / api_structure['core_inc_dir']
    dst = FMOD_LIB_DIR / "core" / "inc"
    if src.exists():
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        print(f"✓ Copied core headers ({len(list(dst.glob('*')))} files)")
    else:
        print(f"⚠ Warning: Core headers not found at {src}")

    # Copy core libraries
    src = source_root / api_structure['core_lib_dir']
    dst = FMOD_LIB_DIR / "core" / "lib" / config['lib_subdir']
    if src.exists():
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        print(f"✓ Copied core libraries ({len(list(dst.glob('*')))} files)")
    else:
        print(f"⚠ Warning: Core libraries not found at {src}")


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python install_fmod_api.py <installer_file> [--delete-installer]")
        print("Example: python install_fmod_api.py fmodstudioapi20312win-installer.exe")
        print("Example: python install_fmod_api.py fmodstudioapi20312win-installer.exe --delete-installer")
        print("\nOptions:")
        print("  --delete-installer    Delete the installer file after successful installation")
        sys.exit(1)

    installer_path = Path(sys.argv[1])
    delete_installer = len(sys.argv) == 3 and sys.argv[2] == '--delete-installer'

    # Validate installer exists
    if not installer_path.is_file():
        print(f"Error: Installer not found: {installer_path}")
        sys.exit(1)

    print("\n" + "=" * 60)
    print(f"FMOD Core API Installer")
    print("\n" + "=" * 60)

    # Detect platform
    try:
        platform = detect_platform(installer_path)
        print(f"Installer found: {installer_path.name}")
        print(f"Platform: {platform}")
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

    # Create a temporary directory
    temp_dir = Path(tempfile.mkdtemp(prefix="temp_fmod_install_"))
    print(f"Temp directory: {temp_dir}")

    try:
        # Extract installer
        if platform == 'windows':
            extract_windows_exe(installer_path, temp_dir)
        elif platform == 'linux':
            extract_linux_tar(installer_path, temp_dir)
        elif platform == 'mac':
            extract_macos_dmg(installer_path, temp_dir)

        # Copy API files to the project
        copy_api_files(temp_dir, platform)

        print("\n✓ Installation complete!")
        print(f"FMOD Core API files copied to: {FMOD_LIB_DIR}")

        # Delete installer if requested
        if delete_installer:
            print("\n[--delete-installer] flag passed. Deleting installer...")
            installer_path.unlink()
            print(f"✓ Deleted {installer_path.name}")

    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)
    finally:
        # Cleanup temporary directory
        print("\nCleaning up...")
        shutil.rmtree(temp_dir, ignore_errors=False)
        print("✓ Temporary files removed")


if __name__ == "__main__":
    main()
