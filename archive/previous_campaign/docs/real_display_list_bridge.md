# Real Display-List Bridge

## Goal

Advance from the synthetic scene-like slice to the smallest honest real
display-list-backed path, without jumping to engine boot or claiming title/menu
rendering.

## Ranked Options

1. Tiny real GX display-list subset in `GXCallDisplayList`
   - Best gradient increase.
   - Keeps the already working game-owned entry point: `fn_800DAD10` in
     `src/game/gs_render.c`.
   - Needs only a narrow parser for one verified command shape instead of a
     full HSD object path.
   - Lets the same scene-like smoke prove that the visible result is no longer
     backed by the host diagnostic `GXHostDisplayList` struct.

2. Real narrow repository object feeding `fn_800DAD10`
   - More honest in the long term, but larger right now.
   - `src/hsd/hsd_pobj_disp.c` shows that even the narrow HSD path expects
     vertex descriptor setup, array pointers, matrix state, and an actual
     `HSD_PObj` / `HSD_VtxDescList` object graph before calling
     `GXCallDisplayList`.
   - That is a bigger step than proving a first real display-list subset.

3. Minimal bridge around `fn_800D9B58`
   - Still smaller than engine boot.
   - Not the right step for this goal because it does not replace the synthetic
     display-list bridge.

## Chosen Path

I chose option 1: add the smallest real GX display-list subset to
`GXCallDisplayList`, then rerun the existing `fn_800DAD10` scene-like smoke
through that path.

Why this was the fastest path toward a title/menu-capable slice:

- `fn_800DAD10` was already integrated and visibly working
- the next blocker was explicitly `GXCallDisplayList`
- `src/hsd/hsd_pobj_disp.c` already points at the future direction:
  `GXClearVtxDesc`, `GXSetVtxDesc`, `GXSetVtxAttrFmt`, `GXSetArray`, then
  `GXCallDisplayList`
- this path crosses from a host-only fake display-list format to actual GX
  command bytes without dragging in object loading or engine boot

## Integrated Real Narrow Path

The verified path is now:

1. `pcport_main.c` configures indexed GX vertex state with:
   - `GXClearVtxDesc`
   - `GXSetVtxDesc(GX_VA_POS, GX_INDEX8)`
   - `GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8)`
   - `GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8)`
   - `GXSetVtxAttrFmt(... GX_POS_XYZ, GX_F32 ...)`
   - `GXSetVtxAttrFmt(... GX_CLR_RGBA, GX_RGBA8 ...)`
   - `GXSetVtxAttrFmt(... GX_TEX_ST, GX_F32 ...)`
   - `GXSetArray(...)` for positions, colors, and texcoords
2. The smoke object passed to game-owned `fn_800DAD10` now contains raw GX
   command bytes:
   - primitive command `0x80` (`GX_QUADS`, `GX_VTXFMT0`)
   - big-endian vertex count
   - indexed attribute triplets for `POS`, `CLR0`, `TEX0`
3. `fn_800DAD10` still owns dispatch through `fn_800BD0FC` to
   `GXCallDisplayList`
4. `GXCallDisplayList` now parses that narrow real GX stream and submits it
   through the existing host primitive path

What is real now:

- the display-list command stream shape
- the GX vertex-descriptor / array setup contract
- the game-owned dispatch path in `gs_render.c`

What is still synthetic:

- the geometry arrays are still host-authored smoke data, not real game assets
  or real HSD-loaded objects

## Minimum Code Changes

`src/pcport/gx_shim.h`

- added narrow GX vertex descriptor enums and prototypes needed by the real
  display-list subset:
  - `GXAttr`
  - `GXAttrType`
  - `GXCompCnt`
  - `GXCompType`
  - `GXSetCurrentMtx`
  - `GXSetVtxDesc`
  - `GXClearVtxDesc`
  - `GXSetVtxAttrFmt`
  - `GXSetArray`

`src/pcport/gx_shim.c`

- added descriptor state storage
- added indexed attribute decode for:
  - `GX_VA_POS`
  - `GX_VA_CLR0`
  - `GX_VA_TEX0`
- added narrow raw display-list parsing in `GXCallDisplayList`
- kept the older host diagnostic format as a fallback, but the verified
  scene-like smoke no longer depends on it

`src/pcport/pcport_main.c`

- replaced the old `GXHostDisplayList` scene-like data with:
  - raw position/color/texcoord arrays
  - raw GX display-list bytes
  - descriptor setup via `ConfigureSceneLikeDisplayListState()`
- kept the same `RunGSgfxSceneLikeSmoke()` entry point and pixel checks

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

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

Build passed.

Primary verification passed:

```text
[pcport_bootstrap] GSgfx scene-like display-list smoke passed (panel=230,220,188,255 header=208,122,32,255 bg=48,213,94,255)
```

Regression checks also passed:

- `--gsgfx-visible-smoke`
- `--gsgfx-scissor-retry`
- `--gsgfx-smoke`

## What Was Verified

Verified directly:

- `fn_800DAD10` still produced the layered scene-like result
- the visible result is no longer backed by the host diagnostic
  `GXHostDisplayList` format
- the same visible output now comes from:
  - real GX vertex-descriptor setup
  - real narrow GX command bytes
  - game-owned dispatch through `fn_800DAD10`

This is a real narrow display-list-backed path.

## Exact Remaining Blockers

1. `GXCallDisplayList` still supports only a tiny subset:
   - indexed `GX_VA_POS`
   - indexed `GX_VA_CLR0`
   - indexed `GX_VA_TEX0`
   - the verified smoke path uses `GX_INDEX8`, `GX_VTXFMT0`, and `GX_QUADS`
2. Real title/menu objects are still not feeding the path.
   - the geometry is host-authored smoke data
   - no real `HSD_PObj` or asset-backed object graph is integrated yet
3. `fn_800D7A70` and `fn_800D892C` are still stubs in the `PCPORT`
   `gs_render.c` subset
4. TEV/material/texture/light behavior for real content is still largely
   partial or stubbed
5. Engine-owned title/menu boot is still not integrated, but it is not yet the
   smaller next step than broadening the real display-list / object path

## Next Smallest Milestone

The next smallest honest milestone is to feed this same `fn_800DAD10` path from
one real narrow repository object setup, most likely the smallest `HSD_PObj`
path that can stay within the currently supported indexed attribute subset.

If that proves larger than expected, the exact blocker chain should then decide
whether broadening `GXCallDisplayList` or pulling one minimal HSD object-loader
path is smaller than engine boot.
