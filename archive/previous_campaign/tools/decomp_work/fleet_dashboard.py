#!/usr/bin/env python3
"""fleet_dashboard.py — web dashboard for the Mac decomp fleet + bucket campaign.

Serves a single live-updating HTML page (no deps, stdlib only) bound to
0.0.0.0 so it's reachable from another machine over Tailscale, e.g. from Windows:
    http://<mac-tailscale-ip>:8770/

Shows: campaign bucket progress (LOW/STRUCT/NEARWALL/ASM attempted/total),
committed wins this run, per-lane current file + live function match%, and the
permuter status if running on the remote box.

    python3 tools/decomp_work/fleet_dashboard.py [--port 8770] [--host 0.0.0.0]
"""
import argparse
import html
import json
import os
import re
import sqlite3
import subprocess
import time
from collections import deque
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
LEDGER = os.path.join(ROOT, "build", "wall_ledger.json")
DEBT = os.path.join(ROOT, "build", "real_c_debt_audit.json")
LOCKS = os.path.join(ROOT, "build", "fleet_locks")
DATA_LOCKS = os.path.join(ROOT, "build", "data_fleet_locks")
WINS = os.path.join(ROOT, "build", "band_wins")
NEARWINS = os.path.join(ROOT, "build", "permuter_nearwins", "manifest.jsonl")
MATCHED_FNS = os.path.join(ROOT, "build", "matched_fns.txt")
REPORT = os.path.join(ROOT, "report.json")
GATE_MARK = os.path.join(ROOT, "build", ".last_gate")
GATE_LOG = os.path.join(ROOT, "build", "gate.log")
DATA_PROGRESS = os.path.join(ROOT, "config", "GC6E01", "data_progress.json")
DATA_SEED_STATE = os.path.join(ROOT, "build", "data_seed_v3_state.json")
DATA_SDATA2_WORKLIST = os.environ.get(
    "DATA_WORKLIST",
    os.path.join(ROOT, "tools", "decomp_work", "data_sdata2_worklist.json"),
)
DATA_CAMPAIGN_QUEUE = os.environ.get(
    "DATA_QUEUE",
    os.path.join(ROOT, "tools", "decomp_work", "data_campaign_queue.json"),
)
METRICS_HISTORY = os.path.join(ROOT, "tools", "decomp_work", "metrics_history.jsonl")
LANES = os.environ.get("FLEET_LANES", "opus glm codex codex2 sonnet seed").split()
LANE_STREAM_LINES = max(10, int(os.environ.get("LANE_STREAM_LINES", "80")))
DATA_LANE_ROLES = {"glm", "codex", "codex2"}
_FN = re.compile(r"fn_[0-9A-Fa-f]{8}")
_ANSI = re.compile(r"\x1b(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")
START_HEAD = None  # set at startup so "this run" win counts are stable
META_KEYS = {"_src", "_srcs", "_pct"}


def sh(cmd, timeout=8):
    try:
        return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                              timeout=timeout).stdout.strip()
    except Exception:
        return ""


def fmt_age(seconds):
    if seconds is None:
        return "never"
    seconds = max(0, int(seconds))
    if seconds < 90:
        return f"{seconds}s"
    minutes = seconds // 60
    if minutes < 90:
        return f"{minutes}m"
    hours = minutes // 60
    if hours < 48:
        return f"{hours}h"
    return f"{hours // 24}d"


def clean_log_line(line):
    line = _ANSI.sub("", line.rstrip())
    line = re.sub(r"[\x00-\x08\x0b\x0c\x0e-\x1f\x7f]", "", line)
    return line.strip()


def bucket_stats():
    out = {}
    try:
        led = json.load(open(LEDGER))
    except Exception:
        return out
    for v in led.values():
        b = v.get("bucket", "?")
        d = out.setdefault(b, {"total": 0, "attempted": 0, "done": 0})
        d["total"] += 1
        if v.get("attempted"):
            d["attempted"] += 1
        if v.get("pct", 0) >= 100 or v.get("committed"):
            d["done"] += 1
    return out


def debt_stats():
    try:
        return json.load(open(DEBT)).get("totals", {})
    except Exception:
        return {}


def permuter_status():
    try:
        return json.load(open(os.path.join(ROOT, "build", "permuter_status.json")))
    except Exception:
        return {}


def permuter_statuses():
    out = []
    for label, name in (
        ("Windows CPU", "permuter_status.json"),
        ("3090 GPU/CPU", "permuter_status_3090.json"),
    ):
        try:
            data = json.load(open(os.path.join(ROOT, "build", name)))
        except Exception:
            data = {}
        out.append((label, data))
    return out


