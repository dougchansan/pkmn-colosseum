#!/usr/bin/env python3
"""firstdef_invert.py — constraint-directed first-definition-order search.

The second half of the allocator-inversion method. coloring_oracle.py decides a
near-miss is a PURE_RENAME and emits target_coloring = {your_reg: target_reg}
over the saved band r14..r31. This tool INVERTS ALLOCATOR_MODEL.md's rule to
turn that coloring into the C edit that produces it.

ALLOCATOR_MODEL rule (empirically proven, 13 one-factor experiments):
  Among values that survive a call, CW fills the saved band r(32-k)..r31
  ASCENDING by ORDER OF FIRST DEFINITION (first assignment), params first.
  Declaration order and use order are irrelevant.

Inversion: the value the target keeps in the LOWEST saved reg must be
first-assigned FIRST; next-lowest second; ... highest last. So the required
first-definition order is simply the call-surviving values sorted by their
TARGET register number, ascending.

The payoff (the whole point vs annealing): the consistent first-def order is
USUALLY UNIQUE — a directed search of 1 candidate (or a tiny factorial over
genuinely-tied values), NOT the N! random space the permuter flails in. We make
that size explicit so the pipeline can show "1 directed candidate" instead of
"millions of random mutations, 0% hit."

usage:
  python3 firstdef_invert.py fn_800E3604     # recipe for one oracle target
  python3 firstdef_invert.py --all           # recipe for every PURE_RENAME
Reads .omc/coloring_oracle.json; writes .omc/firstdef_plan.json."""
import json, os, sys, math

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
ORACLE = os.path.join(REPO, ".omc", "coloring_oracle.json")
OUT = os.path.join(REPO, ".omc", "firstdef_plan.json")


def regnum(r):
    return int(r[1:]) if r and r[0] in "rf" and r[1:].isdigit() else 999


def invert(target_coloring):
    """target_coloring: {your_reg: target_reg} on the saved band.
    Returns the required first-definition order as a list of value-slots
    (identified by the reg they CURRENTLY occupy), ascending by target reg,
    plus the size of the directed search space."""
    # each value is keyed by its current (yours) reg; it must move to target reg
    values = [{"value_now_in": yr, "must_become": tr, "target_num": regnum(tr)}
              for yr, tr in target_coloring.items()]
    # required first-def order = sort by target reg ascending
    order = sorted(values, key=lambda v: v["target_num"])
    # count genuine ties (values whose target regs are equal -> ambiguous order).
    # for a true bijection there are none, so the directed search is size 1.
    from collections import Counter
    tie = Counter(v["target_num"] for v in values)
    candidates = 1
    for c in tie.values():
        candidates *= math.factorial(c)
    n = len(values)
    random_space = math.factorial(n) if n else 1
    return order, candidates, random_space


def recipe_for(entry):
    tc = entry.get("target_coloring") or {}
    order, candidates, random_space = invert(tc)
    steps = []
    for i, v in enumerate(order, 1):
        steps.append(f"{i}. first-assign the value currently in {v['value_now_in']} "
                     f"(target wants it in {v['must_become']})")
    return {
        "fn": entry["fn"], "file": entry.get("file"),
        "verdict": entry.get("verdict"),
        "swap_distance": entry.get("swap_distance"),
        "first_definition_order": [v["value_now_in"] for v in order],
        "target_regs_ascending": [v["must_become"] for v in order],
        "directed_candidates": candidates,
        "random_search_space": random_space,
        "speedup_vs_random": (random_space // candidates) if candidates else None,
        "recipe": steps,
        "note": ("Reorder ONLY the producing statements (the calls/assignments that "
                 "first write each value) into this order; leave declarations and uses "
                 "free for readability. Then batch-compile with the project flags and "
                 "verify with compile_check.py. If byte-match fails with regs now "
                 "correct, the residual is instruction SCHEDULING (not C-controllable) "
                 "-> triage out, do not anneal."),
    }


def main():
    if not os.path.exists(ORACLE):
        print(f"missing {ORACLE} — run coloring_oracle.py first")
        sys.exit(1)
    oracle = json.load(open(ORACLE))
    pure = {v["fn"]: v for v in oracle["verdicts"]
            if v["verdict"] in ("PURE_RENAME", "REG_DOMINANT_RENAME")}

    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    do_all = "--all" in sys.argv or not args

    if args and not do_all:
        v = pure.get(args[0])
        if not v:
            print(f"{args[0]}: not a PURE_RENAME oracle target "
                  f"(only these invert: {', '.join(pure)})")
            return
        print(json.dumps(recipe_for(v), indent=2))
        return

    plans = [recipe_for(v) for v in sorted(pure.values(),
                                           key=lambda x: x.get("swap_distance", 99))]
    json.dump({"count": len(plans), "plans": plans}, open(OUT, "w"), indent=1)
    print(f"first-definition inversion plans for {len(plans)} PURE_RENAME walls:\n")
    for p in plans:
        print(f"  {p['fn']:<14} dist={p['swap_distance']}  "
              f"directed={p['directed_candidates']} candidate(s) vs "
              f"{p['random_search_space']:,} random  ({p['file']})")
        print(f"      first-def order: {' -> '.join(p['first_definition_order'])}  "
              f"=> target {' < '.join(p['target_regs_ascending'])}")
    print(f"\nwrote {OUT}")


if __name__ == "__main__":
    main()
