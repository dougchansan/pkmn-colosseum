/**
 * @file gba_comm_ext.c
 * @brief GBA communication transfers (0x80092C90-0x800937F4)
 *
 * Address range: 0x80092C90 - 0x800937F4
 * Total functions: 9
 */

#include "dolphin/types.h"

#define GBA_DATA_OFFSET 0x20
#define GBA_STATE_PORT 0x4338
#define GBA_STATE_TIMEOUT 0x433C
#define GBA_STATE_PHASE 0x4340
#define GBA_THREAD_PRIORITY 8

/* ===== External function declarations ===== */
extern void fn_800716C8();
extern void fn_800716E8();
extern void fn_80089048();
extern void fn_8009F77C();
extern void fn_8009F7B4();
extern void fn_8009F890();
extern void fn_8009F9C8();
extern void fn_8009FABC();
extern void fn_800A19CC();
extern void fn_800A1E54();
extern void fn_800A1F94();
extern void fn_800A257C();
extern void strcpy();
extern u32 fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void _threadSwitch();
extern void __assert();
extern u32 strlen();
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047C1E8;

/* ===== Rodata / data labels ===== */
extern u8 lbl_8026F5A8[];
extern u8 lbl_803FB328[];

/* ===== Forward declarations ===== */
void fn_80092C90(void);
void fn_80092E38(void);
void fn_80092FC8(void);
void fn_80093160(void);
void fn_800932F0(void);
s32 fn_800934E4(s32 channel);
u32 fn_80093574(s32 channel);
u32 fn_80093610(s32 channel);
s32 fn_80093698(s32 channel);

/* ===== Function implementations ===== */


/* 0x80092C90 | size: 0x1A8 */
void fn_80092C90(void) {
    extern void fn_800937F4();
    extern void fn_80093B04();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    if ((s32)r29 < 0 || (s32)r29 > 3) {
        /* L_80092CBC */
        r0 = 0x0;
    } else {
        /* L_80092CC4 */
        r3 = (u32)&lbl_803FB328;
        r26 = r29 << 2;
        r27 = (u32)&lbl_803FB328;
        r0 = *(u32*)(r27 + r26);
        if (r0 != 0) {
            r0 = 0x1;
        } else {
    r3 = 0x44a0;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r28 = r3;
    r0 = r28 & 0xFFFF;
    if (r0 == 0x0) {
        r3 = (u32)&lbl_8026F5A8;
        r4 = 0x1dd;
        r3 = (u32)&lbl_8026F5A8;
        r5 = (u32)&lbl_8047C1E8;
        ((void(*)(void))__assert)();
    }
    r3 = r28;
    ((void(*)(void))fn_800E27B0)();
    r28 = r3;
    r4 = 0x0;
    r5 = 0x4490;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)(r27 + r26) = r28;
    r3 = (u32)fn_80093B04;
    r5 = (u32)fn_80093B04;
    r0 = 0x0;
    r26 = *(u32*)(r27 + r26);
    r3 = r29;
    *(u32*)((u8*)r26 + 0x4340) = r0;
    r4 = r26 + 0x20;
    *(u32*)((u8*)r26 + 0x4338) = r29;
    ((void(*)(void))fn_800716C8)();
    r3 = r26;
    ((void(*)(void))fn_8009F77C)();
    r3 = r26 + 0x18;
    ((void(*)(void))fn_8009F9C8)();
    r3 = (u32)fn_800937F4;
    r5 = r26;
    r4 = (u32)fn_800937F4;
    r6 = r26 + 0x4338;
    r3 = r26 + 0x20;
    r7 = 0x4000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r26 + 0x20;
    ((void(*)(void))fn_800A1F94)();
    r0 = 0x1;
        }
    }
    /* L_80092D94 */
    if ((s32)r0 == 0x0) {
        r3 = 0x0;
    } else {

        r3 = (u32)&lbl_803FB328;
        r0 = r29 << 2;
        r3 = (u32)&lbl_803FB328;
        r26 = 0x0;
        r27 = *(u32*)(r3 + r0);
        r3 = r27;
        ((void(*)(void))fn_8009F7B4)();
        r0 = *(u32*)((u8*)r27 + 0x4340);
        if ((s32)r0 == 0x0) {
            r4 = r30;
            r5 = r31;
            r3 = r27 + 0x4344;
            ((void(*)(void))fn_80089048)();
            r26 = r3;
            if ((s32)r26 != 0x0) {
                r0 = 0xc;
                r3 = (0x3 << 16);
                *(u32*)((u8*)r27 + 0x4340) = r0;
                r0 = r3 + 0xc;
                *(u32*)((u8*)r27 + 0x433C) = r0;
        }
        }
        r3 = r27;
        ((void(*)(void))fn_8009F890)();
        r3 = r27 + 0x20;
        r4 = 0x8;
        ((void(*)(void))fn_800A257C)();
        if ((s32)r26 != 0x0) {
            r3 = r27 + 0x18;
            ((void(*)(void))fn_8009FABC)();
        }
        r3 = r26;
    }
    return;
}

