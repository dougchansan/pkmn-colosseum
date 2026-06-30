#include "dolphin/types.h"

/*
 * runtime.c - MetroWerks CodeWarrior runtime support.
 *
 * Contains the GPR and FPR save/restore trampolines used by the compiler.
 * Each entry point saves/restores consecutive callee-saved registers
 * (r14-r31 for GPR, f14-f31 for FPR) via r11-relative offsets.
 *
 * Note: In the original binary, these functions have multiple entry points
 * (_savefpr_14 through _savefpr_31, etc.) that the compiler branches into.
 * CW 1.2.5n inline asm does not support the 'entry' directive, so
 * each function emits all 18 save/restore instructions. The linker
 * resolves the actual entry points from the symbol table.
 *
 * Matches: 0x800C470C - 0x800C483C
 */

/* __save_fpr - 0x800C470C | size: 0x4C */
/* Saves FPR f14-f31 via r11-relative stores */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __save_fpr(void) {
    nofralloc
    stfd    f14, -0x90(r11)
    stfd    f15, -0x88(r11)
    stfd    f16, -0x80(r11)
    stfd    f17, -0x78(r11)
    stfd    f18, -0x70(r11)
    stfd    f19, -0x68(r11)
    stfd    f20, -0x60(r11)
    stfd    f21, -0x58(r11)
    stfd    f22, -0x50(r11)
    stfd    f23, -0x48(r11)
    stfd    f24, -0x40(r11)
    stfd    f25, -0x38(r11)
    stfd    f26, -0x30(r11)
    stfd    f27, -0x28(r11)
    stfd    f28, -0x20(r11)
    stfd    f29, -0x18(r11)
    stfd    f30, -0x10(r11)
    stfd    f31, -0x08(r11)
    blr
}
#pragma pop

/* __restore_fpr - 0x800C4758 | size: 0x4C */
/* Restores FPR f14-f31 via r11-relative loads */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __restore_fpr(void) {
    nofralloc
    lfd     f14, -0x90(r11)
    lfd     f15, -0x88(r11)
    lfd     f16, -0x80(r11)
    lfd     f17, -0x78(r11)
    lfd     f18, -0x70(r11)
    lfd     f19, -0x68(r11)
    lfd     f20, -0x60(r11)
    lfd     f21, -0x58(r11)
    lfd     f22, -0x50(r11)
    lfd     f23, -0x48(r11)
    lfd     f24, -0x40(r11)
    lfd     f25, -0x38(r11)
    lfd     f26, -0x30(r11)
    lfd     f27, -0x28(r11)
    lfd     f28, -0x20(r11)
    lfd     f29, -0x18(r11)
    lfd     f30, -0x10(r11)
    lfd     f31, -0x08(r11)
    blr
}
#pragma pop

/* __save_gpr - 0x800C47A4 | size: 0x4C */
/* Saves GPR r14-r31 via r11-relative stores */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __save_gpr(void) {
    nofralloc
    stw     r14, -0x48(r11)
    stw     r15, -0x44(r11)
    stw     r16, -0x40(r11)
    stw     r17, -0x3C(r11)
    stw     r18, -0x38(r11)
    stw     r19, -0x34(r11)
    stw     r20, -0x30(r11)
    stw     r21, -0x2C(r11)
    stw     r22, -0x28(r11)
    stw     r23, -0x24(r11)
    stw     r24, -0x20(r11)
    stw     r25, -0x1C(r11)
    stw     r26, -0x18(r11)
    stw     r27, -0x14(r11)
    stw     r28, -0x10(r11)
    stw     r29, -0x0C(r11)
    stw     r30, -0x08(r11)
    stw     r31, -0x04(r11)
    blr
}
#pragma pop

/* __restore_gpr - 0x800C47F0 | size: 0x4C */
/* Restores GPR r14-r31 via r11-relative loads */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __restore_gpr(void) {
    nofralloc
    lwz     r14, -0x48(r11)
    lwz     r15, -0x44(r11)
    lwz     r16, -0x40(r11)
    lwz     r17, -0x3C(r11)
    lwz     r18, -0x38(r11)
    lwz     r19, -0x34(r11)
    lwz     r20, -0x30(r11)
    lwz     r21, -0x2C(r11)
    lwz     r22, -0x28(r11)
    lwz     r23, -0x24(r11)
    lwz     r24, -0x20(r11)
    lwz     r25, -0x1C(r11)
    lwz     r26, -0x18(r11)
    lwz     r27, -0x14(r11)
    lwz     r28, -0x10(r11)
    lwz     r29, -0x0C(r11)
    lwz     r30, -0x08(r11)
    lwz     r31, -0x04(r11)
    blr
}
#pragma pop
