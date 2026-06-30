# PC Bootstrap Feasibility

## Conclusion

Yes, a native executable can exist in this repository right now, but only as a
stub bootstrap target.

It does **not** prove the game is playable, that the decompiled game code runs
on PC, or that rendering/audio/input are implemented. It only proves that the
native CMake path can configure, build, link, and execute a process that calls
the current PC-port shim stubs.

## Current scaffold inspection

Repository-backed observations:

- `CMakeLists.txt` originally built `pcport_shim` only.
- The intended main executable section was still commented out.
- `GAME_SOURCES` and `HSD_SOURCES` are still empty, so no decompiled game code
  is part of the native build yet.
- `src/pcport/*.c` mostly contains stub implementations guarded for host builds.
- `third_party/glad/` does not exist.
- SDL2 and SDL2_mixer are optional in the current CMake logic, but the comments
  still describe them as future work for real input/audio.

## Exact blockers found

### Blockers before changes

1. No executable target existed.
   - The native path had no `add_executable(...)`, so there was nothing to run.

2. The current shell environment did not expose host build tools on `PATH`.
   - `cmake`, `cl`, `gcc`, and `clang` were all missing from `PATH`.
   - Visual Studio was installed, but native configure only worked after
     explicitly calling `vcvars64.bat` and invoking the bundled `cmake.exe`.

3. The shim library did not fully compile as checked in.
   - `src/pcport/audio_shim.h` used `s16` in public prototypes without defining
     it.
   - This caused `pcport_shim` to fail at compile time under MSVC.

### Remaining blockers after changes

These still prevent a meaningful game bootstrap:

1. No game code is linked into the native build.
   - `GAME_SOURCES` and `HSD_SOURCES` remain empty.

2. The runtime is still stub-only.
   - `GXInit`, `DVDInit_PC`, `JAudio_Init`, and PAD functions initialize stub
     state and print messages, but they do not implement real rendering,
     asset loading, controller input, or audio output.

3. GLAD is still missing.
   - This is not a blocker for the current start-and-exit bootstrap because no
     OpenGL loader calls are made.
   - It remains a blocker for any real GL-backed rendering path.

4. SDL2 and SDL2_mixer are still missing in the tested environment.
   - This is not a blocker for the current stub bootstrap because the audio and
     input shims compile as stubs without real SDL calls.
   - It remains a blocker for real controller/audio behavior.

## Minimum concrete work required

The smallest honest bootstrap required only:

1. Fix the shim compile break in `src/pcport/audio_shim.h`.
2. Add one minimal entry point that initializes the existing stub subsystems and
   exits.
3. Add one executable target in `CMakeLists.txt` that links only `pcport_shim`.

No game code was added. No attempt was made to start a game loop, load assets,
or claim playability.

## Changes made

- Added `typedef signed short s16;` to `src/pcport/audio_shim.h`
- Added `src/pcport/pcport_main.c`
- Added `pcport_bootstrap` executable target to `CMakeLists.txt`

## Exact steps attempted

1. Verified tool availability in the shell:

   ```powershell
   cmake --version
   where.exe cmake
   where.exe cl
   ```

2. Located the Visual Studio toolchain explicitly with `vswhere.exe`.

3. Configured the native build in a separate directory:

   ```powershell
   $vc='C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat'
   $cm='C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
   cmd /c "call `"$vc`" >nul && `"$cm`" -S . -B build_pcbootstrap -G Ninja"
   ```

4. Built the shim library and captured the failure in `audio_shim.h`.

5. Applied the narrow header fix and added the bootstrap executable target.

6. Built the executable:

   ```powershell
   cmd /c "call `"$vc`" >nul && `"$cm`" --build build_pcbootstrap --target pcport_bootstrap -v"
   ```

7. Ran the executable:

   ```powershell
   .\pcport_bootstrap.exe
   ```

## Results

Verified:

- CMake configure succeeds when run from the Visual Studio toolchain
  environment.
- `pcport_shim` now builds successfully.
- `pcport_bootstrap.exe` builds successfully.
- `pcport_bootstrap.exe` runs successfully and exits with code 0.

Observed runtime output:

- OS arena stub initialized
- PAD stub initialized
- audio stub initialized
- DVD stub initialized
- GX stub initialized
- bootstrap exited cleanly

## What this proves

- The repository can now produce a native Windows executable from the PC-port
  scaffold.
- The existing shim stubs can be linked together and exercised in-process.
- The minimal bootstrap path is real, not hypothetical.

## What this does not prove

- It does not prove any part of the game is playable.
- It does not prove the decompiled game code can be linked natively.
- It does not prove the renderer works.
- It does not prove assets can be loaded from ISO or extracted files.
- It does not prove SDL input or audio output works.

## Next smallest milestone

The next smallest honest milestone is:

- create a stub window/bootstrap mode that explicitly initializes GLFW and
  opens a window, while still not starting game code

After that, the next prerequisite for meaningful progress is:

- add exactly one narrow decompiled translation unit to `GAME_SOURCES` only if
  it can compile against the shim headers without broad porting work
