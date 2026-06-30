#!/usr/bin/env python3
"""
compile_check.py - Compile a decompiled source file and compare against the original.

Compiles the given .c file with mwcceppc using the appropriate flags for
Pokemon Colosseum (GCN), then optionally runs objdiff-cli to compare the
result against the original object file.

Usage:
    python tools/compile_check.py src/game/main.c
    python tools/compile_check.py src/crt/__init_cpp_exceptions.c --symbol main
    python tools/compile_check.py src/game/main.c --compiler-version 1.2.5n
    python tools/compile_check.py src/game/main.c --diff
    python tools/compile_check.py --all

The output .o file is placed at build/GC6E01/base/<relative_path>.o
where <relative_path> mirrors the source file path under src/.
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

from headless_subprocess import run as run_headless

# ============================================================================
# Project layout
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = PROJECT_ROOT / "build" / "GC6E01"
BASE_DIR = BUILD_DIR / "base"
OBJ_DIR = BUILD_DIR / "obj"
SRC_DIR = PROJECT_ROOT / "src"
INCLUDE_DIR = PROJECT_ROOT / "include"
TOOLS_DIR = PROJECT_ROOT / "tools"

# Toolchain
BUILD_ROOT = PROJECT_ROOT / "build"
MWCC_BASE = TOOLS_DIR / "mwcc_compiler"          # Windows: compilers vendored here
MWCC_DEFAULT = MWCC_BASE / "mwcceppc.exe"
MWCC_GC_DIR = MWCC_BASE / "GC"
BUILD_COMPILERS_GC = BUILD_ROOT / "compilers" / "GC"  # configure.py download location
_EXE = ".exe" if os.name == "nt" else ""


def _first_existing(cands):
    for c in cands:
        if c and Path(c).exists():
            return Path(c)
    return None


# objdiff-cli: vendored tools/ (Windows) or downloaded build/tools/ (mac/linux).
OBJDIFF_CLI = _first_existing([
    TOOLS_DIR / f"objdiff-cli{_EXE}",
    BUILD_ROOT / "tools" / f"objdiff-cli{_EXE}",
]) or (TOOLS_DIR / f"objdiff-cli{_EXE}")
OBJDIFF_JSON = PROJECT_ROOT / "objdiff.json"


def _resolve_wibo():
    """wibo runs the Windows mwcceppc.exe PE on macOS/Linux. None on Windows."""
    if os.name == "nt":
        return None
    found = _first_existing([
        BUILD_ROOT / "tools" / "wibo",            # configure.py download
        Path.home() / ".local" / "bin" / "wibo-macos",
        Path.home() / ".local" / "bin" / "wibo",
    ])
    return found or (Path(shutil.which("wibo")) if shutil.which("wibo") else None)


WIBO = _resolve_wibo()


def run_tool(cmd, **kwargs):
    """Run a tool subprocess without opening extra windows on Windows."""
    return run_headless(cmd, **kwargs)

# ============================================================================
# Compile configuration (loaded from compile_config.json)
# ============================================================================

COMPILE_CONFIG_PATH = PROJECT_ROOT / "config" / "GC6E01" / "compile_config.json"

# Default compiler version (GC subdir name).
# CW comment version 8 -> one of: 1.0, 1.1, 1.1p1, 1.2.5, 1.2.5n
# 1.2.5n is the most common for early GCN titles.
DEFAULT_COMPILER_VERSION = "1.2.5n"


def _load_compile_config() -> dict:
    """Load compile_config.json and return the parsed config.

    Returns a dict with keys:
        - "default": {"compiler": "GC/X.Y.Z", "flags": "..."}
        - "overrides": {"src/crt/mem.c": {"compiler": "GC/X.Y.Z", ...}, ...}

    Falls back to hardcoded defaults if the config file is not found.
    """
    if COMPILE_CONFIG_PATH.exists():
        try:
            with open(COMPILE_CONFIG_PATH, "r") as f:
                cfg = json.load(f)
            return cfg
        except (json.JSONDecodeError, OSError) as e:
            print(f"WARNING: Failed to load {COMPILE_CONFIG_PATH}: {e}")
            print("  Falling back to hardcoded defaults.")

    # Fallback: hardcoded defaults matching the original behaviour
    return {
        "default": {
            "compiler": "GC/1.2.5n",
            "flags": "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off",
        },
        "overrides": {},
    }


# Load the config once at import time.
_COMPILE_CONFIG = _load_compile_config()


def _parse_flags_string(flags_str: str) -> list:
    """Parse a flags string like '-O4,p -proc gekko' into a list of tokens."""
    # shlex-like splitting that respects comma-separated values
    tokens = []
    for part in flags_str.split():
        tokens.append(part)
    return tokens


def get_default_cflags() -> list:
    """Get the default compiler flags from compile_config.json."""
    flags_str = _COMPILE_CONFIG.get("default", {}).get(
        "flags",
        "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off",
    )
    flags = _parse_flags_string(flags_str)
    # Always add the include path
    flags.extend(["-i", str(INCLUDE_DIR)])
    return flags


# Legacy aliases used elsewhere
DEFAULT_CFLAGS = get_default_cflags()

# Per-module flag overrides. Some TUs were compiled with different settings.
# Keys are relative source paths (e.g., "crt/__init_cpp_exceptions.c").
MODULE_FLAGS = {
    # CRT modules may use slightly different optimization
    # Add overrides here as you discover them during matching.
}


def _compiler_path_to_version(compiler_str: str) -> str:
    """Extract the version portion from a compiler path like 'GC/1.3' -> '1.3'."""
    if "/" in compiler_str:
        return compiler_str.split("/", 1)[1]
    return compiler_str


def get_file_compiler_version(src_path: Path) -> str:
    """Look up the compiler version for a source file from compile_config.json.

    Checks the 'overrides' dict using the source path relative to the project
    root (e.g., 'src/crt/mem.c'). Falls back to the default compiler version.

    Returns:
        The version string (e.g., '1.3' or '1.2.5n').
    """
    try:
        rel = str(src_path.resolve().relative_to(PROJECT_ROOT)).replace("\\", "/")
    except ValueError:
        rel = src_path.name

    overrides = _COMPILE_CONFIG.get("overrides", {})

    # Try full path relative to project root (e.g., "src/crt/mem.c")
    if rel in overrides:
        compiler_str = overrides[rel].get("compiler")
        if compiler_str:
            return _compiler_path_to_version(compiler_str)

    # Try path relative to src/ (e.g., "crt/mem.c") for legacy compat
    try:
        rel_src = str(src_path.resolve().relative_to(SRC_DIR)).replace("\\", "/")
    except ValueError:
        rel_src = src_path.name

    if rel_src in overrides:
        compiler_str = overrides[rel_src].get("compiler")
        if compiler_str:
            return _compiler_path_to_version(compiler_str)

    # Default
    default_compiler = _COMPILE_CONFIG.get("default", {}).get("compiler", "GC/1.2.5n")
    return _compiler_path_to_version(default_compiler)


def get_file_cflags(src_path: Path) -> list:
    """Look up compiler flags for a source file from compile_config.json.

    Checks the 'overrides' dict for per-file flag overrides.
    Falls back to the default flags.

    Returns:
        List of flag tokens.
    """
    try:
        rel = str(src_path.resolve().relative_to(PROJECT_ROOT)).replace("\\", "/")
    except ValueError:
        rel = src_path.name

    overrides = _COMPILE_CONFIG.get("overrides", {})

    # Check for per-file flag override
    for key in [rel]:
        if key in overrides and "flags" in overrides[key]:
            flags = _parse_flags_string(overrides[key]["flags"])
            flags.extend(["-i", str(INCLUDE_DIR)])
            return flags

    # Also try path relative to src/
    try:
        rel_src = str(src_path.resolve().relative_to(SRC_DIR)).replace("\\", "/")
    except ValueError:
        rel_src = src_path.name

    if rel_src in overrides and "flags" in overrides[rel_src]:
        flags = _parse_flags_string(overrides[rel_src]["flags"])
        flags.extend(["-i", str(INCLUDE_DIR)])
        return flags

    return get_default_cflags()

# ============================================================================
# Source -> object mapping
# ============================================================================

def source_to_base_obj(src_path: Path) -> Path:
    """Convert a source file path to the base (recompiled) .o path.

    src/game/main.c -> build/GC6E01/base/game/main.o
    """
    try:
        rel = src_path.relative_to(SRC_DIR)
    except ValueError:
        # If the path isn't under src/, use just the filename
        rel = Path(src_path.stem)
    return BASE_DIR / rel.with_suffix(".o")


def find_target_obj(src_path: Path) -> Path:
    """Find the original (target) .o file that should contain code from src_path.

    For __init_cpp_exceptions.c, the target is __init_cpp_exceptions.o.
    For most other files, the code is in the monolithic auto_01_800055E0_text.o
    since dtk hasn't split individual TUs yet.
    """
    stem = src_path.stem
    rel = None
    try:
        rel = src_path.relative_to(SRC_DIR)
    except ValueError:
        pass

    # Check if there's a dedicated object file matching this source
    for obj_file in OBJ_DIR.glob("*.o"):
        if stem in obj_file.stem and "auto_" not in obj_file.stem:
            return obj_file

    # Check objdiff.json for explicit mapping
    if OBJDIFF_JSON.exists():
        try:
            with open(OBJDIFF_JSON) as f:
                cfg = json.load(f)
            rel_str = str(rel.with_suffix("")) if rel else stem
            for unit in cfg.get("units", []):
                unit_name = unit.get("name", "")
                if unit_name == rel_str or unit_name.endswith("/" + stem):
                    target = unit.get("target_path")
                    if target:
                        candidate = PROJECT_ROOT / target
                        if candidate.exists():
                            return candidate
        except (json.JSONDecodeError, KeyError):
            pass

    # Default: the monolithic text object
    return OBJ_DIR / "auto_01_800055E0_text.o"


# ============================================================================
# Compilation
# ============================================================================

def get_compiler(version: str = None) -> Path:
    """Get the path to mwcceppc.exe for the given compiler version."""
    if version is None:
        version = DEFAULT_COMPILER_VERSION

    # Vendored (Windows) first, then the configure.py download (mac/linux), then default.
    found = _first_existing([
        MWCC_GC_DIR / version / "mwcceppc.exe",
        BUILD_COMPILERS_GC / version / "mwcceppc.exe",
        MWCC_DEFAULT,
    ])
    if found:
        return found

    print(f"ERROR: No mwcceppc.exe found for version {version}")
    print(f"  Checked: {MWCC_GC_DIR / version / 'mwcceppc.exe'}")
    print(f"  Checked: {BUILD_COMPILERS_GC / version / 'mwcceppc.exe'}")
    print(f"  Checked: {MWCC_DEFAULT}")
    sys.exit(1)


def get_cflags(src_path: Path) -> list:
    """Get compiler flags for the given source file.

    Checks compile_config.json overrides first, then legacy MODULE_FLAGS,
    then falls back to the default flags.
    """
    try:
        rel = str(src_path.relative_to(SRC_DIR))
    except ValueError:
        rel = src_path.name

    # Normalize to forward slashes for lookup
    rel = rel.replace("\\", "/")

    # Legacy per-module override (for any flags set directly in code)
    if rel in MODULE_FLAGS:
        return MODULE_FLAGS[rel]

    # Config-driven flags (checks compile_config.json)
    return get_file_cflags(src_path)


def compile_source(src_path: Path, compiler_version: str = None,
                   extra_flags: list = None, verbose: bool = False) -> Path:
    """Compile a single source file and return the output .o path.

    Returns:
        Path to the compiled .o file.
    Raises:
        SystemExit on compilation failure.
    """
    src_path = Path(src_path).resolve()
    if not src_path.exists():
        print(f"ERROR: Source file not found: {src_path}")
        sys.exit(1)

    out_obj = source_to_base_obj(src_path)
    out_obj.parent.mkdir(parents=True, exist_ok=True)

    # Check per-file compiler version override from compile_config.json
    # (only if not explicitly set via --compiler-version CLI flag)
    if compiler_version is None:
        compiler_version = get_file_compiler_version(src_path)

    compiler = get_compiler(compiler_version)
    cflags = get_cflags(src_path)

    if extra_flags:
        cflags.extend(extra_flags)

    cmd = [str(compiler), "-c", "-o", str(out_obj)] + cflags + [str(src_path)]
    # mwcceppc.exe is a Windows PE; run it through wibo on macOS/Linux.
    if WIBO is not None:
        cmd = [str(WIBO)] + cmd

    if verbose:
        print(f"  Compiler: {compiler}")
        if WIBO is not None:
            print(f"  Wrapper:  {WIBO}")
        print(f"  Command:  {' '.join(cmd)}")
        print(f"  Output:   {out_obj}")

    result = run_tool(cmd, capture_output=True, text=True, cwd=str(PROJECT_ROOT))

    if result.returncode != 0:
        print(f"COMPILE FAILED (exit code {result.returncode})")
        if result.stdout.strip():
            print("--- stdout ---")
            print(result.stdout)
        if result.stderr.strip():
            print("--- stderr ---")
            print(result.stderr)
        sys.exit(1)

    if result.stderr.strip() and verbose:
        print("--- compiler warnings ---")
        print(result.stderr)

    if not out_obj.exists():
        print(f"ERROR: Compilation succeeded but output not found at {out_obj}")
        sys.exit(1)

    print(f"OK: {out_obj.relative_to(PROJECT_ROOT)}")
    return out_obj


# ============================================================================
# Diffing with objdiff-cli
# ============================================================================


OBJDIFF_CONFIG = "ppc.calculatePoolRelocations=false"


def _run_objdiff_json(target_obj: Path, base_obj: Path, symbol: str = None,
                      timeout: int = 120) -> dict:
    """Run objdiff in non-interactive JSON mode."""
    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj),
        "-2", str(base_obj),
        "-o", "-",
        "--format", "json",
        "-c", OBJDIFF_CONFIG,
    ]
    if symbol:
        cmd.append(symbol)

    result = run_tool(
        cmd, capture_output=True, text=True, cwd=str(PROJECT_ROOT),
        timeout=timeout,
    )
    if result.returncode != 0:
        print(f"objdiff error: {result.stderr}")
        return None

    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return None


def _summarize_symbol(diff_json: dict, symbol: str) -> dict:
    """Extract one symbol's match summary from objdiff JSON."""
    result = {
        "match_percent": 0.0,
        "total_instructions": 0,
        "matched_instructions": 0,
        "mismatches": 0,
        "symbol_found": False,
    }
    if not diff_json:
        return result

    for entry in diff_json.get("right", {}).get("symbols", []):
        if entry.get("name") != symbol:
            continue
        if entry.get("kind") not in (None, "SYMBOL_FUNCTION"):
            continue

        result["symbol_found"] = True
        instructions = entry.get("instructions", [])
        if instructions:
            matched = 0
            for instr in instructions:
                diff_kind = instr.get("diff_kind")
                if diff_kind is None or diff_kind == "DIFF_NONE":
                    matched += 1
            total = len(instructions)
            result["total_instructions"] = total
            result["matched_instructions"] = matched
            result["mismatches"] = total - matched
            result["match_percent"] = (
                100.0 * matched / total if total else 0.0
            )
        else:
            result["match_percent"] = entry.get("match_percent", 0.0)
        return result

    return result


