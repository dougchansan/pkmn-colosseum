#!/usr/bin/env python3
"""
match_test.py - Test whether a decompiled function matches the original binary.

Looks up a function by name or address in the symbol table, determines which
source file contains it, compiles that file, then uses objdiff-cli to compare
the specific function and report the match percentage.

Usage:
    python tools/match_test.py main
    python tools/match_test.py 0x800055E0
    python tools/match_test.py SetPauseFlag --verbose
    python tools/match_test.py --list                    # List all known symbols
    python tools/match_test.py --list-sources             # List source -> symbol mappings
    python tools/match_test.py --scan src/game/main.c     # Test all functions in a file

The symbol table is read from config/GC6E01/symbols.txt.
Source file mappings come from objdiff.json and source file annotations.
"""

import os
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

# ============================================================================
# Project layout
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = PROJECT_ROOT / "build" / "GC6E01"
BASE_DIR = BUILD_DIR / "base"
OBJ_DIR = BUILD_DIR / "obj"
SRC_DIR = PROJECT_ROOT / "src"
CONFIG_DIR = PROJECT_ROOT / "config" / "GC6E01"
TOOLS_DIR = PROJECT_ROOT / "tools"
REPORTS_DIR = PROJECT_ROOT / "tools" / "decomp_work" / "reports"

SYMBOLS_TXT = CONFIG_DIR / "symbols.txt"
OBJDIFF_CLI = TOOLS_DIR / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
OBJDIFF_JSON = PROJECT_ROOT / "objdiff.json"

# Import compile_check for compilation
sys.path.insert(0, str(TOOLS_DIR))
from compile_check import (
    compile_source, source_to_base_obj, find_target_obj,
    run_diff_json, DEFAULT_COMPILER_VERSION, get_file_compiler_version,
    run_tool, PROJECT_ROOT as _
)

# ============================================================================
# Symbol table parsing
# ============================================================================

class Symbol:
    """A parsed symbol from symbols.txt."""
    def __init__(self, name, section, address, sym_type=None, size=None,
                 scope=None):
        self.name = name
        self.section = section
        self.address = address  # int
        self.type = sym_type
        self.size = size        # int or None
        self.scope = scope

    def __repr__(self):
        size_str = f" size:0x{self.size:X}" if self.size else ""
        return f"{self.name} = {self.section}:0x{self.address:08X}{size_str}"

    @property
    def is_function(self):
        return self.type == "function"

    @property
    def end_address(self):
        if self.size:
            return self.address + self.size
        return self.address


# Pattern: name = .section:0x80XXXXXX; // type:T size:0xNN scope:S
SYMBOL_RE = re.compile(
    r'^(\S+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+)\s*;'
    r'(?:\s*//\s*(.*))?$'
)


def parse_symbols(path: Path = None) -> list:
    """Parse symbols.txt into a list of Symbol objects."""
    if path is None:
        path = SYMBOLS_TXT

    if not path.exists():
        print(f"ERROR: Symbol file not found: {path}")
        sys.exit(1)

    symbols = []
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                continue

            m = SYMBOL_RE.match(line)
            if not m:
                continue

            name = m.group(1)
            section = m.group(2)
            address = int(m.group(3), 16)
            comment = m.group(4) or ""

            sym_type = None
            size = None
            scope = None

            for part in comment.split():
                if part.startswith("type:"):
                    sym_type = part.split(":", 1)[1]
                elif part.startswith("size:"):
                    try:
                        size = int(part.split(":", 1)[1], 0)
                    except ValueError:
                        pass
                elif part.startswith("scope:"):
                    scope = part.split(":", 1)[1]

            symbols.append(Symbol(name, section, address, sym_type, size, scope))

    return symbols


