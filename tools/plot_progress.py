#!/usr/bin/env python3
"""Render the decompilation progress chart for the dtk matching campaign.

Two sources feed one series, both of them real objdiff measurements:

  * "readme"  — every commit that touched README.md carries the canonical
    dtk/objdiff status table, so the table's git history is a dense record of
    measured values from 2026-07-01 onward.
  * "rebuilt" — commits before the README table existed are measured directly:
    check the commit out, run the real dtk build, read build/GC6E01/report.json.
    `tools/harvest_progress.sh` produces these.

The campaign starts on 2026-06-27, when config/GC6E01/splits.txt grew past its
12-line stub and units became matchable. Earlier history is deliberately not
plotted: the metric did not exist yet, and the work of that period (the archived
campaign, the PC port) was measured a different way. Charting it as a flat zero
would be a false statement about that work.

Usage:
    uv run --with matplotlib tools/plot_progress.py -o docs/progress.png
    tools/plot_progress.py --merge <harvest-dir> --write-history
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HISTORY = ROOT / "docs" / "progress_history.json"

# Campaign start: first commit where splits.txt defines real units.
CAMPAIGN_START = 1782000000  # 2026-06-26, safely before the 12 -> 15 line growth

# Validated categorical palette against the dark surface #0b0f11: lightness
# band, chroma floor, CVD separation, normal-vision floor and contrast all pass.
SERIES = [
    ("fuzzy", "Fuzzy match %", "#3E9CC6"),
    ("code", "Matched code %", "#EC4B87"),
    ("fn", "Matched functions %", "#9C9A3E"),
]

SURFACE = "#000000"
PLOT_BG = "#0b0f11"
INK = "#e8e8e6"
MUTED = "#8a8f92"
GRID = "#2a2f33"

PATTERNS = {
    "fuzzy": r"Fuzzy match\s*\|\s*([\d.]+)%",
    "fn": r"Function match\s*\|\s*([\d.]+)%",
    "code": r"Code match\s*\|\s*([\d.]+)%",
}


def git(*args: str) -> str:
    return subprocess.run(
        ["git", *args], cwd=ROOT, capture_output=True, text=True, check=True
    ).stdout


def from_readme() -> list[dict]:
    rows = []
    for line in git("log", "--format=%H %ct", "--", "README.md").splitlines():
        if not line.strip():
            continue
        sha, ts = line.split()
        if int(ts) < CAMPAIGN_START:
            continue
        text = git("show", f"{sha}:README.md")
        point = {"commit": sha, "ts": int(ts), "source": "readme"}
        for key, pattern in PATTERNS.items():
            match = re.search(pattern, text)
            point[key] = float(match.group(1)) if match else None
        if point["fuzzy"] is None or point["code"] is None:
            continue
        rows.append(point)
    return rows


def from_harvest(directory: Path) -> list[dict]:
    """Rebuilt points, minus commits whose objects failed to compile.

    Some historical commits are broken intermediate states. Their report is a
    real file but an understatement — the failed units simply score zero — so
    plotting them would draw dips that never happened.
    """
    rows = []
    for path in sorted(directory.glob("*.json")):
        point = json.loads(path.read_text())
        if point.get("ts", 0) < CAMPAIGN_START:
            continue
        if not point.get("build_clean", True):
            continue
        rows.append(point)
    return rows


def from_report(report: Path) -> dict:
    """One measured point from a freshly built build/<version>/report.json."""
    measures = json.loads(report.read_text())["measures"]
    sha = git("rev-parse", "HEAD").strip()
    return {
        "commit": sha,
        "ts": int(git("log", "-1", "--format=%ct", sha).strip()),
        "fuzzy": round(measures["fuzzy_match_percent"], 6),
        "code": round(measures["matched_code_percent"], 6),
        "fn": round(measures["matched_functions_percent"], 6),
        "data": round(measures["matched_data_percent"], 6),
        "total_functions": measures["total_functions"],
        "matched_functions": measures["matched_functions"],
        "source": "ci",
        "build_clean": True,
    }


def is_duplicate(point: dict, rows: list[dict]) -> bool:
    """True when the metrics are unchanged from the newest point on record.

    The history is committed to the repo, so an unchanged push must not add a
    row or redraw the image — otherwise every no-op commit grows the repo by
    another copy of the PNG.
    """
    if not rows:
        return False
    last = rows[-1]
    return all(
        abs((point[k] or 0) - (last.get(k) or 0)) < 1e-9 for k in ("fuzzy", "code", "fn")
    )


def merge(*groups: list[dict]) -> list[dict]:
    """One point per commit; a direct measurement wins over a README table."""
    by_commit: dict[str, dict] = {}
    for group in groups:
        for point in group:
            existing = by_commit.get(point["commit"])
            if existing is None or point.get("source") in ("rebuilt", "ci"):
                by_commit[point["commit"]] = point
    return sorted(by_commit.values(), key=lambda r: r["ts"])


def render(rows: list[dict], out: Path) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.dates as mdates
    import matplotlib.pyplot as plt

    xs = [datetime.fromtimestamp(r["ts"], tz=timezone.utc) for r in rows]

    fig, ax = plt.subplots(figsize=(16, 8), dpi=125)
    fig.patch.set_facecolor(SURFACE)
    ax.set_facecolor(PLOT_BG)

    for key, label, color in SERIES:
        ys = [r[key] for r in rows]
        ax.plot(xs, ys, drawstyle="steps-post", lw=2, color=color, label=label)
        ax.fill_between(xs, ys, step="post", color=color, alpha=0.06, lw=0)

    ax.set_ylim(0, 100)
    ax.set_xlim(xs[0], xs[-1])
    ax.set_ylabel("Percent", color=INK, fontsize=12)
    ax.set_title(
        "Pokémon Colosseum decompilation progress (GC6E01)",
        color=INK,
        fontsize=14,
        pad=14,
    )

    ax.grid(True, color=GRID, lw=0.8, alpha=0.9)
    ax.set_axisbelow(True)
    for spine in ax.spines.values():
        spine.set_color(GRID)
    ax.tick_params(colors=MUTED, labelsize=11)
    ax.xaxis.set_major_locator(mdates.DayLocator(interval=3))
    ax.xaxis.set_major_formatter(mdates.DateFormatter("%b %d"))

    # Direct labels at the right edge: identity is never carried by color alone.
    for key, label, color in SERIES:
        ax.annotate(
            f"{rows[-1][key]:.2f}%",
            xy=(xs[-1], rows[-1][key]),
            xytext=(8, 0),
            textcoords="offset points",
            color=color,
            fontsize=11,
            fontweight="bold",
            va="center",
            annotation_clip=False,
        )

    legend = ax.legend(loc="upper left", facecolor=PLOT_BG, edgecolor=GRID, fontsize=11)
    for text in legend.get_texts():
        text.set_color(INK)

    rebuilt = sum(1 for r in rows if r.get("source") == "rebuilt")
    fig.text(
        0.5,
        0.015,
        f"{len(rows)} measured objdiff reports · {xs[0]:%Y-%m-%d} to {xs[-1]:%Y-%m-%d} (UTC) · "
        f"{rebuilt} rebuilt from history, {len(rows) - rebuilt} from the README table",
        color=MUTED,
        fontsize=9,
        ha="center",
    )

    out.parent.mkdir(parents=True, exist_ok=True)
    fig.subplots_adjust(left=0.055, right=0.94, top=0.93, bottom=0.10)
    fig.savefig(out, facecolor=SURFACE)
    print(f"wrote {out} ({len(rows)} points, {rebuilt} rebuilt)")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-o", "--out", type=Path, default=ROOT / "docs" / "progress.png")
    parser.add_argument("--merge", type=Path, help="directory of rebuilt report points")
    parser.add_argument(
        "--append", type=Path, help="add one point from a built report.json"
    )
    parser.add_argument(
        "--write-history", action="store_true", help=f"refresh {HISTORY.name}"
    )
    args = parser.parse_args()

    if args.append:
        rows = json.loads(HISTORY.read_text()) if HISTORY.exists() else []
        point = from_report(args.append)
        if is_duplicate(point, rows):
            print("progress unchanged since the last recorded point; nothing to do")
            return
        rows = merge(rows, [point])
        if args.write_history:
            HISTORY.write_text(json.dumps(rows, indent=1) + "\n")
            print(f"wrote {HISTORY} ({len(rows)} points)")
        render(rows, args.out)
        return

    if args.merge or not HISTORY.exists():
        groups = [from_readme()]
        if args.merge:
            groups.append(from_harvest(args.merge))
        rows = merge(*groups)
        if args.write_history:
            HISTORY.parent.mkdir(parents=True, exist_ok=True)
            HISTORY.write_text(json.dumps(rows, indent=1) + "\n")
            print(f"wrote {HISTORY} ({len(rows)} points)")
    else:
        rows = json.loads(HISTORY.read_text())

    if not rows:
        raise SystemExit("no progress points found")
    render(rows, args.out)


if __name__ == "__main__":
    main()
