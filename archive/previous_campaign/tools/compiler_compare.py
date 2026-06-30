#!/usr/bin/env python3
"""
compiler_compare.py - Compare CW GC/1.2.5n vs GC/1.3 for each source file.
Optimized: compiles each file once per compiler, then batch-tests functions.
"""

import json
import os
import re
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
TOOLS_DIR = PROJECT_ROOT / "tools"
MWCC_BASE = TOOLS_DIR / "mwcc_compiler" / "GC"
OBJDIFF_CLI = TOOLS_DIR / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
INCLUDE_DIR = PROJECT_ROOT / "include"
SRC_DIR = PROJECT_ROOT / "src"
BUILD_DIR = PROJECT_ROOT / "build" / "GC6E01"
OBJ_DIR = BUILD_DIR / "obj"
CONFIG_DIR = PROJECT_ROOT / "config" / "GC6E01"
SYMBOLS_TXT = CONFIG_DIR / "symbols.txt"

CFLAGS = "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off".split()
TARGET_OBJ = OBJ_DIR / "auto_01_800055E0_text.o"

FILES = [
    "src/game/colosseum_script.c",
    "src/game/colosseum_event.c",
    "src/game/pokemon.c",
    "src/game/trainer.c",
    "src/game/gs_render.c",
    "src/game/gs_field_world.c",
    "src/game/gs_model.c",
    "src/game/gs_thread.c",
    "src/game/battle/battle_waza.c",
    "src/game/battle/battle_scene.c",
    "src/game/battle/battle_logic.c",
    "src/game/ui/ui_core.c",
    "src/game/sound/sound.c",
]

SAMPLE_SIZE = 6  # functions per file


def parse_symbols():
    """Parse symbols.txt -> dict of name -> (address, size) for functions."""
    sym_re = re.compile(
        r'^(\S+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+)\s*;(?:\s*//\s*(.*))?$'
    )
    symbols = {}
    with open(SYMBOLS_TXT) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                continue
            m = sym_re.match(line)
            if not m:
                continue
            name, section, addr_str, comment = m.group(1), m.group(2), m.group(3), m.group(4) or ""
            address = int(addr_str, 16)
            sym_type = size = None
            for part in comment.split():
                if part.startswith("type:"):
                    sym_type = part.split(":", 1)[1]
                elif part.startswith("size:"):
                    try:
                        size = int(part.split(":", 1)[1], 0)
                    except ValueError:
                        pass
            if sym_type == "function" and size and size > 0:
                symbols[name] = (address, size)
    return symbols


def get_file_functions(src_path, all_symbols):
    """Get function names in a source file that also exist in symbols."""
    with open(src_path, "r", errors="replace") as f:
        content = f.read()

    # Collect addresses mentioned as fn_XXXXXXXX
    fn_pat = re.compile(r'\b(fn_([0-9A-Fa-f]{8}))\b')
    addr_to_name = {}
    for m in fn_pat.finditer(content):
        addr = int(m.group(2), 16)
        addr_to_name[addr] = m.group(1)

    # Collect named function definitions
    func_def = re.compile(r'^(?:\w[\w\s\*]+)\s+(\w+)\s*\([^)]*\)\s*\{', re.MULTILINE)
    defined = set()
    for m in func_def.finditer(content):
        n = m.group(1)
        if n not in {'if', 'while', 'for', 'switch', 'return', 'else'}:
            defined.add(n)

    functions = []
    seen = set()
    for sym_name, (addr, size) in all_symbols.items():
        if addr in addr_to_name and addr not in seen:
            functions.append((sym_name, addr, size))
            seen.add(addr)
        elif sym_name in defined and addr not in seen:
            functions.append((sym_name, addr, size))
            seen.add(addr)

    functions.sort(key=lambda x: x[1])
    return functions


def compile_file(src_path, compiler_version, output_path):
    """Compile and return True on success."""
    compiler = MWCC_BASE / compiler_version / "mwcceppc.exe"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    cmd = [str(compiler), "-c"] + CFLAGS + ["-i", str(INCLUDE_DIR), "-o", str(output_path), str(src_path)]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(PROJECT_ROOT))
    return r.returncode == 0


def diff_function(target_obj, base_obj, symbol_name):
    """Return match percentage for a symbol, or None on failure."""
    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj), "-2", str(base_obj),
        "-o", "-", "--format", "json",
        symbol_name,
    ]
    try:
        r = subprocess.run(cmd, capture_output=True, text=True,
                           cwd=str(PROJECT_ROOT), timeout=60)
    except subprocess.TimeoutExpired:
        return None

    if r.returncode != 0 or not r.stdout.strip():
        return None

    try:
        d = json.loads(r.stdout)
    except json.JSONDecodeError:
        return None

    for s in d.get("left", {}).get("symbols", []):
        if s.get("name") == symbol_name:
            instrs = s.get("instructions", [])
            total = len(instrs)
            if total == 0:
                return None
            matched = sum(1 for i in instrs if i.get("diff_kind") in (None, "DIFF_NONE"))
            return 100.0 * matched / total

    return None


