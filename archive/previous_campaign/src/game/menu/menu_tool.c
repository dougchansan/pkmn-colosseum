/**
 * @file menu_tool.c
 * @brief Menu tool functions (0x80072A00-0x80075818)
 *
 * Address range: 0x80072A00 - 0x8007581C
 * Total functions: 24
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_800060F0();
extern void fn_8008ABE4();
extern void fn_800A501C();
extern void fn_800A50E4();
extern void fn_800A541C();
extern void fn_800A7BCC();
extern void fn_800CE148();
extern void fn_800D0F44();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void fn_800D59B8();
extern void fn_800D5CB8();
extern void fn_800D61E4();
extern void fn_800D6728();
extern void fn_800D67BC();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D85D4();
extern void fn_800D888C();
extern void fn_800D88DC();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void fn_800E3DC4();
extern void _threadSwitch();
extern void fn_800FF56C();
extern void fn_80102568();
extern void fn_80102620();
extern void fn_801026A4();
extern void fn_80109934();
extern void fn_80109B90();
extern void fn_80109C88();
extern void fn_8010A420();
extern void fn_8010A5BC();
extern void fn_801240C4();
extern void fn_80135938();
extern void fn_801CB9D8();
extern void fn_801DAC3C();
extern void fn_8025F350();
extern void fn_8025F3F4();
extern void fn_8025F484();
extern void fn_8025F584();
extern void fn_8025F648();
extern void OSGetTick();
extern void OSReport();
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047A5D0;
extern u8 lbl_8047A604;
extern u8 lbl_8047A608;
extern u8 lbl_8047A60C;
extern u8 lbl_8047A610;
extern u8 lbl_8047C098;
extern u8 lbl_8047C09C;
extern u8 lbl_8047C0A0;
extern u8 lbl_8047C0A4;
extern u8 lbl_8047C0A8;
extern u8 lbl_8047C0AC;
extern u8 lbl_8047C0B0;
extern u8 lbl_8047C0B8;

/* ===== Rodata / data labels ===== */
extern u8 lbl_80268780[];
extern u8 lbl_802EF0A8[];
extern u8 lbl_80314F98[];
extern u8 lbl_803B6E08[];
extern u8 lbl_803B6E18[];
extern u8 lbl_803B6E40[];
extern u8 lbl_803D6E40[];

/* ===== Forward declarations ===== */
void fn_80072A00(void);
s32 fn_80072C74(s32, u32*);
void fn_80072D58(void);
s32 _AGB_EntryGetStatus__FlPUl(s32, u32*);
void fn_800730F8(void);
void fn_800733D0(void);
s32 fn_80073690(s32, s32);
void fn_80073700(void);
s32 fn_80073990(s32);
void fn_80073A44(void);
void fn_80073C38(void);
s32 fn_80073E84(void);
void fn_80073E8C(void);
s32 fn_80074324(s32);
void fn_80074360(void);
void fn_800745B4(void);
void fn_8007480C(void);
s32 fn_80075390(void);
void fn_800753D0(void);
void fn_80075518(s32, u8*);
s32 fn_80075638(void);
void fn_8007565C(void);
void fn_800756C8(void);
void fn_800757F0(void);

/* ===== Function implementations ===== */


/* 0x80072A00 | size: 0x274 */
void fn_80072A00(void) {
    extern void fn_80073C38();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r27 = r3;
    r29 = r27 + 0x1;
    r4 = 0x2;
    r3 = r29;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r27;
    fn_80073C38();
    if ((s32)r3 != 0x0) {
        r28 = r3;
    } else {
        r0 = 0x60;
        r3 = r27;
        *(u32*)(sp + 0x14) = r0;
        r4 = (u32)sp + 0x14;
        r5 = (u32)sp + 0xa;
        ((void(*)(void))fn_8025F648)();
        r28 = r3;
        if ((s32)r28 != 0x0) {
            r28 = 0xb;
        } else {
            r3 = (0x8000 << 16);
            r0 = *(u32*)((u8*)r3 + 0xF8);
            r0 = (u32)r0 >> 2;
            r20 = r0 * 0x3;
            OSGetTick();
            r5 = (u32)&lbl_803B6E18;
            r4 = (u32)&lbl_803B6E08;
            r6 = r27 << 3;
            r21 = r3;
            r0 = (u32)&lbl_803B6E18;
            r22 = r27 << 2;
            r30 = r0 + r6;
            r23 = (u32)&lbl_803B6E08;
            r31 = r30 + 0x4;
            r3 = (0x1062 << 16);
            r25 = (0x8000 << 16);
            r24 = r3 + 0x4dd3;
            while (1) {
                OSGetTick();
                r0 = r3 - r21;
                if (r0 > r20) {
                    r28 = 0x10;
                    break;
                }
                r0 = *(u32*)((u8*)r25 + 0xF8);
                r0 = (u32)r0 >> 2;
                r0 = (u32)((u64)r24 * (u64)r0 >> 32);
                r0 = (u32)r0 >> 6;
                r19 = r0 * 0x5;
                OSGetTick();
                r26 = r3;
                do {
                    OSGetTick();
                    r4 = r3 - r26;
                    r3 = r27;
                    r0 = r4 ^ r19;
                    r0 = __cntlzw(r0);
                    r0 = r4 << r0;
                    r18 = (u32)r0 >> 31;
                    fn_80073C38();
                } while ((s32)r3 == 0x1 && (s32)r18 == 0x0);
                if ((s32)r3 != 0x0) {
                    r18 = r3;
                } else {
                    r0 = 0xaa;
                    r3 = r27;
                    *(u32*)(sp + 0xC) = r0;
                    r4 = (u32)sp + 0xc;
                    r5 = (u32)sp + 0x9;
                    ((void(*)(void))fn_8025F648)();
                    if ((s32)r3 != 0x0) {
                        r18 = 0xb;
                    } else {
                        r4 = (0x8000 << 16);
                        r3 = (0x1062 << 16);
                        r0 = *(u32*)((u8*)r4 + 0xF8);
                        r3 = r3 + 0x4dd3;
                        r0 = (u32)r0 >> 2;
                        r0 = (u32)((u64)r3 * (u64)r0 >> 32);
                        r0 = (u32)r0 >> 6;
                        r19 = r0 * 0x64;
                        OSGetTick();
                        r26 = r3;
                        while (1) {
                            OSGetTick();
                            r0 = r3 - r26;
                            if (r0 > r19) {
                                r3 = 0x1;
                                break;
                            }
                            r3 = r27;
                            r4 = (u32)sp + 0x8;
                            ((void(*)(void))fn_8025F3F4)();
                            if ((s32)r3 != 0x0) {
                                r3 = 0x2;
                                break;
                            }
                            r0 = *(u8*)(sp + 0x8);
                            r0 = r0 & 0xa;
                            if ((s32)r0 == 0x8) {
                                r3 = r27;
                                r4 = (u32)sp + 0x10;
                                r5 = (u32)sp + 0x9;
                                ((void(*)(void))fn_8025F584)();
                                if ((s32)r3 != 0x0) {
                                    r3 = 0x3;
                                } else {
                                    r3 = 0x0;
                                }
                                break;
                            }
                            r12 = *(u32*)((u8*)r30 + 0x0);
                            if (r12 != 0x0) {
                                r3 = r27;
                                r4 = *(u32*)((u8*)r31 + 0x0);
                                ctr_fn = (void(*)(void))r12;
                                ctr_fn();
                            }
                            r0 = *(u32*)(r23 + r22);
                            if ((s32)r0 != 0x0) {
                                r3 = 0x3e8;
                                break;
                            }
                        }
                        if ((s32)r3 != 0x0) {
                            r18 = r3 + 0xb;
                        } else {
                            r18 = 0x0;
                        }
                    }
                    if ((s32)r18 == 0x0) {
                        r0 = (u32)r0 >> 24;
                        if (r0 != 0xaa) {
                            r18 = 0xf;
                        } else {
                            r18 = 0x0;
                        }
                    }
                }
                ((void(*)(void))_threadSwitch)();
                if ((s32)r18 == 0x0) {
                    r28 = 0x0;
                    break;
                }
            }
        }
    }
    r3 = r29;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r28;
    return;
}

/* 0x80072C74 | size: 0xE4 */
s32 fn_80072C74(s32 arg0, u32* arg1) {
    #pragma peephole off
    extern s32 fn_800D0F44(s32);
    extern s32 fn_8025F3F4(s32, void*);
    extern s32 fn_8025F584(s32, void*, void*);
    extern s32 GBAWrite(s32, void*, void*);
    u8 rbuf[4];
    u8 wbuf[4];
    u8 stat[4];
    s32 ret;

    if (fn_800D0F44(arg0) != 0x40000) {
        ret = 1;
    } else if (fn_8025F3F4(arg0, stat) != 0) {
        ret = 2;
    } else if ((stat[0] & 0x8) == 0) {
        *(u32*)wbuf = 0x11;
        GBAWrite(arg0, wbuf, stat);
        ret = -1;
    } else if (fn_8025F584(arg0, rbuf, stat) != 0) {
        ret = 3;
    } else {
        ret = 0;
        *arg1 = *(u32*)rbuf;
    }
    if (ret >= 0) {
        fn_8008ABE4(arg0 + 1, 1);
    }
    return ret;
}

