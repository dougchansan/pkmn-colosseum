/**
 * @file hsd_jobj_display.c
 * @brief HSD internal functions (0x801A1988-0x801A3FBC).
 *
 * Stub coverage for 20 functions.
 */

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"

/* 0x78 | fn_801A1988 | generic */
u32 fn_801A1988(void) {
    __assert();
    fn_8019D9DC();
    return 1;
}

/* 0x801A1A00 | 0x140 */
void fn_801A1A00(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void fn_800A2D98();
    extern void fn_800A2EB4();
    extern void HSD_CObjGetCurrent();
    extern void __assert();
    extern void fn_8019D9DC();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r5;
    /* mr. r29, r4 */;
    r28 = r3;
    if ((s32)tmp != 0) {
        if (r29 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r29 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r29 == 0) {
            tmp = *(u32*)((u8*)r29 + 0x14);
            tmp = tmp & 0x00000040;
            if (r29 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) {
            r3 = r29;
            fn_8019D9DC();
    }
    }
    r31 = *(u32*)((u8*)r29 + 0x10);
    if (r31 != 0) {
        if (r31 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r31 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r31 == 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x00000040;
            if (r31 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) {
            r3 = r31;
            fn_8019D9DC();
    }
    }
    r3 = *(u32*)((u8*)r29 + 0x10);
    r4 = r30;
    r3 = r3 + 0x44;
    fn_800A2EB4();
    r4 = r30;
    r5 = r30;
    r3 = r29 + 0x44;
    fn_800A2D98();
    if (r28 != 0) {
        r3 = r28;
        r4 = r30;
        r5 = r30;
        fn_800A2D98();
        return;
    }
    HSD_CObjGetCurrent();
    if (r3 == 0) return;
    r3 = r3 + 0x54;
    r4 = r30;
    r5 = r30;
    fn_800A2D98();

    return;
}

/* 0x801A1B40 | 0x3C */
void fn_801A1B40(void) {
    extern void fn_801A1B7C();
    extern void fn_801C2A04();
    extern void fn_801C2A60();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp != 0) {
        fn_801C2A60();
        r3 = r31;
        fn_801A1B7C();
        fn_801C2A04();
    }
    return;
}

/* 0x801A1B7C | 0x3B0 */
void fn_801A1B7C(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void __assert();
    extern void HSD_DObjAnimAll();
    extern void fn_8019D980();
    extern void fn_801A1B7C();
    extern void fn_801A1F2C();
    extern void fn_801A3D04();
    extern void fn_801B0040();
    extern void fn_801C27F4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp == 0) return;
    do {
    if (r31 == 0) break;

    if (r31 != 0) {
        if (r31 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r31 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r31 == 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x00000040;
            if (r31 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) goto L_801A1CD8;
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00800000;
        if ((s32)r3 != 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x01000000;
            if ((s32)r3 == 0) {
                tmp = *(u32*)((u8*)r31 + 0xC);
                if (tmp != 0) {
                    r30 = *(u32*)((u8*)r31 + 0xC);
                    if (r30 == 0) {
                        r3 = (u32)lbl_8047DB34;
                        r4 = 0x25d;
                        r5 = (u32)lbl_8047DB3C;
                        __assert();
                    }
                    tmp = *(u32*)((u8*)r30 + 0x14);
                    r3 = 0x0;
                    tmp = tmp & 0x00800000;
                    if (r30 == 0) {
                        tmp = *(u32*)((u8*)r30 + 0x14);
                        tmp = tmp & 0x00000040;
                        if (r30 != 0) {
                            r3 = 0x1;
                    }
                    }
                    if ((s32)r3 != 0) {
                        tmp = *(u32*)((u8*)r31 + 0x14);
                        tmp = tmp | 0x40;
                        *(u32*)((u8*)r31 + 0x14) = tmp;
            }
            }
            }
            goto L_801A1CD8;
        }
        tmp = *(u32*)((u8*)r31 + 0xC);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r3 + 0x14);
            tmp = tmp & 0x00000040;
            if (tmp == 0) {
            }
            tmp = *(u32*)((u8*)r31 + 0x14);
            r3 = tmp & 0x00600000;
            /* subis tmp, r3, 0x20 */;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x14);
                r3 = tmp & 0x00600000;
                /* subis tmp, r3, 0x40 */;
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r31 + 0x14);
                    r3 = tmp & 0x00600000;
                    /* subis tmp, r3, 0x60 */;
            }
            }
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x80);
                if (tmp == 0) goto L_801A1CD8;
            }
            }
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp | 0x40;
        *(u32*)((u8*)r31 + 0x14) = tmp;
    }
L_801A1CD8:
    r5 = *(u32*)((u8*)r31 + 0x0);
    r4 = r31;
    r3 = *(u32*)((u8*)r31 + 0x7C);
    r5 = *(u32*)((u8*)r5 + 0x50);
    fn_801C27F4();
    r3 = *(u32*)((u8*)r31 + 0x80);
    fn_801B0040();
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x4020;
    tmp = __cntlzw(tmp);
    /* srwi. tmp, tmp, 5 */;
    if (tmp == 0) break;

    r3 = *(u32*)((u8*)r31 + 0x18);
    HSD_DObjAnimAll();
    } while (0);

    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x00001000;
    if (tmp != 0) return;
    r31 = *(u32*)((u8*)r31 + 0x10);
    while (1) {
        if (r31 == 0) break;
        do {
        if (r31 == 0) break;

        do {
        if (r31 == 0) break;

        if (r31 != 0) {
            r3 = r31;
            fn_8019D980();
            if ((s32)r3 != 0) goto L_801A1E00;
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x00800000;
            if ((s32)r3 != 0) {
                tmp = *(u32*)((u8*)r31 + 0x14);
                tmp = tmp & 0x01000000;
                if ((s32)r3 == 0) {
                    tmp = *(u32*)((u8*)r31 + 0xC);
                    if (tmp != 0) {
                        r3 = *(u32*)((u8*)r31 + 0xC);
                        fn_8019D980();
                        if ((s32)r3 != 0) {
                            tmp = *(u32*)((u8*)r31 + 0x14);
                            tmp = tmp | 0x40;
                            *(u32*)((u8*)r31 + 0x14) = tmp;
                }
                }
                }
                goto L_801A1E00;
            }
            tmp = *(u32*)((u8*)r31 + 0xC);
            if (tmp != 0) {
                r3 = *(u32*)((u8*)r31 + 0xC);
                tmp = *(u32*)((u8*)r3 + 0x14);
                tmp = tmp & 0x00000040;
                if (tmp == 0) {
                }
                tmp = *(u32*)((u8*)r31 + 0x14);
                r3 = tmp & 0x00600000;
                /* subis tmp, r3, 0x20 */;
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r31 + 0x14);
                    r3 = tmp & 0x00600000;
                    /* subis tmp, r3, 0x40 */;
                    if (tmp != 0) {
                        tmp = *(u32*)((u8*)r31 + 0x14);
                        r3 = tmp & 0x00600000;
                        /* subis tmp, r3, 0x60 */;
                }
                }
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r31 + 0x80);
                    if (tmp == 0) goto L_801A1E00;
                }
                }
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp | 0x40;
            *(u32*)((u8*)r31 + 0x14) = tmp;
        }
    L_801A1E00:
        r5 = *(u32*)((u8*)r31 + 0x0);
        r4 = r31;
        r3 = *(u32*)((u8*)r31 + 0x7C);
        r5 = *(u32*)((u8*)r5 + 0x50);
        fn_801C27F4();
        r3 = *(u32*)((u8*)r31 + 0x80);
        fn_801B0040();
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x4020;
        tmp = __cntlzw(tmp);
        /* srwi. tmp, tmp, 5 */;
        if (tmp == 0) break;

        r3 = *(u32*)((u8*)r31 + 0x18);
        HSD_DObjAnimAll();
        } while (0);

        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00001000;
        if (tmp != 0) break;

        r30 = *(u32*)((u8*)r31 + 0x10);
        while (r30 != 0) {

            if (r30 != 0) {
                if (r30 != 0) {
                    r3 = r30;
                    fn_801A3D04();
                    r5 = *(u32*)((u8*)r30 + 0x0);
                    r4 = r30;
                    r3 = *(u32*)((u8*)r30 + 0x7C);
                    r5 = *(u32*)((u8*)r5 + 0x50);
                    fn_801C27F4();
                    r3 = *(u32*)((u8*)r30 + 0x80);
                    fn_801B0040();
                    tmp = *(u32*)((u8*)r30 + 0x14);
                    tmp = tmp & 0x4020;
                    tmp = __cntlzw(tmp);
                    /* srwi. tmp, tmp, 5 */;
                    if (r30 != 0) {
                        r3 = *(u32*)((u8*)r30 + 0x18);
                        HSD_DObjAnimAll();
                }
                }
                tmp = *(u32*)((u8*)r30 + 0x14);
                tmp = tmp & 0x00001000;
                if (r30 == 0) {
                    r28 = *(u32*)((u8*)r30 + 0x10);
                    while (r28 != 0) {

                        if (r28 != 0) {
                            r3 = r28;
                            fn_801A1F2C();
                            tmp = *(u32*)((u8*)r28 + 0x14);
                            tmp = tmp & 0x00001000;
                            if (r28 == 0) {
                                r29 = *(u32*)((u8*)r28 + 0x10);
                                while (r29 != 0) {

                                    r3 = r29;
                                    fn_801A1B7C();
                                    r29 = *(u32*)((u8*)r29 + 0x8);

                                }
                        }
                        }
                        r28 = *(u32*)((u8*)r28 + 0x8);

                    }
            }
            }
            r30 = *(u32*)((u8*)r30 + 0x8);

        }
        } while (0);

        r31 = *(u32*)((u8*)r31 + 0x8);

    }

    return;
}

