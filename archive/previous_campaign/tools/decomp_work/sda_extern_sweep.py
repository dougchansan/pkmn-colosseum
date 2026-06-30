#!/usr/bin/env python3
"""sda_extern_sweep.py -- find (and optionally apply) extern-sizing fixes.

An unsized `extern u8 lbl_XXXX[];` to a SMALL .sbss/.sdata global makes CW/the
assembler address the .inc's `la/lwz rX, lbl(r13)` as ABSOLUTE instead of
@sda21, so the band UNDER-measures vs the real DOL. Sizing the extern to the
symbol's true size (<= -sdata 8) restores @sda21.

This tool ONLY proposes sizing `extern u8 lbl_XXXX[];` (unsized BYTE arrays) whose
symbol is in {.sbss,.sdata,.sbss2,.sdata2} with size <= 8 per symbols.txt.

Usage:
  sda_extern_sweep.py discover            # list candidates grouped by file
  sda_extern_sweep.py apply <file.c>      # size all candidates in one TU (in place)
"""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
SYMS = os.path.join(REPO, "config", "GC6E01", "symbols.txt")
SDA_SECTIONS = (".sbss", ".sdata", ".sbss2", ".sdata2")
THRESH = 8

def load_syms():
    m = {}
    rx = re.compile(r"^(\w+)\s*=\s*(\.\w+):0x[0-9A-Fa-f]+;.*?size:0x([0-9A-Fa-f]+)")
    for ln in open(SYMS, encoding="utf-8", errors="replace"):
        g = rx.match(ln.strip())
        if g:
            name, sec, size = g.group(1), g.group(2), int(g.group(3), 16)
            m[name] = (sec, size)
    return m

# unsized byte-array extern:  extern u8 lbl_XXXXXXXX[];
DECL = re.compile(r"^(\s*extern\s+u8\s+)(lbl_[0-9A-Fa-f]+)(\[\]\s*;)")

def candidates():
    syms = load_syms()
    out = {}  # file -> list[(lineno, symbol, size, raw)]
    for dp, _, fns in os.walk(os.path.join(REPO, "src")):
        for fn in fns:
            if not fn.endswith(".c"):
                continue
            path = os.path.join(dp, fn)
            rel = os.path.relpath(path, REPO).replace("\\", "/")
            try:
                lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
            except Exception:
                continue
            for i, ln in enumerate(lines):
                g = DECL.match(ln)
                if not g:
                    continue
                sym = g.group(2)
                info = syms.get(sym)
                if not info:
                    continue
                sec, size = info
                if sec in SDA_SECTIONS and 0 < size <= THRESH:
                    out.setdefault(rel, []).append((i + 1, sym, size, ln))
    return out

def do_discover():
    c = candidates()
    total = sum(len(v) for v in c.values())
    print(f"# {total} candidate unsized-byte-array externs in {len(c)} files\n")
    for rel in sorted(c, key=lambda r: -len(c[r])):
        print(f"## {rel}  ({len(c[rel])})")
        for lineno, sym, size, _ in c[rel]:
            print(f"   L{lineno}  {sym}  size=0x{size:X}")
    print(f"\nTOTAL {total} across {len(c)} files")

def do_apply(target_rel, only_sym=None):
    target_rel = target_rel.replace("\\", "/")
    c = candidates().get(target_rel)
    if not c:
        print(f"no candidates in {target_rel}")
        return 0
    if only_sym:
        c = [x for x in c if x[1] == only_sym]
    path = os.path.join(REPO, target_rel)
    lines = open(path, encoding="utf-8", errors="replace").read().splitlines(keepends=True)
    n = 0
    syms = load_syms()
    for lineno, sym, size, _ in c:
        idx = lineno - 1
        new = re.sub(r"(\bextern\s+u8\s+%s)\[\]" % re.escape(sym),
                     r"\1[%d]" % size, lines[idx])
        if new != lines[idx]:
            lines[idx] = new
            n += 1
    open(path, "w", encoding="utf-8", newline="").write("".join(lines))
    print(f"sized {n} extern(s) in {target_rel}")
    return n

if __name__ == "__main__":
    if len(sys.argv) >= 2 and sys.argv[1] == "discover":
        do_discover()
    elif len(sys.argv) >= 3 and sys.argv[1] == "apply":
        do_apply(sys.argv[2], sys.argv[3] if len(sys.argv) >= 4 else None)
    else:
        sys.exit(__doc__)