/* 0x80072D58 | size: 0x2DC */
void fn_80072D58(void) {
    extern void fn_80073C38();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r21 = r3;
    r22 = r4;
    r23 = r5;
    r26 = r21 + 0x1;
    r4 = 0x2;
    r3 = r26;
    ((void(*)(void))fn_8008ABE4)();
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r21 << 3;
    r29 = r3;
    tmp = (u32)&lbl_803B6E18;
    r30 = r21 << 2;
    r27 = tmp + r6;
    r31 = (u32)&lbl_803B6E08;
    r28 = r27 + 0x4;
    do {
        OSGetTick();
        r4 = r3 - r29;
        r3 = r21;
        tmp = r4 ^ r25;
        tmp = __cntlzw(tmp);
        tmp = r4 << tmp;
        r24 = (u32)tmp >> 31;
        fn_80073C38();
        if ((s32)r3 != 0) {
            r15 = r3;
        } else {
            tmp = 0x66;
            r3 = r21;
            *(u32*)(sp + 0x10) = tmp;
            r4 = (u32)sp + 0x10;
            r5 = (u32)sp + 0xa;
            ((void(*)(void))fn_8025F648)();
            if ((s32)r3 != 0) {
                r15 = 0xb;
            } else {
                r4 = 0x80000000;
                r3 = 0x10620000;
                tmp = *(u32*)((u8*)r4 + 0xF8);
                r3 = r3 + 0x4dd3;
                tmp = (u32)tmp >> 2;
                tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                tmp = (u32)tmp >> 6;
                r16 = tmp * 0x64;
                OSGetTick();
                r18 = r3;
                while (1) {
                    OSGetTick();
                    tmp = r3 - r18;
                    if (tmp > r16) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r21;
                    r4 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8025F3F4)();
                    if ((s32)r3 != 0) {
                        r3 = 0x2;
                        break;
                    }
                    tmp = *(u8*)(sp + 0x8);
                    tmp = tmp & 0xa;
                    if ((s32)tmp == 8) {
                        r3 = r21;
                        r4 = (u32)sp + 0x14;
                        r5 = (u32)sp + 0xa;
                        ((void(*)(void))fn_8025F584)();
                        if ((s32)r3 != 0) {
                            r3 = 0x3;
                        } else {
                            r3 = 0x0;
                        }
                        break;
                    }
                    r12 = *(u32*)((u8*)r27 + 0x0);
                    if (r12 != 0) {
                        r3 = r21;
                        r4 = *(u32*)((u8*)r28 + 0x0);
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                    }
                    tmp = *(u32*)(r31 + r30);
                    if ((s32)tmp != 0) {
                        r3 = 0x3e8;
                        break;
                    }
                }
                if ((s32)r3 != 0) {
                    r15 = r3 + 0xb;
                } else {
                    r15 = 0x0;
                }
            }
            if ((s32)r15 == 0) {
                tmp = (u32)tmp >> 24;
                if (tmp != 0x66) {
                    r15 = 0xf;
                } else {
                    r17 = 0x0;
                    r3 = 0x10620000;
                    r15 = r22;
                    r18 = r3 + 0x4dd3;
                    r19 = 0x80000000;
                    while ((s32)r17 < (s32)r23) {
                        tmp = *(u32*)((u8*)r15 + 0x0);
                        r3 = r21;
                        r4 = (u32)sp + 0xc;
                        r5 = (u32)sp + 0x9;
                        *(u32*)(sp + 0xC) = tmp;
                        ((void(*)(void))fn_8025F648)();
                        if ((s32)r3 != 0) {
                            r15 = 0x10;
                            break;
                        }
                        tmp = *(u32*)((u8*)r19 + 0xF8);
                        tmp = (u32)tmp >> 2;
                        tmp = (u32)((u64)r18 * (u64)tmp >> 32);
                        tmp = (u32)tmp >> 6;
                        r16 = tmp * 0x64;
                        OSGetTick();
                        r20 = r3;
                        while (1) {
                            OSGetTick();
                            tmp = r3 - r20;
                            if (tmp > r16) {
                                r15 = 0x11;
                                break;
                            }
                            r3 = r21;
                            r4 = (u32)sp + 0x9;
                            ((void(*)(void))fn_8025F3F4)();
                            if ((s32)r3 != 0) {
                                r15 = 0x12;
                                break;
                            }
                            tmp = *(u8*)(sp + 0x9);
                            tmp = tmp & 0x00000002;
                            if ((s32)tmp == 0) {
                                r15 = 0x0;
                                break;
                            }
                        }
                        if ((s32)r15 != 0) {
                            break;
                        }
                        tmp = r17 << 26;
                        r3 = (u32)r17 >> 31;
                        tmp = tmp - r3;
                        /* rotlwi tmp, tmp, 6 */;
                        tmp = tmp + r3;
                        if ((s32)tmp == 0) {
                            ((void(*)(void))_threadSwitch)();
                        }
                        r17 = r17 + 0x4;
                        r15 = r15 + 0x4;
                    }
                    if ((s32)r17 >= (s32)r23 && (s32)r15 != 0x10 && (s32)r15 != 0x11 && (s32)r15 != 0x12) {
                        r15 = 0x0;
                    }
                }
            }
        }
    } while ((s32)r15 == 1 && (s32)r24 == 0);
    r3 = r26;
    if ((s32)r15 != 0) {
        r4 = 0x1;
    } else {

        r4 = 0x3;
    }
    ((void(*)(void))fn_8008ABE4)();
    r3 = r15;
    return;
}

/* 0x80073034 | size: 0xC4 */
s32 _AGB_EntryGetStatus__FlPUl(s32 arg0, u32* arg1) {
    #pragma peephole off
    extern s32 fn_800D0F44(s32);
    extern s32 fn_8025F3F4(s32, void*);
    extern s32 fn_8025F584(s32, void*, void*);
    extern s32 GBAWrite(s32, void*, void*);
    u8 rbuf[4];
    u8 wbuf[4];
    u8 stat[4];

    if (fn_800D0F44(arg0) != 0x40000) {
        return 1;
    }
    if (fn_8025F3F4(arg0, stat) != 0) {
        return 2;
    }
    if ((stat[0] & 0x8) == 0) {
        *(u32*)wbuf = 0x11;
        GBAWrite(arg0, wbuf, stat);
        return -1;
    }
    if (fn_8025F584(arg0, rbuf, stat) != 0) {
        return 3;
    }
    *arg1 = *(u32*)rbuf;
    return 0;
}

/* 0x800730F8 | size: 0x2D8 */
void fn_800730F8(void) {
    extern void fn_80073C38();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r12 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r22 = r3;
    r23 = r4;
    r25 = r22 + 0x1;
    r4 = 0x2;
    r3 = r25;
    ((void(*)(void))fn_8008ABE4)();
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r24 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)r23 >> 24;
    tmp = ((r23 << 24) | (r23 >> 8)) & 0x0000FF00;
    r7 = r22 << 3;
    r5 = (u32)&lbl_803B6E18;
    r6 = (u32)&lbl_803B6E08;
    r26 = r5 + r7;
    r5 = ((r23 << 8) | (r23 >> 24)) & 0x00FF0000;
    tmp = r4 | tmp;
    r4 = r23 << 24;
    tmp = r5 | tmp;
    r29 = r3;
    r28 = r26 + 0x4;
    r30 = r22 << 2;
    r31 = (u32)&lbl_803B6E08;
    r27 = r4 | tmp;
    do {
        OSGetTick();
        r4 = r3 - r29;
        r3 = r22;
        tmp = r4 ^ r24;
        tmp = __cntlzw(tmp);
        tmp = r4 << tmp;
        r23 = (u32)tmp >> 31;
        fn_80073C38();
        if ((s32)r3 != 0) {
            r20 = r3;
        } else {
            tmp = 0x77;
            r3 = r22;
            *(u32*)(sp + 0x14) = tmp;
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x9;
            *(u32*)(sp + 0xC) = tmp;
            ((void(*)(void))fn_8025F648)();
            if ((s32)r3 != 0) {
                r20 = 0xb;
            } else {
                r4 = 0x80000000;
                r3 = 0x10620000;
                tmp = *(u32*)((u8*)r4 + 0xF8);
                r3 = r3 + 0x4dd3;
                tmp = (u32)tmp >> 2;
                tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                tmp = (u32)tmp >> 6;
                r20 = tmp * 0x64;
                OSGetTick();
                r21 = r3;
                while (1) {
                    OSGetTick();
                    tmp = r3 - r21;
                    if (tmp > r20) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r22;
                    r4 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8025F3F4)();
                    if ((s32)r3 != 0) {
                        r3 = 0x2;
                        break;
                    }
                    tmp = *(u8*)(sp + 0x8);
                    tmp = tmp & 0xa;
                    if ((s32)tmp == 8) {
                        r3 = r22;
                        r4 = (u32)sp + 0x10;
                        r5 = (u32)sp + 0x9;
                        ((void(*)(void))fn_8025F584)();
                        if ((s32)r3 != 0) {
                            r3 = 0x3;
                        } else {
                            r3 = 0x0;
                        }
                        break;
                    }
                    r12 = *(u32*)((u8*)r26 + 0x0);
                    if (r12 != 0) {
                        r3 = r22;
                        r4 = *(u32*)((u8*)r28 + 0x0);
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                    }
                    tmp = *(u32*)(r31 + r30);
                    if ((s32)tmp != 0) {
                        r3 = 0x3e8;
                        break;
                    }
                }
                if ((s32)r3 != 0) {
                    r20 = r3 + 0xb;
                } else {
                    r20 = 0x0;
                }
            }
            if ((s32)r20 == 0) {
                tmp = (u32)tmp >> 24;
                if (tmp != 0x77) {
                    r20 = 0xf;
                } else {
                    r3 = r22;
                    r4 = (u32)sp + 0x14;
                    r5 = (u32)sp + 0xa;
                    ((void(*)(void))fn_8025F648)();
                    r4 = 0x80000000;
                    r3 = 0x10620000;
                    tmp = *(u32*)((u8*)r4 + 0xF8);
                    r3 = r3 + 0x4dd3;
                    tmp = (u32)tmp >> 2;
                    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                    tmp = (u32)tmp >> 6;
                    r20 = tmp * 0x64;
                    OSGetTick();
                    r21 = r3;
                    while (1) {
                        OSGetTick();
                        tmp = r3 - r21;
                        if (tmp > r20) {
                            r20 = 0x10;
                            break;
                        }
                        r3 = r22;
                        r4 = (u32)sp + 0xa;
                        ((void(*)(void))fn_8025F3F4)();
                        if ((s32)r3 != 0) {
                            r20 = 0x11;
                            break;
                        }
                        tmp = *(u8*)(sp + 0xA);
                        tmp = tmp & 0x00000002;
                        if ((s32)tmp == 0) {
                            r20 = 0x0;
                            break;
                        }
                        r12 = *(u32*)((u8*)r26 + 0x0);
                        if (r12 != 0) {
                            r3 = r22;
                            r4 = *(u32*)((u8*)r28 + 0x0);
                            ctr_fn = (void(*)(void))r12;
                            ctr_fn();
                        }
                        tmp = *(u32*)(r31 + r30);
                        if ((s32)tmp != 0) {
                            r20 = 0x3e8;
                            break;
                        }
                    }
                }
            }
        }
    } while ((s32)r20 == 1 && (s32)r23 == 0);
    r3 = r25;
    if ((s32)r20 != 0) {
        r4 = 0x1;
    } else {

        r4 = 0x3;
    }
    ((void(*)(void))fn_8008ABE4)();
    r3 = r20;
    return;
}

