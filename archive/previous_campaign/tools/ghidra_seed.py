#!/usr/bin/env python3
"""ghidra_seed.py - per-function Ghidra work-packet extractor.

The bulk of unmatched code is low-% asm-style functions, not near-misses.
Full-decompiling them is LLM work, but an agent should not waste tokens
(a) finding the function, (b) regenerating Ghidra output, or (c) digging
the target asm. This packages all three into one packet.

For a function fn_<VA> it emits:
  - the fresh Ghidra C (from build/ghidra_output/raw_decompilation.c,
    located by VA -> DOL file offset -> FUN_<off> block)
  - the function's current match% and a hint of its current src state
  - where the target disassembly can be diffed

Usage:
    python tools/ghidra_seed.py fn_8025E534
    python tools/ghidra_seed.py --file src/game/colosseum_battle.c \
        --max-pct 40 --limit 20 --out G:/decomp-worktrees/seeds_cb.md
"""

import os
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
RAW = ROOT / "build" / "ghidra_output" / "raw_decompilation.c"
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
sys.path.insert(0, str(ROOT / "tools"))
import compile_check          # noqa: E402
from dol_addr import va_to_off  # noqa: E402
from headless_subprocess import run as run_tool  # noqa: E402

HDR = re.compile(r"/\*\s*[-=]+\s*(FUN_[0-9A-Fa-f]+|fn_[0-9A-Fa-f]+)\s+"
                 r"addr=0x([0-9A-Fa-f]+)\s+size=0x([0-9A-Fa-f]+)")


def raw_index():
    """offset(int) -> (header_line_idx, end_idx) over raw_decompilation.c."""
    lines = RAW.read_text(encoding="utf-8", errors="replace").splitlines()
    hdrs = [(i, int(m.group(2), 16))
            for i, ln in enumerate(lines)
            for m in [HDR.search(ln)] if m]
    idx = {}
    for k, (li, off) in enumerate(hdrs):
        end = hdrs[k + 1][0] if k + 1 < len(hdrs) else len(lines)
        idx[off] = (li, end)
    return lines, idx


def ghidra_block(lines, idx, va):
    off = va_to_off(va)
    if off is None or off not in idx:
        return None
    a, b = idx[off]
    return "\n".join(lines[a:b]).strip()


def file_targets(src, max_pct):
    base_o = compile_check.source_to_base_obj(Path(src).resolve())
    if not base_o.exists():
        compile_check.compile_source(Path(src).resolve())
    r = run_tool(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT))
    j = json.loads(r.stdout)
    out = []
    for s in j.get("right", {}).get("symbols", []):
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        n = s.get("name", "")
        p = s.get("match_percent", 0.0)
        if n.startswith("fn_") and p < max_pct:
            out.append((p, n))
    out.sort()
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol", nargs="?")
    ap.add_argument("--file")
    ap.add_argument("--max-pct", type=float, default=40.0)
    ap.add_argument("--limit", type=int, default=20)
    ap.add_argument("--out")
    args = ap.parse_args()

    if not RAW.exists():
        sys.exit(f"missing {RAW} — run tools/ghidra_batch_decompile.py")
    lines, idx = raw_index()

    if args.symbol:
        targets = [(None, args.symbol)]
    elif args.file:
        targets = file_targets(args.file, args.max_pct)[:args.limit]
    else:
        sys.exit("give a symbol or --file")

    blocks = []
    miss = 0
    for pct, name in targets:
        va = int(name[3:], 16)
        g = ghidra_block(lines, idx, va)
        if g is None:
            miss += 1
            continue
        pct_s = f"{pct:.1f}%" if pct is not None else "?"
        blocks.append(f"### {name}  (current {pct_s})\n\n"
                       f"```c\n{g}\n```\n")

    hdr = (f"# Ghidra seeds: {args.file or args.symbol}\n\n"
           f"{len(blocks)} packets "
           f"(max-pct {args.max_pct}, {miss} had no Ghidra block)\n\n"
           "Each block is fresh Ghidra C for one undecompiled function. "
           "Clean it (types, signature, externs, CW idioms) into "
           "compilable C, compile-check, measure, iterate. Do NOT trust "
           "Ghidra register guesses (unaff_rN) — derive real params from "
           "the target asm.\n\n")
    body = hdr + "\n".join(blocks)
    if args.out:
        Path(args.out).write_text(body, encoding="utf-8")
        print(f"[seed] {len(blocks)} packets -> {args.out}")
    else:
        print(body)


if __name__ == "__main__":
    main()