def _list_function_symbols(diff_json: dict) -> list:
    """Return function symbols present on the compiled/right side."""
    out = []
    if not diff_json:
        return out
    for entry in diff_json.get("right", {}).get("symbols", []):
        name = entry.get("name", "")
        kind = entry.get("kind")
        if not name or name.startswith("[") or kind == "SYMBOL_SECTION":
            continue
        if kind and kind != "SYMBOL_FUNCTION":
            continue
        if entry.get("instructions"):
            out.append(name)
    return out


def _active_source_lines(src_path: Path) -> list:
    """Return source lines active under the local #if 0/#if 1 wrapper style."""
    lines = src_path.read_text(errors="ignore").splitlines()
    active_stack = [True]
    frames = []
    active_lines = []

    for line_no, line in enumerate(lines, start=1):
        stripped = line.strip()
        if stripped.startswith("#if "):
            expr = stripped[3:].strip()
            parent_active = active_stack[-1]
            if expr == "0":
                cond_active = False
            elif expr == "1":
                cond_active = True
            else:
                cond_active = parent_active
            frames.append((parent_active, cond_active))
            active_stack.append(parent_active and cond_active)
            continue
        if stripped == "#else" and frames:
            parent_active, cond_active = frames[-1]
            active_stack[-1] = parent_active and not cond_active
            continue
        if stripped == "#endif" and frames:
            frames.pop()
            active_stack.pop()
            continue
        if active_stack[-1]:
            active_lines.append((line_no, line))

    return active_lines


