/**
 * @file math_exact_800CE04C.c
 * @brief Exact libm wrappers, 0x800CE04C - 0x800CE378.
 */
#include "crt/math.h"

typedef struct __FILE {
    u8 pad0[4];
    union {
        u16 all;
        struct {
            u8 high;
            union {
                u8 raw;
                struct {
                    u8 top:2;
                    u8 orient:2;
                    u8 bottom:4;
                } bits;
            } low;
        } byte;
    } flags;
} __FILE;

typedef union DoubleShape {
    f64 value;
    struct {
        u32 hi;
        u32 lo;
    } parts;
} DoubleShape;

typedef union FloatShape {
    f32 value;
    u32 bits;
} FloatShape;

extern u32 OSGetConsoleType(void);
extern s32 InitializeUART(u32 baud);
extern s32 fn_800CF4EC(const void* data, u32 count);
extern u32 fn_800C3A40(u32 handle, const void* data, u32* count, void* ref);

extern f64 __ieee754_acos(f64 x);
extern f64 __ieee754_asin(f64 x);
extern f64 __ieee754_atan2(f64 y, f64 x);
extern f64 __ieee754_exp(f64 x);
extern f64 __ieee754_fmod(f64 x, f64 y);
extern f64 __ieee754_log(f64 x);
extern f64 __ieee754_pow(f64 x, f64 y);
extern s32 __ieee754_rem_pio2(f64 x, f64* y);
extern f64 __ieee754_sqrt(f64 x);
extern f64 __kernel_cos(f64 x, f64 y);
extern f64 __kernel_sin(f64 x, f64 y, s32 iy);
extern f64 __kernel_tan(f64 x, f64 y, s32 iy);
extern f64 atan(f64 x);
s32 __fpclassifyf(f32 x);

extern s32 lbl_8047AA18;
extern s32 lbl_8047AA10;
extern const f32 lbl_80478AC0[];

extern const f64 lbl_8047C528;
extern const f64 lbl_8047C530;
extern const f64 lbl_8047C538;
extern const f64 lbl_8047C540;
extern const f64 lbl_8047C548;
extern const f64 lbl_8047C550;
extern const f64 lbl_8047C558;
extern const f64 lbl_8047C560;
extern const f64 lbl_8047C568;
extern const f64 lbl_8047C570;
extern const f64 lbl_8047C578;
extern const f64 lbl_8047C900;
extern const f64 lbl_8047C908;
extern const f64 lbl_8047C910;
extern const f64 lbl_8047C918;
extern const f64 lbl_8047C920;
extern const f64 lbl_8047C928;
extern const f64 lbl_8047C930;
extern const f64 lbl_8047C938;
extern const f64 lbl_8047C940;
extern const f64 lbl_8047C948;
extern const f64 lbl_8047C950;
extern const f64 lbl_8047C958;
extern const f64 lbl_8047C960;
extern const f64 lbl_8047C968;
extern const f32 lbl_8047C970;
extern const f64 lbl_8047C978;
extern const f64 lbl_8047C980;
extern const f64 lbl_8047C988;


f64 modf(f64 x, f64* integral) {
    DoubleShape shape;
    DoubleShape* intpart;
    s32 hi;
    u32 lo;
    s32 j0;
    s32 i;
    u32 ui;

    shape.value = x;
    intpart = (DoubleShape*)integral;
    hi = shape.parts.hi;
    lo = shape.parts.lo;
    j0 = ((hi >> 20) & 0x7ff) - 0x3ff;

    if (j0 < 20) {
        if (j0 < 0) {
            intpart->parts.hi = hi & 0x80000000;
            intpart->parts.lo = 0;
            return x;
        }
        i = 0x000fffff >> j0;
        if (((hi & i) | lo) == 0) {
            shape.parts.hi = hi & 0x80000000;
            shape.parts.lo = 0;
            *integral = x;
            return shape.value;
        }
        intpart->parts.hi = hi & ~i;
        intpart->parts.lo = 0;
        return x - *integral;
    }

    if (j0 > 51) {
        shape.parts.hi = hi & 0x80000000;
        shape.parts.lo = 0;
        *integral = x;
        return shape.value;
    }

    ui = 0xffffffffU >> (j0 - 20);
    if ((lo & ui) == 0) {
        shape.parts.hi = hi & 0x80000000;
        shape.parts.lo = 0;
        *integral = x;
        return shape.value;
    }

    intpart->parts.hi = hi;
    intpart->parts.lo = lo & ~ui;
    return x - *integral;
}

f64 sin(f64 x) {
    DoubleShape shape;
    f64 y[2];
    s32 ix;
    s32 n;

    shape.value = x;
    ix = shape.parts.hi & 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_sin(x, lbl_8047C958, 0);
    }
    if (ix >= 0x7ff00000) {
        return x - x;
    }

    n = __ieee754_rem_pio2(x, y);
    switch (n & 3) {
    case 0:
        return __kernel_sin(y[0], y[1], 1);
    case 1:
        return __kernel_cos(y[0], y[1]);
    case 2:
        return -__kernel_sin(y[0], y[1], 1);
    default:
        return -__kernel_cos(y[0], y[1]);
    }
}

f64 tan(f64 x) {
    DoubleShape shape;
    f64 y[2];
    s32 ix;
    s32 n;

    shape.value = x;
    ix = shape.parts.hi & 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_tan(x, lbl_8047C960, 1);
    }
    if (ix >= 0x7ff00000) {
        return x - x;
    }

    n = __ieee754_rem_pio2(x, y);
    return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
}

f64 acos(f64 x) {
    return __ieee754_acos(x);
}

f64 asin(f64 x) {
    return __ieee754_asin(x);
}

f64 atan2(f64 y, f64 x) {
    return __ieee754_atan2(y, x);
}

f64 exp(f64 x) {
    return __ieee754_exp(x);
}

f64 fmod(f64 x, f64 y) {
    return __ieee754_fmod(x, y);
}

f64 log(f64 x) {
    return __ieee754_log(x);
}

f64 pow(f64 x, f64 y) {
    return __ieee754_pow(x, y);
}
