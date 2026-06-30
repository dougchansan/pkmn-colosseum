/**
 * @file menu_middle.c
 * @brief Menu middle code between battle and common (0x80069C0C-0x8007109C)
 *
 * Address range: 0x80069C0C - 0x8007109C
 * Total functions: 100
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8001DA60();
extern void fn_8005D830();
extern void fn_8005D858();
extern void fn_8005D8F8();
extern void fn_80071160();
extern void fn_80071208();
extern void fn_80071318();
extern void fn_8007162C();
extern void fn_80071644();
extern void fn_80076334();
extern void fn_80076398();
extern void fn_800767B8();
extern void fn_80076A8C();
extern void fn_80076F2C();
extern void fn_800772AC();
extern void fn_800774D4();
extern void fn_80077A5C();
extern void fn_80077BD0();
extern void fn_80077C1C();
extern void fn_80077C68();
extern void fn_80077D88();
extern void fn_80077DB8();
extern void fn_80077E50();
extern void fn_80088EA8();
extern void fn_800C8520();
extern void fn_800CE318();
extern void fn_800D5648();
extern void fn_800D5BA0();
extern void fn_800D61E4();
extern void fn_800D6728();
extern void fn_800D67BC();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D888C();
extern void fn_800D88DC();
extern void fn_800E0C54();
extern void _threadSwitch();
extern void fn_800F9D04();
extern void fn_800FA280();
extern void fn_800FA444();
extern void fn_800FB680();
extern void fn_800FE35C();
extern void fn_800FE38C();
extern void fn_800FF730();
extern void fn_80102138();
extern void fn_801022B8();
extern void fn_80102398();
extern void fn_80102868();
extern void fn_80102ED4();
extern void fn_80102F38();
extern void fn_801040D0();
extern void fn_801044D0();
extern void fn_801046B8();
extern void fn_801046C8();
extern void fn_80104704();
extern void fn_80104CA0();
extern void fn_80105624();
extern void fn_80107F38();
extern void fn_801081F8();
extern void fn_80108518();
/* ... and 50 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478938;
extern u8 lbl_80478F20;
extern u8 lbl_8047A5A4;
extern u8 lbl_8047A5D8;
extern u8 lbl_8047A5E0;
extern u8 lbl_8047A5E8;
extern u8 lbl_8047A5EC;
extern u8 lbl_8047A5F0;
extern u8 lbl_8047A5F4;
extern u8 lbl_8047A5F8;
extern u8 lbl_8047A5FC;
extern u8 lbl_8047C028;
extern u8 lbl_8047C030;
extern u8 lbl_8047C038;
extern u8 lbl_8047C040;
extern u8 lbl_8047C048;
extern u8 lbl_8047C050;
extern u8 lbl_8047C058;
extern u8 lbl_8047C060;
extern u8 lbl_8047C064;
extern u8 lbl_8047C068;
extern u8 lbl_8047C070;
extern u8 lbl_8047C078;
extern u8 lbl_8047C080;
extern u8 lbl_8047C088;
extern u8 lbl_8047C08C;
extern u8 lbl_8047E708;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EDE78[];
extern u8 jumptable_802EDEFC[];
extern u8 jumptable_802EDF20[];
extern u8 jumptable_802EDFB0[];
extern u8 jumptable_802EDFCC[];
extern u8 jumptable_802EE06C[];
extern u8 jumptable_802EE0F0[];
extern u8 jumptable_802EE20C[];
extern u8 jumptable_802EE31C[];
extern u8 lbl_80267C18[];
extern u8 lbl_80267DD8[];
extern u8 lbl_80267DE8[];
extern u8 lbl_80267E70[];
extern u8 lbl_80267EA8[];
extern u8 lbl_80267F68[];
extern u8 lbl_80267FE8[];
extern u8 lbl_80268184[];
extern u8 lbl_802681B4[];
extern u8 lbl_80268234[];
extern u8 lbl_80268424[];
extern u8 lbl_80268560[];
extern u8 lbl_80268574[];
extern u8 lbl_8026858C[];
extern u8 lbl_8026860C[];
extern u8 lbl_8026864C[];
extern u8 lbl_80268674[];
extern u8 lbl_80268680[];
extern u8 lbl_802686D0[];
extern u8 lbl_802EDE58[];
extern u8 lbl_802EE618[];
extern u8 lbl_80314E08[];
extern u8 lbl_803B6D68[];

/* ===== Forward declarations ===== */
void fn_80069C0C(void);
u16 fn_8006A65C(void);
void fn_8006A718(void);
u8 fn_8006A76C(void);
void fn_8006A79C(u8* p);
void fn_8006A7AC(u8* p);
u8 fn_8006A7BC(u8* p);
u32 fn_8006A7C8(u32 r3);
u16 fn_8006A7D0(u32 r3);
u16 fn_8006A7D8(u32 r3);
void fn_8006A7E0(u32 r3, u32 r4);
u32 fn_8006A7E8(u32 r3);
void fn_8006A7F0(void* dst, const void* src);
u32 fn_8006A814(u32 r3);
void fn_8006A81C(u32 r3, u32 r4);
void fn_8006A824(void);
void fn_8006A990(void);
void fn_8006AABC(void);
void fn_8006AC28(void* p, u16 value);
s32 fn_8006AC6C(u32 id);
void fn_8006ACCC(void);
void fn_8006ADB4(s32 value);
s32 fn_8006ADEC(void);
void fn_8006AE18(void);
u8* fn_8006AEEC(void);
void fn_8006AF44(void);
u8* fn_8006AFC4(u8* p);
void fn_8006AFE4(void);
void fn_8006B09C(void);
void fn_8006B0F8(void);
void fn_8006B154(void);
u32 fn_8006B1C0(s32 i);
void fn_8006B1D4(void);
u32 fn_8006B1F4(s32 index, s32 slot);
void fn_8006B2A4(void);
void fn_8006B354(s32 index);
u32 fn_8006B3C8(s32 index);
void fn_8006B420(void);
void fn_8006B4AC(void);
void fn_8006B51C(void);
s32 fn_8006B57C(void);
s32 fn_8006B5A8(void);
void fn_8006B5D0(void);
void fn_8006B6B4(void);
u8 fn_8006B8E8(void);
void fn_8006B8F0(void);
void fn_8006B8FC(void);
void fn_8006B908(u32 r3);
void fn_8006B930(void);
void fn_8006B9B8(void);
void fn_8006BB34(void);
void fn_8006C018(void);
void fn_8006C0DC(void);
void fn_8006C164(void);
void fn_8006C5D8(void);
void fn_8006C7D4(void);
void fn_8006CCC0(void);
void fn_8006D550(void);
void fn_8006D940(void);
void fn_8006D98C(void);
void fn_8006DAE4(void* arg0);
void fn_8006DC28(void);
void fn_8006E0CC(void);
u32 fn_8006E128(u8* p);
void fn_8006E160(u32 r3);
void fn_8006E188(void);
void fn_8006E18C(void);
void fn_8006E258(void);
void fn_8006E338(void);
void fn_8006E798(void);
void fn_8006E9A4(void);
void fn_8006EE7C(void);
void fn_8006EF24(void);
void fn_8006EFF8(void);
void fn_8006F284(void);
void fn_8006F720(void);
void fn_8006FBFC(void);
void fn_8006FCF8(u32 r3);
void fn_8006FD24(u32 r3);
void fn_8006FD4C(u32 r3);
void fn_8006FD74(u32 r3);
void fn_8006FD9C(u32 r3);
void fn_8006FDC4(u32 r3);
void fn_8006FDEC(u32 r3);
void fn_8006FE14(u32 r3);
void fn_8006FE3C(u32 r3);
void fn_8006FE64(void);
void fn_8006FEE4(void);
void fn_80070274(u32 r3);
void fn_8007029C(u32 r3);
void fn_800702C8(u32 r3);
void fn_800702F0(u32 r3);
void fn_80070318(void);
void fn_80070428(void);
void fn_800704A4(void);
void fn_800704A8(void);
void fn_800704AC(void);
void fn_800706C4(void);
void fn_80070A9C(void);
void fn_80070D84(void);

/* ===== Function implementations ===== */


/* 0x80069C0C | size: 0xA50 */
void fn_80069C0C(void) {
    extern void fn_8006A7E0();
    extern void fn_8006A7F0();
    extern void fn_8006A81C();
    extern void fn_8006A990();
    extern void fn_8006AABC();
    extern void fn_8006AFC4();
    extern void fn_8006B1C0();
    extern void fn_8011CE44();
    extern void fn_8011CE74();
    extern void fn_8011DCB4();
    extern void fn_8011DE98();
    extern void fn_8011E520();
    extern void fn_8011E778();
    extern void fn_8011F4A8();
    extern void fn_8011F5C8();
    extern void fn_80123FBC();
    extern void fn_8012546C();
    extern void fn_8012AA64();
    extern void fn_8012AC08();
    extern void fn_8012AC64();
    extern void __assert();
    extern void fn_801F9CBC();
    extern void fn_8020DEF0();
    extern void fn_8020DF00();
    extern void fn_8020DF10();
    extern void fn_8020DF50();
    extern void fn_8020DF90();
    extern void fn_8020DFA0();
    extern void fn_8020DFB0();
    extern void fn_8020E0F8();
    u8 sp[0xBC0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
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

    
    r31 = r3;
    r0 = *(u32*)((u8*)r31 + 0x4);
    r3 = (u32)&lbl_80267C18;
    r29 = (u32)&lbl_80267C18;
    r25 = 0x0;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_80069C68;
            }
            if ((s32)r0 >= (s32)0x3) goto L_80069C68;
            goto L_80069C64;
            }
        r25 = 0x20a;
        goto L_80069C68;
    }
    r25 = 0x20b;
    goto L_80069C68;
    L_80069C64: ;
    r25 = 0x20c;
    L_80069C68: ;
    r0 = r25 & 0xFFFF;
    if (r0 == (u32)0x0) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x8c;
        r4 = 0xf8;
        __assert();
    }
    r3 = r25;
    fn_8020E0F8();
    r0 = *(u32*)((u8*)r31 + 0x0);
    r30 = r3;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) goto L_80069D48;
        if ((s32)r0 < (s32)0x0) {
            goto L_80069D48;
        }
        r0 = *(u32*)((u8*)r31 + 0x14);
        do {
            if ((s32)r0 == (s32)0x7 || (s32)r0 >= (s32)0x7) break;

            if ((s32)r0 < (s32)0x0) {
                break;
            }
            r4 = 0x1f;
            goto L_80069D7C;
        } while (0);
        r0 = *(u32*)((u8*)r31 + 0xC);
        do {
            if ((s32)r0 == (s32)0x4) break;
            if ((s32)r0 < (s32)0x4) {
                if ((s32)r0 >= (s32)0x3) break;
                if ((s32)r0 >= (s32)0x0) break;
                break;
            }
            break;
        } while (0);

        r4 = 0x20;
        goto L_80069D7C;


    }
    r0 = *(u32*)((u8*)r31 + 0x14);
    if (r0 < (u32)0x1e) {
        r4 = 0x1a;

    } else if (r0 < (u32)0x3c) {
        r4 = 0x17;

    } else if (r0 < (u32)0x63) {
        r4 = 0x18;

    }
    r4 = 0x3d5;
    goto L_80069D7C;
    L_80069D48: ;
    r5 = *(u32*)&lbl_8047A5D8;
    r3 = (0xaaab << 16);
    /* subi r0, r3, 0x5555 */;
    r4 = r29 + 0x0;
    r3 = r5 + 0x1;
    r5 = r5 << 2;
    r0 = (u32)((u64)r0 * (u64)r3 >> 32);
    *(u32*)&lbl_8047A5D8 = r3;
    r4 = *(u32*)(r4 + r5);
    r0 = (u32)r0 >> 1;
    r0 = r0 * 0x3;
    r0 = r3 - r0;
    *(u32*)&lbl_8047A5D8 = r0;
    L_80069D7C: ;
    r3 = r30;
    fn_8020DF00();
    r0 = *(u32*)((u8*)r31 + 0x0);
    do {
        if ((s32)r0 != (s32)0x1) break;
        r0 = *(u32*)((u8*)r31 + 0xC);
        if ((s32)r0 != (s32)0x6) break;
        r0 = *(u32*)((u8*)r31 + 0x14);
        if (r0 < (u32)0x1e) {
            r4 = 0x28;

        } else if (r0 < (u32)0x3c) {
            r4 = 0x29;

        } else if (r0 < (u32)0x63) {
            r4 = 0x2a;

        }
        r4 = 0x2e;
        goto L_80069E08;
    } while (0);

    r0 = *(u32*)((u8*)r31 + 0xC);
    if (r0 >= (u32)0x7) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0xb8;
        r4 = 0x166;
        __assert();
    }
    r0 = *(u32*)((u8*)r31 + 0xC);
    r3 = r29 + 0xc;
    r0 = r0 << 1;
    r0 = *(u16*)(r3 + r0);
    r4 = r0;
    L_80069E08: ;
    r3 = r30;
    fn_8020DF90();
    r0 = *(u32*)((u8*)r31 + 0x0);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_80069EEC;
                }
                goto L_80069EEC;
                }
            r0 = *(u32*)((u8*)r31 + 0x14);
            if (r0 >= (u32)0x8) {
                r3 = r29 + 0x7c;
                r5 = r29 + 0xe0;
                r4 = 0x17f;
                __assert();
            }
            r0 = *(u32*)((u8*)r31 + 0x14);
            if (r0 < (u32)0x7) {
                r3 = r30;
                r4 = 0xd;
                fn_8020DFB0();
            } else {

                r3 = r30;
                r4 = 0xe;
                fn_8020DFB0();
            }
            r0 = *(u32*)((u8*)r31 + 0x14);
            r4 = r29 + 0x1c;
            r3 = r30;
            r0 = r0 << 2;
            r4 = *(u32*)(r4 + r0);
            fn_8020DEF0();
            goto L_80069F04;
                }
        r0 = *(u32*)((u8*)r31 + 0x14);
        if (r0 < (u32)0x63) {
            r3 = r30;
            r4 = 0xf;
            fn_8020DFB0();
        } else {

            r3 = r30;
            r4 = 0x12;
            fn_8020DFB0();
        }
        r3 = r30;
        r4 = 0x0;
        fn_8020DEF0();
        goto L_80069F04;
    }
    r3 = r30;
    r4 = 0x10;
    fn_8020DFB0();
    r3 = r30;
    r4 = 0x0;
    fn_8020DEF0();
    goto L_80069F04;
    L_80069EEC: ;
    r3 = r30;
    r4 = 0xc;
    fn_8020DFB0();
    r3 = r30;
    r4 = 0x0;
    fn_8020DEF0();
    L_80069F04: ;
    ((void(*)(void))fn_80077DB8)();
    if ((s32)r3 == (s32)0x6) {
        r0 = *(u32*)((u8*)r31 + 0x4);
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 < (s32)0x1) {
                goto L_80069F54;
            }
            if ((s32)r0 >= (s32)0x3) goto L_80069F54;

        } else {
        r3 = r30;
        r4 = 0x1;
        fn_8020DFA0();
        goto L_80069FFC;
        }
        r3 = r30;
        r4 = 0x2;
        fn_8020DFA0();
        goto L_80069FFC;
        L_80069F54: ;
        r3 = r30;
        r4 = 0x0;
        fn_8020DFA0();
        goto L_80069FFC;
    }
    r0 = *(u32*)((u8*)r31 + 0x4);
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            goto L_80069FD8;
        }
        if ((s32)r0 >= (s32)0x3) goto L_80069FD8;

    } else {
    if ((s32)r3 != (s32)0x4) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x108;
        r4 = 0x1c0;
        __assert();
    }
    r3 = r30;
    r4 = 0x5;
    fn_8020DFA0();
    goto L_80069FFC;
    }
    if ((s32)r3 != (s32)0x2) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x114;
        r4 = 0x1c5;
        __assert();
    }
    r3 = r30;
    r4 = 0x6;
    fn_8020DFA0();
    goto L_80069FFC;
    L_80069FD8: ;
    if ((s32)r3 != (s32)0x3) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x120;
        r4 = 0x1cb;
        __assert();
    }
    r3 = r30;
    r4 = 0x4;
    fn_8020DFA0();
    L_80069FFC: ;
    r0 = *(u32*)((u8*)r31 + 0x0);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006A5A0;
            }
            if ((s32)r0 < (s32)0x4) {
                r0 = *(u32*)((u8*)r31 + 0x4);
                if (r0 >= (u32)0x2) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x12c;
                    r4 = 0x221;
                    __assert();
                }
                r0 = *(u32*)((u8*)r31 + 0x0);
                if (r0 >= (u32)0x4) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x150;
                    r4 = 0x222;
                    __assert();
                }
                r4 = *(u32*)((u8*)r31 + 0x4);
                r3 = r29 + 0x3c;
                r0 = *(u32*)((u8*)r31 + 0x10);
                r4 = r4 << 5;
                r0 = r0 << 3;
                r27 = r4 + r0;
                r27 = r3 + r27;
                ((void(*)(void))fn_800E0C54)();
                r0 = r3 & 0xFFFF;
                r4 = *(u32*)((u8*)r31 + 0x4);
                r0 = r0 & 0x7;
                r3 = *(u8*)(r27 + r0);
                /* subi r5, r3, 0x1 */;
                if ((s32)r4 == (s32)0x0 || (s32)r4 == (s32)0x1) {


                if (r5 < (u32)0x64) {


                    r3 = r4 * 0x64;
                    r0 = *(u32*)&lbl_80478938;
                    r25 = r3 + r5;
                    r25 = r25 + 0x60;
                    if (r0 <= (u32)r25) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x174;
                    r4 = 0xca;
                    __assert();
                    }
                    r3 = (u32)&lbl_802EE618;
                    r0 = r25 << 1;
                    r3 = (u32)&lbl_802EE618;
                    r25 = *(u16*)(r3 + r0);
                }
            }
                r26 = r25 & 0xFFFF;
                do {
                    ((void(*)(void))fn_800E0C54)();
                    r0 = r3 & 0xFFFF;
                    r4 = *(u32*)((u8*)r31 + 0x4);
                    r0 = r0 & 0x7;
                    r3 = *(u8*)(r27 + r0);
                    /* subi r5, r3, 0x1 */;
                    if ((s32)r4 == (s32)0x0 || (s32)r4 == (s32)0x1) {


                    if (r5 < (u32)0x64) {


                        r3 = r4 * 0x64;
                        r0 = *(u32*)&lbl_80478938;
                        r28 = r3 + r5;
                        r28 = r28 + 0x60;
                        if (r0 <= (u32)r28) {
                        r3 = r29 + 0x7c;
                        r5 = r29 + 0x174;
                        r4 = 0xca;
                        __assert();
                        }
                        r3 = (u32)&lbl_802EE618;
                        r0 = r28 << 1;
                        r3 = (u32)&lbl_802EE618;
                        r28 = *(u16*)(r3 + r0);
                    }
            }
                    r0 = r28 & 0xFFFF;
                } while (r26 == (u32)r0);
                r4 = r25;
                r3 = r31 + 0x1684;
                fn_8006AABC();
                r3 = r31 + 0x1684;
                r4 = 0x0;
                fn_8006A81C();
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = r31 + 0x16b0;
                    fn_8012AC08();
                    r4 = 0x0;
                    fn_8011DCB4();
                    r25 = r25 + 0x1;


                }
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = r31 + 0x21c8;
                    fn_8012AC08();
                    r4 = 0x0;
                    fn_8011DCB4();
                    r25 = r25 + 0x1;


                }
                ((void(*)(void))fn_800E0C54)();
                r4 = (0xaaab << 16);
                r5 = r3 & 0xFFFF;
                /* subi r0, r4, 0x5555 */;
                r3 = (u32)&lbl_8047C028;
                r0 = (u32)((u64)r0 * (u64)r5 >> 32);
                r0 = (u32)r0 >> 1;
                r0 = r0 * 0x3;
                r0 = r5 - r0;
                r0 = r0 << 1;
                r3 = *(u16*)(r3 + r0);
                ((void(*)(void))fn_800FA280)();
                r4 = (u32)sp + 0x8;
                while (1) {
                    r0 = *(u16*)((u8*)r3 + 0x0);
                    if (r0 == (u32)0x0) break;
                    *(u16*)((u8*)r4 + 0x0) = r0;
                    r3 = r3 + 0x2;
                    r4 = r4 + 0x2;


                }
                r0 = 0x0;
                r3 = 0x0;
                *(u16*)((u8*)r4 + 0x0) = r0;
                fn_8006B1C0();
                r4 = r3;
                r3 = r28;
                r5 = (u32)sp + 0x88;
                fn_801F9CBC();
                r3 = (u32)sp + 0x88;
                r4 = (u32)sp + 0x8;
                fn_8012AA64();
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = (u32)sp + 0x88;
                    fn_8012AC08();
                    r4 = 0x0;
                    fn_8011DCB4();
                    r25 = r25 + 0x1;


                }
                r3 = r31 + 0x24;
                r4 = (u32)sp + 0x88;
                r5 = 0x1;
                fn_8006A990();
                r3 = 0x0;
                fn_8006B1C0();
                r4 = r3;
                r3 = r31 + 0x24;
                fn_8006A81C();
                r3 = r31 + 0x24;
                r4 = 0x0;
                fn_8006A7E0();
            }
            goto L_8006A5A0;
            }
        r0 = *(u32*)((u8*)r31 + 0x4);
        r4 = *(u32*)((u8*)r31 + 0x14);
        r3 = *(u32*)((u8*)r31 + 0xC);
        if ((s32)r0 == (s32)0x0 || (s32)r0 == (s32)0x1) {


        if ((s32)r3 != (s32)0x6) {
            if ((s32)r3 >= (s32)0x6) goto L_8006A33C;
            if ((s32)r3 < (s32)0x0) {
                goto L_8006A33C;
            }
            if (r4 < (u32)0x8) {


                r0 = r0 << 3;
                r3 = r3 << 4;
                r28 = r0 + r4;
                r28 = r3 + r28;
                goto L_8006A33C;
        }
        if (r4 < (u32)0x64) {


                r0 = r0 * 0x64;
                r28 = r0 + r4;
                r28 = r28 + 0x60;
                L_8006A33C: ;
                r0 = *(u32*)&lbl_80478938;
                if (r0 <= (u32)r28) {
                r3 = r29 + 0x7c;
                r5 = r29 + 0x174;
                r4 = 0xca;
                __assert();
                }
                r3 = (u32)&lbl_802EE618;
                r0 = r28 << 1;
                r3 = (u32)&lbl_802EE618;
                r4 = *(u16*)(r3 + r0);
        }
        }
        }
        r3 = r31 + 0x7008;
        fn_8006AABC();
        r3 = r31;
        fn_8006AFC4();
        r4 = r3;
        r3 = r31 + 0x59a8;
        fn_8006A7F0();
        r0 = *(u32*)((u8*)r31 + 0x0);
        if ((s32)r0 == (s32)0x1) {
            r27 = 0x0;
            r26 = r27;
            do {
                r3 = r31 + 0x64ec;
                r4 = r26 & 0xFFFF;
                fn_8012AC08();
                r25 = r3;
                fn_80123FBC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = r25;
                    fn_8011F4A8();
                    r0 = r3 & 0xFF;
                    if ((s32)r27 < (s32)r0) {
                        r3 = r25;
                        fn_8011F4A8();
                        r27 = r3 & 0xFF;
                }
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            if (r27 > (u32)0x64) {
                r27 = 0x64;
            }
            r28 = r31 + 0x7b4c;
            r26 = 0x0;
            do {
                r3 = r28;
                r4 = r26 & 0xFFFF;
                fn_8012AC08();
                r25 = r3;
                fn_80123FBC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = r25;
                    fn_8011F4A8();
                    r0 = r3 & 0xFF;
                    if (r27 > r0) {
                        r3 = r25;
                        fn_8011F5C8();
                        fn_8011E778();
                        fn_8011E520();
                        fn_8011CE74();
                        r4 = r27 & 0xFF;
                        fn_8011CE44();
                        r4 = r3;
                        r3 = r25;
                        fn_8011DE98();
                        r3 = r25;
                        fn_8012546C();
                }
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            r4 = r28;
            r3 = r31 + 0x7034;
            fn_8012AC64();
        }
        r3 = r31 + 0x24;
        r4 = r31 + 0x59a8;
        fn_8006A7F0();
        r3 = r31 + 0x1684;
        r4 = r31 + 0x7008;
        fn_8006A7F0();
        r3 = 0x0;
        fn_8006B1C0();
        r4 = r3;
        r3 = r31 + 0x24;
        fn_8006A81C();
        r3 = r31 + 0x24;
        r4 = 0x0;
        fn_8006A7E0();
        r3 = 0x0;
        r0 = 0x1;
        *(u16*)((u8*)r31 + 0x59AA) = r3;
        *(u16*)((u8*)r31 + 0x26) = r3;
        *(u16*)((u8*)r31 + 0x700A) = r0;
        *(u16*)((u8*)r31 + 0x1686) = r0;
        goto L_8006A5A0;
    }
    r0 = *(u32*)((u8*)r31 + 0x4);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 >= (s32)0x2) goto L_8006A590;
        if ((s32)r0 < (s32)0x0) {
            goto L_8006A590;
        }
        r0 = 0x0;
        r3 = 0x1;
        *(u32*)((u8*)r31 + 0x59D0) = r0;
        r5 = r31 + (0x1 << 16);
        r6 = 0x2;
        r0 = 0x3;
        *(u32*)((u8*)r31 + 0x7030) = r3;
        r3 = r31 + 0x24;
        r4 = r31 + 0x59a8;
        *(u32*)((u8*)r5 + (-31088)) = r6;
        *(u32*)((u8*)r5 + (-25360)) = r0;
        fn_8006A7F0();
        r3 = r31 + 0x1684;
        r4 = r31 + 0x7008;
        fn_8006A7F0();
        goto L_8006A5A0;
    }
    r0 = *(u32*)((u8*)r31 + 0x59D0);
    r3 = r31 + 0x24;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r0 = *(u32*)((u8*)r31 + 0x7030);
    r3 = r31 + 0x1684;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r4 = r31 + (0x1 << 16);
    r3 = r31 + 0x2ce4;
    r0 = *(u32*)((u8*)r4 + (-31088));
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r4 = r31 + (0x1 << 16);
    r3 = r31 + 0x4344;
    r0 = *(u32*)((u8*)r4 + (-25360));
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    goto L_8006A5A0;
    L_8006A590: ;
    r3 = r29 + 0x7c;
    r4 = 0x291;
    r5 = (u32)&lbl_8047C030;
    __assert();
    L_8006A5A0: ;
    r0 = *(u32*)((u8*)r31 + 0x4);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 >= (s32)0x2) goto L_8006A63C;
        if ((s32)r0 < (s32)0x0) {
            goto L_8006A63C;
        }
        r5 = *(u16*)((u8*)r31 + 0x2CE4);
        r3 = r30;
        r4 = 0x2;
        fn_8020DF50();
        r5 = *(u16*)((u8*)r31 + 0x4344);
        r3 = r30;
        r4 = 0x3;
        fn_8020DF50();
        r5 = *(u32*)((u8*)r31 + 0x2D08);
        r3 = r30;
        r4 = 0x2;
        fn_8020DF10();
        r5 = *(u32*)((u8*)r31 + 0x4368);
        r3 = r30;
        r4 = 0x3;
        fn_8020DF10();
        }
    r5 = *(u16*)((u8*)r31 + 0x24);
    r3 = r30;
    r4 = 0x0;
    fn_8020DF50();
    r5 = *(u16*)((u8*)r31 + 0x1684);
    r3 = r30;
    r4 = 0x1;
    fn_8020DF50();
    r5 = *(u32*)((u8*)r31 + 0x48);
    r3 = r30;
    r4 = 0x0;
    fn_8020DF10();
    r5 = *(u32*)((u8*)r31 + 0x16A8);
    r3 = r30;
    r4 = 0x1;
    fn_8020DF10();
    L_8006A63C: ;
    r0 = 0x1;
    r3 = 0x0;
    *(u8*)((u8*)r31 + 0x1C) = r0;
    return;
}


