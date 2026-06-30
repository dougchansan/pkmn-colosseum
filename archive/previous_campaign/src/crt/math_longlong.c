#include "dolphin/types.h"

/*
 * math_longlong.c - CRT library functions.
 *
 * Stub implementations for function coverage.
 */

/* __div2u - 0x800C483C | size: 0xEC
 * Unsigned 64-bit division: (r3:r4) / (r5:r6) -> quotient in r3:r4
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __div2u(void) {
    nofralloc
    cmpwi   cr0, r3, 0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     cr0, _d2u_s1
    addi    r0, r9, 32
_d2u_s1:
    cmpwi   cr0, r5, 0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     cr0, _d2u_s2
    addi    r9, r10, 32
_d2u_s2:
    subf    r10, r0, r9
    cmpwi   cr0, r10, 0
    blt     cr0, _d2u_zero
    addi    r10, r10, 1
    subfic  r9, r10, 64
    subf    r10, r10, r0
    cmpwi   cr0, r10, 32
    blt     cr0, _d2u_sm
    srw     r8, r3, r10
    li      r7, 0
    b       _d2u_sd
_d2u_sm:
    srw     r8, r4, r10
    subfic  r7, r10, 32
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r10
_d2u_sd:
    cmpwi   cr0, r9, 32
    blt     cr0, _d2u_sm2
    slw     r3, r4, r10
    li      r4, 0
    b       _d2u_ls
_d2u_sm2:
    slw     r3, r3, r9
    subfic  r10, r9, 32
    srw     r10, r4, r10
    or      r3, r3, r10
    slw     r4, r4, r9
_d2u_ls:
    li      r10, -1
    addic   r7, r7, 0
_d2u_lp:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     cr0, _d2u_ns
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 1
_d2u_ns:
    bdnz    _d2u_lp
    adde    r4, r4, r4
    adde    r3, r3, r3
    blr
_d2u_zero:
    li      r4, 0
    li      r3, 0
    blr
}
#pragma pop

/* fn_800C4928 - 0x800C4928 | size: 0x138 */
void fn_800C4928(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    /* clrrwi. r9, r3, 31 */;
    if ((s32)tmp != 0) {
        r4 = 0x0 - r4;
        /* subfze r3, r3 */;
    }
    /* clrrwi. r10, r5, 31 */;
    if ((s32)tmp != 0) {
        r6 = 0x0 - r6;
        /* subfze r5, r5 */;
    }
    tmp = __cntlzw(r3);
    r9 = __cntlzw(r4);
    if ((s32)r3 == 0) {
        tmp = r9 + 0x20;
    }
    r9 = __cntlzw(r5);
    r10 = __cntlzw(r6);
    if ((s32)r5 == 0) {
        r9 = r10 + 0x20;
    }
    r10 = 0x40 - tmp;
    if ((s32)tmp <= (s32)r9) {
        r9 = r9 + 0x1;
        r9 = 0x40 - r9;
        tmp = tmp + r9;
        r9 = r10 - r9;
        ctr_fn = (void(*)(void))r9;
        if ((s32)r9 >= 0x20) {
            r8 = (u32)r3 >> r7;
            r7 = 0x0;
        } else {

            r8 = (u32)r4 >> r9;
            r7 = 0x20 - r9;
            r7 = r3 << r7;
            r8 = r8 | r7;
            r7 = (u32)r3 >> r9;
        }
        if ((s32)tmp >= 0x20) {
            r3 = r4 << r9;
            r4 = 0x0;
        } else {

            r3 = r3 << tmp;
            r9 = 0x20 - tmp;
            r9 = (u32)r4 >> r9;
            r3 = r3 | r9;
            r4 = r4 << tmp;
        }
        r10 = -0x1;
        r7 = r7 + 0x0;
        do {
            r4 = r4 + r4; /* +carry */;
            r3 = r3 + r3; /* +carry */;
            r8 = r8 + r8; /* +carry */;
            r7 = r7 + r7; /* +carry */;
            tmp = r8 - r6;
            /* subfe. r9, r5, r7 */;
            if ((s32)tmp >= 0x20) {
                r8 = tmp;
                r7 = r9;
                tmp = r10 + 0x1;
            }
        } while (--ctr != 0);
        r4 = r4 + r4; /* +carry */;
        r3 = r3 + r3; /* +carry */;
        /* xor. r7, r9, r10 */;
        if ((s32)tmp == 0x20) return;
        r4 = 0x0 - r4;
        /* subfze r3, r3 */;
        return;
    }
    r4 = 0x0;
    r3 = 0x0;

    return;
}

