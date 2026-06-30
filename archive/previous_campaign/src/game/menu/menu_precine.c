/**
 * @file menu_precine.c
 * @brief Pre-cinema menu/UI functions (0x80034280-0x80035DD4)
 *
 * Address range: 0x80034280 - 0x80035DD4
 * Total functions: 19
 *
 * Unidentified TU between menu_carde.c and movie.c.
 * Contains menu-related helper functions and a large 0xC94-byte
 * state machine function.
 */

#include "dolphin/types.h"

/* 0x80034280 | 0x324 */
void fn_80034280(void) {
    extern u8 lbl_803A3334[];
    extern u8 lbl_803F7A30[];
    extern f32 lbl_8047B9F8;
    extern f64 lbl_8047BA08;
    extern f64 lbl_8047BA10;
    extern f32 lbl_8047BA28;
    extern void fn_8007AAA8();
    extern void fn_8007AAFC();
    extern void fn_8007AB10();
    extern void fn_8007B090();
    extern void fn_80080310();
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void _threadSwitch();
    extern void fn_801046B8();
    extern void fn_80105624();
    extern void fn_801069FC();
    extern void fn_80106D3C();
    extern void fn_80166A28();
    extern void fn_8025F350();
    extern u8 jumptable_802E504C[];
    u8 sp[0x80];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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

    r28 = -0x1;
    fn_8025F350();
    r3 = 0x0;
    fn_8007B090();
    tmp = 0x0;
    r31 = 0x1;
    *(u32*)(sp + 0x8) = tmp;
    r29 = 0x0;
while (1) {
        fn_801046B8();
        if ((s32)r28 == (s32)r3) {
            fn_80105624();
            tmp = *(u16*)((u8*)r3 + 0x4);
            tmp = tmp & 0x00000020;
            if ((s32)tmp == 0) goto L_80034358;
            if ((s32)r31 == 0x12) {
                fn_8007AAFC();
                goto L_80034358;
            }
            if ((s32)tmp != 0) {
                fn_8007AAA8();
            }
            r3 = 0x3c7;
            fn_80166A28();
            r3 = 0x1;
            fn_801069FC();
            r3 = 0x8;
            r4 = 0x44ed;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            r3 = 0x1;
            return;
        }
    L_80034358:
        r3 = r31;
        r4 = (u32)sp + 0x8;
        fn_8007AB10();
        r30 = r3;
        _threadSwitch();
        tmp = r29 & 0xFF;
        if (tmp != 0) {
            if ((s32)r30 == 0) continue;
        }
        r29 = 0x0;
        if ((s32)r30 != 0) {
            r31 = r30;
        }
        if (r31 > 0x15) continue;
        r3 = (u32)jumptable_802E504C;
        tmp = r31 << 2;
        r3 = (u32)jumptable_802E504C;
        tmp = *(u32*)(r3 + tmp);
        ctr_fn = (void(*)(void))tmp;
        r3 = 0x8;
        r4 = 0x3b88;
        r5 = 0x0;
        r6 = 0x0;
        fn_80106D3C();
        f27 = lbl_8047B9F8;
        f31 = lbl_8047BA08;
        r31 = 0x43300000;
        f29 = lbl_8047BA10;
        f28 = lbl_8047BA28;
        while (f27 < f28) {

            _threadSwitch();
            fn_800D37CC();
            *(u32*)(sp + 0x14) = tmp;
            f30 = f0 - f31;
            fn_800D3088();
            f0 = f0 - f29;
            f0 = f0 / f30;
            f27 = f27 + f0;

        }
        r3 = 0x3c7;
        fn_80166A28();
        if ((s32)tmp != 0) {
            fn_8007AAA8();
        }
        r3 = 0x8;
        r4 = 0x3b91;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
        r3 = 0x0;
        return;
        r3 = 0x8;
        r4 = 0x3b88;
        r5 = 0x0;
        r6 = 0x0;
        fn_80106D3C();
        f27 = lbl_8047B9F8;
        f28 = lbl_8047BA08;
        r31 = 0x43300000;
        f30 = lbl_8047BA10;
        f31 = lbl_8047BA28;
        while (f27 < f31) {

            _threadSwitch();
            fn_800D37CC();
            *(u32*)(sp + 0x1C) = tmp;
            f29 = f0 - f28;
            fn_800D3088();
            f0 = f0 - f30;
            f0 = f0 / f29;
            f27 = f27 + f0;

        }
        r4 = (u32)sp + 0x8;
        r31 = 0x12;
        r3 = 0x12;
        fn_8007AB10();
        continue;
        if ((s32)tmp != 0) {
            fn_8007AAA8();
        }
        r3 = 0x3c7;
        fn_80166A28();
        r3 = 0x1;
        fn_801069FC();
        r3 = 0x8;
        r4 = 0x44ed;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
        r3 = 0x1;
        fn_801069FC();
        r3 = 0x1;
        return;
        r3 = 0x8;
        r4 = 0x3b8e;
        r5 = 0x0;
        r6 = 0x0;
        fn_80106D3C();
        fn_801046B8();
        r29 = 0x1;
        r28 = r3;
        continue;
}
    fn_8007AAA8();
    r4 = (u32)lbl_803F7A30;
    r3 = (u32)lbl_803A3334;
    r4 = (u32)lbl_803F7A30;
    r5 = 0x1040;
    r3 = (u32)lbl_803A3334;
    r4 = r4 + 0x2388;
    fn_80080310();
    r3 = 0x2;

    return;
}

/* 0x800345A4 | 0x164 */
void fn_800345A4(s32 unused, u8 *r31) {
    #pragma peephole off
    extern u32 lbl_802E61D8[];
    extern void fn_80109220(u8 *, s32);
    extern u32 fn_8012A5B0(s32, s32, s32);
    u32 r6;
    u32 r4;
    s32 r0;
    s32 r5;

    r6 = fn_8012A5B0(0, 0xe, 0);
    r0 = *(s16*)(r31 + 0x6);
    do {
        if (r0 == 0x7f1) break;
        if (r0 < 0x7f1) {
            if (r0 == 0x7ee) goto L_800346C0;
            if (r0 < 0x7ee) {
                if (r0 == 0x7bc) break;
                return;
            }
            if (r0 >= 0x7f0) goto L_80034650;
            goto L_80034688;
        }
        if (r0 == 0x804) goto L_80034688;
        if (r0 < 0x804) {
            if (r0 >= 0x803) goto L_800346C0;
            return;
        }
        if (r0 >= 0x806) goto L_end;
        goto L_80034650;
    } while (0);

    {
        u32 a, b;
        b = lbl_802E61D8[3];
        a = r6 - b;
        b = r6 | ~b;
        a = b - (a >> 1);
        fn_80109220(r31, a >> 31);
    }
    return;
L_80034650:
    r5 = 0x0;
    if (r6 >= lbl_802E61D8[2] && r6 < lbl_802E61D8[3]) {
        r5 = 0x1;
    }
    fn_80109220(r31, r5 & 0xFF);
    return;
L_80034688:
    r5 = 0x0;
    if (r6 >= lbl_802E61D8[1] && r6 < lbl_802E61D8[2]) {
        r5 = 0x1;
    }
    fn_80109220(r31, r5 & 0xFF);
    return;
L_800346C0:
    r5 = 0x0;
    if (r6 >= lbl_802E61D8[0] && r6 < lbl_802E61D8[1]) {
        r5 = 0x1;
    }
    fn_80109220(r31, r5 & 0xFF);
    return;
L_end:
    return;
}

