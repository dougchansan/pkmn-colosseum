# GSgfx Callback Render Path

## Chosen Callback Path

Chosen path: `GSgfx_PreRetraceCallback` (`fn_800D3EC4`) from `src/game/gs_render.c`.

Why this path:
- It is the smallest honest GSgfx-owned callback that sits on the playable-slice critical path.
- It is more meaningful than the draw-done callback because it performs per-frame viewport/projection setup before present.
- It can be driven from the host retrace loop without assets, input, audio, or a broader engine boot path.

## Smallest Required Host Pieces

Dependency order used in this task:
1. `src/game/gs_gfx.c`
   Registers the GSgfx callbacks during `GSgfxInit`.
2. `src/pcport/gs_gfx_host_support.c`
   Stores the pre-retrace callback pointer and now exposes a native dispatch entry point.
3. `src/pcport/os_shim.c`
   `VIWaitForRetrace_PC` now drives the registered pre-retrace callback once per host-frame iteration.
4. `src/game/gs_render.c`
   Host build now compiles only a minimal `PCPORT` subset:
   - `GSgfx_PreRetraceCallback`
   - `GSgfx_DrawDoneCallback`
   - small verification accessors
5. `src/pcport/gx_shim.c`
   Existing host `GXSetViewport`, `GXSetProjection`, and `GXCopyDisp` paths are exercised.

## Minimum Changes Made

- `src/pcport/gs_gfx_host_support.c`
  - Added `PCPort_RunPreRetraceCallback()`.
  - Removed the placeholder definitions for `fn_800D3EC4` and `fn_800D3F50` so `gs_render.c` owns them.
- `src/pcport/os_shim.c`
  - `VIWaitForRetrace_PC()` now dispatches the registered pre-retrace callback.
- `src/game/gs_render.c`
  - Added a `PCPORT`-guarded top slice that compiles only:
    - `GSgfx_PreRetraceCallback`
    - `GSgfx_DrawDoneCallback`
    - `GSgfxHostGetPreRetraceCount`
    - `GSgfxHostGetDrawDoneFlag`
  - The callback calls `GXSetViewport` and `GXSetProjection`, then increments a verification counter.
- `src/pcport/pcport_main.c`
  - `--gsgfx-smoke` now drives `VIWaitForRetrace_PC()` before each `GSgfxSwapBuffers()`.
  - Verifies both the pre-retrace callback count and the draw-done flag.
- `CMakeLists.txt`
  - Added `src/game/gs_render.c` to `pcport_bootstrap`.

## Exact Steps Attempted

Build:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"" --build build_pcbootstrap --target pcport_bootstrap -v"
```

Run:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke
```

Regression check:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe
```

## Results

- `src/game/gs_render.c` compiled on the host toolchain.
- `pcport_bootstrap` linked with `gs_render.c`.
- The host loop drove the registered GSgfx pre-retrace callback through `VIWaitForRetrace_PC`.
- `GSgfx_PreRetraceCallback` ran three times during the smoke loop.
- The callback reached the host GX path through:
  - `GXSetViewport`
  - `GXSetProjection`
- `GSgfx_DrawDoneCallback` also ran through the registered GSgfx draw-done callback path during `GXCopyDisp`.
- The default headless bootstrap path still ran and exited cleanly.

What is honestly verified:
- one real game-owned per-frame callback path now executes from the native host loop
- that path crosses:
  - host retrace scheduler
  - `GSgfx` callback registration
  - minimal `gs_render.c`
  - host GX state application
  - host present path

## Remaining Blockers

Still not proven:
- real `GSgfx_VBlankCallback` / frame-end behavior
- broader `gs_render.c` pipeline setup beyond viewport/projection + draw-done flag
- TEV/material/texture/draw submission
- engine boot or menu/title-state logic
- any visible game-owned scene content

Critical blockers still remaining for a first visible game-owned slice:
1. A real frame-begin/render callback path from `gs_render.c`, not just pre-retrace setup.
2. More GX host behavior for the render-state and draw calls that the next `gs_render.c` slice needs.
3. The next narrow game-owned render entry point behind GSgfx, without jumping to full `main.c` boot.
