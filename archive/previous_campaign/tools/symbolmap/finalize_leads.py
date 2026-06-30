#!/usr/bin/env python3
"""finalize_leads.py - reconcile leads doc + symbols annotations after wiring.

After some leads are wired (added to applied_symbols.txt), regenerate
leads_needs_wiring.md for the remaining (proposed - applied) and refresh the
`// lead:` annotations in symbols.txt so only still-unwired functions carry them.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
SM = ROOT / "config/GC6E01/symbolmap"


def load(path, pat):
    out = {}
    for line in (SM / path).read_text(encoding="utf-8").splitlines():
        m = re.match(pat, line)
        if m:
            out[m.group(1)] = (m.group(2), (m.group(3) or "").strip()
                               if m.lastindex and m.lastindex >= 3 else "")
    return out


def main() -> None:
    proposed = load("proposed_symbols.txt",
                    r"(fn_[0-9A-Fa-f]{8}) -> (\S+)(?:\s+// (.*))?")
    applied = {m.group(1) for m in
               (re.match(r"(fn_[0-9A-Fa-f]{8})", l)
                for l in (SM / "applied_symbols.txt").read_text(
                    encoding="utf-8").splitlines() if l.strip()) if m}
    leads = {fn: v for fn, v in proposed.items() if fn not in applied}

    # header location lookup
    per = {p.as_posix(): p.read_text(encoding="utf-8", errors="replace")
           for p in (ROOT / "include").rglob("*.h")}

    lines = ["# Confirmed-name leads needing signature/typing work", "",
             f"{len(leads)} remaining (of the original proposals). These names are "
             "confirmed but the asm-wrapper `(void)` signature / untyped `lbl_` "
             "globals / a name collision block a clean bulk rename — each needs "
             "per-function decomp work (match the real prototype, type its "
             "globals). Wired leads have been moved to applied_symbols.txt.", "",
             "| addr (fn_) | confirmed name | provenance | header proto in |",
             "|---|---|---|---|"]
    for fn, (new, prov) in sorted(leads.items(), key=lambda kv: kv[1][0]):
        loc = next((fp for fp, t in per.items()
                    if re.search(r"\b" + re.escape(new) + r"\s*\(", t)), "?")
        lines.append(f"| `{fn}` | {new} | {prov} | {loc} |")
    (SM / "leads_needs_wiring.md").write_text("\n".join(lines) + "\n",
                                              encoding="utf-8")

    # refresh symbols.txt annotations: strip all, re-add for current leads
    sym = ROOT / "config/GC6E01/symbols.txt"
    out = []
    for line in sym.read_text(encoding="utf-8").splitlines():
        line = re.sub(r"\s*// lead:.*$", "", line)
        m = re.match(r"^(fn_[0-9A-Fa-f]{8})\b", line)
        if m and m.group(1) in leads:
            line = line.rstrip() + (
                f"  // lead: {leads[m.group(1)][0]} "
                "(see config/GC6E01/symbolmap/leads_needs_wiring.md)")
        out.append(line)
    sym.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"[finalize] {len(applied)} applied, {len(leads)} leads remaining; "
          "leads_needs_wiring.md + symbols.txt annotations refreshed")


if __name__ == "__main__":
    main()
