# MetroTRK cache and MSR helpers in Pokémon Colosseum (GC6E01)

Two small units in the MetroTRK block:

- `trk/TRKDispatch_range_800C0CD8` (.text 0x800C0D70–0x800C0DA8) is MetroTRK's
  `Processor/ppc/Generic/flush_cache.c`: the single routine `TRK_flush_cache`.
- `trk/TRKDispatch_candidate_800C0E60` (.text 0x800C0E60–0x800C0EAC) holds three
  routines from MetroTRK's `Processor/ppc/Generic/targimpl.c`: `__TRK_get_MSR`
  and `__TRK_set_MSR` (named `__TRK_get_MSR` and `__TRK_set_MSR` in this
  project's symbol map) and `TRK_ppc_memcpy`.

Only these routines are admitted, with the exact retail mnemonics; each matches
the retail bytes in objdiff and in the final link.

## TRK_flush_cache

Retail address 0x800C0D70, size 0x38.

- Why it cannot be C: it walks the range eight bytes at a time with the cache-block instructions `dcbst`, `dcbf` and `icbi` separated by `sync`, counts down with the record form `subic.` and ends with `isync`. MWCC never emits `dcbst`, `icbi` or `isync` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c#L8), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/flush_cache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/flush_cache.c#L8) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/flush_cache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/flush_cache.c#L3) keep `TRK_flush_cache` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## __TRK_get_MSR

Retail address 0x800C0E60, size 0x8 (upstream `__TRK_get_MSR`).

- Why it cannot be C: its whole body is `mfmsr r3; blr`. MWCC emits no MSR read from C; the vendor source writes it as a two-instruction asm function.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `.../Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L822), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L822) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L91) keep `__TRK_get_MSR` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## __TRK_set_MSR

Retail address 0x800C0E68, size 0x8 (upstream `__TRK_set_MSR`).

- Why it cannot be C: its whole body is `mtmsr r3; blr`. MWCC emits no MSR write from C; the vendor source writes it as a two-instruction asm function.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `.../Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L830), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L830) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L102) keep `__TRK_set_MSR` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRK_ppc_memcpy

Retail address 0x800C0E70, size 0x3C.

- Why it cannot be C: it copies a byte at a time while switching the MSR between the source and destination address-translation settings for every byte (`mtmsr r7` before each `lbzx`, `mtmsr r6` before each `stbx`, each followed by `sync`), then restores the original MSR saved with `mfmsr`. MWCC emits no MSR access from C, and a C loop could not keep the load and store under different translation modes.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `.../Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L838), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L838) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L160) keep `TRK_ppc_memcpy` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