/* 0x801A1F2C | 0x19C */
void fn_801A1F2C(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void __assert();
    extern void HSD_DObjAnimAll();
    extern void fn_801B0040();
    extern void fn_801C27F4();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp == 0) return;
    if (r31 != 0) {
        if (r31 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r31 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r31 == 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x00000040;
            if (r31 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) goto L_801A2078;
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00800000;
        if ((s32)r3 != 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x01000000;
            if ((s32)r3 == 0) {
                tmp = *(u32*)((u8*)r31 + 0xC);
                if (tmp != 0) {
                    r30 = *(u32*)((u8*)r31 + 0xC);
                    if (r30 == 0) {
                        r3 = (u32)lbl_8047DB34;
                        r4 = 0x25d;
                        r5 = (u32)lbl_8047DB3C;
                        __assert();
                    }
                    tmp = *(u32*)((u8*)r30 + 0x14);
                    r3 = 0x0;
                    tmp = tmp & 0x00800000;
                    if (r30 == 0) {
                        tmp = *(u32*)((u8*)r30 + 0x14);
                        tmp = tmp & 0x00000040;
                        if (r30 != 0) {
                            r3 = 0x1;
                    }
                    }
                    if ((s32)r3 != 0) {
                        tmp = *(u32*)((u8*)r31 + 0x14);
                        tmp = tmp | 0x40;
                        *(u32*)((u8*)r31 + 0x14) = tmp;
            }
            }
            }
            goto L_801A2078;
        }
        tmp = *(u32*)((u8*)r31 + 0xC);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r3 + 0x14);
            tmp = tmp & 0x00000040;
            if (tmp == 0) {
            }
            tmp = *(u32*)((u8*)r31 + 0x14);
            r3 = tmp & 0x00600000;
            /* subis tmp, r3, 0x20 */;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x14);
                r3 = tmp & 0x00600000;
                /* subis tmp, r3, 0x40 */;
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r31 + 0x14);
                    r3 = tmp & 0x00600000;
                    /* subis tmp, r3, 0x60 */;
            }
            }
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x80);
                if (tmp == 0) goto L_801A2078;
            }
            }
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp | 0x40;
        *(u32*)((u8*)r31 + 0x14) = tmp;
    }
L_801A2078:
    r5 = *(u32*)((u8*)r31 + 0x0);
    r4 = r31;
    r3 = *(u32*)((u8*)r31 + 0x7C);
    r5 = *(u32*)((u8*)r5 + 0x50);
    fn_801C27F4();
    r3 = *(u32*)((u8*)r31 + 0x80);
    fn_801B0040();
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x4020;
    tmp = __cntlzw(tmp);
    /* srwi. tmp, tmp, 5 */;
    if (tmp == 0) return;
    r3 = *(u32*)((u8*)r31 + 0x18);
    HSD_DObjAnimAll();

    return;
}

