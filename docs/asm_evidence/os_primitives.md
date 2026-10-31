# Dolphin OS (and AI) primitives in Pokémon Colosseum (GC6E01)

Small hand-written routines from the Dolphin SDK's OS library: the MSR[EE]
interrupt primitives (`OSInterrupt.c`), the reboot jump (`OSReboot.c`'s `Run`)
and the system-call vector (`OSSync.c`). Only the routines named below are
admitted. Each body uses the exact retail mnemonics and matches the retail
bytes in objdiff and in the final link. None of them branches to another
function (`__OSDBJump`'s `bla 0x60` is an absolute branch to low memory,
not to a function).

Common origin: Nintendo Dolphin SDK, `os/`, linked into Colosseum's main.dol.
The exact SDK build is not established.

## OSDisableInterrupts

Retail address 0x8009DF3C, size 0x14; unit `dolphin/sdk_candidate_8009DF3C.c`.

- Why it cannot be C: it reads and writes the machine state register (`mfmsr`/`mtmsr`) to clear MSR[EE] and returns the old EE bit, with no frame. MWCC never emits `mfmsr`/`mtmsr` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSInterrupt.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSInterrupt.c#L10) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSInterrupt.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSInterrupt.c#L59) keep `OSDisableInterrupts` as an `asm` function. Colosseum's build has no `__RAS_OSDisableInterrupts_begin`/`_end` labels, so none are emitted.
- Origin: Nintendo Dolphin SDK, `os/OSInterrupt.c`, hand-written assembly in the vendor source.

## OSEnableInterrupts

Retail address 0x8009DF50, size 0x14; unit `dolphin/sdk_candidate_8009DF3C.c`.

- Why it cannot be C: it sets MSR[EE] through `mfmsr`/`ori`/`mtmsr` and returns the old EE bit, with no frame. MWCC never emits `mfmsr`/`mtmsr` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSInterrupt.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSInterrupt.c#L31) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSInterrupt.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSInterrupt.c#L72) keep `OSEnableInterrupts` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `os/OSInterrupt.c`, hand-written assembly in the vendor source.

## OSRestoreInterrupts

Retail address 0x8009DF64, size 0x24; unit `dolphin/sdk_candidate_8009DF3C.c`.

- Why it cannot be C: it sets or clears MSR[EE] from its argument (`mfmsr`/`mtmsr`, with local branches only) and returns the old EE bit. MWCC never emits `mfmsr`/`mtmsr` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSInterrupt.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSInterrupt.c#L48) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSInterrupt.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSInterrupt.c#L85) keep `OSRestoreInterrupts` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `os/OSInterrupt.c`, hand-written assembly in the vendor source.

## Run_8009FADC

Retail address 0x8009FADC, size 0x10; unit `dolphin/sdk_candidate_8009FADC.c`.
This is `OSReboot.c`'s static `Run`: `OSResetSystem` calls it with 0x81300000
after `OSDisableInterrupts` and `ICFlashInvalidate`.

- Why it cannot be C: it synchronises (`sync`, `isync`), moves its argument into the link register (`mtlr`) and returns into it, i.e. jumps to an address without a call frame. MWCC never emits `isync` or a jump through `mtlr` of an argument from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSReboot.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSReboot.c#L25) keeps `Run` as an `asm` function with the same four instructions.
- Origin: Nintendo Dolphin SDK, `os/OSReboot.c`, hand-written assembly in the vendor source.

## SystemCallVector

Retail address 0x800A1208, size 0x20 (with the `__OSSystemCallVectorStart`
and `__OSSystemCallVectorEnd` labels); unit `dolphin/sdk_range_800A0D00.c`
via `src/dolphin/sdk_range_800A07C4.c`. This is `OSSync.c`'s
`SystemCallVector`, which `__OSInitSystemCall` copies to 0x80000C00.

- Why it cannot be C: it is exception-vector code: it toggles HID0 (`mfspr`/`mtspr HID0`), synchronises (`isync`, `sync`) and returns with `rfi`, and it exposes two `entry` labels that bound the copy. MWCC never emits `rfi`, HID0 moves or `entry` points from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSSync.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSSync.c#L7) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSSync.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSSync.c#L9) keep `SystemCallVector` as an `asm` function with the same `entry` labels.
- Origin: Nintendo Dolphin SDK, `os/OSSync.c`, hand-written assembly in the vendor source.

## __OSFPRInit

Retail address 0x80099790, size 0x128; unit `dolphin/sdk_r48_80099790_suffix.c`
via `src/dolphin/sdk_range_80098108.c` (`OS.c`).

- Why it cannot be C: it enables the FPU through the MSR (`mfmsr`/`ori`/`mtmsr`), tests HID2[PSE] (`mfspr` of SPR 920), and then zeroes all 32 floating-point registers, as paired singles (`psq_l`, `ps_mr`) when paired-single mode is on and with `lfd`/`fmr` otherwise, before clearing the FPSCR with `mtfsf`. MWCC never emits MSR or HID2 moves, paired-single instructions or writes to fixed FPRs it does not allocate from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L49) keeps `__OSFPRInit` as an `asm` function, and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L71) keeps the FPR-zeroing part (an older SDK, without the paired-single path) as the `asm` function `__OSInitFPRs`.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, hand-written assembly in the vendor source.

