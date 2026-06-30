#!/usr/bin/env python3
"""Harvest wave-2 workflow results: re-verify in parent, commit per file.

Reads the 3 workflow task outputs, extracts per-file winners, runs
compile_check --require-match for each file, and commits ONLY files where
every claimed winner re-measures at exactly 100.0000%. Prints a summary.
"""
import json, re, subprocess, sys, os
from collections import OrderedDict

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
TASKS = r"C:\Users\DOUGLA~1\AppData\Local\Temp\claude\C--Users-douglaswhittingham-pkmn-colosseum\31c04acb-00af-4d12-bedb-20d96524391b\tasks"
OUTPUTS = {
    "batch1": os.path.join(TASKS, "wvwot096b.output"),
    "bigfile": os.path.join(TASKS, "wo5obuwnk.output"),
    "dynamic": os.path.join(TASKS, "wwz3k2a6w.output"),
}

def git(args):
    return subprocess.run(["git", "-C", ROOT] + args, capture_output=True, text=True)

# 1. consolidate winners
winners = OrderedDict()
for tag, p in OUTPUTS.items():
    try:
        d = json.load(open(p, encoding="utf-8"))["result"]
    except Exception as e:
        print(f"!! could not parse {tag}: {e}"); continue
    for f in d:
        fp = f["file"].replace("\\", "/")
        for x in (f.get("matched") or f.get("confirmed") or []):
            winners.setdefault(fp, [])
            if x["function"] not in winners[fp]:
                winners[fp].append(x["function"])

print(f"Consolidated: {sum(len(v) for v in winners.values())} winners across {len(winners)} files\n")

committed = {}
failed = {}
for fp, fns in winners.items():
    cmd = [sys.executable, "tools/compile_check.py", fp, "--diff", "--require-match", "--symbols"] + fns
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    out = r.stdout + r.stderr
    bad = []
    for fn in fns:
        m = re.search(re.escape(fn) + r"\s+([0-9.]+)%", out)
        if not m or m.group(1) != "100.0000":
            bad.append(fn + ("=" + m.group(1) + "%" if m else "=MISSING"))
    if bad:
        failed[fp] = bad
        print(f"[SKIP] {fp}: not all 100% -> {bad}")
        continue
    # scope guard: only this file gets staged
    git(["add", "--", fp])
    # reactivation guard: ensure staged diff didn't add `#if 1` before asm void
    diff = git(["diff", "--cached", "--", fp]).stdout
    react = re.search(r"^\+\s*#if\s+1\b", diff, re.M)
    if react and re.search(r"asm\s+void", diff):
        print(f"[ABORT] {fp}: staged diff adds #if 1 near asm void -- unstaging, NOT committing")
        git(["reset", "HEAD", "--", fp])
        failed[fp] = ["reactivation-suspected"]
        continue
    msg = f"{os.path.basename(fp).replace('.c','')}: {len(fns)} fn(s) to 100% (wave-2 triage+fix harvest)\n\n" + \
          " ".join(fns) + \
          "\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>"
    c = git(["commit", "-q", "-m", msg])
    if c.returncode == 0:
        committed[fp] = fns
        print(f"[COMMIT] {fp}: {len(fns)} fns")
    else:
        print(f"[ERR] {fp}: commit failed rc={c.returncode} {c.stderr[:200]}")
        failed[fp] = ["commit-failed"]

print("\n==== SUMMARY ====")
print(f"committed files: {len(committed)}  total fns: {sum(len(v) for v in committed.values())}")
for fp, fns in committed.items():
    print(f"  {fp}: {len(fns)}")
if failed:
    print(f"failed/skipped files: {len(failed)}")
    for fp, b in failed.items():
        print(f"  {fp}: {b}")
print("\nrecent log:")
print(git(["log", "--oneline", "-" + str(max(1, len(committed)))]).stdout)
