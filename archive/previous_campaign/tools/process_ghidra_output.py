#!/usr/bin/env python3
"""
process_ghidra_output.py - Clean up raw Ghidra decompilation output.

Reads build/ghidra_output/raw_decompilation.c produced by
ExportDecompilation.java and:

  1. Parses individual functions (delimited by the ===== header comments).
  2. Replaces Ghidra type names with project types (undefined4 -> u32, etc.).
  3. Renames Ghidra parameter placeholders (param_1 -> r3, param_2 -> r4, ...).
  4. Groups functions by address range into source files according to
     config/GC6E01/link_order.txt.
  5. Skips functions that already exist in our src/ directory.
  6. Writes cleaned-up C files to build/ghidra_output/<source_path>.

Usage:
    python tools/process_ghidra_output.py
    python tools/process_ghidra_output.py --input build/ghidra_output/raw_decompilation.c
    python tools/process_ghidra_output.py --skip-existing    # default: skip existing
    python tools/process_ghidra_output.py --no-skip-existing # include all functions
"""

import argparse
import os
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

# ============================================================================
# Paths
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_INPUT = PROJECT_ROOT / "build" / "ghidra_output" / "raw_decompilation.c"
OUTPUT_BASE = PROJECT_ROOT / "build" / "ghidra_output"
LINK_ORDER = PROJECT_ROOT / "config" / "GC6E01" / "link_order.txt"
SRC_DIR = PROJECT_ROOT / "src"

# ============================================================================
# Type replacement table: Ghidra type -> project type
# ============================================================================

TYPE_REPLACEMENTS: list[tuple[str, str]] = [
    # Ghidra undefined types -> project unsigned types (longest first).
    (r"\bundefined8\b", "u64"),
    (r"\bundefined4\b", "u32"),
    (r"\bundefined2\b", "u16"),
    (r"\bundefined1\b", "u8"),
    (r"\bundefined\b",  "u8"),
    # Ghidra C types -> project types.
    (r"\bulonglong\b",  "u64"),
    (r"\blonglong\b",   "s64"),
    (r"\buint\b",       "u32"),
    (r"\bushort\b",     "u16"),
    (r"\bbyte\b",       "u8"),
    # Pointer-sized integer.
    (r"\bulong\b",      "u32"),
    # Ghidra bool type.
    (r"\bbool\b",       "BOOL"),
]

# Compiled regex list (built once).
TYPE_REGEX: list[tuple[re.Pattern, str]] = [
    (re.compile(pat), repl) for pat, repl in TYPE_REPLACEMENTS
]

# ============================================================================
# Parameter renaming: param_N -> rN (PowerPC calling convention)
#
# PowerPC/Gekko calling convention:
#   Integer args: r3, r4, r5, r6, r7, r8, r9, r10
#   Float args:   f1, f2, f3, f4, f5, f6, f7, f8
#
# Ghidra uses param_1, param_2, ... for integer params and
# sometimes fParam_1, fParam_2, ... for float params.
# ============================================================================

# Integer parameter mapping (param_1 = first arg = r3, etc.)
INT_PARAM_MAP = {
    "param_1": "r3",
    "param_2": "r4",
    "param_3": "r5",
    "param_4": "r6",
    "param_5": "r7",
    "param_6": "r8",
    "param_7": "r9",
    "param_8": "r10",
}

# Float parameter mapping.
FLOAT_PARAM_MAP = {
    "fParam_1": "f1",
    "fParam_2": "f2",
    "fParam_3": "f3",
    "fParam_4": "f4",
    "fParam_5": "f5",
    "fParam_6": "f6",
    "fParam_7": "f7",
    "fParam_8": "f8",
}

# Build a combined regex for parameter renaming.
ALL_PARAMS = {}
ALL_PARAMS.update(INT_PARAM_MAP)
ALL_PARAMS.update(FLOAT_PARAM_MAP)

# Sort by length descending to avoid partial matches (e.g. param_10 before param_1).
PARAM_PATTERN = re.compile(
    r"\b(" + "|".join(
        re.escape(k) for k in sorted(ALL_PARAMS.keys(), key=len, reverse=True)
    ) + r")\b"
)

# ============================================================================
# Additional cleanup patterns
# ============================================================================

# Remove Ghidra's "in_rN" register references and replace with just "rN".
IN_REGISTER_PATTERN = re.compile(r"\bin_(r\d+)\b")
IN_FREGISTER_PATTERN = re.compile(r"\bin_(f\d+)\b")
IN_CR_PATTERN = re.compile(r"\bin_(cr\d+)\b")

