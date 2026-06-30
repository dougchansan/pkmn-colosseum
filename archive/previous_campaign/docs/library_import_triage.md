# Library-Import Triage — what's actually importable (reality-checked)

**Date:** 2026-05-28 20:04. **Author:** opus48 pivot session.
**Supersedes the optimism in `library_import_plan.md` §4** with measured data.

> **Bottom line:** the genuinely *push-button* tier (MSL string/mem byte-at-a-time
> loops) is **already done** (string.c 4/5, mem.c 7/10 by the parallel work). What
> remains in the library zone is **medium-to-hard per file** — intricate runtime
> helpers, large SDK state machines, or register-soup stubs of uncertain identity.
> There is **no large remaining pool of trivial imports.** Prioritize accordingly.

Reference clones (exact source — WebFetch only paraphrases, unusable for byte match):
- `tools/decomp_work/refs/dolsdk2001/` ✅ cloned (Dolphin SDK OS/DVD/VI/EXI/SI source + headers)
- tww MSL_C — **still needs cloning** for the remaining MSL/Runtime functions.

## Stuck-function census (measured 2026-05-28)

| File | active asm | fn_ stubs | total bytes | class |
|---|---|---|---|---|
| crt/math_longlong.c | 0 | 7 | 0x608 | **B** runtime 64-bit helpers (intricate) |
| crt/mwtrace.c | 0 | 3 | 0x58 | **A** tiny interrupt-wrapper/no-op |
| crt/exit.c | 0 | 4 | 0x524 | B MSL abort_exit |
| crt/extras.c | 0 | 39 | 0x3CBC | C/B mixed grab-bag (12 tiny wrappers + huge 0xE54/0x830) |
| crt/printf.c | 0 | 6 | 0xCB8 | C large MSL printf |
| crt/stdio.c | 0 | 5 | 0x203C | C large MSL file_io |
| crt/strtoul.c | 0 | 4 | 0x648 | B — **MISLABELED** (these are stdio FILE fns, not strtoul) |
| dolphin/db/DB.c | 0 | 2 | 0x1C | A tiny debug |
| dolphin/dvd/DVDFs.c | 0 | 10 | 0x96C | B dolsdk dvdfs |
| dolphin/dvd/DVDLow.c | 11 | 25 | 0xB60 | B/C dolsdk dvdlow (large) |
| dolphin/exi/EXI2.c | 6 | 17 | 0x6EC | B dolsdk OSUartExi/EXI |
| dolphin/exi/EXIBios.c | 0 | 2 | 0x10C | B |
| dolphin/gx/GXInit.c | 0 | 156 | 0x7C9C | C huge — GX init, partly game-tuned |
| dolphin/os/OSInterrupt.c | 0 | 1 | 0x344 | B large handler |
| dolphin/os/OSThread.c | 8 | 22 | — | B/C dolsdk OSThread |
| dolphin/os/OSTime.c | 0 | 2 | 0x3A0 | B — calendar; **64-bit modulo + runtime clock** (attempted, ~0.6%, see below) |
| dolphin/os/OSSram.c | 0 | 2 | 0x2020 | C large |
| dolphin/pad/Pad.c | 0 | 74 | 0xB804 | C huge controller |
| dolphin/si/SI.c | 0 | 11 | 0x1650 | B/C dolsdk SI |
| dolphin/vi/VIFull.c | 16 | 40 | 0x2D8C | C large VI config |
| (others: OSException, OSInit, OSMemory, OSReboot) | few | few | small | B |

Class key: **A** = small + plausibly quick; **B** = importable with struct/SDA/version
work; **C** = large / game-tuned / low ROI.

## Prioritized worklist (realistic)

### P1 — foundational + small
1. **crt/math_longlong.c** (7 fns: `__div2i`/`__mod2i`/`__div2u`/`__mod2u`/shifts).
   CW Runtime, byte-stable, and **foundational** — OSTime/many fns call
   fn_800C4928/fn_800C4B44. Intricate bit-by-bit 64-bit divide loops; needs the
   exact CW Runtime source (tww `Runtime` or `__div2i.c`). Cracking these unblocks
   downstream 64-bit-arithmetic functions.
2. **crt/mwtrace.c** (3 tiny) + **dolphin/db/DB.c** (2 tiny). Likely trivial
   (interrupt save/restore wrappers; one is an empty `blr`). Quick coverage.

### P2 — SDK state machines (dolsdk2001 source in hand)
3. **EXIBios.c (2)**, **DVDFs.c (10)**, **EXI2.c (23)**, **SI.c (11)** — map to
   dolsdk `OSUartExi.c`/`OSExi.c`/`dvdfs.c`/`SI*`. Medium: struct + SDA verify,
   per-file CW version sweep.
4. **OSInit/OSMemory/OSReboot/OSException/OSInterrupt** — small-count dolsdk OS.

### P3 — resolve identity first, then decide
5. **crt/extras.c (39)** — grab-bag; the ~12 tiny 0x20 ones are thin forwarding
   wrappers, the 0xE54/0x830 ones are big runtime. Identify the library (looks like
   C++ runtime/locale/iostream support) before investing.
6. **crt/strtoul.c** — RENAME mentally: its stubs are stdio FILE functions
   (`__flush_buffer`, FILE-struct offsets), NOT strtoul. Map against MSL file_io.