def _active_asm_symbols(src_path: Path, symbols: list) -> list:
    """Return requested symbols whose active definition is still an asm wrapper."""
    wanted = set(symbols or [])
    if not wanted:
        return []

    patterns = {
        symbol: re.compile(rf"^\s*asm\b.*\b{re.escape(symbol)}\s*\(")
        for symbol in wanted
    }
    active_asm = []
    for _, line in _active_source_lines(src_path):
        for symbol, pattern in patterns.items():
            if pattern.search(line):
                active_asm.append(symbol)
    return sorted(set(active_asm))


def _active_c_symbols(src_path: Path, symbols: list) -> list:
    """Return requested symbols that have an active non-asm function body."""
    wanted = set(symbols or [])
    if not wanted:
        return []

    active_lines = _active_source_lines(src_path)
    found = set()
    for idx, (_, line) in enumerate(active_lines):
        stripped = line.strip()
        if (not stripped or stripped.startswith("extern ")
                or stripped.startswith("asm ") or stripped.startswith("typedef ")):
            continue

        for symbol in wanted:
            if symbol in found or not re.search(rf"\b{re.escape(symbol)}\s*\(", stripped):
                continue
            if ";" in stripped:
                continue
            if "{" in stripped:
                found.add(symbol)
                continue

            for _, next_line in active_lines[idx + 1:]:
                next_stripped = next_line.strip()
                if not next_stripped:
                    continue
                if next_stripped.startswith("#pragma"):
                    continue
                if next_stripped.startswith("{"):
                    found.add(symbol)
                break

    return sorted(found)


