/**
 * @file gba_conv.c
 * @brief GBA data conversion and validation (0x80083AF4-0x80089048)
 *
 * Address range: 0x80083AF4 - 0x80089048
 * Total functions: 28
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8001E074();
extern void fn_8005CF2C();
extern void fn_8006A76C();
extern void fn_8006A79C();
extern void fn_8006A7AC();
extern void fn_8006A7BC();
extern void fn_8006ADB4();
extern void fn_8006ADEC();
extern void fn_8006AE18();
extern void fn_8007109C();
extern void fn_80071104();
extern void fn_800776E4();
extern void fn_8008ABA0();
extern void fn_80092E38();
extern void fn_80092FC8();
extern void fn_80093160();
extern void fn_800932F0();
extern void fn_800934E4();
extern void fn_80093610();
extern void fn_80093698();
extern void fn_800D0F44();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void fn_800EC960();
extern void fn_800EC990();
extern void fn_800EC9DC();
extern void fn_800ECA78();
extern void fn_800ECB74();
extern void fn_800ECCA8();
extern void _threadSwitch();
extern void fn_800F92D4();
extern void fn_800F9AEC();
extern void fn_800FA280();
extern void fn_800FF56C();
extern void fn_800FF660();
extern void fn_800FF730();
extern void fn_80102510();
extern void fn_80102568();
extern void fn_80102620();
extern void fn_8010264C();
extern void fn_80103CB0();
extern void fn_80103CC0();
extern void fn_801040A0();
extern void fn_801046C8();
extern void fn_80104704();
extern void fn_801054B8();
extern void fn_80105624();
extern void fn_80106934();
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_801081F8();
extern void fn_80109220();
extern void fn_8011394C();
extern void fn_8011C7C0();
extern void fn_8011CA34();
/* ... and 27 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478950;
extern u8 lbl_80478954;
extern u8 lbl_80478958;
extern u8 lbl_8047A660;
extern u8 lbl_8047A664;
extern u8 lbl_8047A668;
extern u8 lbl_8047A66C;
extern u8 lbl_8047C190;
extern u8 lbl_8047C198;
extern u8 lbl_8047C1A0;
extern u8 lbl_8047C1A8;
extern u8 lbl_8047C1AC;
extern u8 lbl_8047C1B0;
extern u8 lbl_8047C1B8;
extern u8 lbl_8047C1C0;
extern u8 lbl_8047C1C4;
extern u8 lbl_8047C1C8;
extern u8 lbl_8047C1CC;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEB78[];
extern u8 lbl_8026F2E8[];
extern u8 lbl_8026F488[];
extern u8 lbl_8026F4F8[];
extern u8 lbl_803FB2F8[];

/* ===== Forward declarations ===== */
void fn_80083AF4(void);
void fn_80083BF8(void);
void fn_80083CBC(void);
void fn_80083CFC(void);
void fn_80083D30(void);
void fn_80083ECC(void);
void fn_80084034(void);
void fn_80084038(void);
void fn_800849B4(void);
void fn_80084A8C(void);
void fn_80087AE8(void);
void fn_80087C64(void);
void fn_80088428(void);
void fn_800884BC(void);
void fn_800886D0(void);
s32 fn_80088964(void);
s32 fn_800889A4(void);
void fn_800889E4(void);
void fn_80088C60(void);
void fn_80088D84(void);
void fn_80088EA8(u8* p);
s32 fn_80088F58(void);
s32 fn_80088F74(void);
s32 fn_80088F88(void);
s32 fn_80088FA4(void);
s32 fn_80088FF8(s32 a, s32 b);
s32 fn_80089028(void);
void fn_80089030(u8 x);

/* ===== Function implementations ===== */


/* 0x80083AF4 | size: 0x104 */
void fn_80083AF4(void) {
    extern void fn_80129280();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r31 = 0;

    r31 = r4;
    if (r3 != 0x0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        fn_80129280();
    }
    r0 = (u32)sp + 0x8;
    r6 = r3 + 0x4000;
    if (r0 != 0x0) {
        r0 = 0x0;
        *(u32*)(sp + 0x8) = r0;
    }
    r7 = 0x0;
while (1) {
        r0 = r3 + 0x24;
        if (r6 >= r0) {
            r0 = *(u16*)((u8*)r3 + 0x0);
            if (r0 != 0x0) {
                r0 = *(u8*)((u8*)r3 + 0x1B);
                r0 = (s8)r0;
                if ((s32)r0 <= 0x3) {
                    r0 = *(u8*)((u8*)r3 + 0x1C);
                    r0 = (s8)r0;
                    if ((s32)r0 <= 0x6) {
                        r0 = *(u8*)((u8*)r3 + 0x1D);
                        r0 = (s8)r0;
                        if ((s32)r0 > 0x5) {
                }
                    }
                    r0 = 0x0;
                    *(u16*)((u8*)r3 + 0x0) = r0;
            }
            }
            break;
                        }
        if ((s32)r7 == (s32)r31) {
        }
        r4 = *(u8*)((u8*)r3 + 0x1C);
        r7 = r7 + 0x1;
        r0 = *(u8*)((u8*)r3 + 0x1D);
        r4 = (s8)r4;
        r5 = *(u8*)((u8*)r3 + 0x1B);
        r0 = (s8)r0;
        r0 = r4 * r0;
        r5 = (s8)r5;
        r4 = r0 << 4;
        r0 = r4 + 0x76;
        r0 = r5 * r0;
        r3 = r0 + r3;
        r3 = r3 + 0x24;
}

    if ((s32)r31 < 0x0) {
    }
    return;
}

/* 0x80083BF8 | size: 0xC4 */
void fn_80083BF8(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;

    if (r3 != 0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        fn_80129280();
    }
    r6 = r3 + 0x4000;
    r7 = 0x0;
while (1) {
        tmp = r3 + 0x24;
        if (r6 >= tmp) {
            tmp = *(u16*)((u8*)r3 + 0x0);
            if (tmp != 0) {
                tmp = *(u8*)((u8*)r3 + 0x1B);
                r5 = (s8)tmp;
                if ((s32)r5 <= 3) {
                    tmp = *(u8*)((u8*)r3 + 0x1C);
                    r4 = (s8)tmp;
                    if ((s32)r4 <= 6) {
                        tmp = *(u8*)((u8*)r3 + 0x1D);
                        tmp = (s8)tmp;
                        if ((s32)tmp > 5) {
                }
                    }
                    tmp = 0x0;
                    *(u16*)((u8*)r3 + 0x0) = tmp;
            }
            }
            break;
                        }
        tmp = r4 * tmp;
        r7 = r7 + 0x1;
        r4 = tmp << 4;
        tmp = r4 + 0x76;
        tmp = r5 * tmp;
        r3 = tmp + r3;
        r3 = r3 + 0x24;
}

    tmp = (u32)sp + 0x8;
    if (tmp != 0) {
    }
    return;
}

/* 0x80083CBC | size: 0x40 */
void fn_80083CBC(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    if (r3 != 0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        fn_80129280();
    }
    r4 = 0x0;
    r5 = 0x49cc;
    memset((void*)r3, (int)r4, (u32)r5);
    return;
}

/* 0x80083CFC | size: 0x34 */
void fn_80083CFC(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    if (r3 != 0) {
    } else {

        r3 = 0x0;
        r4 = 0xd;
        fn_80129280();
    }
    return;
}

/* 0x80083D30 | size: 0x19C */
void fn_80083D30(void) {
    extern void fn_8011F228();
    extern void fn_8012AC08();
    extern void fn_80135938();
    u8 sp[0x170];
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

    r27 = r3;
    r28 = r4;
    r31 = 0x0;
    do {
        r3 = r27;
        r4 = r31 & 0xFFFF;
        fn_8012AC08();
        r30 = 0x0;
        r29 = r3;
        do {
            r3 = r29;
            r4 = r30 & 0xFFFF;
            fn_8011F228();
            ((void(*)(void))fn_8011CA34)();
            r26 = r3;
            r3 = r28;
            r4 = 0x0;
            r5 = 0x50;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r26 != 0) {
                r3 = r26;
                ((void(*)(void))fn_8011C7C0)();
                ((void(*)(void))fn_800FA280)();
                r4 = 0x0;
                r26 = r3;
                r3 = (u32)sp + 0xac;
                while (1) {

                    if ((s32)r4 < 0x50) {
                        tmp = r4;
                        r4 = r4 + 0x1;
                        tmp = tmp << 1;
                        *(u16*)(r3 + tmp) = r5;
                    }
                    r26 = r26 + 0x2;

                    r5 = *(u16*)((u8*)r26 + 0x0);
                    if (r5 != 0) {
                        if (r5 == 0xffff) break;
                }
                }
                tmp = r4 << 1;
                r4 = (u32)sp + 0xac;
                r5 = 0x0;
                r3 = 0x0;
                *(u16*)(r4 + tmp) = r5;
                r4 = 0x5;
                fn_80135938();
                r5 = r3;
                r3 = r28;
                r4 = (u32)sp + 0xac;
                ((void(*)(void))fn_800F9AEC)();
                tmp = *(u16*)((u8*)r26 + 0x0);
                r25 = r28 + r3;
                if (tmp == 0xffff) {
                    tmp = 0xfe;
                    r6 = r26 + 0x3;
                    *(u8*)((u8*)r25 + 0x0) = tmp;
                    r25 = r25 + 0x1;
                    r4 = 0x0;
                    r3 = (u32)sp + 0x8;
                    while (1) {

                        if ((s32)r4 < 0x50) {
                            tmp = r4;
                            r4 = r4 + 0x1;
                            tmp = tmp << 1;
                            *(u16*)(r3 + tmp) = r5;
                        }
                        r6 = r6 + 0x2;

                        r5 = *(u16*)((u8*)r6 + 0x0);
                        if (r5 != 0) {
                            if (r5 == 0xffff) break;
                    }
                    }
                    tmp = r4 << 1;
                    r4 = (u32)sp + 0x8;
                    r5 = 0x0;
                    r3 = 0x0;
                    *(u16*)(r4 + tmp) = r5;
                    r4 = 0x5;
                    fn_80135938();
                    r5 = r3;
                    r3 = r25;
                    r4 = (u32)sp + 0x8;
                    ((void(*)(void))fn_800F9AEC)();
                    r25 = r25 + r3;
                }
                tmp = 0xff;
                *(u8*)((u8*)r25 + 0x0) = tmp;
            }
            r28 = r28 + 0x50;
            r30 = r30 + 0x1;
        } while ((s32)r30 < 4);
        r31 = r31 + 0x1;
    } while ((s32)r31 < 6);
    r3 = 0x0;
    return;
}

