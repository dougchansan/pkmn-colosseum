#!/usr/bin/env python3
"""seed_permute.py <src/file.c> <fn> [iters] — the v3 -> permuter handoff (the
real-world phased flow, all local on the Mac):

  phase 0  v3 drafts a near-miss (free, fast)         -> apply best draft to the fn
  phase 1  local annealer (permute_match.py) searches  -> decl-reorder x pragma over
           the SAME compile_check/wibo oracle for the byte-exact match

The seed lane already OWNS the file (fleet lock), so editing the real source for the
anneal is safe; the original is always restored at the end. A win (100%) is printed
for banking; otherwise the function is left at its best near-miss for the next phase.
"""
import json
import re
import subprocess
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
import lane_glm as L          # noqa: E402
import compile_check as cc    # noqa: E402
V3 = "http://192.168.50.101:8780/gen"


def gen(asm, n=4, temp=0.4):
    req = urllib.request.Request(V3, data=json.dumps({"asm": asm, "n": n, "temp": temp}).encode(),
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=400) as r:
        return json.loads(r.read()).get("candidates", [])


def rename_def(c, fn):
    m = re.search(r"\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{", c)
    return None if not m else (c if m.group(1) == fn else c.replace(m.group(1), fn))


def pct(src, tgt, fn):
    o = cc.compile_source(src)
    return cc._summarize_symbol(cc._run_objdiff_json(tgt, o, fn), fn)["match_percent"]


def main():
    file, fn = sys.argv[1], sys.argv[2]
    iters = int(sys.argv[3]) if len(sys.argv) > 3 else 400
    src = (ROOT / file).resolve()
    cc.compile_source(src)
    tgt = cc.find_target_obj(src)
    base = cc.source_to_base_obj(src)
    asm = L.target_asm(tgt, base, fn)
    orig = src.read_text(encoding="latin-1")
    span = L.find_fn_span(orig, fn)
    if not span:
        print("no C definition for", fn); return
    s0, e0, _ = span

    b = cc._summarize_symbol(cc._run_objdiff_json(tgt, base, fn), fn)["match_percent"]
    print(f"baseline {fn}: {b:.1f}%", flush=True)

    # phase 0 — best v3 draft
    best_pct, best = b, None
    for c in gen(asm):
        rc = rename_def(c, fn)
        if not rc:
            continue
        src.write_text(orig[:s0] + rc + orig[e0:], encoding="latin-1")
        try:
            p = pct(src, tgt, fn)
        except SystemExit:
            continue
        if p > best_pct:
            best_pct, best = p, rc
    print(f"phase 0  v3 draft: {best_pct:.1f}%", flush=True)
    if best is None:
        src.write_text(orig, encoding="latin-1")
        print("no compilable improvement — nothing to anneal"); return

    # phase 1 — anneal the draft (permute_match owns its own backup/restore)
    src.write_text(orig[:s0] + best + orig[e0:], encoding="latin-1")
    print(f"phase 1  annealing {iters} iters (decl-reorder x pragma)...", flush=True)
    try:
        r = subprocess.run([sys.executable, "tools/decomp_work/permute_match.py", file, fn,
                            "--iters", str(iters)], cwd=str(ROOT), capture_output=True,
                           text=True, timeout=900)
        tail = "\n".join(l for l in r.stdout.splitlines()
                         if re.search(r"WIN|NOWIN|best|score|match|%|exact", l, re.I))[-1200:]
        print(tail or r.stdout[-600:], flush=True)
        won = "WIN" in r.stdout or re.search(r"100\.0+%|score 0|exact", r.stdout)
    finally:
        src.write_text(orig, encoding="latin-1")  # always restore original
    print(f"=== {fn}: {'BYTE-EXACT WIN (v3 draft + anneal)' if won else f'best near-miss {best_pct:.1f}% (needs lever/agent phase)'} ===")


if __name__ == "__main__":
    main()
