#!/usr/bin/env python3
"""fndiff.py - print the aligned target/your-code diff for ONE function.

Runs objdiff-cli once against the current base .o, filters to a single
symbol, and prints a side-by-side of mismatching slots only.

  -1 = ORIGINAL target (match it)  -> printed as TARGET
  -2 = YOUR compiled C             -> printed as YOURS
"""
import os
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")


def main():
    base_o = Path(sys.argv[1])
    sym = sys.argv[2]
    show_all = "--all" in sys.argv
    r = subprocess.run(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT),
    )
    if r.returncode != 0:
        sys.exit("objdiff failed: " + r.stderr[:300])
    j = json.loads(r.stdout)
    left = {s["name"]: s for s in j["left"]["symbols"]
            if s.get("kind") == "SYMBOL_FUNCTION"}
    right = {s["name"]: s for s in j["right"]["symbols"]
             if s.get("kind") == "SYMBOL_FUNCTION"}
    ls = left.get(sym)
    rs = right.get(sym)
    if rs is None:
        sys.exit("symbol not found in right: " + sym)
    _mp = rs.get('match_percent')
    print(f"=== {sym}  match={_mp} ===")
    li = ls.get("instructions", []) if ls else []
    ri = rs.get("instructions", [])
    if "--raw" in sys.argv:
        which = ls if "--yours" not in sys.argv else rs
        label = "TARGET" if "--yours" not in sys.argv else "YOURS"
        print(f"--- raw {label} ---")
        for ins in (which.get("instructions", []) if which else []):
            i = ins.get("instruction") or {}
            parts = i.get("parts") or []
            extra = []
            for p in parts:
                a = p.get("arg") if isinstance(p, dict) else None
                if isinstance(a, dict) and "reloc" in a:
                    extra.append("RELOC=" + json.dumps(a["reloc"]))
            rel = i.get("relocations") or which.get("relocations")
            print(f"  @{str(i.get('address','')):>5}  "
                  f"{i.get('formatted',''):<34} {' '.join(extra)}")
        return
    n = max(len(li), len(ri))
    for idx in range(n):
        l = li[idx] if idx < len(li) else {}
        rr = ri[idx] if idx < len(ri) else {}
        k = l.get("diff_kind") or rr.get("diff_kind")
        lf = (l.get("instruction") or {}).get("formatted") or ""
        rf = (rr.get("instruction") or {}).get("formatted") or ""
        addr = (rr.get("instruction") or l.get("instruction") or {}).get(
            "address", "")
        marker = " " if (not k or k == "DIFF_NONE") else "X"
        if marker == " " and not show_all:
            continue
        print(f"{marker} @{str(addr):>5}  TGT: {lf:<34} | YOU: {rf}")


if __name__ == "__main__":
    main()
