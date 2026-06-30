/**
 * @file late_game.c
 * @brief Late game code before SDK (0x800937F4-0x80097FFC)
 *
 * Address range: 0x800937F4 - 0x80098004
 * Total functions: 25
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8001D624();
extern void fn_8001DA60();
extern void fn_8001E58C();
extern void fn_8005D858();
extern void fn_8005D934();
extern void fn_8006AEEC();
extern void fn_80071E34();
extern void fn_80073690();
extern void fn_80073E84();
extern void fn_80073E8C();
extern void fn_80074324();
extern void fn_800745B4();
extern void fn_80089380();
extern void fn_800895A4();
extern void fn_80089C84();
extern void fn_80089CA8();
extern void fn_80089D30();
extern void fn_8009F7B4();
extern void fn_8009F890();
extern void fn_8009F9E8();
extern u32 fn_800A13F8();
extern void fn_800A1990();
extern void fn_800A257C(u32, u32);
extern void fn_800C46B0();
extern u64 fn_800C4CC0(s32, f32);
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
extern void _threadSwitch();
extern void fn_800F9318();
extern void fn_800F9EE4();
extern void fn_800FA280();
extern void fn_800FA444();
extern void fn_800FAEF8();
extern void fn_800FB680();
extern void fn_800FB8C8();
extern void fn_800FBB34();
extern void fn_800FF660();
extern void fn_800FF730();
extern void fn_80102510();
extern void fn_80102568();
extern void fn_80102620();
extern void fn_8010264C();
extern void fn_801026A4();
extern void fn_80103484();
extern void fn_801040F0();
extern void fn_80104160();
extern u8* fn_80104704(u32);
extern void fn_80105624();
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_80109220();
extern void fn_80109934();
/* ... and 47 more external functions */
extern u64 OSGetTime(void);
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047C1F0;
extern u8 lbl_8047C1F8;
extern u8 lbl_8047C200;
extern u8 lbl_8047C204;
extern u8 lbl_8047C208;
extern u8 lbl_8047C20C;
extern u8 lbl_8047C210;
extern u8 lbl_8047C214;
extern u8 lbl_8047C218;
extern u8 lbl_8047C21C;
extern u8 lbl_8047C220;
extern u8 lbl_8047C228;
extern u8 lbl_8047C230;
extern u8 lbl_8047C234;
extern u8 lbl_8047C238;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EECF0[];
extern u8 jumptable_802EF01C[];
extern u8 jumptable_802EF038[];
extern u8 jumptable_802EF05C[];
extern u8 jumptable_802EF080[];
extern u8 lbl_8026F5C0[];
extern u8 lbl_8026F5E4[];
extern u8 lbl_802EED28[];
extern u8 lbl_802EED44[];
extern u8 lbl_802EEEC4[];
extern u8 lbl_802EEFC4[];
extern u8 lbl_802EEFD8[];
extern u8 lbl_802EF000[];
extern u8 lbl_80314F98[];
extern u8 lbl_803FB338[];
extern u8 lbl_803FB380[];

/* ===== Forward declarations ===== */
s32 fn_800937F4(void* arg0);
void fn_80093B04(u32 a, u32 b);
void fn_80093B4C(void);
void fn_80093F2C(void);
void fn_80093F64(void);
void fn_80094650(void);
void fn_8009567C(void);
void fn_800965C8(void);
void fn_80096C48(u32 unused, u8* dst);
void fn_80096D54(void* arg0);
void fn_80096FA0(void* arg0);
void fn_800973EC(void);
void fn_8009769C(void);
void fn_800979EC(void);
void fn_80097A38(void);
void fn_80097B04(void);
s32 fn_80097BBC(u8 chan);
void fn_80097CD0(void);
void fn_80097D94(void);
void fn_80097E58(void);
void fn_80097F08(void);
void fn_80097FCC(void);
void fn_80097FD0(void);
void fn_80097FF8(void);
void PPCMfmsr(void);

/* ===== Function implementations ===== */

/* 0x800937F4 | size: 0x310 */
#pragma peephole off
s32 fn_800937F4(void* arg0) {
    extern void fn_8009F7B4();
    extern void fn_8009F9E8();
    extern void fn_8009F890();
    extern int fn_80073E8C();
    extern int fn_80073E84();
    extern int fn_800A13F8();
    extern void fn_800A257C();
    extern void fn_800A1990();
    extern int fn_80074324();
    extern int fn_800745B4();
    extern int fn_80073690();
    extern void fn_800895A4();
    extern int fn_80071E34();
    extern void fn_80089380();
    extern int fn_80089D30();
    extern int fn_80089CA8();
    extern int fn_80089C84();
    u8* p;
    s32 result;
    s32 status;
    s32 state;
    void* arg;
    u64 start;
    u64 now;
    f32 rate;
    u8 buf_8[0xd8];
    u8 buf_e0[0x278];

    p = (u8*)arg0;
    result = 0;
    for (;;) {
        fn_8009F7B4(p);
        if (*(s32*)(p + 0x4340) != 0xd) {
            *(s32*)(p + 0x433c) = result;
            *(s32*)(p + 0x4340) = 0;
            while (*(s32*)(p + 0x4340) == 0) {
                fn_8009F9E8(p + 0x18, p);
            }
        }
        state = *(s32*)(p + 0x4340);
        fn_8009F890(p);
        status = 0;
        switch (state) {
            case 0:
            case 3:
                break;
            case 1:
                if ((s8)p[0x43c4] != 0) {
                    arg = p + 0x43c4;
                } else {
                    arg = 0;
                }
                status = fn_80073E8C(p + 0x4344, arg);
                if (status == 0) {
                    while (fn_80073E84() == 0) {
                        fn_800A257C(fn_800A13F8(), 0x10);
                        fn_800A1990();
                    }
                    result = 1;
                }
                break;
            case 2:
                start = OSGetTime();
                if (*(s32*)(p + 0x4344) == 0) {
                    rate = *(f32*)&lbl_8047C1F0;
                } else {
                    rate = *(f32*)&lbl_8047C1F0;
                }
                for (;;) {
                    status = fn_80074324(*(s32*)(p + 0x4338));
                    if (status != 0) {
                        if (status == 0x3e8) {
                            break;
                        }
                        now = OSGetTime();
                        if ((s32)(u32)fn_800C4CC0((s32)((now - start) >> 32),
                                rate * (f32)(*(u32*)0x800000F8 >> 2))
                            > (s32)((u32)now - (u32)start)) {
                            fn_800A257C(fn_800A13F8(), 0x10);
                            fn_800A1990();
                        } else {
                            result = 0x20002;
                            break;
                        }
                    } else {
                        status = fn_800745B4(*(s32*)(p + 0x4338), *(s32*)(p + 0x4344));
                        if (status == 0) {
                            result = 2;
                        }
                        break;
                    }
                }
                break;
            case 4:
                status = fn_80073690(*(s32*)(p + 0x4338), buf_e0);
                if (status == 0) {
                    fn_800895A4(*(s32*)(p + 0x4344), buf_e0);
                    result = 4;
                    *(*(u32**)(p + 0x4348)) =
                        (buf_e0[3] << 24) | (buf_e0[2] << 16) | (buf_e0[1] << 8) | buf_e0[0];
                }
                break;
            case 5:
                result = 5;
                break;
            case 6:
                result = 6;
                break;
            case 7:
                result = 7;
                break;
            case 8:
                result = 8;
                break;
            case 9:
                result = 9;
                break;
            case 10:
                result = 0xa;
                break;
            case 11:
                status = fn_80071E34(*(s32*)(p + 0x4338), buf_8);
                if (status == 0) {
                    fn_80089380(*(s32*)(p + 0x4344), buf_8);
                    result = 0xb;
                }
                break;
            case 12:
                status = fn_80089D30(*(s32*)(p + 0x4338) + 1, p + 0x4344);
                if (status == 0) {
                    for (;;) {
                        status = fn_80089CA8(*(s32*)(p + 0x4338) + 1);
                        if (status == 0) {
                            status = fn_80089C84(*(s32*)(p + 0x4338) + 1);
                        }
                        if (status >= 0) {
                            break;
                        }
                        fn_800A257C(fn_800A13F8(), 0x10);
                        fn_800A1990();
                    }
                    if (status == 0) {
                        result = 0xc;
                    }
                }
                break;
            case 13:
                return 0;
            }
        if (status != 0) {
            result = (state & 0xffff) | 0x10000;
        }
    }
}
#pragma peephole reset

/* 0x80093B04 | size: 0x48 */
void fn_80093B04(u32 a, u32 b) {
    u32 r31;
    u32 result;
    r31 = b;
    result = fn_800A13F8();
    if (r31 != 0) {
        if (r31 != result) return;
    }
    fn_800A257C(result, 0x10);
    fn_800A1990();
    return;
}

/* 0x80093B4C | size: 0x3E0 */
void fn_80093B4C(void) {
    extern void fn_80132A38();
    extern void fn_80265F14();
    u8 sp[0x40];
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
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    r31 = r3;
    r21 = r4;
    r3 = 0x53;
    ((void(*)(void))fn_80104704)();
    if (r3 == 0) return;
    r3 = (u32)&lbl_803FB380;
    r30 = (u32)&lbl_803FB380;
    tmp = *(u32*)((u8*)r30 + 0xC);
    if (tmp == 0) return;
    tmp = *(s16*)((u8*)r21 + 0x6);
    r3 = -0x100;
    r4 = *(u8*)((u8*)r31 + 0x8B);
    r23 = r4 | r3;
    if ((s32)tmp != 0x1f7) {
    do {
        if ((s32)tmp < 0x1f7) {
            if ((s32)tmp < 0x1f6) {
                return;
            }
            if ((s32)tmp != 0x1291) {
                return;
            }
            r4 = *(u32*)((u8*)r30 + 0x1C);
            r3 = 0x34;
            fn_80132A38();
            r5 = *(s16*)((u8*)r21 + 0x54);
            r7 = r23;
            r6 = *(s16*)((u8*)r21 + 0x56);
            r3 = 0x0;
            r4 = 0x0;
            r8 = 0xdd;
            ((void(*)(void))fn_800FBB34)();
            return;
                }
        r7 = *(u8*)((u8*)r30 + 0x1A);
        r3 = (u32)&lbl_8026F5E4;
        r6 = (u32)&lbl_8026F5E4;
        r4 = 0xc8;
        r3 = 0x810000;
        r7 = (s8)r7;
        r3 = 0x78;
        ((void(*)(void))fn_800FAEF8)();
        r3 = (u32)&lbl_803FB380;
        r5 = (u32)&lbl_803FB380;
        tmp = *(u8*)((u8*)r5 + 0x1);
        if (tmp != 6) return;
        r6 = *(u8*)((u8*)r30 + 0x1A);
        r3 = 0x38E40000;
        r6 = (s8)r6;
        tmp = (s32)((s64)tmp * (s64)r6 >> 32);
        r3 = (s32)tmp >> 1;
        r4 = (u32)r3 >> 31;
        tmp = (s32)tmp >> 1;
        r3 = r3 + r4;
        r4 = r3 * 0x9;
        r3 = (u32)tmp >> 31;
        tmp = tmp + r3;
        r3 = r6 - r4;
        r3 = r3 << 2;
        r3 = r5 + r3;
        r3 = r3 + tmp;
        r24 = *(u8*)((u8*)r3 + 0x20);
        r24 = (s8)r24;
        if ((s32)r24 < 0) {
            if (r24 >= 0x20) return;
        }
        r3 = (u32)&lbl_8026F5E4;
        r4 = 0x810000;
        r6 = (u32)&lbl_8026F5E4;
        r7 = r24;
        r3 = 0x78;
        r4 = 0xc8;
        ((void(*)(void))fn_800FAEF8)();
        r4 = r24 * 0xc;
        r3 = (u32)&lbl_802EED44;
        tmp = (u32)&lbl_802EED44;
        r3 = tmp + r4;
        r3 = r3 + 0x8;
        r3 = *(u32*)((u8*)r3 + 0x0);
        if ((s32)r3 != (s32)-0x1) {
            if ((s32)r3 < (s32)-0x1 || (s32)r3 >= 7) break;

            fn_80265F14();
            r3 = r3 & 0xFF;
            if (r3 != 0) {
                r3 = (u32)&lbl_802EEEC4;
                tmp = tmp << 2;
                r3 = (u32)&lbl_802EEEC4;
                r22 = *(u32*)(r3 + tmp);
                break;
            }
            r22 = 0x0;
            break;
        }
        r3 = tmp + r4;
        r22 = *(u32*)((u8*)r3 + 0x4);
    } while (0);
        r5 = *(s16*)((u8*)r21 + 0x54);
        r7 = r23;
        r6 = *(s16*)((u8*)r21 + 0x56);
        r8 = r22;
        r3 = 0x0;
        r4 = 0x0;
        ((void(*)(void))fn_800FBB34)();
        return;
            }
    r26 = r30;
    r22 = 0x0;
    r27 = 0x0;
    do {
        r29 = r27;
        r28 = r26;
        r24 = 0x0;
        do {
            tmp = *(u8*)((u8*)r28 + 0x20);
            tmp = (s8)tmp;
            if ((s32)tmp >= 0) {
                r4 = tmp * 0xc;
                r3 = (u32)&lbl_802EED44;
                tmp = (u32)&lbl_802EED44;
                r25 = tmp + r4;
                r3 = *(u16*)((u8*)r25 + 0x2);
                ((void(*)(void))fn_8005D934)();
                tmp = *(u8*)((u8*)r30 + 0x1A);
                tmp = (s8)tmp;
                if ((s32)r29 != (s32)tmp) {
                    r5 = *(s16*)((u8*)r21 + 0x54);
                    r4 = 0x38E40000;
                    tmp = *(s16*)((u8*)r21 + 0x56);
                    r4 = r5 * r24;
                    r5 = *(s16*)((u8*)r3 + 0x6);
                    r6 = *(s16*)((u8*)r3 + 0x8);
                    r7 = r23;
                    r9 = *(u16*)((u8*)r25 + 0x0);
                    r8 = r31;
                    r3 = (s32)((s64)r10 * (s64)r4 >> 32);
                    r10 = 0x0;
                    r3 = (s32)r3 >> 1;
                    tmp = tmp * r22;
                    r4 = (u32)r3 >> 31;
                    r3 = r3 + r4;
                    tmp = (s32)tmp >> 2;
                    r3 = (s16)r3;
                    r4 = (s16)tmp;
                    ((void(*)(void))fn_80104160)();
            }
            }
            r29 = r29 + 0x1;
            r28 = r28 + 0x4;
            r24 = r24 + 0x1;
        } while ((s32)r24 < 9);
        r27 = r27 + 0x9;
        r26 = r26 + 0x1;
        r22 = r22 + 0x1;
    } while ((s32)r22 < 4);
    tmp = *(u8*)((u8*)r30 + 0x1A);
    r6 = (s8)tmp;
    if ((s32)r6 < 0) return;
    r4 = 0x38E40000;
    r3 = (u32)&lbl_803FB380;
    tmp = (s32)((s64)tmp * (s64)r6 >> 32);
    r3 = (u32)&lbl_803FB380;
    r4 = (s32)tmp >> 1;
    r5 = (u32)r4 >> 31;
    tmp = (s32)tmp >> 1;
    r4 = r4 + r5;
    r5 = r4 * 0x9;
    r4 = (u32)tmp >> 31;
    r24 = tmp + r4;
    r22 = r6 - r5;
    tmp = r22 << 2;
    tmp = r3 + tmp;
    r3 = tmp + r24;
    tmp = *(u8*)((u8*)r3 + 0x20);
    tmp = (s8)tmp;
    if ((s32)tmp < 0) return;
    r25 = tmp * 0xc;
    r3 = (u32)&lbl_802EED44;
    tmp = (u32)&lbl_802EED44;
    r3 = tmp + r25;
    r3 = *(u16*)((u8*)r3 + 0x2);
    ((void(*)(void))fn_8005D934)();
    r6 = *(s16*)((u8*)r21 + 0x54);
    r5 = 0x38E40000;
    tmp = *(s16*)((u8*)r21 + 0x56);
    r4 = (u32)&lbl_802EED44;
    r6 = r6 * r22;
    r21 = (u32)&lbl_802EED44;
    r30 = r3;
    r3 = *(u16*)(r21 + r25);
    r4 = (s32)((s64)r5 * (s64)r6 >> 32);
    r4 = (s32)r4 >> 1;
    tmp = tmp * r24;
    r5 = (u32)r4 >> 31;
    r4 = r4 + r5;
    tmp = (s32)tmp >> 2;
    r22 = (s16)r4;
    r24 = (s16)tmp;
    ((void(*)(void))fn_8005D858)();
    r5 = *(s16*)((u8*)r3 + 0xC);
    r7 = r23;
    r4 = *(s16*)((u8*)r30 + 0x6);
    r8 = r31;
    r6 = *(s16*)((u8*)r3 + 0xE);
    r10 = 0x0;
    tmp = *(s16*)((u8*)r30 + 0x8);
    r3 = r5 - r4;
    r11 = (s16)r3;
    r9 = *(u16*)(r21 + r25);
    tmp = r6 - tmp;
    r4 = (u32)r11 >> 31;
    r3 = (s16)tmp;
    r4 = r4 + r11;
    tmp = (u32)r3 >> 31;
    r4 = (s32)r4 >> 1;
    tmp = tmp + r3;
    tmp = (s32)tmp >> 1;
    r3 = r22 - r4;
    tmp = r24 - tmp;
    r22 = (s16)r3;
    r24 = (s16)tmp;
    r3 = r22;
    r4 = r24;
    ((void(*)(void))fn_80104160)();

    return;
}

