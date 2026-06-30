# PC TU Integration

## Chosen TU

`src/dolphin/vi/VI.c`

## Why this TU was chosen

I inspected the most recent or recently edited candidates first:

- `src/game/people/people_data.c`
- `src/dolphin/exi/EXI2.c`
- `src/game/gs_texture.c`
- `src/game/scene_init.c`

Those were all riskier than needed for a first bridge:

- `people_data.c` is large, game-specific, and depends on many engine globals,
  model systems, and unresolved cross-module calls.
- `EXI2.c` is smaller but still hardware-facing, contains multiple asm-wrapper
  sections, and depends on interrupt and SI behavior that is not part of the
  current bootstrap.
- `gs_texture.c` is decompiled C, but it already expects broader GX and GSmem
  behavior than the bootstrap currently proves.
- `scene_init.c` is much too large and entangled for a first native bridge.

`VI.c` was the narrowest realistic decompiled TU that still represented real
repo-backed decompilation work:

- one small function: `VIGetTvFormat`
- no inline assembly in this TU
- dependency surface limited to:
  - `CurrTvMode`
  - `OSDisableInterrupts`
  - `OSRestoreInterrupts`
- trivial and safe to exercise from `pcport_bootstrap`

## Minimum work required

To compile and link `VI.c` into the native bootstrap, the following narrowly
scoped changes were required:

1. Guard `size_t` in `include/dolphin/types.h` so host compilers do not see a
   conflicting typedef.
2. Provide thin SDK-name interrupt wrappers in `src/pcport/os_shim.c` for:
   - `OSDisableInterrupts`
   - `OSEnableInterrupts`
   - `OSRestoreInterrupts`
3. Add `src/dolphin/vi/VI.c` to `pcport_bootstrap`.
4. Define a stub `CurrTvMode` global and call `VIGetTvFormat()` from
   `src/pcport/pcport_main.c`.

No rendering, audio, input, or gameplay logic was broadened beyond that.

## Exact steps attempted

1. Inspected recent candidate TUs and compared dependency surface.

2. Tested `VI.c` in isolation with the host compiler:

   ```powershell
   cmd /c "call `"$vc`" >nul && cl /nologo /c /I include /I include\dolphin /I include\game /I include\hsd /std:c11 src\dolphin\vi\VI.c /FoNUL"
   ```

3. Hit the first blocker:
   - `include/dolphin/types.h` redefined `size_t` under MSVC

4. Added the host-only guard around `size_t`, then re-ran the isolated compile.

5. Added the decompiled TU to the bootstrap target and added thin interrupt
   wrappers in the OS shim.

6. Rebuilt the native bootstrap:

   ```powershell
   cmd /c "call `"$vc`" >nul && `"$cm`" --build build_pcbootstrap --target pcport_bootstrap -v"
   ```

7. Ran the executable:

   ```powershell
   .\pcport_bootstrap.exe
   ```

## Results

Verified:

- `src/dolphin/vi/VI.c` compiles on the host toolchain.
- It links into `pcport_bootstrap.exe`.
- `pcport_bootstrap.exe` runs successfully with the decompiled TU linked in.
- A trivial safe call path exercised the TU:
  - `CurrTvMode = 2`
  - `VIGetTvFormat()` returned `1`
  - this matches the TU's PAL mapping logic

Observed runtime output included:

- shim subsystem initialization
- `Linked decomp TU VIGetTvFormat verified (mode=1)`
- clean shutdown

## Exact blockers encountered

The integration succeeded. The concrete blockers encountered and resolved were:

1. `include/dolphin/types.h` was not host-safe because it unconditionally
   typedefed `size_t`.
2. The native shim layer did not yet export the original Dolphin SDK interrupt
   function names expected by the decompiled TU.

## What this proves

- One real decompiled translation unit can now be compiled with the host
  toolchain and linked into the native bootstrap path.
- The bridge between the decomp tree and `pcport_bootstrap` is now real, not
  hypothetical.
- The specific decompiled logic in `VIGetTvFormat` was exercised at runtime.

## What this does not prove

- It does not prove the game is playable.
- It does not prove broader game TUs are host-compilable.
- It does not prove asset loading, rendering, audio output, or controller input
  work.
- It does not prove the broader Dolphin SDK layer is ported.

## Next smallest milestone

The next smallest follow-up milestone is:

- add one more equally narrow decompiled TU with a similarly tiny dependency
  surface, preferably another single-function SDK or utility module

The next likely candidate should be chosen using the same filter:

- no asm in the TU itself
- minimal global data
- dependencies already satisfied by the shim layer or easy to wrap
