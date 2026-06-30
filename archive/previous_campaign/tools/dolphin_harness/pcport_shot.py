#!/usr/bin/env python3
"""Launch the PC port HEADED (visible window) and screenshot its window.

Host-side only (no pcport/ changes). Runs from a stable COPY of the exe so it
never collides with an in-flight rebuild/link of build_pc\\pcport_bootstrap.exe.

Usage:
  python pcport_shot.py --map S1_out --out shots/pcport_s1out_headed.png
  python pcport_shot.py --map D1_garage_1F --out shots/pcport_garage_headed.png --settle 12
  python pcport_shot.py --keep         # leave the window open after capturing

--map sets PCPORT_FIELD_ARCHIVE. No frame cap is set, so the window stays open
(interactive free-fly) for visual confirmation; pass --frames N to cap+exit.
"""
import argparse
import ctypes
import json
import os
import shutil
import subprocess
import time

import win32gui
import win32process
import win32ui
from PIL import Image

try:
    ctypes.windll.shcore.SetProcessDpiAwareness(2)
except OSError:
    pass

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
SRC_EXE = os.path.join(REPO, "build_pc", "pcport_bootstrap.exe")
COPY_EXE = os.path.join(REPO, "build_pc", "_headed_pcport.exe")


def stable_copy():
    """Copy the live exe to a stable path so a concurrent relink can't lock or
    corrupt the file we launch. Verifies it's a non-trivial PE first."""
    if not os.path.exists(SRC_EXE):
        raise SystemExit(
            f"missing {SRC_EXE} - run `python tools\\pcport_link.py` first"
        )
    sz = os.path.getsize(SRC_EXE)
    if sz < 100_000:
        raise SystemExit(f"{SRC_EXE} looks truncated ({sz} bytes) - a build may be in progress; retry")
    shutil.copy2(SRC_EXE, COPY_EXE)
    return COPY_EXE


def find_window(pid):
    cands = []

    def cb(hwnd, _):
        _, wpid = win32process.GetWindowThreadProcessId(hwnd)
        if wpid == pid and win32gui.IsWindowVisible(hwnd):
            l, t, r, b = win32gui.GetWindowRect(hwnd)
            if (r - l) >= 200 and (b - t) >= 150:
                cands.append((-(r - l) * (b - t), hwnd))
        return True

    win32gui.EnumWindows(cb, None)
    cands.sort()
    return cands[0][1] if cands else None


def grab(hwnd, out_path):
    wl, wt, wr, wb = win32gui.GetWindowRect(hwnd)
    ww, wh = wr - wl, wb - wt
    cl, ct = win32gui.ClientToScreen(hwnd, (0, 0))
    _, _, cw, ch = win32gui.GetClientRect(hwnd)
    hwnd_dc = win32gui.GetWindowDC(hwnd)
    mfc = win32ui.CreateDCFromHandle(hwnd_dc)
    sdc = mfc.CreateCompatibleDC()
    bmp = win32ui.CreateBitmap()
    bmp.CreateCompatibleBitmap(mfc, ww, wh)
    sdc.SelectObject(bmp)
    ctypes.windll.user32.PrintWindow(hwnd, sdc.GetSafeHdc(), 2)
    info = bmp.GetInfo()
    data = bmp.GetBitmapBits(True)
    img = Image.frombuffer("RGB", (info["bmWidth"], info["bmHeight"]), data, "raw", "BGRX", 0, 1)
    win32gui.DeleteObject(bmp.GetHandle())
    sdc.DeleteDC()
    mfc.DeleteDC()
    win32gui.ReleaseDC(hwnd, hwnd_dc)
    if cw > 2 and ch > 2:
        ox, oy = cl - wl, ct - wt
        img = img.crop((ox, oy, ox + cw, oy + ch))
    os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
    img.save(out_path)
    return img.width, img.height


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--map", default=None, help="PCPORT_FIELD_ARCHIVE map name (e.g. S1_out, D1_garage_1F)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--settle", type=float, default=12, help="seconds to wait for render before capture")
    ap.add_argument("--frames", type=int, default=None, help="if set, cap to N frames + exit (PCPORT_MENU_FRAMES)")
    ap.add_argument("--keep", action="store_true", help="leave the window open after capture")
    ap.add_argument("--timeout", type=float, default=40, help="kill the process after this many seconds if --frames")
    args = ap.parse_args()

    exe = stable_copy()
    env = dict(os.environ)
    if args.map:
        env["PCPORT_FIELD_ARCHIVE"] = args.map
    if args.frames is not None:
        env["PCPORT_MENU_FRAMES"] = str(args.frames)
    env.pop("PCPORT_DUMP", None)

    proc = subprocess.Popen([exe, "--field"], env=env, cwd=REPO)
    time.sleep(args.settle)

    if proc.poll() is not None:
        raise SystemExit(json.dumps({"ok": False, "error": f"exe exited early rc={proc.returncode} (map may hang/fail headless)"}))

    hwnd = find_window(proc.pid)
    if not hwnd:
        if not args.keep:
            proc.kill()
        raise SystemExit(json.dumps({"ok": False, "error": f"no window for pid {proc.pid}"}))

    w, h = grab(hwnd, args.out)
    res = {"ok": True, "path": os.path.abspath(args.out), "width": w, "height": h,
           "pid": proc.pid, "map": args.map or "D1_garage_1F (default)",
           "title": win32gui.GetWindowText(hwnd), "left_open": args.keep}
    if not args.keep:
        proc.kill()
    print(json.dumps(res, indent=2))


if __name__ == "__main__":
    main()