/* 0x80034708 | 0xB0 */
void fn_80034708(s32 unused, u8 *r4) {
    #pragma peephole off
    extern u32 lbl_80266FA0[];
    extern u32 lbl_8047A458;
    extern u8 *fn_80104704(s32);
    extern void fn_80109220(u8 *, s32);
    u8 *r30;
    u32 *r31;
    u8 *r3;

    r30 = r4;
    r31 = 0;
    switch ((s32)lbl_8047A458) {
    case 1:
        r31 = lbl_80266FA0;
        break;
    }
    if (r31 != 0) {
        fn_80109220(r30, 1);
        switch (*(s16*)(r30 + 0x6)) {
        case 0x7cc:
            r3 = fn_80104704(0xa4);
            if (r3 != 0) {
                *(u32*)(r30 + 0x4C) = r31[(s8)r3[0x95]];
            }
            break;
        }
        return;
    }
    *(u32*)(r30 + 0x4C) = 0;
    fn_80109220(r30, 0);
    return;
}

/* 0x80034830 | 0x32C */
void fn_80034830(void) {
    extern u8 lbl_803A3334[];
    extern u32 lbl_8047A430;
    extern void fn_800F9E70();
    u8 sp[0x10];
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
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f2 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r5;
    r5 = (u32)lbl_803A3334;
    r6 = lbl_8047A430;
    r5 = (u32)lbl_803A3334;
    r11 = 0x1;
    r8 = *(u8*)((u8*)r5 + 0x24);
    r10 = 0x0;
    r9 = *(u8*)((u8*)r5 + 0x26);
    r7 = *(u8*)((u8*)r5 + 0x8);
    r8 = (s8)r8;
    r9 = (s8)r9;
    tmp = 0x2;
    ctr_fn = (void(*)(void))tmp;
    do {
        tmp = *(u8*)((u8*)r6 + 0x41E4);
        do {
            if (tmp != r7) break;
            tmp = *(u8*)((u8*)r6 + 0x41E5);
            if ((s32)tmp != (s32)r8) break;
            tmp = *(u8*)((u8*)r6 + 0x41E6);
            if ((s32)tmp != (s32)r9) break;
            r11 = 0x0;
            break;
        } while (0);

        tmp = *(u8*)((u8*)r6 + 0x42AE);
        do {
            if (tmp != r7) break;
            tmp = *(u8*)((u8*)r6 + 0x42AF);
            if ((s32)tmp != (s32)r8) break;
            tmp = *(u8*)((u8*)r6 + 0x42B0);
            if ((s32)tmp != (s32)r9) break;
            r11 = 0x0;
            break;
        } while (0);

        tmp = *(u8*)((u8*)r6 + 0x4378);
        do {
            if (tmp != r7) break;
            tmp = *(u8*)((u8*)r6 + 0x4379);
            if ((s32)tmp != (s32)r8) break;
            tmp = *(u8*)((u8*)r6 + 0x437A);
            if ((s32)tmp != (s32)r9) break;
            r11 = 0x0;
            break;
        } while (0);

        tmp = *(u8*)((u8*)r6 + 0x4442);
        do {
            if (tmp != r7) break;
            tmp = *(u8*)((u8*)r6 + 0x4443);
            if ((s32)tmp != (s32)r8) break;
            tmp = *(u8*)((u8*)r6 + 0x4444);
            if ((s32)tmp != (s32)r9) break;
            r11 = 0x0;
            break;
        } while (0);

        tmp = *(u8*)((u8*)r6 + 0x450C);
        do {
            if (tmp != r7) break;
            tmp = *(u8*)((u8*)r6 + 0x450D);
            if ((s32)tmp != (s32)r8) break;
            tmp = *(u8*)((u8*)r6 + 0x450E);
            if ((s32)tmp != (s32)r9) break;
            r11 = 0x0;
            break;
        } while (0);

        r6 = r6 + 0x3f2;
        r10 = r10 + 0x4;
    } while (--ctr != 0);

    tmp = r11 & 0xFF;
    if (tmp != 0) {
        r6 = 0x0;
        if ((s32)r6 < 9) {
            r10 = r6;
            do {
                r7 = lbl_8047A430;
                tmp = 0x19;
                r7 = r7 + r10;
                r9 = r7 + 0x41e0;
                r8 = r7 + 0x42aa;
                ctr_fn = (void(*)(void))tmp;
                do {
                    r7 = *(u32*)((u8*)r8 + 0x4);
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    *(u32*)((u8*)r9 + 0x4) = r7;
                    r9 += 8; *(u32*)r9 = tmp;
                } while (--ctr != 0);
                r7 = *(u16*)((u8*)r8 + 0x4);
                tmp = 0x19;
                *(u16*)((u8*)r9 + 0x4) = r7;
                r7 = lbl_8047A430;
                r7 = r7 + r10;
                r9 = r7 + 0x42aa;
                r8 = r7 + 0x4374;
                ctr_fn = (void(*)(void))tmp;
                do {
                    r7 = *(u32*)((u8*)r8 + 0x4);
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    *(u32*)((u8*)r9 + 0x4) = r7;
                    r9 += 8; *(u32*)r9 = tmp;
                } while (--ctr != 0);
                r7 = *(u16*)((u8*)r8 + 0x4);
                tmp = 0x19;
                *(u16*)((u8*)r9 + 0x4) = r7;
                r7 = lbl_8047A430;
                r7 = r7 + r10;
                r9 = r7 + 0x4374;
                r8 = r7 + 0x443e;
                ctr_fn = (void(*)(void))tmp;
                do {
                    r7 = *(u32*)((u8*)r8 + 0x4);
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    *(u32*)((u8*)r9 + 0x4) = r7;
                    r9 += 8; *(u32*)r9 = tmp;
                } while (--ctr != 0);
                tmp = *(u16*)((u8*)r8 + 0x4);
                r10 = r10 + 0x25e;
                r6 = r6 + 0x3;
                *(u16*)((u8*)r9 + 0x4) = tmp;
            } while ((s32)r6 < 9);
        }
        r7 = lbl_8047A430;
        tmp = r4 & 0xFF;
        r6 = *(u8*)((u8*)r5 + 0x8);
        r4 = r30;
        r31 = r7 + 0x48fe;
        *(u8*)((u8*)r31 + 0x0) = r6;
        r6 = *(u8*)((u8*)r5 + 0x24);
        *(u8*)((u8*)r31 + 0x1) = r6;
        r5 = *(u8*)((u8*)r5 + 0x26);
        *(u8*)((u8*)r31 + 0x2) = r5;
        *(u8*)((u8*)r31 + 0x3) = r3;
        r3 = r31 + 0x6;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        fn_800F9E70();
        tmp = *(u8*)((u8*)r30 + 0xC);
        r3 = 0x0;
        *(u8*)((u8*)r31 + 0x12) = tmp;
        tmp = *(u16*)((u8*)r30 + 0x12);
        *(u16*)((u8*)r31 + 0x14) = tmp;
        tmp = *(u16*)((u8*)r30 + 0x14);
        *(u16*)((u8*)r31 + 0x16) = tmp;
        tmp = *(u16*)((u8*)r30 + 0x16);
        *(u16*)((u8*)r31 + 0x18) = tmp;
        tmp = *(u16*)((u8*)r30 + 0x18);
        *(u16*)((u8*)r31 + 0x1A) = tmp;
        tmp = *(u32*)((u8*)r30 + 0x1C);
        tmp = tmp & 0xFF;
        *(u8*)((u8*)r31 + 0x1C) = tmp;
        tmp = *(u16*)((u8*)r30 + 0x20);
        *(u16*)((u8*)r31 + 0x1E) = tmp;
        tmp = *(u8*)((u8*)r30 + 0x24);
        *(u8*)((u8*)r31 + 0x20) = tmp;
        tmp = 0x4;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = r3 + 0xd;
            tmp = *(u8*)(r30 + tmp);
            tmp = (s8)tmp;
            if ((s32)tmp < 0) {
                tmp = 0x0;
                *(u16*)((u8*)r31 + 0x22) = tmp;
            } else {

                r5 = tmp * 0x2a;
                r4 = (u32)lbl_803A3334;
                tmp = (u32)lbl_803A3334;
                r5 = tmp + r5;
                r4 = *(u32*)((u8*)r5 + 0x514);
                tmp = *(u32*)((u8*)r5 + 0x518);
                *(u32*)((u8*)r31 + 0x22) = r4;
                *(u32*)((u8*)r31 + 0x26) = tmp;
                r4 = *(u32*)((u8*)r5 + 0x51C);
                tmp = *(u32*)((u8*)r5 + 0x520);
                *(u32*)((u8*)r31 + 0x2A) = r4;
                *(u32*)((u8*)r31 + 0x2E) = tmp;
                r4 = *(u32*)((u8*)r5 + 0x524);
                tmp = *(u32*)((u8*)r5 + 0x528);
                *(u32*)((u8*)r31 + 0x32) = r4;
                *(u32*)((u8*)r31 + 0x36) = tmp;
                r4 = *(u32*)((u8*)r5 + 0x52C);
                tmp = *(u32*)((u8*)r5 + 0x530);
                *(u32*)((u8*)r31 + 0x3A) = r4;
                *(u32*)((u8*)r31 + 0x3E) = tmp;
                r4 = *(u32*)((u8*)r5 + 0x534);
                tmp = *(u32*)((u8*)r5 + 0x538);
                *(u32*)((u8*)r31 + 0x42) = r4;
                *(u32*)((u8*)r31 + 0x46) = tmp;
                tmp = *(u16*)((u8*)r5 + 0x53C);
                *(u16*)((u8*)r31 + 0x4A) = tmp;
            }
            r31 = r31 + 0x2a;
            r3 = r3 + 0x1;
        } while (--ctr != 0);
    }
    return;
}

