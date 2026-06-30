/**
 * @file save_carde.c
 * @brief Card-E save data validation (0x80082650-0x80083AF4)
 *
 * Address range: 0x80082650 - 0x80083AF4
 * Total functions: 11
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_800CAA3C();
extern void fn_80129280();
extern void __assert();
extern void fn_801EE10C();
extern void fn_801EE1E0();
extern void fn_801EE2B4();
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047C180;
extern u8 lbl_8047C188;

/* ===== Rodata / data labels ===== */
extern u8 lbl_8026F1C8[];
extern u8 lbl_8026F1D8[];

/* ===== Forward declarations ===== */
void fn_80082650(void* carde);
void fn_80082738(void);
void fn_80082960(void);
s32 fn_80082A88(void* carde, s32 levelIndex);
void fn_80082BA4(void);
void fn_80082CF0(void);
void fn_80082EA4(void);
void* fn_80082FE4(void* carde, s32 index);
void fn_800830A4(void);
void fn_800832C8(void);
void fn_800836AC(void);

/* ===== Function implementations ===== */

/* 0x80082650 | size: 0xE8 */
void fn_80082650(void* carde) {
#pragma peephole off
    u8* base;
    u8* block;
    s32 width;
    s32 height;
    s32 count;
    u32 found;

    base = carde;
    if (base == NULL) {
        __assert(lbl_8026F1C8, 0x17f, &lbl_8047C180);
    }
    if ((s32)(s8)*(u8*)(base + 0x1B) <= 0) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    block = base + 0x24;
    if (block == NULL) {
        __assert(lbl_8026F1C8, 0x1f1, &lbl_8047C188);
    }
    width = (s8)*(u8*)(base + 0x1C);
    height = (s8)*(u8*)(base + 0x1D);
    count = width;
    count *= height;
    for (; count > 0; count--) {
        if (*(u8*)(block + 0x82) != 0) {
            found = 1;
            goto done;
        }
        block += 0x10;
    }
    found = 0;
done:
    if ((u8)found == 0) {
        *(u16*)base = 0;
    }
}

/* 0x80082738 | size: 0x228 */
void fn_80082738(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r26 = r3;
    r28 = r4;
    r27 = r5;
    r3 = *(u8*)((u8*)r26 + 0x1A);
    r4 = (u32)&lbl_8026F1C8;
    tmp = *(u8*)((u8*)r28 + 0x8);
    r31 = (u32)&lbl_8026F1C8;
    if (r3 != tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x38;
        r4 = 0x225;
        ((void(*)(void))__assert)();
    }
    r29 = *(u8*)((u8*)r28 + 0x26);
    r30 = *(u8*)((u8*)r28 + 0x24);
    if (r26 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r27;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r26 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r26 + 0x1C);
    r4 = (s8)r27;
    tmp = *(u8*)((u8*)r26 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r28 = r26 + tmp;
    r28 = r28 + 0x24;
    if (r28 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x198;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r26 + 0x1C);
    r3 = (s8)r30;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x68;
        r4 = 0x199;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r26 + 0x1D);
    r3 = (s8)r29;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x80;
        r4 = 0x19a;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r26 + 0x1D);
    tmp = (s8)r27;
    r5 = (s8)r30;
    r6 = (s8)r29;
    r4 = (s8)r3;
    r3 = 0x0;
    r4 = r5 * r4;
    tmp = r6 + r4;
    r4 = tmp << 4;
    r4 = r4 + 0x76;
    r4 = r28 + r4;
    *(u8*)((u8*)r4 + 0xC) = r3;
    *(u16*)((u8*)r4 + 0x0) = r3;
    if ((s32)tmp != 0) { r3 = 0x0; return; }
    if (r26 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r26 + 0x1B);
    tmp = (s8)tmp;
    if ((s32)tmp <= 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r27 = r26 + 0x24;
    if (r27 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x1f1;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r26 + 0x1C);
    tmp = *(u8*)((u8*)r26 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)tmp > 0) {
        do {
            tmp = *(u8*)((u8*)r27 + 0x82);
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            r27 = r27 + 0x10;
        } while (--ctr != 0);
    }
    tmp = 0x0;

    tmp = tmp & 0xFF;
    if (tmp != 0) { r3 = 0x0; return; }
    r3 = 0x1;
    return;

    r3 = 0x0;

    return;
}