/* 0x801A20C8 | 0xA94 */
void fn_801A20C8(void) {
    extern u8 lbl_80274AA0[];
    extern u32 lbl_8047B29C;
    extern u32 lbl_8047B2A0;
    extern u32 lbl_8047B2A4;
    extern u32 lbl_8047B2A8;
    extern u8 lbl_8047DB20[];
    extern f32 lbl_8047DB30;
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern f32 lbl_8047DB48;
    extern f64 lbl_8047DB50;
    extern f64 lbl_8047DB60;
    extern u8 lbl_8047DB88[];
    extern f64 lbl_8047DB90;
    extern f64 lbl_8047DB98;
    extern void fn_800A2D64();
    extern void __assert();
    extern void fn_8019D620();
    extern void fn_8019F7F0();
    extern void fn_8019FB90();
    extern void fn_801A8D1C();
    extern void HSD_MtxGetTranslate();
    extern void fn_801A98CC();
    extern void fn_801A9DF0();
    extern void fn_801B00E0();
    extern void fn_801B1890();
    extern u8 jumptable_8036C934[];
    u8 sp[0x80];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;

    /* mr. r30, r3 */;
    r6 = (u32)lbl_80274AA0;
    r27 = r4;
    r29 = r5;
    r31 = (u32)lbl_80274AA0;
    if ((s32)tmp == 0) return;
    if (r27 > 0x39) return;
    r3 = (u32)jumptable_8036C934;
    tmp = r27 << 2;
    r3 = (u32)jumptable_8036C934;
    r3 = *(u32*)(r3 + tmp);
    ctr_fn = (void(*)(void))r3;
    f1 = *(f32*)((u8*)r29 + 0x0);
    f0 = lbl_8047DB60;
    if (f1 < f0) {
        f0 = lbl_8047DB48;
        *(f32*)((u8*)r29 + 0x0) = f0;
    }
    f1 = lbl_8047DB90;
    f0 = *(f32*)((u8*)r29 + 0x0);
    if (f1 < f0) {
        f0 = lbl_8047DB30;
        *(f32*)((u8*)r29 + 0x0) = f0;
    }
    tmp = *(u32*)((u8*)r30 + 0x7C);
    if (tmp == 0) {
        r5 = r31 + 0x260;
        r3 = (u32)lbl_8047DB20;
        r4 = 0x24e;
        __assert();
    }
    r3 = *(u32*)((u8*)r30 + 0x7C);
    r28 = *(u32*)((u8*)r3 + 0x18);
    if (r28 == 0) {
        r3 = (u32)lbl_8047DB20;
        r4 = 0x250;
        r5 = (u32)lbl_8047DB88;
        __assert();
    }
    tmp = *(u32*)((u8*)r28 + 0x18);
    if (tmp == 0) {
        r5 = r31 + 0x26c;
        r3 = (u32)lbl_8047DB20;
        r4 = 0x251;
        __assert();
    }
    r4 = *(u32*)((u8*)r28 + 0x18);
    r3 = (u32)sp + 0x8;
    f1 = *(f32*)((u8*)r29 + 0x0);
    fn_801B1890();
    f31 = *(f32*)(sp + 0x8);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3b8;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x38) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 == 0) {
        if (r30 != 0) {
            if (r30 == 0) {
                r3 = (u32)lbl_8047DB34;
                r4 = 0x25d;
                r5 = (u32)lbl_8047DB3C;
                __assert();
            }
            tmp = *(u32*)((u8*)r30 + 0x14);
            r3 = 0x0;
            tmp = tmp & 0x00800000;
            if (r30 == 0) {
                tmp = *(u32*)((u8*)r30 + 0x14);
                tmp = tmp & 0x00000040;
                if (r30 != 0) {
                    r3 = 0x1;
            }
            }
            if ((s32)r3 == 0) {
                r3 = r30;
                fn_8019D620();
    }
    }
    }
    f31 = *(f32*)(sp + 0xC);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3c6;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x3C) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 == 0) {
        if (r30 != 0) {
            if (r30 == 0) {
                r3 = (u32)lbl_8047DB34;
                r4 = 0x25d;
                r5 = (u32)lbl_8047DB3C;
                __assert();
            }
            tmp = *(u32*)((u8*)r30 + 0x14);
            r3 = 0x0;
            tmp = tmp & 0x00800000;
            if (r30 == 0) {
                tmp = *(u32*)((u8*)r30 + 0x14);
                tmp = tmp & 0x00000040;
                if (r30 != 0) {
                    r3 = 0x1;
            }
            }
            if ((s32)r3 == 0) {
                r3 = r30;
                fn_8019D620();
    }
    }
    }
    f31 = *(f32*)(sp + 0x10);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3d4;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x40) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x00200000;
    if ((s32)r3 != 0) {
        r3 = *(u32*)((u8*)r30 + 0x80);
        r4 = 0x40000000;
        r5 = 0x0;
        fn_801B00E0();
        if (r3 != 0) {
            f0 = *(f32*)((u8*)r29 + 0x0);
            *(f32*)((u8*)r3 + 0xC) = f0;
    }
    }
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2a4;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x00020000;
    if (r30 != 0) {
        r5 = r31 + 0x27c;
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2a5;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x1C) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2b8;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x00020000;
    if (r30 != 0) {
        r5 = r31 + 0x27c;
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2b9;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x20) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2cc;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x00020000;
    if (r30 != 0) {
        r5 = r31 + 0x27c;
        r3 = (u32)lbl_8047DB34;
        r4 = 0x2cd;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x24) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3b8;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x38) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3c6;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x3C) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x3d4;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x40) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x325;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x2C) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x333;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x30) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f31 = *(f32*)((u8*)r29 + 0x0);
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x341;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    *(f32*)((u8*)r30 + 0x34) = f31;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp & 0x02000000;
    if (r30 != 0) return;
    if (r30 == 0) return;
    if (r30 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r30 == 0) {
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x00000040;
        if (r30 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) return;
    r3 = r30;
    fn_8019D620();
    return;
    f1 = *(f32*)((u8*)r29 + 0x0);
    f0 = lbl_8047DB50;
    if (f1 > f0) {
        r3 = r30;
        r4 = 0x10;
        fn_8019F7F0();
        return;
    }
    r3 = r30;
    r4 = 0x10;
    fn_8019FB90();
    return;
    f1 = *(f32*)((u8*)r29 + 0x0);
    f0 = lbl_8047DB50;
    if (f1 > f0) {
        if (r30 == 0) return;
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp ^ 0x10;
        tmp = tmp & 0x00000008;
        if (r30 != 0) {
            if (r30 != 0) {
                if (r30 == 0) {
                    r3 = (u32)lbl_8047DB34;
                    r4 = 0x25d;
                    r5 = (u32)lbl_8047DB3C;
                    __assert();
                }
                tmp = *(u32*)((u8*)r30 + 0x14);
                r3 = 0x0;
                tmp = tmp & 0x00800000;
                if (r30 == 0) {
                    tmp = *(u32*)((u8*)r30 + 0x14);
                    tmp = tmp & 0x00000040;
                    if (r30 != 0) {
                        r3 = 0x1;
                }
                }
                if ((s32)r3 == 0) {
                    r3 = r30;
                    fn_8019D620();
        }
        }
        }
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0xFFFFFFEF;
        *(u32*)((u8*)r30 + 0x14) = tmp;
        return;
    }
    if (r30 == 0) return;
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp ^ 0x10;
    tmp = tmp & 0x00000008;
    if (r30 != 0) {
        if (r30 != 0) {
            if (r30 == 0) {
                r3 = (u32)lbl_8047DB34;
                r4 = 0x25d;
                r5 = (u32)lbl_8047DB3C;
                __assert();
            }
            tmp = *(u32*)((u8*)r30 + 0x14);
            r3 = 0x0;
            tmp = tmp & 0x00800000;
            if (r30 == 0) {
                tmp = *(u32*)((u8*)r30 + 0x14);
                tmp = tmp & 0x00000040;
                if (r30 != 0) {
                    r3 = 0x1;
            }
            }
            if ((s32)r3 == 0) {
                r3 = r30;
                fn_8019D620();
    }
    }
    }
    tmp = *(u32*)((u8*)r30 + 0x14);
    tmp = tmp | 0x10;
    *(u32*)((u8*)r30 + 0x14) = tmp;
    return;
    r28 = lbl_8047B29C;
    while (r28 != 0) {

        r4 = *(u32*)((u8*)r29 + 0x0);
        tmp = 0x43300000;
        *(u32*)(sp + 0x48) = tmp;
        r3 = r30;
        f1 = lbl_8047DB98;
        *(u32*)(sp + 0x4C) = tmp;
        r4 = r27;
        r12 = *(u32*)((u8*)r28 + 0x4);
        f1 = f0 - f1;
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        r28 = *(u32*)((u8*)r28 + 0x0);

    }
    return;
    r28 = lbl_8047B29C;
    while (r28 != 0) {

        r12 = *(u32*)((u8*)r28 + 0x4);
        r3 = r30;
        r4 = r27;
        f1 = *(f32*)((u8*)r29 + 0x0);
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        r28 = *(u32*)((u8*)r28 + 0x0);

    }
    return;
    tmp = lbl_8047B2A0;
    r3 = *(u32*)((u8*)r29 + 0x0);
    r4 = *(u32*)((u8*)r29 + 0x0);
    /* extrwi r5, r3, 24, 2 */;
    r4 = r4 & 0x3F;
    if (tmp == 0) return;
    r12 = lbl_8047B2A0;
    r6 = r30;
    r3 = 0x0;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    return;
    tmp = lbl_8047B2A4;
    if (tmp == 0) return;
    r12 = lbl_8047B2A4;
    r3 = *(u32*)((u8*)r29 + 0x0);
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    return;
    tmp = lbl_8047B2A8;
    if (tmp == 0) return;
    r12 = lbl_8047B2A8;
    r3 = r30;
    r4 = *(u32*)((u8*)r29 + 0x0);
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    return;
    f0 = *(f32*)((u8*)r29 + 0x0);
    *(f32*)((u8*)r30 + 0x44) = f0;
    f0 = *(f32*)((u8*)r29 + 0x4);
    *(f32*)((u8*)r30 + 0x54) = f0;
    f0 = *(f32*)((u8*)r29 + 0x8);
    *(f32*)((u8*)r30 + 0x64) = f0;
    return;
    f0 = *(f32*)((u8*)r29 + 0x0);
    *(f32*)((u8*)r30 + 0x48) = f0;
    f0 = *(f32*)((u8*)r29 + 0x4);
    *(f32*)((u8*)r30 + 0x58) = f0;
    f0 = *(f32*)((u8*)r29 + 0x8);
    *(f32*)((u8*)r30 + 0x68) = f0;
    return;
    f0 = *(f32*)((u8*)r29 + 0x0);
    *(f32*)((u8*)r30 + 0x4C) = f0;
    f0 = *(f32*)((u8*)r29 + 0x4);
    *(f32*)((u8*)r30 + 0x5C) = f0;
    f0 = *(f32*)((u8*)r29 + 0x8);
    *(f32*)((u8*)r30 + 0x6C) = f0;
    return;
    f0 = *(f32*)((u8*)r29 + 0x0);
    *(f32*)((u8*)r30 + 0x50) = f0;
    f0 = *(f32*)((u8*)r29 + 0x4);
    *(f32*)((u8*)r30 + 0x60) = f0;
    f0 = *(f32*)((u8*)r29 + 0x8);
    *(f32*)((u8*)r30 + 0x70) = f0;
    return;
    tmp = *(u32*)((u8*)r30 + 0xC);
    if (tmp != 0) {
        r3 = *(u32*)((u8*)r30 + 0xC);
        r4 = r30 + 0x44;
        r5 = (u32)sp + 0x14;
        r3 = r3 + 0x44;
        fn_801A9DF0();
    } else {

        r3 = r30 + 0x44;
        r4 = (u32)sp + 0x14;
        fn_800A2D64();
    }

    if (r27 == 0x36 && r27 != 0x38) {

        r3 = (u32)sp + 0x14;
        r4 = r30 + 0x38;
        HSD_MtxGetTranslate();
    }

    if (r27 == 0x36 && r27 != 0x37) {

        r3 = (u32)sp + 0x14;
        r4 = r30 + 0x1c;
        fn_801A98CC();
    }
    if (r27 != 0x36) {
        if (r27 != 0x39) return;
    }
    r3 = (u32)sp + 0x14;
    r4 = r30 + 0x2c;
    fn_801A8D1C();

    return;
}

