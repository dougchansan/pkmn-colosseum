/**
 * @file menu_common_ext.c
 * @brief Menu common extensions - helpers, draw, input (0x8007109C-0x80072A00)
 *
 * Address range: 0x8007109C - 0x80072A00
 * Total functions: 23
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8006A7E8();
extern void fn_8006B154();
extern void fn_80073A44();
extern void fn_80073C38();
extern void fn_8008ABA0();
extern void fn_8008ABE4();
extern void fn_800A13F8();
extern void fn_800A1F94();
extern void fn_800A221C();
extern void fn_800D0F44();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void fn_800F7AF0();
extern void fn_800F7BC4();
extern void fn_80102568();
extern void fn_801026A4();
extern void fn_801046B8();
extern void fn_80104704();
extern void fn_80113828();
extern void fn_80129280();
extern void __assert();
extern void fn_8025F3F4();
extern void fn_8025F584();
extern void fn_8025F648();
extern void OSCreateAlarm();
extern void OSDisableInterrupts();
extern void OSGetTick();
extern void OSRestoreInterrupts();
extern void OSSetAlarm();

/* ===== SDA globals ===== */
extern u8 lbl_8047A600;
extern u8 lbl_8047C090;

/* ===== Rodata / data labels ===== */
extern u8 lbl_80268708[];
extern u8 lbl_80268718[];
extern u8 lbl_80268750[];
extern u8 lbl_803B6D88[];
extern u8 lbl_803B6DE0[];
extern u8 lbl_803B6E08[];
extern u8 lbl_803B6E18[];

/* ===== Forward declarations ===== */
void fn_8007109C(void);
void fn_80071104(void);
void fn_80071160(void);
void fn_80071208(void);
void fn_80071318(u8* dst, u8* src);
void fn_80071344(void);
void fn_80071398(void);
void fn_800714C8(void);
void fn_800715BC(void);
s32 fn_8007162C(void);
void fn_80071644(void);
s32 fn_8007169C(void);
s32 fn_800716C8(void);
s32 fn_800716E8(s32 channel, s32 value);
void fn_80071700(void);
void fn_800719A8(void);
void fn_80071AE4(void);
void fn_80071E34(void);
void fn_80071EA4(void);
void fn_800722A0(void);
void fn_80072548(void);
void fn_80072684(void);
void fn_800726A8(void);

/* ===== Function implementations ===== */


/* 0x8007109C | size: 0x68 */
void fn_8007109C(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    ((void(*)(void))fn_800E202C)();
    r31 = r3;
    r0 = r31 & 0xFFFF;
    if (r0 == 0x0) {
        r3 = (u32)&lbl_80268708;
        r4 = 0xde;
        r3 = (u32)&lbl_80268708;
        r5 = (u32)&lbl_8047C090;
        ((void(*)(void))__assert)();
    }
    r0 = r31 & 0xFFFF;
    if (r0 != 0x0) {
        r3 = r31;
        ((void(*)(void))fn_800E24B0)();
        r3 = r31;
        ((void(*)(void))fn_800E209C)();
    }
    return;
}

/* 0x80071104 | size: 0x5C */
void fn_80071104(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    tmp = r3 + 0x1f;
    r4 = 0x20;
    /* clrrwi r3, tmp, 5 */;
    ((void(*)(void))fn_800E2C04)();
    tmp = r3 & 0xFFFF;
    if (tmp != 0) {
        ((void(*)(void))fn_800E27B0)();
    } else {

        if (tmp == 0) {
            r3 = (u32)&lbl_80268708;
            r4 = 0xd5;
            r3 = (u32)&lbl_80268708;
            r5 = (u32)&lbl_8047C090;
            ((void(*)(void))__assert)();
        }
        r3 = 0x0;
    }
    return;
}

/* 0x80071160 | size: 0xA8 */
void fn_80071160(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = 0x0;
    r31 = 0x0;
    do {
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        tmp = r31 + 0x59cc;
        r29 = *(u32*)(r3 + tmp);
        if ((s32)r29 != 0) {
            r3 = 0x0;
            r4 = 0xe;
            ((void(*)(void))fn_80129280)();
            tmp = r31 + 0x59a8;
            r3 = r3 + tmp;
            ((void(*)(void))fn_8006A7E8)();
            if ((s32)r3 != 0) {
                r3 = r29;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    r3 = r29;
                    return;
        }
        }
        }
        r31 = r31 + 0x1660;
        r30 = r30 + 0x1;
    } while (r30 < 4);
    r3 = 0x0;

    return;
}

/* 0x80071208 | size: 0x110 */
void fn_80071208(void) {
    ((void(*)(void))fn_800F7AF0)();
    ((void(*)(void))fn_800F7BC4)();
    ((void(*)(void))fn_8008ABA0)();
}

/* 0x80071318 | size: 0x2C */
#pragma optimization_level 2
void fn_80071318(u8* dst, u8* src) {
    s16 upperX;
    s16 upperY;
    u32 entryWord;
    s16 lowerX;
    s16 lowerY;

    upperX = *(s16*)(src + 0xC);
    upperY = *(s16*)(src + 0xE);
    *(s16*)(dst + 0x54) = upperX;
    entryWord = *(u32*)(src + 0x10);
    *(s16*)(dst + 0x56) = upperY;
    lowerX = *(s16*)(src + 0x8);
    *(u32*)(dst + 0x58) = entryWord;
    lowerY = *(s16*)(src + 0xA);
    *(s16*)(dst + 0x5C) = lowerX;
    *(s16*)(dst + 0x5E) = lowerY;
}
#pragma optimization_level 4