/* 0x80082960 | size: 0x128 */
void fn_80082960(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = r4;
    r28 = r5;
    r3 = *(u8*)((u8*)r30 + 0x1A);
    r4 = (u32)&lbl_8026F1C8;
    tmp = *(u8*)((u8*)r31 + 0x8);
    r29 = (u32)&lbl_8026F1C8;
    if (r3 != tmp) {
        r3 = r29 + 0x0;
        r5 = r29 + 0x38;
        r4 = 0x209;
        ((void(*)(void))__assert)();
    }
    if (r30 == 0) {
        r3 = r29 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r28;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r30 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r29 + 0x0;
        r5 = r29 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r30 + 0x1C);
    r4 = (s8)r28;
    tmp = *(u8*)((u8*)r30 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r30 = r30 + tmp;
    r30 = r30 + 0x24;
    if (r30 == 0) {
        r3 = r29 + 0x0;
        r4 = 0x20c;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r31 + 0x24);
    tmp = 0x0;
    r3 = (s8)r3;
    r3 = r3 * 0xe;
    r3 = r3 + 0x10;
    r3 = r30 + r3;
    *(u16*)((u8*)r3 + 0x0) = tmp;
    *(u8*)((u8*)r3 + 0xC) = tmp;
    return;
}

/* 0x80082A88 | size: 0x11C */
s32 fn_80082A88(void* carde, s32 levelIndex) {
#pragma peephole off
    u8* base;
    u8* block;
    s32 valid;
    s32 width;
    s32 height;
    s32 stride;
    s32 count;
    s32 offset;

    base = carde;
    if (base == NULL) {
        __assert(lbl_8026F1C8, 0x17f, &lbl_8047C180);
    }
    valid = 0;
    if ((s32)(s8)levelIndex >= 0) {
        if ((s32)(s8)levelIndex < (s32)(s8)*(u8*)(base + 0x1B)) {
            valid = 1;
        }
    }
    if (valid == 0) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    width = (s8)*(u8*)(base + 0x1C);
    height = (s8)*(u8*)(base + 0x1D);
    stride = width;
    stride *= height;
    stride <<= 4;
    stride += 0x76;
    offset = (s32)(s8)levelIndex;
    offset *= stride;
    block = base + offset;
    if ((block += 0x24) == NULL) {
        __assert(lbl_8026F1C8, 0x1f1, &lbl_8047C188);
    }
    width = (s8)*(u8*)(base + 0x1C);
    height = (s8)*(u8*)(base + 0x1D);
    count = width;
    count *= height;
    for (; count > 0; count--) {
        if (*(u8*)(block + 0x82) != 0) {
            return 1;
        }
        block += 0x10;
    }
    return 0;
}

/* 0x80082BA4 | size: 0x14C */
void fn_80082BA4(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = *(u8*)((u8*)r28 + 0x1A);
    r4 = (u32)&lbl_8026F1C8;
    tmp = *(u8*)((u8*)r29 + 0x8);
    r31 = (u32)&lbl_8026F1C8;
    if (r3 != tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x38;
        r4 = 0x1d1;
        ((void(*)(void))__assert)();
    }
    if (r28 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r30;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r28 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r28 + 0x1C);
    r4 = (s8)r30;
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r28 = r28 + tmp;
    r28 = r28 + 0x24;
    if (r28 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x1d4;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    tmp = (s8)r30;
    r4 = *(u8*)((u8*)r29 + 0x24);
    r3 = r29 + tmp;
    tmp = *(u8*)((u8*)r3 + 0x5E);
    r3 = (s8)r4;
    r4 = r3 * 0xe;
    tmp = (s8)tmp;
    r3 = tmp * 0x28;
    r30 = r4 + 0x10;
    r30 = r28 + r30;
    r4 = r3 + 0x3ac;
    r3 = r30;
    r4 = r29 + r4;
    ((void(*)(void))fn_800CAA3C)();
    tmp = 0x1;
    r3 = r28;
    *(u8*)((u8*)r30 + 0xC) = tmp;
    return;
}

/* 0x80082CF0 | size: 0x1B4 */
void fn_80082CF0(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r25 = r3;
    r26 = r4;
    r27 = r5;
    r3 = *(u8*)((u8*)r25 + 0x1A);
    r4 = (u32)&lbl_8026F1C8;
    tmp = *(u8*)((u8*)r26 + 0x8);
    r31 = (u32)&lbl_8026F1C8;
    if (r3 != tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x38;
        r4 = 0x1b0;
        ((void(*)(void))__assert)();
    }
    r29 = *(u8*)((u8*)r26 + 0x26);
    r30 = *(u8*)((u8*)r26 + 0x24);
    if (r25 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r27;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r25 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r25 + 0x1C);
    r4 = (s8)r27;
    tmp = *(u8*)((u8*)r25 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r28 = r25 + tmp;
    r28 = r28 + 0x24;
    if (r28 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x198;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r25 + 0x1C);
    r3 = (s8)r30;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x68;
        r4 = 0x199;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r25 + 0x1D);
    r3 = (s8)r29;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x80;
        r4 = 0x19a;
        ((void(*)(void))__assert)();
    }
    r4 = *(u8*)((u8*)r25 + 0x1D);
    tmp = (s8)r27;
    r3 = r26 + tmp;
    r5 = (s8)r30;
    r4 = (s8)r4;
    tmp = *(u8*)((u8*)r3 + 0x5B);
    r4 = r5 * r4;
    r5 = (s8)r29;
    tmp = (s8)tmp;
    r3 = tmp * 0x28;
    tmp = r5 + r4;
    r4 = tmp << 4;
    r27 = r4 + 0x76;
    r29 = r3 + 0x3ac;
    r27 = r28 + r27;
    r29 = r26 + r29;
    r3 = r27;
    r4 = r29;
    ((void(*)(void))fn_800CAA3C)();
    tmp = 0x1;
    r3 = r27;
    *(u8*)((u8*)r27 + 0xC) = tmp;
    tmp = *(u16*)((u8*)r29 + 0x22);
    *(u16*)((u8*)r27 + 0xE) = tmp;
    tmp = *(u8*)((u8*)r26 + 0x24);
    r5 = *(u8*)((u8*)r26 + 0x25);
    tmp = (s8)tmp;
    r4 = r25 + tmp;
    *(u8*)((u8*)r4 + 0x1E) = r5;
    return;
}

/* 0x80082EA4 | size: 0x140 */
void fn_80082EA4(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r27 = r4;
    r29 = r5;
    r30 = r6;
    r3 = (u32)&lbl_8026F1C8;
    r31 = (u32)&lbl_8026F1C8;
    if (r28 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r27;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r28 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r28 + 0x1C);
    r4 = (s8)r27;
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r27 = r28 + tmp;
    r27 = r27 + 0x24;
    if (r27 == 0) {
        r3 = r31 + 0x0;
        r4 = 0x198;
        r5 = (u32)&lbl_8047C188;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r28 + 0x1C);
    r3 = (s8)r29;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x68;
        r4 = 0x199;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r3 = (s8)r30;
    tmp = (s8)tmp;
    if ((s32)r3 >= (s32)tmp) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x80;
        r4 = 0x19a;
        ((void(*)(void))__assert)();
    }
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r3 = (s8)r29;
    r4 = (s8)r30;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    tmp = r4 + tmp;
    r3 = tmp << 4;
    r3 = r3 + 0x76;
    r3 = r27 + r3;
    return;
}