/* 0x801A2B5C | 0x4C0 */
void fn_801A2B5C(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void __assert();
    extern void HSD_DObjAddAnimAll();
    extern void fn_8019D620();
    extern void fn_8019FAEC();
    extern void fn_8019FE8C();
    extern void fn_801A2B5C();
    extern void fn_801A301C();
    extern void fn_801A323C();
    extern void fn_801AFE68();
    extern void fn_801C25E4();
    extern void fn_801C2670();
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

    /* mr. r31, r3 */;
    r30 = r4;
    r29 = r5;
    r28 = r6;
    if ((s32)tmp == 0) return;
    do {
    if (r31 == 0) break;

    if (r30 != 0) {
        tmp = *(u32*)((u8*)r31 + 0x7C);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0x7C);
            fn_801C25E4();
        }
        r3 = *(u32*)((u8*)r30 + 0x8);
        fn_801C2670();
        *(u32*)((u8*)r31 + 0x7C) = r3;
        r6 = *(u32*)((u8*)r31 + 0x7C);
        do {
        if (r6 == 0) break;

        tmp = *(u32*)((u8*)r6 + 0x14);
        if (tmp == 0) break;

        r5 = r6 + 0x14;
        while (1) {
            tmp = *(u32*)((u8*)r5 + 0x0);
            if (tmp == 0) break;
            r3 = *(u32*)((u8*)r5 + 0x0);
            tmp = *(u8*)((u8*)r3 + 0x13);
            if (tmp == 0xc) {
                r3 = *(u32*)((u8*)r5 + 0x0);
                r4 = *(u32*)((u8*)r5 + 0x0);
                tmp = *(u32*)((u8*)r3 + 0x0);
                *(u32*)((u8*)r5 + 0x0) = tmp;
                tmp = *(u32*)((u8*)r6 + 0x14);
                *(u32*)((u8*)r4 + 0x0) = tmp;
                *(u32*)((u8*)r6 + 0x14) = r4;
                break;
            }
            r5 = *(u32*)((u8*)r5 + 0x0);

        }
        } while (0);

        r3 = *(u32*)((u8*)r31 + 0x80);
        r4 = *(u32*)((u8*)r30 + 0xC);
        fn_801AFE68();
        tmp = *(u32*)((u8*)r30 + 0x10);
        tmp = tmp & 0x1;
        if (tmp != 0) {
            if (r31 != 0) {
                tmp = *(u32*)((u8*)r31 + 0x14);
                tmp = tmp ^ 0x8;
                tmp = tmp & 0x00000008;
                if (r31 != 0) {
                    if (r31 != 0) {
                        if (r31 == 0) {
                            r3 = (u32)lbl_8047DB34;
                            r4 = 0x25d;
                            r5 = (u32)lbl_8047DB3C;
                            __assert();
                        }
                        tmp = *(u32*)((u8*)r31 + 0x14);
                        r3 = 0x0;
                        tmp = tmp & 0x00800000;
                        if (r31 == 0) {
                            tmp = *(u32*)((u8*)r31 + 0x14);
                            tmp = tmp & 0x00000040;
                            if (r31 != 0) {
                                r3 = 0x1;
                        }
                        }
                        if ((s32)r3 == 0) {
                            r3 = r31;
                            fn_8019D620();
                }
                }
                }
                tmp = *(u32*)((u8*)r31 + 0x14);
                tmp = tmp | 0x8;
                *(u32*)((u8*)r31 + 0x14) = tmp;
            }
            goto L_801A2D14;
        }
        if (r31 != 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp ^ 0x8;
            tmp = tmp & 0x00000008;
            if (r31 != 0) {
                if (r31 != 0) {
                    if (r31 == 0) {
                        r3 = (u32)lbl_8047DB34;
                        r4 = 0x25d;
                        r5 = (u32)lbl_8047DB3C;
                        __assert();
                    }
                    tmp = *(u32*)((u8*)r31 + 0x14);
                    r3 = 0x0;
                    tmp = tmp & 0x00800000;
                    if (r31 == 0) {
                        tmp = *(u32*)((u8*)r31 + 0x14);
                        tmp = tmp & 0x00000040;
                        if (r31 != 0) {
                            r3 = 0x1;
                    }
                    }
                    if ((s32)r3 == 0) {
                        r3 = r31;
                        fn_8019D620();
            }
            }
            }
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0xFFFFFFF7;
            *(u32*)((u8*)r31 + 0x14) = tmp;
        }
    }
L_801A2D14:
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x4020;
    tmp = __cntlzw(tmp);
    /* srwi. tmp, tmp, 5 */;
    if ((s32)r3 == 0) break;

    if (r28 != 0) {
        r5 = *(u32*)((u8*)r28 + 0x8);
    } else {

        r5 = 0x0;
    }
    r3 = *(u32*)((u8*)r31 + 0x18);
    if (r29 != 0) {
        r4 = *(u32*)((u8*)r29 + 0x8);
    } else {

        r4 = 0x0;
    }
    HSD_DObjAddAnimAll();
    } while (0);

    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x00001000;
    if (r29 != 0) return;
    r31 = *(u32*)((u8*)r31 + 0x10);
    if (r30 != 0) {
        r30 = *(u32*)((u8*)r30 + 0x0);
    } else {

        r30 = 0x0;
    }
    if (r29 != 0) {
        r29 = *(u32*)((u8*)r29 + 0x0);
    } else {

        r29 = 0x0;
    }
    if (r28 != 0) {
        r28 = *(u32*)((u8*)r28 + 0x0);
        goto L_801A3000;
    }
    r28 = 0x0;
    goto L_801A3000;
while (1) {
        do {
        if (r31 == 0) break;

        do {
        if (r31 == 0) break;

        if (r30 != 0) {
            tmp = *(u32*)((u8*)r31 + 0x7C);
            if (tmp != 0) {
                r3 = *(u32*)((u8*)r31 + 0x7C);
                fn_801C25E4();
            }
            r3 = *(u32*)((u8*)r30 + 0x8);
            fn_801C2670();
            *(u32*)((u8*)r31 + 0x7C) = r3;
            r3 = *(u32*)((u8*)r31 + 0x7C);
            fn_801A323C();
            r3 = *(u32*)((u8*)r31 + 0x80);
            r4 = *(u32*)((u8*)r30 + 0xC);
            fn_801AFE68();
            tmp = *(u32*)((u8*)r30 + 0x10);
            tmp = tmp & 0x1;
            if (tmp != 0) {
                r3 = r31;
                r4 = 0x8;
                fn_8019FE8C();
                goto L_801A2E1C;
            }
            r3 = r31;
            r4 = 0x8;
            fn_8019FAEC();
        }
    L_801A2E1C:
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x4020;
        tmp = __cntlzw(tmp);
        /* srwi. tmp, tmp, 5 */;
        if (tmp == 0) break;

        if (r28 != 0) {
            r5 = *(u32*)((u8*)r28 + 0x8);
        } else {

            r5 = 0x0;
        }
        r3 = *(u32*)((u8*)r31 + 0x18);
        if (r29 != 0) {
            r4 = *(u32*)((u8*)r29 + 0x8);
        } else {

            r4 = 0x0;
        }
        HSD_DObjAddAnimAll();
        } while (0);

        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00001000;
        if (r29 != 0) break;

        r23 = *(u32*)((u8*)r31 + 0x10);
        if (r30 != 0) {
            r22 = *(u32*)((u8*)r30 + 0x0);
        } else {

            r22 = 0x0;
        }
        if (r29 != 0) {
            r21 = *(u32*)((u8*)r29 + 0x0);
        } else {

            r21 = 0x0;
        }
        if (r28 != 0) {
            r20 = *(u32*)((u8*)r28 + 0x0);
            if (r23 != 0) continue;
        break;
    }
        } while (0);

        r31 = *(u32*)((u8*)r31 + 0x8);
        if (r30 != 0) {
            r30 = *(u32*)((u8*)r30 + 0x4);
        } else {

            r30 = 0x0;
        }
        if (r29 != 0) {
            r29 = *(u32*)((u8*)r29 + 0x4);
        } else {

            r29 = 0x0;
        }
        if (r28 != 0) {
            r28 = *(u32*)((u8*)r28 + 0x4);

        } else {
            r28 = 0x0;
        }
    L_801A3000:
        if (r31 != 0) continue;
    break;
}

    return;
}

