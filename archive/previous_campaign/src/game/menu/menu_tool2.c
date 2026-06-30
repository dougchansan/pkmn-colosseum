/**
 * @file menu_tool2.c
 * @brief Menu tool functions continued (0x80075818-0x80076788)
 *
 * Address range: 0x8007581C - 0x800767B8
 * Total functions: 25
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8005E750();
extern void fn_80062948();
extern void fn_80063D14();
extern void fn_80069A60();
extern void fn_8006B420();
extern void fn_80071398();
extern void fn_800715BC();
extern void fn_80076F2C();
extern void fn_800C46B0();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void _threadSwitch();
extern void GSthreadCreate();
extern void fn_800F9318();
extern void fn_800FA280();
extern void fn_800FF52C();
extern void fn_800FF560();
extern void fn_800FF58C();
extern void fn_800FF660();
extern void fn_80102510();
extern void fn_801026A4();
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_8011288C();
extern void fn_80113828();
extern void fn_80113F48();
extern void fn_8011CA34();
extern void fn_8011E7A4();
extern void fn_8011E8DC();
extern void fn_8011EF78();
extern void fn_8011EFA4();
extern void fn_8011EFD0();
extern void fn_8011EFFC();
extern void fn_8011F028();
extern void fn_8011F054();
extern void fn_8011F080();
extern void fn_8011F0AC();
extern void fn_8011F0D8();
extern void fn_8011F104();
extern void fn_8011F130();
extern void fn_8011F15C();
extern void fn_8011F1A0();
extern void fn_8011F4A8();
extern void fn_8011F5C8();
extern void fn_8011F5FC();
extern void fn_8011FC74();
extern void fn_80123FBC();
extern void fn_8012546C();
extern void fn_8012640C();
extern void fn_80129280();
extern void fn_8012AC08();
extern void fn_80132A38();
extern void fn_80135168();
extern void fn_80142984();
extern void fn_80165A20();
extern void fn_80176E0C();
extern void GSscene_SetMode();
extern void fn_801902E0();
extern void fn_80190528();
extern void fn_801906A0();
/* ... and 7 more external functions */

/* ===== SDA globals ===== */
extern u8 lbl_80478928;
extern u8 lbl_8047A5D0;
extern u8 lbl_8047A618;
extern u8 lbl_8047C0C0;
extern u8 lbl_8047C0C4;
extern u8 lbl_8047C0C8;
extern u8 lbl_8047C0D0;
extern u8 lbl_8047C0D8;

/* ===== Rodata / data labels ===== */
extern u8 lbl_802688F8[];
extern u8 lbl_8026890C[];
extern u8 lbl_80268940[];
extern u8 lbl_802EE458[];

/* ===== Forward declarations ===== */
void fn_8007581C(void);
void fn_80075A34(void);
s32 fn_80075A9C(void);
s32 fn_80075AC0(void);
s32 fn_80075AE4(void);
s32 fn_80075B08(void);
s32 fn_80075B2C(void);
s32 fn_80075B50(void);
void fn_80075B74(void);
void fn_80075BC4(void);
s32 fn_80075BFC(void);
s32 fn_80075C20(void);
s32 fn_80075C44(void);
s32 fn_80075C68(void);
void fn_80075C94(void);
void fn_80075D98(void);
s32 fn_80075D9C(void);
void fn_80075DC8(void);
void fn_80075EE0(void);
s32 fn_80075F4C(void);
void fn_80075F78(void);
void fn_80075FEC(void);
void fn_80076054(void);
void fn_80076334(void);
void fn_80076398(void);

/* ===== Function implementations ===== */

