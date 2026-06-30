#!/usr/bin/env python3
"""attribute_tus.py - Stage 1: TU attribution from __FILE__ string xrefs.

CodeWarrior emits one `__FILE__` string literal per source file that uses an
assert/log macro, and *every* function compiled from that file references that
same literal. So a function that references the string "cobj.c" was compiled
from cobj.c - full stop. This pass overlays that ground truth onto the existing
address-range attribution in splits_refined.txt to:

  1. CONFIRM   ranges whose functions reference the .c file they're assigned to.
  2. CONTRADICT ranges whose functions reference a *different* .c file.
  3. ATTRIBUTE  GAP-range functions that reference a .c file (new evidence).

It also clusters functions by shared non-.c format strings (e.g. a "people[%d]"
warning shared across one menu TU) to bound TUs that have no __FILE__ literal.

Inputs:
  config/GC6E01/symbolmap/fn_strings.json   (from mine_xrefs.py)
  config/GC6E01/splits_refined.txt
Outputs:
  config/GC6E01/symbolmap/tu_evidence.json
  config/GC6E01/symbolmap/tu_attribution.md
"""

import argparse
import bisect
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

SPLIT_RE = re.compile(
    r"^(?P<status>KNOWN|LIKELY|GAP)\s+0x(?P<start>[0-9A-Fa-f]+)\s+"
    r"0x(?P<end>[0-9A-Fa-f]+)\s+(?P<src>\S+)")


def load_splits(path: Path):
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = SPLIT_RE.match(line)
        if m:
            rows.append({
                "status": m.group("status"),
                "start": int(m.group("start"), 16),
                "end": int(m.group("end"), 16),
                "src": m.group("src"),
            })
    rows.sort(key=lambda r: r["start"])
    return rows


def range_for(addr: int, rows, starts):
    i = bisect.bisect_right(starts, addr) - 1
    if 0 <= i < len(rows) and rows[i]["start"] <= addr < rows[i]["end"]:
        return rows[i]
    return None


def norm(name: str) -> str:
    """Fold naming conventions: basename, lowercase, drop ext/hsd_, alnum-only."""
    base = Path(name).name.lower()
    if base.endswith(".c"):
        base = base[:-2]
    if base.startswith("hsd_"):
        base = base[4:]
    return re.sub(r"[^a-z0-9]", "", base)


def lcs_len(a: str, b: str) -> int:
    """Longest common contiguous substring length (cheap, strings are short)."""
    best = 0
    for i in range(len(a)):
        for j in range(len(b)):
            k = 0
            while (i + k < len(a) and j + k < len(b)
                   and a[i + k] == b[j + k]):
                k += 1
            best = max(best, k)
    return best


