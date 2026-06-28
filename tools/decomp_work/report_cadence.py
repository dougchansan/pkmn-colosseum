#!/usr/bin/env python3
"""report_cadence.py — keep the published decomp percentages current.

Every cycle: regenerate report.json via gen_decomp_report.py (which self-
validates against progress.py), and if the matched-function count CHANGED,
sync the README metrics line, commit, and push — so decomp.dev and GitHub
never lag more than one cycle behind reality.

Safety rails:
  * gen_decomp_report must print RESULT: PASS or the cycle is skipped.
  * A drop of more than DROP_GUARD matched functions is NOT auto-pushed
    (that signature means measurement contamination, not honest reverts) —
    it logs loudly and waits for a human.
  * Commits touch ONLY report.json + README.md.

Run (Windows python, from repo root):
  python tools/decomp_work/report_cadence.py            # loop forever, hourly
  python tools/decomp_work/report_cadence.py --once     # single cycle
"""
import json
import os
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
INTERVAL_S = 3600
DROP_GUARD = 20


def log(msg: str) -> None:
    print(f"[{datetime.now():%H:%M:%S}] {msg}", flush=True)


def run(cmd, **kw):
    return subprocess.run(cmd, cwd=REPO, capture_output=True, text=True, **kw)


def committed_matched() -> int:
    r = run(["git", "show", "HEAD:report.json"])
    try:
        return int(json.loads(r.stdout)["measures"]["matched_functions"])
    except Exception:
        return -1


def cycle() -> None:
    old = committed_matched()
    # Refresh base/ from CURRENT source before measuring. gen_decomp_report only
    # DIFFS the existing build/GC6E01/base/*.o objects against the target — it does
    # not recompile — so without this step the cadence can never reflect newly
    # landed matches (the published number would freeze at whenever base/ was last
    # built). compile_check --all rewrites each TU's base object from current source;
    # wall TUs that fail to compile keep their previous object (compile_all catches
    # the failure and continues). ~5 min; the objdiff measure-cache keeps gen fast
    # for TUs whose recompiled object is byte-identical.
    comp = run([sys.executable, "tools/compile_check.py", "--all"], timeout=1800)
    if "Results:" not in (comp.stdout + comp.stderr):
        log("SKIP: compile_check --all did not complete — base/ not refreshed")
        return
    gen = run([sys.executable, "tools/gen_decomp_report.py", "-o", "report.json"],
              timeout=1800)
    if "RESULT: PASS" not in (gen.stdout + gen.stderr):
        log("SKIP: gen_decomp_report did not self-validate (no RESULT: PASS)")
        run(["git", "checkout", "--", "report.json"])
        return
    m = json.loads((REPO / "report.json").read_text(encoding="utf-8"))["measures"]
    new = int(m["matched_functions"])
    total = int(m["total_functions"])
    codep = float(m["matched_code_percent"])
    if new == old:
        log(f"no change ({new}/{total}) — nothing to publish")
        run(["git", "checkout", "--", "report.json"])
        return
    if old - new > DROP_GUARD:
        log(f"!!! GUARD: matched dropped {old} -> {new} (>{DROP_GUARD}) — "
            f"NOT auto-pushing; investigate measurement contamination")
        run(["git", "checkout", "--", "report.json"])
        return
    fnp = 100.0 * new / total
    sync = run([sys.executable, "tools/decomp_work/update_readme_progress.py",
                datetime.now().date().isoformat()])
    if sync.returncode != 0:
        log("SKIP: README progress sync failed")
        run(["git", "checkout", "--", "report.json", "README.md"])
        return
    msg = (f"progress: {new}/{total} functions ({fnp:.2f}%), {codep:.2f}% code "
           f"[auto-cadence]\n\nCo-Authored-By: Claude Fable 5 <noreply@anthropic.com>")
    for attempt in range(3):
        a = run(["git", "add", "report.json", "README.md"])
        c = run(["git", "commit", "-m", msg])
        if c.returncode == 0:
            break
        log(f"commit retry {attempt+1}: {c.stderr.strip()[:120]}")
        time.sleep(5)
    # The pre-push hook blocks direct master pushes unless this is set; the cadence is
    # the intended automated publisher (report.json + README only), so opt in explicitly.
    p = run(["git", "push", "origin", "master"], env={**os.environ, "ALLOW_MASTER_PUSH": "1"})
    log(f"PUBLISHED {old} -> {new}/{total} ({fnp:.2f}%), code {codep:.2f}% "
        f"(push rc={p.returncode})")


def main() -> None:
    once = "--once" in sys.argv
    log(f"report cadence up (interval {INTERVAL_S}s, drop-guard {DROP_GUARD})")
    while True:
        try:
            cycle()
        except Exception as e:
            log(f"cycle error: {e}")
        if once:
            break
        time.sleep(INTERVAL_S)


if __name__ == "__main__":
    main()
