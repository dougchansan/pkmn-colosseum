#!/usr/bin/env python3
"""propose_names.py - Stage 2: function-name proposals from log/assert strings.

CodeWarrior debug/log strings frequently embed the *name of the function that
emits them*. This pass mines three patterns and attaches a proposed name to the
function that references the string:

  A. leading self-name   "GSmaterialCreate: Run out of materials"   -> GSmaterialCreate
                         "GSthreadCreate. Warning: ..."             -> GSthreadCreate
  B. file:func           "GCN_Mem_Alloc.c : InitDefaultHeap. No..." -> InitDefaultHeap
  C. call in assert/log  "_menuPop():stack under."                  -> _menuPop
                         "TargetDoStep()\n"                          -> TargetDoStep

Accuracy gates (proposals only - never written to asm truth files):
  * Pattern A/B fire only when the string is referenced by <= MAX_REFS
    functions, so a format string shared across a whole TU (e.g. a 70-function
    "Warining: people[%d]" cluster) is NOT mistaken for one function's name.
  * Candidates that collide with an operator/keyword or the .c filename are
    dropped.
  * Confidence is HIGH when a function gets exactly one candidate from a
    uniquely-referenced string; MEDIUM otherwise.

Output is advisory: name_proposals.md (review) + name_proposals.json, and an
optional symbols `// Proposed:` patch the user can apply. It does not modify
symbols.txt or any .s/.inc file.
"""

import argparse
import json
import re
from collections import defaultdict
from pathlib import Path

MAX_REFS = 3  # a self-naming log string is referenced by very few functions

# Tokens that look like C identifiers but are not function names.
STOPWORDS = {
    "Warning", "Warining", "ERROR", "Error", "OK", "TRUE", "FALSE", "FALE",
    "NULL", "for", "the", "and", "Run", "No", "Can", "Calling", "int",
    "void", "char", "size", "Invalid", "invalid",
}
# Identifier prefixes that strongly indicate a real engine function name.
GOOD_PREFIX = ("GS", "HSD_", "hsd", "CARDE", "menu", "_menu", "snd", "cc_",
               "OS", "DVD", "TRK", "Target", "Message", "Send", "Init",
               "GX", "VI", "PAD", "Card", "card", "pkjb", "gba", "poke")

IDENT = r"[A-Za-z_][A-Za-z0-9_]{2,}"
PAT_LEADING = re.compile(rf"^({IDENT})\s*[.:]\s")          # A
PAT_FILEFUNC = re.compile(rf"\.c\s*:\s*({IDENT})")          # B
PAT_CALL = re.compile(rf"({IDENT})\s*\(")                   # C


def good(name: str) -> bool:
    if name in STOPWORDS or name.endswith("_c") or name.lower().endswith("c"):
        pass
    if name in STOPWORDS:
        return False
    if name.isupper() and len(name) > 5:        # OS_ERROR_* style macros
        return False
    return name.startswith(GOOD_PREFIX) or bool(re.search(r"[a-z][A-Z]", name)) \
        or ("_" in name and not name.isupper())


