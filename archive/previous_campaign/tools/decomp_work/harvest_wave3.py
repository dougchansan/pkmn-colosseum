#!/usr/bin/env python3
"""Harvest wave-3: re-verify (confirmed + unverified-rejected) in parent, commit 100%s."""
import json, re, subprocess, sys, os
ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
OUT = r"C:\Users\DOUGLA~1\AppData\Local\Temp\claude\C--Users-douglaswhittingham-pkmn-colosseum\31c04acb-00af-4d12-bedb-20d96524391b\tasks\w61nrcv4d.output"

def git(a): return subprocess.run(["git","-C",ROOT]+a,capture_output=True,text=True)

data = json.load(open(OUT, encoding="utf-8"))["result"]
winners = {}
for f in data:
    fp = f["file"].replace("\\","/")
    fns = [x["function"] for x in (f.get("confirmed") or [])]
    # include rejected-for-no-evidence (env failures), parent decides
    fns += [r for r in (f.get("rejected") or [])]
    if fns:
        winners[fp] = list(dict.fromkeys(fns))

committed = {}; failed = {}
for fp, fns in winners.items():
    cmd = [sys.executable,"tools/compile_check.py",fp,"--diff","--require-match","--symbols"]+fns
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    out = r.stdout + r.stderr
    good = [fn for fn in fns if (re.search(re.escape(fn)+r"\s+([0-9.]+)%", out) or [None]) and
            (lambda m: m and m.group(1)=="100.0000")(re.search(re.escape(fn)+r"\s+([0-9.]+)%", out))]
    bad = [fn for fn in fns if fn not in good]
    if not good:
        failed[fp] = bad or ["none-100"]; print(f"[SKIP] {fp}: {bad}"); continue
    git(["add","--",fp])
    diff = git(["diff","--cached","--",fp]).stdout
    if re.search(r"^\+\s*#if\s+1\b", diff, re.M) and re.search(r"asm\s+void", diff):
        git(["reset","HEAD","--",fp]); failed[fp]=["reactivation"]; print(f"[ABORT] {fp}"); continue
    msg = f"{os.path.basename(fp).replace('.c','')}: {len(good)} fn(s) to 100% (wave-3 trk/dolphin harvest)\n\n"+" ".join(good)+"\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>"
    c = git(["commit","-q","-m",msg])
    if c.returncode==0: committed[fp]=good; print(f"[COMMIT] {fp}: {len(good)} ({'partial '+str(bad) if bad else 'all'})")
    else: failed[fp]=["commit-fail"]; print(f"[ERR] {fp}: {c.stderr[:120]}")
print(f"\n==== committed {sum(len(v) for v in committed.values())} fns across {len(committed)} files ====")
for fp,v in committed.items(): print(f"  {fp}: {' '.join(v)}")
if failed: print("failed:", failed)
