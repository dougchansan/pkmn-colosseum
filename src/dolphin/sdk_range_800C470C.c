/**
 * @file sdk_range_800C470C.c
 * @brief dolphin-sdk code, 0x800C470C - 0x800C4E44 (13 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

void __save_fpr(double* frame, double f14, double f15, double f16, double f17,
                double f18, double f19, double f20, double f21, double f22,
                double f23, double f24, double f25, double f26, double f27,
                double f28, double f29, double f30, double f31) {
    frame[-18] = f14;
    frame[-17] = f15;
    frame[-16] = f16;
    frame[-15] = f17;
    frame[-14] = f18;
    frame[-13] = f19;
    frame[-12] = f20;
    frame[-11] = f21;
    frame[-10] = f22;
    frame[-9] = f23;
    frame[-8] = f24;
    frame[-7] = f25;
    frame[-6] = f26;
    frame[-5] = f27;
    frame[-4] = f28;
    frame[-3] = f29;
    frame[-2] = f30;
    frame[-1] = f31;
}

void __restore_fpr(volatile double* frame) {
    (void)frame[-18];
    (void)frame[-17];
    (void)frame[-16];
    (void)frame[-15];
    (void)frame[-14];
    (void)frame[-13];
    (void)frame[-12];
    (void)frame[-11];
    (void)frame[-10];
    (void)frame[-9];
    (void)frame[-8];
    (void)frame[-7];
    (void)frame[-6];
    (void)frame[-5];
    (void)frame[-4];
    (void)frame[-3];
    (void)frame[-2];
    (void)frame[-1];
}

void __save_gpr(volatile u32* frame, u32 r14, u32 r15, u32 r16, u32 r17,
                u32 r18, u32 r19, u32 r20, u32 r21, u32 r22, u32 r23,
                u32 r24, u32 r25, u32 r26, u32 r27, u32 r28, u32 r29,
                u32 r30, u32 r31) {
    frame[-18] = r14;
    frame[-17] = r15;
    frame[-16] = r16;
    frame[-15] = r17;
    frame[-14] = r18;
    frame[-13] = r19;
    frame[-12] = r20;
    frame[-11] = r21;
    frame[-10] = r22;
    frame[-9] = r23;
    frame[-8] = r24;
    frame[-7] = r25;
    frame[-6] = r26;
    frame[-5] = r27;
    frame[-4] = r28;
    frame[-3] = r29;
    frame[-2] = r30;
    frame[-1] = r31;
}

void __restore_gpr(volatile u32* frame) {
    (void)frame[-18];
    (void)frame[-17];
    (void)frame[-16];
    (void)frame[-15];
    (void)frame[-14];
    (void)frame[-13];
    (void)frame[-12];
    (void)frame[-11];
    (void)frame[-10];
    (void)frame[-9];
    (void)frame[-8];
    (void)frame[-7];
    (void)frame[-6];
    (void)frame[-5];
    (void)frame[-4];
    (void)frame[-3];
    (void)frame[-2];
    (void)frame[-1];
}

u64 __div2u(u32 dividendHi, u32 dividendLo, u32 divisorHi, u32 divisorLo) {
    u32 dividendLeading;
    u32 dividendLowLeading;
    u32 divisorLeading;
    u32 divisorLowLeading;
    u32 dividendBits;
    u32 divisorBits;
    u32 shift;
    u32 count;
    u32 quotientHi;
    u32 quotientLo;
    u32 remainderHi;
    u32 remainderLo;
    u32 carry;
    u32 borrow;

    dividendLeading = __cntlzw(dividendHi);
    dividendLowLeading = __cntlzw(dividendLo);

    if ((s32)dividendHi == 0) {
        dividendLeading = dividendLowLeading + 32;
    }

    divisorLeading = __cntlzw(divisorHi);
    divisorLowLeading = __cntlzw(divisorLo);
    if ((s32)divisorHi == 0) {
        divisorLeading = divisorLowLeading + 32;
    }

    dividendBits = 64 - dividendLeading;
    if (dividendLeading > divisorLeading) {
        return 0;
    }

    divisorBits = 64 - (divisorLeading + 1);
    shift = dividendLeading + divisorBits;
    count = dividendBits - divisorBits;

    if (count >= 32) {
        remainderLo = dividendHi >> (count - 32);
        remainderHi = 0;
    } else {
        remainderLo = (dividendLo >> count) |
                      (dividendHi << (32 - count));
        remainderHi = dividendHi >> count;
    }

    if (shift >= 32) {
        quotientHi = dividendLo << (shift - 32);
        quotientLo = 0;
    } else {
        quotientHi = (dividendHi << shift) |
                     (dividendLo >> (32 - shift));
        quotientLo = dividendLo << shift;
    }

    while (count-- != 0) {
        carry = quotientHi >> 31;
        quotientHi = (quotientHi << 1) | (quotientLo >> 31);
        quotientLo <<= 1;
        remainderHi = (remainderHi << 1) | (remainderLo >> 31);
        remainderLo = (remainderLo << 1) | carry;

        if (remainderHi > divisorHi ||
            (remainderHi == divisorHi && remainderLo >= divisorLo)) {
            borrow = remainderLo < divisorLo;
            remainderLo -= divisorLo;
            remainderHi -= divisorHi + borrow;
            quotientLo |= 1;
        }
    }

    return ((u64)quotientHi << 32) | quotientLo;
}

u64 __mod2u(u32 dividendHi, u32 dividendLo, u32 divisorHi, u32 divisorLo) {
    u32 dividendLeading;
    u32 dividendLowLeading;
    u32 divisorLeading;
    u32 divisorLowLeading;
    u32 dividendBits;
    u32 divisorBits;
    u32 shift;
    u32 count;
    u32 quotientHi;
    u32 quotientLo;
    u32 remainderHi;
    u32 remainderLo;
    u32 carry;
    u32 borrow;

    dividendLeading = __cntlzw(dividendHi);
    dividendLowLeading = __cntlzw(dividendLo);

    if ((s32)dividendHi == 0) {
        dividendLeading = dividendLowLeading + 32;
    }

    divisorLeading = __cntlzw(divisorHi);
    divisorLowLeading = __cntlzw(divisorLo);
    if ((s32)divisorHi == 0) {
        divisorLeading = divisorLowLeading + 32;
    }

    dividendBits = 64 - dividendLeading;
    if (dividendLeading > divisorLeading) {
        return ((u64)dividendHi << 32) | dividendLo;
    }

    divisorBits = 64 - (divisorLeading + 1);
    shift = dividendLeading + divisorBits;
    count = dividendBits - divisorBits;

    if (count >= 32) {
        remainderLo = dividendHi >> (count - 32);
        remainderHi = 0;
    } else {
        remainderLo = (dividendLo >> count) |
                      (dividendHi << (32 - count));
        remainderHi = dividendHi >> count;
    }

    if (shift >= 32) {
        quotientHi = dividendLo << (shift - 32);
        quotientLo = 0;
    } else {
        quotientHi = (dividendHi << shift) |
                     (dividendLo >> (32 - shift));
        quotientLo = dividendLo << shift;
    }

    while (count-- != 0) {
        carry = quotientHi >> 31;
        quotientHi = (quotientHi << 1) | (quotientLo >> 31);
        quotientLo <<= 1;
        remainderHi = (remainderHi << 1) | (remainderLo >> 31);
        remainderLo = (remainderLo << 1) | carry;

        if (remainderHi > divisorHi ||
            (remainderHi == divisorHi && remainderLo >= divisorLo)) {
            borrow = remainderLo < divisorLo;
            remainderLo -= divisorLo;
            remainderHi -= divisorHi + borrow;
            quotientLo |= 1;
        }
    }

    return ((u64)remainderHi << 32) | remainderLo;
}

/*
 * Left shift for a 64-bit value represented as (r3:r4) by r5.
 */
