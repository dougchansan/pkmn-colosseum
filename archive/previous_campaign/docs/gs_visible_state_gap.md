# GS Visible State Gap

## Goal

Resolve the state/setup blocker that prevented the existing game-owned `GSgfx_BeginFrame` path from producing visible pixels, without changing the entry point or widening scope into broader runtime work.

## Starting Point

Already verified before this step:

- host window creation works
- backbuffer readback works
- the host GX primitive submission path behind `GXBegin` / `GXEnd` works
- `GSgfx_BeginFrame` reaches the correct primitive submission path

Still failing before this step:

```text
[pcport_bootstrap] GSgfx_BeginFrame visible attempt failed (rgba=0,0,0,255 submitted=4 expanded=6 prim=0x80)
```

## Direct Comparison

Passing host-only diagnostic in `src/pcport/pcport_main.c` sets:

- `GXSetViewport(0, 0, 640, 480, 0, 1)`
- `GXSetProjection(identity, GX_ORTHOGRAPHIC)`
- `GXLoadPosMtxImm(identity, 0)`
- `GXSetCullMode(GX_CULL_NONE)`
- `GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE)`
- `GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY)`

Current game-owned `GSgfx_BeginFrame` in `src/game/gs_render.c` originally set:

- `GXSetProjection(identity, ORTHOGRAPHIC)`
- `GXLoadPosMtxImm(identity, 0)`
- `GXSetCullMode(CULL_NONE)`
- immediate quad submission

What was already covered elsewhere:

- viewport was already being set by the registered pre-retrace callback before `GSgfx_BeginFrame`
- this was verified from the code path:
  - `VIWaitForRetrace_PC()` in `src/pcport/os_shim.c`
  - `PCPort_RunPreRetraceCallback()` in `src/pcport/gs_gfx_host_support.c`
  - `GSgfx_PreRetraceCallback()` in `src/game/gs_render.c`

So the remaining live state gap was reduced to explicit depth/blend alignment immediately before draw.

## Candidate Ranking

1. Add explicit game-owned blend-state alignment in `GSgfx_BeginFrame`
   - smallest remaining state delta after viewport was accounted for
   - keeps the chosen entry point game-owned
   - does not rely on changing host defaults

2. Add explicit game-owned depth-state alignment in `GSgfx_BeginFrame`
   - also small, but less likely because the original path already had a sensible cleared depth buffer and valid geometry depth

3. Change host GX defaults in `GXInit`
   - less honest because it papers over missing game-owned setup from the host side
   - only justified if game-owned alignment failed

## Chosen Fix Path

Use the smallest additional game-owned state setup inside `GSgfx_BeginFrame`.

I tested this incrementally:

1. Add `GXSetZMode(0, GX_ALWAYS, 0)` only.
   - Result: still failed, `rgba=0,0,0,255`
2. Add explicit blend disable before draw.
   - Result: visible pixels appeared
3. Remove the temporary depth override and rerun.
   - Result: visible pixels still appeared

That directly verifies that the minimum required fix was the blend-state alignment, not the depth override.

## Code Change

Final minimal change in `src/game/gs_render.c`:

- add `GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);` inside the `PCPORT` `GSgfx_BeginFrame` subset before `GXBegin`

No changes were needed to `GXBegin` / `GXEnd` submission logic in this task.

## Commands Attempted

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Visible test:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gx-primitive-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
.\build_pcbootstrap\pcport_bootstrap.exe
```

## Results

Host GX diagnostic still passed:

```text
[pcport_bootstrap] Host GX primitive smoke passed (rgba=32,208,64,255 submitted=4 expanded=6 prim=0x80)
```

Game-owned `GSgfx_BeginFrame` visible attempt now passes:

```text
[pcport_bootstrap] GSgfx_BeginFrame visible attempt passed (rgba=48,213,94,255)
```

What is directly verified:

- visible pixels now appear because of game-owned code in `GSgfx_BeginFrame`
- the produced pixel color matches the game-owned quad color written through the GX shim path
- the existing `--gsgfx-smoke` callback/swap path still passes
- the default headless bootstrap still passes

What is still inferred:

- the practical reason blend was the missing step is inferred from the experiment sequence and the state comparison
- specifically: viewport was already covered, depth override was unnecessary, and blend alignment was the remaining tested delta that changed the result

## Remaining Blockers

This is the first visible game-owned slice, but it is still very narrow:

- it is a host-only `PCPORT` subset of `GSgfx_BeginFrame`, not the full original decompiled function
- it only proves a flat visible quad path, not a title/menu/game state
- no broader `gs_render` draw-command path is integrated
- TEV/material/texture/light handling in the host GX layer is still partial or stubbed
- no engine-owned boot loop or `main.c` path is integrated

## Next Smallest Milestone

Advance from a visible flat quad to the next smallest real game-owned render step:

- either the next `gs_render.c` helper that sets additional per-frame state used by real draws
- or the smallest draw helper that emits one non-trivial game-owned visible operation without requiring assets

The critical path has moved past “can the bridge show game-owned pixels at all” and onto “which next real game-owned render step can be integrated without pulling in the full engine boot path.”
