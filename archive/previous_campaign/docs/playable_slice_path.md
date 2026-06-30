# Playable Slice Path

## Goal

Find the shortest realistic path from the current `pcport_bootstrap` to a first
meaningful native slice, then advance exactly the first required module on that
path without claiming the game is runnable.

## Current Baseline

- `pcport_bootstrap` already builds and runs natively.
- It already links and exercises three narrow decompiled TUs:
  - `src/dolphin/vi/VI.c`
  - `src/trk/TRKUtil.c`
  - `src/dolphin/os/OSStateFlags.c`
- Before this task, the native bootstrap still had no real host window or frame
  loop.

## Evidence For The Shortest Path

The repository's real boot sequence in `src/game/main.c` is:

1. `main()` performs heap/DVD/ARAM/XFB/VI setup.
2. `GameInit()` immediately starts with `GSgfxInit` (`fn_800D39E0`), then
   initializes FIFO, sound, object pools, VI callbacks, textures, materials,
   PAD, GSthread, floor/scene, script, save/card, effects, and finally creates
   the main game loop thread.
3. `TaskVBlank()` calls into the render path each frame and uses
   `GSgfxSwapBuffers`.

`src/game/gs_gfx.c` shows that `GSgfxInit` itself already depends on the host
presentation path:

- video mode configuration
- `VIConfigure`
- `VIFlush`
- `VISetBlack`
- `GXInit`
- draw-done / pre-retrace callback registration
- framebuffer swap behavior through the GX/VI layer

That makes a real native window and frame loop the first mandatory increment.
Without it, even a narrow `GSgfxInit` attempt is still building against a
headless bootstrap.

## Proposed Shortest Path To A First Meaningful Slice

This is the shortest plausible path found in the repository:

1. `src/pcport/pcport_main.c`
   Create a real host window, GL context, event pump, and bounded frame loop.
2. `src/pcport/gx_shim.c` and `src/pcport/os_shim.c`
   Replace the current present/sync stubs used by the real boot path:
   `GXInit`, `GXCopyDisp`, `VIConfigure_PC`, `VIWaitForRetrace_PC`,
   `VISetBlack_PC`.
3. `src/game/gs_gfx.c`
   Host-compile and link `GSgfxInit`/`GSgfxSwapBuffers`, then exercise a
   minimal graphics-init path.
4. `src/game/main.c` or a narrowly carved bootstrap path that reaches the same
   early init order
   Start the first real engine-owned init sequence without yet claiming full
   gameplay.
5. `src/game/gs_thread.c` plus the smallest scheduler/task path needed by the
   chosen slice.
6. First semi-playable or visibly game-owned state
   Most likely a menu/title-adjacent slice only after the above pieces are in
   place.

## Module Buckets

### Mandatory Now

1. `src/pcport/pcport_main.c`
2. `src/pcport/gx_shim.c`
3. `src/pcport/os_shim.c`
4. `src/game/gs_gfx.c`

### Soon After

1. `src/game/main.c` early boot path
2. `src/game/gs_thread.c`
3. whichever callback/render support from `src/game/gs_render.c` is required by
   `GSgfxInit` and swap/retrace registration

### Optional / Later

1. PAD improvements beyond the current shim stub
2. audio improvements beyond the current shim stub
3. DVD/assets/filesystem work
4. title/menu/game-state modules such as `src/game/gs_title.c`
5. broader scene/script/save/card integration

## Chosen Target For This Task

`src/pcport/pcport_main.c`

### Why This Was Chosen

- It is the first module on the critical path to any native visible slice.
- It is smaller and safer than jumping directly into `gs_gfx.c` or
  `main.c`.
- It can be verified honestly today.
- It avoids pretending that the decompiled boot/runtime path is already ready.

## Changes Made

- Added an opt-in `--window-smoke` path to `pcport_bootstrap`.
- Added GLFW-based native window creation and GL context setup in
  `src/pcport/pcport_main.c`.
- Added a bounded 120-frame event/render loop that:
  - clears the backbuffer
  - swaps buffers
  - polls window events
  - touches `VIWaitForRetrace_PC()` so the future VI hook point stays visible
- Kept the previous headless bridge checks for `VI.c`, `TRKUtil.c`, and
  `OSStateFlags.c`.

No broader runtime, asset, rendering, audio, or game-loop work was added.

## Exact Steps Attempted

1. Rebuilt the native target:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"" --build build_pcbootstrap --target pcport_bootstrap -v"
```

2. Re-ran the existing headless bootstrap:

```powershell
.\pcport_bootstrap.exe
```

3. Ran the new native window smoke path:

```powershell
.\pcport_bootstrap.exe --window-smoke
```

## Results

- `pcport_bootstrap` rebuilt successfully.
- The existing headless path still ran successfully.
- `pcport_bootstrap --window-smoke`:
  - created a native window
  - created a GL context
  - ran a bounded 120-frame loop
  - exited with code 0
- The previously bridged decompiled TUs still compiled, linked, and executed in
  the same binary.

## What This Proves

- The native bridge now reaches a visible, host-owned runtime stage:
  window creation + event pump + frame loop.
- The repository now has a verified first step toward a meaningful slice that is
  actually on the boot/render path, not just another isolated utility TU.

## What This Does Not Prove

- It does not prove `GSgfxInit` works natively.
- It does not prove `GXCopyDisp`, `VIConfigure_PC`, or `VIWaitForRetrace_PC`
  are wired into the real game frame path.
- It does not prove any decompiled game-owned frame, menu, title, or gameplay
  state is runnable.
- It does not prove asset loading, real input, or real audio.

## Next Blocker On The Critical Path

The next blocker is the real GX/VI present path required by `src/game/gs_gfx.c`.

Concretely, the current blockers are:

1. `GXInit` in `src/pcport/gx_shim.c` is still a stub and does not own the GL
   loader/state path expected by the graphics subsystem.
2. `GXCopyDisp` is still a stub, so the decompiled swap path still cannot
   present through game-owned code.
3. `VIConfigure_PC`, `VIWaitForRetrace_PC`, and `VISetBlack_PC` are still stub
   placeholders.
4. `GSgfxInit` also references callback registration and state setup that still
   need enough host behavior to compile, link, and run honestly.

## Next Smallest Milestone

Advance the present/sync path just enough for `src/game/gs_gfx.c` to become the
next host-integration target:

1. make `GXCopyDisp` perform the native swap
2. make `VIConfigure_PC` and `VISetBlack_PC` affect the host window state
3. keep the scope limited to compiling/linking/exercising `GSgfxInit` or
   `GSgfxSwapBuffers`, not broader game init
