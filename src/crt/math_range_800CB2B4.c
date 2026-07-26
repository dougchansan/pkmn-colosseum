/**
 * @file math_range_800CB2B4.c
 * @brief Shared libm candidate range, 0x800CB2B4 - 0x800CC754.
 */
#include "src/crt/math_range_800CAA58.c"

static const f64 fmodOne = 1.0;
static const f64 fmodZero[] = { 0.0, -0.0 };
static const f64 expOne = 1.0;
static const f64 expHalf[] = { 0.5, -0.5 };
static const f64 expHuge = 1.0e300;
static const f64 expTwoM1000 = 9.33263618503218878990e-302;
static const f64 expOverflow = 7.09782712893383973096e+02;
static const f64 expUnderflow = -7.45133219101941108420e+02;
static const f64 expLn2High[] = {
    6.93147180369123816490e-01,
    -6.93147180369123816490e-01,
};
static const f64 expLn2Low[] = {
    1.90821492927058770002e-10,
    -1.90821492927058770002e-10,
};
static const f64 expInvLn2 = 1.44269504088896338700e+00;
static const f64 expP1 = 1.66666666666666019037e-01;
static const f64 expP2 = -2.77777777770155933842e-03;
static const f64 expP3 = 6.61375632143793436117e-05;
static const f64 expP4 = -1.65339022054652515390e-06;
static const f64 expP5 = 4.13813679705723846039e-08;
static const f64 logLn2High = 6.93147180369123816490e-01;
static const f64 logLn2Low = 1.90821492927058770002e-10;
static const f64 logTwo54 = 1.80143985094819840000e+16;
static const f64 logLg1 = 6.666666666666735130e-01;
static const f64 logLg2 = 3.999999999940941908e-01;
static const f64 logLg3 = 2.857142874366239149e-01;
static const f64 logLg4 = 2.222219843214978396e-01;
static const f64 logLg5 = 1.818357216161805012e-01;
static const f64 logLg6 = 1.531383769920937332e-01;
static const f64 logLg7 = 1.479819860511658591e-01;
static const f64 logZero = 0.0;
static const s32 remTwoOverPi[] = {
    0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
    0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
    0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
    0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
    0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
    0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
    0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
    0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
    0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
    0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
    0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
};
static const s32 remNpio2High[] = {
    0x3FF921FB, 0x400921FB, 0x4012D97C, 0x401921FB, 0x401F6A7A,
    0x4022D97C, 0x4025FDBB, 0x402921FB, 0x402C463A, 0x402F6A7A,
    0x4031475C, 0x4032D97C, 0x40346B9C, 0x4035FDBB, 0x40378FDB,
    0x403921FB, 0x403AB41B, 0x403C463A, 0x403DD85A, 0x403F6A7A,
    0x40407E4C, 0x4041475C, 0x4042106C, 0x4042D97C, 0x4043A28C,
    0x40446B9C, 0x404534AC, 0x4045FDBB, 0x4046C6CB, 0x40478FDB,
    0x404858EB, 0x404921FB,
};
static const f64 remZero = 0.0;
static const f64 remHalf = 0.5;
static const f64 remTwo24 = 1.67772160000000000000e+07;
static const f64 remInvPio2 = 6.36619772367581382433e-01;
static const f64 remPio2_1 = 1.57079632673412561417e+00;
static const f64 remPio2_1t = 6.07710050650619224932e-11;
static const f64 remPio2_2 = 6.07710050630396597660e-11;
static const f64 remPio2_2t = 2.02226624879595063154e-21;
static const f64 remPio2_3 = 2.02226624871116645580e-21;
static const f64 remPio2_3t = 8.47842766036889956997e-32;

f64 __ieee754_exp(f64 x)
{
    DoubleShape shape;
    DoubleShape result;
    f64 y;
    f64 high;
    f64 low;
    f64 correction;
    f64 square;
    s32 k;
    s32 sign;
    u32 hx;

    shape.value = x;
    hx = shape.parts.hi;
    sign = (hx >> 31) & 1;
    hx &= 0x7FFFFFFF;

    if (hx >= 0x40862E42) {
        if (hx >= 0x7FF00000) {
            if (((hx & 0xFFFFF) | shape.parts.lo) != 0) {
                return x + x;
            }
            return sign == 0 ? x : 0.0;
        }
        if (x > expOverflow) {
            return expHuge * expHuge;
        }
        if (x < expUnderflow) {
            return expTwoM1000 * expTwoM1000;
        }
    }

    if (hx > 0x3FD62E42) {
        if (hx < 0x3FF0A2B2) {
            high = x - expLn2High[sign];
            low = expLn2Low[sign];
            k = 1 - sign - sign;
        } else {
            k = (s32)(expInvLn2 * x + expHalf[sign]);
            square = k;
            high = x - square * expLn2High[0];
            low = square * expLn2Low[0];
        }
        x = high - low;
    } else if (hx < 0x3E300000) {
        if (expHuge + x > expOne) {
            return expOne + x;
        }
    } else {
        k = 0;
    }

    square = x * x;
    correction =
        x - square *
                (expP1 +
                 square *
                     (expP2 +
                      square *
                          (expP3 + square * (expP4 + square * expP5))));
    if (k == 0) {
        return expOne - ((x * correction) / (correction - 2.0) - x);
    }
    y = expOne - ((low - (x * correction) / (2.0 - correction)) - high);
    result.value = y;
    if (k >= -1021) {
        result.parts.hi += k << 20;
        return result.value;
    }
    result.parts.hi += (k + 1000) << 20;
    return result.value * expTwoM1000;
}

