/**
 * @file math_range_800CAA58.c
 * @brief MSL stream orientation/console output and fdlibm acos/asin.
 *
 * The two transcendental routines follow Sun fdlibm's e_acos.c and
 * e_asin.c.  Their coefficients live in the retail MSL constant pool.
 */

#include "dolphin/types.h"

typedef struct __FILE {
    u8 pad0[4];
    union {
        u16 all;
        struct {
            u8 high;
            union {
                u8 raw;
                struct {
                    u8 top : 2;
                    u8 orient : 2;
                    u8 bottom : 4;
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

extern u32 OSGetConsoleType(void);
extern s32 InitializeUART(u32 baud);
extern s32 WriteUARTN(const void* data, u32 count);
extern u32 TRKCommSendRead(u32 handle, const void* data, u32* count, void* ref);
extern f64 sqrt(f64 x);

extern s32 lbl_8047AA18;
extern const f32 lbl_80478AC0[];

/* e_acos.c constant pool. */
extern const f64 lbl_8047C418;
extern const f64 lbl_8047C420;
extern const f64 lbl_8047C428;
extern const f64 lbl_8047C430;
extern const f64 lbl_8047C438;
extern const f64 lbl_8047C440;
extern const f64 lbl_8047C448;
extern const f64 lbl_8047C450;
extern const f64 lbl_8047C458;
extern const f64 lbl_8047C460;
extern const f64 lbl_8047C468;
extern const f64 lbl_8047C470;
extern const f64 lbl_8047C478;
extern const f64 lbl_8047C480;
extern const f64 lbl_8047C488;
extern const f64 lbl_8047C490;
extern const f64 lbl_8047C498;

/* e_asin.c constants, in the original fdlibm declaration order. */
static const f64 one = 1.00000000000000000000e+00;
static const f64 huge = 1.000e+300;
static const f64 pio2_hi = 1.57079632679489655800e+00;
static const f64 pio2_lo = 6.12323399573676603587e-17;
static const f64 pio4_hi = 7.85398163397448278999e-01;
static const f64 pS0 = 1.66666666666666657415e-01;
static const f64 pS1 = -3.25565818622400915405e-01;
static const f64 pS2 = 2.01212532134862925881e-01;
static const f64 pS3 = -4.00555345006794114027e-02;
static const f64 pS4 = 7.91534994289814532176e-04;
static const f64 pS5 = 3.47933107596021167570e-05;
static const f64 qS1 = -2.40339491173441421878e+00;
static const f64 qS2 = 2.02094576023350569471e+00;
static const f64 qS3 = -6.88283971605453293030e-01;
static const f64 qS4 = 7.70381505559019352791e-02;

s32 fwide(__FILE* file, s32 mode)
{
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

s32 __write_console(u32 handle, const void* data, u32* count, void* ref)
{
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

        if (WriteUARTN(data, *count) != 0) {
            *count = 0;
            return 1;
        }
    }

    TRKCommSendRead(handle, data, count, ref);
    return 0;
}

f64 __ieee754_acos(f64 x)
{
    f64 df;
    DoubleShape bits;
    f64 z;
    f64 p;
    f64 q;
    f64 r;
    f64 w;
    f64 s;
    f64 c;
    s32 hx;
    s32 ix;

    bits.value = x;
    hx = bits.parts.hi;
    ix = hx & 0x7FFFFFFF;
    if (ix >= 0x3FF00000) {
        if (((ix - 0x3FF00000) | bits.parts.lo) == 0) {
            if (hx > 0) {
                return lbl_8047C418;
            }
            return lbl_8047C420;
        }
        return lbl_80478AC0[0];
    }
    if (ix < 0x3FE00000) {
        if (ix <= 0x3C600000) {
            return lbl_8047C428;
        }
        z = x * x;
        p = z * (lbl_8047C438 + z * (lbl_8047C440 + z *
            (lbl_8047C448 + z * (lbl_8047C450 + z *
            (lbl_8047C458 + lbl_8047C460 * z)))));
        q = lbl_8047C468 + z * (lbl_8047C470 + z *
            (lbl_8047C478 + z * (lbl_8047C480 + lbl_8047C488 * z)));
        r = p / q;
        return lbl_8047C428 - (x - (lbl_8047C430 - x * r));
    }
    if (hx < 0) {
        z = (lbl_8047C468 + x) * lbl_8047C490;
        s = sqrt(z);
        p = z * (lbl_8047C438 + z * (lbl_8047C440 + z *
            (lbl_8047C448 + z * (lbl_8047C450 + z *
            (lbl_8047C458 + lbl_8047C460 * z)))));
        q = lbl_8047C468 + z * (lbl_8047C470 + z *
            (lbl_8047C478 + z * (lbl_8047C480 + lbl_8047C488 * z)));
        r = p / q;
        w = r * s - lbl_8047C430;
        return lbl_8047C420 - lbl_8047C498 * (s + w);
    }

    z = (lbl_8047C468 - x) * lbl_8047C490;
    s = sqrt(z);
    df = s;
    ((u32*)&df)[1] = 0;
    c = (z - df * df) / (s + df);
    p = z * (lbl_8047C438 + z * (lbl_8047C440 + z *
        (lbl_8047C448 + z * (lbl_8047C450 + z *
        (lbl_8047C458 + lbl_8047C460 * z)))));
    q = lbl_8047C468 + z * (lbl_8047C470 + z *
        (lbl_8047C478 + z * (lbl_8047C480 + lbl_8047C488 * z)));
    r = p / q;
    w = r * s + c;
    return lbl_8047C498 * (df + w);
}

f64 __ieee754_asin(f64 x)
{
    f64 t;
    f64 w;
    f64 p;
    f64 q;
    f64 c;
    f64 r;
    f64 s;
    s32 hx;
    s32 ix;

    hx = ((s32*)&x)[0];
    ix = hx & 0x7FFFFFFF;
    if (ix >= 0x3FF00000) {
        if (((ix - 0x3FF00000) | ((u32*)&x)[1]) == 0) {
            return x * pio2_hi + x * pio2_lo;
        }
        return lbl_80478AC0[0];
    }
    if (ix < 0x3FE00000) {
        if (ix < 0x3E400000) {
            if (huge + x > one) {
                return x;
            }
        } else {
            t = x * x;
        }
        p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
        q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
        w = p / q;
        return x + x * w;
    }

    w = one - __fabs(x);
    t = w * 0.5;
    p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
    q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
    s = sqrt(t);
    if (ix >= 0x3FEF3333) {
        w = p / q;
        t = pio2_hi - (2.0 * (s + s * w) - pio2_lo);
    } else {
        w = s;
        *(1 + (s32*)&w) = 0;
        c = (t - w * w) / (s + w);
        r = p / q;
        p = 2.0 * s * r - (pio2_lo - 2.0 * c);
        q = pio4_hi - 2.0 * w;
        t = pio4_hi - (p - q);
    }
    if (hx > 0) {
        return t;
    }
    return -t;
}
