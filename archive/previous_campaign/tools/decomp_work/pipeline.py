#!/usr/bin/env python3
"""Stage-aware decomp pipeline with FILE-LEVEL ownership.

Fixes the lock contention we hit when 5 lanes piled onto one file: here a file is
owned by exactly ONE lane at a time. Files flow through stages; different files
can be at different stages concurrently (pipeline parallelism) but two lanes
never touch the same file.

Stages:
  A  asm -> C draft        (hard; Opus / gpt-5.5)         m2c_draft + faithful real C
  B  near-miss finisher    (mechanical; Sonnet / spark)   take 90-99.99% drafts -> 100% via levers
  C  permuter              (automated; Haiku babysits)    walled fns -> decomp-permuter

A function's pct decides its next stage on release:
  >=100   -> done (byte-exact, band save -> band_integrate)
  90..100 -> stage B (finish) or salvage as Equivalent (band_integrate --equivalent)
  <90     -> stay in A for another draft pass, or park

Commands:
  pipeline.py seed <seed.json>        load files+fns (one entry per file)
  pipeline.py claim <lane>            assign next available file for lane's stage
  pipeline.py release <lane> [pct.json]  free lane's file; advance fns by new pct
  pipeline.py packet <lane>           print the tight work packet for lane's file
  pipeline.py status [--json]         queue state (for the dashboard)
  pipeline.py assign <lane> <stage> <model>   configure a lane

State: tools/decomp_work/coordination/pipeline.json
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
STATE = HERE / "coordination" / "pipeline.json"
STAGE_NEXT = {"A": "B", "B": "C", "C": "done"}


def load():
    if STATE.exists():
        return json.loads(STATE.read_text(encoding="utf-8"))
    return {"files": {}, "lanes": {}}


def save(st):
    STATE.parent.mkdir(parents=True, exist_ok=True)
    STATE.write_text(json.dumps(st, indent=2), encoding="utf-8")


def cmd_assign(st, lane, stage, model):
    st["lanes"][lane] = {"stage": stage, "model": model, "file": None}
    save(st)
    print(f"lane {lane}: stage {stage}, model {model}")


def cmd_seed(st, seed_path):
    seed = json.loads(Path(seed_path).read_text(encoding="utf-8"))
    for f in seed:
        st["files"][f["file"]] = {
            "stage": f.get("stage", "A"),
            "owner": None,
            "tag": f.get("tag"),
            "fns": {fn: {"pct": p, "status": "todo"} for fn, p in f["fns"].items()},
        }
    save(st)
    print(f"seeded {len(seed)} file(s): " + ", ".join(s['file'] for s in seed))


def _available_file(st, stage):
    for fpath, fd in st["files"].items():
        if fd["stage"] == stage and not fd["owner"]:
            return fpath
    return None


def cmd_claim(st, lane):
    lane_cfg = st["lanes"].get(lane)
    if not lane_cfg:
        print(f"lane {lane} not configured; run: pipeline.py assign {lane} <stage> <model>")
        return 1
    if lane_cfg.get("file"):
        print(f"{lane} already owns {lane_cfg['file']} (release it first)")
        return 0
    fpath = _available_file(st, lane_cfg["stage"])
    if not fpath:
        print(f"no available file at stage {lane_cfg['stage']} for {lane}")
        return 0
    st["files"][fpath]["owner"] = lane
    lane_cfg["file"] = fpath
    save(st)
    print(f"{lane} claimed {fpath} (stage {lane_cfg['stage']})")


def cmd_release(st, lane, pct_path=None):
    lane_cfg = st["lanes"].get(lane, {})
    fpath = lane_cfg.get("file")
    if not fpath:
        print(f"{lane} owns no file")
        return 0
    fd = st["files"][fpath]
    new_pct = {}
    if pct_path:
        new_pct = json.loads(Path(pct_path).read_text(encoding="utf-8"))
    for fn, fninfo in fd["fns"].items():
        if fn in new_pct:
            fninfo["pct"] = new_pct[fn]
    # advance the FILE's stage: a file moves to stage B once it has any 90-99.99
    # fn left to finish; if all fns are >=100 or <90-and-exhausted it's done/parked.
    finishable = [fn for fn, i in fd["fns"].items() if 90.0 <= i["pct"] < 100.0]
    done = [fn for fn, i in fd["fns"].items() if i["pct"] >= 100.0]
    for fn in done:
        fd["fns"][fn]["status"] = "done"
    fd["owner"] = None
    lane_cfg["file"] = None
    if fd["stage"] == "A" and finishable:
        fd["stage"] = "B"
    elif fd["stage"] == "B" and finishable:
        fd["stage"] = "C"          # B couldn't close them -> permuter
    elif not finishable:
        fd["stage"] = "done"
    save(st)
    print(f"{lane} released {fpath}: {len(done)} done, {len(finishable)} finishable "
          f"-> file now stage {fd['stage']}")


PACKET_A = """# PIPELINE PACKET (stage A: asm -> C draft) — lane {lane} [{model}]
File:  {file}
Tag:   {tag}
Fns (pct now):
{fnlist}