/* 0x80034B5C | 0x264 */
void fn_80034B5C(void) {
    extern u8 lbl_803A3334[];
    extern u32 lbl_8047A434;
    extern void fn_80082FE4();
    extern void fn_800E0C04();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    extern void fn_8012A5B0();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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

    r20 = r3;
    r21 = r4;
    r22 = r5;
    r3 = (u32)lbl_803A3334;
    r24 = 0x0;
    r3 = (u32)lbl_803A3334;
    r23 = 0x1;
    r28 = *(u8*)((u8*)r3 + 0x24);
    r26 = 0x1;
    r27 = 0x0;
    while (1) {
        tmp = r27 & 0xFFFF;
        if (tmp >= 6) break;
        r5 = r27;
        r3 = 0x0;
        r4 = 0x3;
        fn_8012A5B0();
        r25 = r3;
        fn_80123FBC();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            r3 = r25;
            r4 = 0x0;
            r5 = 0x7a;
            r6 = 0x0;
            fn_8012640C();
            r3 = r3 & 0xFF;
            tmp = r26 & 0xFF;
            if (tmp < r3) {
                r26 = r3;
        }
        }
        r27 = r27 + 0x1;

    }
    tmp = (s8)r28;
    r3 = (u32)lbl_803A3334;
    r27 = tmp * 0xe;
    r29 = (u32)sp + 0x8;
    r30 = (u32)lbl_803A3334;
    r26 = r26 & 0xFF;
    r28 = r30;
    r25 = 0x0;
    r3 = 0x80000000;
    while (1) {
    do {
        tmp = *(u8*)((u8*)r30 + 0x58);
        tmp = (s8)tmp;
        if ((s32)r25 >= (s32)tmp) break;
        *(u32*)((u8*)r29 + 0x0) = r31;
        r4 = (s8)r25;
        r3 = lbl_8047A434;
        fn_80082FE4();
        tmp = r3 + 0x1c;
        tmp = *(u8*)(r27 + tmp);
        if (tmp != 0) {
            tmp = *(u8*)((u8*)r28 + 0x5E);
            r3 = (u32)lbl_803A3334;
            r5 = 0x0;
            tmp = (s8)tmp;
            r4 = (u32)lbl_803A3334;
            tmp = tmp * 0x28;
            r6 = r5;
            r7 = r4 + tmp;
            while (1) {
                tmp = r6 & 0xFFFF;
                if (tmp >= 4) break;
                r3 = r6 & 0xFFFF;
                tmp = r3 + 0x3b9;
                tmp = *(u8*)(r7 + tmp);
                tmp = (s8)tmp;
                if ((s32)tmp >= 0) {
                    r3 = tmp * 0x2a;
                    tmp = r5 & 0xFF;
                    r3 = r4 + r3;
                    r3 = *(u8*)((u8*)r3 + 0x517);
                    if (tmp < r3) {
                        r5 = r3;
                }
                }
                r6 = r6 + 0x1;

            }
            r3 = r5 & 0xFF;
            tmp = (s8)r24;
            r4 = r26 - r3;
            r3 = (u32)sp + 0x8;
            r5 = (s32)r4 >> 31;
            tmp = tmp << 2;
            r4 = r5 ^ r4;
            r4 = r4 - r5;
            *(u32*)((u8*)r29 + 0x0) = r4;
            r4 = *(u32*)((u8*)r29 + 0x0);
            tmp = *(u32*)(r3 + tmp);
            if ((s32)r4 < (s32)tmp) {
                r24 = (s8)r25;
                r23 = 0x1;
                break;
            }
            if ((s32)r4 != (s32)tmp) break;
            r23 = r23 + 0x1;
            r3 = r23;
            fn_800E0C04();
            if (r3 != 0) break;
            r24 = (s8)r25;
        }
    } while (0);
        r29 = r29 + 0x4;
        r28 = r28 + 0x1;
        r25 = r25 + 0x1;

    }
    tmp = (s8)r24;
    r3 = 0x80000000;
    tmp = tmp << 2;
    r4 = (u32)sp + 0x8;
    tmp = *(u32*)(r4 + tmp);
    tmp = r4 ^ tmp;
    r3 = (s32)tmp >> 1;
    tmp = tmp & r4;
    tmp = r3 - tmp;
    tmp = (u32)tmp >> 31;
    *(u8*)((u8*)r20 + 0x0) = tmp;
    tmp = *(u8*)((u8*)r20 + 0x0);
    if (tmp != 0) {
        *(u8*)((u8*)r21 + 0x0) = r24;
        r3 = (u32)lbl_803A3334;
        r3 = (u32)lbl_803A3334;
        tmp = *(u8*)((u8*)r21 + 0x0);
        tmp = (s8)tmp;
        r3 = r3 + tmp;
        tmp = *(u8*)((u8*)r3 + 0x5E);
        *(u8*)((u8*)r22 + 0x0) = tmp;
    } else {

        r4 = *(u8*)((u8*)r30 + 0x58);
        r3 = (u32)lbl_803A3334;
        r3 = (u32)lbl_803A3334;
        tmp = (s8)tmp;
        *(u8*)((u8*)r21 + 0x0) = tmp;
        tmp = *(u8*)((u8*)r21 + 0x0);
        tmp = (s8)tmp;
        r3 = r3 + tmp;
        tmp = *(u8*)((u8*)r3 + 0x5B);
        *(u8*)((u8*)r22 + 0x0) = tmp;
    }
    return;
}

