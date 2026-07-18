/**
 * @file math_kernel_sin.c
 * @brief Exact sine kernel, 0x800CD5A8 - 0x800CD648.
 */
#include "crt/math.h"

typedef union DoubleShape {
    f64 value;
    struct {
        u32 hi;
        u32 lo;
    } parts;
} DoubleShape;

extern const f64 lbl_8047C868;
extern const f64 lbl_8047C870;
extern const f64 lbl_8047C878;
extern const f64 lbl_8047C880;
extern const f64 lbl_8047C888;
extern const f64 lbl_8047C890;
extern const f64 lbl_8047C898;

f64 __kernel_sin(f64 x, f64 y, s32 iy) {
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

/*
 * Keep the definitions after the function. MWCC otherwise folds same-unit
 * constants into a duplicate local pool instead of referencing these symbols.
 */
#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

SDATA2 const f64 lbl_8047C868 = 8.33333333332248946124e-03;
SDATA2 const f64 lbl_8047C870 = -1.98412698298579493134e-04;
SDATA2 const f64 lbl_8047C878 = 2.75573137070700676789e-06;
SDATA2 const f64 lbl_8047C880 = -2.50507602534068634195e-08;
SDATA2 const f64 lbl_8047C888 = 1.58969099521155010221e-10;
SDATA2 const f64 lbl_8047C890 = -1.66666666666666324348e-01;
SDATA2 const f64 lbl_8047C898 = 5.00000000000000000000e-01;