f64 __ieee754_fmod(f64 x, f64 y)
{
    DoubleShape xShape;
    DoubleShape yShape;
    s32 n;
    s32 hx;
    s32 hy;
    s32 hz;
    s32 ix;
    s32 iy;
    s32 sx;
    s32 i;
    u32 lx;
    u32 ly;
    u32 lz;

    xShape.value = x;
    yShape.value = y;
    hx = xShape.parts.hi;
    lx = xShape.parts.lo;
    hy = yShape.parts.hi;
    ly = yShape.parts.lo;
    sx = hx & 0x80000000;
    hx ^= sx;
    hy &= 0x7FFFFFFF;

    if ((hy | ly) == 0 || hx >= 0x7FF00000 ||
        (hy | ((ly | -ly) >> 31)) > 0x7FF00000)
    {
        return (x * y) / (x * y);
    }
    if (hx <= hy) {
        if (hx < hy || lx < ly) {
            return x;
        }
        if (lx == ly) {
            return fmodZero[(u32)sx >> 31];
        }
    }

    if (hx < 0x00100000) {
        if (hx == 0) {
            for (ix = -1043, i = lx; i > 0; i <<= 1) {
                ix--;
            }
        } else {
            for (ix = -1022, i = hx << 11; i > 0; i <<= 1) {
                ix--;
            }
        }
    } else {
        ix = (hx >> 20) - 1023;
    }

    if (hy < 0x00100000) {
        if (hy == 0) {
            for (iy = -1043, i = ly; i > 0; i <<= 1) {
                iy--;
            }
        } else {
            for (iy = -1022, i = hy << 11; i > 0; i <<= 1) {
                iy--;
            }
        }
    } else {
        iy = (hy >> 20) - 1023;
    }

    if (ix >= -1022) {
        hx = 0x00100000 | (hx & 0x000FFFFF);
    } else {
        n = -1022 - ix;
        if (n <= 31) {
            hx = (hx << n) | (lx >> (32 - n));
            lx <<= n;
        } else {
            hx = lx << (n - 32);
            lx = 0;
        }
    }
    if (iy >= -1022) {
        hy = 0x00100000 | (hy & 0x000FFFFF);
    } else {
        n = -1022 - iy;
        if (n <= 31) {
            hy = (hy << n) | (ly >> (32 - n));
            ly <<= n;
        } else {
            hy = ly << (n - 32);
            ly = 0;
        }
    }

    n = ix - iy;
    while (n--) {
        hz = hx - hy;
        lz = lx - ly;
        if (lx < ly) {
            hz--;
        }
        if (hz < 0) {
            hx = hx + hx + (lx >> 31);
            lx += lx;
        } else {
            if ((hz | lz) == 0) {
                return fmodZero[(u32)sx >> 31];
            }
            hx = hz + hz + (lz >> 31);
            lx = lz + lz;
        }
    }
    hz = hx - hy;
    lz = lx - ly;
    if (lx < ly) {
        hz--;
    }
    if (hz >= 0) {
        hx = hz;
        lx = lz;
    }

    if ((hx | lx) == 0) {
        return fmodZero[(u32)sx >> 31];
    }
    while (hx < 0x00100000) {
        hx = hx + hx + (lx >> 31);
        lx += lx;
        iy--;
    }
    if (iy >= -1022) {
        hx = (hx - 0x00100000) | ((iy + 1023) << 20);
        xShape.parts.hi = hx | sx;
        xShape.parts.lo = lx;
    } else {
        n = -1022 - iy;
        if (n <= 20) {
            lx = (lx >> n) | ((u32)hx << (32 - n));
            hx >>= n;
        } else if (n <= 31) {
            lx = (hx << (32 - n)) | (lx >> n);
            hx = sx;
        } else {
            lx = hx >> (n - 32);
            hx = sx;
        }
        xShape.parts.hi = hx | sx;
        xShape.parts.lo = lx;
        xShape.value *= fmodOne;
    }
    return xShape.value;
}

