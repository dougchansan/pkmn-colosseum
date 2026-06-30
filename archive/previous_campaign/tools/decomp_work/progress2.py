#!/usr/bin/env python3
"""
Two-axis decomp progress (the TWW/Pikmin2 model).

Classifies every function in src/**/*.c by its ACTIVE branch:
  ASM      - active asm wrapper (`asm T fn(){#include .inc}` not under #if 0, OR #if 1 asm/#else)
             => trivially byte-matches the target but is NOT decompiled.
  STUB     - active body is empty / `/* TODO */` / trivial placeholder => not decompiled.
  REAL_C   - active body is real decompiled C (the thing that counts as "converted").

Then it crosses REAL_C with byte-exactness (from the objdiff number / an optional
equivalent registry) to report the two honest axes:
  BYTE-EXACT %  = real-C functions that byte-match              (the strict axis)
  C-CONVERTED % = real-C functions, match or not (incl. Equivalent)  (the headline axis)

Asm-wrappers are reported separately (ROM-complete but not decompiled), so the
headline isn't inflated by them.

Usage: python tools/decomp_work/progress2.py [--byte-exact N]
  --byte-exact N : the count of REAL-C functions confirmed byte-exact (else estimated).
Equivalent registry (optional): config/GC6E01/equivalent.txt (one fn per line).
"""
import argparse, re, os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "src"
# Equivalent registry: confirmed-wall fns with correct C that doesn't byte-match.
# Kept OUT of config/GC6E01 (build-truth dir) so no build glob can pick it up.
EQUIV_FILE = ROOT / "tools" / "decomp_work" / "equivalent.txt"

FUNC_DEF = re.compile(
    r'^[ \t]*((?:static\s+|asm\s+|inline\s+)*)'          # qualifiers (group1)
    r'(?:[A-Za-z_]\w*[\s*]+)+'                            # return type(s) + ptr
    r'(fn_[0-9A-Fa-f]+|[A-Za-z_]\w*)\s*\([^;]*\)\s*\{?\s*$'  # name(group2)(args){
)
STUB_RE = re.compile(r'/\*\s*TODO|^\s*\{\s*\}\s*$|/\*\s*stub', re.I)


def classify_all():
    """Scan src/**/*.c, return (counts, fn2class) where fn2class maps fn name -> class."""
    total = {"ASM": 0, "STUB": 0, "REAL_C": 0}
    fn2class = {}
    for cf in SRC.rglob("*.c"):
        if "pcport" in cf.as_posix():
            continue
        c, names = classify_file(cf)
        for k in total:
            total[k] += c[k]
        for k in ("ASM", "STUB", "REAL_C"):
            for nm in names[k]:
                fn2class[nm] = k  # last writer wins; names are unique in practice
    return total, fn2class


def measure_match():
    """Per-fn match from the canonical base build via measure_cache, returned as a
    LIST of (name, match%) entries -- one per (object, symbol), exactly as
    tools/progress.py counts them, so axis 2 reconciles bit-for-bit with the
    official 4605/8287 number. (A few fn_ names recur across objects; counting
    every entry, not the de-duped set, is what the official metric does.)
    Returns None if unavailable."""
    import sys as _sys
    _sys.path.insert(0, str(ROOT / "tools"))
    try:
        import measure_cache
    except Exception as e:
        print(f"  (measure_cache unavailable: {e})")
        return None
    target_o = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
    base_dir = ROOT / "build" / "GC6E01" / "base"
    if not target_o.exists() or not base_dir.exists():
        print("  (base build absent; run the base build first)")
        return None
    entries = []
    for o in sorted(base_dir.rglob("*.o")):
        funcs = measure_cache.diff_funcs(target_o, o)
        if not funcs:
            continue
        for f in funcs:
            entries.append((f["name"], float(f["match"])))
    try:
        measure_cache.flush()
    except Exception:
        pass
    return entries