/* 0x80071344 | size: 0x54 */
void fn_80071344(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

    r3 = (u32)&lbl_803B6D88;
    r4 = 0x0;
    r3 = (u32)&lbl_803B6D88;
    r6 = 0x10;
    tmp = *(u32*)((u8*)r3 + 0x40);
    r7 = 0x1;
    r8 = 0x0;
    r9 = 0x0;
    tmp = tmp << 3;
    r5 = r3 + tmp;
    r3 = *(u32*)((u8*)r5 + 0x0);
    r5 = r5 + 0x4;
    ((void(*)(void))fn_801026A4)();
    return;
}

/* 0x80071398 | size: 0x130 */
void fn_80071398(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r3 = (u32)&lbl_803B6D88;
    r31 = (u32)&lbl_803B6D88;
    tmp = *(u32*)((u8*)r31 + 0x40);
    tmp = tmp << 3;
    r29 = *(u32*)(r31 + tmp);
    ((void(*)(void))fn_801046B8)();
    if ((s32)r3 == (s32)r29) {
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = (u32)&lbl_803B6D88;
        r3 = (u32)&lbl_803B6D88;
        r4 = 0x0;
        tmp = tmp << 3;
        r5 = 0x0;
        r3 = *(u32*)(r3 + tmp);
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0xbe;
    ((void(*)(void))fn_80104704)();
    if (r3 != 0) {
        r3 = 0xbe;
        r4 = 0x0;
        r5 = 0x1;
        ((void(*)(void))fn_80102568)();
    }
    tmp = *(u32*)((u8*)r31 + 0x40);
    r3 = (u32)&lbl_803B6D88;
    r3 = (u32)&lbl_803B6D88;
    r4 = 0x0;
    tmp = tmp << 3;
    r3 = r3 + tmp;
    *(u32*)((u8*)r3 + 0x4) = r4;
    tmp = *(u32*)((u8*)r31 + 0x40);
    if ((s32)tmp != 0) {
        if ((s32)tmp <= 0) {
            r3 = (u32)&lbl_80268708;
            r5 = (u32)&lbl_80268718;
            r3 = (u32)&lbl_80268708;
            r4 = 0x5c;
            r5 = (u32)&lbl_80268718;
            ((void(*)(void))__assert)();
        }
        r3 = *(u32*)((u8*)r31 + 0x40);
        *(u32*)((u8*)r31 + 0x40) = tmp;
    }
    r3 = (u32)&lbl_803B6D88;
    r3 = (u32)&lbl_803B6D88;
    while ((s32)r30 != (s32)tmp) {
            r4 = *(u32*)((u8*)r31 + 0x40);
            tmp = r4 << 3;
            tmp = *(u32*)(r3 + tmp);
            if ((s32)r4 == 0) break;
            r4 = *(u32*)((u8*)r31 + 0x40);
            *(u32*)((u8*)r31 + 0x40) = tmp;
    }

    tmp = *(u32*)((u8*)r31 + 0x40);
    r3 = (u32)&lbl_803B6D88;
    r3 = (u32)&lbl_803B6D88;
    tmp = tmp << 3;
    r3 = *(u32*)(r3 + tmp);
    return;
}

/* 0x800714C8 | size: 0xF4 */
void fn_800714C8(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r3 = (u32)&lbl_803B6D88;
    r31 = (u32)&lbl_803B6D88;
    tmp = *(u32*)((u8*)r31 + 0x40);
    tmp = tmp << 3;
    r30 = *(u32*)(r31 + tmp);
    ((void(*)(void))fn_801046B8)();
    if ((s32)r3 == (s32)r30) {
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = (u32)&lbl_803B6D88;
        r3 = (u32)&lbl_803B6D88;
        r4 = 0x0;
        tmp = tmp << 3;
        r5 = 0x0;
        r3 = *(u32*)(r3 + tmp);
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0xbe;
    ((void(*)(void))fn_80104704)();
    if (r3 != 0) {
        r3 = 0xbe;
        r4 = 0x0;
        r5 = 0x1;
        ((void(*)(void))fn_80102568)();
    }
    tmp = *(u32*)((u8*)r31 + 0x40);
    r3 = (u32)&lbl_803B6D88;
    r3 = (u32)&lbl_803B6D88;
    r4 = 0x0;
    tmp = tmp << 3;
    r3 = r3 + tmp;
    *(u32*)((u8*)r3 + 0x4) = r4;
    tmp = *(u32*)((u8*)r31 + 0x40);
    if ((s32)tmp == 0) {
        r3 = -0x1;
    } else {

        if ((s32)tmp <= 0) {
            r3 = (u32)&lbl_80268708;
            r5 = (u32)&lbl_80268718;
            r3 = (u32)&lbl_80268708;
            r4 = 0x5c;
            r5 = (u32)&lbl_80268718;
            ((void(*)(void))__assert)();
        }
        r4 = *(u32*)((u8*)r31 + 0x40);
        r3 = (u32)&lbl_803B6D88;
        r3 = (u32)&lbl_803B6D88;
        *(u32*)((u8*)r31 + 0x40) = tmp;
        tmp = tmp << 3;
        r3 = *(u32*)(r3 + tmp);
    }
    return;
}

/* 0x800715BC | size: 0x70 */
void fn_800715BC(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    r4 = (u32)&lbl_803B6D88;
    r5 = (u32)&lbl_803B6D88;
    r4 = *(u32*)((u8*)r5 + 0x40);
    if (r4 >= 8) {
        r3 = (u32)&lbl_80268708;
        r5 = (u32)&lbl_80268750;
        r3 = (u32)&lbl_80268708;
        r4 = 0x41;
        r5 = (u32)&lbl_80268750;
        ((void(*)(void))__assert)();
    } else {

        tmp = r4 + 0x1;
        r4 = 0x0;
        *(u32*)((u8*)r5 + 0x40) = tmp;
        tmp = tmp << 3;
        *(u32*)(r5 + tmp) = r3;
        tmp = *(u32*)((u8*)r5 + 0x40);
        tmp = tmp << 3;
        r3 = r5 + tmp;
        *(u32*)((u8*)r3 + 0x4) = r4;
    }
    return;
}

/* 0x8007162C | size: 0x18 */
s32 fn_8007162C(void) {
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    return *(s32*)(lbl_803B6D88 + depth * 8);
}

/* 0x80071644 | size: 0x58 */
void fn_80071644(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    r4 = (u32)&lbl_803B6D88;
    tmp = 0x0;
    r4 = (u32)&lbl_803B6D88;
    *(u32*)((u8*)r4 + 0x0) = tmp;
    *(u32*)((u8*)r4 + 0x4) = tmp;
    *(u32*)((u8*)r4 + 0x8) = tmp;
    *(u32*)((u8*)r4 + 0xC) = tmp;
    *(u32*)((u8*)r4 + 0x10) = tmp;
    *(u32*)((u8*)r4 + 0x14) = tmp;
    *(u32*)((u8*)r4 + 0x18) = tmp;
    *(u32*)((u8*)r4 + 0x1C) = tmp;
    *(u32*)((u8*)r4 + 0x20) = tmp;
    *(u32*)((u8*)r4 + 0x24) = tmp;
    *(u32*)((u8*)r4 + 0x28) = tmp;
    *(u32*)((u8*)r4 + 0x2C) = tmp;
    *(u32*)((u8*)r4 + 0x30) = tmp;
    *(u32*)((u8*)r4 + 0x34) = tmp;
    *(u32*)((u8*)r4 + 0x38) = tmp;
    *(u32*)((u8*)r4 + 0x3C) = tmp;
    *(u32*)((u8*)r4 + 0x0) = r3;
    *(u32*)((u8*)r4 + 0x40) = tmp;
    return;
}

/* 0x8007169C | size: 0x2C */
#pragma push
#pragma peephole off
s32 fn_8007169C(void) {
    extern void fn_80113828(s32, s32);
    fn_80113828(0x385, 0);
    return 0;
}
#pragma pop

/* 0x800716C8 | size: 0x20 */
s32 fn_800716C8(void) {
    return 0;
}

/* 0x800716E8 | size: 0x18 */
s32 fn_800716E8(s32 channel, s32 value) {
    *(s32*)(lbl_803B6E08 + (u32)channel * 4) = value;
    return 0;
}

/* 0x80071700 | size: 0x2A8 */
void fn_80071700(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r30 = r3;
    r31 = r30 + 0x1;
    r4 = 0x2;
    r3 = r31;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r30;
    ((void(*)(void))fn_80073C38)();
    if ((s32)r3 != 0) {
        r27 = r3;
    } else {
        tmp = 0x44;
        r3 = r30;
        *(u32*)(sp + 0xC) = tmp;
        r4 = (u32)sp + 0xc;
        r5 = (u32)sp + 0xa;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) {
            r27 = 0xb;
        } else {
            r4 = 0x80000000;
            r3 = 0x10620000;
            tmp = *(u32*)((u8*)r4 + 0xF8);
            r3 = r3 + 0x4dd3;
            tmp = (u32)tmp >> 2;
            tmp = (u32)((u64)r3 * (u64)tmp >> 32);
            tmp = (u32)tmp >> 6;
            r26 = tmp * 0x64;
            OSGetTick();
            r5 = (u32)&lbl_803B6E18;
            r4 = (u32)&lbl_803B6E08;
            r6 = r30 << 3;
            r29 = r3;
            tmp = (u32)&lbl_803B6E18;
            r28 = r30 << 2;
            r24 = tmp + r6;
            r27 = (u32)&lbl_803B6E08;
            r25 = r24 + 0x4;
            while (1) {
                OSGetTick();
                tmp = r3 - r29;
                if (tmp > r26) {
                    r3 = 0x1;
                    break;
                }
                r3 = r30;
                r4 = (u32)sp + 0x8;
                ((void(*)(void))fn_8025F3F4)();
                if ((s32)r3 != 0) {
                    r3 = 0x2;
                    break;
                }
                tmp = *(u8*)(sp + 0x8);
                tmp = tmp & 0xa;
                if ((s32)tmp == 8) {
                    r3 = r30;
                    r4 = (u32)sp + 0x10;
                    r5 = (u32)sp + 0xa;
                    ((void(*)(void))fn_8025F584)();
                    if ((s32)r3 != 0) {
                        r3 = 0x3;
                    } else {
                        r3 = 0x0;
                    }
                    break;
                }
                r12 = *(u32*)((u8*)r24 + 0x0);
                if (r12 != 0) {
                    r3 = r30;
                    r4 = *(u32*)((u8*)r25 + 0x0);
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                }
                tmp = *(u32*)(r27 + r28);
                if ((s32)tmp != 0) {
                    r3 = 0x3e8;
                    break;
                }
            }
            if ((s32)r3 != 0) {
                r27 = r3 + 0xb;
            } else {
                r27 = 0x0;
            }
        }
        if ((s32)r27 == 0) {
            tmp = (u32)tmp >> 24;
            if (tmp != 0x44) {
                r27 = 0xf;
            } else {
                r4 = 0x80000000;
                r3 = 0x10620000;
                tmp = *(u32*)((u8*)r4 + 0xF8);
                r3 = r3 + 0x4dd3;
                tmp = (u32)tmp >> 2;
                tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                tmp = (u32)tmp >> 6;
                r26 = tmp * 0x7530;
                OSGetTick();
                r5 = (u32)&lbl_803B6E18;
                r4 = (u32)&lbl_803B6E08;
                r6 = r30 << 3;
                r27 = r3;
                tmp = (u32)&lbl_803B6E18;
                r28 = r30 << 2;
                r24 = tmp + r6;
                r29 = (u32)&lbl_803B6E08;
                r25 = r24 + 0x4;
                while (1) {
                    OSGetTick();
                    tmp = r3 - r27;
                    if (tmp > r26) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r30;
                    r4 = (u32)sp + 0x9;
                    ((void(*)(void))fn_8025F3F4)();
                    if ((s32)r3 != 0) {
                        r3 = 0x2;
                        break;
                    }
                    tmp = *(u8*)(sp + 0x9);
                    tmp = tmp & 0xa;
                    if ((s32)tmp == 8) {
                        r3 = r30;
                        r4 = (u32)sp + 0x10;
                        r5 = (u32)sp + 0xb;
                        ((void(*)(void))fn_8025F584)();
                        if ((s32)r3 != 0) {
                            r3 = 0x3;
                        } else {
                            r3 = 0x0;
                        }
                        break;
                    }
                    r12 = *(u32*)((u8*)r24 + 0x0);
                    if (r12 != 0) {
                        r3 = r30;
                        r4 = *(u32*)((u8*)r25 + 0x0);
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                    }
                    tmp = *(u32*)(r29 + r28);
                    if ((s32)tmp != 0) {
                        r3 = 0x3e8;
                        break;
                    }
                }
                if ((s32)r3 != 0) {
                    r27 = r3 + 0xf;
                } else if (tmp != 0) {
                    r27 = 0x13;
                } else {
                    r27 = 0x0;
                }
            }
        }
    }
    r3 = r31;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r27;
    return;
}

/* 0x800719A8 | size: 0x13C */
void fn_800719A8(void) {
    extern void fn_80072684();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;

    r31 = r3;
    ((void(*)(void))fn_800A13F8)();
    r4 = (u32)&lbl_803B6DE0;
    *(u32*)&lbl_8047A600 = r3;
    r3 = (u32)&lbl_803B6DE0;
    OSCreateAlarm();
    OSDisableInterrupts();
    r4 = 0x80000000;
    r5 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r4 = (u32)fn_80072684;
    r5 = r5 + 0x4dd3;
    r6 = (u32)&lbl_803B6DE0;
    tmp = (u32)tmp >> 2;
    r7 = (u32)fn_80072684;
    tmp = (u32)((u64)r5 * (u64)tmp >> 32);
    r5 = (u32)&lbl_803B6DE0;
    r30 = r3;
    r3 = r5;
    r5 = 0x0;
    r6 = (u32)tmp >> 6;
    OSSetAlarm();
    r3 = *(u32*)&lbl_8047A600;
    ((void(*)(void))fn_800A221C)();
    r3 = r30;
    OSRestoreInterrupts();
    r3 = r31;
    ((void(*)(void))fn_800D0F44)();
    /* subis tmp, r3, 0x4 */;
    if (tmp != 0) {
        r30 = 0x1;
    } else {
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r30 = 0x2;
        } else {
            tmp = *(u8*)(sp + 0x8);
            tmp = tmp & 0x00000008;
            if ((s32)tmp == 0) {
                r30 = -0x1;
            } else {
                r3 = r31;
                r4 = (u32)sp + 0xc;
                r5 = (u32)sp + 0x8;
                ((void(*)(void))fn_8025F584)();
                if ((s32)r3 != 0) {
                    r30 = 0x3;
                } else if (tmp != 0) {
                    r30 = 0x4;
                } else {
                    r30 = 0x0;
                }
            }
        }
    }
    if ((s32)r30 != 0) {
        if ((s32)r30 < 3) { r3 = r30; return; }
    }
    r3 = r31 + 0x1;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();

    r3 = r30;
    return;
}

