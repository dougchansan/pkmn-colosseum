#!/usr/bin/env python3
"""coloring_oracle.py — the keystone research tool for the reg-alloc walls.

Given a near-miss's register-remap (your_reg -> target_reg counts, as recorded
per-fn in walltriage_out.json, or parsed live from an objdiff JSON), decide
whether the residual is:

  PURE_RENAME      a single consistent bijection on the saved GPRs r14..r31 —
                   the allocator chose a different *coloring* of the SAME
                   instructions. Distance = minimal swap count (cycle
                   decomposition). This is exactly what blind annealing can
                   NEVER reach (proven 0% hit) but constraint-directed
                   first-definition inversion (firstdef_invert.py) CAN.
  SCHEDULING       registers line up but instructions are reordered — not
                   C-controllable in general; TRIAGE OUT, never anneal.
  STRUCTURAL       different instructions / control flow; a different problem.

The "constraints" it emits (value-slot -> target physical reg) are the input to
firstdef_invert.py, which inverts ALLOCATOR_MODEL.md's first-definition rule to
produce the C statement order that yields the target coloring.

Validated against the controlled case fn_800E3604 (gs_material): the research
ground truth is a clean 3-cycle r29->r31->r30->r29, distance 2.

usage:
  python3 coloring_oracle.py                # classify all of walltriage_out.json
  python3 coloring_oracle.py fn_800E3604    # one function, verbose
  python3 coloring_oracle.py --json         # machine-readable to stdout
Writes .omc/coloring_oracle.json (routing input for triage_gate.py)."""
import json, os, sys

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
WALLTRIAGE = os.path.join(REPO, "tools", "decomp_work", "ra", "walltriage_out.json")
OUT = os.path.join(REPO, ".omc", "coloring_oracle.json")

SAVED = {f"r{n}" for n in range(14, 32)}        # non-volatile GPRs CW colors
FLOAT_SAVED = {f"f{n}" for n in range(14, 32)}


def _dominant_bijection(remap):
    """From a weighted your_reg->target_reg count map, recover the most-likely
    1:1 mapping (greedy by weight) and report how cleanly it is a bijection.
    Returns (mapping, leftover_weight, total_weight)."""
    edges = []
    for k, w in remap.items():
        if "->" not in k:
            continue
        a, b = k.split("->", 1)
        edges.append((w, a.strip(), b.strip()))
    edges.sort(reverse=True)
    total = sum(w for w, _, _ in edges)
    src_used, tgt_used, mapping, leftover = set(), set(), {}, 0
    for w, a, b in edges:
        if a in src_used or b in tgt_used:
            leftover += w            # a conflicting edge => not a clean bijection
            continue
        mapping[a] = b
        src_used.add(a); tgt_used.add(b)
    return mapping, leftover, total


def _cycle_distance(mapping):
    """Swap distance of a permutation = N - (#cycles), counting only regs that
    actually move. Identity maps contribute 0."""
    moved = {a: b for a, b in mapping.items() if a != b}
    seen, cycles = set(), 0
    for start in moved:
        if start in seen:
            continue
        cycles += 1
        cur = start
        while cur in moved and cur not in seen:
            seen.add(cur)
            cur = moved[cur]
    return len(moved) - cycles, cycles