### P4 — large, low ROI (defer)
GXInit.c (156), Pad.c (74), VIFull.c (40), OSThread.c, DVDLow.c, printf.c, stdio.c,
OSSram.c. Big and/or game-tuned; import only opportunistically.

## Hard-won caveats (from attempts this session)
- **OSTime `OSTicksToCalendarTime`**: imported genuine dolsdk source + all constants
  (BIAS=0xB2575 etc.), swept all CW versions → all ~0.6% (156-172 instr vs target
  103). The 64-bit `ticks % OS_SEC_TO_TICKS(1)` + runtime clock (`*(u32*)0x800000F8`)
  generate a totally different shape than the target's magic-constant calendar math.
  **OSTime is gated on math_longlong (P1) being matched first** + figuring out the
  clock representation. Not a version-sweep win.
- **__cvt_fp2unsigned** (global_destructor_chain.c, 19%): finicky `fcmpo`/`cror`
  NaN codegen — needs exact CW Runtime source, not hand-derivation.
- Don't trust split filenames for identity (strtoul.c case). ID by behavior.

## CRITICAL UPDATE (2026-05-28, after cloning tww) — much of "CRT" is ASM, not C

Cloned tww `src/PowerPC_EABI_Support/` (sparse) →
`tools/decomp_work/refs/tww/`. Inspected `Runtime/Src/runtime.c`. The CW Runtime
helpers are **pure assembly** (`ASM void __div2i(void) { nofralloc ... }`), NOT C:
- `__div2u __div2i __mod2u __mod2i __shl2i __shr2u __shr2i __cvt_dbl_usll`
  **= the entirety of `crt/math_longlong.c`** → ASM-origin, **NOT active-C-importable.**
- `__cvt_fp2unsigned` (the global_destructor_chain.c stuck fn) → also `ASM`.
- save/restore FPR/GPR helpers → ASM.

**Implication:** these can only be matched as asm blocks (the genuine source *is*
asm) — which the project's active-C goal avoids, and which overlaps the
asm-wrapper-fraud risk ([[feedback_asm_wrapper_revert_fraud]]). They are NOT a
clean import. **P1 (math_longlong) is struck.** OSTime stays blocked (its 64-bit
modulo calls these asm helpers and the codegen won't reduce to matchable C).

### Revised bottom line
The active-C library-import vein is **largely mined out**:
- MSL **string/mem** byte-loops (real C) → DONE (string 4/5, mem 7/10).
- CW **Runtime** (math_longlong, __cvt_fp2unsigned, save/restore) → **ASM, not C.**
- Remaining genuine-C candidates = **dolsdk2001 SDK state machines** (DVD/EXI/SI/VI/
  OS — medium-hard, struct/SDA verify) and **large MSL stdio/printf** (hard).
- **mwtrace.c / DB.c** tiny ones may still be quick (not runtime-asm).

There is **no remaining pool of trivial active-C imports.** Realistic continuation:
pick individual dolsdk SDK functions (EXIBios/DVDFs/SI) and verify per-file, OR
accept that the demoralizing library files split into "done", "asm-only", and
"large SDK" — and redirect effort to game-specific code (no public reference, but
that's where the uncracked volume is).

## Recommended immediate action (revised)
Try the genuinely-C, small remainders first: **mwtrace.c (3)** and **DB.c (2)** for
quick coverage, then a couple **EXIBios.c/DVDFs.c** functions against dolsdk2001 to
test the SDK-import path. Do NOT spend more on math_longlong/__cvt_fp2unsigned/
OSTime — they are asm-origin or asm-gated.

---

## SDK-import path TEST RESULT (2026-05-28) — viable for structure, not push-button

Tested DVDOpen (fn_800A501C, DVDFs.c) end-to-end with exact dolsdk2001 source +
Colosseum's own dvd.h structs (DVDFileInfo/DVDCommandBlock — offsets all confirmed:
startAddr@0x30, length@0x34, callback@0x38, cb.state@0xC; DVD_STATE_END=0).

**Result:** compiles, links, and produces the **structurally-correct** sequence —
54/54 instructions, right calls (fn_800A4D28/fn_800A5268/OSReport), right control
flow. But only ~31% byte-match (best at CW 1.2.5n). The gap is **CW register
allocation**, not logic:
- Target caches `FstStart` in a callee-saved reg (`lwz r31, FstStart@sda21`) loaded
  ONCE before the `bl`, and keeps it; my build reloads it twice after the call.
- Target uses 3 saved regs (r29=path, r30=fileInfo, r31=fst); mine uses 2 + reloads
  → cascading r29/r30/r31 renumbering inflates the mismatch count.
- The `entryIsDir` dir-check branch shape also needs polish.

**Conclusion:** exact SDK source is a big head start (correct structure/logic, ~50%)
but reaching 100% still requires the **same per-function CW reg-alloc/branch polish**
that gates the hand-decomp work. The SDK-import path is NOT push-button — it's
"import for structure, then grind codegen." Only the MSL byte-loop functions were
ever truly push-button (and they're done).

**Net strategic read:** the library-import pivot's quick-win tier is exhausted.
Remaining library functions (SDK state machines) are import-assisted but still
codegen-grind to finish — comparable effort to game-specific near-misses, but at
least with a correct logic skeleton to start from. Reasonable to do opportunistically;
not a fast batch.