def find_symbol(query: str, symbols: list) -> Symbol:
    """Find a symbol by name or address.

    Accepts:
        - Exact function name: "main", "SetPauseFlag"
        - Hex address: "0x800055E0" or "800055E0"
        - Auto-generated name: "fn_800055E0"
    """
    # Try as hex address
    addr = None
    query_clean = query.strip()
    if query_clean.startswith("0x") or query_clean.startswith("0X"):
        try:
            addr = int(query_clean, 16)
        except ValueError:
            pass
    elif query_clean.startswith("fn_"):
        try:
            addr = int(query_clean[3:], 16)
        except ValueError:
            pass
    elif all(c in "0123456789abcdefABCDEF" for c in query_clean) and len(query_clean) == 8:
        try:
            addr = int(query_clean, 16)
        except ValueError:
            pass

    if addr is not None:
        for sym in symbols:
            if sym.address == addr:
                return sym
        # Also check if address falls within a function's range
        for sym in symbols:
            if sym.is_function and sym.size:
                if sym.address <= addr < sym.address + sym.size:
                    return sym
        print(f"ERROR: No symbol found at address 0x{addr:08X}")
        sys.exit(1)

    # Try exact name match
    for sym in symbols:
        if sym.name == query:
            return sym

    # Try case-insensitive match
    query_lower = query.lower()
    matches = [s for s in symbols if s.name.lower() == query_lower]
    if len(matches) == 1:
        return matches[0]

    # Try partial match
    matches = [s for s in symbols if query_lower in s.name.lower()]
    if len(matches) == 1:
        return matches[0]
    elif len(matches) > 1:
        print(f"Ambiguous symbol '{query}'. Matches:")
        for m in matches[:20]:
            print(f"  {m}")
        if len(matches) > 20:
            print(f"  ... and {len(matches) - 20} more")
        sys.exit(1)

    print(f"ERROR: Symbol '{query}' not found in {SYMBOLS_TXT.name}")
    sys.exit(1)


# ============================================================================
# Source file mapping
# ============================================================================

def find_source_for_address(address: int) -> Path:
    """Determine which source file contains code at the given address.

    Strategy:
    1. Check source file header comments for address ranges.
    2. Check objdiff.json unit metadata for source_path hints.
    3. Fall back to scanning source files for the function address in comments.
    """
    # Strategy 1: Scan source files for address range annotations
    # Many decompiled files have "Address range: 0xNNNNNNNN - 0xNNNNNNNN" in headers
    for src_file in sorted(SRC_DIR.rglob("*.c")):
        try:
            with open(src_file, "r", errors="replace") as f:
                header = f.read(4096)  # Read first 4KB for header comments
        except OSError:
            continue

        # Look for address mentions in the file
        addr_hex = f"0x{address:08X}"
        addr_hex_lower = f"0x{address:08x}"
        if addr_hex in header or addr_hex_lower in header:
            return src_file

        # Check for "Address: 0xNNNNNNNN" pattern
        range_match = re.search(
            r'Address range:\s*0x([0-9A-Fa-f]+)\s*-\s*0x([0-9A-Fa-f]+)',
            header
        )
        if range_match:
            range_start = int(range_match.group(1), 16)
            range_end = int(range_match.group(2), 16)
            if range_start <= address < range_end:
                return src_file

    # Strategy 2: Check objdiff.json
    if OBJDIFF_JSON.exists():
        try:
            with open(OBJDIFF_JSON) as f:
                cfg = json.load(f)
            for unit in cfg.get("units", []):
                src_path = unit.get("metadata", {}).get("source_path")
                if src_path:
                    full_path = PROJECT_ROOT / src_path
                    if full_path.exists():
                        try:
                            with open(full_path, "r", errors="replace") as f:
                                content = f.read()
                            addr_hex = f"0x{address:08X}"
                            if addr_hex in content or addr_hex.lower() in content:
                                return full_path
                        except OSError:
                            pass
        except (json.JSONDecodeError, KeyError):
            pass

    return None


def find_source_for_symbol(sym: Symbol) -> Path:
    """Find the source file that contains the given symbol."""
    # First try by address
    src = find_source_for_address(sym.address)
    if src:
        return src

    # Try by name - check if any source file defines this symbol
    for src_file in sorted(SRC_DIR.rglob("*.c")):
        try:
            with open(src_file, "r", errors="replace") as f:
                content = f.read()
        except OSError:
            continue

        # Look for function definition patterns
        if sym.name in content:
            # Simple heuristic: if the symbol name appears in the file,
            # it's probably defined there. Check for definition-like context.
            if re.search(rf'\b{re.escape(sym.name)}\s*\(', content):
                return src_file

    return None


# ============================================================================
# Match testing
# ============================================================================

