#include "dolphin/types.h"

/*
 * strtoul.c - CRT library functions.
 *
 * Stub implementations for function coverage.
 */

/* fn_800C7904 - 0x800C7904 | size: 0x138 */
void fn_800C7904(void) {
    extern void fn_800C53E8();
    extern void fn_800C7ED4();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp == 0) {
        fn_800C53E8();
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0xA);
    if (tmp != 0) { r3 = -0x1; return; }
    tmp = *(u16*)((u8*)r31 + 0x4);
    /* extrwi. tmp, tmp, 3, 23 */;
    if (tmp == 0) {

        r3 = -0x1;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    /* extrwi tmp, tmp, 3, 26 */;
    if (tmp == 1) {
        r3 = 0x0;
        return;
    }
    r3 = *(u8*)((u8*)r31 + 0x8);
    /* extrwi tmp, r3, 3, 24 */;
    if (tmp >= 3) {
        tmp = 0x2;
        r3 = (r3 & ~0x000000E0) | (((tmp << 5) | ((u32)tmp >> 27)) & 0x000000E0);
        *(u8*)((u8*)r31 + 0x8) = r3;
    }
    tmp = *(u8*)((u8*)r31 + 0x8);
    /* extrwi tmp, tmp, 3, 24 */;
    if (tmp == 2) {
        tmp = 0x0;
        *(u32*)((u8*)r31 + 0x28) = tmp;
    }
    r4 = *(u8*)((u8*)r31 + 0x8);
    /* extrwi tmp, r4, 3, 24 */;
    if (tmp != 1) {
        tmp = 0x0;
        r3 = 0x0;
        r4 = (r4 & ~0x000000E0) | (((tmp << 5) | ((u32)tmp >> 27)) & 0x000000E0);
        *(u8*)((u8*)r31 + 0x8) = r4;
        return;
    }
    tmp = *(u16*)((u8*)r31 + 0x4);
    /* extrwi tmp, tmp, 3, 23 */;
    if (tmp != 1) {
        r30 = 0x0;
    } else {

        r3 = r31;
        fn_800C7ED4();
        r30 = r3;
    }
    r3 = r31;
    r4 = 0x0;
    __flush_buffer();
    if ((s32)r3 != 0) {
        r3 = 0x1;
        tmp = 0x0;
        *(u8*)((u8*)r31 + 0xA) = r3;
        r3 = -0x1;
        *(u32*)((u8*)r31 + 0x28) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x8);
    r4 = 0x0;
    tmp = (tmp & ~0x000000E0) | (((r4 << 5) | ((u32)r4 >> 27)) & 0x000000E0);
    r3 = 0x0;
    *(u8*)((u8*)r31 + 0x8) = tmp;
    *(u32*)((u8*)r31 + 0x18) = r30;
    *(u32*)((u8*)r31 + 0x28) = r4;

    return;
}