/* 0x80093F2C | size: 0x38 */
#pragma push
#pragma scheduling off
void fn_80093F2C(void) {
    extern void fn_80093F64();
    u8 *r4 = (u8*)&lbl_803FB380;
    u32 r3 = *(u32*)(r4 + 0xC);

    if (r3 != 0) {
        fn_80093F64(r3, r4 + 0x1c);
    }
    return;
}
#pragma pop

/* 0x80093F64 | size: 0x6EC */
void fn_80093F64(void) {
    extern void fn_8012640C();
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

    r30 = r3;
    r31 = r4;
    tmp = -0x1;
    r3 = 0x0;
    if ((s32)r3 < 9) {
        *(u8*)((u8*)r31 + 0x4) = tmp;
        *(u8*)((u8*)r31 + 0x8) = tmp;
        *(u8*)((u8*)r31 + 0xC) = tmp;
        *(u8*)((u8*)r31 + 0x10) = tmp;
        *(u8*)((u8*)r31 + 0x14) = tmp;
        *(u8*)((u8*)r31 + 0x18) = tmp;
        *(u8*)((u8*)r31 + 0x1C) = tmp;
        *(u8*)((u8*)r31 + 0x20) = tmp;
        *(u8*)((u8*)r31 + 0x24) = tmp;
    }
    r3 = 0x0;
    if ((s32)r3 < 9) {
        *(u8*)((u8*)r31 + 0x5) = tmp;
        *(u8*)((u8*)r31 + 0x9) = tmp;
        *(u8*)((u8*)r31 + 0xD) = tmp;
        *(u8*)((u8*)r31 + 0x11) = tmp;
        *(u8*)((u8*)r31 + 0x15) = tmp;
        *(u8*)((u8*)r31 + 0x19) = tmp;
        *(u8*)((u8*)r31 + 0x1D) = tmp;
        *(u8*)((u8*)r31 + 0x21) = tmp;
        *(u8*)((u8*)r31 + 0x25) = tmp;
    }
    r3 = 0x0;
    if ((s32)r3 < 9) {
        *(u8*)((u8*)r31 + 0x6) = tmp;
        *(u8*)((u8*)r31 + 0xA) = tmp;
        *(u8*)((u8*)r31 + 0xE) = tmp;
        *(u8*)((u8*)r31 + 0x12) = tmp;
        *(u8*)((u8*)r31 + 0x16) = tmp;
        *(u8*)((u8*)r31 + 0x1A) = tmp;
        *(u8*)((u8*)r31 + 0x1E) = tmp;
        *(u8*)((u8*)r31 + 0x22) = tmp;
        *(u8*)((u8*)r31 + 0x26) = tmp;
    }
    r3 = 0x0;
    if ((s32)r3 < 9) {
        *(u8*)((u8*)r31 + 0x7) = tmp;
        *(u8*)((u8*)r31 + 0xB) = tmp;
        *(u8*)((u8*)r31 + 0xF) = tmp;
        *(u8*)((u8*)r31 + 0x13) = tmp;
        *(u8*)((u8*)r31 + 0x17) = tmp;
        *(u8*)((u8*)r31 + 0x1B) = tmp;
        *(u8*)((u8*)r31 + 0x1F) = tmp;
        *(u8*)((u8*)r31 + 0x23) = tmp;
        *(u8*)((u8*)r31 + 0x27) = tmp;
    }
    r3 = (u32)&lbl_802EEFD8;
    r28 = 0x0;
    r29 = (u32)&lbl_802EEFD8;
    r27 = 0x0;
    do {
        r5 = *(u16*)((u8*)r29 + 0x0);
        r3 = r30;
        r4 = 0x0;
        r6 = 0x0;
        fn_8012640C();
        tmp = *(u8*)((u8*)r29 + 0x3);
        if ((s32)r3 > (s32)tmp) {
            r3 = tmp;
        }
        r5 = 0x0;
        if ((s32)r3 > 0) {
            if ((s32)r3 > 8) {
                r4 = r29 + 0x2;
                r6 = r8 + 0x7;
                r7 = 0x38E40000;
                r6 = (u32)r6 >> 3;
                ctr_fn = (void(*)(void))r6;
                if ((s32)r8 > 0) {
                    do {
                        r7 = (s32)((s64)tmp * (s64)r28 >> 32);
                        r12 = r28 + 0x1;
                        r6 = *(u8*)((u8*)r4 + 0x0);
                        r11 = r28 + 0x2;
                        r26 = r28 + 0x3;
                        r6 = (s8)r6;
                        r8 = (s32)r7 >> 1;
                        r6 = r5 + r6;
                        r9 = (u32)r8 >> 31;
                        r7 = (s32)r7 >> 1;
                        r9 = r8 + r9;
                        r8 = r28 + 0x4;
                        r10 = r9 * 0x9;
                        r9 = (u32)r7 >> 31;
                        r25 = (s8)r6;
                        r24 = r7 + r9;
                        r7 = (s32)((s64)tmp * (s64)r12 >> 32);
                        r6 = r28 - r10;
                        r9 = r6 << 2;
                        r6 = (s32)r7 >> 1;
                        r9 = r9 + r24;
                        r10 = r9 + 0x4;
                        r7 = (s32)r7 >> 1;
                        r9 = (u32)r6 >> 31;
                        *(u8*)(r31 + r10) = r25;
                        r6 = r6 + r9;
                        r9 = (u32)r7 >> 31;
                        r10 = r6 * 0x9;
                        r6 = *(u8*)((u8*)r4 + 0x0);
                        r25 = r7 + r9;
                        r7 = (s8)r6;
                        r6 = (s32)((s64)tmp * (s64)r11 >> 32);
                        r10 = r12 - r10;
                        r9 = r5 + r7;
                        r7 = r10 << 2;
                        r12 = r9 + 0x1;
                        r9 = (s32)r6 >> 1;
                        r7 = r7 + r25;
                        r6 = (s32)r6 >> 1;
                        r10 = (u32)r9 >> 31;
                        r25 = (s8)r12;
                        r12 = r7 + 0x4;
                        r7 = (u32)r6 >> 31;
                        *(u8*)(r31 + r12) = r25;
                        r9 = r9 + r10;
                        r10 = r9 * 0x9;
                        r12 = r6 + r7;
                        r6 = *(u8*)((u8*)r4 + 0x0);
                        r9 = (s32)((s64)tmp * (s64)r26 >> 32);
                        r6 = (s8)r6;
                        r7 = r11 - r10;
                        r6 = r5 + r6;
                        r7 = r7 << 2;
                        r11 = r6 + 0x2;
                        r6 = (s32)r9 >> 1;
                        r10 = r7 + r12;
                        r7 = (u32)r6 >> 31;
                        r12 = (s32)r9 >> 1;
                        r9 = (s32)((s64)tmp * (s64)r8 >> 32);
                        r11 = (s8)r11;
                        r10 = r10 + 0x4;
                        *(u8*)(r31 + r10) = r11;
                        r6 = r6 + r7;
                        r25 = (u32)r12 >> 31;
                        r11 = r6 * 0x9;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r6 = (s32)r9 >> 1;
                        r10 = (s8)r7;
                        r24 = r26 - r11;
                        r7 = (u32)r6 >> 31;
                        r11 = r5 + r10;
                        r12 = r12 + r25;
                        r10 = r24 << 2;
                        r6 = r6 + r7;
                        r11 = r11 + 0x3;
                        r7 = r10 + r12;
                        r6 = r6 * 0x9;
                        r10 = (s8)r11;
                        r7 = r7 + 0x4;
                        *(u8*)(r31 + r7) = r10;
                        r11 = r8 - r6;
                        r25 = r28 + 0x5;
                        r6 = *(u8*)((u8*)r4 + 0x0);
                        r8 = (s32)((s64)tmp * (s64)r25 >> 32);
                        r7 = (s32)r9 >> 1;
                        r6 = (s8)r6;
                        r10 = (u32)r7 >> 31;
                        r9 = r5 + r6;
                        r12 = r7 + r10;
                        r6 = (s32)r8 >> 1;
                        r7 = r11 << 2;
                        r10 = r9 + 0x4;
                        r11 = r28 + 0x6;
                        r9 = r7 + r12;
                        r7 = (u32)r6 >> 31;
                        r6 = r6 + r7;
                        r10 = (s8)r10;
                        r7 = r9 + 0x4;
                        r8 = (s32)r8 >> 1;
                        *(u8*)(r31 + r7) = r10;
                        r12 = r6 * 0x9;
                        r10 = (u32)r8 >> 31;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r9 = r28 + 0x7;
                        r6 = (s32)((s64)tmp * (s64)r11 >> 32);
                        r26 = r8 + r10;
                        r10 = r25 - r12;
                        r8 = (s8)r7;
                        r7 = r10 << 2;
                        r10 = r5 + r8;
                        r8 = (s32)r6 >> 1;
                        r12 = r10 + 0x5;
                        r7 = r7 + r26;
                        r6 = (s32)r6 >> 1;
                        r10 = (u32)r8 >> 31;
                        r25 = (s8)r12;
                        r12 = r7 + 0x4;
                        r7 = (u32)r6 >> 31;
                        *(u8*)(r31 + r12) = r25;
                        r8 = r8 + r10;
                        r8 = r8 * 0x9;
                        r10 = r6 + r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r28 = r28 + 0x8;
                        r6 = (s32)((s64)tmp * (s64)r9 >> 32);
                        r7 = (s8)r7;
                        r8 = r11 - r8;
                        r7 = r5 + r7;
                        r8 = r8 << 2;
                        r11 = r7 + 0x6;
                        r7 = (s32)r6 >> 1;
                        r10 = r8 + r10;
                        r8 = (u32)r7 >> 31;
                        r6 = (s32)r6 >> 1;
                        r8 = r7 + r8;
                        r11 = (s8)r11;
                        r10 = r10 + 0x4;
                        r7 = (u32)r6 >> 31;
                        *(u8*)(r31 + r10) = r11;
                        r8 = r8 * 0x9;
                        r6 = r6 + r7;
                        r10 = *(u8*)((u8*)r4 + 0x0);
                        r7 = r9 - r8;
                        r8 = (s8)r10;
                        r8 = r5 + r8;
                        r7 = r7 << 2;
                        r8 = r8 + 0x7;
                        r5 = r5 + 0x8;
                        r6 = r7 + r6;
                        r7 = (s8)r8;
                        r6 = r6 + 0x4;
                        *(u8*)(r31 + r6) = r7;
                    } while (--ctr != 0);
            }
            }
            r8 = r29 + 0x2;
            r4 = 0x38E40000;
            tmp = r3 - r5;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r5 < (s32)r3) {
                do {
                    tmp = (s32)((s64)r6 * (s64)r28 >> 32);
                    r3 = *(u8*)((u8*)r8 + 0x0);
                    r3 = (s8)r3;
                    r7 = r5 + r3;
                    r3 = (s32)tmp >> 1;
                    r5 = r5 + 0x1;
                    r4 = (u32)r3 >> 31;
                    tmp = (s32)tmp >> 1;
                    r3 = r3 + r4;
                    r7 = (s8)r7;
                    r4 = r3 * 0x9;
                    r3 = (u32)tmp >> 31;
                    tmp = tmp + r3;
                    r3 = r28 - r4;
                    r28 = r28 + 0x1;
                    r3 = r3 << 2;
                    r3 = r3 + tmp;
                    tmp = r3 + 0x4;
                    *(u8*)(r31 + tmp) = r7;
                } while (--ctr != 0);
        }
        }
        r29 = r29 + 0x4;
        r27 = r27 + 0x1;
    } while (r27 < 0xa);
    r3 = (u32)&lbl_802EF000;
    r28 = 0x0;
    r29 = (u32)&lbl_802EF000;
    r27 = r28;
    do {
        r5 = *(u16*)((u8*)r29 + 0x0);
        r3 = r30;
        r4 = 0x0;
        r6 = 0x0;
        fn_8012640C();
        tmp = *(u8*)((u8*)r29 + 0x3);
        if ((s32)r3 > (s32)tmp) {
            r3 = tmp;
        }
        r5 = 0x0;
        if ((s32)r3 > 0) {
            if ((s32)r3 > 8) {
                r4 = r29 + 0x2;
                tmp = r7 + 0x7;
                r6 = r28 << 2;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r7 > 0) {
                    do {
                        tmp = *(u8*)((u8*)r4 + 0x0);
                        r9 = r6 + 0x7;
                        r8 = r28 + 0x1;
                        r7 = r28 + 0x2;
                        r10 = (s8)tmp;
                        tmp = r28 + 0x3;
                        r10 = r5 + r10;
                        r26 = r8 << 2;
                        r8 = (s8)r10;
                        r12 = r7 << 2;
                        *(u8*)(r31 + r9) = r8;
                        r11 = tmp << 2;
                        r7 = r28 + 0x4;
                        tmp = r28 + 0x5;
                        r8 = *(u8*)((u8*)r4 + 0x0);
                        r10 = r7 << 2;
                        r9 = tmp << 2;
                        r7 = r28 + 0x6;
                        r8 = (s8)r8;
                        tmp = r28 + 0x7;
                        r25 = r5 + r8;
                        r8 = r7 << 2;
                        r25 = r25 + 0x1;
                        r7 = tmp << 2;
                        r25 = (s8)r25;
                        tmp = r26 + 0x7;
                        *(u8*)(r31 + tmp) = r25;
                        r12 = r12 + 0x7;
                        r11 = r11 + 0x7;
                        r10 = r10 + 0x7;
                        r26 = *(u8*)((u8*)r4 + 0x0);
                        r9 = r9 + 0x7;
                        r8 = r8 + 0x7;
                        tmp = r7 + 0x7;
                        r7 = (s8)r26;
                        r28 = r28 + 0x8;
                        r7 = r5 + r7;
                        r6 = r6 + 0x20;
                        r7 = r7 + 0x2;
                        r7 = (s8)r7;
                        *(u8*)(r31 + r12) = r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r7 = (s8)r7;
                        r7 = r5 + r7;
                        r7 = r7 + 0x3;
                        r7 = (s8)r7;
                        *(u8*)(r31 + r11) = r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r7 = (s8)r7;
                        r7 = r5 + r7;
                        r7 = r7 + 0x4;
                        r7 = (s8)r7;
                        *(u8*)(r31 + r10) = r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r7 = (s8)r7;
                        r7 = r5 + r7;
                        r7 = r7 + 0x5;
                        r7 = (s8)r7;
                        *(u8*)(r31 + r9) = r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r7 = (s8)r7;
                        r7 = r5 + r7;
                        r7 = r7 + 0x6;
                        r7 = (s8)r7;
                        *(u8*)(r31 + r8) = r7;
                        r7 = *(u8*)((u8*)r4 + 0x0);
                        r7 = (s8)r7;
                        r7 = r5 + r7;
                        r5 = r5 + 0x8;
                        r7 = r7 + 0x7;
                        r7 = (s8)r7;
                        *(u8*)(r31 + tmp) = r7;
                    } while (--ctr != 0);
            }
            }
            tmp = r28 << 2;
            r6 = r29 + 0x2;
            r4 = r31 + tmp;
            tmp = r3 - r5;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r5 < (s32)r3) {
                do {
                    tmp = *(u8*)((u8*)r6 + 0x0);
                    r28 = r28 + 0x1;
                    tmp = (s8)tmp;
                    tmp = r5 + tmp;
                    r5 = r5 + 0x1;
                    tmp = (s8)tmp;
                    *(u8*)((u8*)r4 + 0x7) = tmp;
                    r4 = r4 + 0x4;
                } while (--ctr != 0);
        }
        }
        r29 = r29 + 0x4;
        r27 = r27 + 0x1;
    } while (r27 < 7);
    r5 = 0x0;
    r4 = r5;
    tmp = 0x4;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = r31 + r4;
        tmp = *(u8*)((u8*)r3 + 0x4);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x8);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0xC);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x10);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x14);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x18);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x1C);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x20);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        tmp = *(u8*)((u8*)r3 + 0x24);
        tmp = (s8)tmp;
        if ((s32)tmp >= 0) {
            r5 = r5 + 0x1;
        }
        r4 = r4 + 0x1;
    } while (--ctr != 0);
    *(u32*)((u8*)r31 + 0x0) = r5;
    return;
}