/* 0x80071AE4 | size: 0x350 */
void fn_80071AE4(void) {
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r15 = r3;
    r16 = r4;
    tmp = r15 + 0x1;
    r4 = 0x2;
    *(u32*)(sp + 0x18) = tmp;
    r3 = tmp;
    ((void(*)(void))fn_8008ABE4)();
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r18 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r15 << 3;
    r25 = r3;
    tmp = (u32)&lbl_803B6E18;
    r26 = r15 << 2;
    r19 = tmp + r6;
    r27 = (u32)&lbl_803B6E08;
    r20 = r19 + 0x4;
    r5 = *(u32*)((u8*)r16 + 0xC);
    r3 = (u32)r5 >> 24;
    tmp = ((r5 << 24) | (r5 >> 8)) & 0x0000FF00;
    r4 = ((r5 << 8) | (r5 >> 24)) & 0x00FF0000;
    r5 = r5 << 24;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    r14 = (u32)tmp >> 16;
    r23 = tmp & 0xFFFF;
L_80071B80:
do {
    OSGetTick();
    r4 = r3 - r25;
    r3 = r15;
    tmp = r4 ^ r18;
    tmp = __cntlzw(tmp);
    tmp = r4 << tmp;
    r17 = (u32)tmp >> 31;
    ((void(*)(void))fn_80073C38)();
    if ((s32)r3 != 0) {
        r21 = r3;
        break;
    }
    tmp = 0x22;
    r3 = r15;
    *(u32*)(sp + 0x10) = tmp;
    r4 = (u32)sp + 0x10;
    r5 = (u32)sp + 0xa;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r21 = 0xb;
        goto L_80071CAC;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r22 = tmp * 0x64;
    OSGetTick();
    r21 = r3;
L_80071C00:
    OSGetTick();
    tmp = r3 - r21;
    if (tmp > r22) {
        r3 = 0x1;
        goto L_80071C98;
    }
    r3 = r15;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F3F4)();
    if ((s32)r3 != 0) {
        r3 = 0x2;
        goto L_80071C98;
    }
    tmp = *(u8*)(sp + 0x8);
    tmp = tmp & 0xa;
    if ((s32)tmp != 8) {
        r12 = *(u32*)((u8*)r19 + 0x0);
        if (r12 != 0) {
            r3 = r15;
            r4 = *(u32*)((u8*)r20 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r27 + r26);
        if ((s32)tmp == 0) goto L_80071C00;
        r3 = 0x3e8;
        goto L_80071C98;
    }
    r3 = r15;
    r4 = (u32)sp + 0x14;
    r5 = (u32)sp + 0xa;
    ((void(*)(void))fn_8025F584)();
    if ((s32)r3 != 0) {
        r3 = 0x3;
        goto L_80071C98;
    }
    r3 = 0x0;
L_80071C98:
    if ((s32)r3 != 0) {
        r21 = r3 + 0xb;
        goto L_80071CAC;
    }
    r21 = 0x0;
L_80071CAC:
do {
    if ((s32)r21 != 0) {
        break;
    }
    tmp = (u32)tmp >> 24;
    if (tmp != 0x22) {
        r21 = 0xf;
        break;
    }
    tmp = r14 & 0xFFFF;
    r3 = r23 << 2;
    r24 = r3 + 0x10;
    if (tmp != 0) {
        r24 = r24 + 0x70;
    }
    r22 = 0x0;
    r3 = 0x10620000;
    r31 = r16;
    r28 = r3 + 0x4dd3;
    r29 = 0x80000000;
    while ((s32)r22 < (s32)r24) {

        tmp = *(u32*)((u8*)r31 + 0x0);
        r3 = r15;
        r4 = (u32)sp + 0xc;
        r5 = (u32)sp + 0x9;
        *(u32*)(sp + 0xC) = tmp;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) {
            r21 = 0x10;
            break;
        }
        tmp = *(u32*)((u8*)r29 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r28 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r21 = tmp * 0x64;
        OSGetTick();
        r30 = r3;
    L_80071D44:
        OSGetTick();
        tmp = r3 - r30;
        if (tmp > r21) {
            r21 = 0x11;
            break;
        }
        r3 = r15;
        r4 = (u32)sp + 0x9;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r21 = 0x12;
            break;
        }
        tmp = *(u8*)(sp + 0x9);
        tmp = tmp & 0x00000002;
        if ((s32)tmp != 0) {
            r12 = *(u32*)((u8*)r19 + 0x0);
            if (r12 != 0) {
                r3 = r15;
                r4 = *(u32*)((u8*)r20 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r27 + r26);
            if ((s32)tmp == 0) goto L_80071D44;
            r21 = 0x3e8;
            break;
        }
        tmp = *(u32*)(r27 + r26);
        if ((s32)tmp != 0) {
            r21 = 0x3e8;
            break;
        }
        r22 = r22 + 0x4;
        r31 = r31 + 0x4;

    }
    r21 = 0x0;
} while (0);
    if ((s32)r21 != 0) {
        break;
    }
    r21 = 0x0;
} while (0);
    if ((s32)r21 == 1) {
        if ((s32)r17 == 0) goto L_80071B80;
    }
    if ((s32)r21 != 0) {
        r4 = 0x1;
    } else {

        r4 = 0x3;
    }
    ((void(*)(void))fn_8008ABE4)();
    r3 = r21;
    return;
}