def extract_match_info(diff_json: dict, symbol_name: str) -> dict:
    """Extract match percentage and details from objdiff JSON output.

    Returns a dict with:
        - match_percent: float (0.0 - 100.0)
        - total_instructions: int
        - matched_instructions: int
        - symbol_found: bool
    """
    result = {
        "match_percent": 0.0,
        "total_instructions": 0,
        "matched_instructions": 0,
        "symbol_found": False,
    }

    if not diff_json:
        return result

    # Look through sections for match percentages
    left = diff_json.get("left", {})
    right = diff_json.get("right", {})

    # Check sections for overall match
    for section in left.get("sections", []):
        mp = section.get("match_percent")
        if mp is not None:
            result["match_percent"] = max(result["match_percent"], mp)

    # Look for specific symbol match in left (target) symbols
    left_symbols = left.get("symbols", [])
    right_symbols = right.get("symbols", [])

    for lsym in left_symbols:
        name = lsym.get("name", "")
        if name == symbol_name or name.startswith(f"["):
            instructions = lsym.get("instructions", [])
            if instructions:
                result["symbol_found"] = True
                result["total_instructions"] = len(instructions)

                # Count matched instructions by checking the diff status.
                # In objdiff JSON output, instructions that match have
                # NO "diff_kind" key at all (or diff_kind == "DIFF_NONE").
                # Mismatches have diff_kind set to "DIFF_REPLACE",
                # "DIFF_INSERT", "DIFF_DELETE", etc.
                matched = 0
                for instr in instructions:
                    diff_kind = instr.get("diff_kind")
                    if diff_kind is None or diff_kind == "DIFF_NONE":
                        matched += 1
                result["matched_instructions"] = matched

                if result["total_instructions"] > 0:
                    result["match_percent"] = (
                        100.0 * result["matched_instructions"]
                        / result["total_instructions"]
                    )
                break

    return result


def test_function(symbol_query: str, compiler_version: str = None,
                  verbose: bool = False) -> dict:
    """Test a single function for matching.

    Returns a dict with match info, or None on failure.
    """
    symbols = parse_symbols()
    sym = find_symbol(symbol_query, symbols)

    print(f"Symbol: {sym}")
    if sym.is_function:
        print(f"  Type: function, Size: 0x{sym.size:X} ({sym.size} bytes)"
              if sym.size else f"  Type: function")
    else:
        print(f"  Type: {sym.type or 'unknown'}")

    # Find the source file
    src_file = find_source_for_symbol(sym)
    if not src_file:
        print(f"\n  No source file found for {sym.name}")
        print(f"  Address 0x{sym.address:08X} is not covered by any decompiled file.")
        print(f"  To test this function, create a source file containing it.")
        return None

    print(f"  Source: {src_file.relative_to(PROJECT_ROOT)}")

    # Show which compiler version will be used
    effective_version = compiler_version or get_file_compiler_version(src_file)
    print(f"  Compiler: GC/{effective_version}")

    # Compile the source file
    print()
    base_obj = compile_source(src_file, compiler_version=compiler_version,
                              verbose=verbose)
    target_obj = find_target_obj(src_file)

    if not target_obj.exists():
        print(f"\n  Target object not found: {target_obj}")
        return None

    if not OBJDIFF_CLI.exists():
        print(f"\n  objdiff-cli not found at {OBJDIFF_CLI}")
        print("  Cannot compute match percentage.")
        return None

    # Run objdiff in JSON mode for the specific symbol
    print(f"\nComparing {sym.name} ...")
    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj),
        "-2", str(base_obj),
        "-o", "-",
        "--format", "json",
        "-c", "ppc.calculatePoolRelocations=false",
        sym.name,
    ]

    if verbose:
        print(f"  Command: {' '.join(cmd)}")

    result = run_tool(cmd, capture_output=True, text=True,
                      cwd=str(PROJECT_ROOT))

    if result.returncode != 0:
        # objdiff may fail if symbol isn't in the base .o, which is expected
        # for functions not yet decompiled in that file
        if verbose:
            print(f"  objdiff stderr: {result.stderr.strip()}")

        # Try without specifying the symbol (whole-file diff)
        cmd_nosym = [
            str(OBJDIFF_CLI), "diff",
            "-1", str(target_obj),
            "-2", str(base_obj),
            "-o", "-",
            "--format", "json",
            "-c", "ppc.calculatePoolRelocations=false",
        ]
        result = run_tool(cmd_nosym, capture_output=True, text=True,
                          cwd=str(PROJECT_ROOT))

    diff_json = None
    if result.returncode == 0 and result.stdout.strip():
        try:
            diff_json = json.loads(result.stdout)
        except json.JSONDecodeError:
            if verbose:
                print(f"  Failed to parse objdiff JSON output")

    if diff_json:
        info = extract_match_info(diff_json, sym.name)

        print(f"\n{'='*50}")
        print(f"  Function:     {sym.name}")
        print(f"  Address:      0x{sym.address:08X}")
        if sym.size:
            print(f"  Size:         0x{sym.size:X} ({sym.size} bytes, "
                  f"~{sym.size // 4} instructions)")

        if info["symbol_found"]:
            pct = info["match_percent"]
            bar_len = 30
            filled = int(pct / 100 * bar_len)
            bar = "#" * filled + "-" * (bar_len - filled)
            print(f"  Match:        [{bar}] {pct:.1f}%")
            print(f"  Instructions: {info['matched_instructions']}"
                  f"/{info['total_instructions']} matched")

            if pct == 100.0:
                print(f"\n  *** MATCHING! ***")
            elif pct >= 90.0:
                print(f"\n  Very close! Check register allocation and instruction ordering.")
            elif pct >= 50.0:
                print(f"\n  Partial match. Structure looks right, details differ.")
            else:
                print(f"\n  Significant differences. Review the decompilation.")
        else:
            print(f"\n  Symbol '{sym.name}' not found in diff output.")
            print(f"  The function may not be present in the compiled base object.")
            print(f"  Check that the function is defined (not just declared) in the source.")

        return info
    else:
        print(f"\n  Could not produce diff output.")
        if verbose and result.stderr:
            print(f"  stderr: {result.stderr.strip()}")
        return None


