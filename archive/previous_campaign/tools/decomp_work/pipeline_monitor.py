#!/usr/bin/env python3
"""Compact live monitor for the decomp pipeline (own port; leaves the 391KB
renaming dashboard untouched and running on :8792).

Serves an auto-refreshing page showing:
  - per-file pipeline stage / owner / done-finishable-total counts
  - per-lane stage + model + current file
  - TRUE context usage per session (from the proxy's context_usage.json) — the
    authoritative number the in-TUI meter gets wrong under the [1m]/GLM setup
  - staged band_wins awaiting the gate

Usage: python tools/decomp_work/pipeline_monitor.py [--port 8794]
"""
import json
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
PIPELINE = HERE / "coordination" / "pipeline.json"
CTX = ROOT / "tools" / "llm-proxy" / "context_usage.json"
WINS = ROOT / "build" / "band_wins"


def _load(p):
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except Exception:
        return {}


def render():
    st = _load(PIPELINE)
    ctx = _load(CTX)
    rows = []
    rows.append("<h2>Pipeline files</h2><table><tr><th>stage</th><th>file</th>"
                "<th>owner</th><th>done</th><th>finishable</th><th>total</th></tr>")
    for fpath, fd in st.get("files", {}).items():
        fns = fd.get("fns", {})
        done = sum(1 for i in fns.values() if i["pct"] >= 100.0)
        fin = sum(1 for i in fns.values() if 90.0 <= i["pct"] < 100.0)
        rows.append(f"<tr><td class=st>{fd['stage']}</td><td>{fpath}</td>"
                    f"<td>{fd.get('owner') or '-'}</td><td class=ok>{done}</td>"
                    f"<td class=warn>{fin}</td><td>{len(fns)}</td></tr>")
    rows.append("</table>")

    rows.append("<h2>Lanes</h2><table><tr><th>lane</th><th>stage</th><th>model</th><th>file</th></tr>")
    for lane, c in st.get("lanes", {}).items():
        rows.append(f"<tr><td>{lane}</td><td class=st>{c['stage']}</td>"
                    f"<td>{c['model']}</td><td>{c.get('file') or '(idle)'}</td></tr>")
    rows.append("</table>")

    rows.append("<h2>TRUE context usage (proxy-measured)</h2>"
                "<table><tr><th>session</th><th>model</th><th>tokens</th>"
                "<th>window</th><th>real %</th><th>at</th></tr>")
    for sk, u in sorted(ctx.items()):
        cls = "bad" if u["pct"] >= 90 else ("warn" if u["pct"] >= 70 else "ok")
        rows.append(f"<tr><td>{sk}</td><td>{u['model']}</td><td>{u['fullInput']:,}</td>"
                    f"<td>{u['window']:,}</td><td class={cls}>{u['pct']}%</td>"
                    f"<td>{u['ts'][11:19]}</td></tr>")
    rows.append("</table>")

    staged = sorted(p.stem for p in WINS.glob("*.json")) if WINS.exists() else []
    rows.append(f"<h2>Staged band_wins ({len(staged)}) awaiting gate</h2>"
                f"<p class=mono>{', '.join(staged) or '(none)'}</p>")
    return "\n".join(rows)


HTML = """<!doctype html><html><head><meta charset=utf-8>
<title>decomp pipeline</title><meta http-equiv=refresh content=5>
<style>
body{{background:#0c0f14;color:#cdd6e0;font:13px/1.4 ui-monospace,monospace;margin:18px}}
h2{{color:#7fd1ff;border-bottom:1px solid #243;padding-bottom:3px;margin-top:22px}}
table{{border-collapse:collapse;margin:6px 0}}td,th{{padding:3px 10px;border:1px solid #1c2530;text-align:left}}
th{{color:#8aa}}.st{{color:#ffd479;font-weight:bold}}.ok{{color:#6ee7a8}}.warn{{color:#ffd479}}.bad{{color:#ff7a7a}}
.mono{{color:#9ab}}
</style></head><body>
<h1>decomp pipeline <span style=color:#566>· refresh 5s · renaming dash :8792</span></h1>
{body}
</body></html>"""


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def do_GET(self):
        if self.path.startswith("/api"):
            payload = json.dumps({"pipeline": _load(PIPELINE), "context": _load(CTX)}).encode()
            self.send_response(200)
            self.send_header("content-type", "application/json")
            self.end_headers()
            self.wfile.write(payload)
            return
        body = HTML.format(body=render()).encode("utf-8")
        self.send_response(200)
        self.send_header("content-type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(body)


def main():
    port = 8794
    if "--port" in sys.argv:
        port = int(sys.argv[sys.argv.index("--port") + 1])
    print(f"pipeline monitor on http://127.0.0.1:{port}  (api at /api)")
    HTTPServer(("127.0.0.1", port), H).serve_forever()


if __name__ == "__main__":
    main()