f64 __ieee754_log(f64 x)
{
    DoubleShape shape;
    f64 hfsq;
    f64 f;
    f64 s;
    f64 z;
    f64 R;
    f64 w;
    f64 t1;
    f64 t2;
    f64 dk;
    s32 k;
    s32 hx;
    s32 i;
    s32 j;
    u32 lx;

    shape.value = x;
    hx = shape.parts.hi;
    lx = shape.parts.lo;
    k = 0;
    if (hx < 0x00100000) {
        if (((hx & 0x7FFFFFFF) | lx) == 0) {
            return -logTwo54 / logZero;
        }
        if (hx < 0) {
            return (x - x) / logZero;
        }
        k -= 54;
        x *= logTwo54;
        shape.value = x;
        hx = shape.parts.hi;
    }
    if (hx >= 0x7FF00000) {
        return x + x;
    }
    k += (hx >> 20) - 1023;
    hx &= 0x000FFFFF;
    i = (hx + 0x95F64) & 0x100000;
    shape.value = x;
    shape.parts.hi = hx | (i ^ 0x3FF00000);
    x = shape.value;
    k += i >> 20;
    f = x - 1.0;
    if ((0x000FFFFF & (2 + hx)) < 3) {
        if (f == logZero) {
            if (k == 0) {
                return logZero;
            }
            dk = k;
            return dk * logLn2High + dk * logLn2Low;
        }
        R = f * f * (0.5 - 0.33333333333333333 * f);
        if (k == 0) {
            return f - R;
        }
        dk = k;
        return dk * logLn2High - ((R - dk * logLn2Low) - f);
    }
    s = f / (2.0 + f);
    dk = k;
    z = s * s;
    i = hx - 0x6147A;
    w = z * z;
    j = 0x6B851 - hx;
    t1 = w * (logLg2 + w * (logLg4 + w * logLg6));
    t2 = z * (logLg1 + w * (logLg3 + w * (logLg5 + w * logLg7)));
    i |= j;
    R = t2 + t1;
    if (i > 0) {
        hfsq = 0.5 * f * f;
        if (k == 0) {
            return f - (hfsq - s * (hfsq + R));
        }
        return dk * logLn2High -
               ((hfsq - (s * (hfsq + R) + dk * logLn2Low)) - f);
    }
    if (k == 0) {
        return f - s * (f - R);
    }
    return dk * logLn2High -
           ((s * (f - R) - dk * logLn2Low) - f);
}

s32 __ieee754_rem_pio2(f64 x, f64* y)
{
    DoubleShape shape;
    DoubleShape zshape;
    f64 z;
    f64 w;
    f64 t;
    f64 r;
    f64 fn;
    f64 tx[3];
    s32 e0;
    s32 i;
    s32 j;
    s32 nx;
    s32 n;
    s32 ix;
    s32 hx;

    shape.value = x;
    hx = shape.parts.hi;
    ix = hx & 0x7FFFFFFF;
    if (ix <= 0x3FE921FB) {
        y[0] = x;
        y[1] = 0.0;
        return 0;
    }
    if (ix < 0x4002D97C) {
        if (hx > 0) {
            z = x - remPio2_1;
            if (ix != 0x3FF921FB) {
                y[0] = z - remPio2_1t;
                y[1] = (z - y[0]) - remPio2_1t;
            } else {
                z -= remPio2_2;
                y[0] = z - remPio2_2t;
                y[1] = (z - y[0]) - remPio2_2t;
            }
            return 1;
        }
        z = x + remPio2_1;
        if (ix != 0x3FF921FB) {
            y[0] = z + remPio2_1t;
            y[1] = (z - y[0]) + remPio2_1t;
        } else {
            z += remPio2_2;
            y[0] = z + remPio2_2t;
            y[1] = (z - y[0]) + remPio2_2t;
        }
        return -1;
    }
    if (ix <= 0x413921FB) {
        shape.parts.hi = ix;
        t = shape.value;
        n = (s32)(t * remInvPio2 + remHalf);
        fn = n;
        r = t - fn * remPio2_1;
        w = fn * remPio2_1t;
        if (n < 32 && ix != remNpio2High[n - 1]) {
            y[0] = r - w;
        } else {
            j = ix >> 20;
            y[0] = r - w;
            zshape.value = y[0];
            i = j - ((zshape.parts.hi >> 20) & 0x7FF);
            if (i > 16) {
                t = r;
                w = fn * remPio2_2;
                r = t - w;
                w = fn * remPio2_2t - ((t - r) - w);
                y[0] = r - w;
                zshape.value = y[0];
                i = j - ((zshape.parts.hi >> 20) & 0x7FF);
                if (i > 49) {
                    t = r;
                    w = fn * remPio2_3;
                    r = t - w;
                    w = fn * remPio2_3t - ((t - r) - w);
                    y[0] = r - w;
                }
            }
        }
        y[1] = (r - y[0]) - w;
        if (hx < 0) {
            y[0] = -y[0];
            y[1] = -y[1];
            return -n;
        }
        return n;
    }
    if (ix >= 0x7FF00000) {
        y[0] = y[1] = x - x;
        return 0;
    }
    zshape.value = x;
    zshape.parts.lo = shape.parts.lo;
    e0 = (ix >> 20) - 1046;
    zshape.parts.hi = ix - (e0 << 20);
    z = zshape.value;
    for (i = 0; i < 2; i++) {
        tx[i] = (s32)z;
        z = (z - tx[i]) * remTwo24;
    }
    tx[2] = z;
    nx = 3;
    while (tx[nx - 1] == remZero) {
        nx--;
    }
    n = __kernel_rem_pio2(tx, y, e0, nx, 2, remTwoOverPi);
    if (hx < 0) {
        y[0] = -y[0];
        y[1] = -y[1];
        return -n;
    }
    return n;
}