## __OSDBIntegrator

Retail address 0x8009A09C, size 0x24, between the `__OSDBINTSTART` and
`__OSDBINTEND` labels; unit `dolphin/sdk_r48_80099790_suffix.c` via
`src/dolphin/sdk_range_80098108.c` (`OS.c`).

- Why it cannot be C: `OSExceptionInit` copies it into low memory as the debugger integrator. It saves LR into the debugger block at 0x40 through a hard-coded base register, jumps to the debugger's handler through `mtlr` with address translation turned off by writing the MSR directly (`mtmsr`), and exposes two `entry` labels. MWCC never emits `mtmsr` or `entry` points from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L458) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L351) keep `__OSDBIntegrator` as an `asm` function with the same labels.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, hand-written assembly in the vendor source.

## __OSDBJump

Retail address 0x8009A0C0, size 0x4, ending at the `__OSDBJUMPEND` label;
unit `dolphin/sdk_r48_80099790_suffix.c` via `src/dolphin/sdk_range_80098108.c`.
This is `OS.c`'s `__OSDBJump`, which `OSExceptionInit` patches into the
exception vectors.

- Why it cannot be C: it is the single instruction `bla 0x60`, an absolute branch-and-link to the debugger integrator's low-memory copy. MWCC never emits absolute branches to a fixed address, and the routine has to be exactly one instruction between its labels.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L477) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L368) keep `__OSDBJump` as an `asm` function containing `bla 0x60`.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, hand-written assembly in the vendor source.

## __AICallbackStackSwitch

Retail address 0x800AC6D4, size 0x58; unit `dolphin/sdk_range_800AC6D4.c` via
`src/dolphin/sdk_range_800AC02C.c` (the audio interface library, `ai/ai.c`).

- Why it cannot be C: it runs the AI DMA callback on a separate stack. It saves r1 to `__OldStack`, loads r1 from `__CallbackStack` (both addressed with raw `lis`/`addi` of small-data variables rather than r13-relative), calls the callback through `mtlr`/`blrl`, then restores r1 from `__OldStack` before unwinding its own frame. MWCC never reassigns the stack pointer from C and never addresses small data with `lis`/`@l`.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/ai/ai.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/ai/ai.c#L249) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/ai/ai.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/ai/ai.c#L341) keep `__AICallbackStackSwitch` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `ai/ai.c`, hand-written assembly in the vendor source.

## Config24MB

Retail address 0x8009F54C, size 0x80 (local symbol); unit
`dolphin/os/OSMemory_privileged.c` via `src/dolphin/os/OSMemory.c`. Defined
global here so the quality scan sees the asm body.

- Why it cannot be C: it reprograms the block address translation registers for a 24 MB memory map (`mtdbatu`/`mtdbatl`/`mtibatu`/`mtibatl`, `isync`) while running in real mode, and returns to the caller's address through SRR0/SRR1 with `rfi`. MWCC never emits BAT moves, SRR moves or `rfi` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSMemory.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSMemory.c#L76) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSMemory.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSMemory.c#L54) keep `Config24MB` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `os/OSMemory.c`, hand-written assembly in the vendor source.

## Config48MB

Retail address 0x8009F5CC, size 0x80 (local symbol); unit
`dolphin/os/OSMemory_privileged.c` via `src/dolphin/os/OSMemory.c`.

- Why it cannot be C: as `Config24MB`, for a 48 MB memory map: BAT register moves, `isync`, and a return through SRR0/SRR1 with `rfi`. MWCC never emits these from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSMemory.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSMemory.c#L117) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSMemory.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSMemory.c#L91) keep `Config48MB` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `os/OSMemory.c`, hand-written assembly in the vendor source.

## RealMode

Retail address 0x8009F64C, size 0x18 (local symbol); unit
`dolphin/os/OSMemory_privileged.c` via `src/dolphin/os/OSMemory.c`.

- Why it cannot be C: it converts its argument to a physical address, clears MSR[IR|DR] into SRR1 (`mfmsr`, `mtsrr1`), puts the target in SRR0 and enters it with `rfi`, i.e. jumps to a routine with address translation off. MWCC never emits SRR moves or `rfi` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSMemory.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSMemory.c#L158) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSMemory.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSMemory.c#L128) keep `RealMode` as an `asm` function.
- Origin: Nintendo Dolphin SDK, `os/OSMemory.c`, hand-written assembly in the vendor source.
