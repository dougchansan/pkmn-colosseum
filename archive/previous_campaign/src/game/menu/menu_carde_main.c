/**
 * @file menu_carde_main.c
 * @brief Card-E main handlers (0x8007FD64-0x80082650)
 *
 * Address range: 0x8007FD64 - 0x80082650
 * Total functions: 5
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_80083AF4();
extern void fn_80083BF8();
extern void fn_800CA620();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void fn_800F9D24();
extern void fn_800F9E70();
extern void fn_801046C8();
extern void __assert();
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478948;
extern u8 lbl_8047C140;
extern u8 lbl_8047C178;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EE890[];
extern u8 jumptable_802EE924[];
extern u8 jumptable_802EE9B8[];
extern u8 jumptable_802EEA4C[];
extern u8 lbl_80268B88[];
extern u8 lbl_80268DC0[];
extern u8 lbl_80269B68[];
extern u8 lbl_8026C7F8[];

/* ===== Forward declarations ===== */
s32 menuCardE_CompareEntryPtrs(u32 lhsPtr, u32 rhsPtr);
void fn_8007FDBC(void);
void fn_80080310(void);
void fn_80080ED8(void);
void fn_8008102C(void);

/* ===== Function implementations ===== */

/*
 * 0x8007FD64 | size: 0x58
 * menuCardE_CompareEntryPtrs: qsort-style comparator for MenuCardEEntry*
 * elements.
 * Primary key is signed entry+0x1C descending; secondary key is entry+0x1A
 * ascending. Passed to fn_800CA620 with element size 4.
 */
s32 menuCardE_CompareEntryPtrs(u32 r3, u32 r4) {
    u32 r0;
    u32 r5;

    r5 = *(u32*)((u8*)r3 + 0x0);
    r4 = *(u32*)((u8*)r4 + 0x0);
    r3 = *(u8*)((u8*)r5 + 0x1C);
    r0 = *(u8*)((u8*)r4 + 0x1C);
    r3 = (s8)r3;
    r0 = (s8)r0;
    if ((s32)r3 < (s32)r0) {
        return 0x1;
    }
    if ((s32)r3 > (s32)r0) {
        return -0x1;
    }
    r3 = *(u8*)((u8*)r5 + 0x1A);
    r0 = *(u8*)((u8*)r4 + 0x1A);
    if (r3 < r0) {
        return -0x1;
    }
    r0 = r0 - r3;
    r3 = (u32)r0 >> 31;
    return r3;
}

/*
 * 0x8007FDBC | size: 0x554
 * Proposed role: menuCardE_Matrix_CreateContext. Allocates and initializes
 * the Card-E matrix context, including
 * context+0xAC entryCount and context+0xB0 sorted entry pointer array.
 */