/* 0x80083ECC | size: 0x168 */
void fn_80083ECC(void) {
    extern void fn_80135938();
    u8 sp[0x160];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r3;
    r3 = r4;
    ((void(*)(void))fn_8011CA34)();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = 0x0;
    r5 = 0x50;
    memset((void*)r3, (int)r4, (u32)r5);
    if (r30 == 0) { r3 = 0x0; return; }
    r3 = r30;
    ((void(*)(void))fn_8011C7C0)();
    ((void(*)(void))fn_800FA280)();
    r4 = 0x0;
    r30 = r3;
    r3 = (u32)sp + 0xac;
    while (1) {

        if ((s32)r4 < 0x50) {
            tmp = r4;
            r4 = r4 + 0x1;
            tmp = tmp << 1;
            *(u16*)(r3 + tmp) = r5;
        }
        r30 = r30 + 0x2;

        r5 = *(u16*)((u8*)r30 + 0x0);
        if (r5 != 0) {
            if (r5 == 0xffff) break;
    }
    }
    tmp = r4 << 1;
    r4 = (u32)sp + 0xac;
    r5 = 0x0;
    r3 = 0x0;
    *(u16*)(r4 + tmp) = r5;
    r4 = 0x5;
    fn_80135938();
    r5 = r3;
    r3 = r31;
    r4 = (u32)sp + 0xac;
    ((void(*)(void))fn_800F9AEC)();
    tmp = *(u16*)((u8*)r30 + 0x0);
    r31 = r31 + r3;
    if (tmp == 0xffff) {
        tmp = 0xfe;
        r6 = r30 + 0x3;
        *(u8*)((u8*)r31 + 0x0) = tmp;
        r31 = r31 + 0x1;
        r4 = 0x0;
        r3 = (u32)sp + 0x8;
        while (1) {

            if ((s32)r4 < 0x50) {
                tmp = r4;
                r4 = r4 + 0x1;
                tmp = tmp << 1;
                *(u16*)(r3 + tmp) = r5;
            }
            r6 = r6 + 0x2;

            r5 = *(u16*)((u8*)r6 + 0x0);
            if (r5 != 0) {
                if (r5 == 0xffff) break;
        }
        }
        tmp = r4 << 1;
        r4 = (u32)sp + 0x8;
        r5 = 0x0;
        r3 = 0x0;
        *(u16*)(r4 + tmp) = r5;
        r4 = 0x5;
        fn_80135938();
        r5 = r3;
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_800F9AEC)();
        r31 = r31 + r3;
    }
    tmp = 0xff;
    *(u8*)((u8*)r31 + 0x0) = tmp;

    r3 = 0x0;
    return;
}

/* 0x80084034 | size: 0x4 */
void fn_80084034(void) {
}

/* 0x80084038 | size: 0x97C */
void fn_80084038(void) {
    extern void __assert();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    r29 = r3;
    r4 = (u32)&lbl_8026F2E8;
    r25 = (u32)&lbl_8026F2E8;
    if (r29 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
    }
    ((void(*)(void))fn_801040A0)();
    tmp = *(u8*)((u8*)r29 + 0x1);
    r26 = *(u32*)((u8*)r3 + 0x0);
    tmp = (s8)tmp;
    r30 = r26;
    if ((s32)tmp != 3) {
        if ((s32)tmp < 3) {
            if ((s32)tmp != 0) {
                goto L_80084458;
            }
            if ((s32)tmp != 5) {
                goto L_80084458;
                }
            tmp = *(u8*)((u8*)r29 + 0x2);
            tmp = (s8)tmp;
            if ((s32)tmp == 0) {
                r3 = 0xe0;
                r4 = 0x20;
                ((void(*)(void))fn_800E2C04)();
                r26 = r3;
                tmp = r26 & 0xFFFF;
                if (tmp == 0) {
                    r3 = r25 + 0x184;
                    r4 = 0xea;
                    r5 = (u32)&lbl_8047C198;
                    __assert();
                }
                r3 = r26;
                ((void(*)(void))fn_800E27B0)();
                r4 = 0x0;
                r26 = r3;
                r5 = 0xcc;
                memset((void*)r3, (int)r4, (u32)r5);
                r30 = r26;
                r3 = r29;
                ((void(*)(void))fn_801040A0)();
                *(u32*)((u8*)r3 + 0x0) = r26;
                r5 = 0x0;
                tmp = 0x1;
                r3 = r29;
                *(u32*)((u8*)r26 + 0x10) = r5;
                r4 = 0x10f6;
                *(u32*)((u8*)r26 + 0x0) = r5;
                *(u32*)((u8*)r26 + 0x14) = r5;
                *(u32*)((u8*)r26 + 0x4) = r5;
                *(u32*)((u8*)r26 + 0x18) = r5;
                *(u32*)((u8*)r26 + 0x8) = r5;
                *(u32*)((u8*)r26 + 0x1C) = r5;
                *(u32*)((u8*)r26 + 0xC) = r5;
                *(u8*)((u8*)r26 + 0x20) = tmp;
                *(u8*)((u8*)r26 + 0x21) = tmp;
                *(u32*)((u8*)r26 + 0x24) = r5;
                *(u32*)((u8*)r26 + 0x28) = r5;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x30) = r3;
                r3 = r29;
                r4 = 0x10f7;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x34) = r3;
                r3 = r29;
                r4 = 0x10d5;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x38) = r3;
                r3 = r29;
                r4 = 0x10da;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x3C) = r3;
                r3 = r29;
                r4 = 0x10e3;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x40) = r3;
                r3 = r29;
                r4 = 0x10f0;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x44) = r3;
                r3 = r29;
                r4 = 0x10f5;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x48) = r3;
                r3 = r29;
                r4 = 0x10d1;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x4C) = r3;
                r3 = r29;
                r4 = 0x10d6;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x5C) = r3;
                r3 = r29;
                r4 = 0x10db;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x6C) = r3;
                r3 = r29;
                r4 = 0x10df;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x7C) = r3;
                r3 = r29;
                r4 = 0x10e4;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x8C) = r3;
                r3 = r29;
                r4 = 0x10e8;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x9C) = r3;
                r3 = r29;
                r4 = 0x10ec;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xAC) = r3;
                r3 = r29;
                r4 = 0x10f1;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xBC) = r3;
                r3 = r29;
                r4 = 0x10d2;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x50) = r3;
                r3 = r29;
                r4 = 0x10d7;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x60) = r3;
                r3 = r29;
                r4 = 0x10dc;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x70) = r3;
                r3 = r29;
                r4 = 0x10e0;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x80) = r3;
                r3 = r29;
                r4 = 0x10e5;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x90) = r3;
                r3 = r29;
                r4 = 0x10e9;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xA0) = r3;
                r3 = r29;
                r4 = 0x10ed;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xB0) = r3;
                r3 = r29;
                r4 = 0x10f2;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xC0) = r3;
                r3 = r29;
                r4 = 0x10d3;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x54) = r3;
                r3 = r29;
                r4 = 0x10d8;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x64) = r3;
                r3 = r29;
                r4 = 0x10dd;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x74) = r3;
                r3 = r29;
                r4 = 0x10e1;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x84) = r3;
                r3 = r29;
                r4 = 0x10e6;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x94) = r3;
                r3 = r29;
                r4 = 0x10ea;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xA4) = r3;
                r3 = r29;
                r4 = 0x10ee;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xB4) = r3;
                r3 = r29;
                r4 = 0x10f3;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xC4) = r3;
                r3 = r29;
                r4 = 0x10d4;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x58) = r3;
                r3 = r29;
                r4 = 0x10d9;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x68) = r3;
                r3 = r29;
                r4 = 0x10de;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x78) = r3;
                r3 = r29;
                r4 = 0x10e2;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x88) = r3;
                r3 = r29;
                r4 = 0x10e7;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0x98) = r3;
                r3 = r29;
                r4 = 0x10eb;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xA8) = r3;
                r3 = r29;
                r4 = 0x10ef;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xB8) = r3;
                r3 = r29;
                r4 = 0x10f4;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r26 + 0xC8) = r3;
                r24 = r25 + 0x70;
                r26 = 0x0;
                do {
                    r4 = *(u16*)((u8*)r24 + 0x0);
                    r3 = r29;
                    r5 = *(u16*)((u8*)r24 + 0x2);
                    ((void(*)(void))fn_801081F8)();
                    r24 = r24 + 0x6;
                    r26 = r26 + 0x1;
                } while (r26 < 0x2e);
            }
            goto L_80084458;
        }
        tmp = *(u8*)((u8*)r29 + 0x2);
        tmp = (s8)tmp;
        if ((s32)tmp == 0) {
            r24 = r25 + 0x70;
            r27 = 0x0;
            do {
                r4 = *(u16*)((u8*)r24 + 0x0);
                r3 = r29;
                r5 = *(u16*)((u8*)r24 + 0x4);
                ((void(*)(void))fn_801081F8)();
                r24 = r24 + 0x6;
                r27 = r27 + 0x1;
            } while (r27 < 0x2e);
            tmp = 0x1;
            *(u8*)((u8*)r29 + 0x2) = tmp;
            *(u8*)((u8*)r26 + 0x20) = tmp;
        }
        goto L_80084458;
            }
    r3 = r26;
    ((void(*)(void))fn_800E202C)();
    r26 = r3;
    tmp = r26 & 0xFFFF;
    if (tmp == 0) {
        r3 = r25 + 0x184;
        r4 = 0xf3;
        r5 = (u32)&lbl_8047C198;
        __assert();
    }
    r3 = r26;
    ((void(*)(void))fn_800E24B0)();
    r3 = r26;
    ((void(*)(void))fn_800E209C)();
    return;
