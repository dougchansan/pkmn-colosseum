/**
 * @file gs_task_util.c
 * @brief Decompiled functions.
 *
 * Address range: 0x800FEBA0 - 0x800FF0A0
 */

#include "dolphin/types.h"

/* ===================================================================
 * Generated: 0 pattern-matched + 6 stubs
 * Range: 0x800FEBA0 - 0x800FF0A0
 * =================================================================== */

/* 0x800FEBA0 | 0x94 */
void fn_800FEBA0(void) {
    extern u32 lbl_8047AC7C;
    extern u32 lbl_8047AC94;
    extern u32 lbl_8047AC98;
    u8 *p;
    u8 *next;
    void (*cb)(u32, u32);
    u32 idx;

    p = (u8*)lbl_8047AC98;
    while (p != 0) {
        next = (u8*)*(u32*)(p + 0x4);
        if ((s32)*(u32*)(p + 0x8) == 3) {
            if (*(u8*)(p + 0xD) == 0) {
                lbl_8047AC94 = (u32)p;
                idx = ((u32)p - lbl_8047AC7C) / 0x18;
                cb = (void(*)(u32, u32))*(u32*)(p + 0x14);
                cb(idx + 1, *(u32*)(p + 0x10));
            }
        }
        p = next;
    }
    lbl_8047AC94 = 0;
}

/* 0x800FEC34 | 0x84 */
void fn_800FEC34(void) {
    extern u32 lbl_8047ACB0;
    extern u32 lbl_8047ACC0;
    extern void fn_800F0424();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = lbl_8047ACC0;
    r4 = lbl_8047ACB0;
    ctr_fn = (void(*)(void))tmp;
    if (tmp != 0) {
        while (ctr != 0) {
            tmp = *(u32*)((u8*)r4 + 0x8);
            if ((s32)tmp == 3) {
                tmp = *(u32*)((u8*)r4 + 0x18);
                if (tmp == r3) break;
            }
            r4 = r4 + 0x24;
            --ctr;
        }
        if (ctr == 0) r4 = 0x0;
    } else {
        r4 = 0x0;
    }
    if (r4 != 0) {
        tmp = 0x0;
        *(u8*)((u8*)r4 + 0x15) = tmp;
        tmp = *(u32*)((u8*)r4 + 0xC);
        if ((s32)tmp == 1) {
            r3 = *(u32*)((u8*)r4 + 0x1C);
            if (r3 != 0) {
                fn_800F0424();
    }
    }
    }
    return;
}

/* 0x800FECB8 | 0x84 */
void fn_800FECB8(void) {
    extern u32 lbl_8047ACB0;
    extern u32 lbl_8047ACC0;
    extern void fn_800F0438();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = lbl_8047ACC0;
    r4 = lbl_8047ACB0;
    ctr_fn = (void(*)(void))tmp;
    if (tmp != 0) {
        while (ctr != 0) {
            tmp = *(u32*)((u8*)r4 + 0x8);
            if ((s32)tmp == 3) {
                tmp = *(u32*)((u8*)r4 + 0x18);
                if (tmp == r3) break;
            }
            r4 = r4 + 0x24;
            --ctr;
        }
        if (ctr == 0) r4 = 0x0;
    } else {
        r4 = 0x0;
    }
    if (r4 != 0) {
        tmp = 0x1;
        *(u8*)((u8*)r4 + 0x15) = tmp;
        tmp = *(u32*)((u8*)r4 + 0xC);
        if ((s32)tmp == 1) {
            r3 = *(u32*)((u8*)r4 + 0x1C);
            if (r3 != 0) {
                fn_800F0438();
    }
    }
    }
    return;
}