/* 0x801A301C | 0x220 */
void fn_801A301C(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void __assert();
    extern void HSD_DObjAddAnimAll();
    extern void fn_8019D620();
    extern void fn_801AFE68();
    extern void fn_801C25E4();
    extern void fn_801C2670();
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

    r31 = r6;
    r30 = r5;
    /* mr. r29, r3 */;
    r28 = r4;
    if ((s32)tmp == 0) return;
    if (r28 != 0) {
        tmp = *(u32*)((u8*)r29 + 0x7C);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r29 + 0x7C);
            fn_801C25E4();
        }
        r3 = *(u32*)((u8*)r28 + 0x8);
        fn_801C2670();
        *(u32*)((u8*)r29 + 0x7C) = r3;
        r6 = *(u32*)((u8*)r29 + 0x7C);
        do {
        if (r6 == 0) break;

        tmp = *(u32*)((u8*)r6 + 0x14);
        if (tmp == 0) break;

        r5 = r6 + 0x14;
        while (1) {
            tmp = *(u32*)((u8*)r5 + 0x0);
            if (tmp == 0) break;
            r3 = *(u32*)((u8*)r5 + 0x0);
            tmp = *(u8*)((u8*)r3 + 0x13);
            if (tmp == 0xc) {
                r3 = *(u32*)((u8*)r5 + 0x0);
                r4 = *(u32*)((u8*)r5 + 0x0);
                tmp = *(u32*)((u8*)r3 + 0x0);
                *(u32*)((u8*)r5 + 0x0) = tmp;
                tmp = *(u32*)((u8*)r6 + 0x14);
                *(u32*)((u8*)r4 + 0x0) = tmp;
                *(u32*)((u8*)r6 + 0x14) = r4;
                break;
            }
            r5 = *(u32*)((u8*)r5 + 0x0);

        }
        } while (0);

        r3 = *(u32*)((u8*)r29 + 0x80);
        r4 = *(u32*)((u8*)r28 + 0xC);
        fn_801AFE68();
        tmp = *(u32*)((u8*)r28 + 0x10);
        tmp = tmp & 0x1;
        if (tmp != 0) {
            if (r29 != 0) {
                tmp = *(u32*)((u8*)r29 + 0x14);
                tmp = tmp ^ 0x8;
                tmp = tmp & 0x00000008;
                if (r29 != 0) {
                    if (r29 != 0) {
                        if (r29 == 0) {
                            r3 = (u32)lbl_8047DB34;
                            r4 = 0x25d;
                            r5 = (u32)lbl_8047DB3C;
                            __assert();
                        }
                        tmp = *(u32*)((u8*)r29 + 0x14);
                        r3 = 0x0;
                        tmp = tmp & 0x00800000;
                        if (r29 == 0) {
                            tmp = *(u32*)((u8*)r29 + 0x14);
                            tmp = tmp & 0x00000040;
                            if (r29 != 0) {
                                r3 = 0x1;
                        }
                        }
                        if ((s32)r3 == 0) {
                            r3 = r29;
                            fn_8019D620();
                }
                }
                }
                tmp = *(u32*)((u8*)r29 + 0x14);
                tmp = tmp | 0x8;
                *(u32*)((u8*)r29 + 0x14) = tmp;
            }
            goto L_801A31D8;
        }
        if (r29 != 0) {
            tmp = *(u32*)((u8*)r29 + 0x14);
            tmp = tmp ^ 0x8;
            tmp = tmp & 0x00000008;
            if (r29 != 0) {
                if (r29 != 0) {
                    if (r29 == 0) {
                        r3 = (u32)lbl_8047DB34;
                        r4 = 0x25d;
                        r5 = (u32)lbl_8047DB3C;
                        __assert();
                    }
                    tmp = *(u32*)((u8*)r29 + 0x14);
                    r3 = 0x0;
                    tmp = tmp & 0x00800000;
                    if (r29 == 0) {
                        tmp = *(u32*)((u8*)r29 + 0x14);
                        tmp = tmp & 0x00000040;
                        if (r29 != 0) {
                            r3 = 0x1;
                    }
                    }
                    if ((s32)r3 == 0) {
                        r3 = r29;
                        fn_8019D620();
            }
            }
            }
            tmp = *(u32*)((u8*)r29 + 0x14);
            tmp = tmp & 0xFFFFFFF7;
            *(u32*)((u8*)r29 + 0x14) = tmp;
        }
    }
L_801A31D8:
    tmp = *(u32*)((u8*)r29 + 0x14);
    tmp = tmp & 0x4020;
    tmp = __cntlzw(tmp);
    /* srwi. tmp, tmp, 5 */;
    if ((s32)r3 == 0) return;
    if (r31 != 0) {
        r5 = *(u32*)((u8*)r31 + 0x8);
    } else {

        r5 = 0x0;
    }
    r3 = *(u32*)((u8*)r29 + 0x18);
    if (r30 != 0) {
        r4 = *(u32*)((u8*)r30 + 0x8);
    } else {

        r4 = 0x0;
    }
    HSD_DObjAddAnimAll();

    return;
}

/* 0x64 | fn_801A323C | generic */
void fn_801A323C(u32 arg1) {

}

