#!/usr/bin/env python3
"""grind.py — drive simonlindholm's decomp-permuter over a QUEUE of near-miss
functions (simulated-annealing search for byte-exact CW codegen), emitting live
state to .omc/permuter_state.json for the quantum-annealer dashboard.

Each function: build_dir -> permuter.py anneal (Metropolis acceptance over random
source mutations) -> WIN (score 0, saved to wins/) or NOWIN <best score>.
Run with WSL python3 (the permuter scorer needs native powerpc objdump)."""
import json, os, subprocess, time, re

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM = os.path.join(REPO, "tools", "decomp_work", "permuter")
STATE = os.path.join(REPO, ".omc", "permuter_state.json")
QUEUE_FILE = os.path.join(REPO, ".omc", "permuter_queue.json")

# Default queue: high active-C near-misses where the permuter has the best shot.
DEFAULT_QUEUE = [
    ["fn_80007B30", "src/game/gs_task.c"],
    ["fn_800F8138", "src/game/input.c"],
    ["fn_8001BAC4", "src/game/gs_pcbox.c"],
    ["fn_80012D20", "src/game/gs_event_exec.c"],
    ["fn_800330B8", "src/game/gs_npc_event.c"],
    ["fn_8003258C", "src/game/gs_npc_event.c"],
]


def load_queue():
    if os.path.exists(QUEUE_FILE):
        try:
            return json.load(open(QUEUE_FILE))
        except Exception:
            pass
    return DEFAULT_QUEUE


def write(s):
    os.makedirs(os.path.dirname(STATE), exist_ok=True)
    # emit the dashboard schema (active dict + cores) from the flat fields
    af = s.get("active_fn")
    s["active"] = ({af: {"fn": af, "replica": 0, "iter": s.get("iteration", 0),
                         "score": s.get("score"), "best": s.get("best_score"),
                         "hist": s.get("score_history", [])}} if af else {})
    s["cores"] = os.cpu_count() or 4
    s["workers"] = 1
    s["jobs"] = 4
    tmp = STATE + ".tmp"
    json.dump(s, open(tmp, "w"))
    os.replace(tmp, STATE)


def main():
    queue = load_queue()
    done = []
    state = {"queue": [q[0] for q in queue], "done": done, "wins": [],
             "active_fn": None, "active_file": None, "iteration": 0,
             "score": None, "best_score": None, "start_score": None,
             "score_history": [], "started": time.time(), "fn_started": time.time()}
    write(state)
    for i, (fn, src) in enumerate(queue):
        state.update(active_fn=fn, active_file=src, iteration=0, score=None,
                     best_score=None, start_score=None, score_history=[],
                     fn_started=time.time())
        state["queue"] = [q[0] for q in queue[i + 1:]]
        write(state)
        runlog = os.path.join(PERM, "logs", f"run_{fn}.log")
        grindlog = os.path.join(PERM, "logs", f"grind_{fn}.log")
        try:
            if os.path.exists(runlog):
                os.remove(runlog)
        except OSError:
            pass
        proc = subprocess.Popen(
            ["bash", os.path.join(PERM, "anneal_one.sh"), fn, src, "480", "4"],
            stdout=open(grindlog, "w"), stderr=subprocess.STDOUT)
        while proc.poll() is None:
            time.sleep(2)
            txt = ""
            try:
                if os.path.exists(runlog):
                    txt = open(runlog, errors="replace").read()
            except OSError:
                pass
            its = re.findall(r'iteration (\d+)', txt)
            scs = re.findall(r'score = (-?\d+)', txt)
            if its:
                state["iteration"] = int(its[-1])
            if scs:
                sc = int(scs[-1])
                state["score"] = sc
                if state["start_score"] is None:
                    state["start_score"] = sc
                if state["best_score"] is None or sc < state["best_score"]:
                    state["best_score"] = sc
                state["score_history"] = (state["score_history"] + [sc])[-80:]
            write(state)
        res = ""
        try:
            res = open(grindlog, errors="replace").read()
        except OSError:
            pass
        win = (any(l.strip() == f"WIN {fn}" for l in res.splitlines())
           and os.path.exists(os.path.join(PERM, "wins", f"{fn}.c")))
        done.append({"fn": fn, "result": "WIN" if win else "NOWIN",
                     "score": state.get("best_score")})
        if win:
            state["wins"].append(fn)
        state["done"] = done
        write(state)
    state.update(active_fn=None, active_file="(queue drained)")
    write(state)


if __name__ == "__main__":
    main()