void fn_8007FDBC(void) {
    extern s32 menuCardE_CompareEntryPtrs(u32, u32);
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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
    f32 f0 = 0.0f;
    f32 f2 = 0.0f;
    f32 f4 = 0.0f;
    f32 f6 = 0.0f;
    f32 f8 = 0.0f;

    r29 = r3;
    r20 = r4;
    r4 = (u32)&lbl_80268B88;
    r3 = 0x500;
    r31 = (u32)&lbl_80268B88;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r28 = r3;
    tmp = r28 & 0xFFFF;
    if (tmp == 0) {
        r3 = r31 + 0x1f0;
        r4 = 0x1a2;
        r5 = (u32)&lbl_8047C140;
        ((void(*)(void))__assert)();
    }
    r3 = r28;
    ((void(*)(void))fn_800E27B0)();
    r4 = 0x0;
    r28 = r3;
    r5 = 0x4e8;
    memset((void*)r3, (int)r4, (u32)r5);
    r30 = r28;
    if (r20 != 0) {
        r3 = r28;
        r4 = r20;
        r5 = 0x50;
        ((void(*)(void))fn_800F9D24)();
        tmp = 0x0;
        *(u16*)((u8*)r28 + 0x9E) = tmp;
    } else {

        tmp = 0x0;
        *(u16*)((u8*)r28 + 0x0) = tmp;
    }
    r3 = *(u32*)((u8*)r30 + 0xB0);
    if (r3 != 0) {
        ((void(*)(void))fn_800E202C)();
        r26 = r3;
        tmp = r26 & 0xFFFF;
        if (tmp == 0) {
            r3 = r31 + 0x1f0;
            r4 = 0x1ab;
            r5 = (u32)&lbl_8047C140;
            ((void(*)(void))__assert)();
        }
        r3 = r26;
        ((void(*)(void))fn_800E24B0)();
        r3 = r26;
        ((void(*)(void))fn_800E209C)();
        tmp = 0x0;
        *(u32*)((u8*)r30 + 0xB0) = tmp;
    }
    r3 = 0x0;
    ((void(*)(void))fn_80083BF8)();
    r22 = r3;
    *(u32*)((u8*)r30 + 0xAC) = r3;
    if ((s32)r22 != 0) {
        r26 = r22 << 2;
        r4 = 0x20;
        tmp = r26 + 0x1f;
        /* clrrwi r3, tmp, 5 */;
        ((void(*)(void))fn_800E2C04)();
        r27 = r3;
        tmp = r27 & 0xFFFF;
        if (tmp == 0) {
            r3 = r31 + 0x1f0;
            r4 = 0x1a2;
            r5 = (u32)&lbl_8047C140;
            ((void(*)(void))__assert)();
        }
        r3 = r27;
        ((void(*)(void))fn_800E27B0)();
        r28 = r3;
        r5 = r26;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
        r27 = 0x0;
        *(u32*)((u8*)r30 + 0xB0) = r28;
        r26 = r27;
        while ((s32)r27 < (s32)r22) {

            r4 = r27;
            r3 = 0x0;
            ((void(*)(void))fn_80083AF4)();
            r4 = *(u32*)((u8*)r30 + 0xB0);
            r27 = r27 + 0x1;
            *(u32*)(r4 + r26) = r3;
            r26 = r26 + 0x4;

        }
        r4 = (u32)menuCardE_CompareEntryPtrs;
        r3 = *(u32*)((u8*)r30 + 0xB0);
        r6 = (u32)menuCardE_CompareEntryPtrs;
        r5 = 0x4;
        r4 = r22;
        ((void(*)(void))fn_800CA620)();
    }
    tmp = *(u32*)((u8*)r30 + 0xAC);
    if ((s32)tmp != 0) {
        tmp = 0x0;
    } else {

        tmp = -0x1;
    }
    *(u32*)((u8*)r30 + 0xA4) = tmp;
    r3 = r29;
    r4 = 0x79b;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x118) = r3;
    r3 = r29;
    r4 = 0x79c;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x11C) = r3;
    r3 = r29;
    r4 = 0x79d;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x120) = r3;
    r3 = r29;
    r4 = 0x780;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x124) = r3;
    r3 = r29;
    r4 = 0x781;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x128) = r3;
    r3 = r29;
    r4 = 0x782;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x12C) = r3;
    r3 = r29;
    r4 = 0x1193;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x130) = r3;
    r3 = r29;
    r4 = 0x1195;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x134) = r3;
    r3 = r29;
    r4 = 0x1194;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x138) = r3;
    r3 = r29;
    r4 = 0x796;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x13C) = r3;
    r3 = r29;
    r4 = 0x793;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x140) = r3;
    r3 = r29;
    r4 = 0x797;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x144) = r3;
    r3 = r29;
    r4 = 0x1196;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x148) = r3;
    r3 = r29;
    r4 = 0x792;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x14C) = r3;
    r3 = r29;
    r4 = 0x1126;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x150) = r3;
    r3 = r29;
    r4 = 0x795;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x154) = r3;
    r3 = r29;
    r4 = 0x791;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x158) = r3;
    r3 = r29;
    r4 = 0x1125;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x15C) = r3;
    r3 = r29;
    r4 = 0x799;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x160) = r3;
    r3 = r29;
    r4 = 0x79a;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x164) = r3;
    r3 = r29;
    r4 = 0x825;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x168) = r3;
    r3 = r29;
    r4 = 0x826;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x16C) = r3;
    r24 = r30;
    r25 = r31 + 0x0;
    r23 = r31 + 0x90;
    r22 = r31 + 0x120;
    r21 = 0x0;
    do {
        r26 = r25;
        r27 = r24;
        r28 = r23;
        r31 = r22;
        r20 = 0x0;
        do {
            r4 = *(u16*)((u8*)r26 + 0x0);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            *(u32*)((u8*)r27 + 0x170) = r3;
            r3 = r29;
            r4 = *(u16*)((u8*)r28 + 0x0);
            ((void(*)(void))fn_801046C8)();
            *(u32*)((u8*)r27 + 0x3B0) = r3;
            r3 = r29;
            r4 = *(u16*)((u8*)r31 + 0x0);
            ((void(*)(void))fn_801046C8)();
            *(u32*)((u8*)r27 + 0x290) = r3;
            r26 = r26 + 0x48;
            r27 = r27 + 0x90;
            r28 = r28 + 0x48;
            r31 = r31 + 0x48;
            r20 = r20 + 0x1;
        } while ((s32)r20 < 2);
        r25 = r25 + 0x2;
        r24 = r24 + 0x4;
        r23 = r23 + 0x2;
        r22 = r22 + 0x2;
        r21 = r21 + 0x1;
    } while ((s32)r21 < 0x24);
    r3 = r29;
    r4 = 0x119a;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4D0) = r3;
    r3 = r29;
    r4 = 0x11c2;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4D4) = r3;
    r3 = r29;
    r4 = 0x790;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4D8) = r3;
    r3 = r29;
    r4 = 0x798;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4DC) = r3;
    r3 = r29;
    r4 = 0x78f;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4E0) = r3;
    r3 = r29;
    r4 = 0x794;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r30 + 0x4E4) = r3;
    r3 = r30;
    r4 = *(u32*)((u8*)r30 + 0x200);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xCE) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xD0) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xD4) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xD2) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x440);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xD6) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xD8) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xDC) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xDA) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x320);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xDE) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xE0) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xE4) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xE2) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x118);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xE6) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xE8) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xEC) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xEA) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x11C);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xEE) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xF0) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xF4) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xF2) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x120);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xF6) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0xF8) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0xFC) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0xFA) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x15C);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0xFE) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0x100) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0x104) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0x102) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x154);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0x106) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0x108) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0x10C) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0x10A) = tmp;
    r4 = *(u32*)((u8*)r30 + 0x14C);
    tmp = *(s16*)((u8*)r4 + 0x50);
    *(u16*)((u8*)r30 + 0x10E) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x52);
    *(u16*)((u8*)r30 + 0x110) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x54);
    *(u16*)((u8*)r30 + 0x114) = tmp;
    tmp = *(s16*)((u8*)r4 + 0x56);
    *(u16*)((u8*)r30 + 0x112) = tmp;
    return;
}

