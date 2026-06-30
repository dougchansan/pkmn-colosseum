"""In-Dolphin command server for the play harness.

Runs inside Felk's Dolphin scripting fork (scripting-preview4) via --script.
The script subinterpreter is ISOLATED: no threads allowed, no __file__.
So everything happens on the emu loop: each frameadvance we poll a
non-blocking localhost socket for NDJSON requests and execute them at frame
granularity (<=16ms command latency at 60fps).

Protocol: one JSON object per line in, one per line out, FIFO order.
Timed commands (press/stick/wait) delay their response until the last frame
they cover; queued requests are simply left in the recv buffer meanwhile.

controller.set_gc_buttons() only holds for the current frame, so held/press
state is re-applied on every frameadvance. Screenshots use the awaitable
framedrawn event (raw RGB) and return pixels base64-encoded; the host-side
client (ctl.py) encodes to PNG.
"""
import base64
import json
import os
import socket
import time
import traceback

from dolphin import controller, event, memory, savestate  # type: ignore

ROOT = os.environ.get(
    "DOLPHIN_HARNESS_ROOT",
    r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\dolphin_harness",
)
PORT = int(os.environ.get("DOLPHIN_HARNESS_PORT", "53741"))
RUN_DIR = os.path.join(ROOT, "run")
os.makedirs(RUN_DIR, exist_ok=True)
LOG_PATH = os.path.join(RUN_DIR, "server_log.txt")


def log(msg):
    with open(LOG_PATH, "a") as f:
        f.write(f"{time.strftime('%H:%M:%S')} {msg}\n")


BUTTON_KEYS = ("A", "B", "X", "Y", "Z", "L", "R", "Start", "Up", "Down", "Left", "Right")

NEUTRAL = {
    "A": False, "B": False, "X": False, "Y": False, "Z": False,
    "L": False, "R": False, "Start": False,
    "Up": False, "Down": False, "Left": False, "Right": False,
    "StickX": 128, "StickY": 128, "CStickX": 128, "CStickY": 128,
    "TriggerLeft": 0, "TriggerRight": 0,
    "AnalogA": 0, "AnalogB": 0,
    "Connected": True,
}

srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
srv.bind(("127.0.0.1", PORT))
srv.listen(1)
srv.setblocking(False)
log(f"listening on 127.0.0.1:{PORT}")

conn = None
rxbuf = b""

frame_counter = 0
held = {}      # persistent overrides (hold/release)
active = None  # {"overrides": {...}, "remaining": N, "id": req_id}


def send_resp(resp):
    global conn, rxbuf
    if conn is None:
        return
    try:
        conn.sendall((json.dumps(resp) + "\n").encode())
    except OSError as e:
        log(f"send failed, dropping client: {e!r}")
        try:
            conn.close()
        except OSError:
            pass
        conn = None
        rxbuf = b""


def read_mem(addr, size):
    return bytes(memory.read_u8(addr + i) for i in range(size))


def handle_instant(msg):
    """Commands that complete immediately. Returns response dict."""
    cmd = msg.get("cmd")
    if cmd == "status":
        return {"ok": True, "frame": frame_counter, "port": PORT, "held": dict(held)}
    if cmd == "read":
        addr = int(msg["addr"])
        size = int(msg.get("size", 4))
        if size > 4096:
            return {"ok": False, "error": "size > 4096"}
        data = read_mem(addr, size)
        resp = {"ok": True, "addr": addr, "hex": data.hex()}
        if size == 1:
            resp["u8"] = data[0]
        elif size == 2:
            resp["u16"] = int.from_bytes(data, "big")
        elif size == 4:
            resp["u32"] = int.from_bytes(data, "big")
            resp["f32"] = memory.read_f32(addr)
        return resp
    if cmd == "write":
        addr = int(msg["addr"])
        size = int(msg.get("size", 4))
        value = int(msg["value"])
        if size == 1:
            memory.write_u8(addr, value & 0xFF)
        elif size == 2:
            memory.write_u16(addr, value & 0xFFFF)
        elif size == 4:
            memory.write_u32(addr, value & 0xFFFFFFFF)
        else:
            return {"ok": False, "error": f"unsupported write size {size}"}
        return {"ok": True, "addr": addr, "value": value, "size": size}
    if cmd == "save_state":
        savestate.save_to_file(msg["path"])
        return {"ok": True, "path": msg["path"]}
    if cmd == "load_state":
        savestate.load_from_file(msg["path"])
        return {"ok": True, "path": msg["path"]}
    if cmd == "hold":
        for b in msg.get("buttons", []):
            if b not in BUTTON_KEYS:
                return {"ok": False, "error": f"unknown button {b!r}"}
            held[b] = True
        if "x" in msg:
            held["StickX"] = max(0, min(255, int(msg["x"])))
            held["StickY"] = max(0, min(255, int(msg["y"])))
        return {"ok": True, "held": dict(held)}
    if cmd == "release":
        if msg.get("buttons"):
            for b in msg["buttons"]:
                held.pop(b, None)
        else:
            held.clear()
        return {"ok": True, "held": dict(held)}
    return {"ok": False, "error": f"unknown cmd {msg.get('cmd')!r}"}