/* 0x800733D0 | size: 0x2C0 */
void fn_800733D0(void) {
    extern void fn_80073C38();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r26 = r3;
    r28 = r4;
    r27 = r26 + 0x1;
    r4 = 0x2;
    r3 = r27;
    ((void(*)(void))fn_8008ABE4)();
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r30 = tmp * 0x5;
    OSGetTick();
    r25 = r3;
    do {
        OSGetTick();
        r4 = r3 - r25;
        r3 = r26;
        tmp = r4 ^ r30;
        tmp = __cntlzw(tmp);
        tmp = r4 << tmp;
        r29 = (u32)tmp >> 31;
        fn_80073C38();
    } while ((s32)r3 == 1 && (s32)r29 == 0);
    if ((s32)r3 != 0) {
        r25 = r3;
    } else {
        tmp = 0x88;
        r3 = r26;
        *(u32*)(sp + 0x10) = tmp;
        r4 = (u32)sp + 0x10;
        r5 = (u32)sp + 0xa;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) {
            r25 = 0xb;
        } else {
            r4 = 0x80000000;
            r3 = 0x10620000;
            tmp = *(u32*)((u8*)r4 + 0xF8);
            r3 = r3 + 0x4dd3;
            tmp = (u32)tmp >> 2;
            tmp = (u32)((u64)r3 * (u64)tmp >> 32);
            tmp = (u32)tmp >> 6;
            r24 = tmp * 0x64;
            OSGetTick();
            r5 = (u32)&lbl_803B6E18;
            r4 = (u32)&lbl_803B6E08;
            r6 = r26 << 3;
            r30 = r3;
            tmp = (u32)&lbl_803B6E18;
            r29 = r26 << 2;
            r23 = tmp + r6;
            r25 = (u32)&lbl_803B6E08;
            r31 = r23 + 0x4;
            while (1) {
                OSGetTick();
                tmp = r3 - r30;
                if (tmp > r24) {
                    r3 = 0x1;
                    break;
                }
                r3 = r26;
                r4 = (u32)sp + 0x8;
                ((void(*)(void))fn_8025F3F4)();
                if ((s32)r3 != 0) {
                    r3 = 0x2;
                    break;
                }
                tmp = *(u8*)(sp + 0x8);
                tmp = tmp & 0xa;
                if ((s32)tmp == 8) {
                    r3 = r26;
                    r4 = (u32)sp + 0x14;
                    r5 = (u32)sp + 0xa;
                    ((void(*)(void))fn_8025F584)();
                    if ((s32)r3 != 0) {
                        r3 = 0x3;
                    } else {
                        r3 = 0x0;
                    }
                    break;
                }
                r12 = *(u32*)((u8*)r23 + 0x0);
                if (r12 != 0) {
                    r3 = r26;
                    r4 = *(u32*)((u8*)r31 + 0x0);
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                }
                tmp = *(u32*)(r25 + r29);
                if ((s32)tmp != 0) {
                    r3 = 0x3e8;
                    break;
                }
            }
            if ((s32)r3 != 0) {
                r25 = r3 + 0xb;
            } else {
                r25 = 0x0;
            }
        }
        if ((s32)r25 == 0) {
            tmp = (u32)tmp >> 24;
            if (tmp != 0x88) {
                r25 = 0xf;
            } else {
                r29 = 0x0;
                r3 = 0x10620000;
                r31 = 0x80000000;
                r30 = r3 + 0x4dd3;
                while ((s32)r29 < 0x780) {
                    tmp = *(u32*)((u8*)r28 + 0x0);
                    r3 = r26;
                    r4 = (u32)sp + 0xc;
                    r5 = (u32)sp + 0x9;
                    *(u32*)(sp + 0xC) = tmp;
                    ((void(*)(void))fn_8025F648)();
                    if ((s32)r3 != 0) {
                        r25 = 0x10;
                        break;
                    }
                    tmp = *(u32*)((u8*)r31 + 0xF8);
                    tmp = (u32)tmp >> 2;
                    tmp = (u32)((u64)r30 * (u64)tmp >> 32);
                    tmp = (u32)tmp >> 6;
                    r24 = tmp * 0x64;
                    OSGetTick();
                    r25 = r3;
                    while (1) {
                        OSGetTick();
                        tmp = r3 - r25;
                        if (tmp > r24) {
                            r25 = 0x11;
                            break;
                        }
                        r3 = r26;
                        r4 = (u32)sp + 0x9;
                        ((void(*)(void))fn_8025F3F4)();
                        if ((s32)r3 != 0) {
                            r25 = 0x12;
                            break;
                        }
                        tmp = *(u8*)(sp + 0x9);
                        tmp = tmp & 0x00000002;
                        if ((s32)tmp == 0) {
                            r25 = 0x0;
                            break;
                        }
                    }
                    if ((s32)r25 != 0) {
                        break;
                    }
                    tmp = r29 << 26;
                    r3 = (u32)r29 >> 31;
                    tmp = tmp - r3;
                    /* rotlwi tmp, tmp, 6 */;
                    tmp = tmp + r3;
                    if ((s32)tmp == 0) {
                        ((void(*)(void))_threadSwitch)();
                    }
                    r28 = r28 + 0x4;
                    r29 = r29 + 0x4;
                }
                if ((s32)r29 >= 0x780 && (s32)r25 == 0) {
                    r25 = 0x0;
                }
            }
        }
    }
    r3 = r27;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();
    r3 = r25;
    return;
}

/* 0x80073690 | size: 0x70 */
s32 fn_80073690(s32 arg0, s32 arg1) {
    #pragma peephole off
    extern s32 fn_80073700(s32, s32);
    s32 ch = arg0 + 1;
    s32 ret;

    fn_8008ABE4(ch, 2);
    ret = fn_80073700(arg0, arg1);
    fn_8008ABE4(ch, 1);
    return ret;
}