def data_campaign_stats():
    out = {
        "matched": 0,
        "total": 0,
        "pct": 0.0,
        "entries": 0,
        "sdata2_chunks": 0,
        "sdata2_chunks_done": 0,
        "sdata2_chunks_total": 0,
        "sdata2_bytes": 0,
        "sdata2_symbols": 0,
        "sdata2_padding": 0,
        "queue_lanes": {},
        "queue_section": "",
        "queue_total": 0,
        "queue_sections": {},
        "queue_top": [],
    }
    try:
        rep = json.load(open(REPORT))
        m = rep.get("measures", {}) or {}
        out["matched"] = int(m.get("matched_data") or 0)
        out["total"] = int(m.get("total_data") or 0)
        out["pct"] = (100.0 * out["matched"] / out["total"]) if out["total"] else 0.0
    except Exception:
        pass
    try:
        prog = json.load(open(DATA_PROGRESS))
        out["entries"] = len(prog.get("matched", []) or [])
    except Exception:
        pass
    try:
        work = json.load(open(DATA_SDATA2_WORKLIST))
        meta = work.get("metadata", {}) or {}
        remaining_chunks = int(meta.get("chunk_count") or 0)
        out["sdata2_chunks"] = remaining_chunks
        chunk_bytes = int(meta.get("chunk_bytes") or 256)
        done_chunks = 0
        try:
            prog = json.load(open(DATA_PROGRESS))
            section = meta.get("section") or ".sdata2"
            for item in prog.get("matched", []) or []:
                if item.get("section") != section:
                    continue
                size = int(item.get("size") or 0)
                done_chunks += (size + chunk_bytes - 1) // chunk_bytes
        except Exception:
            pass
        out["sdata2_chunks_done"] = done_chunks
        out["sdata2_chunks_total"] = remaining_chunks + done_chunks
        out["sdata2_bytes"] = int(meta.get("section_size") or 0)
        out["sdata2_symbols"] = int(meta.get("symbol_count") or 0)
        out["sdata2_padding"] = int(meta.get("unattributed_or_padding_bytes") or 0)
    except Exception:
        pass
    try:
        queue = json.load(open(DATA_CAMPAIGN_QUEUE))
        qmeta = (queue.get("metadata", {}) or {})
        qitems = queue.get("queue", []) or []
        out["queue_lanes"] = qmeta.get("lanes", {}) or {}
        out["queue_section"] = qmeta.get("section") or ""
        out["queue_total"] = int(qmeta.get("chunk_count") or len(qitems))
        queue_sections = {}
        for item in qitems:
            section = item.get("section") or "?"
            queue_sections[section] = queue_sections.get(section, 0) + 1
        out["queue_sections"] = queue_sections
        out["queue_top"] = [
            {
                "id": item.get("id"),
                "section": item.get("section"),
                "lane": item.get("lane"),
                "start": item.get("start"),
                "end": item.get("end"),
                "size": item.get("size"),
                "score": item.get("priority_score"),
            }
            for item in (queue.get("queue", []) or [])[:8]
        ]
    except Exception:
        pass
    return out


def metrics_history(limit=14):
    rows = []
    try:
        for line in open(METRICS_HISTORY):
            line = line.strip()
            if line:
                rows.append(json.loads(line))
    except Exception:
        return []
    # Keep the latest snapshot per date; snapshot_metrics.py already writes
    # this way by default, but this makes the dashboard robust to --append runs.
    by_date = {}
    for row in rows:
        date = row.get("date")
        if date:
            by_date[date] = row
    return [by_date[k] for k in sorted(by_date)[-limit:]]


def saved_fns():
    """All fns saved >=100% to band_wins (this run's wins as they land)."""
    out = {}
    if os.path.isdir(WINS):
        for f in os.listdir(WINS):
            if f.startswith("pl_") and f.endswith(".json"):
                try:
                    d = json.load(open(os.path.join(WINS, f)))
                    out[f[:-5]] = [k for k in d if k not in META_KEYS]
                except Exception:
                    pass
    return out


def band_win_stats(wins=None):
    """Local band_wins are scratch-bank entries, not necessarily committed wins."""
    wins = wins if wins is not None else saved_fns()
    keys = []
    for fns in wins.values():
        keys.extend(fns)
    unique = set(keys)
    report_pct = {}
    try:
        rep = json.load(open(REPORT))
        for u in rep.get("units", []):
            for f in u.get("functions") or []:
                name = f.get("name")
                if name:
                    report_pct[name] = max(report_pct.get(name, -1.0),
                                           float(f.get("fuzzy_match_percent") or 0))
    except Exception:
        pass
    already_100 = sum(1 for fn in unique if report_pct.get(fn, -1.0) >= 99.95)
    known_not_100 = sum(1 for fn in unique if 0 <= report_pct.get(fn, -1.0) < 99.95)
    missing = sum(1 for fn in unique if fn not in report_pct)
    return {"entries": len(keys), "unique": len(unique),
            "duplicates": max(0, len(keys) - len(unique)),
            "already_100": already_100, "not_100": known_not_100,
            "not_in_report": missing}


