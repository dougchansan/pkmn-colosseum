# GX Primitive Submission Debug

## Goal

Prove whether the host GX primitive submission path behind `GXBegin` / `GXEnd` can rasterize one primitive correctly, then rerun the existing `GSgfx_BeginFrame` visible-slice attempt unchanged.

## Scope

- Repository only.
- No new game-owned entry point beyond the existing `GSgfx_BeginFrame` host slice.
- No assets, gameplay, audio, or menu work.

## Inspected Code

- `src/pcport/gx_shim.c`
  - `GXBegin`
  - `GXPosition3f32`
  - `GXColor4u8`
  - `GXTexCoord2f32`
  - `GXEnd`
- `src/pcport/pcport_main.c`
  - new host-only diagnostic helpers and `--gx-primitive-smoke`
- `src/game/gs_render.c`
  - existing `PCPORT` `GSgfx_BeginFrame` path, rerun unchanged as the game-owned check

## Minimal Changes

1. `src/pcport/gx_shim.c`
   - Added a real immediate submission path in `GXEnd`.
   - `GX_QUADS` are expanded to two triangles.
   - Submission now uses `glVertexPointer` / `glColorPointer` / `glTexCoordPointer` plus `glDrawArrays`.
   - Added debug counters for last submitted primitive and vertex counts.

2. `src/pcport/pcport_main.c`
   - Added `--gx-primitive-smoke`.
   - Added a host-only diagnostic that uses the same GX APIs as the attempted game-owned path:
     - `GXBegin`
     - `GXColor4u8`
     - `GXPosition3f32`
     - `GXTexCoord2f32`
     - `GXEnd`
   - Reads back one backbuffer pixel with `glReadPixels`.
   - If the host-only GX path succeeds, immediately reruns `GSgfx_BeginFrame` unchanged and reads back the same pixel.

3. `src/game/gs_render.c`
   - Corrected the `PCPORT` `GXBegin` declaration from `u8` to `u32`.
   - This was required on Win64 so the primitive ID passed from `GSgfx_BeginFrame` matches the shim implementation width.

## Commands Attempted

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Run primitive diagnostic:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gx-primitive-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
.\build_pcbootstrap\pcport_bootstrap.exe
```

## Results

### 1. Host-only GX primitive diagnostic

Succeeded.

Observed output:

```text
[pcport_bootstrap] Host GX primitive smoke passed (rgba=32,208,64,255 submitted=4 expanded=6 prim=0x80)
```

What this proves:

- The host GX submission path can now rasterize a primitive into the backbuffer.
- `GX_QUADS` expansion is working for the tested path.
- Backbuffer readback still works.

### 2. Unchanged game-owned `GSgfx_BeginFrame` rerun

Still failed to produce visible pixels.

Observed output after the `GXBegin` declaration fix:

```text
[pcport_bootstrap] GSgfx_BeginFrame visible attempt failed (rgba=0,0,0,255 submitted=4 expanded=6 prim=0x80)
```

What this proves:

- `GSgfx_BeginFrame` now reaches the same primitive submission machinery with the correct primitive ID.
- The failure is no longer a corrupted primitive type or broken `GXEnd` submission.
- The remaining blocker is somewhere earlier in the render-state setup difference between the host-only diagnostic path and the current game-owned `GSgfx_BeginFrame` subset.

## Exact Fix Required

Two concrete fixes were required:

1. `src/pcport/gx_shim.c`
   - Replace the ineffective immediate submission path with real OpenGL draw submission in `GXEnd`, including quad-to-triangle expansion.

2. `src/game/gs_render.c`
   - Fix the `PCPORT` `GXBegin` declaration width mismatch so Win64 does not pass a corrupted primitive ID from `GSgfx_BeginFrame`.

## Current Blocker Chain

The primitive submission blocker is resolved.

The next blocker is narrower:

1. The host-only smoke path succeeds when it explicitly sets:
   - viewport
   - projection
   - modelview
   - cull mode
   - Z mode
   - blend mode
2. The current `PCPORT` `GSgfx_BeginFrame` subset only sets:
   - projection
   - modelview
   - cull mode
   - immediate quad submission
3. With submission now proven, the evidence points to missing or incorrect game-owned render-state setup before draw, not a broken `GXBegin` / `GXEnd` path.

This last point is an inference from the code and the observed run results.

## What Was Verified vs Not Verified

Verified:

- `pcport_bootstrap` rebuilt successfully.
- A host-only primitive using the same GX submission APIs rasterized successfully.
- The unchanged `GSgfx_BeginFrame` path now submits a correct quad through the same shim path.
- Existing `--gsgfx-smoke` and default bootstrap paths still pass.

Not verified:

- Any visible game-owned pixels.
- A title, menu, or broader game slice.
- That the current `GSgfx_BeginFrame` host slice contains enough render-state setup to draw visibly.

## Next Smallest Milestone

Keep `GXBegin` / `GXEnd` unchanged and isolate the minimum missing state difference on the game-owned side:

- either bring one more honest render-state call into the `GSgfx_BeginFrame` host slice,
- or align the host GX default state so the existing `GSgfx_BeginFrame` state set is sufficient.

The next debugging step should target render-state setup, not primitive submission.
