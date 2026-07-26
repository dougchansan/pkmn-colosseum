/**
 * @file math_range_800CAA58.c
 * @brief libm code, 0x800CAA58 - 0x800CE79C (40 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01).
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

s32 fwide(__FILE* file, s32 mode) {
    u8 orient;

    if (file == NULL || (((u32)file->flags.all >> 6) & 7) == 0) {
        return 0;
    }

    orient = file->flags.byte.low.bits.orient;
    switch (orient) {
    case 0:
        if (mode > 0) {
            file->flags.byte.low.bits.orient = 2;
        } else if (mode < 0) {
            file->flags.byte.low.bits.orient = 1;
        }
        return mode;
    case 2:
        return 1;
    case 1:
        return -1;
    default:
        return (s32)file;
    }
}

s32 __write_console(u32 handle, const void* data, u32* count, void* ref) {
    s32 err;

    if ((OSGetConsoleType() & 0x20000000) == 0) {
        err = 0;
        if (lbl_8047AA18 == 0) {
            err = InitializeUART(0xE100);
            if (err == 0) {
                lbl_8047AA18 = 1;
            }
        }

        if (err != 0) {
            return 1;
        }

        if (fn_800CF4EC(data, *count) != 0) {
            *count = 0;
            return 1;
        }
    }

    fn_800C3A40(handle, data, count, ref);
    return 0;
}

f64 __ieee754_atan2(f64 y, f64 x) {
    f64 z;
    s32 k;
    s32 m;
    s32 hx;
    s32 hy;
    s32 ix;
    s32 iy;
    u32 lx;
    u32 ly;

    hx = *(s32*)&x;
    ix = hx & 0x7fffffff;
    lx = *((u32*)&x + 1);
    hy = *(s32*)&y;
    iy = hy & 0x7fffffff;
    ly = *((u32*)&y + 1);

    if (((ix | ((lx | -lx) >> 31)) > 0x7ff00000) ||
        ((iy | ((ly | -ly) >> 31)) > 0x7ff00000)) {
        return x + y;
    }
    if (((hx - 0x3ff00000) | lx) == 0) {
        return atan(y);
    }

    m = ((hy >> 31) & 1) | ((hx >> 30) & 2);
    if ((iy | ly) == 0) {
        switch (m) {
        case 0:
        case 1:
            return y;
        case 2:
            return lbl_8047C528;
        case 3:
            return lbl_8047C530;
        }
    }

    if ((ix | lx) == 0) {
        return hy < 0 ? lbl_8047C538 : lbl_8047C540;
    }

    if (ix == 0x7ff00000) {
        if (iy == 0x7ff00000) {
            switch (m) {
            case 0:
                return lbl_8047C548;
            case 1:
                return lbl_8047C550;
            case 2:
                return lbl_8047C558;
            case 3:
                return lbl_8047C560;
            }
        } else {
            switch (m) {
            case 0:
                return lbl_8047C568;
            case 1:
                return lbl_8047C570;
            case 2:
                return lbl_8047C528;
            case 3:
                return lbl_8047C530;
            }
        }
    }

    if (iy == 0x7ff00000) {
        return hy < 0 ? lbl_8047C538 : lbl_8047C540;
    }

    k = (iy - ix) >> 20;
    if (k > 60) {
        z = lbl_8047C540;
    } else if (hx < 0 && k < -60) {
        z = lbl_8047C568;
    } else {
        z = atan(__fabs(y / x));
    }

    switch (m) {
    case 0:
        return z;
    case 1:
        *(u32*)&z ^= 0x80000000;
        return z;
    case 2:
        return lbl_8047C528 - (z - lbl_8047C578);
    default:
        return (z - lbl_8047C578) - lbl_8047C528;
    }
}

f64 __kernel_cos(f64 x, f64 y) {
    extern const f64 lbl_8047C7E0;
    extern const f64 lbl_8047C7E8;
    extern const f64 lbl_8047C7F0;
    extern const f64 lbl_8047C7F8;
    extern const f64 lbl_8047C800;
    extern const f64 lbl_8047C808;
    extern const f64 lbl_8047C810;
    extern const f64 lbl_8047C818;
    extern const f64 lbl_8047C820;
    DoubleShape qshape;
    DoubleShape shape;
    f64 z;
    f64 r;
    f64 hz;
    f64 a;
    s32 ix;

    shape.value = x;
    ix = shape.parts.hi & 0x7fffffff;
    if (ix < 0x3e400000 && (s32)x == 0) {
        return lbl_8047C7E0;
    }

    z = shape.value * shape.value;
    r = z * (lbl_8047C7E8 + z * (lbl_8047C7F0 + z * (lbl_8047C7F8 +
        z * (lbl_8047C800 + z * (lbl_8047C808 + lbl_8047C810 * z)))));
    if (ix < 0x3fd33333) {
        return lbl_8047C7E0 - (lbl_8047C818 * z - (z * r - shape.value * y));
    }

    if (ix > 0x3fe90000) {
        qshape.value = lbl_8047C820;
    } else {
        qshape.parts.hi = ix - 0x00200000;
        qshape.parts.lo = 0;
    }
    hz = lbl_8047C818 * z - qshape.value;
    a = lbl_8047C7E0 - qshape.value;
    return a - (hz - (z * r - shape.value * y));
}

f64 __kernel_sin(f64 x, f64 y, s32 iy) {
    extern const f64 lbl_8047C868;
    extern const f64 lbl_8047C870;
    extern const f64 lbl_8047C878;
    extern const f64 lbl_8047C880;
    extern const f64 lbl_8047C888;
    extern const f64 lbl_8047C890;
    extern const f64 lbl_8047C898;
    DoubleShape shape;
    f64 z;
    f64 r;
    f64 v;
    s32 ix;

    shape.value = x;
    ix = shape.parts.hi & 0x7fffffff;
    if (ix < 0x3e400000 && (s32)x == 0) {
        return x;
    }

    z = shape.value * shape.value;
    v = z * shape.value;
    r = lbl_8047C868 + z * (lbl_8047C870 + z * (lbl_8047C878 +
        z * (lbl_8047C880 + lbl_8047C888 * z)));
    if (iy == 0) {
        return shape.value + v * (lbl_8047C890 + z * r);
    }
    return shape.value - ((z * (lbl_8047C898 * y - v * r) - y) -
        lbl_8047C890 * v);
}

f64 copysign(f64 x, f64 y) {
    DoubleShape uy;
    DoubleShape ux;

    ux.value = x;
    uy.value = y;
    ux.parts.hi = (ux.parts.hi & 0x7fffffff) | (uy.parts.hi & 0x80000000);
    return ux.value;
}

#pragma dont_inline on
f64 cos(f64 x) {
    DoubleShape shape;
    f64 y[2];
    s32 ix;
    s32 n;

    shape.value = x;
    ix = shape.parts.hi & 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_cos(x, lbl_8047C910);
    }
    if (ix >= 0x7ff00000) {
        return x - x;
    }

    n = __ieee754_rem_pio2(x, y);
    switch (n & 3) {
    case 0:
        return __kernel_cos(y[0], y[1]);
    case 1:
        return -__kernel_sin(y[0], y[1], 1);
    case 2:
        return -__kernel_cos(y[0], y[1]);
    default:
        return __kernel_sin(y[0], y[1], 1);
    }
}

f64 ceil(f64 x) {
    DoubleShape shape;
    s32 i0;
    s32 i1;
    s32 j0;
    u32 i;
    u32 j;

    shape.value = x;
    i0 = shape.parts.hi;
    i1 = shape.parts.lo;
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;

    if (j0 < 20) {
        if (j0 < 0) {
            if (lbl_8047C900 + x > lbl_8047C908) {
                if (i0 < 0) {
                    i0 = 0x80000000;
                    i1 = 0;
                } else if ((i0 | i1) != 0) {
                    i0 = 0x3ff00000;
                    i1 = 0;
                }
            }
        } else {
            i = 0x000fffff >> j0;
            if (((i0 & i) | i1) == 0) {
                return x;
            }
            if (lbl_8047C900 + x > lbl_8047C908) {
                if (i0 > 0) {
                    i0 += 0x00100000 >> j0;
                }
                i0 &= ~i;
                i1 = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) {
            return x + x;
        }
        return x;
    } else {
        i = 0xffffffffU >> (j0 - 20);
        if ((i1 & i) == 0) {
            return x;
        }
        if (lbl_8047C900 + x > lbl_8047C908) {
            if (i0 > 0) {
                if (j0 == 20) {
                    i0 += 1;
                } else {
                    j = i1 + (1 << (52 - j0));
                    if (j < (u32)i1) {
                        i0 += 1;
                    }
                    i1 = j;
                }
            }
            i1 &= ~i;
        }
    }

    shape.parts.hi = i0;
    shape.parts.lo = i1;
    return shape.value;
}

f64 floor(f64 x) {
    DoubleShape shape;
    s32 hi;
    u32 lo;
    s32 j0;
    s32 i;
    u32 ui;
    u32 old;

    shape.value = x;
    hi = shape.parts.hi;
    lo = shape.parts.lo;
    j0 = ((hi >> 20) & 0x7ff) - 0x3ff;

    if (j0 < 20) {
        if (j0 < 0) {
            if (lbl_8047C918 + x > lbl_8047C920) {
                if (hi >= 0) {
                    hi = 0;
                    lo = 0;
                } else if (((hi & 0x7fffffff) | lo) != 0) {
                    hi = 0xbff00000;
                    lo = 0;
                }
            }
        } else {
            i = 0x000fffff >> j0;
            if (((hi & i) | lo) == 0) {
                return x;
            }
            if (lbl_8047C918 + x > lbl_8047C920) {
                if (hi < 0) {
                    hi += 0x00100000 >> j0;
                }
                hi &= ~i;
                lo = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) {
            return x + x;
        }
        return x;
    } else {
        ui = 0xffffffffU >> (j0 - 20);
        if ((lo & ui) == 0) {
            return x;
        }
        if (lbl_8047C918 + x > lbl_8047C920) {
            if (hi < 0) {
                if (j0 == 20) {
                    hi += 1;
                } else {
                    i = 1 << (52 - j0);
                    old = lo;
                    lo += i;
                    if (lo < old) {
                        hi += 1;
                    }
                }
            }
            lo &= ~ui;
        }
    }

    shape.parts.hi = hi;
    shape.parts.lo = lo;
    return shape.value;
}

f64 frexp(f64 x, s32* exponent) {
    DoubleShape shape;
    s32 hi;
    s32 ix;
    s32 lo;

    shape.value = x;
    hi = shape.parts.hi;
    *exponent = 0;
    ix = hi & 0x7fffffff;
    lo = shape.parts.lo;
    if (ix >= 0x7ff00000 || ((ix | lo) == 0)) {
        return shape.value;
    }
    if (ix < 0x00100000) {
        *exponent = -54;
        shape.value = x * lbl_8047C928;
        hi = shape.parts.hi;
        ix = hi & 0x7fffffff;
    }
    *exponent += (ix >> 20) - 0x3fe;
    hi = (hi & 0x800fffff) | 0x3fe00000;
    shape.parts.hi = hi;
    return shape.value;
}

f64 ldexp(f64 x, s32 n) {
    DoubleShape shape;
    s32 hi;
    u32 lo;
    s32 k;
    s32 fpclass;

    shape.value = x;
    hi = shape.parts.hi;
    lo = shape.parts.lo;
    k = hi & 0x7ff00000;

    if (k == 0x7ff00000) {
        if ((hi & 0x000fffff) != 0 || lo != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
    } else if (k == 0) {
        if ((hi & 0x000fffff) != 0 || lo != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
    } else {
        fpclass = 4;
    }

    if (fpclass <= 2 || x == lbl_8047C930) {
        return x;
    }

    hi = shape.parts.hi;
    lo = shape.parts.lo;
    k = (hi >> 20) & 0x7ff;

    if (k == 0) {
        if (((hi & 0x7fffffff) | lo) == 0) {
            return x;
        }
        x *= lbl_8047C938;
        shape.value = x;
        hi = shape.parts.hi;
        k = ((hi >> 20) & 0x7ff) - 54;
        if (n < -50000) {
            return lbl_8047C940 * x;
        }
    }
    if (k == 0x7ff) {
        return x + x;
    }

    k += n;
    if (k > 0x7fe) {
        return lbl_8047C948 * copysign(lbl_8047C948, x);
    }
    if (k > 0) {
        shape.parts.hi = (hi & 0x800fffff) | (k << 20);
        return shape.value;
    }
    if (k <= -54) {
        if (n > 50000) {
            return lbl_8047C948 * copysign(lbl_8047C948, x);
        }
        return lbl_8047C940 * copysign(lbl_8047C940, x);
    }

    k += 54;
    shape.parts.hi = (hi & 0x800fffff) | (k << 20);
    return lbl_8047C950 * shape.value;
}

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
#pragma dont_inline reset

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

f64 fabs(f64 x) {
    return __fabs(x);
}

f32 sqrtf(f32 x) {
    FloatShape shape;
    f64 y;
    u32 bits;
    s32 exp;
    s32 fpclass;

    if (x > lbl_8047C970) {
        y = __frsqrte(x);
        y = lbl_8047C978 * y * (lbl_8047C980 - x * (y * y));
        y = lbl_8047C978 * y * (lbl_8047C980 - x * (y * y));
        y = lbl_8047C978 * y * (lbl_8047C980 - x * (y * y));
        return (f32)(x * y);
    }

    if ((f64)x < lbl_8047C988) {
        return lbl_80478AC0[0];
    }

    shape.value = x;
    bits = shape.bits;
    exp = bits & 0x7f800000;
    switch (exp) {
    case 0x7f800000:
        if ((bits & 0x007fffff) != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
        break;
    case 0:
        if ((bits & 0x007fffff) != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
        break;
    default:
        fpclass = 4;
        break;
    }

    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }

    return x;
}

f32 tanf(f32 x) {
    return (f32)tan(x);
}

f32 sinf(f32 x) {
    return (f32)sin(x);
}

f32 cosf(f32 x) {
    return (f32)cos(x);
}

f32 acosf(f32 x) {
    return (f32)acos(x);
}

s32 __fpclassifyf(f32 x) {
    FloatShape shape;
    u32 bits;
    s32 exp;
    u32 frac;

    shape.value = x;
    bits = shape.bits;
    exp = bits & 0x7f800000;
    switch (exp) {
    case 0x7f800000:
        frac = bits & 0x007fffff;
        return ((s32)(-frac | frac) >> 31) + 2;
    case 0:
        return (bits & 0x007fffff) ? 5 : 3;
    default:
        return 4;
    }
}

/*
 * fdlibm e_sqrt.c, bit-by-bit integer square root.  The MSL variant sets
 * errno to EDOM (33) for non-finite and negative cases.
 */
