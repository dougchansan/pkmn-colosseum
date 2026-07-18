/**
 * @file math_kernel_cos.c
 * @brief Exact cosine kernel, 0x800CC660 - 0x800CC754.
 */
#include "crt/math.h"

typedef union DoubleShape {
    f64 value;
    struct {
        u32 hi;
        u32 lo;
    } parts;
} DoubleShape;

extern const f64 lbl_8047C7E0;
extern const f64 lbl_8047C7E8;
extern const f64 lbl_8047C7F0;
extern const f64 lbl_8047C7F8;
extern const f64 lbl_8047C800;
extern const f64 lbl_8047C808;
extern const f64 lbl_8047C810;
extern const f64 lbl_8047C818;
extern const f64 lbl_8047C820;

f64 __kernel_cos(f64 x, f64 y) {
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

/*
 * Keep the definitions after the function. MWCC otherwise folds same-unit
 * constants into a duplicate local pool instead of referencing these symbols.
 */
#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

SDATA2 const f64 lbl_8047C7E0 = 1.00000000000000000000e+00;
SDATA2 const f64 lbl_8047C7E8 = 4.16666666666666019037e-02;
SDATA2 const f64 lbl_8047C7F0 = -1.38888888888741095749e-03;
SDATA2 const f64 lbl_8047C7F8 = 2.48015872894767294178e-05;
SDATA2 const f64 lbl_8047C800 = -2.75573143513906633035e-07;
SDATA2 const f64 lbl_8047C808 = 2.08757232129817482790e-09;
SDATA2 const f64 lbl_8047C810 = -1.13596475577881948265e-11;
SDATA2 const f64 lbl_8047C818 = 5.00000000000000000000e-01;
SDATA2 const f64 lbl_8047C820 = 2.81250000000000000000e-01;