def gate_status():
    now = time.time()
    try:
        ps = sh(["ps", "-axo", "command"], timeout=3).splitlines()
        running = any("auto_gate.sh" in line and "ps -axo" not in line for line in ps)
    except Exception:
        running = False
    mark_mtime = None
    try:
        mark_mtime = os.path.getmtime(GATE_MARK)
    except OSError:
        pass
    pending = []
    if os.path.isdir(WINS):
        for name in os.listdir(WINS):
            if not (name.startswith("pl_") and name.endswith(".json")):
                continue
            path = os.path.join(WINS, name)
            try:
                if mark_mtime is None or os.path.getmtime(path) > mark_mtime:
                    pending.append(name[:-5])
            except OSError:
                pass
    last_line = ""
    try:
        for line in deque(open(GATE_LOG, errors="replace"), maxlen=20):
            line = clean_log_line(line)
            if line:
                last_line = line
    except Exception:
        pass
    return {
        "running": running,
        "last_gate_age": fmt_age(None if mark_mtime is None else now - mark_mtime),
        "pending_tags": sorted(pending),
        "pending_count": len(pending),
        "last_line": last_line or "no gate log yet",
    }


def nearwin_stats(limit=8):
    rows = []
    try:
        for line in open(NEARWINS, errors="replace"):
            if line.strip():
                rows.append(json.loads(line))
    except Exception:
        pass
    by_fn = {}
    for row in rows:
        fn = row.get("fn")
        if not fn:
            continue
        cur = by_fn.get(fn)
        if cur is None or int(row.get("score", 999999)) < int(cur.get("score", 999999)):
            by_fn[fn] = row
    recent = rows[-limit:]
    best = sorted(by_fn.values(), key=lambda r: int(r.get("score", 999999)))[:limit]
    return {"entries": len(rows), "unique": len(by_fn), "recent": recent, "best": best}


def verified_win_stats(data_stats=None):
    data_stats = data_stats if data_stats is not None else data_campaign_stats()
    matched_seen = set()
    try:
        matched_seen = {line.strip() for line in open(MATCHED_FNS) if line.strip()}
    except Exception:
        pass
    report = {}
    try:
        report = json.load(open(REPORT)).get("measures", {}) or {}
    except Exception:
        pass
    return {
        "code_matches_total": int(float(report.get("matched_functions") or 0)),
        "code_functions_total": int(float(report.get("total_functions") or 0)),
        "code_matches_logged": len(matched_seen),
        "code_match_percent": float(report.get("matched_functions_percent") or 0.0),
        "code_bytes_matched": int(float(report.get("matched_code") or 0)),
        "code_bytes_total": int(float(report.get("total_code") or 0)),
        "data_bytes_matched": int(data_stats.get("matched") or 0),
        "data_bytes_total": int(data_stats.get("total") or 0),
        "data_entries": int(data_stats.get("entries") or 0),
        "data_match_percent": float(data_stats.get("pct") or 0.0),
    }


HISTORY = os.path.join(ROOT, "build", "bucket_history.jsonl")
KG_DB = os.path.join(ROOT, "tools", "decomp_work", "kg", "kg.db")
_last_snap = [0.0]


def snapshot_history(buckets, every=300):
    """Append a bucket snapshot to bucket_history.jsonl (throttled), return the series."""
    now = time.time()
    if now - _last_snap[0] >= every:
        _last_snap[0] = now
        rec = {"t": int(now)}
        for b in ("LOW", "STRUCT", "NEARWALL", "ASM"):
            d = buckets.get(b, {})
            rec[b] = d.get("attempted", 0)
            rec[b + "_done"] = d.get("done", 0)
        try:
            with open(HISTORY, "a") as f:
                f.write(json.dumps(rec) + "\n")
        except Exception:
            pass
    series = []
    try:
        series = [json.loads(l) for l in open(HISTORY) if l.strip()][-300:]
    except Exception:
        pass
    return series


def kg_levers(limit=8):
    """Top levers by #cracks + recent cracks, read straight from kg.db (fast)."""
    out = {"top": [], "recent": []}
    if not os.path.exists(KG_DB):
        return out
    try:
        con = sqlite3.connect(f"file:{KG_DB}?mode=ro", uri=True, timeout=2)
        out["top"] = con.execute(
            "SELECT l.slug, l.title, COUNT(c.lever_slug) n FROM levers l "
            "LEFT JOIN cracked_by c ON c.lever_slug=l.slug "
            "GROUP BY l.slug ORDER BY n DESC LIMIT ?", (limit,)).fetchall()
        try:
            out["recent"] = con.execute(
                "SELECT addr, lever_slug FROM cracked_by ORDER BY rowid DESC LIMIT 10").fetchall()
        except Exception:
            pass
        con.close()
    except Exception:
        pass
    return out