/* 0x8007581C | size: 0x218 */
void fn_8007581C(void) {
    extern void fn_8019075C();
    extern void __assert();
    extern void fn_801C40F0();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = 0x1;
    r31 = 0x0;
    while (1) {
        r0 = r30 & 0xFF;
        if (r0 == 0x0) break;
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        ((void(*)(void))fn_8005E750)();
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        ((void(*)(void))fn_80063D14)();
        r31 = r3;
        if ((s32)r31 != 0xb8 || (s32)r31 >= 0xb8) {

            if ((s32)r31 == 0xb3) {
                ((void(*)(void))fn_80071398)();
                break;
            }
        }
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        ((void(*)(void))fn_80069A60)();
        r31 = r3;
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        *(u32*)((u8*)r3 + 0x20) = r31;
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        ((void(*)(void))fn_80062948)();
        r31 = r3;
        if ((s32)r31 != 0xb4) {
            if ((s32)r31 < 0xb4) {
                if ((s32)r31 != 0xac) {
                    if ((s32)r31 < 0xac) {
                        { r3 = r31; ((void(*)(void))fn_80071398)(); r30 = 0x0; continue; }
                    }
                    { r3 = r31; ((void(*)(void))fn_80071398)(); r30 = 0x0; continue; }
                }
                if ((s32)r31 != 0xd1) {
                    if ((s32)r31 < 0xd1) {
                        { r3 = r31; ((void(*)(void))fn_80071398)(); r30 = 0x0; continue; }
                    }
                    if ((s32)r31 != 0x105) {
                        { r3 = r31; ((void(*)(void))fn_80071398)(); r30 = 0x0; continue; }
                    }
                    r3 = 0x0;
                    r4 = 0xe;
                    ((void(*)(void))fn_80129280)();
                    r0 = *(u32*)((u8*)r3 + 0x0);
                    if ((s32)r0 == 0x1) {
                        r3 = 0x0;
                        r4 = 0xe;
                        ((void(*)(void))fn_80129280)();
                        r0 = *(u32*)((u8*)r3 + 0x20);
                        if ((s32)r0 == 0x2) {
                            r30 = 0x0;
                    }
                    }
                    continue;
                    }
                r3 = 0x105;
                ((void(*)(void))fn_800715BC)();
                r30 = 0x0;
                continue;
                }
            r3 = 0x0;
            r4 = 0xe;
            ((void(*)(void))fn_80129280)();
            r0 = *(u32*)((u8*)r3 + 0x0);
            if ((s32)r0 != 0x0) {
                { r3 = r31; ((void(*)(void))fn_80071398)(); r30 = 0x0; continue; }
            }
            r31 = 0xae;
        }

    }

    ((void(*)(void))fn_800FF52C)();
    r0 = r3 & 0xFF;
    if (r0 != 0x0) {
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        r0 = *(u32*)((u8*)r3 + 0x0);
        if ((s32)r0 != 0x1) {
            r3 = (u32)&lbl_802688F8;
            r5 = (u32)&lbl_8026890C;
            r3 = (u32)&lbl_802688F8;
            r4 = 0xa7;
            r5 = (u32)&lbl_8026890C;
            __assert();
        }
        ((void(*)(void))fn_800FF660)();
        r3 = 0x0;
        r4 = 0xe;
        ((void(*)(void))fn_80129280)();
        r0 = *(u32*)((u8*)r3 + 0x0);
        if ((s32)r0 != 0x1) {
            return;
        }
        if ((s32)r31 == 0xd1) return;
        r3 = 0x8ae;
        r4 = 0x0;
        fn_8019075C();
        r3 = (0x596 << 16);
        r4 = 0x0;
        r3 = r3 + 0x9;
        ((void(*)(void))fn_8011288C)();
        r3 = 0x1;
        fn_801C40F0();
        return;
    }
    r3 = 0x395;
    ((void(*)(void))fn_800FF58C)();

    return;
}

/* 0x80075A34 | size: 0x68 */
void fn_80075A34(void) {
    extern void fn_801CBA0C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;

    ((void(*)(void))fn_80113F48)();
    r4 = 0x10800000;
    r31 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    r4 = r3;
    r3 = r31;
    *(u32*)&lbl_8047A5D0 = r4;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x10820000;
    r3 = 0x5e0;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x1;
    ((void(*)(void))fn_80176E0C)();
    r3 = 0x4;
    ((void(*)(void))GSscene_SetMode)();
    return;
}

