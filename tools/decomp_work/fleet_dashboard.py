#!/usr/bin/env python3
"""fleet_dashboard.py — web dashboard for the Mac decomp fleet + bucket campaign.

Serves a single auto-refreshing HTML page (no deps, stdlib only) bound to
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
import subprocess
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
LEDGER = os.path.join(ROOT, "build", "wall_ledger.json")
LOCKS = os.path.join(ROOT, "build", "fleet_locks")
WINS = os.path.join(ROOT, "build", "band_wins")
LANES = os.environ.get("FLEET_LANES", "opus glm codex codex2 sonnet").split()
_FN = re.compile(r"fn_[0-9A-Fa-f]{8}")
START_HEAD = None  # set at startup so "this run" win counts are stable


def sh(cmd, timeout=8):
    try:
        return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                              timeout=timeout).stdout.strip()
    except Exception:
        return ""


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


def saved_fns():
    """All fns saved >=100% to band_wins (this run's wins as they land)."""
    out = {}
    if os.path.isdir(WINS):
        for f in os.listdir(WINS):
            if f.endswith(".json"):
                try:
                    d = json.load(open(os.path.join(WINS, f)))
                    out[f[:-5]] = [k for k in d if not k.startswith("_")]
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
    log = os.path.join(ROOT, "build", f"lane_{role}.log")
    last = ""
    if os.path.exists(log):
        try:
            lines = [l.rstrip() for l in open(log, errors="replace") if l.strip()]
            last = lines[-1][:120] if lines else ""
        except Exception:
            pass
    alive = bool(sh(["pgrep", "-f", rf"lane_worker\.sh {role}$"]))
    return {"file": cur, "last": last, "alive": alive}


def commits_this_run():
    if not START_HEAD:
        return 0, []
    rng = f"{START_HEAD}..HEAD"
    n = sh(["git", "rev-list", "--count", rng]) or "0"
    lines = sh(["git", "log", rng, "--pretty=%h %s"]).splitlines()
    return int(n), lines[:8]


def render():
    buckets = bucket_stats()
    wins = saved_fns()
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
        lane_cards += f"""
        <div class="lane">
          <div class="lane-h"><span class="dot" style="background:{dot}"></span>
            <b>{r}</b> <span class="file">{html.escape(s['file'] or '(idle)')}</span></div>
          <div class="saved">wins: {len(wf)} {' '.join(wf[:6])}</div>
          <div class="last">{html.escape(s['last'])}</div>
        </div>"""

    commit_html = "<br>".join(html.escape(c) for c in clog) or "<i>none yet</i>"
    return f"""<!doctype html><html><head><meta charset=utf-8>
<meta http-equiv=refresh content=5>
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
 .file{{color:#d29922}} .saved{{color:#3fb950;font-size:12px;margin:4px 0}}
 .last{{color:#8b949e;font-size:11px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}}
 .big{{font-size:26px;color:#3fb950}}
</style></head><body>
<h1>🏟️ Pokémon Colosseum — decomp fleet</h1>
<div class=meta>branch <b>{branch}</b> · {time.strftime('%H:%M:%S')} · auto-refresh 5s</div>

<div><span class=big>{ncommit}</span> commits this run &nbsp; · &nbsp;
     <span class=big>{nwin_fns}</span> functions banked (band_wins)</div>

<h2>Campaign bucket progress</h2>
<table>{rows}</table>

<h2>Lanes</h2>
<div class=lanes>{lane_cards}</div>

<h2>Recent committed wins</h2>
<div class=last style="white-space:normal">{commit_html}</div>
</body></html>"""


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def do_GET(self):
        if self.path.startswith("/api"):
            body = json.dumps({"buckets": bucket_stats(),
                               "wins": saved_fns(),
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