def classify_file(path):
    """Return dict class->count for one .c file."""
    lines = path.read_text(errors="replace").splitlines()
    counts = {"ASM": 0, "STUB": 0, "REAL_C": 0}
    names = {"ASM": [], "STUB": [], "REAL_C": []}
    # preprocessor branch stack: each entry is 'live'(in active text) bool
    # we only need: are we in an #if 0 (dead) region or #else-of-#if0 (live) etc.
    if_stack = []  # list of ('if0'|'if1'|'other', in_else)
    i = 0
    n = len(lines)
    while i < n:
        ln = lines[i]
        s = ln.strip()
        # track preprocessor
        m = re.match(r'#\s*if\s+(0|1)\b', s)
        if m:
            if_stack.append([("if0" if m.group(1) == "0" else "if1"), False]); i += 1; continue
        if re.match(r'#\s*if', s):
            if_stack.append(["other", False]); i += 1; continue
        if re.match(r'#\s*else', s):
            if if_stack: if_stack[-1][1] = True
            i += 1; continue
        if re.match(r'#\s*endif', s):
            if if_stack: if_stack.pop()
            i += 1; continue
        # is this position in ACTIVE (compiled) text?
        active = True
        for kind, in_else in if_stack:
            if kind == "if0":
                if not in_else: active = False; break
            elif kind == "if1":
                if in_else: active = False; break
            # 'other' (#ifdef etc): treat as active (best-effort)
        if not active:
            i += 1; continue
        # function definition on this line?
        fm = FUNC_DEF.match(ln)
        if fm:
            quals, name = fm.group(1), fm.group(2)
            if "asm" in quals:
                counts["ASM"] += 1; names["ASM"].append(name)
            else:
                # Locate the opening brace. It may be on the def line OR on a later
                # line (Ghidra-imported files put `{` on its own line after the
                # signature) -- handling the latter is essential: otherwise every
                # such function reads as empty and is miscounted as a STUB.
                bo = i
                while bo < n and "{" not in lines[bo] and bo < i + 4:
                    bo += 1
                if bo >= n or "{" not in lines[bo]:
                    # no body brace nearby: it has a signature, count as REAL_C
                    counts["REAL_C"] += 1; names["REAL_C"].append(name); i += 1; continue
                # collect the body until the matching close brace (capped window)
                depth = 0
                inner_parts = []
                j = bo
                while j < n and j < bo + 120:
                    line = lines[j]
                    if j == bo:
                        inner_parts.append(line[line.index("{") + 1:])
                    else:
                        inner_parts.append(line)
                    depth += line.count("{") - line.count("}")
                    if depth <= 0:
                        break
                    j += 1
                stripped = re.sub(r'\s+', '', "\n".join(inner_parts))
                body_only = stripped.rstrip("}")
                is_stub = (
                    body_only == ""
                    or "/*TODO" in stripped
                    or body_only == "return;"
                    or bool(re.fullmatch(r'return[-0-9xA-Fa-f]*;?', body_only))
                )
                if is_stub:
                    counts["STUB"] += 1; names["STUB"].append(name)
                else:
                    counts["REAL_C"] += 1; names["REAL_C"].append(name)
        i += 1
    return counts, names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--measure", action="store_true",
                    help="rigorous mode: join the source scan with per-fn objdiff match%% "
                         "from the canonical base build (no derivation-by-subtraction)")
    ap.add_argument("--list-equiv", metavar="FILE", default=None,
                    help="write REAL_C-but-not-matching fn names (Equivalent pool) to FILE")
    a = ap.parse_args()

    equiv = set()
    if EQUIV_FILE.exists():
        equiv = {l.split("#")[0].strip() for l in EQUIV_FILE.read_text().splitlines()
                 if l.split("#")[0].strip()}

    total, fn2class = classify_all()
    src_total = total["ASM"] + total["STUB"] + total["REAL_C"]
    converted = total["REAL_C"]

    print("=== SOURCE FUNCTION CLASSIFICATION (active branch) ===")
    print(f"  REAL_C (decompiled):     {total['REAL_C']:>5}")
    print(f"  ASM-wrapper (ROM-only):  {total['ASM']:>5}   (trivially byte-match; NOT decompiled)")
    print(f"  STUB (TODO/empty):       {total['STUB']:>5}")
    print(f"  source functions total:  {src_total:>5}")
    print()

    if not a.measure:
        print("=== ONE-AXIS (scan only) ===")
        print(f"  C-CONVERTED (real C):  {converted} / {src_total} = {100*converted/src_total:.2f}%  [headline]")
        print(f"  (pass --measure for the rigorous byte-exact axis via objdiff)")
        print(f"  Equivalent registry entries: {len(equiv)}")
        return

    entries = measure_match()
    if entries is None:
        print("  rigorous mode unavailable; see message above.")
        return

    # rigorous join over the OBJDIFF universe (every (object,symbol) entry, exactly as
    # tools/progress.py counts -> axis 2 reconciles with the official 4605/8287).
    # Each objdiff fn is looked up in the source scan; fns with no .c definition
    # (pure split asm / unscanned file) are NO_SOURCE.
    MATCH = 100.0 - 1e-6  # 100.0000%
    CATS = ("REAL_C", "ASM", "STUB", "NO_SOURCE")
    cell = {(k, m): 0 for k in CATS for m in (True, False)}
    equiv_seen = {}
    for nm, pct in entries:
        cls = fn2class.get(nm, "NO_SOURCE")
        is_match = pct >= MATCH
        cell[(cls, is_match)] += 1
        if cls == "REAL_C" and not is_match:
            equiv_seen[nm] = max(equiv_seen.get(nm, -1.0), pct)
    equiv_pool = list(equiv_seen.items())

    obj_total = len(entries)
    realc_exact = cell[("REAL_C", True)]
    realc_near = cell[("REAL_C", False)]
    asm_match = cell[("ASM", True)] + cell[("NO_SOURCE", True)]  # asm-wrapper + pure-split-asm both ROM-only
    realc_total = realc_exact + realc_near
    rom_match = sum(cell[(k, True)] for k in CATS)  # == objdiff matched

    print("=== RIGOROUS JOIN (objdiff universe x source class) ===")
    print(f"  {'class':<10} {'byte-exact':>10} {'near-miss':>10}")
    for k in CATS:
        print(f"  {k:<10} {cell[(k,True)]:>10} {cell[(k,False)]:>10}")
    print(f"  objdiff fn_ universe: {obj_total}   matched(ROM): {rom_match} "
          f"({100*rom_match/obj_total:.2f}%)  [reconciles with tools/progress.py]")
    print()
    print("=== THREE HONEST AXES (denominator = objdiff fn_ universe) ===")
    print(f"  1. C-CONVERTED   (real C written, match or not): {realc_total:>5} / {obj_total} = "
          f"{100*realc_total/obj_total:.2f}%   [decompilation headline]")
    print(f"  2. ROM-REPRODUCIBLE (real-C-match + asm/split):  {rom_match:>5} / {obj_total} = "
          f"{100*rom_match/obj_total:.2f}%   [the official objdiff number; main.dol byte-identical]")
    print(f"  3. BYTE-EXACT C  (real C that byte-matches):     {realc_exact:>5} / {obj_total} = "
          f"{100*realc_exact/obj_total:.2f}%   [strict; excludes {asm_match} asm/split wrappers]")
    print()
    print(f"  EQUIVALENT POOL (real C, not byte-exact):        {realc_near}  "
          f"(near-miss; confirmed walls -> {EQUIV_FILE.name}, registry has {len(equiv)})")
    print(f"  ROM-only (asm-wrapper + pure split asm):         {asm_match}  "
          f"(inflates axis 2 over axis 3; NOT decompiled)")

    if a.list_equiv:
        equiv_pool.sort(key=lambda x: -x[1])  # closest-to-match first
        with open(a.list_equiv, "w") as fh:
            fh.write("# REAL_C functions not byte-exact (Equivalent pool), sorted closest-first\n")
            fh.write("# name  match%\n")
            for nm, pct in equiv_pool:
                fh.write(f"{nm}\t{pct:.4f}\n")
        print(f"  wrote {len(equiv_pool)} equivalent-pool fns -> {a.list_equiv}")


if __name__ == "__main__":
    main()