/* 0x80073700 | size: 0x290 */
void fn_80073700(void) {
    extern void fn_80073C38();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r24 = r3;
    r30 = r4;
    fn_80073C38();
    if ((s32)r3 != 0) {
        return;
    }
    tmp = 0x99;
    r3 = r24;
    *(u32*)(sp + 0xC) = tmp;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x9;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0xb;
    } else {
        r4 = 0x80000000;
        r3 = 0x10620000;
        tmp = *(u32*)((u8*)r4 + 0xF8);
        r3 = r3 + 0x4dd3;
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r3 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r26 = tmp * 0x64;
        OSGetTick();
        r5 = (u32)&lbl_803B6E18;
        r4 = (u32)&lbl_803B6E08;
        r6 = r24 << 3;
        r25 = r3;
        tmp = (u32)&lbl_803B6E18;
        r23 = r24 << 2;
        r28 = tmp + r6;
        r22 = (u32)&lbl_803B6E08;
        r27 = r28 + 0x4;
        while (1) {
            OSGetTick();
            tmp = r3 - r25;
            if (tmp > r26) {
                r3 = 0x1;
                break;
            }
            r3 = r24;
            r4 = (u32)sp + 0x8;
            ((void(*)(void))fn_8025F3F4)();
            if ((s32)r3 != 0) {
                r3 = 0x2;
                break;
            }
            tmp = *(u8*)(sp + 0x8);
            tmp = tmp & 0xa;
            if ((s32)tmp == 8) {
                r3 = r24;
                r4 = (u32)sp + 0x10;
                r5 = (u32)sp + 0x9;
                ((void(*)(void))fn_8025F584)();
                if ((s32)r3 != 0) {
                    r3 = 0x3;
                } else {
                    r3 = 0x0;
                }
                break;
            }
            r12 = *(u32*)((u8*)r28 + 0x0);
            if (r12 != 0) {
                r3 = r24;
                r4 = *(u32*)((u8*)r27 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r22 + r23);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                break;
            }
        }
        if ((s32)r3 != 0) {
            r3 = r3 + 0xb;
        } else {
            r3 = 0x0;
        }
    }
    if ((s32)r3 != 0) {
        return;
    }
    tmp = (u32)tmp >> 24;
    if (tmp != 0x99) {
        r3 = 0xf;
        return;
    }
    r4 = (u32)&lbl_803B6E18;
    r3 = (u32)&lbl_803B6E08;
    r5 = r24 << 3;
    r28 = r24 << 2;
    tmp = (u32)&lbl_803B6E18;
    r29 = (u32)&lbl_803B6E08;
    r26 = tmp + r5;
    r25 = 0x0;
    r27 = r26 + 0x4;
    r3 = 0x10620000;
    r23 = r30;
    r30 = r3 + 0x4dd3;
    r31 = 0x80000000;
    while ((s32)r25 < 0x278) {
        tmp = *(u32*)((u8*)r31 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r30 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r21 = tmp * 0x64;
        OSGetTick();
        r22 = r3;
        while (1) {
            OSGetTick();
            tmp = r3 - r22;
            if (tmp > r21) {
                r3 = 0x10;
                return;
            }
            r3 = r24;
            r4 = (u32)sp + 0xa;
            ((void(*)(void))fn_8025F3F4)();
            if ((s32)r3 != 0) {
                r3 = 0x11;
                return;
            }
            tmp = *(u8*)(sp + 0xA);
            tmp = tmp & 0x00000008;
            if ((s32)tmp != 0) {
                break;
            }
            r12 = *(u32*)((u8*)r26 + 0x0);
            if (r12 != 0) {
                r3 = r24;
                r4 = *(u32*)((u8*)r27 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r29 + r28);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                return;
            }
        }
        r3 = r24;
        r4 = (u32)sp + 0x10;
        r5 = (u32)sp + 0xa;
        ((void(*)(void))fn_8025F584)();
        if ((s32)r3 != 0) {
            r3 = 0x12;
            return;
        }
        *(u32*)((u8*)r23 + 0x0) = tmp;
        tmp = *(u32*)(r29 + r28);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            return;
        }
        r25 = r25 + 0x4;
        r23 = r23 + 0x4;
    }
    r3 = 0x0;

    return;
}

/* 0x80073990 | size: 0xB4 */
s32 fn_80073990(s32 arg0) {
    extern u32 OSGetTick(void);
    extern s32 fn_800D0F44(s32);
    extern s32 GBAWrite(s32, void*, void*);
    u8 wbuf[4];
    u8 rbuf[4];
    u32 delay;
    u32 start;
    u32 bc;

    bc = *(volatile u32*)0x800000F8;
    delay = __mulhwu(0x10624DD3, bc >> 2) >> 6;
    start = OSGetTick();
    do {
    } while (OSGetTick() - start < delay);
    if (fn_800D0F44(arg0) != 0x40000) {
        return 1;
    }
    *(u32*)wbuf = 0x11;
    if (GBAWrite(arg0, wbuf, rbuf) != 0) {
        return 2;
    }
    return 0;
}

/* 0x80073A44 | size: 0x1F4 */
void fn_80073A44(void) {
    extern void fn_80073C38();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r30 = r3;
    r31 = r4;
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r24 = tmp * 0x5;
    OSGetTick();
    r27 = r3;
    do {
        OSGetTick();
        r4 = r3 - r27;
        r3 = r30;
        tmp = r4 ^ r24;
        tmp = __cntlzw(tmp);
        tmp = r4 << tmp;
        r25 = (u32)tmp >> 31;
        fn_80073C38();
    } while ((s32)r3 == 1 && (s32)r25 == 0);
    if ((s32)r3 != 0) {
        return;
    }
    tmp = 0xaa;
    r3 = r30;
    *(u32*)(sp + 0xC) = tmp;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x9;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0xb;
    } else {
        r4 = 0x80000000;
        r3 = 0x10620000;
        tmp = *(u32*)((u8*)r4 + 0xF8);
        r3 = r3 + 0x4dd3;
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r3 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r26 = tmp * 0x64;
        OSGetTick();
        r5 = (u32)&lbl_803B6E18;
        r4 = (u32)&lbl_803B6E08;
        r6 = r30 << 3;
        r27 = r3;
        tmp = (u32)&lbl_803B6E18;
        r28 = r30 << 2;
        r24 = tmp + r6;
        r29 = (u32)&lbl_803B6E08;
        r25 = r24 + 0x4;
        while (1) {
            OSGetTick();
            tmp = r3 - r27;
            if (tmp > r26) {
                r3 = 0x1;
                break;
            }
            r3 = r30;
            r4 = (u32)sp + 0x8;
            ((void(*)(void))fn_8025F3F4)();
            if ((s32)r3 != 0) {
                r3 = 0x2;
                break;
            }
            tmp = *(u8*)(sp + 0x8);
            tmp = tmp & 0xa;
            if ((s32)tmp == 8) {
                r3 = r30;
                r4 = (u32)sp + 0x10;
                r5 = (u32)sp + 0x9;
                ((void(*)(void))fn_8025F584)();
                if ((s32)r3 != 0) {
                    r3 = 0x3;
                } else {
                    r3 = 0x0;
                }
                break;
            }
            r12 = *(u32*)((u8*)r24 + 0x0);
            if (r12 != 0) {
                r3 = r30;
                r4 = *(u32*)((u8*)r25 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r29 + r28);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                break;
            }
        }
        if ((s32)r3 != 0) {
            r3 = r3 + 0xb;
        } else {
            r3 = 0x0;
        }
    }
    if ((s32)r3 != 0) {
        return;
    }
    r5 = (u32)r4 >> 24;
    if (r5 != 0xaa) {
        r3 = 0xf;
        return;
    }
    tmp = ((r4 << 24) | (r4 >> 8)) & 0x0000FF00;
    r3 = ((r4 << 8) | (r4 >> 24)) & 0x00FF0000;
    tmp = r5 | tmp;
    r4 = r4 << 24;
    tmp = r3 | tmp;
    r3 = 0x0;
    tmp = r4 | tmp;
    tmp = (u32)tmp >> 16;
    *(u16*)((u8*)r31 + 0x0) = tmp;

    return;
}

