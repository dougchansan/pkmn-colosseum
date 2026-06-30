# Library Import Plan — Porting Already-Matched Reference C

**Status:** Proposal + staged proof-of-concept. **Author:** library-import agent.
**Date:** 2026-05-28. **Game:** Pokémon Colosseum (GC6E01).

> **Honesty note.** Nothing in this document claims a byte match. Imports are
> *staged for verification*. Real match% must be measured in the main tree with
> `tools/compile_check.py` + `tools/match_test.py` after the parent reviews and
> merges. The team has previously been burned by faked 100% claims — every
> staged import carries a `STAGED-IMPORT (UNVERIFIED)` banner and must be
> re-measured before being trusted.

---

## 1. The insight

Colosseum was built with **MetroWerks CodeWarrior GC 1.2.5n / 1.3** plus three
library layers that are **byte-identical across every GC title built with the
same toolchain**:

| Layer | Where in this repo | Stuck functions |
|-------|--------------------|-----------------|
| Nintendo Dolphin SDK (OS/EXI/DVD/VI/SI/...) | `src/dolphin/**` | ~119 asm wrappers across 16 files |
| MetroWerks Standard Library (MSL / CRT) | `src/crt`, `src/string`, `src/libc`, `src/runtime` | ~74 `fn_XXXX` register-soup stubs across 9 files |
| HAL sysdolphin (HSD) | `src/hsd/**` | mostly DONE — proves the method |

**Proof the method works:** every HSD module in this repo is already at 100%,
because the team cribbed from the public Melee decomp (`melee_cobj_ref.c`).
The stuck zone is *exactly* the library code they're hand-decompiling from asm —
the same functions already matched in public GC decomp projects. Port, don't
reinvent.

---

## 2. Authoritative reference sources (with URLs + license)

All are matching, CodeWarrior-for-GC decomp projects from the decomp community.
License posture: these projects ship reconstructed C of Nintendo/MetroWerks
library code under permissive/community terms (TWW, dolsdk2001, pikmin2 use MIT
on their own tooling/source). This is a port of shared SDK/CRT reconstructions,
consistent with how this repo already cribbed HSD from Melee. **Confirm the
LICENSE of each source repo before merging a batch** (note it in the commit).

### 2.1 Dolphin SDK (OS / EXI / DVD / VI / SI / AI / AR / GX / PAD / CARD)
- **doldecomp/dolsdk2001** — decomp of the 5-23-2001 Dolphin SDK libraries.
  https://github.com/doldecomp/dolsdk2001
  - `src/os/` : `OS.c OSAlarm.c OSAlloc.c OSArena.c OSCache.c OSContext.c
    OSError.c OSExi.c OSUartExi.c OSInterrupt.c OSLink.c OSMemory.c OSMessage.c
    OSMutex.c OSReset.c OSRtc.c OSThread.c OSTime.c OSTimer.c ...`
    (raw e.g. `https://raw.githubusercontent.com/doldecomp/dolsdk2001/main/src/os/OSTime.c`)
  - `src/dvd/` : `dvd.c dvdfs.c dvdlow.c dvdqueue.c fstload.c`
  - `src/vi/`  : `vi.c gpioexi.c i2c.c initphilips.c`
  - EXI lives under `os/OSExi.c` + `os/OSUartExi.c` (maps to colosseum's
    `EXI.c`/`EXIBios.c`/`EXIUart.c`).
- **zeldaret/tww** — Wind Waker decomp, has a clean `src/dolphin/` tree
  (`os/ dvd/ gx/ mtx/ ...`) built with the same CW. Good cross-check when the
  dolsdk2001 version differs slightly from colosseum's SDK revision.
  https://github.com/zeldaret/tww/tree/main/src/dolphin

### 2.2 MetroWerks Standard Library (MSL / CRT)
- **zeldaret/tww** — full MSL tree, the cleanest MSL_C source available:
  `src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src/`
  https://github.com/zeldaret/tww/tree/main/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src
  Files: `abort_exit.c alloc.c ansi_files.c arith.c buffer_io.c ctype.c
  direct_io.c errno.c file_io.c float.c mbstring.c mem.c mem_funcs.c misc_io.c
  printf.c scanf.c string.c strtoul.c wchar_io.c`
  - PPC-EABI runtime support lives at
    `src/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Src/` (`math_ppc.c`) and
    `src/PowerPC_EABI_Support/Runtime/` (runtime / `__va_arg` / global
    destructor chain / `__init_cpp_exceptions`).
