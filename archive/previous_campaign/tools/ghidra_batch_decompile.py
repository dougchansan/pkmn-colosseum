#!/usr/bin/env python3
"""
ghidra_batch_decompile.py - Run Ghidra headless analysis on the DOL and export
decompiled C for every function.

This script:
  1. Locates Ghidra's analyzeHeadless command
  2. Imports the DOL with the Gekko/Broadway processor
  3. Runs the ExportDecompilation.java GhidraScript
  4. Writes raw decompiled C to build/ghidra_output/raw_decompilation.c

Usage:
    python tools/ghidra_batch_decompile.py
    python tools/ghidra_batch_decompile.py --ghidra-home C:/path/to/ghidra
    python tools/ghidra_batch_decompile.py --reimport   # delete existing project first

Environment:
    GHIDRA_HOME   - Path to the Ghidra installation root (alternative to --ghidra-home)
"""

import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path

# ============================================================================
# Paths
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DOL_PATH = PROJECT_ROOT / "orig" / "GC6E01" / "start.dol"
OUTPUT_DIR = PROJECT_ROOT / "build" / "ghidra_output"
RAW_OUTPUT = OUTPUT_DIR / "raw_decompilation.c"
GHIDRA_PROJECT_DIR = PROJECT_ROOT / "build" / "ghidra_project"
GHIDRA_PROJECT_NAME = "PokemonColosseum"
SCRIPT_DIR = PROJECT_ROOT / "tools" / "ghidra_scripts"

# Ghidra processor language ID for GameCube (Gekko/Broadway variant).
PROCESSOR_ID = "PowerPC:BE:32:Gekko_Broadway"

# Default Ghidra install location.
DEFAULT_GHIDRA_HOME = Path(r"C:\Users\douglaswhittingham\tools\ghidra_12.0.4_PUBLIC")


def find_ghidra(ghidra_home: Path | None) -> Path:
    """Return the path to the analyzeHeadless script/bat."""
    candidates = []

    if ghidra_home:
        candidates.append(ghidra_home)
    if "GHIDRA_HOME" in os.environ:
        candidates.append(Path(os.environ["GHIDRA_HOME"]))
    candidates.append(DEFAULT_GHIDRA_HOME)

    for base in candidates:
        if not base.exists():
            continue
        support = base / "support"
        if platform.system() == "Windows":
            bat = support / "analyzeHeadless.bat"
            if bat.exists():
                return bat
        else:
            sh = support / "analyzeHeadless"
            if sh.exists():
                return sh

    print("ERROR: Could not locate Ghidra analyzeHeadless.", file=sys.stderr)
    print("  Set GHIDRA_HOME or pass --ghidra-home.", file=sys.stderr)
    sys.exit(1)


def build_command(analyze_headless: Path, reimport: bool) -> list[str]:
    """Build the analyzeHeadless command line."""
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    GHIDRA_PROJECT_DIR.mkdir(parents=True, exist_ok=True)

    cmd = [
        str(analyze_headless),
        str(GHIDRA_PROJECT_DIR),
        GHIDRA_PROJECT_NAME,
    ]

    # Check if a Ghidra project already exists.
    project_file = GHIDRA_PROJECT_DIR / (GHIDRA_PROJECT_NAME + ".gpr")
    if reimport or not project_file.exists():
        if reimport and project_file.exists():
            # Remove old project files.
            for f in GHIDRA_PROJECT_DIR.glob(GHIDRA_PROJECT_NAME + ".*"):
                if f.is_dir():
                    shutil.rmtree(f)
                else:
                    f.unlink()
            print(f"Removed existing Ghidra project: {project_file}")

        cmd += [
            "-import", str(DOL_PATH),
            "-processor", PROCESSOR_ID,
        ]
    else:
        # Project exists -- just process it (no re-import).
        cmd += ["-process", GHIDRA_PROJECT_NAME]

    cmd += [
        "-scriptPath", str(SCRIPT_DIR),
        "-postScript", "ExportDecompilation.java",
        str(RAW_OUTPUT),        # passed as script argument
        "-noanalysis" if project_file.exists() and not reimport else "",
    ]

    # Remove empty strings from command.
    cmd = [c for c in cmd if c]

    return cmd


def main():
    parser = argparse.ArgumentParser(
        description="Run Ghidra headless decompilation on the DOL."
    )
    parser.add_argument(
        "--ghidra-home",
        type=Path,
        default=None,
        help="Path to Ghidra installation root.",
    )
    parser.add_argument(
        "--reimport",
        action="store_true",
        help="Delete any existing Ghidra project and re-import the DOL.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print the command that would be run, without executing it.",
    )
    args = parser.parse_args()

    # Validate DOL.
    if not DOL_PATH.exists():
        print(f"ERROR: DOL not found at {DOL_PATH}", file=sys.stderr)
        sys.exit(1)

    analyze_headless = find_ghidra(args.ghidra_home)
    print(f"Using Ghidra: {analyze_headless}")

    cmd = build_command(analyze_headless, args.reimport)

    print()
    print("Command:")
    print("  " + " ".join(cmd))
    print()

    if args.dry_run:
        print("(dry-run mode -- not executing)")
        return

    print("=" * 72)
    print("Starting Ghidra headless analysis...")
    print("This may take 10-30 minutes depending on your machine.")
    print("=" * 72)
    print()

    try:
        result = subprocess.run(
            cmd,
            cwd=str(PROJECT_ROOT),
            # Let stdout/stderr pass through to the terminal.
            timeout=3600,  # 1 hour max
        )
    except subprocess.TimeoutExpired:
        print("\nERROR: Ghidra analysis timed out after 1 hour.", file=sys.stderr)
        sys.exit(1)
    except FileNotFoundError:
        print(f"\nERROR: Could not execute: {cmd[0]}", file=sys.stderr)
        print("Make sure Java is installed and on PATH.", file=sys.stderr)
        sys.exit(1)

    if result.returncode != 0:
        print(f"\nWARNING: Ghidra exited with code {result.returncode}",
              file=sys.stderr)

    if RAW_OUTPUT.exists():
        size_kb = RAW_OUTPUT.stat().st_size / 1024
        print(f"\nSuccess! Raw decompilation written to:")
        print(f"  {RAW_OUTPUT}  ({size_kb:.1f} KB)")
        print(f"\nNext step: python tools/process_ghidra_output.py")
    else:
        print(f"\nWARNING: Expected output file not found: {RAW_OUTPUT}",
              file=sys.stderr)
        print("Check Ghidra log output above for errors.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
