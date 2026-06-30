#!/usr/bin/env python3
"""CLI client for the Dolphin play harness (Windows host side).

Each invocation is one prompt-returning command against the in-Dolphin server
(tools/dolphin_harness/inproc/harness_server.py over localhost NDJSON).

Examples:
  python ctl.py launch
  python ctl.py launch --load-state title_screen
  python ctl.py status
  python ctl.py wait 600
  python ctl.py screenshot shots/title.png
  python ctl.py press Start
  python ctl.py press A --frames 4
  python ctl.py stick 1.0 0.0 --frames 30          # walk right half a second
  python ctl.py read gsFloorCurrentId
  python ctl.py write gsFloorCurrentId 0x80
  python ctl.py warp 0x80
  python ctl.py state
  python ctl.py save-state phenac_city
  python ctl.py load-state phenac_city
  python ctl.py kill
"""
import argparse
import base64
import json
import os
import shutil
import socket
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
RUN_DIR = os.path.join(HERE, "run")
SESSION_JSON = os.path.join(RUN_DIR, "session.json")
STATES_DIR = os.path.join(HERE, "states")
DEFAULT_ISO = os.path.join(REPO, "orig", "GC6E01", "game.iso")
DEFAULT_PORT = 53741

sys.path.insert(0, HERE)
import symbols  # noqa: E402

BUTTON_ALIASES = {
    "a": "A", "b": "B", "x": "X", "y": "Y", "z": "Z", "l": "L", "r": "R",
    "start": "Start", "up": "Up", "down": "Down", "left": "Left", "right": "Right",
    "dup": "Up", "ddown": "Down", "dleft": "Left", "dright": "Right",
}


def norm_buttons(items):
    out = []
    for item in items:
        for b in item.replace(",", " ").split():
            key = b.lower()
            if key not in BUTTON_ALIASES:
                sys.exit(f"unknown button: {b} (valid: {sorted(set(BUTTON_ALIASES.values()))})")
            out.append(BUTTON_ALIASES[key])
    return out


def stick_byte(v, axis="x"):
    """Map a CLI stick value to a GC pad byte (0-255, 128 = neutral).

    Convention (natural / joystick): floats in [-1.0, 1.0] where
      +x = right, -x = left, +y = UP, -y = DOWN.
    Values written with magnitude > 1 (e.g. "200") are taken as raw bytes.

    The Y axis is inverted relative to the raw pad byte: this game's pad
    reports StickY byte 255 when pushed DOWN and 1 when pushed UP, so +y
    (up) maps to a low byte. Verified against the New Game YES/NO prompt.
    """
    f = float(v)
    if -1.0 <= f <= 1.0 and ("." in str(v) or abs(f) <= 1):
        if axis == "y":
            return int(round(128 - f * 127))  # +1.0 (up) -> 1, -1.0 (down) -> 255
        return int(round(128 + f * 127))      # +1.0 (right) -> 255
    return max(0, min(255, int(f)))


def session():
    if not os.path.exists(SESSION_JSON):
        return None
    with open(SESSION_JSON) as f:
        return json.load(f)


def pid_alive(pid):
    res = subprocess.run(
        ["tasklist", "/FI", f"PID eq {pid}", "/NH"], capture_output=True, text=True
    )
    return str(pid) in res.stdout


def send(msg, timeout=60.0, port=None):
    if port is None:
        sess = session()
        if not sess:
            sys.exit("no session (run: ctl.py launch)")
        port = sess["port"]
    s = socket.create_connection(("127.0.0.1", port), timeout=10)
    s.settimeout(timeout)
    f = s.makefile("rwb")
    f.write((json.dumps(msg) + "\n").encode())
    f.flush()
    line = f.readline()
    s.close()
    if not line:
        sys.exit("server closed connection without responding")
    return json.loads(line)


def out(resp):
    print(json.dumps(resp, indent=2))
    if not resp.get("ok"):
        sys.exit(1)


def resolve_state_path(name, must_exist):
    if os.path.sep in name or name.endswith(".sav"):
        path = os.path.abspath(name)
    else:
        path = os.path.join(STATES_DIR, name + ".sav")
    if must_exist and not os.path.exists(path):
        sys.exit(f"savestate not found: {path}")
    return path


# ---------------------------------------------------------------- commands

