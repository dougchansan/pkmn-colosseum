#!/usr/bin/env python3
"""Generate docs/struct_sizes.md from typedef struct definitions in include/.

Inspired by zeldaret/tp's docs/re_notes.md catalog. Index of every named struct,
its defining header, the highest documented `/* 0xNN */` offset, and field count.
"""

import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
INCLUDE = ROOT / "include"
OUT = ROOT / "docs" / "struct_sizes.md"

TYPEDEF_RE = re.compile(
    r"typedef\s+struct\s+(\w+)\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}\s*(\w+)?\s*;",
    re.DOTALL,
)
OFFSET_RE = re.compile(r"/\*\s*0x([0-9A-Fa-f]+)")


def collect():
    items = []
    for h in INCLUDE.rglob("*.h"):
        try:
            src = h.read_text(errors="ignore")
        except OSError:
            continue
        for m in TYPEDEF_RE.finditer(src):
            tag = m.group(1)
            body = m.group(2)
            name = m.group(3) or tag
            offs = [int(o, 16) for o in OFFSET_RE.findall(body)]
            last = max(offs) if offs else None
            n_fields = body.count(";")
            rel = h.relative_to(INCLUDE).as_posix()
            items.append((name, rel, last, n_fields))
    items.sort(key=lambda x: (x[1], x[0]))
    return items


def render(items):
    out = []
    out.append("# Pokemon Colosseum Struct Size Catalog\n\n")
    out.append("Auto-generated from `include/**/*.h` typedef struct definitions.\n")
    out.append("Run `python3 tools/gen_struct_catalog.py` to refresh after header changes.\n\n")
    out.append("For structs with `/* 0xNN */` offset comments, **Last offset** is the highest documented field offset (a floor on struct size — trailing padding/undocumented fields can push real size higher).\n\n")
    out.append("Inspired by [zeldaret/tp `docs/re_notes.md`](https://github.com/zeldaret/tp/blob/main/docs/re_notes.md).\n\n")

    out.append("## Index\n\n")
    out.append("| Struct | Header | Last offset | Fields |\n")
    out.append("|--------|--------|-------------|--------|\n")
    for name, file, off, nf in items:
        off_str = f"0x{off:X}" if off is not None else "—"
        out.append(f"| `{name}` | `include/{file}` | {off_str} | {nf} |\n")

    out.append("\n## How to use\n\n")
    out.append("1. When recovering a typed signature for a function, grep this index for the struct hosting the field offset you see in asm.\n")
    out.append("2. The canonical header always wins; this file is just an index.\n")
    out.append("3. When you decompile or expand a struct, document field offsets with `/* 0xNN */` comments before each member in the header. They feed both this catalog and reader comprehension.\n")

    return "".join(out)


def main():
    items = collect()
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(render(items), encoding="utf-8")
    print(f"Wrote {OUT} with {len(items)} entries")


if __name__ == "__main__":
    main()