/* 0x80080310 | size: 0xBC8 */
void fn_80080310(void) {
    extern void fn_8008102C();
    u8 sp[0xA40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r23 = r3;
    r22 = r4;
    r25 = r5;
    r4 = (u32)&lbl_80268DC0;
    r31 = 0x1;
    r30 = (u32)&lbl_80268DC0;
    r4 = 0x0;
    r5 = 0xb20;
    memset((void*)r3, (int)r4, (u32)r5);
    r5 = r30 + 0x0;
    r26 = 0x0;
    r24 = *(u32*)((u8*)r5 + 0x4);
    if ((s32)r24 < 0x10) {
        r22 = *(u32*)((u8*)r5 + 0x8);
        while ((s32)r26 < (s32)r22) {

            r5 = 0x0;
            r3 = r7 + r24;
            tmp = r3 - r7;
            r4 = (u32)&lbl_80478948;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r7 < (s32)r3) {
                do {
                    tmp = (s32)r7 >> 3;
                    r3 = r7 & 0x7;
                    r5 = r5 & 0xFFFF;
                    r3 = *(u8*)(r4 + r3);
                    r5 = r5 << 1;
                    tmp = *(u8*)(r6 + tmp);
                    tmp = r3 & tmp;
                    if ((s32)tmp != 0) {
                        tmp = 0x1;
                    } else {

                        tmp = 0x0;
                    }
                    tmp = r5 | tmp;
                    r7 = r7 + 0x1;
                    r5 = tmp & 0xFFFF;
                } while (--ctr != 0);
            }
            r8 = r26;
            r3 = (u32)sp + 0x8;
            r4 = r30 + 0x0;
            tmp = tmp + r24;
            r6 = r5 & 0xFFFF;
            *(u32*)(sp + 0x14) = tmp;
            r5 = -0x1;
            r7 = 0x0;
            fn_8008102C();
            r26 = r26 + 0x1;

        }
    } else {

        r4 = (u32)sp + 0x818;
        r3 = 0x0;
        while ((s32)r24 > 0x10) {

            r9 = r3;
            r8 = 0x0;
            r6 = r3 + 0x10;
            r7 = (u32)&lbl_80478948;
            tmp = r6 - r3;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r3 < (s32)r6) {
                do {
                    tmp = (s32)r9 >> 3;
                    r6 = r9 & 0x7;
                    r8 = r8 & 0xFFFF;
                    r6 = *(u8*)(r7 + r6);
                    r8 = r8 << 1;
                    tmp = *(u8*)(r22 + tmp);
                    tmp = r6 & tmp;
                    if ((s32)tmp != 0) {
                        tmp = 0x1;
                    } else {

                        tmp = 0x0;
                    }
                    tmp = r8 | tmp;
                    r9 = r9 + 0x1;
                    r8 = tmp & 0xFFFF;
                } while (--ctr != 0);
            }
            *(u16*)((u8*)r4 + 0x0) = r8;
            r4 = r4 + 0x2;
            r3 = r3 + 0x10;

        }
        if ((s32)r24 != 0) {
            r6 = r3 + r24;
            r8 = 0x0;
            tmp = r6 - r3;
            r7 = (u32)&lbl_80478948;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r3 < (s32)r6) {
                do {
                    tmp = (s32)r3 >> 3;
                    r6 = r3 & 0x7;
                    r8 = r8 & 0xFFFF;
                    r6 = *(u8*)(r7 + r6);
                    r8 = r8 << 1;
                    tmp = *(u8*)(r22 + tmp);
                    tmp = r6 & tmp;
                    if ((s32)tmp != 0) {
                        tmp = 0x1;
                    } else {

                        tmp = 0x0;
                    }
                    tmp = r8 | tmp;
                    r3 = r3 + 0x1;
                    r8 = tmp & 0xFFFF;
                } while (--ctr != 0);
            }
            *(u16*)((u8*)r4 + 0x0) = r8;
            r4 = r4 + 0x2;
        }
        r7 = 0x0;
        tmp = *(u32*)((u8*)r5 + 0x4);
        r3 = (u32)sp + 0x8;
        *(u16*)((u8*)r4 + 0x0) = r7;
        r4 = r30 + 0x0;
        tmp = r6 + tmp;
        r7 = (u32)sp + 0x818;
        *(u32*)(sp + 0x14) = tmp;
        r5 = -0x1;
        r6 = 0x0;
        r8 = -0x1;
        fn_8008102C();
    }
    r27 = 0x0;
    tmp = *(u32*)((u8*)r23 + 0x0);
    if ((s32)tmp != 1) {
        if ((s32)tmp >= 1 || (s32)tmp < 0) {
            r31 = 0x0;
        } else {
        r28 = r30 + 0x0;
        r22 = 0x0;
        do {
            r9 = *(u32*)((u8*)r28 + 0x4);
            r27 = 0x1;
            if ((s32)r9 < 0x10) {
                r24 = r28 + 0x4;
                r25 = r28 + 0x8;
                r26 = 0x0;
                while (1) {
                    tmp = *(u32*)((u8*)r25 + 0x0);
                    if ((s32)r26 >= (s32)tmp) break;
                    r6 = 0x0;
                    tmp = *(u32*)((u8*)r24 + 0x0);
                    r4 = r8 + tmp;
                    r3 = r4 - r8;
                    r5 = (u32)&lbl_80478948;
                    ctr_fn = (void(*)(void))r3;
                    if ((s32)r8 < (s32)r4) {
                        do {
                            r3 = (s32)r8 >> 3;
                            r4 = r8 & 0x7;
                            r6 = r6 & 0xFFFF;
                            r4 = *(u8*)(r5 + r4);
                            r6 = r6 << 1;
                            r3 = *(u8*)(r7 + r3);
                            r3 = r4 & r3;
                            if ((s32)r3 != 0) {
                                r3 = 0x1;
                            } else {

                                r3 = 0x0;
                            }
                            r3 = r6 | r3;
                            r8 = r8 + 0x1;
                            r6 = r3 & 0xFFFF;
                        } while (--ctr != 0);
                    }
                    r4 = r28;
                    r8 = r26;
                    r3 = (u32)sp + 0x8;
                    tmp = r5 + tmp;
                    r6 = r6 & 0xFFFF;
                    *(u32*)(sp + 0x14) = tmp;
                    r5 = -0x1;
                    r7 = 0x0;
                    fn_8008102C();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r27 = 0x0;
                    }
                    r26 = r26 + 0x1;

                }

            } else {
                r5 = (u32)sp + 0x618;
                while ((s32)r9 > 0x10) {

                    r10 = r4;
                    r8 = 0x0;
                    r6 = r4 + 0x10;
                    r7 = (u32)&lbl_80478948;
                    tmp = r6 - r4;
                    ctr_fn = (void(*)(void))tmp;
                    if ((s32)r4 < (s32)r6) {
                        do {
                            tmp = (s32)r10 >> 3;
                            r6 = r10 & 0x7;
                            r8 = r8 & 0xFFFF;
                            r6 = *(u8*)(r7 + r6);
                            r8 = r8 << 1;
                            tmp = *(u8*)(r3 + tmp);
                            tmp = r6 & tmp;
                            if ((s32)tmp != 0) {
                                tmp = 0x1;
                            } else {

                                tmp = 0x0;
                            }
                            tmp = r8 | tmp;
                            r10 = r10 + 0x1;
                            r8 = tmp & 0xFFFF;
                        } while (--ctr != 0);
                    }
                    *(u16*)((u8*)r5 + 0x0) = r8;
                    r5 = r5 + 0x2;
                    r4 = r4 + 0x10;

                }
                if ((s32)r9 != 0) {
                    r6 = r4 + r9;
                    r8 = 0x0;
                    tmp = r6 - r4;
                    r7 = (u32)&lbl_80478948;
                    ctr_fn = (void(*)(void))tmp;
                    if ((s32)r4 < (s32)r6) {
                        do {
                            tmp = (s32)r4 >> 3;
                            r6 = r4 & 0x7;
                            r8 = r8 & 0xFFFF;
                            r6 = *(u8*)(r7 + r6);
                            r8 = r8 << 1;
                            tmp = *(u8*)(r3 + tmp);
                            tmp = r6 & tmp;
                            if ((s32)tmp != 0) {
                                tmp = 0x1;
                            } else {

                                tmp = 0x0;
                            }
                            tmp = r8 | tmp;
                            r4 = r4 + 0x1;
                            r8 = tmp & 0xFFFF;
                        } while (--ctr != 0);
                    }
                    *(u16*)((u8*)r5 + 0x0) = r8;
                    r5 = r5 + 0x2;
                }
                r3 = 0x0;
                tmp = *(u32*)((u8*)r28 + 0x4);
                r4 = r28;
                *(u16*)((u8*)r5 + 0x0) = r3;
                r3 = (u32)sp + 0x8;
                tmp = r6 + tmp;
                r7 = (u32)sp + 0x618;
                *(u32*)(sp + 0x14) = tmp;
                r5 = -0x1;
                r6 = 0x0;
                r8 = -0x1;
                fn_8008102C();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    r27 = 0x0;
                }
            }
            tmp = r27 & 0xFF;
            if (tmp == 0) {
                r31 = 0x0;
            }
            r28 = r28 + 0xc;
            r22 = r22 + 0x1;
        } while (r22 < 0x28);
        r26 = 0x0;
        do {
            r28 = r26;
            r29 = r30 + 0x1e0;
            r27 = 0x0;
            do {
                r9 = *(u32*)((u8*)r29 + 0x4);
                r24 = 0x1;
                if ((s32)r9 < 0x10) {
                    r23 = r29 + 0x4;
                    r22 = r29 + 0x8;
                    r25 = 0x0;
                    while (1) {
                        tmp = *(u32*)((u8*)r22 + 0x0);
                        if ((s32)r25 >= (s32)tmp) break;
                        r9 = 0x0;
                        tmp = *(u32*)((u8*)r23 + 0x0);
                        r6 = r4 + tmp;
                        r5 = r6 - r4;
                        r7 = (u32)&lbl_80478948;
                        ctr_fn = (void(*)(void))r5;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                r5 = (s32)r4 >> 3;
                                r6 = r4 & 0x7;
                                r8 = r9 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                r5 = *(u8*)(r3 + r5);
                                r5 = r6 & r5;
                                if ((s32)r5 != 0) {
                                    r5 = 0x1;
                                } else {

                                    r5 = 0x0;
                                }
                                r5 = r8 | r5;
                                r4 = r4 + 0x1;
                                r9 = r5 & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        r4 = r29;
                        r5 = r28;
                        r8 = r25;
                        tmp = r3 + tmp;
                        r3 = (u32)sp + 0x8;
                        *(u32*)(sp + 0x14) = tmp;
                        r6 = r9 & 0xFFFF;
                        r7 = 0x0;
                        fn_8008102C();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            r24 = 0x0;
                        }
                        r25 = r25 + 0x1;

                    }

                } else {
                    r5 = (u32)sp + 0x418;
                    while ((s32)r9 > 0x10) {

                        r10 = r4;
                        r8 = 0x0;
                        r6 = r4 + 0x10;
                        r7 = (u32)&lbl_80478948;
                        tmp = r6 - r4;
                        ctr_fn = (void(*)(void))tmp;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                tmp = (s32)r10 >> 3;
                                r6 = r10 & 0x7;
                                r8 = r8 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                tmp = *(u8*)(r3 + tmp);
                                tmp = r6 & tmp;
                                if ((s32)tmp != 0) {
                                    tmp = 0x1;
                                } else {

                                    tmp = 0x0;
                                }
                                tmp = r8 | tmp;
                                r10 = r10 + 0x1;
                                r8 = tmp & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        *(u16*)((u8*)r5 + 0x0) = r8;
                        r5 = r5 + 0x2;
                        r4 = r4 + 0x10;

                    }
                    if ((s32)r9 != 0) {
                        r6 = r4 + r9;
                        r8 = 0x0;
                        tmp = r6 - r4;
                        r7 = (u32)&lbl_80478948;
                        ctr_fn = (void(*)(void))tmp;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                tmp = (s32)r4 >> 3;
                                r6 = r4 & 0x7;
                                r8 = r8 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                tmp = *(u8*)(r3 + tmp);
                                tmp = r6 & tmp;
                                if ((s32)tmp != 0) {
                                    tmp = 0x1;
                                } else {

                                    tmp = 0x0;
                                }
                                tmp = r8 | tmp;
                                r4 = r4 + 0x1;
                                r8 = tmp & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        *(u16*)((u8*)r5 + 0x0) = r8;
                        r5 = r5 + 0x2;
                    }
                    r6 = 0x0;
                    tmp = *(u32*)((u8*)r29 + 0x4);
                    r4 = r29;
                    *(u16*)((u8*)r5 + 0x0) = r6;
                    r5 = r28;
                    tmp = r3 + tmp;
                    r3 = (u32)sp + 0x8;
                    *(u32*)(sp + 0x14) = tmp;
                    r7 = (u32)sp + 0x418;
                    r6 = 0x0;
                    r8 = -0x1;
                    fn_8008102C();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r24 = 0x0;
                    }
                }
                tmp = r24 & 0xFF;
                if (tmp == 0) {
                    r31 = 0x0;
                }
                r29 = r29 + 0xc;
                r27 = r27 + 0x1;
            } while (r27 < 8);
            r26 = r26 + 0x1;
        } while ((s32)r26 < 9);
        r27 = 0x0;
        do {
            r28 = r30 + 0x240;
            r29 = 0x0;
            do {
                r9 = *(u32*)((u8*)r28 + 0x4);
                r25 = 0x1;
                if ((s32)r9 < 0x10) {
                    r23 = r27;
                    r22 = r28 + 0x4;
                    r26 = r28 + 0x8;
                    r24 = 0x0;
                    while (1) {
                        tmp = *(u32*)((u8*)r26 + 0x0);
                        if ((s32)r24 >= (s32)tmp) break;
                        r9 = 0x0;
                        tmp = *(u32*)((u8*)r22 + 0x0);
                        r6 = r4 + tmp;
                        r5 = r6 - r4;
                        r7 = (u32)&lbl_80478948;
                        ctr_fn = (void(*)(void))r5;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                r5 = (s32)r4 >> 3;
                                r6 = r4 & 0x7;
                                r8 = r9 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                r5 = *(u8*)(r3 + r5);
                                r5 = r6 & r5;
                                if ((s32)r5 != 0) {
                                    r5 = 0x1;
                                } else {

                                    r5 = 0x0;
                                }
                                r5 = r8 | r5;
                                r4 = r4 + 0x1;
                                r9 = r5 & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        r4 = r28;
                        r5 = r23;
                        r8 = r24;
                        tmp = r3 + tmp;
                        r3 = (u32)sp + 0x8;
                        *(u32*)(sp + 0x14) = tmp;
                        r6 = r9 & 0xFFFF;
                        r7 = 0x0;
                        fn_8008102C();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            r25 = 0x0;
                        }
                        r24 = r24 + 0x1;

                    }

                } else {
                    r5 = (u32)sp + 0x218;
                    while ((s32)r9 > 0x10) {

                        r10 = r4;
                        r8 = 0x0;
                        r6 = r4 + 0x10;
                        r7 = (u32)&lbl_80478948;
                        tmp = r6 - r4;
                        ctr_fn = (void(*)(void))tmp;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                tmp = (s32)r10 >> 3;
                                r6 = r10 & 0x7;
                                r8 = r8 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                tmp = *(u8*)(r3 + tmp);
                                tmp = r6 & tmp;
                                if ((s32)tmp != 0) {
                                    tmp = 0x1;
                                } else {

                                    tmp = 0x0;
                                }
                                tmp = r8 | tmp;
                                r10 = r10 + 0x1;
                                r8 = tmp & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        *(u16*)((u8*)r5 + 0x0) = r8;
                        r5 = r5 + 0x2;
                        r4 = r4 + 0x10;

                    }
                    if ((s32)r9 != 0) {
                        r6 = r4 + r9;
                        r8 = 0x0;
                        tmp = r6 - r4;
                        r7 = (u32)&lbl_80478948;
                        ctr_fn = (void(*)(void))tmp;
                        if ((s32)r4 < (s32)r6) {
                            do {
                                tmp = (s32)r4 >> 3;
                                r6 = r4 & 0x7;
                                r8 = r8 & 0xFFFF;
                                r6 = *(u8*)(r7 + r6);
                                r8 = r8 << 1;
                                tmp = *(u8*)(r3 + tmp);
                                tmp = r6 & tmp;
                                if ((s32)tmp != 0) {
                                    tmp = 0x1;
                                } else {

                                    tmp = 0x0;
                                }
                                tmp = r8 | tmp;
                                r4 = r4 + 0x1;
                                r8 = tmp & 0xFFFF;
                            } while (--ctr != 0);
                        }
                        *(u16*)((u8*)r5 + 0x0) = r8;
                        r5 = r5 + 0x2;
                    }
                    r6 = 0x0;
                    tmp = *(u32*)((u8*)r28 + 0x4);
                    r4 = r28;
                    *(u16*)((u8*)r5 + 0x0) = r6;
                    r5 = r27;
                    tmp = r3 + tmp;
                    r3 = (u32)sp + 0x8;
                    *(u32*)(sp + 0x14) = tmp;
                    r7 = (u32)sp + 0x218;
                    r6 = 0x0;
                    r8 = -0x1;
                    fn_8008102C();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r25 = 0x0;
                    }
                }
                tmp = r25 & 0xFF;
                if (tmp == 0) {
                    r31 = 0x0;
                }
                r28 = r28 + 0xc;
                r29 = r29 + 0x1;
            } while (r29 < 0x18);
            r27 = r27 + 0x1;
        } while ((s32)r27 < 0x24);
        }
    } else {
    r26 = r30 + 0x360;
    do {
        r9 = *(u32*)((u8*)r26 + 0x4);
        r25 = 0x1;
        if ((s32)r9 < 0x10) {
            r22 = r26 + 0x4;
            r23 = r26 + 0x8;
            r24 = 0x0;
            while (1) {
                tmp = *(u32*)((u8*)r23 + 0x0);
                if ((s32)r24 >= (s32)tmp) break;
                r6 = 0x0;
                tmp = *(u32*)((u8*)r22 + 0x0);
                r4 = r8 + tmp;
                r3 = r4 - r8;
                r5 = (u32)&lbl_80478948;
                ctr_fn = (void(*)(void))r3;
                if ((s32)r8 < (s32)r4) {
                    do {
                        r3 = (s32)r8 >> 3;
                        r4 = r8 & 0x7;
                        r6 = r6 & 0xFFFF;
                        r4 = *(u8*)(r5 + r4);
                        r6 = r6 << 1;
                        r3 = *(u8*)(r7 + r3);
                        r3 = r4 & r3;
                        if ((s32)r3 != 0) {
                            r3 = 0x1;
                        } else {

                            r3 = 0x0;
                        }
                        r3 = r6 | r3;
                        r8 = r8 + 0x1;
                        r6 = r3 & 0xFFFF;
                    } while (--ctr != 0);
                }
                r4 = r26;
                r8 = r24;
                r3 = (u32)sp + 0x8;
                tmp = r5 + tmp;
                r6 = r6 & 0xFFFF;
                *(u32*)(sp + 0x14) = tmp;
                r5 = -0x1;
                r7 = 0x0;
                fn_8008102C();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    r25 = 0x0;
                }
                r24 = r24 + 0x1;

            }

        } else {
            r5 = (u32)sp + 0x18;
            while ((s32)r9 > 0x10) {

                r10 = r4;
                r8 = 0x0;
                r6 = r4 + 0x10;
                r7 = (u32)&lbl_80478948;
                tmp = r6 - r4;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r4 < (s32)r6) {
                    do {
                        tmp = (s32)r10 >> 3;
                        r6 = r10 & 0x7;
                        r8 = r8 & 0xFFFF;
                        r6 = *(u8*)(r7 + r6);
                        r8 = r8 << 1;
                        tmp = *(u8*)(r3 + tmp);
                        tmp = r6 & tmp;
                        if ((s32)tmp != 0) {
                            tmp = 0x1;
                        } else {

                            tmp = 0x0;
                        }
                        tmp = r8 | tmp;
                        r10 = r10 + 0x1;
                        r8 = tmp & 0xFFFF;
                    } while (--ctr != 0);
                }
                *(u16*)((u8*)r5 + 0x0) = r8;
                r5 = r5 + 0x2;
                r4 = r4 + 0x10;

            }
            if ((s32)r9 != 0) {
                r6 = r4 + r9;
                r8 = 0x0;
                tmp = r6 - r4;
                r7 = (u32)&lbl_80478948;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r4 < (s32)r6) {
                    do {
                        tmp = (s32)r4 >> 3;
                        r6 = r4 & 0x7;
                        r8 = r8 & 0xFFFF;
                        r6 = *(u8*)(r7 + r6);
                        r8 = r8 << 1;
                        tmp = *(u8*)(r3 + tmp);
                        tmp = r6 & tmp;
                        if ((s32)tmp != 0) {
                            tmp = 0x1;
                        } else {

                            tmp = 0x0;
                        }
                        tmp = r8 | tmp;
                        r4 = r4 + 0x1;
                        r8 = tmp & 0xFFFF;
                    } while (--ctr != 0);
                }
                *(u16*)((u8*)r5 + 0x0) = r8;
                r5 = r5 + 0x2;
            }
            r3 = 0x0;
            tmp = *(u32*)((u8*)r26 + 0x4);
            r4 = r26;
            *(u16*)((u8*)r5 + 0x0) = r3;
            r3 = (u32)sp + 0x8;
            tmp = r6 + tmp;
            r7 = (u32)sp + 0x18;
            *(u32*)(sp + 0x14) = tmp;
            r5 = -0x1;
            r6 = 0x0;
            r8 = -0x1;
            fn_8008102C();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r25 = 0x0;
            }
        }
        tmp = r25 & 0xFF;
        if (tmp == 0) {
            r31 = 0x0;
        }
        r26 = r26 + 0xc;
        r27 = r27 + 0x1;
    } while (r27 < 3);
    }
    /* L_80080E90: */
    tmp = r31 & 0xFF;
    if (tmp == 0) {
        r3 = 0x0;
    } else {

        r4 = tmp << 3;
        tmp = r4 - r3;
        r3 = r4 | ~r3;
        tmp = (u32)tmp >> 1;
        tmp = r3 - tmp;
        r3 = (u32)tmp >> 31;
    }
    return;
}

