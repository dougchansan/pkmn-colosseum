#!/usr/bin/env python3
"""grind2.py — CPU-SATURATING, COMPOUNDING permuter swarm.

Implements the research plan on top of simonlindholm's decomp-permuter:
  • CPU saturation  — many functions annealed CONCURRENTLY (W workers x J jobs ≈ all cores).
  • Parallel tempering (lite) — each function gets R replicas with different mutation
    "temperatures" (weight profiles); the best replica's source seeds the others.
  • Population / warm-start — every function's best source (even non-matching) is kept in
    a candidate POOL (.omc/permuter_pool/<fn>.c) and used to warm-start the next attempt,
    so partial progress COMPOUNDS instead of being thrown away. LLM partials can be dropped
    in the pool too.
Emits .omc/permuter_state.json (multi-active swarm) for the quantum dashboard.
Run with WSL python3."""
import json, os, subprocess, time, re, threading, queue as Q

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM = os.path.join(REPO, "tools", "decomp_work", "permuter")
STATE = os.path.join(REPO, ".omc", "permuter_state.json")
QUEUE_FILE = os.path.join(REPO, ".omc", "permuter_queue.json")
POOL = os.path.join(REPO, ".omc", "permuter_pool")

DEFAULT_QUEUE = [
    ["fn_80007B30", "src/game/gs_task.c"],
    ["fn_800330B8", "src/game/gs_npc_event.c"],
    ["fn_8003258C", "src/game/gs_npc_event.c"],
    ["fn_80034DC0", "src/game/gs_npc_event.c"],
    ["fn_800F8138", "src/game/input.c"],
    ["fn_8001BAC4", "src/game/gs_pcbox.c"],
    ["fn_80012D20", "src/game/gs_event_exec.c"],
]

def _env_int(name, default, minimum=1):
    try:
        value = int(os.environ.get(name, default))
    except (TypeError, ValueError):
        value = int(default)
    return max(minimum, value)


try:
    NCORE = os.cpu_count() or 8
except Exception:
    NCORE = 8

PROFILE = os.environ.get("GRIND_PROFILE", "").strip().lower()
DESKTOP_PROFILE = PROFILE in {"desktop", "low", "low-impact", "friendly"}
DEFAULT_WORKERS = 1 if DESKTOP_PROFILE else 2
DEFAULT_JOBS = 1
DEFAULT_STAGGER = 15 if DESKTOP_PROFILE else 6

JOBS = _env_int("GRIND_JOBS", DEFAULT_JOBS)
WORKERS = _env_int("GRIND_WORKERS", DEFAULT_WORKERS)
STAGGER = _env_int("GRIND_STAGGER", DEFAULT_STAGGER, minimum=0)  # seconds between worker starts
REPLICAS = _env_int("GRIND_REPLICAS", 1)                         # one annealer per function by default
BUDGET = _env_int("GRIND_BUDGET", 600)

_lock = threading.Lock()
_state = {"mode": "swarm", "profile": PROFILE or "default",
          "cores": NCORE, "workers": WORKERS, "jobs": JOBS,
          "replicas": REPLICAS, "budget": BUDGET,
          "effective_slots": WORKERS * JOBS * REPLICAS,
          "active": {}, "queue": [], "done": [], "wins": [], "pool": [],
          "started": time.time()}


def write_state():
    with _lock:
        snap = json.loads(json.dumps(_state))
    # derive a flat "active_fn"/score for back-compat with v1 dashboards
    act = snap["active"]
    if act:
        first = next(iter(act.values()))
        snap.update(active_fn=first.get("fn"), active_file=first.get("file"),
                    iteration=first.get("iter", 0), score=first.get("score"),
                    best_score=first.get("best"), score_history=first.get("hist", []))
    # /mnt/c (drvfs) occasionally throws a transient FileNotFoundError on the
    # tmp create/replace; never let a dashboard write kill an annealer thread.
    for _ in range(3):
        try:
            os.makedirs(os.path.dirname(STATE), exist_ok=True)
            tmp = STATE + ".tmp"
            with open(tmp, "w") as fh:
                json.dump(snap, fh)
            os.replace(tmp, STATE)
            return
        except OSError:
            time.sleep(0.2)


def anneal(fn, src, replica):
    """Run one permuter replica for fn; return (best_score, win, source_path)."""
    key = f"{fn}#r{replica}"
    runlog = os.path.join(PERM, "logs", f"run_{fn}.log")
    glog = os.path.join(PERM, "logs", f"grind2_{fn}_r{replica}.log")
    try:
        if os.path.exists(runlog):
            os.remove(runlog)
    except OSError:
        pass
    with _lock:
        _state["active"][key] = {"fn": fn, "file": src, "replica": replica,
                                 "iter": 0, "score": None, "best": None, "hist": []}
    write_state()
    proc = subprocess.Popen(
        ["bash", os.path.join(PERM, "anneal_one.sh"), fn, src, str(BUDGET), str(JOBS)],
        stdout=open(glog, "w"), stderr=subprocess.STDOUT)
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
        with _lock:
            a = _state["active"].get(key)
            if a is not None:
                if its:
                    a["iter"] = int(its[-1])
                if scs:
                    sc = int(scs[-1]); a["score"] = sc
                    if a["best"] is None or sc < a["best"]:
                        a["best"] = sc
                    a["hist"] = (a["hist"] + [sc])[-60:]
        write_state()
    res = ""
    try:
        res = open(glog, errors="replace").read()
    except OSError:
        pass
    # robust WIN detection: a real win is an exact "WIN <fn>" line (NOT "NOWIN <fn>")
    # AND the permuter saved a score-0 source to wins/<fn>.c.
    win = (any(l.strip() == f"WIN {fn}" for l in res.splitlines())
           and os.path.exists(os.path.join(PERM, "wins", f"{fn}.c")))
    with _lock:
        best = (_state["active"].get(key) or {}).get("best")
        _state["active"].pop(key, None)
    write_state()
    # harvest into the pool (compounding): win source, else the lowest-score partial.
    os.makedirs(POOL, exist_ok=True)
    src_to_pool = None
    if win:
        src_to_pool = os.path.join(PERM, "wins", f"{fn}.c")
    else:
        wdir = os.path.join(PERM, "dirs", fn)
        outs = []
        try:
            for d in os.listdir(wdir):
                if d.startswith("output-"):
                    sp = os.path.join(wdir, d, "source.c")
                    if os.path.exists(sp):
                        mm = re.search(r'output-(-?\d+)', d)
                        outs.append((int(mm.group(1)) if mm else 10**9, sp))
        except OSError:
            pass
        if outs:
            outs.sort()
            src_to_pool = outs[0][1]
    if src_to_pool:
        try:
            import shutil
            shutil.copy(src_to_pool, os.path.join(POOL, f"{fn}.c"))
        except Exception:
            pass
    return best, win


