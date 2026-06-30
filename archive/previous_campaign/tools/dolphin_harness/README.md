# Dolphin Play Harness

Lets an AI agent (or any script) play Pokémon Colosseum (GC6E01) in Dolphin:
send GameCube pad input, capture screenshots, read/write emulated RAM
(symbol-named via the decomp's `config/GC6E01/symbols.txt`), and save/load
states. Built for capturing ground-truth reference behavior to verify the PC
port (`pcport/`) visually and mechanically.

## Stack

- **Emulator**: [Felk's Dolphin scripting fork](https://github.com/Felk/dolphin),
  release `scripting-preview4` (embedded Python 3.12). Installed to `bin/`
  (gitignored) by `setup.ps1`.
- **In-Dolphin server**: `inproc/harness_server.py`, attached via `--script`.
  Polls a non-blocking localhost TCP socket each frame (the script
  subinterpreter is isolated: no threads, no `__file__`) and executes NDJSON
  commands at frame granularity.
- **Host CLI**: `ctl.py` — one prompt-returning command per invocation.
- **Window capture**: `winshot.py` (PrintWindow + PW_RENDERFULLCONTENT,
  DPI-aware, crops to the detached render window's client area).

## Setup

```powershell
powershell -ExecutionPolicy Bypass -File tools\dolphin_harness\setup.ps1
python tools\dolphin_harness\ctl.py launch
```

Requires Windows-side Python 3.12 with `pillow` + `pywin32`. Always launch
via native PowerShell/Windows Python (WSL interop hangs launching exes).

## Verbs

| Verb | Example | Notes |
| --- | --- | --- |
| launch | `ctl.py launch [--load-state NAME] [--iso PATH]` | isolated profile seeded from `profile_template/`; polls until server is live |
| kill / status | `ctl.py status` | status detects dead-pid and dead-socket separately |
| press | `ctl.py press A --frames 4` | buttons: A B X Y Z L R Start Up Down Left Right |
| stick | `ctl.py stick 0 -1 --frames 6` | -1..1 floats: `+x`=right, `-x`=left, `+y`=UP, `-y`=DOWN (raw 0-255 if magnitude>1); `--c` for C-stick. Menus use the stick, NOT the D-pad — see PLAYBOOK.md |
| hold / release | `ctl.py hold R` | persists across commands, reapplied every frame |
| wait | `ctl.py wait 600` | frames (60/s) |
| screenshot | `ctl.py screenshot out.png` | default `--method window` (crash-free). `--method framedrawn` = exact emulated pixels, but can crash Dolphin during scene transitions — stable scenes only |
| read / write | `ctl.py read gsFloorCurrentId` | symbol names via `symbols.py` (symbols.txt + `symbol_aliases.json`) or `0x8047B420` |
| state | `ctl.py state` | curated JSON snapshot (floor id/state + frame) |
| warp | `ctl.py warp 0x80` | validated floor-warp data path (docs/dolphin_ar_validation.md) |
| save-state / load-state | `ctl.py save-state phenac_city` | stored in `states/<name>.sav` (gitignored) |

## Environment findings (hard-won, do not rediscover)

- Felk script subinterpreter: **no threads, no `__file__`**; top-level await
  works. Paths come from `DOLPHIN_HARNESS_ROOT` env var.
- `controller.set_gc_buttons()` holds for **one frame only** → server
  re-applies pad state every frameadvance.
- `framedrawn` returns black for XFB-direct content (title card, video)
  unless GFX.ini sets `XFBToTextureEnable = False` (XFB-to-RAM) — shipped in
  `profile_template/GFX.ini`. Even then it can **crash Dolphin during scene
  transitions**, hence window capture is the default screenshot method.
- Window capture needs `SetProcessDpiAwareness(2)` or captures are
  cropped/zoomed on >100% display scaling.
- The detached render window (RenderToMain=False) is identified by the
  `" | "` segments in its title; the main window title is bare.
- Game saves: the harness profile is fresh (no memory card) → new-game flow.
  The user's real save is backed up in `backup/01-GC6E-pokemon_colosseum.gci`
  (copy it into `run/profile/GC/USA/Card A/` to play from the Story Mode save).