/* 0x801A32A0 | 0x2D4 */
void fn_801A32A0(void) {
    extern void HSD_DObjReqAnimAllByFlags();
    extern void fn_801A32A0();
    extern void fn_801A3574();
    extern void fn_801AFF64();
    extern void fn_801C29C4();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;

    f31 = f1;
    /* mr. r25, r3 */;
    r28 = r4;
    if ((s32)tmp == 0) return;
    if (r25 != 0) {
        tmp = r28 & 0x1;
        if (r25 != 0) {
            f1 = f31;
            r3 = *(u32*)((u8*)r25 + 0x7C);
            fn_801C29C4();
        }
        tmp = *(u32*)((u8*)r25 + 0x14);
        tmp = tmp & 0x4020;
        tmp = __cntlzw(tmp);
        /* srwi. tmp, tmp, 5 */;
        if (r25 != 0) {
            f1 = f31;
            r3 = *(u32*)((u8*)r25 + 0x18);
            r4 = r28;
            HSD_DObjReqAnimAllByFlags();
        }
        f1 = f31;
        r3 = *(u32*)((u8*)r25 + 0x80);
        r4 = r28;
        fn_801AFF64();
    }
    tmp = *(u32*)((u8*)r25 + 0x14);
    tmp = tmp & 0x00001000;
    if (r25 != 0) return;
    r29 = *(u32*)((u8*)r25 + 0x10);
    while (1) {
        if (r29 == 0) break;
        do {
        if (r29 == 0) break;

        if (r29 != 0) {
            tmp = r28 & 0x1;
            if (r29 != 0) {
                f1 = f31;
                r3 = *(u32*)((u8*)r29 + 0x7C);
                fn_801C29C4();
            }
            tmp = *(u32*)((u8*)r29 + 0x14);
            tmp = tmp & 0x4020;
            tmp = __cntlzw(tmp);
            /* srwi. tmp, tmp, 5 */;
            if (r29 != 0) {
                f1 = f31;
                r3 = *(u32*)((u8*)r29 + 0x18);
                r4 = r28;
                HSD_DObjReqAnimAllByFlags();
            }
            f1 = f31;
            r3 = *(u32*)((u8*)r29 + 0x80);
            r4 = r28;
            fn_801AFF64();
        }
        tmp = *(u32*)((u8*)r29 + 0x14);
        tmp = tmp & 0x00001000;
        if (r29 != 0) break;

        r31 = *(u32*)((u8*)r29 + 0x10);
        while (1) {
            if (r31 == 0) break;
            do {
            if (r31 == 0) break;

            if (r31 != 0) {
                tmp = r28 & 0x1;
                if (r31 != 0) {
                    f1 = f31;
                    r3 = *(u32*)((u8*)r31 + 0x7C);
                    fn_801C29C4();
                }
                tmp = *(u32*)((u8*)r31 + 0x14);
                tmp = tmp & 0x4020;
                tmp = __cntlzw(tmp);
                /* srwi. tmp, tmp, 5 */;
                if (r31 != 0) {
                    f1 = f31;
                    r3 = *(u32*)((u8*)r31 + 0x18);
                    r4 = r28;
                    HSD_DObjReqAnimAllByFlags();
                }
                f1 = f31;
                r3 = *(u32*)((u8*)r31 + 0x80);
                r4 = r28;
                fn_801AFF64();
            }
            tmp = *(u32*)((u8*)r31 + 0x14);
            tmp = tmp & 0x00001000;
            if (r31 != 0) break;

            r30 = *(u32*)((u8*)r31 + 0x10);
            while (1) {
                if (r30 == 0) break;
                do {
                if (r30 == 0) break;

                if (r30 != 0) {
                    tmp = r28 & 0x1;
                    if (r30 != 0) {
                        f1 = f31;
                        r3 = *(u32*)((u8*)r30 + 0x7C);
                        fn_801C29C4();
                    }
                    tmp = *(u32*)((u8*)r30 + 0x14);
                    tmp = tmp & 0x4020;
                    tmp = __cntlzw(tmp);
                    /* srwi. tmp, tmp, 5 */;
                    if (r30 != 0) {
                        f1 = f31;
                        r3 = *(u32*)((u8*)r30 + 0x18);
                        r4 = r28;
                        HSD_DObjReqAnimAllByFlags();
                    }
                    f1 = f31;
                    r3 = *(u32*)((u8*)r30 + 0x80);
                    r4 = r28;
                    fn_801AFF64();
                }
                tmp = *(u32*)((u8*)r30 + 0x14);
                tmp = tmp & 0x00001000;
                if (r30 != 0) break;

                r27 = *(u32*)((u8*)r30 + 0x10);
                while (r27 != 0) {

                    if (r27 != 0) {
                        if (r27 != 0) {
                            tmp = r28 & 0x1;
                            if (r27 != 0) {
                                f1 = f31;
                                r3 = *(u32*)((u8*)r27 + 0x7C);
                                fn_801C29C4();
                            }
                            tmp = *(u32*)((u8*)r27 + 0x14);
                            tmp = tmp & 0x4020;
                            tmp = __cntlzw(tmp);
                            /* srwi. tmp, tmp, 5 */;
                            if (r27 != 0) {
                                f1 = f31;
                                r3 = *(u32*)((u8*)r27 + 0x18);
                                r4 = r28;
                                HSD_DObjReqAnimAllByFlags();
                            }
                            f1 = f31;
                            r3 = *(u32*)((u8*)r27 + 0x80);
                            r4 = r28;
                            fn_801AFF64();
                        }
                        tmp = *(u32*)((u8*)r27 + 0x14);
                        tmp = tmp & 0x00001000;
                        if (r27 == 0) {
                            r25 = *(u32*)((u8*)r27 + 0x10);
                            while (r25 != 0) {

                                if (r25 != 0) {
                                    f1 = f31;
                                    r3 = r25;
                                    r4 = r28;
                                    fn_801A3574();
                                    tmp = *(u32*)((u8*)r25 + 0x14);
                                    tmp = tmp & 0x00001000;
                                    if (r25 == 0) {
                                        r26 = *(u32*)((u8*)r25 + 0x10);
                                        while (r26 != 0) {

                                            f1 = f31;
                                            r3 = r26;
                                            r4 = r28;
                                            fn_801A32A0();
                                            r26 = *(u32*)((u8*)r26 + 0x8);

                                        }
                                }
                                }
                                r25 = *(u32*)((u8*)r25 + 0x8);

                            }
                    }
                    }
                    r27 = *(u32*)((u8*)r27 + 0x8);

                }
                } while (0);

                r30 = *(u32*)((u8*)r30 + 0x8);

            }
            } while (0);

            r31 = *(u32*)((u8*)r31 + 0x8);

        }
        } while (0);

        r29 = *(u32*)((u8*)r29 + 0x8);

    }

    return;
}

/* 0x801A3574 | 0x8C */
void fn_801A3574(void) {
    extern void HSD_DObjReqAnimAllByFlags();
    extern void fn_801AFF64();
    extern void fn_801C29C4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;

    f31 = f1;
    r31 = r4;
    /* mr. r30, r3 */;
    if ((s32)tmp != 0) {
        tmp = r31 & 0x1;
        if ((s32)tmp != 0) {
            f1 = f31;
            r3 = *(u32*)((u8*)r30 + 0x7C);
            fn_801C29C4();
        }
        tmp = *(u32*)((u8*)r30 + 0x14);
        tmp = tmp & 0x4020;
        tmp = __cntlzw(tmp);
        /* srwi. tmp, tmp, 5 */;
        if ((s32)tmp != 0) {
            f1 = f31;
            r3 = *(u32*)((u8*)r30 + 0x18);
            r4 = r31;
            HSD_DObjReqAnimAllByFlags();
        }
        f1 = f31;
        r3 = *(u32*)((u8*)r30 + 0x80);
        r4 = r31;
        fn_801AFF64();
    }
    return;
}

/* 0x801A3600 | 0x318 */
void fn_801A3600(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void fn_800A2D98();
    extern void fn_800A37CC();
    extern void __assert();
    extern void fn_8019D9DC();
    extern void HSD_VecFree();
    extern void HSD_VecAlloc();
    extern void fn_801A86B4();
    extern void fn_801A8884();
    u8 sp[0x20];
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

    r31 = r3;
    r30 = *(u32*)((u8*)r3 + 0xC);
    if (r30 != 0) {
        if (r30 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r30 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r30 == 0) {
            tmp = *(u32*)((u8*)r30 + 0x14);
            tmp = tmp & 0x00000040;
            if (r30 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) {
            r3 = r30;
            fn_8019D9DC();
    }
    }
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x00000008;
    if ((s32)r3 != 0) {
        tmp = *(u32*)((u8*)r31 + 0xC);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r3 + 0x74);
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x74);
                if (tmp == 0) {
                    HSD_VecAlloc();
                    *(u32*)((u8*)r31 + 0x74) = r3;
                }
                r3 = *(u32*)((u8*)r31 + 0xC);
                r4 = *(u32*)((u8*)r31 + 0x74);
                r5 = *(u32*)((u8*)r3 + 0x74);
                r3 = *(u32*)((u8*)r5 + 0x0);
                tmp = *(u32*)((u8*)r5 + 0x4);
                *(u32*)((u8*)r4 + 0x0) = r3;
                *(u32*)((u8*)r4 + 0x4) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x8);
                *(u32*)((u8*)r4 + 0x8) = tmp;
                goto L_801A3794;
            }
            }
        tmp = *(u32*)((u8*)r31 + 0x74);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0x74);
            HSD_VecFree();
            tmp = 0x0;
            *(u32*)((u8*)r31 + 0x74) = tmp;
        }

    } else {
        tmp = *(u32*)((u8*)r31 + 0x74);
        if (tmp == 0) {
            HSD_VecAlloc();
            *(u32*)((u8*)r31 + 0x74) = r3;
        }
        tmp = *(u32*)((u8*)r31 + 0xC);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r3 + 0x74);
            if (tmp != 0) {
                r3 = *(u32*)((u8*)r31 + 0xC);
                f1 = *(f32*)((u8*)r31 + 0x2C);
                r4 = *(u32*)((u8*)r3 + 0x74);
                r3 = *(u32*)((u8*)r31 + 0x74);
                f0 = *(f32*)((u8*)r4 + 0x0);
                f0 = f1 * f0;
                *(f32*)((u8*)r3 + 0x0) = f0;
                r3 = *(u32*)((u8*)r31 + 0xC);
                f1 = *(f32*)((u8*)r31 + 0x30);
                r4 = *(u32*)((u8*)r3 + 0x74);
                r3 = *(u32*)((u8*)r31 + 0x74);
                f0 = *(f32*)((u8*)r4 + 0x4);
                f0 = f1 * f0;
                *(f32*)((u8*)r3 + 0x4) = f0;
                r3 = *(u32*)((u8*)r31 + 0xC);
                f1 = *(f32*)((u8*)r31 + 0x34);
                r4 = *(u32*)((u8*)r3 + 0x74);
                r3 = *(u32*)((u8*)r31 + 0x74);
                f0 = *(f32*)((u8*)r4 + 0x8);
                f0 = f1 * f0;
                *(f32*)((u8*)r3 + 0x8) = f0;
                goto L_801A3794;
            }
            }
        r4 = *(u32*)((u8*)r31 + 0x74);
        r3 = *(u32*)((u8*)r31 + 0x2C);
        tmp = *(u32*)((u8*)r31 + 0x30);
        *(u32*)((u8*)r4 + 0x0) = r3;
        *(u32*)((u8*)r4 + 0x4) = tmp;
        tmp = *(u32*)((u8*)r31 + 0x34);
        *(u32*)((u8*)r4 + 0x8) = tmp;
    }
