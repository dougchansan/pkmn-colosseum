/**
 * @file menu_carde_matrix.c
 * @brief Card-E matrix display (0x8007C300-0x8007FD64)
 *
 * Address range: 0x8007C300 - 0x8007FD64
 * Total functions: 15
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8005D858();
extern void fn_8007FDBC();
extern void fn_80082A88();
extern void fn_80082EA4();
extern void fn_80082FE4();
extern void fn_80083AF4();
extern void fn_80083BF8();
extern void fn_800CA620();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void _threadSwitch();
extern void GScharLenCpy();
extern void fn_801040A0();
extern void fn_801040D0();
extern void fn_801046B8();
extern void fn_80104704();
extern void fn_80105624();
extern void fn_801081F8();
extern void fn_801091F4();
extern void fn_80109220();
extern void fn_80132A38();
extern void fn_80166A28();
extern void __assert();
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047A658;
extern u8 lbl_8047C130;
extern u8 lbl_8047C138;
extern u8 lbl_8047C140;
extern u8 lbl_8047C148;
extern u8 lbl_8047C14C;
extern u8 lbl_8047C150;
extern u8 lbl_8047C154;
extern u8 lbl_8047C158;
extern u8 lbl_8047C15C;
extern u8 lbl_8047C160;
extern u8 lbl_8047C168;
extern u8 lbl_8047C170;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EE868[];
extern u8 lbl_80268B88[];
extern u8 lbl_80268D78[];
extern u8 lbl_80268D8C[];
extern u8 lbl_80268DA0[];
extern u8 lbl_80268DB4[];

/* ===== Forward declarations ===== */
void fn_8007C300(void);
void fn_8007C414(void);
void fn_8007C450(void);
void fn_8007C634(void);
void fn_8007C764(u8 arg);
void fn_8007C7A8(u8 arg);
void fn_8007C7EC(void);
void fn_8007CAB0(void);
void fn_8007CB54(u32 arg);
void fn_8007CBB4(void);
void fn_8007D4FC(void);
void fn_8007D564(void);
void fn_8007D79C(void);
void fn_8007D89C(void);
void fn_8007D978(u32 r3);

/*
 * Raw Card-E matrix offset map. Keep the function bodies below in raw
 * register-style form for matching; these names document the supported
 * MenuCardEMatrixContext overlay in include/game/menu/menu.h.
 *
 *   +0x0A0  prevEntryIndex
 *   +0x0A4  currentEntryIndex
 *   +0x0AC  entryCount
 *   +0x0B0  entries (MenuCardEEntry**)
 *   +0x0B4  prevSubIndex
 *   +0x0B5  currentSubIndex
 *   +0x0B6  transitionActive
 *   +0x0B8  transitionFrame
 *   +0x0BC  state/switch selector used by fn_8007D978
 */

/* ===== Function implementations ===== */

/*
 * 0x8007C300 | size: 0x114
 * Proposed role: set the current Card-E entry by entry+0x1A card id and copy
 * that target into both current/previous entry and sub-selection fields.
 */
void fn_8007C300(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r28 = r3;
    r29 = r4;
    r3 = 0xa6;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r31 = *(u32*)((u8*)r3 + 0x0);
    if (r31 != 0x0) {
        r0 = 0x0;
        *(u8*)((u8*)r31 + 0xC8) = r0;
        r0 = *(u8*)((u8*)r31 + 0xB6);
        while (r0 != 0x0) {
            ((void(*)(void))_threadSwitch)();
            r3 = 0xa6;
            ((void(*)(void))fn_80104704)();
            ((void(*)(void))fn_801040A0)();
            r31 = *(u32*)((u8*)r3 + 0x0);
            if (r31 == 0x0) {
                r31 = 0x0;
                break;
            }
            r0 = *(u8*)((u8*)r31 + 0xB6);
        }
    } else {
        r31 = 0x0;
    }

    if (r31 == 0x0) return;
    r6 = *(u32*)((u8*)r31 + 0xAC);
    r30 = 0x0;
    r5 = 0x0;
    r4 = r28 & 0xFF;
    ctr_fn = (void(*)(void))r6;
    if ((s32)r6 > 0x0) {
        do {
            r3 = *(u32*)((u8*)r31 + 0xB0);
            r3 = *(u32*)(r3 + r5);
            r0 = *(u8*)((u8*)r3 + 0x1A);
            if (r4 == r0) break;
            r5 = r5 + 0x4;
            r30 = r30 + 0x1;
        } while (--ctr != 0);
    }
    if ((s32)r30 >= (s32)r6) {
        r3 = (u32)&lbl_80268D78;
        r5 = (u32)&lbl_80268D8C;
        r3 = (u32)&lbl_80268D78;
        r4 = 0x648;
        r5 = (u32)&lbl_80268D8C;
        ((void(*)(void))__assert)();
    }
    *(u32*)((u8*)r31 + 0xA4) = r30;
    *(u32*)((u8*)r31 + 0xA0) = r30;
    *(u8*)((u8*)r31 + 0xB5) = r29;
    *(u8*)((u8*)r31 + 0xB4) = r29;

    return;
}

/* 0x8007C414 | size: 0x3C */
#pragma push
#pragma scheduling off
void fn_8007C414(void) {
    extern void* fn_80104704(u32 id);
    extern u32* fn_801040A0(void* obj);
    u32 p;

    p = *fn_801040A0(fn_80104704(0xa6));
    if (p != 0) {
        *(u32*)(p + 0xBC) = 0;
    }
    return;
}
#pragma pop

/*
 * 0x8007C450 | size: 0x1E4
 * Proposed role: set a target Card-E selection, preserve the previous
 * entry/sub-selection, and arm the transition flag/frame when the target
 * differs.
 */
void fn_8007C450(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    r3 = 0xa6;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r31 = *(u32*)((u8*)r3 + 0x0);
    if (r31 != 0) {
        tmp = 0x0;
        *(u8*)((u8*)r31 + 0xC8) = tmp;
        tmp = *(u8*)((u8*)r31 + 0xB6);
        while (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            r3 = 0xa6;
            ((void(*)(void))fn_80104704)();
            ((void(*)(void))fn_801040A0)();
            r31 = *(u32*)((u8*)r3 + 0x0);
            if (r31 == 0) {
                r31 = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r31 + 0xB6);
        }
    } else {
        r31 = 0x0;
    }

    if (r31 == 0) return;
    tmp = *(u32*)((u8*)r31 + 0xA4);
    r30 = 0x0;
    r5 = 0x0;
    *(u32*)((u8*)r31 + 0xA0) = tmp;
    tmp = *(u8*)((u8*)r31 + 0xB5);
    *(u8*)((u8*)r31 + 0xB4) = tmp;
    r6 = *(u32*)((u8*)r31 + 0xAC);
    r4 = r25 & 0xFF;
    ctr_fn = (void(*)(void))r6;
    if ((s32)r6 > 0) {
        do {
            r3 = *(u32*)((u8*)r31 + 0xB0);
            r3 = *(u32*)(r3 + r5);
            tmp = *(u8*)((u8*)r3 + 0x1A);
            if (r4 == tmp) break;
            r5 = r5 + 0x4;
            r30 = r30 + 0x1;
        } while (--ctr != 0);
    }
    if ((s32)r30 >= (s32)r6) {
        r3 = (u32)&lbl_80268D78;
        r5 = (u32)&lbl_80268D8C;
        r3 = (u32)&lbl_80268D78;
        r4 = 0x608;
        r5 = (u32)&lbl_80268D8C;
        ((void(*)(void))__assert)();
    }
    *(u32*)((u8*)r31 + 0xA4) = r30;
    r3 = (s8)r28;
    tmp = r30 << 2;
    *(u8*)((u8*)r31 + 0xB5) = r26;
    r4 = *(u32*)((u8*)r31 + 0xB0);
    r4 = *(u32*)(r4 + tmp);
    *(u32*)((u8*)r31 + 0xBC) = r29;
    if ((s32)r3 < 0) {
        tmp = (s8)r27;
        r3 = *(u8*)((u8*)r4 + 0x1D);
        tmp = tmp * 0x6;
        r3 = (s8)r3;
        tmp = r3 + tmp;
        *(u32*)((u8*)r31 + 0xC0) = tmp;
    } else {

        tmp = (s8)r27;
        tmp = tmp * 0x6;
        tmp = r3 + tmp;
        *(u32*)((u8*)r31 + 0xC0) = tmp;
    }
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0xC4) = tmp;
    r3 = *(u32*)((u8*)r31 + 0xA4);
    tmp = *(u32*)((u8*)r31 + 0xA0);
    if ((s32)r3 == (s32)tmp) {
        r3 = *(u8*)((u8*)r31 + 0xB5);
        tmp = *(u8*)((u8*)r31 + 0xB4);
        r3 = (s8)r3;
        tmp = (s8)tmp;
        if ((s32)r3 == (s32)tmp) return;
    }
    r3 = 0x1;
    tmp = 0x0;
    *(u8*)((u8*)r31 + 0xB6) = r3;
    r3 = 0xa6;
    *(u32*)((u8*)r31 + 0xB8) = tmp;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r3 = *(u32*)((u8*)r3 + 0x0);
    if (r3 == 0) return;
    tmp = 0x0;
    *(u8*)((u8*)r3 + 0xC8) = tmp;
    while (1) {
        tmp = *(u8*)((u8*)r3 + 0xB6);
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
        ((void(*)(void))fn_801040A0)();
        r3 = *(u32*)((u8*)r3 + 0x0);
        if (r3 == 0) return;

    }

    return;
}

/* 0x8007C634 | size: 0x130 */
void fn_8007C634(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    r3 = 0xa6;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r4 = *(u32*)((u8*)r3 + 0x0);
    if (r4 != 0) {
        tmp = 0x0;
        *(u8*)((u8*)r4 + 0xC8) = tmp;
        tmp = *(u8*)((u8*)r4 + 0xB6);
        while (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            r3 = 0xa6;
            ((void(*)(void))fn_80104704)();
            ((void(*)(void))fn_801040A0)();
            r4 = *(u32*)((u8*)r3 + 0x0);
            if (r4 == 0) {
                r4 = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r4 + 0xB6);
        }
    } else {
        r4 = 0x0;
    }

    if (r4 == 0) return;
    tmp = *(u32*)((u8*)r4 + 0xAC);
    if ((s32)tmp <= 0) {
        return;
    }
    r3 = *(u32*)((u8*)r4 + 0xA4);
    tmp = 0x0;
    *(u32*)((u8*)r4 + 0xA0) = r3;
    r3 = *(u8*)((u8*)r4 + 0xB5);
    *(u8*)((u8*)r4 + 0xB4) = r3;
    *(u32*)((u8*)r4 + 0xA4) = tmp;
    *(u8*)((u8*)r4 + 0xB5) = tmp;
    r3 = *(u32*)((u8*)r4 + 0xA4);
    tmp = *(u32*)((u8*)r4 + 0xA0);
    if ((s32)r3 == (s32)tmp) {
        r3 = *(u8*)((u8*)r4 + 0xB5);
        tmp = *(u8*)((u8*)r4 + 0xB4);
        r3 = (s8)r3;
        tmp = (s8)tmp;
        if ((s32)r3 == (s32)tmp) return;
    }
    r3 = 0x1;
    tmp = 0x0;
    *(u8*)((u8*)r4 + 0xB6) = r3;
    r3 = 0xa6;
    *(u32*)((u8*)r4 + 0xB8) = tmp;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r3 = *(u32*)((u8*)r3 + 0x0);
    if (r3 == 0) return;
    tmp = 0x0;
    *(u8*)((u8*)r3 + 0xC8) = tmp;
    while (1) {
        tmp = *(u8*)((u8*)r3 + 0xB6);
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
        ((void(*)(void))fn_801040A0)();
        r3 = *(u32*)((u8*)r3 + 0x0);
        if (r3 == 0) return;

    }

    return;
}

/* 0x8007C764 | size: 0x44 */
void fn_8007C764(u8 arg) {
    extern void* fn_80104704(u32 id);
    extern u32* fn_801040A0(void* obj);
    u32 p;

    p = *fn_801040A0(fn_80104704(0xa6));
    if (p != 0) {
        *(u8*)(p + 0xC9) = arg;
    }
    return;
}

/* 0x8007C7A8 | size: 0x44 */
void fn_8007C7A8(u8 arg) {
    extern void* fn_80104704(u32 id);
    extern u32* fn_801040A0(void* obj);
    u32 p;

    p = *fn_801040A0(fn_80104704(0xa6));
    if (p != 0) {
        *(u8*)(p + 0xC8) = arg;
    }
    return;
}

/*
 * 0x8007C7EC | size: 0x2C4
 * Proposed role: rebuild and sort the Card-E entry pointer array, then
 * reselect the current entry using the saved card id at context+0xAA.
 */
