#!/usr/bin/env python3
"""wall_ledger.py — systematic coverage ledger for the whole function universe.

Classifies EVERY function (from report.json) into a bucket, cross-referenced with
the active-asm-wrapper list, equivalent.txt, and WALLS.md, so we can systematically
work each bucket and never lose track of what's been attempted.

Buckets:
  DONE      real decompiled C, byte-exact (>=99.95% and NOT an asm-wrapper)
  ASM       still an active asm-wrapper (matches 100% trivially, NOT decompiled) -> from-scratch target
  NEARWALL  real C 95-99.95% -> reg-alloc / scheduler wall, prime cracking target
  STRUCT    real C 70-95%    -> wrong shape/types, needs rework
  LOW       real C <70%      -> early draft / wrong
  EQUIV     registered in equivalent.txt (correct C, wall-accepted, counts on C-axis)

Usage:
  wall_ledger.py build              rebuild build/wall_ledger.json + docs/wall_ledger.md
  wall_ledger.py summary            print bucket + per-file counts
  wall_ledger.py next <bucket> [N]  list N closest unattempted targets in a bucket
  wall_ledger.py mark <fn> <note>   mark a fn attempted (appends to attempts ledger)
"""
import json, os, re, sys
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
REPORT = os.path.join(ROOT, "report.json")
ASMW = os.path.join(ROOT, "build", "asmw.txt")
EQUIV = os.path.join(ROOT, "tools", "decomp_work", "equivalent.txt")
WALLS = os.path.join(ROOT, "WALLS.md")
LEDGER = os.path.join(ROOT, "build", "wall_ledger.json")
ATTEMPTS = os.path.join(ROOT, "build", "wall_attempts.txt")
MD = os.path.join(ROOT, "docs", "wall_ledger.md")


def _load_set(path, pat=r"(fn_[0-9A-Fa-f]+)"):
    s = set()
    if os.path.exists(path):
        for line in open(path, encoding="utf-8", errors="replace"):
            m = re.search(pat, line)
            if m:
                s.add(m.group(1))
    return s


def _source_asm_set():
    """Return active asm-wrapper symbols from the source classifier.

    build/asmw.txt is a useful cache when it exists, but the dashboard should
    not lose the ASM bucket just because that cache was not regenerated.
    """
    try:
        sys.path.insert(0, os.path.dirname(__file__))
        import progress2

        _counts, fn2class = progress2.classify_all()
        return {fn for fn, cls in fn2class.items() if cls == "ASM"}
    except Exception as e:
        print(f"(warning: source ASM scan unavailable: {e})", file=sys.stderr)
        return set()


def classify(pct, fn, asm):
    if fn in asm:
        return "ASM"
    if pct >= 99.95:
        return "DONE"
    if pct >= 95.0:
        return "NEARWALL"
    if pct >= 70.0:
        return "STRUCT"
    return "LOW"


def build():
    rep = json.load(open(REPORT, encoding="utf-8"))
    asm = _source_asm_set() | _load_set(ASMW)
    equiv = _load_set(EQUIV)
    walls = _load_set(WALLS)
    attempted = _load_set(ATTEMPTS) | walls | equiv
    led = {}
    for u in rep.get("units", []):
        filename = u.get("name", "?")
        for f in u.get("functions", []):
            fn = f.get("name")
            if not fn:
                continue
            pct = f.get("fuzzy_match_percent")
            if pct is None:
                continue
            b = "EQUIV" if fn in equiv else classify(pct, fn, asm)
            led[fn] = {"pct": round(pct, 2), "bucket": b, "file": filename,
                       "size": f.get("size", 0), "attempted": fn in attempted}
    json.dump(led, open(LEDGER, "w"), indent=0)
    _write_md(led)
    return led


def _counts(led):
    from collections import Counter, defaultdict
    bc = Counter(v["bucket"] for v in led.values())
    att = Counter(v["bucket"] for v in led.values() if v["attempted"])
    return bc, att


def _write_md(led):
    from collections import Counter, defaultdict
    bc, att = _counts(led)
    order = ["DONE", "EQUIV", "NEARWALL", "STRUCT", "LOW", "ASM"]
    lines = ["# Wall Ledger — function coverage by bucket", ""]
    lines.append(f"Total functions: {len(led)}")
    lines.append("")
    lines.append("| Bucket | Count | Attempted | Remaining | Meaning |")
    lines.append("|---|---|---|---|---|")
    mean = {"DONE": "byte-exact real C (achieved)", "EQUIV": "correct C, wall-accepted (C-axis)",
            "NEARWALL": "95-99.95% reg-alloc/scheduler wall — CRACK TARGET",
            "STRUCT": "70-95% wrong shape/types — rework", "LOW": "<70% early/wrong",
            "ASM": "still asm-wrapper — from-scratch decomp target"}
    for b in order:
        rem = bc[b] - att[b]
        lines.append(f"| {b} | {bc[b]} | {att[b]} | {rem} | {mean[b]} |")
    # per-file crack-target (NEARWALL) breakdown
    byfile = defaultdict(list)
    for fn, v in led.items():
        if v["bucket"] == "NEARWALL":
            byfile[v["file"]].append((v["pct"], fn, v["attempted"]))
    lines += ["", "## NEARWALL crack targets by file (closest-first)", ""]
    for fileName, fns in sorted(byfile.items(), key=lambda kv: -len(kv[1]))[:25]:
        un = sum(1 for _, _, a in fns if not a)
        top = sorted(fns, reverse=True)[:3]
        ts = ", ".join(f"{fn}@{p}{'*' if a else ''}" for p, fn, a in top)
        lines.append(f"- **{fileName}** ({len(fns)} walls, {un} unattempted): {ts}")
    os.makedirs(os.path.dirname(MD), exist_ok=True)
    open(MD, "w", encoding="utf-8").write("\n".join(lines) + "\n")


def summary():
    led = json.load(open(LEDGER)) if os.path.exists(LEDGER) else build()
    bc, att = _counts(led)
    print("bucket      total  attempted  remaining")
    for b in ["DONE", "EQUIV", "NEARWALL", "STRUCT", "LOW", "ASM"]:
        print(f"  {b:9s} {bc[b]:5d}  {att[b]:8d}  {bc[b]-att[b]:8d}")
    print(f"  {'TOTAL':9s} {len(led):5d}")


def nxt(bucket, n=20):
    led = json.load(open(LEDGER)) if os.path.exists(LEDGER) else build()
    rows = [(v["pct"], fn, v["file"]) for fn, v in led.items()
            if v["bucket"] == bucket.upper() and not v["attempted"]]
    rows.sort(reverse=True)
    for pct, fn, fl in rows[:int(n)]:
        print(f"  {pct:6.2f}  {fn}  {fl}")


def mark(fn, note=""):
    with open(ATTEMPTS, "a", encoding="utf-8") as fh:
        fh.write(f"{fn}  {note}\n")
    print(f"marked {fn}")


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "summary"
    if cmd == "build":
        build(); summary()
    elif cmd == "summary":
        summary()
    elif cmd == "next":
        nxt(sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else 20)
    elif cmd == "mark":
        mark(sys.argv[2], " ".join(sys.argv[3:]))
    else:
        print(__doc__)
