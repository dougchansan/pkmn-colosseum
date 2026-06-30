#!/usr/bin/env python3
"""triage_gate.py — the front of the integrated reg-alloc pipeline.

The research's single highest-leverage output is not another solver — it's the
TRIAGE GATE that classifies every near-miss UP FRONT so effort flows only where
it can land, instead of burning annealing-hours on walls with a proven 0% hit
rate. This implements that gate over the existing inventory + coloring oracle.

Routing (each near-miss goes to exactly one track):

  ANNEAL          small residual, NOT pure-reg-alloc — random-mutation SA still
                  has a downhill gradient here. -> grind2 swarm.
  ALLOC_INVERT    PURE_RENAME (coloring oracle): a consistent saved-reg bijection.
                  Blind SA proven 0% — route to firstdef_invert.py (1 directed
                  candidate). THIS IS THE NEW WINNABLE TRACK the research opened.
  PARK_SCHEDULING regs align, residual is instruction scheduling — not
                  C-controllable; never anneal.
  PARK_STRUCTURAL large structural mass (different instrs/control-flow) — wrong
                  shape; needs an LLM/manual reshape, not coloring or SA.

Writes .omc/triage_routes.json. grind2.py reads it to DROP ALLOC_INVERT /
PARK_* functions from the annealer queue (no wasted budget); pipeline_dash.py
reads it to show the live routing.

usage:
  python3 triage_gate.py                 # classify the whole wall inventory
  python3 triage_gate.py --queue         # also evaluate grind2's current queue
                                         # and emit reroute advice
Depends on .omc/coloring_oracle.json (auto-runs coloring_oracle.py if missing)."""
import json, os, sys, subprocess

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
RA = os.path.join(REPO, "tools", "decomp_work", "ra")
PERM = os.path.join(REPO, "tools", "decomp_work", "permuter")
WALLTRIAGE = os.path.join(RA, "walltriage_out.json")
ORACLE = os.path.join(REPO, ".omc", "coloring_oracle.json")
QUEUE_FILE = os.path.join(REPO, ".omc", "permuter_queue.json")
OUT = os.path.join(REPO, ".omc", "triage_routes.json")

# annealer is only worth it for small, non-pure-reg residuals
ANNEAL_MAX_MISMATCH = 8


def ensure_oracle():
    if not os.path.exists(ORACLE):
        subprocess.run([sys.executable, os.path.join(RA, "coloring_oracle.py")],
                       cwd=REPO, capture_output=True)


def route(entry, oracle_by_fn):
    # ALLOC_INVERT is driven ONLY by the coloring oracle's verdict — a consistent
    # saved-reg bijection — NOT the looser walltriage class. (The class-based
    # heuristic over-counted: REG-DOMINANT entries with volatile-reg / IMM /
    # many-to-many noise are NOT pure inversions. Confirmed by teammate analysis.)
    fn = entry["fn"]
    ov = oracle_by_fn.get(fn, {})
    verdict = ov.get("verdict")
    if verdict == "PURE_RENAME":
        return "ALLOC_INVERT", (f"pure saved-reg bijection, swap-distance "
                                f"{ov.get('swap_distance')} -> 1 directed candidate")
    if verdict == "REG_DOMINANT_RENAME":
        return "ALLOC_INVERT", (f"clean saved-reg bijection dominates (swap-distance "
                                f"{ov.get('swap_distance')}); minor non-reg residual after inversion")
    if verdict == "SCHEDULING":
        return "PARK_SCHEDULING", "regs align, residual is instruction scheduling (not C-controllable)"
    mm = entry.get("mismatches", 999)
    if mm <= ANNEAL_MAX_MISMATCH:
        return "ANNEAL", f"small residual ({mm} mismatches), not pure-reg — SA has gradient"
    return "PARK_STRUCTURAL", f"structural mass ({entry.get('buckets',{}).get('STRUCT',0)} instr) — needs reshape"


def main():
    ensure_oracle()
    wall = json.load(open(WALLTRIAGE))
    oracle = json.load(open(ORACLE)) if os.path.exists(ORACLE) else {"verdicts": []}
    oracle_by_fn = {v["fn"]: v for v in oracle.get("verdicts", [])}

    routes = {}
    for e in wall:
        track, reason = route(e, oracle_by_fn)
        routes.setdefault(track, []).append(
            {"fn": e["fn"], "file": e.get("file"), "match": e.get("match"),
             "mismatches": e.get("mismatches"), "reason": reason})
    counts = {k: len(v) for k, v in routes.items()}

    payload = {
        "counts": counts,
        "anneal_max_mismatch": ANNEAL_MAX_MISMATCH,
        "tracks": routes,
        # explicit deny-list for grind2: never anneal these (proven hopeless)
        "anneal_denylist": sorted(
            [r["fn"] for t in ("ALLOC_INVERT", "PARK_SCHEDULING", "PARK_STRUCTURAL")
             for r in routes.get(t, [])]),
        "alloc_invert_queue": sorted(
            [r["fn"] for r in routes.get("ALLOC_INVERT", [])]),
    }

    # optional: evaluate the live annealer queue and advise reroutes
    if "--queue" in sys.argv and os.path.exists(QUEUE_FILE):
        try:
            q = json.load(open(QUEUE_FILE))
            qfns = [item[0] if isinstance(item, list) else item for item in q]
            advice = []
            deny = set(payload["anneal_denylist"])
            for fn in qfns:
                if fn in deny:
                    ov = oracle_by_fn.get(fn, {})
                    advice.append({"fn": fn, "action": "REROUTE_OFF_ANNEALER",
                                   "to": ("ALLOC_INVERT" if ov.get("verdict") == "PURE_RENAME"
                                          else "PARK"),
                                   "why": ov.get("reason", "on deny-list")})
            payload["queue_advice"] = advice
        except Exception:
            pass

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    json.dump(payload, open(OUT, "w"), indent=1)

    print("TRIAGE GATE over", len(wall), "wall functions ->")
    for k in ("ANNEAL", "ALLOC_INVERT", "PARK_SCHEDULING", "PARK_STRUCTURAL"):
        print(f"  {k:<16} {counts.get(k, 0)}")
    print(f"\nALLOC_INVERT (new winnable track): {', '.join(payload['alloc_invert_queue']) or '-'}")
    if payload.get("queue_advice"):
        print("\nlive annealer-queue reroute advice:")
        for a in payload["queue_advice"]:
            print(f"  {a['fn']:<14} -> {a['action']} ({a['to']})")
    print(f"\nwrote {OUT}")


if __name__ == "__main__":
    main()
