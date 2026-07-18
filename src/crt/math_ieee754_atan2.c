/**
 * @file math_ieee754_atan2.c
 * @brief Exact IEEE-754 atan2 implementation, 0x800CB024 - 0x800CB2B4.
 */
#include "crt/math.h"

extern f64 atan(f64 x);

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

/*
 * Keep the definitions after the function. MWCC otherwise folds same-unit
 * constants into a duplicate local pool instead of referencing these symbols.
 */
#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

SDATA2 const f64 lbl_8047C528 = 3.14159265358979311600e+00;
SDATA2 const f64 lbl_8047C530 = -3.14159265358979311600e+00;
SDATA2 const f64 lbl_8047C538 = -1.57079632679489655800e+00;
SDATA2 const f64 lbl_8047C540 = 1.57079632679489655800e+00;
SDATA2 const f64 lbl_8047C548 = 7.85398163397448278999e-01;
SDATA2 const f64 lbl_8047C550 = -7.85398163397448278999e-01;
SDATA2 const f64 lbl_8047C558 = 2.35619449019234483700e+00;
SDATA2 const f64 lbl_8047C560 = -2.35619449019234483700e+00;
SDATA2 const f64 lbl_8047C568 = 0.0;
SDATA2 const f64 lbl_8047C570 = -0.0;
SDATA2 const f64 lbl_8047C578 = 1.22464679914735320717e-16;