- **projectPiki/pikmin2** and **NWPlayer123/PaperMario2 (TTYD)** — additional
  CW-for-GC projects carrying MSL/Runtime; use to disambiguate MSL revision.
  https://github.com/projectPiki/pikmin2 ·
  https://github.com/NWPlayer123/PaperMario2/tree/master/src/PowerPC_EABI_Support

### 2.3 Why the colosseum copies don't match today (verified by inspection)
The current `src/crt/string.c` / `mem.c` are **hand-rewritten with different
loop idioms** than the real MSL source:
- colosseum `strncmp`/`strncpy`/`memchr` use a forward `for (i = 0; i < n; i++)`
  shape; MSL uses the canonical `n++; while (--n) { ... *++p ... }` decrement
  idiom that produces the target's `bdnz`/`lbzu` codegen.
- many CRT functions are literal register transcriptions (`fn_800CA7FC`,
  `fn_800C8174`, ...) — never going to match; they ARE the MSL functions.
This is precisely the gap importing fixes.

---

## 3. Ranked worklist

Confidence reflects how byte-stable the function is across CW-for-GC titles and
how directly the reference maps. **High** = drop-in idiom port; **Med** = port +
struct/SDA verification; **Low** = large/optimized, SDA/section-sensitive, or
SDK-revision-sensitive.

### Tier 1 — HIGH confidence (MSL string/mem, simple OS)
| Target file | Function(s) | Reference | Conf |
|---|---|---|---|
| `src/crt/string.c` | **strncmp** ✅ staged | tww MSL_Common `string.c` | high |
| `src/crt/string.c` | strncpy, strcat, strncat, strcmp, strrchr, strstr, strspn, strcspn, strpbrk, strtok | tww MSL_Common `string.c` | high |
| `src/crt/mem.c` | memchr, __memrchr, memcmp, memmove | tww MSL_Common `mem.c` | high |
| `src/crt/mem.c` | __fill_mem (`mem.c`), __copy_longs_* / __copy_mem (`mem_funcs.c`) | tww MSL_Common `mem.c`/`mem_funcs.c` | med |
| `src/crt/strtoul.c` | strtoul, strtol | tww MSL_Common `strtoul.c` | high |
| `src/crt/__va_arg.c` | __va_arg | tww Runtime | high |
| `src/crt/global_destructor_chain.c` | __register_global_object, __destroy_global_chain | tww Runtime | high |
| `src/dolphin/os/OSArena.c` | OSGetArenaHi/Lo, OSSetArenaHi/Lo (the known 50%) | dolsdk2001 `OSArena.c` | high |
| `src/dolphin/os/OSTime.c` | `fn_800A27FC` = **OSTicksToCalendarTime**, `fn_800A2998` = OSCalendarTimeToTicks (+ IsLeapYear/GetYearDays/GetLeapDays/GetDates helpers) | dolsdk2001 `OSTime.c` | high |

### Tier 2 — MED confidence (OS core, DVD, VI — struct/SDA verify)
| Target file | Scope | Reference | Conf |
|---|---|---|---|
| `src/dolphin/os/OSInit.c` | OSInit, ClearArena, OSRegisterVersion, InquiryCallback | dolsdk2001 `OS.c` | med |
| `src/dolphin/os/OSInterrupt.c` | interrupt handler table / set/unset mask | dolsdk2001 `OSInterrupt.c` | med |
| `src/dolphin/os/OSAlarm.c`, `OSCache.c`, `OSMemory.c`, `OSContext.c`, `OSThread.c` | remaining asm wrappers | dolsdk2001 (same names) | med |
| `src/dolphin/dvd/DVD.c`, `DVDQueue.c`, `DVDLow.c`, `DVDFs.c` | queue/state machine | dolsdk2001 `dvd*.c` | med |
| `src/dolphin/vi/VI.c`, `VIFull.c` | VI config/retrace | dolsdk2001 `vi.c` | med |
| `src/dolphin/exi/EXI.c`, `EXIBios.c`, `EXI2.c`, `EXIUart.c` | EXI channel state | dolsdk2001 `OSExi.c`/`OSUartExi.c` | med |
| `src/dolphin/os/PPCArch.c` | PPCMfhid2/PPCMthid2 (SPR-number fixes noted in status) | dolsdk2001 `OS.c`/`OSContext.c` asm | med |

