/**
 * @file quatlib.c
 * @brief HAL sysdolphin quatlib.c: quaternion helpers, 0x801AD7CC-0x801ADC08.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/quatlib.c) and checked against Colosseum's newer
 * library, where the helpers return 1 and the slerp's second half no
 * longer keeps 2t in a temporary.
 *
 * Only the slerp (fn_801AD7CC, Melee HSD_QuatLib_8037EF28) and EulerToQuat
 * (EulerToQuat) are referenced by the game; the linker strips the rest.
 * Under deferred inlining MWCC emits functions in reverse source order, so
 * EulerToQuat precedes the slerp here and the unreferenced helpers follow
 * it: they are generated first, and the constants they create first
 * (1.0f, then 0.5f) are why retail's .sdata2 pool reads 1.0f, 0.5f,
 * 1e-10f, 1.0, pi/2, 2.0f although the slerp itself first uses 1e-10f.
 */
#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

/* EulerToQuat */
s32 EulerToQuat(Vec3* euler, Quaternion* q)
{
    f32 cx;
    f32 cy;
    f32 cz;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 cc;
    f32 ss;

    cx = cosf(0.5F * euler->x);
    cy = cosf(0.5F * euler->y);
    cz = cosf(0.5F * euler->z);
    sx = sinf(0.5F * euler->x);
    sy = sinf(0.5F * euler->y);
    sz = sinf(0.5F * euler->z);

    ss = sy * sz;
    cc = cy * cz;
    q->w = cx * cc + sx * ss;
    q->x = sx * cc - cx * ss;
    q->y = cz * (cx * sy) + sz * (sx * cy);
    q->z = sz * (cx * cy) - cz * (sx * sy);

    return 1;
}

/* Quaternion slerp (Melee HSD_QuatLib_8037EF28) */
s32 fn_801AD7CC(Quaternion* p, Quaternion* q, Quaternion* out, f32 t)
{
    f32 cosom;
    f32 theta;
    f32 sinom;
    f32 sp;
    f32 sq;

    cosom = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;

    if ((1.0F + cosom) > 1e-10F) {
        if ((1.0F - cosom) > 1e-10F) {
            theta = acosf(cosom);
            sinom = sinf(theta);
            sp = sinf((1.0F - t) * theta) / sinom;
            sq = sinf(t * theta) / sinom;
        } else {
            sq = t;
            sp = (f32) (1.0 - (f64) t);
        }
        out->x = sp * p->x + sq * q->x;
        out->y = sp * p->y + sq * q->y;
        out->z = sp * p->z + sq * q->z;
        out->w = sp * p->w + sq * q->w;
    } else {
        out->x = -p->y;
        out->y = p->x;
        out->z = -p->w;
        out->w = p->z;

        if (t < 0.5F) {
            sp = sinf((f32) (1.5707963267948966 * (1.0F - (2.0F * t))));
            sq = sinf((f32) (1.5707963267948966 * (2.0F * t)));
            out->x = sp * p->x + sq * q->x;
            out->y = sp * p->y + sq * q->y;
            out->z = sp * p->z + sq * q->z;
            out->w = sp * p->w + sq * q->w;
        } else {
            t -= 0.5F;
            sp = sinf((f32) (1.5707963267948966 * (1.0F - (2.0F * t))));
            sq = sinf((f32) (1.5707963267948966 * (2.0F * t)));
            out->x = sp * p->x + sq * q->x;
            out->y = sp * p->y + sq * q->y;
            out->z = sp * p->z + sq * q->z;
            out->w = sp * p->w + sq * q->w;
        }
    }

    return 1;
}

s32 MatToQuat(Mtx m, Quaternion* q)
{
    f32 q3[3];
    int nxt[] = { 1, 2, 0 };
    f32 lenCol[3];
    f32 s;
    f32 scale;
    int i;
    int j;
    int k;

    lenCol[0] =
        sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    lenCol[1] =
        sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
    lenCol[2] =
        sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);

    s = m[0][0] / lenCol[0] + m[1][1] / lenCol[1] + m[2][2] / lenCol[2];

    if (s > 0.0F) {
        s = sqrtf(1.0F + s);
        q->w = 0.5F * s;
        scale = 0.5F / s;
        q->x = scale * ((m[2][1] / lenCol[1]) - (m[1][2] / lenCol[2]));
        q->y = scale * ((m[0][2] / lenCol[2]) - (m[2][0] / lenCol[0]));
        q->z = scale * ((m[1][0] / lenCol[0]) - (m[0][1] / lenCol[1]));
    } else {
        i = 0;
        if (m[1][1] / lenCol[1] > m[0][0] / lenCol[0]) {
            i = 1;
        }
        if (m[2][2] / lenCol[2] > m[i][i] / lenCol[i]) {
            i = 2;
        }
        j = nxt[i];
        k = nxt[j];

        s = sqrtf(1.0F + (((m[i][i] / lenCol[i]) - (m[j][j] / lenCol[j])) -
                          (m[k][k] / lenCol[k])));
        scale = 0.5F / s;
        q3[i] = 0.5F * s;
        q->w = scale * ((m[k][j] / lenCol[j]) - (m[j][k] / lenCol[k]));
        q3[j] = scale * ((m[j][i] / lenCol[i]) + (m[i][j] / lenCol[j]));
        q3[k] = scale * ((m[k][i] / lenCol[i]) + (m[i][k] / lenCol[k]));
        q->x = q3[0];
        q->y = q3[1];
        q->z = q3[2];
    }

    return 0;
}

s32 HSD_QuatLib_8037EB28(Mtx m, Vec3* euler)
{
    f32 len;

    len = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0]);
    if (len > 1e-05) {
        euler->x = atan2f(m[2][1], m[2][2]);
        euler->y = atan2f(-m[2][0], len);
        euler->z = atan2f(m[1][0], m[0][0]);
    } else {
        euler->x = atan2f(-m[1][2], m[1][1]);
        euler->y = atan2f(-m[2][0], len);
        euler->z = 0.0F;
    }

    return 0;
}

s32 HSD_QuatLib_8037EC4C(Quaternion* p, Quaternion* q, Quaternion* out)
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;

    x = q->w * p->x + p->w * q->x + (p->y * q->z - q->y * p->z);
    y = q->w * p->y + p->w * q->y + (q->x * p->z - p->x * q->z);
    z = q->w * p->z + p->w * q->z + (p->x * q->y - q->x * p->y);
    w = p->w * q->w - (p->z * q->z + (p->x * q->x + p->y * q->y));

    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;

    return 0;
}

s32 HSD_QuatLib_8037ECE0(Vec3* axis, Quaternion* q, f32 angle)
{
    f32 len;
    f32 half_angle;
    f32 inv_len;
    f32 s;

    len = sqrtf(axis->x * axis->x + axis->y * axis->y + axis->z * axis->z);
    if (fabsf(len) < 1.1754944E-38F) {
        return -1;
    }
    inv_len = 1.0F / len;
    half_angle = 0.5F * angle;
    q->w = cosf(half_angle);
    s = sinf(half_angle);
    q->x = s * (inv_len * axis->x);
    q->y = s * (inv_len * axis->y);
    q->z = s * (inv_len * axis->z);

    return 0;
}
