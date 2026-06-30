#!/usr/bin/env python3
"""Quarantine non-actionable rows from saved band-win ledgers.

Input is a parent `band_integrate.py` log. Rows that dropped, failed compile,
failed splice, referenced missing canonical sources, or were rejected as asm are
removed from active `build/band_wins/pl_*.json` files and copied to a timestamped
archive under `build/band_wins_quarantine/`.

Held rows are kept by default. With `--drop-held-same`, held rows whose integrated
file is byte-identical to current canon are also archived as already represented.
"""
import argparse
import datetime
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
WINS = ROOT / "build" / "band_wins"
OUTDIR = ROOT / "build" / "band_wins_quarantine"
META_KEYS = {"_src", "_srcs", "_pct"}

HEADER_RE = re.compile(r"^===\s+(.+?)\s+\((\d+)\s+saved win\(s\)\)\s+===")
DROP_RE = re.compile(r"\bDROP\s+(\S+)\s+([0-9.]+)%")
HELD_RE = re.compile(r"\bHELD\s+(\S+)\s+([0-9.]+)%")
REJECT_RE = re.compile(r"\bREJECT-ASM\s+(\S+)\s+([0-9.]+)%")


def fn_keys(data):
    return [k for k in data if k not in META_KEYS]


def src_for(data, fn):
    srcs = data.get("_srcs")
    if isinstance(srcs, dict) and srcs.get(fn):
        return srcs[fn]
    return data.get("_src")


def remove_fn(data, fn):
    body = data.pop(fn, None)
    meta = {}
    for key in ("_srcs", "_pct"):
        value = data.get(key)
        if isinstance(value, dict) and fn in value:
            meta[key] = value.pop(fn)
    return body, meta


def integrated_path(src):
    stem = Path(src).stem
    return ROOT / "build" / f"band_{stem}_integrated.c"


def parse_log(path):
    current = None
    held = {}
    remove = {}
    source_fail = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = HEADER_RE.match(line)
        if m:
            current = m.group(1)
            continue
        if current is None:
            continue
        m = HELD_RE.search(line)
        if m:
            held.setdefault(current, {})[m.group(1)] = float(m.group(2))
            continue
        m = DROP_RE.search(line)
        if m:
            remove.setdefault(current, {})[m.group(1)] = {
                "reason": "drop",
                "pct": float(m.group(2)),
            }
            continue
        m = REJECT_RE.search(line)
        if m:
            remove.setdefault(current, {})[m.group(1)] = {
                "reason": "reject_asm",
                "pct": float(m.group(2)),
            }
            continue
        if "MEASURE FAILED:" in line:
            source_fail[current] = "measure_failed"
        elif "SPLICE FAILED:" in line:
            source_fail[current] = "splice_failed"
        elif "SKIP: canonical source missing:" in line:
            source_fail[current] = "missing_source"
    return held, remove, source_fail


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log", help="parent band_integrate log")
    ap.add_argument("--write", action="store_true", help="rewrite active ledgers")
    ap.add_argument("--drop-held-same", action="store_true",
                    help="archive held rows whose integrated file matches current canon")
    ap.add_argument("--all-json", action="store_true",
                    help="process every build/band_wins/*.json, not only pl_*.json")
    args = ap.parse_args()

    log_path = Path(args.log)
    held, explicit_remove, source_fail = parse_log(log_path)
    files = sorted(WINS.glob("*.json" if args.all_json else "pl_*.json"))
    archive = {
        "log": str(log_path),
        "created_utc": datetime.datetime.now(datetime.UTC).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "entries": [],
    }
    kept_held_diff = 0
    removed = 0

    for path in files:
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            continue
        changed = False
        for fn in list(fn_keys(data)):
            src = src_for(data, fn)
            if not src:
                continue
            info = explicit_remove.get(src, {}).get(fn)
            if info is None and src in source_fail:
                info = {"reason": source_fail[src], "pct": None}
            if info is None and args.drop_held_same and fn in held.get(src, {}):
                int_path = integrated_path(src)
                canon = ROOT / src
                if int_path.exists() and canon.exists() and int_path.read_bytes() == canon.read_bytes():
                    info = {"reason": "held_already_in_canon", "pct": held[src][fn]}
                else:
                    kept_held_diff += 1
            if info is None:
                continue
            body, meta = remove_fn(data, fn)
            archive["entries"].append({
                "bundle": str(path.relative_to(ROOT)),
                "src": src,
                "fn": fn,
                "reason": info["reason"],
                "pct": info.get("pct"),
                "body": body,
                "meta": meta,
            })
            changed = True
            removed += 1
        if changed and args.write:
            path.write_text(json.dumps(data, indent=1) + "\n", encoding="utf-8")

    if args.write and archive["entries"]:
        OUTDIR.mkdir(parents=True, exist_ok=True)
        stamp = datetime.datetime.now(datetime.UTC).strftime("%Y%m%dT%H%M%SZ")
        out = OUTDIR / f"quarantine_{stamp}.json"
        out.write_text(json.dumps(archive, indent=1) + "\n", encoding="utf-8")
        print(f"archive {out.relative_to(ROOT)}")
    print(f"{'updated' if args.write else 'dry-run'}: remove {removed}, "
          f"kept held-diff {kept_held_diff}, files {len(files)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