L_80084458:
    tmp = *(u8*)((u8*)r29 + 0x1);
    r27 = 0x0;
    tmp = (s8)tmp;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r30 + 0x20);
        if (tmp != 0) {
            tmp = 0x0;
            r27 = 0x1;
            *(u8*)((u8*)r30 + 0x20) = tmp;
    }
    }
    r24 = r30;
    r31 = 0x0;
    r23 = (u32)&lbl_80478950;
    r28 = 0x7;
    r26 = 0x8;
    do {
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 5) {
            if ((s32)tmp == 4) {
            }
            r3 = r31 + 0x1;
            ((void(*)(void))fn_8008ABA0)();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))fn_80103CB0)();
                tmp = *(u8*)((u8*)r23 + 0x0);
                tmp = r3 & ~tmp;
                r3 = tmp & 0xFF;
                ((void(*)(void))fn_80103CC0)();
                *(u32*)((u8*)r24 + 0x0) = r28;
                *(u32*)((u8*)r30 + 0x28) = r26;
            }
            }
        r24 = r24 + 0x4;
        r23 = r23 + 0x1;
        r31 = r31 + 0x1;
    } while ((s32)r31 <= 3);
    r3 = *(u32*)((u8*)r30 + 0x10);
    tmp = *(u32*)((u8*)r30 + 0x0);
    if ((s32)r3 != (s32)tmp) {
        r27 = 0x1;
    }
    tmp = *(u32*)((u8*)r30 + 0x0);
    *(u32*)((u8*)r30 + 0x10) = tmp;
    r3 = *(u32*)((u8*)r30 + 0x14);
    tmp = *(u32*)((u8*)r30 + 0x4);
    if ((s32)r3 != (s32)tmp) {
        r27 = 0x1;
    }
    tmp = *(u32*)((u8*)r30 + 0x4);
    *(u32*)((u8*)r30 + 0x14) = tmp;
    r3 = *(u32*)((u8*)r30 + 0x18);
    tmp = *(u32*)((u8*)r30 + 0x8);
    if ((s32)r3 != (s32)tmp) {
        r27 = 0x1;
    }
    tmp = *(u32*)((u8*)r30 + 0x8);
    *(u32*)((u8*)r30 + 0x18) = tmp;
    r3 = *(u32*)((u8*)r30 + 0x1C);
    tmp = *(u32*)((u8*)r30 + 0xC);
    if ((s32)r3 != (s32)tmp) {
        r27 = 0x1;
    }
    tmp = *(u32*)((u8*)r30 + 0xC);
    r26 = 0x4;
    *(u32*)((u8*)r30 + 0x1C) = tmp;
    tmp = r27 & 0xFF;
    if (tmp == 0) return;
    r3 = *(u32*)((u8*)r30 + 0x30);
    if (r3 != 0) {
        r4 = 0x1;
        ((void(*)(void))fn_80109220)();
    }
    r3 = *(u32*)((u8*)r30 + 0x34);
    if (r3 != 0) {
        r4 = 0x1;
        ((void(*)(void))fn_80109220)();
    }
    tmp = *(u32*)((u8*)r30 + 0x0);
    r4 = r25 + 0x0;
    r3 = *(u32*)((u8*)r30 + 0x38);
    tmp = tmp << 2;
    r27 = *(u32*)(r4 + tmp);
    if (r3 != 0) {
        r4 = r27 & 0x00000100;
        tmp = -r4;
        tmp = tmp | r4;
        r4 = (u32)tmp >> 31;
        ((void(*)(void))fn_80109220)();
    }
    r3 = *(u32*)((u8*)r30 + 0x3C);
    if (r3 != 0) {
        r4 = r27 & 0x00000200;
        tmp = -r4;
        tmp = tmp | r4;
        r4 = (u32)tmp >> 31;
        ((void(*)(void))fn_80109220)();
    }
    r3 = *(u32*)((u8*)r30 + 0x40);
    if (r3 != 0) {
        r4 = r27 & 0x00000400;
        tmp = -r4;
        tmp = tmp | r4;
        r4 = (u32)tmp >> 31;
        ((void(*)(void))fn_80109220)();
    }
    r3 = *(u32*)((u8*)r30 + 0x44);
    if (r3 != 0) {
        r4 = r27 & 0x00000800;
        tmp = -r4;
        tmp = tmp | r4;
        r4 = (u32)tmp >> 31;
        ((void(*)(void))fn_80109220)();
    }
    r3 = *(u32*)((u8*)r30 + 0x48);
    if (r3 != 0) {
        r4 = r27 & 0x00001000;
        tmp = -r4;
        tmp = tmp | r4;
        r4 = (u32)tmp >> 31;
        ((void(*)(void))fn_80109220)();
    }
    tmp = *(u8*)((u8*)r30 + 0x20);
    if (tmp == 0) {
        tmp = r26 << 2;
        r26 = *(u32*)((u8*)r30 + 0x38);
        tmp = *(u32*)(r30 + tmp);
        tmp = 0x9 - tmp;
        tmp = __cntlzw(tmp);
        tmp = (u32)tmp >> 5;
        r23 = tmp & 0xFF;
        if (r26 != 0) {
            if ((s32)r23 != 0) {
                tmp = *(s16*)((u8*)r26 + 0x6);
                r3 = r29;
                r5 = 0x1ba;
                r4 = tmp & 0xFFFF;
                ((void(*)(void))fn_801081F8)();
        }

            } else {
        tmp = *(s16*)((u8*)r26 + 0x6);
        r3 = r29;
        r5 = 0x0;
        r4 = tmp & 0xFFFF;
        ((void(*)(void))fn_801081F8)();
        tmp = -0x1;
        *(u32*)((u8*)r26 + 0x64) = tmp;
            }
        r26 = *(u32*)((u8*)r30 + 0x40);
        if (r26 != 0) {
            if ((s32)r23 != 0) {
                tmp = *(s16*)((u8*)r26 + 0x6);
                r3 = r29;
                r5 = 0x1ba;
                r4 = tmp & 0xFFFF;
                ((void(*)(void))fn_801081F8)();
        }
        }

            } else {
    tmp = *(s16*)((u8*)r26 + 0x6);
    r3 = r29;
    r5 = 0x0;
    r4 = tmp & 0xFFFF;
    ((void(*)(void))fn_801081F8)();
    tmp = -0x1;
    *(u32*)((u8*)r26 + 0x64) = tmp;
            }
    r31 = r30;
    r27 = 0x0;
    r26 = (u32)&lbl_8047C190;
    r25 = r25 + 0x0;
    do {
        tmp = *(u32*)((u8*)r31 + 0x0);
        r3 = *(u32*)((u8*)r31 + 0x4C);
        tmp = tmp << 2;
        r28 = *(u32*)(r25 + tmp);
        if (r3 != 0) {
            r4 = r28 & 0x1;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0x5C);
        if (r3 != 0) {
            r4 = r28 & 0x00000002;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0x6C);
        if (r3 != 0) {
            r4 = r28 & 0x00000004;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0x7C);
        if (r3 != 0) {
            r4 = r28 & 0x00000008;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0x8C);
        if (r3 != 0) {
            r4 = r28 & 0x00000010;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0x9C);
        if (r3 != 0) {
            r4 = r28 & 0x00000020;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0xAC);
        if (r3 != 0) {
            r4 = r28 & 0x00000040;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r31 + 0xBC);
        if (r3 != 0) {
            r4 = r28 & 0x00000080;
            tmp = -r4;
            tmp = tmp | r4;
            r4 = (u32)tmp >> 31;
            ((void(*)(void))fn_80109220)();
        }
        tmp = *(u32*)((u8*)r31 + 0x0);
        if ((s32)tmp != 0xb) {
            if ((s32)tmp >= 0xb || (s32)tmp >= 8) goto L_80084864;

            if ((s32)tmp < 6) {
                goto L_80084864;
            }
        }
        r3 = *(u32*)((u8*)r30 + 0x30);
        if (r3 != 0) {
            r4 = 0x0;
            ((void(*)(void))fn_80109220)();
        }
        r3 = *(u32*)((u8*)r30 + 0x34);
        if (r3 != 0) {
            r4 = 0x0;
            ((void(*)(void))fn_80109220)();
        }
    L_80084864:
        tmp = *(u8*)((u8*)r30 + 0x20);
        if (tmp == 0) {
            tmp = *(u32*)((u8*)r31 + 0x0);
            r24 = *(u32*)((u8*)r31 + 0x4C);
            tmp = 0x2 - tmp;
            tmp = __cntlzw(tmp);
            tmp = (u32)tmp >> 5;
            r23 = tmp & 0xFF;
            if (r24 != 0) {
                if ((s32)r23 != 0) {
                    tmp = *(s16*)((u8*)r24 + 0x6);
                    r3 = r29;
                    r5 = 0x1ba;
                    r4 = tmp & 0xFFFF;
                    ((void(*)(void))fn_801081F8)();
                }

                } else {
            tmp = *(s16*)((u8*)r24 + 0x6);
            r3 = r29;
            r5 = 0x0;
            r4 = tmp & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            tmp = -0x1;
            *(u32*)((u8*)r24 + 0x64) = tmp;
                }
            r24 = *(u32*)((u8*)r31 + 0x7C);
            if (r24 != 0) {
                if ((s32)r23 != 0) {
                    tmp = *(s16*)((u8*)r24 + 0x6);
                    r3 = r29;
                    r5 = 0x1ba;
                    r4 = tmp & 0xFFFF;
                    ((void(*)(void))fn_801081F8)();
                }

                } else {
            tmp = *(s16*)((u8*)r24 + 0x6);
            r3 = r29;
            r5 = 0x0;
            r4 = tmp & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            tmp = -0x1;
            *(u32*)((u8*)r24 + 0x64) = tmp;
                }
            r24 = *(u32*)((u8*)r31 + 0x8C);
            if (r24 != 0) {
                if ((s32)r23 != 0) {
                    tmp = *(s16*)((u8*)r24 + 0x6);
                    r3 = r29;
                    r5 = 0x1ba;
                    r4 = tmp & 0xFFFF;
                    ((void(*)(void))fn_801081F8)();
                }

                } else {
            tmp = *(s16*)((u8*)r24 + 0x6);
            r3 = r29;
            r5 = 0x0;
            r4 = tmp & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            tmp = -0x1;
            *(u32*)((u8*)r24 + 0x64) = tmp;
                }
            tmp = r28 & 0x00000004;
            if ((s32)tmp != 0) {
                r3 = *(u32*)((u8*)r31 + 0x6C);
                tmp = *(u32*)((u8*)r3 + 0xC);
                if (tmp == 0) {
                    tmp = *(s16*)((u8*)r3 + 0x6);
                    r3 = r29;
                    r5 = *(u16*)((u8*)r26 + 0x0);
                    r4 = tmp & 0xFFFF;
                    ((void(*)(void))fn_801081F8)();
            }
            }
        }
        r31 = r31 + 0x4;
        r26 = r26 + 0x2;
        r27 = r27 + 0x1;
    } while ((s32)r27 <= 3);

    return;
}

/* 0x800849B4 | size: 0xD8 */
void fn_800849B4(void) {
    extern void fn_80084A8C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r28 = r4;
    r29 = r5;
    r31 = r6;
    r3 = 0x1;
    ((void(*)(void))fn_80103CC0)();
    tmp = r3;
    r3 = r30;
    r30 = tmp;
    r4 = r28;
    r5 = r29;
    r6 = r31;
    fn_80084A8C();
    r31 = r3;
    r3 = 0x0;
    ((void(*)(void))fn_801069FC)();
    r3 = 0xe4;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0xe4;
        r4 = 0x0;
        r5 = 0x1;
        ((void(*)(void))fn_80102568)();
    }
    r3 = r30;
    ((void(*)(void))fn_80103CC0)();
    r30 = 0x0;
    do {
        r3 = r30;
        ((void(*)(void))fn_80093698)();
        r30 = r30 + 0x1;
    } while ((s32)r30 < 3);
    tmp = r31 & 0xFF;
    if (tmp != 0) {
        r3 = 0x0;
    } else {

        r3 = -0x1;
    }
    return;
}

