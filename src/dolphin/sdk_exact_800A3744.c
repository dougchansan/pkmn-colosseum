/**
 * @file sdk_exact_800A3744.c
 * @brief Exact light orthographic matrix constructor, 0x800A3744 - 0x800A37CC.
 */

#include "dolphin/types.h"

typedef f32 Mtx[3][4];

extern const f32 lbl_8047C288;
extern const f32 lbl_8047C28C;
extern const f32 lbl_8047C298;

#pragma fp_contract off
void C_MTXLightOrtho(Mtx m, f32 top, f32 bottom, f32 left, f32 right,
                     f32 scaleS, f32 scaleT, f32 transS, f32 transT)
{
    f32 tmp;
    f32 factor;

    tmp = lbl_8047C288 / (right - left);
    factor = lbl_8047C298 * tmp;
    m[0][0] = factor * scaleS;
    m[0][1] = lbl_8047C28C;
    m[0][2] = lbl_8047C28C;
    m[0][3] = -(right + left) * tmp * scaleS + transS;

    tmp = lbl_8047C288 / (top - bottom);
    m[1][0] = lbl_8047C28C;
    factor = lbl_8047C298 * tmp;
    m[1][1] = factor * scaleT;
    m[1][2] = lbl_8047C28C;
    m[1][3] = -(top + bottom) * tmp * scaleT + transT;

    m[2][0] = lbl_8047C28C;
    m[2][1] = lbl_8047C28C;
    m[2][2] = lbl_8047C28C;
    m[2][3] = lbl_8047C288;
}
#pragma fp_contract reset