L_801A3794:
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x00020000;
    if (tmp != 0) {
        tmp = *(u32*)((u8*)r31 + 0xC);
        r3 = r31 + 0x44;
        r4 = r31 + 0x2c;
        r5 = r31 + 0x1c;
        r6 = r31 + 0x38;
        r8 = 0x0;
        if (tmp != 0) {
            r7 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r7 + 0x74);
            if (tmp != 0) {
                r8 = 0x1;
        }
        }
        if ((s32)r8 != 0) {
            r7 = *(u32*)((u8*)r31 + 0xC);
            r7 = *(u32*)((u8*)r7 + 0x74);
        } else {

            r7 = 0x0;
        }
        fn_801A86B4();
    } else {

        tmp = *(u32*)((u8*)r31 + 0xC);
        r3 = r31 + 0x44;
        r4 = r31 + 0x2c;
        r5 = r31 + 0x1c;
        r6 = r31 + 0x38;
        r8 = 0x0;
        if (tmp != 0) {
            r7 = *(u32*)((u8*)r31 + 0xC);
            tmp = *(u32*)((u8*)r7 + 0x74);
            if (tmp != 0) {
                r8 = 0x1;
        }
        }
        if ((s32)r8 != 0) {
            r7 = *(u32*)((u8*)r31 + 0xC);
            r7 = *(u32*)((u8*)r7 + 0x74);
        } else {

            r7 = 0x0;
        }
        fn_801A8884();
    }
    tmp = *(u32*)((u8*)r31 + 0xC);
    if (tmp != 0) {
        r3 = *(u32*)((u8*)r31 + 0xC);
        r4 = r31 + 0x44;
        r5 = r31 + 0x44;
        r3 = r3 + 0x44;
        fn_800A2D98();
    }
    tmp = *(u32*)((u8*)r31 + 0x7C);
    if (tmp == 0) return;
    r3 = *(u32*)((u8*)r31 + 0x7C);
    tmp = *(u32*)((u8*)r3 + 0x18);
    if (tmp == 0) return;
    r3 = *(u32*)((u8*)r31 + 0x7C);
    r30 = *(u32*)((u8*)r3 + 0x18);
    if (r30 != 0) {
        if (r30 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r30 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r30 == 0) {
            tmp = *(u32*)((u8*)r30 + 0x14);
            tmp = tmp & 0x00000040;
            if (r30 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) {
            r3 = r30;
            fn_8019D9DC();
    }
    }
    r3 = r30 + 0x44;
    r4 = r31 + 0x38;
    r5 = (u32)sp + 0x8;
    fn_800A37CC();
    f0 = *(f32*)(sp + 0x8);
    *(f32*)((u8*)r31 + 0x50) = f0;
    f0 = *(f32*)(sp + 0xC);
    *(f32*)((u8*)r31 + 0x60) = f0;
    f0 = *(f32*)(sp + 0x10);
    *(f32*)((u8*)r31 + 0x70) = f0;

    return;
}