void fn_8007C7EC(void) {
    extern s32 menuCardE_CompareEntryPtrs(u32, u32);
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = 0xa6;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r31 = *(u32*)((u8*)r3 + 0x0);
    if (r31 == 0) return;
    tmp = *(u8*)((u8*)r31 + 0xB6);
    if (tmp != 0) {
        r3 = (u32)&lbl_80268D78;
        r5 = (u32)&lbl_80268DA0;
        r3 = (u32)&lbl_80268D78;
        r4 = 0x594;
        r5 = (u32)&lbl_80268DA0;
        ((void(*)(void))__assert)();
    }
    r3 = *(u32*)((u8*)r31 + 0xB0);
    r30 = *(u32*)((u8*)r31 + 0xA4);
    if (r3 != 0) {
        ((void(*)(void))fn_800E202C)();
        r27 = r3;
        tmp = r27 & 0xFFFF;
        if (tmp == 0) {
            r3 = (u32)&lbl_80268D78;
            r4 = 0x1ab;
            r3 = (u32)&lbl_80268D78;
            r5 = (u32)&lbl_8047C140;
            ((void(*)(void))__assert)();
        }
        r3 = r27;
        ((void(*)(void))fn_800E24B0)();
        r3 = r27;
        ((void(*)(void))fn_800E209C)();
        tmp = 0x0;
        *(u32*)((u8*)r31 + 0xB0) = tmp;
    }
    r3 = 0x0;
    ((void(*)(void))fn_80083BF8)();
    r26 = r3;
    *(u32*)((u8*)r31 + 0xAC) = r3;
    if ((s32)r26 != 0) {
        r27 = r26 << 2;
        r4 = 0x20;
        tmp = r27 + 0x1f;
        /* clrrwi r3, tmp, 5 */;
        ((void(*)(void))fn_800E2C04)();
        r28 = r3;
        tmp = r28 & 0xFFFF;
        if (tmp == 0) {
            r3 = (u32)&lbl_80268D78;
            r4 = 0x1a2;
            r3 = (u32)&lbl_80268D78;
            r5 = (u32)&lbl_8047C140;
            ((void(*)(void))__assert)();
        }
        r3 = r28;
        ((void(*)(void))fn_800E27B0)();
        r29 = r3;
        r5 = r27;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
        r28 = 0x0;
        *(u32*)((u8*)r31 + 0xB0) = r29;
        r27 = r28;
        while ((s32)r28 < (s32)r26) {

            r4 = r28;
            r3 = 0x0;
            ((void(*)(void))fn_80083AF4)();
            r4 = *(u32*)((u8*)r31 + 0xB0);
            r28 = r28 + 0x1;
            *(u32*)(r4 + r27) = r3;
            r27 = r27 + 0x4;

        }
        r4 = (u32)menuCardE_CompareEntryPtrs;
        r3 = *(u32*)((u8*)r31 + 0xB0);
        r6 = (u32)menuCardE_CompareEntryPtrs;
        r5 = 0x4;
        r4 = r26;
        ((void(*)(void))fn_800CA620)();
    }
    tmp = -0x1;
    r5 = 0x0;
    *(u32*)((u8*)r31 + 0xA4) = tmp;
    r6 = r5;
    tmp = *(u32*)((u8*)r31 + 0xAC);
    ctr_fn = (void(*)(void))tmp;
    if ((s32)tmp > 0) {
        do {
            r3 = *(u32*)((u8*)r31 + 0xB0);
            r4 = *(s16*)((u8*)r31 + 0xAA);
            r3 = *(u32*)(r3 + r6);
            tmp = *(u8*)((u8*)r3 + 0x1A);
            if ((s32)r4 == (s32)tmp) {
                *(u32*)((u8*)r31 + 0xA4) = r5;
                break;
            }
            r6 = r6 + 0x4;
            r5 = r5 + 0x1;
        } while (--ctr != 0);
    }
    tmp = *(u32*)((u8*)r31 + 0xA4);
    if ((s32)tmp < 0) {
        r3 = *(u32*)((u8*)r31 + 0xAC);
        if ((s32)r3 <= (s32)r30) {
            *(u32*)((u8*)r31 + 0xA4) = tmp;
    }
    }
    tmp = *(u32*)((u8*)r31 + 0xA4);
    *(u32*)((u8*)r31 + 0xA0) = tmp;
    tmp = *(u32*)((u8*)r31 + 0xAC);
    if ((s32)tmp > 0) {
        tmp = *(u32*)((u8*)r31 + 0xA4);
        if ((s32)tmp < 0) {
        }
        r28 = 0x0;

        } else {
        r3 = *(u32*)((u8*)r31 + 0xB0);
        tmp = tmp << 2;
        r28 = *(u32*)(r3 + tmp);
        }
    if (r28 != 0) {
        r29 = *(u8*)((u8*)r31 + 0xB5);
        while (1) {
            tmp = (s8)r29;
            if ((s32)tmp <= 0) break;
            r3 = r28;
            r4 = r29;
            ((void(*)(void))fn_80082A88)();
            tmp = r3 & 0xFF;
            if (tmp != 0) break;

        }

        *(u8*)((u8*)r31 + 0xB5) = r29;
    }
    r3 = *(u32*)((u8*)r31 + 0xA4);
    tmp = *(u32*)((u8*)r31 + 0xA0);
    if ((s32)r3 == (s32)tmp) {
        r3 = *(u8*)((u8*)r31 + 0xB5);
        tmp = *(u8*)((u8*)r31 + 0xB4);
        r3 = (s8)r3;
        tmp = (s8)tmp;
        if ((s32)r3 == (s32)tmp) return;
    }
    r3 = 0x1;
    tmp = 0x0;
    *(u8*)((u8*)r31 + 0xB6) = r3;
    r3 = 0xa6;
    *(u32*)((u8*)r31 + 0xB8) = tmp;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r3 = *(u32*)((u8*)r3 + 0x0);
    if (r3 == 0) return;
    tmp = 0x0;
    *(u8*)((u8*)r3 + 0xC8) = tmp;
    while (1) {
        tmp = *(u8*)((u8*)r3 + 0xB6);
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
        ((void(*)(void))fn_801040A0)();
        r3 = *(u32*)((u8*)r3 + 0x0);
        if (r3 == 0) return;

    }

    return;
}

/* 0x8007CAB0 | size: 0xA4 */
void fn_8007CAB0(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    r3 = 0xa6;
    ((void(*)(void))fn_80104704)();
    ((void(*)(void))fn_801040A0)();
    r5 = *(u32*)((u8*)r3 + 0x0);
    if (r5 == 0) return;
    tmp = *(u8*)((u8*)r5 + 0xB6);
    if (tmp == 0) {
        tmp = *(u32*)((u8*)r5 + 0xA4);
        *(u32*)((u8*)r5 + 0xA0) = tmp;
        tmp = *(u8*)((u8*)r5 + 0xB5);
        *(u8*)((u8*)r5 + 0xB4) = tmp;
    }
    r4 = -0x1;
    tmp = *(u32*)((u8*)r5 + 0xA0);
    if ((s32)tmp < 0) {
        *(u16*)((u8*)r5 + 0xA8) = r4;
    } else {

        r3 = *(u32*)((u8*)r5 + 0xB0);
        tmp = tmp << 2;
        r3 = *(u32*)(r3 + tmp);
        tmp = *(u8*)((u8*)r3 + 0x1A);
        *(u16*)((u8*)r5 + 0xA8) = tmp;
    }
    tmp = *(u32*)((u8*)r5 + 0xA4);
    if ((s32)tmp < 0) {
        *(u16*)((u8*)r5 + 0xAA) = r4;
        return;
    }
    r3 = *(u32*)((u8*)r5 + 0xB0);
    tmp = tmp << 2;
    r3 = *(u32*)(r3 + tmp);
    tmp = *(u8*)((u8*)r3 + 0x1A);
    *(u16*)((u8*)r5 + 0xAA) = tmp;

    return;
}

/* 0x8007CB54 | size: 0x60 */
void fn_8007CB54(u32 arg) {
    extern void* fn_80104704(u32 id);
    extern u32* fn_801040A0(void* obj);
    extern void GScharLenCpy(u32 p, u32 arg, u32 size);
    u32 p;

    p = *fn_801040A0(fn_80104704(0xa6));
    if (p != 0) {
        GScharLenCpy(p, arg, 0x50);
        *(u16*)(p + 0x9E) = 0;
    }
    return;
}

/* 0x8007CBB4 | size: 0x948 */
void fn_8007CBB4(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r4;
    r4 = (u32)&lbl_80268B88;
    r29 = (u32)&lbl_80268B88;
    if (r3 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    r28 = *(u32*)((u8*)r3 + 0x0);
    tmp = 0x0;
    r30 = 0x0;
    *(u32*)((u8*)r31 + 0x4C) = tmp;
    if (r28 == 0) return;
    r3 = r31;
    ((void(*)(void))fn_801091F4)();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    tmp = *(s16*)((u8*)r31 + 0x6);
    r6 = r29 + 0x0;
    r5 = r6;
    r4 = 0x0;
    tmp = tmp & 0xFFFF;
    r3 = 0x4;
    ctr_fn = (void(*)(void))r3;
    do {
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
        r3 = *(u16*)((u8*)r5 + 0x0);
        if (tmp == r3) {
            break;
        }
        r5 = r5 + 0x2;
        r4 = r4 + 0x1;
    } while (--ctr != 0);
    r4 = -0x1;

    r7 = r4;
    if ((s32)r4 >= 0) {
        tmp = 0x0;
        r26 = 0x0;

    } else { do {
        r4 = 0x0;
        r3 = 0x4;
        ctr_fn = (void(*)(void))r3;
        do {
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
        } while (--ctr != 0);
        r4 = -0x1;

        r7 = r4;
        if ((s32)r4 >= 0) {
            tmp = 0x1;
            r26 = 0x0;
            break;
        }
        r6 = r29 + 0x120;
        r4 = 0x0;
        r5 = r6;
        r3 = 0x4;
        ctr_fn = (void(*)(void))r3;
        do {
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
        } while (--ctr != 0);
        r4 = -0x1;

        r7 = r4;
        if ((s32)r4 >= 0) {
            tmp = 0x0;
            r26 = 0x1;
            break;
        }
        r4 = 0x0;
        r3 = 0x4;
        ctr_fn = (void(*)(void))r3;
        do {
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r6 + 0x48);
            if (tmp == r3) {
                break;
            }
            r6 = r6 + 0x2;
            r4 = r4 + 0x1;
        } while (--ctr != 0);
        r4 = -0x1;

        r7 = r4;
        if ((s32)r4 >= 0) {
            tmp = 0x1;
            r26 = 0x1;
            break;
        }
        r6 = r29 + 0x90;
        r4 = 0x0;
        r5 = r6;
        r3 = 0x4;
        ctr_fn = (void(*)(void))r3;
        do {
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
            r3 = *(u16*)((u8*)r5 + 0x0);
            if (tmp == r3) {
                break;
            }
            r5 = r5 + 0x2;
            r4 = r4 + 0x1;
        } while (--ctr != 0);
        r4 = -0x1;

        r7 = r4;
        if ((s32)r4 >= 0) {
            tmp = 0x0;
            r26 = 0x2;

        } else {
            r4 = 0x0;
            r3 = 0x4;
            ctr_fn = (void(*)(void))r3;
            do {
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
                r3 = *(u16*)((u8*)r6 + 0x48);
                if (tmp == r3) {
                    break;
                }
                r6 = r6 + 0x2;
                r4 = r4 + 0x1;
            } while (--ctr != 0);
            r4 = -0x1;

            r7 = r4;
            if ((s32)r4 < 0) return;
            tmp = 0x1;
            r26 = 0x2;
        }
    } while (0); }
do {
    r3 = *(u32*)((u8*)r28 + 0xAC);
    if ((s32)r3 > 0) {
        r3 = tmp << 2;
        r3 = r28 + r3;
        r3 = *(u32*)((u8*)r3 + 0xA0);
        if ((s32)r3 < 0) {
        }
        r3 = 0x0;

        } else {
        r4 = *(u32*)((u8*)r28 + 0xB0);
        r3 = r3 << 2;
        r3 = *(u32*)(r4 + r3);
        }
    if (r3 == 0) return;
    r5 = 0x2AAB0000;
    r4 = *(u8*)((u8*)r3 + 0x1C);
    r6 = (s32)((s64)r5 * (s64)r7 >> 32);
    r4 = (s8)r4;
    r5 = (u32)r6 >> 31;
    r5 = r6 + r5;
    r6 = r5 * 0x6;
    r27 = (s8)r5;
    r5 = r3 + r27;
    r4 = r7 - r6;
    r25 = *(u8*)((u8*)r5 + 0x1E);
    r6 = (s8)r4;
    if ((s32)r27 >= (s32)r4) return;
    r4 = *(u8*)((u8*)r3 + 0x1D);
    r4 = (s8)r4;
    if ((s32)r6 == (s32)r4) {
        r4 = r28 + tmp;
        r4 = *(u8*)((u8*)r4 + 0xB4);
        ((void(*)(void))fn_80082FE4)();
        if ((s32)r26 < 0 || (s32)r26 >= 3) {
            break;
        } else if ((s32)r26 == 0) {
            tmp = 0x3ccf;
            *(u32*)((u8*)r31 + 0x4C) = tmp;
            break;
        } else if ((s32)r26 == 1) {
            r5 = r27 * 0xe;
            r4 = r3 + r5;
            tmp = *(u8*)((u8*)r4 + 0x1C);
            if (tmp == 0) break;
            tmp = 0xe5;
            r4 = r5 + 0x10;
            *(u32*)((u8*)r31 + 0x4C) = tmp;
            r4 = r3 + r4;
            r3 = 0x37;
            ((void(*)(void))fn_80132A38)();
            break;
        } else {
            tmp = r25 << 2;
            r3 = r29 + 0x1d0;
            r3 = *(u32*)(r3 + tmp);
            ((void(*)(void))fn_8005D858)();
            r30 = r3;
            break;
        }
    }
    if (tmp >= 0) return;
    r4 = r28 + tmp;
    r5 = r27;
    r4 = *(u8*)((u8*)r4 + 0xB4);
    ((void(*)(void))fn_80082EA4)();
    r27 = r3;
    tmp = *(u8*)((u8*)r27 + 0xC);
    if (tmp != 0) {
        r4 = r27;
        r3 = 0x37;
        ((void(*)(void))fn_80132A38)();
    }
    if ((s32)r26 < 0 || (s32)r26 >= 3) {
        break;
    } else if ((s32)r26 == 1) {
        tmp = *(u8*)((u8*)r27 + 0xC);
        if (tmp == 0) break;
        tmp = 0xe5;
        r4 = r27;
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        r3 = 0x37;
        ((void(*)(void))fn_80132A38)();
        break;
    } else if ((s32)r26 >= 2) {
        tmp = r25 << 2;
        r3 = r29 + 0x1b0;
        r3 = *(u32*)(r3 + tmp);
        ((void(*)(void))fn_8005D858)();
        r30 = r3;
        break;
    }
    tmp = *(u8*)((u8*)r27 + 0xC);
    if (tmp != 0) {
        tmp = 0xe5;
        r4 = 0x51EC0000;
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        r3 = 0x66660000;
        r7 = (u32)&lbl_8047A658;
        tmp = *(u16*)((u8*)r27 + 0xE);
        r8 = r3 + 0x6667;
        r4 = 0x0;
        r3 = 0x37;
        r5 = (s32)((s64)r5 * (s64)tmp >> 32);
        *(u16*)((u8*)r7 + 0x6) = r4;
        r4 = (u32)&lbl_8047A658;
        r5 = (s32)r5 >> 5;
        r6 = (u32)r5 >> 31;
        r12 = r5 + r6;
        r5 = (s32)((s64)r8 * (s64)r12 >> 32);
        r5 = (s32)r5 >> 2;
        r9 = (s32)((s64)r8 * (s64)tmp >> 32);
        r6 = (u32)r5 >> 31;
        r11 = r5 + r6;
        r5 = (s32)r9 >> 2;
        r6 = (u32)r5 >> 31;
        r10 = r5 + r6;
        r5 = (s32)((s64)r8 * (s64)r10 >> 32);
        r8 = (s32)r5 >> 2;
        r5 = (s32)r9 >> 2;
        r9 = (u32)r8 >> 31;
        r6 = (u32)r5 >> 31;
        r8 = r8 + r9;
        r5 = r5 + r6;
        r9 = r11 * 0xa;
        r6 = r8 * 0xa;
        r8 = r12 - r9;
        r5 = r5 * 0xa;
        r8 = r8 + 0x30;
        r6 = r10 - r6;
        r8 = r8 & 0xFFFF;
        r5 = tmp - r5;
        r6 = r6 + 0x30;
        tmp = r5 + 0x30;
        *(u16*)&lbl_8047A658 = r8;
        r5 = r6 & 0xFFFF;
        tmp = tmp & 0xFFFF;
        *(u16*)((u8*)r7 + 0x2) = r5;
        *(u16*)((u8*)r7 + 0x4) = tmp;
        ((void(*)(void))fn_80132A38)();
        break;
        return;
    }
} while (0);
    if (r30 == 0) return;
    tmp = *(u32*)((u8*)r30 + 0x10);
    *(u32*)((u8*)r31 + 0x58) = tmp;
    tmp = *(s16*)((u8*)r30 + 0x8);
    *(u16*)((u8*)r31 + 0x5C) = tmp;
    tmp = *(s16*)((u8*)r30 + 0xA);
    *(u16*)((u8*)r31 + 0x5E) = tmp;
    tmp = *(s16*)((u8*)r30 + 0xC);
    *(u16*)((u8*)r31 + 0x60) = tmp;
    tmp = *(s16*)((u8*)r30 + 0xE);
    *(u16*)((u8*)r31 + 0x62) = tmp;

    return;
}

