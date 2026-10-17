/**
 * @file mtx.h
 * @brief HAL sysdolphin mtx.h: matrix helpers and column inlines.
 *
 * Inline bodies follow the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/mtx.h).
 */
#ifndef SYSDOLPHIN_BASELIB_MTX_H
#define SYSDOLPHIN_BASELIB_MTX_H

#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"

void HSD_MtxGetScale(MtxPtr m, Vec3* scale);
/* HSD_MtxGetRotation */
void HSD_MtxGetRotation(MtxPtr m, Vec3* rotate);

static inline void HSD_MtxColVec(MtxPtr mtx, int col, Vec3* vec)
{
    vec->x = mtx[0][col];
    vec->y = mtx[1][col];
    vec->z = mtx[2][col];
}

static inline void HSD_MtxSetColVec(MtxPtr mtx, int col, Vec3* vec)
{
    mtx[0][col] = vec->x;
    mtx[1][col] = vec->y;
    mtx[2][col] = vec->z;
}

static inline f32 HSD_MtxColMag(MtxPtr mtx, int col)
{
    return sqrtf((mtx[0][col] * mtx[0][col]) + (mtx[1][col] * mtx[1][col]) +
                 (mtx[2][col] * mtx[2][col]));
}

#endif /* SYSDOLPHIN_BASELIB_MTX_H */