/* 0x80084A8C | size: 0x305C */
void fn_80084A8C(void) {
    extern void fn_80087AE8();
    extern void fn_80128E04();
    extern void fn_80128E24();
    extern void fn_80129280();
    extern void fn_8012A248();
    extern void fn_8012AC64();
    extern void fn_80132A38();
    extern void fn_80135A70();
    extern void fn_80135B0C();
    extern void fn_80166A28();
    extern void __assert();
    extern u8 jumptable_802EEB78[];
    u8 sp[0xCD0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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
    f32 f4 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r3 = (u32)&lbl_8026F2E8;
    r15 = 0x0;
    tmp = (u32)&lbl_8026F2E8;
    *(u32*)(sp + 0xC34) = tmp;
    r3 = 0x1;
    ((void(*)(void))fn_80093698)();
    while (1) {
        r5 = 0x0;
        r4 = r3 + 0x190;
        r3 = 0x1;
        ((void(*)(void))fn_800932F0)();
        if ((s32)r3 != 0) break;
        ((void(*)(void))_threadSwitch)();


    }
    r3 = 0xe4;
    r4 = 0x0;
    ((void(*)(void))fn_8010264C)();
    r3 = 0xe4;
    ((void(*)(void))fn_80104704)();
    r16 = r3;
    if (r16 == 0) {
        r4 = 0x1f4;
        r5 = (u32)&lbl_8047C1A0;
        r3 = r3 + 0x184;
        __assert();
    }
    if (r16 == 0) {
        r3 = 0xa6;
        ((void(*)(void))fn_80104704)();
        r16 = r3;
    }
    r3 = r16;
    ((void(*)(void))fn_801040A0)();
    r5 = 0x0;
    r16 = *(u32*)((u8*)r3 + 0x0);
    r30 = (u32)&lbl_80478954;
    r4 = tmp << 4;
    *(u32*)((u8*)r16 + 0x24) = tmp;
    r29 = r3 + 0x30;
    r29 = r29 + r4;
    *(u32*)((u8*)r16 + 0x2C) = tmp;
    tmp = *(u8*)&lbl_80478954;
    *(u8*)((u8*)r16 + 0x21) = tmp;
    tmp = 0x5;
    r3 = *(u8*)((u8*)r30 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r30 + 0x1;
    r5 = 0x1;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r4 + 0x1;
    r5 = 0x2;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r4 + 0x1;
    r5 = 0x3;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r24 = r16;
    tmp = tmp & 0x00000010;
    if (tmp != 0) {
        if ((s32)tmp != 0) {
        }
        if ((s32)tmp == 2) {
            }
        r3 = 0x0;
        r4 = 0x2;
        fn_80129280();
        r4 = 0x0;
        *(u8*)((u8*)r16 + 0x21) = r4;
        r4 = 0x8;
        tmp = tmp & 0x00000002;
        r18 = r3;
        *(u32*)((u8*)r16 + 0x0) = r4;
        if (tmp != 0) {
            r3 = 0x2f;
            r4 = 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x3d88;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            f27 = *(f32*)&lbl_8047C1A8;
            f31 = *(f64*)&lbl_8047C1B0;
            r17 = 0x43300000;
            f29 = *(f64*)&lbl_8047C1B8;
            f28 = *(f32*)&lbl_8047C1AC;
            while (f27 < f28) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0xC14) = tmp;
                f30 = f0 - f31;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;

            }
            r3 = r18;
            ((void(*)(void))fn_800776E4)();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r3 = 0xe4;
                r4 = 0x0;
                r5 = 0x1;
                ((void(*)(void))fn_80102568)();
                r3 = r18;
                r4 = 0x0;
                ((void(*)(void))fn_8005CF2C)();
                r3 = 0x0;
                return;
        }
        }
        if (tmp != 0) {
            r3 = tmp;
            r3 = *(u32*)((u8*)r3 + 0x0);
            if (r3 != 0) {
                r4 = r18;
                fn_8012AC64();
        }
        }
        tmp = 0xa;
        *(u32*)((u8*)r16 + 0x0) = tmp;
        }
    r25 = 0x0;
    tmp = tmp & 0x00000040;
    *(u32*)(sp + 0xC2C) = tmp;
    tmp = tmp & 0x1;
    *(u32*)(sp + 0xC28) = tmp;
    tmp = tmp & 0x00000002;
    *(u32*)(sp + 0xC24) = tmp;
    tmp = tmp & 0x00000008;
    *(u32*)(sp + 0xC20) = tmp;
    r31 = tmp & 0x00000020;
    do {
        r23 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r24 + 0x21) = r23;
        if (tmp != 0) {
            tmp = (s8)r23;
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            if (tmp != 0) {
                r22 = tmp;
                goto L_80084E14;
            }
        }
        r22 = (u32)sp + 0xf4;
    L_80084E14:
        if (tmp != 0) {
            tmp = (s8)r23;
            if ((s32)tmp == 1) {
                goto L_80084E38;
            }
        }
        r21 = (u32)sp + 0x1c;
    L_80084E38:
        r28 = (s8)r23;
        r26 = r28 + 0x1;
        r27 = r28 << 2;
    L_80084E44:
        ((void(*)(void))fn_80103CB0)();
        r4 = (u32)&lbl_80478950;
        tmp = *(u8*)(r4 + r28);
        tmp = r3 & ~tmp;
        r3 = tmp & 0xFF;
        ((void(*)(void))fn_80103CC0)();
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp != 4) {
            tmp = 0x2;
            r4 = r26;
            *(u32*)(r27 + r24) = tmp;
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x3c42;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            r3 = r24;
            r4 = 0x6;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r15 = 0x0;
            L_80084EB4:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))fn_80103CB0)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))fn_80103CC0)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_80084EB4;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_80085010;
                }
                }
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_80085010:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
            }
        }
        r4 = r26;
        r3 = 0x2f;
        fn_80132A38();
        r3 = 0x7;
        r4 = 0x3c43;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        tmp = r15 & 0xFF;
        r3 = 0x3;
        *(u32*)(r27 + r24) = r3;
        if (tmp == 0) {
            tmp = 0x0;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_80085114;
        L_800850BC:
            r3 = 0x10c;
            ((void(*)(void))fn_80102620)();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                ((void(*)(void))_threadSwitch)();
                goto L_80085114;
            }
            ((void(*)(void))fn_80105624)();
            tmp = *(u16*)((u8*)r3 + 0x4);
            tmp = tmp & 0x00000020;
            if ((s32)tmp != 0) {
                tmp = 0x2;
                r3 = 0xe;
                *(u32*)((u8*)r24 + 0x28) = tmp;
                goto L_8008513C;
            }
            tmp = *(u32*)((u8*)r24 + 0x28);
            if (tmp == 8) {
                r3 = 0xe;
                goto L_8008513C;
            }
            ((void(*)(void))_threadSwitch)();
        L_80085114:
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_800934E4)();
            if ((s32)r3 == 0) goto L_800850BC;
            tmp = 0x0;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093610)();
        L_8008513C:
            if ((s32)r3 == 0xe) {
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r15 = 0x0;
            L_80085150:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))fn_80103CB0)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))fn_80103CC0)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_80085150;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_800852AC;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_800852AC:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
            }
            r15 = 0x1;
        }
        r3 = r28;
        r4 = 0x0;
        ((void(*)(void))fn_80093160)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_8008538C;
    L_80085334:
        r3 = 0x10c;
        ((void(*)(void))fn_80102620)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_8008538C;
        }
        ((void(*)(void))fn_80105624)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r3 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_800853B4;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r3 = 0xe;
            goto L_800853B4;
        }
        ((void(*)(void))_threadSwitch)();
    L_8008538C:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_80085334;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
    L_800853B4:
        if ((s32)r3 != 0xe) {
        do {
            if ((s32)r3 < 0xe) {
                if ((s32)r3 != 2) {
                    break;
                }
                r4 = 0x20000;
                tmp = r4 + 0x2;
                if ((s32)r3 == (s32)tmp) break;
                break;
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r15 = 0x0;
        L_800853EC:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800853EC;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085548;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085548:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        } while (0);
            r20 = 0x0;
        L_800855B8:
            r19 = 0x0;
            r17 = r24;
            r18 = r19;
            r16 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r17 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r18 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r16 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r17 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r17 + 0x0);
                if ((s32)tmp == 7) {
                    r19 = 0x1;
                }
                r17 = r17 + 0x4;
                r16 = r16 + 0x1;
                r18 = r18 + 0x1;
            } while ((s32)r18 <= 3);
            tmp = r19 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_800855B8;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085714;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085714:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x3c47;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
                }
        tmp = 0x4;
        r16 = 0x0;
        *(u32*)(r27 + r24) = tmp;
        do {
            r3 = r26;
            ((void(*)(void))fn_8008ABA0)();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                ((void(*)(void))fn_80103CB0)();
                r4 = (u32)&lbl_80478950;
                tmp = *(u8*)(r4 + r28);
                tmp = tmp | r3;
                r3 = tmp & 0xFF;
                ((void(*)(void))fn_80103CC0)();
                break;
            }
            ((void(*)(void))_threadSwitch)();
            r16 = r16 + 0x1;
        } while ((s32)r16 < 0x12c);

        if (tmp == 0) {
            r4 = r26;
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x3c4d;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            f27 = *(f32*)&lbl_8047C1A8;
            f28 = *(f64*)&lbl_8047C1B0;
            r16 = 0x43300000;
            f30 = *(f64*)&lbl_8047C1B8;
            f31 = *(f32*)&lbl_8047C1AC;
            while (f27 < f31) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0xC1C) = tmp;
                f29 = f0 - f28;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;

            }
        }
        r3 = r22;
        fn_8012A248();
        tmp = 0x0;
        r3 = r28;
        *(u32*)(sp + 0x18) = tmp;
        r4 = r22;
        r5 = (u32)sp + 0x18;
        ((void(*)(void))fn_80092FC8)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_80085934;
    L_800858DC:
        r3 = 0x10c;
        ((void(*)(void))fn_80102620)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_80085934;
        }
        ((void(*)(void))fn_80105624)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r16 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_80085960;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r16 = 0xe;
            goto L_80085960;
        }
        ((void(*)(void))_threadSwitch)();
    L_80085934:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_800858DC;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
        r16 = r3;
    L_80085960:
        if ((s32)r16 != 0xe) {
            goto L_80085B40;
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r15 = 0x0;
    L_80085978:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))fn_80103CB0)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))fn_80103CC0)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_80085978;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80085AD4;
            }
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        fn_80132A38();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80085AD4:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_80085B40:
        /* extrwi tmp, r3, 2, 22 */;
        if (tmp != 0) {
            r20 = 0x0;
        L_80085B54:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80085B54;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085CB0;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085CB0:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x3c49;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        /* extrwi tmp, r3, 4, 24 */;
        if (tmp <= 7) {
            r3 = (u32)jumptable_802EEB78;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEB78;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            r17 = 0x1;


        } else {
        tmp = 0x0;
        goto L_80085E1C;
        }
        fn_80128E24();
        if (r3 != 0) {
            fn_80128E04();
            if (r3 != 0) {
                fn_80135B0C();
                if (r3 != 0) {
                    fn_80135A70();
                    r3 = r3 & 0xFF;
                    tmp = r17 & 0xFF;
                    if (r3 == tmp) {
                        tmp = 0x1;
                        goto L_80085E1C;
            }
            }
            }
        }
        tmp = 0x0;
    L_80085E1C:
        tmp = tmp & 0xFF;
        if (tmp == 0) {
            r20 = 0x0;
        L_80085E2C:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80085E2C;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085F88;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085F88:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x44f0;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        if ((s32)r16 != 4) {

        } else {
        tmp = r3 & 0x00000002;
        if (tmp != 0) goto L_8008629C;
        }
        r20 = 0x0;
    L_80086074:
        r18 = 0x0;
        r16 = r24;
        r19 = r18;
        r17 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r19 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))fn_80103CB0)();
                    tmp = *(u8*)((u8*)r17 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))fn_80103CC0)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r16 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp == 7) {
                r18 = 0x1;
            }
            r16 = r16 + 0x4;
            r17 = r17 + 0x1;
            r19 = r19 + 0x1;
        } while ((s32)r19 <= 3);
        tmp = r18 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r20 = r20 + 0x1;
            if ((s32)r20 < 0xf) goto L_80086074;
        }
        r3 = 0x26;
        fn_80166A28();
        r16 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r16 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r16 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r16 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r16 = 0x4;
        }
        }
        }
        }
        if ((s32)r16 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_800861D0;
            }
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r4 = r16 + 0x1;
        r3 = 0x2f;
        fn_80132A38();
        if ((s32)r16 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_800861D0:
    do {
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            tmp = 0x0;
            break;
        }
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = 0x6;
        r3 = 0x2f;
        tmp = (s8)tmp;
        tmp = tmp << 2;
        *(u32*)(r24 + tmp) = r4;
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = (s8)tmp;
        r4 = r4 + 0x1;
        fn_80132A38();
        r3 = 0x7;
        r4 = 0x3c49;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        tmp = *(u32*)((u8*)r24 + 0x24);
        tmp = tmp & 0x00000008;
        if (tmp == 0) {
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();

        } else {
        r3 = r24;
        r4 = 0x7;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = 0x1;
            break;
        }
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        tmp = 0x0;
    } while (0);
        tmp = tmp & 0xFF;
        if (tmp != 0) goto L_80084E44;
        r3 = 0x0;
        return;
    L_8008629C:
        if (tmp != 0) {
            tmp = r3 & 0x00000004;
            r4 = 0x1;
            if (tmp != 0) {
                tmp = r3 & 0x00000008;
                if (tmp == 0) {
                    r4 = 0x0;
                }

            } else {
            tmp = r3 & 0x1;
            if (tmp == 0) {
                r4 = 0x0;
        }
            }
            tmp = r4 & 0xFF;
        }
        if (tmp == 0) {
            r20 = 0x0;
        L_800862EC:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_800862EC;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80086448;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80086448:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x44c3;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        do {
            if (tmp == 0) break;
            tmp = r3 & 0x1;
            if (tmp == 0) {
                r20 = 0x0;
            L_80086530:
                r18 = 0x0;
                r16 = r24;
                r19 = r18;
                r17 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r16 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r19 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))fn_80103CB0)();
                            tmp = *(u8*)((u8*)r17 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))fn_80103CC0)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r16 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r16 + 0x0);
                    if ((s32)tmp == 7) {
                        r18 = 0x1;
                    }
                    r16 = r16 + 0x4;
                    r17 = r17 + 0x1;
                    r19 = r19 + 0x1;
                } while ((s32)r19 <= 3);
                tmp = r18 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r20 = r20 + 0x1;
                    if ((s32)r20 < 0xf) goto L_80086530;
                }
                r3 = 0x26;
                fn_80166A28();
                r16 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r16 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r16 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r16 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r16 = 0x4;
                }
                }
                }
                }
                if ((s32)r16 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_8008668C;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r4 = r16 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                if ((s32)r16 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_8008668C:
            do {
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r24 + 0x24);
                    tmp = tmp & 0x00000008;
                    if (tmp != 0) {
                        tmp = 0x1;
                        break;
                    }
                    tmp = 0x0;
                    break;
                }
                tmp = *(u8*)((u8*)r24 + 0x21);
                r4 = 0x6;
                r3 = 0x2f;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r4;
                tmp = *(u8*)((u8*)r24 + 0x21);
                r4 = (s8)tmp;
                r4 = r4 + 0x1;
                fn_80132A38();
                r3 = 0x7;
                r4 = 0x44c3;
                r5 = 0x0;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = r24;
                    r4 = 0x1;
                    fn_80087AE8();

                } else {
                r3 = r24;
                r4 = 0x7;
                fn_80087AE8();
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                tmp = 0x0;
            } while (0);
                tmp = tmp & 0xFF;
                if (tmp != 0) goto L_80084E44;
                r3 = 0x0;
                return;
            }
            r3 = r22;
            ((void(*)(void))fn_800776E4)();
            tmp = r3 & 0xFF;
            if (tmp != 0) break;
            r3 = 0xe4;
            r4 = 0x0;
            r5 = 0x1;
            ((void(*)(void))fn_80102568)();
            r3 = r22;
            r4 = 0x1;
            ((void(*)(void))fn_8005CF2C)();
            if (tmp == 0) {
                r3 = 0x0;
                return;
            }
            r3 = 0xe4;
            r4 = 0x0;
            ((void(*)(void))fn_8010264C)();
            r3 = 0xe4;
            ((void(*)(void))fn_80104704)();
            r16 = r3;
            if (r16 == 0) {
                r4 = 0x1f4;
                r5 = (u32)&lbl_8047C1A0;
                r3 = r3 + 0x184;
                __assert();
            }
            if (r16 == 0) {
                r3 = 0xa6;
                ((void(*)(void))fn_80104704)();
                r16 = r3;
            }
            r3 = r16;
            ((void(*)(void))fn_801040A0)();
            r5 = *(u32*)((u8*)r3 + 0x0);
            r4 = 0x0;
            *(u32*)((u8*)r5 + 0x24) = tmp;
            *(u32*)((u8*)r5 + 0x2C) = tmp;
            tmp = *(u8*)((u8*)r3 + 0x0);
            *(u8*)((u8*)r5 + 0x21) = tmp;
            tmp = 0x5;
            r3 = *(u8*)((u8*)r30 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r30 + 0x1;
            r4 = 0x1;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r6 + 0x1;
            r4 = 0x2;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r6 + 0x1;
            r4 = 0x3;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            tmp = 0x6;
            r24 = r5;
            *(u32*)(r27 + r5) = tmp;
            goto L_80084E44;
        } while (0);
        if (tmp != 0) {
            tmp = (s8)r23;
            if ((s32)tmp != 1) {
            }
            if (r31 == 0) goto L_80086DBC;
            }
        r3 = r28;
        r4 = r21;
        ((void(*)(void))fn_80092E38)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_8008697C;
    L_80086924:
        r3 = 0x10c;
        ((void(*)(void))fn_80102620)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_8008697C;
        }
        ((void(*)(void))fn_80105624)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r3 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_800869A4;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r3 = 0xe;
            goto L_800869A4;
        }
        ((void(*)(void))_threadSwitch)();
    L_8008697C:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_80086924;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
    L_800869A4:
        if ((s32)r3 != 0xe) {
            if ((s32)r3 < 0xe) {
                if ((s32)r3 == 0xb) goto L_80086DBC;
            }
            goto L_80086B90;
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r15 = 0x0;
    L_800869C8:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))fn_80103CB0)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))fn_80103CC0)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_800869C8;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80086B24;
            }
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        fn_80132A38();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80086B24:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_80086B90:
        r20 = 0x0;
    L_80086B94:
        r18 = 0x0;
        r16 = r24;
        r19 = r18;
        r17 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r19 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))fn_80103CB0)();
                    tmp = *(u8*)((u8*)r17 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))fn_80103CC0)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r16 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp == 7) {
                r18 = 0x1;
            }
            r16 = r16 + 0x4;
            r17 = r17 + 0x1;
            r19 = r19 + 0x1;
        } while ((s32)r19 <= 3);
        tmp = r18 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r20 = r20 + 0x1;
            if ((s32)r20 < 0xf) goto L_80086B94;
        }
        r3 = 0x26;
        fn_80166A28();
        r16 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r16 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r16 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r16 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r16 = 0x4;
        }
        }
        }
        }
        if ((s32)r16 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80086CF0;
            }
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r4 = r16 + 0x1;
        r3 = 0x2f;
        fn_80132A38();
        if ((s32)r16 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80086CF0:
    do {
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            tmp = 0x0;
            break;
        }
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = 0x6;
        r3 = 0x2f;
        tmp = (s8)tmp;
        tmp = tmp << 2;
        *(u32*)(r24 + tmp) = r4;
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = (s8)tmp;
        r4 = r4 + 0x1;
        fn_80132A38();
        r3 = 0x7;
        r4 = 0x3c47;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        tmp = *(u32*)((u8*)r24 + 0x24);
        tmp = tmp & 0x00000008;
        if (tmp == 0) {
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();

        } else {
        r3 = r24;
        r4 = 0x7;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = 0x1;
            break;
        }
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        tmp = 0x0;
    } while (0);
        tmp = tmp & 0xFF;
        if (tmp != 0) goto L_80084E44;
        r3 = 0x0;
        return;
    L_80086DBC:
        if (r31 != 0) {
            tmp = *(u32*)((u8*)r21 + 0x8);
            tmp = tmp & 0x00000010;
        }
        if (tmp == 0) {
            r20 = 0x0;
        L_80086DD8:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80086DD8;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80086F34;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80086F34:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x4417;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x2f;
            r4 = 0x0;
            fn_80132A38();
            r3 = 0x7;
            r4 = 0x44cf;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();
            r3 = 0x0;
            return;
        }
    do {
        r3 = r28;
        ((void(*)(void))fn_80093698)();
        r3 = 0x3cc;
        fn_80166A28();
        tmp = 0x5;
        *(u32*)(r24 + r27) = tmp;
        ((void(*)(void))fn_80103CB0)();
        r4 = (u32)&lbl_80478950;
        tmp = *(u8*)(r4 + r28);
        tmp = tmp | r3;
        r3 = tmp & 0xFF;
        ((void(*)(void))fn_80103CC0)();
        r4 = r26;
        r3 = 0x2f;
        fn_80132A38();
        r3 = 0x7;
        r4 = 0x3c4b;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        if ((s32)tmp != 2) {
            if ((s32)tmp < 2) {
                if ((s32)tmp != 0) {
                    if ((s32)tmp < 0) {
                        break;
                    }
                    if ((s32)tmp >= 4) break;
                    goto L_8008769C;
                    }
                r3 = r24;
                r4 = 0x3;
                fn_80087AE8();
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    r3 = 0x1;
                    return;
                }
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r15 = 0x0;
            L_800870D4:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))fn_80103CB0)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))fn_80103CC0)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_800870D4;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_80087230;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))fn_80103CC0)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_80087230:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
                    }
            tmp = (s8)r23;
            if ((s32)tmp != 2) break;
            r3 = r24;
            r4 = 0x3;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = 0x1;
                return;
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r15 = 0x0;
        L_800872D4:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800872D4;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087430;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087430:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
        tmp = (s8)r23;
        if ((s32)tmp != 3) break;
        r3 = r24;
        r4 = 0x3;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0x1;
            return;
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r15 = 0x0;
    L_800874D4:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))fn_80103CB0)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))fn_80103CC0)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_800874D4;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80087630;
            }
        }
        r3 = 0x1;
        ((void(*)(void))fn_80103CC0)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        fn_80132A38();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80087630:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_8008769C:
        tmp = (s8)r23;
        if ((s32)tmp == 0) {
            r3 = r24;
            r4 = 0x3;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = 0x1;
                return;
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r15 = 0x0;
        L_800876D4:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800876D4;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087830;
            }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087830:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
    } while (0);
        r4 = 0x7;
        tmp = *(u8*)((u8*)r3 + 0x1);
        r3 = r24;
        *(u8*)((u8*)r24 + 0x21) = tmp;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r15 = 0x0;
        L_800878CC:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))fn_80103CB0)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))fn_80103CC0)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800878CC;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087A28;
                }
            }
            r3 = 0x1;
            ((void(*)(void))fn_80103CC0)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087A28:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
        r25 = r25 + 0x1;
        r4 = r4 + 0x1;
    } while (r25 < 4);

    return;
}

