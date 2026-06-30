"""Minimal --script execution probe: writes a marker file at import, then per-frame."""
import os
import sys

# __file__ is not defined inside Felk's embedded interpreter; the launcher
# exports DOLPHIN_HARNESS_ROOT instead.
_root = os.environ.get(
    "DOLPHIN_HARNESS_ROOT",
    r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\dolphin_harness",
)
_log = os.path.join(_root, "run", "probe_log.txt")
os.makedirs(os.path.dirname(_log), exist_ok=True)


def _w(msg):
    with open(_log, "a") as f:
        f.write(msg + "\n")


_w(f"probe imported, python={sys.version}")

try:
    from dolphin import event, memory
    _w("dolphin module imported")
except Exception as e:  # noqa: BLE001
    _w(f"dolphin import FAILED: {e!r}")
    raise

frame = 0
while True:
    await event.frameadvance()
    frame += 1
    if frame % 60 == 0:
        game_id = bytes(memory.read_u8(0x80000000 + i) for i in range(6))
        _w(f"frame={frame} game_id={game_id!r}")
        if frame >= 300:
            break

_w("probe finished cleanly")