def run_diff_interactive(src_path: Path, symbol: str = None,
                         compiler_version: str = None,
                         verbose: bool = False):
    """Compile a source file and open objdiff's interactive TUI."""
    if os.environ.get("PKMN_ALLOW_INTERACTIVE_OBJD") != "1":
        print(
            "ERROR: interactive objdiff is disabled for automation. "
            "Set PKMN_ALLOW_INTERACTIVE_OBJD=1 to open the TUI explicitly."
        )
        return 1
    if os.name == "nt" and os.environ.get("PKMN_ALLOW_WINDOW_POPUPS") != "1":
        print(
            "ERROR: interactive objdiff windows are disabled on Windows. "
            "Set PKMN_ALLOW_WINDOW_POPUPS=1 only when you explicitly want a popup."
        )
        return 1

    base_obj = compile_source(src_path, compiler_version=compiler_version,
                              verbose=verbose)
    target_obj = find_target_obj(src_path)

    if not target_obj.exists():
        print(f"WARNING: Target object not found: {target_obj}")
        print("  (Compile succeeded but cannot diff without original .o)")
        return

    if not OBJDIFF_CLI.exists():
        print(f"WARNING: objdiff-cli not found at {OBJDIFF_CLI}")
        print("  (Compile succeeded but cannot diff without objdiff-cli)")
        return

    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj),
        "-2", str(base_obj),
    ]
    if symbol:
        cmd.append(symbol)

    if verbose:
        print(f"\n  Diff command: {' '.join(cmd)}")

    print(f"\nDiffing against: {target_obj.relative_to(PROJECT_ROOT)}")

    # Run in interactive mode (no -o flag) so user gets the TUI
    result = subprocess.run(cmd, cwd=str(PROJECT_ROOT))
    return result.returncode


