#!/usr/bin/env python3
"""
ghidra_import.py - Import cleaned Ghidra decompilation into the project.

Reads the staged output from build/ghidra_output/ (produced by
process_ghidra_output.py) and:

  1. For each staged source file, compares against the existing src/ file.
  2. For functions NOT yet decompiled in src/, inserts the Ghidra stub.
  3. Compiles each modified file with mwcceppc to verify it builds.
  4. Reports overall match rate / compilation success.

Usage:
    python tools/ghidra_import.py                    # dry-run (default)
    python tools/ghidra_import.py --apply            # actually write to src/
    python tools/ghidra_import.py --compile-check    # compile after importing
    python tools/ghidra_import.py --apply --compile-check
    python tools/ghidra_import.py --file src/game/main.c  # single file only

Modes:
    dry-run (default):  Shows what would be imported, without writing anything.
    --apply:            Writes Ghidra stubs into src/ files.
    --compile-check:    Runs mwcceppc on each modified file after import.
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path

# ============================================================================
# Paths
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parent.parent
SRC_DIR = PROJECT_ROOT / "src"
STAGED_DIR = PROJECT_ROOT / "build" / "ghidra_output"
INCLUDE_DIR = PROJECT_ROOT / "include"
BUILD_DIR = PROJECT_ROOT / "build" / "GC6E01"
TOOLS_DIR = PROJECT_ROOT / "tools"
MWCC_BASE = TOOLS_DIR / "mwcc_compiler"
MWCC_DEFAULT = MWCC_BASE / "mwcceppc.exe"
MWCC_GC_DIR = MWCC_BASE / "GC"

# Compile config.
COMPILE_CONFIG_PATH = PROJECT_ROOT / "config" / "GC6E01" / "compile_config.json"
DEFAULT_COMPILER_VERSION = "1.2.5n"

# ============================================================================
# Data structures
# ============================================================================


@dataclass
class StagedFunction:
    """A function parsed from a staged Ghidra output file."""
    name: str
    address: int
    size: int
    code: str
    failed: bool = False


@dataclass
class ImportResult:
    """Result of importing a single file."""
    src_path: str
    functions_available: int    # total functions in staged file
    functions_imported: int     # functions actually imported
    functions_skipped: int      # functions already in src/
    compile_success: bool | None = None  # None if not tested
    compile_error: str = ""


# ============================================================================
# Parsing staged files
# ============================================================================

FUNC_HEADER = re.compile(
    r"/\*\s*----\s+(\S+)\s+"
    r"addr=0x([0-9a-fA-F]+)\s+"
    r"size=0x([0-9a-fA-F]+)"
    r"\s*----\s*\*/"
)


def parse_staged_file(filepath: Path) -> list[StagedFunction]:
    """Parse a staged Ghidra output file into function objects."""
    text = filepath.read_text(encoding="utf-8", errors="replace")
    functions = []

    # Split on function headers.
    parts = FUNC_HEADER.split(text)

    i = 1
    while i + 3 < len(parts):
        name = parts[i]
        addr = int(parts[i + 1], 16)
        size = int(parts[i + 2], 16)
        code_block = parts[i + 3] if i + 3 < len(parts) else ""

        # Trim to just the function code (stop at next function or EOF).
        code_block = code_block.strip()

        failed = "DECOMPILATION FAILED" in code_block
        functions.append(StagedFunction(
            name=name,
            address=addr,
            size=size,
            code=code_block,
            failed=failed,
        ))
        i += 4

    return functions


# ============================================================================
# Check if function exists in source
# ============================================================================


def function_exists_in_source(func: StagedFunction, src_text: str) -> bool:
    """Check if a function (by name or address) already has a body in the source."""
    # Check by function name.
    if func.name in src_text:
        # Look for a definition (not just a declaration).
        # A definition has a { after the function signature.
        pattern = re.compile(
            r"\b" + re.escape(func.name) + r"\s*\([^)]*\)\s*\{",
            re.MULTILINE
        )
        if pattern.search(src_text):
            return True

    # Check by address-based name variants.
    addr_str = f"{func.address:08X}"
    addr_str_lower = f"{func.address:08x}"
    for prefix in ["fn_", "FUN_"]:
        for addr in [addr_str, addr_str_lower]:
            tag = prefix + addr
            if tag in src_text:
                # Same definition check.
                pattern = re.compile(
                    r"\b" + re.escape(tag) + r"\s*\([^)]*\)\s*\{",
                    re.MULTILINE
                )
                if pattern.search(src_text):
                    return True

    return False


# ============================================================================
# Import functions into source files
# ============================================================================


def import_into_source(
    src_path: Path,
    staged_funcs: list[StagedFunction],
    apply: bool,
) -> ImportResult:
    """Import staged Ghidra functions into an existing (or new) source file.

    If apply=False, only report what would be done (dry run).
    """
    result = ImportResult(
        src_path=str(src_path),
        functions_available=len(staged_funcs),
        functions_imported=0,
        functions_skipped=0,
    )

    # Read existing source (if any).
    src_full = SRC_DIR / src_path if not src_path.startswith("src") else PROJECT_ROOT / src_path
    if isinstance(src_full, str):
        src_full = Path(src_full)

    # Normalize: if src_path is "src/game/main.c", we want PROJECT_ROOT / src_path.
    # If it's "game/main.c", we want SRC_DIR / src_path.
    if str(src_path).startswith("src/") or str(src_path).startswith("src\\"):
        src_full = PROJECT_ROOT / src_path
    else:
        src_full = SRC_DIR / src_path

    existing_text = ""
    if src_full.exists():
        existing_text = src_full.read_text(encoding="utf-8", errors="replace")

    # Determine which functions to import.
    to_import = []
    for func in staged_funcs:
        if func.failed:
            result.functions_skipped += 1
            continue
        if function_exists_in_source(func, existing_text):
            result.functions_skipped += 1
            continue
        to_import.append(func)

    result.functions_imported = len(to_import)

    if not to_import:
        return result

    if not apply:
        return result

    # Build the import block to append.
    import_block = []
    import_block.append("")
    import_block.append("/* ================================================================")
    import_block.append(" * AUTO-IMPORTED FROM GHIDRA DECOMPILATION")
    import_block.append(" * These stubs need manual review and cleanup.")
    import_block.append(" * ================================================================ */")
    import_block.append("")

    for func in to_import:
        import_block.append(f"/* {func.name}  addr=0x{func.address:08X}"
                           f"  size=0x{func.size:X} */")
        import_block.append(func.code)
        import_block.append("")

    import_text = "\n".join(import_block) + "\n"

    # Write.
    src_full.parent.mkdir(parents=True, exist_ok=True)
    if existing_text:
        # Append to existing file.
        with open(src_full, "a", encoding="utf-8") as f:
            f.write(import_text)
    else:
        # New file: add a header.
        header = (
            f'/**\n'
            f' * @file {Path(src_path).name}\n'
            f' * @brief Auto-generated from Ghidra decompilation.\n'
            f' */\n\n'
            f'#include "dolphin/types.h"\n\n'
        )
        with open(src_full, "w", encoding="utf-8") as f:
            f.write(header + import_text)

    return result


# ============================================================================
# Compilation check
# ============================================================================


def load_compile_config() -> dict:
    """Load compile_config.json."""
    if COMPILE_CONFIG_PATH.exists():
        try:
            with open(COMPILE_CONFIG_PATH) as f:
                return json.load(f)
        except (json.JSONDecodeError, OSError):
            pass
    return {
        "default": {
            "compiler": "GC/1.2.5n",
            "flags": "-O4,p -nodefaults -proc gekko -fp hard "
                     "-Cpp_exceptions off -enum int -warn off",
        },
        "overrides": {},
    }


def get_compiler_for_file(src_path: str, config: dict) -> Path:
    """Get the mwcceppc.exe path for the given source file."""
    overrides = config.get("overrides", {})
    compiler_str = config.get("default", {}).get("compiler", "GC/1.2.5n")

    for key in [src_path, src_path.replace("\\", "/")]:
        if key in overrides and "compiler" in overrides[key]:
            compiler_str = overrides[key]["compiler"]
            break

    # Extract version.
    version = compiler_str.split("/", 1)[1] if "/" in compiler_str else compiler_str
    versioned = MWCC_GC_DIR / version / "mwcceppc.exe"
    if versioned.exists():
        return versioned
    if MWCC_DEFAULT.exists():
        return MWCC_DEFAULT
    return None


def get_flags_for_file(src_path: str, config: dict) -> list[str]:
    """Get compiler flags for the given source file."""
    overrides = config.get("overrides", {})
    flags_str = config.get("default", {}).get(
        "flags",
        "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off"
    )

    for key in [src_path, src_path.replace("\\", "/")]:
        if key in overrides and "flags" in overrides[key]:
            flags_str = overrides[key]["flags"]
            break

    return flags_str.split() + ["-i", str(INCLUDE_DIR)]


def compile_check_file(src_path: str) -> tuple[bool, str]:
    """Try to compile a source file with mwcceppc. Returns (success, error_msg)."""
    config = load_compile_config()
    compiler = get_compiler_for_file(src_path, config)
    if compiler is None:
        return False, "mwcceppc.exe not found"

    # Resolve the full path.
    if src_path.startswith("src/") or src_path.startswith("src\\"):
        full_path = PROJECT_ROOT / src_path
    else:
        full_path = SRC_DIR / src_path

    if not full_path.exists():
        return False, f"Source file not found: {full_path}"

    flags = get_flags_for_file(src_path, config)

    # Compile to a temp .o file.
    with tempfile.NamedTemporaryFile(suffix=".o", delete=False) as tmp:
        tmp_obj = tmp.name

    try:
        cmd = [str(compiler)] + flags + ["-c", "-o", tmp_obj, str(full_path)]
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=30,
            cwd=str(PROJECT_ROOT),
        )
        if result.returncode == 0:
            return True, ""
        else:
            error = result.stderr.strip() or result.stdout.strip()
            # Truncate long error messages.
            if len(error) > 500:
                error = error[:500] + "..."
            return False, error
    except FileNotFoundError:
        return False, f"Compiler not found: {compiler}"
    except subprocess.TimeoutExpired:
        return False, "Compilation timed out"
    finally:
        try:
            os.unlink(tmp_obj)
        except OSError:
            pass


# ============================================================================
# Main
# ============================================================================


def main():
    parser = argparse.ArgumentParser(
        description="Import cleaned Ghidra stubs into the project source tree."
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually write to src/ (default is dry-run).",
    )
    parser.add_argument(
        "--compile-check",
        action="store_true",
        help="Compile each modified file after import to verify it builds.",
    )
    parser.add_argument(
        "--staged-dir",
        type=Path,
        default=STAGED_DIR,
        help="Directory containing staged Ghidra output.",
    )
    parser.add_argument(
        "--file",
        type=str,
        default=None,
        help="Only process a single source file (e.g. src/game/main.c).",
    )
    args = parser.parse_args()

    if not args.staged_dir.exists():
        print(f"ERROR: Staged directory not found: {args.staged_dir}", file=sys.stderr)
        print("Run 'python tools/process_ghidra_output.py' first.", file=sys.stderr)
        sys.exit(1)

    mode_str = "APPLY" if args.apply else "DRY-RUN"
    print(f"Ghidra Import ({mode_str})")
    print("=" * 72)

    # Find all staged .c files.
    staged_files = sorted(args.staged_dir.rglob("*.c"))
    if not staged_files:
        print("No staged .c files found in", args.staged_dir)
        sys.exit(0)

    # Filter to single file if requested.
    if args.file:
        target = args.file.replace("\\", "/")
        staged_files = [
            f for f in staged_files
            if target in str(f).replace("\\", "/")
        ]
        if not staged_files:
            print(f"No staged file matching '{args.file}' found.", file=sys.stderr)
            sys.exit(1)

    print(f"Found {len(staged_files)} staged file(s) to process.\n")

    # Process each file.
    results: list[ImportResult] = []
    for staged_file in staged_files:
        # Determine the source path relative to the staged dir.
        try:
            rel = staged_file.relative_to(args.staged_dir)
        except ValueError:
            continue

        src_path = str(rel).replace("\\", "/")

        # Skip __unassigned__.c in dry-run summary but still report it.
        if "__unassigned__" in src_path:
            pass  # Still process it.

        # Parse staged functions.
        funcs = parse_staged_file(staged_file)
        if not funcs:
            continue

        # Import.
        result = import_into_source(src_path, funcs, apply=args.apply)

        # Compile check if requested and we actually wrote something.
        if args.compile_check and args.apply and result.functions_imported > 0:
            success, error = compile_check_file(src_path)
            result.compile_success = success
            result.compile_error = error

        results.append(result)

    # Summary.
    print()
    print("=" * 72)
    print("Import Summary")
    print("=" * 72)
    print()

    total_available = 0
    total_imported = 0
    total_skipped = 0
    total_compiled = 0
    total_compile_fail = 0

    for r in results:
        total_available += r.functions_available
        total_imported += r.functions_imported
        total_skipped += r.functions_skipped

        status = ""
        if r.compile_success is True:
            status = " [COMPILES]"
            total_compiled += 1
        elif r.compile_success is False:
            status = " [COMPILE FAIL]"
            total_compile_fail += 1

        if r.functions_imported > 0 or r.functions_available > 5:
            print(f"  {r.src_path}")
            print(f"    Available: {r.functions_available}  "
                  f"Imported: {r.functions_imported}  "
                  f"Skipped: {r.functions_skipped}{status}")
            if r.compile_error:
                # Show first line of error.
                first_line = r.compile_error.split("\n")[0]
                print(f"    Error: {first_line}")

    print()
    print(f"Total functions in staged files:  {total_available}")
    print(f"Total imported (new):             {total_imported}")
    print(f"Total skipped (already exist):    {total_skipped}")

    if args.compile_check and args.apply:
        tested = total_compiled + total_compile_fail
        if tested > 0:
            rate = total_compiled / tested * 100
            print(f"Compile success rate:             {total_compiled}/{tested}"
                  f" ({rate:.1f}%)")

    if not args.apply:
        print()
        print("This was a dry run. Use --apply to actually write to src/.")

    print()


if __name__ == "__main__":
    main()