/* 0x8006A65C | size: 0xBC */
#pragma scheduling off
u16 fn_8006A65C(void) {
    extern void* fn_80129280(int, int);
    extern void fn_801657D0(int);
    extern void fn_80088EA8(void*);
    extern u32 fn_801906A0(int);
    extern void fn_80069C0C(void*);
    extern void fn_800FF730(int);
    extern void _threadSwitch(void);
    void* menuPtr;
    u32 x;

    menuPtr = (u8*)fn_80129280(0, 0xe) + 0xC9A8;
    fn_801657D0(0x3e8);
    fn_80088EA8(menuPtr);
    x = fn_801906A0(0xb59);
    *(u32*)((u8*)fn_80129280(0, 0xe) + 0x14) = x;
    *(u32*)((u8*)fn_80129280(0, 0xe) + 0xc) = 6;
    *(u32*)((u8*)fn_80129280(0, 0xe) + 0x0) = 1;
    fn_80069C0C(fn_80129280(0, 0xe));
    fn_800FF730(0x397);
    _threadSwitch();
    return (u16) * (u32*)((u8*)fn_80129280(0, 0xe) + 0x20);
}
#pragma scheduling reset


/* 0x8006A718 | size: 0x54 */
void fn_8006A718(void) {
    extern void fn_80129280();
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    
    r4 = 0xe;
    fn_80129280();
    r3 = r3 + (0x1 << 16);
    r0 = *(u8*)((u8*)r3 + (-13944));
    if (r0 != (u32)0x0) {
        /* subi r3, r3, 0x4cd8 */;
    } else {

        r3 = 0x0;
    }
    if (r3 != (u32)0x0) {
        r3 = *(u16*)((u8*)r3 + 0x0);
    } else {

        r3 = 0x0;
    }
    return;
}


/* 0x8006A76C | size: 0x30 */
#pragma push
#pragma peephole off
u8 fn_8006A76C(void) {
    extern u8 fn_801D04E8(void);
    return (u8)((fn_801D04E8() & 0xFF) == 0);
}
#pragma pop

/* 0x8006A79C | size: 0x10 */
void fn_8006A79C(u8* p) {
    p[0xC98B] = 0;
}

/* 0x8006A7AC | size: 0x10 */
void fn_8006A7AC(u8* p) {
    p[0xC98B] = 1;
}

/* 0x8006A7BC | size: 0xC */
u8 fn_8006A7BC(u8* p) {
    return p[0xC98B];
}

/* 0x8006A7C8 | size: 0x8 */
u32 fn_8006A7C8(u32 r3) {
    return r3 + 0xb44;
}

/* 0x8006A7D0 | size: 0x8 */
u16 fn_8006A7D0(u32 r3) {
    return *(u16*)((u8*)r3 + 0x0);
}

/* 0x8006A7D8 | size: 0x8 */
u16 fn_8006A7D8(u32 r3) {
    return *(u16*)((u8*)r3 + 0x2);
}

/* 0x8006A7E0 | size: 0x8 */
void fn_8006A7E0(u32 r3, u32 r4) {
    *(u32*)((u8*)r3 + 0x4) = r4;
}

/* 0x8006A7E8 | size: 0x8 */
u32 fn_8006A7E8(u32 r3) {
    return *(u32*)((u8*)r3 + 0x4);
}

/* 0x8006A7F0 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_8006A7F0(void* dst, const void* src) {
    memcpy(dst, src, 0x1660);
}
#pragma pop

/* 0x8006A814 | size: 0x8 */
u32 fn_8006A814(u32 r3) {
    return *(u32*)((u8*)r3 + 0x24);
}

/* 0x8006A81C | size: 0x8 */
void fn_8006A81C(u32 r3, u32 r4) {
    *(u32*)((u8*)r3 + 0x24) = r4;
}

/* 0x8006A824 | size: 0x16C */
void fn_8006A824(void) {
    extern void fn_8006A990();
    extern void fn_8012A784();
    extern void fn_8012AA2C();
    extern void __assert();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r4;
    r4 = (u32)&lbl_80267DD8;
    r3 = r29;
    r31 = (u32)&lbl_80267DD8;
    r30 = 0x1;
    fn_8012A784();
    r0 = r3 & 0xFF;
    switch ((s32)r0) {
        case 0:
            r3 = r29;
            r30 = 0x1;
            fn_8012AA2C();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = r31 + 0x10;
                r5 = r31 + 0x20;
                r4 = 0x281;
                __assert();
            }
            break;
        case 1:
            r3 = r29;
            fn_8012AA2C();
            r0 = r3 & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x2; break;
                case 1: r30 = 0x3; break;
                default:
                    r3 = r31 + 0x10;
                    r5 = r31 + 0x4c;
                    r4 = 0x28a;
                    __assert();
                    break;
            }
            break;
        case 2:
            r3 = r29;
            fn_8012AA2C();
            r0 = r3 & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x309; break;
                case 1: r30 = 0x308; break;
                default:
                    r3 = r31 + 0x10;
                    r5 = r31 + 0x4c;
                    r4 = 0x294;
                    __assert();
                    break;
            }
            break;
        default:
            r3 = r31 + 0x10;
            r5 = r31 + 0x60;
            r4 = 0x299;
            __assert();
            break;
    }
    r3 = r28;
    r4 = r29;
    r5 = r30;
    fn_8006A990();
    return;
}


/* 0x8006A990 | size: 0x12C */
void fn_8006A990(void) {
    extern void fn_8012086C();
    extern void fn_8012A7B4();
    extern void fn_8012AC08();
    extern void fn_8012AC64();
    u8 sp[0xB30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r5;
    r3 = (u32)sp + 0x8;
    fn_8012AC64();
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    fn_8012A7B4();
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r4 = r30;
        r3 = (u32)sp + 0x8;
        fn_8012AC08();
        r31 = r3;
        ((void(*)(void))fn_80077A5C)();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = r31;
            fn_8012086C();
        }
        r30 = r30 + 0x1;


    }
    r31 = *(u16*)((u8*)r28 + 0x2);
    r3 = r28;
    r4 = 0x0;
    r5 = 0x1660;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u16*)((u8*)r28 + 0x2) = r31;
    r3 = r28 + 0x2c;
    r4 = (u32)sp + 0x8;
    fn_8012AC64();
    r3 = r28 + 0xb44;
    r4 = (u32)sp + 0x8;
    fn_8012AC64();
    *(u16*)((u8*)r28 + 0x0) = r29;
    r4 = r29 & 0xFFFF;
    r3 = *(u32*)&lbl_80478F20;
    r0 = *(u32*)((u8*)r3 + 0x0);
    if (r0 <= (u32)r4) {
        r0 = -0x1;
    } else if ((s32)r4 >= (s32)0x9) {
        r0 = 0x2;
    } else if ((s32)r4 == (s32)0x1 || (s32)r4 < (s32)0x1 || (s32)r4 >= (s32)0x30a || (s32)r4 < (s32)0x308) {
        r0 = 0x0;
    } else {
        r0 = 0x1;
    }
    *(u32*)((u8*)r28 + 0x4) = r0;
    return;
}


/* 0x8006AABC | size: 0x16C */
void fn_8006AABC(void) {
    extern void fn_8012086C();
    extern void fn_8012A130();
    extern void fn_8012A7B4();
    extern void fn_8012AC08();
    extern void fn_8012AC64();
    extern void __assert();
    extern void fn_801F9CBC();
    u8 sp[0xB30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r4;
    r4 = (u32)&lbl_80267DD8;
    r3 = (u32)sp + 0x8;
    r31 = (u32)&lbl_80267DD8;
    r4 = r28 + 0xb44;
    fn_8012AC64();
    r4 = *(u32*)((u8*)r31 + 0x0);
    r3 = r29;
    r5 = (u32)sp + 0x8;
    fn_801F9CBC();
    r3 = (u32)sp + 0x8;
    fn_8012A130();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = r31 + 0x10;
        r5 = r31 + 0x7c;
        r4 = 0x258;
        __assert();
    }
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    fn_8012A7B4();
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r4 = r30;
        r3 = (u32)sp + 0x8;
        fn_8012AC08();
        r31 = r3;
        ((void(*)(void))fn_80077A5C)();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = r31;
            fn_8012086C();
        }
        r30 = r30 + 0x1;


    }
    r31 = *(u16*)((u8*)r28 + 0x2);
    r3 = r28;
    r4 = 0x0;
    r5 = 0x1660;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u16*)((u8*)r28 + 0x2) = r31;
    r3 = r28 + 0x2c;
    r4 = (u32)sp + 0x8;
    fn_8012AC64();
    r3 = r28 + 0xb44;
    r4 = (u32)sp + 0x8;
    fn_8012AC64();
    *(u16*)((u8*)r28 + 0x0) = r29;
    r4 = r29 & 0xFFFF;
    r3 = *(u32*)&lbl_80478F20;
    r0 = *(u32*)((u8*)r3 + 0x0);
    if (r0 <= (u32)r4) {
        r0 = -0x1;
    } else if ((s32)r4 >= (s32)0x9) {
        r0 = 0x2;
    } else if ((s32)r4 == (s32)0x1 || (s32)r4 < (s32)0x1 || (s32)r4 >= (s32)0x30a || (s32)r4 < (s32)0x308) {
        r0 = 0x0;
    } else {
        r0 = 0x1;
    }
    *(u32*)((u8*)r28 + 0x4) = r0;
    return;
}


/* 0x8006AC28 | size: 0x44 */
#pragma push
#pragma peephole off
void fn_8006AC28(void* p, u16 value) {
    memset(p, 0, 0x1660);
    *(u16*)((u8*)p + 2) = value;
}
#pragma pop


/* 0x8006AC6C | size: 0x60 */
s32 fn_8006AC6C(u32 r3) {
    u32 r0;
    u32 r4;

    r4 = *(u32*)&lbl_80478F20;
    r3 = r3 & 0xffff;
    r0 = *(u32*)((u8*)r4 + 0);
    if (r0 <= r3) {
        return -1;
    }
    if ((s32)r3 < 9) {
        if ((s32)r3 == 1) {
            goto ret0;
        }
        if ((s32)r3 >= 1) {
            goto ret1;
        }
        goto ret0;
    }
    if ((s32)r3 >= 0x30a) {
        goto ret0;
    }
    if ((s32)r3 >= 0x308) {
        goto ret2;
    }
    goto ret0;
ret1:
    return 1;
ret2:
    return 2;
ret0:
    return 0;
}


/* 0x8006ACCC | size: 0xE8 */
void fn_8006ACCC(void) {
    extern void fn_80129280();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x4);
    do {
        if ((s32)r0 == (s32)0x2 || (s32)r0 >= (s32)0x2) break;

        if ((s32)r0 < (s32)0x0) {
            break;
        }
        if ((s32)r29 < (s32)0x0) { r3 = 0x0; return; }
        if ((s32)r29 > (s32)0x1) { r3 = 0x0; return; }
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r4 = r29 * 0x1660;
        r0 = r3;
        r3 = r4 + 0x24;
        r3 = r0 + r3;
        return;
    } while (0);
    r30 = 0x0;
    r31 = 0x0;
    do {
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r0 = r31 + 0x4c;
        r0 = *(u32*)(r3 + r0);
        if ((s32)r29 == (s32)r0) {
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r4 = r30 * 0x1660;
            r0 = r3;
            r3 = r4 + 0x24;
            r3 = r0 + r3;
            return;
        }
        r31 = r31 + 0x1660;
        r30 = r30 + 0x1;
    } while ((s32)r30 < (s32)0x4);

    r3 = 0x0;

    return;
}


/* 0x8006ADB4 | size: 0x38 */
#pragma push
#pragma peephole off
void fn_8006ADB4(s32 value) {
    extern u8* fn_80129280(s32 idx, s32 type);

    *(s32*)(fn_80129280(0, 0xe) + 0x59a4) = value;
}
#pragma pop


/* 0x8006ADEC | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_8006ADEC(void) {
    extern u8 *fn_80129280(s32 idx, s32 type);
    return *(s32*)(fn_80129280(0x0, 0xe) + 0x59a4);
}
#pragma pop

/* 0x8006AE18 | size: 0xD4 */
void fn_8006AE18(void) {
    extern void fn_80129280();
    extern void fn_801906A0();
    extern void __assert();
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    
    r3 = 0x8ae;
    fn_801906A0();
    if (r3 == (u32)0x0) { r3 = 0x0; return; }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r3 = r3 + (0x1 << 16);
    r0 = *(u8*)((u8*)r3 + (-13944));
    if (r0 != (u32)0x0) {
        /* subi r3, r3, 0x4cd8 */;
    } else {

        r3 = 0x0;
    }
    if (r3 == (u32)0x0) { r3 = 0x0; return; }
    r0 = *(u16*)((u8*)r3 + 0x0);
    if ((s32)r0 == (s32)0x3) { r3 = 0x2; return; }
    if ((s32)r0 < (s32)0x3) {
        if ((s32)r0 == (s32)0x1) { r3 = 0x0; return; }
        if ((s32)r0 >= (s32)0x1) { r3 = 0x1; return; }

    } else {
    if ((s32)r0 == (s32)0x309) { r3 = 0x3; return; }
    if ((s32)r0 < (s32)0x309) {
        if ((s32)r0 >= (s32)0x308) { r3 = 0x4; return; }
    }


    }
    r3 = (u32)&lbl_80267DE8;
    r4 = 0x1c2;
    r3 = (u32)&lbl_80267DE8;
    r5 = (u32)&lbl_8047C040;
    __assert();

    r3 = 0x0;

    return;
}


/* 0x8006AEEC | size: 0x58 */
#pragma push
#pragma scheduling off
u8* fn_8006AEEC(void) {
    extern u8* fn_80129280(s32 idx, s32 type);
    u8* p;

    p = fn_80129280(0, 0xe) + 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        p -= 0x4cd8;
    } else {
        p = NULL;
    }
    if (p == NULL) {
        return NULL;
    }
    return p + 0xb44;
}
#pragma pop


/* 0x8006AF44 | size: 0x80 */
void fn_8006AF44(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    if (r4 != (u32)0x0) {
        r3 = r31 + (0x1 << 16);
        r5 = 0x1660;
        /* subi r3, r3, 0x4cd8 */;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r3 = (u32)&lbl_80267DD8;
        r4 = r31 + (0x1 << 16);
        r5 = (u32)&lbl_80267DD8;
        r3 = 0x0;
        r5 = *(u32*)((u8*)r5 + 0x0);
        r0 = 0x1;
        *(u32*)((u8*)r4 + (-19636)) = r5;
        *(u16*)((u8*)r4 + (-19670)) = r3;
        *(u8*)((u8*)r4 + (-13944)) = r0;
    } else {

        r3 = r31 + (0x1 << 16);
        r0 = 0x0;
        *(u8*)((u8*)r3 + (-13944)) = r0;
    }
    r3 = r31 + (0x1 << 16);
    r0 = 0x0;
    *(u8*)((u8*)r3 + (-13941)) = r0;
    return;
}


/* 0x8006AFC4 | size: 0x20 */
u8* fn_8006AFC4(u8* p) {
    p += 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        return p - 0x4cd8;
    }
    return 0;
}

/* 0x8006AFE4 | size: 0xB8 */
void fn_8006AFE4(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r4 = (u32)&lbl_80267DD8;
    r4 = (u32)&lbl_80267DD8;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r31 = 0x0;
        goto L_8006B05C;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r31 = 0x1;
        goto L_8006B05C;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r31 = 0x2;
        goto L_8006B05C;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r31 = 0x3;

    } else {
    r31 = -0x1;
    }
    L_8006B05C: ;
    if ((s32)r31 < (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r4 = r31 * 0x1660;
        r0 = r3;
        r3 = r4 + 0x24;
        r3 = r0 + r3;
    }
    return;
}


/* 0x8006B09C | size: 0x5C */
void fn_8006B09C(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    if ((s32)r31 < (s32)0x0) { r3 = 0x0; return; }
    if ((s32)r31 >= (s32)0x4) {

        r3 = 0x0;
        return;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r31 * 0x1660;
    r0 = r3;
    r3 = r4 + 0x24;
    r3 = r0 + r3;

    return;
}


/* 0x8006B0F8 | size: 0x5C */
void fn_8006B0F8(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    if ((s32)r31 < (s32)0x0) { r3 = 0x0; return; }
    if (r31 >= (u32)0x4) {

        r3 = 0x0;
        return;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r31 * 0x1660;
    r0 = r3;
    r3 = r4 + 0x50;
    r3 = r0 + r3;

    return;
}


/* 0x8006B154 | size: 0x6C */
void fn_8006B154(void) {
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;

    
    r4 = (u32)&lbl_80267DD8;
    r4 = (u32)&lbl_80267DD8;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r3 = 0x0;
        return;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r3 = 0x1;
        return;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r3 = 0x2;
        return;
    }
    r4 = r4 + 0x4;
    r0 = *(u32*)((u8*)r4 + 0x0);
    if ((s32)r3 == (s32)r0) {
        r3 = 0x3;
        return;
    }
    r3 = -0x1;
    return;
}


/* 0x8006B1C0 | size: 0x14 */
u32 fn_8006B1C0(s32 i) {
    return ((u32*)lbl_80267DD8)[i];
}

/* 0x8006B1D4 | size: 0x20 */
void fn_8006B1D4(void) {
    fn_80077DB8();
}

/* 0x8006B1F4 | size: 0xB0 */
#pragma push
#pragma peephole off
u32 fn_8006B1F4(s32 index, s32 slot) {
    extern u8* fn_80129280(s32 idx, s32 type);
    s32 r30;
    s32 r31;
    u32 r0;
    u8* r3;

    r30 = index;
    r31 = slot;
    if (r30 < 0) {
        goto invalid_index;
    }
    if (r30 < 7) {
        goto valid_index;
    }
invalid_index:
    r0 = 0;
    goto check_enabled;
valid_index:
    r3 = fn_80129280(0, 0xe);
    r0 = *(u8*)(r3 + (r30 + (1 << 16)) - 0x342c);
check_enabled:
    r0 = (u8)r0;
    if (r0 != 0) {
        goto enabled;
    }
    return 0;
enabled:
    if (r31 < 0) {
        goto ret0;
    }
    if ((u32)r31 < 2) {
        goto valid_slot;
    }
ret0:
    return 0;
valid_slot:
    r3 = fn_80129280(0, 0xe);
    return *(u8*)(r3 + (r31 + (1 << 16)) + r30 * 2 - 0x3425);
}
#pragma pop


/* 0x8006B2A4 | size: 0xB0 */
void fn_8006B2A4(void) {
    extern void fn_80129280();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r30 = r4;
    if ((s32)r29 >= (s32)0x0) {
        if ((s32)r29 >= (s32)0x7) {
        }
        r0 = 0x0;

        } else {
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = r29 + (0x1 << 16);
    r3 = r0 + r3;
    r0 = *(u8*)((u8*)r3 + (-13356));
        }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x0) return;
    if ((s32)r30 < (s32)0x0) return;
    if (r30 >= (u32)0x2) {
        return;
    }
    r31 = 0x1;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r30 + (0x1 << 16);
    r0 = r29 << 1;
    r3 = r4 + r3;
    r3 = r3 + r0;
    *(u8*)((u8*)r3 + (-13349)) = r31;

    return;
}


/* 0x8006B354 | size: 0x74 */
#pragma push
#pragma peephole off
void fn_8006B354(s32 index) {
    extern u8* fn_80129280(s32 idx, s32 type);
    extern void __assert(char* file, s32 line, char* expr);
    s32 r30;
    u32 r31;

    r30 = index;
    if (r30 >= 0) {
        if (r30 < 7) {
            goto valid_index;
        }
    }
    __assert((char*)&lbl_80267DE8, 0xe4, (char*)&lbl_8047C040);
    return;
valid_index:
    r31 = 1;
    *(u8*)(fn_80129280(0, 0xe) + (r30 + (1 << 16)) - 0x342c) = r31;
}
#pragma pop


/* 0x8006B3C8 | size: 0x58 */
#pragma push
#pragma peephole off
u32 fn_8006B3C8(s32 index) {
    extern u8* fn_80129280(s32 idx, s32 type);
    s32 r31;

    r31 = index;
    if (r31 < 0) {
        goto ret0;
    }
    if (r31 < 7) {
        goto valid_index;
    }
ret0:
    return 0;
valid_index:
    return *(u8*)(fn_80129280(0, 0xe) + (r31 + (1 << 16)) - 0x342c);
}
#pragma pop


