#!/usr/bin/env python3
"""harvest.py - one-command zero-token pipeline for a source file.

Chains the deterministic stages and gates every commit through
verify_commit.py, so a whole file is processed (and safely committed)
with a single invocation instead of the manual wave choreography:

    diffclass (classify, informational)
      -> automatch  --apply --jobs N   (pragma sweep)        -> commit
      -> autorewrite --apply --jobs N  (mechanical rewrites)  -> commit

Each commit is verified: if verify_commit.py rejects it (truth-file edit,
asm-wrapper flip) the commit is rolled back (`git reset --hard HEAD~1`).
A net match%-regression in a stage is already auto-reverted inside
automatch/autorewrite before it writes anything.

Usage:
    python tools/harvest.py src/game/gs_render.c --jobs 6
    python tools/harvest.py src/game/scene_init.c --band 88 99.99 -j 8
    python tools/harvest.py src/game/foo.c --no-commit   # leave changes staged
"""

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PY = sys.executable


def sh(*args, cwd=ROOT):
    return subprocess.run(args, cwd=str(cwd), capture_output=True, text=True)


def git(*args):
    return sh("git", "-C", str(ROOT), *args)


def matched(src):
    sys.path.insert(0, str(ROOT / "tools"))
    import automatch
    m = automatch.measure(Path(src) if Path(src).is_absolute()
                           else ROOT / src, None)
    return automatch.matched_count(m) if m else None


def run_stage(tool, src, band, jobs, extra=()):
    cmd = [PY, str(ROOT / "tools" / tool), src,
           "--band", str(band[0]), str(band[1]),
           "--apply", "--jobs", str(jobs), *extra]
    print(f"\n[harvest] === {tool} (jobs={jobs}) ===")
    r = subprocess.run(cmd, cwd=str(ROOT))
    return r.returncode == 0


def commit_if_clean(src, label):
    """Stage src, commit, verify; roll back if verify_commit rejects."""
    st = git("status", "--porcelain", "--", src).stdout.strip()
    if not st:
        print(f"[harvest] {label}: no changes")
        return False
    git("add", "--", src)
    msg = (f"{Path(src).stem}: harvest {label} (zero-token)\n\n"
           f"Co-Authored-By: Claude Opus 4.7 (1M context) "
           f"<noreply@anthropic.com>")
    git("commit", "-q", "-m", msg)
    v = sh(PY, str(ROOT / "tools" / "verify_commit.py"),
           "--range", "HEAD~1..HEAD")
    print(v.stdout.strip().splitlines()[-1] if v.stdout.strip() else "")
    if v.returncode != 0:
        git("reset", "--hard", "HEAD~1")
        print(f"[harvest] {label}: REJECTED by verify_commit — rolled back")
        return False
    print(f"[harvest] {label}: committed + verified")
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--band", nargs=2, type=float, default=[85.0, 99.99])
    ap.add_argument("--jobs", "-j", type=int, default=4)
    ap.add_argument("--max-fns", type=int, default=0,
                    help="cap functions per stage (0 = no cap); keeps a "
                         "loop iteration inside the background-task cap")
    ap.add_argument("--no-commit", action="store_true")
    args = ap.parse_args()

    cap = (["--max-fns", str(args.max_fns)] if args.max_fns else [])

    src = args.source
    b0 = matched(src)
    print(f"[harvest] {src}: baseline {b0} fns @ 100%")

    # 1. classify (informational — also flags compiler-blocked fns)
    subprocess.run([PY, str(ROOT / "tools" / "diffclass.py"), src,
                    "--band", str(args.band[0]), str(args.band[1])],
                   cwd=str(ROOT))

    committed = []
    # 2. pragma sweep
    if run_stage("automatch.py", src, args.band, args.jobs, cap):
        if args.no_commit or commit_if_clean(src, "automatch"):
            committed.append("automatch")
    # 3. mechanical rewrites
    if run_stage("autorewrite.py", src, args.band, args.jobs, cap):
        if args.no_commit or commit_if_clean(src, "autorewrite"):
            committed.append("autorewrite")

    b1 = matched(src)
    print(f"\n[harvest] {src}: {b0} -> {b1} fns @ 100% "
          f"(+{(b1 or 0) - (b0 or 0)})  stages committed: "
          f"{committed or 'none'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