/* 0x80071E34 | size: 0x70 */
void fn_80071E34(void) {
    extern void fn_80071EA4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    r31 = r29 + 0x1;
    r4 = 0x2;
    r3 = r31;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r29;
    r4 = r30;
    fn_80071EA4();
    tmp = r3;
    r3 = r31;
    r31 = tmp;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r31;
    return;
}

/* 0x80071EA4 | size: 0x3FC */
void fn_80071EA4(void) {
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r28 = r3;
    r29 = r4;
    ((void(*)(void))fn_80073C38)();
    if ((s32)r3 != 0) {
        return;
    }
    tmp = 0x33;
    r3 = r28;
    *(u32*)(sp + 0x18) = tmp;
    r4 = (u32)sp + 0x18;
    r5 = (u32)sp + 0xd;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0xb;
        goto L_80071FE8;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r24 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r28 << 3;
    r23 = r3;
    tmp = (u32)&lbl_803B6E18;
    r22 = r28 << 2;
    r25 = tmp + r6;
    r21 = (u32)&lbl_803B6E08;
    r20 = r25 + 0x4;
L_80071F3C:
    OSGetTick();
    tmp = r3 - r23;
    if (tmp > r24) {
        r3 = 0x1;
        goto L_80071FD4;
    }
    r3 = r28;
    r4 = (u32)sp + 0xa;
    ((void(*)(void))fn_8025F3F4)();
    if ((s32)r3 != 0) {
        r3 = 0x2;
        goto L_80071FD4;
    }
    tmp = *(u8*)(sp + 0xA);
    tmp = tmp & 0xa;
    if ((s32)tmp != 8) {
        r12 = *(u32*)((u8*)r25 + 0x0);
        if (r12 != 0) {
            r3 = r28;
            r4 = *(u32*)((u8*)r20 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r21 + r22);
        if ((s32)tmp == 0) goto L_80071F3C;
        r3 = 0x3e8;
        goto L_80071FD4;
    }
    r3 = r28;
    r4 = (u32)sp + 0x1c;
    r5 = (u32)sp + 0xd;
    ((void(*)(void))fn_8025F584)();
    if ((s32)r3 != 0) {
        r3 = 0x3;
        goto L_80071FD4;
    }
    r3 = 0x0;
L_80071FD4:
    if ((s32)r3 != 0) {
        r3 = r3 + 0xb;
        goto L_80071FE8;
    }
    r3 = 0x0;
L_80071FE8:
    if ((s32)r3 != 0) {
        return;
    }
    tmp = (u32)tmp >> 24;
    if (tmp != 0x33) {
        r3 = 0xf;
        return;
    }
    r4 = (u32)&lbl_803B6E18;
    r3 = (u32)&lbl_803B6E08;
    r5 = r28 << 3;
    r27 = r28 << 2;
    tmp = (u32)&lbl_803B6E18;
    r26 = (u32)&lbl_803B6E08;
    r31 = tmp + r5;
    r23 = 0x0;
    r30 = r31 + 0x4;
    r3 = 0x10620000;
    r22 = r29;
    r25 = r3 + 0x4dd3;
    r24 = 0x80000000;
    do {
        tmp = *(u32*)((u8*)r24 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r25 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r20 = tmp * 0x64;
        OSGetTick();
        r21 = r3;
    L_8007205C:
        OSGetTick();
        tmp = r3 - r21;
        if (tmp > r20) {
            r3 = 0x1;
            goto L_800720F4;
        }
        r3 = r28;
        r4 = (u32)sp + 0x9;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x2;
            goto L_800720F4;
        }
        tmp = *(u8*)(sp + 0x9);
        tmp = tmp & 0xa;
        if ((s32)tmp != 8) {
            r12 = *(u32*)((u8*)r31 + 0x0);
            if (r12 != 0) {
                r3 = r28;
                r4 = *(u32*)((u8*)r30 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r26 + r27);
            if ((s32)tmp == 0) goto L_8007205C;
            r3 = 0x3e8;
            goto L_800720F4;
        }
        r3 = r28;
        r4 = (u32)sp + 0x14;
        r5 = (u32)sp + 0xc;
        ((void(*)(void))fn_8025F584)();
        if ((s32)r3 != 0) {
            r3 = 0x3;
            goto L_800720F4;
        }
        r3 = 0x0;
    L_800720F4:
        if ((s32)r3 != 0) {
            goto L_80072130;
        }
        *(u32*)((u8*)r22 + 0x0) = tmp;
        tmp = *(u32*)(r26 + r27);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            goto L_80072130;
        }
        r23 = r23 + 0x4;
        r22 = r22 + 0x4;
    } while ((s32)r23 < 0x10);
    r3 = 0x0;
L_80072130:
    if ((s32)r3 != 0) {
        r3 = r3 + 0xf;
        return;
    }
    r5 = *(u32*)((u8*)r29 + 0xC);
    r6 = (u32)&lbl_803B6E08;
    r24 = r28 << 2;
    r23 = 0x0;
    r3 = (u32)r5 >> 24;
    tmp = ((r5 << 24) | (r5 >> 8)) & 0x0000FF00;
    r4 = ((r5 << 8) | (r5 >> 24)) & 0x00FF0000;
    r5 = r5 << 24;
    tmp = r3 | tmp;
    r25 = (u32)&lbl_803B6E08;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    tmp = tmp & 0xFFFF;
    r22 = tmp << 2;
    r3 = 0x10620000;
    r27 = 0x80000000;
    r26 = r3 + 0x4dd3;
    while ((s32)r23 < (s32)r22) {

        tmp = *(u32*)((u8*)r27 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r26 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r20 = tmp * 0x64;
        OSGetTick();
        r21 = r3;
    L_800721A4:
        OSGetTick();
        tmp = r3 - r21;
        if (tmp > r20) {
            r3 = 0x1;
            goto L_8007223C;
        }
        r3 = r28;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x2;
            goto L_8007223C;
        }
        tmp = *(u8*)(sp + 0x8);
        tmp = tmp & 0xa;
        if ((s32)tmp != 8) {
            r12 = *(u32*)((u8*)r31 + 0x0);
            if (r12 != 0) {
                r3 = r28;
                r4 = *(u32*)((u8*)r30 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r25 + r24);
            if ((s32)tmp == 0) goto L_800721A4;
            r3 = 0x3e8;
            goto L_8007223C;
        }
        r3 = r28;
        r4 = (u32)sp + 0x10;
        r5 = (u32)sp + 0xb;
        ((void(*)(void))fn_8025F584)();
        if ((s32)r3 != 0) {
            r3 = 0x3;
            goto L_8007223C;
        }
        r3 = 0x0;
    L_8007223C:
        if ((s32)r3 != 0) {
            goto L_80072278;
        }
        r3 = r29 + r23;
        *(u32*)((u8*)r3 + 0x10) = tmp;
        tmp = *(u32*)(r25 + r24);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            goto L_80072278;
        }
        r23 = r23 + 0x4;

    }
    r3 = 0x0;
L_80072278:
    if ((s32)r3 != 0) {
        r3 = r3 + 0x12;
        return;
    }
    r3 = 0x0;

    return;
}

/* 0x800722A0 | size: 0x2A8 */
void fn_800722A0(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r30 = r3;
    r31 = r30 + 0x1;
    r4 = 0x2;
    r3 = r31;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r30;
    ((void(*)(void))fn_80073C38)();
    if ((s32)r3 != 0) {
        r27 = r3;
    } else {
        tmp = 0x44;
        r3 = r30;
        *(u32*)(sp + 0xC) = tmp;
        r4 = (u32)sp + 0xc;
        r5 = (u32)sp + 0xa;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) {
            r27 = 0xb;
        } else {
            r4 = 0x80000000;
            r3 = 0x10620000;
            tmp = *(u32*)((u8*)r4 + 0xF8);
            r3 = r3 + 0x4dd3;
            tmp = (u32)tmp >> 2;
            tmp = (u32)((u64)r3 * (u64)tmp >> 32);
            tmp = (u32)tmp >> 6;
            r26 = tmp * 0x64;
            OSGetTick();
            r5 = (u32)&lbl_803B6E18;
            r4 = (u32)&lbl_803B6E08;
            r6 = r30 << 3;
            r29 = r3;
            tmp = (u32)&lbl_803B6E18;
            r28 = r30 << 2;
            r24 = tmp + r6;
            r27 = (u32)&lbl_803B6E08;
            r25 = r24 + 0x4;
            while (1) {
                OSGetTick();
                tmp = r3 - r29;
                if (tmp > r26) {
                    r3 = 0x1;
                    break;
                }
                r3 = r30;
                r4 = (u32)sp + 0x8;
                ((void(*)(void))fn_8025F3F4)();
                if ((s32)r3 != 0) {
                    r3 = 0x2;
                    break;
                }
                tmp = *(u8*)(sp + 0x8);
                tmp = tmp & 0xa;
                if ((s32)tmp == 8) {
                    r3 = r30;
                    r4 = (u32)sp + 0x10;
                    r5 = (u32)sp + 0xa;
                    ((void(*)(void))fn_8025F584)();
                    if ((s32)r3 != 0) {
                        r3 = 0x3;
                    } else {
                        r3 = 0x0;
                    }
                    break;
                }
                r12 = *(u32*)((u8*)r24 + 0x0);
                if (r12 != 0) {
                    r3 = r30;
                    r4 = *(u32*)((u8*)r25 + 0x0);
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                }
                tmp = *(u32*)(r27 + r28);
                if ((s32)tmp != 0) {
                    r3 = 0x3e8;
                    break;
                }
            }
            if ((s32)r3 != 0) {
                r27 = r3 + 0xb;
            } else {
                r27 = 0x0;
            }
        }
        if ((s32)r27 == 0) {
            tmp = (u32)tmp >> 24;
            if (tmp != 0x44) {
                r27 = 0xf;
            } else {
                r4 = 0x80000000;
                r3 = 0x10620000;
                tmp = *(u32*)((u8*)r4 + 0xF8);
                r3 = r3 + 0x4dd3;
                tmp = (u32)tmp >> 2;
                tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                tmp = (u32)tmp >> 6;
                r26 = tmp * 0x7530;
                OSGetTick();
                r5 = (u32)&lbl_803B6E18;
                r4 = (u32)&lbl_803B6E08;
                r6 = r30 << 3;
                r27 = r3;
                tmp = (u32)&lbl_803B6E18;
                r28 = r30 << 2;
                r24 = tmp + r6;
                r29 = (u32)&lbl_803B6E08;
                r25 = r24 + 0x4;
                while (1) {
                    OSGetTick();
                    tmp = r3 - r27;
                    if (tmp > r26) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r30;
                    r4 = (u32)sp + 0x9;
                    ((void(*)(void))fn_8025F3F4)();
                    if ((s32)r3 != 0) {
                        r3 = 0x2;
                        break;
                    }
                    tmp = *(u8*)(sp + 0x9);
                    tmp = tmp & 0xa;
                    if ((s32)tmp == 8) {
                        r3 = r30;
                        r4 = (u32)sp + 0x10;
                        r5 = (u32)sp + 0xb;
                        ((void(*)(void))fn_8025F584)();
                        if ((s32)r3 != 0) {
                            r3 = 0x3;
                        } else {
                            r3 = 0x0;
                        }
                        break;
                    }
                    r12 = *(u32*)((u8*)r24 + 0x0);
                    if (r12 != 0) {
                        r3 = r30;
                        r4 = *(u32*)((u8*)r25 + 0x0);
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                    }
                    tmp = *(u32*)(r29 + r28);
                    if ((s32)tmp != 0) {
                        r3 = 0x3e8;
                        break;
                    }
                }
                if ((s32)r3 != 0) {
                    r27 = r3 + 0xf;
                } else if (tmp != 0) {
                    r27 = 0x13;
                } else {
                    r27 = 0x0;
                }
            }
        }
    }
    r3 = r31;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r27;
    return;
}

