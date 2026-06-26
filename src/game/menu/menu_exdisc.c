/**
 * @file menu_exdisc.c
 * @brief Extra disc shrine and related menus (0x80077A5C-0x80078D38)
 *
 * Address range: 0x80077A5C - 0x80078D38
 * Total functions: 20
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8001E074();
extern void fn_8006B420();
extern void fn_80075B74();
extern void fn_80075BFC();
extern void fn_80092C90();
extern void fn_80093574();
extern void fn_80093610();
extern void fn_80093698();
extern void fn_800C80D0();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void _threadSwitch();
extern void fn_80103CC0();
extern void fn_801067E8();
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_801159F0();
extern void fn_80115BD8();
extern void fn_80123FBC();
extern void fn_80124A60();
extern void fn_8012640C();
extern void fn_80129280();
extern void fn_8012A5B0();
extern void fn_8012AA2C();
extern void fn_8012AC08();
extern void fn_8012AC54();
extern void fn_80130660();
extern void fn_80132A38();
extern void fn_80142984();
extern void fn_80165668();
extern void fn_80166A28();
extern void __assert();
extern void fn_801C40F0();
extern void fn_801C41C8();
extern void fn_801CB708();
extern void fn_801CB834();
extern void fn_801D0314();
extern void fn_801D036C();
extern void fn_801D0748();
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u32 lbl_80478928;
extern u8 lbl_8047A620;
extern u8 lbl_8047C0E0;
extern u8 lbl_8047C0E4;
extern u8 lbl_8047C0E8;
extern u8 lbl_8047C0F0;
extern u8 lbl_8047C0F8;
extern u8 lbl_8047C100;

/* ===== Rodata / data labels ===== */
extern u8 lbl_80268940[];
extern u8 lbl_80268AB8[];
extern u8 lbl_802EE458[];
extern u8 lbl_803F6E40[];
extern u8 lbl_803F6F18[];

/* ===== Forward declarations ===== */
void fn_80077A5C(void);
u8 fn_80077AAC(void);
u8 fn_80077AD0(void);
u8 fn_80077AF4(void);
u8 fn_80077B18(void);
u8 fn_80077B3C(void);
u8 fn_80077B60(void);
s16 fn_80077B84(void);
s32 fn_80077BA8(void);
u8 fn_80077BD0(void);
u8 fn_80077C1C(u32 arg0);
void fn_80077C68(void);
u16 fn_80077D88(s32 index);
void fn_80077DB8(void);
void* fn_80077E50(s32 index);
void fn_80077E80(void* dst, void* src);
u8 fn_80077EA4(u16* s1, u16* s2);
void fn_80077ED4(void);
void fn_80078390(void);
void fn_800788BC(void);

/* ===== Function implementations ===== */


/* 0x80077A5C | size: 0x50 */
void fn_80077A5C(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;

    r31 = 0x0;
    if (r3 != 0x0) {
        r4 = 0x0;
        r5 = 0x6e;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if ((s32)r3 != 0x0) { r3 = r31 & 0xFF; return; }
    }
    r31 = 0x1;

    r3 = r31 & 0xFF;
    return;
}

/* 0x80077AAC | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077AAC(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0x13];
}
#pragma pop

/* 0x80077AD0 | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077AD0(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0x12];
}
#pragma pop

/* 0x80077AF4 | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077AF4(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0x11];
}
#pragma pop

/* 0x80077B18 | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077B18(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0x10];
}
#pragma pop

/* 0x80077B3C | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077B3C(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0xf];
}
#pragma pop

/* 0x80077B60 | size: 0x24 */
#pragma push
#pragma scheduling off
u8 fn_80077B60(void) {
    extern u8* fn_8006B420(void);
    return fn_8006B420()[0xe];
}
#pragma pop

/* 0x80077B84 | size: 0x24 */
#pragma push
#pragma scheduling off
s16 fn_80077B84(void) {
    extern s16* fn_8006B420(void);
    return fn_8006B420()[0xb];
}
#pragma pop

/* 0x80077BA8 | size: 0x28 */
s32 fn_80077BA8(void) {
    extern s16* fn_8006B420(void);
    return fn_8006B420()[0xa] * 0x3c;
}