/* 0x80094650 | size: 0x102C */
void fn_80094650(void) {
    extern void fn_8010C46C();
    extern void fn_8011BEB4();
    extern void fn_80123CD4();
    extern void fn_80123E70();
    extern void fn_8012640C();
    extern void fn_80132A38();
    extern void fn_801EE034();
    extern void fn_801EE04C();
    extern void fn_801EE064();
    extern void fn_801EE07C();
    extern void fn_801EE0A8();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    r27 = r3;
    r31 = r4;
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r29 = *(u32*)((u8*)r3 + 0xC);
    if (r29 == 0) return;
    tmp = *(s16*)((u8*)r31 + 0x6);
    r30 = 0x1;
    if ((s32)tmp < 0x1b8) {
        if ((s32)tmp < 0x18b) {
            if ((s32)tmp >= 0x182) goto L_8009473C;
            if ((s32)tmp < 0x170) {
                goto L_8009473C;
            }
            if ((s32)tmp < 0x191) {
            }
            goto L_800946D0;
        }
        if ((s32)tmp < 0x1d3) {
        }
        if ((s32)tmp < 0x1ca) {

        } else {
        }
        if ((s32)tmp >= 0x1d9) goto L_8009473C;
    }
L_800946D0:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 7) {
        if ((s32)tmp >= 7 || (s32)tmp >= 5) goto L_8009472C;

        if ((s32)tmp < 3) {
            goto L_8009472C;
        }
        }
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x2);
    tmp = (s8)tmp;
    if ((s32)tmp >= 0 || (s32)tmp > 4) {

        r30 = 0x1;
        goto L_80094730;
    }
    r30 = 0x0;
    goto L_80094730;
L_8009472C:
    r30 = 0x0;
L_80094730:
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_80109220)();
L_8009473C:
    tmp = r30 & 0xFF;
    if (tmp == 0) return;
    r4 = *(s16*)((u8*)r31 + 0x6);
    tmp = -0x100;
    r3 = *(u8*)((u8*)r27 + 0x8B);
    r30 = r3 | tmp;
    if ((s32)r4 < 0x1c1) {
        if ((s32)r4 != 0x18d) {
            if ((s32)r4 < 0x18d) {
                if ((s32)r4 == 0x181) return;
                if ((s32)r4 < 0x181) {
                    if ((s32)r4 == 0x170) goto L_80094F3C;
                    if ((s32)r4 < 0x170) return;
                    if ((s32)r4 >= 0x179) goto L_800952B4;
                    goto L_80095490;
                }
                if ((s32)r4 == 0x18b) goto L_80095010;
                if ((s32)r4 >= 0x18b) goto L_800950A8;
                if ((s32)r4 >= 0x187) return;
                goto L_800949F4;
            }
            if ((s32)r4 < 0x196) {
                if ((s32)r4 == 0x190) return;
                if ((s32)r4 >= 0x190) goto L_80094B58;
                if ((s32)r4 < 0x18f) return;

            }
            if ((s32)r4 == 0x1b8) goto L_80094F3C;
            if ((s32)r4 >= 0x1b8) goto L_80095490;
            if ((s32)r4 >= 0x19b) return;
            goto L_80094CB8;
        }
        if ((s32)r4 == 0x1d7) goto L_800951F4;
        if ((s32)r4 < 0x1d7) {
            if ((s32)r4 == 0x1d3) goto L_80095010;
            if ((s32)r4 < 0x1d3) {
                if ((s32)r4 == 0x1c9) return;
                if ((s32)r4 < 0x1c9) goto L_800952B4;
                if ((s32)r4 >= 0x1ce) return;
                goto L_800949F4;
            }
        }
        if ((s32)r4 == 0x1d5) goto L_80095134;
        if ((s32)r4 >= 0x1d5) return;
        goto L_800950A8;
    }
    if ((s32)r4 < 0x59b) {
        if ((s32)r4 < 0x1dd) {
            if ((s32)r4 < 0x1d9) return;

        }
        if ((s32)r4 >= 0x1e1) return;
        goto L_80094CB8;
    }
    if ((s32)r4 < 0x12b3) {
        if ((s32)r4 >= 0x59f) return;
    } else {

        if ((s32)r4 >= 0x12b8) return;
    }
    r3 = *(u32*)&lbl_8047C200;
    tmp = *(u32*)&lbl_8047C204;
    *(u32*)(sp + 0x14) = tmp;
    if ((s32)r4 != 0x12b3) {
        if ((s32)r4 < 0x12b3) {
            if ((s32)r4 != 0x59d) {
                if ((s32)r4 < 0x59d) {
                    if ((s32)r4 != 0x59b) {
                        if ((s32)r4 < 0x59b) {
                            goto L_80094910;
                        }
                        if ((s32)r4 >= 0x59f) goto L_80094910;
                        goto L_800948E4;
                    }
                    if ((s32)r4 == 0x12b6) goto L_8009490C;
                    if ((s32)r4 < 0x12b6) {
                        if ((s32)r4 >= 0x12b5) goto L_80094904;
                        goto L_800948FC;
                    }
                    if ((s32)r4 >= 0x12b8) goto L_80094910;
                    goto L_800948EC;
                        }
                r28 = 0x0;
                goto L_80094910;
                        }
            r28 = 0x1;
            goto L_80094910;
            }
        r28 = 0x2;
        goto L_80094910;
    L_800948E4:
        r28 = 0x3;
        goto L_80094910;
    L_800948EC:
        r28 = 0x0;
        goto L_80094910;
    }
    r28 = 0x1;
    goto L_80094910;
L_800948FC:
    r28 = 0x2;
    goto L_80094910;
L_80094904:
    r28 = 0x3;
    goto L_80094910;
L_8009490C:
    r28 = 0x4;
L_80094910:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 4) {
        if ((s32)tmp < 4) {
            if ((s32)tmp < 3) return;

        }
        if ((s32)tmp != 7) return;

    }
    r3 = *(u8*)((u8*)r3 + 0x3);
    tmp = (s8)r28;
    r3 = (s8)r3;
    if ((s32)r3 == (s32)tmp) {
        r7 = (u32)sp + 0x10;
        r3 = 0x0;
        r4 = 0x0;
        *(u32*)(sp + 0x10) = tmp;
        r5 = *(s16*)((u8*)r31 + 0x54);
        r6 = *(s16*)((u8*)r31 + 0x56);
        ((void(*)(void))fn_8001E58C)();
    }
    r3 = (u32)&lbl_803FB380;
    tmp = (s8)r28;
    r3 = (u32)&lbl_803FB380;
    r3 = *(u8*)((u8*)r3 + 0x2);
    r3 = (s8)r3;
    if ((s32)r3 != (s32)tmp) return;
    r7 = (u32)sp + 0xc;
    r3 = 0x0;
    r4 = 0x0;
    *(u32*)(sp + 0xC) = tmp;
    r5 = *(s16*)((u8*)r31 + 0x54);
    r6 = *(s16*)((u8*)r31 + 0x56);
    ((void(*)(void))fn_8001E58C)();
    return;

    r3 = (u32)&lbl_803FB380;
    tmp = (s8)r28;
    r3 = (u32)&lbl_803FB380;
    r3 = *(u8*)((u8*)r3 + 0x2);
    r3 = (s8)r3;
    if ((s32)r3 != (s32)tmp) return;
    r7 = (u32)sp + 0x8;
    r3 = 0x0;
    r4 = 0x0;
    *(u32*)(sp + 0x8) = tmp;
    r5 = *(s16*)((u8*)r31 + 0x54);
    r6 = *(s16*)((u8*)r31 + 0x56);
    ((void(*)(void))fn_8001E58C)();
    return;