def cmd_launch(args):
    os.makedirs(RUN_DIR, exist_ok=True)
    os.makedirs(STATES_DIR, exist_ok=True)
    sess = session()
    if sess and pid_alive(sess["pid"]):
        sys.exit(f"already running (pid {sess['pid']}); use ctl.py kill first")

    dolphin = os.path.join(HERE, "bin", "Dolphin.exe")
    if not os.path.exists(dolphin):
        sys.exit("bin\\Dolphin.exe missing - run setup.ps1 first")

    profile = os.path.join(RUN_DIR, "profile")
    os.makedirs(os.path.join(profile, "Config"), exist_ok=True)
    template = os.path.join(HERE, "profile_template")
    for ini in os.listdir(template):
        if ini.endswith(".ini"):
            shutil.copy(os.path.join(template, ini), os.path.join(profile, "Config", ini))

    cmdline = [
        dolphin,
        "-u", profile,
        "--script", os.path.join(HERE, "inproc", "harness_server.py"),
        "-e", os.path.abspath(args.iso),
    ]
    if args.load_state:
        cmdline += ["-s", resolve_state_path(args.load_state, must_exist=True)]

    env = dict(os.environ)
    env["DOLPHIN_HARNESS_ROOT"] = HERE
    env["DOLPHIN_HARNESS_PORT"] = str(args.port)

    server_log = os.path.join(RUN_DIR, "server_log.txt")
    if os.path.exists(server_log):
        os.remove(server_log)

    DETACHED = 0x00000008 | 0x00000200  # DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP
    proc = subprocess.Popen(cmdline, env=env, creationflags=DETACHED, close_fds=True)

    with open(SESSION_JSON, "w") as f:
        json.dump({"pid": proc.pid, "port": args.port, "iso": os.path.abspath(args.iso),
                   "started": time.strftime("%Y-%m-%d %H:%M:%S")}, f, indent=2)

    deadline = time.time() + args.boot_timeout
    while time.time() < deadline:
        if not pid_alive(proc.pid):
            sys.exit("Dolphin exited during startup - check run\\profile\\Logs\\dolphin.log")
        try:
            resp = send({"cmd": "status"}, timeout=5, port=args.port)
            if resp.get("ok"):
                resp["pid"] = proc.pid
                out(resp)
                return
        except (ConnectionRefusedError, OSError, socket.timeout):
            pass
        time.sleep(1.0)
    sys.exit("timed out waiting for harness server (check run\\server_log.txt)")


def cmd_kill(_args):
    sess = session()
    if not sess:
        print("no session")
        return
    subprocess.run(["taskkill", "/PID", str(sess["pid"]), "/F"], capture_output=True)
    os.remove(SESSION_JSON)
    print(f"killed pid {sess['pid']}")


def cmd_status(_args):
    sess = session()
    if not sess:
        print(json.dumps({"ok": False, "running": False, "error": "no session"}))
        sys.exit(1)
    if not pid_alive(sess["pid"]):
        print(json.dumps({"ok": False, "running": False, "error": "pid dead (stale session)"}))
        sys.exit(1)
    try:
        resp = send({"cmd": "status"}, timeout=5)
        resp["running"] = True
        resp["pid"] = sess["pid"]
        out(resp)
    except (ConnectionRefusedError, OSError) as e:
        print(json.dumps({"ok": False, "running": True, "pid": sess["pid"],
                          "error": f"pid alive but socket dead: {e}"}))
        sys.exit(1)


def cmd_press(args):
    frames = args.frames
    out(send({"cmd": "press", "buttons": norm_buttons(args.buttons), "frames": frames},
             timeout=frames / 60 + 30))


def cmd_stick(args):
    frames = args.frames
    msg = {"cmd": "stick", "x": stick_byte(args.x, "x"), "y": stick_byte(args.y, "y"),
           "frames": frames, "which": "c" if args.c else "main"}
    if args.buttons:
        msg["buttons"] = norm_buttons(args.buttons)
    out(send(msg, timeout=frames / 60 + 30))


def cmd_hold(args):
    msg = {"cmd": "hold", "buttons": norm_buttons(args.buttons)}
    out(send(msg))


def cmd_release(args):
    out(send({"cmd": "release", "buttons": norm_buttons(args.buttons) if args.buttons else []}))


def cmd_wait(args):
    out(send({"cmd": "wait", "frames": args.frames}, timeout=args.frames / 60 + 30))


def cmd_screenshot(args):
    if args.method == "window":
        # Default: PrintWindow capture of the render window. Crash-free.
        import winshot
        try:
            out(winshot.shoot(os.path.abspath(args.path)))
        except Exception as e:  # noqa: BLE001
            out({"ok": False, "error": str(e)})
        return
    # framedrawn: exact emulated pixels, but can crash Dolphin during scene
    # transitions - use only in known-stable scenes (parity captures).
    from PIL import Image
    resp = send({"cmd": "screenshot"}, timeout=60)
    if not resp.get("ok"):
        out(resp)
    data = base64.b64decode(resp.pop("rgb_b64"))
    img = Image.frombytes("RGB", (resp["width"], resp["height"]), data)
    if args.flip:
        img = img.transpose(Image.FLIP_TOP_BOTTOM)
    path = os.path.abspath(args.path)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path)
    resp["path"] = path
    out(resp)