def classify(assigned_src: str, voted_files: list) -> tuple:
    """Return (tag, note) describing how the voted .c files relate to assigned."""
    distinct = sorted(set(voted_files))
    if assigned_src in ("???",) or not assigned_src.endswith(".c"):
        return ("gap", "")
    na = norm(assigned_src)
    matches = [f for f in distinct if norm(f) == na or lcs_len(norm(f), na) >= 4]
    extras = [f for f in distinct if f not in matches]
    if len(distinct) >= 2 and extras:
        # Range spans more than one source file -> split candidate.
        return ("split", f"also holds: {', '.join(extras)}")
    if matches and not extras:
        return ("match", "")
    if not matches and len(distinct) == 1:
        return ("relabel", f"assigned {assigned_src} but code is {distinct[0]}")
    return ("split", f"multiple: {', '.join(distinct)}")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sm-dir", required=True, type=Path)
    ap.add_argument("--splits", required=True, type=Path)
    args = ap.parse_args()

    fns = json.loads((args.sm_dir / "fn_strings.json").read_text(encoding="utf-8"))
    rows = load_splits(args.splits)
    starts = [r["start"] for r in rows]

    # fn -> the .c file(s) it references (its __FILE__).
    fn_cfile = {}
    for name, f in fns.items():
        cfiles = sorted(set(f["file_strings"]))
        if cfiles:
            fn_cfile[name] = cfiles

    # Per range: tally the .c files its member functions reference.
    range_votes = defaultdict(Counter)   # range_start -> Counter(cfile)
    gap_hits = []                        # (addr, name, cfiles) in GAP ranges
    for name, cfiles in fn_cfile.items():
        addr = int(fns[name]["addr"], 16)
        r = range_for(addr, rows, starts)
        if r is None:
            continue
        for c in cfiles:
            range_votes[r["start"]][c] += 1
        if r["status"] == "GAP":
            gap_hits.append((addr, name, cfiles))

    # Cluster by shared non-.c format strings to bound un-asserted TUs.
    # label -> functions referencing it; keep clusters that span a tight range.
    label_fns = defaultdict(list)
    for name, f in fns.items():
        for s in f["strings"]:
            if not s["text"].endswith(".c"):
                label_fns[s["label"]].append((int(f["addr"], 16), name, s["text"]))

    evidence = {"match": [], "relabel": [], "split": [], "gap_attribution": []}
    tagmark = {"match": "✅ match", "relabel": "🔀 relabel",
               "split": "✂️ split", "gap": ""}
    md = ["# TU attribution from __FILE__ string xrefs",
          "",
          "Ground truth: every function referencing a `*.c` string literal was "
          "compiled from that file (CodeWarrior `__FILE__`). Filename folding "
          "treats `cobj.c`≡`hsd_cobj.c`, `DVD.c`≡`dvd.c`, etc.",
          "",
          "- ✅ **match** — string evidence agrees with the assigned source.",
          "- 🔀 **relabel** — range assigned the wrong file; code is a different TU.",
          "- ✂️ **split** — range spans 2+ source files; needs a split point.",
          ""]

    md.append("## Range cross-check (existing splits vs. string evidence)\n")
    md.append("| range | status | assigned src | .c strings referenced | finding |")
    md.append("|---|---|---|---|---|")
    for r in rows:
        votes = range_votes.get(r["start"])
        if not votes:
            continue
        votestr = ", ".join(f"`{c}`×{n}" for c, n in votes.most_common())
        assigned = Path(r["src"]).name if r["src"] != "???" else r["src"]
        tag, note = classify(assigned, list(votes.keys()))
        flag = tagmark.get(tag, "")
        if note:
            flag = f"{flag} ({note})" if flag else note
        if tag in evidence:
            evidence[tag].append({"range": hex(r["start"]), "end": hex(r["end"]),
                                  "status": r["status"], "assigned": r["src"],
                                  "votes": dict(votes), "note": note})
        md.append(f"| `{hex(r['start'])}`-`{hex(r['end'])}` | {r['status']} "
                  f"| {assigned} | {votestr} | {flag} |")

    # GAP attribution: contiguous runs of GAP functions voting one .c file.
    md.append("\n## GAP-range attribution proposals\n")
    md.append("Functions in unattributed GAP regions that reference a `*.c` "
              "literal — these regions can be split to that TU.\n")
    gap_by_file = defaultdict(list)
    for addr, name, cfiles in sorted(gap_hits):
        for c in cfiles:
            gap_by_file[c].append((addr, name))
    for c, items in sorted(gap_by_file.items(), key=lambda kv: -len(kv[1])):
        items.sort()
        lo, hi = items[0][0], items[-1][0]
        md.append(f"- **`{c}`** — {len(items)} GAP function(s), "
                  f"`{hex(lo)}`..`{hex(hi)}` "
                  f"(e.g. {', '.join(n for _, n in items[:4])}"
                  f"{', …' if len(items) > 4 else ''})")
        evidence["gap_attribution"].append({
            "cfile": c, "count": len(items),
            "lo": hex(lo), "hi": hex(hi),
            "fns": [n for _, n in items],
        })

    (args.sm_dir / "tu_evidence.json").write_text(
        json.dumps(evidence, ensure_ascii=False, indent=1), encoding="utf-8")
    (args.sm_dir / "tu_attribution.md").write_text(
        "\n".join(md) + "\n", encoding="utf-8")

    print(f"[tu] functions with __FILE__ evidence: {len(fn_cfile)}")
    print(f"[tu] match={len(evidence['match'])} "
          f"relabel={len(evidence['relabel'])} "
          f"split={len(evidence['split'])} "
          f"gap_files={len(evidence['gap_attribution'])}")
    print(f"[tu] wrote tu_attribution.md / tu_evidence.json")


if __name__ == "__main__":
    main()
