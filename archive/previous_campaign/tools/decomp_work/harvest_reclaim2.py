#!/usr/bin/env python3
"""Harvest reclaimed-stub conversions: re-verify each in parent, commit fraud-checked, gate."""
import json, re, subprocess, sys, os
ROOT=r"C:\Users\douglaswhittingham\pkmn-colosseum"
def git(a): return subprocess.run(["git","-C",ROOT]+a,capture_output=True,text=True)
def gate():
    r=subprocess.run([sys.executable,"tools/verify_gate.py","--range","HEAD~1..HEAD"],capture_output=True,text=True,cwd=ROOT)
    return "=== verify_gate: PASS ===" in r.stdout, r.stdout

conv=json.load(open(os.path.join(ROOT,"tools/decomp_work/reports/reclaim_conv.json")))
# global truth-tamper guard
bad=[l for l in git(["diff","--name-only"]).stdout.splitlines() if re.search(r"\.(inc|h)$|config/|symbols",l.strip())]
if bad:
    print("ABORT: tracked truth/config modified:"); [print(" ",b) for b in bad]; sys.exit(1)
print("OK no truth/config tamper.\n")
committed=[]; failed=[]
for f,fns in conv.items():
    r=subprocess.run([sys.executable,"tools/compile_check.py",f,"--diff","--require-match","--symbols"]+fns,
                     capture_output=True,text=True,cwd=ROOT)
    out=r.stdout+r.stderr
    bad=[fn for fn in fns if not (lambda m:m and m.group(1)=="100.0000")(re.search(re.escape(fn)+r"\s+([0-9.]+)%",out))]
    if bad: failed.append((f,bad)); print(f"[SKIP] {f}: not 100% {bad}"); continue
    diff=git(["diff","--",f]).stdout
    if re.search(r"^\+\s*#if\s+1\b",diff,re.M) and "asm void" in diff:
        failed.append((f,["reactivation"])); print(f"[SKIP] {f}: reactivation"); continue
    git(["add","--",f])
    base=os.path.basename(f)[:-2]
    git(["commit","-q","-m",f"{base}: {len(fns)} stub(s) to 100% (reclaimed lane, fresh conversion)\n\n{' '.join(fns)}\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>"])
    ok,g=gate()
    if not ok:
        git(["reset","--mixed","HEAD~1"]); failed.append((f,["gate"])); print(f"[REVERT] {f}: gate reject\n{g[-200:]}"); continue
    committed.append((f,len(fns))); print(f"[COMMIT+GATE] {f} ({len(fns)})")
print(f"\n==== committed {sum(n for _,n in committed)} fns / {len(committed)} files ====")
for f,n in committed: print(f"  +{n}  {f}")
if failed: print("failed:",failed)
PY = None