/* 0x80082FE4 | size: 0xC0 */
void* fn_80082FE4(void* carde, s32 index) {
#pragma peephole off
    u8* base;
    s32 valid;
    s32 width;
    s32 height;
    s32 stride;
    s32 finalIndex;
    s32 offset;

    base = carde;
    if (base == NULL) {
        __assert(lbl_8026F1C8, 0x17f, &lbl_8047C180);
    }
    valid = 0;
    if ((s32)(s8)index >= 0) {
        if ((s32)(s8)index < (s32)(s8)*(u8*)(base + 0x1B)) {
            valid = 1;
        }
    }
    if (valid == 0) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    finalIndex = (s8)index;
    width = (s8)*(u8*)(base + 0x1C);
    height = (s8)*(u8*)(base + 0x1D);
    stride = width;
    stride *= height;
    stride <<= 4;
    stride += 0x76;
    offset = finalIndex;
    offset *= stride;
    base += offset;
    return base + 0x24;
}

/* 0x800830A4 | size: 0x224 */
void fn_800830A4(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r3 = (u32)&lbl_8026F1C8;
    r29 = (u32)&lbl_8026F1C8;
    if (r30 != 0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        ((void(*)(void))fn_80129280)();
        r30 = r3;
    }
    r3 = *(u8*)((u8*)r30 + 0x4000);
    tmp = 0x0;

    if (r3 == 1 && r3 != 2) {

        tmp = 0x1;
    }
    if ((s32)tmp == 0) {
        r3 = r29 + 0x0;
        r5 = r29 + 0xa0;
        r4 = 0x161;
        ((void(*)(void))__assert)();
    }
    tmp = 0x0;
    r27 = 0x0;
    *(u8*)((u8*)r30 + 0x4000) = tmp;
    r31 = (u32)sp + 0x8;
while (1) {
        if (r30 != 0) {
            r3 = r30;
        } else {

            r3 = 0x0;
            r4 = 0xd;
            ((void(*)(void))fn_80129280)();
        }
        r6 = r3 + 0x4000;
        if (r31 != 0) {
            tmp = 0x0;
            *(u32*)(sp + 0x8) = tmp;
        }
        r7 = 0x0;
        do {
            tmp = r3 + 0x24;
            if (r6 < tmp) break;
            tmp = *(u16*)((u8*)r3 + 0x0);
            if (tmp == 0) break;
            tmp = *(u8*)((u8*)r3 + 0x1B);
            tmp = (s8)tmp;
            do {
                if ((s32)tmp > 3) break;
                tmp = *(u8*)((u8*)r3 + 0x1C);
                tmp = (s8)tmp;
                if ((s32)tmp > 6) break;
                tmp = *(u8*)((u8*)r3 + 0x1D);
                tmp = (s8)tmp;
                if ((s32)tmp > 5) break;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x0) = tmp;
            } while (0);
            if ((s32)r7 == (s32)r27) {
            }
            r4 = *(u8*)((u8*)r3 + 0x1C);
            r7 = r7 + 0x1;
            tmp = *(u8*)((u8*)r3 + 0x1D);
            r4 = (s8)r4;
            r5 = *(u8*)((u8*)r3 + 0x1B);
            tmp = (s8)tmp;
            tmp = r4 * tmp;
            r5 = (s8)r5;
            r4 = tmp << 4;
            tmp = r4 + 0x76;
            tmp = r5 * tmp;
            r3 = tmp + r3;
            r3 = r3 + 0x24;
        } while (1);

        if ((s32)r27 < 0) {
        }
        if (r28 == 0) {
            r3 = r29 + 0x0;
            r4 = 0x169;
            r5 = (u32)&lbl_8047C180;
            ((void(*)(void))__assert)();
        }
        r3 = *(u8*)((u8*)r28 + 0x1A);
        tmp = *(u8*)((u8*)r30 + 0x4001);
        if (r3 != tmp) {
            r27 = r27 + 0x1;
}
    }
    r27 = *(u8*)((u8*)r30 + 0x4002);
    if (r28 == 0) {
        r3 = r29 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r27;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r28 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r29 + 0x0;
        r5 = r29 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r28 + 0x1C);
    r4 = (s8)r27;
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r5 = 0x1;
    r3 = (s8)r3;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r3 = r28 + tmp;
    *(u8*)((u8*)r3 + 0x95) = r5;
    return;
}

