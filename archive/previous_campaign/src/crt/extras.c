#include "dolphin/types.h"

/*
 * extras.c - CRT library functions.
 *
 * Stub implementations for function coverage.
 */

/* fn_800CAAE0 - 0x800CAAE0 | size: 0xD0 */
void fn_800CAAE0(void) {
    extern u32 lbl_8047AA18;
    extern void fn_800998B8();
    extern void fn_800C3A40();
    extern void fn_800CF47C();
    extern void fn_800CF4EC();
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
    f32 f0 = 0.0f;

    r31 = r6;
    r30 = r5;
    r29 = r4;
    r28 = r3;
    fn_800998B8();
    tmp = r3 & 0x20000000;
    if ((s32)tmp == 0) {
        tmp = lbl_8047AA18;
        r3 = 0x0;
        if ((s32)tmp == 0) {
            r3 = 0x10000;
            fn_800CF47C();
            if ((s32)r3 == 0) {
                tmp = 0x1;
                lbl_8047AA18 = tmp;
        }
        }
        if ((s32)r3 != 0) {
            r3 = 0x1;
            return;
        }
        r4 = *(u32*)((u8*)r30 + 0x0);
        r3 = r29;
        fn_800CF4EC();
        if ((s32)r3 != 0) {
            tmp = 0x0;
            r3 = 0x1;
            *(u32*)((u8*)r30 + 0x0) = tmp;
            return;
    }
    }
    r3 = r28;
    r4 = r29;
    r5 = r30;
    r6 = r31;
    fn_800C3A40();
    r3 = 0x0;

    return;
}

/* __ieee754_acos - 0x800CABB0 | size: 0x23C */
void __ieee754_acos(void) {
    extern u8 lbl_80478AC0[];
    extern f64 lbl_8047C418;
    extern f64 lbl_8047C420;
    extern f64 lbl_8047C428;
    extern f64 lbl_8047C430;
    extern f64 lbl_8047C438;
    extern f64 lbl_8047C440;
    extern f64 lbl_8047C448;
    extern f64 lbl_8047C450;
    extern f64 lbl_8047C458;
    extern f64 lbl_8047C460;
    extern f64 lbl_8047C468;
    extern f64 lbl_8047C470;
    extern f64 lbl_8047C478;
    extern f64 lbl_8047C480;
    extern f64 lbl_8047C488;
    extern f64 lbl_8047C490;
    extern f64 lbl_8047C498;
    extern void fn_800CE77C();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f31 = 0.0f;

    tmp = 0x3FF00000;
    r3 = r4 & 0x7FFFFFFF;
    if ((s32)r3 >= (s32)tmp) {
        /* subis r3, r3, 0x3ff0 */;
        /* or. tmp, r3, tmp */;
        if ((s32)r3 == (s32)tmp) {
            if ((s32)r4 > 0) {
                f1 = lbl_8047C418;
                return;
            }
            f1 = lbl_8047C420;
            return;
        }
        r3 = (u32)lbl_80478AC0;
        f1 = *(f32*)lbl_80478AC0;
        return;
    }
    tmp = 0x3FE00000;
    if ((s32)r3 < (s32)tmp) {
        tmp = 0x3C600000;
        if ((s32)r3 <= (s32)tmp) {
            f1 = lbl_8047C428;
            return;
        }
        f10 = f1 * f1;
        f2 = lbl_8047C460;
        f0 = lbl_8047C458;
        f3 = lbl_8047C450;
        f8 = lbl_8047C448;
        f4 = f2 * f10 + f0;
        f2 = lbl_8047C488;
        f0 = lbl_8047C480;
        f7 = lbl_8047C440;
        f9 = f10 * f4 + f3;
        f4 = lbl_8047C478;
        f6 = lbl_8047C438;
        f5 = f2 * f10 + f0;
        f3 = lbl_8047C470;
        f2 = lbl_8047C468;
        f8 = f10 * f9 + f8;
        f0 = lbl_8047C430;
        f9 = lbl_8047C428;
        f4 = f10 * f5 + f4;
        f5 = f10 * f8 + f7;
        f3 = f10 * f4 + f3;
        f4 = f10 * f5 + f6;
        f2 = f10 * f3 + f2;
        f3 = f10 * f4;
        f2 = f3 / f2;
        f0 = -(f1 * f2 - f0);
        f0 = f1 - f0;
        f1 = f9 - f0;
        return;
    }
    if ((s32)r4 < 0) {
        f0 = lbl_8047C468;
        f2 = lbl_8047C490;
        f0 = f0 + f1;
        f31 = f2 * f0;
        f1 = f31;
        fn_800CE77C();
        f3 = lbl_8047C460;
        f2 = lbl_8047C458;
        f0 = lbl_8047C450;
        f4 = f3 * f31 + f2;
        f5 = lbl_8047C448;
        f3 = lbl_8047C488;
        f2 = lbl_8047C480;
        f7 = lbl_8047C440;
        f8 = f31 * f4 + f0;
        f0 = lbl_8047C478;
        f2 = f3 * f31 + f2;
        f6 = lbl_8047C438;
        f4 = lbl_8047C470;
        f8 = f31 * f8 + f5;
        f3 = lbl_8047C468;
        f5 = f31 * f2 + f0;
        f2 = lbl_8047C430;
        f7 = f31 * f8 + f7;
        f8 = lbl_8047C498;
        f0 = lbl_8047C420;
        f4 = f31 * f5 + f4;
        f5 = f31 * f7 + f6;
        f3 = f31 * f4 + f3;
        f4 = f31 * f5;
        f3 = f4 / f3;
        f2 = f3 * f1 - f2;
        f1 = f1 + f2;
        f1 = -(f8 * f1 - f0);
        return;
    }
    f0 = lbl_8047C468;
    f2 = lbl_8047C490;
    f0 = f0 - f1;
    f31 = f2 * f0;
    f1 = f31;
    fn_800CE77C();
    f2 = lbl_8047C460;
    tmp = 0x0;
    f0 = lbl_8047C458;
    f3 = f2 * f31 + f0;
    f0 = lbl_8047C450;
    *(u32*)(sp + 0x14) = tmp;
    f2 = lbl_8047C448;
    f5 = f31 * f3 + f0;
    f4 = lbl_8047C488;
    f0 = lbl_8047C480;
    f3 = -(f9 * f9 - f31);
    f7 = lbl_8047C440;
    f8 = f31 * f5 + f2;
    f2 = lbl_8047C478;
    f5 = f4 * f31 + f0;
    f6 = lbl_8047C438;
    f0 = lbl_8047C470;
    f7 = f31 * f8 + f7;
    f4 = lbl_8047C468;
    f5 = f31 * f5 + f2;
    f2 = lbl_8047C498;
    f6 = f31 * f7 + f6;
    f5 = f31 * f5 + f0;
    f0 = f1 + f9;
    f6 = f31 * f6;
    f4 = f31 * f5 + f4;
    f0 = f3 / f0;
    f3 = f6 / f4;
    f0 = f3 * f1 + f0;
    f0 = f9 + f0;
    f1 = f2 * f0;

    return;
}

/* __ieee754_asin - 0x800CADEC | size: 0x238 */
void __ieee754_asin(void) {
    extern u8 lbl_80478AC0[];
    extern f64 lbl_8047C4A0;
    extern f64 lbl_8047C4A8;
    extern f64 lbl_8047C4B0;
    extern f64 lbl_8047C4B8;
    extern f64 lbl_8047C4C0;
    extern f64 lbl_8047C4C8;
    extern f64 lbl_8047C4D0;
    extern f64 lbl_8047C4D8;
    extern f64 lbl_8047C4E0;
    extern f64 lbl_8047C4E8;
    extern f64 lbl_8047C4F0;
    extern f64 lbl_8047C4F8;
    extern f64 lbl_8047C500;
    extern f64 lbl_8047C508;
    extern f64 lbl_8047C510;
    extern f64 lbl_8047C518;
    extern f64 lbl_8047C520;
    extern void fn_800CE77C();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
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
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    tmp = 0x3FF00000;
    r30 = r31 & 0x7FFFFFFF;
    if ((s32)r30 >= (s32)tmp) {
        /* subis r3, r30, 0x3ff0 */;
        /* or. tmp, r3, tmp */;
        if ((s32)r30 == (s32)tmp) {
            f0 = lbl_8047C4A8;
            f2 = lbl_8047C4A0;
            f0 = f0 * f1;
            f1 = f2 * f1 + f0;
            return;
        }
        r3 = (u32)lbl_80478AC0;
        f1 = *(f32*)lbl_80478AC0;
        return;
    }
    tmp = 0x3FE00000;
    if ((s32)r30 < (s32)tmp) {
        tmp = 0x3E400000;
        if ((s32)r30 < (s32)tmp) {
            f2 = lbl_8047C4B0;
            f0 = lbl_8047C4B8;
            f2 = f2 + f1;
            if (f2 > f0) {
                return;
            }
            f31 = f1 * f1;
            }
        f1 = lbl_8047C4E8;
        f0 = lbl_8047C4E0;
        f2 = lbl_8047C4D8;
        f3 = f1 * f31 + f0;
        f6 = lbl_8047C4D0;
        f1 = lbl_8047C508;
        f0 = lbl_8047C500;
        f5 = lbl_8047C4C8;
        f7 = f31 * f3 + f2;
        f2 = lbl_8047C4F8;
        f3 = f1 * f31 + f0;
        f4 = lbl_8047C4C0;
        f1 = lbl_8047C4F0;
        f6 = f31 * f7 + f6;
        f0 = lbl_8047C4B8;
        f2 = f31 * f3 + f2;
        f3 = f31 * f6 + f5;
        f1 = f31 * f2 + f1;
        f2 = f31 * f3 + f4;
        f0 = f31 * f1 + f0;
        f1 = f31 * f2;
        f0 = f1 / f0;
        f1 = f7 * f0 + f7;
        return;
    }
    /* fabs */ f1 = (f1 < 0) ? -f1 : f1;
    f9 = lbl_8047C4B8;
    f0 = lbl_8047C510;
    f7 = lbl_8047C4E8;
    f8 = f9 - f1;
    f3 = lbl_8047C4E0;
    f6 = lbl_8047C4D8;
    f5 = lbl_8047C4D0;
    f31 = f0 * f8;
    f2 = lbl_8047C508;
    f0 = lbl_8047C500;
    f4 = lbl_8047C4C8;
    f1 = lbl_8047C4F8;
    f7 = f7 * f31 + f3;
    f3 = lbl_8047C4C0;
    f2 = f2 * f31 + f0;
    f0 = lbl_8047C4F0;
    f6 = f31 * f7 + f6;
    f1 = f31 * f2 + f1;
    f2 = f31 * f6 + f5;
    f0 = f31 * f1 + f0;
    f1 = f31 * f2 + f4;
    f29 = f31 * f0 + f9;
    f0 = f31 * f1 + f3;
    f1 = f31;
    f30 = f31 * f0;
    fn_800CE77C();
    r3 = 0x3FEF0000;
    tmp = r3 + 0x3333;
    if ((s32)r30 >= (s32)tmp) {
        f4 = f30 / f29;
        f2 = lbl_8047C518;
        f0 = lbl_8047C4A8;
        f3 = lbl_8047C4A0;
        f1 = f1 * f4 + f1;
        f0 = f2 * f1 - f0;
        f1 = f3 - f0;
    } else {

        tmp = 0x0;
        f7 = lbl_8047C518;
        f5 = f30 / f29;
        *(u32*)(sp + 0x14) = tmp;
        f0 = lbl_8047C4A8;
        f2 = lbl_8047C520;
        f4 = -(f8 * f8 - f31);
        f3 = f1 + f8;
        f6 = f7 * f1;
        f1 = f4 / f3;
        f1 = -(f7 * f1 - f0);
        f0 = -(f7 * f8 - f2);
        f1 = f6 * f5 - f1;
        f0 = f1 - f0;
        f1 = f2 - f0;
    }
    if ((s32)r31 > 0) {
        return;
    }
    f1 = -f1;

    return;
}

/* __ieee754_atan2 - 0x800CB024 | size: 0x290 */
void __ieee754_atan2(void) {
    extern f64 lbl_8047C528;
    extern f64 lbl_8047C530;
    extern f64 lbl_8047C538;
    extern f64 lbl_8047C540;
    extern f64 lbl_8047C548;
    extern f64 lbl_8047C550;
    extern f64 lbl_8047C558;
    extern f64 lbl_8047C560;
    extern f64 lbl_8047C568;
    extern f64 lbl_8047C570;
    extern f64 lbl_8047C578;
    extern void fn_800CD85C();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r3 = 0x7FF00000;
    tmp = -r8;
    tmp = r8 | tmp;
    r6 = r4 & 0x7FFFFFFF;
    tmp = (u32)tmp >> 31;
    tmp = r6 | tmp;
    r7 = r5 & 0x7FFFFFFF;
    if (tmp > r3) { f1 = f1 + f0; return; }
    tmp = -r9;
    tmp = r9 | tmp;
    tmp = (u32)tmp >> 31;
    tmp = r7 | tmp;
    if (tmp > r3) {

        f1 = f1 + f0;
        return;
    }
    /* subis tmp, r4, 0x3ff0 */;
    /* or. tmp, tmp, r8 */;
    if (tmp == r3) {
        fn_800CD85C();
        return;
    }
    /* or. tmp, r7, r9 */;
    tmp = ((r4 << 2) | ((u32)r4 >> 30)) & 0x00000002;
    r31 = tmp;
    r31 = (r31 & ~0x00000001) | (((r5 << 1) | ((u32)r5 >> 31)) & 0x00000001);
    if (tmp == r3) {
        if ((s32)r31 == 2) { f1 = lbl_8047C528; return; }
        if ((s32)r31 < 2) {
            if ((s32)r31 >= 0) return;
            goto L_800CB0F8;
        }
        if ((s32)r31 < 4) {
            f1 = lbl_8047C530;
            return;


        }
    }
L_800CB0F8:
do {
    /* or. tmp, r6, r8 */;
    if ((s32)r31 == 4) {
        if ((s32)r5 < 0) {
            f1 = lbl_8047C538;
            return;
        }
        f1 = lbl_8047C540;
        return;
    }
    /* subis tmp, r6, 0x7ff0 */;
    if (tmp == 0) {
        /* subis tmp, r7, 0x7ff0 */;
        if (tmp == 0) {
            if ((s32)r31 == 2) { f1 = lbl_8047C558; return; }
            if ((s32)r31 < 2) {
                if ((s32)r31 == 0) { f1 = lbl_8047C548; return; }
                if ((s32)r31 >= 0) { f1 = lbl_8047C550; return; }
                break;
            }
            if ((s32)r31 >= 4) break;
            f1 = lbl_8047C560;
            return;


        }
        if ((s32)r31 == 2) { f1 = lbl_8047C528; return; }
        if ((s32)r31 < 2) {
            if ((s32)r31 == 0) { f1 = lbl_8047C568; return; }
            if ((s32)r31 >= 0) { f1 = lbl_8047C570; return; }
            break;
        }
        if ((s32)r31 >= 4) break;
        f1 = lbl_8047C530;
        return;


    }
} while (0);
    /* subis tmp, r7, 0x7ff0 */;
    if (tmp == 0) {
        if ((s32)r5 < 0) {
            f1 = lbl_8047C538;
            return;
        }
        f1 = lbl_8047C540;
        return;
    }
    tmp = r7 - r6;
    tmp = (s32)tmp >> 20;
    do {
        if ((s32)tmp > 0x3c) {
            f0 = lbl_8047C540;
            break;
        }
        if ((s32)r4 < 0 && (s32)tmp < (s32)-0x3c) {

            f0 = lbl_8047C568;
            break;
        }
        f0 = f1 / f0;
        /* fabs */ f1 = (f0 < 0) ? -f0 : f0;
        fn_800CD85C();
    } while (0);

    if ((s32)r31 != 1) {
        if ((s32)r31 < 1) {
            if ((s32)r31 >= 0) return;
            goto L_800CB28C;
        }
        if ((s32)r31 >= 3) goto L_800CB28C;


    } else {
    *(u32*)(sp + 0x18) = tmp;
    return;
    }
    f0 = lbl_8047C578;
    f2 = lbl_8047C528;
    f0 = f1 - f0;
    f1 = f2 - f0;
    return;
L_800CB28C:
    f1 = lbl_8047C578;
    f0 = lbl_8047C528;
    f1 = f2 - f1;
    f1 = f1 - f0;

    return;
}

