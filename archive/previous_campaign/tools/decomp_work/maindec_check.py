#!/usr/bin/env python3
"""maindec_check.py - isolated compile+measure harness for src/game/main.c work.

Compiles a WORKING COPY of main.c with the project's canonical flags
(GC/1.3, -O4,p ... from compile_config.json default) to a .o next to the
working copy, then objdiffs the named functions against the original target
object. Never writes shared build outputs, so parallel agents can't collide.

Usage:
    python tools/decomp_work/maindec_check.py <work.c> <fn> [<fn> ...]
    python tools/decomp_work/maindec_check.py <work.c> --diff <fn>

Output (measure mode):   <fn>\t<match% | NA | NOT FOUND>
Output (--diff mode):    side-by-side instruction rows, '*' = mismatch.
Orientation: TARGET = original (match this); YOURS = the compiled work copy.
"""
import os
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MWCC = ROOT / "tools/mwcc_compiler/GC/1.3/mwcceppc.exe"
OBJDIFF = ROOT / ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli")
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
FLAGS = ("-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off "
         "-enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8").split()


def compile_work(src: Path) -> Path:
    obj = src.with_suffix(".o")
    cmd = [str(MWCC), "-c", "-o", str(obj)] + FLAGS + ["-i", str(ROOT / "include"), str(src)]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    if r.returncode != 0:
        print("COMPILE FAILED")
        print(r.stdout)
        print(r.stderr)
        sys.exit(1)
    return obj


def objdiff_json(obj: Path, fn: str) -> dict:
    cmd = [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(obj), "-o", "-",
           "--format", "json", "-c", "ppc.calculatePoolRelocations=false", fn]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    try:
        return json.loads(r.stdout)
    except Exception:
        return {}


def measure(obj: Path, fn: str):
    d = objdiff_json(obj, fn)
    for s in (d.get("right") or {}).get("symbols", []):
        if s.get("name") == fn:
            return s.get("match_percent")
    return None


def rows(d: dict, side: str, fn: str):
    for s in (d.get(side) or {}).get("symbols", []):
        if s.get("name") == fn and "instructions" in s:
            return s["instructions"]
    return []


def fmt(ins):
    i = ins.get("instruction") or {}
    t = i.get("formatted", "---")
    rel = i.get("relocation") or {}
    name = (rel.get("target", {}) or {}).get("name", "")
    return t + ("  ; -> " + name if name else "")


def show_diff(obj: Path, fn: str):
    d = objdiff_json(obj, fn)
    L, R = rows(d, "left", fn), rows(d, "right", fn)
    n = max(len(L), len(R))
    print(f"=== {fn}: TARGET ({len(L)} instr)  vs  YOURS ({len(R)} instr) ===")
    for k in range(n):
        lt = fmt(L[k]) if k < len(L) else "---"
        rt = fmt(R[k]) if k < len(R) else "---"
        lk = (L[k].get("diff_kind") or "") if k < len(L) else "X"
        rk = (R[k].get("diff_kind") or "") if k < len(R) else "X"
        mark = " " if (lk in ("", "DIFF_NONE") and rk in ("", "DIFF_NONE") and lt == rt) else "*"
        print(f" {k:>3}{mark} {lt:<52} | {rt}")


def main():
    src = Path(sys.argv[1]).resolve()
    args = sys.argv[2:]
    obj = compile_work(src)
    if args and args[0] == "--diff":
        show_diff(obj, args[1])
        return
    for fn in args:
        p = measure(obj, fn)
        print(f"{fn}\t{p if p is not None else 'NOT FOUND'}")


if __name__ == "__main__":
    main()