/* 0x800832C8 | size: 0x3E4 */
void fn_800832C8(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r24 = r3;
    r31 = r4;
    r27 = r5;
    r3 = (u32)&lbl_8026F1C8;
    r29 = (u32)&lbl_8026F1C8;
    if (r24 != 0) {
        r3 = r24;
    } else {

        r3 = 0x0;
        r4 = 0xd;
        ((void(*)(void))fn_80129280)();
    }
    r28 = (s8)r27;
    r24 = r3;
    r4 = r31 + r28;
    r25 = r3 + 0x4000;
    tmp = *(u8*)((u8*)r4 + 0x61);
    tmp = (s8)tmp;
    r3 = tmp * 0x28;
    r26 = r3 + 0x3ac;
    r26 = r31 + r26;
    tmp = *(u16*)((u8*)r26 + 0x0);
    if (tmp == 0) {
        r3 = r29 + 0x0;
        r5 = r29 + 0x10c;
        r4 = 0x108;
        ((void(*)(void))__assert)();
    }
    tmp = 0x1;
    r3 = r25 + 0x4;
    *(u8*)((u8*)r25 + 0x0) = tmp;
    tmp = r28 * 0x5c;
    r4 = *(u8*)((u8*)r31 + 0x8);
    r30 = r31 + tmp;
    *(u8*)((u8*)r25 + 0x1) = r4;
    r4 = r30 + 0x6e;
    *(u8*)((u8*)r25 + 0x2) = r27;
    ((void(*)(void))fn_800CAA3C)();
    r3 = r25 + 0x60;
    r4 = r30 + 0x182;
    ((void(*)(void))fn_800CAA3C)();
    r3 = r25 + 0xbc;
    r4 = r30 + 0x296;
    ((void(*)(void))fn_800CAA3C)();
    r4 = r26;
    r3 = r25 + 0x118;
    ((void(*)(void))fn_800CAA3C)();
    r5 = *(u8*)((u8*)r26 + 0xC);
    r4 = r31 + r28;
    r3 = -0x1;
    tmp = 0x0;
    *(u8*)((u8*)r25 + 0x124) = r5;
    r27 = 0x0;
    r4 = *(u8*)((u8*)r4 + 0x6A);
    *(u8*)((u8*)r25 + 0x125) = r4;
    r4 = *(u16*)((u8*)r26 + 0x12);
    *(u16*)((u8*)r25 + 0x126) = r4;
    r4 = *(u16*)((u8*)r26 + 0x14);
    *(u16*)((u8*)r25 + 0x128) = r4;
    r4 = *(u16*)((u8*)r26 + 0x16);
    *(u16*)((u8*)r25 + 0x12A) = r4;
    r4 = *(u16*)((u8*)r26 + 0x18);
    *(u16*)((u8*)r25 + 0x12C) = r4;
    r4 = *(u32*)((u8*)r26 + 0x1C);
    *(u32*)((u8*)r25 + 0x130) = r4;
    r4 = *(u16*)((u8*)r26 + 0x20);
    *(u16*)((u8*)r25 + 0x134) = r4;
    r4 = *(u8*)((u8*)r26 + 0x24);
    *(u8*)((u8*)r25 + 0x136) = r4;
    *(u8*)((u8*)r25 + 0x1E0) = r3;
    *(u8*)((u8*)r25 + 0x1E1) = tmp;
    *(u8*)((u8*)r25 + 0x1E2) = tmp;
    r30 = (u32)sp + 0x8;
while (1) {
        if (r24 != 0) {
            r3 = r24;
        } else {

            r3 = 0x0;
            r4 = 0xd;
            ((void(*)(void))fn_80129280)();
        }
        tmp = r3 + 0x4000;
        if (r30 != 0) {
            r4 = 0x0;
        }
        r4 = 0x0;
        do {
            r5 = r3 + 0x24;
            if (tmp < r5) break;
            r5 = *(u16*)((u8*)r3 + 0x0);
            if (r5 == 0) break;
            r5 = *(u8*)((u8*)r3 + 0x1B);
            r5 = (s8)r5;
            do {
                if ((s32)r5 > 3) break;
                r5 = *(u8*)((u8*)r3 + 0x1C);
                r5 = (s8)r5;
                if ((s32)r5 > 6) break;
                r5 = *(u8*)((u8*)r3 + 0x1D);
                r5 = (s8)r5;
                if ((s32)r5 > 5) break;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x0) = tmp;
            } while (0);
            if ((s32)r4 == (s32)r27) {
            }
            r6 = *(u8*)((u8*)r3 + 0x1C);
            r4 = r4 + 0x1;
            r5 = *(u8*)((u8*)r3 + 0x1D);
            r6 = (s8)r6;
            r7 = *(u8*)((u8*)r3 + 0x1B);
            r5 = (s8)r5;
            r5 = r6 * r5;
            r6 = (s8)r7;
            r5 = r5 << 4;
            r5 = r5 + 0x76;
            r5 = r6 * r5;
            r3 = r5 + r3;
            r3 = r3 + 0x24;
        } while (1);

        if ((s32)r27 < 0) {
        }
        if (r28 == 0) {
            r3 = r29 + 0x0;
            r4 = 0x121;
            r5 = (u32)&lbl_8047C180;
            ((void(*)(void))__assert)();
        }
        r3 = *(u8*)((u8*)r28 + 0x1A);
        tmp = *(u8*)((u8*)r25 + 0x1);
        if (r3 != tmp) {
            r27 = r27 + 0x1;
}
    }
    r24 = *(u8*)((u8*)r25 + 0x2);
    if (r28 == 0) {
        r3 = r29 + 0x0;
        r4 = 0x17f;
        r5 = (u32)&lbl_8047C180;
        ((void(*)(void))__assert)();
    }
    r3 = (s8)r24;
    r4 = 0x0;
    if ((s32)r3 >= 0) {
        tmp = *(u8*)((u8*)r28 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r4 = 0x1;
    }
    }
    if ((s32)r4 == 0) {
        r3 = r29 + 0x0;
        r5 = r29 + 0x10;
        r4 = 0x180;
        ((void(*)(void))__assert)();
    }
    r3 = *(u8*)((u8*)r28 + 0x1C);
    r4 = (s8)r24;
    tmp = *(u8*)((u8*)r28 + 0x1D);
    r30 = r25;
    r3 = (s8)r3;
    r27 = 0x0;
    tmp = (s8)tmp;
    tmp = r3 * tmp;
    r3 = tmp << 4;
    tmp = r3 + 0x76;
    tmp = r4 * tmp;
    r29 = r28 + tmp;
    do {
        tmp = r27 + 0xd;
        tmp = *(u8*)(r26 + tmp);
        tmp = (s8)tmp;
        if ((s32)tmp < 0) {
            tmp = 0x0;
            *(u16*)((u8*)r30 + 0x138) = tmp;
            continue;
        }
        r3 = tmp * 0x2a;
        r4 = r3 + 0x514;
        r4 = r31 + r4;
        r3 = *(u32*)((u8*)r4 + 0x0);
        tmp = *(u32*)((u8*)r4 + 0x4);
        *(u32*)((u8*)r30 + 0x138) = r3;
        *(u32*)((u8*)r30 + 0x13C) = tmp;
        r3 = *(u32*)((u8*)r4 + 0x8);
        tmp = *(u32*)((u8*)r4 + 0xC);
        *(u32*)((u8*)r30 + 0x140) = r3;
        *(u32*)((u8*)r30 + 0x144) = tmp;
        r3 = *(u32*)((u8*)r4 + 0x10);
        tmp = *(u32*)((u8*)r4 + 0x14);
        *(u32*)((u8*)r30 + 0x148) = r3;
        *(u32*)((u8*)r30 + 0x14C) = tmp;
        r3 = *(u32*)((u8*)r4 + 0x18);
        tmp = *(u32*)((u8*)r4 + 0x1C);
        *(u32*)((u8*)r30 + 0x150) = r3;
        *(u32*)((u8*)r30 + 0x154) = tmp;
        r3 = *(u32*)((u8*)r4 + 0x20);
        tmp = *(u32*)((u8*)r4 + 0x24);
        *(u32*)((u8*)r30 + 0x158) = r3;
        *(u32*)((u8*)r30 + 0x15C) = tmp;
        tmp = *(u16*)((u8*)r4 + 0x28);
        *(u16*)((u8*)r30 + 0x160) = tmp;
        tmp = *(u8*)((u8*)r4 + 0x2);
        if (tmp == 0) continue;
        tmp = (s8)r27;
        *(u8*)((u8*)r25 + 0x1E0) = tmp;
        tmp = *(u8*)((u8*)r4 + 0x28);
        *(u8*)((u8*)r25 + 0x1E2) = tmp;
        tmp = *(u8*)((u8*)r4 + 0x2);
        *(u8*)((u8*)r25 + 0x1E1) = tmp;
        tmp = *(u16*)((u8*)r4 + 0x0);
        *(u16*)((u8*)r29 + 0x98) = tmp;
        r3 = *(u8*)((u8*)r25 + 0x1E1);
        r4 = *(u16*)((u8*)r4 + 0x0);
        ((void(*)(void))fn_801EE1E0)();

        r30 = r30 + 0x2a;
        r27 = r27 + 0x1;
    } while ((s32)r27 < 4);
    r3 = r29 + 0x88;
    r4 = r25 + 0x118;
    ((void(*)(void))fn_800CAA3C)();
    r3 = *(u8*)((u8*)r25 + 0x125);
    tmp = 0x0;
    r4 = r25 + 0x118;
    *(u8*)((u8*)r29 + 0x94) = r3;
    *(u8*)((u8*)r29 + 0x95) = tmp;
    tmp = *(u8*)((u8*)r25 + 0x1E1);
    *(u8*)((u8*)r29 + 0x96) = tmp;
    r3 = *(u8*)((u8*)r25 + 0x1E1);
    ((void(*)(void))fn_801EE2B4)();
    r3 = *(u8*)((u8*)r25 + 0x1E1);
    r4 = *(u8*)((u8*)r25 + 0x125);
    ((void(*)(void))fn_801EE10C)();
    return;
}