/* 0x801A3918 | 0x94 */
void fn_801A3918(void) {
    extern void fn_801A39AC();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r12 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    /* mr. r31, r3 */;
    r30 = r5;
    r29 = r4;
    if ((s32)tmp != 0) {
        if (r29 != 0) {
            r12 = r29;
            r3 = r31;
            r4 = r30;
            r5 = 0x0;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00001000;
        if (r29 == 0) {
            r31 = *(u32*)((u8*)r31 + 0x10);
            while (r31 != 0) {

                r3 = r31;
                r4 = r29;
                r5 = r30;
                fn_801A39AC();
                r31 = *(u32*)((u8*)r31 + 0x8);

            }
    }
    }
    return;
}

/* 0x801A39AC | 0x358 */
void fn_801A39AC(void) {
    extern u8 lbl_80274D44[];
    extern u8 lbl_8047DB20[];
    extern void __assert();
    extern void fn_801A39AC();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r12 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    /* mr. r24, r3 */;
    r31 = r4;
    r30 = r5;
    if ((s32)tmp == 0) return;
    tmp = *(u32*)((u8*)r24 + 0xC);
    if (tmp == 0) {
        r4 = (u32)lbl_80274D44;
        r3 = (u32)lbl_8047DB20;
        r5 = (u32)lbl_80274D44;
        r4 = 0xad;
        __assert();
    }
    r3 = *(u32*)((u8*)r24 + 0xC);
    tmp = 0x2;
    r3 = *(u32*)((u8*)r3 + 0x10);
    r4 = r3 - r24;
    r3 = r24 - r3;
    r3 = ~(r4 | r3);
    r3 = (s32)r3 >> 31;
    tmp = r3 + tmp;
    r5 = tmp;
    if (r31 != 0) {
        r12 = r31;
        r3 = r24;
        r4 = r30;
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
    }
    tmp = *(u32*)((u8*)r24 + 0x14);
    tmp = tmp & 0x00001000;
    if (r31 != 0) return;
    r28 = *(u32*)((u8*)r24 + 0x10);
    while (1) {
        if (r28 == 0) break;
        do {
        if (r28 == 0) break;

        tmp = *(u32*)((u8*)r28 + 0xC);
        if (tmp == 0) {
            r4 = (u32)lbl_80274D44;
            r3 = (u32)lbl_8047DB20;
            r5 = (u32)lbl_80274D44;
            r4 = 0xad;
            __assert();
        }
        r3 = *(u32*)((u8*)r28 + 0xC);
        r5 = 0x2;
        tmp = *(u32*)((u8*)r3 + 0x10);
        r3 = tmp - r28;
        tmp = r28 - tmp;
        tmp = ~(r3 | tmp);
        tmp = (s32)tmp >> 31;
        r5 = tmp + r5;
        if (r31 != 0) {
            r12 = r31;
            r3 = r28;
            r4 = r30;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)((u8*)r28 + 0x14);
        tmp = tmp & 0x00001000;
        if (r31 != 0) break;

        r29 = *(u32*)((u8*)r28 + 0x10);
        while (1) {
            if (r29 == 0) break;
            do {
            if (r29 == 0) break;

            tmp = *(u32*)((u8*)r29 + 0xC);
            if (tmp == 0) {
                r4 = (u32)lbl_80274D44;
                r3 = (u32)lbl_8047DB20;
                r5 = (u32)lbl_80274D44;
                r4 = 0xad;
                __assert();
            }
            r3 = *(u32*)((u8*)r29 + 0xC);
            r5 = 0x2;
            tmp = *(u32*)((u8*)r3 + 0x10);
            r3 = tmp - r29;
            tmp = r29 - tmp;
            tmp = ~(r3 | tmp);
            tmp = (s32)tmp >> 31;
            r5 = tmp + r5;
            if (r31 != 0) {
                r12 = r31;
                r3 = r29;
                r4 = r30;
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)((u8*)r29 + 0x14);
            tmp = tmp & 0x00001000;
            if (r31 != 0) break;

            r27 = *(u32*)((u8*)r29 + 0x10);
            while (1) {
                if (r27 == 0) break;
                do {
                if (r27 == 0) break;

                tmp = *(u32*)((u8*)r27 + 0xC);
                if (tmp == 0) {
                    r4 = (u32)lbl_80274D44;
                    r3 = (u32)lbl_8047DB20;
                    r5 = (u32)lbl_80274D44;
                    r4 = 0xad;
                    __assert();
                }
                r3 = *(u32*)((u8*)r27 + 0xC);
                r5 = 0x2;
                tmp = *(u32*)((u8*)r3 + 0x10);
                r3 = tmp - r27;
                tmp = r27 - tmp;
                tmp = ~(r3 | tmp);
                tmp = (s32)tmp >> 31;
                r5 = tmp + r5;
                if (r31 != 0) {
                    r12 = r31;
                    r3 = r27;
                    r4 = r30;
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                }
                tmp = *(u32*)((u8*)r27 + 0x14);
                tmp = tmp & 0x00001000;
                if (r31 != 0) break;

                r26 = *(u32*)((u8*)r27 + 0x10);
                while (r26 != 0) {

                    if (r26 != 0) {
                        tmp = *(u32*)((u8*)r26 + 0xC);
                        if (tmp == 0) {
                            r4 = (u32)lbl_80274D44;
                            r3 = (u32)lbl_8047DB20;
                            r5 = (u32)lbl_80274D44;
                            r4 = 0xad;
                            __assert();
                        }
                        r3 = *(u32*)((u8*)r26 + 0xC);
                        r5 = 0x2;
                        tmp = *(u32*)((u8*)r3 + 0x10);
                        r3 = tmp - r26;
                        tmp = r26 - tmp;
                        tmp = ~(r3 | tmp);
                        tmp = (s32)tmp >> 31;
                        r5 = tmp + r5;
                        if (r31 != 0) {
                            r12 = r31;
                            r3 = r26;
                            r4 = r30;
                            ctr_fn = (void(*)(void))r12;
                            ctr_fn();
                        }
                        tmp = *(u32*)((u8*)r26 + 0x14);
                        tmp = tmp & 0x00001000;
                        if (r31 == 0) {
                            r25 = *(u32*)((u8*)r26 + 0x10);
                            while (r25 != 0) {

                                if (r25 != 0) {
                                    tmp = *(u32*)((u8*)r25 + 0xC);
                                    if (tmp == 0) {
                                        r4 = (u32)lbl_80274D44;
                                        r3 = (u32)lbl_8047DB20;
                                        r5 = (u32)lbl_80274D44;
                                        r4 = 0xad;
                                        __assert();
                                    }
                                    r3 = *(u32*)((u8*)r25 + 0xC);
                                    r5 = 0x2;
                                    tmp = *(u32*)((u8*)r3 + 0x10);
                                    r3 = tmp - r25;
                                    tmp = r25 - tmp;
                                    tmp = ~(r3 | tmp);
                                    tmp = (s32)tmp >> 31;
                                    r5 = tmp + r5;
                                    if (r31 != 0) {
                                        r12 = r31;
                                        r3 = r25;
                                        r4 = r30;
                                        ctr_fn = (void(*)(void))r12;
                                        ctr_fn();
                                    }
                                    tmp = *(u32*)((u8*)r25 + 0x14);
                                    tmp = tmp & 0x00001000;
                                    if (r31 == 0) {
                                        r24 = *(u32*)((u8*)r25 + 0x10);
                                        while (r24 != 0) {

                                            r3 = r24;
                                            r4 = r31;
                                            r5 = r30;
                                            fn_801A39AC();
                                            r24 = *(u32*)((u8*)r24 + 0x8);

                                        }
                                }
                                }
                                r25 = *(u32*)((u8*)r25 + 0x8);

                            }
                    }
                    }
                    r26 = *(u32*)((u8*)r26 + 0x8);

                }
                } while (0);

                r27 = *(u32*)((u8*)r27 + 0x8);

            }
            } while (0);

            r29 = *(u32*)((u8*)r29 + 0x8);

        }
        } while (0);

        r28 = *(u32*)((u8*)r28 + 0x8);

    }

    return;
}

/* 0x801A3D04 | 0x160 */
void fn_801A3D04(void) {
    extern u8 lbl_8047DB34[];
    extern u8 lbl_8047DB3C[];
    extern void __assert();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)tmp == 0) return;
    if (r31 == 0) {
        r3 = (u32)lbl_8047DB34;
        r4 = 0x25d;
        r5 = (u32)lbl_8047DB3C;
        __assert();
    }
    tmp = *(u32*)((u8*)r31 + 0x14);
    r3 = 0x0;
    tmp = tmp & 0x00800000;
    if (r31 == 0) {
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x00000040;
        if (r31 != 0) {
            r3 = 0x1;
    }
    }
    if ((s32)r3 != 0) {
        return;
    }
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp & 0x00800000;
    if ((s32)r3 != 0) {
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp & 0x01000000;
        if ((s32)r3 != 0) return;
        tmp = *(u32*)((u8*)r31 + 0xC);
        if (tmp == 0) return;
        r30 = *(u32*)((u8*)r31 + 0xC);
        if (r30 == 0) {
            r3 = (u32)lbl_8047DB34;
            r4 = 0x25d;
            r5 = (u32)lbl_8047DB3C;
            __assert();
        }
        tmp = *(u32*)((u8*)r30 + 0x14);
        r3 = 0x0;
        tmp = tmp & 0x00800000;
        if (r30 == 0) {
            tmp = *(u32*)((u8*)r30 + 0x14);
            tmp = tmp & 0x00000040;
            if (r30 != 0) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 == 0) return;
        tmp = *(u32*)((u8*)r31 + 0x14);
        tmp = tmp | 0x40;
        *(u32*)((u8*)r31 + 0x14) = tmp;
        return;
    }
    tmp = *(u32*)((u8*)r31 + 0xC);
    if (tmp != 0) {
        r3 = *(u32*)((u8*)r31 + 0xC);
        tmp = *(u32*)((u8*)r3 + 0x14);
        tmp = tmp & 0x00000040;
        if (tmp == 0) {
        }
        tmp = *(u32*)((u8*)r31 + 0x14);
        r3 = tmp & 0x00600000;
        /* subis tmp, r3, 0x20 */;
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r31 + 0x14);
            r3 = tmp & 0x00600000;
            /* subis tmp, r3, 0x40 */;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r31 + 0x14);
                r3 = tmp & 0x00600000;
                /* subis tmp, r3, 0x60 */;
        }
        }
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r31 + 0x80);
            if (tmp == 0) return;
        }
        }
    tmp = *(u32*)((u8*)r31 + 0x14);
    tmp = tmp | 0x40;
    *(u32*)((u8*)r31 + 0x14) = tmp;

    return;
}

/* 0x50 | fn_801A3E64 | global_cond_call */
u32 fn_801A3E64(void) {
    /* uses lbl_804655B4 */
    if (0 /* field check */) { return 0; }
    HSD_ObjFree();
    return 0;
}

/* 0x801A3EB4 | 0x94 */
void fn_801A3EB4(void) {
    extern u8 lbl_804655B4[];
    extern u8 lbl_8047DBA0[];
    extern u8 lbl_8047DBA8[];
    extern u8 lbl_8047DBB0[];
    extern void __assert();
    extern void HSD_ObjAlloc();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = (u32)lbl_804655B4;
    r30 = r4;
    r29 = r3;
    r3 = (u32)lbl_804655B4;
    HSD_ObjAlloc();
    /* mr. r31, r3 */;
    if ((s32)tmp == 0) {
        r3 = (u32)lbl_8047DBA0;
        r4 = 0x4c;
        r5 = (u32)lbl_8047DBB0;
        __assert();
    }
    r3 = r31;
    r4 = 0x0;
    r5 = 0x8;
    memset((void*)r3, (int)r4, (u32)r5);
    *(u32*)((u8*)r31 + 0x4) = r30;
    if (r31 == 0) {
        r3 = (u32)lbl_8047DBA0;
        r4 = 0xca;
        r5 = (u32)lbl_8047DBA8;
        __assert();
    }
    *(u32*)((u8*)r31 + 0x0) = r29;
    r3 = r31;
    return;
}

/* 0x5C | fn_801A3F48 | multi_call_guarded */
void fn_801A3F48(void) {
    { HSD_ObjAlloc(); return; }
    __assert();
    memset();
}

/* 0x801A3FA4 | 0xC */
extern u8 lbl_80465588[];
extern u8 lbl_804655B4[];
void* fn_801A3FA4(void) {
    return lbl_80465588;
}

/* 0x801A3FB0 | 0xC */
void* fn_801A3FB0(void) {
    return lbl_804655B4;
}
