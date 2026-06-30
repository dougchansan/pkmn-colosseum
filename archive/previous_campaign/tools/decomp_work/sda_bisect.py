#!/usr/bin/env python3
"""Bisect a multi-candidate TU that the all-apply sweep reverted on regression:
test each candidate symbol individually, keep only the gainers (non-regressors),
then apply all gainers together and final-verify."""
import json, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
PY = sys.executable
EPS = 0.01

def band_json(tag, src):
    subprocess.run([PY, os.path.join(HERE, "band.py"), "init", tag, src],
                   capture_output=True, text=True, cwd=REPO)
    r = subprocess.run([PY, os.path.join(HERE, "band.py"), "json", tag],
                       capture_output=True, text=True, cwd=REPO)
    for ln in r.stdout.splitlines():
        ln = ln.strip()
        if ln.startswith("{"):
            try:
                return {k: v for k, v in json.loads(ln).items() if isinstance(v, (int, float))}
            except Exception:
                return None
    return None

def checkout(rel):
    subprocess.run(["git", "checkout", "--", rel], cwd=REPO, capture_output=True, text=True)

def apply_one(rel, sym):
    subprocess.run([PY, os.path.join(HERE, "sda_extern_sweep.py"), "apply", rel, sym],
                   capture_output=True, text=True, cwd=REPO)

def syms_for(rel):
    out = subprocess.run([PY, os.path.join(HERE, "sda_extern_sweep.py"), "discover"],
                         capture_output=True, text=True, cwd=REPO).stdout
    res, cur = [], None
    for ln in out.splitlines():
        if ln.startswith("## "):
            cur = ln[3:].split("  ")[0].strip()
        elif ln.strip().startswith("L") and cur == rel.replace("\\", "/"):
            res.append(ln.split()[1])
    # de-dup preserving order
    seen, uniq = set(), []
    for s in res:
        if s not in seen:
            seen.add(s); uniq.append(s)
    return uniq

def diff(base, new):
    gains, regs = [], []
    for fn, b in base.items():
        a = new.get(fn, b)
        if a > b + EPS: gains.append((fn, round(b, 2), round(a, 2)))
        elif a < b - EPS: regs.append((fn, round(b, 2), round(a, 2)))
    return gains, regs

def main():
    rel = sys.argv[1].replace("\\", "/")
    checkout(rel)
    base = band_json("_bis", rel)
    if not base:
        print(f"{rel}: baseline compile failed"); return
    good = []
    for s in syms_for(rel):
        checkout(rel)
        apply_one(rel, s)
        new = band_json("_bis", rel)
        checkout(rel)
        if not new:
            print(f"  {s}: compile failed -> skip", flush=True); continue
        g, r = diff(base, new)
        if r:
            print(f"  {s}: REGRESS {r} -> drop", flush=True)
        elif g:
            print(f"  {s}: GAIN {g} -> keep", flush=True); good.append(s)
        else:
            print(f"  {s}: no change -> drop", flush=True)
    if not good:
        checkout(rel); print(f"{rel}: nothing to keep"); return
    # apply all gainers together, final verify
    checkout(rel)
    for s in good:
        apply_one(rel, s)
    new = band_json("_bis", rel)
    g, r = diff(base, new)
    if r:
        checkout(rel)
        print(f"{rel}: COMBINED regressed {r} -> reverted all (needs manual)", flush=True)
    else:
        print(f"{rel}: KEPT {len(good)} sym(s), gains {g}", flush=True)

if __name__ == "__main__":
    main()
