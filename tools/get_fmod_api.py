"""

FMOD Studio API Downloader
Downloads the FMOD Studio API installer for the specified platform.
"""

import requests
import sys

# Constants
FMOD_LOGIN_URL = "https://www.fmod.com/api-login"
FMOD_DOWNLOAD_BASE = "https://www.fmod.com/api-get-download-link"

# Platform configuration
PLATFORM_CONFIG = {
    'mac': {
        'filename': 'fmodstudioapi{version}mac-installer.dmg',
        'path': 'files/fmodstudio/api/Mac/',
    },
    'linux': {
        'filename': 'fmodstudioapi{version}linux.tar.gz',
        'path': 'files/fmodstudio/api/Linux/',
    },
    'windows': {
        'filename': 'fmodstudioapi{version}win-installer.exe',
        'path': 'files/fmodstudio/api/Windows/',
    }
}

def download_with_progress(url, filepath, chunk_size=65536):
    """Download file with progress indication."""
    response = requests.get(url, stream=True, allow_redirects=True)
    response.raise_for_status()

    total_size = int(response.headers.get('content-length', 0))
    downloaded = 0

    with open(filepath, 'wb') as f:
        for chunk in response.iter_content(chunk_size=chunk_size):
            if chunk:
                f.write(chunk)
                downloaded += len(chunk)
                if total_size:
                    progress = (downloaded / total_size) * 100
                    print(f"\rDownloading: {progress:.1f}%", end='', flush=True)

    print()  # New line after progress
    return downloaded


def main():
    # Validate arguments
    if len(sys.argv) != 5:
        print("Usage: python get_fmod_api.py <username> <password> <platform> <api_version>")
        print("Platforms: mac, linux, windows")
        print("Example: python get_fmod_api.py user@example.com password123 windows 20314")
        sys.exit(1)

    user = sys.argv[1]
    password = sys.argv[2]
    platform = sys.argv[3]
    api_version = sys.argv[4]

    # Validate platform
    if platform not in PLATFORM_CONFIG:
        print(f"Error: Unsupported platform '{platform}'")
        print(f"Supported platforms: {', '.join(PLATFORM_CONFIG.keys())}")
        sys.exit(1)

    # Get platform configuration
    config = PLATFORM_CONFIG[platform]
    filename = config['filename'].format(version=api_version)

    # Build download URL
    download_url = (
        f"{FMOD_DOWNLOAD_BASE}?"
        f"path={config['path']}&"
        f"filename={filename}&"
        f"user={user}"
    )

    try:
        # Step 1: Login and get token
        print("Authenticating with FMOD API...")
        login_response = requests.post(FMOD_LOGIN_URL, auth=(user, password), timeout=30)
        login_response.raise_for_status()

        token = login_response.json().get("token")
        if not token:
            print("Error: Failed to retrieve authentication token")
            sys.exit(1)

        print("✓ Authentication successful")

        # Step 2: Request the download link
        print("Requesting download link...")
        link_response = requests.get(
            download_url,
            headers={"Authorization": f"Bearer {token}"},
            timeout=30
        )
        link_response.raise_for_status()

        url = link_response.json().get("url")
        if not url:
            print("Error: Failed to retrieve download URL")
            sys.exit(1)

        print("✓ Download link obtained")

        # Step 3: Download the file
        print(f"Downloading {filename}...")
        file_size = download_with_progress(url, filename)

        print(f"✓ Download complete: {filename} ({file_size:,} bytes)")

    except requests.exceptions.HTTPError as e:
        print(f"HTTP Error: {e}")
        print(f"Response: {e.response.text if e.response else 'No response'}")
        sys.exit(1)
    except requests.exceptions.RequestException as e:
        print(f"Network Error: {e}")
        sys.exit(1)
    except KeyError as e:
        print(f"Error parsing API response: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()