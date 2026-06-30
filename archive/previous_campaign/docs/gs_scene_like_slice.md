# GS Scene-Like Slice

## Goal

Advance from the current visible game-owned flat-quad and scissored-panel slices
 to the next smallest render path that is more scene-like, without pulling in
assets or broad engine boot.

## Candidate Ranking

1. `fn_800DAD10`
   - Best next gradient increase.
   - It is the first small draw-dispatch entry in `gs_render.c` that is closer
     to how title/menu/model paths actually submit geometry.
   - It is still much smaller than `GSgfx_DrawDispatch`.
   - Required GX support is narrow and concrete: a minimal display-list replay
     bridge plus the two adjacent pipeline hooks it calls.

2. `fn_800D9B58` plus the title/menu state bundle
   - This bundle is used by title/menu/effect call sequences.
   - It is a real step toward those paths, but it is still mostly render-state
     setup.
   - It needs more host projection / TEV alignment before it adds visible payoff
     beyond the current panel.

3. `GSgfx_DrawDispatch` (`fn_800E1544`)
   - This is the most direct real renderer path.
   - It is too large for this step.
   - It would immediately drag in broader display-list parsing, vertex
     descriptor handling, material state, and more GX coverage than was needed
     for a first scene-like increment.

## Chosen Path

I chose `fn_800DAD10`.

Why:

- it is the first small `gs_render.c` draw-dispatch entry, not just another
  state helper
- it is materially closer to title/menu/model rendering than the current
  `GSgfx_BeginFrame` flat quad
- it can be exercised without assets if the host provides a minimal diagnostic
  display-list format
- it avoids jumping straight to `GSgfx_DrawDispatch`

## Additional GX Support Required

The minimum support needed for this path was:

- a host-only diagnostic display-list format in
  `src/pcport/gx_shim.h`
- host replay of that format in `GXCallDisplayList` in
  `src/pcport/gx_shim.c`
- an original-name wrapper `fn_800BD0FC` in
  `src/pcport/gs_gfx_host_support.c`
- a `PCPORT` `fn_800DAD10` subset in `src/game/gs_render.c`
- the smallest adjacent helpers that `fn_800DAD10` requires:
  - `fn_800D6A5C`
  - `fn_800D7A70`
  - `fn_800D892C`

What those helpers mean in this step:

- `fn_800D6A5C` is real counter bookkeeping and was trivial to carry over.
- `fn_800D7A70` and `fn_800D892C` are still host no-ops here.
- actual GameCube display-list parsing is still not implemented.

## What Was Exercised

I added a narrow smoke path in `src/pcport/pcport_main.c`:

1. initialize `GSgfx`
2. run one retrace
3. clear the backbuffer
4. call game-owned `GSgfx_BeginFrame()` to keep the verified green background
5. call game-owned `fn_800DAD10()` with a synthetic host display-list object
6. read back three pixels:
   - panel center
   - header strip
   - background outside the overlay

The synthetic display list contains three quads:

- a dark outer frame
- a light inner panel
- an amber header strip

This is host-authored geometry, not game assets. The game-owned part is the
`gs_render.c` dispatch path that submits it.

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Scene-like smoke:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
```

## Results

Scene-like smoke passed:

```text
[pcport_bootstrap] GSgfx scene-like display-list smoke passed (panel=230,220,188,255 header=208,122,32,255 bg=48,213,94,255)
```

This directly verifies:

- `gs_render.c` compiled with the new `fn_800DAD10` `PCPORT` subset
- the executable linked
- a real game-owned draw-dispatch entry ran
- the visible result is richer than the previous clipped panel:
  - green background from `GSgfx_BeginFrame`
  - dark framed panel
  - light interior
  - amber header strip

Regression checks also still passed:

- `--gsgfx-visible-smoke`
- `--gsgfx-scissor-retry`
- `--gsgfx-smoke`

## What This Proves

This is materially closer to a title/menu/game slice than the previous panel.

Why:

- the path now includes a real `gs_render.c` draw-dispatch entry
- the visible result is layered, not just a single full-screen quad or a simple
  scissor clip
- the bridge now reaches the first display-list-style submit point that broader
  title/menu/model paths are expected to converge on

## What It Does Not Prove

This does not prove that actual title or menu rendering is working.

Still unproven:

- native playback of real GameCube display lists
- `fn_800D7A70` pipeline selection
- `fn_800D892C` pipeline configuration
- TEV/material/texture/light behavior for real content
- title/menu object creation and engine-owned boot flow

## Remaining Blockers

The next blockers on the path to a first title/menu/game slice are now more
specific:

1. `GXCallDisplayList` only handles the host diagnostic format used in this
   step. It does not parse or replay real GameCube display lists yet.
2. `fn_800D7A70` and `fn_800D892C` are still stubs in the `PCPORT` subset, so
   real pipeline selection is not yet represented.
3. Real title/menu paths will need display-list data coming from game or HSD
   objects, not the synthetic bootstrap object used here.
4. After real display lists are bridged, TEV/material/texture/light behavior is
   still the next large GX gap.

## Next Smallest Milestone

The next smallest honest milestone is:

- feed `fn_800DAD10` a real narrow display-list object from repository code,
  or implement the smallest real GameCube display-list parse/replay path in
  `GXCallDisplayList` for a single primitive subset

If that becomes larger than expected, the fallback path is to bridge the title
/ menu render-state bundle around `fn_800D9B58` only after the real display-list
format is available.