DO NOT explore. Run exactly this per fn, in order:
  python3 tools/decomp_work/m2c_draft.py <fn> {file}
  # rewrite the m2c draft into FAITHFUL real C (real struct types, not raw casts)
  # in tools/decomp_work/scratch/band_{tag}.c
  tools/decomp_work/decomp.sh band init  {tag} {file}     # once, first
  tools/decomp_work/decomp.sh band check {tag} <fn>       # the REAL % (paste it)
  tools/decomp_work/decomp.sh band diff  {tag} <fn>       # if <100, see divergence
  tools/decomp_work/decomp.sh band save  {tag} <fn>       # ONLY if band check shows 100.00
Use the KG to understand a fn (don't guess): kg q call-neighborhood/name-evidence <fn>.
Real C only — the gate rejects asm{{}}/inline-asm/#include .inc and #if0->#if1 flips.
Report one line per fn:  REALC <fn> <real-band-check-%> (struct notes: offset/type)
When done with all fns, STOP. The orchestrator harvests + advances the file."""

PACKET_B = """# PIPELINE PACKET (stage B: near-miss finisher) — lane {lane} [{model}]
File:  {file}
Tag:   {tag}
Fns to push to 100% (already faithful C at the pct shown):
{fnlist}

The asm->C draft exists in scratch/band_{tag}.c (or canon). Your job: close the
last few % with the LEVER LIBRARY, not a rewrite. Run:
  tools/decomp_work/decomp.sh band init  {tag} {file}     # once
  tools/decomp_work/decomp.sh band diff  {tag} <fn>       # see the exact divergence
  kg q top-levers ; kg q siblings <fn>                    # which lever fits this shape
  tools/decomp_work/decomp.sh band check {tag} <fn>
  tools/decomp_work/decomp.sh band save  {tag} <fn>       # ONLY at 100.00
Levers: CW_QUIRKS.md (decl-order reg-alloc, no-temp-rmw-base-color, peephole-off,
scheduling on/off, real-signature, tail-call-inversion-u32-return-raw).
STOP after ~3-4 honest lever attempts per fn. Report: SAVED <fn> 100.00 (lever) | WALL <fn> <best%> <residual>
Walls route to stage C (permuter) — just report them, don't grind further."""


def cmd_packet(st, lane):
    lane_cfg = st["lanes"].get(lane, {})
    fpath = lane_cfg.get("file")
    if not fpath:
        print(f"{lane} owns no file (claim one first)")
        return 1
    fd = st["files"][fpath]
    stage = fd["stage"]
    if stage == "B":
        fns = {fn: i for fn, i in fd["fns"].items() if 90.0 <= i["pct"] < 100.0}
        tmpl = PACKET_B
    else:
        fns = {fn: i for fn, i in fd["fns"].items() if i["pct"] < 100.0}
        tmpl = PACKET_A
    fnlist = "\n".join(f"  {fn}   {i['pct']:.2f}%" for fn, i in fns.items())
    print(tmpl.format(lane=lane, model=lane_cfg.get("model", "?"),
                      file=fpath, tag=fd.get("tag") or lane.lower().replace("-", ""),
                      fnlist=fnlist))


def cmd_status(st, as_json):
    if as_json:
        print(json.dumps(st, indent=2))
        return
    print("=== PIPELINE STATUS ===")
    for fpath, fd in st["files"].items():
        done = sum(1 for i in fd["fns"].values() if i["pct"] >= 100.0)
        fin = sum(1 for i in fd["fns"].values() if 90.0 <= i["pct"] < 100.0)
        print(f"  [{fd['stage']}] {fpath:<34} owner={fd['owner'] or '-':<8} "
              f"{done} done / {fin} finishable / {len(fd['fns'])} total")
    print("  lanes:")
    for lane, c in st["lanes"].items():
        print(f"    {lane:<9} stage {c['stage']} [{c['model']}] -> {c.get('file') or '(idle)'}")


def main():
    a = sys.argv[1:]
    if not a:
        print(__doc__)
        return 0
    st = load()
    cmd = a[0]
    if cmd == "seed":
        return cmd_seed(st, a[1])
    if cmd == "assign":
        return cmd_assign(st, a[1], a[2], a[3])
    if cmd == "claim":
        return cmd_claim(st, a[1])
    if cmd == "release":
        return cmd_release(st, a[1], a[2] if len(a) > 2 else None)
    if cmd == "packet":
        return cmd_packet(st, a[1])
    if cmd == "status":
        return cmd_status(st, "--json" in a)
    print(f"unknown command: {cmd}")
    return 2


if __name__ == "__main__":
    sys.exit(main())