def run_diff_symbols(src_path: Path, symbols: list = None,
                     compiler_version: str = None, whole_file: bool = False,
                     verbose: bool = False, timeout: int = 120) -> dict:
    """Compile once, then measure one or more symbols non-interactively.

    By default this runs one objdiff JSON pass per requested symbol after a
    single compile. Use `whole_file=True` to run one full-object objdiff pass
    and parse all symbols from that JSON.
    """
    symbols = list(symbols or [])
    base_obj = compile_source(src_path, compiler_version=compiler_version,
                              verbose=verbose)
    target_obj = find_target_obj(src_path)

    result = {
        "source": str(src_path),
        "target_obj": str(target_obj),
        "base_obj": str(base_obj),
        "symbols": {},
        "active_asm_symbols": _active_asm_symbols(src_path, symbols),
        "active_c_symbols": _active_c_symbols(src_path, symbols),
        "error": None,
    }

    if not target_obj.exists():
        result["error"] = f"target object not found: {target_obj}"
        return result
    if not OBJDIFF_CLI.exists():
        result["error"] = f"objdiff-cli not found: {OBJDIFF_CLI}"
        return result

    if whole_file or not symbols:
        diff_json = _run_objdiff_json(target_obj, base_obj, timeout=timeout)
        if diff_json is None:
            result["error"] = "objdiff failed"
            return result
        if not symbols:
            symbols = _list_function_symbols(diff_json)
        for symbol in symbols:
            result["symbols"][symbol] = _summarize_symbol(diff_json, symbol)
        return result

    for symbol in symbols:
        diff_json = _run_objdiff_json(
            target_obj, base_obj, symbol=symbol, timeout=timeout,
        )
        if diff_json is None:
            result["symbols"][symbol] = {
                "match_percent": 0.0,
                "total_instructions": 0,
                "matched_instructions": 0,
                "mismatches": 0,
                "symbol_found": False,
                "error": "objdiff failed",
            }
            continue
        result["symbols"][symbol] = _summarize_symbol(diff_json, symbol)

    return result


