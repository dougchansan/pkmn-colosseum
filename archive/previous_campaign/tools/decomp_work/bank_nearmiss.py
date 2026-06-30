#!/usr/bin/env python3
"""bank_nearmiss.py — splice BANKED real-C near-misses into canon with strict no-regress,
so the permuter (refill_queue.py) can finish the last 1-10%.

Reads build/band_nearmiss/<tag>.json (written by `band.py bank`: real-C fns at [90,100)%).
For each fn it splices the body into canon and KEEPS it ONLY if the spliced result:
  (a) is real C        -- band_integrate's asm/.inc fraud guard rejects wrappers, and
  (b) re-measures >= FLOOR and < 100, and
  (c) STRICTLY beats the function's CURRENT committed pct (no-regress).
The committed baseline is measured with the SAME objdiff path as the re-measure (so the
comparison is apples-to-apples: a byte-matching asm wrapper measures ~99-100 and thus can
NEVER be replaced by a <100 real-C body -- this is the farming-regression guard).

Banked fns are applied WITHOUT the Equivalent tag, so refill_queue keeps queueing them for
the annealer to push to 100%.

usage:
  python bank_nearmiss.py                  # DRY RUN: show what would be banked (no src change)
  python bank_nearmiss.py --apply          # splice the improvements into src/
  python bank_nearmiss.py --apply --queue  # ...then refresh the permuter queue (refill_queue)
  python bank_nearmiss.py --floor 92       # raise the no-regress floor (default 90)
Run with the same python as band_integrate."""
import json
import re
import sys
import subprocess
from pathlib import Path

import band_integrate as bi   # reuse integrate_source (splice + re-measure + fraud guard)

ROOT = bi.ROOT
NEARMISS = bi.NEARMISS
EPS = 1e-6
_ASM_ADD = re.compile(r'(^|\s)asm\s+[\w*]+\s+\w+\s*\(|(^|\s)asm\s*\{|__asm\b|#include\s+"[^"]*\.inc"')


def run_files(apply, queue):
    """FILE-LEVEL bank for RESHAPE near-misses: commit a whole scratch snapshot
    (build/band_nearmiss/<tag>.file.c, from `band.py bank-file`) to canon ONLY if every
    function is no-regress vs the CURRENT committed src and at least one improves, and the
    snapshot adds no asm/.inc (fraud). The all-fns no-regress check also rejects a STALE
    snapshot (one whose other fns fell behind a concurrent commit)."""
    snaps = sorted(NEARMISS.glob("*.file.c"))
    if not snaps:
        print("no build/band_nearmiss/*.file.c — run `band.py bank-file <tag>` first.")
        return 0
    print(f"[bank_nearmiss --files] {'APPLY' if apply else 'DRY-RUN'} | strict all-fns "
          f"no-regress | {len(snaps)} file snapshot(s)\n")
    applied = 0
    for snap in snaps:
        tag = snap.name[:-len(".file.c")]
        srcf = NEARMISS / f"{tag}.file.src"
        if not srcf.exists():
            print(f"== {snap.name}: SKIP (no .src sidecar)")
            continue
        src = srcf.read_text(encoding="utf-8").strip()
        canon = ROOT / src
        if not canon.exists():
            print(f"== {src}: SKIP (canon missing)")
            continue
        stem = Path(src).stem
        print(f"== {src}  (snapshot {tag}) ==")
        snap_m, e1 = bi._scratch_json(f"{stem}_snap", snap, canon)
        base_m, e2 = bi._scratch_json(stem, canon, canon)
        if snap_m is None or base_m is None:
            print(f"   SKIP: measure failed ({'snapshot' if snap_m is None else 'canon baseline'})")
            continue
        canon_lines = set(canon.read_text(errors="replace").splitlines())
        added_asm = [l for l in snap.read_text(errors="replace").splitlines()
                     if l not in canon_lines and _ASM_ADD.search(l)]
        if added_asm:
            print(f"   REJECT (fraud): snapshot adds asm/.inc, e.g. {added_asm[0].strip()[:70]}")
            continue
        regressed = [(fn, base_m[fn], snap_m.get(fn, 0.0)) for fn in base_m
                     if snap_m.get(fn, 0.0) < base_m[fn] - EPS]
        improved = [(fn, base_m.get(fn, 0.0), snap_m[fn]) for fn in snap_m
                    if snap_m[fn] > base_m.get(fn, 0.0) + EPS]
        if regressed:
            fn, c, s = regressed[0]
            print(f"   REJECT: {len(regressed)} fn(s) would REGRESS (e.g. {fn} {c:.1f}%->{s:.1f}%) "
                  f"— stale snapshot or bad edit; not committing")
            continue
        if not improved:
            print("   SKIP: no fn improves over canon (nothing to bank)")
            continue
        print(f"   OK: {len(improved)} fn(s) improve, 0 regress:")
        for fn, c, s in sorted(improved, key=lambda x: -(x[2] - x[1]))[:10]:
            print(f"     {fn}  {c:.1f}% -> {s:.1f}%")
        if apply:
            canon.write_bytes(snap.read_bytes())
            snap.unlink()
            srcf.unlink()
            applied += 1
            print(f"   APPLIED -> {src}  (snapshot consumed)")
    print(f"\n[bank_nearmiss --files] {'committed ' + str(applied) if apply else 'WOULD commit'} file(s)")
    if not apply:
        print("  (dry run — re-run with --apply to commit the held files; --queue to refresh permuter)")
    if apply and queue and applied:
        print("[bank_nearmiss] refreshing permuter queue (refill_queue.py) ...")
        subprocess.run(["wsl", "python3", "tools/decomp_work/permuter/refill_queue.py",
                        "--min", "90", "--max", "99.999", "--n", "300"], cwd=str(ROOT))
    return 0