/* 0x78 | fn_80034DC0 | call_sequence */
void fn_80034DC0(void) {
    fn_800FB680();
    fn_80132A38();
    fn_800FBB34();
}

/* 0x80034E38 | 0xB8 */
void fn_80034E38(void) {
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void fn_80132A38();
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

    r30 = r4;
    r4 = lbl_8047A450;
    tmp = lbl_8047A44C;
    if (r4 > tmp) {
        tmp = *(u8*)((u8*)r3 + 0x8B);
        r31 = tmp | (0xffa0 << 16);
        r31 = r31 | 0x8000;
    } else {

        r3 = *(u8*)((u8*)r3 + 0x8B);
        tmp = -0x100;
        r31 = r3 | tmp;
    }
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0x3cc7;
    fn_800FB680();
    r3 = 0x990000;
    r4 = lbl_8047A450;
    if (r4 <= tmp) {
        r3 = 0x34;
        fn_80132A38();
    } else {

        r4 = tmp;
        r3 = 0x34;
        fn_80132A38();
    }
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r31;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x3cc9;
    fn_800FBB34();
    return;
}

/* 0x80034EF0 | 0x94 */
void fn_80034EF0(void) {
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void fn_8012A5B0();
    extern void fn_80132A38();
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

    r30 = r4;
    r5 = *(u8*)((u8*)r3 + 0x8B);
    tmp = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r31 = r5 | tmp;
    r6 = 0x3cc6;
    r5 = r31;
    fn_800FB680();
    r3 = 0x0;
    r4 = 0xd;
    r5 = 0x0;
    fn_8012A5B0();
    tmp = r3;
    r3 = 0x34;
    r4 = tmp;
    fn_80132A38();
    r4 = *(s16*)((u8*)r30 + 0x54);
    r7 = r31;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    tmp = r4 + 0x2;
    r4 = 0x0;
    r8 = 0x3cd3;
    r5 = (s16)tmp;
    fn_800FBB34();
    return;
}

/* 0x80034F84 | 0x2C */
#pragma push
#pragma peephole off
void fn_80034F84(void) {
    extern u8 lbl_803A3288[];
    extern void fn_80132A38(s32 r3, u32 r4);

    fn_80132A38(0x37, (u32)lbl_803A3288);
    return;
}
#pragma pop

/* 0x80034FB0 | 0x4 -- nop/blr */
void fn_80034FB0(void) { }