/* __ieee754_exp - 0x800CB2B4 | size: 0x224 */
void __ieee754_exp(void) {
    extern u8 lbl_80270008[];
    extern f64 lbl_8047C580;
    extern f64 lbl_8047C588;
    extern f64 lbl_8047C590;
    extern f64 lbl_8047C598;
    extern f64 lbl_8047C5A0;
    extern f64 lbl_8047C5A8;
    extern f64 lbl_8047C5B0;
    extern f64 lbl_8047C5B8;
    extern f64 lbl_8047C5C0;
    extern f64 lbl_8047C5C8;
    extern f64 lbl_8047C5D0;
    extern f64 lbl_8047C5D8;
    extern f64 lbl_8047C5E0;
    extern f64 lbl_8047C5E8;
    extern f64 lbl_8047C5F0;
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;

    r3 = 0x40860000;
    r4 = (u32)lbl_80270008;
    tmp = r3 + 0x2e42;
    r5 = (u32)lbl_80270008;
    r4 = r8 & 0x7FFFFFFF;
    r7 = (u32)r8 >> 31;
    if (r4 >= tmp) {
        tmp = 0x7FF00000;
        if (r4 >= tmp) {
            r3 = r8 & 0xFFFFF;
            /* or. tmp, r3, tmp */;
            if (r4 != tmp) {
                f1 = f1 + f1;
                return;
            }
            if ((s32)r7 == 0) {
                return;
            }
            f1 = lbl_8047C580;
            return;
        }
        f0 = lbl_8047C588;
        if (f1 > f0) {
            f1 = lbl_8047C590;
            return;
        }
        f0 = lbl_8047C598;
        if (f1 < f0) {
            f1 = lbl_8047C580;
            return;
    }
    }
    r3 = 0x3FD60000;
    tmp = r3 + 0x2e42;
    if (r4 > tmp) {
        r3 = 0x3FF10000;
        if (r4 < tmp) {
            r6 = r7 << 3;
            r4 = r5 + 0x10;
            r3 = r5 + 0x20;
            f0 = *(f64*)(r4 + r6);
            tmp = 0x1 - r7;
            f8 = *(f64*)(r3 + r6);
            r6 = tmp - r7;
            f7 = f1 - f0;
        } else {

            r4 = r7 << 3;
            r3 = r5 + 0x0;
            f1 = lbl_8047C5A0;
            tmp = 0x43300000;
            f0 = *(f64*)(r3 + r4);
            *(u32*)(sp + 0x20) = tmp;
            f2 = f1 * f4 + f0;
            f3 = lbl_8047C5F0;
            f1 = *(f64*)((u8*)r5 + 0x10);
            f0 = *(f64*)((u8*)r5 + 0x20);
            f2 = (f64)(s32)f2;
            *(u32*)(sp + 0x24) = tmp;
            f2 = f2 - f3;
            f7 = -(f2 * f1 - f4);
            f8 = f2 * f0;
        }
        f0 = f7 - f8;

    } else {
        tmp = 0x3E300000;
        if (r4 < tmp) {
            f1 = lbl_8047C5A8;
            f0 = lbl_8047C5B0;
            f1 = f1 + f2;
            if (f1 > f0) {
                f1 = f0 + f2;
                return;
            }
            r6 = 0x0;
        }
            }
    f4 = lbl_8047C5D8;
    f6 = f5 * f5;
    f3 = lbl_8047C5D0;
    f2 = lbl_8047C5C8;
    f1 = lbl_8047C5C0;
    f0 = lbl_8047C5B8;
    f3 = f4 * f6 + f3;
    f2 = f6 * f3 + f2;
    f1 = f6 * f2 + f1;
    f0 = f6 * f1 + f0;
    f3 = -(f6 * f0 - f5);
    if ((s32)r6 == 0) {
        f0 = lbl_8047C5E0;
        f1 = f5 * f3;
        f2 = lbl_8047C5B0;
        f0 = f3 - f0;
        f0 = f1 / f0;
        f0 = f0 - f5;
        f1 = f2 - f0;
        return;
    }
    f0 = lbl_8047C5E0;
    f1 = f5 * f3;
    f2 = lbl_8047C5B0;
    f0 = f0 - f3;
    f0 = f1 / f0;
    f0 = f8 - f0;
    f0 = f0 - f7;
    f0 = f2 - f0;
    if ((s32)r6 >= (s32)-0x3fd) {
        tmp = r6 << 20;
        tmp = r3 + tmp;
        *(u32*)(sp + 0x10) = tmp;
        return;
    }
    tmp = r6 + 0x3e8;
    tmp = tmp << 20;
    f1 = lbl_8047C5E8;
    tmp = r3 + tmp;
    *(u32*)(sp + 0x10) = tmp;
    f1 = f1 * f0;

    return;
}

/* __ieee754_fmod - 0x800CB4D8 | size: 0x33C */
void __ieee754_fmod(void) {
    extern u8 lbl_80270038[];
    u8 sp[0x20];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r8 = r10 & 0x7FFFFFFF;
    /* clrrwi tmp, r6, 31 */;
    /* or. r3, r8, r5 */;
    r7 = r6 ^ tmp;
    do {
        if ((s32)tmp == 0) break;
        r6 = 0x7FF00000;
        if ((s32)r7 >= (s32)r6) break;
        r3 = -r5;
        r3 = r5 | r3;
        r3 = (u32)r3 >> 31;
        r3 = r8 | r3;
        if (r3 > r6) break;
    } while (0);
    if (r3 > r6) {
        f0 = f1 * f0;
        f1 = f0 / f0;
        return;
    }
    do {
        if ((s32)r7 > (s32)r8) break;
        if ((s32)r7 < (s32)r8) return;
        if (r4 < r5) {

            return;
        }
        if (r4 != r5) break;
        r3 = (u32)lbl_80270038;
        tmp = ((tmp << 4) | ((u32)tmp >> 28)) & 0x00000008;
        r3 = (u32)lbl_80270038;
        f1 = *(f64*)(r3 + tmp);
        return;
    } while (0);

    r3 = 0x100000;
    if ((s32)r7 < (s32)r3) {
        if ((s32)r7 == 0) {
            r3 = r4;
            r11 = -0x413;
            while ((s32)r3 > 0) {

                r3 = r3 << 1;

            }
            goto L_800CB5D0;
        }
        r3 = r7 << 11;
        r11 = -0x3fe;
        while ((s32)r3 > 0) {

            r3 = r3 << 1;

        }

    } else {
        r3 = (s32)r7 >> 20;
    }
L_800CB5D0:
    r3 = 0x100000;
    if ((s32)r8 < (s32)r3) {
        if ((s32)r8 == 0) {
            r6 = r5;
            r3 = -0x413;
            while ((s32)r6 > 0) {

                r6 = r6 << 1;

            }
            goto L_800CB62C;
        }
        r6 = r8 << 11;
        r3 = -0x3fe;
        while ((s32)r6 > 0) {

            r6 = r6 << 1;

        }

    } else {
        r3 = (s32)r8 >> 20;
    }
L_800CB62C:
    do {
        if ((s32)r11 >= (s32)-0x3fe) {
            r6 = r7 & 0xFFFFF;
            r9 = r6 | (0x10 << 16);
            break;
        }
        r9 = -0x3fe - r11;
        if ((s32)r9 <= 0x1f) {
            r6 = 0x20 - r9;
            r7 = r7 << r9;
            r6 = (u32)r4 >> r6;
            r4 = r4 << r9;
            r9 = r7 | r6;
            break;
        }
        r9 = r4 << r6;
        r4 = 0x0;
    } while (0);

    do {
        if ((s32)r3 >= (s32)-0x3fe) {
            r6 = r10 & 0xFFFFF;
            r7 = r6 | (0x10 << 16);
            break;
        }
        r10 = -0x3fe - r3;
        if ((s32)r10 <= 0x1f) {
            r6 = 0x20 - r10;
            r7 = r8 << r10;
            r6 = (u32)r5 >> r6;
            r5 = r5 << r10;
            r7 = r7 | r6;
            break;
        }
        r7 = r5 << r6;
        r5 = 0x0;
    } while (0);

    /* subf. r6, r3, r11 */;
    ctr_fn = (void(*)(void))r6;
    if ((s32)r10 != 0x1f) {
        do {
            r8 = r9 - r7;
            r10 = r4 - r5;
            if (r4 < r5) {
            }
            if ((s32)r8 < 0) {
                r6 = (u32)r4 >> 31;
                r4 = r4 + r4;
                r6 = r9 + r6;
                r9 = r9 + r6;
            } else {

                /* or. r4, r8, r10 */;
                if ((s32)r8 == 0) {
                    r3 = (u32)lbl_80270038;
                    tmp = ((tmp << 4) | ((u32)tmp >> 28)) & 0x00000008;
                    r3 = (u32)lbl_80270038;
                    f1 = *(f64*)(r3 + tmp);
                    return;
                }
                r6 = (u32)r10 >> 31;
                r4 = r10 + r10;
                r9 = r8 + r6;
                r9 = r8 + r9;
            }
        } while (--ctr != 0);
    }
    r6 = r9 - r7;
    r5 = r4 - r5;
    if (r4 < r5) {
    }
    if ((s32)r6 >= 0) {
        r9 = r6;
        r4 = r5;
    }
    /* or. r5, r9, r4 */;
    if ((s32)r6 == 0) {
        r3 = (u32)lbl_80270038;
        tmp = ((tmp << 4) | ((u32)tmp >> 28)) & 0x00000008;
        r3 = (u32)lbl_80270038;
        f1 = *(f64*)(r3 + tmp);
        return;
    }
    r5 = 0x100000;
    while ((s32)r9 < (s32)r5) {

        r6 = (u32)r4 >> 31;
        r4 = r4 + r4;
        r6 = r9 + r6;
        r9 = r9 + r6;

    }
    if ((s32)r3 >= (s32)-0x3fe) {
        r3 = r3 + 0x3ff;
        /* subis r5, r9, 0x10 */;
        r3 = r3 << 20;
        r3 = r5 | r3;
        tmp = r3 | tmp;
        *(u32*)(sp + 0x8) = tmp;
        return;
    }
    r6 = -0x3fe - r3;
    if ((s32)r6 <= 0x14) {
        r3 = 0x20 - r6;
        r4 = (u32)r4 >> r6;
        r3 = r9 << r3;
        r9 = (s32)r9 >> r6;
        r3 = r4 | r3;

    } else if ((s32)r6 <= 0x1f) {
        r5 = 0x20 - r6;
        r3 = (u32)r4 >> r6;
        r4 = r9 << r5;
        r9 = tmp;
        r3 = r4 | r3;

    } else {
        r3 = (s32)r9 >> r3;
        r9 = tmp;
    }
    tmp = r9 | tmp;
    *(u32*)(sp + 0x8) = tmp;

    return;
}

/* __ieee754_log - 0x800CB814 | size: 0x27C */
void __ieee754_log(void) {
    extern u32 lbl_8047AA10;
    extern f64 lbl_8047AA20;
    extern f64 lbl_8047C5F8;
    extern f64 lbl_8047C600;
    extern f64 lbl_8047C608;
    extern f64 lbl_8047C610;
    extern f64 lbl_8047C618;
    extern f64 lbl_8047C620;
    extern f64 lbl_8047C628;
    extern f64 lbl_8047C630;
    extern f64 lbl_8047C638;
    extern f64 lbl_8047C640;
    extern f64 lbl_8047C648;
    extern f64 lbl_8047C650;
    extern f64 lbl_8047C658;
    extern f64 lbl_8047C660;
    extern f64 lbl_8047C668;
    extern f64 lbl_8047C670;
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;

    tmp = 0x100000;
    r8 = 0x0;
    if ((s32)r3 < (s32)tmp) {
        tmp = r3 & 0x7FFFFFFF;
        /* or. tmp, tmp, r4 */;
        if ((s32)r3 == (s32)tmp) {
            f1 = lbl_8047C5F8;
            f0 = lbl_8047AA20;
            f1 = f1 / f0;
            return;
        }
        if ((s32)r3 < 0) {
            f1 = f1 - f1;
            f0 = lbl_8047AA20;
            tmp = 0x21;
            lbl_8047AA10 = tmp;
            f1 = f1 / f0;
            return;
        }
        f0 = lbl_8047C600;
        r8 = -0x36;
        f0 = f1 * f0;
    }
    tmp = 0x7FF00000;
    if ((s32)r3 >= (s32)tmp) {
        f1 = f0 + f0;
        return;
    }
    r6 = r3 & 0xFFFFF;
    r4 = (s32)r3 >> 20;
    r3 = r6 + (0x9 << 16);
    f0 = lbl_8047C608;
    r5 = r3 + 0x5f64;
    r8 = r4 + r8;
    r3 = r5 & 0x00100000;
    tmp = r6 + 0x2;
    r4 = r6 | r3;
    tmp = tmp & 0xFFFFF;
    /* extrwi r3, r5, 1, 11 */;
    r8 = r8 + r3;
    f0 = f1 - f0;
    if ((s32)tmp < 3) {
        f1 = lbl_8047AA20;
        if (f0 == f1) {
            if ((s32)r8 == 0) {
                return;
            }
            tmp = 0x43300000;
            f3 = lbl_8047C670;
            *(u32*)(sp + 0x10) = tmp;
            f0 = lbl_8047C618;
            f1 = lbl_8047C610;
            f2 = f2 - f3;
            f0 = f0 * f2;
            f1 = f1 * f2 + f0;
            return;
        }
        f3 = lbl_8047C628;
        f1 = f0 * f0;
        f2 = lbl_8047C620;
        f2 = -(f3 * f0 - f2);
        f5 = f2 * f1;
        if ((s32)r8 == 0) {
            f1 = f0 - f5;
            return;
        }
        tmp = 0x43300000;
        f4 = lbl_8047C670;
        *(u32*)(sp + 0x10) = tmp;
        f1 = lbl_8047C618;
        f2 = lbl_8047C610;
        f3 = f3 - f4;
        f1 = -(f1 * f3 - f5);
        f0 = f1 - f0;
        f1 = f2 * f3 - f0;
        return;
    }
    f1 = lbl_8047C630;
    r4 = 0x43300000;
    r3 = 0x70000;
    f1 = f1 + f0;
    /* subis r7, r6, 0x6 */;
    f8 = lbl_8047C650;
    f7 = lbl_8047C648;
    tmp = tmp - r6;
    f1 = f0 / f1;
    f6 = lbl_8047C640;
    /* or. r7, r7, tmp */;
    f4 = lbl_8047C668;
    f3 = lbl_8047C660;
    f11 = f1 * f1;
    f5 = lbl_8047C638;
    f2 = lbl_8047C658;
    f10 = lbl_8047C670;
    f12 = f11 * f11;
    f7 = f8 * f12 + f7;
    f3 = f4 * f12 + f3;
    f4 = f12 * f7 + f6;
    f2 = f12 * f3 + f2;
    f3 = f12 * f4 + f5;
    f2 = f12 * f2;
    f3 = f11 * f3;
    f5 = f9 - f10;
    f3 = f3 + f2;
    if ((s32)r8 > 0) {
        f2 = lbl_8047C620;
        f2 = f2 * f0;
        f6 = f2 * f0;
        if ((s32)r8 == 0) {
            f2 = f6 + f3;
            f1 = -(f1 * f2 - f6);
            f1 = f0 - f1;
            return;
        }
        f2 = lbl_8047C618;
        f3 = f6 + f3;
        f4 = lbl_8047C610;
        f2 = f2 * f5;
        f1 = f1 * f3 + f2;
        f1 = f6 - f1;
        f0 = f1 - f0;
        f1 = f4 * f5 - f0;
        return;
    }
    if ((s32)r8 == 0) {
        f2 = f0 - f3;
        f1 = -(f1 * f2 - f0);
        return;
    }
    f2 = lbl_8047C618;
    f3 = f0 - f3;
    f4 = lbl_8047C610;
    f2 = f2 * f5;
    f1 = f1 * f3 - f2;
    f0 = f1 - f0;
    f1 = f4 * f5 - f0;

    return;
}