### Tier 3 — LOW confidence (large hand-optimized MSL stdio/printf)
| Target file | Scope | Reference | Conf |
|---|---|---|---|
| `src/crt/printf.c` | __pformatter (0x774), float2str (0x71C), sprintf, vsnprintf | tww MSL_Common `printf.c` | low |
| `src/crt/stdio.c` | __fwrite, __flush_buffer, __prep_buffer, fseek, fwide, __close_all | tww MSL_Common `file_io.c`/`buffer_io.c`/`direct_io.c` | low |
| `src/crt/exit.c` | exit, atexit chain | tww MSL_Common `abort_exit.c` | low |

Low ≠ impossible — these are still byte-shared; they're "low" because they're
large, SDA-heavy, and depend on the exact MSL/SDK revision. Verify the source
revision matches before investing.

---

## 4. How many of the ~62 stuck files could this plausibly resolve?

Counts from the current tree: **~119 asm wrappers in 16 `src/dolphin` files**
and **~74 `fn_XXXX` stubs in 9 `src/crt`-family files**.

| Bucket | Files | Realistic outcome |
|---|---|---|
| Tier 1 (high) | string.c, mem.c, strtoul.c, __va_arg.c, global_destructor_chain.c, OSArena.c, OSTime.c | **fully resolvable** — ~7 files |
| Tier 2 (med) | OSInit/OSInterrupt/OSAlarm/OSCache/OSMemory/OSContext/OSThread, DVD*, VI*, EXI*, PPCArch | **mostly resolvable with verification** — ~18–22 files |
| Tier 3 (low) | printf.c, stdio.c, exit.c | partial; biggest functions may stay asm |

**Realistic estimate:** this approach can plausibly *resolve or near-resolve*
**~25–30 of the ~62 stuck library files**, and unlock the large majority of the
~193 stuck library functions (119 + 74). At the project scale (8,603 functions),
that is a meaningful but bounded bump — on the order of **~1.5–2.5 absolute
match%** — with the real value being that it *clears the hardest, most
demoralizing files cheaply* and frees agent time for the genuinely
game-specific code that has no public reference.

---

## 5. Exact next steps for a human/parent to verify a batch

1. **Pick a Tier-1 function** and grab the exact reference C (raw GitHub URL).
2. **Stage it:**
   ```
   python tools/import_reference.py stage \
     --target src/crt/string.c --func strncpy \
     --ref <reference.c> \
     --source "zeldaret/tww MSL_Common/Src/string.c"
   ```
   (use `--dry-run` first to review the diff).
3. **Compile-gate:** `python tools/compile_check.py src/crt/string.c` → must be `OK`.
4. **Match-gate (the only source of truth):**
   `python tools/match_test.py strncpy` → record the real %.
5. **If <100%**, apply `docs/matching_guide.md` levers (signed/unsigned types,
   `int` vs `u32` param widths, peephole pragma, SDA section). The reference
   idiom should already be 90%+ — remaining gaps are usually type-width or
   SDA-section, not structural.
6. **Only after a real measured 100%**, remove the `STAGED-IMPORT (UNVERIFIED)`
   banner and let the parent commit. **Re-measure in the parent context** — do
   not trust a subagent's self-reported %.
7. **Never** edit `*.inc` files and **never** flip `#if 0`→`#if 1` to fake it.

---

## 6. Tooling: `tools/import_reference.py`

- `normalize` — maps reference types to project types (u8/s16/u32/...), strips
  GCC-isms/attributes, flags C99 for-decls and declaration-after-statement
  (C89), and reports externs. (`size_t`, bare `int`/`char` are deliberately
  preserved — they are load-bearing for CW codegen.)
- `stage` — replaces an existing asm wrapper / `c-def` / `fn_XXXX` stub in the
  target with the normalized reference, wrapped in an `UNVERIFIED` banner.
  Leaves preceding doc comments intact; absorbs a `#pragma push/pop` cluster for
  asm wrappers.
- **Refuses to touch `.inc` files. Never flips `#if`. Never claims a match.**
- **symdb hook:** routes externs through `tools/symdb.py` if it exists; it does
  not exist in this tree today, so externs are passed through with a
  `TODO(symdb)` note for manual type/SDA review.