/* 0x80080ED8 | size: 0x154 */
void fn_80080ED8(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;

    r5 = 0x0;
    if (r4 == 0) {
        r3 = 0x0;
        return;
    }
    if (r3 == 0) {
    r3 = *(u8*)((u8*)r4 + 0x0);
    while (r3 != 0) {
            if (r3 >= 0x81) {
                tmp = r3 & 0xFF;
                if (tmp > 0x9f) {
                }
                tmp = r3 & 0xFF;
                if (tmp >= 0xe0 && tmp <= 0xfc) {

                    }
                r4 = r4 + 0x2;
                r5 = r5 + 0x2;

                } else {
                r4 = r4 + 0x1;
                r5 = r5 + 0x1;
                }
            r3 = *(u8*)((u8*)r4 + 0x0);
    }
        r3 = r5;
        return;
    }
    r6 = *(u8*)((u8*)r4 + 0x0);
    while (r6 != 0) {
        if (r6 >= 0x81) {
            tmp = r6 & 0xFF;
            if (tmp > 0x9f) {
            }
            tmp = r6 & 0xFF;
            if (tmp >= 0xe0 && tmp <= 0xfc) {

                }
            if (r6 >= 0x81 || r6 > 0x9f) {

                r7 = r6 * 0x170;
                tmp = *(u8*)((u8*)r4 + 0x1);
                r6 = (u32)&lbl_80269B68;
                tmp = tmp << 1;
                r6 = (u32)&lbl_80269B68;
                r6 = r6 + r7;
                r6 = r6 + tmp;
                /* subis r6, r6, 0x1 */;
                tmp = *(u16*)((u8*)r6 + 0x4610);
                r6 = tmp;
            } else {

                r7 = r6 * 0x170;
                tmp = *(u8*)((u8*)r4 + 0x1);
                r6 = (u32)&lbl_8026C7F8;
                tmp = tmp << 1;
                r6 = (u32)&lbl_8026C7F8;
                r6 = r6 + r7;
                r6 = r6 + tmp;
                /* subis r6, r6, 0x1 */;
                tmp = *(u16*)((u8*)r6 + (-17024));
                r6 = tmp;
            }
            tmp = 0x2;

            } else {
            tmp = r6 & 0xFF;
            if ((tmp >= 0xa1) && (tmp <= 0xdf)) {

                r6 = r6 + (0x1 << 16);
                r6 = tmp & 0xFFFF;
            }
            tmp = 0x1;
            }
        *(u16*)((u8*)r3 + 0x0) = r6;
        r3 = r3 + 0x2;
        r4 = r4 + tmp;
        r5 = r5 + tmp;
        r6 = *(u8*)((u8*)r4 + 0x0);
    }
    tmp = 0x0;
    *(u16*)((u8*)r3 + 0x0) = tmp;

    r3 = r5;
    return;
}