/* 0x80073C38 | size: 0x24C */
void fn_80073C38(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r29 = r3;
    ((void(*)(void))fn_800D0F44)();
    /* subis tmp, r3, 0x4 */;
    if (tmp != 0) {
        r3 = 0x1;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0x9;
    ((void(*)(void))fn_8025F484)();
    if ((s32)r3 != 0) {
        r3 = 0x2;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x5;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r29 << 3;
    r28 = r3;
    tmp = (u32)&lbl_803B6E18;
    r27 = r29 << 2;
    r31 = tmp + r6;
    r26 = (u32)&lbl_803B6E08;
    r30 = r31 + 0x4;
    while (1) {
        OSGetTick();
        tmp = r3 - r28;
        if (tmp > r25) {
            r3 = 0x1;
            break;
        }
        r3 = r29;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x2;
            break;
        }
        tmp = *(u8*)(sp + 0x8);
        tmp = tmp & 0xa;
        if ((s32)tmp == 8) {
            r3 = r29;
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x9;
            ((void(*)(void))fn_8025F584)();
            if ((s32)r3 != 0) {
                r3 = 0x3;
            } else {
                r3 = 0x0;
            }
            break;
        }
        r12 = *(u32*)((u8*)r31 + 0x0);
        if (r12 != 0) {
            r3 = r29;
            r4 = *(u32*)((u8*)r30 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r26 + r27);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            break;
        }
    }
    if ((s32)r3 != 0) {
        r3 = r3 + 0x2;
        return;
    }
    tmp = *(u32*)&lbl_8047A60C;
    if (r3 != tmp) {
        r3 = 0x6;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0x9;
    ((void(*)(void))fn_8025F3F4)();
    if ((s32)r3 != 0) {
        r3 = 0x7;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x9;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0x8;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x64;
    OSGetTick();
    r4 = (u32)&lbl_803B6E08;
    r26 = r3;
    r27 = r29 << 2;
    r28 = (u32)&lbl_803B6E08;
    do {
        OSGetTick();
        tmp = r3 - r26;
        if (tmp > r25) {
            r3 = 0x9;
            return;
        }
        r3 = r29;
        r4 = (u32)sp + 0x9;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0xa;
            return;
        }
        tmp = *(u8*)(sp + 0x9);
        tmp = tmp & 0x00000002;
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        r12 = *(u32*)((u8*)r31 + 0x0);
        if (r12 != 0) {
            r3 = r29;
            r4 = *(u32*)((u8*)r30 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r28 + r27);
    } while ((s32)tmp == 0);
    r3 = 0x3e8;
    return;

    r3 = 0x0;

    return;
}

/* 0x80073E84 | size: 0x8 */
s32 fn_80073E84(void) {
    return 0x1;
}

/* 0x80073E8C | size: 0x498 */
void fn_80073E8C(void) {
    u8 sp[0x90];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r3;
    r29 = r4;
    r3 = (u32)&lbl_80268780;
    r31 = (u32)&lbl_80268780;
    ((void(*)(void))fn_800A7BCC)();
    tmp = r3;
    r3 = (u32)&lbl_8047A60C;
    r4 = tmp;
    r5 = 0x4;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    tmp = *(u32*)&lbl_8047A60C;
    tmp = tmp | (0x20 << 16);
    tmp = tmp | 0x2020;
    *(u32*)&lbl_8047A60C = tmp;
    ((void(*)(void))fn_8025F350)();
    r3 = r30;
    r4 = (u32)sp + 0x44;
    ((void(*)(void))fn_800A501C)();
    if ((s32)r3 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x1d6;
        ((void(*)(void))fn_800060F0)();
    }
    tmp = r3 + 0x7;
    /* clrrwi r3, tmp, 3 */;
    *(u32*)&lbl_8047A608 = r3;
    if (r3 == 0 || r3 > 0x20000) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x28;
        r4 = 0x1dc;
        ((void(*)(void))fn_800060F0)();
    }
    r5 = *(u32*)&lbl_8047A608;
    r3 = (u32)&lbl_803D6E40;
    r4 = (u32)&lbl_803D6E40;
    r6 = 0x0;
    tmp = r5 + 0x1f;
    r3 = (u32)sp + 0x44;
    /* clrrwi r5, tmp, 5 */;
    r7 = 0x2;
    *(u32*)&lbl_8047A608 = r5;
    ((void(*)(void))fn_800A541C)();
    if ((s32)r3 < 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x4c;
        r4 = 0x1e1;
        ((void(*)(void))fn_800060F0)();
    }
    r3 = (u32)sp + 0x44;
    ((void(*)(void))fn_800A50E4)();
    r30 = (u32)&lbl_8047A60C;
    r3 = (u32)&lbl_803D6E40;
    r8 = 0xa0;
    r6 = *(u8*)((u8*)r30 + 0x0);
    r5 = (u32)&lbl_803D6E40;
    r4 = *(u8*)((u8*)r30 + 0x1);
    r3 = *(u8*)((u8*)r30 + 0x2);
    tmp = *(u8*)((u8*)r30 + 0x3);
    r7 = 0xe7;
    *(u8*)((u8*)r5 + 0xAC) = r6;
    *(u8*)((u8*)r5 + 0xAD) = r4;
    *(u8*)((u8*)r5 + 0xAE) = r3;
    *(u8*)((u8*)r5 + 0xAF) = tmp;
    if ((s32)r8 < 0xbd) {
        r4 = *(u8*)((u8*)r5 + 0xA0);
        r6 = r5 + 0xa7;
        r3 = *(u8*)((u8*)r5 + 0xA1);
        r8 = 0xbc;
        r7 = 0xe7 - r4;
        tmp = *(u8*)((u8*)r5 + 0xA2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r5 + 0xA3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r5 + 0xA4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r5 + 0xA5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r5 + 0xA6);
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r6 = r6 + 0x7;
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r6 = r6 + 0x7;
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r7 = r7 - r3;
        r7 = r7 - tmp;
        r3 = r5 + 0xbc;
        tmp = 0xbd - r8;
        ctr_fn = (void(*)(void))tmp;
        if ((s32)r8 < 0xbd) {
            do {
                tmp = *(u8*)((u8*)r3 + 0x0);
                r3 = r3 + 0x1;
                r8 = r8 + 0x1;
                r7 = r7 - tmp;
            } while (--ctr != 0);
    }
    }
    r3 = (u32)&lbl_803D6E40;
    tmp = r7 & 0xFF;
    r3 = (u32)&lbl_803D6E40;
    *(u8*)(r3 + r8) = tmp;
    if (r29 == 0) { r3 = 0x0; return; }
    r3 = r29;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_800A501C)();
    if ((s32)r3 == 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x10;
        r4 = 0x1d6;
        ((void(*)(void))fn_800060F0)();
    }
    tmp = r3 + 0x7;
    /* clrrwi r3, tmp, 3 */;
    *(u32*)&lbl_8047A604 = r3;
    if (r3 == 0 || r3 > 0x20000) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x28;
        r4 = 0x1dc;
        ((void(*)(void))fn_800060F0)();
    }
    r5 = *(u32*)&lbl_8047A604;
    r3 = (u32)&lbl_803B6E40;
    r4 = (u32)&lbl_803B6E40;
    r6 = 0x0;
    tmp = r5 + 0x1f;
    r3 = (u32)sp + 0x8;
    /* clrrwi r5, tmp, 5 */;
    r7 = 0x2;
    *(u32*)&lbl_8047A604 = r5;
    ((void(*)(void))fn_800A541C)();
    if ((s32)r3 < 0) {
        r3 = r31 + 0x0;
        r5 = r31 + 0x4c;
        r4 = 0x1e1;
        ((void(*)(void))fn_800060F0)();
    }
    r3 = (u32)sp + 0x8;
    ((void(*)(void))fn_800A50E4)();
    r3 = (u32)&lbl_803B6E40;
    r8 = 0xa0;
    r6 = *(u8*)((u8*)r30 + 0x0);
    r5 = (u32)&lbl_803B6E40;
    r4 = *(u8*)((u8*)r30 + 0x1);
    r3 = *(u8*)((u8*)r30 + 0x2);
    r7 = 0xe7;
    tmp = *(u8*)((u8*)r30 + 0x3);
    *(u8*)((u8*)r5 + 0xAC) = r6;
    *(u8*)((u8*)r5 + 0xAD) = r4;
    *(u8*)((u8*)r5 + 0xAE) = r3;
    *(u8*)((u8*)r5 + 0xAF) = tmp;
    if ((s32)r8 < 0xbd) {
        r4 = *(u8*)((u8*)r5 + 0xA0);
        r6 = r5 + 0xa7;
        r3 = *(u8*)((u8*)r5 + 0xA1);
        r8 = 0xbc;
        r7 = 0xe7 - r4;
        tmp = *(u8*)((u8*)r5 + 0xA2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r5 + 0xA3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r5 + 0xA4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r5 + 0xA5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r5 + 0xA6);
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r6 = r6 + 0x7;
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r6 = r6 + 0x7;
        r7 = r7 - r3;
        r4 = *(u8*)((u8*)r6 + 0x0);
        r7 = r7 - tmp;
        r3 = *(u8*)((u8*)r6 + 0x1);
        r7 = r7 - r4;
        tmp = *(u8*)((u8*)r6 + 0x2);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x3);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x4);
        r7 = r7 - r3;
        r3 = *(u8*)((u8*)r6 + 0x5);
        r7 = r7 - tmp;
        tmp = *(u8*)((u8*)r6 + 0x6);
        r7 = r7 - r3;
        r7 = r7 - tmp;
        r3 = r5 + 0xbc;
        tmp = 0xbd - r8;
        ctr_fn = (void(*)(void))tmp;
        if ((s32)r8 < 0xbd) {
            do {
                tmp = *(u8*)((u8*)r3 + 0x0);
                r3 = r3 + 0x1;
                r8 = r8 + 0x1;
                r7 = r7 - tmp;
            } while (--ctr != 0);
    }
    }
    r3 = (u32)&lbl_803B6E40;
    tmp = r7 & 0xFF;
    r3 = (u32)&lbl_803B6E40;
    *(u8*)(r3 + r8) = tmp;

    r3 = 0x0;
    return;
}

/* 0x80074324 | size: 0x3C */
s32 fn_80074324(s32 arg0) {
    #pragma peephole off
    extern s32 fn_80074360(s32);
    fn_8008ABE4(arg0 + 1, 0);
    return fn_80074360(arg0);
}

/* 0x80074360 | size: 0x254 */
void fn_80074360(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r29 = r3;
    ((void(*)(void))fn_800D0F44)();
    /* subis tmp, r3, 0x4 */;
    if (tmp != 0) {
        r3 = 0x1;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F484)();
    if ((s32)r3 != 0) {
        r3 = 0x2;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r29 << 3;
    r28 = r3;
    tmp = (u32)&lbl_803B6E18;
    r27 = r29 << 2;
    r31 = tmp + r6;
    r26 = (u32)&lbl_803B6E08;
    r30 = r31 + 0x4;
    while (1) {
        OSGetTick();
        tmp = r3 - r28;
        if (tmp > r25) {
            r3 = 0x3;
            return;
        }
        r3 = r29;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x4;
            return;
        }
        tmp = *(u8*)(sp + 0x8);
        if (tmp == 0x18) break;
        r12 = *(u32*)((u8*)r31 + 0x0);
        if (r12 != 0) {
            r3 = r29;
            r4 = *(u32*)((u8*)r30 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r26 + r27);
        if ((s32)tmp == 0) continue;
        r3 = 0x3e8;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F584)();
    if ((s32)r3 != 0) {
        r3 = 0x5;
        return;
    }
    /* subis tmp, r3, 0x4158 */;
    if (tmp != 0x5645) {
        r3 = 0x6;
        return;
    }
    r3 = r29;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F3F4)();
    if ((s32)r3 != 0) {
        tmp = *(u8*)(sp + 0x8);
        if (tmp != 0x10) {
            r3 = 0x7;
            return;
    }
    }
    r3 = r29;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0x8;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r25 = tmp * 0x64;
    OSGetTick();
    r4 = (u32)&lbl_803B6E08;
    r26 = r3;
    r27 = r29 << 2;
    r28 = (u32)&lbl_803B6E08;
    do {
        OSGetTick();
        tmp = r3 - r26;
        if (tmp > r25) {
            r3 = 0x9;
            return;
        }
        r3 = r29;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0xa;
            return;
        }
        r3 = *(u8*)(sp + 0x8);
        tmp = r3 & 0x00000030;
        if ((s32)tmp != 0x10) {
            r3 = 0xb;
            return;
        }
        tmp = r3 & 0x00000002;
        if ((s32)tmp == 0) { r3 = 0x0; return; }
        r12 = *(u32*)((u8*)r31 + 0x0);
        if (r12 != 0) {
            r3 = r29;
            r4 = *(u32*)((u8*)r30 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r28 + r27);
    } while ((s32)tmp == 0);
    r3 = 0x3e8;
    return;

    r3 = 0x0;

    return;
}

