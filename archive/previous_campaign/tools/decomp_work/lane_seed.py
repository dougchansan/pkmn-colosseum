#!/usr/bin/env python3
"""lane_seed.py — the flywheel proposer lane (fleet lane #6).

The SeedCoder-8B-v3 model (served on the 3090 by serve_v3.py) DRAFTS C for
low-bucket functions; the LOCAL band gate (mwcceppc via wibo + objdiff) VERIFIES;
only strict improvements are kept and >=100% byte-exact wins bank to
build/band_wins/pl_seed.json for auto_gate to commit. Same hardened "remote
propose, local gate" loop as the retired lane_3090 — now with the trained model,
zero API cost (free local GPU). The model proposes; the gate decides.

Claims a file from the queue (atomic lock, shared with the agent lanes), works
its target fns in a PRIVATE band scratch (no collision), then loops.

  SEED_SERVER (default http://192.168.50.101:8780/gen)   the v3 inference server
  FLEET_QUEUE (default build/low_attack_queue.txt)
"""
import json
import os
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
import lane_glm as L          # noqa: E402  target_asm / find_fn_span
import compile_check as cc    # noqa: E402

SERVER = os.environ.get("SEED_SERVER", "http://192.168.50.101:8780/gen")
_queue_env = os.environ.get("FLEET_QUEUE", "build/low_attack_queue.txt")
_queue_path = Path(_queue_env)
QUEUE = _queue_path if _queue_path.is_absolute() else ROOT / _queue_path
LOCKD = ROOT / "build" / "fleet_locks"
TAG = "pl_seed"
BAND = ROOT / "tools" / "decomp_work" / "band.py"
SCRATCH = ROOT / "tools" / "decomp_work" / "scratch" / f"band_{TAG}.c"
N_CAND = int(os.environ.get("SEED_N", "4"))
REPAIR_N = int(os.environ.get("SEED_REPAIR_N", "2"))
MAX_NEW = int(os.environ.get("SEED_MAX_NEW", "1200"))
TEMP = float(os.environ.get("SEED_TEMP", "0.6"))
REPAIR_TEMP = float(os.environ.get("SEED_REPAIR_TEMP", "0.35"))
SERIAL_GEN = os.environ.get("SEED_SERIAL_GEN", "1") != "0"
TRAIN = ROOT / "build" / "seed_training" / "attempts.jsonl"
KG = ROOT / "tools" / "decomp_work" / "kg" / "kg.py"


def band(*args, timeout=180):
    return subprocess.run([sys.executable, str(BAND), *args], cwd=str(ROOT),
                          capture_output=True, text=True, timeout=timeout)


def measure():
    """band.py json <tag> -> {fn: pct}."""
    r = band("json", TAG)
    try:
        return json.loads(r.stdout)
    except Exception:
        return {}


def _clip(text, limit):
    text = text or ""
    if len(text) <= limit:
        return text
    return text[-limit:]


def _run_text(args, timeout=60):
    try:
        r = subprocess.run(args, cwd=str(ROOT), capture_output=True, text=True, timeout=timeout)
    except Exception as e:
        return f"(tool error: {e})"
    out = (r.stdout or "") + (("\n" + r.stderr) if r.stderr else "")
    return out.strip()


def kg_levers():
    out = _run_text([sys.executable, str(KG), "q", "top-levers"], timeout=30)
    lines = [ln for ln in out.splitlines() if ln.strip()]
    return "\n".join(lines[:34])


TOP_LEVERS = kg_levers()


def residual_hint(fn):
    return _clip(_run_text([sys.executable, "tools/decomp_work/classify_residual.py", TAG, fn],
                           timeout=120), 2200)


def file_context(scratch_txt, fn_start, fn, residual):
    """Small, grounded context for serve_v3.py. Keep <=5k because the server caps it."""
    top = scratch_txt[:3200]
    before = scratch_txt[max(0, fn_start - 1600):fn_start]
    parts = [
        "PROJECT RULES: real C only; no inline asm; no .inc includes; C89 declarations first.",
        "KG TOP LEVERS:\n" + TOP_LEVERS,
        f"RESIDUAL CLASSIFIER FOR {fn}:\n{residual}",
        "FILE PRELUDE / DECLARATIONS:\n" + top,
        "LOCAL PRECEDING CONTEXT:\n" + before,
    ]
    return _clip("\n\n".join(parts), 5000)


def gen(asm, fn, current, context, n=N_CAND, temp=TEMP, draft=None, diff=None):
    if SERIAL_GEN and n > 1:
        out = []
        for _ in range(n):
            out.extend(gen(asm, fn, current, context, n=1, temp=temp, draft=draft, diff=diff))
        return out
    body = json.dumps({
        "asm": asm,
        "fn": fn,
        "current": current,
        "context": context,
        "draft": draft,
        "diff": diff,
        "n": n,
        "temp": temp,
        "max_new": MAX_NEW,
    }).encode()
    req = urllib.request.Request(SERVER, data=body,
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=400) as r:
        return json.loads(r.read()).get("candidates", [])


def rename_def(c, fn):
    """Rename the candidate's function definition to the real target fn name."""
    m = re.search(r"\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{", c)
    if not m:
        return None
    name = m.group(1)
    return c if name == fn else c.replace(name, fn)


def _hash(s):
    import hashlib
    return hashlib.md5(s.encode()).hexdigest()


def log_attempt(rec):
    TRAIN.parent.mkdir(parents=True, exist_ok=True)
    with TRAIN.open("a", encoding="utf-8") as f:
        f.write(json.dumps(rec, ensure_ascii=True) + "\n")


