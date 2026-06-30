#!/usr/bin/env python3
"""update_report.py <fn ...> — mark the given functions 100% in report.json.

Called by auto_gate right after a win commits, so report.json (and therefore the
wall_ledger / bucket queues built from it) immediately reflects the function as
DONE instead of re-offering it for hours until the dashboard's periodic regen.
Atomic write so it never collides with a concurrent dashboard regen.
"""
import os
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
REPORT = os.path.join(ROOT, "report.json")


def main():
    fns = set(sys.argv[1:])
    if not fns or not os.path.exists(REPORT):
        return

    fd, tmp = tempfile.mkstemp(dir=ROOT, suffix=".fns")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as fh:
            for fn in sorted(fns):
                fh.write(fn + "\n")
        subprocess.run(
            [sys.executable, "tools/decomp_work/surgical_report_update.py", tmp, REPORT],
            cwd=ROOT,
            check=True,
        )
    except Exception:
        raise
    finally:
        try:
            os.unlink(tmp)
        except OSError:
            pass


if __name__ == "__main__":
    main()