def main():
    raw = sys.argv[1:]
    apply = "--apply" in raw
    queue = "--queue" in raw
    if "--files" in raw:        # FILE-level reshape bank (whole scratch snapshot, all-fns no-regress)
        return run_files(apply, queue)
    floor = 90.0
    if "--floor" in raw:
        try:
            floor = float(raw[raw.index("--floor") + 1])
        except (IndexError, ValueError):
            print("--floor needs a number, e.g. --floor 92")
            return 2

    if not NEARMISS.exists() or not any(NEARMISS.glob("*.json")):
        print("no build/band_nearmiss/*.json — run `band.py bank <tag> <fn>` first.")
        return 0

    # group banked bodies by per-fn source (mirrors band_integrate's _srcs handling)
    by_src = {}
    for wf in sorted(NEARMISS.glob("*.json")):
        try:
            d = json.loads(wf.read_text(encoding="utf-8"))
        except ValueError:
            print(f"WARN: unparseable {wf.name}; skipping")
            continue
        srcs = d.get("_srcs", {})
        default = d.get("_src")
        for fn, body in d.items():
            if fn in bi.META_KEYS:
                continue
            src = srcs.get(fn, default)
            if src:
                by_src.setdefault(src, {})[fn] = body

    if not by_src:
        print("nothing banked yet.")
        return 0
    nfn = sum(len(v) for v in by_src.values())
    print(f"[bank_nearmiss] {'APPLY' if apply else 'DRY-RUN'} | floor {floor:.0f}% | "
          f"strict no-regress | {nfn} banked fn(s) across {len(by_src)} file(s)\n")

    total = 0
    for src, bodies in sorted(by_src.items()):
        canon = ROOT / src
        if not canon.exists():
            print(f"== {src}: SKIP (canon missing)")
            continue
        stem = Path(src).stem
        print(f"== {src} ({len(bodies)} banked) ==")
        # committed baseline, measured the SAME way as the re-measure -> apples-to-apples
        base, err = bi._scratch_json(stem, canon, canon)
        if base is None:
            tail = (err or "").splitlines()[-1] if err else "?"
            print(f"   SKIP: baseline measure failed ({tail})")
            continue
        held, _dropped = bi.integrate_source(src, bodies, apply, min_pct=floor,
                                             bank=True, committed=base)
        total += len(held or [])

    print(f"\n[bank_nearmiss] {'banked (applied to src/)' if apply else 'WOULD bank'}: "
          f"{total} near-miss fn(s)")
    if not apply:
        print("  (dry run — re-run with --apply to splice into src/; add --queue to refresh "
              "the permuter queue)")
    if apply and queue and total:
        print("[bank_nearmiss] refreshing permuter queue (refill_queue.py) ...")
        subprocess.run(["wsl", "python3", "tools/decomp_work/permuter/refill_queue.py",
                        "--min", "90", "--max", "99.999", "--n", "300"], cwd=str(ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())