def claim_file():
    LOCKD.mkdir(parents=True, exist_ok=True)
    for ln in QUEUE.read_text().splitlines():
        ln = ln.strip()
        if not ln or ln.startswith("#"):
            continue
        parts = ln.split()
        f = parts[0]
        if not (ROOT / f).exists():
            continue
        d = LOCKD / _hash(f)
        try:
            d.mkdir()
        except FileExistsError:
            continue
        (d / "owner").write_text("seed")
        (d / "file").write_text(f)
        return f, parts[1:]
    return None, []


def main():
    print(f"[seed] lane up — server {SERVER}  tag {TAG}", flush=True)
    src_root = ROOT
    while True:
        f, fns = claim_file()
        if not f:
            print("[seed] queue drained — idle 90s", flush=True)
            time.sleep(90)
            continue
        src = (ROOT / f).resolve()
        print(f"[seed] ===== {f} ({len(fns)} fns) {time.strftime('%H:%M:%S')} =====", flush=True)
        ic = band("init", TAG, f, "--reset")
        if ic.returncode != 0:
            print(f"[seed] init failed: {ic.stderr[:200]}", flush=True)
            continue
        try:
            cc.compile_source(src)  # ensure a base obj exists for asm dumping
            tgt = cc.find_target_obj(src)
            base_obj = cc.source_to_base_obj(src)
        except SystemExit:
            print("[seed] baseline compile failed, skip file", flush=True)
            continue
        base = measure()
        for fn in fns:
            cur = base.get(fn, 0.0)
            if cur >= 100.0:
                continue
            asm = L.target_asm(tgt, base_obj, fn)
            if not asm:
                print(f"[seed]   {fn}: no asm, skip", flush=True)
                continue
            scratch_txt = SCRATCH.read_text(encoding="latin-1")
            span = L.find_fn_span(scratch_txt, fn)
            if not span:
                continue
            s0, e0, _ = span
            current_c = scratch_txt[s0:e0]
            residual = residual_hint(fn)
            context = file_context(scratch_txt, s0, fn, residual)
            try:
                cands = gen(asm, fn, current_c, context)
            except Exception as e:
                print(f"[seed]   {fn}: server error {e}", flush=True)
                log_attempt({"ts": time.time(), "role": "seed", "src": f, "fn": fn,
                             "before": cur, "error": str(e), "residual": residual})
                continue
            best_pct, best_txt = cur, None
            scored = []
            for i, c in enumerate(cands):
                rc = rename_def(c, fn)
                if not rc:
                    scored.append({"idx": i, "pct": None, "error": "no function definition", "body": c})
                    continue
                SCRATCH.write_text(scratch_txt[:s0] + rc + scratch_txt[e0:], encoding="latin-1")
                pct = measure().get(fn, 0.0)
                scored.append({"idx": i, "pct": pct, "body": rc})
                if pct > best_pct:
                    best_pct, best_txt = pct, rc
            if best_txt is not None and best_pct < 100.0 and REPAIR_N > 0:
                SCRATCH.write_text(scratch_txt[:s0] + best_txt + scratch_txt[e0:], encoding="latin-1")
                diff = _clip(band("diff", TAG, fn, timeout=180).stdout, 4200)
                try:
                    repairs = gen(asm, fn, current_c, context, n=REPAIR_N,
                                  temp=REPAIR_TEMP, draft=best_txt, diff=diff)
                except Exception as e:
                    repairs = []
                    scored.append({"idx": "repair-error", "pct": None, "error": str(e)})
                for i, c in enumerate(repairs):
                    rc = rename_def(c, fn)
                    if not rc:
                        scored.append({"idx": f"repair-{i}", "pct": None,
                                       "error": "no function definition", "body": c})
                        continue
                    SCRATCH.write_text(scratch_txt[:s0] + rc + scratch_txt[e0:], encoding="latin-1")
                    pct = measure().get(fn, 0.0)
                    scored.append({"idx": f"repair-{i}", "pct": pct, "body": rc})
                    if pct > best_pct:
                        best_pct, best_txt = pct, rc
            # commit the best draft into the scratch (or restore baseline)
            if best_txt is not None:
                SCRATCH.write_text(scratch_txt[:s0] + best_txt + scratch_txt[e0:], encoding="latin-1")
                tag = "  *100% BANKED*" if best_pct >= 100.0 else ""
                print(f"[seed]   {fn}: {cur:.1f}% -> {best_pct:.1f}%{tag}", flush=True)
                if best_pct >= 100.0:
                    band("save", TAG, fn)
                base = measure()
            else:
                SCRATCH.write_text(scratch_txt, encoding="latin-1")
                print(f"[seed]   {fn}: {cur:.1f}% (no improvement from {len(cands)} drafts)", flush=True)
            log_attempt({
                "ts": time.time(),
                "role": "seed",
                "server": SERVER,
                "src": f,
                "fn": fn,
                "before": cur,
                "after": best_pct,
                "n": N_CAND,
                "repair_n": REPAIR_N,
                "temp": TEMP,
                "repair_temp": REPAIR_TEMP,
                "max_new": MAX_NEW,
                "residual": residual,
                "kg_top_levers": TOP_LEVERS,
                "current": current_c,
                "asm": asm,
                "candidates": scored,
                "selected": best_txt,
            })
        print(f"[seed] ===== done {f} {time.strftime('%H:%M:%S')} =====", flush=True)


if __name__ == "__main__":
    main()
