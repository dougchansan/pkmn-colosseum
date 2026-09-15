"""Read the game's pad table directly, with no co-op payload involved.

Validates pad registration independently of the co-op patch, and counts
frameadvance vs framedrawn as a liveness signal.

  COOP_OUT, COOP_FRAMES, COOP_LABEL
"""
import json, os, sys

OUT = os.environ["COOP_OUT"]
FRAMES = int(os.environ.get("COOP_FRAMES", "1800"))
LABEL = os.environ.get("COOP_LABEL", "unlabelled")
LOG = OUT + ".log"
os.makedirs(os.path.dirname(OUT), exist_ok=True)

def w(m):
    with open(LOG, "a") as f:
        f.write(str(m) + "\n")

from dolphin import event, memory

PAD_TABLE = 0x80401C10
STRIDE = 0x6C

drawn = 0
def _on_drawn():
    global drawn
    drawn += 1
event.on_framedrawn(_on_drawn)

def table():
    out = []
    for i in range(4):
        e = PAD_TABLE + i * STRIDE
        out.append({
            "slot": i,
            "logical_id": memory.read_u32(e + 0x00),
            "status": memory.read_u32(e + 0x0C),
            "held": memory.read_u16(e + 0x24),
            "prev": memory.read_u32(e + 0x30),
            "raw_x": memory.read_u8(e + 0x26),
            "raw_y": memory.read_u8(e + 0x27),
            "proc_x": memory.read_u8(e + 0x58),
            "proc_y": memory.read_u8(e + 0x59),
        })
    return out

res = {"label": LABEL, "samples": []}
frame = 0
while frame < FRAMES:
    await event.frameadvance()  # noqa: F704
    frame += 1
    if frame % 300 == 0 or frame == 60:
        t = table()
        res["samples"].append({"frame": frame, "drawn": drawn, "table": t})
        w(f"frame={frame} drawn={drawn} " +
          " ".join(f"[id={s['logical_id']} st={s['status']} held=0x{s['held']:04X}]" for s in t))

res["frames_run"] = frame
res["drawn_total"] = drawn
res["final_table"] = table()
with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
w("padtable probe done")
