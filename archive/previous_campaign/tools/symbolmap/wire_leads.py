#!/usr/bin/env python3
"""wire_leads.py - fix asm-wrapper/extern return types to match header protos.

After apply_names.py has renamed a void-arg lead `fn_X -> NewName`, its wrapper
and file-scope extern are still `void NewName(void)`, which clashes with a header
prototype that has a different return type (e.g. `u16 NewName(void)`). The
asm-wrapper body is verbatim `.inc`, so changing only the declared return type is
byte-neutral; it just makes the C declaration agree with the header.

Input: a ret-map file, one `NewName <ret-type>` per line.
"""
import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--ret-map", type=Path, required=True)
    args = ap.parse_args()

    ret = {}
    for line in args.ret_map.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if line and not line.startswith("#"):
            name, rtype = line.split()
            ret[name] = rtype

    # `[asm |extern ]void NewName(void)` -> `... <ret> NewName(void)`
    pats = {n: re.compile(r"\bvoid(\s+" + re.escape(n) + r"\s*\(\s*void\s*\))")
            for n in ret}
    edited = 0
    for p in sorted((ROOT / "src").rglob("*")):
        if p.suffix not in (".c", ".h"):
            continue
        t = p.read_text(encoding="utf-8", errors="replace")
        new = t
        for n, rtype in ret.items():
            if n in new and rtype != "void":
                new = pats[n].sub(rtype + r"\1", new)
        if new != t:
            p.write_text(new, encoding="utf-8", newline="\n")
            edited += 1
    print(f"[wire] return-type fixes applied across {edited} files "
          f"({len([r for r in ret.values() if r!='void'])} funcs needed a change)")


if __name__ == "__main__":
    main()