/* 0x800FED3C | 0x12C */
void fn_800FED3C(void) {
    extern u32 lbl_8047ACB0;
    extern u32 lbl_8047ACB4;
    extern u32 lbl_8047ACB8;
    extern u32 lbl_8047ACBC;
    extern u32 lbl_8047ACCC;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r6 = lbl_8047ACB4;
    tmp = lbl_8047ACB8;
    r7 = lbl_8047ACB0;
    tmp = r6 + tmp;
    r6 = lbl_8047ACBC;
    tmp = tmp * 0x24;
    r8 = r7 + tmp;
    ctr_fn = (void(*)(void))r6;
    if (r6 != 0) {
        while (ctr != 0) {
            tmp = *(u32*)((u8*)r8 + 0x8);
            if ((s32)tmp == 0) break;
            r8 = r8 + 0x24;
            --ctr;
        }
        if (ctr == 0) r8 = 0x0;
    } else {
        r8 = 0x0;
    }
    if (r8 == (u32)0x0) return;
    r7 = 0x0;
    r6 = 0x5;
    *(u32*)((u8*)r8 + 0x0) = r7;
    tmp = 0x1;
    *(u32*)((u8*)r8 + 0x4) = r7;
    *(u32*)((u8*)r8 + 0x8) = r6;
    *(u32*)((u8*)r8 + 0xC) = tmp;
    *(u32*)((u8*)r8 + 0x10) = r4;
    *(u8*)((u8*)r8 + 0x14) = r3;
    *(u8*)((u8*)r8 + 0x15) = r7;
    tmp = *(u32*)((u8*)r8 + 0xC);
    if ((s32)tmp == 0) {
        *(u32*)((u8*)r8 + 0x18) = r5;
    } else {

        *(u32*)((u8*)r8 + 0x18) = r5;
        *(u16*)((u8*)r8 + 0x20) = r7;
        *(u32*)((u8*)r8 + 0x1C) = r7;
    }
    r4 = lbl_8047ACCC;
    if (r4 == 0) {
        lbl_8047ACCC = r8;
        return;
    }
    /* Walk list to find insertion point by priority */
    while (1) {
        r5 = *(u32*)((u8*)r4 + 0x4);
        if (r5 == 0) break;
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r8 + 0x14);
        if (r3 >= tmp) break;
        r4 = r5;
    }
    if (r5 == 0) {
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r8 + 0x14);
        if (r3 < tmp) {
            *(u32*)((u8*)r8 + 0x0) = r4;
            tmp = 0x0;
            *(u32*)((u8*)r8 + 0x4) = tmp;
            *(u32*)((u8*)r4 + 0x4) = r8;
            return;
    }
    }
    r3 = *(u32*)((u8*)r4 + 0x0);
    if (r3 != 0) {
        *(u32*)((u8*)r3 + 0x4) = r8;
    }
    tmp = *(u32*)((u8*)r4 + 0x0);
    *(u32*)((u8*)r8 + 0x0) = tmp;
    *(u32*)((u8*)r8 + 0x4) = r4;
    *(u32*)((u8*)r4 + 0x0) = r8;
    tmp = lbl_8047ACCC;
    if (tmp != (u32)r4) return;
    lbl_8047ACCC = r8;
    return;
}

/* 0x800FEE68 | 0x124 */
void fn_800FEE68(void) {
    extern u32 lbl_8047ACB0;
    extern u32 lbl_8047ACB4;
    extern u32 lbl_8047ACB8;
    extern u32 lbl_8047ACCC;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = lbl_8047ACB4;
    r6 = lbl_8047ACB0;
    tmp = tmp * 0x24;
    r7 = lbl_8047ACB8;
    r8 = r6 + tmp;
    ctr_fn = (void(*)(void))r7;
    if (r7 != 0) {
        while (ctr != 0) {
            tmp = *(u32*)((u8*)r8 + 0x8);
            if ((s32)tmp == 0) break;
            r8 = r8 + 0x24;
            --ctr;
        }
        if (ctr == 0) r8 = 0x0;
    } else {
        r8 = 0x0;
    }
    if (r8 == (u32)0x0) return;
    r7 = 0x0;
    r6 = 0x3;
    *(u32*)((u8*)r8 + 0x0) = r7;
    tmp = 0x1;
    *(u32*)((u8*)r8 + 0x4) = r7;
    *(u32*)((u8*)r8 + 0x8) = r6;
    *(u32*)((u8*)r8 + 0xC) = tmp;
    *(u32*)((u8*)r8 + 0x10) = r4;
    *(u8*)((u8*)r8 + 0x14) = r3;
    *(u8*)((u8*)r8 + 0x15) = r7;
    tmp = *(u32*)((u8*)r8 + 0xC);
    if ((s32)tmp == 0) {
        *(u32*)((u8*)r8 + 0x18) = r5;
    } else {

        *(u32*)((u8*)r8 + 0x18) = r5;
        *(u16*)((u8*)r8 + 0x20) = r7;
        *(u32*)((u8*)r8 + 0x1C) = r7;
    }
    r4 = lbl_8047ACCC;
    if (r4 == 0) {
        lbl_8047ACCC = r8;
        return;
    }
    /* Walk list to find insertion point by priority */
    while (1) {
        r5 = *(u32*)((u8*)r4 + 0x4);
        if (r5 == 0) break;
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r8 + 0x14);
        if (r3 >= tmp) break;
        r4 = r5;
    }
    if (r5 == 0) {
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r8 + 0x14);
        if (r3 < tmp) {
            *(u32*)((u8*)r8 + 0x0) = r4;
            tmp = 0x0;
            *(u32*)((u8*)r8 + 0x4) = tmp;
            *(u32*)((u8*)r4 + 0x4) = r8;
            return;
    }
    }
    r3 = *(u32*)((u8*)r4 + 0x0);
    if (r3 != 0) {
        *(u32*)((u8*)r3 + 0x4) = r8;
    }
    tmp = *(u32*)((u8*)r4 + 0x0);
    *(u32*)((u8*)r8 + 0x0) = tmp;
    *(u32*)((u8*)r8 + 0x4) = r4;
    *(u32*)((u8*)r4 + 0x0) = r8;
    tmp = lbl_8047ACCC;
    if (tmp != (u32)r4) return;
    lbl_8047ACCC = r8;
    return;
}