/* 0x80087AE8 | size: 0x17C */
void fn_80087AE8(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r26 = r4;
    tmp = 0x0;
    r31 = 0x1;
    *(u32*)((u8*)r30 + 0x28) = tmp;
    tmp = *(u8*)((u8*)r30 + 0x21);
    r3 = (s8)tmp;
    if ((s32)r3 >= 0) {
        ((void(*)(void))fn_800D0F44)();
        /* subis tmp, r3, 0x4 */;
        if (tmp == 0) {
            r31 = 0x0;
    }
    }
    r28 = r26 & 0x00000002;
    r27 = r26 & 0x1;
    r26 = r26 & 0x00000004;
L_80087B3C:
    r3 = 0x10c;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        ((void(*)(void))_threadSwitch)();
        goto L_80087B3C;
    }
    if ((s32)r28 != 0) {
        ((void(*)(void))fn_80105624)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r3 = 0x0;
            *(u32*)((u8*)r30 + 0x28) = tmp;
            return;
    }
    }
    tmp = *(u32*)((u8*)r30 + 0x28);
    if (tmp == 8) {
        r3 = 0x0;
        return;
    }
    if ((s32)r27 != 0) {
        ((void(*)(void))fn_80106934)();
        r29 = r3;
        tmp = (s8)r29;
        if ((s32)tmp == 0) {
            tmp = 0x1;
            r3 = 0x1;
            *(u32*)((u8*)r30 + 0x28) = tmp;
            return;
        }
        ((void(*)(void))fn_80105624)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000010;
        if ((s32)tmp != 0) {
            tmp = (s8)r29;
            if ((s32)tmp == (s32)-0x1) {
                tmp = 0x1;
                r3 = 0x1;
                *(u32*)((u8*)r30 + 0x28) = tmp;
                return;
    }
    }
    }
    if ((s32)r26 != 0) {
        r3 = *(u8*)((u8*)r30 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800D0F44)();
        if (r3 != 0x80) {
            tmp = r31 & 0xFF;
            if (tmp != 0) {
                /* subis tmp, r3, 0x4 */;
        }
        }
        if (tmp == 0) {
            tmp = 0x4;
            r3 = 0x1;
            *(u32*)((u8*)r30 + 0x28) = tmp;
            return;
                }
        /* subis tmp, r3, 0x4 */;
        if (tmp != 0) {
            r31 = 0x1;
        }
        }
    ((void(*)(void))_threadSwitch)();
    goto L_80087B3C;


}