/* 0x8008102C | size: 0x1624 */
void fn_8008102C(void) {
    extern u8 jumptable_802EE890[];
    extern u8 jumptable_802EE924[];
    extern u8 jumptable_802EE9B8[];
    extern u8 jumptable_802EEA4C[];
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    f32 f0 = 0.0f;
    f32 f2 = 0.0f;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = *(u32*)((u8*)r4 + 0x0);
    r4 = (u32)&lbl_80268DC0;
    r4 = (u32)&lbl_80268DC0;
    if (tmp <= 0x4a) {
        r9 = (u32)jumptable_802EEA4C;
        tmp = tmp << 2;
        r9 = (u32)jumptable_802EEA4C;
        tmp = *(u32*)(r9 + tmp);
        ctr_fn = (void(*)(void))tmp;
        r4 = *(u32*)((u8*)r3 + 0x0);
        *(u32*)((u8*)r4 + 0x0) = r6;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u32*)((u8*)r3 + 0x0);
        if ((s32)tmp >= 2) { r3 = 0x0; return; }
        if ((s32)tmp >= 0) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x4) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x4);
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        if ((s32)tmp < 0) { r3 = 0x0; return; }
        if ((s32)tmp >= 6) { r3 = 0x0; return; }
        r3 = 0x1;
        return;

        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x5) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x5);
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        if ((s32)tmp < 0) { r3 = 0x0; return; }
        if ((s32)tmp >= 4) { r3 = 0x0; return; }
        r3 = 0x1;
        return;

        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x6) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x6);
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        if ((s32)tmp < 0) { r3 = 0x0; return; }
        if ((s32)tmp >= 0xa) { r3 = 0x0; return; }
        r3 = 0x1;
        return;

        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x7) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x7);
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        if ((s32)tmp < 0) { r3 = 0x1; return; }
        r3 = 0x1;
        return;

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r3 + 0x8) = tmp;
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0xa;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x24) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 < 6) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r3 + 0x25) = tmp;
        r3 = 0x1;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r4 + 0x26) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x26);
        tmp = (s8)tmp;
        if ((s32)tmp < 0) { r3 = 0x0; return; }
        if ((s32)tmp < 5) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x28;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x38;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x48;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)r6;
        *(u8*)((u8*)r3 + 0x58) = tmp;
        if ((s32)r6 < 1) { r3 = 0x0; return; }
        if ((s32)r6 <= 3) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)r6;
        *(u8*)((u8*)r3 + 0x59) = tmp;
        if ((s32)r6 < 1) { r3 = 0x0; return; }
        if ((s32)r6 <= 6) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)r6;
        *(u8*)((u8*)r3 + 0x5A) = tmp;
        if ((s32)r6 < 1) { r3 = 0x0; return; }
        if ((s32)r6 <= 5) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x5B) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x5C) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x5D) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x5E) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x5F) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x60) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x61) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x62) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = (s8)tmp;
        *(u8*)((u8*)r3 + 0x63) = tmp;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 9) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r5 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFFFF;
        r6 = r4 + 0x384;
        r4 = 0x0;
        *(u16*)((u8*)r5 + 0x64) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = *(u16*)((u8*)r3 + 0x64);
        tmp = 0x2f;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x6;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r5 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFFFF;
        r6 = r4 + 0x384;
        r4 = 0x0;
        *(u16*)((u8*)r5 + 0x66) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = *(u16*)((u8*)r3 + 0x66);
        tmp = 0x2f;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x6;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r5 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFFFF;
        r6 = r4 + 0x384;
        r4 = 0x0;
        *(u16*)((u8*)r5 + 0x68) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = *(u16*)((u8*)r3 + 0x68);
        tmp = 0x2f;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x6;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x6A) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x6A);
        if (tmp <= 0x24) {
            r3 = (u32)jumptable_802EE9B8;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EE9B8;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = 0x1;
        } else {

            tmp = 0x0;
        }
        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x6B) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x6B);
        if (tmp <= 0x24) {
            r3 = (u32)jumptable_802EE924;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EE924;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = 0x1;
        } else {

            tmp = 0x0;
        }
        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r6 & 0xFF;
        *(u8*)((u8*)r4 + 0x6C) = tmp;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x6C);
        if (tmp <= 0x24) {
            r3 = (u32)jumptable_802EE890;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EE890;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = 0x1;
        } else {

            tmp = 0x0;
        }
        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x6e;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x182;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x296;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0xca;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x1de;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x2f2;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x126;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x23a;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0x34e;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        r5 = r5 * 0x28;
        tmp = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r5 + 0x3ac;
        r3 = tmp + r3;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
        if ((s32)r6 != 1) {
            if ((s32)r6 >= 1 || (s32)r6 < 0) {
                tmp = r5 * 0x28;
                r4 = *(u32*)((u8*)r3 + 0x0);
                r5 = 0x0;
                r3 = 0x0;
                r4 = r4 + tmp;
                *(u8*)((u8*)r4 + 0x3B8) = r5;
                return;
            }
            tmp = r5 * 0x28;
            r3 = *(u32*)((u8*)r3 + 0x0);
            r4 = 0x1;
            r3 = r3 + tmp;
            *(u8*)((u8*)r3 + 0x3B8) = r4;
            r3 = 0x1;
            return;
        }
        tmp = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = 0x0;
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x3B8) = r4;
        r3 = 0x1;
        return;
        tmp = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = r3 + tmp;
        r4 = (s8)r4;
        r3 = tmp + r8;
        *(u8*)((u8*)r3 + 0x3B9) = r4;
        if ((s32)r6 < 0) { r3 = 0x0; return; }
        if ((s32)r6 <= 0x24) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r5 = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = r8 << 1;
        r6 = r6 & 0xFFFF;
        r3 = r3 + r5;
        r5 = r4 + 0x384;
        r3 = r3 + tmp;
        r4 = 0x0;
        *(u16*)((u8*)r3 + 0x3BE) = r6;
        tmp = 0x2f;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x6;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u32*)((u8*)r3 + 0x3C8) = r6;
        r3 = 0x1;
        return;
        tmp = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFFFF;
        r6 = r4 + 0x618;
        r3 = r3 + tmp;
        r4 = 0x0;
        *(u16*)((u8*)r3 + 0x3CC) = r5;
        tmp = 0x13;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x3;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x28;
        r4 = *(u32*)((u8*)r3 + 0x0);
        r7 = r6 & 0xFFFF;
        r4 = r4 + tmp;
        *(u16*)((u8*)r4 + 0x3CE) = r7;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r3 + 0x5B);
        tmp = (s8)tmp;
        do {
            if ((s32)tmp == (s32)r5) {
                tmp = 0x1;
                break;
            }
            tmp = *(u8*)((u8*)r3 + 0x5C);
            tmp = (s8)tmp;
            if ((s32)tmp == (s32)r5) {
                tmp = 0x1;
                break;
            }
            tmp = *(u8*)((u8*)r3 + 0x5D);
            tmp = (s8)tmp;
            if ((s32)tmp == (s32)r5) {
                tmp = 0x1;
                break;
            }
            tmp = 0x0;
        } while (0);

        tmp = tmp & 0xFF;
        if (tmp == 0) { r3 = 0x1; return; }
        tmp = r6 & 0xFFFF;
        if (tmp <= 0x3e7) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x28;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFF;
        r6 = r4 + 0x6b0;
        r3 = r3 + tmp;
        r4 = 0x0;
        *(u8*)((u8*)r3 + 0x3D0) = r5;
        tmp = 0x4;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x7;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFFFF;
        r7 = r4 + 0x6f0;
        r4 = r3 + tmp;
        r3 = r6 & 0xFF;
        *(u16*)((u8*)r4 + 0x514) = r5;
        r4 = 0x0;
        tmp = 0x2b;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            tmp = *(u16*)((u8*)r7 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r7 = r7 + 0x2;
            r4 = r4 + 0x8;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFF;
        r6 = r4 + 0x9f8;
        r3 = r3 + tmp;
        r4 = 0x0;
        *(u8*)((u8*)r3 + 0x516) = r5;
        tmp = 0x3;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            tmp = *(u8*)((u8*)r6 + 0x0);
            if (tmp == r5) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x1;
            r4 = r4 + 0x9;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r6 & 0xFF;
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x517) = r4;
        r3 = 0x1;
        return;
        r5 = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        tmp = r8 << 1;
        r7 = r6 & 0xFFFF;
        r3 = r3 + r5;
        r5 = r4 + 0xa18;
        r4 = r3 + tmp;
        r3 = r6 & 0xFF;
        *(u16*)((u8*)r4 + 0x518) = r7;
        r4 = 0x0;
        tmp = 0x47;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            tmp = *(u16*)((u8*)r5 + 0x0);
            if (r3 == tmp) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFFFF;
        r6 = r4 + 0x384;
        r3 = r3 + tmp;
        r4 = 0x0;
        *(u16*)((u8*)r3 + 0x520) = r5;
        tmp = 0x2f;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            tmp = *(u16*)((u8*)r6 + 0x0);
            if (r5 == tmp) {
                tmp = 0x1;
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x6;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        if ((s32)r6 < 2) {
            if ((s32)r6 >= 0) {
                tmp = r5 * 0x2a;
                r3 = *(u32*)((u8*)r3 + 0x0);
                r4 = (s8)r6;
                r3 = r3 + tmp;
                *(u8*)((u8*)r3 + 0x522) = r4;
                r3 = 0x1;
                return;
            }
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = -0x1;
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x522) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x523) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x524) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x525) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x526) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x527) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0x1f)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s8)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x528) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x52A) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x52C) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x52E) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x530) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x532) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x534) = r4;
        r3 = 0x1;
        return;
        tmp = 0x0;
        if (((s32)r6 >= 0) && ((s32)r6 <= 0xff)) {

            tmp = 0x1;
        }
        if ((s32)tmp != 0) {
            r4 = (s16)r6;
        } else {

            r4 = -0x1;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r3 = r3 + tmp;
        *(u16*)((u8*)r3 + 0x536) = r4;
        r3 = 0x1;
        return;
        if ((s32)r6 != 2) {
            if ((s32)r6 < 2) {
                if ((s32)r6 != 0) {
                    if ((s32)r6 < 0) {
                    }

                } else {
                    if ((s32)r6 < 4) {
                        tmp = r5 * 0x2a;
                        r3 = *(u32*)((u8*)r3 + 0x0);
                        r4 = 0x2;
                        r3 = r3 + tmp;
                        *(u8*)((u8*)r3 + 0x538) = r4;
                        r3 = 0x1;
                        return;
                    }
                }
                tmp = r5 * 0x2a;
                r3 = *(u32*)((u8*)r3 + 0x0);
                r4 = 0x0;
                r3 = r3 + tmp;
                *(u8*)((u8*)r3 + 0x538) = r4;
                r3 = 0x1;
                return;
                    }
            tmp = r5 * 0x2a;
            r3 = *(u32*)((u8*)r3 + 0x0);
            r4 = 0x0;
            r3 = r3 + tmp;
            *(u8*)((u8*)r3 + 0x538) = r4;
            r3 = 0x1;
            return;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = 0x1;
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x538) = r4;
        r3 = 0x1;
        return;
        tmp = r6 & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = r5 * 0x2a;
            r6 = *(u32*)((u8*)r3 + 0x0);
            r7 = -0x1;
            r6 = r6 + tmp;
            *(u8*)((u8*)r6 + 0x539) = r7;
        } else {

            tmp = r5 * 0x2a;
            r7 = *(u32*)((u8*)r3 + 0x0);
            r8 = (s8)r6;
            r6 = r7 + tmp;
            *(u8*)((u8*)r6 + 0x539) = r8;
        }
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r5 = r4 + 0xce0;
        r4 = 0x0;
        r3 = r3 + tmp;
        r3 = *(u8*)((u8*)r3 + 0x539);
        tmp = 0xd;
        r3 = (s8)r3;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)tmp == (s32)r3) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x1;
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)tmp == (s32)r3) {
                tmp = 0x1;
                break;
            }
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r7 = r5 * 0x2a;
        tmp = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFF;
        r4 = tmp + r7;
        *(u8*)((u8*)r4 + 0x53A) = r5;
        tmp = *(u32*)((u8*)r3 + 0x0);
        r3 = tmp + r7;
        tmp = *(u8*)((u8*)r3 + 0x53A);
        if ((s32)tmp >= 4) { r3 = 0x0; return; }
        if ((s32)tmp >= 0) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
        r7 = r5 * 0x2a;
        tmp = *(u32*)((u8*)r3 + 0x0);
        r5 = r6 & 0xFF;
        r4 = tmp + r7;
        *(u8*)((u8*)r4 + 0x53B) = r5;
        tmp = *(u32*)((u8*)r3 + 0x0);
        r4 = tmp + r7;
        tmp = *(u8*)((u8*)r4 + 0x53B);
        if ((s32)tmp < 5) {
            if ((s32)tmp >= 0) { r3 = 0x1; return; }
        }
        tmp = 0x0;
        r3 = 0x0;
        *(u8*)((u8*)r4 + 0x53B) = tmp;
        return;
        tmp = r5 * 0x2a;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r6 & 0xFF;
        r3 = r3 + tmp;
        *(u8*)((u8*)r3 + 0x53C) = r4;
        r3 = 0x1;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r4 + 0xcfc;
        *(u32*)((u8*)r3 + 0xAFC) = r6;
        tmp = 0x2b;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u32*)((u8*)r4 + 0x0);
            if (r6 == tmp) {
                tmp = 0x1;
                break;
            }
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
        r3 = *(u32*)((u8*)r3 + 0x0);
        r4 = r7;
        r3 = r3 + 0xb00;
        ((void(*)(void))fn_800F9E70)();
        r3 = 0x1;
        return;
    }
    r3 = r4 + 0x63e8;
    r4 = 0x8c5;
    r5 = (u32)&lbl_8047C178;
    ((void(*)(void))__assert)();
    r3 = 0x0;
    return;

    r3 = 0x1;

    return;
}
