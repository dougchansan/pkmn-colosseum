#include "dolphin/os/PPCArch.h"

/*
 * PPCArch.c - PowerPC architecture-specific register access functions.
 *
 * These are single-instruction inline assembly wrappers for accessing
 * PowerPC special-purpose registers (SPRs) and other privileged operations.
 *
 * Matches: 0x80097FFC - 0x80098110
 */

/* 0x80097FFC - mfmsr r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 PPCMfmsr(void) {
    nofralloc
    mfmsr r3
    blr
}
#pragma pop

/* 0x80098004 - mtmsr r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtmsr(register u32 val) {
    nofralloc
    mtmsr r3
    blr
}
#pragma pop

/* 0x8009800C - mfspr r3, HID0; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 PPCMfhid0(void) {
    nofralloc
    mfspr r3, HID0
    blr
}
#pragma pop

/* 0x80098014 - mtspr HID0, r3; blr  (unnamed: PPCMthid0) */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMthid0(register u32 val) {
    nofralloc
    mtspr HID0, r3
    blr
}
#pragma pop

/* 0x8009801C - mfspr r3, L2CR; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 PPCMfl2cr(void) {
    nofralloc
    mfspr r3, L2CR
    blr
}
#pragma pop

/* 0x80098024 - mtspr L2CR, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtl2cr(register u32 val) {
    nofralloc
    mtspr L2CR, r3
    blr
}
#pragma pop

/* 0x8009802C - mtdec r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtdec(register u32 val) {
    nofralloc
    mtdec r3
    blr
}
#pragma pop

/* 0x80098034 - sc; blr (PPCSync - system call) */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCSync(void) {
    nofralloc
    sc
    blr
}
#pragma pop

/* 0x8009803C - sync; loop forever (PPCHalt) */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCHalt(void) {
    nofralloc
    sync
_loop:
    nop
    li r3, 0
    nop
    b _loop
}
#pragma pop

/* 0x80098050 - mtspr MMCR0, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtmmcr0(register u32 val) {
    nofralloc
    mtspr MMCR0, r3
    blr
}
#pragma pop

/* 0x80098058 - mtspr MMCR1, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtmmcr1(register u32 val) {
    nofralloc
    mtspr MMCR1, r3
    blr
}
#pragma pop

/* 0x80098060 - mtspr PMC1, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtpmc1(register u32 val) {
    nofralloc
    mtspr PMC1, r3
    blr
}
#pragma pop

/* 0x80098068 - mtspr PMC2, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtpmc2(register u32 val) {
    nofralloc
    mtspr PMC2, r3
    blr
}
#pragma pop

/* 0x80098070 - mtspr PMC3, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtpmc3(register u32 val) {
    nofralloc
    mtspr PMC3, r3
    blr
}
#pragma pop

/* 0x80098078 - mtspr PMC4, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtpmc4(register u32 val) {
    nofralloc
    mtspr PMC4, r3
    blr
}
#pragma pop

/*
 * 0x80098080 - PPCMffpscr
 * Read FPSCR: save f31, mffs f31, store to stack, load low word, restore f31
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 PPCMffpscr(void) {
    nofralloc
    stwu r1, -0x18(r1)
    stfd f31, 0x10(r1)
    mffs f31
    stfd f31, 0x8(r1)
    lwz r3, 0xC(r1)
    lfd f31, 0x10(r1)
    addi r1, r1, 0x18
    blr
}
#pragma pop

/*
 * 0x800980A0 - PPCMtfpscr
 * Write FPSCR: save f31, build double on stack from (0, val), mtfsf 255, f31, restore
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtfpscr(register u32 val) {
    nofralloc
    stwu r1, -0x20(r1)
    stfd f31, 0x18(r1)
    li r4, 0
    stw r4, 0x10(r1)
    stw r3, 0x14(r1)
    lfd f31, 0x10(r1)
    mtfsf 0xFF, f31
    lfd f31, 0x18(r1)
    addi r1, r1, 0x20
    blr
}
#pragma pop

/* 0x800980C8 - mfspr r3, HID2; blr */
/* HID2 is SPR 920 on Gekko; CW's built-in HID2 name maps to a different SPR */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, 920
    blr
}
#pragma pop

/* 0x800980D0 - mtspr HID2, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMthid2(register u32 val) {
    nofralloc
    mtspr 920, r3
    blr
}
#pragma pop

/* 0x800980D8 - mtspr WPAR, r3; blr */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCMtwpar(register u32 val) {
    nofralloc
    mtspr WPAR, r3
    blr
}
#pragma pop

/*
 * 0x800980E0 - PPCDisableSpeculation
 * Read HID0, set bit 0x200 (SPD), write back
 */
void PPCDisableSpeculation(void) {
    u32 hid0 = PPCMfhid0();
    hid0 |= 0x00000200;
    PPCMthid0(hid0);
}

/*
 * 0x80098108 - PPCSetFpNonIEEEMode
 * Set FPSCR[NI] bit (bit 29, cr7gt position)
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCSetFpNonIEEEMode(void) {
    nofralloc
    mtfsb1 29
    blr
}
#pragma pop