/* __ieee754_pow - 0x800CBA90 | size: 0x830 */
void __ieee754_pow(void) {
    extern u8 lbl_80270048[];
    extern u8 lbl_80478AC0[];
    extern u32 lbl_8047AA10;
    extern f64 lbl_8047C678;
    extern f64 lbl_8047C680;
    extern f64 lbl_8047C688;
    extern f64 lbl_8047C690;
    extern f64 lbl_8047C698;
    extern f64 lbl_8047C6A0;
    extern f64 lbl_8047C6A8;
    extern f64 lbl_8047C6B0;
    extern f64 lbl_8047C6B8;
    extern f64 lbl_8047C6C0;
    extern f64 lbl_8047C6C8;
    extern f64 lbl_8047C6D0;
    extern f64 lbl_8047C6D8;
    extern f64 lbl_8047C6E0;
    extern f64 lbl_8047C6E8;
    extern f64 lbl_8047C6F0;
    extern f64 lbl_8047C6F8;
    extern f64 lbl_8047C700;
    extern f64 lbl_8047C708;
    extern f64 lbl_8047C710;
    extern f64 lbl_8047C718;
    extern f64 lbl_8047C720;
    extern f64 lbl_8047C728;
    extern f64 lbl_8047C730;
    extern f64 lbl_8047C738;
    extern f64 lbl_8047C740;
    extern f64 lbl_8047C748;
    extern f64 lbl_8047C750;
    extern f64 lbl_8047C758;
    extern f64 lbl_8047C760;
    extern f64 lbl_8047C768;
    extern f64 lbl_8047C770;
    extern f64 lbl_8047C778;
    extern f64 lbl_8047C780;
    extern void fn_800CDE88();
    extern void fn_800CE77C();
    u8 sp[0xB0];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;
    f32 f13 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r3 = (u32)lbl_80270048;
    r3 = (u32)lbl_80270048;
    r7 = r5 & 0x7FFFFFFF;
    /* or. r4, r7, r11 */;
    r6 = tmp & 0x7FFFFFFF;
    if ((s32)tmp == 0) {
        f1 = lbl_8047C678;
        return;
    }
    r4 = 0x7FF00000;
    if ((s32)r6 > (s32)r4) { f1 = f1 + f0; return; }
    /* subis r4, r6, 0x7ff0 */;
    if (r4 == 0) {
        if (r10 != 0) { f1 = f1 + f0; return; }
    }
    r4 = 0x7FF00000;
    if ((s32)r7 > (s32)r4) { f1 = f1 + f0; return; }
    /* subis r4, r7, 0x7ff0 */;
    if (r4 == 0 && r11 != 0) {

        f1 = f1 + f0;
        return;
    }
    r4 = 0x0;
    if ((s32)tmp < 0) {
        r8 = 0x43400000;
        if ((s32)r7 >= (s32)r8) {
            r4 = 0x2;
            goto L_800CBBCC;
        }
        r8 = 0x3FF00000;
        if ((s32)r7 < (s32)r8) goto L_800CBBCC;
        r8 = (s32)r7 >> 20;
        if ((s32)r8 > 0x14) {
            r8 = 0x34 - r8;
            r9 = (u32)r11 >> r8;
            r8 = r9 << r8;
            if (r11 == r8) {
                r4 = r9 & 0x1;
                r4 = 0x2 - r4;
            }
            goto L_800CBBCC;
        }
        if (r11 == 0) {
            r8 = 0x14 - r8;
            r9 = (s32)r7 >> r8;
            r8 = r9 << r8;
        }
        if ((s32)r7 == (s32)r8) {
            r4 = r9 & 0x1;
            r4 = 0x2 - r4;
        }
    }
L_800CBBCC:
    if (r11 == 0) {
        /* subis r8, r7, 0x7ff0 */;
        if (r8 == 0) {
            /* subis tmp, r6, 0x3ff0 */;
            /* or. tmp, tmp, r10 */;
            if (r8 == 0) {
                f1 = f0 - f0;
                return;
            }
            tmp = 0x3FF00000;
            if ((s32)r6 >= (s32)tmp) {
                if ((s32)r5 >= 0) {
                    return;
                }
                f1 = lbl_8047C680;
                return;
            }
            if ((s32)r5 < 0) {
                f1 = -f0;
                return;
            }
            f1 = lbl_8047C680;
            return;
        }
        /* subis r8, r7, 0x3ff0 */;
        if (r8 == 0) {
            if ((s32)r5 < 0) {
                f1 = lbl_8047C678;
                f1 = f1 / f0;
                return;
            }
            return;
        }
        /* subis r8, r5, 0x4000 */;
        if (r8 == 0) {
            f1 = f0 * f0;
            return;
        }
        /* subis r8, r5, 0x3fe0 */;
        if (r8 == 0) {
            if ((s32)tmp >= 0) {
                fn_800CE77C();
                return;
    }
    }
    }
    /* fabs */ f1 = (f0 < 0) ? -f0 : f0;
    do {
        if (r10 != 0) break;
        /* subis r8, r6, 0x7ff0 */;
        if (r8 != 0 && (s32)r6 != 0) {

            /* subis r8, r6, 0x3ff0 */;
            if (r8 != 0) break;
        }
        if ((s32)r5 < 0) {
            f0 = lbl_8047C678;
            f0 = f0 / f1;
        }
        if ((s32)tmp >= 0) return;
        /* subis tmp, r6, 0x3ff0 */;
        /* or. tmp, tmp, r4 */;
        if ((s32)tmp == 0) {
            f0 = f0 - f0;
            f0 = f0 / f0;
            return;
        }
        if ((s32)r4 != 1) return;
        f0 = -f0;

        return;
    } while (0);

    r8 = (s32)tmp >> 31;
    tmp = r8 + 0x1;
    /* or. r8, tmp, r4 */;
    if ((s32)r4 == 1) {
        r3 = (u32)lbl_80478AC0;
        tmp = 0x21;
        lbl_8047AA10 = tmp;
        f1 = *(f32*)lbl_80478AC0;
        return;
    }
    r8 = 0x41E00000;
    if ((s32)r7 > (s32)r8) {
        r3 = 0x43F00000;
        if ((s32)r7 > (s32)r3) {
            r3 = 0x3FF00000;
            if ((s32)r6 <= (s32)r7) {
                if ((s32)r5 < 0) {
                    f1 = lbl_8047C688;
                    return;
                }
                f1 = lbl_8047C680;
                return;
            }
            if ((s32)r6 >= (s32)r3) {
                if ((s32)r5 > 0) {
                    f1 = lbl_8047C688;
                    return;
                }
                f1 = lbl_8047C680;
                return;
        }
        }
        r3 = 0x3FF00000;
        if ((s32)r6 < (s32)r7) {
            if ((s32)r5 < 0) {
                f1 = lbl_8047C688;
                return;
            }
            f1 = lbl_8047C680;
            return;
        }
        if ((s32)r6 > (s32)r3) {
            if ((s32)r5 > 0) {
                f1 = lbl_8047C688;
                return;
            }
            f1 = lbl_8047C680;
            return;
        }
        r3 = 0x0;
        f0 = lbl_8047C678;
        f1 = lbl_8047C6A0;
        f6 = f2 - f0;
        f0 = lbl_8047C698;
        f2 = lbl_8047C6A8;
        f3 = lbl_8047C690;
        f4 = -(f1 * f6 - f0);
        f0 = lbl_8047C6B8;
        f1 = lbl_8047C6B0;
        f5 = f6 * f6;
        f3 = -(f6 * f4 - f3);
        f2 = f2 * f6;
        f3 = f5 * f3;
        f0 = f0 * f3;
        f1 = f1 * f6 - f0;
        f0 = f2 + f1;
        f0 = f0 - f2;
        f0 = f1 - f0;

    } else {
        r5 = 0x100000;
        r10 = 0x0;
        if ((s32)r6 < (s32)r5) {
            r10 = -0x35;
            f0 = lbl_8047C6C0;
            f0 = f1 * f0;
        }
        r5 = 0x40000;
        r8 = r6 & 0xFFFFF;
        r6 = (s32)r6 >> 20;
        r7 = r8 | (0x3ff0 << 16);
        r10 = r6 + r10;
        do {
            if ((s32)r8 <= (s32)r5) {
                r11 = 0x0;
                break;
            }
            r5 = 0xC0000;
            if ((s32)r8 < (s32)r5) {
                r11 = 0x1;
                break;
            }
            /* subis r7, r7, 0x10 */;
            r11 = 0x0;
            r10 = r10 + 0x1;
        } while (0);

        r5 = (s32)r7 >> 1;
        r9 = r11 << 3;
        r6 = r3 + 0x0;
        r8 = r5 | (0x2000 << 16);
        f5 = *(f64*)(r6 + r9);
        r7 = r3 + 0x20;
        f1 = lbl_8047C680;
        f0 = f30 + f5;
        f2 = lbl_8047C678;
        r5 = 0x43300000;
        r10 = r3 + 0x10;
        f31 = f30 - f5;
        f4 = lbl_8047C6F0;
        f28 = f2 / f0;
        f0 = lbl_8047C6E8;
        f3 = lbl_8047C6E0;
        r8 = r8 + (0x8 << 16);
        r3 = r11 << 18;
        f1 = f31 * f28;
        r3 = r8 + r3;
        r3 = 0x0;
        f2 = lbl_8047C6D8;
        f27 = f1 * f1;
        f11 = lbl_8047C6D0;
        f13 = f12 - f5;
        f9 = lbl_8047C6C8;
        f4 = f4 * f27 + f0;
        f10 = lbl_8047C6F8;
        f0 = f29 * f29;
        f5 = lbl_8047C710;
        f6 = lbl_8047C708;
        f3 = f27 * f4 + f3;
        f8 = lbl_8047C700;
        f7 = *(f64*)(r7 + r9);
        f31 = -(f29 * f12 - f31);
        f4 = lbl_8047C780;
        f12 = f27 * f3 + f2;
        f2 = *(f64*)(r10 + r9);
        f30 = f30 - f13;
        f13 = f27 * f27;
        f11 = f27 * f12 + f11;
        f12 = -(f29 * f30 - f31);
        f9 = f27 * f11 + f9;
        f27 = f28 * f12;
        f12 = f13 * f9;
        f11 = f29 + f1;
        f9 = f10 + f0;
        f3 = f3 - f4;
        f12 = f27 * f11 + f12;
        f4 = f9 + f12;
        f4 = f9 - f10;
        f10 = f29 * f9;
        f0 = f4 - f0;
        f0 = f12 - f0;
        f0 = f0 * f1;
        f4 = f27 * f9 + f0;
        f0 = f10 + f4;
        f0 = f1 - f10;
        f8 = f8 * f1;
        f0 = f4 - f0;
        f0 = f5 * f0;
        f0 = f6 * f1 + f0;
        f1 = f7 + f0;
        f0 = f8 + f1;
        f0 = f0 + f2;
        f0 = f3 + f0;
        f0 = f0 - f3;
        f0 = f0 - f2;
        f0 = f0 - f8;
        f0 = f1 - f0;
    }
    f31 = lbl_8047C678;
    /* or. tmp, tmp, r3 */;
    if ((s32)r8 == (s32)r5) {
        f31 = lbl_8047C718;
    }
    r3 = 0x0;
    tmp = 0x40900000;
    f0 = f1 * f0;
    f1 = f1 - f2;
    f2 = f2 * f3;
    f12 = f3 * f1 + f0;
    f0 = f12 + f2;
    if ((s32)r6 >= (s32)tmp) {
        /* subis tmp, r6, 0x4090 */;
        /* or. tmp, tmp, r5 */;
        if ((s32)r6 != (s32)tmp) {
            f1 = lbl_8047C720;
            f0 = f1 * f31;
            f1 = f1 * f0;
            return;
        }
        f1 = lbl_8047C728;
        f0 = f0 - f2;
        f1 = f1 + f12;
        if (f1 > f0) {
            f1 = lbl_8047C720;
            f0 = f1 * f31;
            f1 = f1 * f0;
            return;
        }
        r3 = 0x40910000;
        r4 = r6 & 0x7FFFFFFF;
        if ((s32)r4 >= (s32)tmp) {
            r3 = r6 + (0x3f6f << 16);
            tmp = r3 + 0x3400;
            /* or. tmp, tmp, r5 */;
            if ((s32)r4 != (s32)tmp) {
                f1 = lbl_8047C730;
                f0 = f1 * f31;
                f1 = f1 * f0;
                return;
            }
            f0 = f0 - f2;
            /* cror eq, lt, eq */;
        }
        if (f12 == f0) {
            f1 = lbl_8047C730;
            f0 = f1 * f31;
            f1 = f1 * f0;
            return;
        }
        }
    r3 = r6 & 0x7FFFFFFF;
    tmp = 0x3FE00000;
    /* extrwi r4, r6, 11, 1 */;
    r3 = 0x0;
    if ((s32)r3 > (s32)tmp) {
        r3 = 0x100000;
        tmp = (s32)r3 >> tmp;
        f0 = lbl_8047C680;
        r7 = r6 + tmp;
        tmp = r7 & 0x7FFFFFFF;
        r4 = (s32)tmp >> 20;
        tmp = r7 & 0xFFFFF;
        r4 = (s32)r3 >> r5;
        r3 = tmp | (0x10 << 16);
        r4 = r7 & ~r4;
        tmp = 0x14 - r5;
        r3 = (s32)r3 >> tmp;
        if ((s32)r6 < 0) {
            r3 = -r3;
        }
        f0 = f1 - f0;
    }
    tmp = 0x0;
    f1 = lbl_8047C748;
    r4 = r3 << 20;
    f0 = f12 + f2;
    f10 = lbl_8047C738;
    f9 = lbl_8047C740;
    f6 = lbl_8047C770;
    f5 = lbl_8047C768;
    *(u32*)(sp + 0x2C) = tmp;
    f0 = lbl_8047C760;
    f4 = lbl_8047C758;
    f8 = f11 - f2;
    f3 = lbl_8047C750;
    f7 = f1 * f11;
    f2 = lbl_8047C778;
    f1 = lbl_8047C678;
    f8 = f12 - f8;
    f10 = f10 * f11;
    f11 = f9 * f8 + f7;
    f9 = f10 + f11;
    f7 = f9 * f9;
    f8 = f9 - f10;
    f5 = f6 * f7 + f5;
    f6 = f11 - f8;
    f5 = f7 * f5 + f0;
    f0 = f9 * f6 + f6;
    f4 = f7 * f5 + f4;
    f3 = f7 * f4 + f3;
    f3 = f7 * f3;
    f4 = f9 - f3;
    f3 = f9 * f4;
    f2 = f4 - f2;
    f2 = f3 / f2;
    f0 = f2 - f0;
    f0 = f0 - f9;
    f1 = f1 - f0;
    tmp = tmp + r4;
    /* srawi. tmp, tmp, 20 */;
    if ((s32)r6 <= 0) {
        fn_800CDE88();
    } else {

        tmp = tmp + r4;
        *(u32*)(sp + 0x50) = tmp;
    }
    f1 = f31 * f0;

    return;
}