/* 0x8006B420 | size: 0x8C */
void fn_8006B420(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r3 = *(u32*)((u8*)r3 + 0x8);
    ((void(*)(void))fn_80077E50)();
    if (r3 != (u32)0x0) {
        return;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r31 = *(u32*)((u8*)r3 + 0x8);
    if ((s32)r31 >= (s32)0x0) {
        if (r31 >= (u32)0x6) {
        }
        r4 = 0x0;
        r3 = r4;
        return;
        }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r31 * 0x54;
    r4 = r4 + (0x1 << 16);
    /* subi r4, r4, 0x3624 */;
    r4 = r3 + r4;

    r3 = r4;

    return;
}


/* 0x8006B4AC | size: 0x70 */
void fn_8006B4AC(void) {
    extern void fn_80129280();
    extern void __assert();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = 0x0;
    if (((s32)r31 >= (s32)0x0) && (r31 < (u32)0x6)) {

        r0 = 0x1;
    }
    if ((s32)r0 == (s32)0x0) {
        r3 = (u32)&lbl_80267DE8;
        r5 = (u32)&lbl_80267E70;
        r3 = (u32)&lbl_80267DE8;
        r4 = 0xb9;
        r5 = (u32)&lbl_80267E70;
        __assert();
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    *(u32*)((u8*)r3 + 0x8) = r31;
    return;
}


/* 0x8006B51C | size: 0x60 */
void fn_8006B51C(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    if ((s32)r31 < (s32)0x0) { r3 = 0x0; return; }
    if (r31 >= (u32)0x6) {

        r3 = 0x0;
        return;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r31 * 0x54;
    r0 = r3;
    r3 = r4 + (0x1 << 16);
    /* subi r3, r3, 0x3624 */;
    r3 = r0 + r3;

    return;
}


/* 0x8006B57C | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_8006B57C(void) {
    extern u8 *fn_80129280(s32 idx, s32 type);
    return fn_80129280(0x0, 0xe)[0x1c];
}
#pragma pop

/* 0x8006B5A8 | size: 0x28 */
#pragma push
#pragma scheduling off
s32 fn_8006B5A8(void) {
    extern s32 fn_80129280(s32 idx, s32 type);
    return fn_80129280(0x0, 0xe);
}
#pragma pop

/* 0x8006B5D0 | size: 0xE4 */
void fn_8006B5D0(void) {
    extern void fn_8006AABC();
    extern void fn_80129280();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r27 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u8*)&lbl_8047A5E0;
    *(u32*)&lbl_8047A5A4 = r3;
    do {
    if (r0 != (u32)0x0) break;

    r3 = (u32)&lbl_80267DD8;
    r30 = r27;
    r29 = (u32)&lbl_80267DD8;
    r28 = 0x0;
    r31 = (u32)&lbl_8047C038;
    do {
        r4 = *(u16*)((u8*)r31 + 0x0);
        r3 = r30 + 0x24;
        fn_8006AABC();
        r5 = *(u32*)((u8*)r29 + 0x0);
        r0 = r28 & 0xFFFF;
        r3 = r30 + 0x59a8;
        r4 = r30 + 0x24;
        *(u32*)((u8*)r30 + 0x48) = r5;
        r5 = 0x1660;
        *(u16*)((u8*)r30 + 0x26) = r0;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r31 = r31 + 0x2;
        r30 = r30 + 0x1660;
        r29 = r29 + 0x4;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x4);
    r0 = *(u32*)((u8*)r27 + 0x0);
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3 || (s32)r0 == (s32)0x0) goto L_8006B688;


    } else {
    r3 = 0xaf;
    ((void(*)(void))fn_80071644)();
    r0 = 0x0;
    *(u32*)((u8*)r27 + 0x10) = r0;
    break;

    }
    L_8006B688: ;
    r3 = 0xa8;
    ((void(*)(void))fn_80071644)();
    r0 = 0x4;
    *(u32*)((u8*)r27 + 0x10) = r0;
    } while (0);

    r0 = 0x0;
    *(u8*)&lbl_8047A5E0 = r0;
    return;
}


/* 0x8006B6B4 | size: 0x234 */
void fn_8006B6B4(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r3;
    r0 = 0x0;
    r4 = (0x1 << 16);
    *(u8*)&lbl_8047A5E0 = r0;
    /* subi r5, r4, 0x33d4 */;
    r4 = 0x0;
    memset((void*)r3, (int)r4, (u32)r5);
    r0 = 0x0;
    r3 = 0x0;
    *(u8*)((u8*)r31 + 0x1C) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x3628 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = 0x1;
    *(u32*)((u8*)r5 + 0x4) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x35d4 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = 0x2;
    *(u32*)((u8*)r5 + 0x4) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x3580 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = 0x0;
    *(u32*)((u8*)r5 + 0x4) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x352c */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = 0x0;
    *(u32*)((u8*)r5 + 0x4) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x34d8 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = 0x0;
    *(u32*)((u8*)r5 + 0x4) = r0;
    ((void(*)(void))fn_80077E50)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r6, r4, 0x3484 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r4 = r31 + (0x1 << 16);
    r5 = 0x6;
    r3 = 0x1;
    *(u32*)((u8*)r6 + 0x4) = r0;
    r0 = 0x0;
    *(u16*)((u8*)r4 + (-13434)) = r5;
    *(u16*)((u8*)r4 + (-13518)) = r5;
    *(u16*)((u8*)r4 + (-13602)) = r5;
    *(u8*)((u8*)r4 + (-13356)) = r3;
    *(u8*)((u8*)r4 + (-13355)) = r3;
    *(u8*)((u8*)r4 + (-13354)) = r3;
    *(u8*)((u8*)r4 + (-13353)) = r0;
    *(u8*)((u8*)r4 + (-13352)) = r3;
    *(u8*)((u8*)r4 + (-13351)) = r0;
    *(u8*)((u8*)r4 + (-13349)) = r0;
    r5 = r31 + 0x2;
    r3 = r5 + (0x1 << 16);
    *(u8*)((u8*)r4 + (-13348)) = r0;
    r5 = r5 + 0x2;
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    r3 = r5 + (0x1 << 16);
    *(u8*)((u8*)r3 + (-13349)) = r0;
    *(u8*)((u8*)r3 + (-13348)) = r0;
    return;
}


/* 0x8006B8E8 | size: 0x8 */
u8 fn_8006B8E8(void) {
    return lbl_8047A5E0;
}

/* 0x8006B8F0 | size: 0xC */
void fn_8006B8F0(void) {
    lbl_8047A5E0 = 0;
}

/* 0x8006B8FC | size: 0xC */
void fn_8006B8FC(void) {
    lbl_8047A5E0 = 1;
}

/* 0x8006B908 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006B908(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006B930 | size: 0x88 */
void fn_8006B930(void) {
    extern void fn_80129280();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    ((void(*)(void))fn_80071160)();
    if ((s32)r3 != (s32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        *(u8*)((u8*)r31 + 0x99) = r0;
        return;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r3 = *(u32*)((u8*)r3 + 0x59CC);
    ((void(*)(void))fn_80071208)();
    r0 = r3 & 0x00001000;
    if (r0 != (u32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        return;
    }
    r0 = r3 & 0x00000200;
    if (r0 == (u32)0x0) return;
    r0 = 0x1;
    *(u8*)((u8*)r31 + 0x98) = r0;
    *(u8*)((u8*)r31 + 0x99) = r0;

    return;
}


/* 0x8006B9B8 | size: 0x17C */
void fn_8006B9B8(void) {
    extern void fn_8006A814();
    extern void fn_80129280();
    extern void fn_80166A28();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x4);
    if ((s32)r0 == (s32)0x2 || (s32)r0 >= (s32)0x2 || (s32)r0 < (s32)0x0) {
        r27 = 0x4;
    } else {
        r27 = 0x2;
    }
    r0 = *(u8*)((u8*)r31 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            return;
        }
        return;
    }
    ((void(*)(void))fn_80071160)();
    if ((s32)r3 != (s32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        *(u8*)((u8*)r31 + 0x99) = r0;
        return;
    }
    r28 = 0x0;
    r29 = 0x0;
    while (1) {
        if ((s32)r28 >= (s32)r27) break;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r0 = r29 + 0x59a8;
        r3 = r3 + r0;
        fn_8006A814();
        ((void(*)(void))fn_80071208)();
        r0 = r3 & 0x00000100;
        if (r0 != (u32)0x0) {
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r0 = r29 + 0x7005;
            r0 = *(u8*)(r3 + r0);
            if (r0 == (u32)0x0) {
                r30 = 0x1;
                r3 = 0x0;
                r4 = 0xe;
                fn_80129280();
                r0 = r29 + 0x7005;
                *(u8*)(r3 + r0) = r30;
                r3 = 0x24;
                fn_80166A28();
            }
        } else {
            r0 = r3 & 0x00000200;
            if (r0 != (u32)0x0) {
                r3 = 0x0;
                r4 = 0xe;
                fn_80129280();
                r0 = r29 + 0x7005;
                r0 = *(u8*)(r3 + r0);
                if (r0 != (u32)0x0) {
                    r30 = 0x0;
                    r3 = 0x0;
                    r4 = 0xe;
                    fn_80129280();
                    r0 = r29 + 0x7005;
                    *(u8*)(r3 + r0) = r30;
                    r3 = 0x25;
                    fn_80166A28();
                } else {
                    r0 = 0x1;
                    *(u8*)((u8*)r31 + 0x98) = r0;
                    *(u8*)((u8*)r31 + 0x99) = r0;
                    return;
                }
            }
        }
        r29 = r29 + 0x1660;
        r28 = r28 + 0x1;

    }

    return;
}


/* 0x8006BB34 | size: 0x4E4 */
void fn_8006BB34(void) {
    extern void fn_80166A28();
    extern void __assert();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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

    
    r22 = r3;
    r3 = (u32)&lbl_80267EA8;
    r25 = (u32)&lbl_80267EA8;
    ((void(*)(void))fn_80105624)();
    r0 = *(u8*)((u8*)r22 + 0xA);
    r26 = r3;
    if (r0 != (u32)0x0) return;
    r5 = *(u16*)((u8*)r26 + 0x6);
    r3 = 0x0;
    r4 = r5 & 0x00000004;
    r7 = r5 & 0x1;
    r0 = -r4;
    r5 = r5 & 0x00000008;
    r0 = r0 | r4;
    r6 = -r7;
    r28 = (u32)r0 >> 31;
    r4 = -r5;
    r0 = r28 & 0xFF;
    r6 = r6 | r7;
    r0 = r4 | r5;
    r27 = (u32)r6 >> 31;
    r29 = (u32)r0 >> 31;
    if (r0 == (u32)0x0) {
        r0 = r29 & 0xFF;
        if (r0 != (u32)0x0) {
        }
        r3 = 0x1;
        }
    r0 = r28 & 0xFF;
    r24 = r3 & 0xFF;
    r4 = 0x0;
    if (r0 == (u32)0x0) {
        r0 = r29 & 0xFF;
        if (r0 == (u32)0x0) {
        }
        r4 = 0x1;
        }
    r3 = *(u32*)((u8*)r22 + 0x4);
    r23 = r4 & 0xFF;
    ((void(*)(void))fn_801022B8)();
    r30 = r3;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = r27 & 0xFF;
        if (r0 == (u32)0x0) return;
        r3 = *(u32*)((u8*)r22 + 0x4);
        r4 = 0xe35;
        ((void(*)(void))fn_80102138)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0x11) = r3;
        r4 = (u32)sp + 0xc;
        *(u8*)(sp + 0x10) = r0;
        r0 = *(u16*)(sp + 0x10);
        *(u16*)(sp + 0xC) = r0;
        r3 = *(u32*)((u8*)r22 + 0x4);
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r3 = r22;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r4 = *(u16*)((u8*)r26 + 0x6);
    r31 = r3;
    r3 = 0x0;
    r0 = r4 & 0x00000002;
    if ((s32)r0 != (s32)0x0) {
        r3 = -0x1;

    } else {
    r0 = r4 & 0x1;
    if ((s32)r0 != (s32)0x0) {
        r3 = 0x1;
    }
    }
    if ((s32)r30 != (s32)0xe33) {
        if ((s32)r30 < (s32)0xe33) {
            if ((s32)r30 != (s32)0xa0d) {
                if ((s32)r30 >= (s32)0xa0d) goto L_8006BD20;
                if ((s32)r30 < (s32)0xa0c) {
                    goto L_8006BD20;
                }
                if ((s32)r30 >= (s32)0xe35) goto L_8006BD20;
                goto L_8006BCE4;
                }
            r3 = r3 * 0xa;
            }
        r0 = *(s16*)((u8*)r31 + 0x14);
        if ((s32)r0 >= (s32)0x0) {
            r0 = r0 + r3;
            if ((s32)r0 < (s32)0x1) {
                r0 = 0x1;

            } else {
            if ((s32)r0 > (s32)0x63) {
                r0 = 0x63;
        }
            }
            r0 = (s16)r0;
            *(u16*)((u8*)r31 + 0x14) = r0;
        }
        goto L_8006BD24;
        L_8006BCE4: ;
        r3 = r3 * 0xa;
    }
    r0 = *(s16*)((u8*)r31 + 0x16);
    if ((s32)r0 >= (s32)0x0) {
        r0 = r0 + r3;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;

        } else {
        if ((s32)r0 > (s32)0x63) {
            r0 = 0x63;
    }
        }
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x16) = r0;
    }
    goto L_8006BD24;
    L_8006BD20: ;
    r3 = 0x0;
    L_8006BD24: ;
    if ((s32)r3 != (s32)0x0) return;
    r0 = *(u16*)((u8*)r22 + 0x94);
    *(u16*)(sp + 0x14) = r0;
    if ((s32)r30 != (s32)0x9fd) {
        if ((s32)r30 < (s32)0x9fd) {
            if ((s32)r30 != (s32)0x9f9) {
                if ((s32)r30 < (s32)0x9f9) {
                    if ((s32)r30 != (s32)0x9f7) {
                        if ((s32)r30 < (s32)0x9f7) {
                            goto L_8006BFFC;
                        }
                        if ((s32)r30 != (s32)0x9fb) {
                            if ((s32)r30 < (s32)0x9fb) {
                                goto L_8006BE08;
                            }
                            if ((s32)r30 == (s32)0xe33) goto L_8006BF70;
                            if ((s32)r30 < (s32)0xe33) {
                                if ((s32)r30 == (s32)0xa0d) goto L_8006BF1C;
                                if ((s32)r30 < (s32)0xa0d) {
                                    if ((s32)r30 >= (s32)0xa0c) goto L_8006BF10;
                            }
                            }
                            goto L_8006BFFC;
                        }
                        if ((s32)r30 >= (s32)0xe35) goto L_8006BFFC;
                        goto L_8006BF64;
                            }
                    if (r24 == (u32)0x0) goto L_8006BFFC;
                    r0 = *(u8*)((u8*)r31 + 0x11);
                    if (r0 != (u32)r23) {
                        r3 = 0x24;
                        fn_80166A28();
                    }
                    *(u8*)((u8*)r31 + 0x11) = r23;
                    return;
                            }
                if (r24 == (u32)0x0) goto L_8006BFFC;
                r0 = *(u8*)((u8*)r31 + 0x12);
                if (r0 != (u32)r23) {
                    r3 = 0x24;
                    fn_80166A28();
                }
                *(u8*)((u8*)r31 + 0x12) = r23;
                return;
                }
            if (r24 == (u32)0x0) goto L_8006BFFC;
            r0 = *(u8*)((u8*)r31 + 0x13);
            if (r0 != (u32)r23) {
                r3 = 0x24;
                fn_80166A28();
            }
            *(u8*)((u8*)r31 + 0x13) = r23;
            return;
            L_8006BE08: ;
            if (r24 != (u32)0x0) {
                r4 = *(s16*)((u8*)r31 + 0x14);
                r0 = (u32)r4 >> 31;
                r3 = (s32)r4 >> 31;
                r22 = r3 ^ r4;
                r22 = r22 - r3;
                if ((s32)r0 == (s32)r23) {
                    r3 = 0x24;
                    fn_80166A28();
                }
                if (r23 != (u32)0x0) {
                    r0 = r22;
                } else {

                    r0 = -r22;
                }
                r0 = (s16)r0;
                *(u16*)((u8*)r31 + 0x14) = r0;
                return;
            }
            r0 = *(s16*)((u8*)r31 + 0x14);
            if ((s32)r0 >= (s32)0x0) {
                r0 = *(u16*)((u8*)r26 + 0x4);
                r0 = r0 & 0x00000010;
            }
            if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
            r3 = *(u32*)((u8*)r22 + 0x4);
            r4 = 0xa0c;
            ((void(*)(void))fn_80102138)();
            r4 = r3;
            r3 = *(u32*)((u8*)r22 + 0x4);
            ((void(*)(void))fn_80102398)();
            return;
                        }
        if (r24 != (u32)0x0) {
            r4 = *(s16*)((u8*)r31 + 0x16);
            r0 = (u32)r4 >> 31;
            r3 = (s32)r4 >> 31;
            r22 = r3 ^ r4;
            r22 = r22 - r3;
            if ((s32)r0 == (s32)r23) {
                r3 = 0x24;
                fn_80166A28();
            }
            if (r23 != (u32)0x0) {
                r0 = r22;
            } else {

                r0 = -r22;
            }
            r0 = (s16)r0;
            *(u16*)((u8*)r31 + 0x16) = r0;
            return;
        }
        r0 = *(s16*)((u8*)r31 + 0x16);
        if ((s32)r0 >= (s32)0x0) {
            r0 = *(u16*)((u8*)r26 + 0x4);
            r0 = r0 & 0x00000010;
        }
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = *(u32*)((u8*)r22 + 0x4);
        r4 = 0xe34;
        ((void(*)(void))fn_80102138)();
        r4 = r3;
        r3 = *(u32*)((u8*)r22 + 0x4);
        ((void(*)(void))fn_80102398)();
        return;
        L_8006BF10: ;
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) return;
        L_8006BF1C: ;
        r0 = *(s16*)((u8*)r31 + 0x14);
        if ((s32)r0 < (s32)0x0) {
            r3 = r25 + 0x7d8;
            r5 = r25 + 0x7e8;
            r4 = 0xe73;
            __assert();
        }
        r0 = *(u16*)((u8*)r26 + 0x4);
        r0 = r0 & 0x00000030;
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = *(u32*)((u8*)r22 + 0x4);
        r4 = 0x9fa;
        ((void(*)(void))fn_80102138)();
        r4 = r3;
        r3 = *(u32*)((u8*)r22 + 0x4);
        ((void(*)(void))fn_80102398)();
        return;
        L_8006BF64: ;
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) return;
        L_8006BF70: ;
        r0 = *(s16*)((u8*)r31 + 0x16);
        if ((s32)r0 < (s32)0x0) {
            r3 = r25 + 0x7d8;
            r5 = r25 + 0x808;
            r4 = 0xe7f;
            __assert();
        }
        r0 = *(u16*)((u8*)r26 + 0x4);
        r0 = r0 & 0x00000030;
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = *(u32*)((u8*)r22 + 0x4);
        r4 = 0x9fb;
        ((void(*)(void))fn_80102138)();
        r4 = r3;
        r3 = *(u32*)((u8*)r22 + 0x4);
        ((void(*)(void))fn_80102398)();
        return;
    }
    r0 = r27 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = *(u32*)((u8*)r22 + 0x4);
        r4 = 0x9fb;
        ((void(*)(void))fn_80102138)();
        r0 = (s8)r3;
        r4 = (u32)sp + 0x8;
        *(u8*)(sp + 0x15) = r0;
        r0 = *(u16*)(sp + 0x14);
        *(u16*)(sp + 0x8) = r0;
        r3 = *(u32*)((u8*)r22 + 0x4);
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r0 = r29 & 0xFF;
    if (r0 != (u32)0x0) return;
    L_8006BFFC: ;
    r3 = r22;
    ((void(*)(void))fn_80102F38)();

    return;
}


/* 0x8006C018 | size: 0xC4 */
void fn_8006C018(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        return;
    }
    ((void(*)(void))fn_80105624)();
    r31 = r3;
    r3 = *(u32*)((u8*)r30 + 0x4);
    ((void(*)(void))fn_801022B8)();
    if ((s32)r3 < (s32)0xa0e) {
        if ((s32)r3 < (s32)0x9fc) {
            if ((s32)r3 < (s32)0x9f7) {
                goto L_8006C0BC;
            }
            if ((s32)r3 < (s32)0xa0c) {
                goto L_8006C0BC;
            }
            if ((s32)r3 < (s32)0xe35) {
            }
            if ((s32)r3 < (s32)0xe33) {
            }
            goto L_8006C0BC;
                }
        r0 = *(u16*)((u8*)r31 + 0x4);
        r0 = r0 & 0x00000010;
        if ((s32)r0 != (s32)0x0) return;

            }
    r0 = *(u16*)((u8*)r31 + 0x4);
    r0 = r0 & 0x00000030;
    if ((s32)r0 != (s32)0x0) return;

    return;
    L_8006C0BC: ;
    r3 = r30;
    ((void(*)(void))fn_80102ED4)();

    return;
}


/* 0x8006C0DC | size: 0x88 */
void fn_8006C0DC(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        return;
    }
    ((void(*)(void))fn_80105624)();
    r31 = r3;
    r3 = *(u32*)((u8*)r30 + 0x4);
    ((void(*)(void))fn_801022B8)();
    do {
    if ((s32)r3 >= (s32)0x9d2) break;
    if ((s32)r3 < (s32)0x9ca) {
        break;
    }
    r0 = *(u16*)((u8*)r31 + 0x4);
    r0 = r0 & 0x00000010;
    if ((s32)r0 == (s32)0x0) break;
    return;

    } while (0);
    r3 = r30;
    ((void(*)(void))fn_80102ED4)();

    return;
}