/* 0x80034FB4 | 0xC94 */
void fn_80034FB4(void) {
    extern u8 lbl_803A3278[];
    extern u32 lbl_8047A430;
    extern u32 lbl_8047A434;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u32 lbl_8047A43C;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u8 lbl_8047A44A;
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern u8 lbl_8047A454;
    extern u32 lbl_8047A458;
    extern f32 lbl_8047B9F8;
    extern f32 lbl_8047B9FC;
    extern f32 lbl_8047BA00;
    extern f64 lbl_8047BA08;
    extern f64 lbl_8047BA10;
    extern u8 lbl_8047BA1C[];
    extern void fn_8003258C();
    extern void fn_800327FC();
    extern void fn_80033278();
    extern void fn_80034280();
    extern void fn_80034830();
    extern void fn_80034B5C();
    extern void fn_80082EA4();
    extern void fn_800836AC();
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void _threadSwitch();
    extern void fn_800F9E70();
    extern void fn_800FA280();
    extern void fn_800FF52C();
    extern void fn_800FF660();
    extern void fn_80102510();
    extern void fn_80102620();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_801046B8();
    extern void fn_801069FC();
    extern void fn_80106D3C();
    extern void fn_80113828();
    extern void fn_8011DCB4();
    extern void fn_8011F1A0();
    extern void fn_80129280();
    extern void fn_8012AC08();
    extern void fn_80132A38();
    extern void fn_80165668();
    extern void fn_80166A28();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801D0AFC();
    extern void fn_801F6600();
    extern void fn_801F6738();
    extern u8 jumptable_802E50A4[];
    u8 sp[0x90];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
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
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)lbl_803A3278;
    tmp = 0x1;
    r31 = (u32)lbl_803A3278;
    lbl_8047A438 = tmp;
    r30 = r31 + 0xbc;
    r29 = r31 + 0x10;
    r28 = r30 + 0xb00;
    while (1) {
        tmp = lbl_8047A458;
        if ((s32)tmp <= 0) break;
        if (tmp <= 0xe) {
        do {
            r3 = (u32)jumptable_802E50A4;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802E50A4;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = lbl_8047A454;
            if (tmp == 1) {
                f1 = lbl_8047BA00;
                r3 = 0x2;
                fn_801C41C8();
            }
            r3 = 0xa4;
            r4 = 0x1;
            fn_8010264C();
            tmp = 0x1;
            r27 = r3;
            lbl_8047A454 = tmp;
            r3 = 0x1;
            fn_801C40F0();
            if ((s32)r27 != 1) {
                if ((s32)r27 < 1) {
                    if ((s32)r27 != (s32)-0x1) {
                        if ((s32)r27 < (s32)-0x1) {
                            goto L_800350BC;
                        }
                        if ((s32)r27 >= 3) goto L_800350BC;
                        goto L_800350B0;
                        }
                    tmp = 0x2;
                    lbl_8047A458 = tmp;
                    break;
                }
                tmp = 0x3;
                lbl_8047A458 = tmp;
                break;
                    }
            tmp = 0x0;
            lbl_8047A458 = tmp;
            break;
        L_800350B0:
            tmp = 0x0;
            lbl_8047A458 = tmp;
            break;
        L_800350BC:
            tmp = 0x0;
            lbl_8047A458 = tmp;
        } while (0);
            tmp = lbl_8047A458;
            if ((s32)tmp == 0) {
                r3 = 0x3c7;
                fn_80166A28();
                r3 = 0x8;
                r4 = 0x3b54;
                r5 = 0x1;
                r6 = 0x0;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
                continue;
            }
            f1 = lbl_8047BA00;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0xa4;
            fn_80102510();
            continue;
            f1 = lbl_8047BA00;
            r3 = 0x2;
            fn_801C41C8();
            fn_801046B8();
            r4 = r3;
            r9 = r29;
            r3 = 0xa6;
            r5 = 0x0;
            r6 = 0x0;
            r7 = 0x0;
            r8 = 0x1;
            fn_801026A4();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r24 = r31 + 0xb0;
            r27 = r3;
            r26 = 0x0;
            r25 = r24;
            do {
                r4 = r26 & 0xFF;
                r3 = r27;
                fn_8012AC08();
                fn_8011F1A0();
                *(u16*)((u8*)r25 + 0x0) = r3;
                r25 = r25 + 0x2;
                r26 = r26 + 0x1;
            } while ((s32)r26 < 6);
            while (1) {
                tmp = lbl_8047A458;
                if ((s32)tmp != 2) break;
                fn_80033278();
                lbl_8047A458 = r3;

            }
            r3 = 0x0;
            fn_801D0AFC();
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r26 = r3;
            r27 = 0x0;
            do {
                r4 = r27 & 0xFF;
                r3 = r26;
                fn_8012AC08();
                r4 = *(u16*)((u8*)r24 + 0x0);
                fn_8011DCB4();
                r24 = r24 + 0x2;
                r27 = r27 + 0x1;
            } while ((s32)r27 < 6);
            tmp = lbl_8047A439;
            if (tmp != 0) {
                tmp = 0x0;
                lbl_8047A458 = tmp;
            }
            tmp = lbl_8047A458;
            if ((s32)tmp != 0) {
                r3 = 0x3c7;
                fn_80166A28();
                f1 = lbl_8047BA00;
                r3 = 0x3;
                fn_801C41C8();
                r3 = 0x1;
                fn_801C40F0();
                r3 = 0xa6;
                fn_80102510();
            }
            continue;
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r25 = r31 + 0xb0;
            r26 = r3;
            r27 = 0x0;
            do {
                r4 = r27 & 0xFF;
                r3 = r26;
                fn_8012AC08();
                fn_8011F1A0();
                *(u16*)((u8*)r25 + 0x0) = r3;
                r25 = r25 + 0x2;
                r27 = r27 + 0x1;
            } while ((s32)r27 < 6);
            tmp = 0x0;
            r4 = r31 + 0x0;
            lbl_8047A450 = tmp;
            r3 = 0xa5;
            lbl_8047A44A = tmp;
            lbl_8047A43C = tmp;
            *(u8*)((u8*)r31 + 0x0) = tmp;
            *(u8*)((u8*)r4 + 0x1) = tmp;
            *(u8*)((u8*)r4 + 0x2) = tmp;
            *(u8*)((u8*)r4 + 0x3) = tmp;
            *(u8*)((u8*)r4 + 0x4) = tmp;
            *(u8*)((u8*)r4 + 0x5) = tmp;
            *(u8*)((u8*)r4 + 0x6) = tmp;
            *(u8*)((u8*)r4 + 0x7) = tmp;
            *(u8*)((u8*)r4 + 0x8) = tmp;
            *(u8*)((u8*)r4 + 0x9) = tmp;
            *(u8*)((u8*)r4 + 0xA) = tmp;
            *(u8*)((u8*)r4 + 0xB) = tmp;
            *(u8*)((u8*)r4 + 0xC) = tmp;
            *(u8*)((u8*)r4 + 0xD) = tmp;
            *(u8*)((u8*)r4 + 0xE) = tmp;
            fn_80102620();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = 0xa5;
                fn_80102510();
            }
            f1 = lbl_8047BA00;
            r3 = 0x2;
            fn_801C41C8();
            r3 = 0xa5;
            r4 = 0x1;
            fn_8010264C();
            r3 = 0x1;
            fn_801C40F0();
            tmp = 0x5;
            lbl_8047A458 = tmp;
            continue;
            tmp = lbl_8047A449;
            if (tmp != 0) {
                r3 = 0x0;
                fn_801D0AFC();
            }
            r3 = lbl_8047A450;
            tmp = lbl_8047A44C;
            if (r3 > tmp) {
                r3 = 0x4ce;
                r4 = 0x0;
                r5 = 0xff;
                fn_80165668();
                r3 = 0x990000;
                r8 = lbl_8047A450;
                if (r8 <= tmp) {
                } else {

                    r8 = tmp;
                }
                lbl_8047A44C = r8;
                r3 = 0x8;
                r7 = lbl_8047A430;
                r4 = 0x3b8d;
                r5 = 0x1;
                r6 = 0x0;
                *(u32*)((u8*)r7 + 0x49C8) = r8;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
                f27 = lbl_8047B9F8;
                f31 = lbl_8047BA08;
                r27 = 0x43300000;
                f29 = lbl_8047BA10;
                f28 = lbl_8047B9FC;
                while (f27 < f28) {

                    _threadSwitch();
                    fn_800D37CC();
                    *(u32*)(sp + 0x14) = tmp;
                    f30 = f0 - f31;
                    fn_800D3088();
                    f0 = f0 - f29;
                    f0 = f0 / f30;
                    f27 = f27 + f0;

                }
            }
            r3 = 0x8;
            r4 = 0x3b54;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            tmp = 0x0;
            r5 = r31 + 0x0;
            lbl_8047A43C = tmp;
            r3 = 0x0;
            r4 = 0x2;
            *(u8*)((u8*)r31 + 0x0) = tmp;
            *(u8*)((u8*)r5 + 0x1) = tmp;
            *(u8*)((u8*)r5 + 0x2) = tmp;
            *(u8*)((u8*)r5 + 0x3) = tmp;
            *(u8*)((u8*)r5 + 0x4) = tmp;
            *(u8*)((u8*)r5 + 0x5) = tmp;
            *(u8*)((u8*)r5 + 0x6) = tmp;
            *(u8*)((u8*)r5 + 0x7) = tmp;
            *(u8*)((u8*)r5 + 0x8) = tmp;
            *(u8*)((u8*)r5 + 0x9) = tmp;
            *(u8*)((u8*)r5 + 0xA) = tmp;
            *(u8*)((u8*)r5 + 0xB) = tmp;
            *(u8*)((u8*)r5 + 0xC) = tmp;
            *(u8*)((u8*)r5 + 0xD) = tmp;
            *(u8*)((u8*)r5 + 0xE) = tmp;
            fn_80129280();
            r25 = r31 + 0xb0;
            r26 = r3;
            r27 = 0x0;
            do {
                r4 = r27 & 0xFF;
                r3 = r26;
                fn_8012AC08();
                r4 = *(u16*)((u8*)r25 + 0x0);
                fn_8011DCB4();
                r25 = r25 + 0x2;
                r27 = r27 + 0x1;
            } while ((s32)r27 < 6);
            tmp = 0x0;
            lbl_8047A458 = tmp;
            continue;
            fn_80034280();
            if ((s32)r3 != 1) {
                if ((s32)r3 < 1) {
                    if ((s32)r3 < 0) {
                        goto L_800354C0;
                    }
                    goto L_800354C0;
                    }
                tmp = 0x4;
                lbl_8047A458 = tmp;
                continue;
            }
            tmp = 0x6;
            lbl_8047A458 = tmp;
            continue;
        L_800354C0:
            tmp = *(u32*)((u8*)r31 + 0xBC);
            if ((s32)tmp != 1) {
                if ((s32)tmp >= 1) continue;
                if ((s32)tmp < 0) {
                    continue;
                }
                tmp = 0x7;
                lbl_8047A458 = tmp;
                continue;
                }
            r3 = 0x0;
            r4 = 0xd;
            fn_80129280();
            r4 = r31 + 0xbc;
            r5 = 0x0;
            fn_800836AC();
            lbl_8047A434 = r3;
            if (r3 != 0) {
                r4 = *(u8*)((u8*)r30 + 0x58);
                r27 = 0x4;
                r5 = *(u8*)((u8*)r30 + 0x24);
                r6 = *(u8*)((u8*)r30 + 0x26);
                r4 = (s8)tmp;
                fn_80082EA4();
                tmp = *(u8*)((u8*)r3 + 0xC);
                if (tmp != 0) {
                    tmp = lbl_8047A43C;
                    r7 = 0x0;
                    do {
                        if ((s32)tmp <= 0) break;
                        r6 = *(u8*)((u8*)r30 + 0x8);
                        r5 = r31 + 0x0;
                        r3 = *(u8*)((u8*)r30 + 0x24);
                        r8 = *(u8*)((u8*)r30 + 0x26);
                        r4 = (s8)r3;
                        r3 = (s8)r8;
                        ctr_fn = (void(*)(void))tmp;
                        if ((s32)tmp <= 0) break;
                        do {
                            tmp = *(u8*)((u8*)r5 + 0x0);
                            do {
                                if (r6 != tmp) break;
                                tmp = *(u8*)((u8*)r5 + 0x1);
                                tmp = (s8)tmp;
                                if ((s32)r4 != (s32)tmp) break;
                                tmp = *(u8*)((u8*)r5 + 0x2);
                                tmp = (s8)tmp;
                                if ((s32)r3 != (s32)tmp) break;
                                r7 = 0x1;
                                break;
                            } while (0);

                            r5 = r5 + 0x3;
                        } while (--ctr != 0);
                    } while (0);

                    tmp = r7 & 0xFF;
                    if (tmp == 0) {
                        r3 = lbl_8047A43C;
                        r8 = r31 + 0x0;
                        r6 = *(u8*)((u8*)r30 + 0x8);
                        r5 = r3 * 0x3;
                        tmp = r3 + 0x1;
                        r4 = *(u8*)((u8*)r30 + 0x24);
                        r3 = *(u8*)((u8*)r30 + 0x26);
                        r8 = r8 + r5;
                        *(u8*)((u8*)r8 + 0x0) = r6;
                        *(u8*)((u8*)r8 + 0x1) = r4;
                        *(u8*)((u8*)r8 + 0x2) = r3;
                        lbl_8047A43C = tmp;
                        if ((s32)tmp >= 5) {
                            tmp = 0x4;
                            lbl_8047A43C = tmp;
                    }
                    }
                    tmp = r7 & 0xFF;
                    if (tmp != 0) {
                        r27 = 0x3;
                    }
                    goto L_8003560C;
                }
                r27 = 0x2;

            } else {
                r27 = 0x2;
            }
        L_8003560C:
            if ((s32)r27 != 4) {
                if ((s32)r27 < 4) {
                    if ((s32)r27 != 2) {
                        if ((s32)r27 < 2) {
                            goto L_80035654;
                        }
                        goto L_80035654;
                        }
                    tmp = 0x8;
                    lbl_8047A458 = tmp;
                    continue;
                        }
                tmp = 0x9;
                lbl_8047A458 = tmp;
                continue;
            }
            tmp = 0xa;
            lbl_8047A458 = tmp;
            continue;
        L_80035654:
            tmp = 0x4;
            lbl_8047A458 = tmp;
            continue;
            r3 = 0x8;
            r4 = 0x3b8a;
            r5 = 0x0;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0xa9;
            r4 = 0x1;
            fn_8010264C();
            r27 = r3;
            r3 = 0xa9;
            fn_80102510();
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r27 != 0) {
                if ((s32)r27 < 0) {
                    if ((s32)r27 < (s32)-0x1) {
                        goto L_800356CC;
                    }
                    if ((s32)r27 >= 2) goto L_800356CC;
                    tmp = 0x5;
                    lbl_8047A458 = tmp;
                    continue;
                    }
                tmp = 0x5;
                lbl_8047A458 = tmp;
                continue;
            }
        L_800356CC:
            r3 = lbl_8047A450;
            tmp = lbl_8047A44C;
            if (r3 > tmp) {
                r3 = 0x4ce;
                r4 = 0x0;
                r5 = 0xff;
                fn_80165668();
                r3 = 0x990000;
                r8 = lbl_8047A450;
                if (r8 <= tmp) {
                } else {

                    r8 = tmp;
                }
                lbl_8047A44C = r8;
                r3 = 0x8;
                r7 = lbl_8047A430;
                r4 = 0x3b8d;
                r5 = 0x1;
                r6 = 0x0;
                *(u32*)((u8*)r7 + 0x49C8) = r8;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
                f27 = lbl_8047B9F8;
                f31 = lbl_8047BA08;
                r27 = 0x43300000;
                f29 = lbl_8047BA10;
                f28 = lbl_8047B9FC;
                while (f27 < f28) {

                    _threadSwitch();
                    fn_800D37CC();
                    *(u32*)(sp + 0x1C) = tmp;
                    f30 = f0 - f31;
                    fn_800D3088();
                    f0 = f0 - f29;
                    f0 = f0 / f30;
                    f27 = f27 + f0;

                }
            }
            tmp = lbl_8047A449;
            if (tmp != 0) {
                r3 = 0x0;
                fn_801D0AFC();
            }
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r25 = r31 + 0xb0;
            r26 = r3;
            r27 = 0x0;
            do {
                r4 = r27 & 0xFF;
                r3 = r26;
                fn_8012AC08();
                r4 = *(u16*)((u8*)r25 + 0x0);
                fn_8011DCB4();
                r25 = r25 + 0x2;
                r27 = r27 + 0x1;
            } while ((s32)r27 < 6);
            tmp = 0x1;
            lbl_8047A458 = tmp;
            if ((s32)tmp != 0) {
                f1 = lbl_8047BA00;
                r3 = 0x3;
                fn_801C41C8();
                r3 = 0x1;
                fn_801C40F0();
                r3 = 0xa5;
                fn_80102510();
            }
            continue;
            tmp = *(u16*)((u8*)r28 + 0x0);
            r3 = *(u32*)((u8*)r30 + 0xAFC);
            lbl_8047A444 = r3;
            do {
                if (tmp != 0) {
                    r4 = r28;
                    r3 = r31 + 0x10;
                    fn_800F9E70();
                    break;
                }
                r3 = r3 & 0xFFFF;
                fn_801F6738();
                if (r3 != 0) {
                    fn_801F6600();
                    fn_800FA280();
                    r4 = r3;
                    r3 = r31 + 0x10;
                    fn_800F9E70();
                    break;
                }
                r3 = r31 + 0x10;
                r4 = (u32)lbl_8047BA1C;
                fn_800F9E70();
            } while (0);

            r4 = r29;
            r3 = 0x4d;
            fn_80132A38();
            r3 = 0x8;
            r4 = 0x3b57;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            tmp = 0x5;
            lbl_8047A458 = tmp;
            continue;
            r3 = 0x8;
            r4 = 0x3b5a;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            tmp = 0x5;
            lbl_8047A458 = tmp;
            continue;
            r3 = 0x8;
            r4 = 0x3b5d;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            tmp = 0x5;
            lbl_8047A458 = tmp;
            continue;
            r3 = (u32)sp + 0x8;
            r4 = (u32)sp + 0x9;
            r5 = (u32)sp + 0xa;
            fn_80034B5C();
            r4 = *(u8*)(sp + 0xA);
            tmp = r31 + 0xbc;
            r3 = *(u8*)(sp + 0x8);
            r5 = (s8)r4;
            r4 = *(u8*)(sp + 0x9);
            r5 = r5 * 0x28;
            r5 = r5 + 0x3ac;
            r5 = tmp + r5;
            fn_80034830();
            r4 = lbl_8047A450;
            r3 = 0x990000;
            r4 = r4 + 0x1;
            if (r4 > tmp) {
                r4 = tmp;
            }
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x8;
            r4 = 0x3b5e;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0x1;
            fn_801069FC();
            tmp = lbl_8047A450;
            if (tmp == 0) {
                r3 = 0x0;
                fn_801D0AFC();
            }
            tmp = 0xb;
            lbl_8047A458 = tmp;
            continue;
            fn_800327FC();
            continue;
            r3 = lbl_8047A450;
            r4 = 0x0;
            tmp = lbl_8047A44C;
            lbl_8047A44A = r4;
            if (r3 > tmp) {
                r3 = 0x4ce;
                r4 = 0x0;
                r5 = 0xff;
                fn_80165668();
                r3 = 0x990000;
                r8 = lbl_8047A450;
                if (r8 <= tmp) {
                } else {

                    r8 = tmp;
                }
                lbl_8047A44C = r8;
                r3 = 0x8;
                r7 = lbl_8047A430;
                r4 = 0x3b75;
                r5 = 0x1;
                r6 = 0x0;
                *(u32*)((u8*)r7 + 0x49C8) = r8;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
            }
            tmp = 0x0;
            r4 = r31 + 0x0;
            lbl_8047A43C = tmp;
            r3 = 0x0;
            *(u8*)((u8*)r31 + 0x0) = tmp;
            *(u8*)((u8*)r4 + 0x1) = tmp;
            *(u8*)((u8*)r4 + 0x2) = tmp;
            *(u8*)((u8*)r4 + 0x3) = tmp;
            *(u8*)((u8*)r4 + 0x4) = tmp;
            *(u8*)((u8*)r4 + 0x5) = tmp;
            *(u8*)((u8*)r4 + 0x6) = tmp;
            *(u8*)((u8*)r4 + 0x7) = tmp;
            *(u8*)((u8*)r4 + 0x8) = tmp;
            *(u8*)((u8*)r4 + 0x9) = tmp;
            *(u8*)((u8*)r4 + 0xA) = tmp;
            *(u8*)((u8*)r4 + 0xB) = tmp;
            *(u8*)((u8*)r4 + 0xC) = tmp;
            *(u8*)((u8*)r4 + 0xD) = tmp;
            *(u8*)((u8*)r4 + 0xE) = tmp;
            fn_801D0AFC();
            r3 = 0x8;
            r4 = 0x3bd7;
            r5 = 0x1;
            r6 = 0x0;
            fn_80106D3C();
            r3 = 0xa9;
            r4 = 0x1;
            fn_8010264C();
            r27 = r3;
            r3 = 0xa9;
            fn_80102510();
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r27 != 0) {
                if ((s32)r27 < 0) {
                    if ((s32)r27 < (s32)-0x1) {
                        goto L_80035AB4;
                    }
                    if ((s32)r27 >= 2) goto L_80035AB4;

                } else {
                    tmp = 0x3;
                    lbl_8047A458 = tmp;
                    continue;
                }
                tmp = 0x1;
                lbl_8047A458 = tmp;

                    } else {
                tmp = 0x1;
                lbl_8047A458 = tmp;
                    }
        L_80035AB4:
            f27 = lbl_8047B9F8;
            f28 = lbl_8047BA08;
            r27 = 0x43300000;
            f30 = lbl_8047BA10;
            f31 = lbl_8047B9FC;
            while (f27 < f31) {

                _threadSwitch();
                fn_800D37CC();
                *(u32*)(sp + 0x1C) = tmp;
                f29 = f0 - f28;
                fn_800D3088();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;

            }
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r25 = r31 + 0xb0;
            r26 = r3;
            r27 = 0x0;
            do {
                r4 = r27 & 0xFF;
                r3 = r26;
                fn_8012AC08();
                r4 = *(u16*)((u8*)r25 + 0x0);
                fn_8011DCB4();
                r25 = r25 + 0x2;
                r27 = r27 + 0x1;
            } while ((s32)r27 < 6);
            tmp = lbl_8047A458;
            if ((s32)tmp != 0) {
                f1 = lbl_8047BA00;
                r3 = 0x3;
                fn_801C41C8();
                r3 = 0x1;
                fn_801C40F0();
                r3 = 0xa5;
                fn_80102510();
            }
            continue;
            fn_8003258C();
            continue;
            tmp = lbl_8047A450;
            if (tmp > 0x1e) {
                r3 = 0x8;
                r4 = 0x3b5c;
                r5 = 0x1;
                r6 = 0x0;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
            } else {

                r3 = 0x8;
                r4 = 0x3b5f;
                r5 = 0x1;
                r6 = 0x0;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
            }
            r3 = 0x1;
            tmp = 0xb;
            lbl_8047A44A = r3;
            lbl_8047A458 = tmp;
        }
    }
    fn_800FF52C();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        fn_800FF660();
    } else {

        r3 = 0x80;
        r4 = 0x0;
        fn_80113828();
    }
    tmp = 0x0;
    lbl_8047A438 = tmp;
    return;
}