L_800949F4:
do {
    if ((s32)r4 != 0x186) {
        if ((s32)r4 < 0x186) {
            if ((s32)r4 != 0x183) {
                if ((s32)r4 < 0x183) {
                    if ((s32)r4 < 0x182) {
                        break;
                    }
                    if ((s32)r4 < 0x185) {
                        goto L_80094A78;
                    }
                    if ((s32)r4 != 0x1cc) {
                        if ((s32)r4 < 0x1cc) {
                            if ((s32)r4 != 0x1ca) {
                                if ((s32)r4 < 0x1ca) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1ce) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094A78:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r30 = r28 & 0xFFFF;
    if (r30 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r28 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r28 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r30;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r28 = 0x0;
        }
    }
    tmp = r28 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094B14;
            }
            if ((s32)tmp >= 0x166) goto L_80094B14;
            goto L_80094B0C;
        }
            }
    tmp = 0x0;
    goto L_80094B34;
L_80094B0C:
    tmp = 0x5d;
    goto L_80094B34;
L_80094B14:
    r4 = r28;
    r3 = 0x0;
    r5 = 0x3;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFFFF;
    fn_8010C46C();
    tmp = r3 & 0xFFFF;
L_80094B34:
    if (tmp == 0) return;
    r5 = r27;
    r6 = tmp & 0xFFFF;
    r3 = 0x0;
    r4 = 0x0;
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80094B58:
do {
    if ((s32)r4 != 0x195) {
        if ((s32)r4 < 0x195) {
            if ((s32)r4 != 0x192) {
                if ((s32)r4 < 0x192) {
                    if ((s32)r4 < 0x191) {
                        break;
                    }
                    if ((s32)r4 < 0x194) {
                        goto L_80094BDC;
                    }
                    if ((s32)r4 != 0x1db) {
                        if ((s32)r4 < 0x1db) {
                            if ((s32)r4 != 0x1d9) {
                                if ((s32)r4 < 0x1d9) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1dd) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094BDC:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r28 = r28 & 0xFFFF;
    if (r28 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    tmp = r27 & 0xFFFF;
    if (tmp == 0) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be0;
        ((void(*)(void))fn_800FBB34)();
        return;
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 == 0) return;
    ((void(*)(void))fn_800FA280)();
    r4 = r3;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xe7;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80094CB8:
do {
    if ((s32)r4 != 0x19a) {
        if ((s32)r4 < 0x19a) {
            if ((s32)r4 != 0x197) {
                if ((s32)r4 < 0x197) {
                    if ((s32)r4 < 0x196) {
                        break;
                    }
                    if ((s32)r4 < 0x199) {
                        goto L_80094D3C;
                    }
                    if ((s32)r4 != 0x1df) {
                        if ((s32)r4 < 0x1df) {
                            if ((s32)r4 != 0x1dd) {
                                if ((s32)r4 < 0x1dd) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1e1) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094D3C:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r26 = r28 & 0xFFFF;
    if (r26 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r26;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r26;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r3 = 0x2bd4;
    ((void(*)(void))fn_800FA444)();
    r3 = (u32)r3 >> 16;
    tmp = *(s16*)((u8*)r31 + 0x54);
    r3 = (s16)r3;
    r5 = r30;
    r3 = tmp - r3;
    r4 = 0x0;
    tmp = (u32)r3 >> 31;
    r6 = 0x2bd4;
    tmp = tmp + r3;
    tmp = (s32)tmp >> 1;
    r25 = (s16)tmp;
    r3 = r25;
    ((void(*)(void))fn_800FB680)();
    tmp = r27 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094E7C;
            }
            if ((s32)tmp >= 0x166) goto L_80094E7C;
            goto L_80094E40;
        }
            }
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2be1;
    ((void(*)(void))fn_800FB8C8)();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2be1;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094E40:
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2b6d;
    ((void(*)(void))fn_800FB8C8)();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2b6d;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094E7C:
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x2;
        r6 = 0x0;
        fn_8011BEB4();
    } else {

        r3 = r29;
        r6 = r26;
        r4 = 0x0;
        r5 = 0x80;
        fn_8012640C();
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x2;
        r6 = 0x0;
        fn_8011BEB4();
    } else {

        r3 = r29;
        r4 = r28;
        fn_80123E70();
        r3 = r3 & 0xFF;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094F3C:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r30 = *(u8*)((u8*)r3 + 0x2);
    r30 = (s8)r30;
    tmp = r30 & 0xFFFF;
    if (tmp == 4) {
        r28 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r28 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r30;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r28 = 0x0;
        }
    }
    tmp = r28 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094FCC;
            }
            if ((s32)tmp >= 0x166) goto L_80094FCC;
            goto L_80094FC4;
        }
            }
    tmp = 0x0;
    goto L_80094FEC;
L_80094FC4:
    tmp = 0x5d;
    goto L_80094FEC;
L_80094FCC:
    r4 = r28;
    r3 = 0x0;
    r5 = 0x24;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFF;
    fn_801EE0A8();
    tmp = r3 & 0xFFFF;
L_80094FEC:
    if (tmp == 0) return;
    r5 = r27;
    r6 = tmp & 0xFFFF;
    r3 = 0x0;
    r4 = 0x0;
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80095010:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x23;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFFFF;
    fn_801EE07C();
    fn_801EE034();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r8 = r3;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800FBB34)();
    return;
L_800950A8:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x22;
    r6 = 0x0;
    fn_8011BEB4();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r8 = r3;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80095134:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x6;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 <= 1) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be2;
        ((void(*)(void))fn_800FB8C8)();
        return;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_800951F4:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x7;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 <= 1) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be2;
        ((void(*)(void))fn_800FB8C8)();
        return;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_800952B4:
do {
    if ((s32)r4 != 0x1c1) {
        if ((s32)r4 < 0x1c1) {
            if ((s32)r4 != 0x17d) {
                if ((s32)r4 < 0x17d) {
                    if ((s32)r4 != 0x17a) {
                        if ((s32)r4 < 0x17a) {
                            if ((s32)r4 < 0x179) {
                                break;
                            }
                            if ((s32)r4 < 0x17c) {
                                goto L_800953A4;
                            }
                            if ((s32)r4 != 0x180) {
                                if ((s32)r4 >= 0x180) break;
                                if ((s32)r4 < 0x17f) {
                                    goto L_8009538C;
                                }
                                if ((s32)r4 != 0x1c6) {
                                    if ((s32)r4 < 0x1c6) {
                                        if ((s32)r4 != 0x1c4) {
                                            if ((s32)r4 < 0x1c4) {
                                                if ((s32)r4 < 0x1c3) {
                                                    goto L_8009536C;
                                                }
                                                if ((s32)r4 != 0x1c8) {
                                                    if ((s32)r4 >= 0x1c8) break;

                                                } else {
                                                    r28 = 0x1;
                                                    break;
                                                }
                                                r28 = 0x2;
                                                break;
                                            }
                                            r28 = 0x3;
                                            break;
                                                }
                                        r28 = 0x4;
                                        break;
                                            }
                                    r28 = 0x5;
                                    break;
                                                }
                                r28 = 0x6;
                                break;
                            L_8009536C:
                                r28 = 0x7;
                                break;
                            }
                            r28 = 0x8;
                            break;
                                }
                        r28 = 0x1;
                        break;
                                }
                    r28 = 0x2;
                    break;
                L_8009538C:
                    r28 = 0x3;
                    break;
                        }
                r28 = 0x4;
                break;
                            }
            r28 = 0x5;
            break;
        L_800953A4:
            r28 = 0x6;
            break;
                        }
        r28 = 0x7;
        break;
                            }
    r28 = 0x8;
} while (0);
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r31 = *(u8*)((u8*)r3 + 0x2);
    r31 = (s8)r31;
    tmp = r31 & 0xFFFF;
    if (tmp == 4) {
        r30 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r31;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r30 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r30 = 0x0;
        }
    }
    tmp = r30 & 0xFFFF;
    if (tmp != 0) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x23;
        r6 = 0x0;
        fn_8011BEB4();
        r3 = r3 & 0xFFFF;
        fn_801EE07C();
        fn_801EE064();
        r4 = r3 & 0xFF;
    } else {

        r4 = 0x0;
    }
    r3 = 0x66660000;
    tmp = r28 & 0xFFFF;
    r3 = r3 + 0x6667;
    r5 = r27;
    r6 = (s32)((s64)r3 * (s64)r4 >> 32);
    r3 = 0x0;
    r4 = 0x0;
    r6 = (s32)r6 >> 2;
    r7 = (u32)r6 >> 31;
    r6 = r6 + r7;
    if ((s32)r6 >= (s32)tmp) {
        r6 = 0xf6;
    } else {

        r6 = 0xf5;
    }
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80095490:
do {
    if ((s32)r4 != 0x1b9) {
        if ((s32)r4 < 0x1b9) {
            if ((s32)r4 != 0x175) {
                if ((s32)r4 < 0x175) {
                    if ((s32)r4 != 0x172) {
                        if ((s32)r4 < 0x172) {
                            if ((s32)r4 < 0x171) {
                                break;
                            }
                            if ((s32)r4 < 0x174) {
                                goto L_80095580;
                            }
                            if ((s32)r4 != 0x178) {
                                if ((s32)r4 >= 0x178) break;
                                if ((s32)r4 < 0x177) {
                                    goto L_80095568;
                                }
                                if ((s32)r4 != 0x1be) {
                                    if ((s32)r4 < 0x1be) {
                                        if ((s32)r4 != 0x1bc) {
                                            if ((s32)r4 < 0x1bc) {
                                                if ((s32)r4 < 0x1bb) {
                                                    goto L_80095548;
                                                }
                                                if ((s32)r4 != 0x1c0) {
                                                    if ((s32)r4 >= 0x1c0) break;

                                                } else {
                                                    r28 = 0x1;
                                                    break;
                                                }
                                                r28 = 0x2;
                                                break;
                                            }
                                            r28 = 0x3;
                                            break;
                                                }
                                        r28 = 0x4;
                                        break;
                                            }
                                    r28 = 0x5;
                                    break;
                                                }
                                r28 = 0x6;
                                break;
                            L_80095548:
                                r28 = 0x7;
                                break;
                            }
                            r28 = 0x8;
                            break;
                                }
                        r28 = 0x1;
                        break;
                                }
                    r28 = 0x2;
                    break;
                L_80095568:
                    r28 = 0x3;
                    break;
                        }
                r28 = 0x4;
                break;
                            }
            r28 = 0x5;
            break;
        L_80095580:
            r28 = 0x6;
            break;
                        }
        r28 = 0x7;
        break;
                            }
    r28 = 0x8;
} while (0);
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r31 = *(u8*)((u8*)r3 + 0x2);
    r31 = (s8)r31;
    tmp = r31 & 0xFFFF;
    if (tmp == 4) {
        r30 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r31;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r30 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r30 = 0x0;
        }
    }
    tmp = r30 & 0xFFFF;
    if (tmp != 0) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x23;
        r6 = 0x0;
        fn_8011BEB4();
        r3 = r3 & 0xFFFF;
        fn_801EE07C();
        fn_801EE04C();
        r4 = r3 & 0xFF;
    } else {

        r4 = 0x0;
    }
    r3 = 0x66660000;
    tmp = r28 & 0xFFFF;
    r3 = r3 + 0x6667;
    r5 = r27;
    r6 = (s32)((s64)r3 * (s64)r4 >> 32);
    r3 = 0x0;
    r4 = 0x0;
    r6 = (s32)r6 >> 2;
    r7 = (u32)r6 >> 31;
    r6 = r6 + r7;
    if ((s32)r6 >= (s32)tmp) {
        r6 = 0xf7;
    } else {

        r6 = 0xf5;
    }
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();

    return;
}

/* 0x8009567C | size: 0xF4C */
void fn_8009567C(void) {
    extern void fn_8010C46C();
    extern void fn_8011396C();
    extern void fn_8011CB3C();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_8011CE00();
    extern void fn_8011CE18();
    extern void fn_8011E474();
    extern void fn_8011E778();
    extern void fn_8011F77C();
    extern void fn_8011FC14();
    extern void fn_8011FC74();
    extern void fn_801229F4();
    extern void fn_801248C4();
    extern void fn_8012640C();
    extern void fn_80129280();
    extern void fn_8012AC3C();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern void fn_80135938();
    extern void fn_801906A0();
    extern void fn_801F2A7C();
    extern void fn_801FCEAC();
    extern u8 jumptable_802EF01C[];
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    void (*ctr_fn)(void) = 0;

    r28 = r3;
    r30 = r4;
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r31 = *(u32*)((u8*)r3 + 0xC);
    if (r31 == 0) return;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x6e;
    r6 = 0x0;
    fn_8012640C();
    r3 = r3 & 0xFFFF;
    fn_8011E778();
    r26 = r3;
    if (r26 == 0) return;
    tmp = *(s16*)((u8*)r30 + 0x6);
    r25 = 0x1;
    if ((s32)tmp < 0x1b8) {
        if ((s32)tmp < 0x18b) {
            if ((s32)tmp >= 0x182) goto L_80095790;
            if ((s32)tmp < 0x170) {
                goto L_80095790;
            }
            if ((s32)tmp < 0x191) {
            }
            goto L_80095724;
        }
        if ((s32)tmp < 0x1d3) {
        }
        if ((s32)tmp < 0x1ca) {

        } else {
        }
        if ((s32)tmp >= 0x1d9) goto L_80095790;
    }
L_80095724:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 7) {
        if ((s32)tmp >= 7 || (s32)tmp >= 5) goto L_80095780;

        if ((s32)tmp < 3) {
            goto L_80095780;
        }
        }
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x2);
    tmp = (s8)tmp;
    if ((s32)tmp >= 0 || (s32)tmp > 4) {

        r25 = 0x1;
        goto L_80095784;
    }
    r25 = 0x0;
    goto L_80095784;
L_80095780:
    r25 = 0x0;
L_80095784:
    r3 = r30;
    r4 = r25;
    ((void(*)(void))fn_80109220)();
L_80095790:
    tmp = r25 & 0xFF;
    if (tmp == 0) return;
    r4 = *(s16*)((u8*)r30 + 0x6);
    tmp = -0x100;
    r3 = *(u8*)((u8*)r28 + 0x8B);
    r29 = r3 | tmp;
    if ((s32)r4 != 0x57b) {
        if ((s32)r4 < 0x57b) {
            if ((s32)r4 == 0x14b) goto L_800960B8;
            if ((s32)r4 < 0x14b) {
                if ((s32)r4 != 0x144) {
                    if ((s32)r4 < 0x144) {
                        if ((s32)r4 != 0x13c) {
                            if ((s32)r4 < 0x13c) {
                                if ((s32)r4 < 0x13b) {
                                    return;
                                }
                                if ((s32)r4 != 0x142) {
                                    if ((s32)r4 < 0x142) return;

                                }
                                if ((s32)r4 != 0x148) {
                                    if ((s32)r4 < 0x148) {
                                        if ((s32)r4 < 0x147) return;

                                    }
                                    if ((s32)r4 < 0x14a) {
                                        goto L_80096038;
                                    }
                                    if ((s32)r4 == 0x158) goto L_80096330;
                                    if ((s32)r4 < 0x158) {
                                        if ((s32)r4 == 0x154) goto L_80096210;
                                        if ((s32)r4 < 0x154) {
                                            if ((s32)r4 >= 0x153) goto L_80096188;
                                            if ((s32)r4 >= 0x14d) return;
                                            goto L_800960F8;
                                        }
                                        if ((s32)r4 >= 0x156) return;
                                        goto L_800962C0;
                                    }
                                    if ((s32)r4 != 0x54e) {
                                        if ((s32)r4 >= 0x54e) return;
                                        if ((s32)r4 < 0x54d) return;

                                    }
                                    if ((s32)r4 == 0x58f) goto L_80096188;
                                    if ((s32)r4 < 0x58f) {
                                    }
                                    if ((s32)r4 != 0x584) {
                                        if ((s32)r4 < 0x584) {
                                        }
                                        if ((s32)r4 != 0x581) {
                                            if ((s32)r4 < 0x581) {
                                                if ((s32)r4 != 0x57d) return;

                                            }
                                            if ((s32)r4 < 0x583) {
                                                goto L_80095E84;
                                            }
                                            if ((s32)r4 == 0x587) goto L_800960B8;
                                            if ((s32)r4 < 0x587) {
                                            }
                                            if ((s32)r4 >= 0x586) goto L_80096078;
                                            goto L_80096038;
                                        }
                                        if ((s32)r4 >= 0x589) return;
                                        goto L_800960F8;
                                    }
                                    if ((s32)r4 != 0x595) {
                                        if ((s32)r4 < 0x595) {
                                            if ((s32)r4 == 0x592) goto L_800962C0;
                                            if ((s32)r4 >= 0x592) return;
                                            if ((s32)r4 >= 0x591) goto L_80096210;
                                            goto L_800961CC;
                                        }
                                        if ((s32)r4 < 0x12b8) {
                                            if ((s32)r4 != 0x599) return;

                                        }
                                        if ((s32)r4 >= 0x12bc) return;
                                    }
                                    goto L_800963CC;
                                }
                                                }
                            r3 = r26;
                            r4 = 0x0;
                            fn_8011E474();
                            r3 = r3 & 0xFF;
                            fn_8010C46C();
                            r6 = r3 & 0xFFFF;
                            r5 = r28;
                            r3 = 0x0;
                            r4 = 0x0;
                            r7 = 0x0;
                            ((void(*)(void))fn_801040F0)();
                            return;
                                        }
                        r3 = r26;
                        r4 = 0x0;
                        fn_8011E474();
                        r24 = r3 & 0xFF;
                        r3 = r26;
                        r4 = 0x1;
                        fn_8011E474();
                        r3 = r3 & 0xFF;
                        if (r24 == r3) return;
                        fn_8010C46C();
                        r6 = r3 & 0xFFFF;
                        r5 = r28;
                        r3 = 0x0;
                        r4 = 0x0;
                        r7 = 0x0;
                        ((void(*)(void))fn_801040F0)();
                        return;
                                            }
                    r3 = r31;
                    fn_8011F77C();
                    tmp = r3 & 0xFF;
                    if (tmp < 3) {
                        r24 = 0x934;
                    } else {

                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0xbf;
                        r6 = 0x0;
                        fn_8012640C();
                        r3 = r3 & 0xFF;
                        fn_8011CE18();
                        fn_8011CE00();
                        r24 = r3;
                    }
                    r4 = r24;
                    r3 = 0x55;
                    fn_80132A38();
                    if ((s32)r24 != 0xc96) {
                        if ((s32)r24 >= 0xc96) goto L_800959FC;
                        if ((s32)r24 != 0xc86) {
                            goto L_800959FC;
                        }
                        }
                    r3 = 0x56;
                    r4 = 0x1;
                    fn_80132A38();
                    goto L_80095A08;
                L_800959FC:
                    r3 = 0x56;
                    r4 = 0x2bd8;
                    fn_80132A38();
                L_80095A08:
                do {
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x72;
                    r6 = 0x0;
                    fn_8012640C();
                    r4 = r3 & 0xFF;
                    if (r4 == 0) {
                        r4 = 0x5;
                    }
                    r3 = 0x34;
                    fn_80132A38();
                    r3 = (u32)&lbl_803FB380;
                    r3 = (u32)&lbl_803FB380;
                    r25 = *(u32*)((u8*)r3 + 0x8);
                    if (r31 == 0) {
                        tmp = 0x0;
                        break;
                    }
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x70;
                    r6 = 0x0;
                    fn_8012640C();
                    r4 = 0x2;
                    fn_80135938();
                    tmp = r3 & 0xFF;
                    if (tmp != 0xb) {
                        tmp = 0x0;
                        break;
                    }
                    r3 = (u32)&lbl_803FB380;
                    r3 = (u32)&lbl_803FB380;
                    tmp = *(u8*)((u8*)r3 + 0x0);
                    tmp = tmp & 0x00000020;
                    if ((s32)tmp != 0) {
                        if (r25 == 0) {
                            r3 = 0x0;
                            fn_801F2A7C();
                            r25 = r3;
                        }
                        if (r25 == 0) {
                            tmp = 0x0;
                            break;
                        }
                        r3 = r25;
                        fn_801FCEAC();
                        r25 = r3;

                    } else {
                        r3 = 0x8ae;
                        fn_801906A0();
                        if (r3 == 0) {
                            r3 = 0x0;
                            r4 = 0x2;
                            fn_80129280();
                            r25 = r3;
                            goto L_80095AF8;
                        }
                        ((void(*)(void))fn_8006AEEC)();
                        r25 = r3;
                    }
                L_80095AF8:
                    r3 = r25;
                    fn_8012AC54();
                    r26 = r3;
                    r3 = r25;
                    fn_8012AC3C();
                    r25 = r3;
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x75;
                    r6 = 0x0;
                    fn_8012640C();
                    if (r25 == r3) {
                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0x76;
                        r6 = 0x0;
                        fn_8012640C();
                        r4 = r3;
                        r3 = r26;
                        ((void(*)(void))fn_800F9EE4)();
                        if ((s32)r3 == 0) {
                            tmp = 0x1;
                            break;
                        }
                        }
                    tmp = 0x0;
                } while (0);
                    tmp = tmp & 0xFF;
                    if (tmp != 0) {
                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0x6e;
                        r6 = 0x0;
                        fn_8012640C();
                        tmp = r3 & 0xFFFF;
                        if ((s32)tmp < 0xc6) {
                            if ((s32)tmp < 0xc4) {
                                goto L_80095BA0;
                            }
                            r8 = 0x2be3;
                            goto L_80095BAC;
                        }
                    L_80095BA0:
                        r8 = 0x2bcd;

                    } else {
                        r8 = 0x2bcd;
                    }
                L_80095BAC:
                    r5 = *(s16*)((u8*)r30 + 0x54);
                    r7 = r29;
                    r6 = *(s16*)((u8*)r30 + 0x56);
                    r3 = 0x0;
                    r4 = 0x0;
                    ((void(*)(void))fn_800FBB34)();
                    return;
                                            }
            do {
                r3 = r31;
                r26 = 0x2be6;
                r4 = 0x0;
                r5 = 0x71;
                r6 = 0x0;
                fn_8012640C();
                r28 = r3;
                fn_8011396C();
                r4 = (u32)&lbl_803FB380;
                r4 = (u32)&lbl_803FB380;
                r27 = r3;
                r24 = *(u32*)((u8*)r4 + 0x8);
                if (r31 == 0) {
                    tmp = 0x0;
                    break;
                }
                r3 = r31;
                r4 = 0x0;
                r5 = 0x70;
                r6 = 0x0;
                fn_8012640C();
                r4 = 0x2;
                fn_80135938();
                tmp = r3 & 0xFF;
                if (tmp != 0xb) {
                    tmp = 0x0;
                    break;
                }
                r3 = (u32)&lbl_803FB380;
                r3 = (u32)&lbl_803FB380;
                tmp = *(u8*)((u8*)r3 + 0x0);
                tmp = tmp & 0x00000020;
                if ((s32)tmp != 0) {
                    if (r24 == 0) {
                        r3 = 0x0;
                        fn_801F2A7C();
                        r24 = r3;
                    }
                    if (r24 == 0) {
                        tmp = 0x0;
                        break;
                    }
                    r3 = r24;
                    fn_801FCEAC();
                    r25 = r3;

                } else {
                    r3 = 0x8ae;
                    fn_801906A0();
                    if (r3 == 0) {
                        r3 = 0x0;
                        r4 = 0x2;
                        fn_80129280();
                        r25 = r3;
                        goto L_80095CB0;
                    }
                    ((void(*)(void))fn_8006AEEC)();
                    r25 = r3;
                }
            L_80095CB0:
                r3 = r25;
                fn_8012AC54();
                r24 = r3;
                r3 = r25;
                fn_8012AC3C();
                r25 = r3;
                r3 = r31;
                r4 = 0x0;
                r5 = 0x75;
                r6 = 0x0;
                fn_8012640C();
                if (r25 == r3) {
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x76;
                    r6 = 0x0;
                    fn_8012640C();
                    r4 = r3;
                    r3 = r24;
                    ((void(*)(void))fn_800F9EE4)();
                    if ((s32)r3 == 0) {
                        tmp = 0x1;
                        break;
                    }
                    }
                tmp = 0x0;
            } while (0);
            do {
                tmp = tmp & 0xFF;
                if (tmp != 0) {
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x6e;
                    r6 = 0x0;
                    fn_8012640C();
                    tmp = r3 & 0xFFFF;
                    if ((s32)tmp < 0xc6) {
                        if ((s32)tmp < 0xc4) {
                            goto L_80095D58;
                        }
                        r26 = 0x2be4;
                        break;
                    }
                L_80095D58:
                    if (r27 == 0) break;
                    r3 = r27;
                    ((void(*)(void))fn_800FA280)();
                    r4 = r3;
                    r3 = 0x37;
                    fn_80132A38();
                    r26 = 0x2bd7;
                    break;
                }
                r3 = r31;
                r4 = 0x0;
                r5 = 0x70;
                r6 = 0x0;
                fn_8012640C();
                r4 = 0x2;
                fn_80135938();
                tmp = r3 & 0xFF;
                if ((s32)tmp != 0xb) {
                    if ((s32)tmp >= 0xb) break;
                    if ((s32)tmp < 8) {
                        break;
                    }
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x75;
                    r6 = 0x0;
                    fn_8012640C();
                    tmp = r3 + (0x0 << 16);
                    if (tmp == 0x911d) {
                        r3 = 0x12ac;
                        ((void(*)(void))fn_800FA280)();
                        r25 = r3;
                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0x76;
                        r6 = 0x0;
                        fn_8012640C();
                        r4 = r25;
                        ((void(*)(void))fn_800F9EE4)();
                        if ((s32)r3 == 0) {
                            tmp = 0x1;
                            goto L_80095E10;
                        }
                        }
                    tmp = 0x0;
                L_80095E10:
                    tmp = tmp & 0xFF;
                    if (tmp != 0) {
                        r26 = 0x2be7;

                    } else if (r28 == 0xff) {
                        r26 = 0x2be5;

                    }
                    if (r27 == 0) break;
                    r3 = r27;
                    ((void(*)(void))fn_800FA280)();
                    r4 = r3;
                    r3 = 0x37;
                    fn_80132A38();
                    r26 = 0x2bd7;
                    break;
                    }
                if (r28 != 0xff) break;
                r26 = 0x2be5;
            } while (0);
                r5 = *(s16*)((u8*)r30 + 0x54);
                r7 = r29;
                r6 = *(s16*)((u8*)r30 + 0x56);
                r8 = r26;
                r3 = 0x0;
                r4 = 0x0;
                ((void(*)(void))fn_800FBB34)();
                return;
            L_80095E84:
                r3 = r31;
                fn_8011F77C();
                tmp = r3 & 0xFF;
                do {
                if (tmp > 6) break;

                r3 = (u32)jumptable_802EF01C;
                tmp = tmp << 2;
                r3 = (u32)jumptable_802EF01C;
                tmp = *(u32*)(r3 + tmp);
                ctr_fn = (void(*)(void))tmp;
                r27 = 0x2bd9;
                break;

                r27 = 0x2bda;
                break;

                r27 = 0x2bdb;
                break;

                r27 = 0x2bdc;
                break;

                r27 = 0x2bdd;
                break;

                r27 = 0x2bde;
                break;

                r27 = 0x2bdf;
                } while (0);

                r5 = r29;
                r6 = r27;
                r3 = 0x0;
                r4 = 0x0;
                ((void(*)(void))fn_800FB680)();
                return;

                r3 = r31;
                r4 = 0x0;
                r5 = 0x7a;
                r6 = 0x0;
                fn_8012640C();
                r4 = r3 & 0xFF;
                r3 = r31;
                tmp = r4 + 0x1;
                r4 = tmp & 0xFF;
                fn_801229F4();
                r26 = r3;
                if (r26 == 0) {
                    r4 = 0x0;
                } else {

                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0x79;
                    r6 = 0x0;
                    fn_8012640C();
                    r4 = r26 - r3;
                }
                r3 = 0x34;
                fn_80132A38();
                r5 = *(s16*)((u8*)r30 + 0x54);
                r7 = r29;
                r6 = *(s16*)((u8*)r30 + 0x56);
                r3 = 0x0;
                r4 = 0x0;
                r8 = 0xde;
                ((void(*)(void))fn_800FBB34)();
                return;
                        }
            r3 = r31;
            r4 = 0x0;
            r5 = 0x79;
            r6 = 0x0;
            fn_8012640C();
            r4 = r3;
            r3 = 0x34;
            fn_80132A38();
            r5 = *(s16*)((u8*)r30 + 0x54);
            r7 = r29;
            r6 = *(s16*)((u8*)r30 + 0x56);
            r3 = 0x0;
            r4 = 0x0;
            r8 = 0xde;
            ((void(*)(void))fn_800FBB34)();
            return;
                                            }
        r3 = r31;
        r4 = 0x0;
        r5 = 0x8c;
        r6 = 0x0;
        fn_8012640C();
        r4 = (s16)r3;
        r3 = 0x34;
        fn_80132A38();
        r5 = *(s16*)((u8*)r30 + 0x54);
        r7 = r29;
        r6 = *(s16*)((u8*)r30 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0xde;
        ((void(*)(void))fn_800FBB34)();
        return;
                                    }
    r3 = r31;
    r4 = 0x0;
    r5 = 0x8b;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80096038:
    r3 = r31;
    r4 = 0x0;
    r5 = 0x8a;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80096078:
    r3 = r31;
    r4 = 0x0;
    r5 = 0x89;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    return;
L_800960B8:
    r3 = r31;
    r4 = 0x0;
    r5 = 0x88;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    return;
L_800960F8:
    r3 = r31;
    r4 = 0x0;
    r5 = 0x83;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r6 = *(s16*)((u8*)r30 + 0x56);
    r7 = r29;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x37;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    r5 = r29;
    r3 = 0x37;
    r4 = 0x0;
    r6 = 0x2bd4;
    ((void(*)(void))fn_800FB680)();
    r3 = r31;
    r4 = 0x0;
    r5 = 0x87;
    r6 = 0x0;
    fn_8012640C();
    r4 = (s16)r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xde;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80096188:
    r3 = r31;
    fn_801248C4();
    r3 = r3 & 0xFFFF;
    fn_8011CB6C();
    fn_8011CB3C();
    ((void(*)(void))fn_800FA280)();
    r4 = r3;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xcf;
    ((void(*)(void))fn_800FBB34)();
    return;
L_800961CC:
    r3 = r31;
    fn_801248C4();
    r3 = r3 & 0xFFFF;
    fn_8011CB6C();
    fn_8011CB54();
    ((void(*)(void))fn_800FA280)();
    r4 = r3;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(s16*)((u8*)r30 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xcf;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80096210:
    r3 = *(u32*)((u8*)r30 + 0x4C);
    ((void(*)(void))fn_800FA444)();
    tmp = (u32)r3 >> 16;
    r3 = r31;
    tmp = (s16)tmp;
    r24 = tmp;
    fn_8011FC74();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        r3 = r24;
        r5 = r29;
        r4 = 0x0;
        r6 = 0x2b70;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    r3 = r31;
    r4 = 0x0;
    r5 = 0x75;
    r6 = 0x0;
    fn_8012640C();
    r27 = r24;
    r24 = r3 & 0xFFFF;
    r28 = 0x2710;
    r25 = 0x0;
    r3 = 0xCCCD0000;
    do {
        r4 = (u32)r24 / (u32)r28;
        r3 = 0x34;
        r5 = r4 * r28;
        tmp = (u32)((u64)r26 * (u64)r28 >> 32);
        r24 = r24 - r5;
        r28 = (u32)tmp >> 3;
        fn_80132A38();
        r3 = r27;
        r5 = r29;
        r4 = 0x0;
        r6 = 0xca;
        ((void(*)(void))fn_800FB680)();
        r27 = r27 + 0xd;
        r25 = r25 + 0x1;
    } while ((s32)r25 < 5);
    return;
L_800962C0:
    r3 = r31;
    fn_8011FC74();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        r3 = 0x2b70;
        ((void(*)(void))fn_800FA280)();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
    } else {

        r3 = r31;
        r4 = 0x0;
        r5 = 0x76;
        r6 = 0x0;
        fn_8012640C();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
    }
    r3 = *(u32*)((u8*)r30 + 0x4C);
    ((void(*)(void))fn_800FA444)();
    tmp = (u32)r3 >> 16;
    r5 = r29;
    r3 = (s16)tmp;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
L_80096330:
    r3 = r31;
    r4 = 0x0;
    r5 = 0x7a;
    r6 = 0x0;
    fn_8012640C();
    r24 = r3 & 0xFF;
    r3 = r31;
    tmp = r24 + 0x1;
    r4 = tmp & 0xFF;
    fn_801229F4();
    r27 = r3;
    if (r27 == 0) return;
    r3 = r31;
    r4 = r24;
    fn_801229F4();
    r26 = r3;
    r3 = r31;
    r24 = r27 - r26;
    r4 = 0x0;
    r5 = 0x79;
    r6 = 0x0;
    fn_8012640C();
    tmp = *(s16*)((u8*)r30 + 0x54);
    r3 = r3 - r26;
    r6 = *(s16*)((u8*)r30 + 0x56);
    r7 = r29;
    tmp = r3 * tmp;
    r8 = r28;
    r3 = 0x0;
    r4 = 0x0;
    r9 = 0x117;
    r10 = 0x0;
    r5 = r24 + tmp;
    tmp = (u32)tmp / (u32)r24;
    r5 = (s16)tmp;
    ((void(*)(void))fn_80104160)();
    return;
L_800963CC:
do {
    if ((s32)r4 != 0x12b9) {
        if ((s32)r4 < 0x12b9) {
            if ((s32)r4 != 0x595) {
                if ((s32)r4 < 0x595) break;
                if ((s32)r4 < 0x12b8) {
                    break;
                }
                if ((s32)r4 != 0x12bb) {
                    if ((s32)r4 >= 0x12bb) break;
                    goto L_80096418;
                    }
                r27 = 0x0;
                break;
                    }
            r27 = 0x1;
            break;
        }
        r27 = 0x2;
        break;
    L_80096418:
        r27 = 0x3;
        break;
                }
    r27 = 0x4;
} while (0);
do {
    r3 = r31;
    r4 = 0x0;
    r5 = 0xc4;
    r6 = 0x0;
    fn_8012640C();
    r24 = r3 & 0xFFFF;
    if (r24 == 0) {
        f4 = *(f32*)&lbl_8047C208;
    } else {

        r3 = r31;
        fn_8011FC14();
        tmp = 0x43300000;
        f2 = *(f64*)&lbl_8047C220;
        *(u32*)(sp + 0x8) = tmp;
        f0 = f0 - f2;
        if (f1 > f0) {
            *(u32*)(sp + 0x10) = tmp;
            f1 = f0 - f2;
        }
        tmp = 0x43300000;
        f2 = *(f64*)&lbl_8047C220;
        *(u32*)(sp + 0x18) = tmp;
        f0 = f0 - f2;
        f4 = f1 / f0;
    }
    f0 = *(f32*)&lbl_8047C20C;
    /* cror eq, gt, eq */;
    if (f4 == f0) {
        tmp = 0x4;
        break;
    }
    f0 = *(f32*)&lbl_8047C210;
    /* cror eq, gt, eq */;
    if (f4 == f0) {
        tmp = 0x3;
        break;
    }
    f0 = *(f32*)&lbl_8047C214;
    /* cror eq, gt, eq */;
    if (f4 == f0) {
        tmp = 0x2;
        break;
    }
    f0 = *(f32*)&lbl_8047C218;
    /* cror eq, gt, eq */;
    if (f4 == f0) {
        tmp = 0x1;
        break;
    }
    tmp = 0x0;
} while (0);
    r4 = tmp & 0xFFFF;
    tmp = r27 & 0xFFFF;
    if (r4 > tmp) {
        r5 = *(s16*)((u8*)r30 + 0x54);
        r7 = r29;
        r6 = *(s16*)((u8*)r30 + 0x56);
        r8 = r28;
        r3 = 0x0;
        r4 = 0x0;
        r9 = 0x116;
        r10 = 0x0;
        ((void(*)(void))fn_80104160)();
        return;
    }
    if (r4 != tmp) return;
    r3 = 0x43300000;
    tmp = *(s16*)((u8*)r30 + 0x54);
    f1 = *(f64*)&lbl_8047C220;
    f2 = *(f32*)&lbl_8047C218;
    *(u32*)(sp + 0x14) = tmp;
    f0 = f0 - f1;
    f3 = *(f32*)&lbl_8047C21C;
    f1 = *(f64*)&lbl_8047C228;
    f2 = f2 * f0;
    f0 = f0 - f1;
    f1 = f4 - f2;
    f1 = f3 * f1;
    f1 = f0 * f1;
    ((void(*)(void))fn_800C46B0)();
    r6 = *(s16*)((u8*)r30 + 0x56);
    r5 = (s16)r3;
    r7 = r29;
    r8 = r28;
    r3 = 0x0;
    r4 = 0x0;
    r9 = 0x116;
    r10 = 0x0;
    ((void(*)(void))fn_80104160)();

    return;
}

/* 0x800965C8 | size: 0x680 */
void fn_800965C8(void) {
    extern void fn_8011E760();
    extern void fn_8011E778();
    extern void fn_8011F550();
    extern void fn_801230E0();
    extern void fn_8012640C();
    extern void fn_80132A38();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f5 = 0.0f;

    r29 = r3;
    r30 = r4;
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u32*)((u8*)r3 + 0xC);
    if (r28 == 0) return;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x6e;
    r6 = 0x0;
    fn_8012640C();
    r3 = r3 & 0xFFFF;
    fn_8011E778();
    if (r3 == 0) return;
    tmp = *(s16*)((u8*)r30 + 0x6);
    r4 = -0x100;
    r5 = *(u8*)((u8*)r29 + 0x8B);
    r6 = *(u8*)((u8*)r29 + 0x95);
    r31 = r5 | r4;
    r6 = (s8)r6;
    if ((s32)tmp == 0x10f) return;
    if ((s32)tmp < 0x10f) {
        if ((s32)tmp != 0x106) {
            if ((s32)tmp < 0x106) {
                if ((s32)tmp == 0xf3) goto L_80096AC4;
                if ((s32)tmp < 0xf3) {
                    if ((s32)tmp != 0xe7) {
                        return;
                    }
                    if ((s32)tmp == 0xf5) goto L_80096ADC;
                    if ((s32)tmp >= 0xf5) return;
                    goto L_80096AF8;
                }
                if ((s32)tmp != 0x10c) {
                    if ((s32)tmp < 0x10c) {
                        if ((s32)tmp < 0x10b) {
                            goto L_800967B0;
                        }
                        if ((s32)tmp >= 0x10e) goto L_800968E4;
                        goto L_800968BC;
                    }
                    if ((s32)tmp == 0x553) return;
                    if ((s32)tmp < 0x553) {
                        if ((s32)tmp == 0x112) goto L_80096ADC;
                        if ((s32)tmp < 0x112) {
                            if ((s32)tmp >= 0x111) goto L_80096AF8;
                            goto L_80096AC4;
                        }
                        if ((s32)tmp == 0x551) goto L_80096918;
                        if ((s32)tmp < 0x551) return;

                    }
                    if ((s32)tmp == 0x598) goto L_80096B94;
                    if ((s32)tmp >= 0x598) return;
                    if ((s32)tmp == 0x555) goto L_80096A08;
                    if ((s32)tmp >= 0x555) return;
                    goto L_800969A8;
                            }
                r3 = (u32)&lbl_803FB338;
                r3 = (u32)&lbl_803FB338;
                ((void(*)(void))fn_80109934)();
                r28 = r3;
                if (r28 == 0) return;
                r3 = 0x3;
                ((void(*)(void))fn_800D88DC)();
                r3 = 0x4;
                ((void(*)(void))fn_800D888C)();
                r3 = 0x7;
                ((void(*)(void))fn_800D6A00)();
                r3 = (u32)&lbl_80314F98;
                r3 = (u32)&lbl_80314F98;
                ((void(*)(void))fn_800D7820)();
                r4 = r28;
                r3 = 0x0;
                ((void(*)(void))fn_800D85D4)();
                r3 = 0x2;
                ((void(*)(void))fn_800D67BC)();
                r3 = 0x0;
                r4 = 0x0;
                ((void(*)(void))fn_800D61E4)();
                r3 = 0x0;
                r4 = 0xff;
                r5 = 0xff;
                r6 = 0xff;
                r7 = 0xff;
                ((void(*)(void))fn_800D5CB8)();
                f1 = *(f32*)&lbl_8047C230;
                r3 = 0x0;
                f2 = f1;
                ((void(*)(void))fn_800D59B8)();
                r3 = *(s16*)((u8*)r30 + 0x54);
                r4 = *(s16*)((u8*)r30 + 0x56);
                ((void(*)(void))fn_800D61E4)();
                r3 = 0x0;
                r4 = 0xff;
                r5 = 0xff;
                r6 = 0xff;
                r7 = 0xff;
                ((void(*)(void))fn_800D5CB8)();
                f1 = *(f32*)&lbl_8047C208;
                r3 = 0x0;
                f2 = f1;
                ((void(*)(void))fn_800D59B8)();
                ((void(*)(void))fn_800D6728)();
                return;
            L_800967B0:
            do {
                r3 = r28;
                r28 = 0x0;
                r4 = 0x0;
                r5 = 0xbb;
                r6 = 0x0;
                fn_8012640C();
                tmp = *(s16*)((u8*)r30 + 0x6);
                r3 = r3 & 0xFF;
                if ((s32)tmp != 0x109) {
                    if ((s32)tmp < 0x109) {
                        if ((s32)tmp != 0x107) {
                            if ((s32)tmp < 0x107) {
                                break;
                            }
                            if ((s32)tmp >= 0x10b) break;
                            goto L_80096810;
                            }
                        r28 = 0x8;
                        break;
                            }
                    r28 = 0x4;
                    break;
                }
                r28 = 0x2;
                break;
            L_80096810:
                r28 = 0x1;
            } while (0);
                tmp = r3 & r28;
                r3 = r30;
                r4 = tmp & 0xFF;
                ((void(*)(void))fn_80109220)();
                return;
                        }
            r3 = r28;
            fn_8011F550();
            tmp = r3 & 0xFF;
            if (tmp >= 0xd) return;
            r3 = (u32)&lbl_802EED28;
            tmp = tmp << 1;
            r3 = (u32)&lbl_802EED28;
            r5 = r29;
            r6 = *(u16*)(r3 + tmp);
            r3 = 0x0;
            r4 = 0x0;
            r7 = 0x0;
            ((void(*)(void))fn_801040F0)();
            return;
                }
        r3 = r28;
        r4 = 0x0;
        r5 = 0xb5;
        r6 = 0x0;
        fn_8012640C();
        r3 = r3 & 0xFF;
        tmp = r3 & 0xF;
        if ((s32)tmp != 0) {
            r6 = 0xe8;

        } else if (r3 != 0) {
            r6 = 0xe7;

        } else {
            r6 = 0x0;
        }
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r7 = 0x0;
        ((void(*)(void))fn_801040F0)();
        return;
    L_800968BC:
        r3 = r28;
        r4 = 0x1;
        ((void(*)(void))fn_8001D624)();
        r6 = r3;
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r7 = 0x0;
        ((void(*)(void))fn_801040F0)();
        return;
    L_800968E4:
        r3 = r28;
        fn_801230E0();
        tmp = r3;
        r3 = r30;
        tmp = tmp & 0xFFFF;
        if (tmp != 0) {
            tmp = 0x1;
        } else {

            tmp = 0x0;
        }
        r4 = tmp & 0xFF;
        ((void(*)(void))fn_80109220)();
        return;
    L_80096918:
        r3 = r28;
        fn_801230E0();
        r4 = r3 & 0xFFFF;
        if (r4 == 0) return;
        r3 = 0x2d;
        fn_80132A38();
        r5 = *(s16*)((u8*)r30 + 0x54);
        r7 = r31;
        r6 = *(s16*)((u8*)r30 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2bd3;
        ((void(*)(void))fn_800FBB34)();
        return;

        r3 = *(u32*)((u8*)r30 + 0x4C);
        ((void(*)(void))fn_800FA444)();
        tmp = (u32)r3 >> 16;
        r3 = r28;
        tmp = (s16)tmp;
        r4 = 0x0;
        r28 = tmp;
        r5 = 0x7a;
        r6 = 0x0;
        fn_8012640C();
        r4 = r3 & 0xFF;
        r3 = 0x34;
        fn_80132A38();
        r5 = *(s16*)((u8*)r30 + 0x54);
        r3 = r28;
        r6 = *(s16*)((u8*)r30 + 0x56);
        r7 = r31;
        r4 = 0x0;
        r8 = 0xd2;
        ((void(*)(void))fn_800FBB34)();
        return;
    L_800969A8:
        fn_8011E760();
        r29 = r3;
        r3 = 0x2bd4;
        ((void(*)(void))fn_800FA444)();
        tmp = (u32)r3 >> 16;
        r5 = r31;
        r30 = (s16)tmp;
        r3 = 0x0;
        r4 = 0x0;
        r6 = 0x2bd4;
        ((void(*)(void))fn_800FB680)();
        if (r29 == 0) return;
        r3 = r29;
        ((void(*)(void))fn_800FA280)();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
        r3 = r30;
        r5 = r31;
        r4 = 0x0;
        r6 = 0xe7;
        ((void(*)(void))fn_800FB680)();
        return;
    L_80096A08:
        r3 = r28;
        r4 = 0x0;
        r5 = 0x77;
        r6 = 0x0;
        fn_8012640C();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
        r5 = *(s16*)((u8*)r30 + 0x54);
        r7 = r31;
        r6 = *(s16*)((u8*)r30 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0xe7;
        ((void(*)(void))fn_800FBB34)();
        r3 = 0xe7;
        ((void(*)(void))fn_800FA444)();
        tmp = (u32)r3 >> 16;
        r3 = r28;
        r29 = (s16)tmp;
        ((void(*)(void))fn_8001DA60)();
        tmp = r3 & 0xFF;
        if ((s32)tmp != 1) {
            if ((s32)tmp < 1) {
                if ((s32)tmp < 0) {
                    goto L_80096A90;
                }
                goto L_80096A90;
                }
            r3 = 0xd67;
            goto L_80096A94;
        }
        r3 = 0xd68;
        goto L_80096A94;
    L_80096A90:
        r3 = 0x0;
    L_80096A94:
        if (r3 == 0) return;
        ((void(*)(void))fn_800FA280)();
        r4 = r3;
        r3 = 0x37;
        fn_80132A38();
        r3 = r29;
        r5 = r31;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
    L_80096AC4:
        tmp = __cntlzw(r6);
        r3 = r30;
        tmp = (u32)tmp >> 5;
        r4 = tmp & 0xFF;
        ((void(*)(void))fn_80109220)();
        return;
    L_80096ADC:
        tmp = 0x1 - r6;
        r3 = r30;
        tmp = __cntlzw(tmp);
        tmp = (u32)tmp >> 5;
        r4 = tmp & 0xFF;
        ((void(*)(void))fn_80109220)();
        return;
    L_80096AF8:
        tmp = 0x2 - r6;
        r3 = r30;
        tmp = __cntlzw(tmp);
        tmp = (u32)tmp >> 5;
        r4 = tmp & 0xFF;
        ((void(*)(void))fn_80109220)();
        return;
        }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 4) {
        if ((s32)tmp < 4) {
            if ((s32)tmp != 2) {
                if ((s32)tmp < 2) {
                    goto L_80096B88;
                }
                if ((s32)tmp >= 8 || (s32)tmp >= 6) goto L_80096B88;

                goto L_80096B78;
            }
                }
        r4 = 0x1;
        goto L_80096B88;
                }
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x00000002;
    if ((s32)tmp != 0) {
        r4 = 0x1;
    }
    goto L_80096B88;
L_80096B78:
    tmp = *(u32*)((u8*)r3 + 0x1C);
    if ((s32)tmp > 0) {
        r4 = 0x1;
    }
L_80096B88:
    r3 = r30;
    ((void(*)(void))fn_80109220)();
    return;
L_80096B94:
do {
    r3 = (u32)&lbl_803FB380;
    r6 = 0x0;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 4) {
        if ((s32)tmp < 4) {
            if ((s32)tmp != 2) {
                if ((s32)tmp < 2) {
                    break;
                }
                if ((s32)tmp >= 8 || (s32)tmp >= 6) break;


                } else {
                r6 = 0x2bcf;
                break;
                }
            tmp = *(u32*)((u8*)r3 + 0x1C);
            if ((s32)tmp <= 0) break;
            r6 = 0x2bd2;
            break;
                }
        tmp = *(u8*)((u8*)r3 + 0x0);
        tmp = tmp & 0x00000002;
        if ((s32)tmp == 0) break;
        r6 = 0x2bd0;
        break;
    }
    r6 = 0x2bd0;
} while (0);
    if (r6 == 0) return;
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800FB680)();

    return;
}

