# MetroTRK FPSCR access helpers in Pokémon Colosseum (GC6E01)

`ReadFPSCR` and `WriteFPSCR` sit in the `trk/TRKInterrupt.c` unit
(.text 0x800C0EAC–0x800C1300). They are MetroTRK's `ReadFPSCR` and
`WriteFPSCR` from `Processor/ppc/Generic/targimpl.c`, called by
`TRKPPCAccessFPRegister` (in this tree `TRKTarget_range_800C1348.c`, which calls
them for FPR index 0x20). The project symbol map keeps their address names.
Only the two routines below are admitted. Each body uses the exact retail
mnemonics and matches the retail bytes in objdiff.

Common origin: Metrowerks MetroTRK for Dolphin (`TRK_MINNOW_DOLPHIN`),
`targimpl.c`, linked into Colosseum's main.dol from the CodeWarrior debugger
library.

## ReadFPSCR

Retail address 0x800C11F4, size 0x24 (upstream `ReadFPSCR`).

- Why it cannot be C: it builds its own 0x40-byte frame and preserves f31 both as a double (`stfd`) and as a paired single (`psq_st`/`psq_l` with GQR 0), then reads the FPSCR with `mffs` into f31 and stores it through r3. MWCC emits `mffs` only through hand-written asm, and never saves f31 with a paired-single store in a function whose C body would not use f31.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L1116) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L1116) keep `ReadFPSCR` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## WriteFPSCR

Retail address 0x800C1218, size 0x24 (upstream `WriteFPSCR`).

- Why it cannot be C: the inverse of `ReadFPSCR`: it preserves f31 with `stfd` and the paired-single `psq_st`, loads the saved value through r3 and writes the whole FPSCR with `mtfsf 0xff, f31`, then restores f31 with `psq_l`/`lfd`. MWCC emits `mtfsf` only through hand-written asm.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L1131) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L1131) keep `WriteFPSCR` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
