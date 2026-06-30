# GS Visible Slice

## Candidate Ranking

1. `GSgfx_BeginFrame` (`fn_800D3FA4`) in `src/game/gs_render.c`
2. Low-level vertex helpers `fn_800D67BC` / `fn_800D6680` / `fn_800D5CB8`
3. Pipeline-heavy helpers around `fn_800D76A8`

Why `GSgfx_BeginFrame` was chosen:
- It is the smallest render-facing entry point in `gs_render.c` whose role is explicitly per-frame drawing setup.
- It can own visible output without requiring assets, menu logic, or the broader engine boot path.
- The lower-level vertex helpers are smaller individually, but they are not an honest standalone path and would require inventing a caller around them.

## Chosen Path

Attempted path:

`VIWaitForRetrace_PC` -> `GSgfx_PreRetraceCallback` -> `GSgfx_BeginFrame` -> host GX immediate-mode draw -> `GSgfxSwapBuffers`

## Additional GX Support Attempted

Narrow host GX work attempted for this path:
- apply `GXSetProjection` to host GL matrix state
- apply `GXLoadPosMtxImm` to host GL matrix state
- emit immediate-mode geometry in `GXEnd`
- verify the backbuffer with `glReadPixels` before swap

No assets, textures, TEV materials, menu logic, gameplay logic, or engine boot were added.

## Exact Steps Attempted

Build:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"" --build build_pcbootstrap --target pcport_bootstrap -v"
```

Visible-slice attempt:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
```

Baseline regression after stopping at the blocker:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
.\build_pcbootstrap\pcport_bootstrap.exe
```

## Results

What succeeded:
- `src/game/gs_render.c` host-compiled with a narrow `GSgfx_BeginFrame` subset.
- `pcport_bootstrap` linked with that `gs_render.c` subset.
- The attempted per-frame path executed up to the visible-pixel check.
- The backbuffer readback path itself was verified separately in the same runtime:
  - a host-only diagnostic clear produced `hostRgba=255,0,255,255`

What failed:
- The game-owned draw path still produced `rgba=0,0,0,255` at the sampled backbuffer pixel.
- `glGetError()` remained `0x0` after the attempted game-owned draw.
- Because visible pixels were not verified, the bootstrap smoke path was restored to the prior verified `GSgfxInit` + `GSgfxSwapBuffers` path.

Exact blocker chain:
1. `GSgfx_BeginFrame` can be host-compiled and linked.
2. The native host loop can call into that game-owned path.
3. The host backbuffer readback path works.
4. The current host primitive submission attempt behind `GXBegin` / `GXEnd` does not produce observable pixels, even for a full-screen clip-space quad.
5. Therefore the next prerequisite is not more game code. It is a verified host draw-submission path.

## What This Does And Does Not Prove

Proven:
- `gs_render.c` can be moved one step further into the native build than before.
- The blocker is now narrowed to host draw submission, not window creation, swap, or backbuffer readback.

Not proven:
- No visible pixels are currently verified as being produced by game-owned code.
- No title, menu, gameplay, texture, or material slice is runnable.

## Next Smallest Milestone

The next smallest milestone is:

1. Verify one host primitive path in isolation behind the GX shim.
2. Only after that, re-run the same `GSgfx_BeginFrame` visible-slice attempt.

Concretely, the first prerequisite is a tiny host-side proof that the current `GXBegin` / `GXPosition3f32` / `GXColor4u8` / `GXEnd` implementation can rasterize one colored primitive into the backbuffer and survive `glReadPixels`.