/* 0x80096C48 | size: 0x10C */
#pragma peephole off
void fn_80096C48(u32 unused, u8* dst) {
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } ColorTriple;

    ColorTriple state0;
    ColorTriple state1;
    ColorTriple state2;
    register u8* out;
    register ColorTriple* triple;
    u8* obj;
    s32 state;

    out = dst;
    state0 = *(ColorTriple*)(lbl_8026F5C0 + 0x00);
    state1 = *(ColorTriple*)(lbl_8026F5C0 + 0x0C);
    state2 = *(ColorTriple*)(lbl_8026F5C0 + 0x18);

    obj = fn_80104704(0x53);
    if (obj == NULL) {
        return;
    }

    state = (s8)obj[0x95];
    switch (state) {
    case 0:
        triple = &state0;
        break;
    case 1:
        triple = &state1;
        break;
    case 2:
        triple = &state2;
        break;
    }

    out[0x64] = triple->x;
    out[0x65] = triple->y;
    out[0x66] = triple->z;
}
#pragma peephole on

/* 0x80096D54 | size: 0x24C */
#pragma peephole off
void fn_80096D54(void* arg0) {
    extern void* fn_80105624();
    extern int fn_8012640C();
    extern void fn_80166A28();
    extern void fn_80123C54();
    void* obj;
    void* cur;
    s32 i;

    obj = fn_80105624();
    if (*(u16*)((u8*)obj + 0x4) & 0x10) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 1:
        case 6:
        case 8:
            break;
        case 2:
            lbl_803FB380[2] = 0;
            lbl_803FB380[1] = 3;
            break;
        case 3:
            cur = *(void**)(lbl_803FB380 + 0xc);
            if (cur != 0) {
                if (fn_8012640C(cur, 0, 0xc2, 0) != 0) {
                    fn_80166A28(0x26);
                } else if (lbl_803FB380[0] & 2) {
                    lbl_803FB380[1] = 4;
                    lbl_803FB380[3] = lbl_803FB380[2];
                }
            }
            break;
        case 4:
            cur = *(void**)(lbl_803FB380 + 0xc);
            if (cur != 0) {
                fn_80123C54(cur, (s8)lbl_803FB380[3], (s8)lbl_803FB380[2]);
            }
            *(s8*)(lbl_803FB380 + 0x3) = -1;
            lbl_803FB380[1] = 3;
            break;
        case 5:
            if (*(s32*)(lbl_803FB380 + 0x1c) > 0) {
                for (i = 0; (s8)i < 36; i++) {
                    if ((s8)lbl_803FB380[0x20 + ((s8)i % 9) * 4 + ((s8)i % 4)] >= 0) {
                        break;
                    }
                }
                lbl_803FB380[1] = 6;
                lbl_803FB380[0x1a] = i;
            }
            break;
        case 7:
            if (lbl_803FB380[0] & 0x10) {
                *(u8*)((u8*)arg0 + 0x98) = 1;
                *(s32*)(lbl_803FB380 + 0x4) = (s8)lbl_803FB380[2];
            }
            break;
        }
    } else if (*(u16*)((u8*)obj + 0x4) & 0x20) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 8:
            break;
        case 1:
        case 2:
        case 5:
        case 7:
            *(u8*)((u8*)arg0 + 0x98) = 1;
            *(u8*)((u8*)arg0 + 0x99) = 1;
            lbl_803FB380[1] = 8;
            break;
        case 3:
            lbl_803FB380[1] = 2;
            break;
        case 4:
            lbl_803FB380[1] = 3;
            break;
        case 6:
            *(s8*)(lbl_803FB380 + 0x1a) = -1;
            lbl_803FB380[1] = 5;
            break;
        }
    }
}
#pragma peephole reset