def lane_state(role):
    cur = ""
    if os.path.isdir(LOCKS):
        for d in os.listdir(LOCKS):
            p = os.path.join(LOCKS, d)
            try:
                if open(os.path.join(p, "owner")).read().strip() == role:
                    cur = open(os.path.join(p, "file")).read().strip()
            except Exception:
                pass
    data_cur = ""
    if role in DATA_LANE_ROLES and os.path.isdir(DATA_LOCKS):
        claims = []
        for d in os.listdir(DATA_LOCKS):
            p = os.path.join(DATA_LOCKS, d)
            try:
                if open(os.path.join(p, "owner")).read().strip() != role:
                    continue
                item = json.load(open(os.path.join(p, "item.json")))
                claims.append((os.path.getmtime(p), d, item))
            except Exception:
                pass
        if claims:
            _, d, item = sorted(claims)[-1]
            data_cur = (
                f"data:{item.get('id') or d} {item.get('section') or '?'} "
                f"{item.get('lane') or '?'} {item.get('start') or '?'}..{item.get('end') or '?'}"
            )
    if data_cur:
        cur = data_cur
    if role == "seed" and bool(sh(["pgrep", "-f", "data_seed_v3_loop\\.sh"])):
        try:
            seed_state = json.load(open(DATA_SEED_STATE))
            phase = seed_state.get("phase") or "data"
            chunk = seed_state.get("chunk") or ""
            cur = f"data-seed:{phase} {chunk}".strip()
        except Exception:
            cur = "data-seed"
    data_log = os.path.join(ROOT, "build", f"data_lane_{role}.log")
    seed_data_log = os.path.join(ROOT, "build", "data_seed_v3.log")
    if role in DATA_LANE_ROLES and os.path.exists(data_log):
        log = data_log
    elif role == "seed" and bool(sh(["pgrep", "-f", "data_seed_v3_loop\\.sh"])) and os.path.exists(seed_data_log):
        log = seed_data_log
    else:
        log = os.path.join(ROOT, "build", f"lane_{role}.log")
    last = ""
    stream = []
    if os.path.exists(log):
        try:
            lines = deque(maxlen=LANE_STREAM_LINES)
            for line in open(log, errors="replace"):
                line = clean_log_line(line)
                if line.strip():
                    lines.append(line)
            stream = list(lines)
            last = lines[-1][:120] if lines else ""
        except Exception:
            pass
    if role == "seed":
        alive = bool(sh(["pgrep", "-f", "lane_seed\\.py|data_seed_v3_loop\\.sh"]))
    elif role in DATA_LANE_ROLES:
        alive = bool(sh(["pgrep", "-f", rf"data_lane_worker\.sh {role}$|lane_worker\.sh {role}$"]))
    else:
        alive = bool(sh(["pgrep", "-f", rf"lane_worker\.sh {role}$"]))
    return {"file": cur, "last": last, "stream": stream, "alive": alive}


def commits_this_run():
    if not START_HEAD:
        return 0, []
    rng = f"{START_HEAD}..HEAD"
    n = sh(["git", "rev-list", "--count", rng]) or "0"
    lines = sh(["git", "log", rng, "--pretty=%h %s"]).splitlines()
    return int(n), lines[:8]


