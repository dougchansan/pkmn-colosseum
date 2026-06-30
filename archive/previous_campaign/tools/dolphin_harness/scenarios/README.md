# PC-port parity scenarios

Compare the same scene rendered by the real game (Dolphin, via the play
harness) against the PC port, to verify visual/mechanical parity.

Both capture paths are proven:

| Backend | How a frame is captured |
| --- | --- |
| **Dolphin (reference)** | drive to the scene with `ctl.py` (or `launch --load-state <name>`), then `ctl.py screenshot ref.png` |
| **PC port** | `pcport_bootstrap.exe --field` with `PCPORT_FIELD_ARCHIVE=<map>`, `PCPORT_DUMP=<out.bmp>`, `PCPORT_MENU_FRAMES=120`, run via native PowerShell; convert BMP→PNG with Pillow |

## Comparing

`compare.py` pairs a reference and a port capture and emits a side-by-side HTML
gallery + similarity metrics into `out/` (gitignored):

```
python compare.py --ref dolphin.png --port pcport.png --label outskirt_stand
# or batch:
python compare.py --manifest pairs.json   # [{ "label":..., "ref":..., "port":... }]
```

Metrics (exact pixels are NOT expected across Dolphin's GX and the port's host GL):
- **dHash distance** (0–64): composition similarity; <~10 = same shot, >20 flags a mismatch.
- **channel MAD** (0–255 per R,G,B): brightness/color drift.

The gallery is for human sign-off; the metrics gate regressions.

## Capturing matching scenes

The PC port's `--field` path renders **D1_garage_1F** (Wes's hideout) by default
and accepts `PCPORT_FIELD_ARCHIVE=<map>` to load others. Pick a map both
backends can reach:

- **D1_garage_1F** — PC port dumps it headless cleanly; reach it on the Dolphin
  side by playing to the hideout (or warping once its real `gsFloorCurrentId`
  is known).
- **S1_out** (The Outskirt Stand) — where a fresh Story-Mode New Game lands on
  Dolphin (savestate `outskirt_stand`). The PC port's S1_out path currently
  runs an interactive walk loop that stalls under a headless frame cap, so a
  clean headless S1_out dump depends on a small change in the PC-port exe
  (that work lives in the PC-port lane, not here). Until then, pair on
  D1_garage_1F.

## Status

- `compare.py` — done, validated (correctly flags mismatched scenes).
- Dolphin capture — done (`ctl.py screenshot`).
- PC-port capture — done for D1_garage_1F (`--field` + `PCPORT_DUMP`).
- Remaining: a same-scene pair end-to-end (blocked only on reaching the same
  map on both sides — see above), then wire a `run_scenario.py` that drives
  both backends from one scenario file.