/* 0x80096FA0 | size: 0x44C */
#pragma peephole off
void fn_80096FA0(void* arg0) {
    extern void* fn_80105624();
    extern void fn_80103484();
    extern void fn_80109C88();
    extern u8 fn_80123CD4();
    extern u16 fn_8012640C();
    u8* p;
    void* obj;
    u16 input;
    s32 status;
    s32 limit;
    s32 v;
    u32 cur;
    u16 ok;
    u32 res;
    s32 row;
    s32 col;
    s32 r7;
    s32 r8;
    s32 t;
    u8* gp;

    p = (u8*)arg0;
    status = 0;
    if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
        limit = 5;
    } else {
        limit = 4;
    }
    obj = fn_80105624();
    input = *(u16*)((u8*)obj + 0x6);
    if (lbl_803FB380[1] <= 8) {
        switch (lbl_803FB380[1]) {
        case 1:
        case 2:
        case 5:
            v = *(u8*)(p + 0x95);
            if (input & 8) {
                v += 1;
            } else if (input & 4) {
                v -= 1;
            }
            if ((s8)v > 2) {
                v = 2;
            }
            if ((s8)v < 0) {
                v = 0;
            }
            *(u8*)(p + 0x95) = v;
            switch ((s8)v) {
            case 0:
                lbl_803FB380[1] = 1;
                break;
            case 1:
                lbl_803FB380[1] = 2;
                break;
            case 2:
                lbl_803FB380[1] = 5;
                break;
            }
            if (input & 1) {
                status = 1;
            } else if (input & 2) {
                status = 2;
            }
            if (status != 0 && *(u32*)(lbl_803FB380 + 0x10) != 0) {
                res = (*(u32(**)(u32, s32, s32))(lbl_803FB380 + 0x10))(
                    *(u32*)(lbl_803FB380 + 0xc), status, *(s32*)(lbl_803FB380 + 0x14));
                if (*(u32*)(lbl_803FB380 + 0xc) != res) {
                    if (res != 0) {
                        fn_80103484(*(s32*)(p + 0x4), 1);
                        fn_80109C88(lbl_803FB338, res);
                    }
                    *(u32*)(lbl_803FB380 + 0xc) = res;
                    return;
                }
            }
            return;
        case 3:
        case 4:
        case 7:
            v = *(u8*)(lbl_803FB380 + 0x2);
            if (input & 1) {
                v -= 1;
            } else if (input & 2) {
                v += 1;
            }
            if ((s8)v >= limit) {
                v = (u8)(s8)(limit - 1);
            }
            if ((s8)v < 0) {
                v = 0;
            }
            cur = *(u32*)(lbl_803FB380 + 0xc);
            if ((u16)(s8)v == 4) {
                ok = *(u16*)(lbl_803FB380 + 0x18);
            } else {
                ok = fn_8012640C(cur, 0, 0x7f, (s8)v);
                if (fn_80123CD4(cur, (s8)v) == 0) {
                    ok = 0;
                }
            }
            if (ok != 0 && (s8)v != (s8)lbl_803FB380[2]) {
                fn_80103484(*(s32*)(p + 0x4), 1);
                lbl_803FB380[2] = v;
                return;
            }
            break;
        case 6:
            row = (s8)lbl_803FB380[0x1a] / 9;
            col = (s8)lbl_803FB380[0x1a] % 9;
            if (input & 1) {
                r8 = row;
            L_up:
                t = r8 > 0;
                r8 -= 1;
                if (t) {
                    r7 = col;
                    gp = lbl_803FB380 + r8 + col * 4;
                L_up2:
                    if ((s8)gp[0x20] >= 0) {
                        row = r8;
                        col = r7;
                        r8 = -1;
                    } else {
                        t = r7 > 0;
                        gp -= 4;
                        r7 -= 1;
                        if (t) {
                            goto L_up2;
                        }
                    }
                    goto L_up;
                }
            } else if (input & 2) {
                r7 = row;
            L_dn:
                r7 += 1;
                if (r7 < 4) {
                    r8 = col;
                    gp = lbl_803FB380 + r7 + col * 4;
                L_dn2:
                    if ((s8)gp[0x20] >= 0) {
                        row = r7;
                        col = r8;
                        r7 = 5;
                    } else {
                        t = r8 > 0;
                        gp -= 4;
                        r8 -= 1;
                        if (t) {
                            goto L_dn2;
                        }
                    }
                    goto L_dn;
                }
            } else if (input & 8) {
                t = col;
                col += 1;
                if (col >= 9) {
                    col = 8;
                }
                if ((s8)lbl_803FB380[0x20 + col * 4 + row] < 0) {
                    col = t;
                }
            } else if (input & 4) {
                t = col;
                col -= 1;
                if (col < 0) {
                    col = 0;
                }
                if ((s8)lbl_803FB380[0x20 + col * 4 + row] < 0) {
                    col = t;
                }
            }
            v = col + row * 9;
            if ((s8)v != (s8)lbl_803FB380[0x1a]) {
                fn_80103484(*(s32*)(p + 0x4), 1);
                lbl_803FB380[0x1a] = v;
            }
            break;
        case 0:
        case 8:
            break;
        }
    }
}
#pragma peephole reset