/* 0x80087C64 | size: 0x7C4 */
void fn_80087C64(void) {
    extern void fn_801666BC();
    extern void fn_80166A28();
    u8 sp[0x1C0];
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
    f32 f4 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r25 = r3;
    r4 = (u32)&lbl_8026F488;
    r3 = 0x1;
    r30 = (u32)&lbl_8026F488;
    ((void(*)(void))fn_801054B8)();
    r29 = (u32)sp + 0x88;
    r28 = 0x1;
    r27 = 0x1;
    r26 = 0x0;
    tmp = 0x0;
    *(u32*)(sp + 0x88) = tmp;
    *(u32*)(sp + 0x8C) = tmp;
    *(u32*)(sp + 0x90) = tmp;
    *(u32*)(sp + 0x94) = tmp;
    *(u32*)(sp + 0x98) = tmp;
    *(u32*)(sp + 0x9C) = tmp;
    *(u32*)(sp + 0xA0) = tmp;
    *(u32*)(sp + 0xA4) = tmp;
    *(u32*)(sp + 0xA8) = tmp;
    while ((s32)r26 < 3) {

        r3 = 0x1;
        ((void(*)(void))fn_801054B8)();
        r31 = r3;
        r6 = 0x0;
        tmp = *(u16*)((u8*)r31 + 0x6);
        tmp = tmp & 0x00000010;
        if ((s32)tmp != 0) {
            if ((s32)r28 >= 0 && (s32)r28 < 3 && (s32)r27 >= 0) {


                if ((s32)r27 >= 3) {
                }
                tmp = 0x0;
                goto L_80087DD4;
                }
            r3 = r27 * 0xc;
            tmp = r28 << 2;
            r24 = (u32)sp + 0x88;
            r20 = r3 + tmp;
            tmp = *(u32*)(r24 + r20);
            if ((s32)tmp != 0) {
                tmp = 0x0;
                goto L_80087DD4;
            }
            tmp = 0x9;
            r5 = (u32)sp + 0xf0;
            r4 = r30 + 0x10;
            ctr_fn = (void(*)(void))tmp;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                tmp = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = tmp;
            } while (--ctr != 0);
            if ((s32)r28 >= 0) {
                if ((s32)r28 < 3) {
                    if ((s32)r27 >= 0) {
                        if ((s32)r27 < 3) {
                            r4 = r27 * 0x18;
                            tmp = r28 << 3;
                            r3 = (u32)sp + 0xf4;
                            r4 = r4 + tmp;
                            r4 = r3 + r4;
                            r19 = *(s16*)((u8*)r4 + 0x4);
                            r3 = *(u32*)((u8*)r4 + 0x0);
                            ((void(*)(void))fn_800F92D4)();
                            r21 = r3;
                            if (r21 != 0) {
                                if (r21 != 0) {
                                    r4 = (s16)r19;
                                    ((void(*)(void))fn_800ECCA8)();
                                    f1 = *(f32*)&lbl_8047C1CC;
                                    r3 = r21;
                                    ((void(*)(void))fn_800ECA78)();
                                    f1 = *(f32*)&lbl_8047C1C8;
                                    r3 = r21;
                                    ((void(*)(void))fn_800EC9DC)();
                                    r3 = r21;
                                    ((void(*)(void))fn_800EC990)();
            }
            }
            }
            }
            }
            }
            tmp = 0x1;
            *(u32*)(r24 + r20) = tmp;
        L_80087DD4:
            if ((s32)tmp != 0) {
                tmp = r26 << 3;
                r4 = (u32)sp + 0x70;
                r4 = r4 + tmp;
                r26 = r26 + 0x1;
                *(u32*)((u8*)r4 + 0x0) = r28;
                r3 = *(u32*)&lbl_8047C1C0;
                tmp = *(u32*)&lbl_8047C1C4;
                *(u32*)((u8*)r4 + 0x4) = r27;
                *(u32*)(sp + 0x1C) = tmp;
                if ((s32)r26 >= 0) {
                    if ((s32)r26 < 4) {
                        tmp = r26 << 1;
                        r4 = (u32)sp + 0x18;
                        r3 = 0x107E0000;
                        r19 = *(s16*)(r4 + tmp);
                        r3 = r3 + 0x100b;
                        ((void(*)(void))fn_800F92D4)();
                        r20 = r3;
                        if (r20 != 0) {
                            if (r20 != 0) {
                                r4 = (s16)r19;
                                ((void(*)(void))fn_800ECCA8)();
                                f1 = *(f32*)&lbl_8047C1CC;
                                r3 = r20;
                                ((void(*)(void))fn_800ECA78)();
                                f1 = *(f32*)&lbl_8047C1C8;
                                r3 = r20;
                                ((void(*)(void))fn_800EC9DC)();
                                r3 = r20;
                                ((void(*)(void))fn_800EC990)();
                }
                }
                }
                }
                if ((s32)r26 >= 3) {
                    r6 = *(u32*)((u8*)r30 + 0x0);
                    r7 = 0x0;
                    r5 = *(u32*)((u8*)r30 + 0x4);
                    r8 = r7;
                    r4 = *(u32*)((u8*)r30 + 0x8);
                    r12 = r29;
                    r3 = *(u32*)((u8*)r30 + 0xC);
                    r24 = (u32)sp + 0x5c;
                    tmp = *(u16*)((u8*)r30 + 0x10);
                    *(u16*)(sp + 0x30) = tmp;
                    tmp = tmp & 0xFFFF;
                    do {
                        r10 = r12;
                        r11 = r24;
                        r9 = 0x0;
                        *(u16*)(sp + 0x44) = tmp;
                        r20 = 0x3;
                        ctr_fn = (void(*)(void))r20;
                        do {
                            r19 = *(u32*)((u8*)r10 + 0x0);
                            if ((s32)r19 != 0) {
                                r19 = *(u16*)(sp + 0x44);
                                *(u16*)(sp + 0x6C) = r19;
                                if ((s32)r9 >= 0 && (s32)r9 < 3 && (s32)r8 >= 0) {


                                    if ((s32)r8 >= 3) {
                                    }
                                    r19 = 0x0;

                                    } else {
                                r19 = *(u16*)((u8*)r11 + 0x0);
                                    }
                                r20 = r19 & 0xFFFF;
                                r19 = *(u16*)((u8*)r25 + 0x0);
                                if (r20 == r19) {
                                    r7 = r7 + 0x1;
                            }
                                goto L_80087F6C;
                                }
                            r19 = *(u16*)((u8*)r25 + 0x2);
                            if (r20 == r19) {
                                r7 = r7 + 0x1;
                                goto L_80087F6C;
                            }
                            r19 = *(u16*)((u8*)r25 + 0x4);
                            if (r20 == r19) {
                                r7 = r7 + 0x1;
                        }
                        L_80087F6C:
                            r10 = r10 + 0x4;
                            r11 = r11 + 0x2;
                            r9 = r9 + 0x1;
                        } while (--ctr != 0);
                        r12 = r12 + 0xc;
                        r24 = r24 + 0x6;
                        r8 = r8 + 0x1;
                    } while ((s32)r8 < 3);
                    if ((s32)r7 < 3) {
                        r3 = 0x26;
                        fn_80166A28();
                        while (1) {
                            r3 = 0x26;
                            fn_801666BC();
                            if ((s32)r3 != 2) break;
                            ((void(*)(void))_threadSwitch)();


                        }
                        r21 = r29;
                        r20 = (u32)sp + 0xac;
                        r24 = 0x0;
                        do {
                            r23 = r21;
                            r22 = r20;
                            r26 = 0x0;
                            do {
                                tmp = *(u32*)((u8*)r23 + 0x0);
                                if ((s32)tmp != 0) {
                                    tmp = 0x9;
                                    r5 = (u32)sp + 0xa8;
                                    r4 = r30 + 0x10;
                                    ctr_fn = (void(*)(void))tmp;
                                    do {
                                        r3 = *(u32*)((u8*)r4 + 0x4);
                                        tmp = *(u32*)((u8*)r4 + 0x8);
                                        *(u32*)((u8*)r5 + 0x4) = r3;
                                        r5 += 8; *(u32*)r5 = tmp;
                                    } while (--ctr != 0);
                                    if ((s32)r26 >= 0) {
                                        if ((s32)r26 < 3) {
                                            if ((s32)r24 >= 0) {
                                                if ((s32)r24 < 3) {
                                                    r19 = *(s16*)((u8*)r22 + 0x6);
                                                    r3 = *(u32*)((u8*)r22 + 0x0);
                                                    ((void(*)(void))fn_800F92D4)();
                                                    r18 = r3;
                                                    if (r18 != 0) {
                                                        if (r18 != 0) {
                                                            r4 = (s16)r19;
                                                            ((void(*)(void))fn_800ECCA8)();
                                                            f1 = *(f32*)&lbl_8047C1CC;
                                                            r3 = r18;
                                                            ((void(*)(void))fn_800ECA78)();
                                                            f1 = *(f32*)&lbl_8047C1C8;
                                                            r3 = r18;
                                                            ((void(*)(void))fn_800EC9DC)();
                                                            r3 = r18;
                                                            ((void(*)(void))fn_800EC990)();
                                    }
                                    }
                                    }
                                    }
                                    }
                                    }
                                    tmp = 0x0;
                                    *(u32*)((u8*)r23 + 0x0) = tmp;
                                }
                                r23 = r23 + 0x4;
                                r22 = r22 + 0x8;
                                r26 = r26 + 0x1;
                            } while ((s32)r26 < 3);
                            r21 = r21 + 0xc;
                            r20 = r20 + 0x18;
                            r24 = r24 + 0x1;
                        } while ((s32)r24 < 3);
                        r4 = *(u32*)&lbl_8047C1C0;
                        r3 = 0x107E0000;
                        tmp = *(u32*)&lbl_8047C1C4;
                        r3 = r3 + 0x100b;
                        r26 = 0x0;
                        *(u32*)(sp + 0x14) = tmp;
                        r20 = *(s16*)((u8*)(u32)sp + 0x10);
                        ((void(*)(void))fn_800F92D4)();
                        r19 = r3;
                        if (r19 != 0 && r19 != 0) {

                            r4 = (s16)r20;
                            ((void(*)(void))fn_800ECCA8)();
                            f1 = *(f32*)&lbl_8047C1CC;
                            r3 = r19;
                            ((void(*)(void))fn_800ECA78)();
                            f1 = *(f32*)&lbl_8047C1C8;
                            r3 = r19;
                            ((void(*)(void))fn_800EC9DC)();
                            r3 = r19;
                            ((void(*)(void))fn_800EC990)();
                    }
                    }
                    goto L_80088120;
                    }
                r3 = 0x4a1;
                fn_80166A28();
                while (1) {
                    r3 = 0x4a1;
                    fn_801666BC();
                    if ((s32)r3 != 2) break;
                    ((void(*)(void))_threadSwitch)();


                }
                goto L_80088120;
                }
            r3 = 0x3c6;
            fn_80166A28();
        L_80088120:
            r6 = 0x1;
        }
        tmp = *(u16*)((u8*)r31 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0 && (s32)r6 == 0) {

            if ((s32)r26 < 0) break;
            r3 = 0x3c7;
            fn_80166A28();
            tmp = r26 << 3;
            r20 = (u32)sp + 0x70;
            r20 = r20 + tmp;
            tmp = 0x9;
            r5 = (u32)sp + 0x138;
            r4 = r30 + 0x10;
            r7 = *(u32*)((u8*)r20 + 0x4);
            r6 = *(u32*)((u8*)r20 + 0x0);
            ctr_fn = (void(*)(void))tmp;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                tmp = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = tmp;
            } while (--ctr != 0);
            if ((s32)r6 >= 0) {
                if ((s32)r6 < 3) {
                    if ((s32)r7 >= 0) {
                        if ((s32)r7 < 3) {
                            r4 = r7 * 0x18;
                            tmp = r6 << 3;
                            r3 = (u32)sp + 0x13c;
                            r4 = r4 + tmp;
                            r4 = r3 + r4;
                            r21 = *(s16*)((u8*)r4 + 0x6);
                            r3 = *(u32*)((u8*)r4 + 0x0);
                            ((void(*)(void))fn_800F92D4)();
                            r19 = r3;
                            if (r19 != 0) {
                                if (r19 != 0) {
                                    r4 = (s16)r21;
                                    ((void(*)(void))fn_800ECCA8)();
                                    f1 = *(f32*)&lbl_8047C1CC;
                                    r3 = r19;
                                    ((void(*)(void))fn_800ECA78)();
                                    f1 = *(f32*)&lbl_8047C1C8;
                                    r3 = r19;
                                    ((void(*)(void))fn_800EC9DC)();
                                    r3 = r19;
                                    ((void(*)(void))fn_800EC990)();
            }
            }
            }
            }
            }
            }
            r3 = *(u32*)&lbl_8047C1C0;
            tmp = *(u32*)&lbl_8047C1C4;
            *(u32*)(sp + 0xC) = tmp;
            if ((s32)r26 >= 0) {
                if ((s32)r26 < 4) {
                    tmp = r26 << 1;
                    r4 = (u32)sp + 0x8;
                    r3 = 0x107E0000;
                    r21 = *(s16*)(r4 + tmp);
                    r3 = r3 + 0x100b;
                    ((void(*)(void))fn_800F92D4)();
                    r19 = r3;
                    if (r19 != 0) {
                        if (r19 != 0) {
                            r4 = (s16)r21;
                            ((void(*)(void))fn_800ECCA8)();
                            f1 = *(f32*)&lbl_8047C1CC;
                            r3 = r19;
                            ((void(*)(void))fn_800ECA78)();
                            f1 = *(f32*)&lbl_8047C1C8;
                            r3 = r19;
                            ((void(*)(void))fn_800EC9DC)();
                            r3 = r19;
                            ((void(*)(void))fn_800EC990)();
            }
            }
            }
            }
            r4 = *(u32*)((u8*)r20 + 0x4);
            r3 = (u32)sp + 0x88;
            tmp = *(u32*)((u8*)r20 + 0x0);
            r5 = 0x0;
            r4 = r4 * 0xc;
            r6 = 0x1;
            tmp = tmp << 2;
            tmp = r4 + tmp;
            *(u32*)(r3 + tmp) = r5;
        }
        if ((s32)r6 == 0) {
            r4 = *(u16*)((u8*)r31 + 0x6);
            r31 = r28;
            r24 = r27;
            r3 = 0x0;
            tmp = r4 & 0x1;
            if (((s32)tmp != 0) && ((s32)r27 > 0)) {

                r3 = 0x1;
            }
            tmp = r4 & 0x00000002;
            if (((s32)tmp != 0) && ((s32)r24 < 2)) {

                r24 = r24 + 0x1;
                r3 = 0x1;
            }
            tmp = r4 & 0x00000004;
            if (((s32)tmp != 0) && ((s32)r28 > 0)) {

                r3 = 0x1;
            }
            tmp = r4 & 0x00000008;
            if (((s32)tmp != 0) && ((s32)r31 < 2)) {

                r31 = r31 + 0x1;
                r3 = 0x1;
            }
            if ((s32)r3 != 0) {
                r6 = *(u32*)((u8*)r30 + 0x5C);
                r5 = *(u32*)((u8*)r30 + 0x60);
                r4 = *(u32*)((u8*)r30 + 0x64);
                r3 = *(u32*)((u8*)r30 + 0x68);
                tmp = *(u16*)((u8*)r30 + 0x6C);
                *(u16*)(sp + 0x58) = tmp;
                if ((s32)r31 >= 0 && (s32)r31 < 3 && (s32)r24 >= 0 && (s32)r24 < 3) {


                    r5 = r24 * 0x6;
                    tmp = r31 << 1;
                    r3 = 0x107E0000;
                    r4 = (u32)sp + 0x48;
                    tmp = r5 + tmp;
                    r3 = r3 + 0x1009;
                    r20 = *(s16*)(r4 + tmp);
                    ((void(*)(void))fn_800F92D4)();
                    r18 = r3;
                    if (r18 != 0) {
                        if (r18 != 0) {
                            r4 = (s16)r20;
                            ((void(*)(void))fn_800ECCA8)();
                            f1 = *(f32*)&lbl_8047C1CC;
                            r3 = r18;
                            ((void(*)(void))fn_800ECA78)();
                            f1 = *(f32*)&lbl_8047C1C8;
                            r3 = r18;
                            ((void(*)(void))fn_800EC9DC)();
                            r3 = r18;
                            ((void(*)(void))fn_800EC990)();
                        }
                        r3 = r18;
                        r4 = 0x0;
                        ((void(*)(void))fn_800ECB74)();
                        while (1) {
                            r3 = r18;
                            ((void(*)(void))fn_800EC960)();
                            tmp = r3 & 0xFF;
                            if (tmp == 0) break;
                            ((void(*)(void))_threadSwitch)();


                        }
                }
                }
                r28 = r31;
                r27 = r24;
            }
        }
        ((void(*)(void))_threadSwitch)();

    }

    if ((s32)r26 < 0) {
        r3 = 0x1;
    } else {

        r3 = 0x0;
    }
    return;
}