/* 0x80077BD0 | size: 0x4C */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
u8 fn_80077BD0(void) {
    extern s32* fn_80129280(s32, s32);
    s32 value;

    value = fn_80129280(0, 0xe)[2];
    switch (value) {
    case 0:
    case 1:
    case 2:
        return 1;
    }
    return 0;
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/* 0x80077C1C | size: 0x4C */
u8 fn_80077C1C(u32 arg0) {
    extern u8 fn_80142984(u32);
    u32 value;

    value = arg0 & 0xffff;
    switch (value) {
    case 0:
        return 1;
    case 0xaf:
        return 0;
    default:
        return fn_80142984(arg0);
    }
}

/* 0x80077C68 | size: 0x120 */
void fn_80077C68(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r3;
    ((void(*)(void))fn_8006B420)();
    tmp = r30 & 0xFFFF;
    r31 = r3;
    if ((s32)tmp == 0xaf) {
        /* L_80077CAC */
        r3 = 0x0;
    } else if ((s32)tmp == 0) {
        /* L_80077CA4 */
        r3 = 0x1;
    } else {
        /* L_80077CB4 */
        r3 = r30;
        ((void(*)(void))fn_80142984)();
    }
    /* L_80077CBC */
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r3 = 0x0;
        return;
    }
    tmp = *(u32*)((u8*)r31 + 0x8);
    if ((s32)tmp == 1) {
        /* L_80077D00 */
        tmp = r30 & 0xFFFF;
        tmp = __cntlzw(tmp);
        tmp = (u32)tmp >> 5;
        r3 = tmp & 0xFF;
        return;
    }
    if ((s32)tmp < 1) {
        if ((s32)tmp >= 0) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
    }
    if ((s32)tmp >= 3) { r3 = 0x0; return; }
    /* L_80077D14 */
    r3 = (u32)&lbl_802EE458;
    tmp = *(u32*)&lbl_80478928;
    r4 = (u32)&lbl_802EE458;
    r5 = 0x0;
    r3 = r30 & 0xFFFF;
    ctr_fn = (void(*)(void))tmp;
    if (tmp > 0) {
        do {
            tmp = *(u16*)((u8*)r4 + 0x0);
            if (r3 == tmp) {
                r3 = r31 + r5;
                tmp = *(u8*)((u8*)r3 + 0x18);
                tmp = __cntlzw(tmp);
                tmp = (u32)tmp >> 5;
                r3 = tmp & 0xFF;
                return;
            }
            r4 = r4 + 0x2;
            r5 = r5 + 0x1;
        } while (--ctr != 0);
    }
    r3 = 0x1;
    return;

    r3 = 0x0;

    return;
}

/* 0x80077D88 | size: 0x30 */
u16 fn_80077D88(s32 index) {
    if (index < 0) {
        goto ret0;
    }
    if (lbl_80478928 > (u32)index) {
        goto retTable;
    }
ret0:
    return 0;
retTable:
    return ((u16*)lbl_802EE458)[index];
}

/* 0x80077DB8 | size: 0x98 */
void fn_80077DB8(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    r3 = 0x0;
    r4 = 0xe;
    ((void(*)(void))fn_80129280)();
    tmp = *(u32*)((u8*)r3 + 0x0);
    if ((s32)tmp == 2) {
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        tmp = *(u32*)((u8*)r3 + 0x8);
        if ((s32)tmp == 0) {
            r3 = 0x6;
            return;
    }
    }
    r3 = 0x0;
    r4 = 0xe;
    ((void(*)(void))fn_80129280)();
    tmp = *(u32*)((u8*)r3 + 0x4);
    if ((s32)tmp == 1) { r3 = 0x4; return; }
    if ((s32)tmp < 1) {
        if ((s32)tmp >= 0) { r3 = 0x3; return; }
        r3 = 0x2;
        return;
    }
    r3 = 0x2;
    return;

    r3 = 0x3;
    return;

    r3 = 0x4;
    return;

    r3 = 0x2;

    return;
}