/* 0x800745B4 | size: 0x258 */
void fn_800745B4(void) {
    extern void fn_80073C38();
    extern void fn_8007480C();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r12 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r27 = r3;
    fn_8007480C();
    r29 = r3;
    if ((s32)r29 != 0) { r3 = r29; return; }
    r3 = 0x80000000;
    tmp = *(u32*)((u8*)r3 + 0xF8);
    tmp = (u32)tmp >> 2;
    r28 = tmp << 3;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r27 << 3;
    r21 = r3;
    tmp = (u32)&lbl_803B6E18;
    r22 = r27 << 2;
    r30 = tmp + r6;
    r23 = (u32)&lbl_803B6E08;
    r31 = r30 + 0x4;
    r3 = 0x10620000;
    r25 = 0x80000000;
    r24 = r3 + 0x4dd3;
    do {
        OSGetTick();
        tmp = r3 - r21;
        if (tmp > r28) {
            r3 = 0x16;
            return;
        }
        r12 = *(u32*)((u8*)r30 + 0x0);
        if (r12 != 0) {
            r3 = r27;
            r4 = *(u32*)((u8*)r31 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r23 + r22);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            return;
        }
        tmp = *(u32*)((u8*)r25 + 0xF8);
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r24 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r20 = tmp * 0x5;
        OSGetTick();
        r26 = r3;
        do {
            OSGetTick();
            r4 = r3 - r26;
            r3 = r27;
            tmp = r4 ^ r20;
            tmp = __cntlzw(tmp);
            tmp = r4 << tmp;
            r19 = (u32)tmp >> 31;
            fn_80073C38();
        } while ((s32)r3 == 1 && (s32)r19 == 0);
        if ((s32)r3 != 0) {
            r3 = r3;
        } else {
            tmp = 0xaa;
            r3 = r27;
            *(u32*)(sp + 0xC) = tmp;
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x9;
            ((void(*)(void))fn_8025F648)();
            if ((s32)r3 != 0) {
                r3 = 0xb;
            } else {
                r4 = 0x80000000;
                r3 = 0x10620000;
                tmp = *(u32*)((u8*)r4 + 0xF8);
                r3 = r3 + 0x4dd3;
                tmp = (u32)tmp >> 2;
                tmp = (u32)((u64)r3 * (u64)tmp >> 32);
                tmp = (u32)tmp >> 6;
                r19 = tmp * 0x64;
                OSGetTick();
                r20 = r3;
                while (1) {
                    OSGetTick();
                    tmp = r3 - r20;
                    if (tmp > r19) {
                        r3 = 0x1;
                        break;
                    }
                    r3 = r27;
                    r4 = (u32)sp + 0x8;
                    ((void(*)(void))fn_8025F3F4)();
                    if ((s32)r3 != 0) {
                        r3 = 0x2;
                        break;
                    }
                    tmp = *(u8*)(sp + 0x8);
                    tmp = tmp & 0xa;
                    if ((s32)tmp == 8) {
                        r3 = r27;
                        r4 = (u32)sp + 0x10;
                        r5 = (u32)sp + 0x9;
                        ((void(*)(void))fn_8025F584)();
                        if ((s32)r3 != 0) {
                            r3 = 0x3;
                        } else {
                            r3 = 0x0;
                        }
                        break;
                    }
                    r12 = *(u32*)((u8*)r30 + 0x0);
                    if (r12 != 0) {
                        r3 = r27;
                        r4 = *(u32*)((u8*)r31 + 0x0);
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                    }
                    tmp = *(u32*)(r23 + r22);
                    if ((s32)tmp != 0) {
                        r3 = 0x3e8;
                        break;
                    }
                }
                if ((s32)r3 != 0) {
                    r3 = r3 + 0xb;
                } else {
                    r3 = 0x0;
                }
            }
            if ((s32)r3 == 0) {
                tmp = (u32)tmp >> 24;
                if (tmp != 0xaa) {
                    r3 = 0xf;
                } else {
                    r3 = 0x0;
                }
            }
        }
    } while ((s32)r3 != 0);
    r3 = r27 + 0x1;
    r4 = 0x1;
    ((void(*)(void))fn_8008ABE4)();

    r3 = r29;

    return;
}

