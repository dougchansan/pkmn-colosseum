#!/usr/bin/env python3
"""update_readme_progress.py [YYYY-MM-DD] — sync the README Status table to the
current report.json measures, so the headline numbers never go stale. Idempotent:
prints nothing-changed if already current. Run from the auto-push loop.
"""
import json, os, re, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
REPORT = os.path.join(ROOT, "report.json")
README = os.path.join(ROOT, "README.md")


def fl(m, k):
    try:
        return float(m.get(k, 0) or 0)
    except (TypeError, ValueError):
        return 0.0


def main():
    if not (os.path.exists(REPORT) and os.path.exists(README)):
        return
    m = json.load(open(REPORT, encoding="utf-8")).get("measures", {})
    fm, ft = int(fl(m, "matched_functions")), int(fl(m, "total_functions"))
    cm, ct = int(fl(m, "matched_code")), int(fl(m, "total_code"))
    if not ft or not ct:
        return
    fpct, cpct = 100.0 * fm / ft, 100.0 * cm / ct
    date = sys.argv[1] if len(sys.argv) > 1 else None
    txt = open(README, encoding="utf-8").read()
    new = txt
    new = re.sub(r"\| Function match \| [^|]*\|",
                 f"| Function match | ~{fpct:.1f}% ({fm:,} / {ft:,} functions) |", new)
    new = re.sub(r"\| Code match \| [^|]*\|",
                 f"| Code match | ~{cpct:.1f}% ({cm:,} / {ct:,} matched code bytes) |", new)
    if date:
        new = re.sub(r"Last measured \d{4}-\d{2}-\d{2}", f"Last measured {date}", new)
    if new != txt:
        open(README, "w", encoding="utf-8").write(new)
        print(f"README updated: functions ~{fpct:.1f}% ({fm}/{ft}), code ~{cpct:.1f}% ({cm}/{ct})")
    else:
        print("README already current")


if __name__ == "__main__":
    main()
