#!/usr/bin/env python3
"""Print target-vs-mine instruction diff for ONE symbol using objdiff-cli JSON.

Orientation: -1 = ORIGINAL TARGET (left, match this), -2 = MY compiled C (right).
The objdiff JSON 'right.symbols[].instructions[]' carries both sides per row.
"""
import os
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLI = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
ORIG = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
BASE = ROOT / "build/GC6E01/base/game/pokemon.o"


def render_row(instr):
    # Try a few shapes for rendered text.
    parts = []
    def grab(d):
        m = d.get("mnemonic") or d.get("opcode_str") or ""
        args = d.get("arguments") or d.get("args") or []
        astrs = []
        for a in args:
            if isinstance(a, str):
                astrs.append(a)
            elif isinstance(a, dict):
                for k in ("value", "plain", "literal", "Plain", "Literal",
                          "reg", "register", "Relocation", "BranchDest",
                          "branch_dest", "opaque", "Opaque"):
                    if k in a:
                        astrs.append(str(a[k]))
                        break
                else:
                    astrs.append(json.dumps(a, separators=(",", ":")))
        return (m + " " + ", ".join(astrs)).strip()
    return grab(instr)


def main():
    symbol = sys.argv[1]
    cmd = [str(CLI), "diff", "-1", str(ORIG), "-2", str(BASE), "-o", "-",
           "--format", "json", "-c", "ppc.calculatePoolRelocations=false", symbol]
    out = subprocess.run(cmd, cwd=str(ROOT), capture_output=True, text=True)
    if out.returncode != 0:
        print("objdiff failed:", out.stderr[:500])
        sys.exit(1)
    data = json.loads(out.stdout)
    for side in ("right", "left"):
        for entry in data.get(side, {}).get("symbols", []):
            if entry.get("name") != symbol:
                continue
            instrs = entry.get("instructions", [])
            if not instrs:
                continue
            print(f"==== {side} symbol {symbol}: {len(instrs)} instrs ====")
            for i, ins in enumerate(instrs):
                dk = ins.get("diff_kind", "")
                tgt = ins.get("instruction") or {}
                # rows often have both this-side and the paired other side
                line = ins.get("arguments")
                # Just dump the raw keys we care about
                txt = render_row(tgt) if isinstance(tgt, dict) else ""
                print(f"{i:3} {dk:14} {txt}")
            return
    # fallback: dump raw structure of first matching symbol
    print("no instructions rendered; dumping raw keys")
    for entry in data.get("right", {}).get("symbols", []):
        if entry.get("name") == symbol:
            print(json.dumps(entry, indent=1)[:4000])
            return


if __name__ == "__main__":
    main()