/* 0x80092E38 | size: 0x190 */
void fn_80092E38(void) {
    extern void fn_800937F4();
    extern void fn_80093B04();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = r4;
    if ((s32)r30 < 0 || (s32)r30 > 3) {
        tmp = 0x0;
    } else {
        r3 = (u32)&lbl_803FB328;
        r27 = r30 << 2;
        r28 = (u32)&lbl_803FB328;
        tmp = *(u32*)(r28 + r27);
        if (tmp != 0) {
            tmp = 0x1;
        } else {
    r3 = 0x44a0;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r29 = r3;
    tmp = r29 & 0xFFFF;
    if (tmp == 0) {
        r3 = (u32)&lbl_8026F5A8;
        r4 = 0x1dd;
        r3 = (u32)&lbl_8026F5A8;
        r5 = (u32)&lbl_8047C1E8;
        ((void(*)(void))__assert)();
    }
    r3 = r29;
    ((void(*)(void))fn_800E27B0)();
    r29 = r3;
    r4 = 0x0;
    r5 = 0x4490;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)(r28 + r27) = r29;
    r3 = (u32)fn_80093B04;
    r5 = (u32)fn_80093B04;
    tmp = 0x0;
    r27 = *(u32*)(r28 + r27);
    r3 = r30;
    *(u32*)((u8*)r27 + 0x4340) = tmp;
    r4 = r27 + 0x20;
    *(u32*)((u8*)r27 + 0x4338) = r30;
    ((void(*)(void))fn_800716C8)();
    r3 = r27;
    ((void(*)(void))fn_8009F77C)();
    r3 = r27 + 0x18;
    ((void(*)(void))fn_8009F9C8)();
    r3 = (u32)fn_800937F4;
    r5 = r27;
    r4 = (u32)fn_800937F4;
    r6 = r27 + 0x4338;
    r3 = r27 + 0x20;
    r7 = 0x4000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r27 + 0x20;
    ((void(*)(void))fn_800A1F94)();
    tmp = 0x1;
        }
    }
    /* L_80092F38 */
    if ((s32)tmp == 0) {
        r3 = 0x0;
    } else {

        r3 = (u32)&lbl_803FB328;
        tmp = r30 << 2;
        r3 = (u32)&lbl_803FB328;
        r27 = 0x0;
        r28 = *(u32*)(r3 + tmp);
        r3 = r28;
        ((void(*)(void))fn_8009F7B4)();
        tmp = *(u32*)((u8*)r28 + 0x4340);
        if ((s32)tmp == 0) {
            tmp = 0xb;
            r3 = 0x30000;
            *(u32*)((u8*)r28 + 0x4340) = tmp;
            tmp = r3 + 0xb;
            r27 = 0x1;
            *(u32*)((u8*)r28 + 0x433C) = tmp;
            *(u32*)((u8*)r28 + 0x4344) = r31;
        }
        r3 = r28;
        ((void(*)(void))fn_8009F890)();
        r3 = r28 + 0x20;
        r4 = 0x8;
        ((void(*)(void))fn_800A257C)();
        if ((s32)r27 != 0) {
            r3 = r28 + 0x18;
            ((void(*)(void))fn_8009FABC)();
        }
        r3 = r27;
    }
    return;
}