/* 0x80088428 | size: 0x94 */
void fn_80088428(void) {
    #pragma peephole off
    extern u32 fn_80087C64(void* p);
    extern void fn_80166A28(s32);
    extern void cameraWaitSyncAnime(s32);
    extern void fn_80176E0C(s32, s32, s32, s32);
    extern void fn_800FF660(void);
    u32 r31;

    fn_80166A28(0x27);
    fn_80176E0C(0x62d, 0x107C1800, 0, 0);
    cameraWaitSyncAnime(1);
    r31 = fn_80087C64(&lbl_803FB2F8);
    fn_80166A28(0x28);
    fn_80176E0C(0x62d, 0x107D1800, 0, 0);
    cameraWaitSyncAnime(1);
    fn_800FF660();
    *(u32*)((u8*)&lbl_803FB2F8 + 0x8) = r31;
}

/* 0x800884BC | size: 0x214 */
void fn_800884BC(void) {
    u8 sp[0x70];
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
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r6 = (u32)&lbl_8026F488;
    r9 = (u32)&lbl_803FB2F8;
    r8 = (u32)&lbl_8026F488;
    r7 = 0x0;
    r29 = *(u32*)((u8*)r8 + 0x0);
    r6 = (u32)&lbl_803FB2F8;
    r30 = *(u32*)((u8*)r8 + 0x4);
    r31 = *(u32*)((u8*)r8 + 0x8);
    r12 = *(u32*)((u8*)r8 + 0xC);
    tmp = *(u16*)((u8*)r8 + 0x10);
    *(u16*)((u8*)r6 + 0x0) = r3;
    *(u16*)((u8*)r6 + 0x2) = r4;
    *(u16*)((u8*)r6 + 0x4) = r5;
    *(u16*)(sp + 0x18) = tmp;
    r11 = tmp & 0xFFFF;
    r4 = r11 & 0xFFFF;
    r10 = r29;
    r9 = r30;
    r8 = r31;
    r5 = r12;
    r3 = r4 & 0xFFFF;
    do {
    do {
        r26 = (u32)sp + 0x44;
        r27 = *(u16*)((u8*)r6 + 0x0);
        r28 = 0x0;
        *(u16*)(sp + 0x2C) = r11;
        tmp = 0x3;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = 0x0;
            *(u16*)(sp + 0x40) = r4;
            *(u16*)(sp + 0x54) = r3;
            if ((s32)tmp >= 0 && (s32)tmp < 3 && (s32)r28 >= 0) {


                if ((s32)r28 >= 3) {
                }
                tmp = 0x0;

                } else {
            tmp = *(u16*)((u8*)r26 + 0x0);
                }
            tmp = tmp & 0xFFFF;
            if (tmp == r27) {
                tmp = 0x1;
                break;
            }
            tmp = 0x1;
            *(u16*)(sp + 0x54) = r3;
            if ((s32)tmp >= 0 && (s32)tmp < 3 && (s32)r28 >= 0) {


                if ((s32)r28 >= 3) {
                }
                tmp = 0x0;

                } else {
            tmp = *(u16*)((u8*)r26 + 0x2);
                }
            tmp = tmp & 0xFFFF;
            if (tmp == r27) {
                tmp = 0x1;
                break;
            }
            tmp = 0x2;
            *(u16*)(sp + 0x54) = r3;
            if ((s32)tmp >= 0 && (s32)tmp < 3 && (s32)r28 >= 0) {


                if ((s32)r28 >= 3) {
                }
                tmp = 0x0;

                } else {
            tmp = *(u16*)((u8*)r26 + 0x4);
                }
            tmp = tmp & 0xFFFF;
            if (tmp == r27) {
                tmp = 0x1;
                break;
            }
            r26 = r26 + 0x6;
            r28 = r28 + 0x1;
        } while (--ctr != 0);
        tmp = 0x0;
    } while (0);
        if ((s32)tmp == 0) {
            r3 = 0x2;
            return;
        }
        r6 = r6 + 0x2;
        r7 = r7 + 0x1;
    } while ((s32)r7 < 3);
    r3 = 0x387;
    ((void(*)(void))fn_800FF730)();
    ((void(*)(void))_threadSwitch)();
    r3 = (u32)&lbl_803FB2F8;
    r3 = (u32)&lbl_803FB2F8;
    r3 = *(u32*)((u8*)r3 + 0x8);

    return;
}

/* 0x800886D0 | size: 0x294 */
void fn_800886D0(void) {
    extern void fn_80128E24();
    extern void fn_80129280();
    extern void fn_801293FC();
    extern void fn_801902E0();
    extern void fn_80190528();
    extern void fn_8019075C();
    extern void fn_801CADA8();
    extern void fn_801D0748();
    extern void fn_801EE398();
    extern void fn_80266320();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    ((void(*)(void))fn_8006A76C)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x0;
        return;
    }
    ((void(*)(void))fn_8006AE18)();
    if ((s32)r3 != 0) {
        r3 = 0x0;
        return;
    }
    fn_801EE398();
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x478;
    fn_801902E0();
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r3 = 0x0;
        return;
    }
    r3 = 0xafd;
    fn_801902E0();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20000;
    ((void(*)(void))fn_80071104)();
    r28 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r3;
    r3 = r28;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = 0x4;
    fn_801CADA8();
    tmp = (s16)r3;
    if ((s32)tmp != (s32)-0x1) {
        if ((s32)tmp < (s32)-0x1) {
            if ((s32)tmp < (s32)-0x2) {
            }
            goto L_800887C4;
        }
        r29 = 0x1;
        goto L_800887C8;
            }
    r29 = 0x2;
    goto L_800887C8;
L_800887C4:
    r29 = 0x0;
L_800887C8:
    r3 = r29;
    fn_80266320();
    if ((s32)r29 == 2) {
        fn_80128E24();
        r5 = 0x20000;
        r4 = r28;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r3 = r28;
        ((void(*)(void))fn_8007109C)();
        r3 = 0x0;
        return;
    }
    ((void(*)(void))fn_8006ADEC)();
    tmp = r3;
    r3 = 0x0;
    r29 = tmp;
    ((void(*)(void))fn_8006ADB4)();
    r4 = r29;
    r3 = 0x0;
    fn_801293FC();
    r3 = 0xafd;
    fn_80190528();
L_80088824:
    r3 = 0x2;
    r4 = 0x3c37;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x0;
    r4 = -0x1;
    r5 = -0x1;
    r6 = 0x0;
    ((void(*)(void))fn_8001E074)();
    tmp = (s8)r3;
    if ((s32)tmp == 0) {
        r30 = 0x0;
        r29 = r30;
        do {
            r31 = r30 & 0xFFFF;
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            tmp = r29 + 0x59aa;
            r4 = 0xe;
            *(u16*)(r3 + tmp) = r31;
            r3 = 0x0;
            fn_80129280();
            tmp = r29 + 0x26;
            r29 = r29 + 0x1660;
            *(u16*)(r3 + tmp) = r31;
            r30 = r30 + 0x1;
        } while (r30 < 4);
        r31 = 0x0;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        *(u8*)((u8*)r3 + 0x1C) = r31;
        r3 = 0x8ae;
        r4 = 0x0;
        fn_8019075C();
        r3 = 0x4;
        r4 = 0x2;
        r5 = 0x0;
        fn_801D0748();
        if (tmp > 1) {
        }
        r3 = 0x2;
        r4 = 0x3c0f;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x0;
        r4 = -0x1;
        r5 = -0x1;
        r6 = 0x1;
        ((void(*)(void))fn_8001E074)();
        tmp = (s8)r3;
        if ((s32)tmp != 0) goto L_80088824;
        fn_80128E24();
        r5 = 0x20000;
        r4 = r28;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r3 = 0x0;
        ((void(*)(void))fn_8006ADB4)();
        r3 = r28;
        ((void(*)(void))fn_8007109C)();
        r3 = 0x0;
        return;
        }
    r3 = r28;
    ((void(*)(void))fn_8007109C)();
    r3 = 0x1;

    return;
}

