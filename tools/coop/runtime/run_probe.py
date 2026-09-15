#!/usr/bin/env python3
"""Launch Dolphin on the patched co-op build and collect the telemetry block.

Uses an isolated Dolphin profile so the user's normal configuration and memory
cards are never touched. The number of emulated controllers is configurable so
the same probe can be run with one pad and with two, which is what shows
whether the game reacts to a second controller at all.

    python tools/coop/runtime/run_probe.py --pads 1 --label one-pad
    python tools/coop/runtime/run_probe.py --pads 2 --label two-pads
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DOLPHIN = ROOT / "tools" / "dolphin_harness" / "bin" / "Dolphin.exe"
FALLBACK_DOLPHIN = ROOT / "dolphin-2603a-x64" / "Dolphin-x64" / "Dolphin.exe"
SCRIPT = Path(__file__).resolve().parent / "coop_probe_script.py"

DOLPHIN_INI = """[Core]
SIDevice0 = {si0}
SIDevice1 = {si1}
SIDevice2 = 0
SIDevice3 = 0
EnableCheats = False
AutoDiscChange = False
[Interface]
ConfirmStop = False
UsePanicHandlers = False
OnScreenDisplayMessages = True
[Display]
RenderToMain = False
RenderWindowAutoSize = False
RenderWindowWidth = 640
RenderWindowHeight = 528
Fullscreen = False
[AutoUpdate]
UpdateTrack =
[Analytics]
Enabled = False
PermissionAsked = True
"""


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--boot",
        type=Path,
        default=ROOT / "build/coop/disc",
        help="extracted disc directory (or ISO) to boot",
    )
    ap.add_argument(
        "--manifest", type=Path, default=ROOT / "build/coop/main.manifest.json"
    )
    ap.add_argument("--pads", type=int, choices=(1, 2), default=1)
    ap.add_argument("--frames", type=int, default=3600)
    ap.add_argument("--label", default=None)
    ap.add_argument(
        "--timeout", type=int, default=420, help="seconds before giving up"
    )
    ap.add_argument("--state", type=Path, default=None, help="savestate to load")
    ap.add_argument("--dolphin", type=Path, default=None)
    args = ap.parse_args()

    label = args.label or f"{args.pads}-pad"
    dolphin = args.dolphin or (DOLPHIN if DOLPHIN.is_file() else FALLBACK_DOLPHIN)
    if not dolphin.is_file():
        sys.exit(f"Dolphin not found: {dolphin}")
    if not args.boot.exists():
        sys.exit(f"boot target not found: {args.boot}")

    manifest = json.loads(args.manifest.read_text())
    stats_addr = manifest["payload"]["symbols"]["coop_stats"]
    payload_addr = manifest["payload"]["symbols"]["coop_entry"]

    run_dir = ROOT / "build/coop/run" / label
    if run_dir.exists():
        shutil.rmtree(run_dir)
    profile = run_dir / "profile"
    (profile / "Config").mkdir(parents=True)
    (profile / "Config" / "Dolphin.ini").write_text(
        DOLPHIN_INI.format(si0=6, si1=6 if args.pads >= 2 else 0)
    )

    out_path = run_dir / "result.json"
    env = dict(os.environ)
    env["COOP_STATS_ADDR"] = stats_addr
    env["COOP_PAYLOAD_ADDR"] = payload_addr
    env["COOP_OUT"] = str(out_path)
    env["COOP_FRAMES"] = str(args.frames)
    env["COOP_LABEL"] = label

    cmd = [
        str(dolphin),
        "-u", str(profile),
        "--script", str(SCRIPT),
        "-e", str(args.boot),
    ]
    if args.state:
        cmd += ["-s", str(args.state)]

    print(f"label       {label}")
    print(f"dolphin     {dolphin}")
    print(f"boot        {args.boot}")
    print(f"pads        {args.pads} (SIDevice1={'6' if args.pads >= 2 else '0'})")
    print(f"coop_stats  {stats_addr}")
    print(f"frames      {args.frames}")
    print("launching...")

    proc = subprocess.Popen(cmd, env=env)
    deadline = time.time() + args.timeout
    try:
        while time.time() < deadline:
            if out_path.is_file():
                time.sleep(1.0)  # let the write settle
                break
            if proc.poll() is not None:
                print(f"Dolphin exited early (code {proc.returncode})")
                break
            time.sleep(2.0)
        else:
            print(f"timed out after {args.timeout}s")
    finally:
        if proc.poll() is None:
            proc.terminate()
            try:
                proc.wait(timeout=15)
            except subprocess.TimeoutExpired:
                proc.kill()

    log_path = Path(str(out_path) + ".log")
    if log_path.is_file():
        print("\n--- probe log ---")
        print(log_path.read_text().strip())

    if not out_path.is_file():
        print("\nNo result written.")
        return 1

    print(f"\nresult: {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