/* 0x800973EC | size: 0x2B0 */
void fn_800973EC(void) {
    extern void fn_8012640C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    tmp = *(u8*)((u8*)r3 + 0x1);
    tmp = (s8)tmp;
    if ((s32)tmp == 1) { r3 = 0x0; return; }
    if ((s32)tmp < 1) {
        if ((s32)tmp < 0) {
            r3 = 0x0;
            return;
        }
        if ((s32)tmp >= 3) { r3 = 0x0; return; }

        } else {
        r7 = -0x1;
        r4 = (u32)&lbl_803FB380;
        *(u8*)((u8*)r3 + 0x97) = r7;
        r5 = (u32)&lbl_803FB380;
        r6 = 0x0;
        *(u8*)((u8*)r5 + 0x1) = r6;
        tmp = *(u8*)((u8*)r5 + 0x0);
        r4 = *(u8*)((u8*)r3 + 0x95);
        tmp = tmp & 0x00000004;
        *(u8*)((u8*)r5 + 0x2) = r4;
        *(u8*)((u8*)r5 + 0x3) = r7;
        *(u8*)((u8*)r5 + 0x1A) = r7;
        if ((s32)tmp != 0) {
            *(u8*)((u8*)r3 + 0x95) = r6;
            tmp = 0x1;
            *(u8*)((u8*)r5 + 0x1) = tmp;
        } else {

            r4 = 0x1;
            tmp = 0x7;
            *(u8*)((u8*)r3 + 0x95) = r4;
            *(u8*)((u8*)r5 + 0x1) = tmp;
        }
        r4 = (u32)&lbl_803FB380;
        tmp = *(u16*)((u8*)r3 + 0x94);
        r4 = (u32)&lbl_803FB380;
        r31 = 0x0;
        r3 = *(u32*)((u8*)r4 + 0xC);
        *(u16*)(sp + 0xC) = tmp;
        if (r3 == 0) { r3 = 0x0; return; }
        tmp = *(u8*)(sp + 0xD);
        tmp = (s8)tmp;
        if ((s32)tmp != 1) {
            if ((s32)tmp < 1) {
                if ((s32)tmp < 0) {
                    goto L_8009751C;
                }
                if ((s32)tmp < 3) {
                    goto L_80097518;
                    }
                r4 = 0x0;
                r5 = 0xc2;
                r6 = 0x0;
                fn_8012640C();
                if ((s32)r3 != 0) {
                    r31 = 0x55;
                }
                goto L_8009751C;
            }
            r31 = 0x54;
            goto L_8009751C;
        }
        tmp = *(u16*)((u8*)r4 + 0x18);
        if (tmp != 0) {
            r31 = 0x56;
            goto L_8009751C;
        }
        r31 = 0x57;
        goto L_8009751C;
    L_80097518:
        r31 = 0x58;
    L_8009751C:
        r3 = (u32)&lbl_802EEFC4;
        r30 = 0x0;
        r29 = (u32)&lbl_802EEFC4;
        do {
        do {
            r3 = *(u32*)((u8*)r29 + 0x0);
            if ((s32)r31 == (s32)r3) {
                r3 = r31;
                ((void(*)(void))fn_80102620)();
                tmp = r3 & 0xFF;
                if (tmp != 0) break;
                r3 = r31;
                r4 = 0x0;
                ((void(*)(void))fn_8010264C)();
                break;
            }
            ((void(*)(void))fn_80102620)();
            tmp = r3 & 0xFF;
            if (tmp == 0) break;
            r3 = *(u32*)((u8*)r29 + 0x0);
            ((void(*)(void))fn_80102510)();
        } while (0);
            r29 = r29 + 0x4;
            r30 = r30 + 0x1;
        } while (r30 < 5);
        r3 = 0x0;
        return;
        }
do {
    r4 = (u32)&lbl_803FB380;
    tmp = *(u16*)((u8*)r3 + 0x94);
    r4 = (u32)&lbl_803FB380;
    r30 = 0x0;
    r3 = *(u32*)((u8*)r4 + 0xC);
    *(u16*)(sp + 0x8) = tmp;
    if (r3 == 0) { r3 = 0x0; return; }
    tmp = *(u8*)(sp + 0x9);
    tmp = (s8)tmp;
    if ((s32)tmp != 1) {
        if ((s32)tmp < 1) {
            if ((s32)tmp < 0) {
                break;
            }
            if ((s32)tmp >= 3) break;
            goto L_80097614;
            }
        r4 = 0x0;
        r5 = 0xc2;
        r6 = 0x0;
        fn_8012640C();
        if ((s32)r3 != 0) {
            r30 = 0x55;
            break;
        }
        r30 = 0x54;
        break;
    }
    tmp = *(u16*)((u8*)r4 + 0x18);
    if (tmp != 0) {
        r30 = 0x56;
        break;
    }
    r30 = 0x57;
    break;
L_80097614:
    r30 = 0x58;
} while (0);
    r3 = (u32)&lbl_802EEFC4;
    r31 = 0x0;
    r29 = (u32)&lbl_802EEFC4;
    do {
    do {
        r3 = *(u32*)((u8*)r29 + 0x0);
        if ((s32)r30 == (s32)r3) {
            r3 = r30;
            ((void(*)(void))fn_80102620)();
            tmp = r3 & 0xFF;
            if (tmp != 0) break;
            r3 = r30;
            r4 = 0x0;
            ((void(*)(void))fn_8010264C)();
            break;
        }
        ((void(*)(void))fn_80102620)();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        r3 = *(u32*)((u8*)r29 + 0x0);
        ((void(*)(void))fn_80102510)();
    } while (0);
        r29 = r29 + 0x4;
        r31 = r31 + 0x1;
    } while (r31 < 5);

    r3 = 0x0;
    return;
}

