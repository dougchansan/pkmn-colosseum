"""Drive both GameCube ports and watch the game's own pad table react.

Answers three things at once, without needing a human on two controllers:
  1. Does entry +0x24 (held) update per frame at all?
  2. Which Dolphin port maps to which logical pad ID?
  3. Can the two pads be driven independently?

Needs no co-op payload, so it can run against the stock game.
  COOP_OUT, COOP_LABEL
"""
import json, os

OUT = os.environ["COOP_OUT"]
LABEL = os.environ.get("COOP_LABEL", "unlabelled")
LOG = OUT + ".log"
os.makedirs(os.path.dirname(OUT), exist_ok=True)

def w(m):
    with open(LOG, "a") as f:
        f.write(str(m) + "\n")

from dolphin import event, memory, controller

PAD_TABLE = 0x80401C10
STRIDE = 0x6C
NEUTRAL = {"A": False, "B": False, "StickX": 0.0, "StickY": 0.0}

# (start_frame, port0 inputs, port1 inputs, description)
PHASES = [
    (400,  {"A": True},                  {},                          "port0 A only"),
    (520,  {},                           {},                          "release"),
    (640,  {},                           {"A": True},                 "port1 A only"),
    (760,  {},                           {},                          "release"),
    (880,  {"A": True},                  {"B": True},                 "port0 A + port1 B"),
    (1000, {},                           {},                          "release"),
    (1120, {"StickX": -1.0},             {"StickX": 1.0},             "sticks opposite"),
    (1240, {},                           {},                          "release"),
]
END = 1360

def table():
    out = []
    for i in range(4):
        e = PAD_TABLE + i * STRIDE
        out.append({
            "logical_id": memory.read_u32(e + 0x00),
            "status": memory.read_u32(e + 0x0C),
            "held": memory.read_u16(e + 0x24),
            "raw_x": memory.read_u8(e + 0x26),
            "proc_x": memory.read_u8(e + 0x58),
        })
    return out

def phase_for(frame):
    cur = (None, {}, {}, "warmup")
    for ph in PHASES:
        if frame >= ph[0]:
            cur = ph
    return cur

res = {"label": LABEL, "observations": []}
frame = 0
last_desc = None
while frame < END:
    await event.frameadvance()  # noqa: F704
    frame += 1
    _, p0, p1, desc = phase_for(frame)

    if frame >= 400:
        a = dict(NEUTRAL); a.update(p0)
        b = dict(NEUTRAL); b.update(p1)
        controller.set_gc_buttons(0, a)
        controller.set_gc_buttons(1, b)

    if frame >= 400 and (desc != last_desc or frame % 40 == 0):
        t = table()
        res["observations"].append({"frame": frame, "phase": desc, "table": t})
        w(f"f={frame:<5} {desc:<20} " + " ".join(
            f"[id{s['logical_id']} held=0x{s['held']:04X} rawx={s['raw_x']:3d} px={s['proc_x']:3d}]"
            for s in t if s["logical_id"] != 0))
        last_desc = desc

res["final_table"] = table()
with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
w("input probe done")
