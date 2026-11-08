/**
 * MetroTRK targimpl.c MSR helpers and TRK_ppc_memcpy,
 * .text 0x800C0E60 - 0x800C0EAC.
 */
#include "dolphin/types.h"

/* __TRK_get_MSR - 0x800C0E60 | size: 0x8 (upstream __TRK_get_MSR)
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_dispatch.md */
asm u32 __TRK_get_MSR(void) {
    nofralloc
    mfmsr r3
    blr
}

/* __TRK_set_MSR - 0x800C0E68 | size: 0x8 (upstream __TRK_set_MSR)
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_dispatch.md */
asm void __TRK_set_MSR(register u32 msr) {
    nofralloc
    mtmsr r3
    blr
}

/* TRK_ppc_memcpy - 0x800C0E70 | size: 0x3C
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_dispatch.md */
asm void TRK_ppc_memcpy(register void* dest, register const void* src, register int n,
                        register u32 dstMsr, register u32 srcMsr) {
    nofralloc
    mfmsr r8
    li r10, 0
_copy_loop:
    cmpw r10, r5
    beq _copy_done
    mtmsr r7
    sync
    lbzx r9, r10, r4
    mtmsr r6
    sync
    stbx r9, r10, r3
    addi r10, r10, 1
    b _copy_loop
_copy_done:
    mtmsr r8
    sync
    blr
}