/* 0x800836AC | size: 0x448 */
void fn_800836AC(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = r4;
    r25 = r5;
    tmp = *(u32*)((u8*)r31 + 0x0);
    if ((s32)tmp != 0) {
        r3 = 0x0;
        return;
    }
    if (r30 != 0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        ((void(*)(void))fn_80129280)();
    }
    r6 = r3 + 0x4000;
    r7 = 0x0;
    do {
        tmp = r3 + 0x24;
        if (r6 < tmp) break;
        tmp = *(u16*)((u8*)r3 + 0x0);
        if (tmp == 0) break;
        tmp = *(u8*)((u8*)r3 + 0x1B);
        r5 = (s8)tmp;
        do {
            if ((s32)r5 > 3) break;
            tmp = *(u8*)((u8*)r3 + 0x1C);
            r4 = (s8)tmp;
            if ((s32)r4 > 6) break;
            tmp = *(u8*)((u8*)r3 + 0x1D);
            tmp = (s8)tmp;
            if ((s32)tmp > 5) break;
            tmp = 0x0;
            *(u16*)((u8*)r3 + 0x0) = tmp;
        } while (0);
        tmp = r4 * tmp;
        r7 = r7 + 0x1;
        r4 = tmp << 4;
        tmp = r4 + 0x76;
        tmp = r5 * tmp;
        r3 = tmp + r3;
        r3 = r3 + 0x24;
    } while (1);

    tmp = (u32)sp + 0x10;
    if (tmp != 0) {
    }
    r26 = 0x0;
    r28 = 0x0;
    r29 = (u32)sp + 0xc;
    while (1) {
        if ((s32)r28 >= (s32)r27) break;
        if (r30 != 0) {
            r3 = r30;
        } else {

            r3 = 0x0;
            r4 = 0xd;
            ((void(*)(void))fn_80129280)();
        }
        tmp = r3 + 0x4000;
        if (r29 != 0) {
            r4 = 0x0;
        }
        r4 = 0x0;
        do {
            r5 = r3 + 0x24;
            if (tmp < r5) break;
            r5 = *(u16*)((u8*)r3 + 0x0);
            if (r5 == 0) break;
            r5 = *(u8*)((u8*)r3 + 0x1B);
            r5 = (s8)r5;
            do {
                if ((s32)r5 > 3) break;
                r5 = *(u8*)((u8*)r3 + 0x1C);
                r5 = (s8)r5;
                if ((s32)r5 > 6) break;
                r5 = *(u8*)((u8*)r3 + 0x1D);
                r5 = (s8)r5;
                if ((s32)r5 > 5) break;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x0) = tmp;
            } while (0);
            if ((s32)r4 == (s32)r28) {
            }
            r6 = *(u8*)((u8*)r3 + 0x1C);
            r4 = r4 + 0x1;
            r5 = *(u8*)((u8*)r3 + 0x1D);
            r6 = (s8)r6;
            r7 = *(u8*)((u8*)r3 + 0x1B);
            r5 = (s8)r5;
            r5 = r6 * r5;
            r6 = (s8)r7;
            r5 = r5 << 4;
            r5 = r5 + 0x76;
            r5 = r6 * r5;
            r3 = r5 + r3;
            r3 = r3 + 0x24;
        } while (1);

        if ((s32)r28 < 0) {
        }
        tmp = *(u8*)((u8*)r31 + 0x8);
        r3 = *(u8*)((u8*)r4 + 0x1A);
        r26 = r4;
        if (r3 == tmp) break;
        r28 = r28 + 0x1;

    }

    if ((s32)r28 != (s32)r27) { r3 = r26; return; }
    tmp = r25 & 0xFF;
    if (tmp == 0) {
        r3 = 0x0;
        return;
    }
    r27 = *(u8*)((u8*)r31 + 0x5A);
    r28 = *(u8*)((u8*)r31 + 0x59);
    r29 = *(u8*)((u8*)r31 + 0x58);
    if (r30 != 0) {
        r3 = r30;
    } else {

        r3 = 0x0;
        r4 = 0xd;
        ((void(*)(void))fn_80129280)();
    }
    tmp = (u32)sp + 0x8;
    r4 = r3;
    r7 = r3 + 0x4000;
    if (tmp != 0) {
        tmp = 0x0;
        *(u32*)(sp + 0x8) = tmp;
    }
    r8 = 0x0;
    do {
        tmp = r4 + 0x24;
        if (r7 < tmp) break;
        tmp = *(u16*)((u8*)r4 + 0x0);
        if (tmp == 0) break;
        tmp = *(u8*)((u8*)r4 + 0x1B);
        tmp = (s8)tmp;
        do {
            if ((s32)tmp > 3) break;
            tmp = *(u8*)((u8*)r4 + 0x1C);
            tmp = (s8)tmp;
            if ((s32)tmp > 6) break;
            tmp = *(u8*)((u8*)r4 + 0x1D);
            tmp = (s8)tmp;
            if ((s32)tmp > 5) break;
            tmp = 0x0;
            *(u16*)((u8*)r4 + 0x0) = tmp;
        } while (0);
        if ((s32)r8 == (s32)-0x1) {
        }
        r5 = *(u8*)((u8*)r4 + 0x1C);
        r8 = r8 + 0x1;
        tmp = *(u8*)((u8*)r4 + 0x1D);
        r5 = (s8)r5;
        r6 = *(u8*)((u8*)r4 + 0x1B);
        tmp = (s8)tmp;
        tmp = r5 * tmp;
        r6 = (s8)r6;
        r5 = tmp << 4;
        tmp = r5 + 0x76;
        tmp = r6 * tmp;
        r4 = tmp + r4;
        r4 = r4 + 0x24;
    } while (1);

    tmp = (u32)sp + 0x8;
    if (tmp != 0) {
    }
    r4 = (s8)r28;
    tmp = (s8)r27;
    r4 = r4 * tmp;
    r5 = (s8)r29;
    tmp = r3 + 0x4000;
    r3 = r4 << 4;
    r3 = r3 + 0x76;
    r3 = r5 * r3;
    r4 = r3 + r6;
    r4 = r4 + 0x24;
    if (tmp < r4) {
        r29 = 0x0;
    } else {

        r3 = r6;
        r5 = r4 - r6;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    }
    r26 = r29;
    if (r29 == 0) { r3 = r26; return; }
    r3 = r29;
    r4 = r31 + 0xa;
    ((void(*)(void))fn_800CAA3C)();
    tmp = *(u8*)((u8*)r31 + 0x8);
    r27 = r31;
    r28 = 0x0;
    *(u8*)((u8*)r29 + 0x1A) = tmp;
    tmp = *(u8*)((u8*)r31 + 0x58);
    *(u8*)((u8*)r29 + 0x1B) = tmp;
    tmp = *(u8*)((u8*)r31 + 0x59);
    *(u8*)((u8*)r29 + 0x1C) = tmp;
    tmp = *(u8*)((u8*)r31 + 0x5A);
    *(u8*)((u8*)r29 + 0x1D) = tmp;
    while (1) {
        tmp = *(u8*)((u8*)r29 + 0x1B);
        tmp = (s8)tmp;
        if ((s32)r28 >= (s32)tmp) break;
        if (r29 == 0) {
            r3 = (u32)&lbl_8026F1C8;
            r4 = 0x17f;
            r3 = (u32)&lbl_8026F1C8;
            r5 = (u32)&lbl_8047C180;
            ((void(*)(void))__assert)();
        }
        r3 = (s8)r28;
        r4 = 0x0;
        if ((s32)r3 >= 0) {
            tmp = *(u8*)((u8*)r29 + 0x1B);
            tmp = (s8)tmp;
            if ((s32)r3 < (s32)tmp) {
                r4 = 0x1;
        }
        }
        if ((s32)r4 == 0) {
            r3 = (u32)&lbl_8026F1C8;
            r5 = (u32)&lbl_8026F1D8;
            r3 = (u32)&lbl_8026F1C8;
            r4 = 0x180;
            r5 = (u32)&lbl_8026F1D8;
            ((void(*)(void))__assert)();
        }
        r3 = *(u8*)((u8*)r29 + 0x1C);
        r5 = (s8)r28;
        tmp = *(u8*)((u8*)r29 + 0x1D);
        r4 = r27 + 0x28;
        r3 = (s8)r3;
        tmp = (s8)tmp;
        tmp = r3 * tmp;
        r3 = tmp << 4;
        tmp = r3 + 0x76;
        tmp = tmp * r5;
        r3 = r29 + tmp;
        r3 = r3 + 0x24;
        ((void(*)(void))fn_800CAA3C)();
        r27 = r27 + 0x10;
        r28 = r28 + 0x1;

    }

    r3 = r26;

    return;
}
