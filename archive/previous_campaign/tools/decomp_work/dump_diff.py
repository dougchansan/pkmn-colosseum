#!/usr/bin/env python3
import os
import json, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE = ROOT / "build" / "GC6E01" / "base" / "game" / "effect" / "effect_util.o"
CFG = "ppc.calculatePoolRelocations=false"

def fmt_instr(row):
    if row is None:
        return ""
    ins = row.get("instruction") or {}
    return ins.get("formatted", "")

def main():
    sym = sys.argv[1]
    cmd = [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(BASE),
           "-o", "-", "--format", "json", "-c", CFG, sym]
    out = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    if out.returncode != 0:
        print("ERR", out.stderr[:500]); return
    data = json.loads(out.stdout)
    # find symbol on both sides
    def find(side):
        for e in data.get(side, {}).get("symbols", []):
            if e.get("name") == sym:
                return e
        return None
    left = find("left")   # target (aim-for)
    right = find("right") # ours
    li = left.get("instructions", []) if left else []
    ri = right.get("instructions", []) if right else []
    n = max(len(li), len(ri))
    print(f"== {sym} ==  target={len(li)} ours={len(ri)}")
    print(f"{'IDX':>3} {'K':1} {'TARGET (aim-for)':<40} {'OURS':<40}")
    for i in range(n):
        l = li[i] if i < len(li) else None
        r = ri[i] if i < len(ri) else None
        lk = l.get("diff_kind") if l else "-"
        rk = r.get("diff_kind") if r else "-"
        mark = " "
        if (lk not in (None, "DIFF_NONE")) or (rk not in (None, "DIFF_NONE")):
            mark = "*"
        print(f"{i:>3} {mark} {fmt_instr(l):<40} {fmt_instr(r):<40}")

if __name__ == "__main__":
    main()
