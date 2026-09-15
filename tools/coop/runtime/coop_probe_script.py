"""In-Dolphin probe: sample the co-op telemetry block and write it as JSON.

Runs inside Felk's Dolphin scripting fork via --script. Note that `__file__` is
not defined in that embedded interpreter, so all paths arrive via environment
variables:

    COOP_STATS_ADDR   hex address of coop_stats (from the build manifest)
    COOP_OUT          path to write the JSON result to
    COOP_FRAMES       how many frames to run before stopping
    COOP_LABEL        free-form label recorded in the output
"""

import json
import os
import sys

STATS = int(os.environ.get("COOP_STATS_ADDR", "0x80496880"), 16)
PAYLOAD = int(os.environ.get("COOP_PAYLOAD_ADDR", "0x80496740"), 16)
OUT = os.environ["COOP_OUT"]
FRAMES = int(os.environ.get("COOP_FRAMES", "3600"))
LABEL = os.environ.get("COOP_LABEL", "unlabelled")
LOG = OUT + ".log"

os.makedirs(os.path.dirname(OUT), exist_ok=True)


def w(msg):
    with open(LOG, "a") as f:
        f.write(str(msg) + "\n")


w(f"probe imported, python={sys.version}")
w(f"stats=0x{STATS:08X} frames={FRAMES} label={LABEL}")

try:
    from dolphin import event, memory
    w("dolphin module imported")
except Exception as e:  # noqa: BLE001
    w(f"dolphin import FAILED: {e!r}")
    raise

MAGIC = 0x434F4F50


def snapshot():
    slots = []
    for i in range(4):
        b = STATS + 0x40 + i * 0x20
        slots.append(
            {
                "logical_id": memory.read_u32(b + 0x00),
                "held": memory.read_u32(b + 0x04),
                "prev": memory.read_u32(b + 0x08),
                "stick_x": memory.read_u32(b + 0x0C),
                "stick_y": memory.read_u32(b + 0x10),
                "raw_x": memory.read_u32(b + 0x14),
                "raw_y": memory.read_u32(b + 0x18),
                "status": memory.read_u32(b + 0x1C),
            }
        )
    return {
        "diag": {
            # Is the patched code actually resident? 0x800F7F64 holds the
            # original `stwu r1,-0x20(r1)` (0x9421FFE0) in a stock build and a
            # branch to the trampoline (0x4839E7E0) in the patched one.
            "hook_poll_site": memory.read_u32(0x800F7F64),
            "hook_reg_site": memory.read_u32(0x800F80B0),
            # First instruction of the payload: coop_entry is `blr`.
            "payload_entry": memory.read_u32(PAYLOAD),
            "arena_lo_site": memory.read_u32(0x80099B54),
            "game_id": bytes(
                memory.read_u8(0x80000000 + i) for i in range(6)
            ).decode("ascii", "replace"),
        },
        "magic": memory.read_u32(STATS + 0x00),
        "version": memory.read_u32(STATS + 0x04),
        "poll_total": memory.read_u32(STATS + 0x08),
        "reg_total": memory.read_u32(STATS + 0x0C),
        "frame_total": memory.read_u32(STATS + 0x10),
        "gate_allow": memory.read_u32(STATS + 0x14),
        "gate_block": memory.read_u32(STATS + 0x18),
        "reg_count": [memory.read_u32(STATS + 0x20 + i * 4) for i in range(4)],
        "reg_other": memory.read_u32(STATS + 0x34),
        "last_poll_id": memory.read_u32(STATS + 0x38),
        "last_reg_id": memory.read_u32(STATS + 0x3C),
        "slots": slots,
    }


result = {
    "label": LABEL,
    "stats_addr": f"0x{STATS:08X}",
    "frames_requested": FRAMES,
    "samples": [],
}

EARLY_WATCH_FRAMES = int(os.environ.get("COOP_EARLY_WATCH", "600"))
result["payload_transitions"] = []
last_payload_word = None

frame = 0
first_magic_frame = None
try:
    while frame < FRAMES:
        await event.frameadvance()  # noqa: F704  (top-level await is supported)
        frame += 1

        # Early per-frame watch: distinguish "the section was never loaded"
        # from "it was loaded and then cleared" by recording every transition
        # of the payload's first word.
        if frame <= EARLY_WATCH_FRAMES:
            word = memory.read_u32(PAYLOAD)
            if word != last_payload_word:
                result["payload_transitions"].append(
                    {"frame": frame, "value": f"0x{word:08X}"}
                )
                w(f"payload_entry transition at frame={frame}: 0x{word:08X}")
                last_payload_word = word

        if frame % 300 == 0 or frame == 60:
            snap = snapshot()
            snap["frame"] = frame
            result["samples"].append(snap)
            if snap["magic"] == MAGIC and first_magic_frame is None:
                first_magic_frame = frame
            w(
                f"frame={frame} magic=0x{snap['magic']:08X} "
                f"frames={snap['frame_total']} poll={snap['poll_total']} "
                f"gate(a/b)={snap['gate_allow']}/{snap['gate_block']} reg={snap['reg_total']} "
                f"reg_count={snap['reg_count']} "
                f"ids={[s['logical_id'] for s in snap['slots']]}"
            )
    result["status"] = "completed"
except Exception as exc:  # noqa: BLE001
    result["status"] = f"error: {exc!r}"
    w(f"ERROR {exc!r}")

result["frames_run"] = frame
result["first_magic_frame"] = first_magic_frame
result["final"] = snapshot()

with open(OUT, "w") as f:
    json.dump(result, f, indent=2)

w(f"probe finished, wrote {OUT}")