# Remove casts to the same type: (type)expr where it's redundant.
# We keep casts generally since they may be meaningful, but clean up
# double-casts like (u32)(u32)x.
DOUBLE_CAST_PATTERN = re.compile(r"\((\w+)\)\s*\(\1\)")

# ============================================================================
# Data structures
# ============================================================================


@dataclass
class GhidraFunction:
    """A single decompiled function parsed from Ghidra output."""
    name: str
    address: int          # Entry point address.
    size: int             # Body size in bytes.
    code: str             # Decompiled C code.
    failed: bool = False  # True if decompilation failed.


@dataclass
class TranslationUnit:
    """A source file entry from link_order.txt."""
    src_path: str         # e.g. "src/game/main.c"
    start_addr: int
    end_addr: int
    func_count: str       # May be "~45" or "21" etc.
    notes: str = ""


@dataclass
class OutputFile:
    """Accumulated output for a single source file."""
    src_path: str
    functions: list[GhidraFunction] = field(default_factory=list)


# ============================================================================
# Parsing
# ============================================================================

# Pattern matching the function header comment.
# /* ===== FUN_XXXXXXXX  addr=0xXXXXXXXX  size=0xNNNN ===== */
# Also matches:  DECOMPILE_FAILED and EMPTY_OUTPUT variants.
FUNC_HEADER = re.compile(
    r"/\*\s*=====\s+(\S+)\s+"
    r"addr=0x([0-9a-fA-F]+)\s+"
    r"size=0x([0-9a-fA-F]+)"
    r"(?:\s+(DECOMPILE_FAILED|EMPTY_OUTPUT)[^=]*)?"
    r"\s*=====\s*\*/"
)


def parse_raw_output(filepath: Path) -> list[GhidraFunction]:
    """Parse the raw decompilation file into a list of GhidraFunction objects."""
    text = filepath.read_text(encoding="utf-8", errors="replace")

    functions = []
    # Split on function headers.
    parts = FUNC_HEADER.split(text)

    # parts[0] is the file header (before any function).
    # Then groups of 5: name, addr, size, fail_tag, code_until_next_header.
    i = 1
    while i + 3 < len(parts):
        name = parts[i]
        addr = int(parts[i + 1], 16)
        size = int(parts[i + 2], 16)
        fail_tag = parts[i + 3]  # None, "DECOMPILE_FAILED", or "EMPTY_OUTPUT"
        code = parts[i + 4] if i + 4 < len(parts) else ""
        code = code.strip()

        failed = fail_tag is not None
        functions.append(GhidraFunction(
            name=name,
            address=addr,
            size=size,
            code=code,
            failed=failed,
        ))
        i += 5

    return functions


def parse_link_order(filepath: Path) -> list[TranslationUnit]:
    """Parse link_order.txt into a list of TranslationUnit entries."""
    units = []
    if not filepath.exists():
        print(f"WARNING: link_order.txt not found at {filepath}", file=sys.stderr)
        return units

    for line in filepath.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        # Format: src_path  start_addr  end_addr  func_count  # notes
        # Split on comment first.
        comment = ""
        if "#" in line:
            line_part, comment = line.split("#", 1)
            comment = comment.strip()
        else:
            line_part = line

        tokens = line_part.split()
        if len(tokens) < 4:
            continue

        src_path = tokens[0]
        try:
            start_addr = int(tokens[1], 16)
            end_addr = int(tokens[2], 16)
        except ValueError:
            continue

        func_count = tokens[3]
        units.append(TranslationUnit(
            src_path=src_path,
            start_addr=start_addr,
            end_addr=end_addr,
            func_count=func_count,
            notes=comment,
        ))

    return units


# ============================================================================
# Cleanup
# ============================================================================


def apply_type_replacements(code: str) -> str:
    """Replace Ghidra types with project types."""
    for pattern, replacement in TYPE_REGEX:
        code = pattern.sub(replacement, code)
    return code


def apply_param_renaming(code: str) -> str:
    """Rename param_N -> rN / fParam_N -> fN."""
    code = PARAM_PATTERN.sub(lambda m: ALL_PARAMS[m.group(1)], code)
    return code


def apply_register_cleanup(code: str) -> str:
    """Clean up in_rN -> rN register references."""
    code = IN_REGISTER_PATTERN.sub(r"\1", code)
    code = IN_FREGISTER_PATTERN.sub(r"\1", code)
    code = IN_CR_PATTERN.sub(r"\1", code)
    return code


