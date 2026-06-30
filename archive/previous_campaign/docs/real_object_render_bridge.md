# Real Object Render Bridge

## Goal

Advance from the real narrow display-list-backed scene-like slice to the first
real repository-object-backed render path, without jumping to engine boot.

## Ranked Options

1. One real narrow `HSD_PObj`-backed path
   - Best next step.
   - The existing host GX subset already supports the indexed attributes needed
     by a minimal `HSD_VtxDescList`: `POS`, `CLR0`, and `TEX0`.
   - `src/hsd/hsd_pobj_disp.c` already contains the smallest useful repository
     helper for this: `fn_801AA568`, which configures GX state from
     `HSD_PObj->verts`.
   - This keeps the already-working game-owned dispatch entry point
     `fn_800DAD10`.

2. Broaden `GXCallDisplayList` slightly to fit the smallest repository object
   - Not needed for this step.
   - The chosen object path fit the current indexed subset without additional GX
     work.

3. Minimal bridge around one adjacent `gs_render` helper
   - Smaller only if the object path failed to host-compile.
   - It would not close the current gap, which was specifically the lack of a
     repository object path.

## Chosen Path

I chose option 1: a real narrow `HSD_PObj`-backed path using `fn_801AA568` from
`src/hsd/hsd_pobj_disp.c`, then feeding the resulting display-list-backed draw
through the existing `fn_800DAD10` path.

Why this was the fastest path toward a title/menu-capable slice:

- it reuses the already verified `fn_800DAD10` path
- it uses real repository object types:
  - `HSD_PObj`
  - `HSD_VtxDescList`
- it uses a real repository helper from `hsd_pobj_disp.c`
- it did not require engine boot
- it did not require any new GX support beyond the current narrow subset

## What Real Repository Object Path Was Integrated

The verified path is now:

1. `pcport_main.c` builds a real `HSD_PObj`
2. that `HSD_PObj` points at a real `HSD_VtxDescList`
3. the repository helper `fn_801AA568` in `src/hsd/hsd_pobj_disp.c` configures
   GX vertex descriptor state from `pobj->verts`
4. `pcport_main.c` passes the same object's `display` and `n_display` into the
   already integrated game-owned `fn_800DAD10`
5. `fn_800DAD10` dispatches the display list through the existing GS render path
   and host GX layer

This is a real repository object path because the object contract and vertex
descriptor setup come from `HSD_PObj` / `HSD_VtxDescList` and the repository
helper in `hsd_pobj_disp.c`.

Important boundary:

- the geometry and display-list bytes are still bootstrap-authored smoke data
- they are now carried by a real repository object path
- they are not yet loaded from real game assets or a real `HSD_PObjLoadDesc`
  path

## Minimum Changes

`src/hsd/hsd_pobj_disp.c`

- added a narrow `PCPORT` subset that host-compiles only the smallest needed
  object-side helper slice:
  - `fn_801AA35C`
  - `fn_801AA498`
  - `fn_801AA4CC`
  - `fn_801AA538`
  - `fn_801AA568`

`src/pcport/pcport_main.c`

- added a real `HSD_VtxDescList` describing the existing narrow indexed vertex
  layout
- added `InitSceneLikePObj()` to build a minimal `HSD_PObj`
- added `RunGSgfxPObjSmoke()` which:
  - initializes `GSgfx`
  - calls `GSgfx_BeginFrame()`
  - runs repository `fn_801AA568(&pobj)`
  - dispatches the object's display list through `fn_800DAD10`
  - verifies panel/header/background pixels
- added the `--gsgfx-pobj-smoke` command-line path

`CMakeLists.txt`

- added `src/hsd/hsd_pobj_disp.c` to `pcport_bootstrap`

## Additional GX Support Required

None.

That is the important result in this step: the current narrow GX subset was
already sufficient for a first `HSD_PObj`-backed path.

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
```

## Results

Build passed.

Primary verification passed:

```text
[pcport_bootstrap] GSgfx HSD_PObj smoke passed (panel=230,220,188,255 header=208,122,32,255 bg=48,213,94,255)
```

Regression checks also passed:

- `--gsgfx-scene-like-smoke`
- `--gsgfx-visible-smoke`
- `--gsgfx-scissor-retry`
- `--gsgfx-smoke`

## What Was Verified

Verified directly:

- `src/hsd/hsd_pobj_disp.c` host-compiled under the narrow `PCPORT` subset
- a real `HSD_PObj` / `HSD_VtxDescList` object path now reaches
  `fn_800DAD10`
- the visible layered result still appears through the same game-owned dispatch
  point
- the current bridge scales from:
  - raw display-list smoke data
  - to a real repository object path carrying that data

## What Is Still Not Proven

Still unproven:

- loading a real `HSD_PObj` from repository asset data
- `HSD_PObjLoadDesc`
- a real title/menu object path
- broader PObj display paths like rigid/envelope/shape dispatch
- richer GX attribute coverage beyond the current subset

## Exact Remaining Blockers

1. The object path is real, but the object contents are still bootstrap-authored
   smoke data rather than asset-backed repository content.
2. `GXCallDisplayList` still only supports a tiny attribute / primitive subset.
3. `fn_800D7A70` and `fn_800D892C` remain stubs in the current `PCPORT`
   `gs_render.c` slice.
4. Real `HSD_PObj` rendering beyond descriptor setup will require either:
   - a narrow repository object-load path, or
   - a broader `GXCallDisplayList` / PObj rendering surface.
5. Engine boot is still not proven to be the smaller next step, because this
   object-path increment succeeded without it.

## Next Smallest Milestone

The next smallest honest milestone is to replace the bootstrap-authored `HSD_PObj`
contents with the smallest real repository-backed object data source that still
fits the current indexed attribute subset.

If that proves larger than expected, the next exact decision point is whether:

- broadening `GXCallDisplayList` for one richer real `HSD_PObj` path, or
- integrating a minimal asset/object load path

is smaller than engine boot.
