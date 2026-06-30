# PC TU Integration 3

## Candidate ranking

I ranked the best next-step candidates for a "slightly harder than utility"
bridge attempt as follows:

1. `src/dolphin/os/OSStateFlags.c`
2. `src/dolphin/dvd/DVDState.c`
3. `src/trk/udp_cc.c`
4. `src/dolphin/os/OSArena.c`

## Why they ranked this way

### 1. `src/dolphin/os/OSStateFlags.c`

Best fit for the next gradient increase:

- no asm in the TU itself
- slightly more connected than `TRKUtil.c`
- has internal global state plus real OS-layer dependencies
- depends on symbols already satisfied by the current native bridge:
  - `OSDisableInterrupts`
  - `OSRestoreInterrupts`
  - `DCFlushRange`
- safe runtime path exists without touching unsafe MMIO:
  - `__OSSetAppType`
  - `__OSGetAppType`

### 2. `src/dolphin/dvd/DVDState.c`

Good conceptual gradient increase, but not the best first choice:

- still small enough to be interesting
- brings in DVD types and interrupt behavior
- safe runtime path is possible if avoiding `__DVDCheckCoverState`
- however, isolated host compile failed under MSVC because
  `include/dolphin/dvd/dvd.h` contains a zero-sized array pattern that MSVC
  rejects in `DVDFileInfo`

That made it a valid blocker candidate, but not the highest-confidence path for
this task.

### 3. `src/trk/udp_cc.c`

- very easy to compile
- still narrow and safe
- but not much of a dependency increase beyond `TRKUtil.c`
- `#pragma weak` also makes it less attractive as the next portability step

### 4. `src/dolphin/os/OSArena.c`

- tiny and decompiled
- but it conflicts directly with arena ownership already implemented in the
  host OS shim (`OSGetArenaHi`, `OSGetArenaLo`, `OSSetArenaHi`, `OSSetArenaLo`)
- integrating it honestly would mean reconciling duplicated OS responsibilities
  instead of testing bridge scalability

## Chosen TU

`src/dolphin/os/OSStateFlags.c`

## Why it was chosen

`OSStateFlags.c` was the best one-step increase over `TRKUtil.c` because it
adds real SDK-style coupling without pulling in gameplay or unsafe runtime
requirements.

Compared with the existing bridged TUs:

- more connected than `src/trk/TRKUtil.c` because it uses:
  - OS interrupt wrappers
  - cache flush API
  - internal module state
- still much safer than broad runtime/gameplay modules because:
  - no assets are needed
  - no rendering/audio/input is needed
  - a safe exercised path exists that does not touch the TU's MMIO-based
    functions

## What changed

Only two scoped changes were needed for this step:

1. Added `src/dolphin/os/OSStateFlags.c` to the `pcport_bootstrap` target in
   `CMakeLists.txt`
2. Added a small runtime verification path to `src/pcport/pcport_main.c` that:
   - calls `__OSSetAppType(0x12345678UL)`
   - calls `__OSGetAppType()`
   - verifies the returned value matches

No new shim wrappers or header compatibility edits were needed in this step.

## Exact steps attempted

1. Ranked candidate TUs by dependency complexity and safety.

2. Compiled `OSStateFlags.c` in isolation:

   ```powershell
   cmd /c "call `"$vc`" >nul && cl /nologo /c /I include /I include\dolphin /I include\game /I include\hsd /std:c11 src\dolphin\os\OSStateFlags.c /FoNUL"
   ```

3. Also tested `DVDState.c` in isolation and observed the blocker:

   - `include/dolphin/dvd/dvd.h(45): error C2229: struct 'DVDFileInfo' has an illegal zero-sized array`

4. Added `OSStateFlags.c` to the native bootstrap target.

5. Added a safe runtime verification path in `pcport_main.c`.

6. Rebuilt:

   ```powershell
   cmd /c "call `"$vc`" >nul && `"$cm`" --build build_pcbootstrap --target pcport_bootstrap -v"
   ```

7. Ran:

   ```powershell
   .\pcport_bootstrap.exe
   ```

## What compiled

Verified:

- `src/dolphin/os/OSStateFlags.c` compiles on the host toolchain
- full native bootstrap build still compiles with:
  - `src/dolphin/vi/VI.c`
  - `src/trk/TRKUtil.c`
  - `src/dolphin/os/OSStateFlags.c`

Observed compile warnings for `OSStateFlags.c`:

- MSVC warned about 32-bit integer to pointer casts for the TU's MMIO-style
  addresses

These were warnings only, not errors, and the exercised runtime path avoided
those functions entirely.

## What linked

Verified:

- `OSStateFlags.c` linked successfully into `pcport_bootstrap.exe`

## What ran

Verified at runtime:

- `pcport_bootstrap.exe` still starts and exits cleanly
- previously bridged TU checks still pass:
  - `VIGetTvFormat`
  - `TRKUtil`
- new bridged TU check passes:
  - `__OSSetAppType(0x12345678UL)`
  - `__OSGetAppType()` returned `0x12345678`

Observed runtime output included:

- `Linked decomp TU VIGetTvFormat verified (mode=1)`
- `Linked decomp TU TRKUtil verified (bridge:trk, len=10)`
- `Linked decomp TU OSStateFlags verified (appType=0x12345678)`

## Exact blockers

The chosen integration succeeded.

Concrete blocker found while ranking candidates:

- `src/dolphin/dvd/DVDState.c` is not yet a high-confidence next step under
  MSVC because `include/dolphin/dvd/dvd.h` uses a zero-sized array layout that
  MSVC rejects

Known non-blocking caveat for the chosen TU:

- `OSStateFlags.c` still contains MMIO-style code paths and pointer-cast
  warnings, but the exercised path in this task avoided those functions

## What remains unproven

- no gameplay runtime exists
- no assets are loaded
- no real rendering/audio/input behavior is established
- this does not prove broader OS/DVD/game modules will integrate cheaply
- this does not validate the TU's MMIO-touching functions on host

## Scalability assessment

Yes, the bridge still appears scalable one step up the dependency ladder.

Why:

- the native bridge now spans:
  - a narrow SDK utility TU (`VI.c`)
  - a pure TRK utility TU (`TRKUtil.c`)
  - a slightly more connected OS TU with internal state and OS dependencies
    (`OSStateFlags.c`)
- the incremental cost of adding `OSStateFlags.c` was still small
- no new shim work was required for this step

The next honest step should stay near this size class:

- small Dolphin OS/DVD/TRK TUs with safe exercised paths
- avoid large game modules
- avoid any runtime path that requires assets, GX rendering, or controller/audio
  integration
