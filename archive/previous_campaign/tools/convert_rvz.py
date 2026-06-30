#!/usr/bin/env python3

"""
Convert an RVZ disc image to ISO format using DolphinTool.

Usage:
    python tools/convert_rvz.py "path/to/Pokemon Colosseum (USA).rvz"
    python tools/convert_rvz.py  # auto-detect from common locations
"""

import os
import shutil
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
OUTPUT_DIR = PROJECT_ROOT / "orig" / "GC6E01"
OUTPUT_ISO = OUTPUT_DIR / "game.iso"

# Common locations to search for the RVZ
SEARCH_PATHS = [
    Path.home() / "Downloads",
    Path.home() / "Downloads" / "Pokemon Colosseum (USA)",
    PROJECT_ROOT / "rom",
    PROJECT_ROOT / "orig" / "GC6E01",
]

RVZ_PATTERNS = [
    "Pokemon Colosseum*.rvz",
    "GC6E01*.rvz",
    "*.rvz",
]


def find_dolphin_tool():
    """Locate DolphinTool executable."""
    # Check PATH
    for name in ("DolphinTool", "dolphin-tool", "DolphinTool.exe"):
        path = shutil.which(name)
        if path:
            return path

    # Common Windows install locations
    if sys.platform == "win32":
        common_dirs = [
            Path(os.environ.get("PROGRAMFILES", "C:/Program Files")) / "Dolphin",
            Path(os.environ.get("PROGRAMFILES(X86)", "C:/Program Files (x86)")) / "Dolphin",
            Path.home() / "AppData" / "Local" / "Dolphin",
            Path.home() / "scoop" / "apps" / "dolphin" / "current",
        ]
        for d in common_dirs:
            tool_path = d / "DolphinTool.exe"
            if tool_path.exists():
                return str(tool_path)

    return None


def find_rvz(explicit_path=None):
    """Find the RVZ file."""
    if explicit_path:
        p = Path(explicit_path)
        if p.exists():
            return p
        print(f"ERROR: File not found: {explicit_path}")
        return None

    # Auto-detect
    for search_dir in SEARCH_PATHS:
        if not search_dir.exists():
            continue
        for pattern in RVZ_PATTERNS:
            matches = list(search_dir.glob(pattern))
            if matches:
                return matches[0]

    return None


def convert_rvz_to_iso(rvz_path, dolphin_tool):
    """Convert RVZ to ISO using DolphinTool."""
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    if OUTPUT_ISO.exists():
        print(f"ISO already exists at {OUTPUT_ISO}")
        resp = input("Overwrite? [y/N] ").strip().lower()
        if resp != "y":
            return True

    print(f"Converting: {rvz_path}")
    print(f"Output:     {OUTPUT_ISO}")
    print("This may take a few minutes...")

    cmd = [
        dolphin_tool,
        "convert",
        "-i", str(rvz_path),
        "-o", str(OUTPUT_ISO),
        "-f", "iso",
    ]

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        if result.stdout:
            print(result.stdout)
        print(f"\nConversion complete: {OUTPUT_ISO}")
        print(f"Size: {OUTPUT_ISO.stat().st_size / (1024*1024):.1f} MB")
        return True
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Conversion failed")
        if e.stderr:
            print(e.stderr)
        return False
    except FileNotFoundError:
        print(f"ERROR: Could not run DolphinTool at {dolphin_tool}")
        return False


def main():
    explicit_path = sys.argv[1] if len(sys.argv) > 1 else None

    # Find DolphinTool
    dolphin_tool = find_dolphin_tool()
    if not dolphin_tool:
        print("ERROR: DolphinTool not found.")
        print("Install Dolphin Emulator from https://dolphin-emu.org/download/")
        print("Then either add it to PATH or convert manually:")
        print('  DolphinTool convert -i "game.rvz" -o "game.iso" -f iso')
        return 1

    print(f"Using DolphinTool: {dolphin_tool}")

    # Find RVZ
    rvz_path = find_rvz(explicit_path)
    if not rvz_path:
        print("ERROR: No RVZ file found.")
        print("Searched in:")
        for d in SEARCH_PATHS:
            print(f"  {d}")
        print("\nProvide the path explicitly:")
        print('  python tools/convert_rvz.py "path/to/game.rvz"')
        return 1

    # Convert
    if not convert_rvz_to_iso(rvz_path, dolphin_tool):
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