def print_diff_summary(result: dict) -> None:
    """Print a compact non-interactive objdiff summary."""
    if result.get("error"):
        print(f"ERROR: {result['error']}")
        return
    print(f"Source: {Path(result['source']).relative_to(PROJECT_ROOT)}")
    print(f"Base:   {Path(result['base_obj']).relative_to(PROJECT_ROOT)}")
    print(f"Target: {Path(result['target_obj']).relative_to(PROJECT_ROOT)}")
    print()
    print(f"{'Function':<35} {'Match':>9} {'Instr':>11} {'Mismatches':>11}")
    print(f"{'-'*35} {'-'*9} {'-'*11} {'-'*11}")
    for symbol, info in result["symbols"].items():
        if not info.get("symbol_found"):
            match = "NO-MATCH"
            instr = "-"
            mismatches = "-"
        else:
            match = f"{info['match_percent']:.4f}%"
            instr = (f"{info['matched_instructions']}/"
                     f"{info['total_instructions']}")
            mismatches = str(info["mismatches"])
        print(f"{symbol:<35} {match:>9} {instr:>11} {mismatches:>11}")


def run_diff(src_path: Path, symbol: str = None, compiler_version: str = None,
             verbose: bool = False):
    """Compile a source file and print a non-interactive objdiff summary."""
    symbols = [symbol] if symbol else []
    result = run_diff_symbols(src_path, symbols=symbols,
                              compiler_version=compiler_version,
                              whole_file=not symbols, verbose=verbose)
    print_diff_summary(result)
    return 1 if result.get("error") else 0


def run_diff_json(src_path: Path, symbol: str = None,
                  compiler_version: str = None) -> dict:
    """Compile and diff, returning JSON results for programmatic use."""
    base_obj = compile_source(src_path, compiler_version=compiler_version)
    target_obj = find_target_obj(src_path)

    if not target_obj.exists() or not OBJDIFF_CLI.exists():
        return None

    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj),
        "-2", str(base_obj),
        "-o", "-",
        "--format", "json",
        "-c", OBJDIFF_CONFIG,
    ]
    if symbol:
        cmd.append(symbol)

    result = run_tool(cmd, capture_output=True, text=True,
                      cwd=str(PROJECT_ROOT))
    if result.returncode != 0:
        print(f"objdiff error: {result.stderr}")
        return None

    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return None


# ============================================================================
# Compile all source files
# ============================================================================

def compile_all(compiler_version: str = None, verbose: bool = False):
    """Compile every .c/.cpp file under src/."""
    # pcport/* is host PC-port code (built by the separate PCPORT CMake build,
    # not the GameCube/CodeWarrior matching toolchain) — exclude it from the gate.
    src_files = [p for p in (sorted(SRC_DIR.rglob("*.c")) + sorted(SRC_DIR.rglob("*.cpp")))
                 if "pcport" not in p.parts]

    if not src_files:
        print("No source files found under src/")
        return

    print(f"Compiling {len(src_files)} source files...")
    print(f"  Compiler version: {compiler_version or DEFAULT_COMPILER_VERSION}")
    print()

    ok = 0
    fail = 0

    for src in src_files:
        rel = src.relative_to(SRC_DIR)
        try:
            compile_source(src, compiler_version=compiler_version, verbose=verbose)
            ok += 1
        except SystemExit:
            print(f"FAIL: {rel}")
            fail += 1

    print(f"\n{'='*50}")
    print(f"Results: {ok} succeeded, {fail} failed out of {len(src_files)} total")