f64 __ieee754_sqrt(f64 x) {
#define SQRT_ONE  1.00000000000000000000e+00
#define SQRT_TINY 1.00000000000000002506e-300
    f64 z;
    u32 sign = 0x80000000;
    u32 r;
    u32 t1;
    u32 s1;
    u32 ix1;
    u32 q1;
    s32 ix0;
    s32 s0;
    s32 q;
    s32 m;
    s32 t;
    s32 i;

    ix0 = *(s32*)&x;
    ix1 = *((u32*)&x + 1);

    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        lbl_8047AA10 = 33;
        return x * x + x;
    }

    if (ix0 <= 0) {
        if (((ix0 & ~sign) | ix1) == 0) {
            return x;
        }
        if (ix0 < 0) {
            lbl_8047AA10 = 33;
            return lbl_80478AC0[0];
        }
    }

    m = ix0 >> 20;
    if (m == 0) {
        while (ix0 == 0) {
            m -= 21;
            ix0 |= ix1 >> 11;
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++) {
            ix0 <<= 1;
        }
        m -= i - 1;
        ix0 |= ix1 >> (32 - i);
        ix1 <<= i;
    }

    m -= 1023;
    ix0 = (ix0 & 0x000fffff) | 0x00100000;
    if (m & 1) {
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1;

    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0;
    r = 0x00200000;

    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;
    while (r != 0) {
        t1 = s1 + r;
        t = s0;
        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1 = t1 + r;
            if (((t1 & sign) == sign) && (s1 & sign) == 0) {
                s0 += 1;
            }
            ix0 -= t;
            if (ix1 < t1) {
                ix0 -= 1;
            }
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    if ((ix0 | ix1) != 0) {
        z = SQRT_ONE - SQRT_TINY;
        if (z >= SQRT_ONE) {
            z = SQRT_ONE + SQRT_TINY;
            if (q1 == 0xffffffff) {
                q1 = 0;
                q += 1;
            } else if (z > SQRT_ONE) {
                if (q1 == 0xfffffffe) {
                    q += 1;
                }
                q1 += 2;
            } else {
                q1 += (q1 & 1);
            }
        }
    }

    ix0 = (q >> 1) + 0x3fe00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1) {
        ix1 |= sign;
    }
    ix0 += (m << 20);
    *(s32*)&z = ix0;
    *((u32*)&z + 1) = ix1;
    return z;
#undef SQRT_ONE
#undef SQRT_TINY
}