/* 0x80072548 | size: 0x13C */
void fn_80072548(void) {
    extern void fn_80072684();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;

    r31 = r3;
    ((void(*)(void))fn_800A13F8)();
    r4 = (u32)&lbl_803B6DE0;
    *(u32*)&lbl_8047A600 = r3;
    r3 = (u32)&lbl_803B6DE0;
    OSCreateAlarm();
    OSDisableInterrupts();
    r4 = 0x80000000;
    r5 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r4 = (u32)fn_80072684;
    r5 = r5 + 0x4dd3;
    r6 = (u32)&lbl_803B6DE0;
    tmp = (u32)tmp >> 2;
    r7 = (u32)fn_80072684;
    tmp = (u32)((u64)r5 * (u64)tmp >> 32);
    r5 = (u32)&lbl_803B6DE0;
    r30 = r3;
    r3 = r5;
    r5 = 0x0;
    r6 = (u32)tmp >> 6;
    OSSetAlarm();
    r3 = *(u32*)&lbl_8047A600;
    ((void(*)(void))fn_800A221C)();
    r3 = r30;
    OSRestoreInterrupts();
    r3 = r31;
    ((void(*)(void))fn_800D0F44)();
    /* subis tmp, r3, 0x4 */;
    if (tmp != 0) {
        r30 = 0x1;
    } else {
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r30 = 0x2;
        } else {
            tmp = *(u8*)(sp + 0x8);
            tmp = tmp & 0x00000008;
            if ((s32)tmp == 0) {
                r30 = -0x1;
            } else {
                r3 = r31;
                r4 = (u32)sp + 0xc;
                r5 = (u32)sp + 0x8;
                ((void(*)(void))fn_8025F584)();
                if ((s32)r3 != 0) {
                    r30 = 0x3;
                } else if (tmp != 0) {
                    r30 = 0x4;
                } else {
                    r30 = 0x0;
                }
            }
        }
    }
    if ((s32)r30 != 0) {
        if ((s32)r30 < 3) { r3 = r30; return; }
    }
    r3 = r31 + 0x1;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();

    r3 = r30;
    return;
}

