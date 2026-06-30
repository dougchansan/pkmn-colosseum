#!/usr/bin/env python3
"""Commit 3 stub conversions + bank 13 reclaim files, with full fraud battery:
no tracked .inc tampered, no reactivated asm wrappers, scope-clean, per-file gate."""
import re, subprocess, sys, os
ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
def git(a): return subprocess.run(["git","-C",ROOT]+a,capture_output=True,text=True)
def gate():
    r=subprocess.run([sys.executable,"tools/verify_gate.py","--range","HEAD~1..HEAD"],capture_output=True,text=True,cwd=ROOT)
    return "=== verify_gate: PASS ===" in r.stdout, r.stdout

CONV = {  # stub conversions (genuine active-C, re-verified 100% in parent)
 "src/game/gs_field_world.c": ("stub conversion to 100% (fn_8012A248: pseudo-register->real signature)", ["fn_8012A248"]),
 "src/game/battle/battle_scene.c": ("stub conversion to 100% (fn_801CB59C: block-local typed externs)", ["fn_801CB59C"]),
 "src/hsd/hsd_object.c": ("stub conversion to 100% (fn_80191484)", ["fn_80191484"]),
}
BANK = ["src/dolphin/exi/EXI2.c","src/dolphin/os/OSCache.c","src/game/gs_render_util.c",
 "src/game/gs_thread.c","src/game/gs_worldmap.c","src/game/input/input.c",
 "src/game/people/people.c","src/game/people/people_field.c","src/hsd/hsd_cobj.c",
 "src/hsd/hsd_jobj.c","src/hsd/hsd_lobj.c","src/hsd/hsd_wobj.c","src/trk/TRKTarget.c"]

# GLOBAL FRAUD GUARD 1: no tracked .inc / .h / config / symbols modified
bad = [l for l in git(["diff","--name-only"]).stdout.splitlines()
       if re.search(r"\.(inc|h)$|config/|symbols", l.strip())]
if bad:
    print("ABORT: tracked truth/config files are modified (possible tamper):"); [print("  "+b) for b in bad]
    sys.exit(1)
print("OK: no tracked .inc/.h/config modified.\n")

def react(f):
    d = git(["diff","--",f]).stdout
    return bool(re.search(r"^\+\s*#if\s+1\b", d, re.M)) and "asm void" in d

committed=[]; skipped=[]
def do(f, msg):
    if react(f): skipped.append((f,"REACTIVATION")); print(f"[SKIP] {f}: reactivation"); return
    git(["add","--",f])
    c=git(["commit","-q","-m",msg+"\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>"])
    if c.returncode!=0: skipped.append((f,"nothing/err")); print(f"[SKIP] {f}: {c.stderr[:80]}"); return
    ok,out=gate()
    if not ok:
        # only acceptable reject is none here (these are .c-only). revert.
        git(["reset","--mixed","HEAD~1"]); skipped.append((f,"GATE-REJECT")); print(f"[REVERT] {f}: gate reject\n{out[-300:]}"); return
    committed.append(f); print(f"[COMMIT+GATE] {f}")

print("== stub conversions ==")
for f,(msg,fns) in CONV.items(): do(f, f"{os.path.basename(f)[:-2]}: {msg}")
print("\n== reclaim bank (prior-lane net-positive matches) ==")
for f in BANK: do(f, f"{os.path.basename(f)[:-2]}: reclaim prior-lane matches (net-positive, no regression, fraud-checked)")

print(f"\n==== committed {len(committed)} files, skipped {len(skipped)} ====")
for f in committed: print(f"  OK   {f}")
for f,r in skipped: print(f"  SKIP {f} ({r})")