f64 sqrt(f64 x) {
    return __ieee754_sqrt(x);
}

/*
 * Payne-Hanek argument reduction from fdlibm.  x[] contains 24-bit chunks
 * of the input and ipio2[] contains the 24-bit expansion of 2/pi.
 */
#pragma use_lmw_stmw on
s32 __kernel_rem_pio2(f64* x, f64* y, s32 e0, s32 nx, s32 prec,
                      const s32* ipio2)
{
    extern const s32 lbl_80270200[];
    extern const f64 lbl_80270210[];
#define K_ZERO   0.0
#define K_TWON24 5.96046447753906250000e-08
#define K_TWO24  1.67772160000000000000e+07
#define K_EIGHT  8.0
#define K_EIGHTH 0.125
#define K_HALF   0.5
#define K_ONE    1.0

    s32 jz;
    s32 jx;
    s32 jv;
    s32 jp;
    s32 jk;
    s32 carry;
    s32 n;
    s32 iq[20];
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 q0;
    s32 ih;
    f64 z;
    f64 fw;
    f64 f[20];
    f64 fq[20];
    f64 q[20];

    jk = lbl_80270200[prec];
    jp = jk;
    jx = nx - 1;
    jv = (e0 - 3) / 24;
    if (jv < 0) {
        jv = 0;
    }
    q0 = e0 - 24 * (jv + 1);

    j = jv - jx;
    m = jx + jk;
    for (i = 0; i <= m; i++, j++) {
        f[i] = (j < 0) ? K_ZERO : (f64)ipio2[j];
    }

    for (i = 0; i <= jk; i++) {
        for (j = 0, fw = K_ZERO; j <= jx; j++) {
            fw += x[j] * f[jx + i - j];
        }
        q[i] = fw;
    }

    jz = jk;
recompute:
    for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
        fw = (f64)((s32)(K_TWON24 * z));
        iq[i] = (s32)(z - K_TWO24 * fw);
        z = q[j - 1] + fw;
    }

    z = ldexp(z, q0);
    z -= K_EIGHT * floor(z * K_EIGHTH);
    n = (s32)z;
    z -= (f64)n;
    ih = 0;
    if (q0 > 0) {
        i = iq[jz - 1] >> (24 - q0);
        n += i;
        iq[jz - 1] -= i << (24 - q0);
        ih = iq[jz - 1] >> (23 - q0);
    } else if (q0 == 0) {
        ih = iq[jz - 1] >> 23;
    } else if (z >= K_HALF) {
        ih = 2;
    }

    if (ih > 0) {
        n += 1;
        carry = 0;
        for (i = 0; i < jz; i++) {
            j = iq[i];
            if (carry == 0) {
                if (j != 0) {
                    carry = 1;
                    iq[i] = 0x1000000 - j;
                }
            } else {
                iq[i] = 0xffffff - j;
            }
        }
        if (q0 > 0) {
            switch (q0) {
            case 1:
                iq[jz - 1] &= 0x7fffff;
                break;
            case 2:
                iq[jz - 1] &= 0x3fffff;
                break;
            }
        }
        if (ih == 2) {
            z = K_ONE - z;
            if (carry != 0) {
                z -= ldexp(K_ONE, q0);
            }
        }
    }

    if (z == K_ZERO) {
        j = 0;
        for (i = jz - 1; i >= jk; i--) {
            j |= iq[i];
        }
        if (j == 0) {
            for (k = 1; iq[jk - k] == 0; k++) {
            }
            for (i = jz + 1; i <= jz + k; i++) {
                f[jx + i] = (f64)ipio2[jv + i];
                for (j = 0, fw = K_ZERO; j <= jx; j++) {
                    fw += x[j] * f[jx + i - j];
                }
                q[i] = fw;
            }
            jz += k;
            goto recompute;
        }
    }

    if (z == K_ZERO) {
        jz -= 1;
        q0 -= 24;
        while (iq[jz] == 0) {
            jz--;
            q0 -= 24;
        }
    } else {
        z = ldexp(z, -q0);
        if (z >= K_TWO24) {
            fw = (f64)((s32)(K_TWON24 * z));
            iq[jz] = (s32)(z - K_TWO24 * fw);
            jz += 1;
            q0 += 24;
            iq[jz] = (s32)fw;
        } else {
            iq[jz] = (s32)z;
        }
    }

    fw = ldexp(K_ONE, q0);
    for (i = jz; i >= 0; i--) {
        q[i] = fw * (f64)iq[i];
        fw *= K_TWON24;
    }

    for (i = jz; i >= 0; i--) {
        for (fw = K_ZERO, k = 0;
             k <= jp && k <= jz - i; k++) {
            fw += lbl_80270210[k] * q[i + k];
        }
        fq[jz - i] = fw;
    }

    switch (prec) {
    case 0:
        fw = K_ZERO;
        for (i = jz; i >= 0; i--) {
            fw += fq[i];
        }
        y[0] = (ih == 0) ? fw : -fw;
        break;
    case 1:
    case 2:
        fw = K_ZERO;
        for (i = jz; i >= 0; i--) {
            fw += fq[i];
        }
        y[0] = (ih == 0) ? fw : -fw;
        fw = fq[0] - fw;
        for (i = 1; i <= jz; i++) {
            fw += fq[i];
        }
        y[1] = (ih == 0) ? fw : -fw;
        break;
    case 3:
        for (i = jz; i > 0; i--) {
            fw = fq[i - 1] + fq[i];
            fq[i] += fq[i - 1] - fw;
            fq[i - 1] = fw;
        }
        for (i = jz; i > 1; i--) {
            fw = fq[i - 1] + fq[i];
            fq[i] += fq[i - 1] - fw;
            fq[i - 1] = fw;
        }
        fw = K_ZERO;
        for (i = jz; i >= 2; i--) {
            fw += fq[i];
        }
        if (ih == 0) {
            y[0] = fq[0];
            y[1] = fq[1];
            y[2] = fw;
        } else {
            y[0] = -fq[0];
            y[1] = -fq[1];
            y[2] = -fw;
        }
        break;
    }
    return n & 7;