def apply_double_cast_cleanup(code: str) -> str:
    """Remove redundant double casts like (u32)(u32)x."""
    return DOUBLE_CAST_PATTERN.sub(r"(\1)", code)


def clean_function(func: GhidraFunction) -> GhidraFunction:
    """Apply all cleanup passes to a function."""
    if func.failed or not func.code:
        return func

    code = func.code
    code = apply_type_replacements(code)
    code = apply_param_renaming(code)
    code = apply_register_cleanup(code)
    code = apply_double_cast_cleanup(code)

    return GhidraFunction(
        name=func.name,
        address=func.address,
        size=func.size,
        code=code,
        failed=func.failed,
    )


# ============================================================================
# Existing function detection
# ============================================================================


def find_existing_functions(src_dir: Path) -> set[str]:
    """Scan src/ directory for function names that are already decompiled.

    Looks for function definitions (not declarations) by matching lines
    that look like:  `type funcname(...)  {`
    Also collects fn_XXXXXXXX patterns from comments that indicate a function
    at a given address is already handled.
    """
    existing = set()

    if not src_dir.exists():
        return existing

    # Match function definitions: return_type FUNCNAME(
    func_def_pattern = re.compile(
        r"^(?:static\s+)?(?:inline\s+)?(?:void|u8|u16|u32|u64|s8|s16|s32|s64"
        r"|f32|f64|BOOL|int|char|long|short|unsigned|signed|struct\s+\w+|enum\s+\w+)"
        r"[\s\*]+(\w+)\s*\(",
        re.MULTILINE
    )
    # Match address-based function names (fn_XXXXXXXX or FUN_XXXXXXXX).
    addr_func_pattern = re.compile(r"\b(?:fn|FUN)_([0-9a-fA-F]{8})\b")

    for c_file in src_dir.rglob("*.c"):
        try:
            text = c_file.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue

        for m in func_def_pattern.finditer(text):
            existing.add(m.group(1))

        # Also record addresses of functions we have.
        for m in addr_func_pattern.finditer(text):
            existing.add(f"FUN_{m.group(1)}")
            existing.add(f"fn_{m.group(1)}")

    return existing


# ============================================================================
# Grouping functions into source files
# ============================================================================


def assign_functions_to_tus(
    functions: list[GhidraFunction],
    tus: list[TranslationUnit],
) -> dict[str, list[GhidraFunction]]:
    """Assign each function to a translation unit based on address range.

    Functions that do not fall within any known TU are placed into a
    special "__unassigned__.c" bucket.
    """
    # Sort TUs by start address for binary search.
    sorted_tus = sorted(tus, key=lambda t: t.start_addr)
    result: dict[str, list[GhidraFunction]] = {}

    # Fallback resolver: the richer splits_refined.txt map (78.8% coverage
    # vs link_order's partial set). Keeps __unassigned__ only for true GAPs.
    try:
        from gen_func_tu_map import tu_for_address
    except Exception:
        def tu_for_address(_a):
            return None

    for func in functions:
        assigned = False
        for tu in sorted_tus:
            if tu.start_addr <= func.address < tu.end_addr:
                result.setdefault(tu.src_path, []).append(func)
                assigned = True
                break
        if not assigned:
            src = tu_for_address(func.address)
            result.setdefault(src or "__unassigned__", []).append(func)

    # Sort functions within each TU by address.
    for path in result:
        result[path].sort(key=lambda f: f.address)

    return result


# ============================================================================
# Output
# ============================================================================


def write_output_file(
    src_path: str,
    functions: list[GhidraFunction],
    output_base: Path,
):
    """Write cleaned-up functions to a staged output file."""
    if src_path == "__unassigned__":
        out_path = output_base / "__unassigned__.c"
    else:
        out_path = output_base / src_path

    out_path.parent.mkdir(parents=True, exist_ok=True)

    with open(out_path, "w", encoding="utf-8") as f:
        f.write("/**\n")
        f.write(f" * @file {Path(src_path).name}\n")
        f.write(f" * @brief Auto-generated Ghidra decompilation (cleaned up).\n")
        f.write(f" *\n")
        if functions:
            f.write(f" * Address range: 0x{functions[0].address:08X}"
                    f" - 0x{functions[-1].address + functions[-1].size:08X}\n")
            f.write(f" * Function count: {len(functions)}\n")
        f.write(f" *\n")
        f.write(f" * WARNING: This is auto-generated stub code from Ghidra.\n")
        f.write(f" * It requires manual review and cleanup before use.\n")
        f.write(f" */\n\n")
        f.write('#include "dolphin/types.h"\n\n')

        for func in functions:
            f.write(f"/* ---- {func.name}  "
                    f"addr=0x{func.address:08X}  "
                    f"size=0x{func.size:X} ---- */\n")
            if func.failed:
                f.write(f"/* DECOMPILATION FAILED */\n")
                f.write(f"// void {func.name}(void) {{ /* TODO */ }}\n")
            else:
                f.write(func.code)
                if not func.code.endswith("\n"):
                    f.write("\n")
            f.write("\n")

    return out_path


