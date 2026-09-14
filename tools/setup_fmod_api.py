"""
FMOD Core API Complete Setup
Downloads, installs, and configures the FMOD Core API in one command.

FMOD is only distributed as a combined Studio+Core installer, so the
installer downloaded in step 1 still contains the Studio API — but
install_fmod_api.py only extracts and copies the Core API subset this
project needs.
"""

import sys
import subprocess
from pathlib import Path

GET_FMOD_API_SCRIPT = 'get_fmod_api.py'
INSTALL_FMOD_API_SCRIPT = 'install_fmod_api.py'

def run_command(script_name, args):
    """Run a Python script with arguments and capture output."""
    python_exe = sys.executable
    script_path = Path(__file__).parent / script_name

    cmd = [python_exe, str(script_path)] + args

    result = subprocess.run(
        cmd,
        capture_output=False,  # Show output in real-time
        text=True
    )

    if result.returncode != 0:
        raise RuntimeError(f"{script_name} failed with exit code {result.returncode}")


def main():
    # Parse arguments
    if len(sys.argv) < 5:
        print("FMOD Core API Complete Setup")
        print("=" * 50)
        print("\nUsage: python setup_fmod_api.py <username> <password> <platform> <api_version> [--delete-installer]")
        print("\nPlatforms: mac, linux, windows")
        print("\nOptions:")
        print("  --delete-installer    Delete the installer file after installation (default: keep)")
        print("\nExample:")
        print("  python setup_fmod_api.py user@example.com password123 windows 20314")
        print("  python setup_fmod_api.py user@example.com password123 windows 20314 --delete-installer")
        print("\nThis script will:")
        print("  1. Download the FMOD Studio API installer (the only distribution FMOD offers)")
        print("  2. Extract and copy only the Core API files to libs/fmod/")
        print("  3. Clean up temporary files")
        sys.exit(1)

    user = sys.argv[1]
    password = sys.argv[2]
    platform = sys.argv[3]
    api_version = sys.argv[4]
    delete_installer = len(sys.argv) > 5 and sys.argv[5] == '--delete-installer'

    print("=" * 60)
    print("FMOD Core API Complete Setup")
    print("=" * 60)
    print(f"Platform: {platform}")
    print(f"Version: {api_version}")
    print(f"Delete installer: {'Yes' if delete_installer else 'No'}")
    print("=" * 60)

    try:
        # Step 1: Download the installer
        print("\n[Step 1/2] Downloading FMOD Studio API installer...")
        print("-" * 60)
        run_command(GET_FMOD_API_SCRIPT, [user, password, platform, api_version])

        # Step 2: Install and extract Core API files
        print("\n[Step 2/2] Installing FMOD Core API files...")
        print("-" * 60)

        # Determine installer filename
        installer_patterns = {
            'windows': f'fmodstudioapi{api_version}win-installer.exe',
            'linux': f'fmodstudioapi{api_version}linux.tar.gz',
            'mac': f'fmodstudioapi{api_version}mac-installer.dmg'
        }

        installer_name = installer_patterns.get(platform)
        if not installer_name:
            raise ValueError(f"Unknown platform: {platform}")

        # Run install script with --delete-installer flag if requested
        install_args = [installer_name]
        if delete_installer:
            install_args.append('--delete-installer')

        run_command(INSTALL_FMOD_API_SCRIPT, install_args)

        # Success message
        print("\n" + "=" * 60)
        print("✓ FMOD Core API Setup Complete!")
        print("=" * 60)
        print(f"Core API files installed to: libs/fmod/")
        if delete_installer:
            print(f"Installer cleaned up: {installer_name}")
        else:
            print(f"Installer preserved: {installer_name}")
        print("\nYou can now build your project with FMOD")

    except Exception as e:
        print("\n" + "=" * 60)
        print("✗ Setup Failed!")
        print("=" * 60)
        print(f"Error: {e}")
        print("\nTroubleshooting:")
        print("  - Check your FMOD credentials")
        print("  - Ensure you have internet connectivity")
        print("  - For Windows: Install 7-Zip (https://www.7-zip.org)")
        print("  - Check that the platform and version are correct")
        sys.exit(1)


if __name__ == "__main__":
    main()