/* 0x8007480C | size: 0xB84 */
void fn_8007480C(void) {
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
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
    f32 f8 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r3;
    r3 = (u32)&lbl_80268780;
    r30 = (u32)&lbl_80268780;
    if ((s32)r4 != 0) {
        r3 = (u32)&lbl_803B6E40;
        r18 = *(u32*)&lbl_8047A604;
        tmp = (u32)&lbl_803B6E40;
        r22 = tmp;
    } else {

        r3 = (u32)&lbl_803D6E40;
        r18 = *(u32*)&lbl_8047A608;
        tmp = (u32)&lbl_803D6E40;
        r22 = tmp;
    }
    r16 = 0x0;
    do {
        if (r16 > 0x20) {
            r3 = 0xDD650000;
            r20 = r3 + 0x4321;
        } else {
            OSGetTick();
            tmp = r3 & 0xFFFFFF;
            r20 = tmp | (0xdd00 << 16);
        }
        r3 = 0x0;
        r5 = r20;
        r4 = r3;
        tmp = 0x4;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            tmp = r5 & 0x1;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r5 = (u32)r5 >> 1;
            r4 = r4 + 0x7;
        } while (--ctr != 0);
        r16 = r16 + 0x1;
    } while (r3 < 0xa || r3 > 0x18);

    r3 = (u32)r20 >> 24;
    tmp = ((r20 << 24) | (r20 >> 8)) & 0x0000FF00;
    r4 = ((r20 << 8) | (r20 >> 24)) & 0x00FF0000;
    r5 = r20 << 24;
    tmp = r3 | tmp;
    r3 = r31;
    tmp = r4 | tmp;
    r4 = (u32)sp + 0xc;
    tmp = r5 | tmp;
    r5 = (u32)sp + 0x8;
    *(u32*)(sp + 0xC) = tmp;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0x1;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r16 = tmp * 0x64;
    OSGetTick();
    r5 = (u32)&lbl_803B6E18;
    r4 = (u32)&lbl_803B6E08;
    r6 = r31 << 3;
    r17 = r3;
    tmp = (u32)&lbl_803B6E18;
    r21 = r31 << 2;
    r29 = tmp + r6;
    r19 = (u32)&lbl_803B6E08;
    r28 = r29 + 0x4;
    while (1) {
        OSGetTick();
        tmp = r3 - r17;
        if (tmp > r16) {
            r3 = 0x2;
            return;
        }
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x3;
            return;
        }
        tmp = *(u8*)(sp + 0x8);
        if (tmp == 0x38) break;
        r12 = *(u32*)((u8*)r29 + 0x0);
        if (r12 != 0) {
            r3 = r31;
            r4 = *(u32*)((u8*)r28 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r19 + r21);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            return;
        }
    }
    r3 = r31;
    r4 = (u32)sp + 0x10;
    r5 = (u32)sp + 0x8;
    ((void(*)(void))fn_8025F584)();
    if ((s32)r3 != 0) {
        r3 = 0x4;
        return;
    }
    r3 = (u32)r5 >> 24;
    tmp = ((r5 << 24) | (r5 >> 8)) & 0x0000FF00;
    r4 = ((r5 << 8) | (r5 >> 24)) & 0x00FF0000;
    r5 = r5 << 24;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    r4 = r5 | tmp;
    tmp = r4 & 0xFF;
    if (tmp != 0xee) {
        r3 = r30 + 0x68;
        OSReport();
        r21 = 0x0;
    } else {
        r3 = 0x0;
        /* clrrwi r21, r4, 8 */;
        r5 = r3;
        tmp = 0x3;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            /* clrrwi tmp, r4, 31 */;
            if (tmp != 0) {
                r3 = r3 + 0x1;
            }
            r4 = r4 << 1;
            r5 = r5 + 0x7;
        } while (--ctr != 0);
        if (r3 < 7 || r3 > 0xe) {
            r4 = r21;
            r3 = r30 + 0x98;
            OSReport();
            r21 = 0x0;
        }
    }
    if (r21 == 0) {
        r3 = 0x5;
        return;
    }
    tmp = r18 + 0x7;
    r3 = r31;
    /* clrrwi r27, tmp, 3 */;
    r4 = (u32)sp + 0xc;
    r6 = (u32)r27 >> 3;
    r5 = (u32)sp + 0x8;
    r6 = (u32)r8 >> 24;
    tmp = ((r8 << 24) | (r8 >> 8)) & 0x0000FF00;
    r7 = ((r8 << 8) | (r8 >> 24)) & 0x00FF0000;
    tmp = r6 | tmp;
    r6 = r8 << 24;
    tmp = r7 | tmp;
    tmp = r6 | tmp;
    *(u32*)(sp + 0xC) = tmp;
    ((void(*)(void))fn_8025F648)();
    if ((s32)r3 != 0) {
        r3 = 0x6;
        return;
    }
    r4 = 0x80000000;
    r3 = 0x10620000;
    tmp = *(u32*)((u8*)r4 + 0xF8);
    r3 = r3 + 0x4dd3;
    tmp = (u32)tmp >> 2;
    tmp = (u32)((u64)r3 * (u64)tmp >> 32);
    tmp = (u32)tmp >> 6;
    r16 = tmp * 0x64;
    OSGetTick();
    r4 = (u32)&lbl_803B6E08;
    r19 = r3;
    r18 = r31 << 2;
    r17 = (u32)&lbl_803B6E08;
    while (1) {
        OSGetTick();
        tmp = r3 - r19;
        if (tmp > r16) {
            r3 = 0x7;
            return;
        }
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0x8;
            return;
        }
        r3 = *(u8*)(sp + 0x8);
        tmp = r3 & 0x00000030;
        if ((s32)tmp != 0x30) {
            r3 = 0x9;
            return;
        }
        tmp = r3 & 0x00000002;
        if ((s32)tmp == 0) break;
        r12 = *(u32*)((u8*)r29 + 0x0);
        if (r12 != 0) {
            r3 = r31;
            r4 = *(u32*)((u8*)r28 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r17 + r18);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            return;
        }
    }
    r3 = 0x61770000;
    r5 = r20 ^ r21;
    tmp = r3 + 0x614b;
    r3 = (u32)&lbl_803B6E08;
    r4 = r5 * tmp;
    r24 = r5;
    r17 = r31 << 2;
    r18 = (u32)&lbl_803B6E08;
    r25 = 0x30;
    r23 = 0x0;
    r26 = r4 + 0x1;
    r3 = 0x10620000;
    r20 = 0x80000000;
    r19 = r3 + 0x4dd3;
    while (r23 < r27) {

        r5 = *(u32*)((u8*)r22 + 0x0);
        r3 = (u32)r5 >> 24;
        tmp = ((r5 << 24) | (r5 >> 8)) & 0x0000FF00;
        r4 = ((r5 << 8) | (r5 >> 24)) & 0x00FF0000;
        r5 = r5 << 24;
        tmp = r3 | tmp;
        tmp = r4 | tmp;
        tmp = r5 | tmp;
        r5 = tmp;
        if (r23 >= 0xa0) {
            r5 = tmp - r24;
            r4 = r24 ^ tmp;
            r5 = r5 ^ r26;
            r3 = 0x20;
            tmp = 0x4;
            ctr_fn = (void(*)(void))tmp;
            do {
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
                tmp = r4 & 0x1;
                if (tmp != 0) {
                    r4 = (u32)r4 >> 1;
                    r4 = r4 ^ 0xa1c1;
                } else {

                    r4 = (u32)r4 >> 1;
                }
            } while (--ctr != 0);
            r3 = 0x61770000;
            r24 = r4;
            tmp = r3 + 0x614b;
            r3 = r26 * tmp;
            r26 = r3 + 0x1;
        }
        r3 = (u32)r5 >> 24;
        tmp = ((r5 << 24) | (r5 >> 8)) & 0x0000FF00;
        r4 = ((r5 << 8) | (r5 >> 24)) & 0x00FF0000;
        r5 = r5 << 24;
        tmp = r3 | tmp;
        r3 = r31;
        tmp = r4 | tmp;
        r4 = (u32)sp + 0xc;
        tmp = r5 | tmp;
        r5 = (u32)sp + 0x8;
        *(u32*)(sp + 0xC) = tmp;
        ((void(*)(void))fn_8025F648)();
        if ((s32)r3 != 0) {
            r3 = 0xa;
            return;
        }
        tmp = *(u8*)(sp + 0x8);
        tmp = tmp & 0x00000030;
        if (tmp != r25) {
            r3 = 0x12;
            return;
        }
        tmp = *(u32*)((u8*)r20 + 0xF8);
        r25 = r25 ^ 0x10;
        tmp = (u32)tmp >> 2;
        tmp = (u32)((u64)r19 * (u64)tmp >> 32);
        tmp = (u32)tmp >> 6;
        r16 = tmp * 0x64;
        OSGetTick();
        r21 = r3;
        while (1) {
            OSGetTick();
            tmp = r3 - r21;
            if (tmp > r16) {
                r3 = 0xb;
                return;
            }
            r3 = r31;
            r4 = (u32)sp + 0x8;
            ((void(*)(void))fn_8025F3F4)();
            if ((s32)r3 != 0) {
                r3 = 0xc;
                return;
            }
            r3 = *(u8*)(sp + 0x8);
            tmp = r3 & 0x00000020;
            if ((s32)tmp == 0) {
                r3 = 0xd;
                return;
            }
            tmp = r3 & 0x00000002;
            if ((s32)tmp == 0) break;
            r12 = *(u32*)((u8*)r29 + 0x0);
            if (r12 != 0) {
                r3 = r31;
                r4 = *(u32*)((u8*)r28 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r18 + r17);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                return;
            }
        }
        tmp = *(u32*)(r18 + r17);
        if ((s32)tmp != 0) {
            r3 = 0x3e8;
            return;
        }
        r23 = r23 + 0x4;
        r22 = r22 + 0x4;

    }
    r3 = 0x80000000;
    tmp = *(u32*)((u8*)r3 + 0xF8);
    tmp = (u32)tmp >> 2;
    r17 = tmp * 0xa;
    OSGetTick();
    r4 = (u32)&lbl_803B6E08;
    r19 = r3;
    r18 = r31 << 2;
    r16 = (u32)&lbl_803B6E08;
    while (1) {
        OSGetTick();
        tmp = r3 - r19;
        if (tmp > r17) {
            r3 = 0xe;
            return;
        }
        r3 = r31;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_8025F3F4)();
        if ((s32)r3 != 0) {
            r3 = 0xf;
            return;
        }
        r3 = *(u8*)(sp + 0x8);
        tmp = r3 & 0x00000030;
        if ((s32)tmp != 0x30) {
            r3 = 0x10;
            return;
        }
        tmp = r3 & 0xa;
        if ((s32)tmp == 8) {
            r3 = r31;
            r4 = (u32)sp + 0x10;
            r5 = (u32)sp + 0x8;
            ((void(*)(void))fn_8025F584)();
            if ((s32)r3 != 0) {
                r3 = 0x11;
                return;
            }
            r5 = (u32)r4 >> 24;
            if (r5 == 0xff) {
                tmp = ((r4 << 24) | (r4 >> 8)) & 0x0000FF00;
                r3 = ((r4 << 8) | (r4 >> 24)) & 0x00FF0000;
                tmp = r5 | tmp;
                r4 = r4 << 24;
                tmp = r3 | tmp;
                r5 = r24;
                r16 = r4 | tmp;
                r3 = r30 + 0xcc;
                r4 = r16;
                OSReport();
                r7 = (u32)r16 >> 8;
                r3 = 0x0;
                r4 = -0x1;
                while (1) {
                    r6 = r4 ^ r24;
                    r5 = 0x20;
                    tmp = 0x4;
                    ctr_fn = (void(*)(void))tmp;
                    do {
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                        tmp = r6 & 0x1;
                        if (tmp != 0) {
                            r6 = (u32)r6 >> 1;
                            r6 = r6 ^ 0xa1c1;
                        } else {
                            r6 = (u32)r6 >> 1;
                        }
                    } while (--ctr != 0);
                    if (r7 == r6) {
                        r5 = r4 ^ 0xbb;
                        r3 = 0x20;
                        tmp = 0x4;
                        ctr_fn = (void(*)(void))tmp;
                        do {
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                            tmp = r5 & 0x1;
                            if (tmp != 0) {
                                r5 = (u32)r5 >> 1;
                                r5 = r5 ^ 0xa1c1;
                            } else {
                                r5 = (u32)r5 >> 1;
                            }
                        } while (--ctr != 0);
                        r16 = r5 | (0xbb00 << 16);
                        r3 = r30 + 0x100;
                        r5 = r16;
                        OSReport();
                        break;
                    }
                    r4 = r4 + (0x100 << 16);
                    r3 = r3 + 0x1;
                    if (r3 >= 0x100) {
                        r4 = r16;
                        r5 = r24;
                        r3 = r30 + 0x130;
                        OSReport();
                        r16 = 0x0;
                        break;
                    }
                }
                if (r16 == 0) {
                    r3 = 0x12;
                    return;
                }
                r3 = (u32)r16 >> 24;
                tmp = ((r16 << 24) | (r16 >> 8)) & 0x0000FF00;
                r4 = ((r16 << 8) | (r16 >> 24)) & 0x00FF0000;
                r5 = r16 << 24;
                tmp = r3 | tmp;
                r3 = r31;
                tmp = r4 | tmp;
                r4 = (u32)sp + 0xc;
                tmp = r5 | tmp;
                r5 = (u32)sp + 0x8;
                *(u32*)(sp + 0xC) = tmp;
                ((void(*)(void))fn_8025F648)();
                if ((s32)r3 != 0) {
                    r3 = 0x13;
                    return;
                }
                break;
            }
            if (r5 != 0xcc) {
                r3 = 0x14;
                return;
            }
            tmp = *(u32*)(r16 + r18);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                return;
            }
        } else {
            r12 = *(u32*)((u8*)r29 + 0x0);
            if (r12 != 0) {
                r3 = r31;
                r4 = *(u32*)((u8*)r28 + 0x0);
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
            }
            tmp = *(u32*)(r16 + r18);
            if ((s32)tmp != 0) {
                r3 = 0x3e8;
                return;
            }
        }
    }
    r3 = 0x80000000;
    tmp = *(u32*)((u8*)r3 + 0xF8);
    r19 = (u32)tmp >> 2;
    OSGetTick();
    r4 = (u32)&lbl_803B6E08;
    r18 = r3;
    r17 = r31 << 2;
    r16 = (u32)&lbl_803B6E08;
    do {
        OSGetTick();
        tmp = r3 - r18;
        if (tmp > r19) {
            r3 = 0x15;
            return;
        }
        r3 = r31;
        ((void(*)(void))fn_800D0F44)();
        /* subis tmp, r3, 0x4 */;
        if (tmp != 0) { r3 = 0x0; return; }
        r12 = *(u32*)((u8*)r29 + 0x0);
        if (r12 != 0) {
            r3 = r31;
            r4 = *(u32*)((u8*)r28 + 0x0);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = *(u32*)(r16 + r17);
    } while ((s32)tmp == 0);
    r3 = 0x3e8;
    return;

    r3 = 0x0;

    return;
}

