#!/usr/bin/env python3
"""bench_v3_vs.py — single-shot compile+match benchmark of the current models on
a suite, scored by the LOCAL Mac gate (mwcceppc via wibo + objdiff). Compares the
SeedCoder-8B-v3 flywheel model against the fleet's other generators.

Each model gets its BEST prompt mode:
  v3   -> the simple asm prompt it was trained on (serve_v3 on the 3090)
  glm  -> z.ai GLM-5.2 with the suite's CW-rules prompt
  opus/sonnet/codex -> single-shot via claude -p / codex exec (CW prompt)

Measurement is isolated via band.py private scratch (won't touch the live fleet's
files). PASS = 100% byte-exact.

  python bench_v3_vs.py [--models v3,glm] [--suite test_suite.json] [--n 2]
"""
import argparse
import json
import os
import re
import subprocess
import sys
import time
import urllib.request
from collections import defaultdict
from pathlib import Path

BENCH = Path(__file__).resolve().parent
ROOT = BENCH.parents[2]
sys.path.insert(0, str(ROOT / "tools"))
import lane_glm as L          # noqa: E402  find_fn_span
BAND = ROOT / "tools" / "decomp_work" / "band.py"
SCR = ROOT / "tools" / "decomp_work" / "scratch"

GPU = os.environ.get("DECOMP_GPU_HOST", "192.168.50.101")
V3_URL = f"http://{GPU}:8780/gen"
ZAI_URL = "https://api.z.ai/api/coding/paas/v4/chat/completions"


def band(*a, timeout=180):
    return subprocess.run([sys.executable, str(BAND), *a], cwd=str(ROOT),
                          capture_output=True, text=True, timeout=timeout)


def zai_key():
    f = Path.home() / ".config" / "decomp-keys" / "cockpit.env"
    if f.exists():
        for ln in f.read_text().splitlines():
            m = re.match(r"\s*export\s+ZAI_API_KEY=(.+)", ln)
            if m:
                return m.group(1).strip().strip('"').strip("'").split()[0]
    return os.environ.get("ZAI_API_KEY", "")


def gen_v3(item, n):
    body = json.dumps({"asm": item["asm"], "n": n, "temp": 0.4}).encode()
    req = urllib.request.Request(V3_URL, data=body, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=400) as r:
        return json.loads(r.read()).get("candidates", [])


def gen_glm(item, n):
    key = zai_key()
    out = []
    for _ in range(n):
        body = json.dumps({"model": "glm-4.6", "max_tokens": 1500, "temperature": 0.4,
                           "messages": [{"role": "user", "content": item["prompt"] + "\n\n```\n" + item["asm"] + "\n```"}]}).encode()
        req = urllib.request.Request(ZAI_URL, data=body, headers={
            "Content-Type": "application/json", "Authorization": f"Bearer {key}"})
        with urllib.request.urlopen(req, timeout=200) as r:
            txt = json.loads(r.read())["choices"][0]["message"].get("content", "") or ""
        m = re.search(r"```c?\s*\n(.*?)```", txt, re.S)
        out.append((m.group(1) if m else txt).strip())
    return out


def gen_agent(cli, model, item, n):
    out = []
    prompt = item["prompt"] + "\n\n```\n" + item["asm"] + "\n```\nOutput ONLY the C function."
    for _ in range(n):
        if cli == "claude":
            cmd = ["claude", "-p", "--model", model, prompt]
        else:
            cmd = ["codex", "exec", "--skip-git-repo-check", prompt]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=300, cwd=str(ROOT))
            txt = r.stdout
        except Exception:
            txt = ""
        m = re.search(r"```c?\s*\n(.*?)```", txt, re.S)
        out.append((m.group(1) if m else txt).strip())
    return out


GENERATORS = {
    "v3": gen_v3,
    "glm": gen_glm,
    "opus": lambda it, n: gen_agent("claude", "opus", it, n),
    "sonnet": lambda it, n: gen_agent("claude", "sonnet", it, n),
    "codex": lambda it, n: gen_agent("codex", "", it, n),
}


def rename_def(c, fn):
    m = re.search(r"\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{", c)
    if not m:
        return None
    name = m.group(1)
    return c if name == fn else c.replace(name, fn)


def measure_candidate(tag, file, fn, cand):
    """Splice cand into the band scratch for `file`, return fn match% (or None=fail)."""
    scratch = SCR / f"band_{tag}.c"
    if not scratch.exists():
        ic = band("init", tag, file)
        if ic.returncode != 0 or not scratch.exists():
            return None
    base = scratch.read_text(encoding="latin-1")
    rc = rename_def(cand, fn)
    if not rc:
        return None
    span = L.find_fn_span(base, fn)
    if not span:
        return None
    s0, e0, _ = span
    scratch.write_text(base[:s0] + rc + base[e0:], encoding="latin-1")
    try:
        d = json.loads(band("json", tag).stdout)
    except Exception:
        d = {}
    scratch.write_text(base, encoding="latin-1")  # restore
    return d.get(fn)  # None if didn't compile / not found


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--models", default="v3,glm")
    ap.add_argument("--suite", default="test_suite.json")
    ap.add_argument("--n", type=int, default=2, help="best-of-n candidates per fn")
    ap.add_argument("--limit", type=int, default=0)
    a = ap.parse_args()

    suite = json.load(open(BENCH / a.suite))
    if a.limit:
        suite = suite[:a.limit]
    models = [m for m in a.models.split(",") if m in GENERATORS]

    results = {m: [] for m in models}
    for m in models:
        print(f"\n=== model: {m} ===", flush=True)
        t0 = time.time()
        for it in suite:
            fn, file = it["function"], it["file"]
            if not (ROOT / file).exists():
                continue
            import hashlib
            tag = "bench_" + hashlib.md5(file.encode()).hexdigest()[:8]  # per-file scratch (shared across models)
            try:
                cands = GENERATORS[m](it, a.n)
            except Exception as e:
                print(f"  {fn}: GEN ERROR {e}", flush=True)
                results[m].append((fn, None, False))
                continue
            best = None
            compiled = False
            for c in cands:
                pct = measure_candidate(tag, file, fn, c)
                if pct is not None:
                    compiled = True
                    if best is None or pct > best:
                        best = pct
            results[m].append((fn, best, compiled))
            print(f"  {fn:14s} {file.split('/')[-1]:18s} best={best if best is not None else 'FAIL':>6} "
                  f"compiled={compiled}", flush=True)
        results[m] = (results[m], time.time() - t0)

    # leaderboard
    print("\n\n========== LEADERBOARD (suite: %s, %d fns, best-of-%d) ==========" % (a.suite, len(suite), a.n))
    print(f"{'model':8s} {'PASS(100%)':>11s} {'compile%':>9s} {'mean%':>7s} {'time':>7s}")
    board = []
    for m in models:
        rows, dt = results[m]
        scored = [p for _, p, _ in rows if p is not None]
        passes = sum(1 for p in scored if p >= 100.0)
        comp = sum(1 for _, _, c in rows if c)
        mean = sum(scored) / len(scored) if scored else 0.0
        board.append((mean, m, passes, comp, len(rows), dt))
    for mean, m, passes, comp, n, dt in sorted(board, reverse=True):
        print(f"{m:8s} {passes:>4d}/{n:<6d} {100*comp/n:>8.0f}% {mean:>6.1f}% {dt:>6.0f}s")
    # save
    out = BENCH / "results" / f"bench_v3_vs_{int(time.time())}.json"
    out.write_text(json.dumps({m: results[m][0] for m in models}, indent=1))
    print(f"\nsaved {out}")


if __name__ == "__main__":
    main()
