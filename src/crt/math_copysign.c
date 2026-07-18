/**
 * @file math_copysign.c
 * @brief Exact sign-copy helper, 0x800CDBB8 - 0x800CDBE0.
 */
#include "crt/math.h"

typedef union DoubleShape {
    f64 value;
    struct {
        u32 hi;
        u32 lo;
    } parts;
} DoubleShape;

f64 copysign(f64 x, f64 y) {
    DoubleShape uy;
    DoubleShape ux;

    ux.value = x;
    uy.value = y;
    ux.parts.hi = (ux.parts.hi & 0x7fffffff) | (uy.parts.hi & 0x80000000);
    return ux.value;
}
