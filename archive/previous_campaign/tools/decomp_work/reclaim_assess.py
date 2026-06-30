#!/usr/bin/env python3
"""Reclaim assessment: for each DIRTY .c file, measure 100%-match function set
at HEAD vs the current (uncommitted) state. Report net-new matches + any
regressions, so we can BANK net-positive files and REVERT broken ones.
Backup-safe: never loses the current dirty content.
"""
import re, subprocess, sys, os, shutil
ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"

def sh(args, **kw):
    return subprocess.run(args, capture_output=True, text=True, cwd=ROOT, **kw)

def git(args): return sh(["git"]+args)

def matches(srcfile):
    """Return (set_of_100pct_fns, total_fns, compiled_ok)."""
    r = sh([sys.executable, "tools/compile_check.py", srcfile, "--diff"])
    out = r.stdout + r.stderr
    if "COMPILE FAILED" in out or "Errors caused tool to abort" in out:
        return set(), 0, False
    rows = re.findall(r"((?:fn_[0-9A-Fa-f]+|[A-Za-z_]\w+))\s+([0-9.]+)%", out)
    if not rows:
        return set(), 0, False
    full = {n for n, p in rows if p == "100.0000"}
    return full, len(rows), True

dirty = [l.strip() for l in git(["diff","--name-only","--","*.c"]).stdout.splitlines()
         if l.strip().endswith(".c") and os.path.exists(os.path.join(ROOT, l.strip()))]

print(f"# {len(dirty)} dirty .c files\n")
print(f"{'head':>4} {'cur':>4} {'net':>4} {'reg':>4}  verdict        file")
bank, revert, review = [], [], []
for f in dirty:
    bak = os.path.join(ROOT, f + ".reclaim_bak")
    abs_f = os.path.join(ROOT, f)
    shutil.copy2(abs_f, bak)
    try:
        git(["checkout","HEAD","--",f])
        head_set, head_tot, head_ok = matches(f)
    finally:
        shutil.copy2(bak, abs_f)  # ALWAYS restore current
        os.remove(bak)
    cur_set, cur_tot, cur_ok = matches(f)
    if not cur_ok:
        verdict = "BROKEN-revert"; revert.append(f)
    else:
        regress = head_set - cur_set
        net = len(cur_set) - len(head_set)
        if regress:
            verdict = f"REGRESS({len(regress)})"; review.append((f, regress))
        elif net > 0:
            verdict = "BANK"; bank.append((f, net))
        else:
            verdict = "no-gain"
    print(f"{len(head_set):>4} {len(cur_set):>4} {len(cur_set)-len(head_set):>+4} "
          f"{len(head_set-cur_set):>4}  {verdict:14s} {f}")

print("\n=== BANK (net-positive, no regression) ===")
for f, n in bank: print(f"  +{n}  {f}")
print(f"  total net-new from banking: {sum(n for _,n in bank)}")
print("\n=== REVIEW (has regressions vs HEAD) ===")
for f, r in review: print(f"  {f}: lost {sorted(r)[:8]}")
print("\n=== BROKEN (doesn't compile, revert) ===")
for f in revert: print(f"  {f}")