/* 0x800FEF8C | 0x114 */
void fn_800FEF8C(void) {
    extern u32 lbl_8047ACB0;
    extern u32 lbl_8047ACB4;
    extern u32 lbl_8047ACCC;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = lbl_8047ACB4;
    r7 = lbl_8047ACB0;
    ctr_fn = (void(*)(void))tmp;
    if (tmp != 0) {
        while (ctr != 0) {
            tmp = *(u32*)((u8*)r7 + 0x8);
            if ((s32)tmp == 0) break;
            r7 = r7 + 0x24;
            --ctr;
        }
        if (ctr == 0) r7 = 0x0;
    } else {
        r7 = 0x0;
    }
    if (r7 == (u32)0x0) return;
    r6 = 0x0;
    tmp = 0x1;
    *(u32*)((u8*)r7 + 0x0) = r6;
    *(u32*)((u8*)r7 + 0x4) = r6;
    *(u32*)((u8*)r7 + 0x8) = tmp;
    *(u32*)((u8*)r7 + 0xC) = tmp;
    *(u32*)((u8*)r7 + 0x10) = r4;
    *(u8*)((u8*)r7 + 0x14) = r3;
    *(u8*)((u8*)r7 + 0x15) = r6;
    tmp = *(u32*)((u8*)r7 + 0xC);
    if ((s32)tmp == 0) {
        *(u32*)((u8*)r7 + 0x18) = r5;
    } else {

        *(u32*)((u8*)r7 + 0x18) = r5;
        *(u16*)((u8*)r7 + 0x20) = r6;
        *(u32*)((u8*)r7 + 0x1C) = r6;
    }
    r4 = lbl_8047ACCC;
    if (r4 == 0) {
        lbl_8047ACCC = r7;
        return;
    }
    /* Walk list to find insertion point by priority */
    while (1) {
        r5 = *(u32*)((u8*)r4 + 0x4);
        if (r5 == 0) break;
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r7 + 0x14);
        if (r3 >= tmp) break;
        r4 = r5;
    }
    if (r5 == 0) {
        r3 = *(u8*)((u8*)r4 + 0x14);
        tmp = *(u8*)((u8*)r7 + 0x14);
        if (r3 < tmp) {
            *(u32*)((u8*)r7 + 0x0) = r4;
            tmp = 0x0;
            *(u32*)((u8*)r7 + 0x4) = tmp;
            *(u32*)((u8*)r4 + 0x4) = r7;
            return;
    }
    }
    r3 = *(u32*)((u8*)r4 + 0x0);
    if (r3 != 0) {
        *(u32*)((u8*)r3 + 0x4) = r7;
    }
    tmp = *(u32*)((u8*)r4 + 0x0);
    *(u32*)((u8*)r7 + 0x0) = tmp;
    *(u32*)((u8*)r7 + 0x4) = r4;
    *(u32*)((u8*)r4 + 0x0) = r7;
    tmp = lbl_8047ACCC;
    if (tmp != (u32)r4) return;
    lbl_8047ACCC = r7;
    return;
}