/* 0x8006C164 | size: 0x474 */
void fn_8006C164(void) {
    extern void fn_80166A28();
    extern u8 jumptable_802EDE78[];
    extern u8 jumptable_802EDEFC[];
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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

    
    r24 = r3;
    r0 = *(u8*)((u8*)r24 + 0xA);
    if (r0 != (u32)0x0) return;
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x6);
    r3 = r0 & 0x1;
    r0 = -r3;
    r0 = r0 | r3;
    r27 = (u32)r0 >> 31;
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x6);
    r3 = r0 & 0x00000002;
    r0 = -r3;
    r0 = r0 | r3;
    r28 = (u32)r0 >> 31;
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x6);
    r3 = r0 & 0x00000004;
    r0 = -r3;
    r0 = r0 | r3;
    r29 = (u32)r0 >> 31;
    ((void(*)(void))fn_80105624)();
    r4 = *(u16*)((u8*)r3 + 0x6);
    r0 = r29 & 0xFF;
    r3 = 0x0;
    r4 = r4 & 0x00000008;
    r0 = -r4;
    r0 = r0 | r4;
    r30 = (u32)r0 >> 31;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 != (u32)0x0) {
        }
        r3 = 0x1;
        }
    r0 = r29 & 0xFF;
    r26 = r3 & 0xFF;
    r3 = 0x0;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 == (u32)0x0) {
        }
        r3 = 0x1;
        }
    r25 = r3 & 0xFF;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 == (u32)0x0) return;
        r3 = *(u32*)((u8*)r24 + 0x4);
        r4 = 0x9d2;
        ((void(*)(void))fn_80102138)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0x11) = r3;
        r4 = (u32)sp + 0xc;
        *(u8*)(sp + 0x10) = r0;
        r0 = *(u16*)(sp + 0x10);
        *(u16*)(sp + 0xC) = r0;
        r3 = *(u32*)((u8*)r24 + 0x4);
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r3 = r24;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r31 = r3;
    r3 = *(u32*)((u8*)r24 + 0x4);
    ((void(*)(void))fn_801022B8)();
    r0 = r28 & 0xFF;
    r5 = 0x0;
    if (r0 != (u32)0x0) {
        r5 = -0x1;
    } else {
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = 0x1;
        }
    }
    /* subi r0, r3, 0x9e2 */;
    if (r0 <= (u32)0x8) {
        r4 = (u32)jumptable_802EDEFC;
        r0 = r0 << 2;
        r4 = (u32)jumptable_802EDEFC;
        r0 = *(u32*)(r4 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = *(s16*)((u8*)r31 + 0x0);
        r0 = r0 + r5;
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x0) = r0;
        r0 = *(s16*)((u8*)r31 + 0x2);
        r4 = *(s16*)((u8*)r31 + 0x0);
        if ((s32)r0 < (s32)r4) {
            *(u16*)((u8*)r31 + 0x2) = r4;
        }
        /* case 1: handled above */
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = *(s16*)((u8*)r31 + 0x2);
        r0 = r0 + r5;
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x2) = r0;
        r4 = *(s16*)((u8*)r31 + 0x2);
        r0 = *(s16*)((u8*)r31 + 0x0);
        if ((s32)r4 < (s32)r0) {
            *(u16*)((u8*)r31 + 0x0) = r4;
        }
        /* case 2: handled above */
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = *(s16*)((u8*)r31 + 0x4);
        r0 = r0 + r5;
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x4) = r0;
        /* case 3: handled above */
    }
    r5 = 0x0;
    if ((s32)r5 != (s32)0x0) {
        r0 = *(s16*)((u8*)r31 + 0x0);
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x0) = r0;
        r0 = *(s16*)((u8*)r31 + 0x2);
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x2) = r0;
        r0 = *(s16*)((u8*)r31 + 0x0);
        r3 = *(s16*)((u8*)r31 + 0x4);
        r0 = r0 * 0x6;
        if ((s32)r0 <= (s32)r3) {
            r0 = *(s16*)((u8*)r31 + 0x2);
            r0 = r0 * 0x6;
            if ((s32)r0 >= (s32)r3) {
                r0 = r3;
            }
        }
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x4) = r0;
        return;
    }
    /* subi r0, r3, 0x9ca */;
    r3 = *(u16*)((u8*)r24 + 0x94);
    *(u16*)(sp + 0x14) = r3;
    if (r0 <= (u32)0x20) {
        r3 = (u32)jumptable_802EDE78;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EDE78;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            return;

            if (r0 != (u32)0x0) return;
            r0 = r29 & 0xFF;
            if (r0 == (u32)0x0) {
                r0 = r29 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    /* subi r0, r3, 0x1 */;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = r30 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    r0 = r3 + 0x1;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = *(u16*)(sp + 0x14);
                r4 = (u32)sp + 0x8;
                *(u16*)(sp + 0x8) = r0;
                r3 = *(u32*)((u8*)r24 + 0x4);
                ((void(*)(void))fn_801044D0)();
                return;
                if (r26 != (u32)0x0) {
                    r0 = *(u8*)((u8*)r31 + 0xC);
                    if (r0 != (u32)r25) {
                        r3 = 0x24;
                        fn_80166A28();
                    }
                    *(u8*)((u8*)r31 + 0xC) = r25;
                    return;

                    if (r0 != (u32)0x0) {
                        r3 = *(u32*)((u8*)r31 + 0x8);
                        /* subi r0, r3, 0x1 */;
                        *(u32*)((u8*)r31 + 0x8) = r0;
                        r0 = *(u32*)((u8*)r31 + 0x8);
                        if ((s32)r0 < (s32)0x0) {
                            r0 = 0x0;
                            *(u32*)((u8*)r31 + 0x8) = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                    }
                    r0 = r30 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r3 = *(u32*)((u8*)r31 + 0x8);
                        r0 = r3 + 0x1;
                        *(u32*)((u8*)r31 + 0x8) = r0;
                        r0 = *(u32*)((u8*)r31 + 0x8);
                        if ((s32)r0 >= (s32)0x3) {
                            r0 = 0x2;
                            *(u32*)((u8*)r31 + 0x8) = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                        if (r26 != (u32)0x0) {
                            r0 = *(u8*)((u8*)r31 + 0xD);
                            if (r0 != (u32)r25) {
                                r3 = 0x24;
                                fn_80166A28();
                            }
                            *(u8*)((u8*)r31 + 0xD) = r25;
                            return;
                            if (r26 != (u32)0x0) {
                                r0 = *(u8*)((u8*)r31 + 0xE);
                                if (r0 != (u32)r25) {
                                    r3 = 0x24;
                                    fn_80166A28();
                                }
                                *(u8*)((u8*)r31 + 0xE) = r25;
                                return;
                                if (r26 != (u32)0x0) {
                                    r0 = *(u8*)((u8*)r31 + 0xF);
                                    if (r0 != (u32)r25) {
                                        r3 = 0x24;
                                        fn_80166A28();
                                    }
                                    *(u8*)((u8*)r31 + 0xF) = r25;
                                    return;
                                    if (r26 != (u32)0x0) {
                                        r0 = *(u8*)((u8*)r31 + 0x10);
                                        if (r0 != (u32)r25) {
                                            r3 = 0x24;
                                            fn_80166A28();
                                        }
                                        *(u8*)((u8*)r31 + 0x10) = r25;
                                        return;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    r3 = r24;
    ((void(*)(void))fn_80102F38)();

    return;
}


/* 0x8006C5D8 | size: 0x1FC */
void fn_8006C5D8(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    
    r31 = r4;
    r3 = (u32)&lbl_80268674;
    r5 = *(s16*)((u8*)r31 + 0x54);
    r9 = (u32)&lbl_80268674;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r8 = *(u32*)((u8*)r9 + 0x0);
    r3 = 0x0;
    r7 = *(u32*)((u8*)r9 + 0x4);
    r4 = 0x0;
    r0 = *(u32*)((u8*)r9 + 0x8);
    *(u32*)(sp + 0x10) = r0;
    ((void(*)(void))fn_800FE38C)();
    r3 = 0x1;
    ((void(*)(void))fn_800D88DC)();
    r3 = 0x6;
    ((void(*)(void))fn_800D888C)();
    r5 = *(u8*)(sp + 0xF);
    r3 = (0x8081 << 16);
    r4 = *(u8*)((u8*)r31 + 0x67);
    /* subi r6, r3, 0x7f7f */;
    r0 = *(u8*)(sp + 0x13);
    r3 = 0x6;
    r5 = r5 * r4;
    r0 = r0 * r4;
    r7 = (s32)((s64)r6 * (s64)r5 >> 32);
    r4 = (s32)((s64)r6 * (s64)r0 >> 32);
    r5 = r7 + r5;
    r5 = (s32)r5 >> 7;
    r0 = r4 + r0;
    r6 = (u32)r5 >> 31;
    r0 = (s32)r0 >> 7;
    r4 = (u32)r0 >> 31;
    r5 = r5 + r6;
    r0 = r0 + r4;
    r4 = r5 & 0xFF;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xF) = r4;
    *(u8*)(sp + 0x13) = r0;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r3 = 0x4;
    ((void(*)(void))fn_800D67BC)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = *(s16*)((u8*)r31 + 0x54);
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = *(s16*)((u8*)r31 + 0x54);
    r4 = *(s16*)((u8*)r31 + 0x56);
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r4 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    ((void(*)(void))fn_800D6728)();
    f1 = *(f32*)&lbl_8047C060;
    ((void(*)(void))fn_800D5648)();
    r3 = 0x1;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r0 = *(u8*)((u8*)r31 + 0x67);
    r4 = 0xff;
    r3 = (0x8081 << 16);
    *(u8*)(sp + 0x8) = r4;
    r0 = r0 * 0x38;
    r30 = 0x0;
    /* subi r3, r3, 0x7f7f */;
    *(u8*)(sp + 0x9) = r4;
    r3 = (s32)((s64)r3 * (s64)r0 >> 32);
    *(u8*)(sp + 0xA) = r4;
    r0 = r3 + r0;
    r0 = (s32)r0 >> 7;
    r3 = (u32)r0 >> 31;
    r0 = r0 + r3;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r0;

    while ((s32)r3 < (s32)r0) {
        r3 = 0x2;
        ((void(*)(void))fn_800D67BC)();
        r4 = r30;
        r3 = 0x0;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        r3 = *(s16*)((u8*)r31 + 0x54);
        r4 = r30;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        ((void(*)(void))fn_800D6728)();
        r30 = r30 + 0x4;

    r0 = *(s16*)((u8*)r31 + 0x56);
    r3 = (s16)r30;
    }
    ((void(*)(void))fn_800FE35C)();
    return;
}


/* 0x8006C7D4 | size: 0x4EC */
void fn_8006C7D4(void) {
    extern void fn_8006A7E8();
    extern void fn_80129280();
    extern void fn_8012AA2C();
    extern void fn_8012AC3C();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern u8 jumptable_802EDF20[];
    u8 sp[0x1A0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
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
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r28 = r4;
    r3 = *(s16*)((u8*)r28 + 0x6);
    /* subi r0, r3, 0xec2 */;
    if (r0 > (u32)0x23) return;
    r3 = (u32)jumptable_802EDF20;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EDF20;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r0 = 0x2;
    r30 = 0x0;
    do {
    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;

    } while (0);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 == (s32)0x0) return;
            if ((s32)r0 < (s32)0x0) {
                return;
            }
            if ((s32)r0 >= (s32)0x4) return;
            goto L_8006CB80;
            }
        r29 = *(u32*)((u8*)r28 + 0x64);
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r4 = r30 * 0x1660;
        r0 = r4 + 0x64ec;
        r0 = r3 + r0;
        r3 = r0;
        fn_8012AC54();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r6 = 0xd0;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    r31 = 0x0;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r30 * 0x1660;
    r29 = r4 + 0x59a8;
    r29 = r3 + r29;
    r3 = r29;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x1) {
        if ((s32)r3 < (s32)0x1) {
            if ((s32)r3 < (s32)0x0) {
                goto L_8006CB08;
            }
            if ((s32)r3 >= (s32)0x3) goto L_8006CB08;
            goto L_8006CACC;
            }
        r0 = 0x0;
        goto L_8006CB0C;
    }
    r3 = r29 + 0xb44;
    fn_8012AA2C();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006CB08;
            }
            goto L_8006CB08;
            }
        r0 = 0x1;
        goto L_8006CB0C;
    }
    r0 = 0x2;
    goto L_8006CB0C;
    L_8006CACC: ;
    r3 = r29 + 0xb44;
    fn_8012AA2C();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006CB08;
            }
            goto L_8006CB08;
            }
        r0 = 0x3;
        goto L_8006CB0C;
    }
    r0 = 0x4;
    goto L_8006CB0C;
    L_8006CB08: ;
    r0 = 0x1;
    L_8006CB0C: ;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006CB5C;
                }
                if ((s32)r0 != (s32)0x4) {
                    if ((s32)r0 >= (s32)0x4) goto L_8006CB5C;
                    goto L_8006CB50;
                    }
                r31 = 0x29f;
                goto L_8006CB5C;
                    }
            r31 = 0x2a1;
            goto L_8006CB5C;
        }
        r31 = 0x2a2;
        goto L_8006CB5C;
        L_8006CB50: ;
        r31 = 0x2a3;

                } else {
    r31 = 0x2a0;
                }
    L_8006CB5C: ;
    if (r31 == (u32)0x0) return;
    r3 = r31;
    ((void(*)(void))fn_8005D858)();
    r0 = r3;
    r3 = r28;
    r4 = r0;
    ((void(*)(void))fn_80071318)();
    return;
    L_8006CB80: ;
    r31 = *(u32*)((u8*)r28 + 0x64);
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r4 = r30 * 0x1660;
    r0 = r4 + 0x64ec;
    r0 = r3 + r0;
    r3 = r0;
    fn_8012AC3C();
    r7 = (0xcccd << 16);
    r6 = (0x51ec << 16);
    r0 = r3 & 0xFFFF;
    r5 = (0x1062 << 16);
    /* subi r9, r7, 0x3333 */;
    /* subi r7, r6, 0x7ae1 */;
    r8 = (u32)((u64)r9 * (u64)r0 >> 32);
    r4 = (0xd1b7 << 16);
    r6 = r5 + 0x4dd3;
    r5 = r4 + 0x1759;
    r4 = (u32)&lbl_802686D0;
    r3 = (u32)sp + 0x10;
    r7 = (u32)((u64)r7 * (u64)r0 >> 32);
    r29 = r8;
    r30 = (u32)r8 >> 3;
    r29 = (u32)r29 >> 3;
    r4 = (u32)&lbl_802686D0;
    r6 = (u32)((u64)r6 * (u64)r0 >> 32);
    r12 = (u32)r7 >> 5;
    r5 = (u32)((u64)r5 * (u64)r0 >> 32);
    r11 = (u32)r6 >> 6;
    r8 = (u32)((u64)r9 * (u64)r30 >> 32);
    r10 = (u32)r5 >> 13;
    r7 = (u32)((u64)r9 * (u64)r12 >> 32);
    r8 = (u32)r8 >> 3;
    r6 = (u32)((u64)r9 * (u64)r11 >> 32);
    r7 = (u32)r7 >> 3;
    r5 = (u32)((u64)r9 * (u64)r10 >> 32);
    r6 = (u32)r6 >> 3;
    r9 = r29 * 0xa;
    r5 = (u32)r5 >> 3;
    r0 = r0 - r9;
    r9 = r0 & 0xFF;
    r0 = r7 * 0xa;
    *(u8*)(sp + 0x8) = r9;
    r8 = r8 * 0xa;
    r0 = r12 - r0;
    r7 = r30 - r8;
    r8 = r7 & 0xFF;
    r7 = r0 & 0xFF;
    *(u8*)(sp + 0x9) = r8;
    r0 = r5 * 0xa;
    *(u8*)(sp + 0xA) = r7;
    r6 = r6 * 0xa;
    r0 = r10 - r0;
    r5 = r11 - r6;
    r6 = r5 & 0xFF;
    r5 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r6;
    *(u8*)(sp + 0xC) = r5;
    /* crclr cr1eq */;
    ((void(*)(void))fn_800C8520)();
    r3 = (u32)sp + 0x90;
    r4 = (u32)sp + 0x10;
    ((void(*)(void))fn_800F9D04)();
    r4 = (u32)sp + 0x90;
    r3 = 0x37;
    fn_80132A38();
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xd0;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006CCC0 | size: 0x890 */
void fn_8006CCC0(void) {
    extern void fn_8010B718();
    extern void fn_8011E8DC();
    extern void fn_8011F4F0();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    extern void fn_8012AC08();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern void __assert();
    extern u8 jumptable_802EDFB0[];
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r23 = r3;
    r24 = r4;
    r31 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r0 = r3;
    r3 = r23;
    r30 = r0;
    r4 = 0x1;
    ((void(*)(void))fn_801040D0)();
    r21 = r3;
    r3 = r23;
    r4 = 0x2;
    ((void(*)(void))fn_801040D0)();
    r0 = r3;
    r3 = r23;
    r29 = r0;
    r4 = 0x3;
    ((void(*)(void))fn_801040D0)();
    r0 = *(u8*)((u8*)r23 + 0x1);
    r22 = r3;
    r28 = 0x0;
    r27 = 0x0;
    r0 = (s8)r0;
    r26 = 0x0;
    r25 = 0x0;
    if ((s32)r0 >= (s32)0x4) return;
    if ((s32)r0 == (s32)0x0) return;
    if ((s32)r0 < (s32)0x0) {
        return;


    }
    r0 = *(s16*)((u8*)r24 + 0x6);
    if ((s32)r0 != (s32)0xf2c) {
        if ((s32)r0 < (s32)0xf2c) {
            if ((s32)r0 != (s32)0xe79) {
                if ((s32)r0 < (s32)0xe79) {
                    if ((s32)r0 != (s32)0xe61) {
                        if ((s32)r0 < (s32)0xe61) {
                            if ((s32)r0 != (s32)0xe5e) {
                                if ((s32)r0 < (s32)0xe5e) {
                                    if ((s32)r0 != (s32)0xe5c) {
                                        if ((s32)r0 < (s32)0xe5c) {
                                            goto L_8006D2C8;
                                        }
                                        if ((s32)r0 < (s32)0xe60) {
                                            goto L_8006D054;
                                        }
                                        if ((s32)r0 == (s32)0xe76) goto L_8006D090;
                                        if ((s32)r0 < (s32)0xe76) {
                                            if ((s32)r0 == (s32)0xe74) goto L_8006D078;
                                            if ((s32)r0 >= (s32)0xe74) goto L_8006D084;
                                            goto L_8006D2C8;
                                        }
                                        if ((s32)r0 >= (s32)0xe78) goto L_8006D0A8;
                                        goto L_8006D09C;
                                    }
                                    if ((s32)r0 != (s32)0xe80) {
                                        if ((s32)r0 < (s32)0xe80) {
                                            if ((s32)r0 == (s32)0xe7d) goto L_8006D0E4;
                                            if ((s32)r0 < (s32)0xe7d) {
                                                if ((s32)r0 == (s32)0xe7b) goto L_8006D0CC;
                                                if ((s32)r0 >= (s32)0xe7b) goto L_8006D0D8;
                                                goto L_8006D0C0;
                                            }
                                            if ((s32)r0 >= (s32)0xe7f) goto L_8006D0FC;
                                            goto L_8006D0F0;
                                        }
                                        if ((s32)r0 != (s32)0xe84) {
                                            if ((s32)r0 < (s32)0xe84) {
                                                if ((s32)r0 != (s32)0xe82) {
                                                    if ((s32)r0 < (s32)0xe82) {
                                                        goto L_8006CFA4;
                                                    }
                                                }
                                                if ((s32)r0 >= (s32)0xe86) goto L_8006D2C8;
                                                goto L_8006D024;
                                            }
                                            if ((s32)r0 == (s32)0xf4a) goto L_8006D198;
                                            if ((s32)r0 < (s32)0xf4a) {
                                                if ((s32)r0 == (s32)0xf44) goto L_8006D150;
                                                if ((s32)r0 < (s32)0xf44) {
                                                    if ((s32)r0 == (s32)0xf30) goto L_8006D138;
                                                    if ((s32)r0 < (s32)0xf30) {
                                                        if ((s32)r0 == (s32)0xf2e) goto L_8006D120;
                                                        if ((s32)r0 >= (s32)0xf2e) goto L_8006D12C;
                                                        goto L_8006D114;
                                                    }
                                                    if ((s32)r0 < (s32)0xf32) {
                                                        goto L_8006D144;
                                                    }
                                                    if ((s32)r0 == (s32)0xf47) goto L_8006D174;
                                                    if ((s32)r0 < (s32)0xf47) {
                                                        if ((s32)r0 >= (s32)0xf46) goto L_8006D168;
                                                        goto L_8006D15C;
                                                    }
                                                    if ((s32)r0 >= (s32)0xf49) goto L_8006D18C;
                                                    goto L_8006D180;
                                                }
                                                if ((s32)r0 != (s32)0xfc9) {
                                                    if ((s32)r0 < (s32)0xfc9) {
                                                        if ((s32)r0 == (s32)0xf4e) goto L_8006D1C8;
                                                        if ((s32)r0 < (s32)0xf4e) {
                                                            if ((s32)r0 == (s32)0xf4c) goto L_8006D1B0;
                                                            if ((s32)r0 >= (s32)0xf4c) goto L_8006D1BC;
                                                            goto L_8006D1A4;
                                                        }
                                                    }
                                                    if ((s32)r0 >= (s32)0xf50) goto L_8006D2C8;
                                                    goto L_8006D1D4;
                                                }
                                                if ((s32)r0 == (s32)0x12bc) goto L_8006D1FC;
                                                if ((s32)r0 < (s32)0x12bc) {
                                                    if ((s32)r0 == (s32)0xfcb) goto L_8006D024;
                                                }
                                                if ((s32)r0 >= (s32)0xfcb) goto L_8006D2C8;
                                            }
                                            goto L_8006D1E0;
                                        }
                                                }
                                    if ((s32)r21 != (s32)0x0) {
                                        if ((s32)r21 < (s32)0x0) {
                                            goto L_8006CF18;
                                        }
                                        if ((s32)r21 >= (s32)0x3) goto L_8006CF18;

                                    } else {
                                    r0 = 0x3d8d;
                                    *(u32*)((u8*)r24 + 0x4C) = r0;
                                    goto L_8006D2C8;
                                    }
                                    r0 = 0x3d8f;
                                    *(u32*)((u8*)r24 + 0x4C) = r0;
                                    goto L_8006D2C8;
                                    L_8006CF18: ;
                                    r0 = 0x0;
                                    *(u32*)((u8*)r24 + 0x4C) = r0;
                                    goto L_8006D2C8;
                                    }
                                if (r22 != (u32)0x0) {
                                    r0 = *(u32*)((u8*)r22 + 0xC);
                                    if (r0 <= (u32)0x6) {
                                        r3 = (u32)jumptable_802EDFB0;
                                        r0 = r0 << 2;
                                        r3 = (u32)jumptable_802EDFB0;
                                        r0 = *(u32*)(r3 + r0);
                                        ctr_fn = (void(*)(void))r0;
                                        /* indirect jump via ctr */;
                                        r0 = 0x3d91;
                                        *(u32*)((u8*)r24 + 0x4C) = r0;
                                }
                                }
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;

                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;
                                L_8006CFA4: ;
                                if (r22 != (u32)0x0) {
                                    r0 = *(u32*)((u8*)r22 + 0x4);
                                    if ((s32)r0 != (s32)0x1) {
                                        if ((s32)r0 >= (s32)0x1) goto L_8006CFE0;
                                        if ((s32)r0 < (s32)0x0) {
                                            goto L_8006CFE0;
                                        }
                                        r0 = 0x3d9d;
                                        *(u32*)((u8*)r24 + 0x4C) = r0;
                                        goto L_8006CFF4;
                                    }
                                    r0 = 0x3d9e;
                                    *(u32*)((u8*)r24 + 0x4C) = r0;
                                    goto L_8006CFF4;
                                    L_8006CFE0: ;
                                    r3 = (u32)&lbl_80268680;
                                    r4 = 0xb47;
                                    r3 = (u32)&lbl_80268680;
                                    r5 = (u32)&lbl_8047C064;
                                    __assert();
                                    L_8006CFF4: ;
                                    r3 = r23;
                                    r4 = 0xe80;
                                    ((void(*)(void))fn_801046C8)();
                                    r22 = r3;
                                    r3 = *(u32*)((u8*)r22 + 0x4C);
                                    ((void(*)(void))fn_800FA444)();
                                    r0 = *(s16*)((u8*)r22 + 0x50);
                                    r3 = (u32)r3 >> 16;
                                    r0 = r0 + r3;
                                    r0 = (s16)r0;
                                    *(u16*)((u8*)r24 + 0x50) = r0;
                                }
                                goto L_8006D2C8;
                                L_8006D024: ;
                                r0 = 0x3d8b;
                                *(u32*)((u8*)r24 + 0x4C) = r0;
                                goto L_8006D2C8;
                                            }
                            r31 = 0x1;
                            r28 = 0x0;
                            goto L_8006D2C8;
                                            }
                        r31 = 0x1;
                        r28 = 0x1;
                        goto L_8006D2C8;
                                }
                    r31 = 0x1;
                    r28 = 0x2;
                    goto L_8006D2C8;
                    L_8006D054: ;
                    r31 = 0x1;
                    r28 = 0x3;
                    goto L_8006D2C8;
                                        }
                r31 = 0x1;
                r28 = 0x4;
                goto L_8006D2C8;
                    }
            r31 = 0x1;
            r28 = 0x5;
            goto L_8006D2C8;
            L_8006D078: ;
            r31 = 0x2;
            r28 = 0x0;
            goto L_8006D2C8;
            L_8006D084: ;
            r31 = 0x2;
            r28 = 0x1;
            goto L_8006D2C8;
            L_8006D090: ;
            r31 = 0x2;
            r28 = 0x2;
            goto L_8006D2C8;
            L_8006D09C: ;
            r31 = 0x2;
            r28 = 0x3;
            goto L_8006D2C8;
            L_8006D0A8: ;
            r31 = 0x2;
            r28 = 0x4;
            goto L_8006D2C8;
            }
        r31 = 0x2;
        r28 = 0x5;
        goto L_8006D2C8;
        L_8006D0C0: ;
        r31 = 0x3;
        r28 = 0x0;
        goto L_8006D2C8;
        L_8006D0CC: ;
        r31 = 0x3;
        r28 = 0x1;
        goto L_8006D2C8;
        L_8006D0D8: ;
        r31 = 0x3;
        r28 = 0x2;
        goto L_8006D2C8;
        L_8006D0E4: ;
        r31 = 0x3;
        r28 = 0x3;
        goto L_8006D2C8;
        L_8006D0F0: ;
        r31 = 0x3;
        r28 = 0x4;
        goto L_8006D2C8;
        L_8006D0FC: ;
        r31 = 0x3;
        r28 = 0x5;
        goto L_8006D2C8;
    }
    r31 = 0x1;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D114: ;
    r31 = 0x1;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D120: ;
    r31 = 0x1;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D12C: ;
    r31 = 0x1;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D138: ;
    r31 = 0x1;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D144: ;
    r31 = 0x1;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D150: ;
    r31 = 0x2;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D15C: ;
    r31 = 0x2;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D168: ;
    r31 = 0x2;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D174: ;
    r31 = 0x2;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D180: ;
    r31 = 0x2;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D18C: ;
    r31 = 0x2;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D198: ;
    r31 = 0x3;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D1A4: ;
    r31 = 0x3;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D1B0: ;
    r31 = 0x3;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D1BC: ;
    r31 = 0x3;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D1C8: ;
    r31 = 0x3;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D1D4: ;
    r31 = 0x3;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D1E0: ;
    if (r30 != (u32)0x0) {
        r3 = r30;
        fn_8012AC54();
        r31 = 0x4;
        r25 = r3;
    }
    goto L_8006D2C8;
    L_8006D1FC: ;
    if (r22 != (u32)0x0) {
        r0 = *(u32*)((u8*)r22 + 0x0);
        r21 = 0x0;
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 >= (s32)0x1) goto L_8006D280;
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D280;
            }
            r3 = *(u32*)((u8*)r22 + 0x14);
            if ((s32)r3 != (s32)0x6) {
                if ((s32)r3 < (s32)0x6) {
                    if ((s32)r3 < (s32)0x0) {
                        goto L_8006D280;
                    }
                    if ((s32)r3 >= (s32)0x8) goto L_8006D280;
                    goto L_8006D268;
                    }
                r4 = r3 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                r21 = 0x3d9f;
                goto L_8006D280;
            }
            r21 = 0x3da0;
            goto L_8006D280;
            L_8006D268: ;
            r21 = 0x3da1;
            goto L_8006D280;
        }
        r4 = *(u32*)((u8*)r22 + 0x14);
        r3 = 0x2f;
        fn_80132A38();
        r21 = 0x3da2;
        L_8006D280: ;
        r5 = *(u32*)((u8*)r24 + 0x64);
        r6 = r21;
        r3 = 0x0;
        r4 = 0x0;
        ((void(*)(void))fn_800FB680)();
        r3 = r21;
        ((void(*)(void))fn_800FA444)();
        r3 = (u32)r3 >> 16;
        r4 = *(u32*)((u8*)r22 + 0x18);
        r0 = r3 + 0x24;
        r3 = 0x2f;
        r21 = (s16)r0;
        fn_80132A38();
        r5 = *(u32*)((u8*)r24 + 0x64);
        r3 = r21;
        r4 = 0x0;
        r6 = 0x3da4;
        ((void(*)(void))fn_800FB680)();
    }
    L_8006D2C8: ;
    if ((s32)r31 == (s32)0x0) return;
    r3 = r30;
    r4 = r28;
    fn_8012AC08();
    r28 = r3;
    ((void(*)(void))fn_80077A5C)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r28 = 0x0;

    } else {
    r3 = r28;
    fn_80123FBC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r28 = 0x0;
        r26 = 0x1;
        r27 = 0x1;

    } else {
    r3 = r28;
    fn_8011E8DC();
    r27 = r3;
    }
    }
    if ((s32)r31 != (s32)0x3) {
        if ((s32)r31 < (s32)0x3) {
            if ((s32)r31 != (s32)0x1) {
                if ((s32)r31 < (s32)0x1) {
                    return;
                }
                if ((s32)r31 >= (s32)0x5) return;
                goto L_8006D514;
                }
            if (r28 == (u32)0x0) return;
            r3 = r23;
            r4 = r24;
            r5 = r28;
            fn_8010B718();
            return;
                }
        r0 = r26 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = *(u32*)((u8*)r24 + 0x64);
            r3 = 0x0;
            r4 = 0x0;
            r6 = 0x56c;
            ((void(*)(void))fn_800FB680)();
            return;
        }
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = *(u32*)((u8*)r24 + 0x64);
            r3 = 0x0;
            r4 = 0x0;
            r6 = 0x56b;
            ((void(*)(void))fn_800FB680)();
            return;
        }
        if (r28 == (u32)0x0) return;
        r3 = r28;
        fn_8011F4F0();
        if (r3 != (u32)0x0) {
            r0 = *(u16*)((u8*)r3 + 0x0);
            if (r0 != (u32)0x0) {
                r4 = r3;
                r3 = 0x37;
                fn_80132A38();
                r5 = *(u32*)((u8*)r24 + 0x64);
                r3 = 0x0;
                r4 = 0x0;
                r6 = 0xe7;
                ((void(*)(void))fn_800FB680)();
        }
        }
        r3 = r28;
        ((void(*)(void))fn_8001DA60)();
        r0 = r3 & 0xFF;
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 < (s32)0x1) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006D434;
                }
                goto L_8006D434;
                }
            r6 = 0xd67;
            goto L_8006D438;
        }
        r6 = 0xd68;
        goto L_8006D438;
        L_8006D434: ;
        r6 = 0x0;
        L_8006D438: ;
        if (r6 == (u32)0x0) return;
        r0 = *(u8*)((u8*)r24 + 0x67);
        if (r0 != (u32)0xff) return;
        r5 = *(u32*)((u8*)r24 + 0x64);
        r3 = 0x5c;
        r4 = 0x0;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    if (r28 == (u32)0x0) return;
    r0 = r27 & 0xFF;
    if (r0 != (u32)0x0) return;
    r21 = *(u32*)((u8*)r24 + 0x64);
    r3 = r28;
    r4 = 0x0;
    r5 = 0x7a;
    r6 = 0x0;
    fn_8012640C();
    r4 = r3;
    r3 = 0x2f;
    fn_80132A38();
    do {
    if (r29 == (u32)0x0) break;

    r3 = r28;
    r4 = r29;
    r5 = 0x0;
    ((void(*)(void))fn_800774D4)();
    r0 = r3 & 0xFF;
    do {
    if (r0 == (u32)0x0) break;

    r3 = r28;
    r4 = r29;
    r5 = 0x1;
    ((void(*)(void))fn_800774D4)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) break;

    r3 = r30;
    r4 = r29;
    r5 = 0x0;
    ((void(*)(void))fn_80076F2C)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) break;

    } while (0);

    r21 = r21 | (0xff00 << 16);
    r21 = r21 & 0xFF0000FF;
    } while (0);

    r5 = r21;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0x41fa;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006D514: ;
    if (r25 == (u32)0x0) return;
    r4 = r25;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(u32*)((u8*)r24 + 0x64);
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006D550 | size: 0x3F0 */
void fn_8006D550(void) {
    extern void fn_8006A7E8();
    extern void fn_8006B154();
    extern void fn_801091F4();
    extern void fn_80129280();
    extern void fn_8012AA2C();
    extern void fn_8012AC3C();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern void __assert();
    extern u8 jumptable_802EDFCC[];
    u8 sp[0x1B0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
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

    
    r31 = r4;
    r3 = r31;
    fn_801091F4();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = *(s16*)((u8*)r31 + 0x6);
    r28 = 0x0;
    /* subi r0, r3, 0xa4f */;
    if (r0 > (u32)0x27) return;
    r3 = (u32)jumptable_802EDFCC;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EDFCC;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r30 = 0x2;
    r26 = 0x0;
    do {
    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;

    } while (0);
    r27 = 0x0;
    r29 = 0x0;
    do {
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r0 = r29 + 0x59cc;
        r3 = *(u32*)(r3 + r0);
        fn_8006B154();
        if ((s32)r26 == (s32)r3) {
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r4 = r27 * 0x1660;
            r28 = r4 + 0x59a8;
            r28 = r3 + r28;
            break;
        }
        r29 = r29 + 0x1660;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x4);

    if (r28 == (u32)0x0) return;
    if ((s32)r30 != (s32)0x2) {
        if ((s32)r30 < (s32)0x2) {
            if ((s32)r30 < (s32)0x1) {
                goto L_8006D918;
            }
            if ((s32)r30 >= (s32)0x4) goto L_8006D918;
            goto L_8006D7F4;
            }
        r29 = *(u32*)((u8*)r31 + 0x64);
        r3 = r28 + 0xb44;
        fn_8012AC54();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    r31 = *(u32*)((u8*)r31 + 0x64);
    r3 = r28 + 0xb44;
    fn_8012AC3C();
    r7 = (0xcccd << 16);
    r6 = (0x51ec << 16);
    r0 = r3 & 0xFFFF;
    r5 = (0x1062 << 16);
    /* subi r9, r7, 0x3333 */;
    /* subi r7, r6, 0x7ae1 */;
    r8 = (u32)((u64)r9 * (u64)r0 >> 32);
    r4 = (0xd1b7 << 16);
    r6 = r5 + 0x4dd3;
    r5 = r4 + 0x1759;
    r4 = (u32)&lbl_802686D0;
    r3 = (u32)sp + 0x10;
    r7 = (u32)((u64)r7 * (u64)r0 >> 32);
    r29 = r8;
    r30 = (u32)r8 >> 3;
    r29 = (u32)r29 >> 3;
    r4 = (u32)&lbl_802686D0;
    r6 = (u32)((u64)r6 * (u64)r0 >> 32);
    r12 = (u32)r7 >> 5;
    r5 = (u32)((u64)r5 * (u64)r0 >> 32);
    r11 = (u32)r6 >> 6;
    r8 = (u32)((u64)r9 * (u64)r30 >> 32);
    r10 = (u32)r5 >> 13;
    r7 = (u32)((u64)r9 * (u64)r12 >> 32);
    r8 = (u32)r8 >> 3;
    r6 = (u32)((u64)r9 * (u64)r11 >> 32);
    r7 = (u32)r7 >> 3;
    r5 = (u32)((u64)r9 * (u64)r10 >> 32);
    r6 = (u32)r6 >> 3;
    r9 = r29 * 0xa;
    r5 = (u32)r5 >> 3;
    r0 = r0 - r9;
    r9 = r0 & 0xFF;
    r0 = r7 * 0xa;
    *(u8*)(sp + 0x8) = r9;
    r8 = r8 * 0xa;
    r0 = r12 - r0;
    r7 = r30 - r8;
    r8 = r7 & 0xFF;
    r7 = r0 & 0xFF;
    *(u8*)(sp + 0x9) = r8;
    r0 = r5 * 0xa;
    *(u8*)(sp + 0xA) = r7;
    r6 = r6 * 0xa;
    r0 = r10 - r0;
    r5 = r11 - r6;
    r6 = r5 & 0xFF;
    r5 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r6;
    *(u8*)(sp + 0xC) = r5;
    /* crclr cr1eq */;
    ((void(*)(void))fn_800C8520)();
    r3 = (u32)sp + 0x90;
    r4 = (u32)sp + 0x10;
    ((void(*)(void))fn_800F9D04)();
    r4 = (u32)sp + 0x90;
    r3 = 0x37;
    fn_80132A38();
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006D7F4: ;
    r3 = r28;
    r26 = 0x0;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x1) {
        if ((s32)r3 < (s32)0x1) {
            if ((s32)r3 < (s32)0x0) {
                goto L_8006D8A4;
            }
            if ((s32)r3 >= (s32)0x3) goto L_8006D8A4;
            goto L_8006D868;
            }
        r0 = 0x0;
        goto L_8006D8A8;
    }
    r3 = r28 + 0xb44;
    fn_8012AA2C();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D8A4;
            }
            goto L_8006D8A4;
            }
        r0 = 0x1;
        goto L_8006D8A8;
    }
    r0 = 0x2;
    goto L_8006D8A8;
    L_8006D868: ;
    r3 = r28 + 0xb44;
    fn_8012AA2C();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D8A4;
            }
            goto L_8006D8A4;
            }
        r0 = 0x3;
        goto L_8006D8A8;
    }
    r0 = 0x4;
    goto L_8006D8A8;
    L_8006D8A4: ;
    r0 = 0x1;
    L_8006D8A8: ;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006D8F8;
                }
                if ((s32)r0 != (s32)0x4) {
                    if ((s32)r0 >= (s32)0x4) goto L_8006D8F8;
                    goto L_8006D8EC;
                    }
                r26 = 0x2ba;
                goto L_8006D8F8;
                    }
            r26 = 0x2bc;
            goto L_8006D8F8;
        }
        r26 = 0x2b5;
        goto L_8006D8F8;
        L_8006D8EC: ;
        r26 = 0x2bb;

                } else {
    r26 = 0x2b4;
                }
    L_8006D8F8: ;
    if (r26 == (u32)0x0) return;
    r3 = r26;
    ((void(*)(void))fn_8005D858)();
    r4 = r3;
    r3 = r31;
    ((void(*)(void))fn_80071318)();
    return;
    L_8006D918: ;
    r3 = (u32)&lbl_80268680;
    r4 = 0xae6;
    r3 = (u32)&lbl_80268680;
    r5 = (u32)&lbl_8047C064;
    __assert();

    return;
}