/* 0x80075A9C | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075A9C(void) {
    extern s32 fn_80190528(s32);
    return fn_80190528(0xab5);
}
#pragma pop

/* 0x80075AC0 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075AC0(void) {
    extern s32 fn_801902E0(s32);
    return fn_801902E0(0xab5);
}
#pragma pop

/* 0x80075AE4 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075AE4(void) {
    extern s32 fn_80190528(s32);
    return fn_80190528(0xab4);
}
#pragma pop

/* 0x80075B08 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075B08(void) {
    extern s32 fn_801902E0(s32);
    return fn_801902E0(0xab4);
}
#pragma pop

/* 0x80075B2C | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075B2C(void) {
    extern s32 fn_80190528(s32);
    return fn_80190528(0xab3);
}
#pragma pop

/* 0x80075B50 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075B50(void) {
    extern s32 fn_801902E0(s32);
    return fn_801902E0(0xab3);
}
#pragma pop

/* 0x80075B74 | size: 0x50 */
void fn_80075B74(void) {
    extern void fn_8019075C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    r3 = 0xab2;
    ((void(*)(void))fn_801906A0)();
    r4 = r3 + 0x1;
    r31 = 0x1;
    if (r4 > 0x30) {
        r4 = 0x30;
        r31 = 0x0;
    }
    r3 = 0xab2;
    fn_8019075C();
    r3 = r31;
    return;
}

/* 0x80075BC4 | size: 0x38 */
void fn_80075BC4(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;

    r3 = 0xab2;
    ((void(*)(void))fn_801906A0)();
    if (r3 > 0x30) {
        r3 = 0x0;
    } else {

        r3 = 0x30 - r3;
    }
    return;
}

/* 0x80075BFC | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075BFC(void) {
    extern s32 fn_80190528(s32);
    return fn_80190528(0xab1);
}
#pragma pop

/* 0x80075C20 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075C20(void) {
    extern s32 fn_801902E0(s32);
    return fn_801902E0(0xab1);
}
#pragma pop

/* 0x80075C44 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075C44(void) {
    extern s32 fn_801902E0(s32);
    return fn_801902E0(0xa14);
}
#pragma pop

/* 0x80075C68 | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_80075C68(void) {
    extern void fn_801C40F0(s32);
    extern s32 fn_80102510(s32);
    fn_801C40F0(0x1);
    return fn_80102510(0xe0);
}
#pragma pop

/* 0x80075C94 | size: 0x104 */
void fn_80075C94(void) {
    extern void fn_801C40F0();
    extern void fn_801D0748();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r31 = 0;

    while (1) {
    r3 = 0x37;
    r4 = 0x0;
    ((void(*)(void))fn_80132A38)();
    r3 = 0xe0;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x10;
    r7 = 0x1;
    r8 = 0x0;
    ((void(*)(void))fn_801026A4)();
    r31 = r3;
    r3 = 0x1;
    fn_801C40F0();
    if ((s32)r31 != 1) {
        if ((s32)r31 < 1) {
            { r3 = 0x320; r4 = 0x0; ((void(*)(void))fn_80113828)(); return; }
            }
        r3 = 0x322;
        r4 = 0x0;
        ((void(*)(void))fn_80113828)();
        return;
    }
    r3 = 0x2;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    r31 = r3;
    if ((s32)r31 == 3) {
        r3 = 0x0;
        r4 = 0x4;
        ((void(*)(void))fn_80135168)();
        if (r3 == 0) {
        }
        if ((s32)r31 == (s32)-0x1) continue;
        r3 = 0x2;
        r4 = 0x44db;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        continue;
        }
    r3 = 0x323;
    r4 = 0x0;
    ((void(*)(void))fn_80113828)();
    return;
    }
}

/* 0x80075D98 | size: 0x4 */
void fn_80075D98(void) {
}

