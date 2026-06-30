#!/usr/bin/env python3
"""build_symbol_map.py - merge all stages into one reviewable symbol map.

Reconciles the three evidence sources into a single rename proposal, with
provenance, prioritising the most authoritative:

  1. XD port          real name from the sister game's decomp (highest trust)
  2. string self-name  name the function logs about itself (Stage 2)

Only HIGH-confidence, unambiguous proposals for currently-`fn_`-named functions
are merged, and a name is never assigned to two functions. Output is advisory -
it never edits symbols.txt, the asm, or any .inc truth file; it writes:

  proposed_symbols.txt        fn_OLD -> NEW   // <provenance>
  symbols.with_proposals.txt  a full copy of symbols.txt with the renames
                              applied + provenance comments, for `git diff`
                              style review before adoption.
"""

import argparse
import json
import re
from pathlib import Path

SYM_LINE = re.compile(r"^(?P<name>\S+)(?P<rest>\s*=\s*.*)$")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sm-dir", required=True, type=Path)
    ap.add_argument("--symbols", required=True, type=Path)
    args = ap.parse_args()

    sm = args.sm_dir
    xd = json.loads((sm / "xd_port.json").read_text(encoding="utf-8"))
    proposals = json.loads(
        (sm / "name_proposals.json").read_text(encoding="utf-8"))

    # fn -> (new_name, provenance), XD first (authoritative).
    rename = {}
    for p in xd.get("ported", []):
        if p.get("confidence") == "HIGH" and not p.get("ambiguous") \
                and not p.get("col_named"):
            rename[p["fn"]] = (p["name"], f"XD port (score {p['score']})")
    for p in proposals:
        if p["confidence"] == "HIGH" and not p["already_named"] \
                and not p.get("collision") and p["fn"] not in rename:
            rename[p["fn"]] = (p["proposed"], f"string self-name ({p['kind']})")

    # Enforce unique target names (skip a later fn that would duplicate).
    used, final = set(), {}
    for fn in sorted(rename):
        name, prov = rename[fn]
        if name in used:
            continue
        used.add(name)
        final[fn] = (name, prov)

    # proposed_symbols.txt
    lines = [f"{fn} -> {name}  // {prov}"
             for fn, (name, prov) in sorted(final.items())]
    (sm / "proposed_symbols.txt").write_text("\n".join(lines) + "\n",
                                             encoding="utf-8")

    # symbols.with_proposals.txt (apply renames in-place, keep provenance)
    out = []
    applied = 0
    for line in args.symbols.read_text(encoding="utf-8").splitlines():
        m = SYM_LINE.match(line)
        if m and m.group("name") in final:
            name, prov = final[m.group("name")]
            sep = "" if line.rstrip().endswith(";") else ""
            comment = f"  // renamed-from {m.group('name')} via {prov}"
            out.append(f"{name}{m.group('rest')}{sep}{comment}")
            applied += 1
        else:
            out.append(line)
    (sm / "symbols.with_proposals.txt").write_text("\n".join(out) + "\n",
                                                   encoding="utf-8")

    by_src = {}
    for fn, (name, prov) in final.items():
        key = "XD port" if prov.startswith("XD") else "string self-name"
        by_src[key] = by_src.get(key, 0) + 1
    print(f"[map] merged {len(final)} renames: {by_src}")
    print(f"[map] wrote proposed_symbols.txt ({applied} applied in "
          "symbols.with_proposals.txt)")


if __name__ == "__main__":
    main()