/* 0x8006D940 | size: 0x4C */
void fn_8006D940(void) {
    u8 sp[0x10];
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r31 = r3;
    r3 = r30;
    r4 = 0xe8e;
    ((void(*)(void))fn_801046C8)();
    *(u32*)((u8*)r3 + 0x4C) = r31;
    return;
}


/* 0x8006D98C | size: 0x158 */
void fn_8006D98C(void) {
    extern void fn_80070D84();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r3 = r3 & 0xFF;
    if (r3 != (u32)0x0) {
        r28 = 0x6;
    } else {

        r28 = 0x3;
    }
    r0 = *(u8*)((u8*)r31 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) goto L_8006DAC0;
        if ((s32)r0 != (s32)0x0) {
            goto L_8006DAC0;
        }
        r0 = *(u8*)((u8*)r31 + 0x2);
        r0 = (s8)r0;
        if ((s32)r0 == (s32)0x0) {
            if (r3 != (u32)0x0) {
                r0 = 0x152;
            } else {

                r0 = 0x0;
            }
            r0 = (s16)r0;
            r3 = (u32)&lbl_8026864C;
            *(u16*)((u8*)r31 + 0x84) = r0;
            r30 = (u32)&lbl_8026864C;
            r27 = 0x0;
            do {
                r4 = *(u16*)((u8*)r30 + 0x0);
                r3 = r31;
                r29 = *(u32*)((u8*)r30 + 0x4);
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r3 + 0x4C) = r29;
                r30 = r30 + 0x8;
                r27 = r27 + 0x1;
            } while (r27 < (u32)0x5);
            r3 = (u32)&lbl_80267EA8;
            r27 = *(u32*)((u8*)r31 + 0x1C);
            r29 = r28 << 2;
            r30 = (u32)&lbl_80267EA8;
            while (r27 != (u32)0x0) {

                r0 = *(s16*)((u8*)r27 + 0x6);
                r3 = r31;
                r5 = *(u16*)(r30 + r29);
                r4 = r0 & 0xFFFF;
                ((void(*)(void))fn_801081F8)();
                r27 = *(u32*)((u8*)r27 + 0x0);

            }
        }

    } else {
    r0 = *(u8*)((u8*)r31 + 0x2);
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        r3 = (u32)&lbl_80267EA8;
        r4 = r28 << 2;
        r0 = (u32)&lbl_80267EA8;
        r27 = *(u32*)((u8*)r31 + 0x1C);
        r3 = r0 + r4;
        r29 = r3 + 0x2;
        while (r27 != (u32)0x0) {

            r0 = *(s16*)((u8*)r27 + 0x6);
            r3 = r31;
            r5 = *(u16*)((u8*)r29 + 0x0);
            r4 = r0 & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            r27 = *(u32*)((u8*)r27 + 0x0);

        }
    }
    }
    L_8006DAC0: ;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    return;
}


/* 0x8006DAE4 | size: 0x144 */
#pragma peephole off
void fn_8006DAE4(void* arg0) {
    extern void* fn_801040D0(void*, int);
    extern void* fn_801046C8(void*, int);
    extern void fn_80102868(int, int, s16);
    extern u32 fn_800FA444(void*);
    extern void fn_80109220(void*, int);
    extern void fn_80070D84(void*, void*, int);
    void* a;
    void* b;
    void* c;
    void* slot;
    s32 d29;
    s32 d27;
    s16 cmp;

    if ((s8)*(s8*)((u8*)arg0 + 0x2) == 0) {
        if ((s8)*(s8*)((u8*)arg0 + 0x1) == 0) {
            a = fn_801040D0(arg0, 0);
            b = fn_801040D0(arg0, 1);
            c = fn_801040D0(arg0, 2);
            fn_80102868(0xd6, 0, (s16)(s32)fn_801040D0(arg0, 3));
            slot = fn_801046C8(arg0, 0xe8c);
            *(u32*)((u8*)slot + 0x4c) = (u32)a;
            slot = fn_801046C8(arg0, 0xe8d);
            *(u32*)((u8*)slot + 0x4c) = (u32)b;
            cmp = (s16)(u16)fn_800FA444(c);
            d29 = 0;
            d27 = 0;
            if (cmp > 0x32) {
                d29 = -0x14;
                d27 = -0xa;
            }
            slot = fn_801046C8(arg0, 0xe8a);
            *(u32*)((u8*)slot + 0x4c) = (u32)c;
            *(s16*)((u8*)slot + 0x52) = *(s16*)((u8*)slot + 0x52) + d29;
            slot = fn_801046C8(arg0, 0xe87);
            fn_80109220(slot, c != 0);
            *(s16*)((u8*)slot + 0x52) = *(s16*)((u8*)slot + 0x52) + d27;
        }
    }
    fn_80070D84(arg0, lbl_8026860C, 8);
}
#pragma peephole reset


/* 0x8006DC28 | size: 0x4A4 */
void fn_8006DC28(void) {
    extern void fn_80070D84();
    extern void fn_80109220();
    extern void fn_8010B01C();
    extern void fn_8011F1A0();
    extern void fn_8011FC74();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    extern u32 fn_8006E128(u8* p);
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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

    
    r29 = r3;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r0 = r3;
    r3 = r29;
    r30 = r0;
    r4 = 0x1;
    ((void(*)(void))fn_801040D0)();
    r3 = r29;
    r4 = 0x2;
    ((void(*)(void))fn_801040D0)();
    r0 = *(u8*)((u8*)r29 + 0x2);
    r31 = r3;
    r0 = (s8)r0;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            if ((s32)r0 < (s32)0x0) {
                break;
            }
            r25 = 0x0;
            r26 = r25;
            r3 = (u32)&lbl_803B6D68;
            r23 = (u32)&lbl_803B6D68;
            do {
                r3 = r30;
                r4 = r26 & 0xFFFF;
                fn_8012AC08();
                r24 = r3;
                fn_80123FBC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r0 = r25;
                    r25 = r25 + 0x1;
                    r0 = r0 << 2;
                    *(u32*)(r23 + r0) = r24;
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            r4 = (u32)&lbl_803B6D68;
            r0 = r25 << 2;
            r5 = (u32)&lbl_803B6D68;
            r6 = 0x0;
            *(u32*)(r5 + r0) = r6;
            r3 = (u32)fn_8006E128;
            r4 = (u32)fn_8006E128;
            *(u32*)((u8*)r5 + 0x1C) = r6;
            r3 = 0x0;
            fn_8010B01C();
            r3 = (u32)&lbl_8026858C;
            r26 = 0x0;
            r27 = (u32)&lbl_8026858C;
            r3 = (0x2aab << 16);
            /* subi r28, r3, 0x5555 */;
            do {
                r4 = (s32)((s64)r28 * (s64)r26 >> 32);
                r3 = r30;
                r0 = (u32)r4 >> 31;
                r0 = r4 + r0;
                r0 = r0 * 0x6;
                r0 = r26 - r0;
                r4 = r0 & 0xFFFF;
                fn_8012AC08();
                r4 = 0x0;
                r25 = r3;
                ((void(*)(void))fn_80076398)();
                r0 = r3 & 0xFF;
                r23 = 0x0;
                r0 = __cntlzw(r0);
                r0 = (u32)r0 >> 5;
                r24 = r0 & 0xFF;
                if (r24 == (u32)0x0) {
                    r3 = r25;
                    ((void(*)(void))fn_80076334)();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x0) {
                    }
                    r23 = 0x1;
                    }
                r23 = r23 & 0xFF;
                if (r31 != (u32)0x0) {
                    if (r25 != (u32)0x0) {
                        r3 = r25;
                        fn_80123FBC();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x0) {
                            r3 = r25;
                            r4 = r31;
                            ((void(*)(void))fn_800772AC)();
                            r0 = r3 & 0xFF;
                            r0 = __cntlzw(r0);
                            r0 = (u32)r0 >> 5;
                            r0 = r23 | r0;
                            r23 = r0 & 0xFF;
                    }
                    }
                    r3 = r30;
                    r4 = r25;
                    r5 = r31;
                    r6 = 0x1;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r3 = r30;
                    r0 = __cntlzw(r0);
                    r4 = r25;
                    r0 = (u32)r0 >> 5;
                    r5 = r31;
                    r0 = r23 | r0;
                    r6 = 0x2;
                    r23 = r0 & 0xFF;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r3 = r30;
                    r0 = __cntlzw(r0);
                    r4 = r25;
                    r0 = (u32)r0 >> 5;
                    r5 = r31;
                    r0 = r23 | r0;
                    r6 = 0x3;
                    r23 = r0 & 0xFF;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r0 = __cntlzw(r0);
                    r0 = (u32)r0 >> 5;
                    r0 = r23 | r0;
                    r23 = r0 & 0xFF;
                }
                r4 = *(u16*)((u8*)r27 + 0x0);
                r3 = r29;
                ((void(*)(void))fn_801046C8)();
                r22 = r3;
                do {
                if (r22 == (u32)0x0) break;
                if (r24 != (u32)0x0) {
                    r3 = 0x375;
                    ((void(*)(void))fn_8005D858)();
                    r4 = r3;
                    r3 = r22;
                    ((void(*)(void))fn_80071318)();
                    break;
                }
                r0 = r23 & 0xFF;
                if (r0 == (u32)0x0) break;
                r3 = 0x25b;
                ((void(*)(void))fn_8005D858)();
                r4 = r3;
                r3 = r22;
                ((void(*)(void))fn_80071318)();
                } while (0);
                r4 = *(u16*)((u8*)r27 + 0x2);
                r3 = r29;
                ((void(*)(void))fn_801046C8)();
                r24 = r3;
                if (r24 != (u32)0x0) {
                    r0 = r23 & 0xFF;
                    r22 = 0x274;
                    if (r0 != (u32)0x0) {
                        r22 = 0x25c;
                    }
                    r3 = r25;
                    fn_8011FC74();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r22 = 0x341;
                    }
                    r3 = r22;
                    ((void(*)(void))fn_8005D858)();
                    r4 = r3;
                    r3 = r24;
                    ((void(*)(void))fn_80071318)();
                }
                r4 = *(u16*)((u8*)r27 + 0x4);
                r3 = r29;
                ((void(*)(void))fn_801046C8)();
                if (r3 != (u32)0x0) {
                    r4 = r23;
                    fn_80109220();
                }
                r27 = r27 + 0x8;
                r26 = r26 + 0x1;
            } while (r26 < (u32)0xc);
            r3 = (u32)&lbl_80267EA8;
            r24 = *(u32*)((u8*)r29 + 0x1C);
            r3 = (u32)&lbl_80267EA8;
            r23 = *(u16*)((u8*)r3 + 0xC);
            while (r24 != (u32)0x0) {

                r4 = r23;
                r3 = r24 + 0xc;
                ((void(*)(void))fn_80108518)();
                r24 = *(u32*)((u8*)r24 + 0x0);

            }
            break;
            }
        r3 = (u32)&lbl_8026858C;
        r23 = 0x0;
        r27 = (u32)&lbl_8026858C;
        do {
            r4 = *(u16*)((u8*)r27 + 0x6);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            r28 = r3;
            do {
            if (r28 == (u32)0x0) break;

            r4 = (0x2aab << 16);
            r3 = r30;
            /* subi r0, r4, 0x5555 */;
            r4 = (s32)((s64)r0 * (s64)r23 >> 32);
            r0 = (u32)r4 >> 31;
            r0 = r4 + r0;
            r0 = r0 * 0x6;
            r0 = r23 - r0;
            r4 = r0 & 0xFFFF;
            fn_8012AC08();
            r22 = r3;
            do {
            if (r22 == (u32)0x0) break;

            fn_80123FBC();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) break;

            r3 = r22;
            fn_8011F1A0();
            r24 = 0x0;
            r25 = r3;
            ((void(*)(void))fn_80077C1C)();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = r25;
                ((void(*)(void))fn_80077C68)();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r24 = 0x1;
            }
            }
            r24 = r24 & 0xFF;
            if (r31 != (u32)0x0) {
                r3 = r30;
                r4 = r22;
                r5 = r31;
                r6 = 0x2;
                ((void(*)(void))fn_80076A8C)();
                r0 = r24 & r3;
                r24 = r0 & 0xFF;
            }
            r4 = r25 & 0xFFFF;
            r3 = r28;
            r0 = -r4;
            r0 = r0 | r4;
            r4 = (u32)r0 >> 31;
            fn_80109220();
            r0 = r24 & 0xFF;
            r3 = *(u32*)((u8*)r28 + 0x64);
            r3 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r0 = -0x100;
            } else {

                r0 = (0xff00 << 16);
            }
            r0 = r3 | r0;
            *(u32*)((u8*)r28 + 0x64) = r0;
            break;

            } while (0);

            r3 = r28;
            r4 = 0x0;
            fn_80109220();
            } while (0);

            r27 = r27 + 0x8;
            r23 = r23 + 0x1;
        } while (r23 < (u32)0xc);
        break;
    }
    r3 = (u32)&lbl_80267EA8;
    r22 = *(u32*)((u8*)r29 + 0x1C);
    r3 = (u32)&lbl_80267EA8;
    r23 = *(u16*)((u8*)r3 + 0x1A);
    while (r22 != (u32)0x0) {

        r4 = r23;
        r3 = r22 + 0xc;
        ((void(*)(void))fn_80108518)();
        r22 = *(u32*)((u8*)r22 + 0x0);

    }
    } while (0);
    r3 = r29;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    return;
}


/* 0x8006E0CC | size: 0x5C */
void fn_8006E0CC(void) {
    extern void fn_8010BBB8();
    extern void fn_8010BCE4();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r31 = 0;

    r3 = (u32)&lbl_803B6D68;
    r31 = (u32)&lbl_803B6D68;
    goto L_8006E108;
    do {
        fn_8010BBB8();
        fn_8010BCE4();
        r0 = (s8)r3;
        if ((s32)r0 == (s32)0x0) {
            ((void(*)(void))_threadSwitch)();

        } else {
        r31 = r31 + 0x4;
        }
        L_8006E108: ;
        r3 = *(u32*)((u8*)r31 + 0x0);
    } while (r3 != (u32)0x0);
    return;
}


/* 0x8006E128 | size: 0x38 */
u32 fn_8006E128(u8* p) {
    u32 index;

    if (p == NULL) {
        return 0;
    }
    index = *(u32*)(p + 0x1C);
    if (index >= 7) {
        return 0;
    }
    *(u32*)(p + 0x1C) = index + 1;
    return *(u32*)(p + index * 4);
}


/* 0x8006E160 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006E160(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006E188 | size: 0x4 */
void fn_8006E188(void) {
}