/* fn_800CC2C0 - 0x800CC2C0 | size: 0x3A0 */
void fn_800CC2C0(void) {
    extern u8 lbl_80270078[];
    extern u8 lbl_80270180[];
    extern f64 lbl_8047C788;
    extern f64 lbl_8047C790;
    extern f64 lbl_8047C798;
    extern f64 lbl_8047C7A0;
    extern f64 lbl_8047C7A8;
    extern f64 lbl_8047C7B0;
    extern f64 lbl_8047C7B8;
    extern f64 lbl_8047C7C0;
    extern f64 lbl_8047C7C8;
    extern f64 lbl_8047C7D0;
    extern f64 lbl_8047C7D8;
    extern void fn_800CC754();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f9 = 0.0f;

    r4 = 0x3FE90000;
    tmp = r4 + 0x21fb;
    r30 = r3;
    r6 = r31 & 0x7FFFFFFF;
    if ((s32)r6 <= (s32)tmp) {
        *(f64*)((u8*)r30 + 0x0) = f1;
        r3 = 0x0;
        f0 = lbl_8047C788;
        *(f64*)((u8*)r30 + 0x8) = f0;
        return;
    }
    r3 = 0x40030000;
    if ((s32)r6 < (s32)tmp) {
        if ((s32)r31 > 0) {
            f0 = lbl_8047C790;
            /* subis tmp, r6, 0x3ff9 */;
            f2 = f1 - f0;
            if (tmp != 0x21fb) {
                f1 = lbl_8047C798;
                f0 = f2 - f1;
                *(f64*)((u8*)r30 + 0x0) = f0;
                f0 = *(f64*)((u8*)r30 + 0x0);
                f0 = f2 - f0;
                f0 = f0 - f1;
                *(f64*)((u8*)r30 + 0x8) = f0;
            } else {

                f0 = lbl_8047C7A0;
                f1 = lbl_8047C7A8;
                f2 = f2 - f0;
                f0 = f2 - f1;
                *(f64*)((u8*)r30 + 0x0) = f0;
                f0 = *(f64*)((u8*)r30 + 0x0);
                f0 = f2 - f0;
                f0 = f0 - f1;
                *(f64*)((u8*)r30 + 0x8) = f0;
            }
            r3 = 0x1;
            return;
        }
        f0 = lbl_8047C790;
        /* subis tmp, r6, 0x3ff9 */;
        f2 = f0 + f1;
        if (tmp != 0x21fb) {
            f1 = lbl_8047C798;
            f0 = f1 + f2;
            *(f64*)((u8*)r30 + 0x0) = f0;
            f0 = *(f64*)((u8*)r30 + 0x0);
            f0 = f2 - f0;
            f0 = f1 + f0;
            *(f64*)((u8*)r30 + 0x8) = f0;
        } else {

            f0 = lbl_8047C7A0;
            f1 = lbl_8047C7A8;
            f2 = f2 + f0;
            f0 = f1 + f2;
            *(f64*)((u8*)r30 + 0x0) = f0;
            f0 = *(f64*)((u8*)r30 + 0x0);
            f0 = f2 - f0;
            f0 = f1 + f0;
            *(f64*)((u8*)r30 + 0x8) = f0;
        }
        r3 = -0x1;
        return;
    }
    r3 = 0x41390000;
    tmp = r3 + 0x21fb;
    if ((s32)r6 <= (s32)tmp) {
        /* fabs */ f4 = (f1 < 0) ? -f1 : f1;
        tmp = 0x43300000;
        f1 = lbl_8047C7B8;
        f0 = lbl_8047C7B0;
        *(u32*)(sp + 0x38) = tmp;
        f2 = f1 * f4 + f0;
        f3 = lbl_8047C7D8;
        f1 = lbl_8047C790;
        f0 = lbl_8047C798;
        f2 = (f64)(s32)f2;
        *(u32*)(sp + 0x3C) = tmp;
        f5 = f2 - f3;
        f4 = -(f1 * f5 - f4);
        f1 = f0 * f5;
        do {
            if ((s32)r3 >= 0x20) break;
            r4 = (u32)lbl_80270180;
            tmp = r3 << 2;
            r4 = (u32)lbl_80270180;
            r4 = r4 + tmp;
            tmp = *(u32*)((u8*)r4 + (-4));
            if ((s32)r6 == (s32)tmp) break;
            f0 = f4 - f1;
            *(f64*)((u8*)r30 + 0x0) = f0;
            goto L_800CC4FC;
        } while (0);

        f0 = f4 - f1;
        r4 = (s32)r6 >> 20;
        *(f64*)((u8*)r30 + 0x0) = f0;
        tmp = *(u32*)((u8*)r30 + 0x0);
        /* extrwi tmp, tmp, 11, 1 */;
        tmp = r4 - tmp;
        if ((s32)tmp > 0x10) {
            f0 = lbl_8047C7A0;
            f3 = f4;
            f1 = lbl_8047C7A8;
            f2 = f0 * f5;
            f4 = f4 - f2;
            f0 = f3 - f4;
            f0 = f0 - f2;
            f1 = f1 * f5 - f0;
            f0 = f4 - f1;
            *(f64*)((u8*)r30 + 0x0) = f0;
            tmp = *(u32*)((u8*)r30 + 0x0);
            /* extrwi tmp, tmp, 11, 1 */;
            tmp = r4 - tmp;
            if ((s32)tmp > 0x31) {
                f0 = lbl_8047C7C0;
                f2 = f4;
                f1 = lbl_8047C7C8;
                f3 = f0 * f5;
                f4 = f4 - f3;
                f0 = f2 - f4;
                f0 = f0 - f3;
                f1 = f1 * f5 - f0;
                f0 = f4 - f1;
                *(f64*)((u8*)r30 + 0x0) = f0;
            }
        }
    L_800CC4FC:
        f0 = *(f64*)((u8*)r30 + 0x0);
        f0 = f4 - f0;
        f0 = f0 - f1;
        *(f64*)((u8*)r30 + 0x8) = f0;
        if ((s32)r31 >= 0) return;
        f0 = *(f64*)((u8*)r30 + 0x0);
        r3 = -r3;
        f0 = -f0;
        *(f64*)((u8*)r30 + 0x0) = f0;
        f0 = *(f64*)((u8*)r30 + 0x8);
        f0 = -f0;
        *(f64*)((u8*)r30 + 0x8) = f0;
        return;

    }
    tmp = 0x7FF00000;
    if ((s32)r6 >= (s32)tmp) {
        f0 = f1 - f1;
        r3 = 0x0;
        *(f64*)((u8*)r30 + 0x8) = f0;
        *(f64*)((u8*)r30 + 0x0) = f0;
        return;
    }
    r3 = (s32)r6 >> 20;
    tmp = 0x43300000;
    r3 = r5 << 20;
    *(u32*)(sp + 0x30) = tmp;
    r3 = r6 - r3;
    f5 = lbl_8047C7D8;
    r4 = (u32)sp + 0x30;
    f4 = lbl_8047C7D0;
    r6 = 0x3;
    f1 = lbl_8047C788;
    *(u32*)(sp + 0x48) = tmp;
    f0 = (f64)(s32)f3;
    *(u32*)(sp + 0x34) = tmp;
    f2 = f0 - f5;
    f0 = f3 - f2;
    f3 = f4 * f0;
    f0 = (f64)(s32)f3;
    *(u32*)(sp + 0x4C) = tmp;
    f2 = f0 - f5;
    f0 = f3 - f2;
    f0 = f4 * f0;
    while (1) {
        f0 = *(f64*)((u8*)r4 + (-8));
        if (f1 != f0) break;

    }
    r3 = (u32)lbl_80270078;
    r4 = r30;
    r8 = (u32)lbl_80270078;
    r7 = 0x2;
    r3 = (u32)sp + 0x18;
    fn_800CC754();
    if ((s32)r31 >= 0) return;
    f0 = *(f64*)((u8*)r30 + 0x0);
    r3 = -r3;
    f0 = -f0;
    *(f64*)((u8*)r30 + 0x0) = f0;
    f0 = *(f64*)((u8*)r30 + 0x8);
    f0 = -f0;
    *(f64*)((u8*)r30 + 0x8) = f0;

    return;
}

/* fn_800CC660 - 0x800CC660 | size: 0xF4 */
void fn_800CC660(void) {
    extern f64 lbl_8047C7E0;
    extern f64 lbl_8047C7E8;
    extern f64 lbl_8047C7F0;
    extern f64 lbl_8047C7F8;
    extern f64 lbl_8047C800;
    extern f64 lbl_8047C808;
    extern f64 lbl_8047C810;
    extern f64 lbl_8047C818;
    extern f64 lbl_8047C820;
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    tmp = 0x3E400000;
    r4 = r3 & 0x7FFFFFFF;
    if ((s32)r4 < (s32)tmp) {
        f0 = (f64)(s32)f1;
        if ((s32)tmp == 0) {
            f1 = lbl_8047C7E0;
            return;
    }
    }
    r3 = 0x3FD30000;
    tmp = r3 + 0x3333;
    f5 = lbl_8047C810;
    f7 = f6 * f6;
    f0 = lbl_8047C808;
    f4 = lbl_8047C800;
    f3 = lbl_8047C7F8;
    f1 = lbl_8047C7F0;
    f5 = f5 * f7 + f0;
    f0 = lbl_8047C7E8;
    f4 = f7 * f5 + f4;
    f3 = f7 * f4 + f3;
    f1 = f7 * f3 + f1;
    f0 = f7 * f1 + f0;
    f4 = f7 * f0;
    if ((s32)r4 < (s32)tmp) {
        f0 = f6 * f2;
        f1 = lbl_8047C818;
        f2 = lbl_8047C7E0;
        f0 = f7 * f4 - f0;
        f0 = f1 * f7 - f0;
        f1 = f2 - f0;
        return;
    }
    tmp = 0x3FE90000;
    if ((s32)r4 > (s32)tmp) {
        f0 = lbl_8047C820;
    } else {

        /* subis r3, r4, 0x20 */;
        tmp = 0x0;
        *(u32*)(sp + 0x14) = tmp;
    }
    f0 = f0 * f2;
    f1 = lbl_8047C818;
    f2 = lbl_8047C7E0;
    f1 = f1 * f7 - f3;
    f0 = f7 * f4 - f0;
    f2 = f2 - f3;
    f0 = f1 - f0;
    f1 = f2 - f0;

    return;
}

