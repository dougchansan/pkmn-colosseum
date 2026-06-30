#!/usr/bin/env python3
"""route_queue.py - batch why_diff classifier over _winnable_queue.md.

Runs why_diff.py on each WINNABLE near-miss and buckets it by the FIRST-divergence
category into an actionable routed worklist, so workers/permuter attack the RIGHT
thing instead of grinding blind. Writes coordination/routed_worklist.json + .md.

Buckets:
  permuter   SCHEDULE-SWAP / REG-RENAME (same save-set)  -> anneal_pragma (autonomous)
  worker-fr  FRAME-SIZE / SAVE-SET-DIFF / SPILL-DIFF     -> worker + frame/save lever
  worker-ex  DIFFERENT-INSN (expression)                 -> worker + expression lever
  reloc-wall why_diff MATCH but objdiff<100              -> reloc artifact, leave walled
  stub       ours ~<=2 insn vs large target              -> full Phase-1 decomp
  blocked    FAIL (NO_COMPILE / no base / no target)     -> compile-blocked TU
"""
import json, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
QUEUE = os.path.join(HERE, "_winnable_queue.md")
WHYDIFF = os.path.join(HERE, "why_diff.py")
LIMIT = int(sys.argv[1]) if len(sys.argv) > 1 else 30
JOBS = int(sys.argv[2]) if len(sys.argv) > 2 else 3

# build basename -> src rel path map
PATHMAP = {}
for dp, _, fns in os.walk(os.path.join(REPO, "src")):
    for fn in fns:
        if fn.endswith(".c"):
            PATHMAP.setdefault(fn, os.path.relpath(os.path.join(dp, fn), REPO).replace("\\", "/"))

ROW = re.compile(r"^\|\s*([0-9]+\.[0-9]+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\w+)\s*\|\s*(fn_[0-9A-Fa-f]+)\s*\|\s*([\w.]+)\s*\|")
rows = []
for ln in open(QUEUE, encoding="utf-8", errors="replace"):
    m = ROW.match(ln.strip())
    if m:
        match, mm, struct, cls, fn, fl = m.groups()
        rows.append({"match": float(match), "mm": int(mm), "struct": int(struct),
                     "cls": cls, "fn": fn, "file": fl})
rows = rows[:LIMIT]

def classify(r):
    fn, fl = r["fn"], r["file"]
    src = PATHMAP.get(fl)
    if not src:
        return {**r, "bucket": "blocked", "why": f"no src path for {fl}"}
    try:
        p = subprocess.run([sys.executable, WHYDIFF, fn, src],
                           capture_output=True, text=True, timeout=240,
                           env=dict(os.environ, MSYS_NO_PATHCONV="1"))
        out = p.stdout + p.stderr
    except subprocess.TimeoutExpired:
        return {**r, "src": src, "bucket": "blocked", "why": "why_diff timeout"}
    if "failed to produce asm pair" in out or "does not compile" in out or "no base.c" in out or "no target.o" in out:
        return {**r, "src": src, "bucket": "blocked", "why": out.strip().splitlines()[-1][:120] if out.strip() else "no asm pair"}
    # parse "ours: N insns" and first finding
    mo = re.search(r"ours:\s+(\d+) insns, frame 0x([0-9A-Fa-f]+)", out)
    mt = re.search(r"target:\s+(\d+) insns", out)
    our_insn = int(mo.group(1)) if mo else -1
    tgt_insn = int(mt.group(1)) if mt else -1
    f1 = re.search(r"^\[1\] ([A-Z-]+): (.+)$", out, re.M)
    cat = f1.group(1) if f1 else "?"
    summ = f1.group(2)[:90] if f1 else ""
    # capture the finding[1] detail block (t|/o| lines) to refine DIFFERENT-INSN
    det = re.findall(r"^\s+([to])\| (.+)$", out, re.M)
    tlines = " ".join(x for s, x in det if s == "t")
    olines = " ".join(x for s, x in det if s == "o")
    # SDA-reloc artifact: target uses r13/r2 small-data addressing (or @sda21),
    # ours uses absolute @ha/@l -> section/reloc placement, NOT C-fixable.
    sda = (("(r13)" in tlines or "(r2)" in tlines or "@sda21" in tlines)
           and "(r13)" not in olines and "(r2)" not in olines)
    if our_insn >= 0 and our_insn <= 2 and tgt_insn > 10:
        bucket = "stub"
    elif cat == "MATCH":
        bucket = "reloc-wall"
    elif cat == "DIFFERENT-INSN" and sda:
        bucket = "reloc-wall"; summ = "SDA(r13) vs absolute: " + summ
    elif cat in ("SCHEDULE-SWAP", "REG-RENAME"):
        bucket = "permuter"
    elif cat in ("FRAME-SIZE", "SAVE-SET-DIFF", "SPILL-DIFF"):
        bucket = "worker-fr"
    elif cat == "DIFFERENT-INSN":
        bucket = "worker-ex"
    else:
        bucket = "other"
    return {**r, "src": src, "bucket": bucket, "cat": cat, "our_insn": our_insn,
            "tgt_insn": tgt_insn, "why": summ}

results = []
with ThreadPoolExecutor(max_workers=JOBS) as ex:
    for i, res in enumerate(ex.map(classify, rows), 1):
        results.append(res)
        print(f"[{i}/{len(rows)}] {res['fn']:<14} {res['bucket']:<10} {res.get('cat','')} {res.get('why','')[:60]}", flush=True)

out_json = os.path.join(HERE, "coordination", "routed_worklist.json")
json.dump(results, open(out_json, "w"), indent=1)

# markdown grouped by bucket
buckets = {}
for r in results:
    buckets.setdefault(r["bucket"], []).append(r)
order = ["permuter", "worker-ex", "worker-fr", "stub", "reloc-wall", "other", "blocked"]
md = ["# Routed near-miss worklist (why_diff) — 2026-06-17\n",
      f"Classified top {len(results)} of `_winnable_queue.md` via why_diff first-divergence.\n"]
for b in order:
    if b not in buckets: continue
    md.append(f"\n## {b}  ({len(buckets[b])})\n")
    md.append("| match | fn | file | cat | note |\n|---|---|---|---|---|")
    for r in sorted(buckets[b], key=lambda x: -x["match"]):
        md.append(f"| {r['match']} | {r['fn']} | {r['file']} | {r.get('cat','')} | {r.get('why','')[:70]} |")
open(os.path.join(HERE, "coordination", "routed_worklist.md"), "w").write("\n".join(md))
print("\nDONE. counts:", {b: len(v) for b, v in buckets.items()})
