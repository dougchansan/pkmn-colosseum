#!/usr/bin/env python3

"""
Toolchain Setup Script for Pokémon Colosseum Decompilation

Downloads and configures the required tools:
  1. decomp-toolkit (dtk) — DOL analysis and splitting
  2. objdiff — Assembly comparison tool
  3. Verifies MetroWerks compiler availability

Prerequisites:
  - Python 3.10+
  - Rust/Cargo (for building dtk from source) OR download pre-built releases
  - Dolphin Emulator (for RVZ → ISO conversion)
"""

import hashlib
import os
import platform
import shutil
import subprocess
import sys
import urllib.request
from pathlib import Path

TOOLS_DIR = Path(__file__).parent
PROJECT_ROOT = TOOLS_DIR.parent

# Release URLs (update these as new versions are released)
DTK_RELEASES = "https://github.com/encounter/decomp-toolkit/releases/latest"
OBJDIFF_RELEASES = "https://github.com/encounter/objdiff/releases/latest"


def is_windows():
    return platform.system() == "Windows"


def exe(name):
    return f"{name}.exe" if is_windows() else name


def check_command(cmd):
    """Check if a command is available on PATH."""
    return shutil.which(cmd) is not None


def print_status(msg, ok=None):
    if ok is True:
        print(f"  [OK] {msg}")
    elif ok is False:
        print(f"  [!!] {msg}")
    else:
        print(f"  ... {msg}")


def check_prerequisites():
    """Check for required system tools."""
    print("Checking prerequisites...")
    all_ok = True

    # Python
    py_ver = sys.version_info
    print_status(f"Python {py_ver.major}.{py_ver.minor}.{py_ver.micro}", py_ver >= (3, 10))
    if py_ver < (3, 10):
        all_ok = False

    # Git
    has_git = check_command("git")
    print_status("Git", has_git)
    if not has_git:
        all_ok = False

    # Rust (optional if downloading pre-built binaries)
    has_rust = check_command("cargo")
    print_status(f"Rust/Cargo {'(available)' if has_rust else '(not found — needed to build dtk from source)'}", has_rust)

    # Ninja
    has_ninja = check_command("ninja")
    print_status(f"Ninja {'(available)' if has_ninja else '(not found — install via pip: pip install ninja)'}", has_ninja)

    return all_ok


def install_dtk_cargo():
    """Install decomp-toolkit via cargo."""
    print("\nInstalling decomp-toolkit via cargo...")
    try:
        subprocess.run(
            ["cargo", "install", "decomp-toolkit"],
            check=True,
        )
        # Copy to tools directory
        dtk_path = shutil.which(exe("dtk"))
        if dtk_path:
            dest = TOOLS_DIR / exe("dtk")
            shutil.copy2(dtk_path, dest)
            print_status(f"dtk installed to {dest}", True)
            return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass
    return False


def download_dtk_release():
    """Download pre-built dtk binary from GitHub releases."""
    print("\nTo download decomp-toolkit:")
    print(f"  1. Go to {DTK_RELEASES}")
    if is_windows():
        print("  2. Download dtk-x86_64-pc-windows-msvc.zip")
    else:
        print("  2. Download dtk-x86_64-unknown-linux-musl.zip")
    print(f"  3. Extract {exe('dtk')} to {TOOLS_DIR}/")


def setup_mwcc_instructions():
    """Print instructions for obtaining the MetroWerks compiler."""
    mwcc_dir = TOOLS_DIR / "mwcc_compiler"
    mwcc_path = mwcc_dir / "mwcceppc.exe"

    print("\nMetroWerks CodeWarrior Compiler:")
    if mwcc_path.exists():
        print_status("mwcceppc.exe found", True)
        return True

    print_status("mwcceppc.exe not found", False)
    print("  The MetroWerks compiler is required for matching decompilation.")
    print("  It must be obtained from a licensed copy of CodeWarrior for Embedded PowerPC.")
    print(f"  Place mwcceppc.exe in: {mwcc_dir}/")
    print("  On Linux, you'll also need 'wibo' to run the Windows binary.")
    mwcc_dir.mkdir(parents=True, exist_ok=True)
    return False


def setup_dolphin_instructions():
    """Print instructions for Dolphin (needed for RVZ conversion)."""
    print("\nDolphin Emulator (for RVZ → ISO conversion):")
    has_dolphin = check_command("DolphinTool") or check_command("dolphin-tool")
    if has_dolphin:
        print_status("DolphinTool found on PATH", True)
        return True

    print_status("DolphinTool not found on PATH", False)
    print("  DolphinTool is needed to convert the .rvz disc image to .iso format.")
    print("  Options:")
    print("    1. Install Dolphin Emulator and add it to PATH")
    print("    2. Use Dolphin GUI: Tools → Convert File")
    print("  Download: https://dolphin-emu.org/download/")
    return False


def verify_disc_image():
    """Check if the disc image or extracted DOL exists."""
    orig_dir = PROJECT_ROOT / "orig" / "GC6E01"
    dol_path = orig_dir / "start.dol"

    print("\nDisc Image / DOL Status:")
    if dol_path.exists():
        # Compute SHA-1
        sha1 = hashlib.sha1(dol_path.read_bytes()).hexdigest()
        print_status(f"start.dol found (SHA-1: {sha1})", True)
        return True

    # Check for ISO in common locations
    search_dirs = [
        orig_dir,
        PROJECT_ROOT / "rom",
    ]
    for d in search_dirs:
        if not d.exists():
            continue
        for ext in ("*.iso", "*.gcm", "*.rvz", "*.ciso"):
            for f in d.glob(ext):
                print_status(f"Found disc image: {f}", True)
                print("  Extract start.dol using: dtk dol extract <iso_path> orig/GC6E01/")
                return False

    print_status("No disc image or DOL found", False)
    print(f"  Place your Pokémon Colosseum disc image in: {orig_dir}/")
    print("  Supported formats: .iso, .gcm, .rvz (RVZ needs conversion first)")
    return False


def main():
    print("=" * 60)
    print("Pokémon Colosseum Decompilation — Toolchain Setup")
    print("=" * 60)

    check_prerequisites()

    # DTK
    dtk_path = TOOLS_DIR / exe("dtk")
    print(f"\ndecomp-toolkit (dtk):")
    if dtk_path.exists():
        print_status("dtk found in tools/", True)
    elif check_command("dtk"):
        print_status("dtk found on PATH", True)
    else:
        print_status("dtk not found", False)
        if check_command("cargo"):
            resp = input("  Install via cargo? [y/N] ").strip().lower()
            if resp == "y":
                install_dtk_cargo()
            else:
                download_dtk_release()
        else:
            download_dtk_release()

    setup_mwcc_instructions()
    setup_dolphin_instructions()
    verify_disc_image()

    # Ninja
    print("\nNinja build system:")
    if check_command("ninja"):
        print_status("ninja found", True)
    else:
        print_status("ninja not found — install with: pip install ninja", False)

    print("\n" + "=" * 60)
    print("Setup check complete. See docs/setup.md for full instructions.")
    print("=" * 60)


if __name__ == "__main__":
    main()