def select_sample(functions, n):
    """Select n representative medium-sized functions spread across the range."""
    # Prefer 48-1024 byte functions
    medium = [f for f in functions if 48 <= f[2] <= 1024]
    if len(medium) < n:
        medium = [f for f in functions if 24 <= f[2] <= 4096]
    if len(medium) < n:
        medium = functions
    if len(medium) <= n:
        return medium
    step = max(1, len(medium) // n)
    return medium[::step][:n]


def main():
    print("=" * 70, flush=True)
    print("COMPILER COMPARISON: CW GC/1.2.5n vs GC/1.3", flush=True)
    print("=" * 70, flush=True)

    all_symbols = parse_symbols()
    print(f"Loaded {len(all_symbols)} function symbols\n", flush=True)

    tmp_dir = PROJECT_ROOT / "build" / "tmp"
    os.makedirs(tmp_dir, exist_ok=True)

    all_results = []

    for src_rel in FILES:
        src_path = PROJECT_ROOT / src_rel
        print(f"\n{'='*70}", flush=True)
        print(f"FILE: {src_rel}", flush=True)

        functions = get_file_functions(src_path, all_symbols)
        print(f"  Functions: {len(functions)} total", flush=True)

        sample = select_sample(functions, SAMPLE_SIZE)
        print(f"  Sampling: {len(sample)} functions", flush=True)

        out_125n = tmp_dir / "test_125n.o"
        out_13 = tmp_dir / "test_13.o"

        ok_125n = compile_file(src_path, "1.2.5n", out_125n)
        ok_13 = compile_file(src_path, "1.3", out_13)
        print(f"  Compile 1.2.5n: {'OK' if ok_125n else 'FAIL'}  |  1.3: {'OK' if ok_13 else 'FAIL'}", flush=True)

        if not ok_125n and not ok_13:
            continue

        results = []
        for sym_name, addr, size in sample:
            m125 = diff_function(TARGET_OBJ, out_125n, sym_name) if ok_125n else None
            m13 = diff_function(TARGET_OBJ, out_13, sym_name) if ok_13 else None

            s125 = f"{m125:.1f}%" if m125 is not None else "N/A"
            s13 = f"{m13:.1f}%" if m13 is not None else "N/A"
            tag = ""
            if m125 is not None and m13 is not None:
                if m13 > m125 + 0.5:
                    tag = " << 1.3 WINS"
                elif m125 > m13 + 0.5:
                    tag = " << 1.2.5n WINS"
                else:
                    tag = " (tied)"

            print(f"    {sym_name:40s} 1.2.5n={s125:>8s}  1.3={s13:>8s}{tag}", flush=True)
            results.append({"name": sym_name, "addr": f"0x{addr:08X}", "size": size,
                            "m125n": m125, "m13": m13})

        # Summarize
        compared = [(r["m125n"], r["m13"]) for r in results if r["m125n"] is not None and r["m13"] is not None]
        if compared:
            avg_125 = sum(a for a, b in compared) / len(compared)
            avg_13 = sum(b for a, b in compared) / len(compared)
            n13_better = sum(1 for a, b in compared if b > a + 0.5)
            n125_better = sum(1 for a, b in compared if a > b + 0.5)
            ntied = len(compared) - n13_better - n125_better

            rec = "1.2.5n"
            if n13_better > n125_better and avg_13 > avg_125:
                rec = "1.3"
            elif n13_better > n125_better * 2:
                rec = "1.3"
            elif avg_13 > avg_125 + 3.0:
                rec = "1.3"

            print(f"\n  SUMMARY: compared={len(compared)}, 1.2.5n wins={n125_better}, 1.3 wins={n13_better}, tied={ntied}", flush=True)
            print(f"  Avg 1.2.5n={avg_125:.1f}%  Avg 1.3={avg_13:.1f}%  => RECOMMEND: GC/{rec}", flush=True)

            all_results.append({
                "file": src_rel, "compared": len(compared),
                "n125_better": n125_better, "n13_better": n13_better, "ntied": ntied,
                "avg_125n": avg_125, "avg_13": avg_13, "rec": rec,
                "details": results,
            })
        else:
            print(f"\n  No comparable results", flush=True)

    # Final summary
    print(f"\n\n{'='*70}", flush=True)
    print("FINAL RESULTS", flush=True)
    print(f"{'='*70}", flush=True)
    print(f"\n{'File':<45s} {'Rec':>6s} {'Avg125n':>8s} {'Avg13':>8s} {'125nW':>6s} {'13W':>4s}", flush=True)
    print("-" * 80, flush=True)

    files_13 = []
    files_125n = []
    for r in all_results:
        short = r["file"].replace("src/game/", "")
        print(f"  {short:<43s} {r['rec']:>6s} {r['avg_125n']:>7.1f}% {r['avg_13']:>7.1f}% {r['n125_better']:>5d} {r['n13_better']:>4d}", flush=True)
        if r["rec"] == "1.3":
            files_13.append(r["file"])
        else:
            files_125n.append(r["file"])

    print(f"\nFiles for GC/1.3: {files_13}", flush=True)
    print(f"Files for GC/1.2.5n: {files_125n}", flush=True)

    # Save results
    with open(PROJECT_ROOT / "compiler_comparison_results.json", "w") as f:
        json.dump(all_results, f, indent=2)

    print("\nDone.", flush=True)
    return files_13


if __name__ == "__main__":
    main()