/* 0x8007D4FC | size: 0x68 */
void fn_8007D4FC(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    r31 = r4;
    if (r3 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    r4 = *(u32*)((u8*)r3 + 0x0);
    tmp = *(u16*)((u8*)r4 + 0x0);
    if (tmp != 0) {
        tmp = 0xe4;
        r3 = 0x37;
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        ((void(*)(void))fn_80132A38)();
    } else {

        tmp = 0x0;
        *(u32*)((u8*)r31 + 0x4C) = tmp;
    }
    return;
}

/* 0x8007D564 | size: 0x238 */
void fn_8007D564(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r4;
    if (r3 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    r3 = *(u32*)((u8*)r3 + 0x0);
    if (r3 == 0) return;
    tmp = *(s16*)((u8*)r30 + 0x6);
    do {
    if ((s32)tmp != 0x797) {
        if ((s32)tmp < 0x797) {
            if ((s32)tmp != 0x793) {
                if ((s32)tmp < 0x793) break;
                if ((s32)tmp < 0x796) {
                    break;
                }
                if ((s32)tmp != 0x1194) {
                    if ((s32)tmp < 0x1194) {
                        if ((s32)tmp < 0x1193) {
                            break;
                        }
                        if ((s32)tmp < 0x1196) {

                            } else {
                            tmp = *(u32*)((u8*)r3 + 0xAC);
                            r31 = 0x0;
                            if ((s32)tmp > 0) {
                                tmp = *(u32*)((u8*)r3 + 0xA0);
                                if ((s32)tmp < 0) {
                                }
                                tmp = 0x0;

                                } else {
                                r3 = *(u32*)((u8*)r3 + 0xB0);
                                tmp = tmp << 2;
                                tmp = *(u32*)(r3 + tmp);
                                }
                            r29 = tmp;
                        }
                        break;
                        }
                    tmp = *(u32*)((u8*)r3 + 0xAC);
                    r31 = 0x1;
                    if ((s32)tmp > 0) {
                        tmp = *(u32*)((u8*)r3 + 0xA0);
                        if ((s32)tmp < 0) {
                        }
                        tmp = 0x0;

                        } else {
                        r3 = *(u32*)((u8*)r3 + 0xB0);
                        tmp = tmp << 2;
                        tmp = *(u32*)(r3 + tmp);
                        }
                    r29 = tmp;
                    break;
                }
                tmp = *(u32*)((u8*)r3 + 0xAC);
                r31 = 0x2;
                if ((s32)tmp > 0) {
                    tmp = *(u32*)((u8*)r3 + 0xA0);
                    if ((s32)tmp < 0) {
                    }
                    tmp = 0x0;

                    } else {
                    r3 = *(u32*)((u8*)r3 + 0xB0);
                    tmp = tmp << 2;
                    tmp = *(u32*)(r3 + tmp);
                    }
                r29 = tmp;
                break;
                }
            tmp = *(u32*)((u8*)r3 + 0xAC);
            r31 = 0x0;
            if ((s32)tmp > 0) {
                tmp = *(u32*)((u8*)r3 + 0xA4);
                if ((s32)tmp < 0) {
                }
                tmp = 0x0;

                } else {
                r3 = *(u32*)((u8*)r3 + 0xB0);
                tmp = tmp << 2;
                tmp = *(u32*)(r3 + tmp);
                }
            r29 = tmp;
            break;
            }
        tmp = *(u32*)((u8*)r3 + 0xAC);
        r31 = 0x1;
        if ((s32)tmp > 0) {
            tmp = *(u32*)((u8*)r3 + 0xA4);
            if ((s32)tmp < 0) {
            }
            tmp = 0x0;

            } else {
            r3 = *(u32*)((u8*)r3 + 0xB0);
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            }
        r29 = tmp;

    } else {
        tmp = *(u32*)((u8*)r3 + 0xAC);
        r31 = 0x2;
        if ((s32)tmp > 0) {
            tmp = *(u32*)((u8*)r3 + 0xA4);
            if ((s32)tmp < 0) {
            }
            tmp = 0x0;

            } else {
            r3 = *(u32*)((u8*)r3 + 0xB0);
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            }
        r29 = tmp;
    }
    } while (0);
    if (r29 != 0) {
        tmp = *(u8*)((u8*)r29 + 0x1B);
        r3 = (s8)r31;
        tmp = (s8)tmp;
        if ((s32)r3 < (s32)tmp) {
            r3 = r29;
            r4 = r31;
            ((void(*)(void))fn_80082FE4)();
            tmp = 0xe4;
            r4 = r3;
            *(u32*)((u8*)r30 + 0x4C) = tmp;
            r3 = 0x37;
            ((void(*)(void))fn_80132A38)();
            return;
    }
    }
    tmp = 0x0;
    *(u32*)((u8*)r30 + 0x4C) = tmp;

    return;
}

/* 0x8007D79C | size: 0x100 */
void fn_8007D79C(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r4;
    if (r3 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    r4 = *(u32*)((u8*)r3 + 0x0);
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0x4C) = tmp;
    if (r4 == 0) return;
    tmp = *(s16*)((u8*)r31 + 0x6);
    do {
        if ((s32)tmp != 0x1126) {
            if ((s32)tmp >= 0x1126) return;
            if ((s32)tmp != 0x795) {
                return;
            }
            r5 = 0x0;
            break;
            }
        r5 = 0x1;
        break;
        return;
    } while (0);

    tmp = *(u32*)((u8*)r4 + 0xAC);
    if ((s32)tmp > 0) {
        tmp = r5 << 2;
        r3 = r4 + tmp;
        tmp = *(u32*)((u8*)r3 + 0xA0);
        if ((s32)tmp < 0) {
        }
        r3 = 0x0;

        } else {
        r3 = *(u32*)((u8*)r4 + 0xB0);
        tmp = tmp << 2;
        r3 = *(u32*)(r3 + tmp);
        }
    if (r3 == 0) return;
    r4 = r4 + r5;
    r4 = *(u8*)((u8*)r4 + 0xB4);
    ((void(*)(void))fn_80082FE4)();
    r30 = r3;
    tmp = *(u8*)((u8*)r30 + 0x71);
    if (tmp == 0) return;
    tmp = 0x3cbe;
    r3 = 0x58;
    *(u32*)((u8*)r31 + 0x4C) = tmp;
    r4 = *(u8*)((u8*)r30 + 0x70);
    ((void(*)(void))fn_80132A38)();
    r4 = r30 + 0x64;
    r3 = 0x23;
    ((void(*)(void))fn_80132A38)();

    return;
}

/* 0x8007D89C | size: 0xDC */
void fn_8007D89C(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    r31 = r4;
    if (r3 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    r3 = *(u32*)((u8*)r3 + 0x0);
    if (r3 == 0) return;
    tmp = *(s16*)((u8*)r31 + 0x6);
    if ((s32)tmp == 0x791) {
        tmp = *(u32*)((u8*)r3 + 0xAC);
        if ((s32)tmp > 0) {
            tmp = *(u32*)((u8*)r3 + 0xA0);
            if ((s32)tmp < 0) {
            }
            tmp = 0x0;

            } else {
            r3 = *(u32*)((u8*)r3 + 0xB0);
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            }
        r4 = tmp;

    } else {
        tmp = *(u32*)((u8*)r3 + 0xAC);
        if ((s32)tmp > 0) {
            tmp = *(u32*)((u8*)r3 + 0xA4);
            if ((s32)tmp < 0) {
            }
            tmp = 0x0;

            } else {
            r3 = *(u32*)((u8*)r3 + 0xB0);
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            }
        r4 = tmp;
    }
    if (r4 != 0) {
        tmp = 0xe3;
        r3 = 0x37;
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        ((void(*)(void))fn_80132A38)();
        return;
    }
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0x4C) = tmp;

    return;
}

/*
 * 0x8007D978 | size: 0x23EC
 * Proposed role: Card-E matrix main state machine. The switch over
 * jumptable_802EE868 has 10 cases and uses context+0xBC as its selector.
 */
#pragma push
#pragma peephole off
#pragma scheduling off
#pragma optimization_level 3
#pragma fp_contract on
void fn_8007D978(u32 r3) {
    extern u8 jumptable_802EE868[];
    u8 sp[0x260];
    u32 tmp = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f8 = 0.0f;
    f32 f26 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r14 = r3;
    if (r14 == 0) {
        r3 = 0xa6;
        fn_80104704();
    }
    fn_801040A0();
    tmp = *(u8*)((u8*)r14 + 0x1);
    r15 = *(u32*)((u8*)r3 + 0x0);
    tmp = (s8)tmp;
    r18 = r15;
    do {
    if ((s32)tmp != 3) {
        if ((s32)tmp < 3) {
            if ((s32)tmp != 0) {
                break;
            }
            if ((s32)tmp != 5) {
                break;
                }
            tmp = *(u8*)((u8*)r14 + 0x2);
            tmp = (s8)tmp;
            if ((s32)tmp == 0) {
                r3 = r14;
                r4 = 0x0;
                fn_801040D0();
                r4 = r3;
                r3 = r14;
                fn_8007FDBC();
                tmp = r3;
                r3 = r14;
                r18 = tmp;
                fn_801040A0();
                *(u32*)((u8*)r3 + 0x0) = r18;
                r3 = r14;
                r4 = 0x79a;
                r5 = 0x40;
                fn_801081F8();
                r3 = r14;
                r4 = 0x826;
                r5 = 0x48;
                fn_801081F8();
            }
            break;
        }
        tmp = *(u8*)((u8*)r14 + 0x2);
        tmp = (s8)tmp;
        if ((s32)tmp == 0) {
            tmp = 0x1;
            *(u8*)((u8*)r14 + 0x2) = tmp;
        }

            } else {
        r3 = *(u32*)((u8*)r15 + 0xB0);
        if (r3 != 0) {
            fn_800E202C();
            r14 = r3;
            tmp = r14 & 0xFFFF;
            if (tmp == 0) {
                r3 = (u32)&lbl_80268D78;
                r4 = 0x1ab;
                r3 = (u32)&lbl_80268D78;
                r5 = (u32)&lbl_8047C140;
                __assert();
            }
            r3 = r14;
            fn_800E24B0();
            r3 = r14;
            fn_800E209C();
            tmp = 0x0;
            *(u32*)((u8*)r15 + 0xB0) = tmp;
        }
        r3 = r15;
        fn_800E202C();
        r14 = r3;
        tmp = r14 & 0xFFFF;
        if (tmp == 0) {
            r3 = (u32)&lbl_80268D78;
            r4 = 0x1ab;
            r3 = (u32)&lbl_80268D78;
            r5 = (u32)&lbl_8047C140;
            __assert();
        }
        r3 = r14;
        fn_800E24B0();
        r3 = r14;
        fn_800E209C();
        return;
            }
    } while (0);
do {
    r16 = r18;
do {
    r19 = 0x0;
    do {
        r3 = *(u32*)((u8*)r16 + 0x118);
        r4 = 0x0;
        fn_80109220();
        r3 = *(u32*)((u8*)r16 + 0x124);
        r4 = 0x0;
        fn_80109220();
        r15 = r16;
        r17 = 0x0;
        do {
            r3 = *(u32*)((u8*)r15 + 0x130);
            r4 = 0x0;
            fn_80109220();
            r15 = r15 + 0xc;
            r17 = r17 + 0x1;
        } while ((s32)r17 < 2);
        r16 = r16 + 0x4;
        r19 = r19 + 0x1;
    } while ((s32)r19 < 3);
    r15 = r18;
    r16 = 0x0;
    do {
        r3 = *(u32*)((u8*)r15 + 0x148);
        r4 = 0x0;
        fn_80109220();
        r3 = *(u32*)((u8*)r15 + 0x150);
        r4 = 0x0;
        fn_80109220();
        r3 = *(u32*)((u8*)r15 + 0x158);
        r4 = 0x0;
        fn_80109220();
        r15 = r15 + 0x4;
        r16 = r16 + 0x1;
    } while ((s32)r16 < 2);
    r3 = *(u32*)((u8*)r18 + 0x160);
    r4 = 0x0;
    fn_80109220();
    r3 = *(u32*)((u8*)r18 + 0x164);
    r4 = 0x0;
    fn_80109220();
    r3 = *(u32*)((u8*)r18 + 0x168);
    r4 = 0x0;
    fn_80109220();
    r3 = *(u32*)((u8*)r18 + 0x16C);
    r4 = 0x0;
    fn_80109220();
    r16 = r18;
    r17 = 0x0;
    do {
        r15 = r16;
        r19 = 0x0;
        do {
            r3 = *(u32*)((u8*)r15 + 0x170);
            r4 = 0x0;
            fn_80109220();
            r3 = *(u32*)((u8*)r15 + 0x290);
            r4 = 0x0;
            fn_80109220();
            r3 = *(u32*)((u8*)r15 + 0x3B0);
            r4 = 0x0;
            fn_80109220();
            r15 = r15 + 0x90;
            r19 = r19 + 0x1;
        } while ((s32)r19 < 2);
        r16 = r16 + 0x4;
        r17 = r17 + 0x1;
    } while ((s32)r17 < 0x24);
    r15 = r18;
    r16 = 0x0;
    do {
        r3 = *(u32*)((u8*)r15 + 0x4D8);
        r4 = 0x0;
        fn_80109220();
        r15 = r15 + 0x4;
        r16 = r16 + 0x1;
    } while (r16 < 2);
    r15 = r18;
    r16 = 0x0;
    do {
        r3 = *(u32*)((u8*)r15 + 0x4E0);
        r4 = 0x0;
        fn_80109220();
        r15 = r15 + 0x4;
        r16 = r16 + 0x1;
    } while (r16 < 2);
    r3 = *(u32*)((u8*)r18 + 0x4D0);
    r4 = 0x0;
    fn_80109220();
    r3 = *(u32*)((u8*)r18 + 0x4D4);
    r4 = 0x0;
    fn_80109220();
    fn_801046B8();
    tmp = *(u32*)((u8*)r14 + 0x4);
    if ((s32)tmp == (s32)r3) {
        tmp = *(u8*)((u8*)r18 + 0xC8);
        if (tmp != 0) {
            tmp = *(u8*)((u8*)r18 + 0xC9);
            if (tmp != 0) {
                r15 = r18;
                r16 = 0x0;
                do {
                    r3 = *(u32*)((u8*)r15 + 0x4D8);
                    r4 = 0x1;
                    fn_80109220();
                    r15 = r15 + 0x4;
                    r16 = r16 + 0x1;
                } while (r16 < 2);
            }
            r15 = r18;
            r16 = 0x0;
            do {
                r3 = *(u32*)((u8*)r15 + 0x4E0);
                r4 = 0x1;
                fn_80109220();
                r15 = r15 + 0x4;
                r16 = r16 + 0x1;
            } while (r16 < 2);
    }
    }
    fn_80105624();
    r4 = *(u16*)((u8*)r3 + 0x0);
    r3 = *(u16*)((u8*)r18 + 0xCC);
    tmp = r4 & 0xFFFF;
    r3 = r3 & 0xF;
    tmp = tmp & 0xF;
    if ((s32)r3 != (s32)tmp) {
        tmp = 0x0;
        *(u8*)((u8*)r18 + 0xCA) = tmp;
    }
    *(u16*)((u8*)r18 + 0xCC) = r4;
    tmp = *(u32*)((u8*)r18 + 0xAC);
    if ((s32)tmp > 0) {
        tmp = *(u32*)((u8*)r18 + 0xA4);
        if ((s32)tmp < 0) {
        }
        tmp = 0x0;

        } else {
        r3 = *(u32*)((u8*)r18 + 0xB0);
        tmp = tmp << 2;
        tmp = *(u32*)(r3 + tmp);
        }
    r4 = 0x0;
    *(u32*)(sp + 0xA8) = tmp;
    if (tmp == 0) {
        r3 = *(u32*)((u8*)r18 + 0x4D4);
        r4 = 0x1;
        fn_80109220();
        fn_801046B8();
        tmp = *(u32*)((u8*)r14 + 0x4);
        if ((s32)tmp != (s32)r3) return;
        tmp = *(u8*)((u8*)r18 + 0xC8);
        if (tmp == 0) return;
        fn_80105624();
        tmp = *(u16*)((u8*)r3 + 0x6);
        tmp = tmp & 0xF;
        if ((s32)tmp == 0) return;
        tmp = *(u8*)((u8*)r18 + 0xCA);
        if (tmp == 0) {
            r3 = 0x26;
            fn_80166A28();
        }
        tmp = 0x1;
        *(u8*)((u8*)r18 + 0xCA) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r18 + 0xB6);
    if (tmp != 0) {
        r3 = *(u32*)((u8*)r18 + 0xB8);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r18 + 0xB8) = tmp;
        tmp = *(u32*)((u8*)r18 + 0xB8);
        if ((s32)tmp == 0x10) {
            *(u32*)((u8*)r18 + 0xB8) = r4;
            f26 = *(f32*)&lbl_8047C148;
            *(u8*)((u8*)r18 + 0xB6) = r4;
            break;
        }
        tmp = 0x43300000;
        f2 = *(f64*)&lbl_8047C168;
        *(u32*)(sp + 0xB0) = tmp;
        f0 = *(f32*)&lbl_8047C14C;
        f1 = f1 - f2;
        f26 = f1 * f0;
        break;
    }
    tmp = *(u32*)((u8*)r18 + 0xA4);
    *(u32*)((u8*)r18 + 0xA0) = tmp;
    tmp = *(u8*)((u8*)r18 + 0xB5);
    *(u8*)((u8*)r18 + 0xB4) = tmp;
    fn_801046B8();
    tmp = *(u32*)((u8*)r14 + 0x4);
    if ((s32)tmp == (s32)r3) {
        tmp = *(u8*)((u8*)r18 + 0xC8);
        if (tmp == 0) break;
        fn_80105624();
        tmp = *(u16*)((u8*)r3 + 0x6);
        tmp = tmp & 0x1;
        if ((s32)tmp != 0) {
            tmp = *(u32*)((u8*)r18 + 0xA4);
            if ((s32)tmp == 0) {
                tmp = *(u8*)((u8*)r18 + 0xCA);
                if (tmp == 0) {
                    r3 = 0x26;
                    fn_80166A28();
                }
                tmp = 0x1;
                *(u8*)((u8*)r18 + 0xCA) = tmp;
                break;
            }
            r3 = 0x3c5;
            fn_80166A28();
            r3 = *(u32*)((u8*)r18 + 0xA4);
            tmp = 0x1;
            *(u32*)((u8*)r18 + 0xA4) = r3;
            *(u8*)((u8*)r18 + 0xB6) = tmp;
            break;
        }
        fn_80105624();
        tmp = *(u16*)((u8*)r3 + 0x6);
        tmp = tmp & 0x00000002;
        if ((s32)tmp != 0) {
            r3 = *(u32*)((u8*)r18 + 0xAC);
            r4 = *(u32*)((u8*)r18 + 0xA4);
            if ((s32)r4 == (s32)tmp) {
                tmp = *(u8*)((u8*)r18 + 0xCA);
                if (tmp == 0) {
                    r3 = 0x26;
                    fn_80166A28();
                }
                tmp = 0x1;
                *(u8*)((u8*)r18 + 0xCA) = tmp;
                break;
            }
            r3 = 0x3c5;
            fn_80166A28();
            r3 = *(u32*)((u8*)r18 + 0xA4);
            tmp = 0x1;
            r3 = r3 + 0x1;
            *(u32*)((u8*)r18 + 0xA4) = r3;
            *(u8*)((u8*)r18 + 0xB6) = tmp;
            break;
        }
        fn_80105624();
        tmp = *(u16*)((u8*)r3 + 0x6);
        tmp = tmp & 0x00000004;
        if ((s32)tmp != 0) {
            tmp = *(u8*)((u8*)r18 + 0xB5);
            tmp = (s8)tmp;
            if ((s32)tmp == 0) {
                tmp = *(u8*)((u8*)r18 + 0xCA);
                if (tmp == 0) {
                    r3 = 0x26;
                    fn_80166A28();
                }
                tmp = 0x1;
                *(u8*)((u8*)r18 + 0xCA) = tmp;
                break;
            }
            r3 = 0x3c5;
            fn_80166A28();
            r3 = *(u8*)((u8*)r18 + 0xB5);
            tmp = 0x1;
            *(u8*)((u8*)r18 + 0xB5) = r3;
            *(u8*)((u8*)r18 + 0xB6) = tmp;
            break;
        }
        fn_80105624();
        tmp = *(u16*)((u8*)r3 + 0x6);
        tmp = tmp & 0x00000008;
        if ((s32)tmp == 0) break;
        if (tmp == 0) {
            r3 = (u32)&lbl_80268D78;
            r5 = (u32)&lbl_80268DB4;
            r3 = (u32)&lbl_80268D78;
            r4 = 0x28e;
            r5 = (u32)&lbl_80268DB4;
            __assert();
        }
        r5 = *(u8*)((u8*)r18 + 0xB5);
        tmp = *(u8*)((u8*)r3 + 0x1B);
        r5 = (s8)r5;
        r4 = (s8)tmp;
        if ((s32)r5 != (s32)tmp) {
            tmp = r5 + 0x1;
            r4 = (s8)tmp;
            fn_80082A88();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
            }
            tmp = *(u8*)((u8*)r18 + 0xCA);
            if (tmp == 0) {
                r3 = 0x26;
                fn_80166A28();
            }
            tmp = 0x1;
            *(u8*)((u8*)r18 + 0xCA) = tmp;
            break;
            }
        r3 = 0x3c5;
        fn_80166A28();
        r3 = *(u8*)((u8*)r18 + 0xB5);
        tmp = 0x1;
        r3 = r3 + 0x1;
        *(u8*)((u8*)r18 + 0xB5) = r3;
        *(u8*)((u8*)r18 + 0xB6) = tmp;
    }
} while (0);
    tmp = *(u8*)((u8*)r18 + 0xB6);
    if (tmp != 0) {
        tmp = *(u32*)((u8*)r18 + 0xAC);
        if ((s32)tmp > 0) {
            tmp = *(u32*)((u8*)r18 + 0xA4);
            if ((s32)tmp < 0) {
            }
            r3 = 0x0;

            } else {
            r3 = *(u32*)((u8*)r18 + 0xB0);
            tmp = tmp << 2;
            r3 = *(u32*)(r3 + tmp);
            }
        tmp = 0x0;
        *(u32*)((u8*)r18 + 0xBC) = tmp;
        *(u32*)((u8*)r18 + 0xB8) = tmp;
        r4 = *(u8*)((u8*)r3 + 0x1B);
        tmp = *(u8*)((u8*)r18 + 0xB5);
        r3 = (s8)r4;
        tmp = (s8)tmp;
        if ((s32)r3 <= (s32)tmp) {
            tmp = (s8)tmp;
            *(u8*)((u8*)r18 + 0xB5) = tmp;
        }
        while (1) {
            r4 = *(u8*)((u8*)r18 + 0xB5);
            r3 = r14;
            fn_80082A88();
            tmp = r3 & 0xFF;
            if (tmp != 0) break;
            r3 = *(u8*)((u8*)r18 + 0xB5);
            *(u8*)((u8*)r18 + 0xB5) = tmp;

        }
        f26 = *(f32*)&lbl_8047C150;
        break;
    }
    f26 = *(f32*)&lbl_8047C148;
} while (0);
    tmp = (u32)sp + 0xa4;
    *(u32*)(sp + 0x1AC) = tmp;
    tmp = *(u32*)((u8*)r18 + 0xAC);
    if ((s32)tmp > 0) {
        tmp = *(u32*)((u8*)r18 + 0xA0);
        if ((s32)tmp < 0) {
        }
        tmp = 0x0;

        } else {
        r3 = *(u32*)((u8*)r18 + 0xB0);
        tmp = tmp << 2;
        tmp = *(u32*)(r3 + tmp);
        }
    *(u32*)(sp + 0xA4) = tmp;
    tmp = *(u32*)((u8*)r18 + 0xAC);
    if ((s32)tmp > 0) {
        tmp = *(u32*)((u8*)r18 + 0xA4);
        if ((s32)tmp < 0) {
        }
        tmp = 0x0;

        } else {
        r3 = *(u32*)((u8*)r18 + 0xB0);
        tmp = tmp << 2;
        tmp = *(u32*)(r3 + tmp);
        }
    *(u32*)(sp + 0xA8) = tmp;
    f0 = *(f32*)&lbl_8047C148;
    tmp = 0x0;
    f1 = *(f32*)&lbl_8047C154;
    r22 = r18;
    f27 = f0 - f26;
    f0 = *(f32*)&lbl_8047C158;
    f30 = f1 * f26;
    f31 = f0 * f26;
    *(u32*)(sp + 0x1A8) = tmp;
    f29 = f1 * f27;
    f28 = f0 * f27;
    do {
        r6 = *(u32*)((u8*)r23 + 0x0);
        tmp = -0x1;
        r4 = *(s16*)((u8*)r18 + 0xD2);
        r3 = -0x1;
        r15 = *(s16*)((u8*)r18 + 0xD4);
        *(u16*)(sp + 0x1A4) = r4;
        if (r6 != 0) {
            r4 = *(u8*)((u8*)r18 + 0xB6);
            if (r4 == 0) {
            do {
                if ((s32)r4 != 0) {
                }
                r5 = *(u8*)((u8*)r6 + 0x1D);
                r5 = (s8)r5;
                r8 = *(s16*)((u8*)r18 + 0xCE);
                r5 = 0x5 - r5;
                r7 = r15 * r5;
                r4 = *(u8*)((u8*)r6 + 0x1C);
                r6 = *(s16*)((u8*)r18 + 0xD0);
                r4 = (s8)r4;
                r5 = (u32)r7 >> 31;
                r5 = r5 + r7;
                r7 = (s32)r5 >> 1;
                r5 = 0x6 - r4;
                r4 = *(s16*)((u8*)(u32)sp + 0x1A4);
                r5 = r4 * r5;
                r4 = r8 + r7;
                r16 = (s16)r4;
                r4 = (u32)r5 >> 31;
                r4 = r4 + r5;
                r4 = (s32)r4 >> 1;
                r4 = r6 + r4;
                r17 = (s16)r4;
                if ((s32)r4 != 1) {
                    if ((s32)r4 >= 1) break;
                    if ((s32)r4 < 0) {
                        break;
                    }
                    r3 = *(u8*)((u8*)r18 + 0xB4);
                    r5 = *(u8*)((u8*)r18 + 0xB5);
                    r4 = (s8)r3;
                    r3 = (s8)r5;
                    if ((s32)r4 < (s32)r3) {
                        r3 = 0x43300000;
                        f1 = *(f64*)&lbl_8047C168;
                        f0 = f0 - f1;
                        f0 = f0 - f30;
                        f0 = (f64)(s32)f0;

                    } else {
                        if ((s32)r3 < (s32)r4) {
                            r3 = 0x43300000;
                            f1 = *(f64*)&lbl_8047C168;
                            f0 = f0 - f1;
                            f0 = f0 + f30;
                            f0 = (f64)(s32)f0;
                        }
                    }
                    r3 = *(u32*)((u8*)r18 + 0xA0);
                    r4 = *(u32*)((u8*)r18 + 0xA4);
                    if ((s32)r3 < (s32)r4) {
                        r3 = 0x43300000;
                        f1 = *(f64*)&lbl_8047C168;
                        f0 = f0 - f1;
                        f0 = f0 - f31;
                        f0 = (f64)(s32)f0;

                    } else {
                        if ((s32)r4 < (s32)r3) {
                            r3 = 0x43300000;
                            f1 = *(f64*)&lbl_8047C168;
                            f0 = f0 - f1;
                            f0 = f0 + f31;
                            f0 = (f64)(s32)f0;
                        }
                    }
                    r3 = -0x100;
                    break;
                }
                tmp = *(u8*)((u8*)r18 + 0xB4);
                r5 = *(u8*)((u8*)r18 + 0xB5);
                r4 = (s8)tmp;
                tmp = (s8)r5;
                if ((s32)r4 < (s32)tmp) {
                    tmp = 0x43300000;
                    f1 = *(f64*)&lbl_8047C168;
                    *(u32*)(sp + 0xB8) = tmp;
                    f0 = f0 - f1;
                    f0 = f0 + f29;
                    f0 = (f64)(s32)f0;

                } else {
                    if ((s32)tmp < (s32)r4) {
                        tmp = 0x43300000;
                        f1 = *(f64*)&lbl_8047C168;
                        *(u32*)(sp + 0xB8) = tmp;
                        f0 = f0 - f1;
                        f0 = f0 - f29;
                        f0 = (f64)(s32)f0;
                    }
                }
                tmp = *(u32*)((u8*)r18 + 0xA0);
                r4 = *(u32*)((u8*)r18 + 0xA4);
                if ((s32)tmp < (s32)r4) {
                    tmp = 0x43300000;
                    f1 = *(f64*)&lbl_8047C168;
                    *(u32*)(sp + 0xB8) = tmp;
                    f0 = f0 - f1;
                    f0 = f0 + f28;
                    f0 = (f64)(s32)f0;

                } else {
                    if ((s32)r4 < (s32)tmp) {
                        tmp = 0x43300000;
                        f1 = *(f64*)&lbl_8047C168;
                        *(u32*)(sp + 0xB8) = tmp;
                        f0 = f0 - f1;
                        f0 = f0 - f28;
                        f0 = (f64)(s32)f0;
                    }
                }
                tmp = -0x100;
            } while (0);
                *(u32*)(sp + 0x98) = tmp;
                r7 = 0x43300000;
                r8 = *(u8*)(sp + 0x98);
                r21 = r4 + 0xb4;
                r3 = *(u8*)(sp + 0x9C);
                r9 = *(u8*)(sp + 0x99);
                r3 = r3 - r8;
                tmp = *(u8*)(sp + 0x9D);
                tmp = tmp - r9;
                f4 = *(f64*)&lbl_8047C168;
                f3 = *(f64*)&lbl_8047C170;
                f2 = f0 - f4;
                r10 = *(u8*)(sp + 0x9A);
                r3 = *(u8*)(sp + 0x9E);
                f1 = f0 - f3;
                r3 = r3 - r10;
                r11 = *(u8*)(sp + 0x9B);
                f2 = f26 * f2 + f1;
                tmp = *(u8*)(sp + 0x9F);
                f1 = f0 - f4;
                f2 = (f64)(s32)f2;
                tmp = tmp - r11;
                f0 = f0 - f3;
                r3 = *(u32*)((u8*)r23 + 0x0);
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                r4 = *(u8*)(r18 + r21);
                f0 = f0 - f3;
                *(u32*)(sp + 0xFC) = tmp;
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                f0 = f0 - f3;
                *(u8*)(sp + 0xA0) = r6;
                f0 = f26 * f1 + f0;
                *(u8*)(sp + 0xA2) = tmp;
                f0 = (f64)(s32)f0;
                *(u8*)(sp + 0xA1) = r5;
                *(u8*)(sp + 0xA3) = tmp;
                fn_80082FE4();
                tmp = (s16)r17;
                r14 = 0x0;
                r24 = (s16)r16;
                r25 = r3;
                r17 = 0x0;
                *(u32*)(sp + 0x1B0) = tmp;
                while (1) {
                    r3 = *(u32*)((u8*)r23 + 0x0);
                    r4 = (s8)r17;
                    tmp = *(u8*)((u8*)r3 + 0x1C);
                    tmp = (s8)tmp;
                    if ((s32)r4 >= (s32)tmp) break;
                    r28 = r14;
                    r16 = 0x0;
                    r27 = 0x0;
                    r26 = r3 + tmp;
                    r30 = (s16)r26;
                    r31 = r14 << 2;
                    while (1) {
                    do {
                        tmp = (s8)r16;
                        if ((s32)tmp >= 6) break;
                        r20 = r22 + r31;
                        r19 = r24 + r27;
                        r5 = *(u32*)((u8*)r20 + 0x170);
                        tmp = (s16)r19;
                        r3 = *(u32*)((u8*)r23 + 0x0);
                        r4 = (s8)r16;
                        *(u16*)((u8*)r5 + 0x50) = tmp;
                        r5 = *(u32*)((u8*)r20 + 0x170);
                        *(u16*)((u8*)r5 + 0x52) = r30;
                        r5 = *(u8*)((u8*)r3 + 0x1D);
                        tmp = (s8)r5;
                        if ((s32)r4 == (s32)tmp) {
                            tmp = *(u8*)((u8*)r25 + 0x1C);
                            if (tmp == 0) {
                            }
                            r4 = (s8)r16;
                            tmp = (s8)r5;
                            if ((s32)r4 >= (s32)tmp) break;
                            r4 = *(u8*)(r18 + r21);
                            r5 = r17;
                            r6 = r16;
                            fn_80082EA4();
                            tmp = *(u8*)((u8*)r3 + 0xC);
                            if (tmp == 0) break;
                            }
                        tmp = *(u32*)((u8*)r18 + 0xBC);
                        do {
                            if ((s32)tmp == 0 || (s32)tmp == 6) break;

                            tmp = *(u32*)((u8*)r18 + 0xC0);
                            if ((s32)tmp != (s32)r28) break;
                            tmp = *(u8*)((u8*)r18 + 0xB6);
                            if (tmp != 0) break;
                        } while (0);

                        r3 = *(u32*)((u8*)r20 + 0x170);
                        r4 = 0x1;
                        fn_80109220();
                        r3 = *(u32*)((u8*)r20 + 0x3B0);
                        r4 = 0x1;
                        fn_80109220();
                        r3 = *(u32*)((u8*)r20 + 0x290);
                        r4 = 0x1;
                        fn_80109220();
                        r3 = *(u32*)((u8*)r20 + 0x170);
                        *(u32*)((u8*)r3 + 0x64) = r29;
                        tmp = *(s16*)((u8*)r18 + 0xD6);
                        r4 = *(s16*)((u8*)r18 + 0xCE);
                        tmp = tmp + r19;
                        r3 = *(u32*)((u8*)r20 + 0x3B0);
                        tmp = tmp - r4;
                        tmp = (s16)tmp;
                        *(u16*)((u8*)r3 + 0x50) = tmp;
                        tmp = *(s16*)((u8*)r18 + 0xD8);
                        r4 = *(s16*)((u8*)r18 + 0xD0);
                        tmp = tmp + r26;
                        r3 = *(u32*)((u8*)r20 + 0x3B0);
                        tmp = tmp - r4;
                        tmp = (s16)tmp;
                        *(u16*)((u8*)r3 + 0x52) = tmp;
                        r3 = *(u32*)((u8*)r20 + 0x3B0);
                        *(u32*)((u8*)r3 + 0x64) = r29;
                        tmp = *(s16*)((u8*)r18 + 0xDE);
                        r4 = *(s16*)((u8*)r18 + 0xCE);
                        tmp = tmp + r19;
                        r3 = *(u32*)((u8*)r20 + 0x290);
                        tmp = tmp - r4;
                        tmp = (s16)tmp;
                        *(u16*)((u8*)r3 + 0x50) = tmp;
                        tmp = *(s16*)((u8*)r18 + 0xE0);
                        r4 = *(s16*)((u8*)r18 + 0xD0);
                        tmp = tmp + r26;
                        r3 = *(u32*)((u8*)r20 + 0x290);
                        tmp = tmp - r4;
                        tmp = (s16)tmp;
                        *(u16*)((u8*)r3 + 0x52) = tmp;
                        r3 = *(u32*)((u8*)r20 + 0x290);
                        *(u32*)((u8*)r3 + 0x64) = r29;
                    } while (0);
                        r28 = r28 + 0x1;
                        r31 = r31 + 0x4;
                        r27 = r27 + r15;
                        r16 = r16 + 0x1;

                    }
                    r14 = r14 + 0x6;
                    tmp = *(s16*)((u8*)(u32)sp + 0x1A4);
                    r25 = r25 + 0xe;
                    r17 = r17 + 0x1;
                    r3 = r3 + tmp;

                }
            }
                }
        r23 = r23 + 0x4;
        r22 = r22 + 0x90;
        r3 = r3 + 0x1;
        tmp = r3;
    } while ((s32)tmp < 2);
    r19 = r18;
    r20 = 0x0;
    do {
        tmp = *(u8*)((u8*)r18 + 0xB6);
        if (tmp == 0) {
            if ((s32)r20 != 0) {
            }
            tmp = *(u32*)((u8*)r17 + 0x0);
            if (tmp != 0) {
                r15 = r19;
                r16 = r20 + 0xb4;
                r21 = 0x0;
                f31 = *(f64*)&lbl_8047C168;
                r14 = 0x43300000;
                f30 = *(f64*)&lbl_8047C170;
                while (1) {
                do {
                    r3 = *(u32*)((u8*)r17 + 0x0);
                    tmp = *(u8*)((u8*)r3 + 0x1B);
                    tmp = (s8)tmp;
                    if ((s32)r21 >= (s32)tmp) break;
                    tmp = *(u8*)(r18 + r16);
                    tmp = (s8)tmp;
                    if ((s32)r21 == (s32)tmp) {
                        r22 = -0x1;
                    } else {

                        r3 = 0x2500000;
                    }
                    if ((s32)r20 != 1) {
                        if ((s32)r20 >= 1) break;
                        if ((s32)r20 < 0) {
                            break;
                        }
                        /* clrrwi tmp, r22, 8 */;
                        *(u32*)(sp + 0x1A0) = tmp;
                        break;
                    }
                    /* clrrwi r22, r22, 8 */;
                } while (0);
                    r3 = *(u32*)((u8*)r15 + 0x130);
                    r4 = 0x1;
                    fn_80109220();
                    r21 = r21 + 0x1;
                    r3 = *(u32*)((u8*)r15 + 0x130);
                    r15 = r15 + 0x4;
                    *(u32*)(sp + 0x90) = tmp;
                    r7 = *(u8*)(sp + 0x8C);
                    r4 = *(u8*)(sp + 0x90);
                    r8 = *(u8*)(sp + 0x8D);
                    r4 = r4 - r7;
                    tmp = *(u8*)(sp + 0x91);
                    tmp = tmp - r8;
                    r9 = *(u8*)(sp + 0x8E);
                    r4 = *(u8*)(sp + 0x92);
                    r4 = r4 - r9;
                    f2 = f0 - f31;
                    r10 = *(u8*)(sp + 0x8F);
                    f1 = f0 - f30;
                    tmp = *(u8*)(sp + 0x93);
                    tmp = tmp - r10;
                    f2 = f26 * f2 + f1;
                    f1 = f0 - f31;
                    f2 = (f64)(s32)f2;
                    f0 = f0 - f30;
                    f1 = f26 * f1 + f0;
                    f2 = (f64)(s32)f1;
                    f1 = f0 - f31;
                    f0 = f0 - f30;
                    *(u32*)(sp + 0xC4) = tmp;
                    f1 = f26 * f1 + f0;
                    f2 = (f64)(s32)f1;
                    f1 = f0 - f31;
                    f0 = f0 - f30;
                    *(u8*)(sp + 0x94) = r5;
                    f0 = f26 * f1 + f0;
                    *(u8*)(sp + 0x96) = tmp;
                    f0 = (f64)(s32)f0;
                    *(u8*)(sp + 0x95) = r4;
                    *(u8*)(sp + 0x97) = tmp;
                    *(u32*)((u8*)r3 + 0x64) = tmp;

                }
            }
            }
        r17 = r17 + 0x4;
        r19 = r19 + 0xc;
        r20 = r20 + 0x1;
    } while ((s32)r20 < 2);
    tmp = *(u8*)((u8*)r18 + 0xB6);
    if (tmp != 0) {
        r5 = *(u8*)((u8*)r3 + 0x1B);
        if (r4 != 0) {
            r4 = *(u8*)((u8*)r4 + 0x1B);
            r3 = (s8)r5;
            tmp = (s8)r4;
            if ((s32)r3 < (s32)tmp) {
                r5 = r4;
        }
        }
        r16 = r18;
        r19 = 0x0;
        r20 = (s8)r5;
        r14 = 0x43300000;
        f30 = *(f64*)&lbl_8047C168;
        f31 = *(f64*)&lbl_8047C170;
        while (1) {
            tmp = (s8)r19;
            if ((s32)tmp >= (s32)r20) break;
            r3 = *(u32*)((u8*)r16 + 0x124);
            r21 = -0x1;
            r22 = -0x1;
            r4 = 0x1;
            fn_80109220();
            if (r17 != 0) {
                r3 = *(u8*)((u8*)r17 + 0x1B);
                tmp = (s8)r19;
                r3 = (s8)r3;
                if ((s32)r3 <= (s32)tmp) {
                }
                r21 = -0x100;
                }
            r3 = *(u8*)((u8*)r15 + 0x1B);
            tmp = (s8)r19;
            r3 = (s8)r3;
            if ((s32)r3 <= (s32)tmp) {
                r22 = -0x100;
            }
            r19 = r19 + 0x1;
            r3 = *(u32*)((u8*)r16 + 0x124);
            r16 = r16 + 0x4;
            r7 = *(u8*)(sp + 0x80);
            r4 = *(u8*)(sp + 0x84);
            r8 = *(u8*)(sp + 0x81);
            tmp = *(u8*)(sp + 0x85);
            r4 = r4 - r7;
            r9 = *(u8*)(sp + 0x82);
            r4 = *(u8*)(sp + 0x86);
            tmp = tmp - r8;
            r10 = *(u8*)(sp + 0x83);
            tmp = *(u8*)(sp + 0x87);
            r4 = r4 - r9;
            tmp = tmp - r10;
            f2 = f0 - f30;
            f1 = f0 - f31;
            f2 = f26 * f2 + f1;
            f1 = f0 - f30;
            f2 = (f64)(s32)f2;
            f0 = f0 - f31;
            f1 = f26 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f30;
            f0 = f0 - f31;
            *(u32*)(sp + 0xC4) = tmp;
            f1 = f26 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f30;
            f0 = f0 - f31;
            *(u8*)(sp + 0x88) = r5;
            f0 = f26 * f1 + f0;
            *(u8*)(sp + 0x89) = r4;
            f0 = (f64)(s32)f0;
            *(u8*)(sp + 0x8A) = tmp;
            *(u8*)(sp + 0x8B) = tmp;
            *(u32*)((u8*)r3 + 0x64) = tmp;

        }

    } else {
        r16 = r18;
        r17 = 0x0;
        r15 = -0x1;
        while (1) {
            tmp = *(u8*)((u8*)r14 + 0x1B);
            r3 = (s8)r17;
            tmp = (s8)tmp;
            if ((s32)r3 >= (s32)tmp) break;
            r3 = *(u32*)((u8*)r16 + 0x124);
            r4 = 0x1;
            fn_80109220();
            r3 = *(u32*)((u8*)r16 + 0x124);
            r16 = r16 + 0x4;
            r17 = r17 + 0x1;
            *(u32*)((u8*)r3 + 0x64) = r15;

        }
    }
    r3 = *(u32*)((u8*)r18 + 0x118);
    r4 = 0x1;
    fn_80109220();
    if (tmp != 0) {
        r4 = *(u8*)((u8*)r18 + 0xB4);
        tmp = 0x43300000;
        r3 = *(u8*)((u8*)r18 + 0xB5);
        r4 = (s8)r4;
        *(u32*)(sp + 0x108) = tmp;
        r3 = (s8)r3;
        f2 = *(f64*)&lbl_8047C168;
        r4 = r4 << 3;
        *(u32*)(sp + 0x100) = tmp;
        tmp = r3 << 3;
        r3 = *(u32*)((u8*)r18 + 0x118);
        r5 = r18 + r4;
        r4 = r18 + tmp;
        r5 = *(s16*)((u8*)r5 + 0xE6);
        r4 = *(s16*)((u8*)r4 + 0xE6);
        r4 = r4 - r5;
        *(u32*)(sp + 0x104) = tmp;
        *(u32*)(sp + 0x10C) = tmp;
        f0 = f0 - f2;
        f1 = f1 - f2;
        f0 = f26 * f1 + f0;
        f0 = (f64)(s32)f0;
        *(u16*)((u8*)r3 + 0x50) = tmp;
    } else {

        tmp = *(u8*)((u8*)r18 + 0xB5);
        r3 = *(u32*)((u8*)r18 + 0x118);
        tmp = (s8)tmp;
        tmp = tmp << 3;
        r4 = r18 + tmp;
        tmp = *(s16*)((u8*)r4 + 0xE6);
        *(u16*)((u8*)r3 + 0x50) = tmp;
    }
    tmp = *(u8*)((u8*)r18 + 0xB6);
    do {
    if (tmp != 0) {
        tmp = *(u32*)((u8*)r18 + 0xA4);
        r3 = *(u32*)((u8*)r18 + 0xA0);
        if ((s32)tmp != (s32)r3) {
            if ((s32)r3 < (s32)tmp) {
                r14 = 0x1;
            } else {

                r14 = -0x1;
            }
            r3 = *(u32*)((u8*)r18 + 0x158);
            r4 = 0x1;
            fn_80109220();
            tmp = -0x1;
            r3 = -0x100;
            *(u32*)(sp + 0x74) = tmp;
            r7 = 0x43300000;
            f0 = *(f32*)&lbl_8047C15C;
            r9 = *(u8*)(sp + 0x74);
            f2 = f0 * f26;
            r4 = *(u8*)(sp + 0x78);
            r10 = *(u8*)(sp + 0x75);
            r3 = *(u8*)(sp + 0x79);
            r4 = r4 - r9;
            r11 = *(u8*)(sp + 0x76);
            r4 = *(u8*)(sp + 0x7A);
            r3 = r3 - r10;
            r12 = *(u8*)(sp + 0x77);
            r3 = *(u8*)(sp + 0x7B);
            r4 = r4 - r11;
            r3 = r3 - r12;
            f5 = *(f64*)&lbl_8047C168;
            f4 = *(f64*)&lbl_8047C170;
            f3 = f0 - f5;
            r3 = *(u32*)((u8*)r18 + 0x158);
            f1 = f0 - f4;
            f3 = f26 * f3 + f1;
            f1 = f0 - f5;
            f3 = (f64)(s32)f3;
            f0 = f0 - f4;
            f1 = f26 * f1 + f0;
            f3 = (f64)(s32)f1;
            f1 = f0 - f5;
            f0 = f0 - f4;
            f1 = f26 * f1 + f0;
            f3 = (f64)(s32)f1;
            f1 = f0 - f5;
            f0 = f0 - f4;
            *(u8*)(sp + 0x7C) = r6;
            f0 = f26 * f1 + f0;
            *(u8*)(sp + 0x7D) = r5;
            f0 = (f64)(s32)f0;
            *(u8*)(sp + 0x7E) = r4;
            *(u32*)(sp + 0x114) = tmp;
            *(u8*)(sp + 0x7F) = tmp;
            f1 = f0 - f5;
            *(u32*)((u8*)r3 + 0x64) = tmp;
            tmp = *(s16*)((u8*)r18 + 0x100);
            r3 = *(u32*)((u8*)r18 + 0x158);
            *(u32*)(sp + 0x11C) = tmp;
            f0 = f0 - f5;
            f0 = -(f2 * f1 - f0);
            f0 = (f64)(s32)f0;
            *(u16*)((u8*)r3 + 0x52) = tmp;
            r3 = *(u32*)((u8*)r18 + 0x15C);
            r4 = 0x1;
            fn_80109220();
            tmp = -0x100;
            r3 = -0x1;
            *(u32*)(sp + 0x68) = tmp;
            r7 = 0x43300000;
            f0 = *(f32*)&lbl_8047C15C;
            r9 = *(u8*)(sp + 0x68);
            f2 = f0 * f27;
            r4 = *(u8*)(sp + 0x6C);
            r10 = *(u8*)(sp + 0x69);
            r3 = *(u8*)(sp + 0x6D);
            r4 = r4 - r9;
            r11 = *(u8*)(sp + 0x6A);
            r4 = *(u8*)(sp + 0x6E);
            r3 = r3 - r10;
            r12 = *(u8*)(sp + 0x6B);
            r3 = *(u8*)(sp + 0x6F);
            r4 = r4 - r11;
            r3 = r3 - r12;
            f5 = *(f64*)&lbl_8047C168;
            f4 = *(f64*)&lbl_8047C170;
            f3 = f0 - f5;
            r3 = *(u32*)((u8*)r18 + 0x15C);
            f1 = f0 - f4;
            f3 = f26 * f3 + f1;
            f1 = f0 - f5;
            f3 = (f64)(s32)f3;
            f0 = f0 - f4;
            f1 = f26 * f1 + f0;
            f3 = (f64)(s32)f1;
            f1 = f0 - f5;
            f0 = f0 - f4;
            f1 = f26 * f1 + f0;
            f3 = (f64)(s32)f1;
            f1 = f0 - f5;
            f0 = f0 - f4;
            *(u8*)(sp + 0x70) = r6;
            f0 = f26 * f1 + f0;
            *(u8*)(sp + 0x71) = r5;
            f0 = (f64)(s32)f0;
            *(u8*)(sp + 0x72) = r4;
            *(u32*)(sp + 0x18C) = tmp;
            *(u8*)(sp + 0x73) = tmp;
            f1 = f0 - f5;
            *(u32*)((u8*)r3 + 0x64) = tmp;
            tmp = *(s16*)((u8*)r18 + 0x100);
            r3 = *(u32*)((u8*)r18 + 0x15C);
            *(u32*)(sp + 0x194) = tmp;
            f0 = f0 - f5;
            f0 = f2 * f1 + f0;
            f0 = (f64)(s32)f0;
            *(u16*)((u8*)r3 + 0x52) = tmp;
            break;
        }
        }
    r3 = *(u32*)((u8*)r18 + 0x15C);
    r4 = 0x1;
    fn_80109220();
    r3 = *(u32*)((u8*)r18 + 0x15C);
    tmp = -0x1;
    *(u32*)((u8*)r3 + 0x64) = tmp;
    tmp = *(s16*)((u8*)r18 + 0x100);
    r3 = *(u32*)((u8*)r18 + 0x15C);
    *(u16*)((u8*)r3 + 0x52) = tmp;
    } while (0);
do {
    r14 = r18;
    r17 = 0x0;
    r15 = (u32)&lbl_8047C130;
    r16 = (u32)&lbl_8047C138;
    do {
        r3 = *(u32*)((u8*)r3 + 0x0);
        if (r3 != 0) {
            tmp = r17 + 0xb4;
            r4 = *(u8*)(r18 + tmp);
            fn_80082FE4();
            tmp = *(u8*)((u8*)r3 + 0x71);
            if (tmp != 0) {
                r3 = *(u32*)((u8*)r14 + 0x150);
                r4 = 0x1;
                fn_80109220();
                r3 = *(u32*)((u8*)r15 + 0x0);
                r7 = 0x43300000;
                tmp = *(u32*)((u8*)r16 + 0x0);
                r4 = 0x1;
                f4 = *(f64*)&lbl_8047C168;
                *(u32*)(sp + 0x60) = tmp;
                r9 = *(u8*)(sp + 0x5C);
                r3 = *(u8*)(sp + 0x60);
                r10 = *(u8*)(sp + 0x5D);
                tmp = *(u8*)(sp + 0x61);
                r3 = r3 - r9;
                r11 = *(u8*)(sp + 0x5E);
                r3 = *(u8*)(sp + 0x62);
                tmp = tmp - r10;
                r12 = *(u8*)(sp + 0x5F);
                tmp = *(u8*)(sp + 0x63);
                r3 = r3 - r11;
                tmp = tmp - r12;
                f3 = *(f64*)&lbl_8047C170;
                r3 = *(u32*)((u8*)r14 + 0x150);
                f2 = f0 - f4;
                f1 = f0 - f3;
                f2 = f26 * f2 + f1;
                f1 = f0 - f4;
                f2 = (f64)(s32)f2;
                f0 = f0 - f3;
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                f0 = f0 - f3;
                *(u32*)(sp + 0x154) = tmp;
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                f0 = f0 - f3;
                *(u8*)(sp + 0x64) = r6;
                f0 = f26 * f1 + f0;
                *(u8*)(sp + 0x65) = r5;
                f0 = (f64)(s32)f0;
                *(u8*)(sp + 0x66) = tmp;
                *(u8*)(sp + 0x67) = tmp;
                *(u32*)((u8*)r3 + 0x64) = tmp;
                r3 = *(u32*)((u8*)r14 + 0x148);
                fn_80109220();
                r3 = *(u32*)((u8*)r15 + 0x0);
                r6 = 0x43300000;
                tmp = *(u32*)((u8*)r16 + 0x0);
                f4 = *(f64*)&lbl_8047C168;
                *(u32*)(sp + 0x54) = tmp;
                r8 = *(u8*)(sp + 0x50);
                r3 = *(u8*)(sp + 0x54);
                r9 = *(u8*)(sp + 0x51);
                tmp = *(u8*)(sp + 0x55);
                r3 = r3 - r8;
                r10 = *(u8*)(sp + 0x52);
                r3 = *(u8*)(sp + 0x56);
                tmp = tmp - r9;
                r11 = *(u8*)(sp + 0x53);
                tmp = *(u8*)(sp + 0x57);
                r3 = r3 - r10;
                tmp = tmp - r11;
                f3 = *(f64*)&lbl_8047C170;
                r3 = *(u32*)((u8*)r14 + 0x148);
                f2 = f0 - f4;
                f1 = f0 - f3;
                f2 = f26 * f2 + f1;
                f1 = f0 - f4;
                f2 = (f64)(s32)f2;
                f0 = f0 - f3;
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                f0 = f0 - f3;
                *(u32*)(sp + 0xF4) = tmp;
                f1 = f26 * f1 + f0;
                f2 = (f64)(s32)f1;
                f1 = f0 - f4;
                f0 = f0 - f3;
                *(u8*)(sp + 0x58) = r5;
                f0 = f26 * f1 + f0;
                *(u8*)(sp + 0x59) = r4;
                f0 = (f64)(s32)f0;
                *(u8*)(sp + 0x5A) = tmp;
                *(u8*)(sp + 0x5B) = tmp;
                *(u32*)((u8*)r3 + 0x64) = tmp;
        }
        }
        r14 = r14 + 0x4;
        r15 = r15 + 0x4;
        r16 = r16 + 0x4;
        r3 = r3 + 0x4;
        r17 = r17 + 0x1;
    } while ((s32)r17 < 2);
    tmp = *(u32*)((u8*)r18 + 0xA4);
    if ((s32)tmp > 0) {
        r3 = *(u32*)((u8*)r18 + 0x160);
        r4 = 0x1;
        fn_80109220();
        r3 = *(u32*)((u8*)r18 + 0x164);
        r4 = 0x1;
        fn_80109220();
    }
    r4 = *(u32*)((u8*)r18 + 0xA4);
    if ((s32)r4 >= 0) {
        r3 = *(u32*)((u8*)r18 + 0xAC);
        if ((s32)r4 < (s32)tmp) {
            r3 = *(u32*)((u8*)r18 + 0x168);
            r4 = 0x1;
            fn_80109220();
            r3 = *(u32*)((u8*)r18 + 0x16C);
            r4 = 0x1;
            fn_80109220();
    }
    }
    tmp = *(u8*)((u8*)r18 + 0xB6);
    if (tmp == 0) {
        r3 = *(u32*)((u8*)r18 + 0xC4);
        r15 = -0x1;
        r14 = *(u32*)((u8*)r18 + 0xC0);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r18 + 0xC4) = tmp;
        tmp = *(u32*)((u8*)r18 + 0xBC);
        if (tmp > 9) break;
        switch (tmp) {
        case 0:
            break;
        case 1:
        r3 = -0x100;
        tmp = -0x1;
        r7 = 0x43300000;
        r3 = *(u32*)((u8*)r18 + 0xC4);
        *(u32*)(sp + 0x48) = tmp;
        r6 = *(u8*)(sp + 0x44);
        r3 = *(u8*)(sp + 0x48);
        r9 = *(u8*)(sp + 0x45);
        tmp = *(u8*)(sp + 0x49);
        r3 = r3 - r6;
        r10 = *(u8*)(sp + 0x46);
        r3 = *(u8*)(sp + 0x4A);
        tmp = tmp - r9;
        r11 = *(u8*)(sp + 0x47);
        tmp = *(u8*)(sp + 0x4B);
        r3 = r3 - r10;
        tmp = tmp - r11;
        f4 = *(f64*)&lbl_8047C168;
        f1 = *(f32*)&lbl_8047C14C;
        f2 = f0 - f4;
        f3 = *(f64*)&lbl_8047C170;
        f5 = f2 * f1;
        f2 = f0 - f4;
        f1 = f0 - f3;
        f2 = f5 * f2 + f1;
        f1 = f0 - f4;
        f2 = (f64)(s32)f2;
        f0 = f0 - f3;
        f1 = f5 * f1 + f0;
        f2 = (f64)(s32)f1;
        f1 = f0 - f4;
        f0 = f0 - f3;
        *(u32*)(sp + 0x14C) = tmp;
        f1 = f5 * f1 + f0;
        f2 = (f64)(s32)f1;
        f1 = f0 - f4;
        f0 = f0 - f3;
        *(u8*)(sp + 0x4C) = r4;
        f0 = f5 * f1 + f0;
        *(u8*)(sp + 0x4D) = r3;
        f0 = (f64)(s32)f0;
        *(u8*)(sp + 0x4E) = tmp;
        *(u8*)(sp + 0x4F) = tmp;
        r14 = r14 << 2;
        r5 = r18 + r14;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r5 + 0x200);
        *(u32*)((u8*)r3 + 0x64) = tmp;
        r3 = *(u32*)((u8*)r5 + 0x440);
        fn_80109220();
        r3 = r18 + r14;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r3 + 0x320);
        fn_80109220();
        tmp = *(u32*)((u8*)r18 + 0xC4);
        if ((s32)tmp != 0x10) break;
        r3 = 0x0;
        tmp = 0x2;
        *(u32*)((u8*)r18 + 0xC4) = r3;
        *(u32*)((u8*)r18 + 0xBC) = tmp;
        break;
        case 2:
        r15 = 0x1;
        case 3:
        r16 = r14 << 2;
        r4 = 0x0;
        r3 = r18 + r16;
        r3 = *(u32*)((u8*)r3 + 0x440);
        fn_80109220();
        r3 = r18 + r16;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r3 + 0x320);
        fn_80109220();
        goto carde_matrix_detail_case;
        case 4:
        r15 = 0x1;
        case 5:
carde_matrix_detail_case:
        r3 = *(u32*)((u8*)r18 + 0x4D0);
        r4 = 0x1;
        fn_80109220();
        tmp = r14 << 2;
        r3 = *(u32*)((u8*)r18 + 0x4D0);
        r5 = r18 + tmp;
        r4 = *(u32*)((u8*)r5 + 0x200);
        tmp = *(s16*)((u8*)r4 + 0x50);
        *(u16*)((u8*)r3 + 0x50) = tmp;
        r4 = *(u32*)((u8*)r5 + 0x200);
        r3 = *(u32*)((u8*)r18 + 0x4D0);
        tmp = *(s16*)((u8*)r4 + 0x52);
        *(u16*)((u8*)r3 + 0x52) = tmp;
        if ((s32)r15 == 1) {
            r3 = -0x100;
            tmp = -0x89;
            r7 = 0x43300000;
            r3 = *(u32*)((u8*)r18 + 0xC4);
            *(u32*)(sp + 0x3C) = tmp;
            r6 = *(u8*)(sp + 0x38);
            r3 = *(u8*)(sp + 0x3C);
            r9 = *(u8*)(sp + 0x39);
            tmp = *(u8*)(sp + 0x3D);
            r3 = r3 - r6;
            r10 = *(u8*)(sp + 0x3A);
            r3 = *(u8*)(sp + 0x3E);
            tmp = tmp - r9;
            r11 = *(u8*)(sp + 0x3B);
            tmp = *(u8*)(sp + 0x3F);
            r3 = r3 - r10;
            tmp = tmp - r11;
            f4 = *(f64*)&lbl_8047C168;
            f1 = *(f32*)&lbl_8047C14C;
            f2 = f0 - f4;
            f3 = *(f64*)&lbl_8047C170;
            f5 = f2 * f1;
            f2 = f0 - f4;
            f1 = f0 - f3;
            f2 = f5 * f2 + f1;
            f1 = f0 - f4;
            f2 = (f64)(s32)f2;
            f0 = f0 - f3;
            f1 = f5 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f4;
            f0 = f0 - f3;
            *(u32*)(sp + 0x14C) = tmp;
            f1 = f5 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f4;
            f0 = f0 - f3;
            *(u8*)(sp + 0x40) = r4;
            f0 = f5 * f1 + f0;
            *(u8*)(sp + 0x41) = r3;
            f0 = (f64)(s32)f0;
            *(u8*)(sp + 0x42) = tmp;
            *(u8*)(sp + 0x43) = tmp;
            r3 = *(u32*)((u8*)r18 + 0x4D0);
            *(u32*)((u8*)r3 + 0x64) = tmp;
        } else {

            r3 = -0x89;
            tmp = -0x100;
            r7 = 0x43300000;
            r3 = *(u32*)((u8*)r18 + 0xC4);
            *(u32*)(sp + 0x30) = tmp;
            r6 = *(u8*)(sp + 0x2C);
            r3 = *(u8*)(sp + 0x30);
            r9 = *(u8*)(sp + 0x2D);
            tmp = *(u8*)(sp + 0x31);
            r3 = r3 - r6;
            r10 = *(u8*)(sp + 0x2E);
            r3 = *(u8*)(sp + 0x32);
            tmp = tmp - r9;
            r11 = *(u8*)(sp + 0x2F);
            tmp = *(u8*)(sp + 0x33);
            r3 = r3 - r10;
            tmp = tmp - r11;
            f4 = *(f64*)&lbl_8047C168;
            f1 = *(f32*)&lbl_8047C14C;
            f2 = f0 - f4;
            f3 = *(f64*)&lbl_8047C170;
            f5 = f2 * f1;
            f2 = f0 - f4;
            f1 = f0 - f3;
            f2 = f5 * f2 + f1;
            f1 = f0 - f4;
            f2 = (f64)(s32)f2;
            f0 = f0 - f3;
            f1 = f5 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f4;
            f0 = f0 - f3;
            *(u32*)(sp + 0x14C) = tmp;
            f1 = f5 * f1 + f0;
            f2 = (f64)(s32)f1;
            f1 = f0 - f4;
            f0 = f0 - f3;
            *(u8*)(sp + 0x34) = r4;
            f0 = f5 * f1 + f0;
            *(u8*)(sp + 0x35) = r3;
            f0 = (f64)(s32)f0;
            *(u8*)(sp + 0x36) = tmp;
            *(u8*)(sp + 0x37) = tmp;
            r3 = *(u32*)((u8*)r18 + 0x4D0);
            *(u32*)((u8*)r3 + 0x64) = tmp;
        }
        tmp = *(u32*)((u8*)r18 + 0xC4);
        if ((s32)tmp != 0x10) break;
        tmp = 0x0;
        *(u32*)((u8*)r18 + 0xC4) = tmp;
        tmp = *(u32*)((u8*)r18 + 0xBC);
        tmp = tmp + r15;
        *(u32*)((u8*)r18 + 0xBC) = tmp;
        break;
        case 6:
        r3 = 0x2AAB0000;
        r17 = 0x0;
        r3 = (s32)((s64)tmp * (s64)r14 >> 32);
        tmp = (u32)r3 >> 31;
        tmp = r3 + tmp;
        r16 = (s8)tmp;
        tmp = r16 * 0x6;
        r19 = tmp << 2;
        while (1) {
            tmp = *(u8*)((u8*)r14 + 0x1D);
            r3 = (s8)r17;
            tmp = (s8)tmp;
            if ((s32)r3 >= (s32)tmp) break;
            r4 = *(u8*)((u8*)r18 + 0xB5);
            r3 = r14;
            r5 = r16;
            r6 = r17;
            fn_80082EA4();
            tmp = *(u8*)((u8*)r3 + 0xC);
            if (tmp == 0) {
                r15 = r19 + 0x200;
                r4 = 0x1;
                r3 = *(u32*)(r18 + r15);
                fn_80109220();
                r3 = *(u32*)((u8*)r18 + 0xC4);
                tmp = 0x43300000;
                *(u32*)(sp + 0x198) = tmp;
                f0 = *(f64*)&lbl_8047C168;
                f2 = *(f32*)&lbl_8047C160;
                f1 = *(f32*)&lbl_8047C148;
                f3 = f3 - f0;
                f5 = f3 * f2;
                if (f5 < f1) {
                    r4 = -0x100;
                    r3 = -0x89;
                    f4 = *(f64*)&lbl_8047C170;
                    r8 = *(u8*)(sp + 0x20);
                    r4 = *(u8*)(sp + 0x24);
                    r9 = *(u8*)(sp + 0x21);
                    r3 = *(u8*)(sp + 0x25);
                    r4 = r4 - r8;
                    r10 = *(u8*)(sp + 0x22);
                    r4 = *(u8*)(sp + 0x26);
                    r3 = r3 - r9;
                    r11 = *(u8*)(sp + 0x23);
                    r3 = *(u8*)(sp + 0x27);
                    r4 = r4 - r10;
                    r4 = r3 - r11;
                    r3 = *(u32*)(r18 + r15);
                    *(u32*)(sp + 0x198) = tmp;
                    f3 = f1 - f0;
                    *(u32*)(sp + 0x190) = tmp;
                    f2 = f1 - f4;
                    *(u32*)(sp + 0x180) = tmp;
                    f3 = f5 * f3 + f2;
                    f2 = f1 - f0;
                    *(u32*)(sp + 0x178) = tmp;
                    f3 = (f64)(s32)f3;
                    f1 = f1 - f4;
                    f2 = f5 * f2 + f1;
                    *(u32*)(sp + 0x168) = tmp;
                    f3 = (f64)(s32)f2;
                    f2 = f1 - f0;
                    *(u32*)(sp + 0x160) = tmp;
                    f1 = f1 - f4;
                    f2 = f5 * f2 + f1;
                    *(u32*)(sp + 0x150) = tmp;
                    f2 = (f64)(s32)f2;
                    f1 = f1 - f0;
                    *(u32*)(sp + 0x148) = tmp;
                    f0 = f0 - f4;
                    *(u8*)(sp + 0x28) = r6;
                    f0 = f5 * f1 + f0;
                    *(u8*)(sp + 0x29) = r5;
                    f0 = (f64)(s32)f0;
                    *(u8*)(sp + 0x2A) = tmp;
                    *(u8*)(sp + 0x2B) = tmp;
                    *(u32*)((u8*)r3 + 0x64) = tmp;
                } else {
                r4 = -0x89;
                r3 = -0x100;
                f5 = f5 - f1;
                f4 = *(f64*)&lbl_8047C170;
                r7 = *(u8*)(sp + 0x14);
                r4 = *(u8*)(sp + 0x18);
                r8 = *(u8*)(sp + 0x15);
                r3 = *(u8*)(sp + 0x19);
                r4 = r4 - r7;
                r9 = *(u8*)(sp + 0x16);
                r4 = *(u8*)(sp + 0x1A);
                r3 = r3 - r8;
                r10 = *(u8*)(sp + 0x17);
                r3 = *(u8*)(sp + 0x1B);
                r4 = r4 - r9;
                r3 = r3 - r10;
                *(u32*)(sp + 0x198) = tmp;
                f3 = f1 - f0;
                *(u32*)(sp + 0x190) = tmp;
                f2 = f1 - f4;
                *(u32*)(sp + 0x180) = tmp;
                f3 = f5 * f3 + f2;
                f2 = f1 - f0;
                *(u32*)(sp + 0x178) = tmp;
                f3 = (f64)(s32)f3;
                f1 = f1 - f4;
                f2 = f5 * f2 + f1;
                *(u32*)(sp + 0x168) = tmp;
                f3 = (f64)(s32)f2;
                f2 = f1 - f0;
                *(u32*)(sp + 0x160) = tmp;
                f1 = f1 - f4;
                f2 = f5 * f2 + f1;
                *(u32*)(sp + 0x150) = tmp;
                f2 = (f64)(s32)f2;
                f1 = f1 - f0;
                *(u32*)(sp + 0x148) = tmp;
                f0 = f0 - f4;
                *(u8*)(sp + 0x1C) = r5;
                f0 = f5 * f1 + f0;
                *(u8*)(sp + 0x1D) = r4;
                f0 = (f64)(s32)f0;
                *(u8*)(sp + 0x1E) = tmp;
                *(u8*)(sp + 0x1F) = tmp;
                r3 = *(u32*)(r18 + r15);
                *(u32*)((u8*)r3 + 0x64) = tmp;
                }
            }
            r19 = r19 + 0x4;
            r17 = r17 + 0x1;

        }
        tmp = *(u32*)((u8*)r18 + 0xC4);
        if ((s32)tmp != 0x40) break;
        tmp = 0x0;
        *(u32*)((u8*)r18 + 0xC4) = tmp;
        break;
        case 7:
        r15 = r14 << 2;
        r4 = 0x0;
        r3 = r18 + r15;
        r3 = *(u32*)((u8*)r3 + 0x440);
        fn_80109220();
        r3 = r18 + r15;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r3 + 0x320);
        fn_80109220();
        case 8:
        r3 = -0x1;
        tmp = -0x100;
        r7 = 0x43300000;
        r3 = *(u32*)((u8*)r18 + 0xC4);
        *(u32*)(sp + 0xC) = tmp;
        r6 = *(u8*)(sp + 0x8);
        r3 = *(u8*)(sp + 0xC);
        r9 = *(u8*)(sp + 0x9);
        tmp = *(u8*)(sp + 0xD);
        r3 = r3 - r6;
        r10 = *(u8*)(sp + 0xA);
        r3 = *(u8*)(sp + 0xE);
        tmp = tmp - r9;
        r11 = *(u8*)(sp + 0xB);
        tmp = *(u8*)(sp + 0xF);
        r3 = r3 - r10;
        tmp = tmp - r11;
        f4 = *(f64*)&lbl_8047C168;
        f1 = *(f32*)&lbl_8047C14C;
        f2 = f0 - f4;
        f3 = *(f64*)&lbl_8047C170;
        f5 = f2 * f1;
        f2 = f0 - f4;
        f1 = f0 - f3;
        f2 = f5 * f2 + f1;
        f1 = f0 - f4;
        f2 = (f64)(s32)f2;
        f0 = f0 - f3;
        f1 = f5 * f1 + f0;
        f2 = (f64)(s32)f1;
        f1 = f0 - f4;
        f0 = f0 - f3;
        *(u32*)(sp + 0x14C) = tmp;
        f1 = f5 * f1 + f0;
        f2 = (f64)(s32)f1;
        f1 = f0 - f4;
        f0 = f0 - f3;
        *(u8*)(sp + 0x10) = r4;
        f0 = f5 * f1 + f0;
        *(u8*)(sp + 0x11) = r3;
        f0 = (f64)(s32)f0;
        *(u8*)(sp + 0x12) = tmp;
        *(u8*)(sp + 0x13) = tmp;
        tmp = r14 << 2;
        r3 = r18 + tmp;
        r3 = *(u32*)((u8*)r3 + 0x200);
        *(u32*)((u8*)r3 + 0x64) = r4;
        tmp = *(u32*)((u8*)r18 + 0xC4);
        if ((s32)tmp != 0x10) break;
        tmp = 0x9;
        *(u32*)((u8*)r18 + 0xBC) = tmp;
        break;
        case 9:
        r14 = r14 << 2;
        r4 = 0x0;
        r3 = r18 + r14;
        r3 = *(u32*)((u8*)r3 + 0x440);
        fn_80109220();
        r3 = r18 + r14;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r3 + 0x320);
        fn_80109220();
        r3 = r18 + r14;
        r4 = 0x0;
        r3 = *(u32*)((u8*)r3 + 0x200);
        fn_80109220();
        }
    }
} while (0);
    r3 = 0xe8;
    fn_80104704();
    if (r3 == 0) return;
    r5 = *(u32*)((u8*)r3 + 0x1C);
    r3 = -0x56;
    tmp = -0x1;
    while (1) {
        if (r5 == 0) break;
        r4 = *(u8*)((u8*)r5 + 0x5);
        r4 = r4 & 0x1;
        if ((s32)r4 != 0) {
            r4 = *(u8*)((u8*)r18 + 0xB6);
            do {
            if (r4 != 0) break;
                r4 = *(u32*)((u8*)r18 + 0xBC);
                if ((s32)r4 == 0) break;
                *(u32*)((u8*)r5 + 0x64) = r3;
                break;
            } while (0);

            *(u32*)((u8*)r5 + 0x64) = tmp;
        }
        r5 = *(u32*)((u8*)r5 + 0x0);

    }

    return;
}
#pragma pop
