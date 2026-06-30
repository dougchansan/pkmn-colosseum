#!/usr/bin/env python3
"""Quick diff helper - extracts a single symbol's diff from objdiff-cli output."""
import os
import json
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
OBJDIFF_CLI = PROJECT_ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")

def diff_symbol(target_obj, base_obj, symbol):
    cmd = [
        str(OBJDIFF_CLI), "diff",
        "-1", str(target_obj),
        "-2", str(base_obj),
        "-o", "-",
        "--format", "json",
        symbol,
    ]
    result = subprocess.run(cmd, capture_output=True, cwd=str(PROJECT_ROOT))
    if result.returncode != 0:
        print(f"Error: {result.stderr.decode()[:200]}")
        return

    d = json.loads(result.stdout)

    for side_name, side in [("TARGET", d["left"]), ("DECOMP", d["right"])]:
        for s in side.get("symbols", []):
            if s.get("name") == symbol and "instructions" in s:
                instrs = s["instructions"]
                total = len(instrs)
                matched = sum(
                    1
                    for i in instrs
                    if i.get("diff_kind") in (None, "DIFF_NONE")
                )
                pct = 100 * matched / total if total else 0
                print(f"\n{side_name}: {symbol} ({matched}/{total} = {pct:.1f}%)")
                for i in instrs:
                    ii = i.get("instruction", i)
                    dk = i.get("diff_kind", "NONE")
                    fmt = ii.get("formatted", "?")
                    mark = "  " if dk in (None, "NONE", "DIFF_NONE") else "! "
                    print(f"  {mark}[{dk:<22}] {fmt}")
                break
        else:
            print(f"\n{side_name}: {symbol} not found")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python tools/quick_diff.py <target.o> <base.o> <symbol>")
        sys.exit(1)
    diff_symbol(sys.argv[1], sys.argv[2], sys.argv[3])