/* 0x80092FC8 | size: 0x198 */
void fn_80092FC8(void) {
    extern void fn_800937F4();
    extern void fn_80093B04();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    if ((s32)r29 < 0 || (s32)r29 > 3) {
        tmp = 0x0;
    } else {
        r3 = (u32)&lbl_803FB328;
        r26 = r29 << 2;
        r27 = (u32)&lbl_803FB328;
        tmp = *(u32*)(r27 + r26);
        if (tmp != 0) {
            tmp = 0x1;
        } else {
    r3 = 0x44a0;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r28 = r3;
    tmp = r28 & 0xFFFF;
    if (tmp == 0) {
        r3 = (u32)&lbl_8026F5A8;
        r4 = 0x1dd;
        r3 = (u32)&lbl_8026F5A8;
        r5 = (u32)&lbl_8047C1E8;
        ((void(*)(void))__assert)();
    }
    r3 = r28;
    ((void(*)(void))fn_800E27B0)();
    r28 = r3;
    r4 = 0x0;
    r5 = 0x4490;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)(r27 + r26) = r28;
    r3 = (u32)fn_80093B04;
    r5 = (u32)fn_80093B04;
    tmp = 0x0;
    r26 = *(u32*)(r27 + r26);
    r3 = r29;
    *(u32*)((u8*)r26 + 0x4340) = tmp;
    r4 = r26 + 0x20;
    *(u32*)((u8*)r26 + 0x4338) = r29;
    ((void(*)(void))fn_800716C8)();
    r3 = r26;
    ((void(*)(void))fn_8009F77C)();
    r3 = r26 + 0x18;
    ((void(*)(void))fn_8009F9C8)();
    r3 = (u32)fn_800937F4;
    r5 = r26;
    r4 = (u32)fn_800937F4;
    r6 = r26 + 0x4338;
    r3 = r26 + 0x20;
    r7 = 0x4000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r26 + 0x20;
    ((void(*)(void))fn_800A1F94)();
    tmp = 0x1;
        }
    }
    /* L_800930CC */
    if ((s32)tmp == 0) {
        r3 = 0x0;
    } else {

        r3 = (u32)&lbl_803FB328;
        tmp = r29 << 2;
        r3 = (u32)&lbl_803FB328;
        r26 = 0x0;
        r27 = *(u32*)(r3 + tmp);
        r3 = r27;
        ((void(*)(void))fn_8009F7B4)();
        tmp = *(u32*)((u8*)r27 + 0x4340);
        if ((s32)tmp == 0) {
            tmp = 0x4;
            r3 = 0x30000;
            *(u32*)((u8*)r27 + 0x4340) = tmp;
            tmp = r3 + 0x4;
            r26 = 0x1;
            *(u32*)((u8*)r27 + 0x433C) = tmp;
            *(u32*)((u8*)r27 + 0x4344) = r30;
            *(u32*)((u8*)r27 + 0x4348) = r31;
        }
        r3 = r27;
        ((void(*)(void))fn_8009F890)();
        r3 = r27 + 0x20;
        r4 = 0x8;
        ((void(*)(void))fn_800A257C)();
        if ((s32)r26 != 0) {
            r3 = r27 + 0x18;
            ((void(*)(void))fn_8009FABC)();
        }
        r3 = r26;
    }
    return;
}