def render():
    buckets = bucket_stats()
    debt = debt_stats()
    data_stats = data_campaign_stats()
    history = metrics_history()
    series = snapshot_history(buckets)
    kg = kg_levers()
    wins = saved_fns()
    win_stats = band_win_stats(wins)
    gate = gate_status()
    near_stats = nearwin_stats()
    verified = verified_win_stats(data_stats)
    nwin_fns = sum(len(v) for v in wins.values())
    ncommit, clog = commits_this_run()
    branch = sh(["git", "branch", "--show-current"])

    def bar(done, total, w=260):
        pct = (100.0 * done / total) if total else 0
        return (f'<div class="bar"><div class="fill" style="width:{pct*w/100:.0f}px">'
                f'</div></div><span class="pct">{done}/{total} ({pct:.1f}%)</span>')

    rows = ""
    for b in ("LOW", "STRUCT", "NEARWALL", "ASM"):
        d = buckets.get(b)
        if not d:
            continue
        rows += (f'<tr><td class="bk">{b}</td><td>{bar(d["attempted"], d["total"])}'
                 f'</td><td class="sub">{d["total"]-d["attempted"]} unattempted · {d["done"]} done</td></tr>')

    lane_cards = ""
    for r in LANES:
        s = lane_state(r)
        dot = "#3fb950" if s["alive"] else "#6e7681"
        wf = wins.get(f"pl_{r}", [])
        winlist = "".join(f'<span class="wchip">{html.escape(w)}</span>' for w in wf) or '<span class="none">no wins yet</span>'
        stream = html.escape("\n".join(s["stream"]) or "(no lane log yet)")
        lane_cards += f"""
        <div class="lane" id="lane-{html.escape(r)}" data-lane="{html.escape(r)}">
          <div class="lane-h"><span class="dot" style="background:{dot}"></span>
            <b class="role">{html.escape(r)}</b> <span class="file">{html.escape(s['file'] or '(idle)')}</span>
            <span class="wn">{len(wf)} wins</span></div>
          <div class="wins">{winlist}</div>
          <pre class="stream">{stream}</pre>
        </div>"""

    # remote permuter statuses, written by permuter_poll*.sh
    perm_html = ""
    for label, perm in permuter_statuses():
        def pv(key):
            value = perm.get(key)
            return "?" if value is None else value
        pdot = "#3fb950" if perm.get("alive") else "#6e7681"
        active_targets = ", ".join(perm.get("active_targets") or [])
        gpu = ""
        if perm.get("gpu_mem_total"):
            gpu = (f' · gpu <b>{pv("gpu_util")}%</b> '
                   f'{pv("gpu_mem_used")}/{pv("gpu_mem_total")} MiB')
        perm_html += (f'<div class="lane"><span class="dot" style="background:{pdot}"></span>'
                      f'<b>permuter</b> ({html.escape(label)}) · cores <b>{pv("cores")}</b> · '
                      f'cpu <b>{pv("cpu_saturation_percent")}%</b> '
                      f'(<b>{pv("cpu_process_percent")}%</b> proc) · '
                      f'target workers <b>{pv("workers")}</b> · '
                      f'per-target -j <b>{pv("jobs")}</b> · '
                      f'slots <b>{pv("effective_slots")}</b> · '
                      f'budget <b>{pv("budget")}s</b> · '
                      f'active <b>{pv("active")}</b> · '
                      f'queued <b>{pv("queued")}/{pv("targets")}</b> · '
                      f'done <b>{pv("done")}</b> · score-0 wins <b>{pv("wins")}</b> · '
                      f'near≤{pv("nearwin_score")} <b>{pv("nearwins")}</b>{gpu}'
                      f'<div class="last">{html.escape(active_targets or str(perm.get("last","(no poll yet)")))[:180]}</div></div>')

    # KG levers panel
    kg_rows = "".join(
        f'<tr><td class="lv">{html.escape(t or s)}</td><td class="lvn">{n}</td></tr>'
        for (s, t, n) in kg["top"]) or '<tr><td colspan=2 class="none">kg.db not built</td></tr>'
    kg_recent = " ".join(f'<span class="wchip">{html.escape(a)}·{html.escape(lv)}</span>'
                         for (a, lv) in kg["recent"]) or '<span class="none">none yet</span>'
    queue_lanes = data_stats.get("queue_lanes") or {}
    queue_top = " ".join(
        f'<span class="wchip">{html.escape(str(item.get("id")))} {html.escape(str(item.get("section")))} {html.escape(str(item.get("lane")))} {html.escape(str(item.get("start")))}</span>'
        for item in data_stats.get("queue_top", [])
    ) or '<span class="none">queue not generated</span>'
    queue_sections = " ".join(
        f'<span class="wchip">{html.escape(str(section))} {count}</span>'
        for section, count in sorted((data_stats.get("queue_sections") or {}).items())
    ) or '<span class="none">none</span>'

    hist_rows = ""
    for row in history:
        rep = row.get("report", {}) or {}
        src = row.get("source_debt", {}) or {}
        dat = row.get("data_campaign", {}) or {}
        hist_rows += (
            f"<tr><td>{html.escape(str(row.get('date', '')))}</td>"
            f"<td>{int(rep.get('matched_functions', 0)):,}/{int(rep.get('total_functions', 0)):,} "
            f"({float(rep.get('matched_functions_percent', 0)):.1f}%)</td>"
            f"<td>{int(rep.get('matched_code', 0)):,}/{int(rep.get('total_code', 0)):,} "
            f"({float(rep.get('matched_code_percent', 0)):.1f}%)</td>"
            f"<td>{int(dat.get('verified_bytes', 0)):,}/{int(dat.get('total_bytes', 0)):,} "
            f"({float(dat.get('verified_percent', 0)):.4f}%)</td>"
            f"<td>{int(rep.get('complete_units', 0)):,}/{int(rep.get('total_units', 0)):,} "
            f"({float(rep.get('complete_units_percent', 0)):.1f}%)</td>"
            f"<td>{int(src.get('real_c_functions', 0)):,}/{int(src.get('source_functions', 0)):,} "
            f"({float(src.get('real_c_percent', 0)):.1f}%)</td>"
            f"<td>{int(src.get('asm_wrapper_functions', 0)):,}</td>"
            f"<td>{int(src.get('stub_functions', 0)):,}</td>"
            f"<td>{int(dat.get('sdata2_chunks_done', 0)):,}/{int(dat.get('sdata2_chunks_total', 0)):,}</td></tr>"
        )
    if not hist_rows:
        hist_rows = '<tr><td colspan=9 class="none">run tools/decomp_work/snapshot_metrics.py</td></tr>'

    near_html = "".join(
        f'<span class="wchip">{html.escape(str(r.get("fn")))} score {html.escape(str(r.get("score")))} {html.escape(str(r.get("machine","")))}</span>'
        for r in near_stats["best"]
    ) or '<span class="none">none banked yet</span>'

    # chart series (labels = HH:MM, one line per bucket attempted-count)
    labels = [time.strftime('%H:%M', time.localtime(p["t"])) for p in series]
    chart = {b: [p.get(b, 0) for p in series] for b in ("LOW", "STRUCT", "NEARWALL", "ASM")}
    chart_json = json.dumps({"labels": labels, "data": chart})
    daily_labels = [str(row.get("date", "")) for row in history]
    daily = {
        "function_matches": [int((row.get("report") or {}).get("matched_functions", 0)) for row in history],
        "code_bytes": [int((row.get("report") or {}).get("matched_code", 0)) for row in history],
        "data_bytes": [int((row.get("data_campaign") or {}).get("verified_bytes", 0)) for row in history],
        "sdata2_chunks": [int((row.get("data_campaign") or {}).get("sdata2_chunks_done", 0)) for row in history],
        "asm_wrappers": [int((row.get("source_debt") or {}).get("asm_wrapper_functions", 0)) for row in history],
        "stubs": [int((row.get("source_debt") or {}).get("stub_functions", 0)) for row in history],
        "pointer_offsets": [int((row.get("source_debt") or {}).get("raw_pointer_offset_lines", 0)) for row in history],
        "real_c": [int((row.get("source_debt") or {}).get("real_c_functions", 0)) for row in history],
    }
    daily_json = json.dumps({"labels": daily_labels, "data": daily})

    commit_html = "<br>".join(html.escape(c) for c in clog) or "<i>none yet</i>"
    lanes_json = json.dumps(LANES)
    return f"""<!doctype html><html><head><meta charset=utf-8>
<title>Colosseum decomp fleet</title>
<style>
 body{{background:#0d1117;color:#c9d1d9;font:14px/1.5 ui-monospace,Menlo,monospace;margin:0;padding:18px}}
 h1{{font-size:18px;margin:0 0 4px}} .meta{{color:#8b949e;margin-bottom:16px}}
 h2{{font-size:14px;color:#58a6ff;border-bottom:1px solid #21262d;padding-bottom:4px;margin:20px 0 10px}}
 table{{border-collapse:collapse;width:100%}} td{{padding:4px 10px;vertical-align:middle}}
 .bk{{font-weight:bold;width:90px}} .sub{{color:#8b949e;font-size:12px}}
 .bar{{display:inline-block;width:260px;height:14px;background:#21262d;border-radius:3px;vertical-align:middle}}
 .fill{{height:14px;background:linear-gradient(90deg,#1f6feb,#3fb950);border-radius:3px}}
 .pct{{margin-left:8px;font-size:12px}}
 .lanes{{display:grid;grid-template-columns:repeat(auto-fill,minmax(330px,1fr));gap:10px}}
 .lane{{background:#161b22;border:1px solid #21262d;border-radius:6px;padding:10px}}
 .lane-h{{font-size:13px}} .dot{{display:inline-block;width:9px;height:9px;border-radius:50%;margin-right:6px}}
 .file{{color:#d29922}} .wn{{float:right;color:#8b949e;font-size:11px}}
 .wins{{margin:6px 0;max-height:84px;overflow:auto;line-height:1.9}}
 .wchip{{display:inline-block;background:#1f6feb22;color:#58a6ff;border:1px solid #1f6feb55;border-radius:3px;padding:0 5px;margin:1px 2px;font-size:11px}}
 .none{{color:#6e7681;font-size:11px}}
 .last{{color:#8b949e;font-size:11px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}}
 .stream{{height:13.8em;margin:7px 0 0;padding:7px;background:#0d1117;border:1px solid #30363d;border-radius:4px;color:#8b949e;font:11px/1.35 ui-monospace,Menlo,monospace;white-space:pre-wrap;overflow:auto;overflow-wrap:anywhere}}
 .big{{font-size:26px;color:#3fb950}}
 .grid2{{display:grid;grid-template-columns:2fr 1fr;gap:16px;align-items:start}}
 .lv{{font-size:12px}} .lvn{{text-align:right;color:#3fb950;width:40px}}
 canvas{{background:#0d1117;max-height:240px}}
</style>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4"></script>
</head><body>
<h1>🏟️ Pokémon Colosseum — decomp fleet</h1>
<div class=meta>branch <b>{branch}</b> · <span id=live-status>live poll starting</span></div>

<div><span class=big>{verified['code_matches_total']:,}</span> verified code matches &nbsp; · &nbsp;
     <span class=big>{verified['data_bytes_matched']:,}</span> verified data bytes &nbsp; · &nbsp;
     <span class=big>{ncommit}</span> commits this run</div>
<div class=meta>verified means byte-exact score-0 code or data bytes accepted by <code>verify_data_progress.py</code>.
     Code: <b>{verified['code_matches_total']:,}/{verified['code_functions_total']:,}</b>
     ({verified['code_match_percent']:.2f}%) · data: <b>{verified['data_bytes_matched']:,}/{verified['data_bytes_total']:,}</b>
     ({verified['data_match_percent']:.4f}%) · data entries <b>{verified['data_entries']}</b> · logged gate matches <b>{verified['code_matches_logged']}</b></div>
<div class=meta>pending scratch bank · band_wins entries <b>{nwin_fns}</b> unique <b>{win_stats['unique']}</b> ·
     duplicate entries <b>{win_stats['duplicates']}</b> · already 100% in report <b>{win_stats['already_100']}</b> ·
     still not 100% in report <b>{win_stats['not_100']}</b> · not in report <b>{win_stats['not_in_report']}</b></div>
<div class=meta>gate health · <b>{'running' if gate['running'] else 'stopped'}</b> ·
     last pass <b>{gate['last_gate_age']}</b> ago · pending tags <b>{gate['pending_count']}</b>
     {' '.join(f'<span class="wchip">{html.escape(t)}</span>' for t in gate['pending_tags'][:8]) or '<span class="none">none</span>'} ·
     last verdict <b>{html.escape(gate['last_line'])}</b></div>
<div class=meta>near-win recovery bank · entries <b>{near_stats['entries']}</b> · unique fns <b>{near_stats['unique']}</b> · {near_html}</div>
<div class=meta>source debt · asm wrappers <b>{debt.get('asm_wrapper_functions','?')}</b> ·
     stubs <b>{debt.get('stub_functions','?')}</b> · .inc lines <b>{debt.get('inc_include_lines','?')}</b> ·
     raw pointer-offset lines <b>{debt.get('raw_pointer_offset_lines','?')}</b></div>
<div class=meta>data match · <b>{data_stats['matched']:,}/{data_stats['total']:,}</b> bytes
     ({data_stats['pct']:.4f}%) · verified entries <b>{data_stats['entries']}</b> ·
     active queue <b>{html.escape(str(data_stats.get('queue_section') or 'unknown'))}</b>
     <b>{int(data_stats.get('queue_total') or 0)}</b> chunks · sections {queue_sections} ·
     .sdata2 <b>{data_stats['sdata2_chunks_done']}</b>/<b>{data_stats['sdata2_chunks_total']}</b> chunks /
     remaining <b>{data_stats['sdata2_chunks']}</b> /
     <b>{data_stats['sdata2_bytes']:,}</b> bytes /
     <b>{data_stats['sdata2_symbols']:,}</b> symbols
     ({data_stats['sdata2_padding']:,} padding/unattributed bytes) ·
     lanes numeric <b>{int(queue_lanes.get('NUMERIC', 0))}</b> /
     string <b>{int(queue_lanes.get('STRING', 0))}</b> /
     layout <b>{int(queue_lanes.get('LAYOUT', 0))}</b> /
     research <b>{int(queue_lanes.get('RESEARCH', 0))}</b></div>
<div class=meta>next data chunks: {queue_top}</div>

<div class=grid2>
 <div>
  <h2>Bucket progress over time</h2>
  <canvas id=chart height=110></canvas>
  <h2>Daily code/data/debt metrics</h2>
  <canvas id=daily height=150></canvas>
 </div>
 <div>
  <h2>Top levers (knowledge graph)</h2>
  <table>{kg_rows}</table>
  <div class=last style="white-space:normal;margin-top:6px">recent cracks: {kg_recent}</div>
 </div>
</div>

<h2>Campaign bucket progress</h2>
<table>{rows}</table>

<h2>Daily metrics</h2>
<table>
 <tr class="sub"><td>Date</td><td>Functions</td><td>Code bytes</td><td>Data bytes</td><td>Complete units</td><td>Real C</td><td>ASM</td><td>Stubs</td><td>.sdata2</td></tr>
 {hist_rows}
</table>

<h2>Lanes</h2>
<div class=lanes>{lane_cards}</div>

<h2>Permuter (remote)</h2>
<div class=lanes>{perm_html}</div>

<h2>Recent committed wins</h2>
<div class=last style="white-space:normal">{commit_html}</div>
<script>
const S={chart_json};
const D={daily_json};
const C={{LOW:'#3fb950',STRUCT:'#1f6feb',NEARWALL:'#d29922',ASM:'#bc8cff'}};
const LANE_ROLES={lanes_json};
function esc(s){{
 return String(s ?? '').replace(/[&<>"']/g, ch => ({{'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}}[ch]));
}}
function nearBottom(el){{
 return (el.scrollHeight - el.scrollTop - el.clientHeight) < 14;
}}
function setStream(pre, lines){{
 const text = (lines && lines.length) ? lines.join('\\n') : '(no lane log yet)';
 if (pre.textContent === text) return;
 const stick = nearBottom(pre);
 const top = pre.scrollTop;
 pre.textContent = text;
 pre.scrollTop = stick ? pre.scrollHeight : top;
}}
function setLane(role, lane, wins){{
 const card = document.getElementById(`lane-${{role}}`);
 if (!card || !lane) return;
 const dot = card.querySelector('.dot');
 const file = card.querySelector('.file');
 const wn = card.querySelector('.wn');
 const winsBox = card.querySelector('.wins');
 const stream = card.querySelector('.stream');
 const wf = wins[`pl_${{role}}`] || [];
 if (dot) dot.style.background = lane.alive ? '#3fb950' : '#6e7681';
 if (file) file.textContent = lane.file || '(idle)';
 if (wn) wn.textContent = `${{wf.length}} wins`;
 if (winsBox) winsBox.innerHTML = wf.length
   ? wf.map(w => `<span class="wchip">${{esc(w)}}</span>`).join('')
   : '<span class="none">no wins yet</span>';
 if (stream) setStream(stream, lane.stream || []);
}}
async function pollState(){{
 try{{
   const r = await fetch('/api/state', {{cache:'no-store'}});
   if (!r.ok) throw new Error(`HTTP ${{r.status}}`);
   const data = await r.json();
   for (const role of LANE_ROLES) setLane(role, data.lanes && data.lanes[role], data.wins || {{}});
   const status = document.getElementById('live-status');
   if (status) status.textContent = `live poll ${{new Date().toLocaleTimeString()}}`;
 }}catch(e){{
   const status = document.getElementById('live-status');
   if (status) status.textContent = `poll error: ${{e.message || e}}`;
 }}
}}
window.addEventListener('load', () => {{
 for (const pre of document.querySelectorAll('.stream')) pre.scrollTop = pre.scrollHeight;
 pollState();
 setInterval(pollState, 5000);
}});
if(window.Chart && S.labels.length){{
 new Chart(document.getElementById('chart'),{{type:'line',
  data:{{labels:S.labels,datasets:Object.keys(S.data).map(k=>({{label:k,data:S.data[k],
    borderColor:C[k],backgroundColor:C[k],tension:.25,pointRadius:0,borderWidth:2}}))}},
  options:{{responsive:true,plugins:{{legend:{{labels:{{color:'#c9d1d9',boxWidth:12}}}}}},
    scales:{{x:{{ticks:{{color:'#8b949e',maxTicksLimit:8}},grid:{{color:'#21262d'}}}},
            y:{{ticks:{{color:'#8b949e'}},grid:{{color:'#21262d'}}}}}}}}}});
}}
if(window.Chart && D.labels.length){{
 new Chart(document.getElementById('daily'),{{type:'line',
  data:{{labels:D.labels,datasets:[
    {{label:'function matches',data:D.data.function_matches,borderColor:'#3fb950',yAxisID:'count',pointRadius:2}},
    {{label:'code bytes',data:D.data.code_bytes,borderColor:'#58a6ff',yAxisID:'bytes',pointRadius:2}},
    {{label:'data bytes',data:D.data.data_bytes,borderColor:'#d29922',yAxisID:'bytes',pointRadius:2}},
    {{label:'.sdata2 chunks',data:D.data.sdata2_chunks,borderColor:'#bc8cff',yAxisID:'count',pointRadius:2}},
    {{label:'real C fns',data:D.data.real_c,borderColor:'#56d364',yAxisID:'count',pointRadius:2}},
    {{label:'asm wrappers',data:D.data.asm_wrappers,borderColor:'#f85149',yAxisID:'debt',pointRadius:2}},
    {{label:'stubs',data:D.data.stubs,borderColor:'#ff7b72',yAxisID:'debt',pointRadius:2}},
    {{label:'pointer offsets',data:D.data.pointer_offsets,borderColor:'#a371f7',yAxisID:'debt',pointRadius:2}}
  ]}},
  options:{{responsive:true,interaction:{{mode:'index',intersect:false}},
    plugins:{{legend:{{labels:{{color:'#c9d1d9',boxWidth:12}}}}}},
    scales:{{
      x:{{ticks:{{color:'#8b949e'}},grid:{{color:'#21262d'}}}},
      count:{{type:'linear',position:'left',ticks:{{color:'#8b949e'}},grid:{{color:'#21262d'}}}},
      bytes:{{type:'linear',position:'right',ticks:{{color:'#58a6ff'}},grid:{{drawOnChartArea:false}}}},
      debt:{{type:'linear',display:false}}
    }}}}}});
}}
</script>
</body></html>"""


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def do_GET(self):
        if self.path.startswith("/api"):
            body = json.dumps({"buckets": bucket_stats(),
                               "debt": debt_stats(),
                               "data": data_campaign_stats(),
                               "history": metrics_history(),
                               "permuter": permuter_status(),
                               "permuters": dict(permuter_statuses()),
                               "wins": saved_fns(),
                               "win_stats": band_win_stats(),
                               "gate": gate_status(),
                               "nearwins": nearwin_stats(),
                               "verified_wins": verified_win_stats(),
                               "lanes": {r: lane_state(r) for r in LANES}}).encode()
            ctype = "application/json"
        else:
            body = render().encode()
            ctype = "text/html; charset=utf-8"
        self.send_response(200)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def main():
    global START_HEAD
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="0.0.0.0")
    ap.add_argument("--port", type=int, default=8770)
    args = ap.parse_args()
    START_HEAD = sh(["git", "rev-parse", "HEAD"]) or "HEAD"
    srv = ThreadingHTTPServer((args.host, args.port), H)
    print(f"[dashboard] http://{args.host}:{args.port}/  (Tailscale-reachable; START_HEAD={START_HEAD[:8]})")
    srv.serve_forever()


if __name__ == "__main__":
    main()
