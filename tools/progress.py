#!/usr/bin/env python3
"""Scan every compiled .o and emit a progress report.

Walks `build/GC6E01/base/**/*.o`, runs objdiff-cli against the unified target,
and aggregates match% by file. Output is JSON-compatible with
https://decomp.dev's project source schema (loose adaptation).

Usage:
    python3 tools/progress.py                       # text summary to stdout
    python3 tools/progress.py --json report.json    # JSON output
    python3 tools/progress.py --threshold 100       # only count 100% as matched

The badge URL on decomp.dev reads from a hosted report.json. To wire that up
you'd register the repo at https://decomp.dev/new and have CI push report.json
to a known location, then the README shield syntax becomes:
    [![Progress]](https://decomp.dev/<user>/<repo>.svg)
"""

import os
import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

ROOT = Path(__file__).resolve().parent.parent
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE_DIR = ROOT / "build" / "GC6E01" / "base"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
SKIP_BASE_PREFIXES = ("pcport/",)
# test_va*: host-only varargs probes. The remaining four are DUPLICATE measurement
# units that double-count a real TU and inflated the published metric (gs_field_world
# and gs_title were counted twice — once top-level, once under game/; gs_render_w2 and
# pokemon_boss2 are 94-95% copies of game/gs_render and game/pokemon). Skip the dup so
# only the canonical game/-prefixed unit is measured. (Confirmed via function-set overlap.)
SKIP_BASE_NAMES = {
    "test_va2.o", "test_va3.o",
    "gs_field_world.o", "gs_title.o",      # top-level dups of game/gs_field_world, game/gs_title
    "game/gs_render_w2.o", "pokemon_boss2.o",  # synthetic copies of game/gs_render, game/pokemon
}
# Band-harness / integration SCRATCH objects must never be counted as real
# translation units. band_integrate.py and the band harness drop transient
# `band_*`, `*_integrated`, `_int_*`, `_evt_*`, `cs_*` objects into base/ for
# measurement; if collect() enumerates them they become phantom units that
# duplicate a real TU (e.g. band_trainer_integrated/manual_integrated each
# duplicated game/trainer's 380 fns) and inflate the denominator — which is
# exactly what dropped decomp_code_pct 53%->44% with the DOL still byte-exact.
# The basename (stem, sans ".o") is matched against this pattern.
SKIP_BASE_SCRATCH = re.compile(
    r"^(band_|manual_integrated|_int_|_evt_|cs_)|_integrated$|__a_fn_",
    re.IGNORECASE,
)


def _is_scratch_base(rel: str) -> bool:
    stem = rel.rsplit("/", 1)[-1]
    if stem.endswith(".o"):
        stem = stem[:-2]
    return bool(SKIP_BASE_SCRATCH.search(stem))


def iter_base_objects():
    for o in sorted(BASE_DIR.rglob("*.o")):
        rel = o.relative_to(BASE_DIR).as_posix()
        if rel.startswith(SKIP_BASE_PREFIXES) or rel in SKIP_BASE_NAMES:
            continue
        if _is_scratch_base(rel):
            continue
        yield rel, o


def diff_one(base_o: Path) -> dict:
    try:
        import measure_cache
        cached = measure_cache.diff_funcs(TARGET_O, base_o)
    except Exception:
        cached = None
    if cached is None:
        return {"error": "objdiff failed", "functions": []}
    return {"functions": cached}


def collect() -> dict:
    files = {}
    for rel, o in iter_base_objects():
        d = diff_one(o)
        funcs = d.get("functions", [])
        if not funcs:
            continue
        matched = sum(1 for f in funcs if f["match"] >= 100)
        matched_bytes = sum(f["size"] for f in funcs if f["match"] >= 100)
        total_bytes = sum(f["size"] for f in funcs)
        files[rel] = {
            "functions": len(funcs),
            "matched": matched,
            "total_bytes": total_bytes,
            "matched_bytes": matched_bytes,
            "match_pct": (100.0 * matched / len(funcs)) if funcs else 0.0,
            "bytes_pct": (100.0 * matched_bytes / total_bytes) if total_bytes else 0.0,
        }
    return files


def summarize(files: dict) -> dict:
    total_fns = sum(f["functions"] for f in files.values())
    matched_fns = sum(f["matched"] for f in files.values())
    total_bytes = sum(f["total_bytes"] for f in files.values())
    matched_bytes = sum(f["matched_bytes"] for f in files.values())
    return {
        "total_functions": total_fns,
        "matched_functions": matched_fns,
        "match_pct": (100.0 * matched_fns / total_fns) if total_fns else 0.0,
        "total_bytes": total_bytes,
        "matched_bytes": matched_bytes,
        "bytes_pct": (100.0 * matched_bytes / total_bytes) if total_bytes else 0.0,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="Write full report to this path")
    ap.add_argument("--no-files", action="store_true",
                    help="Skip per-file breakdown in text output")
    args = ap.parse_args()

    if not TARGET_O.exists():
        sys.exit(f"target .o missing: {TARGET_O}")
    if not BASE_DIR.exists():
        sys.exit(f"build dir missing: {BASE_DIR}")

    files = collect()
    overall = summarize(files)

    report = {
        "schema": "pkmn-colosseum/progress/v1",
        "overall": overall,
        "files": files,
    }
    if args.json:
        Path(args.json).write_text(json.dumps(report, indent=2), encoding="utf-8")
        print(f"wrote {args.json}")

    # Text summary
    print(f"Overall: {overall['matched_functions']}/{overall['total_functions']} "
          f"functions ({overall['match_pct']:.2f}%), "
          f"{overall['matched_bytes']:,}/{overall['total_bytes']:,} bytes "
          f"({overall['bytes_pct']:.2f}%)")

    if not args.no_files:
        print()
        # Sort by inverse match pct (worst first) for actionability
        sorted_files = sorted(files.items(), key=lambda kv: kv[1]["match_pct"])
        for rel, f in sorted_files:
            if f["functions"] == 0:
                continue
            print(f"  {f['match_pct']:5.1f}%  {f['matched']}/{f['functions']:>4d}  "
                  f"{f['matched_bytes']:>8,}/{f['total_bytes']:<8,}B  {rel}")
    try:
        import measure_cache
        measure_cache.flush()
    except Exception:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