/* 0x80093160 | size: 0x190 */
void fn_80093160(void) {
    extern void fn_800937F4();
    extern void fn_80093B04();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = r4;
    if ((s32)r30 < 0 || (s32)r30 > 3) {
        tmp = 0x0;
    } else {
        r3 = (u32)&lbl_803FB328;
        r27 = r30 << 2;
        r28 = (u32)&lbl_803FB328;
        tmp = *(u32*)(r28 + r27);
        if (tmp != 0) {
            tmp = 0x1;
        } else {
    r3 = 0x44a0;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r29 = r3;
    tmp = r29 & 0xFFFF;
    if (tmp == 0) {
        r3 = (u32)&lbl_8026F5A8;
        r4 = 0x1dd;
        r3 = (u32)&lbl_8026F5A8;
        r5 = (u32)&lbl_8047C1E8;
        ((void(*)(void))__assert)();
    }
    r3 = r29;
    ((void(*)(void))fn_800E27B0)();
    r29 = r3;
    r4 = 0x0;
    r5 = 0x4490;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)(r28 + r27) = r29;
    r3 = (u32)fn_80093B04;
    r5 = (u32)fn_80093B04;
    tmp = 0x0;
    r27 = *(u32*)(r28 + r27);
    r3 = r30;
    *(u32*)((u8*)r27 + 0x4340) = tmp;
    r4 = r27 + 0x20;
    *(u32*)((u8*)r27 + 0x4338) = r30;
    ((void(*)(void))fn_800716C8)();
    r3 = r27;
    ((void(*)(void))fn_8009F77C)();
    r3 = r27 + 0x18;
    ((void(*)(void))fn_8009F9C8)();
    r3 = (u32)fn_800937F4;
    r5 = r27;
    r4 = (u32)fn_800937F4;
    r6 = r27 + 0x4338;
    r3 = r27 + 0x20;
    r7 = 0x4000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r27 + 0x20;
    ((void(*)(void))fn_800A1F94)();
    tmp = 0x1;
        }
    }
    /* L_80093260 */
    if ((s32)tmp == 0) {
        r3 = 0x0;
    } else {

        r3 = (u32)&lbl_803FB328;
        tmp = r30 << 2;
        r3 = (u32)&lbl_803FB328;
        r27 = 0x0;
        r28 = *(u32*)(r3 + tmp);
        r3 = r28;
        ((void(*)(void))fn_8009F7B4)();
        tmp = *(u32*)((u8*)r28 + 0x4340);
        if ((s32)tmp == 0) {
            tmp = 0x2;
            r3 = 0x30000;
            *(u32*)((u8*)r28 + 0x4340) = tmp;
            tmp = r3 + 0x2;
            r27 = 0x1;
            *(u32*)((u8*)r28 + 0x433C) = tmp;
            *(u32*)((u8*)r28 + 0x4344) = r31;
        }
        r3 = r28;
        ((void(*)(void))fn_8009F890)();
        r3 = r28 + 0x20;
        r4 = 0x8;
        ((void(*)(void))fn_800A257C)();
        if ((s32)r27 != 0) {
            r3 = r28 + 0x18;
            ((void(*)(void))fn_8009FABC)();
        }
        r3 = r27;
    }
    return;
}

