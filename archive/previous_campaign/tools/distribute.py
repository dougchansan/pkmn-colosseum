#!/usr/bin/env python3
"""
distribute.py - Create a self-contained matching kit archive for distribution.

Packages everything a contributor needs to compile and match functions on a
Windows machine, without requiring the ISO, Dolphin, Ghidra, or the full
compiler archive.

Usage:
    python tools/distribute.py
    python tools/distribute.py --output my_kit.zip
    python tools/distribute.py --dry-run
"""

import argparse
import os
import sys
import zipfile
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT = PROJECT_ROOT / "tools" / "colosseum_matching_kit.zip"

# ---------------------------------------------------------------------------
# Files and directories to include
# ---------------------------------------------------------------------------

# Directories copied recursively (relative to PROJECT_ROOT)
INCLUDE_DIRS = [
    "src",
    "include",
    "config/GC6E01",
    "build/GC6E01/obj",
    "docs",
    "tools/mwcc_compiler/GC/1.2.5n",
    "tools/mwcc_compiler/GC/1.3",
]

# Individual files (relative to PROJECT_ROOT)
INCLUDE_FILES = [
    "tools/compile_check.py",
    "tools/match_test.py",
    "tools/quick_diff.py",
    "tools/assign_work.py",
    "tools/merge_results.py",
    ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli"),
    "tools/mwcc_compiler/mwcceppc.exe",
    "tools/mwcc_compiler/mwasmeppc.exe",
    "tools/mwcc_compiler/mwldeppc.exe",
    "tools/mwcc_compiler/lmgr326b.dll",
    "objdiff.json",
    "README_MATCHING.md",
]

# File extensions to skip inside included directories
SKIP_EXTENSIONS = {
    ".iso", ".rvz", ".gcm", ".nkit",  # disc images
    ".elf",                             # large binaries
    ".zip", ".7z", ".tar", ".gz",       # archives
    ".pyc",                             # bytecode
}

# Directory names to skip (anywhere in the tree)
SKIP_DIRS = {
    "__pycache__",
    ".git",
    "ghidra_12.0.4_PUBLIC",
    "dolphin-2603a-x64",
}

# Max individual file size (50 MB) -- skip anything bigger
MAX_FILE_SIZE = 50 * 1024 * 1024


def should_skip(path: Path) -> bool:
    """Return True if a file should be excluded from the archive."""
    if path.suffix.lower() in SKIP_EXTENSIONS:
        return True
    for part in path.parts:
        if part in SKIP_DIRS:
            return True
    try:
        if path.stat().st_size > MAX_FILE_SIZE:
            return True
    except OSError:
        return True
    return False


def collect_files() -> list:
    """Collect all files to include in the archive.

    Returns a list of (absolute_path, archive_relative_path) tuples.
    """
    files = []

    # Recurse through included directories
    for dir_rel in INCLUDE_DIRS:
        dir_abs = PROJECT_ROOT / dir_rel
        if not dir_abs.is_dir():
            print(f"  WARNING: Directory not found, skipping: {dir_rel}")
            continue
        for root, dirs, filenames in os.walk(dir_abs):
            root_path = Path(root)
            # Prune skipped directories
            dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
            for fname in filenames:
                fpath = root_path / fname
                if should_skip(fpath):
                    continue
                arc_path = fpath.relative_to(PROJECT_ROOT)
                files.append((fpath, str(arc_path).replace("\\", "/")))

    # Add individual files
    for file_rel in INCLUDE_FILES:
        fpath = PROJECT_ROOT / file_rel
        if not fpath.is_file():
            print(f"  WARNING: File not found, skipping: {file_rel}")
            continue
        if should_skip(fpath):
            print(f"  WARNING: File too large or excluded, skipping: {file_rel}")
            continue
        files.append((fpath, file_rel.replace("\\", "/")))

    # Deduplicate (same archive path)
    seen = set()
    deduped = []
    for abs_path, arc_path in files:
        if arc_path not in seen:
            seen.add(arc_path)
            deduped.append((abs_path, arc_path))

    return deduped


def create_archive(output_path: Path, dry_run: bool = False):
    """Create the distribution zip archive."""
    print(f"Collecting files from {PROJECT_ROOT} ...")
    files = collect_files()

    if not files:
        print("ERROR: No files collected. Check your project directory.")
        sys.exit(1)

    total_size = 0
    for abs_path, _ in files:
        try:
            total_size += abs_path.stat().st_size
        except OSError:
            pass

    print(f"  {len(files)} files, {total_size / (1024*1024):.1f} MB uncompressed")

    if dry_run:
        print("\n--- DRY RUN: Files that would be included ---")
        for _, arc_path in sorted(files, key=lambda x: x[1]):
            print(f"  {arc_path}")
        print(f"\nTotal: {len(files)} files")
        return

    output_path.parent.mkdir(parents=True, exist_ok=True)

    print(f"\nCreating archive: {output_path}")
    with zipfile.ZipFile(output_path, "w", zipfile.ZIP_DEFLATED,
                         compresslevel=6) as zf:
        for i, (abs_path, arc_path) in enumerate(files):
            zf.write(abs_path, arc_path)
            if (i + 1) % 100 == 0:
                print(f"  {i + 1}/{len(files)} files written ...")

    archive_size = output_path.stat().st_size
    print(f"\nDone! Archive: {output_path}")
    print(f"  Size: {archive_size / (1024*1024):.1f} MB compressed")
    print(f"  Files: {len(files)}")
    print(f"\nTo use: unzip and follow README_MATCHING.md")


def main():
    parser = argparse.ArgumentParser(
        description="Create a self-contained matching kit archive for "
                    "Pokemon Colosseum decompilation.",
    )
    parser.add_argument(
        "--output", "-o", default=str(DEFAULT_OUTPUT),
        help=f"Output zip path (default: {DEFAULT_OUTPUT.relative_to(PROJECT_ROOT)})",
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="List files that would be included without creating the archive",
    )

    args = parser.parse_args()
    create_archive(Path(args.output), dry_run=args.dry_run)


if __name__ == "__main__":
    main()