/* 0x80072684 | size: 0x24 */
void fn_80072684(void) {
    extern void fn_800A1F94(u32);
    fn_800A1F94(*(u32*)&lbl_8047A600);
}

/* 0x800726A8 | size: 0x358 */
void fn_800726A8(void) {
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r22 = r3;
    r23 = r4;
    r14 = r22 + 0x1;
    r4 = 0x2;
    r3 = r14;
    ((void(*)(void))fn_8008ABE4)();
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r22 << 3;
    r28 = r3;
    tmp = (u32)&lbl_803B6E18;
    r29 = r22 << 2;
    r26 = tmp + r6;
    r30 = (u32)&lbl_803B6E08;
    r27 = r26 + 0x4;
    r3 = 0x10620000;
    r15 = 0x80000000;
    r31 = r3 + 0x4dd3;
L_80072724:
    OSGetTick();
    tmp = *(u32*)((u8*)r15 + 0xF8);
    r4 = r3 - r28;
    r3 = r4 ^ r25;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r31 * (u64)tmp >> 32);
    r3 = __cntlzw(r3);
    r3 = r4 << r3;
    r24 = (u32)r3 >> 31;
    tmp = (u32)tmp >> 6;
    r17 = tmp * 0x64;
    OSGetTick();
    r16 = r3;