# ============================================================================
# Main
# ============================================================================


def main():
    parser = argparse.ArgumentParser(
        description="Clean up raw Ghidra decompilation output."
    )
    parser.add_argument(
        "--input", "-i",
        type=Path,
        default=DEFAULT_INPUT,
        help="Path to raw_decompilation.c from Ghidra.",
    )
    parser.add_argument(
        "--output", "-o",
        type=Path,
        default=OUTPUT_BASE,
        help="Base directory for cleaned output files.",
    )
    parser.add_argument(
        "--skip-existing",
        action="store_true",
        default=True,
        help="Skip functions already decompiled in src/ (default).",
    )
    parser.add_argument(
        "--no-skip-existing",
        action="store_true",
        help="Include all functions, even those already in src/.",
    )
    args = parser.parse_args()

    skip_existing = not args.no_skip_existing

    if not args.input.exists():
        print(f"ERROR: Input file not found: {args.input}", file=sys.stderr)
        print("Run 'python tools/ghidra_batch_decompile.py' first.", file=sys.stderr)
        sys.exit(1)

    # 1. Parse raw Ghidra output.
    print(f"Parsing {args.input} ...")
    functions = parse_raw_output(args.input)
    print(f"  Found {len(functions)} functions "
          f"({sum(1 for f in functions if not f.failed)} decompiled, "
          f"{sum(1 for f in functions if f.failed)} failed)")

    # Ghidra exported DOL *file offsets*, not runtime VAs (it didn't use
    # the GameCube loader). Translate off->VA and rename FUN_<off> ->
    # fn_<VA> so TU assignment / skip / import see real addresses.
    try:
        from dol_addr import normalize_addresses
        before = len(functions)
        functions = normalize_addresses(functions)
        print(f"  Normalized DOL offset->VA: {len(functions)} mapped "
              f"({before - len(functions)} outside any section, dropped)")
    except Exception as e:
        print(f"  WARNING: DOL address normalization skipped: {e}")

    # 2. Clean up each function.
    print("Applying cleanup passes ...")
    functions = [clean_function(f) for f in functions]

    # 3. Optionally skip existing functions.
    if skip_existing:
        print("Scanning src/ for already-decompiled functions ...")
        existing = find_existing_functions(SRC_DIR)
        print(f"  Found {len(existing)} existing function names/addresses")

        before = len(functions)
        functions = [
            f for f in functions
            if f.name not in existing
            and f"fn_{f.address:08x}" not in existing
            and f"fn_{f.address:08X}" not in existing
            and f"FUN_{f.address:08x}" not in existing
            and f"FUN_{f.address:08X}" not in existing
        ]
        skipped = before - len(functions)
        print(f"  Skipped {skipped} already-decompiled functions")
        print(f"  Remaining: {len(functions)} functions to process")

    # 4. Parse link_order.txt and group functions.
    print(f"Loading translation unit map from {LINK_ORDER} ...")
    tus = parse_link_order(LINK_ORDER)
    print(f"  Found {len(tus)} translation units")

    grouped = assign_functions_to_tus(functions, tus)
    print(f"  Functions assigned to {len(grouped)} output files")

    # 5. Write output files.
    print(f"\nWriting cleaned output to {args.output}/ ...")
    total_written = 0
    files_written = []
    for src_path in sorted(grouped.keys()):
        funcs = grouped[src_path]
        if not funcs:
            continue
        out = write_output_file(src_path, funcs, args.output)
        files_written.append((src_path, len(funcs)))
        total_written += len(funcs)

    # 6. Summary.
    print()
    print("=" * 72)
    print("Processing complete.")
    print(f"  Total functions written: {total_written}")
    print(f"  Output files created:    {len(files_written)}")
    print()
    print("Output files:")
    for src_path, count in files_written:
        print(f"  {count:4d} functions -> {args.output / src_path}")
    print()
    print(f"Next step: python tools/ghidra_import.py")
    print("=" * 72)


if __name__ == "__main__":
    main()
