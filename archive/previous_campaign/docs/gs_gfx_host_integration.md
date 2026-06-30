# GSgfx Host Integration

## Goal

Advance the native critical path far enough that `src/game/gs_gfx.c` can be
compiled, linked, and exercised honestly from `pcport_bootstrap`, without
claiming the game is playable.

## Chosen Target

`src/game/gs_gfx.c`

This was the smallest honest next target because `src/game/main.c` reaches
`GSgfxInit` very early, and `GSgfxInit` is the first game-owned graphics module
that actually touches the host GX/VI present path:

- VI mode configuration
- VI flush / black state
- GX init
- draw-done callback registration
- pre-retrace callback registration
- viewport / scissor configuration
- default blend / Z / TEV / fog setup

## Exact Host Surface Required By GSgfx

### Required For `GSgfxInit`

From `src/game/gs_gfx.c`, the minimum host behavior needed was:

- `VIConfigure_PC`
- `VIFlush_PC`
- `VISetBlack_PC`
- `GXInit`
- `GXSetDrawDoneCallback`
- `GXSetDispCopyGamma`
- viewport/scissor state application
- enough local engine wrappers/data to satisfy:
  - `fn_8019C3C4`
  - `fn_8019CB70`
  - `fn_8019C690`
  - `fn_801C021C`
  - `fn_801C01C8`
  - `fn_80196C3C`
  - `GSmemAllocRaw`
  - `GSmemGetPtr`
  - the narrow GSrender setup wrappers it calls directly

### Required For `GSgfxSwapBuffers`

The decompiled C in this repository only toggled the XFB index. For host use,
the minimum extra step was to route that path to `GXCopyDisp` under `PCPORT` so
the game-owned swap path actually reaches the native present hook.

## Changes Made

### `src/pcport/gx_shim.c`

Implemented the minimum real host behavior required by the `GSgfx` subset:

- `GXInit`
  - detects whether a host window/context exists
  - initializes basic GL defaults when it does
  - remains safe in headless bootstrap mode
- `GXSetDrawDoneCallback`
  - stores the registered callback
- `GXSetDispCopyGamma`
  - stores the requested gamma mode
- `GXSetViewport`
  - maps to `glViewport` + `glDepthRange`
- `GXSetScissor`
  - maps to `glScissor`
- `GXSetBlendMode`
  - applies simple host blend state
- `GXSetZMode`
  - applies host depth-test state
- `GXSetCullMode`
  - applies host cull state
- `GXCopyDisp`
  - swaps the GLFW window buffers
  - optionally clears after swap
  - fires the stored draw-done callback

### `src/pcport/os_shim.c`

Implemented the minimum VI/window behavior required by the `GSgfx` subset:

- shared host-window registration
- VI mode width/height tracking
- `VIConfigure_PC`
  - reads width/height from the mode descriptor
  - updates the active display descriptor
- `VIFlush_PC`
  - resizes the host window
- `VIWaitForRetrace_PC`
  - currently just polls window events
- `VISetBlack_PC`
  - stores black-screen state

### `src/game/gs_gfx.c`

Made only host-required changes:

- fixed one 64-bit-host-unsafe pointer arithmetic block
- added `PCPORT` aliases so the TU exports the original symbol names used by
  other decompiled code
- added a `PCPORT` path in `GSgfxSwapBuffers` that reaches `GXCopyDisp`
- excluded the remaining asm-backed tail of the TU from `PCPORT` builds

This last point is important: the host build compiles and links the decompiled C
subset currently present in `gs_gfx.c`; the later Metrowerks `asm` tail is still
not host-buildable.

### `src/pcport/gs_gfx_host_support.c`

Added a narrow host-only support layer for the exact local engine wrappers and
data objects needed by `GSgfxInit`:

- display descriptor storage
- video mode tables
- debug string storage
- minimal raw GSmem handle allocator
- VI/GX wrapper entry points used by `gs_gfx.c`
- narrow GSrender setup wrappers forwarded into the host GX shim
- empty callback bodies for the registered `GSgfx` callbacks

This is intentionally a bridge layer, not a broader engine port.

### `src/pcport/pcport_main.c`

Added `--gsgfx-smoke`:

- creates the native window/context
- runs the existing bridge checks
- calls `GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0)`
- verifies `GSgfxGetFrameCount()` returns `0xFFFFFFFF`
- calls `GSgfxSwapBuffers(1)` three times

### `CMakeLists.txt`

Added:

- `src/game/gs_gfx.c`
- `src/pcport/gs_gfx_host_support.c`

to the `pcport_bootstrap` target.

## Exact Steps Attempted

1. Host-compile `gs_gfx.c` directly:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cl /nologo /std:c11 /DPCPORT=1 /D_CRT_SECURE_NO_WARNINGS /I include /I include\dolphin /I include\game /I include\hsd /I src\pcport /I build_pcbootstrap\_deps\glfw-src\include /c src\game\gs_gfx.c /FoNUL"
```

2. Build the native bootstrap target:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"" --build build_pcbootstrap --target pcport_bootstrap -v"
```

3. Re-run the baseline headless bootstrap:

```powershell
.\pcport_bootstrap.exe
```

4. Exercise the game-owned `GSgfx` path:

```powershell
.\pcport_bootstrap.exe --gsgfx-smoke
```

5. Re-run the prior window smoke path:

```powershell
.\pcport_bootstrap.exe --window-smoke
```

## Results

- `src/game/gs_gfx.c` host-compiled successfully.
- `pcport_bootstrap` linked successfully with `gs_gfx.c`.
- Baseline headless bootstrap still ran successfully.
- `--gsgfx-smoke` ran successfully and printed:
  - host GL init from `GXInit`
  - the game-owned `GSgfxInit` success message
  - completion of three `GSgfxSwapBuffers` calls
- `--window-smoke` still ran successfully.

## What Was Actually Verified

### Verified

- `gs_gfx.c` is part of the native build.
- A real game-owned graphics init path ran:
  - `GSgfxInit`
- A real game-owned graphics swap path ran:
  - `GSgfxSwapBuffers`
  - through the host `GXCopyDisp` shim path added for `PCPORT`
- `GSgfxInit` reached real host GX/VI hooks:
  - `VIConfigure_PC`
  - `VIFlush_PC`
  - `VISetBlack_PC`
  - `GXInit`
  - `GXSetDrawDoneCallback`
  - `GXSetDispCopyGamma`
  - viewport/scissor application

### Not Verified

- the full original `gs_gfx.c` TU, because the later asm-backed tail remains
  excluded under `PCPORT`
- any real render callbacks, scene rendering, draw dispatch, HSD render pass,
  texture upload, TEV-driven material rendering, or game loop ownership

## Remaining GX/VI Stubs Or Partial Implementations

Still stubbed or only partial after this task:

- `GXSetAlphaCompare`
  - state tracking only
- `GXSetFog`
  - state tracking only
- most TEV configuration beyond simple state storage
- texture upload / texture copy path
- lighting object upload path
- immediate-mode draw path
- display-list execution path
- `VIWaitForRetrace_PC`
  - event polling only, not a true retrace/timing model
- `VISetBlack_PC`
  - state only, not a full black-frame presentation policy

Also partial at the game-owned layer:

- `GSgfxSwapBuffers`
  - only the current simplified C subset plus a host-only `GXCopyDisp` bridge
- draw-done, pre-retrace, and VBlank callbacks are registered, but there is no
  real native VI scheduler invoking the full game callback chain yet

## Exact Blockers Still Remaining On The Playable-Slice Path

1. The rest of `src/game/gs_gfx.c` is still blocked by its asm-backed tail under
   host builds.
2. `src/game/gs_render.c` is still the next major dependency wall behind a real
   `GSgfx` frame path.
3. The callback chain registered by `GSgfxInit` is not yet driven by a real
   native scheduler/retrace system.
4. The host GX shim still lacks real TEV/material, texture, lighting, and draw
   execution, which blocks anything beyond init/swap.
5. `src/game/main.c` and `src/game/gs_thread.c` are still not integrated, so no
   real engine-owned boot loop or state machine exists yet.

## Next Smallest Milestone

Keep the scope on the same critical path:

1. drive one registered `GSgfx` callback path honestly from the host loop
2. integrate only the smallest needed piece of `src/game/gs_render.c`
3. stop before broader scene/title/gameplay work