# ============================================================================
# Entry point
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Compile decompiled source files for Pokemon Colosseum "
                    "and compare against originals.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python tools/compile_check.py src/game/main.c
  python tools/compile_check.py src/game/main.c --diff
  python tools/compile_check.py src/game/main.c --diff --symbol main
  python tools/compile_check.py src/game/main.c --diff --symbols fn_A fn_B
  python tools/compile_check.py src/game/main.c --diff --interactive
  python tools/compile_check.py src/game/main.c --compiler-version 1.1
  python tools/compile_check.py --all
        """,
    )

    parser.add_argument(
        "source", nargs="?",
        help="Source file to compile (e.g., src/game/main.c)"
    )
    parser.add_argument(
        "--all", action="store_true",
        help="Compile all source files under src/"
    )
    parser.add_argument(
        "--diff", action="store_true",
        help="After compiling, print a non-interactive objdiff summary"
    )
    parser.add_argument(
        "--interactive", action="store_true",
        help="With --diff, open objdiff-cli's interactive TUI"
    )
    parser.add_argument(
        "--symbol", "-s",
        help="Specific function symbol to diff"
    )
    parser.add_argument(
        "--symbols", nargs="+",
        help="Specific function symbols to diff after a single compile"
    )
    parser.add_argument(
        "--whole-file", action="store_true",
        help="With --diff, use one full-object objdiff JSON pass"
    )
    parser.add_argument(
        "--json", action="store_true",
        help="With --diff, print machine-readable JSON"
    )
    parser.add_argument(
        "--require-match", action="store_true",
        help="With --diff and symbols, exit nonzero unless all symbols are "
             "100%% active C matches"
    )
    parser.add_argument(
        "--timeout", type=int, default=120,
        help="Objdiff timeout in seconds for non-interactive --diff"
    )
    parser.add_argument(
        "--compiler-version", "-cv", default=None,
        help=f"MWCC GC compiler version (default: {DEFAULT_COMPILER_VERSION}). "
             f"Available: see tools/mwcc_compiler/GC/"
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Show detailed compilation commands"
    )

    args = parser.parse_args()

    if args.all:
        compile_all(compiler_version=args.compiler_version, verbose=args.verbose)
        return 0

    if not args.source:
        parser.print_help()
        print("\nERROR: Provide a source file or use --all")
        return 1

    src_path = Path(args.source)
    if not src_path.is_absolute():
        src_path = PROJECT_ROOT / src_path

    # Host PC-port code is built by the separate PCPORT CMake build, not the
    # GameCube/CodeWarrior matching toolchain — skip it so the CI compile gates
    # (which iterate changed src/**/*.c) don't fail on host-only sources.
    if "pcport" in src_path.parts:
        print(f"SKIP (host PCPORT code, not part of the matching build): {args.source}")
        return 0

    if args.diff:
        if args.interactive:
            return run_diff_interactive(
                src_path, symbol=args.symbol,
                compiler_version=args.compiler_version,
                verbose=args.verbose,
            )

        symbols = []
        if args.symbol:
            symbols.append(args.symbol)
        if args.symbols:
            symbols.extend(args.symbols)

        result = run_diff_symbols(
            src_path, symbols=symbols, compiler_version=args.compiler_version,
            whole_file=args.whole_file or not symbols, verbose=args.verbose,
            timeout=args.timeout,
        )
        if args.json:
            print(json.dumps(result, indent=2))
        else:
            print_diff_summary(result)

        if result.get("error"):
            return 1
        if args.require_match and symbols:
            active_asm_symbols = result.get("active_asm_symbols", [])
            if active_asm_symbols:
                joined = ", ".join(active_asm_symbols)
                print(
                    "ERROR: --require-match refuses active asm wrappers: "
                    f"{joined}"
                )
                return 1
            missing_active_c = sorted(
                set(symbols) - set(result.get("active_c_symbols", []))
            )
            if missing_active_c:
                joined = ", ".join(missing_active_c)
                print(
                    "ERROR: --require-match requires active C definitions: "
                    f"{joined}"
                )
                return 1
            for info in result["symbols"].values():
                if (not info.get("symbol_found")
                        or info.get("match_percent") != 100.0
                        or info.get("mismatches") != 0):
                    return 1
        return 0
    else:
        compile_source(src_path, compiler_version=args.compiler_version,
                       verbose=args.verbose)
        return 0


if __name__ == "__main__":
    sys.exit(main() or 0)