/* fn_800C7A3C - 0x800C7A3C | size: 0x1BC */
void fn_800C7A3C(void) {
    extern void fn_800C4F34();
    extern void fn_800C53E8();
    extern void fn_800C7ED4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r12 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp == 0) {
        r3 = -0x1;
        return;
    }
    tmp = *(u16*)((u8*)r31 + 0x4);
    /* extrwi. r3, tmp, 3, 23 */;
    if ((s32)tmp == 0) {
        r3 = 0x0;
        return;
    }
    if (r31 == 0) {
        fn_800C53E8();
        r29 = r3;
    } else {
        tmp = *(u8*)((u8*)r31 + 0xA);
        if (tmp != 0 || r3 == 0) {
            r29 = -0x1;
        } else {
            tmp = *(u8*)((u8*)r31 + 0x4);
            /* extrwi tmp, tmp, 3, 26 */;
            if (tmp == 1) {
                r29 = 0x0;
            } else {
                r3 = *(u8*)((u8*)r31 + 0x8);
                /* extrwi tmp, r3, 3, 24 */;
                if (tmp >= 3) {
                    tmp = 0x2;
                    r3 = (r3 & ~0x000000E0) | (((tmp << 5) | ((u32)tmp >> 27)) & 0x000000E0);
                    *(u8*)((u8*)r31 + 0x8) = r3;
                }
                tmp = *(u8*)((u8*)r31 + 0x8);
                /* extrwi tmp, tmp, 3, 24 */;
                if (tmp == 2) {
                    tmp = 0x0;
                    *(u32*)((u8*)r31 + 0x28) = tmp;
                }
                r3 = *(u8*)((u8*)r31 + 0x8);
                /* extrwi tmp, r3, 3, 24 */;
                if (tmp != 1) {
                    r29 = 0x0;
                    r3 = (r3 & ~0x000000E0) | (((r29 << 5) | ((u32)r29 >> 27)) & 0x000000E0);
                    *(u8*)((u8*)r31 + 0x8) = r3;
                } else {
                    tmp = *(u16*)((u8*)r31 + 0x4);
                    /* extrwi tmp, tmp, 3, 23 */;
                    if (tmp != 1) {
                        r30 = 0x0;
                    } else {
                        r3 = r31;
                        fn_800C7ED4();
                        r30 = r3;
                    }
                    r3 = r31;
                    r4 = 0x0;
                    __flush_buffer();
                    if ((s32)r3 != 0) {
                        r3 = 0x1;
                        tmp = 0x0;
                        *(u8*)((u8*)r31 + 0xA) = r3;
                        r29 = -0x1;
                        *(u32*)((u8*)r31 + 0x28) = tmp;
                    } else {
                        r29 = 0x0;
                        tmp = *(u8*)((u8*)r31 + 0x8);
                        tmp = (tmp & ~0x000000E0) | (((r29 << 5) | ((u32)r29 >> 27)) & 0x000000E0);
                        *(u8*)((u8*)r31 + 0x8) = tmp;
                        *(u32*)((u8*)r31 + 0x18) = r30;
                        *(u32*)((u8*)r31 + 0x28) = r29;
                    }
                }
            }
        }
    }
    /* L_800C7B7C */
    r12 = *(u32*)((u8*)r31 + 0x44);
    r3 = *(u32*)((u8*)r31 + 0x0);
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    tmp = *(u16*)((u8*)r31 + 0x4);
    r4 = 0x0;
    tmp = (tmp & ~0x000001C0) | (((r4 << 6) | ((u32)r4 >> 26)) & 0x000001C0);
    r30 = r3;
    *(u16*)((u8*)r31 + 0x4) = tmp;
    *(u32*)((u8*)r31 + 0x0) = r4;
    tmp = *(u8*)((u8*)r31 + 0x8);
    /* extrwi. tmp, tmp, 1, 27 */;
    if ((s32)r3 != 0) {
        r3 = *(u32*)((u8*)r31 + 0x1C);
        fn_800C4F34();
    }
    r3 = 0x0;
    if ((s32)r29 != 0 || (s32)r30 != 0) {
        r3 = 0x1;
    }
    tmp = -r3;
    tmp = tmp | r3;
    r3 = (s32)tmp >> 31;

    return;
}