/* 0x80077E50 | size: 0x30 */
void* fn_80077E50(s32 index) {
    switch (index) {
    case 0:
    case 1:
    case 2:
        return lbl_80268940 + (index * 0x54);
    }
    return (void*)0;
}

/* 0x80077E80 | size: 0x24 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_80077E80(void* dst, void* src) {
    memcpy(dst, src, 0x54);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/* 0x80077EA4 | size: 0x30 */
#pragma push
#pragma peephole off
u8 fn_80077EA4(u16* s1, u16* s2) {
    extern s32 fn_800C80D0(u16*, u16*, u32);
    return fn_800C80D0(s1, s2, 0x54) == 0;
}
#pragma pop

/* 0x80077ED4 | size: 0x4BC */
void fn_80077ED4(void) {
    u8 sp[0x80];
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
    f32 f0 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)&lbl_803F6F18;
    r3 = (u32)&lbl_803F6F18;
    ((void(*)(void))fn_8012AC54)();
    r30 = r3;
    ((void(*)(void))fn_801D036C)();
    tmp = r3;
    r3 = 0x0;
    r29 = tmp;
    r4 = 0x0;
    ((void(*)(void))fn_80129280)();
    tmp = 0x3bfa;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        tmp = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = tmp;
    } while (--ctr != 0);
    ((void(*)(void))fn_80075B74)();
    r3 = 0x4;
    r4 = 0x2;
    r5 = 0x0;
    ((void(*)(void))fn_801D0748)();
    if ((s32)r3 != 4) {
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    r3 = 0x104F0000;
    r4 = 0x4;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    f27 = *(f32*)&lbl_8047C0E0;
    f31 = *(f64*)&lbl_8047C0F0;
    r28 = 0x43300000;
    f29 = *(f64*)&lbl_8047C0F8;
    f28 = *(f32*)&lbl_8047C0E4;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0xC) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    r4 = r30;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x44b0;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x44cf;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    } else {
    /* L_80078048 */
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x3d83;
    r5 = 0x0;
    r6 = 0x1;
    ((void(*)(void))fn_80106D3C)();
    ((void(*)(void))_threadSwitch)();
    r27 = 0x0;
    r3 = (u32)&lbl_803F6F18;
    r28 = (u32)&lbl_803F6F18;
    while (1) {
        tmp = r27 & 0xFFFF;
        if (tmp >= 6) break;
        r3 = r28;
        r4 = r27;
        ((void(*)(void))fn_8012AC08)();
        ((void(*)(void))fn_80124A60)();
        r27 = r27 + 0x1;


    }
    ((void(*)(void))fn_80115BD8)();
    ((void(*)(void))fn_801159F0)();
    r5 = (u32)&lbl_803F6F18;
    r4 = r3;
    r3 = (u32)&lbl_803F6F18;
    ((void(*)(void))fn_80130660)();
    r27 = 0x0;
    r3 = (u32)&lbl_803F6F18;
    r28 = (u32)&lbl_803F6F18;
    while ((r27 & 0xFFFF) < 6) {
        /* L_800780C0 */
        r3 = r28;
        r4 = r27;
        ((void(*)(void))fn_8012AC08)();
        ((void(*)(void))fn_80123FBC)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = (u32)&lbl_803F6F18;
            r4 = r27;
            r3 = (u32)&lbl_803F6F18;
            ((void(*)(void))fn_8012AC08)();
            r31 = r3;
            break;
        }
        /* L_800780F4 */
        r27 = r27 + 0x1;
    }
    /* L_80078104 */
    if (r31 == 0) {
        r3 = (u32)&lbl_80268AB8;
        r4 = 0x42e;
        r3 = (u32)&lbl_80268AB8;
        r5 = (u32)&lbl_8047C0E8;
        ((void(*)(void))__assert)();
    }
    r4 = (u32)&lbl_803F6E40;
    r3 = 0x1;
    r4 = (u32)&lbl_803F6E40;
    tmp = *(u32*)((u8*)r4 + 0x8);
    tmp = tmp | 0x8;
    *(u32*)((u8*)r4 + 0x8) = tmp;
    ((void(*)(void))fn_80093574)();
    r3 = (u32)&lbl_803F6E40;
    r5 = r31;
    r4 = (u32)&lbl_803F6E40;
    r3 = 0x1;
    ((void(*)(void))fn_80092C90)();
    r3 = 0x1;
    ((void(*)(void))fn_80093574)();
    r3 = 0x1;
    ((void(*)(void))fn_80093610)();
    if ((s32)r3 != 0xc) {
    r3 = 0x1;
    ((void(*)(void))fn_80093698)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x3d85;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_80129280)();
    tmp = 0x3bfa;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        tmp = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = tmp;
    } while (--ctr != 0);
    r3 = 0x4;
    r4 = 0x2;
    r5 = 0x0;
    ((void(*)(void))fn_801D0748)();
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    r3 = 0x104F0000;
    r4 = 0x4;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    f27 = *(f32*)&lbl_8047C0E0;
    f28 = *(f64*)&lbl_8047C0F0;
    r31 = 0x43300000;
    f30 = *(f64*)&lbl_8047C0F8;
    f31 = *(f32*)&lbl_8047C0E4;
    while (f27 < f31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f29 = f0 - f28;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f30;
        f0 = f0 / f29;
        f27 = f27 + f0;

    }
    r4 = r30;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x44b0;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x44cf;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    } else {
    /* L_800782B8 */
    r3 = 0x1;
    ((void(*)(void))fn_80093698)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x3d84;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x3d2;
    r4 = 0x0;
    r5 = 0xff;
    ((void(*)(void))fn_80165668)();
    r4 = r30;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x4435;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x3d55;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    } /* end if/else r3 == 0xc */
    } /* end if/else r3 == 4 */
    /* L_80078344 */
    r3 = r29;
    ((void(*)(void))fn_801D0314)();
    tmp = 0x0;
    *(u32*)&lbl_8047A620 = tmp;
    return;
}

