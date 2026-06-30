#!/usr/bin/env python3
"""Run a command with a portable timeout.

Used by shell fleet scripts on macOS where GNU `timeout` is not guaranteed to
exist. Exits 124 on timeout, matching coreutils convention.
"""
import os
import signal
import subprocess
import sys


def main() -> int:
    if len(sys.argv) < 4 or sys.argv[2] != "--":
        print("usage: timeout_run.py <seconds> -- <command> [args...]", file=sys.stderr)
        return 2
    try:
        seconds = float(sys.argv[1])
    except ValueError:
        print("timeout seconds must be numeric", file=sys.stderr)
        return 2
    cmd = sys.argv[3:]
    try:
        proc = subprocess.Popen(cmd, start_new_session=True)
    except OSError as exc:
        print(f"timeout_run: exec failed: {exc}", file=sys.stderr)
        return 127
    try:
        return proc.wait(timeout=seconds)
    except subprocess.TimeoutExpired:
        print(f"timeout_run: timed out after {seconds:g}s: {' '.join(cmd)}", file=sys.stderr)
        try:
            os.killpg(proc.pid, signal.SIGTERM)
        except OSError:
            pass
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            try:
                os.killpg(proc.pid, signal.SIGKILL)
            except OSError:
                pass
            proc.wait()
        return 124


if __name__ == "__main__":
    sys.exit(main())