def classify(entry):
    """entry: a walltriage record. Returns an oracle verdict dict."""
    fn = entry["fn"]
    buckets = entry.get("buckets", {})
    struct = buckets.get("STRUCT", 0)
    regimm = buckets.get("REG-IMM", 0)
    purereg = buckets.get("PURE-REG", 0)
    remap = entry.get("reg_remap", {})
    mapping, leftover, total = _dominant_bijection(remap)
    # how much of the diff mass is explained by one clean bijection
    clean = (total - leftover) / total if total else 0.0
    saved_only = all((a in SAVED or a in FLOAT_SAVED) and (b in SAVED or b in FLOAT_SAVED)
                     for a, b in mapping.items() if a != b)
    dist, ncyc = _cycle_distance(mapping)
    moved = sum(1 for a, b in mapping.items() if a != b)
    # a TRUE permutation has exactly one distinct pair per moved reg; more distinct
    # pairs than moved regs => a src maps to >1 target (many-to-many ambiguity) =>
    # the first-def order for that reg is undetermined, so it is NOT invertible.
    dpairs = entry.get("distinct_reg_pairs", moved)
    ambiguous = dpairs > moved

    verdict = "STRUCTURAL"
    reason = ""
    if struct == 0 and regimm == 0 and purereg > 0 and clean >= 0.95:
        # the whole residual is register renaming with one consistent bijection
        verdict = "PURE_RENAME"
        reason = (f"clean bijection ({clean:.0%} of diff mass), "
                  f"{'saved-reg only' if saved_only else 'incl. volatile regs'}, "
                  f"swap-distance {dist} ({ncyc} cycle(s))")
    elif (clean >= 0.95 and saved_only and purereg >= 8 * (struct + regimm)
          and struct <= 2 and dist > 0 and not ambiguous):
        # a clean saved-reg bijection DOMINATES, with only minor non-reg residual
        # (1-2 struct/imm instrs, often alignment noise). Inversion is still the
        # right lever; it just won't reach 0 without also clearing the residual.
        verdict = "REG_DOMINANT_RENAME"
        reason = (f"clean saved-reg bijection (swap-distance {dist}, {ncyc} cycle(s)) "
                  f"dominates; {struct} struct + {regimm} imm residual remains after inversion")
    elif struct == 0 and purereg > 0 and clean >= 0.6:
        verdict = "SCHEDULING"     # regs mostly align but reorder/imm noise remains
        reason = f"reg bijection {clean:.0%} clean but residual reorder/imm; triage out"
    else:
        reason = f"structural mass {struct} instr; coloring not the lever"

    return {
        "fn": fn, "file": entry.get("file"), "match": entry.get("match"),
        "mismatches": entry.get("mismatches"), "wall_class": entry.get("class"),
        "verdict": verdict, "reason": reason,
        "bijection": mapping, "swap_distance": dist, "cycles": ncyc,
        "clean_frac": round(clean, 3), "saved_reg_only": saved_only,
        # constraints for firstdef_invert.py: the target reg each of your saved
        # values must end up in (only the saved-reg part of the bijection).
        "target_coloring": {a: b for a, b in mapping.items()
                            if a != b and a in SAVED and b in SAVED},
    }


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    as_json = "--json" in sys.argv
    data = json.load(open(WALLTRIAGE))
    verdicts = [classify(e) for e in data]

    if args:                       # single-function verbose
        want = args[0]
        v = next((x for x in verdicts if x["fn"] == want), None)
        if not v:
            print(f"{want}: not in walltriage_out.json (mismatches<=8 or unknown)")
            return
        print(json.dumps(v, indent=2))
        return

    by = {}
    for v in verdicts:
        by.setdefault(v["verdict"], []).append(v)
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    # both PURE_RENAME and REG_DOMINANT_RENAME are allocator-inversion candidates
    invertible = [x for x in verdicts if x["verdict"] in ("PURE_RENAME", "REG_DOMINANT_RENAME")]
    payload = {"total": len(verdicts),
               "counts": {k: len(v) for k, v in by.items()},
               "invertible": sorted(
                   [{"fn": x["fn"], "file": x["file"], "verdict": x["verdict"],
                     "swap_distance": x["swap_distance"],
                     "target_coloring": x["target_coloring"], "reason": x["reason"]}
                    for x in invertible],
                   key=lambda x: (x["swap_distance"], x["verdict"] != "PURE_RENAME")),
               "verdicts": verdicts}
    payload["pure_rename"] = [x for x in payload["invertible"] if x["verdict"] == "PURE_RENAME"]
    json.dump(payload, open(OUT, "w"), indent=1)

    if as_json:
        print(json.dumps(payload["counts"], indent=1))
        return
    print(f"coloring oracle over {len(verdicts)} wall functions ->")
    for k in ("PURE_RENAME", "REG_DOMINANT_RENAME", "SCHEDULING", "STRUCTURAL"):
        print(f"  {k:<20} {len(by.get(k, []))}")
    print("\nallocator-inversion candidates (easiest first):")
    for x in payload["invertible"]:
        tag = "" if x["verdict"] == "PURE_RENAME" else "  (+residual)"
        print(f"  {x['fn']:<14} dist={x['swap_distance']}  {x['file']}{tag}")
    print(f"\nwrote {OUT}")


if __name__ == "__main__":
    main()