/* fn_800C7C64 - 0x800C7C64 | size: 0x270 */
void fn_800C7C64(void) {
    extern u32 lbl_8047AA10;
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r12 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r5;
    r30 = r3;
    tmp = *(u16*)((u8*)r3 + 0x4);
    /* extrwi tmp, tmp, 3, 23 */;
    if (tmp == 1) {
        tmp = *(u8*)((u8*)r30 + 0xA);
        if (tmp != 0) {
            tmp = 0x28;
            r3 = -0x1;
            lbl_8047AA10 = tmp;
            return;
        }
    } else {
        tmp = 0x28;
        r3 = -0x1;
        lbl_8047AA10 = tmp;
        return;
    }
    /* L_800C7CB0 */
    tmp = *(u8*)((u8*)r30 + 0x8);
    /* extrwi tmp, tmp, 3, 24 */;
    if (tmp == 1) {
        r4 = 0x0;
        __flush_buffer();
        if ((s32)r3 != 0) {
            tmp = 0x1;
            r4 = 0x0;
            *(u8*)((u8*)r30 + 0xA) = tmp;
            tmp = 0x28;
            r3 = -0x1;
            *(u32*)((u8*)r30 + 0x28) = r4;
            lbl_8047AA10 = tmp;
            return;
        }
    }
    if ((s32)r31 == 1) {
        tmp = *(u16*)((u8*)r30 + 0x4);
        r31 = 0x0;
        r4 = 0x0;
        /* extrwi tmp, tmp, 3, 23 */;

        if ((tmp == 1 || tmp == 2)) {
            tmp = *(u8*)((u8*)r30 + 0xA);
            if (tmp == 0) {
                /* L_800C7D34 */
                tmp = *(u8*)((u8*)r30 + 0x8);
                /* extrwi. r5, tmp, 3, 24 */;
                if (tmp == 0) {
                    r3 = *(u32*)((u8*)r30 + 0x18);
                } else {
                    /* L_800C7D48 */
                    r7 = *(u32*)((u8*)r30 + 0x1C);
                    tmp = *(u32*)((u8*)r30 + 0x24);
                    r3 = *(u32*)((u8*)r30 + 0x34);
                    r6 = tmp - r7;
                    r3 = r3 + r6;
                    if (r5 >= 3) {
                        r3 = r3 - r4;
                    }
                    tmp = *(u8*)((u8*)r30 + 0x5);
                    /* extrwi. tmp, tmp, 1, 28 */;
                    if (r5 == 3) {
                        /* subf. tmp, r4, r6 */;
                        ctr_fn = (void(*)(void))tmp;
                        if (r5 != 3) {
                            do {
                                tmp = *(u8*)((u8*)r7 + 0x0);
                                r7 = r7 + 0x1;
                                if (tmp == 0xa) {
                                    r3 = r3 + 0x1;
                                }
                            } while (--ctr != 0);
                        }
                    }
                }
            } else {
                /* L_800C7D24 */
                tmp = 0x28;
                r3 = -0x1;
                lbl_8047AA10 = tmp;
            }
        } else {
            /* L_800C7D24 */
            tmp = 0x28;
            r3 = -0x1;
            lbl_8047AA10 = tmp;
        }
        /* L_800C7D9C */
        tmp = tmp + r3;
        *(u32*)(sp + 0x8) = tmp;
    }
    /* After the r31==1 block, handle the seek logic */
    if ((s32)r31 != 2) {
        tmp = *(u8*)((u8*)r30 + 0x4);
        /* extrwi tmp, tmp, 3, 26 */;
        if (tmp != 3) {
            tmp = *(u8*)((u8*)r30 + 0x8);
            /* extrwi tmp, tmp, 3, 24 */;
            if (tmp == 2 || tmp == 3) {
                u32 val18 = *(u32*)((u8*)r30 + 0x18);
                if (r3 >= val18) {
                    /* L_800C7DF4 */
                    tmp = *(u8*)((u8*)r30 + 0x8);
                    r3 = 0x0;
                    tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
                    *(u8*)((u8*)r30 + 0x8) = tmp;
                } else {
                    u32 val34 = *(u32*)((u8*)r30 + 0x34);
                    if (r3 >= val34) {
                        /* L_800C7E08 */
                        r4 = *(u32*)((u8*)r30 + 0x1C);
                        tmp = r3 - val34;
                        r3 = 0x2;
                        tmp = r4 + tmp;
                        *(u32*)((u8*)r30 + 0x24) = tmp;
                        tmp = *(u32*)((u8*)r30 + 0x18);
                        tmp = tmp - r4;
                        *(u32*)((u8*)r30 + 0x28) = tmp;
                        tmp = *(u8*)((u8*)r30 + 0x8);
                        tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
                        *(u8*)((u8*)r30 + 0x8) = tmp;
                    } else {
                        /* L_800C7DF4 */
                        tmp = *(u8*)((u8*)r30 + 0x8);
                        r3 = 0x0;
                        tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
                        *(u8*)((u8*)r30 + 0x8) = tmp;
                    }
                }
            } else {
                /* L_800C7E3C */
                tmp = *(u8*)((u8*)r30 + 0x8);
                r3 = 0x0;
                tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
                *(u8*)((u8*)r30 + 0x8) = tmp;
            }
        } else {
            /* L_800C7E3C */
            tmp = *(u8*)((u8*)r30 + 0x8);
            r3 = 0x0;
            tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
            *(u8*)((u8*)r30 + 0x8) = tmp;
        }
    } else {
        /* L_800C7E3C */
        tmp = *(u8*)((u8*)r30 + 0x8);
        r3 = 0x0;
        tmp = (tmp & ~0x000000E0) | (((r3 << 5) | ((u32)r3 >> 27)) & 0x000000E0);
        *(u8*)((u8*)r30 + 0x8) = tmp;
    }
    /* L_800C7E4C */
    tmp = *(u8*)((u8*)r30 + 0x8);
    /* extrwi. tmp, tmp, 3, 24 */;
    if (r3 == tmp) {
        r12 = *(u32*)((u8*)r30 + 0x38);
        if (r12 != 0) {
            r5 = r31;
            r4 = (u32)sp + 0x8;
            r3 = *(u32*)((u8*)r30 + 0x0);
            r6 = *(u32*)((u8*)r30 + 0x48);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
            if ((s32)r3 != 0) {
                tmp = 0x1;
                r4 = 0x0;
                *(u8*)((u8*)r30 + 0xA) = tmp;
                tmp = 0x28;
                r3 = -0x1;
                *(u32*)((u8*)r30 + 0x28) = r4;
                lbl_8047AA10 = tmp;
                return;
            }
        }
        r3 = 0x0;
        *(u8*)((u8*)r30 + 0x9) = r3;
        *(u32*)((u8*)r30 + 0x18) = tmp;
        *(u32*)((u8*)r30 + 0x28) = r3;
    }
    r3 = 0x0;

    return;
}