/* 0x8009769C | size: 0x350 */
void fn_8009769C(void) {
    extern void fn_80109C88();
    extern void fn_8010A420();
    extern void fn_8010A5BC();
    extern void fn_8011288C();
    extern void fn_8011BEB4();
    extern void fn_80123CD4();
    extern void fn_8012640C();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r28 = r5;
    r3 = (u32)&lbl_803FB338;
    r4 = 0xc8;
    r3 = (u32)&lbl_803FB338;
    r5 = 0xb4;
    fn_8010A5BC();
    r3 = (u32)&lbl_803FB338;
    r4 = r28;
    r3 = (u32)&lbl_803FB338;
    fn_80109C88();
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x00000008;
    do {
    if ((s32)tmp == 0) break;

    r3 = 0x1;
    fn_801C40F0();
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x00000080;
    if ((s32)tmp != 0) {
        f1 = *(f32*)&lbl_8047C234;
        r3 = 0x2;
        fn_801C41C8();
        break;

    }
    f1 = *(f32*)&lbl_8047C238;
    r3 = 0x2;
    fn_801C41C8();
    } while (0);

    tmp = 0x0;
    r3 = (u32)&lbl_803FB380;
    *(u32*)(sp + 0x8) = tmp;
    r31 = (u32)&lbl_803FB380;
    do {
        r5 = (u32)sp + 0x8;
        r3 = 0x53;
        r4 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        ((void(*)(void))fn_801026A4)();
        if ((s32)r3 == (s32)-0x1) {
            *(u32*)((u8*)r31 + 0x4) = r3;
            break;
        }
        tmp = *(u8*)((u8*)r31 + 0x0);
        r3 = *(u8*)((u8*)r31 + 0x2);
        tmp = tmp & 0x00000040;
        r28 = (s8)r3;
        *(u32*)((u8*)r31 + 0x4) = r28;
        if ((s32)tmp == 0) break;
        tmp = r28 & 0xFFFF;
        r29 = *(u32*)((u8*)r31 + 0xC);
        if (tmp == 4) {
            r30 = *(u16*)((u8*)r31 + 0x18);

        } else {
            r3 = r29;
            r6 = r28;
            r4 = 0x0;
            r5 = 0x7f;
            fn_8012640C();
            r30 = r3 & 0xFFFF;
            r3 = r29;
            r4 = r28;
            fn_80123CD4();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r30 = 0x0;
            }
        }
        r4 = r30;
        r3 = 0x0;
        r5 = 0x19;
        r6 = 0x0;
        fn_8011BEB4();
        if ((s32)r3 == 0) break;
        r3 = 0x2;
        r4 = 0x2be9;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
    } while (1);

    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x00000008;
    if ((s32)tmp != 0) {
        r3 = 0x1;
        fn_801C40F0();
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        tmp = *(u8*)((u8*)r3 + 0x0);
        tmp = tmp & 0x00000080;
        if ((s32)tmp != 0) {
            f1 = *(f32*)&lbl_8047C234;
            r3 = 0x3;
            fn_801C41C8();
        } else {

            f1 = *(f32*)&lbl_8047C238;
            r3 = 0x3;
            fn_801C41C8();
        }
        r3 = 0x1;
        fn_801C40F0();
    }
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r3 = *(u8*)((u8*)r3 + 0x0);
    tmp = r3 & 0x1;
    if ((s32)tmp != 0) {
        tmp = r3 & 0x00000008;
        if ((s32)tmp == 0) {
            r3 = 0x1;
            fn_801C40F0();
            f1 = *(f32*)&lbl_8047C238;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
    }
    }
    r3 = 0x54;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x54;
        r4 = 0x0;
        r5 = 0x0;
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0x55;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x55;
        r4 = 0x0;
        r5 = 0x0;
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0x57;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x57;
        r4 = 0x0;
        r5 = 0x0;
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0x56;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x56;
        r4 = 0x0;
        r5 = 0x0;
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0x58;
    ((void(*)(void))fn_80102620)();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = 0x58;
        r4 = 0x0;
        r5 = 0x0;
        ((void(*)(void))fn_80102568)();
    }
    r3 = 0x53;
    r4 = 0x0;
    r5 = 0x1;
    ((void(*)(void))fn_80102568)();
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x1;
    if ((s32)tmp != 0) {
        ((void(*)(void))fn_800FF660)();
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        tmp = *(u8*)((u8*)r3 + 0x0);
        tmp = tmp & 0x00000008;
        if ((s32)tmp != 0) {
            r3 = 0x0;
            r4 = 0x0;
            fn_8011288C();
    }
    }
    r3 = (u32)&lbl_803FB338;
    r3 = (u32)&lbl_803FB338;
    fn_8010A420();
    ((void(*)(void))_threadSwitch)();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x800979EC | size: 0x4C */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_800979EC(void) {
    extern u32 fn_8009769C(u8, u32, u32, u16, u32, u32);

    *(u32*)(lbl_803FB380 + 4) = fn_8009769C(
        lbl_803FB380[0],
        *(u32*)(lbl_803FB380 + 8),
        *(u32*)(lbl_803FB380 + 0xC),
        *(u16*)(lbl_803FB380 + 0x18),
        *(u32*)(lbl_803FB380 + 0x10),
        *(u32*)(lbl_803FB380 + 0x14));
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/* 0x80097A38 | size: 0xCC */
void fn_80097A38(void) {
    extern void fn_8010B560();
    extern void fn_8011288C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r5 = 0x59;
    r31 = (u32)&lbl_803FB380;
    r4 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r5;
    r3 = 0x39d;
    *(u32*)((u8*)r31 + 0x8) = r4;
    *(u32*)((u8*)r31 + 0xC) = r29;
    *(u16*)((u8*)r31 + 0x18) = r30;
    *(u32*)((u8*)r31 + 0x10) = r4;
    *(u32*)((u8*)r31 + 0x14) = r4;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    ((void(*)(void))fn_800FF730)();
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x0);
    tmp = tmp & 0x00000008;
    if ((s32)tmp != 0) {
        r3 = 0x0;
        r4 = 0x0;
        fn_8011288C();
    }
    ((void(*)(void))_threadSwitch)();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097B04 | size: 0xB8 */
void fn_80097B04(void) {
    extern void fn_8009769C();
    extern void fn_8010B560();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r4 = 0x58;
    r31 = (u32)&lbl_803FB380;
    r9 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r4;
    r5 = r29;
    r6 = r30;
    *(u32*)((u8*)r31 + 0x8) = r9;
    r3 = 0x58;
    r4 = 0x0;
    r7 = 0x0;
    *(u32*)((u8*)r31 + 0xC) = r29;
    r8 = 0x0;
    *(u16*)((u8*)r31 + 0x18) = r30;
    *(u32*)((u8*)r31 + 0x10) = r9;
    *(u32*)((u8*)r31 + 0x14) = r9;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8009769C();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097BBC | size: 0x114 */
#pragma peephole off
s32 fn_80097BBC(u8 chan) {
    extern void* fn_80129280();
    extern void* fn_8012AC08();
    extern int fn_80123FBC();
    extern int fn_8010B560();
    void* entity;
    void* mgr;

    entity = NULL;
    if (chan < 6) {
        mgr = fn_80129280(0, 2);
        if (mgr != 0) {
            entity = fn_8012AC08(mgr, chan);
            if ((u8)fn_80123FBC() == 0) {
                entity = NULL;
            }
        }
    }
    if (entity == 0) {
        return -1;
    }
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x11;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = (u32)entity;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(u32*)(lbl_803FB380 + 0x4) = -1;
    fn_800FF730(0x39d);
    if (lbl_803FB380[0] & 8) {
        fn_8011288C(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}
#pragma peephole on
#pragma peephole reset

/* 0x80097CD0 | size: 0xC4 */
void fn_80097CD0(void) {
    extern void fn_8009769C();
    extern void fn_8010B560();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r4 = 0xc;
    r31 = (u32)&lbl_803FB380;
    r9 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r4;
    r5 = r28;
    r7 = r29;
    *(u32*)((u8*)r31 + 0x8) = r9;
    r8 = r30;
    r3 = 0xc;
    r4 = 0x0;
    *(u32*)((u8*)r31 + 0xC) = r28;
    r6 = 0x0;
    *(u16*)((u8*)r31 + 0x18) = r9;
    *(u32*)((u8*)r31 + 0x10) = r29;
    *(u32*)((u8*)r31 + 0x14) = r30;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8009769C();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097D94 | size: 0xC4 */
void fn_80097D94(void) {
    extern void fn_8009769C();
    extern void fn_8010B560();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r4 = 0xe;
    r31 = (u32)&lbl_803FB380;
    r9 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r4;
    r5 = r28;
    r7 = r29;
    *(u32*)((u8*)r31 + 0x8) = r9;
    r8 = r30;
    r3 = 0xe;
    r4 = 0x0;
    *(u32*)((u8*)r31 + 0xC) = r28;
    r6 = 0x0;
    *(u16*)((u8*)r31 + 0x18) = r9;
    *(u32*)((u8*)r31 + 0x10) = r29;
    *(u32*)((u8*)r31 + 0x14) = r30;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8009769C();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097E58 | size: 0xB0 */
void fn_80097E58(void) {
    extern void fn_8009769C();
    extern void fn_8010B560();
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

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r4 = 0xac;
    r31 = (u32)&lbl_803FB380;
    r9 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r4;
    r4 = r27;
    r5 = r28;
    *(u32*)((u8*)r31 + 0x8) = r27;
    r7 = r29;
    r8 = r30;
    r3 = 0xac;
    *(u32*)((u8*)r31 + 0xC) = r28;
    r6 = 0x0;
    *(u16*)((u8*)r31 + 0x18) = r9;
    *(u32*)((u8*)r31 + 0x10) = r29;
    *(u32*)((u8*)r31 + 0x14) = r30;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8009769C();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097F08 | size: 0xC4 */
void fn_80097F08(void) {
    extern void fn_8009769C();
    extern void fn_8010B560();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    while (1) {
        fn_8010B560();
        tmp = r3 & 0xFF;
        if (tmp == 0) break;
        ((void(*)(void))_threadSwitch)();

    }
    r3 = (u32)&lbl_803FB380;
    r4 = 0x0;
    r3 = (u32)&lbl_803FB380;
    r5 = 0x44;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)&lbl_803FB380;
    r4 = 0x8e;
    r31 = (u32)&lbl_803FB380;
    r9 = 0x0;
    tmp = -0x1;
    *(u8*)((u8*)r31 + 0x0) = r4;
    r5 = r28;
    r7 = r29;
    *(u32*)((u8*)r31 + 0x8) = r9;
    r8 = r30;
    r3 = 0x8e;
    r4 = 0x0;
    *(u32*)((u8*)r31 + 0xC) = r28;
    r6 = 0x0;
    *(u16*)((u8*)r31 + 0x18) = r9;
    *(u32*)((u8*)r31 + 0x10) = r29;
    *(u32*)((u8*)r31 + 0x14) = r30;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8009769C();
    r3 = *(u32*)((u8*)r31 + 0x4);
    return;
}

/* 0x80097FCC | size: 0x4 */
void fn_80097FCC(void) {
}

/* 0x80097FD0 | size: 0x28 */
void fn_80097FD0(void) {
    extern int fn_80113F48();
    fn_800F9318(fn_80113F48(), 0x12670000);
}

/* 0x80097FF8 | size: 0x4 */
void fn_80097FF8(void) {
}

/* 0x80097FFC | size: 0x8 */
void PPCMfmsr(void) {
}