/* 0x80088964 | size: 0x40 */
s32 fn_80088964(void) {
    #pragma peephole off
    extern s32 fn_801D0748(s32, s32, s32);

    if (fn_801D0748(0xd, 2, 0) == 4) {
        return 0;
    } else {
        return -1;
    }
}

/* 0x800889A4 | size: 0x40 */
s32 fn_800889A4(void) {
    #pragma peephole off
    extern s32 fn_801D0748(s32, s32, s32);

    if (fn_801D0748(5, 2, 0) == 4) {
        return 0;
    } else {
        return -1;
    }
}

/* 0x800889E4 | size: 0x27C */
void fn_800889E4(void) {
    extern void fn_80128E24();
    extern void fn_80129280();
    extern void fn_8012D2BC();
    extern void fn_8012D32C();
    extern void fn_801906A0();
    extern void fn_8019075C();
    extern void __assert();
    extern void fn_801D0748();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    r29 = r3;
    r4 = (u32)&lbl_8026F4F8;
    r3 = 0x0;
    r30 = (u32)&lbl_8026F4F8;
    r4 = 0xe;
    fn_80129280();
    r31 = r3;
    r27 = r31 + (0x1 << 16);
    ((void(*)(void))fn_8006A7BC)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = r30 + 0x0;
        r5 = r30 + 0x14;
        r4 = 0x7b;
        __assert();
    }
    tmp = *(u32*)((u8*)r31 + 0x0);
    *(u32*)((u8*)r27 + (-13940)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0x4);
    *(u32*)((u8*)r27 + (-13936)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0x8);
    *(u32*)((u8*)r27 + (-13932)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0xC);
    *(u32*)((u8*)r27 + (-13928)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0x10);
    *(u32*)((u8*)r27 + (-13924)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0x14);
    *(u32*)((u8*)r27 + (-13920)) = tmp;
    tmp = *(u32*)((u8*)r31 + 0x18);
    *(u32*)((u8*)r27 + (-13916)) = tmp;
    if ((s32)r29 != 0) {
        tmp = *(u32*)((u8*)r27 + (-13940));
        if ((s32)tmp != 1) {
            r3 = r30 + 0x0;
            r5 = r30 + 0x38;
            r4 = 0x89;
            __assert();
        }
        r3 = (u32)sp + 0x8;
        fn_8012D32C();
        r3 = (u32)sp + 0x14;
        fn_8012D2BC();
        f0 = *(f32*)(sp + 0x8);
        *(f32*)((u8*)r28 + 0xC) = f0;
        f0 = *(f32*)(sp + 0xC);
        *(f32*)((u8*)r28 + 0x10) = f0;
        f0 = *(f32*)(sp + 0x10);
        *(f32*)((u8*)r28 + 0x14) = f0;
        f0 = *(f32*)(sp + 0x14);
        *(f32*)((u8*)r28 + 0x18) = f0;
        f0 = *(f32*)(sp + 0x18);
        *(f32*)((u8*)r28 + 0x1C) = f0;
        f0 = *(f32*)(sp + 0x1C);
        *(f32*)((u8*)r28 + 0x20) = f0;
        ((void(*)(void))fn_800FF56C)();
        *(u32*)((u8*)r28 + 0x4) = r3;
        ((void(*)(void))fn_8011394C)();
        *(u32*)((u8*)r28 + 0x8) = r3;
        tmp = 0x1;
        r3 = 0xafc;
        *(u8*)((u8*)r28 + 0x0) = tmp;
        fn_801906A0();
        *(u32*)((u8*)r28 + 0x24) = r3;
        r3 = 0xafd;
        fn_801906A0();
        *(u32*)((u8*)r28 + 0x28) = r3;
        r3 = 0xb11;
        fn_801906A0();
        *(u32*)((u8*)r28 + 0x2C) = r3;
        r3 = 0xde1;
        fn_801906A0();
        *(u32*)((u8*)r28 + 0x30) = r3;
        goto L_80088B48;
    }
    tmp = *(u32*)((u8*)r27 + (-13940));
    if ((s32)tmp == 1) {
        tmp = 0x1;
        *(u8*)((u8*)r28 + 0x0) = tmp;
        goto L_80088B48;
    }
    r3 = r28;
    r4 = 0x0;
    r5 = 0x34;
    memset((void*)r3, (int)r4, (u32)r5);
L_80088B48:
    r3 = r31;
    ((void(*)(void))fn_8006A7AC)();
    ((void(*)(void))fn_8006A76C)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r30 = 0x0;
        goto L_80088C38;
    }
    r3 = 0x20000;
    ((void(*)(void))fn_80071104)();
    r29 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r3;
    r3 = r29;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r28 = 0x0;
    r27 = r28;
    do {
        r30 = r28 & 0xFFFF;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        tmp = r27 + 0x59aa;
        r4 = 0xe;
        *(u16*)(r3 + tmp) = r30;
        r3 = 0x0;
        fn_80129280();
        tmp = r27 + 0x26;
        r27 = r27 + 0x1660;
        *(u16*)(r3 + tmp) = r30;
        r28 = r28 + 0x1;
    } while (r28 < 4);
    r30 = 0x0;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    *(u8*)((u8*)r3 + 0x1C) = r30;
    r3 = 0x8ae;
    r4 = 0x0;
    fn_8019075C();
    r3 = 0x6;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    r30 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r29;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = r29;
    ((void(*)(void))fn_8007109C)();
    if ((s32)r30 == 4) {
        r30 = 0x0;
        goto L_80088C38;
    }
    r30 = -0x1;
L_80088C38:
    if ((s32)r30 < 0) {
        r3 = r31;
        ((void(*)(void))fn_8006A79C)();
    }
    r3 = r30;
    return;
}

/* 0x80088C60 | size: 0x124 */
void fn_80088C60(void) {
    extern void fn_80128E24();
    extern void fn_80129280();
    extern void fn_8019075C();
    extern void fn_801D0748();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    ((void(*)(void))fn_8006A76C)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20000;
    ((void(*)(void))fn_80071104)();
    r28 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r3;
    r3 = r28;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r30 = 0x0;
    r29 = r30;
    do {
        r31 = r30 & 0xFFFF;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        tmp = r29 + 0x59aa;
        r4 = 0xe;
        *(u16*)(r3 + tmp) = r31;
        r3 = 0x0;
        fn_80129280();
        tmp = r29 + 0x26;
        r29 = r29 + 0x1660;
        *(u16*)(r3 + tmp) = r31;
        r30 = r30 + 0x1;
    } while (r30 < 4);
    r31 = 0x0;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    *(u8*)((u8*)r3 + 0x1C) = r31;
    r3 = 0x8ae;
    r4 = 0x0;
    fn_8019075C();
    r3 = 0x6;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    r31 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r28;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = r28;
    ((void(*)(void))fn_8007109C)();
    if ((s32)r31 == 4) {
        r3 = 0x0;
        return;
    }
    r3 = -0x1;

    return;
}

/* 0x80088D84 | size: 0x124 */
void fn_80088D84(void) {
    extern void fn_80128E24();
    extern void fn_80129280();
    extern void fn_8019075C();
    extern void fn_801D0748();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    ((void(*)(void))fn_8006A76C)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20000;
    ((void(*)(void))fn_80071104)();
    r30 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r3;
    r3 = r30;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r29 = 0x0;
    r28 = r29;
    do {
        r31 = r29 & 0xFFFF;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        tmp = r28 + 0x59aa;
        r4 = 0xe;
        *(u16*)(r3 + tmp) = r31;
        r3 = 0x0;
        fn_80129280();
        tmp = r28 + 0x26;
        r28 = r28 + 0x1660;
        *(u16*)(r3 + tmp) = r31;
        r29 = r29 + 0x1;
    } while (r29 < 4);
    r31 = 0x0;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    *(u8*)((u8*)r3 + 0x1C) = r31;
    r3 = 0x8ae;
    r4 = 0x0;
    fn_8019075C();
    r3 = 0x6;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    r31 = r3;
    fn_80128E24();
    r5 = 0x20000;
    r4 = r30;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = r30;
    ((void(*)(void))fn_8007109C)();
    if ((s32)r31 == 4) {
        r3 = 0x0;
        return;
    }
    r3 = -0x1;

    return;
}

/* 0x80088EA8 | size: 0xB0 */
void fn_80088EA8(u8* p) {
    extern void fn_8012D2BC(f32*);
    extern void fn_8012D32C(f32*);
    extern u32 fn_800FF56C(void);
    extern u32 fn_8011394C(void);
    extern u32 fn_801906A0(s32);
    f32 a[3];
    f32 b[3];

    fn_8012D32C(a);
    fn_8012D2BC(b);
    *(f32*)(p + 0xC) = a[0];
    *(f32*)(p + 0x10) = a[1];
    *(f32*)(p + 0x14) = a[2];
    *(f32*)(p + 0x18) = b[0];
    *(f32*)(p + 0x1C) = b[1];
    *(f32*)(p + 0x20) = b[2];
    *(u32*)(p + 0x4) = fn_800FF56C();
    *(u32*)(p + 0x8) = fn_8011394C();
    *(u8*)(p + 0x0) = 1;
    *(u32*)(p + 0x24) = fn_801906A0(0xafc);
    *(u32*)(p + 0x28) = fn_801906A0(0xafd);
    *(u32*)(p + 0x2C) = fn_801906A0(0xb11);
    *(u32*)(p + 0x30) = fn_801906A0(0xde1);
}

/* 0x80088F58 | size: 0x1C */
s32 fn_80088F58(void) {
    u32 count;

    count = *(volatile u32*)&lbl_8047A66C;
    count++;
    *(volatile u32*)&lbl_8047A66C = count;
    *(volatile u32*)&lbl_8047A668 = 0;
    return 0;
}

/* 0x80088F74 | size: 0x14 */
s32 fn_80088F74(void) {
    *(u32*)&lbl_8047A668 += 1;
    return 0;
}

/* 0x80088F88 | size: 0x1C */
s32 fn_80088F88(void) {
    u32 count;

    count = *(volatile u32*)&lbl_8047A664;
    count++;
    *(volatile u32*)&lbl_8047A664 = count;
    *(volatile u32*)&lbl_8047A660 = 0;
    return 0;
}

/* 0x80088FA4 | size: 0x54 */
s32 fn_80088FA4(void) {
    #pragma peephole off
    extern s32 fn_8010264C(s32, s32);
    extern void fn_80102510(s32);
    s32 r31;

    r31 = fn_8010264C(2, 1);
    fn_80102510(2);
    if (r31 >= 0) {
        *(u32*)&lbl_8047A660 += r31;
    }
    return 0;
}

/* 0x80088FF8 | size: 0x30 */
#pragma push
#pragma peephole off
s32 fn_80088FF8(s32 a, s32 b) {
    fn_80089030(b == 0);
    return 0;
}
#pragma pop

/* 0x80089028 | size: 0x8 */
s32 fn_80089028(void) {
    return lbl_80478958;
}

/* 0x80089030 | size: 0x18 */
#pragma push
#pragma peephole off
void fn_80089030(u8 x) {
    lbl_80478958 = (x == 0);
}
#pragma pop