/* __mod2u - 0x800C4A60 | size: 0xE4
 * Unsigned 64-bit modulo: (r3:r4) % (r5:r6) -> remainder in r3:r4
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __mod2u(void) {
    nofralloc
    cmpwi   cr0, r3, 0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     cr0, _m2u_s1
    addi    r0, r9, 32
_m2u_s1:
    cmpwi   cr0, r5, 0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     cr0, _m2u_s2
    addi    r9, r10, 32
_m2u_s2:
    subf    r10, r0, r9
    cmpwi   cr0, r10, 0
    blt     cr0, _m2u_ret
    addi    r10, r10, 1
    subfic  r9, r10, 64
    subf    r10, r10, r0
    cmpwi   cr0, r10, 32
    blt     cr0, _m2u_sm
    srw     r8, r3, r10
    li      r7, 0
    b       _m2u_sd
_m2u_sm:
    srw     r8, r4, r10
    subfic  r7, r10, 32
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r10
_m2u_sd:
    cmpwi   cr0, r9, 32
    blt     cr0, _m2u_sm2
    slw     r3, r4, r10
    li      r4, 0
    b       _m2u_ls
_m2u_sm2:
    slw     r3, r3, r9
    subfic  r10, r9, 32
    srw     r10, r4, r10
    or      r3, r3, r10
    slw     r4, r4, r9
_m2u_ls:
    li      r10, -1
    addic   r7, r7, 0
_m2u_lp:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     cr0, _m2u_ns
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 1
_m2u_ns:
    bdnz    _m2u_lp
    mr      r4, r8
    mr      r3, r7
_m2u_ret:
    blr
}
#pragma pop

/* fn_800C4B44 - 0x800C4B44 | size: 0x10C */
void fn_800C4B44(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    if ((s32)r3 < 0) {
        r4 = 0x0 - r4;
        /* subfze r3, r3 */;
    }
    if ((s32)r5 < 0) {
        r6 = 0x0 - r6;
        /* subfze r5, r5 */;
    }
    tmp = __cntlzw(r3);
    r9 = __cntlzw(r4);
    if ((s32)r3 == 0) {
        tmp = r9 + 0x20;
    }
    r9 = __cntlzw(r5);
    r10 = __cntlzw(r6);
    if ((s32)r5 == 0) {
        r9 = r10 + 0x20;
    }
    r10 = 0x40 - tmp;
    if ((s32)tmp <= (s32)r9) {
        r9 = r9 + 0x1;
        r9 = 0x40 - r9;
        tmp = tmp + r9;
        r9 = r10 - r9;
        ctr_fn = (void(*)(void))r9;
        if ((s32)r9 >= 0x20) {
            r8 = (u32)r3 >> r7;
            r7 = 0x0;
        } else {

            r8 = (u32)r4 >> r9;
            r7 = 0x20 - r9;
            r7 = r3 << r7;
            r8 = r8 | r7;
            r7 = (u32)r3 >> r9;
        }
        if ((s32)tmp >= 0x20) {
            r3 = r4 << r9;
            r4 = 0x0;
        } else {

            r3 = r3 << tmp;
            r9 = 0x20 - tmp;
            r9 = (u32)r4 >> r9;
            r3 = r3 | r9;
            r4 = r4 << tmp;
        }
        r10 = -0x1;
        r7 = r7 + 0x0;
        do {
            r4 = r4 + r4; /* +carry */;
            r3 = r3 + r3; /* +carry */;
            r8 = r8 + r8; /* +carry */;
            r7 = r7 + r7; /* +carry */;
            tmp = r8 - r6;
            /* subfe. r9, r5, r7 */;
            if ((s32)tmp >= 0x20) {
                r8 = tmp;
                r7 = r9;
                tmp = r10 + 0x1;
            }
        } while (--ctr != 0);
        r4 = r8;
        r3 = r7;
    }
    if ((s32)tmp >= (s32)0x20) return;
    r4 = 0x0 - r4;
    /* subfze r3, r3 */;
    return;
}

