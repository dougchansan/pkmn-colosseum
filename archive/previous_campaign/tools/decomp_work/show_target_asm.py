#!/usr/bin/env python3
"""show_target_asm.py <fn> - print the TARGET (aim-for) PowerPC asm for one function.

Prefers the clean sibling .inc; falls back to disassembling the canonical target
object with dtk (cached) for functions that have no saved .inc. This is the asm
your decompiled C must reproduce byte-for-byte.
"""
import os
import sys, subprocess, re, tempfile
from pathlib import Path

ROOT = Path("C:/Users/douglaswhittingham/pkmn-colosseum")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
DTK = ROOT / "tools" / ("dtk.exe" if os.name == "nt" else "dtk")
CACHE = Path(tempfile.gettempdir()) / "auto_01_disasm.txt"


def from_inc(fn):
    for p in (ROOT / "src" / "game").glob(f"*_{fn}.inc"):
        return p.read_text(errors="replace")
    return None


def from_dtk(fn):
    if not CACHE.exists() or CACHE.stat().st_size < 1000:
        subprocess.run([str(DTK), "elf", "disasm", str(TARGET), str(CACHE)],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd=str(ROOT))
    txt = CACHE.read_text(errors="replace").splitlines()
    out, grab = [], False
    for ln in txt:
        if re.match(rf"^\.fn\s+{re.escape(fn)}\b", ln):
            grab = True
        if grab:
            out.append(ln)
            if re.match(rf"^\.endfn\s+{re.escape(fn)}\b", ln):
                break
    return "\n".join(out) if out else None


def main():
    fn = sys.argv[1]
    asm = from_inc(fn)
    if asm:
        print(f"# target asm for {fn} (from .inc)\n{asm}")
        return
    asm = from_dtk(fn)
    if asm:
        print(f"# target asm for {fn} (from dtk disasm of target object)\n{asm}")
        return
    print(f"# no target asm found for {fn} (try: grep {fn} the dtk disasm at {CACHE})")


if __name__ == "__main__":
    main()