def scan_source_file(src_path: Path, compiler_version: str = None,
                     verbose: bool = False, max_symbols: int = 40,
                     allow_long_scan: bool = False, report: str = None,
                     timeout: int = 60):
    """Compile a source file and test all functions defined in it."""
    src_path = Path(src_path).resolve()
    if not src_path.exists():
        print(f"ERROR: Source file not found: {src_path}")
        sys.exit(1)

    symbols = parse_symbols()

    # Find all symbols whose addresses appear in this source file
    try:
        with open(src_path, "r", errors="replace") as f:
            content = f.read()
    except OSError as e:
        print(f"ERROR: Cannot read {src_path}: {e}")
        sys.exit(1)

    # Extract addresses mentioned in the source file
    addr_pattern = re.compile(r'Address:\s*0x([0-9A-Fa-f]{8})')
    mentioned_addrs = set()
    for m in addr_pattern.finditer(content):
        mentioned_addrs.add(int(m.group(1), 16))

    # Also look for fn_XXXXXXXX patterns
    fn_pattern = re.compile(r'\bfn_([0-9A-Fa-f]{8})\b')
    for m in fn_pattern.finditer(content):
        addr = int(m.group(1), 16)
        mentioned_addrs.add(addr)

    # Find matching symbols
    file_symbols = []
    for sym in symbols:
        if sym.is_function and sym.address in mentioned_addrs:
            file_symbols.append(sym)

    # Also add symbols whose names appear as function definitions
    func_def_pattern = re.compile(r'^(?:\w[\w\s\*]+)\s+(\w+)\s*\([^)]*\)\s*\{',
                                  re.MULTILINE)
    defined_names = set()
    for m in func_def_pattern.finditer(content):
        defined_names.add(m.group(1))

    for sym in symbols:
        if sym.is_function and sym.name in defined_names:
            if sym not in file_symbols:
                file_symbols.append(sym)

    if not file_symbols:
        print(f"No function symbols found in {src_path.relative_to(PROJECT_ROOT)}")
        return

    # Sort by address
    file_symbols.sort(key=lambda s: s.address)

    effective_version = compiler_version or get_file_compiler_version(src_path)
    print(f"Source: {src_path.relative_to(PROJECT_ROOT)}")
    print(f"Compiler: GC/{effective_version}")
    print(f"Functions found: {len(file_symbols)}")
    if max_symbols and len(file_symbols) > max_symbols and not allow_long_scan:
        print(f"Scan capped at {max_symbols} functions to avoid long runs.")
        print("Pass --allow-long-scan to scan the entire file.")
        file_symbols = file_symbols[:max_symbols]
    print()

    # Compile once
    base_obj = compile_source(src_path, compiler_version=compiler_version,
                              verbose=verbose)
    target_obj = find_target_obj(src_path)

    if not target_obj.exists() or not OBJDIFF_CLI.exists():
        print("Cannot diff (missing target .o or objdiff-cli)")
        return

    # Test each function
    results = []
    for sym in file_symbols:
        info = {"name": sym.name, "address": sym.address, "size": sym.size}
        cmd = [
            str(OBJDIFF_CLI), "diff",
            "-1", str(target_obj),
            "-2", str(base_obj),
            "-o", "-",
            "--format", "json",
            "-c", "ppc.calculatePoolRelocations=false",
            sym.name,
        ]
        try:
            res = run_tool(
                cmd, capture_output=True, text=True,
                cwd=str(PROJECT_ROOT), timeout=timeout,
            )
        except subprocess.TimeoutExpired:
            info["match_percent"] = -1
            info["error"] = "objdiff_timeout"
            results.append(info)
            continue

        if res.returncode == 0 and res.stdout.strip():
            try:
                diff_json = json.loads(res.stdout)
                match = extract_match_info(diff_json, sym.name)
                info.update(match)
            except json.JSONDecodeError:
                info["match_percent"] = -1
        else:
            info["match_percent"] = -1

        results.append(info)

    # Print summary table
    print(f"\n{'='*70}")
    print(f"{'Function':<35} {'Address':<12} {'Size':<8} {'Match':>8}")
    print(f"{'-'*35} {'-'*12} {'-'*8} {'-'*8}")

    total_matched = 0
    total_tested = 0

    for r in results:
        name = r["name"][:34]
        addr = f"0x{r['address']:08X}"
        size = f"0x{r['size']:X}" if r.get("size") else "?"
        if r["match_percent"] < 0:
            match_str = "N/A"
        else:
            match_str = f"{r['match_percent']:.1f}%"
            total_tested += 1
            if r["match_percent"] == 100.0:
                total_matched += 1

        print(f"  {name:<34} {addr:<12} {size:<8} {match_str:>8}")

    print(f"\n  Total: {total_matched}/{total_tested} functions matching "
          f"({len(results)} in file)")

    if report:
        report_path = Path(report)
        if report == "AUTO":
            safe = "_".join(src_path.relative_to(PROJECT_ROOT).with_suffix("").parts)
            report_path = REPORTS_DIR / f"match_test_scan_{safe}.json"
        elif not report_path.is_absolute():
            report_path = PROJECT_ROOT / report_path
        report_path.parent.mkdir(parents=True, exist_ok=True)
        payload = {
            "source": str(src_path.relative_to(PROJECT_ROOT)).replace("\\", "/"),
            "compiler": f"GC/{effective_version}",
            "functions_found": len(file_symbols),
            "results": results,
        }
        report_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
        print(f"  Report: {report_path.relative_to(PROJECT_ROOT)}")


