#!/usr/bin/env python3
"""research_daemon.py — emits a live activity log of the integrated decomp
research pipeline so the work is watchable, not just JSON on disk.

Each cycle it:
  1. (re)runs the TRIAGE gate + coloring ORACLE + first-def INVERT over the wall
     inventory and logs any change in the routing (4 anneal / 17 invert / ...).
  2. tails the live annealer run-logs and emits a SWARM event whenever a chain
     finds a NEW BEST energy (the genuinely live work).
  3. reports the ALLOC_INVERT candidates' readiness (oracle verdict + the single
     directed first-def order the inverter solved).

Writes newline-delimited events to .omc/research_log.jsonl (append, capped).
Read them live with research_log_view.py. Run with WSL python3."""
import json, os, sys, time, subprocess

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM = os.path.join(REPO, "tools", "decomp_work", "permuter")
RA = os.path.join(REPO, "tools", "decomp_work", "ra")
STATE = os.path.join(REPO, ".omc", "permuter_state.json")
TRIAGE = os.path.join(REPO, ".omc", "triage_routes.json")
ORACLE = os.path.join(REPO, ".omc", "coloring_oracle.json")
FIRSTDEF = os.path.join(REPO, ".omc", "firstdef_plan.json")
LOG = os.path.join(REPO, ".omc", "research_log.jsonl")
LOGDIR = os.path.join(PERM, "logs")

import re as _re
_CAP = 2000           # keep the last N events
_best = {}            # per-fn best energy seen (for new-best detection)
_last_routes = None
_t0 = time.time()


def emit(stage, msg, fn=None, level="info"):
    ev = {"ts": round(time.time(), 1), "t": round(time.time() - _t0, 1),
          "stage": stage, "fn": fn, "level": level, "msg": msg}
    try:
        os.makedirs(os.path.dirname(LOG), exist_ok=True)
        # append, then trim if oversized (cheap: only every so often)
        with open(LOG, "a") as fh:
            fh.write(json.dumps(ev) + "\n")
    except OSError:
        return
    # occasional trim
    if int(ev["t"]) % 60 == 0:
        _trim()


def _trim():
    try:
        lines = open(LOG, errors="replace").read().splitlines()
        if len(lines) > _CAP:
            with open(LOG, "w") as fh:
                fh.write("\n".join(lines[-_CAP:]) + "\n")
    except OSError:
        pass


def jload(p, d):
    try:
        return json.load(open(p))
    except Exception:
        return d


def run_research():
    """Run the analysis stages (cheap, static over walltriage_out.json) and log
    routing changes + candidate readiness."""
    global _last_routes
    # triage_gate auto-runs the oracle; firstdef_invert reads the oracle output
    subprocess.run([sys.executable, os.path.join(PERM, "triage_gate.py")],
                   cwd=REPO, capture_output=True, timeout=120)
    subprocess.run([sys.executable, os.path.join(RA, "firstdef_invert.py"), "--all"],
                   cwd=REPO, capture_output=True, timeout=120)
    tri = jload(TRIAGE, {}); orc = jload(ORACLE, {}); fdef = jload(FIRSTDEF, {})
    counts = tri.get("counts", {})
    key = json.dumps(counts, sort_keys=True)
    if key != _last_routes:
        _last_routes = key
        emit("TRIAGE", "routed {} walls -> {} ANNEAL | {} ALLOC_INVERT | {} PARK_SCHED | {} PARK_STRUCT".format(
            sum(counts.values()), counts.get("ANNEAL", 0), counts.get("ALLOC_INVERT", 0),
            counts.get("PARK_SCHEDULING", 0), counts.get("PARK_STRUCTURAL", 0)), level="route")
        npure = (orc.get("counts", {}) or {}).get("PURE_RENAME", 0)
        emit("ORACLE", f"{npure} pure-rename walls (consistent saved-reg bijection)", level="route")
        for p in (fdef.get("plans") or [])[:6]:
            order = " -> ".join(p.get("first_definition_order", []))
            emit("INVERT", "dist={} : 1 directed candidate vs {} random : first-def {}".format(
                p.get("swap_distance"), p.get("random_search_space"), order),
                fn=p.get("fn"), level="invert")


def poll_swarm():
    """Tail the annealer run-logs; emit a SWARM event on each new best energy."""
    st = jload(STATE, {})
    act = st.get("active", {})
    fns = []
    for v in (act.values() if isinstance(act, dict) else []):
        fn = v.get("fn")
        if fn and fn not in fns:
            fns.append(fn)
    for fn in fns:
        try:
            txt = open(os.path.join(LOGDIR, f"run_{fn}.log"), errors="replace").read()[-4000:]
        except OSError:
            continue
        scs = _re.findall(r'score = (-?\d+)', txt)
        its = _re.findall(r'iteration (\d+)', txt)
        if not scs:
            continue
        sc = int(scs[-1]); it = int(its[-1]) if its else 0
        prev = _best.get(fn)
        if prev is None:
            _best[fn] = sc
            emit("SWARM", f"annealing started : E={sc} @ iter {it:,}", fn=fn, level="info")
        elif sc < prev:
            drop = prev - sc
            lvl = "win" if sc == 0 else "best"
            emit("SWARM", f"NEW BEST E={sc} (-{drop}) @ iter {it:,}"
                 + ("   *** BYTE-EXACT MATCH ***" if sc == 0 else ""), fn=fn, level=lvl)
            _best[fn] = sc
    # note functions that left the active set (finished)
    done = st.get("done") or []
    for d in done[-3:]:
        fn = d.get("fn")
        tag = f"_done_{fn}"
        if fn and tag not in _best:
            _best[tag] = True
            emit("VERIFY", "swarm finished -> {}".format(d.get("result", "?")),
                 fn=fn, level=("win" if d.get("result") == "WIN" else "info"))


def main():
    emit("DAEMON", "research activity daemon online — triage/oracle/invert + swarm tail", level="route")
    last_research = 0
    while True:
        try:
            poll_swarm()
            if time.time() - last_research > 20:    # research stages every ~20s
                run_research()
                last_research = time.time()
        except Exception as e:
            emit("DAEMON", f"cycle error: {e}", level="warn")
        time.sleep(3)


if __name__ == "__main__":
    main()
