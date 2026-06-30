#!/usr/bin/env python3

"""
Extract start.dol and REL modules from a Pokémon Colosseum ISO.

Usage:
    python tools/extract_dol.py                    # Auto-detect ISO
    python tools/extract_dol.py orig/GC6E01/game.iso
"""

import os
import hashlib
import shutil
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
ORIG_DIR = PROJECT_ROOT / "orig" / "GC6E01"
TOOLS_DIR = PROJECT_ROOT / "tools"


def find_dtk():
    """Locate the dtk binary."""
    # Check tools directory
    for name in ("dtk", ("dtk.exe" if os.name == "nt" else "dtk")):
        path = TOOLS_DIR / name
        if path.exists():
            return str(path)

    # Check PATH
    path = shutil.which("dtk")
    if path:
        return path

    return None


def find_iso():
    """Find the ISO in the orig directory."""
    for ext in ("*.iso", "*.gcm"):
        matches = list(ORIG_DIR.glob(ext))
        if matches:
            return matches[0]
    return None


def sha1_file(path):
    """Compute SHA-1 hash of a file."""
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def main():
    iso_path = Path(sys.argv[1]) if len(sys.argv) > 1 else find_iso()

    if not iso_path or not iso_path.exists():
        print("ERROR: No ISO found.")
        print(f"Place your ISO in {ORIG_DIR}/ or provide the path as an argument.")
        print("\nIf you have an RVZ file, convert it first:")
        print("  python tools/convert_rvz.py")
        return 1

    dtk = find_dtk()
    if not dtk:
        print("ERROR: decomp-toolkit (dtk) not found.")
        print("Install: cargo install decomp-toolkit")
        return 1

    print(f"ISO:  {iso_path}")
    print(f"DTK:  {dtk}")
    print(f"Dest: {ORIG_DIR}")
    print()

    # Extract using dtk
    # dtk can extract the DOL and filesystem from a GCN ISO
    try:
        # First, get disc info
        result = subprocess.run(
            [dtk, "disc", "info", str(iso_path)],
            capture_output=True,
            text=True,
        )
        if result.stdout:
            print("Disc Info:")
            print(result.stdout)
        if result.returncode != 0 and result.stderr:
            print(result.stderr)

        # Extract system files (DOL, apploader, etc.)
        print("Extracting system files...")
        result = subprocess.run(
            [dtk, "disc", "extract", str(iso_path), str(ORIG_DIR)],
            capture_output=True,
            text=True,
        )
        if result.stdout:
            print(result.stdout)
        if result.returncode != 0:
            if result.stderr:
                print(f"ERROR: {result.stderr}")
            return 1

    except FileNotFoundError:
        print(f"ERROR: Could not run dtk at {dtk}")
        return 1

    # Verify DOL was extracted
    dol_path = ORIG_DIR / "sys" / "main.dol"
    if not dol_path.exists():
        # dtk might use different output paths
        for candidate in (
            ORIG_DIR / "start.dol",
            ORIG_DIR / "sys" / "main.dol",
            ORIG_DIR / "main.dol",
        ):
            if candidate.exists():
                dol_path = candidate
                break
        else:
            print("WARNING: start.dol not found in expected locations.")
            print("Check the extraction output above and locate the DOL manually.")
            return 1

    # Rename to start.dol if needed
    target = ORIG_DIR / "start.dol"
    if dol_path != target:
        shutil.copy2(dol_path, target)

    # Compute and display hash
    sha1 = sha1_file(target)
    print(f"\nDOL extracted: {target}")
    print(f"SHA-1: {sha1}")
    print()
    print("Record this hash in config/GC6E01/config.yml under dol.hash")
    print("and in config/GC6E01/build.sha1 for build verification.")

    # Write build.sha1
    sha1_file_path = PROJECT_ROOT / "config" / "GC6E01" / "build.sha1"
    sha1_file_path.write_text(f"{sha1}  start.dol\n")
    print(f"Wrote {sha1_file_path}")

    # Check for REL files
    rel_files = list(ORIG_DIR.rglob("*.rel"))
    if rel_files:
        print(f"\nFound {len(rel_files)} REL module(s):")
        for rel in rel_files:
            rel_sha1 = sha1_file(rel)
            print(f"  {rel.name} — SHA-1: {rel_sha1}")
    else:
        print("\nNo REL modules found in extraction output.")
        print("RELs may be packed inside FSYS archives and need separate extraction.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
