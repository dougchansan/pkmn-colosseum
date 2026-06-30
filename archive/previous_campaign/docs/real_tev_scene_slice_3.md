## Goal

Advance from the shared real TEV family bridge to the next smallest broader TEV milestone by finding one reachable branch with a second distinct real `TObj->tev` payload shape and exercising it through the existing game-owned draw bridge.

## Ranked TEV Targets

1. `pda_menu.fsys:pda2_bg` object 1
   - Path: `scene_data -> joint 0x2328 -> dobj 0x2298 -> mobj 0x1AEC -> tobj 0x1A7C -> tev 0x1A5C -> pobj 0x2280`
   - Why first: it exposes a real non-null TEV payload distinct from the current `topmenu.fsys:menu_bg00` family, while still fitting the current narrow host `PObj` bridge (`POS`, `CLR0`, `TEX0` only).

2. `pda_menu.fsys:pda2_bg` object 0
   - Path: `scene_data -> joint 0x22E8 -> dobj 0x1FB8 -> mobj 0x1A2C -> tobj 0x19BC -> tev 0x199C -> pobj 0x1FA0`
   - Why second: it is a sibling with another distinct TEV payload, but it is not the smallest next step because its real `PObj` descriptor also requests `GX_VA_TEX1`, which the current narrow `PCPort_TranslatePObjFromArchiveBE` bridge rejects before TEV interpretation becomes relevant.

3. Broader shared `TObj->tev` / `TExp` translation
   - Why deferred: a second distinct real payload shape was reachable more cheaply on `pda2_bg` object 1, so expanding into a broader TEV boundary would have been larger than necessary for this step.

## Chosen Target

`pda_menu.fsys:pda2_bg` object 1.

This was the fastest honest next step because it produced a second real TEV family without requiring:

- broader engine boot
- generic TEV/TExp parsing
- a wider `PObj` attribute bridge
- new GX submission work

## Distinct Real TEV Evidence

Current shared `topmenu` TEV family:

- base `tev 0x3A08`
- sibling `tev 0x393C`
- both resolve to the same verified color pair:
  - `light = 0x7FB2E500`
  - `dark = 0x19334C00`

New reachable `pda2_bg` TEV family:

- chosen object 1 `tev 0x1A5C`
- distinct payload words:
  - `light = 0x66993300`
  - `dark = 0x14230A00`

That is a real second payload shape under a nearby repository scene branch, not a synthetic variation.

## Minimum Work Implemented

- Added a new smoke path in [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3263) for `--real-tev-scene-slice-3-smoke`.
- Reused the existing shared translated TEV boundary from [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L91) and [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L153).
- Translated and rendered only `pda2_bg` object 1.
- Kept object 0 out of the exercised path because its `PObj` is blocked by `GX_VA_TEX1`.

No new generic TEV support was added in this step.

## Exact Steps Attempted

1. Built `pcport_bootstrap`:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Ran the new second-family TEV smoke:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
```

3. Re-ran regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

The new smoke passed:

- `scene=0x28BC4`
- `camera=0x28B38`
- `obj1Joint=0x2328`
- `obj1DObj=0x2298`
- `obj1MObj=0x1AEC`
- `obj1TObj=0x1A7C`
- `obj1Tev=0x1A5C`
- `diffPixels=307200`
- `baselineCenter=0,0,0,59`
- `tevCenter=14,24,7,59`
- `obj1Light=102,153,51,255`
- `obj1Dark=20,35,10,255`
- `baked1=262144`
- `submitted=131`
- `expanded=131`
- `prim=0x98`

This verifies that the visible result now reflects a second real TEV-backed behavior distinct from the current `topmenu` shared family, while still going through the existing game-owned `fn_801AA568 -> fn_800DAD10` bridge.

## Regression Status

These still passed after the new branch was added:

- `--real-tev-scene-slice-2-smoke`
- `--real-tev-scene-slice-smoke`
- `--real-scene-slice-3-smoke`
- `--gsgfx-visible-smoke`

## Exact Blockers Still Remaining

- `pda2_bg` object 0 is still blocked by the current narrow `PObj` bridge because its real vertex descriptor list includes `GX_VA_TEX1`.
- The current TEV bridge still only covers the verified I8 color-ramp family behind the shared translated boundary.
- There is still no general `TObj->tev` / `TExp` parser.
- Multi-stage TEV, TEV alpha semantics, and broader material/lighting behavior are still unresolved.
- No engine-owned title/menu boot or update flow drives these objects yet.

## Next Smallest Milestone

The next smaller step is still broader TEV support, not engine boot:

- either extend the narrow host `PObj` bridge just enough to admit the real `GX_VA_TEX1` object-0 path and test whether its distinct TEV payload changes output
- or expose the next shared `TObj->tev` / `TExp` translation boundary if the `GX_VA_TEX1` expansion turns out larger than expected
