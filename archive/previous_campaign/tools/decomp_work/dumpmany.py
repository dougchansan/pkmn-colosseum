#!/usr/bin/env python3
"""Dump aligned target-vs-ours diffs for MANY symbols from ONE objdiff pass."""
import os
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")


def main():
    base = sys.argv[1]
    syms = sys.argv[2:]
    base_o = ROOT / base
    # one full-object pass (no symbol arg)
    r = subprocess.run(
        [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT),
    )
    if r.returncode != 0:
        print("objdiff failed:", r.stderr[:400])
        return
    d = json.loads(r.stdout)
    left = {s["name"]: s for s in d["left"]["symbols"]}
    right = {s["name"]: s for s in d["right"]["symbols"]}
    for sym in syms:
        ls = left.get(sym, {})
        rs = right.get(sym, {})
        li = ls.get("instructions", [])
        ri = rs.get("instructions", [])
        print(f"\n=== {sym}  target%={ls.get('match_percent')}  "
              f"ours%={rs.get('match_percent')} ===")
        print(f"{'TARGET (aim for)':<46} | OURS")
        print("-" * 95)
        n = max(len(li), len(ri))
        for idx in range(n):
            lf = rf = lk = rk = ""
            if idx < len(li):
                lf = ((li[idx].get("instruction") or {}).get("formatted")) or ""
                lk = li[idx].get("diff_kind") or ""
            if idx < len(ri):
                rf = ((ri[idx].get("instruction") or {}).get("formatted")) or ""
                rk = ri[idx].get("diff_kind") or ""
            mark = " "
            if (lk and lk != "DIFF_NONE") or (rk and rk != "DIFF_NONE"):
                mark = "X"
            print(f"{mark} {lf:<44} | {rf}")


if __name__ == "__main__":
    main()