/* 0x80078390 | size: 0x52C */
void fn_80078390(void) {
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r3 = (u32)&lbl_803F6F18;
    r3 = (u32)&lbl_803F6F18;
    ((void(*)(void))fn_8012AA2C)();
    r4 = (u32)&lbl_803F6F18;
    r31 = r3;
    r3 = (u32)&lbl_803F6F18;
    ((void(*)(void))fn_8012AC54)();
    r4 = (u32)&lbl_803F6E40;
    r4 = (u32)&lbl_803F6E40;
    tmp = *(u32*)((u8*)r4 + 0x8);
    tmp = tmp & 0x00000008;
    if (tmp != 0) {
        r4 = r3;
        r3 = 0x4d;
        ((void(*)(void))fn_80132A38)();
        tmp = r31 & 0xFF;
        if ((s32)tmp == 0) {
            /* L_80078428 */
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r3 = 0x43cb;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
        } else {
            /* L_80078444 */
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r3 = 0x43cd;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
        }
        /* L_8007845C */
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x2;
        r4 = 0x44cf;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        tmp = 0x0;
        *(u32*)&lbl_8047A620 = tmp;
        return;
    }
    r30 = r3;
    r4 = r3;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    tmp = r31 & 0xFF;
    if ((s32)tmp == 0) {
        /* L_800784C0 */
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x43c7;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
    } else {
        /* L_800784DC */
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x43c9;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
    }
    /* L_800784F4 */
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    f1 = *(f32*)&lbl_8047C100;
    r3 = 0x3;
    ((void(*)(void))fn_801C41C8)();
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    r3 = 0x104F0000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    f1 = *(f32*)&lbl_8047C100;
    r3 = 0x2;
    ((void(*)(void))fn_801C41C8)();
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    r3 = 0x104F0000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x1;
    ((void(*)(void))fn_801CB834)();
    r4 = r30;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x4434;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = (u32)&lbl_803F6E40;
    r3 = (u32)&lbl_803F6E40;
    tmp = *(u16*)((u8*)r3 + 0xC);
    if (tmp == 6) {
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x43a6;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x4c7;
        ((void(*)(void))fn_80166A28)();
        r3 = 0x104F0000;
        r4 = 0x4;
        r3 = r3 + 0x1000;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_801CB834)();
        r3 = 0x104F0000;
        r4 = 0x1;
        r3 = r3 + 0x1000;
        ((void(*)(void))fn_801CB708)();
        f27 = *(f32*)&lbl_8047C0E0;
        f31 = *(f64*)&lbl_8047C0F0;
        r31 = 0x43300000;
        f29 = *(f64*)&lbl_8047C0F8;
        f28 = *(f32*)&lbl_8047C0E4;
        while (f27 < f28) {

            ((void(*)(void))_threadSwitch)();
            ((void(*)(void))fn_800D37CC)();
            *(u32*)(sp + 0xC) = tmp;
            f30 = f0 - f31;
            ((void(*)(void))fn_800D3088)();
            f0 = f0 - f29;
            f0 = f0 / f30;
            f27 = f27 + f0;

        }
        r4 = r30;
        r3 = 0x4d;
        ((void(*)(void))fn_80132A38)();
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x44b0;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r3 = 0x2;
        r4 = 0x44cf;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        tmp = 0x0;
        *(u32*)&lbl_8047A620 = tmp;
        return;
    }
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    r3 = 0x104F0000;
    r4 = 0x5;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    f27 = *(f32*)&lbl_8047C0E0;
    f31 = *(f64*)&lbl_8047C0F0;
    r31 = 0x43300000;
    f29 = *(f64*)&lbl_8047C0F8;
    f28 = *(f32*)&lbl_8047C0E4;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    r4 = r30;
    r3 = 0x4d;
    ((void(*)(void))fn_80132A38)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x43b0;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x0;
    r4 = -0x1;
    r5 = -0x1;
    r6 = 0x0;
    ((void(*)(void))fn_8001E074)();
    r31 = r3;
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    tmp = (s8)r31;
    if ((s32)tmp == -1 || (s32)tmp == 1) {
    /* L_8007879C */
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    r3 = 0x104F0000;
    r4 = 0x4;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    f27 = *(f32*)&lbl_8047C0E0;
    f28 = *(f64*)&lbl_8047C0F0;
    r31 = 0x43300000;
    f30 = *(f64*)&lbl_8047C0F8;
    f31 = *(f32*)&lbl_8047C0E4;
    while (f27 < f31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f29 = f0 - f28;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f30;
        f0 = f0 / f29;
        f27 = f27 + f0;

    }
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x44b0;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    r3 = 0x2;
    r4 = 0x44cf;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    tmp = 0x0;
    *(u32*)&lbl_8047A620 = tmp;
    return;
    }
    /* L_80078874 */
    tmp = 0xb;
    *(u32*)&lbl_8047A620 = tmp;

    return;
}

