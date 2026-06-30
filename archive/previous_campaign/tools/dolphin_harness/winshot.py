#!/usr/bin/env python3
"""Window capture for the Dolphin play harness (host side).

Captures the game render window's CLIENT area via PrintWindow
(PW_RENDERFULLCONTENT grabs D3D swapchain content; works while occluded,
validated method from docs/dolphin_ar_validation.md). This is the harness's
default screenshot path because the in-process framedrawn event can crash
Dolphin during scene transitions.

The render window is identified among the pid's top-level windows as the one
with the fewest child windows (the Qt main window hosts dozens of child
widgets; the detached render window has ~none). Requires RenderToMain=False.

Usage: python winshot.py out.png
"""
import ctypes
import json
import os
import sys

import win32gui
import win32process
import win32ui
from PIL import Image

# Without DPI awareness, GetWindowRect/GetClientRect return virtualized
# (display-scale-divided) coordinates while PrintWindow captures physical
# pixels, producing cropped/zoomed captures on >100% display scaling.
try:
    ctypes.windll.shcore.SetProcessDpiAwareness(2)  # PER_MONITOR_DPI_AWARE
except OSError:
    pass

HERE = os.path.dirname(os.path.abspath(__file__))


def find_render_window(pid):
    """With RenderToMain=False the detached render window is the one whose
    title carries the backend/game segments ('... | JIT64 DC | ... (GC6E01)');
    the main window title is just 'Dolphin <version>'."""
    cands = []

    def cb(hwnd, _):
        _, wpid = win32process.GetWindowThreadProcessId(hwnd)
        if wpid == pid and win32gui.IsWindowVisible(hwnd):
            title = win32gui.GetWindowText(hwnd)
            if " | " in title:
                left, top, right, bottom = win32gui.GetWindowRect(hwnd)
                cands.append((-(right - left) * (bottom - top), hwnd))
        return True

    win32gui.EnumWindows(cb, None)
    if not cands:
        return None
    cands.sort()
    return cands[0][1]


def capture_client(hwnd, out_path):
    # PrintWindow renders the full window; crop to the client area.
    wl, wt, wr, wb = win32gui.GetWindowRect(hwnd)
    ww, wh = wr - wl, wb - wt
    cl, ct = win32gui.ClientToScreen(hwnd, (0, 0))
    _, _, cw, ch = win32gui.GetClientRect(hwnd)
    if cw < 2 or ch < 2:
        raise RuntimeError("client area is empty (window minimized?)")

    hwnd_dc = win32gui.GetWindowDC(hwnd)
    mfc_dc = win32ui.CreateDCFromHandle(hwnd_dc)
    save_dc = mfc_dc.CreateCompatibleDC()
    bmp = win32ui.CreateBitmap()
    bmp.CreateCompatibleBitmap(mfc_dc, ww, wh)
    save_dc.SelectObject(bmp)
    ctypes.windll.user32.PrintWindow(hwnd, save_dc.GetSafeHdc(), 2)  # PW_RENDERFULLCONTENT
    info = bmp.GetInfo()
    data = bmp.GetBitmapBits(True)
    img = Image.frombuffer("RGB", (info["bmWidth"], info["bmHeight"]), data, "raw", "BGRX", 0, 1)
    win32gui.DeleteObject(bmp.GetHandle())
    save_dc.DeleteDC()
    mfc_dc.DeleteDC()
    win32gui.ReleaseDC(hwnd, hwnd_dc)

    ox, oy = cl - wl, ct - wt
    img = img.crop((ox, oy, ox + cw, oy + ch))
    if out_path:
        os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
        img.save(out_path)
    return img


def shoot(out_path, pid=None):
    if pid is None:
        with open(os.path.join(HERE, "run", "session.json")) as f:
            pid = json.load(f)["pid"]
    hwnd = find_render_window(pid)
    if not hwnd:
        raise RuntimeError(f"no render window found for pid {pid}")
    img = capture_client(hwnd, out_path)
    return {"ok": True, "path": os.path.abspath(out_path), "width": img.width,
            "height": img.height, "hwnd": hwnd, "title": win32gui.GetWindowText(hwnd)}


def main():
    print(json.dumps(shoot(sys.argv[1])))


if __name__ == "__main__":
    main()