def start_timed(msg):
    """Returns active-command dict for press/stick/wait, or None."""
    cmd = msg.get("cmd")
    if cmd not in ("press", "stick", "wait"):
        return None
    overrides = {}
    if cmd == "press":
        for b in msg.get("buttons", []):
            if b not in BUTTON_KEYS:
                raise ValueError(f"unknown button {b!r}")
            overrides[b] = True
        frames = int(msg.get("frames", 4))
    elif cmd == "stick":
        which = msg.get("which", "main")
        kx = "StickX" if which == "main" else "CStickX"
        ky = "StickY" if which == "main" else "CStickY"
        overrides[kx] = max(0, min(255, int(msg["x"])))
        overrides[ky] = max(0, min(255, int(msg["y"])))
        for b in msg.get("buttons", []) or []:
            if b not in BUTTON_KEYS:
                raise ValueError(f"unknown button {b!r}")
            overrides[b] = True
        frames = int(msg.get("frames", 12))
    else:  # wait
        frames = int(msg.get("frames", 60))
    return {"overrides": overrides, "remaining": max(1, frames), "id": msg.get("id")}


def poll_socket():
    """Accept/read without blocking; returns next complete request or None."""
    global conn, rxbuf
    if conn is None:
        try:
            conn, _ = srv.accept()
            conn.setblocking(False)
            rxbuf = b""
        except (BlockingIOError, OSError):
            return None
    try:
        while True:
            chunk = conn.recv(65536)
            if not chunk:  # client disconnected
                conn.close()
                conn = None
                rxbuf = b""
                return None
            rxbuf += chunk
    except BlockingIOError:
        pass
    except OSError as e:
        log(f"recv error, dropping client: {e!r}")
        try:
            conn.close()
        except OSError:
            pass
        conn = None
        rxbuf = b""
        return None
    if b"\n" in rxbuf:
        line, rxbuf = rxbuf.split(b"\n", 1)
        line = line.strip()
        if line:
            try:
                return json.loads(line)
            except Exception as e:  # noqa: BLE001
                send_resp({"ok": False, "error": f"bad json: {e}"})
    return None


log("server script started, entering emu loop")

while True:
    await event.frameadvance()
    frame_counter += 1

    # Pick up the next request only when no timed command is in flight
    # (pipelined requests just sit in rxbuf / the socket until then).
    if active is None:
        msg = poll_socket()
        if msg is not None:
            try:
                if msg.get("cmd") == "screenshot":
                    w, h, data = await event.framedrawn()
                    send_resp({
                        "id": msg.get("id"), "ok": True, "width": w, "height": h,
                        "rgb_b64": base64.b64encode(data).decode("ascii"),
                    })
                else:
                    timed = start_timed(msg)
                    if timed is not None:
                        active = timed
                    else:
                        resp = handle_instant(msg)
                        resp["id"] = msg.get("id")
                        send_resp(resp)
            except Exception as e:  # noqa: BLE001
                log(f"cmd failed: {traceback.format_exc()}")
                send_resp({"id": msg.get("id"), "ok": False,
                           "error": f"{type(e).__name__}: {e}"})

    # Apply pad state for this frame.
    if held or (active and active["overrides"]):
        inputs = dict(NEUTRAL)
        inputs.update(held)
        if active:
            inputs.update(active["overrides"])
        controller.set_gc_buttons(0, inputs)

    # Advance the timed command.
    if active is not None:
        active["remaining"] -= 1
        if active["remaining"] <= 0:
            send_resp({"id": active["id"], "ok": True, "frame": frame_counter})
            active = None
