#!/usr/bin/env python3
"""Apply + verify the SDA extern-sizing sweep per TU. Keep only net-positive,
zero-regression changes; revert any TU that regresses or doesn't improve."""
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
                return {k: v for k, v in json.loads(ln).items()
                        if isinstance(v, (int, float))}
            except Exception:
                return None
    return None

def git_checkout(rel):
    subprocess.run(["git", "checkout", "--", rel], cwd=REPO,
                   capture_output=True, text=True)

def main():
    files = sys.argv[1:]
    if not files:
        # default: all candidate files from discover
        out = subprocess.run([PY, os.path.join(HERE, "sda_extern_sweep.py"), "discover"],
                             capture_output=True, text=True, cwd=REPO).stdout
        files = [ln[3:].split("  ")[0].strip() for ln in out.splitlines()
                 if ln.startswith("## ")]
    kept_total, results = 0, []
    for rel in files:
        base = band_json("_swp", rel)
        if not base:
            results.append((rel, "SKIP", "baseline compile failed", []))
            print(f"SKIP {rel}: baseline compile failed", flush=True)
            continue
        n = subprocess.run([PY, os.path.join(HERE, "sda_extern_sweep.py"), "apply", rel],
                           capture_output=True, text=True, cwd=REPO).stdout.strip()
        new = band_json("_swp", rel)
        if not new:
            git_checkout(rel)
            results.append((rel, "REVERT", "post-apply compile failed", []))
            print(f"REVERT {rel}: post-apply compile failed", flush=True)
            continue
        gains, regs = [], []
        for fn, b in base.items():
            a = new.get(fn, b)
            if a > b + EPS:
                gains.append((fn, round(b, 2), round(a, 2)))
            elif a < b - EPS:
                regs.append((fn, round(b, 2), round(a, 2)))
        if regs:
            git_checkout(rel)
            results.append((rel, "REVERT-REGRESS", n, regs))
            print(f"REVERT {rel}: regressions {regs}", flush=True)
        elif gains:
            kept_total += len(gains)
            results.append((rel, "KEEP", n, gains))
            print(f"KEEP  {rel}: gains {gains}", flush=True)
        else:
            git_checkout(rel)
            results.append((rel, "REVERT-NOCHANGE", n, []))
            print(f"REVERT {rel}: no change", flush=True)
    json.dump([{"file": r, "verdict": v, "applied": a, "fns": f} for r, v, a, f in results],
              open(os.path.join(HERE, "coordination", "sda_sweep_results.json"), "w"), indent=1)
    print(f"\nDONE. functions improved (kept): {kept_total}", flush=True)
    print("KEPT files:", [r for r, v, _, _ in results if v == "KEEP"], flush=True)

if __name__ == "__main__":
    main()