/* 0x80035C48 | 0x128 */
void fn_80035C48(void) {
    extern u8 lbl_803A3288[];
    extern u32 lbl_8047A430;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u8 lbl_8047A440;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u32 lbl_8047A44C;
    extern u8 lbl_8047A454;
    extern u32 lbl_8047A458;
    extern void* fn_80083AF4(s32, s32);
    extern s32 fn_80083BF8(s32);
    extern u32 fn_800FA280(u32);
    extern u32 fn_80113F48(void);
    extern u8* fn_80129280(s32, s32);
    extern u32 fn_801653C4(void);
    extern void fn_80165A20(u32, s32, s32);
    extern void fn_80176E0C(u32, u32, s32, s32);
    extern void GScharLenCpy(void*, u32, s32);
    u8* work;
    u32 soundId;
    u8 resetFlag;

    if (lbl_8047A438 == 0) {
        if (lbl_8047A439 != 0) {
            lbl_8047A458 = 2;
        } else {
            lbl_8047A458 = 1;
        }
        fn_80176E0C(fn_80113F48(), 0x11171800, 0, 0);
        if (lbl_8047A439 != 0) {
            soundId = 0x446;
        } else {
            soundId = 0x4CD;
        }
        if (soundId != fn_801653C4()) {
            fn_80165A20(soundId, 0, 0x7F);
        }
        lbl_8047A454 = 0;
        lbl_8047A449 = 0;
        lbl_8047A444 = 0x21;
        GScharLenCpy(lbl_803A3288, fn_800FA280(0x3CD1), 0x50);
        *(u16*)(lbl_803A3288 + 0x9E) = 0;
        work = fn_80129280(0, 0xD);
        lbl_8047A430 = (u32)work;
        resetFlag = lbl_8047A440;
        lbl_8047A44C = *(u32*)(work + 0x49C8);
        if (resetFlag != 0) {
            if (fn_80083BF8(0) > 0) {
                *(u16*)fn_80083AF4(0, 0) = 0;
            }
            lbl_8047A440 = 0;
        }
    }
    return;
}