/* 0x80075390 | size: 0x40 */
#pragma push
#pragma peephole off
s32 fn_80075390(void) {
    extern u8 fn_80075638();
    extern void fn_8007565C();
    extern void fn_800756C8();

    if ((u8)fn_80075638() != 0) {
        fn_8007565C();
    } else {
        fn_800756C8(0x3);
    }
    return 0;
}
#pragma pop

/* 0x800753D0 | size: 0x148 */
void fn_800753D0(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f31 = 0.0f;

    ((void(*)(void))fn_800D37CC)();
    tmp = 0x43300000;
    f1 = *(f64*)&lbl_8047C0B0;
    *(u32*)(sp + 0x18) = tmp;
    f31 = f0 - f1;
    ((void(*)(void))fn_800D3088)();
    tmp = 0x43300000;
    f1 = *(f64*)&lbl_8047C0B8;
    *(u32*)(sp + 0x20) = tmp;
    r3 = *(u32*)&lbl_8047A610;
    f0 = f0 - f1;
    f0 = f0 / f31;
    *(f32*)((u8*)r3 + 0x0) = f0;
    r3 = *(u32*)&lbl_8047A610;
    f1 = *(f32*)((u8*)r3 + 0x4);
    ((void(*)(void))fn_800CE148)();
    f2 = (f32)f1;
    f1 = *(f32*)&lbl_8047C09C;
    f0 = *(f32*)&lbl_8047C098;
    r3 = *(u32*)&lbl_8047A610;
    f1 = f1 * f2 + f0;
    f0 = *(f32*)&lbl_8047C0A0;
    *(f32*)((u8*)r3 + 0x18C) = f1;
    r3 = *(u32*)&lbl_8047A610;
    f1 = *(f32*)((u8*)r3 + 0x18C);
    if (f1 > f0) {
        *(f32*)((u8*)r3 + 0x18C) = f0;
    }
    r3 = *(u32*)&lbl_8047A610;
    r4 = 0x0;
    r3 = r3 + 0x144;
    ((void(*)(void))fn_80109B90)();
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r3 = *(u32*)&lbl_8047A610;
        r3 = *(u32*)((u8*)r3 + 0x168);
        ((void(*)(void))fn_801DAC3C)();
        if (r3 != 0) {
            r5 = *(u32*)&lbl_8047A610;
            r4 = (u32)sp + 0x8;
            f2 = *(f32*)&lbl_8047C0A4;
            f0 = *(f32*)((u8*)r5 + 0x0);
            f1 = *(f32*)((u8*)r5 + 0x8);
            f2 = f2 * f0;
            f0 = *(f32*)&lbl_8047C0A8;
            f1 = f1 + f2;
            *(f32*)((u8*)r5 + 0x8) = f1;
            *(f32*)(sp + 0x8) = f0;
            *(f32*)(sp + 0x10) = f0;
            *(f32*)(sp + 0xC) = f2;
            ((void(*)(void))fn_800E3DC4)();
    }
    }
    r3 = *(u32*)&lbl_8047A610;
    f0 = *(f32*)&lbl_8047C0AC;
    f2 = *(f32*)((u8*)r3 + 0x4);
    f1 = *(f32*)((u8*)r3 + 0x0);
    f1 = f2 + f1;
    *(f32*)((u8*)r3 + 0x4) = f1;
    r3 = *(u32*)&lbl_8047A610;
    f1 = *(f32*)((u8*)r3 + 0x4);
    /* cror eq, gt, eq */;
    if (f1 == f0) {
        f0 = *(f32*)&lbl_8047C0A8;
        *(f32*)((u8*)r3 + 0x4) = f0;
    }
    return;
}

/* 0x80075518 | size: 0x120 */
void fn_80075518(s32 arg0, u8* arg1) {
    extern s32 fn_80109934(u32);
    extern void fn_800D88DC(s32);
    extern void fn_800D888C(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(void*);
    extern void fn_800D85D4(s32, s32);
    extern void fn_800D67BC(s32);
    extern void fn_800D61E4(s32, s32);
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D59B8(s32, f32, f32);
    extern void fn_800D6728(void);
    s32 ix;
    u32 obj;
    u32 p;
    s32 v;
    f32 col;

    v = *(s16*)(arg1 + 0x6);
    if (v == 0xd3d || v >= 0xd3d || v < 0xd3c) {
        return;
    }
    p = *(u32*)&lbl_8047A610;
    ix = (s32)*(f32*)(p + 0x18C);
    obj = fn_80109934(p + 0x144);
    if (obj == 0) {
        return;
    }
    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D6A00(7);
    fn_800D7820(&lbl_80314F98);
    fn_800D85D4(0, obj);
    fn_800D67BC(2);
    fn_800D61E4(0, 0);
    fn_800D5CB8(0, 0x28, 0x3e, 0xc8, ix);
    col = *(f32*)&lbl_8047C0A8;
    fn_800D59B8(0, col, col);
    fn_800D61E4(*(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56));
    fn_800D5CB8(0, 0x28, 0x3e, 0xc8, ix);
    col = *(f32*)&lbl_8047C0AC;
    fn_800D59B8(0, col, col);
    fn_800D6728();
}

/* 0x80075638 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80075638(void) {
    extern s32 fn_80102620(s32);
    return fn_80102620(0xd8);
}
#pragma pop

/* 0x8007565C | size: 0x6C */
void fn_8007565C(void) {
    extern void fn_8010A420(u32);
    extern void fn_80102568(s32, s32, s32);
    extern u16 fn_800E202C(u32);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(u16);
    u16 handle;

    fn_8010A420(*(u32*)&lbl_8047A610 + 0x144);
    fn_80102568(0xd8, 0, 1);
    handle = fn_800E202C(*(u32*)&lbl_8047A610);
    if (handle != 0) {
        fn_800E24B0();
        fn_800E209C(handle);
    }
    *(u32*)&lbl_8047A610 = 0;
}

/* 0x800756C8 | size: 0x128 */
void fn_800756C8(void) {
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r30 = r3;
    ((void(*)(void))fn_800FF56C)();
    if (r3 == 0x43) {
        r3 = 0x1a0;
        r4 = 0x20;
        ((void(*)(void))fn_800E2C04)();
        tmp = r3 & 0xFFFF;
        if (tmp != 0) {
            ((void(*)(void))fn_800E27B0)();
        } else {

            r3 = 0x0;
        }
        *(u32*)&lbl_8047A610 = r3;
        r31 = r3 + 0xc;
        r3 = 0x0;
        r4 = 0x1;
        ((void(*)(void))fn_80135938)();
        r6 = r3;
        r3 = r31;
        r4 = r30;
        r5 = 0xa;
        ((void(*)(void))fn_801240C4)();
        f0 = *(f32*)&lbl_8047C0A8;
        r3 = *(u32*)&lbl_8047A610;
        *(f32*)((u8*)r3 + 0x4) = f0;
        r3 = *(u32*)&lbl_8047A610;
        *(f32*)((u8*)r3 + 0x8) = f0;
        r3 = *(u32*)&lbl_8047A610;
        f1 = *(f32*)((u8*)r3 + 0x4);
        ((void(*)(void))fn_800CE148)();
        f2 = (f32)f1;
        f1 = *(f32*)&lbl_8047C09C;
        f0 = *(f32*)&lbl_8047C098;
        r3 = *(u32*)&lbl_8047A610;
        f1 = f1 * f2 + f0;
        f0 = *(f32*)&lbl_8047C0A0;
        *(f32*)((u8*)r3 + 0x18C) = f1;
        r3 = *(u32*)&lbl_8047A610;
        f1 = *(f32*)((u8*)r3 + 0x18C);
        if (f1 > f0) {
            *(f32*)((u8*)r3 + 0x18C) = f0;
        }
        r4 = (u32)&lbl_802EF0A8;
        r3 = *(u32*)&lbl_8047A610;
        r4 = (u32)&lbl_802EF0A8;
        r5 = r4 + (0x1 << 16);
        r3 = r3 + 0x144;
        r4 = *(s16*)((u8*)r5 + 0x7296);
        r5 = *(s16*)((u8*)r5 + 0x7298);
        ((void(*)(void))fn_8010A5BC)();
        r4 = *(u32*)&lbl_8047A610;
        r3 = r4 + 0x144;
        r4 = r4 + 0xc;
        ((void(*)(void))fn_80109C88)();
        r3 = 0xd8;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        ((void(*)(void))fn_801026A4)();
    }
    return;
}

/* 0x800757F0 | size: 0x2C */
void fn_800757F0(void) {
    fn_801CB9D8(*(u32*)&lbl_8047A5D0);
    *(u32*)&lbl_8047A5D0 = 0;
}