/* 0x8006E18C | size: 0xCC */
void fn_8006E18C(void) {
    extern void fn_80070D84();
    extern void fn_80109220();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r3 = (u32)&lbl_80268574;
    r30 = 0x0;
    r31 = (u32)&lbl_80268574;
    do {
        r4 = *(u16*)((u8*)r31 + 0x0);
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r4 = *(u8*)((u8*)r29 + 0x95);
        r0 = *(u8*)((u8*)r31 + 0x2);
        r4 = (s8)r4;
        r0 = r0 - r4;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        fn_80109220();
        r31 = r31 + 0x4;
        r30 = r30 + 0x1;
    } while (r30 < (u32)0x6);
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {

    } else {
    ((void(*)(void))fn_80071160)();
    if ((s32)r3 != (s32)0x0) {
        r3 = *(u32*)((u8*)r29 + 0x4);
        r4 = 0x1ce;
        ((void(*)(void))fn_80107F38)();
        r0 = 0x1;
        *(u8*)((u8*)r29 + 0x98) = r0;
        *(u8*)((u8*)r29 + 0x99) = r0;
        return;
    }
    }
    r3 = r29;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();

    return;
}


/* 0x8006E258 | size: 0xE0 */
void fn_8006E258(void) {
    extern void fn_8006B154();
    extern void fn_80109220();
    extern void fn_80129280();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r3 = *(u32*)((u8*)r3 + 0x59CC);
    fn_8006B154();
    r4 = (u32)&lbl_80268560;
    r29 = r3;
    r26 = (u32)&lbl_80268560;
    r25 = 0x0;
    do {
        r0 = r29 - r25;
        r28 = r26;
        r0 = __cntlzw(r0);
        r27 = (u32)sp + 0x8;
        r0 = (u32)r0 >> 5;
        r23 = 0x0;
        r24 = r0 & 0xFF;
        do {
            r4 = *(u16*)((u8*)r28 + 0x0);
            r3 = r31;
            ((void(*)(void))fn_801046C8)();
            r30 = r3;
            r4 = r24;
            fn_80109220();
            *(u32*)((u8*)r27 + 0x0) = r30;
            r28 = r28 + 0x2;
            r27 = r27 + 0x4;
            r23 = r23 + 0x1;
        } while (r23 < (u32)0x5);
        if (r24 != (u32)0x0) {
            r0 = 0x424b;
        } else {

            r0 = 0x0;
        }
        *(u32*)((u8*)r3 + 0x4C) = r0;
        if (r24 != (u32)0x0) {
            r0 = 0x3f40;
        } else {

            r0 = 0x0;
        }
        r26 = r26 + 0xa;
        r25 = r25 + 0x1;
        *(u32*)((u8*)r3 + 0x4C) = r0;
    } while (r25 < (u32)0x2);
    return;
}


/* 0x8006E338 | size: 0x460 */
void fn_8006E338(void) {
    extern void fn_8006A7E8();
    extern void fn_8006B154();
    extern void fn_80070D84();
    extern void fn_80109220();
    extern void fn_80129280();
    extern void __assert();
    u8 sp[0xB0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
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

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x2);
    r4 = (u32)&lbl_80267EA8;
    r3 = *(u32*)&lbl_8047E708;
    r28 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    r31 = 0x1;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            break;
        }
        r26 = r28 + 0x698;
        r27 = r28 + 0x628;
        r21 = 0x0;
        r25 = r28 + 0x0;
        do {
            r0 = *(u32*)((u8*)r26 + 0x0);
            r24 = r27;
            r22 = 0x0;
            r23 = r0 << 2;
            do {
                r4 = *(u16*)((u8*)r24 + 0x0);
                r3 = r30;
                ((void(*)(void))fn_801046C8)();
                r4 = *(u16*)(r25 + r23);
                r3 = r3 + 0xc;
                ((void(*)(void))fn_80108518)();
                r24 = r24 + 0x2;
                r22 = r22 + 0x1;
            } while (r22 < (u32)0xe);
            r26 = r26 + 0x4;
            r27 = r27 + 0x1c;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0x4);
        break;
    }
    r26 = r28 + 0x698;
    r27 = r28 + 0x628;
    r22 = 0x0;
    r23 = r28 + 0x0;
    do {
        r0 = *(u32*)((u8*)r26 + 0x0);
        r24 = r27;
        r21 = 0x0;
        r3 = r0 << 2;
        r25 = r3 + 0x2;
        do {
            r4 = *(u16*)((u8*)r24 + 0x0);
            r3 = r30;
            ((void(*)(void))fn_801046C8)();
            r4 = *(u16*)(r23 + r25);
            r3 = r3 + 0xc;
            ((void(*)(void))fn_80108518)();
            r24 = r24 + 0x2;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0xe);
        r26 = r26 + 0x4;
        r27 = r27 + 0x1c;
        r22 = r22 + 0x1;
    } while (r22 < (u32)0x4);
    } while (0);
    r24 = 0x0;
    r26 = r28 + 0x6a8;
    r29 = r24;
    r27 = (u32)&lbl_8047C058;
    do {
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        r0 = r29 + 0x59cc;
        r3 = *(u32*)(r3 + r0);
        fn_8006B154();
        if ((s32)r3 >= (s32)0x0) {
            r0 = r3 * 0x1c;
            r23 = r28 + 0x628;
            r4 = (u32)sp + 0x8;
            r5 = 0x1;
            *(u8*)(r4 + r3) = r5;
            r23 = r23 + r0;
            r25 = (u32)sp + 0x44;
            r21 = 0x0;
            do {
                r4 = *(u16*)((u8*)r23 + 0x0);
                r3 = r30;
                ((void(*)(void))fn_801046C8)();
                *(u32*)((u8*)r25 + 0x0) = r3;
                r23 = r23 + 0x2;
                r25 = r25 + 0x4;
                r21 = r21 + 0x1;
            } while (r21 < (u32)0xe);
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r0 = r29 + 0x59a8;
            r3 = r3 + r0;
            fn_8006A7E8();
            if ((s32)r3 != (s32)0x0) {
                r3 = 0x2b2;
            } else {

                r3 = 0x2ae;
            }
            ((void(*)(void))fn_8005D858)();
            r0 = r3;
            r4 = r0;
            ((void(*)(void))fn_80071318)();
            r3 = *(u16*)((u8*)r27 + 0x0);
            ((void(*)(void))fn_8005D858)();
            r4 = r3;
            ((void(*)(void))fn_80071318)();
            r3 = 0x0;
            r5 = *(u32*)((u8*)r26 + 0x0);
            r4 = 0xe;
            r0 = *(u32*)((u8*)r6 + 0x64);
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            *(u32*)((u8*)r6 + 0x64) = r0;
            r0 = *(u32*)((u8*)r8 + 0x64);
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            *(u32*)((u8*)r8 + 0x64) = r0;
            r0 = *(u32*)((u8*)r7 + 0x64);
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            *(u32*)((u8*)r7 + 0x64) = r0;
            fn_80129280();
            r0 = r29 + 0x7005;
            r0 = *(u8*)(r3 + r0);
            r31 = r31 & r0;
        }
        r29 = r29 + 0x1660;
        r27 = r27 + 0x2;
        r26 = r26 + 0x4;
        r24 = r24 + 0x1;
    } while ((s32)r24 < (s32)0x4);
    r0 = r31 & 0xFF;
    r26 = r28 + 0x628;
    r0 = __cntlzw(r0);
    r27 = (u32)sp + 0x8;
    r29 = (u32)r0 >> 5;
    r24 = 0x0;
    r25 = (u32)sp + 0xc;
    do {
        r23 = r26;
        r22 = r25;
        r21 = 0x0;
        do {
            r4 = *(u16*)((u8*)r23 + 0x0);
            r3 = r30;
            ((void(*)(void))fn_801046C8)();
            *(u32*)((u8*)r22 + 0x0) = r3;
            r23 = r23 + 0x2;
            r22 = r22 + 0x4;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0xe);
        r0 = *(u8*)((u8*)r27 + 0x0);
        if (r0 != (u32)0x0) {
            r23 = 0x0;
            r22 = r23;
            L_8006E5E8: ;
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r0 = r22 + 0x59cc;
            r3 = *(u32*)(r3 + r0);
            fn_8006B154();
            if ((s32)r24 != (s32)r3) {
                r22 = r22 + 0x1660;
                r23 = r23 + 0x1;
                if ((s32)r23 < (s32)0x4) goto L_8006E5E8;
            }
            if (r23 >= (u32)0x4) {
                r3 = r28 + 0x7d8;
                r5 = r28 + 0x83c;
                r4 = 0x8a1;
                __assert();
            }
            r3 = 0x0;
            r4 = 0xe;
            fn_80129280();
            r4 = r23 * 0x1660;
            r0 = r4 + 0x7005;
            r21 = *(u8*)(r3 + r0);
            if (r21 != (u32)0x0) {
                r0 = 0x0;
            } else {

                r0 = 0x3f3f;
            }
            *(u32*)((u8*)r3 + 0x4C) = r0;
            if (r21 != (u32)0x0) {
                r0 = 0x0;
            } else {

                r0 = 0x3f40;
            }
            r4 = r29 & 0xFF;
            *(u32*)((u8*)r5 + 0x4C) = r0;
            fn_80109220();
            r0 = __cntlzw(r21);
            r23 = (u32)r0 >> 5;
            r4 = r23 & 0xFF;
            fn_80109220();
            r4 = r23 & 0xFF;
            fn_80109220();
            r0 = 0x0;
            if ((r21 != (u32)0x0) && ((s32)r29 != (s32)0x0)) {

                r0 = 0x1;
            }
            r4 = r0 & 0xFF;
            fn_80109220();

        } else {
        r22 = r25;
        r23 = 0x0;
        do {
            r3 = *(u32*)((u8*)r22 + 0x0);
            r4 = 0x0;
            fn_80109220();
            r22 = r22 + 0x4;
            r23 = r23 + 0x1;
        } while (r23 < (u32)0xe);
        }
        r26 = r26 + 0x1c;
        r27 = r27 + 0x1;
        r24 = r24 + 0x1;
    } while (r24 < (u32)0x4);
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x59CC);
    if ((s32)r0 != (s32)0x1) {
        r3 = 0x2ae;
        ((void(*)(void))fn_8005D858)();
        r4 = r28 + 0x628;
        r21 = r3;
        r4 = *(u16*)((u8*)r4 + 0x8);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = r21;
        r21 = r3;
        ((void(*)(void))fn_80071318)();
        r3 = r21;
        r4 = 0x1;
        fn_80109220();
        r4 = r28 + 0x628;
        r3 = r30;
        r4 = *(u16*)((u8*)r4 + 0xC);
        ((void(*)(void))fn_801046C8)();
        r4 = 0x1;
        fn_80109220();
    }
    r3 = r30;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    *(u8*)((u8*)r30 + 0x98) = r31;
    return;
}


/* 0x8006E798 | size: 0x20C */
void fn_8006E798(void) {
    extern void fn_8006A7E8();
    extern void fn_80070D84();
    extern void fn_80109220();
    extern void fn_80129280();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x2);
    r3 = (u32)&lbl_80267EA8;
    r31 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            break;
        }
        r28 = r31 + 0x5f4;
        r3 = r31 + 0x0;
        r27 = 0x0;
        r29 = *(u16*)((u8*)r3 + 0x18);
        do {
            r4 = *(u16*)((u8*)r28 + 0x0);
            r3 = r30;
            ((void(*)(void))fn_801046C8)();
            r4 = r29;
            r3 = r3 + 0xc;
            ((void(*)(void))fn_80108518)();
            r28 = r28 + 0x2;
            r27 = r27 + 0x1;
        } while (r27 < (u32)0x7);
        break;
    }
    r28 = r31 + 0x5f4;
    r3 = r31 + 0x0;
    r27 = 0x0;
    r29 = *(u16*)((u8*)r3 + 0x1A);
    do {
        r4 = *(u16*)((u8*)r28 + 0x0);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = r29;
        r3 = r3 + 0xc;
        ((void(*)(void))fn_80108518)();
        r28 = r28 + 0x2;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x7);
    } while (0);
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x4);
    do {
        if ((s32)r0 == (s32)0x2 || (s32)r0 >= (s32)0x2) break;

        if ((s32)r0 < (s32)0x0) {
            break;
        }
        r27 = 0x0;
        goto L_8006E88C;
    } while (0);
    r27 = 0x2;
    L_8006E88C: ;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r3 = r3 + 0x59a8;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x0) {
        r0 = 0x1;
    } else {

        r0 = 0x0;
    }
    r3 = r30;
    r27 = r27 + r0;
    r4 = 0x99b;
    ((void(*)(void))fn_801046C8)();
    r0 = r27 << 3;
    r28 = r31 + 0x5d4;
    r29 = 0x0;
    r27 = r3;
    r28 = r28 + r0;
    do {
        r4 = *(u8*)((u8*)r28 + 0x0);
        r3 = r27;
        fn_80109220();
        r27 = *(u32*)((u8*)r27 + 0x0);
        r28 = r28 + 0x1;
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x8);
    r3 = r30;
    r4 = 0x9a7;
    ((void(*)(void))fn_801046C8)();
    r0 = 0x3d2c;
    r4 = 0x9a9;
    *(u32*)((u8*)r3 + 0x4C) = r0;
    r3 = r30;
    ((void(*)(void))fn_801046C8)();
    r0 = 0x3d26;
    r4 = 0x9a6;
    *(u32*)((u8*)r3 + 0x4C) = r0;
    r3 = r30;
    ((void(*)(void))fn_801046C8)();
    r29 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x8);
    r5 = r31 + 0x604;
    r3 = r30;
    r4 = 0x9a8;
    r0 = r0 << 2;
    r0 = *(u32*)(r5 + r0);
    *(u32*)((u8*)r29 + 0x4C) = r0;
    ((void(*)(void))fn_801046C8)();
    r29 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x4);
    r6 = r31 + 0x61c;
    r3 = r30;
    r4 = 0x0;
    r0 = r0 << 2;
    r5 = 0x0;
    r0 = *(u32*)(r6 + r0);
    *(u32*)((u8*)r29 + 0x4C) = r0;
    fn_80070D84();
    return;
}


/* 0x8006E9A4 | size: 0x4D8 */
void fn_8006E9A4(void) {
    extern void fn_8006B3C8();
    extern void fn_801091F4();
    extern void fn_80132A38();
    u8 sp[0x910];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r31 = r4;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) return;


    r3 = r31;
    fn_801091F4();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r0 = *(s16*)((u8*)r31 + 0x6);
    r5 = 0x0;
    if ((s32)r0 != (s32)0xd8e) {
        if ((s32)r0 < (s32)0xd8e) {
            if ((s32)r0 != (s32)0x969) {
                if ((s32)r0 < (s32)0x969) {
                    if ((s32)r0 != (s32)0x966) {
                        if ((s32)r0 < (s32)0x966) {
                            if ((s32)r0 != (s32)0x964) {
                                if ((s32)r0 < (s32)0x964) {
                                    goto L_8006EE30;
                                }
                                if ((s32)r0 < (s32)0x968) {
                                    goto L_8006ECA0;
                                }
                                if ((s32)r0 != (s32)0xa0f) {
                                    if ((s32)r0 < (s32)0xa0f) {
                                        if ((s32)r0 < (s32)0xa0e) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 < (s32)0xd8d) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 != (s32)0xd94) {
                                            if ((s32)r0 < (s32)0xd94) {
                                                if ((s32)r0 != (s32)0xd91) {
                                                    if ((s32)r0 < (s32)0xd91) {
                                                        if ((s32)r0 < (s32)0xd90) {
                                                            goto L_8006EAF8;
                                                        }
                                                        if ((s32)r0 >= (s32)0xd93) goto L_8006EB48;
                                                        goto L_8006EB40;
                                                    }
                                                    if ((s32)r0 == (s32)0xda0) goto L_8006EBFC;
                                                    if ((s32)r0 < (s32)0xda0) {
                                                        if ((s32)r0 >= (s32)0xd96) goto L_8006EE30;
                                                        goto L_8006EB88;
                                                    }
                                                    if ((s32)r0 >= (s32)0xda2) goto L_8006EE30;
                                                    goto L_8006EBD0;
                                                    }
                                                r4 = (0x51ec << 16);
                                                r0 = *(s16*)((u8*)r3 + 0x0);
                                                /* subi r3, r4, 0x7ae1 */;
                                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                                r0 = (s32)r0 >> 5;
                                                r3 = (u32)r0 >> 31;
                                                r5 = r0 + r3;
                                                goto L_8006EE30;
                                            }
                                            r4 = (0x6666 << 16);
                                            r0 = *(s16*)((u8*)r3 + 0x0);
                                            r3 = r4 + 0x6667;
                                            r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                            r0 = (s32)r0 >> 2;
                                            r3 = (u32)r0 >> 31;
                                            r5 = r0 + r3;
                                            goto L_8006EE30;
                                            L_8006EAF8: ;
                                            r5 = *(s16*)((u8*)r3 + 0x0);
                                            goto L_8006EE30;
                                                        }
                                        r4 = (0x51ec << 16);
                                        r0 = *(s16*)((u8*)r3 + 0x2);
                                        /* subi r3, r4, 0x7ae1 */;
                                        r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                        r0 = (s32)r0 >> 5;
                                        r3 = (u32)r0 >> 31;
                                        r5 = r0 + r3;
                                        goto L_8006EE30;
                                                }
                                    r4 = (0x6666 << 16);
                                    r0 = *(s16*)((u8*)r3 + 0x2);
                                    r3 = r4 + 0x6667;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 2;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                    L_8006EB40: ;
                                    r5 = *(s16*)((u8*)r3 + 0x2);
                                    goto L_8006EE30;
                                    L_8006EB48: ;
                                    r4 = (0x51ec << 16);
                                    r0 = *(s16*)((u8*)r3 + 0x4);
                                    /* subi r3, r4, 0x7ae1 */;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 5;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                        }
                                r4 = (0x6666 << 16);
                                r0 = *(s16*)((u8*)r3 + 0x4);
                                r3 = r4 + 0x6667;
                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                r0 = (s32)r0 >> 2;
                                r3 = (u32)r0 >> 31;
                                r5 = r0 + r3;
                                goto L_8006EE30;
                                L_8006EB88: ;
                                r5 = *(s16*)((u8*)r3 + 0x4);
                                goto L_8006EE30;
                                    }
                            r0 = *(s16*)((u8*)r3 + 0x14);
                            r3 = (0x6666 << 16);
                            r4 = r3 + 0x6667;
                            r3 = (s32)r0 >> 31;
                            r0 = r3 ^ r0;
                            r0 = r0 - r3;
                            r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                            r0 = (s32)r0 >> 2;
                            r3 = (u32)r0 >> 31;
                            r5 = r0 + r3;
                            goto L_8006EE30;
                                        }
                        r3 = *(s16*)((u8*)r3 + 0x14);
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                        L_8006EBD0: ;
                        r0 = *(s16*)((u8*)r3 + 0x16);
                        r3 = (0x6666 << 16);
                        r4 = r3 + 0x6667;
                        r3 = (s32)r0 >> 31;
                        r0 = r3 ^ r0;
                        r0 = r0 - r3;
                        r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                        r0 = (s32)r0 >> 2;
                        r3 = (u32)r0 >> 31;
                        r5 = r0 + r3;
                        goto L_8006EE30;
                        L_8006EBFC: ;
                        r3 = *(s16*)((u8*)r3 + 0x16);
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                            }
                    r31 = *(u32*)((u8*)r31 + 0x64);
                    r3 = (u32)sp + 0x288;
                    r4 = (u32)&lbl_8047C068;
                    r5 = 0x32;
                    /* crclr cr1eq */;
                    ((void(*)(void))fn_800C8520)();
                    r3 = (u32)sp + 0x808;
                    r4 = (u32)sp + 0x288;
                    ((void(*)(void))fn_800F9D04)();
                    r4 = (u32)sp + 0x808;
                    r3 = 0x37;
                    fn_80132A38();
                    r5 = r31;
                    r3 = 0xa;
                    r4 = 0x0;
                    r6 = 0xcf;
                    ((void(*)(void))fn_800FB680)();
                    return;
                                }
                r31 = *(u32*)((u8*)r31 + 0x64);
                r3 = (u32)sp + 0x208;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))fn_800C8520)();
                r3 = (u32)sp + 0x708;
                r4 = (u32)sp + 0x208;
                ((void(*)(void))fn_800F9D04)();
                r4 = (u32)sp + 0x708;
                r3 = 0x37;
                fn_80132A38();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                L_8006ECA0: ;
                r31 = *(u32*)((u8*)r31 + 0x64);
                r3 = (u32)sp + 0x188;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))fn_800C8520)();
                r3 = (u32)sp + 0x608;
                r4 = (u32)sp + 0x188;
                ((void(*)(void))fn_800F9D04)();
                r4 = (u32)sp + 0x608;
                r3 = 0x37;
                fn_80132A38();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                            }
            r31 = *(u32*)((u8*)r31 + 0x64);
            r3 = 0x3;
            fn_8006B3C8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r5 = 0x32;
            } else {

                r5 = -0x1;
            }
            if ((s32)r5 >= (s32)0x0) {
                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C068;
                /* crclr cr1eq */;
                ((void(*)(void))fn_800C8520)();
            } else {

                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C070;
                /* crclr cr1eq */;
                ((void(*)(void))fn_800C8520)();
            }
            r3 = (u32)sp + 0x508;
            r4 = (u32)sp + 0x108;
            ((void(*)(void))fn_800F9D04)();
            r4 = (u32)sp + 0x508;
            r3 = 0x37;
            fn_80132A38();
            r5 = r31;
            r3 = 0xa;
            r4 = 0x0;
            r6 = 0xcf;
            ((void(*)(void))fn_800FB680)();
            return;
                                }
        r31 = *(u32*)((u8*)r31 + 0x64);
        r3 = (u32)sp + 0x88;
        r4 = (u32)&lbl_8047C068;
        r5 = 0x64;
        /* crclr cr1eq */;
        ((void(*)(void))fn_800C8520)();
        r3 = (u32)sp + 0x408;
        r4 = (u32)sp + 0x88;
        ((void(*)(void))fn_800F9D04)();
        r4 = (u32)sp + 0x408;
        r3 = 0x37;
        fn_80132A38();
        r5 = r31;
        r3 = 0xa;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
                            }
    r31 = *(u32*)((u8*)r31 + 0x64);
    r3 = 0x5;
    fn_8006B3C8();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r5 = 0x64;
    } else {

        r5 = -0x1;
    }
    if ((s32)r5 >= (s32)0x0) {
        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C068;
        /* crclr cr1eq */;
        ((void(*)(void))fn_800C8520)();
    } else {

        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C070;
        /* crclr cr1eq */;
        ((void(*)(void))fn_800C8520)();
    }
    r3 = (u32)sp + 0x308;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_800F9D04)();
    r4 = (u32)sp + 0x308;
    r3 = 0x37;
    fn_80132A38();
    r5 = r31;
    r3 = 0xa;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006EE30: ;
    r4 = (0xcccd << 16);
    r3 = 0x34;
    /* subi r0, r4, 0x3333 */;
    r0 = (u32)((u64)r0 * (u64)r5 >> 32);
    r0 = (u32)r0 >> 3;
    r0 = r0 * 0xa;
    r4 = r5 - r0;
    fn_80132A38();
    r5 = *(u32*)((u8*)r31 + 0x64);
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xc9;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006EE7C | size: 0xA8 */
void fn_8006EE7C(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {

    } else {
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x4);
    r0 = r0 & 0x00000010;
    if ((s32)r0 != (s32)0x0) {
        r3 = *(u8*)((u8*)r30 + 0x95);
        r0 = *(u8*)((u8*)r30 + 0x94);
        r3 = (s8)r3;
        r0 = (s8)r0;
        r31 = r3 + r0;
        if ((s32)r31 < (s32)0x3c) {
            r3 = r30;
            r4 = 0x0;
            ((void(*)(void))fn_801040D0)();
            r4 = *(u8*)(r3 + r31);
            r0 = 0x0;
            r4 = __cntlzw(r4);
            r4 = (u32)r4 >> 5;
            r4 = r4 & 0xFF;
            *(u8*)(r3 + r31) = r4;
            *(u8*)((u8*)r30 + 0x98) = r0;
            return;
    }
    }
    }
    r3 = r30;
    ((void(*)(void))fn_80102ED4)();

    return;
}


/* 0x8006EF24 | size: 0xD4 */
void fn_8006EF24(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = *(u8*)((u8*)r31 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        goto L_8006EFDC;
    }
    r0 = *(u8*)((u8*)r31 + 0x95);
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        ((void(*)(void))fn_80105624)();
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x1;
        if ((s32)r0 == (s32)0x0) goto L_8006EFDC;
        r3 = *(u8*)((u8*)r31 + 0x94);
        /* subi r0, r3, 0x1 */;
        *(u8*)((u8*)r31 + 0x94) = r0;
        r0 = *(u8*)((u8*)r31 + 0x94);
        r0 = (s8)r0;
        if ((s32)r0 >= (s32)0x0) return;
        r0 = 0x0;
        *(u8*)((u8*)r31 + 0x94) = r0;
        return;
    }
    if ((s32)r0 == (s32)0xa) {
        ((void(*)(void))fn_80105624)();
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x00000002;
        if ((s32)r0 != (s32)0x0) {
            r3 = *(u8*)((u8*)r31 + 0x94);
            r0 = r3 + 0x1;
            *(u8*)((u8*)r31 + 0x94) = r0;
            r0 = *(u8*)((u8*)r31 + 0x94);
            r0 = (s8)r0;
            if ((s32)r0 <= (s32)0x32) return;
            r0 = 0x32;
            *(u8*)((u8*)r31 + 0x94) = r0;
            return;
    }
    }
    L_8006EFDC: ;
    r3 = r31;
    ((void(*)(void))fn_80102F38)();

    return;
}