u64 __shl2i(u32 r3, u32 r4, u32 r5) {
    u32 r8 = 0x20 - r5;
    u32 r9 = r5 - 0x20;
    u32 r10 = 0;
    union { struct { u32 hi, lo; } s; u64 v; } ret;

    r3 = r3 << r5;
    r10 = r4 >> r8;
    r3 = r3 | r10;
    r10 = r4 << r9;
    r3 = r3 | r10;
    r4 = r4 << r5;

    ret.s.hi = r3;
    ret.s.lo = r4;
    return ret.v;
}

u64 fn_800C4C74(u32 r3, u32 r4, u32 r5) {
    u32 r8 = 0x20 - r5;
    u32 r9 = r5 - 0x20;
    u32 r10 = 0;
    union { struct { u32 hi, lo; } s; u64 v; } ret;

    r4 = r4 >> r5;
    r10 = r3 << r8;
    r4 = r4 | r10;
    r10 = (u32)(r3 >> r9);
    r4 = r4 | r10;
    r3 = r3 >> r5;

    ret.s.hi = r3;
    ret.s.lo = r4;
    return ret.v;
}

u64 __shr2i(u32 r3, u32 r4, u32 r5) {
    s32 r9;
    u32 r8 = 0x20 - r5;
    union { struct { u32 hi, lo; } s; u64 v; } ret;

    r4 = (r4 >> r5) | (r3 << r8);
    r9 = (s32)r5 - 0x20;

    if (r9 > 0) {
        r4 |= (u32)((s32)r3 >> r9);
    }
    r3 = (s32)r3 >> r5;

    ret.s.hi = r3;
    ret.s.lo = r4;
    return ret.v;
}