# ============================================================================
# List symbols
# ============================================================================

def list_symbols(filter_type: str = None, section: str = None):
    """List all symbols, optionally filtered."""
    symbols = parse_symbols()

    if filter_type:
        symbols = [s for s in symbols if s.type == filter_type]
    if section:
        symbols = [s for s in symbols if s.section == section]

    functions = [s for s in symbols if s.is_function]

    print(f"Total symbols: {len(parse_symbols())}")
    print(f"Functions: {len(functions)}")
    if filter_type or section:
        print(f"Filtered to: {len(symbols)}")
    print()

    for sym in symbols:
        scope_tag = f" [{sym.scope}]" if sym.scope else ""
        size_tag = f" size:0x{sym.size:X}" if sym.size else ""
        print(f"  0x{sym.address:08X}  {sym.section:<8}  {sym.name}{size_tag}{scope_tag}")


def list_source_mappings():
    """Show which source files cover which address ranges."""
    src_files = sorted(SRC_DIR.rglob("*.c"))

    for src_file in src_files:
        try:
            with open(src_file, "r", errors="replace") as f:
                header = f.read(2048)
        except OSError:
            continue

        # Extract address range
        range_match = re.search(
            r'Address range:\s*0x([0-9A-Fa-f]+)\s*-\s*0x([0-9A-Fa-f]+)',
            header
        )
        if range_match:
            start = int(range_match.group(1), 16)
            end = int(range_match.group(2), 16)
            rel = str(src_file.relative_to(PROJECT_ROOT))
            print(f"  {rel:<45}  0x{start:08X} - 0x{end:08X}")
        else:
            # Count function addresses mentioned
            addr_pattern = re.compile(r'Address:\s*0x([0-9A-Fa-f]{8})')
            addrs = [int(m.group(1), 16) for m in addr_pattern.finditer(header)]
            if addrs:
                rel = str(src_file.relative_to(PROJECT_ROOT))
                print(f"  {rel:<45}  {len(addrs)} functions "
                      f"(0x{min(addrs):08X} - 0x{max(addrs):08X})")