/* 0x8006EFF8 | size: 0x28C */
void fn_8006EFF8(void) {
    extern void fn_80109220();
    extern void fn_80142984();
    extern void fn_80144088();
    extern void fn_801440A0();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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

    
    r31 = r3;
    r4 = 0x0;
    ((void(*)(void))fn_801040D0)();
    r28 = r3;
    r3 = r31;
    r4 = 0xa40;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0x94);
    r4 = (s8)r0;
    r0 = -r4;
    r0 = r0 & ~r4;
    r4 = (u32)r0 >> 31;
    fn_80109220();
    r3 = r31;
    r4 = 0xa3f;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0x94);
    r5 = 0x32;
    r0 = (s8)r0;
    r0 = r5 ^ r0;
    r4 = (s32)r0 >> 1;
    r0 = r0 & r5;
    r0 = r4 - r0;
    r4 = (u32)r0 >> 31;
    fn_80109220();
    r3 = (u32)&lbl_80268424;
    r25 = 0x0;
    r27 = (u32)&lbl_80268424;
    do {
        r4 = *(u16*)((u8*)r27 + 0x0);
        r3 = r31;
        ((void(*)(void))fn_801046C8)();
        r4 = *(u16*)((u8*)r27 + 0x2);
        r29 = r3;
        r3 = r31;
        ((void(*)(void))fn_801046C8)();
        r4 = *(u16*)((u8*)r27 + 0x4);
        r30 = r3;
        r3 = r31;
        ((void(*)(void))fn_801046C8)();
        r4 = *(u16*)((u8*)r27 + 0x6);
        r24 = r3;
        r3 = r31;
        ((void(*)(void))fn_801046C8)();
        r0 = *(u8*)((u8*)r31 + 0x94);
        r23 = r3;
        r0 = (s8)r0;
        r26 = r25 + r0;
        if ((s32)r26 == (s32)0x3c) {
            r3 = 0x43e6;
            r0 = 0x0;
            *(u32*)((u8*)r29 + 0x4C) = r3;
            *(u32*)((u8*)r30 + 0x4C) = r0;

        } else {
        r3 = r26;
        ((void(*)(void))fn_80077D88)();
        r22 = r3;
        fn_80142984();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = r22;
            fn_801440A0();
            fn_80144088();
            *(u32*)((u8*)r29 + 0x4C) = r3;
        } else {

            r0 = 0x12e;
            *(u32*)((u8*)r29 + 0x4C) = r0;
        }
        r0 = *(u8*)(r28 + r26);
        if (r0 != (u32)0x0) {
            if (r0 != (u32)0x0) {
                r0 = 0x3d6f;
            } else {

                r0 = 0x3d68;
            }
            r3 = (0xff00 << 16);
            *(u32*)((u8*)r30 + 0x4C) = r0;
            r0 = r3 + 0xff;
            *(u32*)((u8*)r30 + 0x64) = r0;

        } else {
        if (r0 != (u32)0x0) {
            r0 = 0x3d6f;
        } else {

            r0 = 0x3d68;
        }
        *(u32*)((u8*)r30 + 0x4C) = r0;
        r0 = -0x1;
        *(u32*)((u8*)r30 + 0x64) = r0;
        }
        }
        r4 = *(u8*)(r28 + r26);
        r3 = r24;
        fn_80109220();
        r0 = *(u8*)((u8*)r31 + 0x95);
        r3 = r23;
        r0 = (s8)r0;
        r0 = r0 - r25;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        fn_80109220();
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0xb);
    r0 = *(u8*)((u8*)r31 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = *(u8*)((u8*)r31 + 0x2);
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267EA8;
        r24 = *(u32*)((u8*)r31 + 0x1C);
        r3 = (u32)&lbl_80267EA8;
        r23 = *(u16*)((u8*)r3 + 0x18);
        while (r24 != (u32)0x0) {

            r0 = *(s16*)((u8*)r24 + 0x6);
            r3 = r31;
            r5 = r23;
            r4 = r0 & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            r24 = *(u32*)((u8*)r24 + 0x0);

        }
        r3 = r31;
        r23 = 0x424a;
        r4 = 0xe4c;
        ((void(*)(void))fn_801046C8)();
        *(u32*)((u8*)r3 + 0x4C) = r23;
        return;
    }
    r0 = *(u8*)((u8*)r31 + 0x2);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267EA8;
    r24 = *(u32*)((u8*)r31 + 0x1C);
    r3 = (u32)&lbl_80267EA8;
    r23 = *(u16*)((u8*)r3 + 0x1A);
    while (r24 != (u32)0x0) {

        r0 = *(s16*)((u8*)r24 + 0x6);
        r3 = r31;
        r5 = r23;
        r4 = r0 & 0xFFFF;
        ((void(*)(void))fn_801081F8)();
        r24 = *(u32*)((u8*)r24 + 0x0);

    }
    r0 = 0x1;
    *(u8*)((u8*)r31 + 0x2) = r0;

    return;
}


/* 0x8006F284 | size: 0x49C */
void fn_8006F284(void) {
    extern void fn_80070D84();
    extern void fn_80109220();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = *(u8*)((u8*)r31 + 0x1);
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                goto L_8006F370;
            }
            goto L_8006F370;
            }
        r26 = r30 + 0x4ec;
        r25 = 0x0;
        do {
            r4 = *(u16*)((u8*)r26 + 0x0);
            r3 = r31;
            ((void(*)(void))fn_801046C8)();
            r0 = *(u32*)((u8*)r26 + 0x4);
            r26 = r26 + 0x8;
            r25 = r25 + 0x1;
            *(u32*)((u8*)r3 + 0x4C) = r0;
        } while (r25 < (u32)0x10);
        ((void(*)(void))fn_80077BD0)();
        r0 = r3 & 0xFF;
        r3 = 0x9fc;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r25 = r0 & 0xFF;
        r4 = r25;
        ((void(*)(void))fn_8005D8F8)();
        if (r25 == (u32)0x0) {
            r3 = *(u32*)((u8*)r31 + 0x4);
            r4 = 0x9fd;
            ((void(*)(void))fn_80102138)();
            r3 = (s8)r3;
            r0 = 0x0;
            *(u8*)(sp + 0xD) = r3;
            r4 = (u32)sp + 0x8;
            *(u8*)(sp + 0xC) = r0;
            r0 = *(u16*)(sp + 0xC);
            *(u16*)(sp + 0x8) = r0;
            r3 = *(u32*)((u8*)r31 + 0x4);
            ((void(*)(void))fn_801044D0)();
            r3 = r31;
            ((void(*)(void))fn_80104CA0)();
        }

    } else {
    r3 = *(u32*)((u8*)r31 + 0x4);
    ((void(*)(void))fn_801022B8)();
    if ((s32)r3 == (s32)0xe35) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
    }
    }
    L_8006F370: ;
    r3 = *(u32*)((u8*)r31 + 0x4);
    ((void(*)(void))fn_801022B8)();
    if ((s32)r3 < (s32)0x9fe) {
        if ((s32)r3 != (s32)0x9f9) {
            if ((s32)r3 < (s32)0x9f9) {
                if ((s32)r3 != (s32)0x9f7) {
                    if ((s32)r3 < (s32)0x9f7) {
                        goto L_8006F400;
                    }
                    if ((s32)r3 == (s32)0x9fb) goto L_8006F3F8;
                    if ((s32)r3 < (s32)0x9fb) {
                        goto L_8006F3F0;
                    }
                    if ((s32)r3 < (s32)0xe33) {
                        if ((s32)r3 < (s32)0xa0e) {
                            if ((s32)r3 >= (s32)0xa0c) goto L_8006F3F0;
                    }
                    }
                    goto L_8006F400;
                }
                if ((s32)r3 == (s32)0xe35 || (s32)r3 >= (s32)0xe35) goto L_8006F400;

                goto L_8006F3F8;
                    }
            r25 = 0x0;
            goto L_8006F404;
                    }
        r25 = 0x1;
        goto L_8006F404;
        }
    r25 = 0x2;
    goto L_8006F404;
    L_8006F3F0: ;
    r25 = 0x3;
    goto L_8006F404;
    L_8006F3F8: ;
    r25 = 0x4;
    goto L_8006F404;
    L_8006F400: ;
    r25 = 0x5;
    L_8006F404: ;
    r27 = r30 + 0x4e0;
    r28 = 0x0;
    do {
        r4 = *(u16*)((u8*)r27 + 0x0);
        r3 = r31;
        ((void(*)(void))fn_801046C8)();
        r29 = r3;
        r26 = 0x0;
        if ((s32)r25 == (s32)r28) {
            ((void(*)(void))fn_80077BD0)();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r26 = 0x1;
        }
        }
        r4 = r26 & 0xFF;
        r3 = r29;
        fn_80109220();
        r3 = 0x191;
        ((void(*)(void))fn_8005D830)();
        r0 = *(u32*)((u8*)r29 + 0xC);
        if (r0 != (u32)r3) {
            r4 = *(u16*)((u8*)r27 + 0x0);
            r3 = r31;
            r5 = 0x191;
            ((void(*)(void))fn_801081F8)();
        }
        r27 = r27 + 0x2;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x5);
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    r3 = r31;
    r0 = __cntlzw(r0);
    r4 = 0x0;
    r0 = (u32)r0 >> 5;
    r29 = r0 & 0xFF;
    ((void(*)(void))fn_801040D0)();
    r5 = *(s16*)((u8*)r3 + 0x14);
    r4 = 0xa06;
    r0 = *(s16*)((u8*)r3 + 0x16);
    r5 = (u32)r5 >> 31;
    r25 = *(u8*)((u8*)r3 + 0x11);
    r0 = (u32)r0 >> 31;
    r26 = *(u8*)((u8*)r3 + 0x12);
    r5 = r5 ^ 0x1;
    r24 = *(u8*)((u8*)r3 + 0x13);
    r0 = r0 ^ 0x1;
    r3 = r31;
    r28 = r5 & 0xFF;
    r27 = r0 & 0xFF;
    ((void(*)(void))fn_801046C8)();
    r4 = r25;
    fn_80109220();
    r3 = r31;
    r4 = 0xa07;
    ((void(*)(void))fn_801046C8)();
    r0 = __cntlzw(r25);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r31;
    r4 = 0xa08;
    ((void(*)(void))fn_801046C8)();
    r4 = r26;
    fn_80109220();
    r3 = r31;
    r4 = 0xa09;
    ((void(*)(void))fn_801046C8)();
    r0 = __cntlzw(r26);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r31;
    r4 = 0xa0a;
    ((void(*)(void))fn_801046C8)();
    r4 = r24;
    fn_80109220();
    r3 = r31;
    r4 = 0xa0b;
    ((void(*)(void))fn_801046C8)();
    r0 = __cntlzw(r24);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r31;
    r4 = 0x11a2;
    ((void(*)(void))fn_801046C8)();
    r25 = r3;
    r26 = 0x0;
    if (r28 != (u32)0x0) {
        r3 = *(u32*)((u8*)r31 + 0x4);
        ((void(*)(void))fn_801022B8)();
        if ((s32)r3 != (s32)0xa0c) {
            r3 = *(u32*)((u8*)r31 + 0x4);
            ((void(*)(void))fn_801022B8)();
            if ((s32)r3 != (s32)0xa0d) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    fn_80109220();
    r3 = r31;
    r4 = 0x11a3;
    ((void(*)(void))fn_801046C8)();
    r0 = __cntlzw(r28);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r31;
    r4 = 0x11a4;
    ((void(*)(void))fn_801046C8)();
    r25 = r3;
    r26 = 0x0;
    if (r27 != (u32)0x0) {
        r3 = *(u32*)((u8*)r31 + 0x4);
        ((void(*)(void))fn_801022B8)();
        if ((s32)r3 != (s32)0xe34) {
            r3 = *(u32*)((u8*)r31 + 0x4);
            ((void(*)(void))fn_801022B8)();
            if ((s32)r3 != (s32)0xe33) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    fn_80109220();
    r3 = r31;
    r4 = 0x11a5;
    ((void(*)(void))fn_801046C8)();
    r0 = __cntlzw(r27);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r31;
    r4 = 0xa16;
    ((void(*)(void))fn_801046C8)();
    r4 = r29;
    fn_80109220();
    r3 = r31;
    r4 = 0xd9f;
    ((void(*)(void))fn_801046C8)();
    r4 = r29;
    fn_80109220();
    r3 = r31;
    r4 = 0xa15;
    ((void(*)(void))fn_801046C8)();
    if (r29 != (u32)0x0) {
        r0 = 0x3d7a;
    } else {

        r0 = 0x3d79;
    }
    *(u32*)((u8*)r3 + 0x4C) = r0;
    r3 = r31;
    r4 = 0x11a6;
    ((void(*)(void))fn_801046C8)();
    r25 = r3;
    r26 = 0x0;
    if (r29 != (u32)0x0) {
        if (r28 != (u32)0x0) {
            r3 = *(u32*)((u8*)r31 + 0x4);
            ((void(*)(void))fn_801022B8)();
            if ((s32)r3 == (s32)0x9fa) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    fn_80109220();
    r3 = r31;
    r4 = 0x11a7;
    ((void(*)(void))fn_801046C8)();
    r26 = r3;
    r28 = 0x0;
    if (r29 != (u32)0x0) {
        if (r27 != (u32)0x0) {
            r3 = *(u32*)((u8*)r31 + 0x4);
            ((void(*)(void))fn_801022B8)();
            if ((s32)r3 == (s32)0x9fb) {
                r28 = 0x1;
    }
    }
    }
    r4 = r28 & 0xFF;
    r3 = r26;
    fn_80109220();
    r3 = r31;
    r4 = r30 + 0x56c;
    r5 = 0x2;
    fn_80070D84();
    return;
}


/* 0x8006F720 | size: 0x4DC */
void fn_8006F720(void) {
    extern void fn_80070D84();
    extern void fn_80109220();
    extern void fn_80129280();
    extern u8 jumptable_802EE06C[];
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
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

    
    r29 = r3;
    r5 = (u32)&lbl_80267EA8;
    r4 = 0x0;
    r30 = (u32)&lbl_80267EA8;
    ((void(*)(void))fn_801040D0)();
    r31 = r3;
    r28 = r30 + 0x3dc;
    r26 = 0x0;
    do {
        r4 = *(u16*)((u8*)r28 + 0x0);
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r0 = *(u32*)((u8*)r28 + 0x4);
        r28 = r28 + 0x8;
        r26 = r26 + 0x1;
        *(u32*)((u8*)r3 + 0x4C) = r0;
    } while (r26 < (u32)0x1a);
    r3 = *(u32*)((u8*)r29 + 0x4);
    ((void(*)(void))fn_801022B8)();
    /* subi r0, r3, 0x9ca */;
    if (r0 <= (u32)0x20) {
        r3 = (u32)jumptable_802EE06C;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EE06C;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r25 = 0x0;


    } else {
    r25 = 0x8;
    }
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r28 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = *(u16*)((u8*)r28 + 0x0);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            r4 = 0x0;
            fn_80109220();
            r28 = r28 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    } else {

        r27 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = *(u16*)((u8*)r27 + 0x0);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            r0 = r26 - r25;
            r28 = r3;
            r0 = __cntlzw(r0);
            r0 = (u32)r0 >> 5;
            r4 = r0 & 0xFF;
            fn_80109220();
            r3 = 0x191;
            ((void(*)(void))fn_8005D830)();
            r0 = *(u32*)((u8*)r28 + 0xC);
            if (r0 != (u32)r3) {
                r4 = *(u16*)((u8*)r27 + 0x0);
                r3 = r29;
                r5 = 0x191;
                ((void(*)(void))fn_801081F8)();
            }
            r27 = r27 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    }
    r3 = r29;
    r4 = 0xd80;
    ((void(*)(void))fn_801046C8)();
    r4 = *(u8*)((u8*)r31 + 0xC);
    fn_80109220();
    r3 = r29;
    r4 = 0x9eb;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0xC);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9ec;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u32*)((u8*)r31 + 0x8);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9ed;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u32*)((u8*)r31 + 0x8);
    r0 = 0x1 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9ee;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u32*)((u8*)r31 + 0x8);
    r0 = 0x2 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9ef;
    ((void(*)(void))fn_801046C8)();
    r4 = *(u8*)((u8*)r31 + 0xD);
    fn_80109220();
    r3 = r29;
    r4 = 0x9f0;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0xD);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9f1;
    ((void(*)(void))fn_801046C8)();
    r4 = *(u8*)((u8*)r31 + 0xE);
    fn_80109220();
    r3 = r29;
    r4 = 0x9f2;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0xE);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9f3;
    ((void(*)(void))fn_801046C8)();
    r4 = *(u8*)((u8*)r31 + 0xF);
    fn_80109220();
    r3 = r29;
    r4 = 0x9f4;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0xF);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9f5;
    ((void(*)(void))fn_801046C8)();
    r4 = *(u8*)((u8*)r31 + 0x10);
    fn_80109220();
    r3 = r29;
    r4 = 0x8a2;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r31 + 0x10);
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0x9d4;
    ((void(*)(void))fn_801046C8)();
    r0 = *(s16*)((u8*)r31 + 0x2);
    r6 = *(s16*)((u8*)r31 + 0x4);
    r0 = r0 * 0x6;
    r5 = (s32)r6 >> 31;
    r4 = (u32)r0 >> 31;
    r0 = r6 - r0;
    r0 = r5 + r4; /* +carry */;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0xfb0;
    ((void(*)(void))fn_801046C8)();
    r28 = 0x0;
    r26 = r3;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = *(u32*)((u8*)r29 + 0x4);
        ((void(*)(void))fn_801022B8)();
        if ((s32)r3 == (s32)0x9cd) {
            r0 = *(u32*)((u8*)r31 + 0x8);
            if ((s32)r0 == (s32)0x2) {
                r28 = 0x1;
    }
    }
    }
    r4 = r28 & 0xFF;
    r3 = r26;
    fn_80109220();
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x0);
    if ((s32)r0 == (s32)0x0) {
        r27 = r30 + 0x4ac;
        r28 = 0x0;
        do {
            r4 = *(u16*)((u8*)r27 + 0x0);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            r4 = 0x0;
            fn_80109220();
            r27 = r27 + 0x2;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x12);
        r3 = r29;
        r4 = 0xd9b;
        ((void(*)(void))fn_801046C8)();
        r0 = 0x4238;
        r4 = 0xd9c;
        *(u32*)((u8*)r3 + 0x4C) = r0;
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r0 = 0x0;
        r4 = 0x9d3;
        *(u32*)((u8*)r3 + 0x4C) = r0;
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r0 = 0x4238;
        r4 = 0xd9a;
        *(u32*)((u8*)r3 + 0x4C) = r0;
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r0 = 0x0;
        r4 = 0x9d4;
        *(u32*)((u8*)r3 + 0x4C) = r0;
        r3 = r29;
        ((void(*)(void))fn_801046C8)();
        r0 = 0x0;
        *(u32*)((u8*)r3 + 0x4C) = r0;
    }
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            goto L_8006FBD8;
        }
        goto L_8006FBD8;
    }
    r3 = *(u32*)((u8*)r29 + 0x4);
    ((void(*)(void))fn_801022B8)();
    if ((s32)r3 == (s32)0x9d2) {
        r0 = 0x1;
        *(u8*)((u8*)r29 + 0x98) = r0;
    }
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x4);
    r0 = r0 & 0x00000010;
    if ((s32)r0 == (s32)0x0) goto L_8006FBD8;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = *(u32*)((u8*)r29 + 0x4);
        r4 = 0x9d2;
        ((void(*)(void))fn_80102138)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0xD) = r3;
        r4 = (u32)sp + 0x8;
        *(u8*)(sp + 0xC) = r0;
        r0 = *(u16*)(sp + 0xC);
        *(u16*)(sp + 0x8) = r0;
        r3 = *(u32*)((u8*)r29 + 0x4);
        ((void(*)(void))fn_801044D0)();

    } else {
    r0 = *(u8*)((u8*)r29 + 0x95);
    r3 = 0x0;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x3) {
        r0 = *(u32*)((u8*)r31 + 0x8);
        if ((s32)r0 == (s32)0x2) {
            r3 = 0x1;
    }
    }
    r0 = r3 & 0xFF;
    *(u8*)((u8*)r29 + 0x98) = r0;
    }
    L_8006FBD8: ;
    r3 = r29;
    r4 = r30 + 0x4d0;
    r5 = 0x2;
    fn_80070D84();
    return;
}


/* 0x8006FBFC | size: 0xFC */
void fn_8006FBFC(void) {
    extern void fn_80070D84();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r4 = 0x9bb;
    ((void(*)(void))fn_801046C8)();
    r0 = *(u8*)((u8*)r29 + 0x95);
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x3) {
        r0 = 0x3dc0;
    } else {

        r0 = 0x3dc1;
    }
    *(u32*)((u8*)r3 + 0x4C) = r0;
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                goto L_8006FCC8;
            }
            goto L_8006FCC8;
            }
        r3 = (u32)&lbl_80268234;
        r30 = 0x0;
        r31 = (u32)&lbl_80268234;
        do {
            r4 = *(u16*)((u8*)r31 + 0x0);
            r3 = r29;
            ((void(*)(void))fn_801046C8)();
            r0 = *(u32*)((u8*)r31 + 0x4);
            r31 = r31 + 0x8;
            r30 = r30 + 0x1;
            *(u32*)((u8*)r3 + 0x4C) = r0;
        } while (r30 < (u32)0x8);

    } else {
    ((void(*)(void))fn_80105624)();
    r0 = *(u16*)((u8*)r3 + 0x4);
    r0 = r0 & 0x00000400;
    if ((s32)r0 != (s32)0x0) {
        r0 = *(u8*)((u8*)r29 + 0x95);
        r0 = (s8)r0;
        if ((s32)r0 < (s32)0x6) {
            r0 = 0x1;
            *(u8*)((u8*)r29 + 0x98) = r0;
    }
    }
    }
    L_8006FCC8: ;
    r4 = (u32)&lbl_802681B4;
    r3 = r29;
    r4 = (u32)&lbl_802681B4;
    r5 = 0x10;
    fn_80070D84();
    return;
}


/* 0x8006FCF8 | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8006FCF8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80268184, 0x6);
}
#pragma pop

/* 0x8006FD24 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD24(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD4C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD4C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD74 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD74(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD9C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD9C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDC4 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDC4(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDEC | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDEC(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE14 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE14(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE3C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE3C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE64 | size: 0x80 */
void fn_8006FE64(void) {
    extern void fn_8006B3C8();
    extern void fn_80166A28();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x95);
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x6) {
        r31 = r0;
        ((void(*)(void))fn_80105624)();
        r0 = *(u16*)((u8*)r3 + 0x4);
        r0 = r0 & 0x00000010;
        if ((s32)r0 != (s32)0x0) {
            r3 = r31;
            fn_8006B3C8();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r3 = 0x26;
                fn_80166A28();
                return;
    }
    }
    }
    r3 = r30;
    ((void(*)(void))fn_80102ED4)();

    return;
}