/* 0x80035D70 | 0x30 */
#pragma push
#pragma peephole off
void fn_80035D70(void) {
    extern u8 lbl_8047A439;
    extern void _threadSwitch();
    extern void fn_800FF730(s32 r3);
    u32 tmp = 0;

    tmp = 0x1;
    lbl_8047A439 = tmp;
    fn_800FF730(0x393);
    _threadSwitch();
    return;
}
#pragma pop

/* 0x80035DA0 | 0x34 */
#pragma push
#pragma peephole off
void fn_80035DA0(void) {
    extern u8 lbl_8047A439;
    extern void _threadSwitch();
    extern void fn_80113828(s32 r3, s32 r4);
    u32 tmp = 0;

    tmp = 0x0;
    lbl_8047A439 = tmp;
    fn_80113828(0x393, 0x0);
    _threadSwitch();
    return;
}
#pragma pop

/* 0x80035DD4 | 0x30 */
#pragma push
#pragma peephole off
void fn_80035DD4(void) {
    extern f32 lbl_8047BA30;
    extern void fn_801C40F0(s32 r3);
    extern void fn_801C41C8(s32 r3, f32 f1);

    fn_801C41C8(0x3, lbl_8047BA30);
    fn_801C40F0(0x1);
    return;
}
#pragma pop
