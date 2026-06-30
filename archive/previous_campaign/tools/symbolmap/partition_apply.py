#!/usr/bin/env python3
"""partition_apply.py - split the merged renames into safe-to-apply vs leads.

A proposed name that ALREADY exists as a typed function prototype in the project
headers cannot be bulk-renamed: the asm-wrapper's `(void)` signature and the
untyped `lbl_` globals at its call sites conflict with the real prototype, so
wiring it is per-function decomp work (typing), not a mechanical rename.

Splits proposed_symbols.txt into:
  applied_symbols.txt        - names safe to rename in BOTH symbols and source.
  leads_needs_wiring.md      - confirmed names that need signature/typing work.
"""
import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
SM = ROOT / "config/GC6E01/symbolmap"
INCLUDE = ROOT / "include"

# Names whose typed prototype the header-regex scan missed (declared outside
# include/, or as a non-adjacent form) but which the compile oracle
# (compile_sweep before/after) proved cause a redeclaration conflict. Treated
# as leads like the header-detected conflicts.
ORACLE_CONFLICTS = {
    "DObjLoad",                 # hsd_dobj.c: int(HSD_DObj*, HSD_DObjDesc*)
    "floorReadCameraPreFunc",   # gs_field_resource.c: void*(u32,u32,u32)
    "floorReadMapPreFunc",      # gs_field_resource.c (paired callback)
}


def header_protos() -> tuple:
    """Return (joined header text, {file: text})."""
    joined, per = "", {}
    for p in INCLUDE.rglob("*.h"):
        t = p.read_text(encoding="utf-8", errors="replace")
        joined += t + "\n"
        per[p.as_posix()] = t
    return joined, per


def main() -> None:
    rename = []
    for line in (SM / "proposed_symbols.txt").read_text(encoding="utf-8").splitlines():
        m = re.match(r"(fn_[0-9A-Fa-f]{8}) -> (\S+)(?:\s+// (.*))?", line)
        if m:
            rename.append((m.group(1), m.group(2), (m.group(3) or "").strip()))

    joined, per = header_protos()
    clean, conflict = [], []
    for old, new, prov in rename:
        if new in ORACLE_CONFLICTS or re.search(
                r"\b" + re.escape(new) + r"\s*\(", joined):
            conflict.append((old, new, prov))
        else:
            clean.append((old, new, prov))

    with (SM / "applied_symbols.txt").open("w", encoding="utf-8") as f:
        for old, new, prov in sorted(clean):
            f.write(f"{old} -> {new}  // {prov}\n")

    with (SM / "leads_needs_wiring.md").open("w", encoding="utf-8") as f:
        f.write("# Confirmed-name leads needing signature/typing work\n\n")
        f.write("These functions' names are doubly-confirmed — the XD decomp AND a "
                "typed prototype already in this project's headers agree — but the "
                "asm-wrapper's `(void)` signature and untyped `lbl_` globals at the "
                "call sites conflict with the real prototype. Wiring each is "
                "per-function decomp work (match the header prototype, type its "
                "globals), so they are left as `fn_` for now rather than bulk-renamed.\n\n")
        f.write("| addr (fn_) | confirmed name | provenance | header proto in |\n")
        f.write("|---|---|---|---|\n")
        for old, new, prov in sorted(conflict):
            loc = next((fp for fp, t in per.items()
                        if re.search(r"\b" + re.escape(new) + r"\s*\(", t)), "?")
            f.write(f"| `{old}` | {new} | {prov} | {loc} |\n")

    print(f"clean(apply)={len(clean)}  conflict(leads)={len(conflict)}")
    print("wrote applied_symbols.txt + leads_needs_wiring.md")


if __name__ == "__main__":
    main()
