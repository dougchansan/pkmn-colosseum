#!/usr/bin/env python3
"""Prune stale/duplicate saved band wins.

The fleet reuses build/band_wins/pl_<lane>.json as long-lived ledgers. When a
new win touches one of those files, auto_gate would otherwise re-review every
historical entry in the tag. This tool removes entries that are already
byte-exact in a clean canonical source, and removes duplicate function/source
entries across selected ledgers while keeping one copy.

By default this is a dry run. Pass --write to update the JSON files.
"""
import argparse
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
import band  # noqa: E402


def _wins_files(names):
    if not names:
        return sorted(band.WINS.glob("pl_*.json"))
    out = []
    for name in names:
        p = Path(name)
        if p.exists():
            out.append(p)
            continue
        if not name.endswith(".json"):
            name = f"{name}.json"
        out.append(band.WINS / name)
    return sorted({p.resolve() for p in out if p.exists()})


def _fn_keys(data):
    return [k for k in data if k not in band.META_KEYS]


def _src_for(data, fn):
    srcs = data.get("_srcs")
    if isinstance(srcs, dict) and srcs.get(fn):
        return srcs[fn]
    return data.get("_src")


def _remove(data, fn):
    data.pop(fn, None)
    for key in ("_srcs", "_pct"):
        meta = data.get(key)
        if isinstance(meta, dict):
            meta.pop(fn, None)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--write", action="store_true", help="rewrite bundle JSON files")
    ap.add_argument("--quiet", action="store_true")
    ap.add_argument("--no-dedupe", action="store_true", help="only prune already-exact entries")
    ap.add_argument("bundles", nargs="*", help="bundle paths or tags, defaults to build/band_wins/pl_*.json")
    args = ap.parse_args()

    files = _wins_files(args.bundles)
    if not files:
        if not args.quiet:
            print("no band win bundles")
        return 0

    data_by_file = {}
    entries = []
    for path in files:
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, ValueError) as exc:
            print(f"WARN: skip {path}: {exc}", file=sys.stderr)
            continue
        data_by_file[path] = data
        for fn in _fn_keys(data):
            src = _src_for(data, fn)
            if src:
                entries.append({"path": path, "fn": fn, "src": src})

    rows_by_src = {}
    dirty_srcs = set()
    stale = set()
    for entry in entries:
        src = entry["src"]
        if src in dirty_srcs:
            continue
        if src not in rows_by_src:
            if band.source_is_dirty(src):
                dirty_srcs.add(src)
                continue
            rows_by_src[src] = band.canonical_rows(src)
        if rows_by_src[src].get(entry["fn"], 0.0) >= band.MATCH_FLOOR:
            stale.add((entry["path"], entry["fn"]))

    duplicate = set()
    if not args.no_dedupe:
        kept = {}
        # Prefer the newest touched bundle; ties are stable by path.
        ordered = sorted(
            (e for e in entries if (e["path"], e["fn"]) not in stale),
            key=lambda e: (-e["path"].stat().st_mtime, str(e["path"]), e["src"], e["fn"]),
        )
        for entry in ordered:
            key = (entry["src"], entry["fn"])
            item = (entry["path"], entry["fn"])
            if key in kept:
                duplicate.add(item)
            else:
                kept[key] = item

    removed_by_file = {p: [] for p in data_by_file}
    for path, fn in sorted(stale | duplicate, key=lambda x: (str(x[0]), x[1])):
        if path in data_by_file:
            _remove(data_by_file[path], fn)
            removed_by_file[path].append(fn)

    if args.write:
        for path, data in data_by_file.items():
            if removed_by_file[path]:
                path.write_text(json.dumps(data, indent=1) + "\n", encoding="utf-8")

    kept_count = sum(len(_fn_keys(d)) for d in data_by_file.values())
    removed_stale = len(stale)
    removed_dupe = len(duplicate)
    if not args.quiet:
        mode = "updated" if args.write else "dry-run"
        print(f"{mode}: {len(files)} bundle(s), kept {kept_count}, "
              f"remove {removed_stale} already-exact, {removed_dupe} duplicate")
        if dirty_srcs:
            print("skipped dirty source exact-prune: " + " ".join(sorted(dirty_srcs)))
        for path in files:
            removed = removed_by_file.get(path, [])
            if removed:
                print(f"  {path.relative_to(ROOT)}: removed {len(removed)}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
