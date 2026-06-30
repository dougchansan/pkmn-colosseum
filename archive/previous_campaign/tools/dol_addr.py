#!/usr/bin/env python3
"""dol_addr.py - GameCube DOL file-offset <-> runtime-VA translation.

Ghidra exported function "addresses" as DOL *file offsets* (it did not use
the GameCube loader's memory map), e.g. FUN_00000340 is really the function
at VA 0x80003340. This module parses the DOL section table and converts.

DOL header layout (all big-endian u32):
  0x00  text/data file offsets [18]
  0x48  text/data load addresses [18]
  0x90  text/data sizes [18]
"""

import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOL = ROOT / "orig" / "GC6E01" / "start.dol"

_SECS = None


def _load(dol_path=DOL):
    global _SECS
    if _SECS is not None:
        return _SECS
    d = Path(dol_path).read_bytes()
    offs = struct.unpack(">18I", d[0x00:0x48])
    addrs = struct.unpack(">18I", d[0x48:0x90])
    sizes = struct.unpack(">18I", d[0x90:0xD8])
    _SECS = [(o, a, s) for o, a, s in zip(offs, addrs, sizes) if s]
    return _SECS


def off_to_va(off, dol_path=DOL):
    """DOL file offset -> runtime virtual address, or None if unmapped."""
    for o, a, s in _load(dol_path):
        if o <= off < o + s:
            return a + (off - o)
    return None


def va_to_off(va, dol_path=DOL):
    """Runtime VA -> DOL file offset (inverse), or None if unmapped.
    Used to locate a fn_<VA> in raw_decompilation.c (keyed by FUN_<off>)."""
    for o, a, s in _load(dol_path):
        if a <= va < a + s:
            return o + (va - a)
    return None


def normalize_addresses(funcs, dol_path=DOL):
    """In-place: rewrite each func.address (file off -> VA) and rename
    FUN_<off>/fn_<off> -> fn_<VA> so the rest of the pipeline (TU
    assignment, existing-function skip, import) sees real addresses.
    Drops functions whose offset maps nowhere. Returns kept list."""
    kept = []
    for f in funcs:
        va = off_to_va(f.address, dol_path)
        if va is None:
            continue
        f.address = va
        f.name = f"fn_{va:08X}"
        kept.append(f)
    return kept


if __name__ == "__main__":
    secs = _load()
    print(f"{len(secs)} DOL sections:")
    for o, a, s in secs:
        print(f"  off=0x{o:06X} va=0x{a:08X} size=0x{s:X}")
