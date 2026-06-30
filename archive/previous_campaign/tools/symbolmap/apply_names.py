#!/usr/bin/env python3
"""apply_names.py - apply the merged symbol-map renames to the project.

Reads the rename map (proposed_symbols.txt: `fn_OLD -> NEW  // provenance`) and
applies it in one of two modes:

  --symbols   rename the leading `fn_OLD` token in symbols.txt + symbols.build.txt
              (byte-neutral: the DOL has no symbol table, and config.libs is empty
              so the C is not compiled into the byte-match build).

  --source    whole-token rename `fn_OLD` -> NEW across tracked src/**/*.{c,h}
              and (for objdiff consistency) src/**/*.inc, and rename each
              `*_fn_OLD.inc` body file to `*_NEW.inc`. The `(?![0-9A-Za-z_])`
              lookahead matches call sites, the `asm`/`#if 0` definitions, and
              the `#include "..._fn_OLD.inc"` path uniformly, while never
              partial-matching a longer token.

Idempotent-ish: only `fn_OLD` tokens are touched, so re-running after a rename
is a no-op for already-renamed functions.
"""

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def load_map(path: Path) -> dict:
    m = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        mt = re.match(r"(fn_[0-9A-Fa-f]{8})\s*->\s*(\S+)", line)
        if mt:
            m[mt.group(1)] = mt.group(2)
    return m


def apply_symbols(rename: dict, files) -> None:
    for f in files:
        if not f.is_file():
            print(f"  skip (absent): {f}")
            continue
        out, n = [], 0
        for line in f.read_text(encoding="utf-8").splitlines():
            mt = re.match(r"^(fn_[0-9A-Fa-f]{8})\b", line)
            if mt and mt.group(1) in rename:
                line = rename[mt.group(1)] + line[len(mt.group(1)):]
                n += 1
            out.append(line)
        f.write_text("\n".join(out) + "\n", encoding="utf-8")
        print(f"  {f.name}: renamed {n}")


def apply_source(rename: dict) -> None:
    # Per-name regex: match fn_OLD not followed by another ident char.
    patterns = {old: re.compile(re.escape(old) + r"(?![0-9A-Za-z_])")
                for old in rename}
    src = ROOT / "src"
    edited = renamed_files = 0
    for p in sorted(src.rglob("*")):
        if p.suffix not in (".c", ".h", ".inc"):
            continue
        text = p.read_text(encoding="utf-8", errors="replace")
        new = text
        for old, new_name in rename.items():
            if old in new:
                new = patterns[old].sub(new_name, new)
        if new != text:
            p.write_text(new, encoding="utf-8", newline="\n")
            edited += 1
        # Rename a body .inc file `<tu>_fn_OLD.inc` -> `<tu>_NEW.inc`.
        if p.suffix == ".inc":
            for old, new_name in rename.items():
                if p.name.endswith(f"_{old}.inc"):
                    tgt = p.with_name(p.name[: -len(f"_{old}.inc")] + f"_{new_name}.inc")
                    p.rename(tgt)
                    renamed_files += 1
                    break
    print(f"  edited {edited} files; renamed {renamed_files} .inc body files")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--map", type=Path, required=True)
    ap.add_argument("--symbols", action="store_true")
    ap.add_argument("--source", action="store_true")
    args = ap.parse_args()

    rename = load_map(args.map)
    print(f"[apply] {len(rename)} renames loaded")
    if args.symbols:
        apply_symbols(rename, [
            ROOT / "config/GC6E01/symbols.txt",
            ROOT / "config/GC6E01/symbols.build.txt",
        ])
    if args.source:
        apply_source(rename)
    if not (args.symbols or args.source):
        ap.error("choose --symbols and/or --source")


if __name__ == "__main__":
    main()