/* fn_800C4C50 - 0x800C4C50 | size: 0x24 */
void fn_800C4C50(void) {
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;

    r8 = 0x20 - r5;
    r3 = r3 << r5;
    r10 = (u32)r4 >> r8;
    r3 = r3 | r10;
    r10 = r4 << r9;
    r3 = r3 | r10;
    r4 = r4 << r5;
    return;
}

/* fn_800C4C74 - 0x800C4C74 | size: 0x24 */
void fn_800C4C74(void) {
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;

    r8 = 0x20 - r5;
    r4 = (u32)r4 >> r5;
    r10 = r3 << r8;
    r4 = r4 | r10;
    r10 = (u32)r3 >> r9;
    r4 = r4 | r10;
    r3 = (u32)r3 >> r5;
    return;
}

/* fn_800C4C98 - 0x800C4C98 | size: 0x28 */
void fn_800C4C98(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;

    r8 = 0x20 - r5;
    /* subic. r9, r5, 0x20 */;
    r4 = (u32)r4 >> r5;
    r10 = r3 << r8;
    r4 = r4 | r10;
    r10 = (s32)r3 >> r9;
    if ((s32)tmp > 0) {
        r4 = r4 | r10;
    }
    r3 = (s32)r3 >> r5;
    return;
}

/* fn_800C4CC0 - 0x800C4CC0 | size: 0xCC */
void fn_800C4CC0(void) {
    u8 sp[0x10];
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    f32 f1 = 0.0f;

    /* extrwi r5, r3, 11, 1 */;
    if (r5 < 0x3ff) {
        r3 = 0x0;
        r4 = 0x0;
        return;
    }
    r6 = r3;
    r3 = r3 & 0xFFFFF;
    r3 = r3 | (0x10 << 16);
    if ((s32)r5 < 0) {
        r5 = -r5;
        r8 = 0x20 - r5;
        r4 = (u32)r4 >> r5;
        r10 = r3 << r8;
        r4 = r4 | r10;
        r10 = (u32)r3 >> r9;
        r4 = r4 | r10;
        r3 = (u32)r3 >> r5;
    } else {

        /* ble+ .L_800C4D54 */;
        /* clrrwi. r6, r6, 31 */;
        if ((s32)r5 != 0xa) {
            r3 = 0x80000000;
            r4 = 0x0;
            return;
        }
        r3 = 0x7FFF0000;
        r3 = r3 | 0xffff;
        r4 = -0x1;
        return;

        r8 = 0x20 - r5;
        r3 = r3 << r5;
        r10 = (u32)r4 >> r8;
        r3 = r3 | r10;
        r10 = r4 << r9;
        r3 = r3 | r10;
        r4 = r4 << r5;
    }
    /* clrrwi. r6, r6, 31 */;
    if ((s32)r5 == 0xa) return;
    r4 = 0x0 - r4;
    /* subfze r3, r3 */;

    return;
}

/* __sys_free - 0x800C4D8C | size: 0xB8 */
void __sys_free(void) {
    extern u8 lbl_8026FE70[];
    extern u8 lbl_8026FEA8[];
    extern u32 lbl_80478980;
    extern void fn_8009AAD4();
    extern void fn_8009AB50();
    extern void fn_8009AB60();
    extern void fn_8009ABD0();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    tmp = lbl_80478980;
    if ((s32)tmp == (s32)-0x1) {
        r3 = (u32)lbl_8026FE70;
        r3 = (u32)lbl_8026FE70;
        OSReport();
        r3 = (u32)lbl_8026FEA8;
        r3 = (u32)lbl_8026FEA8;
        OSReport();
        OSGetArenaLo();
        r31 = r3;
        OSGetArenaHi();
        r30 = r3;
        r3 = r31;
        r5 = 0x1;
        r4 = r30;
        fn_8009AB60();
        r31 = r3;
        OSSetArenaLo();
        tmp = r31 + 0x1f;
        /* clrrwi r30, r30, 5 */;
        /* clrrwi r3, tmp, 5 */;
        r4 = r30;
        fn_8009ABD0();
        fn_8009AB50();
        r3 = r30;
        OSSetArenaLo();
    }
    r3 = lbl_80478980;
    r4 = r29;
    fn_8009AAD4();
    return;
}