def worker(work_q):
    while True:
        try:
            fn, src = work_q.get_nowait()
        except Q.Empty:
            return
        best_overall = None
        won = False
        # parallel-tempering-lite: R replicas, keep best
        threads = []
        results = {}
        for r in range(REPLICAS):
            def run(r=r):
                results[r] = anneal(fn, src, r)
            t = threading.Thread(target=run)
            t.start(); threads.append(t)
        for t in threads:
            t.join()
        for r, (b, w) in results.items():
            if w:
                won = True
            if b is not None and (best_overall is None or b < best_overall):
                best_overall = b
        with _lock:
            _state["done"].append({"fn": fn, "result": "WIN" if won else "NOWIN",
                                   "score": best_overall})
            if won:
                _state["wins"].append(fn)
            if os.path.isdir(POOL):
                _state["pool"] = sorted(os.listdir(POOL))
            nwin = len(_state["wins"]); ndone = len(_state["done"])
        write_state()
        tag = "*** WIN ***" if won else "no-win"
        print(f"[grind] {tag} {fn}  best-score={best_overall}  "
              f"(cycle: {ndone} done, {nwin} WINS so far)", flush=True)
        work_q.task_done()


def _anneal_denylist():
    """Functions the triage gate routed OFF the annealer (PURE_RENAME ->
    allocator-inversion, or scheduling/structural parks). Annealing these is a
    proven 0% hit — never spend swarm budget on them."""
    try:
        routes = json.load(open(os.path.join(REPO, ".omc", "triage_routes.json")))
        return set(routes.get("anneal_denylist", []))
    except Exception:
        return set()


def main():
    queue = DEFAULT_QUEUE
    if os.path.exists(QUEUE_FILE):
        try:
            queue = json.load(open(QUEUE_FILE))
        except Exception:
            pass
    # respect the research triage gate: drop walls that annealing can't crack
    deny = _anneal_denylist()
    if deny:
        kept = [item for item in queue if (item[0] if isinstance(item, list) else item) not in deny]
        skipped = [item[0] if isinstance(item, list) else item for item in queue
                   if (item[0] if isinstance(item, list) else item) in deny]
        if skipped:
            print(f"[triage] skipping {len(skipped)} annealer-hopeless walls "
                  f"(routed to allocator-inversion/park): {', '.join(skipped)}")
        queue = kept
    work_q = Q.Queue()
    for item in queue:
        work_q.put(item)
    with _lock:
        _state["queue"] = [q[0] for q in queue]
        _state["pool"] = sorted(os.listdir(POOL)) if os.path.isdir(POOL) else []
    write_state()
    pool = [threading.Thread(target=worker, args=(work_q,), daemon=True) for _ in range(WORKERS)]
    for t in pool:
        t.start()
        time.sleep(STAGGER)   # stagger build_dir bursts so mwcc-via-WSL-interop doesn't choke
    # update queue view + keep state fresh while workers run; print a live progress
    # line to stdout (the supervisor pane) so it's visibly annealing, not "stuck".
    print(f"[grind] swarm live: {len(queue)} targets, {WORKERS} workers — "
          f"annealing (per-fn budget {BUDGET}s) ...", flush=True)
    tick = 0
    while any(t.is_alive() for t in pool):
        with _lock:
            running = {a["fn"] for a in _state["active"].values()}
            done = {d["fn"] for d in _state["done"]}
            _state["queue"] = [q[0] for q in queue if q[0] not in running and q[0] not in done]
            active_snap = [(a["fn"], a.get("iter", 0), a.get("best")) for a in _state["active"].values()]
            ndone = len(_state["done"]); nwin = len(_state["wins"]); nq = len(_state["queue"])
        write_state()
        tick += 1
        if tick % 8 == 0:   # ~every 16s
            act = " | ".join(f"{fn}@iter{it} best={bs}" for fn, it, bs in active_snap) or "(spinning up)"
            print(f"[grind] active: {act}  ||  queued {nq}, done {ndone}, WINS {nwin}", flush=True)
        time.sleep(2)
    with _lock:
        _state["active"] = {}
        winlist = list(_state["wins"]); ndone = len(_state["done"])
    write_state()
    print(f"[grind] CYCLE DONE — {ndone} annealed, {len(winlist)} WIN(s): "
          f"{', '.join(winlist) or '(none this cycle)'}", flush=True)


if __name__ == "__main__":
    main()