def candidates_from(text: str) -> list:
    cands = []
    m = PAT_LEADING.match(text)
    if m and not text[:m.end(1)].endswith(".c"):
        cands.append((m.group(1), "leading"))
    for m in PAT_FILEFUNC.finditer(text):
        cands.append((m.group(1), "file:func"))
    for m in PAT_CALL.finditer(text):
        cands.append((m.group(1), "call"))
    return [(c, k) for c, k in cands if good(c)]


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sm-dir", required=True, type=Path)
    ap.add_argument("--symbols", required=True, type=Path,
                    help="config/GC6E01/symbols.txt (to skip already-named fns)")
    args = ap.parse_args()

    fns = json.loads((args.sm_dir / "fn_strings.json").read_text(encoding="utf-8"))
    strings = json.loads((args.sm_dir / "strings.json").read_text(encoding="utf-8"))

    # Already-named functions: skip proposing for these.
    named = set()
    for line in args.symbols.read_text(encoding="utf-8").splitlines():
        m = re.match(r"^(\S+)\s*=", line)
        if m and not re.match(r"(fn|lbl)_[0-9A-Fa-f]{8}$", m.group(1)):
            named.add(m.group(1))

    # label -> functions referencing it.
    label_refs = defaultdict(list)
    for name, f in fns.items():
        for s in f["strings"]:
            label_refs[s["label"]].append(name)

    # Gather (fn -> [(cand, kind, via_label, refcount)]).
    proposals = defaultdict(list)
    for label, info in strings.items():
        refs = label_refs.get(label, [])
        if not refs:
            continue
        cands = candidates_from(info["text"])
        if not cands:
            continue
        # Patterns A/B (self-naming) only trust low-fanout strings.
        for fn in refs:
            for cand, kind in cands:
                if kind in ("leading", "file:func") and len(refs) > MAX_REFS:
                    continue
                proposals[fn].append({
                    "name": cand, "kind": kind, "via": label,
                    "text": info["text"][:80], "refs": len(refs),
                })

    rows = []
    for fn, cs in proposals.items():
        addr = fns[fn]["addr"]
        # Best candidate: prefer file:func > leading > call; lower fanout wins.
        order = {"file:func": 0, "leading": 1, "call": 2}
        cs.sort(key=lambda c: (order[c["kind"]], c["refs"]))
        best = cs[0]
        distinct_names = {c["name"] for c in cs}
        already = "yes" if fn in named else ""
        conf = "HIGH" if (best["kind"] in ("file:func", "leading")
                          and best["refs"] == 1 and len(distinct_names) == 1) \
            else "MED"
        rows.append({"fn": fn, "addr": addr, "proposed": best["name"],
                     "kind": best["kind"], "confidence": conf,
                     "already_named": already, "evidence": best["text"],
                     "alt": sorted(distinct_names - {best["name"]})})

    # Flag collisions: a proposed name shared by >1 fn is a common prefix, not a
    # unique symbol; applying it twice would break the build. Mark + demote.
    name_count = defaultdict(int)
    for r in rows:
        name_count[r["proposed"]] += 1
    for r in rows:
        r["collision"] = name_count[r["proposed"]] > 1

    rows.sort(key=lambda r: (r["confidence"] != "HIGH", r["fn"]))
    hi = [r for r in rows if r["confidence"] == "HIGH"
          and not r["already_named"] and not r["collision"]]

    md = ["# Function-name proposals from log/assert strings", "",
          f"{len(rows)} candidate(s); {len(hi)} HIGH-confidence & currently unnamed.",
          "Advisory only — review before applying. Never written to asm.", "",
          "| fn | addr | proposed | conf | via | evidence |",
          "|---|---|---|---|---|---|"]
    for r in rows:
        nm = f"**{r['proposed']}**" if r["confidence"] == "HIGH" else r["proposed"]
        flag = " ⟨named⟩" if r["already_named"] else ""
        if r["collision"]:
            flag += " ⟨shared-prefix⟩"
        md.append(f"| `{r['fn']}`{flag} | {r['addr']} | {nm} | {r['confidence']} "
                  f"| {r['kind']} | `{r['evidence']}` |")
    (args.sm_dir / "name_proposals.md").write_text("\n".join(md) + "\n",
                                                    encoding="utf-8")
    (args.sm_dir / "name_proposals.json").write_text(
        json.dumps(rows, ensure_ascii=False, indent=1), encoding="utf-8")

    # Emit an apply-able `// Proposed:` patch for HIGH unnamed fns only.
    patch = [f"{r['fn']}  // Proposed: {r['proposed']}  ({r['kind']}, {r['addr']})"
             for r in hi]
    (args.sm_dir / "name_proposals_high.txt").write_text(
        "\n".join(patch) + "\n", encoding="utf-8")

    print(f"[names] {len(rows)} proposals, {len(hi)} HIGH & unnamed")
    print(f"[names] wrote name_proposals.md / .json / name_proposals_high.txt")


if __name__ == "__main__":
    main()