#undef K_ZERO
#undef K_TWON24
#undef K_TWO24
#undef K_EIGHT
#undef K_EIGHTH
#undef K_HALF
#undef K_ONE
}
#pragma use_lmw_stmw off


f64 __ieee754_pow(f64 x, f64 y) {
    extern f64 fn_800CDE88(f64, s32);
    static const f64 bp[2] = { 1.0, 1.5 };
    static const f64 dp_h[2] = {
        0.0, 5.84962487220764160156e-01
    };
    static const f64 dp_l[2] = {
        0.0, 1.35003920212974897128e-08
    };
    const f64 zero = 0.0;
    const f64 one = 1.0;
    const f64 two = 2.0;
    const f64 two53 = 9007199254740992.0;
    const f64 huge = 1.0e300;
    const f64 tiny = 1.0e-300;
    const f64 L1 = 5.99999999999994648725e-01;
    const f64 L2 = 4.28571428578550184252e-01;
    const f64 L3 = 3.33333329818377432918e-01;
    const f64 L4 = 2.72728123808534006489e-01;
    const f64 L5 = 2.30660745775561754067e-01;
    const f64 L6 = 2.06975017800338417784e-01;
    const f64 P1 = 1.66666666666666019037e-01;
    const f64 P2 = -2.77777777770155933842e-03;
    const f64 P3 = 6.61375632143793436117e-05;
    const f64 P4 = -1.65339022054652515390e-06;
    const f64 P5 = 4.13813679705723846039e-08;
    const f64 lg2 = 6.93147180559945286227e-01;
    const f64 lg2_h = 6.93147182464599609375e-01;
    const f64 lg2_l = -1.90465429995776804525e-09;
    const f64 ovt = 8.0085662595372944372e-17;
    const f64 cp = 9.61796693925975554329e-01;
    const f64 cp_h = 9.61796700954437255859e-01;
    const f64 cp_l = -7.02846165095275826516e-09;
    const f64 ivln2 = 1.44269504088896338700e+00;
    const f64 ivln2_h = 1.44269502162933349609e+00;
    const f64 ivln2_l = 1.92596299112661746887e-08;
    DoubleShape sx;
    DoubleShape sy;
    DoubleShape sw;
    f64 z;
    f64 ax;
    f64 z_h;
    f64 z_l;
    f64 p_h;
    f64 p_l;
    f64 y1;
    f64 t1;
    f64 t2;
    f64 r;
    f64 s;
    f64 t;
    f64 u;
    f64 v;
    f64 w;
    f64 t_h;
    f64 t_l;
    f64 s_l;
    f64 s2;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 yisint;
    s32 hx;
    s32 hy;
    s32 ix;
    s32 iy;
    u32 lx;
    u32 ly;

    sx.value = x;
    sy.value = y;
    hx = (s32)sx.parts.hi;
    lx = sx.parts.lo;
    hy = (s32)sy.parts.hi;
    ly = sy.parts.lo;
    ix = hx & 0x7FFFFFFF;
    iy = hy & 0x7FFFFFFF;

    if ((iy | ly) == 0) {
        return one;
    }
    if (ix > 0x7FF00000 ||
        (ix == 0x7FF00000 && lx != 0) ||
        iy > 0x7FF00000 ||
        (iy == 0x7FF00000 && ly != 0)) {
        return x + y;
    }

    yisint = 0;
    if (hx < 0) {
        if (iy >= 0x43400000) {
            yisint = 2;
        } else if (iy >= 0x3FF00000) {
            k = (iy >> 20) - 0x3FF;
            if (k > 20) {
                j = (s32)(ly >> (52 - k));
                if ((u32)(j << (52 - k)) == ly) {
                    yisint = 2 - (j & 1);
                }
            } else if (ly == 0) {
                j = iy >> (20 - k);
                if ((j << (20 - k)) == iy) {
                    yisint = 2 - (j & 1);
                }
            }
        }
    }

    if (ly == 0) {
        if (iy == 0x7FF00000) {
            if (((ix - 0x3FF00000) | lx) == 0) {
                return y - y;
            }
            if (ix >= 0x3FF00000) {
                return hy >= 0 ? y : zero;
            }
            return hy < 0 ? -y : zero;
        }
        if (iy == 0x3FF00000) {
            return hy < 0 ? one / x : x;
        }
        if (hy == 0x40000000) {
            return x * x;
        }
        if (hy == 0x3FE00000 && hx >= 0) {
            return __ieee754_sqrt(x);
        }
    }

    ax = x < 0 ? -x : x;
    if (lx == 0) {
        if (ix == 0x7FF00000 || ix == 0 || ix == 0x3FF00000) {
            z = ax;
            if (hy < 0) {
                z = one / z;
            }
            if (hx < 0) {
                if (((ix - 0x3FF00000) | yisint) == 0) {
                    z = (z - z) / (z - z);
                } else if (yisint == 1) {
                    z = -z;
                }
            }
            return z;
        }
    }

    n = (hx >> 31) + 1;
    if ((n | yisint) == 0) {
        return (x - x) / (x - x);
    }
    s = one;
    if ((n | (yisint - 1)) == 0) {
        s = -one;
    }

    if (iy > 0x41E00000) {
        if (iy > 0x43F00000) {
            if (ix <= 0x3FEFFFFF) {
                return hy < 0 ? huge * huge : tiny * tiny;
            }
            if (ix >= 0x3FF00000) {
                return hy > 0 ? huge * huge : tiny * tiny;
            }
        }
        if (ix < 0x3FEFFFFF) {
            return hy < 0 ? s * huge * huge : s * tiny * tiny;
        }
        if (ix > 0x3FF00000) {
            return hy > 0 ? s * huge * huge : s * tiny * tiny;
        }
        t = ax - one;
        w = (t * t) * (0.5 - t * (0.3333333333333333333333 -
                                  t * 0.25));
        u = ivln2_h * t;
        v = t * ivln2_l - w * ivln2;
        t1 = u + v;
        sw.value = t1;
        sw.parts.lo = 0;
        t1 = sw.value;
        t2 = v - (t1 - u);
    } else {
        n = 0;
        if (ix < 0x00100000) {
            ax *= two53;
            n -= 53;
            sw.value = ax;
            ix = (s32)sw.parts.hi;
        }
        n += (ix >> 20) - 0x3FF;
        j = ix & 0x000FFFFF;
        ix = j | 0x3FF00000;
        if (j <= 0x3988E) {
            k = 0;
        } else if (j < 0xBB67A) {
            k = 1;
        } else {
            k = 0;
            n += 1;
            ix -= 0x00100000;
        }
        sw.value = ax;
        sw.parts.hi = (u32)ix;
        ax = sw.value;
        u = ax - bp[k];
        v = one / (ax + bp[k]);
        s = u * v;
        sw.value = s;
        sw.parts.lo = 0;
        s = sw.value;
        t = ax + bp[k];
        sw.value = t;
        sw.parts.lo = 0;
        t_h = sw.value;
        t_l = ax - (t_h - bp[k]);
        s_l = v * ((u - s * t_h) - s * t_l);
        s2 = s * s;
        r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 +
            s2 * (L4 + s2 * (L5 + s2 * L6)))));
        r += s_l * (s + s);
        s2 = s * s;
        t_h = 3.0 + s2 + r;
        sw.value = t_h;
        sw.parts.lo = 0;
        t_h = sw.value;
        t_l = r - ((t_h - 3.0) - s2);
        u = s * t_h;
        v = s_l * t_h + t_l * s;
        p_h = u + v;
        sw.value = p_h;
        sw.parts.lo = 0;
        p_h = sw.value;
        p_l = v - (p_h - u);
        z_h = cp_h * p_h;
        z_l = cp_l * p_h + p_l * cp + dp_l[k];
        t = (f64)n;
        t1 = ((z_h + z_l) + dp_h[k]) + t;
        sw.value = t1;
        sw.parts.lo = 0;
        t1 = sw.value;
        t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
    }

    y1 = y;
    sw.value = y1;
    sw.parts.lo = 0;
    y1 = sw.value;
    p_l = (y - y1) * t1 + y * t2;
    p_h = y1 * t1;
    z = p_l + p_h;
    sw.value = z;
    j = (s32)sw.parts.hi;
    i = (s32)sw.parts.lo;
    if (j >= 0x40900000) {
        if (((j - 0x40900000) | i) != 0) {
            return s * huge * huge;
        }
        if (p_l + ovt > z - p_h) {
            return s * huge * huge;
        }
    } else if ((j & 0x7FFFFFFF) >= 0x4090CC00) {
        if (((j - (s32)0xC090CC00) | i) != 0) {
            return s * tiny * tiny;
        }
        if (p_l <= z - p_h) {
            return s * tiny * tiny;
        }
    }

    i = j & 0x7FFFFFFF;
    k = (i >> 20) - 0x3FF;
    n = 0;
    if (i > 0x3FE00000) {
        n = j + (0x00100000 >> (k + 1));
        k = ((n & 0x7FFFFFFF) >> 20) - 0x3FF;
        sw.parts.hi = (u32)(n & ~(0x000FFFFF >> k));
        sw.parts.lo = 0;
        t = sw.value;
        n = ((n & 0x000FFFFF) | 0x00100000) >> (20 - k);
        if (j < 0) {
            n = -n;
        }
        p_h -= t;
    }
    t = p_l + p_h;
    sw.value = t;
    sw.parts.lo = 0;
    t = sw.value;
    u = t * lg2_h;
    v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
    z = u + v;
    w = v - (z - u);
    t = z * z;
    t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    r = (z * t1) / (t1 - two) - (w + z * w);
    z = one - (r - z);
    sw.value = z;
    j = (s32)sw.parts.hi;
    j += n << 20;
    if ((j >> 20) <= 0) {
        z = fn_800CDE88(z, n);
    } else {
        sw.parts.hi = (u32)j;
        z = sw.value;
    }
    return s * z;
}