/* 0x800932F0 | size: 0x1F4 */
void fn_800932F0(void) {
    extern void fn_800937F4();
    extern void fn_80093B04();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    if ((s32)r29 < 0 || (s32)r29 > 3) {
        tmp = 0x0;
    } else {
        r3 = (u32)&lbl_803FB328;
        r26 = r29 << 2;
        r27 = (u32)&lbl_803FB328;
        tmp = *(u32*)(r27 + r26);
        if (tmp != 0) {
            tmp = 0x1;
        } else {
    r3 = 0x44a0;
    r4 = 0x20;
    ((void(*)(void))fn_800E2C04)();
    r28 = r3;
    tmp = r28 & 0xFFFF;
    if (tmp == 0) {
        r3 = (u32)&lbl_8026F5A8;
        r4 = 0x1dd;
        r3 = (u32)&lbl_8026F5A8;
        r5 = (u32)&lbl_8047C1E8;
        ((void(*)(void))__assert)();
    }
    r3 = r28;
    ((void(*)(void))fn_800E27B0)();
    r28 = r3;
    r4 = 0x0;
    r5 = 0x4490;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)(r27 + r26) = r28;
    r3 = (u32)fn_80093B04;
    r5 = (u32)fn_80093B04;
    tmp = 0x0;
    r26 = *(u32*)(r27 + r26);
    r3 = r29;
    *(u32*)((u8*)r26 + 0x4340) = tmp;
    r4 = r26 + 0x20;
    *(u32*)((u8*)r26 + 0x4338) = r29;
    ((void(*)(void))fn_800716C8)();
    r3 = r26;
    ((void(*)(void))fn_8009F77C)();
    r3 = r26 + 0x18;
    ((void(*)(void))fn_8009F9C8)();
    r3 = (u32)fn_800937F4;
    r5 = r26;
    r4 = (u32)fn_800937F4;
    r6 = r26 + 0x4338;
    r3 = r26 + 0x20;
    r7 = 0x4000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r26 + 0x20;
    ((void(*)(void))fn_800A1F94)();
    tmp = 0x1;
        }
    }
    /* L_800933F4 */
    if ((s32)tmp == 0) {
        r3 = 0x0;
        return;
    }
    r3 = (u32)&lbl_803FB328;
    tmp = r29 << 2;
    r4 = (u32)&lbl_803FB328;
    r3 = r30;
    r27 = *(u32*)(r4 + tmp);
    r26 = 0x0;
    r3 = (u32)strlen((const char*)r3);
    r29 = r3;
    if (r31 != 0) {
        r3 = r31;
        r3 = (u32)strlen((const char*)r3);
    } else {

        r3 = 0x0;
    }
    if (r29 >= 0x7f || r3 >= 0x7f) {
        /* L_8009344C */
        r26 = 0x0;
        r3 = r26;
        return;
    }
    /* L_80093454 */
    r3 = r27;
    ((void(*)(void))fn_8009F7B4)();
    tmp = *(u32*)((u8*)r27 + 0x4340);
    if ((s32)tmp == 0) {
        r26 = 0x1;
        r3 = 0x30000;
        *(u32*)((u8*)r27 + 0x4340) = r26;
        tmp = r3 + 0x1;
        r4 = r30;
        r3 = r27 + 0x4344;
        *(u32*)((u8*)r27 + 0x433C) = tmp;
        ((void(*)(void))strcpy)();
        if (r31 != 0) {
            r4 = r31;
            r3 = r27 + 0x43c4;
            ((void(*)(void))strcpy)();
        } else {
            /* L_800934A0 */
            tmp = 0x0;
            *(u8*)((u8*)r27 + 0x43C4) = tmp;
        }
    }
    /* L_800934A8 */
    r3 = r27;
    ((void(*)(void))fn_8009F890)();
    r3 = r27 + 0x20;
    r4 = 0x8;
    ((void(*)(void))fn_800A257C)();
    if ((s32)r26 == 0) { r3 = r26; return; }
    r3 = r27 + 0x18;
    ((void(*)(void))fn_8009FABC)();

    r3 = r26;

    return;
}

/* 0x800934E4 | size: 0x90 */
s32 fn_800934E4(s32 channel)
{
#pragma peephole off
    s32 idle;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work != NULL) {
        fn_8009F7B4(work);
        idle = (*(u32*)(work + GBA_STATE_PHASE) == 0);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    } else {
        idle = 1;
    }

    return idle;
}

/* 0x80093574 | size: 0x9C */
u32 fn_80093574(s32 channel)
{
#pragma peephole off
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            return status;
        }
    }
}

/* 0x80093610 | size: 0x88 */
u32 fn_80093610(s32 channel)
{
#pragma peephole off
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    fn_8009F7B4(work);
    status = *(u32*)(work + GBA_STATE_TIMEOUT);
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);

    return status;
}

/* 0x80093698 | size: 0x15C */
s32 fn_80093698(s32 channel)
{
#pragma peephole off
    u32 slot;
    u32 status;
    u8* work;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 1;
    }

    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 1);
    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            break;
        }
    }

    fn_8009F7B4(work);
    *(u32*)(work + GBA_STATE_PHASE) = 0xD;
    *(u32*)(work + GBA_STATE_TIMEOUT) = 0x3000D;
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    fn_8009FABC(work + 0x18);
    fn_800A1E54(work + GBA_DATA_OFFSET, 0);
    fn_800716C8(*(s32*)(work + GBA_STATE_PORT), NULL, NULL);
    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 0);

    status = fn_800E202C(*(u8**)((u8*)lbl_803FB328 + slot));
    if ((status & 0xFFFF) == 0) {
        __assert(lbl_8026F5A8, 0x1E6, &lbl_8047C1E8);
    }
    fn_800E24B0(status);
    fn_800E209C(status);
    *(u8**)((u8*)lbl_803FB328 + slot) = NULL;

    return 1;
}