/* 0x800788BC | size: 0x47C */
void fn_800788BC(void) {
    u8 sp[0x80];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r3;
    r3 = 0x43a1;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x43a2;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    f1 = *(f32*)&lbl_8047C100;
    r3 = 0x3;
    ((void(*)(void))fn_801C41C8)();
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    r3 = 0x104F0000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    f1 = *(f32*)&lbl_8047C100;
    r3 = 0x2;
    ((void(*)(void))fn_801C41C8)();
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    r3 = 0x104F0000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x1;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x43a3;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x43a4;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x0;
    r4 = 0x2;
    ((void(*)(void))fn_80129280)();
    r30 = 0x0;
    r29 = r3;
    tmp = 0x0; /* default: not found */
    while ((r30 & 0xFF) < 6) {
        /* L_800789E8 */
        r3 = r29;
        r5 = r30 & 0xFF;
        r4 = 0x3;
        ((void(*)(void))fn_8012A5B0)();
        ((void(*)(void))fn_80123FBC)();
        tmp = r3 & 0xFF;
        if (tmp != 1) {
            tmp = 0x1;
            break;
        }
        /* L_80078A10 */
        r30 = r30 + 0x1;
    }
    /* L_80078A24 */
    tmp = tmp & 0xFF;
    if (tmp == 0) {
        r3 = 0x43a6;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x4c7;
        ((void(*)(void))fn_80166A28)();
        r3 = 0x104F0000;
        r4 = 0x4;
        r3 = r3 + 0x1000;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_801CB834)();
        r3 = 0x104F0000;
        r4 = 0x1;
        r3 = r3 + 0x1000;
        ((void(*)(void))fn_801CB708)();
        f27 = *(f32*)&lbl_8047C0E0;
        f31 = *(f64*)&lbl_8047C0F0;
        r30 = 0x43300000;
        f29 = *(f64*)&lbl_8047C0F8;
        f28 = *(f32*)&lbl_8047C0E4;
        while (f27 < f28) {

            ((void(*)(void))_threadSwitch)();
            ((void(*)(void))fn_800D37CC)();
            *(u32*)(sp + 0xC) = tmp;
            f30 = f0 - f31;
            ((void(*)(void))fn_800D3088)();
            f0 = f0 - f29;
            f0 = f0 / f30;
            f27 = f27 + f0;

        }
        r3 = 0x43ac;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        tmp = 0x0;
        *(u32*)&lbl_8047A620 = tmp;
        return;
    }
    r3 = 0x4c7;
    ((void(*)(void))fn_80166A28)();
    r3 = 0x104F0000;
    r4 = 0x5;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    ((void(*)(void))fn_801CB834)();
    r3 = 0x104F0000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    ((void(*)(void))fn_801CB708)();
    f27 = *(f32*)&lbl_8047C0E0;
    f31 = *(f64*)&lbl_8047C0F0;
    r30 = 0x43300000;
    f29 = *(f64*)&lbl_8047C0F8;
    f28 = *(f32*)&lbl_8047C0E4;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_80129280)();
    tmp = 0x3bfa;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        tmp = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = tmp;
    } while (--ctr != 0);
    ((void(*)(void))fn_80075BFC)();
    ((void(*)(void))fn_80115BD8)();
    ((void(*)(void))fn_801159F0)();
    r4 = r3;
    r3 = r29;
    ((void(*)(void))fn_80130660)();
    r3 = 0x4;
    r4 = 0x2;
    r5 = 0x0;
    ((void(*)(void))fn_801D0748)();
    tmp = (s8)r3;
    if ((s32)tmp != 4) {
        r3 = 0x4c7;
        ((void(*)(void))fn_80166A28)();
        r3 = 0x104F0000;
        r4 = 0x4;
        r3 = r3 + 0x1000;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_801CB834)();
        r3 = 0x104F0000;
        r4 = 0x1;
        r3 = r3 + 0x1000;
        ((void(*)(void))fn_801CB708)();
        f27 = *(f32*)&lbl_8047C0E0;
        f28 = *(f64*)&lbl_8047C0F0;
        r30 = 0x43300000;
        f30 = *(f64*)&lbl_8047C0F8;
        f31 = *(f32*)&lbl_8047C0E4;
        while (f27 < f31) {

            ((void(*)(void))_threadSwitch)();
            ((void(*)(void))fn_800D37CC)();
            *(u32*)(sp + 0x14) = tmp;
            f29 = f0 - f28;
            ((void(*)(void))fn_800D3088)();
            f0 = f0 - f30;
            f0 = f0 / f29;
            f27 = f27 + f0;

        }
        r3 = 0x0;
        r4 = 0x0;
        ((void(*)(void))fn_80129280)();
        tmp = 0x3bfa;
        ctr_fn = (void(*)(void))tmp;
        do {
            r3 = *(u32*)((u8*)r4 + 0x4);
            tmp = *(u32*)((u8*)r4 + 0x8);
            *(u32*)((u8*)r5 + 0x4) = r3;
            r5 += 8; *(u32*)r5 = tmp;
        } while (--ctr != 0);
        r3 = 0x43ac;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        tmp = 0x0;
        *(u32*)&lbl_8047A620 = tmp;
        return;
    }
    r3 = 0x3d2;
    r4 = 0x0;
    r5 = 0xff;
    ((void(*)(void))fn_80165668)();
    r3 = 0x43a8;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x43aa;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    tmp = 0x0;
    *(u32*)&lbl_8047A620 = tmp;

    return;
}