u64 __cvt_dbl_usll(double value) {
    union {
        double value;
        struct {
            u32 high;
            u32 low;
        } words;
    } bits;
    u32 exponent;
    u32 sign;
    u32 hi;
    u32 lo;
    s32 shift;
    u32 t8;
    u32 t9;
    u32 t10;
    union { struct { u32 hi, lo; } s; u64 v; } ret;

    bits.value = value;
    hi = bits.words.high;
    lo = bits.words.low;
    exponent = (hi >> 20) & 0x7FF;
    if (exponent < 0x3FF) {
        return 0;
    }

    sign = hi;
    hi = (hi & 0xFFFFF) | 0x100000;
    shift = exponent - 0x433;

    if (shift < 0) {
        shift = -shift;
        t8 = 0x20 - shift;
        t9 = shift - 0x20;
        lo = lo >> shift;
        t10 = hi << t8;
        lo = lo | t10;
        t10 = hi >> t9;
        lo = lo | t10;
        hi = hi >> shift;
    } else {
        if (shift > 10) {
            if (sign & 0x80000000) {
                ret.s.hi = 0x80000000;
                ret.s.lo = 0;
                return ret.v;
            }
            ret.s.hi = 0x7FFFFFFF;
            ret.s.lo = 0xFFFFFFFF;
            return ret.v;
        }
        t8 = 0x20 - shift;
        t9 = shift - 0x20;
        hi = hi << shift;
        t10 = lo >> t8;
        hi = hi | t10;
        t10 = lo << t9;
        hi = hi | t10;
        lo = lo << shift;
    }

    ret.s.hi = hi;
    ret.s.lo = lo;
    if (sign & 0x80000000) {
        ret.v = -ret.v;
    }
    return ret.v;
}

#pragma scheduling off
#pragma peephole off
__declspec(weak) extern void __sys_free(void* ptr) {
    extern s32 lbl_80478980;
    extern char lbl_8026FE70[];
    extern char lbl_8026FEA8[];
    extern void OSReport(const char* format, ...);
    extern void* OSGetArenaLo(void);
    extern void* OSGetArenaHi(void);
    extern void OSSetArenaLo(void* addr);
    extern void* fn_8009AB60(void* arenaLo, void* arenaHi, s32 maxHeaps);
    extern s32 fn_8009ABD0(void* start, void* end);
    extern s32 fn_8009AB50(s32 heap);
    extern void fn_8009AAD4(s32 heap, void* ptr);
    void* arenaLo;
    void* arenaHi;

    if (lbl_80478980 == -1) {
        OSReport(lbl_8026FE70);
        OSReport(lbl_8026FEA8);

        arenaLo = OSGetArenaLo();
        arenaHi = OSGetArenaHi();
        arenaLo = fn_8009AB60(arenaLo, arenaHi, 1);
        OSSetArenaLo(arenaLo);

        arenaLo = (void*)(((u32)arenaLo + 31) & ~31);
        arenaHi = (void*)((u32)arenaHi & ~31);
        fn_8009AB50(fn_8009ABD0(arenaLo, arenaHi));
        OSSetArenaLo(arenaHi);
    }

    fn_8009AAD4(lbl_80478980, ptr);
}
#pragma peephole reset
#pragma scheduling on