/* 0x80075D9C | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_80075D9C(void) {
    extern void fn_801C40F0(s32);
    extern s32 fn_80102510(s32);
    fn_801C40F0(0x1);
    return fn_80102510(0xe2);
}
#pragma pop

/* 0x80075DC8 | size: 0x118 */
void fn_80075DC8(void) {
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801CB834();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    ((void(*)(void))fn_80113F48)();
    r4 = 0xB560000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    ((void(*)(void))fn_80176E0C)();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C0C0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r31 = r31 + r3;

    }
    r3 = 0xB540000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0xe2;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x10;
    r7 = 0x1;
    r8 = 0x0;
    ((void(*)(void))fn_801026A4)();
    if ((s32)r3 == 0) {
        r30 = 0x321;
    } else {
        r30 = 0x384;
    }
    r3 = 0x1;
    fn_801C40F0();
    f1 = *(f32*)&lbl_8047C0C4;
    r3 = 0x3;
    fn_801C41C8();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_80113828)();
    r4 = 0x5960000;
    r3 = 0x0;
    r4 = r4 + 0x8;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80075EE0 | size: 0x6C */
void fn_80075EE0(void) {
    extern void fn_801C41C8();
    extern void fn_80075F4C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    f32 f1 = 0.0f;

    tmp = *(u32*)&lbl_8047A618;
    if ((s32)tmp == 0) {
        ((void(*)(void))fn_800FF560)();
        r5 = (u32)fn_80075F4C;
        r4 = r3;
        r8 = (u32)fn_80075F4C;
        r3 = 0x1;
        r5 = 0x4000;
        r6 = 0x1;
        r7 = 0x1;
        ((void(*)(void))GSthreadCreate)();
    } else {

        r3 = 0x46a;
        r4 = 0x0;
        r5 = 0x7f;
        ((void(*)(void))fn_80165A20)();
    }
    f1 = *(f32*)&lbl_8047C0C8;
    r3 = 0x2;
    fn_801C41C8();
    return;
}

/* 0x80075F4C | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_80075F4C(void) {
    extern s32 fn_80165A20(s32, s32, s32);
    return fn_80165A20(0x46a, 0x0, 0x7f);
}
#pragma pop

/* 0x80075F78 | size: 0x74 */
void fn_80075F78(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    tmp = *(u8*)((u8*)r3 + 0x95);
    tmp = (s8)tmp;
    do {
        if ((s32)tmp < 0x0 || (s32)tmp >= 0x3) {
            r3 = 0x1;
            break;
        }
        if ((s32)tmp == 1) {
            r3 = 0x43ba;
            break;
        }
        if ((s32)tmp >= 0x2) {
            r3 = 0x43bc;
            break;
        }
        r3 = 0x43be;
    } while (0);

    ((void(*)(void))fn_800FA280)();
    r4 = r3;
    r3 = 0x37;
    ((void(*)(void))fn_80132A38)();
    return;
}

/* 0x80075FEC | size: 0x68 */
void fn_80075FEC(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r31 = 0;

    r31 = r3;
    ((void(*)(void))fn_8011F5C8)();
    tmp = r3 & 0xFFFF;
    do {
        if ((s32)tmp == 0x19a) break;
        if ((s32)tmp >= 0x19a) { r3 = 0x1; return; }
        if ((s32)tmp == 0x97) break;
        r3 = 0x1;
        return;
    } while (0);

    r3 = r31;
    ((void(*)(void))fn_8011E7A4)();
    tmp = r3 & 0xFF;
    if (tmp != 0) { r3 = 0x1; return; }
    r3 = 0x0;
    return;

    r3 = 0x1;

    return;
}