# ============================================================================
# Entry point
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Test function matching for Pokemon Colosseum decompilation.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python tools/match_test.py main                  # Test the main() function
  python tools/match_test.py 0x800055E0            # Test by address
  python tools/match_test.py fn_800056C4           # Test by auto-name
  python tools/match_test.py SetPauseFlag -v       # Verbose output
  python tools/match_test.py --scan src/game/main.c  # Test all functions in file
  python tools/match_test.py --scan src/game/main.c --report
  python tools/match_test.py --list                # List all symbols
  python tools/match_test.py --list --functions    # List only functions
  python tools/match_test.py --list-sources        # Show source -> address map
        """,
    )

    parser.add_argument(
        "symbol", nargs="?",
        help="Function name or address to test (e.g., 'main', '0x800055E0', "
             "'fn_800056C4')"
    )
    parser.add_argument(
        "--scan",
        help="Compile a source file and test all its functions"
    )
    parser.add_argument(
        "--max-scan-symbols", type=int, default=40,
        help="Default cap for --scan to avoid long runs (default: 40)"
    )
    parser.add_argument(
        "--allow-long-scan", action="store_true",
        help="Allow --scan to test more than --max-scan-symbols functions"
    )
    parser.add_argument(
        "--report", nargs="?", const="AUTO",
        help="With --scan, write a JSON report; omit value for default reports dir"
    )
    parser.add_argument(
        "--timeout", type=int, default=60,
        help="Per-symbol objdiff timeout in seconds"
    )
    parser.add_argument(
        "--list", action="store_true",
        help="List all symbols from the symbol table"
    )
    parser.add_argument(
        "--list-sources", action="store_true",
        help="Show source file to address range mappings"
    )
    parser.add_argument(
        "--functions", action="store_true",
        help="When listing, show only functions"
    )
    parser.add_argument(
        "--section",
        help="When listing, filter to a specific section (e.g., .text, .init)"
    )
    parser.add_argument(
        "--compiler-version", "-cv", default=None,
        help=f"MWCC GC compiler version (default: {DEFAULT_COMPILER_VERSION})"
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Show detailed output"
    )

    args = parser.parse_args()

    if args.list:
        filter_type = "function" if args.functions else None
        list_symbols(filter_type=filter_type, section=args.section)
        return 0

    if args.list_sources:
        list_source_mappings()
        return 0

    if args.scan:
        scan_path = Path(args.scan)
        if not scan_path.is_absolute():
            scan_path = PROJECT_ROOT / scan_path
        scan_source_file(scan_path, compiler_version=args.compiler_version,
                         verbose=args.verbose,
                         max_symbols=args.max_scan_symbols,
                         allow_long_scan=args.allow_long_scan,
                         report=args.report,
                         timeout=args.timeout)
        return 0

    if not args.symbol:
        parser.print_help()
        print("\nERROR: Provide a symbol name/address, or use --list/--scan")
        return 1

    result = test_function(args.symbol, compiler_version=args.compiler_version,
                           verbose=args.verbose)
    return 0 if result else 1


if __name__ == "__main__":
    sys.exit(main() or 0)
