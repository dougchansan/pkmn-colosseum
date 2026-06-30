#!/usr/bin/env python3
"""Extract a function's decomp.me-ready GAS target from the dtk-built asm.

Pulls the .fn..endfn block from build/dol/asm/auto_01_800055E0_text.s, strips the
dtk address/bytes comments, drops .fn/.endfn, prepends `.global <fn>` + `<fn>:`.
Keeps .L_ local labels and @ha/@l/@sda21 relocations (all GAS-valid for decomp.me).

Usage: python tools/decomp_work/extract_target.py fn_80191788 [out.s]
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
ASM = ROOT / "build" / "dol" / "asm" / "auto_01_800055E0_text.s"
COMMENT = re.compile(r'/\*.*?\*/')


def extract(fn):
    lines = ASM.read_text(errors='replace').splitlines()
    out, inblk = [], False
    for l in lines:
        if l.strip().startswith(f'.fn {fn},') or l.strip() == f'.fn {fn}':
            inblk = True
            continue
        if inblk and l.strip().startswith('.endfn'):
            break
        if inblk:
            stripped = COMMENT.sub('', l).rstrip()
            # collapse leading tabs/spaces left by comment removal
            stripped = re.sub(r'^\s+', '\t', stripped) if stripped.strip() else ''
            if stripped.strip():
                out.append(stripped)
    if not out:
        raise SystemExit(f"function {fn} not found in {ASM}")
    return f".global {fn}\n{fn}:\n" + "\n".join(out) + "\n"


if __name__ == "__main__":
    fn = sys.argv[1]
    gas = extract(fn)
    if len(sys.argv) > 2:
        Path(sys.argv[2]).write_text(gas)
        print(f"wrote {sys.argv[2]} ({gas.count(chr(10))} lines)")
    else:
        sys.stdout.write(gas)
