# PC TU Integration 2

## Candidate ranking

I considered these narrow decompiled TUs as the next bridge candidate:

1. `src/trk/TRKUtil.c`
2. `src/trk/udp_cc.c`
3. `src/dolphin/os/OSArena.c`
4. `src/dolphin/dvd/DVDState.c`

Why they ranked this way:

- `TRKUtil.c`
  - no asm in the TU
  - no global state
  - no external dependencies beyond `dolphin/types.h`
  - pure helper functions that are safe to call from `pcport_bootstrap`
  - host compile succeeded in isolation immediately

- `udp_cc.c`
  - also narrow and stub-like
  - but it uses `#pragma weak`, which is less attractive for portable host
    compilation than `TRKUtil.c`
  - the runtime call path is less informative than exercising pure utility
    functions

- `OSArena.c`
  - very small, but it collides conceptually and symbol-wise with the existing
    OS arena behavior already implemented in the shim layer
  - integrating it honestly would require reconciling arena ownership/state,
    which is more than needed for this step

- `DVDState.c`
  - still relatively small, but more entangled
  - depends on DVD types and interrupt behavior and touches hardware MMIO for
    cover state checks
  - not a good fit for a minimal safe bootstrap call path

## Chosen TU

`src/trk/TRKUtil.c`

## Why it was chosen

`TRKUtil.c` was the best next bridge candidate because it is the cleanest test
of whether the native bridge can scale to a second real decompiled TU without
pulling in more platform work.

It satisfies the selection criteria better than the other candidates:

- no asm in the TU itself
- minimal dependency surface
- no new shim wrappers needed
- safe to exercise at runtime with a trivial pure-data call path
- no assets, rendering, audio, input, or game loop required

## Minimum changes made

Only two scoped changes were needed:

1. Add `src/trk/TRKUtil.c` to the `pcport_bootstrap` target in `CMakeLists.txt`
2. Add a trivial verification path in `src/pcport/pcport_main.c` that calls:
   - `TRK_memcpy`
   - `TRK_strcat`
   - `TRK_strlen`

No new host shim wrappers were needed for this TU.

## Exact steps attempted

1. Compared candidate TUs by dependency surface and safety.

2. Compiled `TRKUtil.c` in isolation:

   ```powershell
   cmd /c "call `"$vc`" >nul && cl /nologo /c /I include /I include\dolphin /I include\game /I include\hsd /std:c11 src\trk\TRKUtil.c /FoNUL"
   ```

3. Added `src/trk/TRKUtil.c` to `pcport_bootstrap`.

4. Added a runtime verification path in `pcport_main.c`:
   - copy `"trk"` into a small buffer
   - build `"bridge:trk"`
   - verify `TRK_strlen(...) == 10`

5. Rebuilt the executable:

   ```powershell
   cmd /c "call `"$vc`" >nul && `"$cm`" --build build_pcbootstrap --target pcport_bootstrap -v"
   ```

6. Ran:

   ```powershell
   .\pcport_bootstrap.exe
   ```

## Results

Verified:

- `src/trk/TRKUtil.c` compiles on the host toolchain
- it links into `pcport_bootstrap.exe`
- `pcport_bootstrap.exe` runs successfully with the second TU linked in
- the runtime call path succeeded:
  - built string: `bridge:trk`
  - measured length: `10`

Observed runtime output included:

- `Linked decomp TU VIGetTvFormat verified (mode=1)`
- `Linked decomp TU TRKUtil verified (bridge:trk, len=10)`

## Blockers

No new blocker stopped this integration.

The previously known larger blockers still remain outside this task's scope:

- no native gameplay runtime
- no linked game TUs
- rendering/audio/input remain stubbed or unproven
- GLAD/SDL-backed real runtime behavior is still not established

## What this proves

- The bridge from decompiled sources into `pcport_bootstrap` is not limited to
  a single TU.
- A second narrow decompiled TU can be added with minimal incremental cost.
- The current bridge approach still appears scalable for similarly narrow,
  host-safe utility/SDK TUs.

## What this does not prove

- It does not prove large or entangled TUs will integrate cheaply.
- It does not prove gameplay, rendering, assets, or engine subsystems work
  natively.
- It does not prove that the next candidate should come from game code rather
  than narrow SDK/debug utilities.

## Next smallest milestone

The next smallest follow-up should again use the same filter:

- no asm in the TU itself
- minimal or no global state
- no hardware MMIO on the exercised path
- no dependency on assets or runtime subsystems

A reasonable next candidate class is another narrow TRK, Dolphin utility, or
single-function SDK helper TU before attempting any broader game module.