L_80072758:
    OSGetTick();
    tmp = r3 - r16;
    if (tmp > r17) {
        r16 = 0x1;
        goto L_800729BC;
    }
    r3 = r22;
    ((void(*)(void))fn_800D0F44)();
    /* subis tmp, r3, 0x4 */;
    if (tmp != 0) {
        r12 = *(u32*)((u8*)r26 + 0x0);
        if (r12 != 0) {
            r3 = r22;
            r4 = *(u32*)((u8*)r27 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r30 + r29);
        if ((s32)tmp == 0) goto L_80072758;
        r16 = 0x3e8;
        goto L_800729BC;
    }
    r3 = r22;
    ((void(*)(void))fn_80073C38)();
    if ((s32)r3 != 0) {
        r16 = r3;
        goto L_800729BC;
    }
    tmp = 0x55;
    r3 = r22;
    *(u32*)(sp + 0x10) = tmp;
    r4 = (u32)sp + 0x10;
    r5 = (u32)sp + 0xa;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r16 = 0xb;
        goto L_800728C8;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r17 = tmp * 0x64;
    OSGetTick();
    r16 = r3;
L_8007281C:
    OSGetTick();
    tmp = r3 - r16;
    if (tmp > r17) {
        r3 = 0x1;
        goto L_800728B4;
    }
    r3 = r22;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F3F4)();
    if ((s32)r3 != 0) {
        r3 = 0x2;
        goto L_800728B4;
    }
    tmp = *(u8*)(sp + 0x8);
    tmp = tmp & 0xa;
    if ((s32)tmp != 8) {
        r12 = *(u32*)((u8*)r26 + 0x0);
        if (r12 != 0) {
            r3 = r22;
            r4 = *(u32*)((u8*)r27 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r30 + r29);
        if ((s32)tmp == 0) goto L_8007281C;
        r3 = 0x3e8;
        goto L_800728B4;
    }
    r3 = r22;
    r4 = (u32)sp + 0x14;
    r5 = (u32)sp + 0xa;
    ((void(*)(void))fn_8025F584)();
    if ((s32)r3 != 0) {
        r3 = 0x3;
        goto L_800728B4;
    }
    r3 = 0x0;
L_800728B4:
    if ((s32)r3 != 0) {
        r16 = r3 + 0xb;
        goto L_800728C8;
    }
    r16 = 0x0;
L_800728C8:
    if ((s32)r16 != 0) {
        goto L_800729BC;
    }
    tmp = (u32)tmp >> 24;
    if (tmp != 0x55) {
        r16 = 0xf;
        goto L_800729BC;
    }
    r16 = r23;
    r18 = 0x0;
    r3 = 0x10620000;
    r20 = 0x80000000;
    r19 = r3 + 0x4dd3;
    do {
        tmp = *(u32*)((u8*)r16 + 0x0);
        r3 = r22;
        r4 = (u32)sp + 0xc;
        r5 = (u32)sp + 0x9;
        *(u32*)(sp + 0xC) = tmp;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) break;
        tmp = *(u32*)((u8*)r20 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r19 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r17 = tmp * 0x64;
        OSGetTick();
        r21 = r3;
    L_8007293C:
        OSGetTick();
        tmp = r3 - r21;
        if (tmp <= r17) {
            r3 = r22;
            r4 = (u32)sp + 0x9;
            ((void(*)(void))fn_8025F3F4)();
        }
        if ((s32)r3 != 0) break;
        tmp = *(u8*)(sp + 0x9);
        tmp = tmp & 0x00000002;
        if ((s32)tmp != 0) {
            r12 = *(u32*)((u8*)r26 + 0x0);
            if (r12 != 0) {
                r3 = r22;
                r4 = *(u32*)((u8*)r27 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r30 + r29);
            if ((s32)tmp == 0) goto L_8007293C;
            break;
        }
        tmp = *(u32*)(r30 + r29);
        if ((s32)tmp != 0) break;
        r16 = r16 + 0x4;
        r18 = r18 + 0x4;
    } while ((s32)r18 < 0x78);

    r16 = 0x0;
L_800729BC:
    if ((s32)r16 == 1) {
        if ((s32)r24 == 0) goto L_80072724;
    }
    r3 = r14;
    if ((s32)r16 != 0) {
        r4 = 0x1;
    } else {

        r4 = 0x3;
    }
    ((void(*)(void))fn_8008ABE4)();
    r3 = r16;
    return;
}