/* fn_800CC754 - 0x800CC754 | size: 0xE54 */
void fn_800CC754(void) {
    extern u8 lbl_80270200[];
    extern u8 lbl_80270210[];
    extern f64 lbl_8047C828;
    extern f64 lbl_8047C830;
    extern f64 lbl_8047C838;
    extern f64 lbl_8047C840;
    extern f64 lbl_8047C848;
    extern f64 lbl_8047C850;
    extern f64 lbl_8047C858;
    extern f64 lbl_8047C860;
    extern void fn_800CDCB4();
    extern void fn_800CDE88();
    u8 sp[0x2D0];
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
    f32 f6 = 0.0f;
    f32 f8 = 0.0f;
    f32 f25 = 0.0f;
    f32 f26 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r11 = (u32)sp + 0x2d0;
    r9 = 0x2AAB0000;
    r23 = r7;
    r10 = (u32)lbl_80270200;
    r9 = r23 << 2;
    tmp = (s32)((s64)r7 * (s64)tmp >> 32);
    r7 = (u32)lbl_80270200;
    r28 = *(u32*)(r7 + r9);
    r21 = r3;
    r22 = r4;
    r24 = r8;
    tmp = (s32)tmp >> 2;
    r3 = (u32)tmp >> 31;
    /* add. r29, tmp, r3 */;
    if ((s32)tmp < 0) {
        r29 = 0x0;
    }
    tmp = r29 + 0x1;
    /* add. r7, r30, r28 */;
    r4 = tmp * 0x18;
    r6 = r29 - r30;
    f1 = lbl_8047C860;
    r3 = r6 << 2;
    r26 = r5 - r4;
    tmp = r7 + 0x1;
    r4 = r24 + r3;
    r5 = (u32)sp + 0x198;
    r3 = 0x43300000;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)tmp >= 0) {
        do {
            if ((s32)r6 < 0) {
                f0 = lbl_8047C828;
            } else {

                tmp = *(u32*)((u8*)r4 + 0x0);
                *(u32*)(sp + 0x23C) = tmp;
                f0 = f0 - f1;
            }
            *(f64*)((u8*)r5 + 0x0) = f0;
            r5 = r5 + 0x8;
            r4 = r4 + 0x4;
            r6 = r6 + 0x1;
        } while (--ctr != 0);
    }
    r5 = (u32)sp + 0x58;
    r7 = 0x0;
    while ((s32)r7 <= (s32)r28) {

        f4 = lbl_8047C828;
        r6 = 0x0;
        if ((s32)r30 >= 0) {
            tmp = r30 + 0x1;
            if ((s32)tmp > 8) {
                r8 = r9 + 0x8;
                r4 = r21;
                r8 = (u32)r8 >> 3;
                tmp = r30 + r7;
                r3 = (u32)sp + 0x198;
                ctr_fn = (void(*)(void))r8;
                if ((s32)r9 >= 0) {
                    do {
                        r8 = tmp - r6;
                        r9 = r6 + 0x1;
                        r10 = r8 << 3;
                        f1 = *(f64*)((u8*)r4 + 0x0);
                        f0 = *(f64*)(r3 + r10);
                        r9 = tmp - r9;
                        r10 = r9 << 3;
                        r8 = r6 + 0x2;
                        f4 = f1 * f0 + f4;
                        r9 = tmp - r8;
                        r8 = r6 + 0x3;
                        f1 = *(f64*)((u8*)r4 + 0x8);
                        f0 = *(f64*)(r3 + r10);
                        r8 = tmp - r8;
                        f4 = f1 * f0 + f4;
                        r9 = r9 << 3;
                        f1 = *(f64*)((u8*)r4 + 0x10);
                        r10 = r8 << 3;
                        f0 = *(f64*)(r3 + r9);
                        r8 = r6 + 0x4;
                        f4 = f1 * f0 + f4;
                        f1 = *(f64*)((u8*)r4 + 0x18);
                        f0 = *(f64*)(r3 + r10);
                        r9 = tmp - r8;
                        r10 = r9 << 3;
                        r8 = r6 + 0x5;
                        f4 = f1 * f0 + f4;
                        f2 = *(f64*)((u8*)r4 + 0x20);
                        f0 = *(f64*)(r3 + r10);
                        r8 = tmp - r8;
                        r10 = r8 << 3;
                        r9 = r6 + 0x6;
                        f4 = f2 * f0 + f4;
                        r9 = tmp - r9;
                        r9 = r9 << 3;
                        f1 = *(f64*)((u8*)r4 + 0x28);
                        f0 = *(f64*)(r3 + r10);
                        r8 = r6 + 0x7;
                        f4 = f1 * f0 + f4;
                        r8 = tmp - r8;
                        r8 = r8 << 3;
                        f3 = *(f64*)((u8*)r4 + 0x30);
                        f2 = *(f64*)(r3 + r9);
                        r6 = r6 + 0x8;
                        f1 = *(f64*)((u8*)r4 + 0x38);
                        f4 = f3 * f2 + f4;
                        f0 = *(f64*)(r3 + r8);
                        r4 = r4 + 0x40;
                        f4 = f1 * f0 + f4;
                    } while (--ctr != 0);
            }
            }
            tmp = r30 + 0x1;
            r3 = r6 << 3;
            tmp = tmp - r6;
            r8 = r30 + r7;
            r4 = r21 + r3;
            r3 = (u32)sp + 0x198;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r6 <= (s32)r30) {
                do {
                    tmp = r8 - r6;
                    f1 = *(f64*)((u8*)r4 + 0x0);
                    tmp = tmp << 3;
                    r4 = r4 + 0x8;
                    f0 = *(f64*)(r3 + tmp);
                    r6 = r6 + 0x1;
                    f4 = f1 * f0 + f4;
                } while (--ctr != 0);
        }
        }
        *(f64*)((u8*)r5 + 0x0) = f4;
        r5 = r5 + 0x8;
        r7 = r7 + 0x1;

    }
    r18 = 0x18 - r26;
    f26 = lbl_8047C830;
    f27 = lbl_8047C860;
    r16 = (u32)sp + 0x8;
    f28 = lbl_8047C838;
    r31 = r28;
    f29 = lbl_8047C848;
    r17 = 0x17 - r26;
    f30 = lbl_8047C840;
    r20 = (u32)sp + 0x198;
    f31 = lbl_8047C828;
    r19 = 0x43300000;
    do {
        tmp = r31 << 3;
        r5 = (u32)sp + 0x58;
        r5 = r5 + tmp;
        f1 = *(f64*)((u8*)r5 + 0x0);
        r4 = r16;
        r3 = r31;
        do {
            if ((s32)r31 <= 0) break;
            /* srwi. tmp, r31, 1 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r31 != 0) {
                do {
                    f3 = f26 * f1;
                    f0 = *(f64*)((u8*)r5 + (-8));
                    f2 = (f64)(s32)f3;
                    *(u32*)(sp + 0x244) = tmp;
                    f3 = f2 - f27;
                    f2 = -(f28 * f3 - f1);
                    f1 = f3 + f0;
                    /* lfdu f0, -0x10(r5) */;
                    f2 = (f64)(s32)f2;
                    f3 = f26 * f1;
                    f2 = (f64)(s32)f3;
                    *(u32*)((u8*)r4 + 0x0) = tmp;
                    *(u32*)(sp + 0x244) = tmp;
                    f3 = f2 - f27;
                    f2 = -(f28 * f3 - f1);
                    f1 = f3 + f0;
                    f2 = (f64)(s32)f2;
                    *(u32*)((u8*)r4 + 0x4) = tmp;
                    r4 = r4 + 0x8;
                } while (--ctr != 0);
                r3 = r3 & 0x1;
                if ((s32)r31 == 0) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                f3 = f26 * f1;
                /* lfdu f0, -0x8(r5) */;
                f2 = (f64)(s32)f3;
                *(u32*)(sp + 0x244) = tmp;
                f3 = f2 - f27;
                f2 = -(f28 * f3 - f1);
                f1 = f3 + f0;
                f2 = (f64)(s32)f2;
                *(u32*)((u8*)r4 + 0x0) = tmp;
                r4 = r4 + 0x4;
            } while (--ctr != 0);
        } while (0);

        r3 = r26;
        fn_800CDE88();
        f25 = f1;
        f1 = f29 * f25;
        fn_800CDCB4();
        f25 = -(f30 * f1 - f25);
        r25 = 0x0;
        f0 = (f64)(s32)f25;
        *(u32*)(sp + 0x244) = tmp;
        f0 = f0 - f27;
        f25 = f25 - f0;
        if ((s32)r26 > 0) {
            r3 = r31 << 2;
            r4 = (u32)sp + 0x8;
            r3 = *(u32*)(r4 + r5);
            r6 = (s32)r3 >> r18;
            tmp = r6 << r18;
            tmp = r3 - tmp;
            r27 = r27 + r6;
            *(u32*)(r4 + r5) = tmp;
            tmp = *(u32*)(r4 + r5);
            r25 = (s32)tmp >> r17;

        } else if ((s32)r26 == 0) {
            r4 = r31 << 2;
            r3 = (u32)sp + 0x8;
            tmp = *(u32*)(r3 + tmp);
            r25 = (s32)tmp >> 23;

        } else {
            f0 = lbl_8047C850;
            /* cror eq, gt, eq */;
            if (f25 == f0) {
                r25 = 0x2;
            }
            }
        do {
        do {
            if ((s32)r25 <= 0) break;
            r5 = 0x1000000;
            r6 = r16;
            tmp = 0x0;
            ctr_fn = (void(*)(void))r31;
            r27 = r27 + 0x1;
            if ((s32)r31 > 0) {
                do {
                    r3 = *(u32*)((u8*)r6 + 0x0);
                    if ((s32)tmp == 0) {
                        if ((s32)r3 != 0) {
                            r3 = r5 - r3;
                            tmp = 0x1;
                            *(u32*)((u8*)r6 + 0x0) = r3;
                        }

                    } else {
                        r3 = r4 - r3;
                        *(u32*)((u8*)r6 + 0x0) = r3;
                    }
                    r6 = r6 + 0x4;
                } while (--ctr != 0);
            }
            if ((s32)r26 > 0) {
                if ((s32)r26 != 2) {
                    if ((s32)r26 >= 2) break;
                    if ((s32)r26 < 1) {
                        break;
                    }
                    r3 = r31 << 2;
                    r5 = (u32)sp + 0x8;
                    r3 = *(u32*)(r5 + r4);
                    r3 = r3 & 0x7FFFFF;
                    *(u32*)(r5 + r4) = r3;
                    break;
                }
                r3 = r31 << 2;
                r5 = (u32)sp + 0x8;
                r3 = *(u32*)(r5 + r4);
                r3 = r3 & 0x3FFFFF;
                *(u32*)(r5 + r4) = r3;
            }
        } while (0);
            if ((s32)r25 != 2) break;
            f1 = lbl_8047C858;
            f25 = f1 - f25;
            if ((s32)tmp == 0) break;
            r3 = r26;
            fn_800CDE88();
            f25 = f25 - f1;
        } while (0);

        if (f31 != f25) break;
        r4 = (u32)sp + 0x8;
        r5 = 0x0;
        tmp = r3 << 2;
        r3 = r3 + 0x1;
        r4 = r4 + tmp;
        r3 = r3 - r28;
        do {
            if ((s32)r3 < (s32)r28) break;
            /* srwi. tmp, r3, 3 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r3 != (s32)r28) {
                do {
                    tmp = *(u32*)((u8*)r4 + 0x0);
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-4));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-8));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-12));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-16));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-20));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-24));
                    r5 = r5 | tmp;
                    tmp = *(u32*)((u8*)r4 + (-28));
                    r5 = r5 | tmp;
                } while (--ctr != 0);
                r3 = r3 & 0x7;
                if ((s32)r3 == (s32)r28) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                tmp = *(u32*)((u8*)r4 + 0x0);
                r5 = r5 | tmp;
            } while (--ctr != 0);
        } while (0);

        if ((s32)r5 != 0) break;
        r10 = 0x1;
        while (1) {
            tmp = r28 - r10;
            tmp = tmp << 2;
            tmp = *(u32*)(r16 + tmp);
            if ((s32)tmp != 0) break;
            r10 = r10 + 0x1;

        }
        r9 = r31 + 0x1;
        r5 = (u32)sp + 0x58;
        tmp = r9 << 3;
        r6 = r31 + r10;
        r5 = r5 + tmp;
        while ((s32)r9 <= (s32)r6) {

            tmp = r29 + r9;
            r7 = r30 + r9;
            tmp = tmp << 2;
            r3 = *(u32*)(r24 + tmp);
            tmp = r7 << 3;
            f4 = lbl_8047C828;
            r8 = 0x0;
            f0 = f0 - f27;
            *(f64*)(r20 + tmp) = f0;
            if ((s32)r30 >= 0) {
                tmp = r30 + 0x1;
                if ((s32)tmp > 8) {
                    tmp = r11 + 0x8;
                    r4 = r21;
                    tmp = (u32)tmp >> 3;
                    r3 = (u32)sp + 0x198;
                    ctr_fn = (void(*)(void))tmp;
                    if ((s32)r11 >= 0) {
                        do {
                            r11 = r7 - r8;
                            tmp = r8 + 0x1;
                            r12 = r11 << 3;
                            f1 = *(f64*)((u8*)r4 + 0x0);
                            f0 = *(f64*)(r3 + r12);
                            tmp = r7 - tmp;
                            r11 = r8 + 0x2;
                            f2 = *(f64*)((u8*)r4 + 0x20);
                            f4 = f1 * f0 + f4;
                            r12 = r7 - r11;
                            tmp = tmp << 3;
                            r11 = r8 + 0x3;
                            f0 = *(f64*)(r3 + tmp);
                            r25 = r12 << 3;
                            f1 = *(f64*)((u8*)r4 + 0x8);
                            r11 = r7 - r11;
                            r12 = r11 << 3;
                            tmp = r8 + 0x4;
                            f4 = f1 * f0 + f4;
                            f1 = *(f64*)((u8*)r4 + 0x10);
                            f0 = *(f64*)(r3 + r25);
                            r11 = r7 - tmp;
                            tmp = r8 + 0x5;
                            f3 = *(f64*)((u8*)r4 + 0x30);
                            f4 = f1 * f0 + f4;
                            f0 = *(f64*)(r3 + r12);
                            f1 = *(f64*)((u8*)r4 + 0x18);
                            r12 = r11 << 3;
                            r11 = r8 + 0x6;
                            tmp = r7 - tmp;
                            f4 = f1 * f0 + f4;
                            f0 = *(f64*)(r3 + r12);
                            r12 = tmp << 3;
                            f1 = *(f64*)((u8*)r4 + 0x28);
                            tmp = r8 + 0x7;
                            r11 = r7 - r11;
                            f4 = f2 * f0 + f4;
                            f0 = *(f64*)(r3 + r12);
                            r11 = r11 << 3;
                            tmp = r7 - tmp;
                            f2 = *(f64*)(r3 + r11);
                            tmp = tmp << 3;
                            f4 = f1 * f0 + f4;
                            f1 = *(f64*)((u8*)r4 + 0x38);
                            f0 = *(f64*)(r3 + tmp);
                            r4 = r4 + 0x40;
                            r8 = r8 + 0x8;
                            f4 = f3 * f2 + f4;
                            f4 = f1 * f0 + f4;
                        } while (--ctr != 0);
                }
                }
                tmp = r30 + 0x1;
                r4 = r8 << 3;
                tmp = tmp - r8;
                r3 = (u32)sp + 0x198;
                r4 = r21 + r4;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r8 <= (s32)r30) {
                    do {
                        tmp = r7 - r8;
                        f1 = *(f64*)((u8*)r4 + 0x0);
                        tmp = tmp << 3;
                        r4 = r4 + 0x8;
                        f0 = *(f64*)(r3 + tmp);
                        r8 = r8 + 0x1;
                        f4 = f1 * f0 + f4;
                    } while (--ctr != 0);
            }
            }
            *(f64*)((u8*)r5 + 0x0) = f4;
            r5 = r5 + 0x8;
            r9 = r9 + 0x1;

        }
        r31 = r31 + r10;
    } while (1);

    f0 = lbl_8047C828;
    do {
        if (f0 == f25) {
            r3 = (u32)sp + 0x8;
            tmp = r31 << 2;
            r3 = r3 + tmp;
            while (1) {
                tmp = *(u32*)((u8*)r3 + 0x0);
                if ((s32)tmp != 0) break;

            }
            break;
        }
        f1 = f25;
        r3 = -r26;
        fn_800CDE88();
        f3 = lbl_8047C838;
        /* cror eq, gt, eq */;
        if (f1 == f3) {
            f0 = lbl_8047C830;
            tmp = 0x43300000;
            r5 = r31 << 2;
            *(u32*)(sp + 0x240) = tmp;
            f0 = f0 * f1;
            r31 = r31 + 0x1;
            f2 = lbl_8047C860;
            r4 = (u32)sp + 0x8;
            tmp = r31 << 2;
            r26 = r26 + 0x18;
            f0 = (f64)(s32)f0;
            f0 = f0 - f2;
            f1 = -(f3 * f0 - f1);
            f0 = (f64)(s32)f0;
            f1 = (f64)(s32)f1;
            *(u32*)(r4 + r5) = r6;
            *(u32*)(r4 + tmp) = r3;
            break;
        }
        f0 = (f64)(s32)f1;
        tmp = r31 << 2;
        r3 = (u32)sp + 0x8;
        *(u32*)(r3 + tmp) = r4;
    } while (0);

    f1 = lbl_8047C858;
    r3 = r26;
    fn_800CDE88();
    tmp = r31 << 2;
    r6 = (u32)sp + 0x8;
    r8 = r31 << 3;
    r7 = (u32)sp + 0x58;
    f5 = lbl_8047C860;
    f0 = lbl_8047C830;
    r6 = r6 + tmp;
    r7 = r7 + r8;
    r3 = r31 + 0x1;
    r4 = 0x43300000;
    do {
        if ((s32)r31 < 0) break;
        /* srwi. tmp, r3, 2 */;
        ctr_fn = (void(*)(void))tmp;
        if ((s32)r31 != 0) {
            do {
                r5 = *(u32*)((u8*)r6 + 0x0);
                r5 = *(u32*)((u8*)r6 + (-4));
                *(u32*)(sp + 0x254) = tmp;
                r5 = *(u32*)((u8*)r6 + (-8));
                *(u32*)(sp + 0x254) = tmp;
                f3 = f4 - f5;
                r5 = *(u32*)((u8*)r6 + (-12));
                f2 = f1 * f3;
                f1 = f1 * f0;
                *(u32*)(sp + 0x254) = tmp;
                f3 = f4 - f5;
                *(f64*)((u8*)r7 + 0x0) = f2;
                f2 = f1 * f3;
                f1 = f1 * f0;
                *(u32*)(sp + 0x254) = tmp;
                f3 = f4 - f5;
                *(f64*)((u8*)r7 + (-8)) = f2;
                f2 = f1 * f3;
                f1 = f1 * f0;
                f3 = f4 - f5;
                *(f64*)((u8*)r7 + (-16)) = f2;
                f2 = f1 * f3;
                f1 = f1 * f0;
                *(f64*)((u8*)r7 + (-24)) = f2;
            } while (--ctr != 0);
            r3 = r3 & 0x3;
            if ((s32)r31 == 0) break;
        }
        ctr_fn = (void(*)(void))r3;
        do {
            r5 = *(u32*)((u8*)r6 + 0x0);
            *(u32*)(sp + 0x254) = tmp;
            f3 = f4 - f5;
            f2 = f1 * f3;
            f1 = f1 * f0;
            *(f64*)((u8*)r7 + 0x0) = f2;
        } while (--ctr != 0);
    } while (0);

    r3 = (u32)lbl_80270210;
    tmp = r31 + 0x1;
    r9 = r31;
    r4 = (u32)sp + 0x58;
    r5 = (u32)lbl_80270210;
    r3 = (u32)sp + 0xf8;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)r31 >= 0) {
        do {
            f2 = lbl_8047C828;
            r6 = r5;
            r7 = r31 - r9;
            r10 = 0x0;
            while ((s32)r10 <= (s32)r28 && (s32)r10 <= (s32)r7) {

                tmp = r9 + r10;
                f1 = *(f64*)((u8*)r6 + 0x0);
                tmp = tmp << 3;
                r6 = r6 + 0x8;
                f0 = *(f64*)(r4 + tmp);
                r10 = r10 + 0x1;
                f2 = f1 * f0 + f2;

            }

            tmp = r7 << 3;
            *(f64*)(r3 + tmp) = f2;
        } while (--ctr != 0);
    }
    if ((s32)r23 != 3) {
        if ((s32)r23 >= 3) goto L_800CD588;
        if ((s32)r23 != 0) {
            if ((s32)r23 < 0) {
                goto L_800CD588;
            }
            r4 = (u32)sp + 0xf8;
            f1 = lbl_8047C828;
            r4 = r4 + r8;
            r3 = r31 + 0x1;
            do {
                if ((s32)r31 < 0) break;
                /* srwi. tmp, r3, 3 */;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r31 != 0) {
                    do {
                        f0 = *(f64*)((u8*)r4 + 0x0);
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-8));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-16));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-24));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-32));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-40));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-48));
                        f1 = f1 + f0;
                        f0 = *(f64*)((u8*)r4 + (-56));
                        f1 = f1 + f0;
                    } while (--ctr != 0);
                    r3 = r3 & 0x7;
                    if ((s32)r31 == 0) break;
                }
                ctr_fn = (void(*)(void))r3;
                do {
                    f0 = *(f64*)((u8*)r4 + 0x0);
                    f1 = f1 + f0;
                } while (--ctr != 0);
            } while (0);

            if ((s32)r25 == 0) {
            } else {

                f1 = -f1;
            }
            *(f64*)((u8*)r22 + 0x0) = f1;
            goto L_800CD588;
            }
        r4 = (u32)sp + 0xf8;
        f2 = lbl_8047C828;
        r4 = r4 + r8;
        r3 = r31 + 0x1;
        do {
            if ((s32)r31 < 0) break;
            /* srwi. tmp, r3, 3 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r31 != 0) {
                do {
                    f0 = *(f64*)((u8*)r4 + 0x0);
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-8));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-16));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-24));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-32));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-40));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-48));
                    f2 = f2 + f0;
                    f0 = *(f64*)((u8*)r4 + (-56));
                    f2 = f2 + f0;
                } while (--ctr != 0);
                r3 = r3 & 0x7;
                if ((s32)r31 == 0) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                f0 = *(f64*)((u8*)r4 + 0x0);
                f2 = f2 + f0;
            } while (--ctr != 0);
        } while (0);

        if ((s32)r25 == 0) {
            f1 = f2;
        } else {

            f1 = -f2;
        }
        *(f64*)((u8*)r22 + 0x0) = f1;
        r5 = 0x1;
        f6 = f0 - f2;
        if ((s32)r31 >= 1) {
            if ((s32)r31 > 8) {
                tmp = r3 + 0x7;
                r4 = (u32)sp + 0x100;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r3 >= 1) {
                    do {
                        f1 = *(f64*)((u8*)r4 + 0x0);
                        r5 = r5 + 0x8;
                        f0 = *(f64*)((u8*)r4 + 0x8);
                        f6 = f6 + f1;
                        f5 = *(f64*)((u8*)r4 + 0x10);
                        f4 = *(f64*)((u8*)r4 + 0x18);
                        f3 = *(f64*)((u8*)r4 + 0x20);
                        f6 = f6 + f0;
                        f2 = *(f64*)((u8*)r4 + 0x28);
                        f1 = *(f64*)((u8*)r4 + 0x30);
                        f0 = *(f64*)((u8*)r4 + 0x38);
                        r4 = r4 + 0x40;
                        f6 = f6 + f5;
                        f6 = f6 + f4;
                        f6 = f6 + f3;
                        f6 = f6 + f2;
                        f6 = f6 + f1;
                        f6 = f6 + f0;
                    } while (--ctr != 0);
            }
            }
            tmp = r31 + 0x1;
            r3 = r5 << 3;
            r4 = (u32)sp + 0xf8;
            tmp = tmp - r5;
            r4 = r4 + r3;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r5 <= (s32)r31) {
                do {
                    f0 = *(f64*)((u8*)r4 + 0x0);
                    r4 = r4 + 0x8;
                    f6 = f6 + f0;
                } while (--ctr != 0);
        }
        }
        if ((s32)r25 == 0) {
        } else {

            f6 = -f6;
        }
        *(f64*)((u8*)r22 + 0x8) = f6;

    } else {
        r5 = (u32)sp + 0xf8;
        r5 = r5 + r8;
        r3 = r31;
        r4 = r5;
        do {
            if ((s32)r31 <= 0) break;
            /* srwi. tmp, r31, 2 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r31 != 0) {
                do {
                    f0 = *(f64*)((u8*)r4 + (-8));
                    f1 = *(f64*)((u8*)r4 + 0x0);
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + 0x0) = f0;
                    *(f64*)((u8*)r4 + (-8)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-16));
                    f1 = *(f64*)((u8*)r4 + (-8));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-8)) = f0;
                    *(f64*)((u8*)r4 + (-16)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-24));
                    f1 = *(f64*)((u8*)r4 + (-16));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-16)) = f0;
                    *(f64*)((u8*)r4 + (-24)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-32));
                    f1 = *(f64*)((u8*)r4 + (-24));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-24)) = f0;
                    /* stfdu f2, -0x20(r4) */;
                } while (--ctr != 0);
                r3 = r3 & 0x3;
                if ((s32)r31 == 0) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                f0 = *(f64*)((u8*)r4 + (-8));
                f1 = *(f64*)((u8*)r4 + 0x0);
                f2 = f0 + f1;
                f0 = f0 - f2;
                f0 = f1 + f0;
                *(f64*)((u8*)r4 + 0x0) = f0;
                /* stfdu f2, -0x8(r4) */;
            } while (--ctr != 0);
        } while (0);

        r4 = r5;
        do {
            if ((s32)r31 <= 1) break;
            /* srwi. tmp, r3, 2 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r31 != 1) {
                do {
                    f0 = *(f64*)((u8*)r4 + (-8));
                    f1 = *(f64*)((u8*)r4 + 0x0);
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + 0x0) = f0;
                    *(f64*)((u8*)r4 + (-8)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-16));
                    f1 = *(f64*)((u8*)r4 + (-8));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-8)) = f0;
                    *(f64*)((u8*)r4 + (-16)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-24));
                    f1 = *(f64*)((u8*)r4 + (-16));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-16)) = f0;
                    *(f64*)((u8*)r4 + (-24)) = f2;
                    f0 = *(f64*)((u8*)r4 + (-32));
                    f1 = *(f64*)((u8*)r4 + (-24));
                    f2 = f0 + f1;
                    f0 = f0 - f2;
                    f0 = f1 + f0;
                    *(f64*)((u8*)r4 + (-24)) = f0;
                    /* stfdu f2, -0x20(r4) */;
                } while (--ctr != 0);
                r3 = r3 & 0x3;
                if ((s32)r31 == 1) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                f0 = *(f64*)((u8*)r4 + (-8));
                f1 = *(f64*)((u8*)r4 + 0x0);
                f2 = f0 + f1;
                f0 = f0 - f2;
                f0 = f1 + f0;
                *(f64*)((u8*)r4 + 0x0) = f0;
                /* stfdu f2, -0x8(r4) */;
            } while (--ctr != 0);
        } while (0);

        f3 = lbl_8047C828;
        do {
            if ((s32)r31 < 2) break;
            /* srwi. tmp, r3, 3 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r31 != 2) {
                do {
                    f0 = *(f64*)((u8*)r5 + 0x0);
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-8));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-16));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-24));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-32));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-40));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-48));
                    f3 = f3 + f0;
                    f0 = *(f64*)((u8*)r5 + (-56));
                    f3 = f3 + f0;
                } while (--ctr != 0);
                r3 = r3 & 0x7;
                if ((s32)r31 == 2) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                f0 = *(f64*)((u8*)r5 + 0x0);
                f3 = f3 + f0;
            } while (--ctr != 0);
        } while (0);

        if ((s32)r25 == 0) {
            *(f64*)((u8*)r22 + 0x0) = f1;
            *(f64*)((u8*)r22 + 0x8) = f0;
            *(f64*)((u8*)r22 + 0x10) = f3;

        } else {
            f0 = -f3;
            f2 = -f2;
            f1 = -f1;
            *(f64*)((u8*)r22 + 0x0) = f2;
            *(f64*)((u8*)r22 + 0x8) = f1;
            *(f64*)((u8*)r22 + 0x10) = f0;
        }
    }
L_800CD588:
    r3 = r27 & 0x7;
    r11 = (u32)sp + 0x2d0;
    return;
}

/* fn_800CD5A8 - 0x800CD5A8 | size: 0xA0 */
void fn_800CD5A8(void) {
    extern f64 lbl_8047C868;
    extern f64 lbl_8047C870;
    extern f64 lbl_8047C878;
    extern f64 lbl_8047C880;
    extern f64 lbl_8047C888;
    extern f64 lbl_8047C890;
    extern f64 lbl_8047C898;
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    tmp = 0x3E400000;
    r4 = r4 & 0x7FFFFFFF;
    if ((s32)r4 < (s32)tmp) {
        f0 = (f64)(s32)f1;
        if ((s32)tmp == 0) {
            return;
    }
    }
    f5 = lbl_8047C888;
    f7 = f6 * f6;
    f4 = lbl_8047C880;
    f3 = lbl_8047C878;
    f1 = lbl_8047C870;
    f0 = lbl_8047C868;
    f4 = f5 * f7 + f4;
    f5 = f7 * f6;
    f3 = f7 * f4 + f3;
    f1 = f7 * f3 + f1;
    f1 = f7 * f1 + f0;
    if ((s32)r3 == 0) {
        f0 = lbl_8047C890;
        f0 = f7 * f1 + f0;
        f1 = f5 * f0 + f6;
        return;
    }
    f0 = f5 * f1;
    f1 = lbl_8047C898;
    f3 = lbl_8047C890;
    f0 = f1 * f2 - f0;
    f0 = f7 * f0 - f2;
    f0 = -(f3 * f5 - f0);
    f1 = f6 - f0;

    return;
}

/* fn_800CD648 - 0x800CD648 | size: 0x214 */
void fn_800CD648(void) {
    extern u8 lbl_80270250[];
    extern f64 lbl_8047C8A0;
    extern f64 lbl_8047C8A8;
    extern f64 lbl_8047C8B0;
    extern f64 lbl_8047C8B8;
    extern f64 lbl_8047C8C0;
    extern f64 lbl_8047C8C8;
    extern f64 lbl_8047C8D0;
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;
    f32 f13 = 0.0f;
    f32 f31 = 0.0f;

    tmp = 0x3E300000;
    r6 = r7 & 0x7FFFFFFF;
    if ((s32)r6 < (s32)tmp) {
        f0 = (f64)(s32)f1;
        if ((s32)tmp == 0) {
            r4 = r3 + 0x1;
            tmp = r6 | tmp;
            /* or. tmp, r4, tmp */;
            if ((s32)tmp == 0) {
                /* fabs */ f1 = (f1 < 0) ? -f1 : f1;
                f0 = lbl_8047C8A0;
                f1 = f0 / f1;
                return;
            }
            if ((s32)r3 == 1) {
                return;
            }
            f0 = lbl_8047C8A8;
            f1 = f0 / f1;
            return;
    }
    }
    r4 = 0x3FE60000;
    if ((s32)r6 >= (s32)tmp) {
        if ((s32)r7 < 0) {
            f2 = -f2;
            f0 = -f0;
        }
        f0 = lbl_8047C8B8;
        f3 = lbl_8047C8B0;
        f0 = f0 - f2;
        f2 = lbl_8047C8C0;
        f1 = f3 - f1;
        f0 = f1 + f0;
    }
    r4 = (u32)lbl_80270250;
    r5 = (u32)lbl_80270250;
    r4 = 0x3FE60000;
    f13 = f0 * f0;
    f5 = *(f64*)((u8*)r5 + 0x60);
    f4 = *(f64*)((u8*)r5 + 0x50);
    f9 = *(f64*)((u8*)r5 + 0x58);
    f31 = f13 * f13;
    f8 = *(f64*)((u8*)r5 + 0x48);
    f3 = *(f64*)((u8*)r5 + 0x40);
    f11 = *(f64*)((u8*)r5 + 0x38);
    f1 = f13 * f0;
    f6 = *(f64*)((u8*)r5 + 0x30);
    f7 = f31 * f5 + f4;
    f10 = *(f64*)((u8*)r5 + 0x28);
    f5 = *(f64*)((u8*)r5 + 0x20);
    f12 = f31 * f9 + f8;
    f9 = *(f64*)((u8*)r5 + 0x18);
    f4 = *(f64*)((u8*)r5 + 0x10);
    f7 = f31 * f7 + f3;
    f8 = *(f64*)((u8*)r5 + 0x8);
    f3 = *(f64*)((u8*)r5 + 0x0);
    f11 = f31 * f12 + f11;
    f6 = f31 * f7 + f6;
    f7 = f31 * f11 + f10;
    f5 = f31 * f6 + f5;
    f6 = f31 * f7 + f9;
    f4 = f31 * f5 + f4;
    f5 = f31 * f6 + f8;
    f4 = f13 * f4;
    f4 = f5 + f4;
    f4 = f1 * f4 + f2;
    f6 = f13 * f4 + f2;
    f6 = f3 * f1 + f6;
    f1 = f0 + f6;
    if ((s32)r6 >= (s32)tmp) {
        r4 = 0x43300000;
        *(u32*)(sp + 0x24) = tmp;
        tmp = ((r7 << 2) | ((u32)r7 >> 30)) & 0x00000002;
        tmp = 0x1 - tmp;
        f5 = lbl_8047C8D0;
        f2 = f1 * f1;
        f3 = lbl_8047C8C8;
        *(u32*)(sp + 0x2C) = tmp;
        f7 = f4 - f5;
        f1 = f1 + f7;
        f4 = f4 - f5;
        f1 = f2 / f1;
        f1 = f1 - f6;
        f0 = f0 - f1;
        f0 = -(f3 * f0 - f7);
        f1 = f4 * f0;
        return;
    }
    if ((s32)r3 == 1) {
        return;
    }
    f2 = lbl_8047C8A8;
    tmp = 0x0;
    f4 = f2 / f1;
    f1 = lbl_8047C8A0;
    *(u32*)(sp + 0x1C) = tmp;
    f0 = f2 - f0;
    *(u32*)(sp + 0x14) = tmp;
    f0 = f6 - f0;
    f1 = f3 * f2 + f1;
    f0 = f3 * f0 + f1;
    f1 = f4 * f0 + f3;

    return;
}

/* fn_800CD85C - 0x800CD85C | size: 0x218 */
void fn_800CD85C(void) {
    extern u8 lbl_802702B8[];
    extern f64 lbl_8047C8D8;
    extern f64 lbl_8047C8E0;
    extern f64 lbl_8047C8E8;
    extern f64 lbl_8047C8F0;
    extern f64 lbl_8047C8F8;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;

    r3 = (u32)lbl_802702B8;
    tmp = 0x44100000;
    r5 = (u32)lbl_802702B8;
    r4 = r6 & 0x7FFFFFFF;
    if ((s32)r4 >= (s32)tmp) {
        tmp = 0x7FF00000;
        if ((s32)r4 > (s32)tmp) { f1 = f0 + f0; return; }
        /* subis tmp, r4, 0x7ff0 */;
        if (tmp == 0 && (s32)tmp != 0) {

            f1 = f0 + f0;
            return;
        }
        if ((s32)r6 > 0) {
            r4 = r5 + 0x0;
            r3 = r5 + 0x20;
            f1 = *(f64*)((u8*)r4 + 0x18);
            f0 = *(f64*)((u8*)r3 + 0x18);
            f1 = f1 + f0;
            return;
        }
        r4 = r5 + 0x0;
        r3 = r5 + 0x20;
        f1 = *(f64*)((u8*)r4 + 0x18);
        f0 = *(f64*)((u8*)r3 + 0x18);
        f1 = -f1;
        f1 = f1 - f0;
        return;
    }
    tmp = 0x3FDC0000;
    do {
        if ((s32)r4 < (s32)tmp) {
            tmp = 0x3E200000;
            if ((s32)r4 < (s32)tmp) {
                f2 = lbl_8047C8D8;
                f0 = lbl_8047C8E0;
                f2 = f2 + f1;
                if (f2 > f0) {
                    return;
            }
            }
            tmp = -0x1;
            break;
        }
        /* fabs */ f3 = (f1 < 0) ? -f1 : f1;
        tmp = 0x3FF30000;
        if ((s32)r4 < (s32)tmp) {
            tmp = 0x3FE60000;
            if ((s32)r4 < (s32)tmp) {
                f2 = lbl_8047C8E8;
                tmp = 0x0;
                f1 = lbl_8047C8E0;
                f0 = f2 + f3;
                f1 = f2 * f3 - f1;
                f0 = f1 / f0;
                break;
            }
            f0 = lbl_8047C8E0;
            tmp = 0x1;
            f1 = f3 - f0;
            f0 = f0 + f3;
            f0 = f1 / f0;
            break;
        }
        r3 = 0x40040000;
        tmp = r3 + -0x8000;
        if ((s32)r4 < (s32)tmp) {
            f2 = lbl_8047C8F0;
            tmp = 0x2;
            f0 = lbl_8047C8E0;
            f1 = f3 - f2;
            f0 = f2 * f3 + f0;
            f0 = f1 / f0;
            break;
        }
        f0 = lbl_8047C8F8;
        tmp = 0x3;
        f0 = f0 / f3;
    } while (0);

    r3 = r5 + 0x40;
    f4 = *(f64*)((u8*)r3 + 0x50);
    f11 = f9 * f9;
    f1 = *(f64*)((u8*)r3 + 0x40);
    f7 = *(f64*)((u8*)r3 + 0x30);
    f3 = *(f64*)((u8*)r3 + 0x48);
    f0 = *(f64*)((u8*)r3 + 0x38);
    f10 = f11 * f11;
    f6 = *(f64*)((u8*)r3 + 0x20);
    f2 = *(f64*)((u8*)r3 + 0x28);
    f5 = *(f64*)((u8*)r3 + 0x10);
    f8 = f10 * f4 + f1;
    f1 = *(f64*)((u8*)r3 + 0x18);
    f4 = *(f64*)((u8*)r5 + 0x40);
    f3 = f10 * f3 + f0;
    f0 = *(f64*)((u8*)r3 + 0x8);
    f7 = f10 * f8 + f7;
    f2 = f10 * f3 + f2;
    f3 = f10 * f7 + f6;
    f1 = f10 * f2 + f1;
    f2 = f10 * f3 + f5;
    f0 = f10 * f1 + f0;
    f1 = f10 * f2 + f4;
    f2 = f10 * f0;
    f0 = f11 * f1;
    if ((s32)tmp < 0) {
        f0 = f0 + f2;
        f1 = -(f9 * f0 - f9);
        return;
    }
    tmp = tmp << 3;
    r3 = r5 + 0x20;
    f1 = f0 + f2;
    f0 = *(f64*)(r3 + tmp);
    r3 = r5 + 0x0;
    f2 = *(f64*)(r3 + tmp);
    f0 = f9 * f1 - f0;
    f0 = f0 - f9;
    f1 = f2 - f0;
    if ((s32)r6 >= 0) return;
    f1 = -f1;

    return;
}

/* fn_800CDA74 - 0x800CDA74 | size: 0x144 */
void fn_800CDA74(void) {
    extern f64 lbl_8047C900;
    extern f64 lbl_8047C908;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    /* extrwi r3, r5, 11, 1 */;
    if ((s32)r7 < 0x14) {
        if ((s32)r7 < 0) {
            f2 = lbl_8047C900;
            f0 = lbl_8047C908;
            f1 = f2 + f1;
            if (f1 <= f0) return;
            if ((s32)r5 < 0) {
                r5 = 0x80000000;
                r6 = 0x0;
                return;
            }
            /* or. tmp, r5, r6 */;
            if ((s32)r5 == 0) return;
            r5 = 0x3FF00000;
            r6 = 0x0;
            return;
        }
        r3 = 0x100000;
        r4 = (s32)tmp >> r7;
        tmp = r5 & r4;
        /* or. tmp, r6, tmp */;
        if ((s32)r5 == 0) {
            return;
        }
        f2 = lbl_8047C900;
        f0 = lbl_8047C908;
        f1 = f2 + f1;
        if (f1 <= f0) return;
        if ((s32)r5 > 0) {
            tmp = (s32)r3 >> r7;
            r5 = r5 + tmp;
        }
        r5 = r5 & ~r4;
        r6 = 0x0;
        return;
    }
    if ((s32)r7 > 0x33) {
        if ((s32)r7 != 0x400) return;
        f1 = f1 + f1;
        return;
    }
    r3 = -0x1;
    r4 = (u32)r3 >> tmp;
    /* and. tmp, r6, r4 */;
    if ((s32)r7 == 0x400) {
        return;
    }
    f2 = lbl_8047C900;
    f0 = lbl_8047C908;
    f1 = f2 + f1;
    if (f1 <= f0) return;
    if ((s32)r5 <= 0) { r6 = r6 & ~r4; return; }
    if ((s32)r7 == 0x14) {
        r5 = r5 + 0x1;
        r6 = r6 & ~r4;
        return;
    }
    tmp = 0x34 - r7;
    r3 = 0x1;
    tmp = r3 << tmp;
    tmp = r6 + tmp;
    if (tmp < r6) {
        r5 = r5 + 0x1;
    }
    r6 = tmp;

    r6 = r6 & ~r4;

    return;
}

/* fn_800CDBB8 - 0x800CDBB8 | size: 0x28 */
f64 fn_800CDBB8(f64 x, f64 y) {
    u32 hx = *(u32*)&x;
    u32 hy = *(u32*)&y;
    u32 r = (hx & 0x7FFFFFFF) | (hy & 0x80000000);
    *(u32*)&x = r;
    return x;
}

/* cos - 0x800CDBE0 | size: 0xD4 */
void cos(void) {
    extern f64 lbl_8047C910;
    extern void fn_800CC2C0();
    extern void fn_800CC660();
    extern void fn_800CD5A8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r3 = 0x3FE90000;
    tmp = r3 + 0x21fb;
    r3 = r3 & 0x7FFFFFFF;
    if ((s32)r3 <= (s32)tmp) {
        f2 = lbl_8047C910;
        fn_800CC660();
        return;
    }
    tmp = 0x7FF00000;
    if ((s32)r3 >= (s32)tmp) {
        f1 = f1 - f1;
        return;
    }
    r3 = (u32)sp + 0x10;
    fn_800CC2C0();
    tmp = r3 & 0x3;
    if ((s32)tmp != 1) {
        if ((s32)tmp < 1) {
            if ((s32)tmp < 0) {
                goto L_800CDC94;
            }
            if ((s32)tmp >= 3) goto L_800CDC94;
            goto L_800CDC80;
            }
        fn_800CC660();
        return;
    }
    r3 = 0x1;
    fn_800CD5A8();
    f1 = -f1;
    return;
L_800CDC80:
    fn_800CC660();
    f1 = -f1;
    return;
L_800CDC94:
    r3 = 0x1;
    fn_800CD5A8();

    return;
}

/* fn_800CDCB4 - 0x800CDCB4 | size: 0x148 */
void fn_800CDCB4(void) {
    extern f64 lbl_8047C918;
    extern f64 lbl_8047C920;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    /* extrwi r3, r5, 11, 1 */;
    if ((s32)r7 < 0x14) {
        if ((s32)r7 < 0) {
            f2 = lbl_8047C918;
            f0 = lbl_8047C920;
            f1 = f2 + f1;
            if (f1 <= f0) return;
            if ((s32)r5 >= 0) {
                r6 = 0x0;
                r5 = 0x0;
                return;
            }
            tmp = r5 & 0x7FFFFFFF;
            /* or. tmp, tmp, r6 */;
            if ((s32)r5 == 0) return;
            r5 = 0xBFF00000;
            r6 = 0x0;
            return;
        }
        r3 = 0x100000;
        r4 = (s32)tmp >> r7;
        tmp = r5 & r4;
        /* or. tmp, r6, tmp */;
        if ((s32)r5 == 0) {
            return;
        }
        f2 = lbl_8047C918;
        f0 = lbl_8047C920;
        f1 = f2 + f1;
        if (f1 <= f0) return;
        if ((s32)r5 < 0) {
            tmp = (s32)r3 >> r7;
            r5 = r5 + tmp;
        }
        r5 = r5 & ~r4;
        r6 = 0x0;
        return;
    }
    if ((s32)r7 > 0x33) {
        if ((s32)r7 != 0x400) return;
        f1 = f1 + f1;
        return;
    }
    r3 = -0x1;
    r4 = (u32)r3 >> tmp;
    /* and. tmp, r6, r4 */;
    if ((s32)r7 == 0x400) {
        return;
    }
    f2 = lbl_8047C918;
    f0 = lbl_8047C920;
    f1 = f2 + f1;
    if (f1 <= f0) return;
    if ((s32)r5 >= 0) { r6 = r6 & ~r4; return; }
    if ((s32)r7 == 0x14) {
        r5 = r5 + 0x1;
        r6 = r6 & ~r4;
        return;
    }
    tmp = 0x34 - r7;
    r3 = 0x1;
    tmp = r3 << tmp;
    tmp = r6 + tmp;
    if (tmp < r6) {
        r5 = r5 + 0x1;
    }
    r6 = tmp;

    r6 = r6 & ~r4;

    return;
}

/* fn_800CDDFC - 0x800CDDFC | size: 0x8C */
void fn_800CDDFC(void) {
    extern f64 lbl_8047C928;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r4 = 0x0;
    tmp = 0x7FF00000;
    *(u32*)((u8*)r3 + 0x0) = r4;
    r4 = r5 & 0x7FFFFFFF;
    if ((s32)r4 >= (s32)tmp) return;
    /* or. tmp, r4, r6 */;
    if ((s32)r4 == (s32)tmp) {

        return;
    }
    tmp = 0x100000;
    if ((s32)r4 < (s32)tmp) {
        f0 = lbl_8047C928;
        tmp = -0x36;
        *(u32*)((u8*)r3 + 0x0) = tmp;
        f0 = f1 * f0;
        r4 = r5 & 0x7FFFFFFF;
    }
    tmp = r5 & 0x800FFFFF;
    r5 = *(u32*)((u8*)r3 + 0x0);
    r4 = (s32)r4 >> 20;
    tmp = tmp | (0x3fe0 << 16);
    *(u32*)(sp + 0x8) = tmp;
    r4 = r4 + r5;
    *(u32*)((u8*)r3 + 0x0) = tmp;

    return;
}

/* fn_800CDE88 - 0x800CDE88 | size: 0x1C4 */
void fn_800CDE88(void) {
    extern f64 lbl_8047C930;
    extern f64 lbl_8047C938;
    extern f64 lbl_8047C940;
    extern f64 lbl_8047C948;
    extern f64 lbl_8047C950;
    extern void fn_800CDBB8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    tmp = 0x7FF00000;
    r4 = r5 & 0x7FF00000;
    if ((s32)r4 != (s32)tmp) {
        if ((s32)r4 >= (s32)tmp) goto L_800CDF08;
        if ((s32)r4 != 0) {
            goto L_800CDF08;
        }
        tmp = r5 & 0xFFFFF;
        if ((s32)r4 == 0) {
            if ((s32)tmp != 0) {
            }
            tmp = 0x1;
            goto L_800CDF0C;
            }
        tmp = 0x2;
        goto L_800CDF0C;
        }
    tmp = r5 & 0xFFFFF;
    if ((s32)tmp == 0) {
        if ((s32)tmp != 0) {
        }
        tmp = 0x5;
        goto L_800CDF0C;
        }
    tmp = 0x3;
    goto L_800CDF0C;
L_800CDF08:
    tmp = 0x4;
L_800CDF0C:
    if ((s32)tmp <= 2) return;
    f0 = lbl_8047C930;
    if (f0 == f1) {
        return;
    }
    /* extrwi. r4, r5, 11, 1 */;
    if (f0 == f1) {
        tmp = r5 & 0x7FFFFFFF;
        /* or. tmp, r6, tmp */;
        if (f0 == f1) {
            return;
        }
        r4 = 0xFFFF0000;
        f0 = lbl_8047C938;
        tmp = r4 + 0x3cb0;
        f1 = f1 * f0;
        /* extrwi r4, r5, 11, 1 */;
        if ((s32)r3 < (s32)tmp) {
            f0 = lbl_8047C940;
            f1 = f0 * f1;
            return;
    }
    }
    if ((s32)r4 == 0x7ff) {
        f1 = f0 + f0;
        return;
    }
    r4 = r4 + r3;
    if ((s32)r4 > 0x7fe) {
        f1 = lbl_8047C948;
        fn_800CDBB8();
        f0 = lbl_8047C948;
        f1 = f0 * f1;
        return;
    }
    if ((s32)r4 > 0) {
        r3 = r5 & 0x800FFFFF;
        tmp = r4 << 20;
        tmp = r3 | tmp;
        *(u32*)(sp + 0x8) = tmp;
        return;
    }
    if ((s32)r4 <= (s32)-0x36) {
        r4 = 0x10000;
        if ((s32)r3 > (s32)tmp) {
            f1 = lbl_8047C948;
            fn_800CDBB8();
            f0 = lbl_8047C948;
            f1 = f0 * f1;
            return;
        }
        f1 = lbl_8047C940;
        fn_800CDBB8();
        f0 = lbl_8047C940;
        f1 = f0 * f1;
        return;
    }
    tmp = r4 + 0x36;
    r3 = r5 & 0x800FFFFF;
    tmp = tmp << 20;
    f1 = lbl_8047C950;
    tmp = r3 | tmp;
    *(u32*)(sp + 0x8) = tmp;
    f1 = f1 * f0;

    return;
}

/* fn_800CE04C - 0x800CE04C | size: 0xFC */
void fn_800CE04C(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    /* extrwi r4, r5, 11, 1 */;
    if ((s32)r7 < 0x14) {
        if ((s32)r7 < 0) {
            /* clrrwi r4, r5, 31 */;
            tmp = 0x0;
            *(u32*)((u8*)r3 + 0x0) = r4;
            *(u32*)((u8*)r3 + 0x4) = tmp;
            return;
        }
        r4 = 0x100000;
        r4 = (s32)tmp >> r7;
        tmp = r5 & r4;
        /* or. tmp, r6, tmp */;
        if ((s32)r7 == 0) {
            /* clrrwi r4, r5, 31 */;
            tmp = 0x0;
            *(f64*)((u8*)r3 + 0x0) = f1;
            return;
        }
        r4 = r5 & ~r4;
        tmp = 0x0;
        *(u32*)((u8*)r3 + 0x0) = r4;
        *(u32*)((u8*)r3 + 0x4) = tmp;
        f0 = *(f64*)((u8*)r3 + 0x0);
        f1 = f1 - f0;
        return;
    }
    if ((s32)r7 > 0x33) {
        /* clrrwi r4, r5, 31 */;
        tmp = 0x0;
        *(f64*)((u8*)r3 + 0x0) = f1;
        return;
    }
    r4 = -0x1;
    r4 = (u32)r4 >> tmp;
    /* and. tmp, r6, r4 */;
    if ((s32)r7 == 0x33) {
        /* clrrwi r4, r5, 31 */;
        tmp = 0x0;
        *(f64*)((u8*)r3 + 0x0) = f1;
        return;
    }
    *(u32*)((u8*)r3 + 0x0) = r5;
    tmp = r6 & ~r4;
    *(u32*)((u8*)r3 + 0x4) = tmp;
    f0 = *(f64*)((u8*)r3 + 0x0);
    f1 = f1 - f0;

    return;
}

/* sin - 0x800CE148 | size: 0xD8 */
void sin(void) {
    extern f64 lbl_8047C958;
    extern void fn_800CC2C0();
    extern void fn_800CC660();
    extern void fn_800CD5A8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r3 = 0x3FE90000;
    tmp = r3 + 0x21fb;
    r3 = r3 & 0x7FFFFFFF;
    if ((s32)r3 <= (s32)tmp) {
        f2 = lbl_8047C958;
        r3 = 0x0;
        fn_800CD5A8();
        return;
    }
    tmp = 0x7FF00000;
    if ((s32)r3 >= (s32)tmp) {
        f1 = f1 - f1;
        return;
    }
    r3 = (u32)sp + 0x10;
    fn_800CC2C0();
    tmp = r3 & 0x3;
    if ((s32)tmp != 1) {
        if ((s32)tmp < 1) {
            if ((s32)tmp < 0) {
                goto L_800CE200;
            }
            if ((s32)tmp >= 3) goto L_800CE200;
            goto L_800CE1E8;
            }
        r3 = 0x1;
        fn_800CD5A8();
        return;
    }
    fn_800CC660();
    return;
L_800CE1E8:
    r3 = 0x1;
    fn_800CD5A8();
    f1 = -f1;
    return;
L_800CE200:
    fn_800CC660();
    f1 = -f1;

    return;
}

/* fn_800CE220 - 0x800CE220 | size: 0x78 */
f64 fn_800CE220(f64 x) {
    extern f64 lbl_8047C960;
    extern f64 __kernel_tan();
    extern s32 __ieee754_rem_pio2();
    f64 y[2];
    s32 n, ix;

    ix = *(s32*)&x & 0x7FFFFFFF;
    if (ix <= 0x3FE921FB) return __kernel_tan(x, lbl_8047C960, 1);
    if (ix >= 0x7FF00000) return x - x;
    n = __ieee754_rem_pio2(x, y);
    return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
}

/* fn_800CE298 - 0x800CE298 | size: 0x20 */
void fn_800CE298(void) {
    extern void __ieee754_acos();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_acos();
    return;
}

/* fn_800CE2B8 - 0x800CE2B8 | size: 0x20 */
void fn_800CE2B8(void) {
    extern void __ieee754_asin();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_asin();
    return;
}

/* fn_800CE2D8 - 0x800CE2D8 | size: 0x20 */
void fn_800CE2D8(void) {
    extern void __ieee754_atan2();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_atan2();
    return;
}

/* fn_800CE2F8 - 0x800CE2F8 | size: 0x20 */
void fn_800CE2F8(void) {
    extern void __ieee754_exp();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_exp();
    return;
}

/* fn_800CE318 - 0x800CE318 | size: 0x20 */
void fn_800CE318(void) {
    extern void __ieee754_fmod();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_fmod();
    return;
}

/* fn_800CE338 - 0x800CE338 | size: 0x20 */
void fn_800CE338(void) {
    extern void __ieee754_log();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_log();
    return;
}

/* fn_800CE358 - 0x800CE358 | size: 0x20 */
void fn_800CE358(void) {
    extern void __ieee754_pow();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_pow();
    return;
}

/* __ieee754_sqrt - 0x800CE378 | size: 0x224 */
void __ieee754_sqrt(void) {
    extern u8 lbl_80478AC0[];
    extern u32 lbl_8047AA10;
    extern f64 lbl_8047C968;
    u8 sp[0x20];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r3 = r6 & 0x7FF00000;
    /* subis r3, r3, 0x7ff0 */;
    if (r3 == 0) {
        f1 = f1 * f1 + f1;
        tmp = 0x21;
        lbl_8047AA10 = tmp;
        return;
    }
    if ((s32)r6 <= 0) {
        r3 = r6 & 0x7FFFFFFF;
        /* or. r3, tmp, r3 */;
        if ((s32)r6 == 0) {
            return;
        }
        if ((s32)r6 < 0) {
            r3 = (u32)lbl_80478AC0;
            tmp = 0x21;
            lbl_8047AA10 = tmp;
            f1 = *(f32*)lbl_80478AC0;
            return;
    }
    }
    /* srawi. r3, r6, 20 */;
    if ((s32)r6 == 0) {
        while ((s32)r6 == 0) {

            r4 = (u32)tmp >> 11;
            tmp = tmp << 21;
            r6 = r6 | r4;

        }
        r7 = 0x0;
        while (1) {
            r4 = r6 & 0x00100000;
            if ((s32)r6 != 0) break;
            r6 = r6 << 1;
            r7 = r7 + 0x1;

        }
        r4 = 0x20 - r7;
        r4 = (u32)tmp >> r4;
        tmp = tmp << r7;
        r3 = r3 - r5;
        r6 = r6 | r4;
    }
    r5 = r6 & 0xFFFFF;
    r4 = r4 & 0x1;
    r5 = r5 | (0x10 << 16);
    if ((s32)r6 != 0) {
        r4 = (u32)tmp >> 31;
        tmp = tmp + tmp;
        r4 = r4 + r5;
        r5 = r5 + r4;
    }
    r4 = (u32)tmp >> 31;
    tmp = tmp + tmp;
    r4 = r4 + r5;
    r9 = 0x0;
    r5 = r5 + r4;
    r11 = 0x0;
    r10 = 0x0;
    r12 = 0x0;
    r6 = 0x200000;
    while (r6 != 0) {

        r4 = r11 + r6;
        if ((s32)r4 <= (s32)r5) {
            r11 = r4 + r6;
            r5 = r5 - r4;
            r12 = r12 + r6;
        }
        r4 = (u32)tmp >> 31;
        tmp = tmp + tmp;
        r4 = r4 + r5;
        r6 = (u32)r6 >> 1;
        r5 = r5 + r4;

    }
    r6 = 0x80000000;
    while (1) {
        if (r6 == 0) break;
        r7 = r11;
        r8 = r9 + r6;
        if ((s32)r11 >= (s32)r5) {
            if ((s32)r11 == (s32)r5 && r8 <= tmp) {

            }
            /* clrrwi r4, r8, 31 */;
            r9 = r8 + r6;
            r4 = r4 + (0x8000 << 16);
            if (r4 == 0) {
                /* clrrwi. r4, r9, 31 */;
                if (r4 == 0) {
                    r11 = r11 + 0x1;
            }
            }
            r5 = r5 - r7;
            if (tmp < r8) {
            }
            tmp = tmp - r8;
            r10 = r10 + r6;
            }
        r4 = (u32)tmp >> 31;
        tmp = tmp + tmp;
        r4 = r4 + r5;
        r6 = (u32)r6 >> 1;
        r5 = r5 + r4;

    }
    /* or. tmp, r5, tmp */;
    if (r6 != 0) {
        f0 = lbl_8047C968;
        tmp = r10 + (0x1 << 16);
        if (tmp == 0xffff) {
            r10 = 0x0;
            r12 = r12 + 0x1;
            goto L_800CE55C;
        }
        tmp = r10 & 0x1;
        r10 = r10 + tmp;
    }
L_800CE55C:
    tmp = r12 & 0x1;
    r4 = (s32)r12 >> 1;
    r5 = (u32)r10 >> 1;
    r4 = r4 + (0x3fe0 << 16);
    if ((s32)tmp == 1) {
        r5 = r5 | (0x8000 << 16);
    }
    tmp = (s32)tmp >> 1;
    tmp = tmp << 20;
    r4 = r4 + tmp;

    return;
}

/* fn_800CE59C - 0x800CE59C | size: 0x8 */
f64 fn_800CE59C(f64 x) {
    return __fabs(x);
}

/* fn_800CE5A4 - 0x800CE5A4 | size: 0xE4 */
void fn_800CE5A4(void) {
    extern u8 lbl_80478AC0[];
    extern f32 lbl_8047C970;
    extern f64 lbl_8047C978;
    extern f64 lbl_8047C980;
    extern f64 lbl_8047C988;
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;

    f0 = lbl_8047C970;
    if (f1 > f0) {
        /* frsqrte f2, f1 */;
        f4 = lbl_8047C978;
        f3 = lbl_8047C980;
        f0 = f2 * f2;
        f2 = f4 * f2;
        f0 = -(f1 * f0 - f3);
        f2 = f2 * f0;
        f0 = f2 * f2;
        f2 = f4 * f2;
        f0 = -(f1 * f0 - f3);
        f2 = f2 * f0;
        f0 = f2 * f2;
        f2 = f4 * f2;
        f0 = -(f1 * f0 - f3);
        f0 = f2 * f0;
        f1 = f1 * f0;
        f1 = (f32)f1;
        return;
    }
    f0 = lbl_8047C988;
    if (f1 < f0) {
        r3 = (u32)lbl_80478AC0;
        f1 = *(f32*)lbl_80478AC0;
        return;
    }
    *(f32*)(sp + 0x8) = f1;
    tmp = 0x7F800000;
    r3 = r4 & 0x7F800000;
    if ((s32)r3 != (s32)tmp) {
        if ((s32)r3 >= (s32)tmp) goto L_800CE66C;
        if ((s32)r3 != 0) {
            goto L_800CE66C;
        }
        tmp = r4 & 0x7FFFFF;
        if ((s32)r3 != 0) {
            tmp = 0x1;
            goto L_800CE670;
        }
        tmp = 0x2;
        goto L_800CE670;
        }
    tmp = r4 & 0x7FFFFF;
    if ((s32)r3 != 0) {
        tmp = 0x5;
        goto L_800CE670;
    }
    tmp = 0x3;
    goto L_800CE670;
L_800CE66C:
    tmp = 0x4;
L_800CE670:
    if ((s32)tmp != 1) return;
    r3 = (u32)lbl_80478AC0;
    f1 = *(f32*)lbl_80478AC0;

    return;
}

/* fn_800CE688 - 0x800CE688 | size: 0x24 */
f32 fn_800CE688(void) {
    extern f64 fn_800CE220();

    return (f32)fn_800CE220();
}

/* fn_800CE6AC - 0x800CE6AC | size: 0x24 */
f32 fn_800CE6AC(void) {
    extern f64 sin();

    return (f32)sin();
}

/* fn_800CE6D0 - 0x800CE6D0 | size: 0x24 */
f32 fn_800CE6D0(void) {
    extern f64 cos();

    return (f32)cos();
}

/* fn_800CE6F4 - 0x800CE6F4 | size: 0x24 */
f32 fn_800CE6F4(void) {
    extern f64 fn_800CE298();

    return (f32)fn_800CE298();
}

/* fn_800CE718 - 0x800CE718 | size: 0x64 */
void fn_800CE718(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    f32 f1 = 0.0f;

    tmp = 0x7F800000;
    *(f32*)(sp + 0x8) = f1;
    r3 = r4 & 0x7F800000;
    if ((s32)r3 != (s32)tmp) {
        if ((s32)r3 >= (s32)tmp) { r3 = 0x4; return; }
        if ((s32)r3 != 0) {
            r3 = 0x4;
            return;
        }
        r3 = r4 & 0x7FFFFF;
        tmp = -r3;
        tmp = tmp | r3;
        r3 = (s32)tmp >> 31;
        r3 = r3 + 0x2;
        return;
        }
    tmp = r4 & 0x7FFFFF;
    r3 = 0x3;
    if ((s32)r3 == 0) return;
    r3 = 0x5;
    return;


}

/* fn_800CE77C - 0x800CE77C | size: 0x20 */
void fn_800CE77C(void) {
    extern void __ieee754_sqrt();
    u8 sp[0x10];
    u32 tmp = 0;

    __ieee754_sqrt();
    return;
}