/* 0x80076054 | size: 0x2E0 */
void fn_80076054(void) {
    extern void fn_80076398();
    extern void __assert();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r27 = r3;
    r28 = r4;
    r3 = (u32)&lbl_80268940;
    r25 = 0x0;
    r31 = (u32)&lbl_80268940;
    do {
        r26 = 0x0;
        do {
            r3 = r27;
            r4 = r26 & 0xFFFF;
            ((void(*)(void))fn_8012AC08)();
            r4 = r25;
            fn_80076398();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = r25 << 1;
                r3 = r31 + 0xfc;
                r3 = *(u16*)(r3 + tmp);
                return;
            }
            r26 = r26 + 0x1;
        } while ((s32)r26 < 6);
        r25 = r25 + 0x1;
    } while (r25 < 6);
    if (r28 == 0) {
        r3 = 0x0;
        return;
    }
    r30 = 0x0;
    do {
        r29 = 0x0;
        do {
            r3 = r27;
            r4 = r29 & 0xFFFF;
            ((void(*)(void))fn_8012AC08)();
            r25 = r3;
            r26 = 0x0;
            if (r25 != 0) {
                r4 = 0x0;
                r5 = 0x6e;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                if ((s32)r3 == 0) {
                }
                r26 = 0x1;
                }
            do {
                if ((s32)r26 != 0) {
                    tmp = 0x1;
                    break;
                }
                if ((s32)r30 < 0x0 || (s32)r30 >= 0x3) {
                    r3 = r31 + 0x108;
                    r5 = r31 + 0x118;
                    r4 = 0xfb;
                    __assert();
                    tmp = 0x0;
                    break;
                }
                if ((s32)r30 == 1) {
                    r3 = r25;
                    ((void(*)(void))fn_8011F4A8)();
                    tmp = *(s16*)((u8*)r28 + 0x2);
                    r5 = r3 & 0xFF;
                    r3 = (u32)r5 >> 31;
                    r4 = (s32)tmp >> 31;
                    tmp = tmp - r5;
                    tmp = r4 + r3; /* +carry */;
                    tmp = tmp & 0xFF;
                    break;
                }
                if ((s32)r30 >= 0x2) {
                    r3 = r25;
                    ((void(*)(void))fn_8011F4A8)();
                    r5 = r3 & 0xFF;
                    tmp = *(s16*)((u8*)r28 + 0x0);
                    r4 = (s32)r5 >> 31;
                    r3 = (u32)tmp >> 31;
                    tmp = r5 - tmp;
                    tmp = r4 + r3; /* +carry */;
                    tmp = tmp & 0xFF;
                    break;
                }
                /* r30 == 0 */
                r3 = r25;
                ((void(*)(void))fn_8011F1A0)();
                r25 = r3;
                ((void(*)(void))fn_8006B420)();
                tmp = r25 & 0xFFFF;
                r26 = r3;
                do {
                    if ((s32)tmp == 0xaf) {
                        r3 = 0x0;
                        break;
                    }
                    if ((s32)tmp == 0) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r25;
                    ((void(*)(void))fn_80142984)();
                } while (0);

                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = 0x0;
                    break;
                }
                tmp = *(u32*)((u8*)r26 + 0x8);
                if ((s32)tmp < 0x0 || (s32)tmp >= 0x3) {
                    tmp = 0x0;
                    break;
                }
                if ((s32)tmp == 1) {
                    tmp = r25 & 0xFFFF;
                    tmp = __cntlzw(tmp);
                    tmp = (u32)tmp >> 5;
                    tmp = tmp & 0xFF;
                    break;
                }
                if ((s32)tmp >= 0x2) {
                    tmp = 0x1;
                    break;
                }
                /* tmp == 0 */
                r3 = (u32)&lbl_802EE458;
                tmp = *(u32*)&lbl_80478928;
                r5 = (u32)&lbl_802EE458;
                r4 = 0x0;
                r3 = r25 & 0xFFFF;
                ctr_fn = (void(*)(void))tmp;
                if (tmp > 0) {
                    do {
                        tmp = *(u16*)((u8*)r5 + 0x0);
                        if (r3 == tmp) {
                            tmp = r4 + 0x18;
                            tmp = *(u8*)(r26 + tmp);
                            tmp = __cntlzw(tmp);
                            tmp = (u32)tmp >> 5;
                            tmp = tmp & 0xFF;
                            break;
                        }
                        r5 = r5 + 0x2;
                        r4 = r4 + 0x1;
                    } while (--ctr != 0);
                }
                tmp = 0x1;
            } while (0);

            tmp = tmp & 0xFF;
            if (tmp == 0) {
                tmp = r30 << 1;
                r3 = (u32)&lbl_8047C0D0;
                r3 = *(u16*)(r3 + tmp);
                return;
            }
            r29 = r29 + 0x1;
        } while ((s32)r29 < 6);
        r30 = r30 + 0x1;
    } while (r30 < 3);
    r25 = 0x0;
    do {
        r3 = r27;
        r4 = r28;
        r5 = r25;
        ((void(*)(void))fn_80076F2C)();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = r25 << 1;
            r3 = (u32)&lbl_8047C0D8;
            r3 = *(u16*)(r3 + tmp);
            return;
        }
        r25 = r25 + 0x1;
    } while (r25 < 4);
    r3 = 0x0;

    return;
}

