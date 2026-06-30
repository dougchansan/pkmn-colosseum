#!/usr/bin/env python3
"""harvest_all.py - unattended zero-token sweep across near-miss files.

Now that harvest.py is one safe, gated command (diffclass -> automatch
-> autorewrite, every commit through verify_commit with auto-rollback),
this drives it across every file that has near-misses worth sweeping,
worst-first, within a time budget. Designed to run unattended (cron /
schedule skill / nightly): it only ever lands verified, regression-free
commits and skips anything risky.

Safety:
  * Skips files with uncommitted changes (an agent/human is editing it).
  * Skips files checked out in another git worktree (live agent).
  * Honors --skip globs and a wall-clock --budget; partial is fine.
  * Each file's wins are committed independently by harvest.py.

Usage:
    python tools/harvest_all.py --jobs 6 --budget 7200
    python tools/harvest_all.py --jobs 8 --max-files 10 --min-near 4
    python tools/harvest_all.py --dry-run            # just print the plan
"""

import argparse
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PY = sys.executable
sys.path.insert(0, str(ROOT / "tools"))


def git(*a):
    return subprocess.run(["git", "-C", str(ROOT), *a],
                           capture_output=True, text=True)


def worktree_busy_files():
    """Source files that other worktrees' branches may be editing — skip
    them so an unattended run never collides with a live agent."""
    busy = set()
    wt = git("worktree", "list", "--porcelain").stdout
    cur = str(ROOT).replace("\\", "/")
    for line in wt.splitlines():
        if line.startswith("worktree "):
            p = line.split(" ", 1)[1].strip().replace("\\", "/")
            if p != cur:
                # any uncommitted edits in that worktree
                st = subprocess.run(["git", "-C", p, "status",
                                     "--porcelain"],
                                    capture_output=True, text=True).stdout
                for s in st.splitlines():
                    f = s[3:].strip()
                    if f.endswith(".c"):
                        busy.add(f)
    return busy


def near_miss_files(min_near, band):
    """[(near_count, rel_src)] sorted worst-first, via measure cache."""
    import measure_cache
    base = ROOT / "build" / "GC6E01" / "base"
    tgt = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
    out = []
    for o in sorted(base.rglob("*.o")):
        funcs = measure_cache.diff_funcs(tgt, o)
        if not funcs:
            continue
        near = [f for f in funcs
                if band[0] <= f["match"] < band[1]]
        if len(near) < min_near:
            continue
        rel = o.relative_to(base).with_suffix(".c")
        src = f"src/{rel.as_posix()}"
        if (ROOT / src).exists():
            out.append((len(near), src))
    measure_cache.flush()
    out.sort(reverse=True)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", "-j", type=int, default=6)
    ap.add_argument("--band", nargs=2, type=float, default=[85.0, 99.99])
    ap.add_argument("--min-near", type=int, default=3)
    ap.add_argument("--max-files", type=int, default=99)
    ap.add_argument("--max-fns", type=int, default=25,
                    help="per-file function cap passed to harvest.py. "
                         "Default 25 keeps each file's harvest inside the "
                         "10-min background-task cap; the loop re-runs to "
                         "chip remaining functions over cycles.")
    ap.add_argument("--budget", type=int, default=7200,
                    help="wall-clock seconds; stop starting files past it")
    ap.add_argument("--skip", action="append", default=[],
                    help="substring of paths to skip (repeatable)")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    busy = worktree_busy_files()
    files = near_miss_files(args.min_near, args.band)

    # Eligible list (filters applied) BEFORE max_files truncation.
    eligible = []
    for near, src in files:
        if any(s in src for s in args.skip):
            continue
        if Path(src).name in {Path(b).name for b in busy} or src in busy:
            continue
        if git("status", "--porcelain", "--", src).stdout.strip():
            continue
        eligible.append((near, src))

    # Rotation cursor: a bounded loop with --max-files 1 + worst-first
    # would re-sweep the single worst file forever. Persist the last
    # file swept and start the next run AFTER it, so consecutive loop
    # iterations walk the whole near-miss set (wrapping) instead of
    # spinning on a dead slice.
    import json as _json
    cur_f = ROOT / "build" / ".harvest_cursor.json"
    last = ""
    try:
        last = _json.loads(cur_f.read_text()).get("last", "")
    except (OSError, ValueError):
        pass
    srcs = [s for _, s in eligible]
    if last in srcs:
        i = srcs.index(last) + 1
        eligible = eligible[i:] + eligible[:i]

    plan = eligible[:args.max_files]
    if plan:
        cur_f.parent.mkdir(parents=True, exist_ok=True)
        cur_f.write_text(_json.dumps({"last": plan[-1][1]}))

    print(f"[harvest-all] {len(plan)} files queued "
          f"(band {args.band[0]}-{args.band[1]}, >= {args.min_near} "
          f"near-misses), budget {args.budget}s, jobs {args.jobs}")
    for near, src in plan:
        print(f"   {near:>4} near  {src}")
    if args.dry_run:
        return 0

    t0 = time.time()
    done = 0
    for near, src in plan:
        if time.time() - t0 > args.budget:
            print(f"[harvest-all] budget reached — stopping "
                  f"({done}/{len(plan)} done)")
            break
        print(f"\n[harvest-all] === {src} ({near} near) ===")
        r = subprocess.run(
            [PY, str(ROOT / "tools" / "harvest.py"), src,
             "--band", str(args.band[0]), str(args.band[1]),
             "--jobs", str(args.jobs),
             "--max-fns", str(args.max_fns)],
            cwd=str(ROOT))
        done += 1
        print(f"[harvest-all] {src} exit {r.returncode} "
              f"({done}/{len(plan)}, {time.time()-t0:.0f}s)")

    print(f"\n[harvest-all] complete: {done} files swept in "
          f"{time.time()-t0:.0f}s, zero tokens")
    return 0


if __name__ == "__main__":
    sys.exit(main())