/* 0x8006FEE4 | size: 0x390 */
void fn_8006FEE4(void) {
    extern void fn_8006B1F4();
    extern void fn_8006B3C8();
    extern void fn_80070D84();
    extern void fn_80109220();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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

    
    r30 = r3;
    r0 = *(u8*)((u8*)r30 + 0x95);
    r3 = (u32)&lbl_80267EA8;
    r31 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x6) {
        *(u32*)&lbl_8047A5FC = r0;
    }
    r3 = *(u32*)&lbl_8047A5FC;
    fn_8006B3C8();
    r28 = r3;
    r3 = r30;
    r4 = 0x957;
    ((void(*)(void))fn_801046C8)();
    r25 = r3;
    if (r25 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = *(u32*)&lbl_8047A5FC;
            r3 = r31 + 0x218;
            r0 = r0 << 2;
            r3 = *(u32*)(r3 + r0);
        } else {

            r3 = 0x26c;
        }
        ((void(*)(void))fn_8005D858)();
        r0 = r3;
        r3 = r25;
        r4 = r0;
        ((void(*)(void))fn_80071318)();
    }
    r27 = r31 + 0x1c8;
    r25 = 0x0;
    do {
        r4 = *(u16*)((u8*)r27 + 0x0);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r29 = r3;
        r3 = r25;
        fn_8006B3C8();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = *(u32*)((u8*)r27 + 0x4);
        } else {

            r0 = 0x3daa;
        }
        *(u32*)((u8*)r29 + 0x4C) = r0;
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while ((s32)r25 < (s32)0x6);
    r0 = r25 << 3;
    r27 = r31 + 0x1c8;
    r27 = r27 + r0;
    while (r25 < (u32)0xa) {

        r4 = *(u16*)((u8*)r27 + 0x0);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r0 = *(u32*)((u8*)r27 + 0x4);
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
        *(u32*)((u8*)r3 + 0x4C) = r0;

    }
    r0 = r28 & 0xFF;
    r27 = r31 + 0x230;
    r0 = __cntlzw(r0);
    r25 = 0x0;
    r29 = (u32)r0 >> 5;
    do {
        r4 = *(u16*)((u8*)r27 + 0x0);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = r29 & 0xFF;
        fn_80109220();
        r27 = r27 + 0x2;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0x5);
    r0 = *(u8*)((u8*)r30 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) goto L_8007011C;
        if ((s32)r0 != (s32)0x0) {
            goto L_8007011C;
        }
        r26 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r27 = r26;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r28 = r3 + 0xc;
            do {
                r4 = *(u16*)((u8*)r27 + 0x0);
                r3 = r30;
                ((void(*)(void))fn_801046C8)();
                r4 = *(u16*)(r29 + r28);
                r3 = r3 + 0xc;
                ((void(*)(void))fn_80108518)();
                r27 = r27 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r26 = r26 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);

    } else {
    r0 = *(u8*)((u8*)r30 + 0x2);
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        r28 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r26 = r28;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r27 = r3 + 0xe;
            do {
                r4 = *(u16*)((u8*)r26 + 0x0);
                r3 = r30;
                ((void(*)(void))fn_801046C8)();
                r4 = *(u16*)(r29 + r27);
                r3 = r3 + 0xc;
                ((void(*)(void))fn_80108518)();
                r26 = r26 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r28 = r28 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);
    }
    }
    L_8007011C: ;
    r3 = r30;
    r4 = r31 + 0x278;
    r5 = 0xb;
    fn_80070D84();
    r26 = r31 + 0x2d0;
    r27 = 0x0;
    do {
        r3 = *(u32*)&lbl_8047A5FC;
        if ((s32)r27 == (s32)0x0) {
            r4 = 0x0;
        } else {

            r4 = 0x1;
        }
        fn_8006B1F4();
        r4 = *(u16*)((u8*)r26 + 0x0);
        r24 = r3;
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = r24;
        fn_80109220();
        r4 = *(u16*)((u8*)r26 + 0x2);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = r24;
        fn_80109220();
        r4 = *(u16*)((u8*)r26 + 0x4);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r0 = r24 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = -0x1;
        } else {

            r4 = (0x6060 << 16);
            r0 = r4 + 0x60ff;
        }
        *(u32*)((u8*)r3 + 0x64) = r0;
        r26 = r26 + 0x6;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x2);
    r27 = 0x0;
    r26 = (u32)&lbl_8047C050;
    do {
        r4 = *(u16*)((u8*)r26 + 0x0);
        r3 = r30;
        ((void(*)(void))fn_801046C8)();
        r4 = (u32)r27 >> 31;
        r0 = r27 & 0x1;
        r0 = r0 ^ r4;
        r31 = r3;
        r0 = r0 - r4;
        if ((s32)r0 != (s32)0x0) {
            f1 = *(f32*)((u8*)r31 + 0x70);
            f0 = *(f32*)&lbl_8047C078;
            f2 = *(f64*)&lbl_8047C080;
            f0 = f1 + f0;
            *(f32*)((u8*)r31 + 0x70) = f0;
            f1 = *(f32*)((u8*)r31 + 0x70);
            ((void(*)(void))fn_800CE318)();
            f0 = (f32)f1;
            *(f32*)((u8*)r31 + 0x70) = f0;
        } else {

            f2 = *(f32*)((u8*)r31 + 0x70);
            f1 = *(f32*)&lbl_8047C078;
            f0 = *(f32*)&lbl_8047C088;
            f1 = f2 - f1;
            *(f32*)((u8*)r31 + 0x70) = f1;
            f1 = *(f32*)((u8*)r31 + 0x70);
            if (f0 > f1) {
                f0 = *(f32*)&lbl_8047C08C;
                f0 = f1 + f0;
                *(f32*)((u8*)r31 + 0x70) = f0;
            }
            f1 = *(f32*)((u8*)r31 + 0x70);
            f2 = *(f64*)&lbl_8047C080;
            ((void(*)(void))fn_800CE318)();
            f0 = (f32)f1;
            *(f32*)((u8*)r31 + 0x70) = f0;
        }
        r26 = r26 + 0x2;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x4);
    return;
}


/* 0x80070274 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_80070274(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8007029C | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8007029C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80267FE8, 0x11);
}
#pragma pop

/* 0x800702C8 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702C8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x800702F0 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702F0(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x80070318 | size: 0x110 */
void fn_80070318(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = *(u8*)((u8*)r31 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = *(u8*)((u8*)r31 + 0x2);
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267F68;
        r28 = 0x0;
        r29 = (u32)&lbl_80267F68;
        r3 = (u32)&lbl_80267EA8;
        r30 = (u32)&lbl_80267EA8;
        do {
            r0 = *(u32*)((u8*)r29 + 0x4);
            r3 = r31;
            r4 = *(u16*)((u8*)r29 + 0x0);
            r0 = r0 << 2;
            r5 = *(u16*)(r30 + r0);
            ((void(*)(void))fn_801081F8)();
            r29 = r29 + 0x8;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x10);
        return;
    }
    r0 = *(u8*)((u8*)r31 + 0x2);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267F68;
    r28 = 0x0;
    r29 = (u32)&lbl_80267F68;
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    do {
        r0 = *(u32*)((u8*)r29 + 0x4);
        r3 = r31;
        r4 = *(u16*)((u8*)r29 + 0x0);
        r0 = r0 << 2;
        r5 = r30 + r0;
        r5 = *(u16*)((u8*)r5 + 0x2);
        ((void(*)(void))fn_801081F8)();
        r29 = r29 + 0x8;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x10);
    r0 = 0x1;
    *(u8*)((u8*)r31 + 0x2) = r0;

    return;
}


/* 0x80070428 | size: 0x7C */
void fn_80070428(void) {
    extern void fn_80132A38();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;

    
    r31 = r4;
    r0 = *(s16*)((u8*)r31 + 0x6);
    if ((s32)r0 >= (s32)0xa28) return;
    if ((s32)r0 < (s32)0xa1d) {
        return;
    }
    r3 = *(u32*)((u8*)r31 + 0x4C);
    if (r3 == (u32)0x0) return;
    ((void(*)(void))fn_800FA280)();
    r0 = r3;
    r3 = 0x37;
    r4 = r0;
    fn_80132A38();
    r5 = *(u32*)((u8*)r31 + 0x64);
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xe7;
    ((void(*)(void))fn_800FB680)();
    r0 = 0x0;
    *(u32*)((u8*)r31 + 0x4C) = r0;

    return;
}


/* 0x800704A4 | size: 0x4 */
void fn_800704A4(void) {
}

/* 0x800704A8 | size: 0x4 */
void fn_800704A8(void) {
}

/* 0x800704AC | size: 0x218 */
void fn_800704AC(void) {
    extern void fn_8006B3C8();
    extern void fn_80109220();
    extern u8 jumptable_802EE0F0[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r0 = *(u8*)((u8*)r3 + 0x1);
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x3) {
        if ((s32)r0 < (s32)0x0) {
            goto L_80070624;
        }
        goto L_80070624;
        }
    ((void(*)(void))fn_8007162C)();
    /* subi r0, r3, 0xa8 */;
    if (r0 <= (u32)0x46) {
        r3 = (u32)jumptable_802EE0F0;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EE0F0;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r0 = r30 + 0x28;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        do {
            r3 = r28;
            fn_8006B3C8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r0 = *(u32*)((u8*)r30 + 0x0);
            } else {

                r0 = 0x43fe;
            }
            *(u32*)((u8*)r29 + 0x0) = r0;
            r30 = r30 + 0x4;
            r29 = r29 + 0x4;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x6);
        r3 = (u32)&lbl_802EDE58;
        r0 = (u32)&lbl_802EDE58;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        if (r3 != (u32)0x0) {
            r0 = r30 + 0x90;
            *(u32*)&lbl_8047A5F8 = r0;
            goto L_80070624;
        }
        r0 = r30 + 0x9c;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        if ((s32)r3 == (s32)r30) {
            r0 = 0x0;
        } else {

            r0 = 0x1;
        }
        r3 = r0 << 2;
        r0 = (u32)&lbl_8047C048;
        r0 = r0 + r3;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;
    }
    r0 = 0x0;
    *(u32*)&lbl_8047A5F8 = r0;
    L_80070624: ;
    r0 = *(u32*)&lbl_8047A5F8;
    if (r0 != (u32)0x0) {
        r3 = r31;
        r4 = 0x1;
        fn_80109220();
        r0 = *(s16*)((u8*)r31 + 0x6);
        if ((s32)r0 != (s32)0x93d) {
            if ((s32)r0 >= (s32)0x93d) return;
            return;
        }
        ((void(*)(void))fn_8007162C)();
        ((void(*)(void))fn_80104704)();
        if (r3 == (u32)0x0) {
            ((void(*)(void))fn_801046B8)();
            ((void(*)(void))fn_80104704)();
        }
        if (r3 == (u32)0x0) return;
        r0 = *(u8*)((u8*)r3 + 0x95);
        r3 = *(u32*)&lbl_8047A5F8;
        r0 = (s8)r0;
        r0 = r0 << 2;
        r0 = *(u32*)(r3 + r0);
        *(u32*)((u8*)r31 + 0x4C) = r0;
        return;
    }
    r0 = 0x0;
    r3 = r31;
    *(u32*)((u8*)r31 + 0x4C) = r0;
    r4 = 0x0;
    fn_80109220();

    return;
}


/* 0x800706C4 | size: 0x3D8 */
void fn_800706C4(void) {
    extern void fn_8006AFC4();
    extern void fn_8006B420();
    extern void fn_80109220();
    extern void fn_80129280();
    extern u8 jumptable_802EE20C[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r29 = r3;
    r30 = r4;
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    do {
    if ((s32)r0 >= (s32)0x3) break;
    if ((s32)r0 < (s32)0x0) {
        break;
    }
    r0 = *(u32*)((u8*)r30 + 0x4C);
    r4 = 0x0;
    *(u32*)&lbl_8047A5F4 = r0;
    ((void(*)(void))fn_801040D0)();
    /* subi r0, r3, 0xa8 */;
    do {
    if (r0 > (u32)0x43) break;
    r3 = (u32)jumptable_802EE20C;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EE20C;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r3 = 0x3be5;
    r0 = 0x1;
    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    if (r3 == (u32)0x0) break;
    fn_8006B420();
    r31 = r3;
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    fn_8006AFC4();
    r4 = r31;
    r3 = r3 + 0xb44;
    ((void(*)(void))fn_800767B8)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) break;
    r3 = 0x3bfc;
    r0 = 0x2;
    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;
    } while (0);
    r0 = 0x0;
    *(u32*)&lbl_8047A5F4 = r0;
    *(u32*)&lbl_8047A5F0 = r0;
    } while (0);
    r3 = *(u32*)&lbl_8047A5F4;
    ((void(*)(void))fn_800FA444)();
    r0 = *(s16*)((u8*)r30 + 0x6);
    r3 = (u32)r3 >> 16;
    /* subi r3, r3, 0x20 */;
    r31 = (s16)r3;
    if ((s32)r0 != (s32)0x939) {
        if ((s32)r0 < (s32)0x939) {
            if ((s32)r0 != (s32)0x809) {
                if ((s32)r0 < (s32)0x809) {
                    if ((s32)r0 < (s32)0x808) {
                        return;
                    }
                    if ((s32)r0 != (s32)0x937) {
                        if ((s32)r0 < (s32)0x937) {
                            return;
                        }
                        if ((s32)r0 != (s32)0xef8) {
                            if ((s32)r0 < (s32)0xef8) {
                                if ((s32)r0 != (s32)0xef6) {
                                    if ((s32)r0 < (s32)0xef6) {
                                        return;
                                    }
                                    if ((s32)r0 >= (s32)0xefa) return;
                                    r0 = *(u32*)&lbl_8047A5F0;
                                    r3 = r30;
                                    r0 = 0x2 - r0;
                                    r0 = __cntlzw(r0);
                                    r0 = (u32)r0 >> 5;
                                    r4 = r0 & 0xFF;
                                    fn_80109220();
                                    r0 = *(u32*)&lbl_8047A5F0;
                                    if ((s32)r0 == (s32)0x2) {
                                        r0 = *(u32*)&lbl_8047A5F4;
                                    } else {

                                        r0 = 0x0;
                                    }
                                    *(u32*)((u8*)r30 + 0x4C) = r0;
                                    return;
                                        }
                                r0 = *(u32*)&lbl_8047A5F0;
                                r3 = r30;
                                r0 = 0x1 - r0;
                                r0 = __cntlzw(r0);
                                r0 = (u32)r0 >> 5;
                                r4 = r0 & 0xFF;
                                fn_80109220();
                                r0 = *(u32*)&lbl_8047A5F0;
                                if ((s32)r0 == (s32)0x1) {
                                    r0 = *(u32*)&lbl_8047A5F4;
                                } else {

                                    r0 = 0x0;
                                }
                                *(u32*)((u8*)r30 + 0x4C) = r0;
                                return;
                                }
                            r3 = r30;
                            r4 = 0x0;
                            fn_80109220();
                            return;
                                    }
                        r0 = *(u32*)&lbl_8047A5F0;
                        r3 = r30;
                        r0 = 0x2 - r0;
                        r0 = __cntlzw(r0);
                        r0 = (u32)r0 >> 5;
                        r4 = r0 & 0xFF;
                        fn_80109220();
                        return;
                                    }
                    r0 = *(u32*)&lbl_8047A5F0;
                    r3 = r30;
                    r0 = 0x2 - r0;
                    r0 = __cntlzw(r0);
                    r0 = (u32)r0 >> 5;
                    r4 = r0 & 0xFF;
                    fn_80109220();
                    *(u16*)((u8*)r30 + 0x54) = r31;
                    return;
                }
                r0 = *(u32*)&lbl_8047A5F0;
                r3 = r30;
                r0 = 0x2 - r0;
                r0 = __cntlzw(r0);
                r0 = (u32)r0 >> 5;
                r4 = r0 & 0xFF;
                fn_80109220();
                r3 = r29;
                r4 = 0x938;
                ((void(*)(void))fn_801046C8)();
                r0 = *(s16*)((u8*)r3 + 0x50);
                r0 = r31 + r0;
                r0 = (s16)r0;
                *(u16*)((u8*)r30 + 0x50) = r0;
                return;
                                    }
            r0 = *(u32*)&lbl_8047A5F0;
            r3 = r30;
            r0 = 0x1 - r0;
            r0 = __cntlzw(r0);
            r0 = (u32)r0 >> 5;
            r4 = r0 & 0xFF;
            fn_80109220();
            return;
                                    }
        r0 = *(u32*)&lbl_8047A5F0;
        r3 = r30;
        r0 = 0x1 - r0;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        fn_80109220();
        *(u16*)((u8*)r30 + 0x54) = r31;
        return;
                        }
    r0 = *(u32*)&lbl_8047A5F0;
    r3 = r30;
    r0 = 0x1 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    fn_80109220();
    r3 = r29;
    r4 = 0xef7;
    ((void(*)(void))fn_801046C8)();
    r0 = *(s16*)((u8*)r3 + 0x50);
    r0 = r31 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r30 + 0x50) = r0;

    return;
}


/* 0x80070A9C | size: 0x2E8 */
void fn_80070A9C(void) {
    extern void fn_80109220();
    extern void fn_80129280();
    extern u8 jumptable_802EE31C[];
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r0 = *(u8*)((u8*)r3 + 0x1);
    r0 = (s8)r0;
    do {
    if ((s32)r0 >= (s32)0x3) break;
    if ((s32)r0 < (s32)0x0) {
        break;
    }
    r0 = *(u32*)((u8*)r31 + 0x4C);
    r4 = 0x0;
    *(u32*)&lbl_8047A5E8 = r0;
    r0 = *(u32*)((u8*)r31 + 0x4C);
    *(u32*)&lbl_8047A5EC = r0;
    ((void(*)(void))fn_801040D0)();
    /* subi r0, r3, 0xa8 */;
    if (r0 > (u32)0x4d) break;
    r3 = (u32)jumptable_802EE31C;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EE31C;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r0 = 0x0;
    *(u32*)&lbl_8047A5E8 = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    if ((s32)r0 == (s32)0x0) {
        r3 = 0x4237;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
    }
    r3 = 0x0;
    r4 = 0xe;
    fn_80129280();
    r0 = *(u32*)((u8*)r3 + 0x8);
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 < (s32)0x3) {
            if ((s32)r0 != (s32)0x1) {
                if ((s32)r0 < (s32)0x1) {
                    if ((s32)r0 < (s32)0x0) {
                        break;
                    }
                    if ((s32)r0 != (s32)0x5) {
                        if ((s32)r0 >= (s32)0x5) break;
                        goto L_80070C74;
                        }
                    r3 = 0x3d7c;
                    r0 = 0x0;
                    *(u32*)&lbl_8047A5E8 = r3;
                    *(u32*)&lbl_8047A5EC = r0;
                    break;
                    }
                r3 = 0x3d7d;
                r0 = 0x0;
                *(u32*)&lbl_8047A5E8 = r3;
                *(u32*)&lbl_8047A5EC = r0;
                break;
                    }
            r3 = 0x3d7e;
            r0 = 0x0;
            *(u32*)&lbl_8047A5E8 = r3;
            *(u32*)&lbl_8047A5EC = r0;
            break;
        }
        r3 = 0x3d7f;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
        L_80070C74: ;
        r3 = 0x3d80;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
                    }
    r3 = 0x3d81;
    r0 = 0x0;
    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    break;
                }
                if ((s32)r0 >= (s32)0x4) break;
                r3 = 0x3d6e;
                r0 = 0x0;
                *(u32*)&lbl_8047A5E8 = r3;
                *(u32*)&lbl_8047A5EC = r0;
                break;
                }
            r3 = 0x3d3a;
            r0 = 0x3dab;
            *(u32*)&lbl_8047A5E8 = r3;
            *(u32*)&lbl_8047A5EC = r0;
            break;
                }
        r3 = 0x3d3a;
        r0 = 0x423c;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
    }
    r3 = 0x3d3a;
    r0 = 0x3d2d;
    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    } while (0);
    r4 = *(u32*)&lbl_8047A5E8;
    r3 = r31;
    r0 = -r4;
    r0 = r0 | r4;
    r4 = (u32)r0 >> 31;
    fn_80109220();
    r0 = *(s16*)((u8*)r31 + 0x6);
    if ((s32)r0 != (s32)0x89b) {
        if ((s32)r0 < (s32)0x89b) {
            if ((s32)r0 == (s32)0x80a) return;
            return;
        }
        if ((s32)r0 != (s32)0x93e) {
            return;
        }
        r0 = *(u32*)&lbl_8047A5E8;
        *(u32*)((u8*)r31 + 0x4C) = r0;
        return;
        }
    r0 = *(u32*)&lbl_8047A5EC;
    *(u32*)((u8*)r31 + 0x4C) = r0;

    return;
}


/* 0x80070D84 | size: 0x318 */
void fn_80070D84(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r5;
    r0 = *(u8*)((u8*)r29 + 0x2);
    r31 = *(u32*)((u8*)r29 + 0x1C);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r0 = *(u8*)((u8*)r29 + 0x1);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) { r3 = 0x1; return; }
        if ((s32)r0 != (s32)0x0) {
            r3 = 0x1;
            return;
        }
        if (r4 != (u32)0x0) {
            r27 = r4;
            r26 = 0x0;
            r3 = (u32)&lbl_80267EA8;
            r28 = (u32)&lbl_80267EA8;
            while (r26 < r30) {

                r0 = *(u32*)((u8*)r27 + 0x4);
                r3 = r29;
                r4 = *(u16*)((u8*)r27 + 0x0);
                r0 = r0 << 2;
                r5 = *(u16*)(r28 + r0);
                ((void(*)(void))fn_801081F8)();
                r27 = r27 + 0x8;
                r26 = r26 + 0x1;

            }
        }
        while (r31 != (u32)0x0) {

            r0 = *(s16*)((u8*)r31 + 0x50);
            if ((s32)r0 < (s32)0x12c) {
                r0 = *(s16*)((u8*)r31 + 0x52);
                if ((s32)r0 < (s32)0x64) {
                    r4 = 0x3;

                } else if ((s32)r0 < (s32)0xc8) {
                    r4 = 0x4;

                }
                r4 = 0x5;

            }

            r0 = *(s16*)((u8*)r31 + 0x52);
            if ((s32)r0 < (s32)0x64) {
                r4 = 0x6;

            } else if ((s32)r0 < (s32)0xc8) {
                r4 = 0x7;

            } else {
                r4 = 0x8;
            }
            r3 = 0x0;
            if (r31 != (u32)0x0) {
                r0 = *(u32*)((u8*)r31 + 0xC);
                if (r0 != (u32)0x0) {
                    r0 = *(u8*)((u8*)r31 + 0x46);
                    if (r0 == (u32)0x0) {
                        r3 = 0x1;
            }
            }
            }
            if ((s32)r3 == (s32)0x0) {
                r3 = (u32)&lbl_80267EA8;
                r0 = r4 << 2;
                r4 = (u32)&lbl_80267EA8;
                r4 = *(u16*)(r4 + r0);
                r3 = r31 + 0xc;
                ((void(*)(void))fn_80108518)();
            }
            r31 = *(u32*)((u8*)r31 + 0x0);

        }
        r27 = *(u32*)((u8*)r29 + 0x20);
        while (r27 != (u32)0x0) {

            r3 = 0x0;
            if (r27 != (u32)0x0) {
                r0 = *(u32*)((u8*)r27 + 0xC);
                if (r0 != (u32)0x0) {
                    r0 = *(u8*)((u8*)r27 + 0x46);
                    if (r0 == (u32)0x0) {
                        r3 = 0x1;
            }
            }
            }
            if ((s32)r3 == (s32)0x0) {
                r3 = r27 + 0xc;
                r4 = 0x1ca;
                ((void(*)(void))fn_80108518)();
            }
            r27 = *(u32*)((u8*)r27 + 0x0);

        }
        r3 = 0x1;
        return;
    }
    if (r4 != (u32)0x0) {
        r27 = r4;
        r26 = 0x0;
        r3 = (u32)&lbl_80267EA8;
        r28 = (u32)&lbl_80267EA8;
        while (r26 < r30) {

            r0 = *(u32*)((u8*)r27 + 0x4);
            r3 = r29;
            r4 = *(u16*)((u8*)r27 + 0x0);
            r0 = r0 << 2;
            r5 = r28 + r0;
            r5 = *(u16*)((u8*)r5 + 0x2);
            ((void(*)(void))fn_801081F8)();
            r27 = r27 + 0x8;
            r26 = r26 + 0x1;

        }
    }
    while (r31 != (u32)0x0) {

        r0 = *(s16*)((u8*)r31 + 0x50);
        if ((s32)r0 < (s32)0x12c) {
            r0 = *(s16*)((u8*)r31 + 0x52);
            if ((s32)r0 < (s32)0x64) {
                r4 = 0x3;

            } else if ((s32)r0 < (s32)0xc8) {
                r4 = 0x4;

            }
            r4 = 0x5;

        }

        r0 = *(s16*)((u8*)r31 + 0x52);
        if ((s32)r0 < (s32)0x64) {
            r4 = 0x6;

        } else if ((s32)r0 < (s32)0xc8) {
            r4 = 0x7;

        } else {
            r4 = 0x8;
        }
        r3 = 0x0;
        if (r31 != (u32)0x0) {
            r0 = *(u32*)((u8*)r31 + 0xC);
            if (r0 != (u32)0x0) {
                r0 = *(u8*)((u8*)r31 + 0x46);
                if (r0 == (u32)0x0) {
                    r3 = 0x1;
        }
        }
        }
        if ((s32)r3 == (s32)0x0) {
            r3 = (u32)&lbl_80267EA8;
            r4 = r4 << 2;
            r0 = (u32)&lbl_80267EA8;
            r4 = r0 + r4;
            r3 = r31 + 0xc;
            r4 = *(u16*)((u8*)r4 + 0x2);
            ((void(*)(void))fn_80108518)();
        }
        r31 = *(u32*)((u8*)r31 + 0x0);

    }
    r27 = *(u32*)((u8*)r29 + 0x20);
    while (r27 != (u32)0x0) {

        r3 = 0x0;
        if (r27 != (u32)0x0) {
            r0 = *(u32*)((u8*)r27 + 0xC);
            if (r0 != (u32)0x0) {
                r0 = *(u8*)((u8*)r27 + 0x46);
                if (r0 == (u32)0x0) {
                    r3 = 0x1;
        }
        }
        }
        if ((s32)r3 == (s32)0x0) {
            r3 = r27 + 0xc;
            r4 = 0x1ce;
            ((void(*)(void))fn_80108518)();
        }
        r27 = *(u32*)((u8*)r27 + 0x0);

    }
    r0 = 0x1;
    *(u8*)((u8*)r29 + 0x2) = r0;

    r3 = 0x1;

    return;
}
