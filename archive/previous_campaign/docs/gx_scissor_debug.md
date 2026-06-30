# GX Scissor Debug

## Goal

Verify whether the blocker behind the failed richer visible slice was in the host
`GXSetScissor` path, then retry the existing game-owned `fn_800D9D68`
(`GSgfx_SetScissor`) path without broadening the chosen entry point.

## Inspected Code

### Host GX scissor path

Relevant host code:

- `src/pcport/gx_shim.c`
  - `GXSetViewport`
  - `GXSetScissor`

The host shim uses top-left GX coordinates and converts them to bottom-left OpenGL
coordinates:

```c
y = fbHeight - (int)(yOrig + ht);
if (y < 0) {
    y = 0;
}
glScissor((int)xOrig, y, (int)wd, (int)ht);
```

`GXSetViewport` uses the same Y-flip rule, which keeps viewport and scissor aligned
in the tested path.

### Game-owned retry path

Relevant game-owned code:

- `src/game/gs_render.c`
  - `fn_800D9D68`
  - `GSgfx_BeginFrame`

Current `PCPORT` scissor helper:

```c
void fn_800D9D68(u32 x1, u32 y1, u32 x2, u32 y2) {
    GXSetScissor(x1, y1, (x2 - x1) + 1, (y2 - y1) + 1);
}
```

The known-good visible baseline in `GSgfx_BeginFrame` now explicitly owns:

- viewport
- projection
- model matrix
- cull mode
- blend mode
- Z mode

That explicit viewport + Z ownership was required earlier to restore a stable
visible baseline before retrying scissor.

## Host-Only Scissor Diagnostic

Diagnostic entry point:

- `src/pcport/pcport_main.c:RunGXScissorSmoke`

Diagnostic behavior:

1. Clear the backbuffer to black.
2. Load the same identity GX state used by the primitive smoke.
3. Apply a centered scissor rect with `GXSetScissor(160, 120, 320, 240)`.
4. Submit a full-screen GX quad through the normal GX immediate path.
5. Read one pixel at the center of the scissor region.
6. Read one pixel outside the scissor region.

Command:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gx-scissor-smoke
```

Result:

```text
[pcport_bootstrap] Host GX scissor smoke passed (inside=32,160,240,255 outside=0,0,0,255)
```

Conclusion:

- host `GXSetScissor` works for the tested centered-rect path
- no `gx_shim.c` fix was required to make scissor behave correctly

## Visible Baseline Regression

Before retrying game-owned scissor, the visible flat-quad regression was rerun to
confirm the baseline still held.

Command:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

Result:

```text
[pcport_bootstrap] GSgfx_BeginFrame visible attempt passed (rgba=48,213,94,255)
```

This proves the known-good game-owned `GSgfx_BeginFrame` flat-quad path remained
intact before the scissor retry.

## Game-Owned Scissor Retry

Retry entry point:

- `src/pcport/pcport_main.c:RunGSgfxScissorRetry`

Retry behavior:

1. Run `GSgfxInit`.
2. Run one retrace.
3. Clear the backbuffer to black.
4. Call game-owned `fn_800D9D68(160, 120, 479, 359)`.
5. Call game-owned `GSgfx_BeginFrame()`.
6. Read one pixel inside the scissor region.
7. Read one pixel outside the scissor region.

Command:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

Result:

```text
[pcport_bootstrap] Game-owned fn_800D9D68 retry passed (inside=48,213,94,255 outside=0,0,0,255)
```

Conclusion:

- the retry produced a richer visible game-owned result than the full-screen quad
- visible pixels now come from the existing game-owned `GSgfx_BeginFrame` path,
  clipped by the game-owned `fn_800D9D68` helper
- the richer slice is still narrow: it is a centered clipped panel, not a title,
  menu, or broader game scene

## Additional Regression Check

Command:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
```

Result:

```text
[pcport_bootstrap] GSgfx smoke path completed (3 swaps, 3 pre-retrace callbacks)
```

This confirms the earlier callback/swap path was preserved while retrying scissor.

## What Was Fixed

What required a fix in this task:

- nothing in `src/pcport/gx_shim.c`; host scissor passed as implemented

What mattered to keep the retry honest and stable:

- preserving the known-good visible baseline through explicit viewport and Z-state
  ownership in the current `PCPORT` `GSgfx_BeginFrame` subset
- retrying the same game-owned entry point instead of replacing it with host-only
  drawing

## Remaining Blockers

The next blockers to a first title/menu/game slice are still:

- the current visible result is only the narrow `PCPORT` subset of
  `GSgfx_BeginFrame`
- there is still no broader integrated `gs_render.c` draw-command path
- TEV/material/texture/light behavior in the host GX layer is still partial or
  stubbed
- there is still no engine-owned boot path driving a title/menu/game state