/* fn_800C7ED4 - 0x800C7ED4 | size: 0xE4 */
void fn_800C7ED4(void) {
    extern u32 lbl_8047AA10;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r3;
    r3 = 0x2;
    __begin_critical_region();
    tmp = *(u16*)((u8*)r30 + 0x4);
    r4 = 0x0;
    /* extrwi tmp, tmp, 3, 23 */;

    if ((tmp == 1 || tmp == 2)) {
        tmp = *(u8*)((u8*)r30 + 0xA);
        if (tmp == 0) {
            /* L_800C7F2C */
            tmp = *(u8*)((u8*)r30 + 0x8);
            /* extrwi. r5, tmp, 3, 24 */;
            if (tmp == 0) {
                r31 = *(u32*)((u8*)r30 + 0x18);
            } else {
                /* L_800C7F40 */
                r7 = *(u32*)((u8*)r30 + 0x1C);
                tmp = *(u32*)((u8*)r30 + 0x24);
                r3 = *(u32*)((u8*)r30 + 0x34);
                r6 = tmp - r7;
                r31 = r3 + r6;
                if (r5 >= 3) {
                    r31 = r31 - r4;
                }
                tmp = *(u8*)((u8*)r30 + 0x5);
                /* extrwi. tmp, tmp, 1, 28 */;
                if (r5 == 3) {
                    /* subf. tmp, r4, r6 */;
                    ctr_fn = (void(*)(void))tmp;
                    if (r5 != 3) {
                        do {
                            tmp = *(u8*)((u8*)r7 + 0x0);
                            r7 = r7 + 0x1;
                            if (tmp == 0xa) {
                                r31 = r31 + 0x1;
                            }
                        } while (--ctr != 0);
                    }
                }
            }
        } else {
            /* L_800C7F1C */
            tmp = 0x28;
            r31 = -0x1;
            lbl_8047AA10 = tmp;
        }
    } else {
        /* L_800C7F1C */
        tmp = 0x28;
        r31 = -0x1;
        lbl_8047AA10 = tmp;
    }
    /* L_800C7F94 */
    r3 = 0x2;
    __end_critical_region();
    r3 = r31;
    return;
}