/* 0x80076334 | size: 0x64 */
void fn_80076334(void) {
    extern void fn_80076398();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = 0x0;
    do {
        r3 = r30;
        r4 = r31;
        fn_80076398();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r3 = 0x0;
            return;
        }
        r31 = r31 + 0x1;
    } while ((s32)r31 < 6);
    r3 = 0x1;

    return;
}

/* 0x80076398 | size: 0x420 */
void fn_80076398(void) {
    extern void fn_80076398();
    extern void __assert();
    u8 sp[0x150];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r3;
    r28 = r4;
    r4 = (u32)&lbl_80268940;
    r29 = (u32)&lbl_80268940;
    r30 = 0x0;
    if (r31 != 0) {
        r4 = 0x0;
        r5 = 0x6e;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if ((s32)r3 == 0) {
        }
        r30 = 0x1;
        }
    if ((s32)r30 != 0) {
        r3 = 0x1;
        return;
    }
    if ((s32)r28 != 3) {
        if ((s32)r28 < 3) {
            if ((s32)r28 != 1) {
                if ((s32)r28 < 1) {
                    if ((s32)r28 < 0) {
                        { r3 = r29 + 0x108; r5 = r29 + 0x118; r4 = 0x274; __assert(); r3 = 0x0; return; }
                    }
                    if ((s32)r28 != 5) {
                        if ((s32)r28 >= 5) { r3 = r29 + 0x108; r5 = r29 + 0x118; r4 = 0x274; __assert(); r3 = 0x0; return; }
                        /* r28 == 0 case */
                        r29 = 0x0;
                        do {
                            r3 = r31;
                            r6 = r29 & 0xFFFF;
                            r4 = 0x0;
                            r5 = 0x7f;
                            ((void(*)(void))fn_8012640C)();
                            r28 = r3 & 0xFFFF;
                            if (r28 != 0) {
                                r3 = 0x0;
                                ((void(*)(void))fn_8011CA34)();
                                r30 = r3;
                                r3 = r28;
                                ((void(*)(void))fn_8011CA34)();
                                if (r3 == r30) {
                                    r3 = 0x0;
                                    return;
                            }
                            }
                            r29 = r29 + 0x1;
                        } while ((s32)r29 < 4);
                        r3 = 0x1;
                        return;
                        }
                    r3 = r31;
                    r4 = 0x2;
                    fn_80076398();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r3 = 0x1;
                        return;
                    }
                    r3 = r31;
                    ((void(*)(void))fn_8011F5C8)();
                    tmp = r3 & 0xFFFF;
                    do {
                        if ((s32)tmp == 0x19a) break;
                        if ((s32)tmp >= 0x19a) break;
                        if ((s32)tmp == 0x97) break;
                        break;
                    } while (0);

                    r3 = r31;
                    ((void(*)(void))fn_8011E7A4)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        tmp = 0x0;
                    } else {

                        tmp = 0x1;
                    }
                    tmp = tmp & 0xFF;
                    if (tmp == 0) {
                        r3 = 0x0;
                        return;
                    }
                    r4 = r31;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F5FC)();
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8012546C)();
                    r3 = r31;
                    ((void(*)(void))fn_8011F15C)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F15C)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F130)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F130)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F104)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F104)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F0D8)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F0D8)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F0AC)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F0AC)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F080)();
                    r30 = r3 & 0xFFFF;
                    r3 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8011F080)();
                    tmp = r3 & 0xFFFF;
                    if (tmp < r30) {

                        r3 = 0x0;
                        return;
                    }
                    r3 = r31;
                    ((void(*)(void))fn_8011F054)();
                    r29 = r3 & 0xFFFF;
                    r3 = r31;
                    ((void(*)(void))fn_8011F028)();
                    tmp = r3 & 0xFFFF;
                    r3 = r31;
                    r29 = r29 + tmp;
                    ((void(*)(void))fn_8011EFFC)();
                    tmp = r3 & 0xFFFF;
                    r3 = r31;
                    r29 = r29 + tmp;
                    ((void(*)(void))fn_8011EFD0)();
                    tmp = r3 & 0xFFFF;
                    r3 = r31;
                    r29 = r29 + tmp;
                    ((void(*)(void))fn_8011EFA4)();
                    tmp = r3 & 0xFFFF;
                    r3 = r31;
                    r29 = r29 + tmp;
                    ((void(*)(void))fn_8011EF78)();
                    tmp = r3 & 0xFFFF;
                    r29 = r29 + tmp;
                    if (r29 > 0x1fe) {
                        r3 = 0x0;
                        return;
                    }
                    r3 = r31;
                    ((void(*)(void))fn_8011F054)();
                    tmp = r3 & 0xFFFF;
                    if (tmp > 0xff) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011F028)();
                    tmp = r3 & 0xFFFF;
                    if (tmp > 0xff) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011EFFC)();
                    tmp = r3 & 0xFFFF;
                    if (tmp > 0xff) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011EFD0)();
                    tmp = r3 & 0xFFFF;
                    if (tmp > 0xff) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011EFA4)();
                    tmp = r3 & 0xFFFF;
                    if (tmp > 0xff) { r3 = 0x0; return; }
                    r3 = r31;
                    ((void(*)(void))fn_8011EF78)();
                    tmp = r3 & 0xFFFF;
                    if (tmp <= 0xff) { r3 = 0x1; return; }

                    r3 = 0x0;
                    return;

                    r3 = 0x1;
                    return;
                    }
                r3 = r31;
                ((void(*)(void))fn_8011FC74)();
                tmp = r3 & 0xFF;
                tmp = __cntlzw(tmp);
                tmp = (u32)tmp >> 5;
                r3 = tmp & 0xFF;
                return;
                    }
            r3 = r31;
            ((void(*)(void))fn_8011E8DC)();
            tmp = r3 & 0xFF;
            tmp = __cntlzw(tmp);
            tmp = (u32)tmp >> 5;
            r3 = tmp & 0xFF;
            return;
        }
        r3 = r31;
        ((void(*)(void))fn_8011F1A0)();
        tmp = r3 & 0xFFFF;
        if ((s32)tmp == 0xaf) { r3 = 0x0; return; }
        if ((s32)tmp < 0xaf) {
            if ((s32)tmp == 0) { r3 = 0x1; return; }
            ((void(*)(void))fn_80142984)();
            return;

            r3 = 0x1;
            return;

            r3 = 0x0;
            return;
        }

                    }
    r3 = r31;
    r4 = 0x0;
    r5 = 0x6e;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    tmp = r3 & 0xFFFF;
    if (tmp == 0) {
        r3 = r29 + 0x108;
        r5 = r29 + 0x14c;
        r4 = 0x25e;
        __assert();
    }
    r3 = r31;
    ((void(*)(void))fn_80123FBC)();
    return;
    r3 = r29 + 0x108;
    r5 = r29 + 0x118;
    r4 = 0x274;
    __assert();
    r3 = 0x0;

    return;
}
