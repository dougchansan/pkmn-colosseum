/**
 * @file math_kernel_rem_pio2.c
 * @brief Payne-Hanek argument reduction, 0x800CC754 - 0x800CD5A8.
 */
#include "crt/math.h"

f64 floor(f64 x);
f64 ldexp(f64 x, s32 n);

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