def cmd_read(args):
    addr, sym_size = symbols.resolve(args.target)
    size = args.size or sym_size or 4
    resp = send({"cmd": "read", "addr": addr, "size": size})
    resp["target"] = args.target
    resp["addr_hex"] = f"0x{addr:08X}"
    out(resp)


def cmd_write(args):
    addr, sym_size = symbols.resolve(args.target)
    size = args.size or sym_size or 4
    value = int(args.value, 0)
    resp = send({"cmd": "write", "addr": addr, "size": size, "value": value})
    resp["target"] = args.target
    resp["addr_hex"] = f"0x{addr:08X}"
    out(resp)


def cmd_state(_args):
    snap = {"ok": True}
    for name in ("gsFloorCurrentId", "gsFloorState", "gsFloorNextState"):
        addr, size = symbols.resolve(name)
        resp = send({"cmd": "read", "addr": addr, "size": size or 4})
        if resp.get("ok"):
            key = "u32" if (size or 4) == 4 else "u8"
            snap[name] = resp.get(key, resp.get("hex"))
            if name == "gsFloorCurrentId":
                snap["floor_id_hex"] = f"0x{snap[name]:04X}"
    st = send({"cmd": "status"}, timeout=5)
    snap["frame"] = st.get("frame")
    out(snap)


def cmd_warp(args):
    """Floor warp via the validated data path (docs/dolphin_ar_validation.md)."""
    floor = int(args.floor_id, 0)
    a_floor, _ = symbols.resolve("gsFloorCurrentId")
    a_state, _ = symbols.resolve("gsFloorState")
    r1 = send({"cmd": "write", "addr": a_floor, "size": 4, "value": floor})
    r2 = send({"cmd": "write", "addr": a_state, "size": 4, "value": 3})
    out({"ok": r1.get("ok") and r2.get("ok"), "floor_id": f"0x{floor:04X}",
         "note": "floor unload requested; wait ~120 frames then screenshot"})


def cmd_save_state(args):
    path = resolve_state_path(args.name, must_exist=False)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    out(send({"cmd": "save_state", "path": path}, timeout=120))


def cmd_load_state(args):
    path = resolve_state_path(args.name, must_exist=True)
    out(send({"cmd": "load_state", "path": path}, timeout=120))


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="verb", required=True)

    sp = sub.add_parser("launch")
    sp.add_argument("--iso", default=DEFAULT_ISO)
    sp.add_argument("--port", type=int, default=DEFAULT_PORT)
    sp.add_argument("--load-state", default=None)
    sp.add_argument("--boot-timeout", type=float, default=90)
    sp.set_defaults(fn=cmd_launch)

    sub.add_parser("kill").set_defaults(fn=cmd_kill)
    sub.add_parser("status").set_defaults(fn=cmd_status)

    sp = sub.add_parser("press")
    sp.add_argument("buttons", nargs="+")
    sp.add_argument("--frames", type=int, default=4)
    sp.set_defaults(fn=cmd_press)

    sp = sub.add_parser("stick")
    sp.add_argument("x")
    sp.add_argument("y")
    sp.add_argument("--frames", type=int, default=12)
    sp.add_argument("--c", action="store_true", help="C-stick instead of main stick")
    sp.add_argument("--buttons", nargs="*", default=None, help="buttons to hold during stick")
    sp.set_defaults(fn=cmd_stick)

    sp = sub.add_parser("hold")
    sp.add_argument("buttons", nargs="+")
    sp.set_defaults(fn=cmd_hold)

    sp = sub.add_parser("release")
    sp.add_argument("buttons", nargs="*")
    sp.set_defaults(fn=cmd_release)

    sp = sub.add_parser("wait")
    sp.add_argument("frames", type=int)
    sp.set_defaults(fn=cmd_wait)

    sp = sub.add_parser("screenshot")
    sp.add_argument("path")
    sp.add_argument("--method", choices=["window", "framedrawn"], default="window")
    sp.add_argument("--flip", action="store_true", help="flip vertically if output is upside down")
    sp.set_defaults(fn=cmd_screenshot)

    sp = sub.add_parser("read")
    sp.add_argument("target", help="symbol name or 0xADDR")
    sp.add_argument("size", nargs="?", type=int, default=None)
    sp.set_defaults(fn=cmd_read)

    sp = sub.add_parser("write")
    sp.add_argument("target")
    sp.add_argument("value")
    sp.add_argument("--size", type=int, default=None)
    sp.set_defaults(fn=cmd_write)

    sub.add_parser("state").set_defaults(fn=cmd_state)

    sp = sub.add_parser("warp")
    sp.add_argument("floor_id")
    sp.set_defaults(fn=cmd_warp)

    sp = sub.add_parser("save-state")
    sp.add_argument("name")
    sp.set_defaults(fn=cmd_save_state)

    sp = sub.add_parser("load-state")
    sp.add_argument("name")
    sp.set_defaults(fn=cmd_load_state)

    args = p.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
