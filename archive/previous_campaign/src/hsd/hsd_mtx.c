/**
 * @file hsd_mtx.c
 * @brief HSD Matrix utilities - matrix operations and transformations.
 *
 * Address range: 0x801A85F0 - 0x801AA350
 * Contains matrix math routines used by the HSD render pipeline:
 * inverse matrix, concatenation, normal matrix extraction,
 * billboard matrix setup, and display list utilities.
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"

/* Matrix / vector helper types (ported from the Melee HSD decomp; our headers
 * carry no Mtx/Vec3 typedef, so they are declared locally). */
typedef f32 Mtx[3][4];

typedef struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

/* MSL trig (sinf/cosf) — resolved to the runtime math routines. */
f32 sinf(f32 x);
f32 cosf(f32 x);
f32 sqrtf(f32 x);
f32 asinf(f32 x);
f32 atan2f(f32 y, f32 x);

/* dolphin MTX/VEC library (matched by their fn_* addresses in the target). */
void MTXCopy(Mtx src, Mtx dest);
f32 VECMag(Vec3* v);
void VECNormalize(Vec3* src, Vec3* dst);
f32 VECDotProduct(Vec3* a, Vec3* b);
void VECScale(Vec3* src, Vec3* dst, f32 scale);
void VECSubtract(Vec3* a, Vec3* b, Vec3* dst);
void VECCrossProduct(Vec3* a, Vec3* b, Vec3* dst);

#define EPSILON 0.0000000001f
#define FLOAT_MIN 1.1754943E-38f

static inline f32 fabsf_bitwise(f32 v)
{
    *(u32*) &v &= ~0x80000000;
    return v;
}

#define M_PI 3.14159265358979323846f

static inline f32 calcVal(f32 x, f32 y)
{
    if (fabsf_bitwise(x) <= FLOAT_MIN) {
        if (y >= 0) {
            return M_PI / 2;
        } else {
            return -M_PI / 2;
        }
    } else {
        return atan2f(y, x);
    }
}

/* Determinant of the top-left 3x3 of a 3x4 matrix. */
static inline f32 HSD_CalcDeterminantMatrix3x4(Mtx m)
{
    return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] +
           m[0][2] * m[1][0] * m[2][1] - m[2][0] * m[1][1] * m[0][2] -
           m[1][0] * m[0][1] * m[2][2] - m[0][0] * m[2][1] * m[1][2];
}

/* ========================================================================= */
/*  Matrix operation stubs                                                   */
/* ========================================================================= */

/* Address: 0x801A85F0 | Size: 0xC4 */
#pragma push
#pragma fp_contract on
void HSD_MtxScaledAdd(f32* src, f32 scale, f32* add, f32* dst) {
    dst[0] = scale * src[0] + add[0];
    dst[1] = scale * src[1] + add[1];
    dst[2] = scale * src[2] + add[2];
    dst[3] = scale * src[3] + add[3];
    dst[4] = scale * src[4] + add[4];
    dst[5] = scale * src[5] + add[5];
    dst[6] = scale * src[6] + add[6];
    dst[7] = scale * src[7] + add[7];
    dst[8] = scale * src[8] + add[8];
    dst[9] = scale * src[9] + add[9];
    dst[10] = scale * src[10] + add[10];
    dst[11] = scale * src[11] + add[11];
}
#pragma pop

/* Address: 0x801A86B4 | Size: 0x1D0 */
/* NOT in melee mtx.c. Axis-char dispatch (0x78/0x79/0x7a) + calcVal divisions;
 * calls dolphin MTX fn_800A3334/fn_800A33B4/MTXCopy(fn_800A2D98). Likely an
 * SRT/quaternion variant from a different HSD TU; needs source identification. */
void fn_801A86B4(void) {
}

/* Address: 0x801A8884 | Size: 0x310 | HSD_MtxSRT */
void HSD_MtxSRT(Mtx m, Vec3* vec1, Vec3* vec2, Vec3* vec3, Vec3* vec4)
{
    f32 vec1x_2;
    f32 vec1y_2;
    f32 vec1z_2;
    f32 vec1x_1;
    f32 vec1y_1;
    f32 vec1z_1;
    f32 vec1x;
    f32 vec1y;
    f32 vec1z;

    f32 sinX = sinf(vec2->x);
    f32 cosX = cosf(vec2->x);
    f32 sinY = sinf(vec2->y);
    f32 cosY = cosf(vec2->y);
    f32 sinZ = sinf(vec2->z);
    f32 cosZ = cosf(vec2->z);

    vec1x_2 = vec1x_1 = vec1x = vec1->x;
    vec1y_2 = vec1y_1 = vec1y = vec1->y;
    vec1z_2 = vec1z_1 = vec1z = vec1->z;

    if (vec4 != NULL) {
        f32 temp1 = 1.0 / vec4->x;
        f32 temp2 = 1.0 / vec4->y;
        f32 temp3 = 1.0 / vec4->z;

        vec1y_2 *= vec4->y * temp1;
        vec1z_2 *= vec4->z * temp1;
        vec1x_1 *= vec4->x * temp2;
        vec1z_1 *= vec4->z * temp2;
        vec1x *= vec4->x * temp3;
        vec1y *= vec4->y * temp3;
    }

    m[0][0] = cosZ * (vec1x_2 * cosY);
    m[1][0] = sinZ * (vec1x_1 * cosY);
    m[2][0] = -vec1x * sinY;
    m[0][1] = vec1y_2 * ((cosZ * (sinX * sinY)) - (cosX * sinZ));
    m[1][1] = vec1y_1 * ((sinZ * (sinX * sinY)) + (cosX * cosZ));
    m[2][1] = cosY * (vec1y * sinX);
    m[0][2] = vec1z_2 * ((cosZ * (cosX * sinY)) + (sinX * sinZ));
    m[1][2] = vec1z_1 * ((sinZ * (cosX * sinY)) - (sinX * cosZ));
    m[2][2] = cosY * (vec1z * cosX);
    m[0][3] = vec3->x;
    m[1][3] = vec3->y;
    m[2][3] = vec3->z;
}

/* Address: 0x801A8B94 | Size: 0x188 | HSD_MkRotationMtx */
void HSD_MkRotationMtx(Mtx arg0, Vec3* arg1)
{
    f32 sinX;
    f32 cosX;
    f32 sinY;
    f32 cosY;
    f32 sinZ;
    f32 cosZ;
    f32 temp1;
    f32 temp2;

    sinX = sinf(arg1->x);
    cosX = cosf(arg1->x);
    sinY = sinf(arg1->y);
    cosY = cosf(arg1->y);
    sinZ = sinf(arg1->z);
    cosZ = cosf(arg1->z);

    temp1 = sinX * sinY;
    arg0[0][0] = cosY * cosZ;
    arg0[1][0] = cosY * sinZ;
    arg0[2][0] = -sinY;
    temp2 = cosX * sinY;
    arg0[0][1] = (cosZ * temp1) - (cosX * sinZ);
    arg0[1][1] = (sinZ * temp1) + (cosX * cosZ);
    arg0[2][1] = sinX * cosY;
    arg0[0][2] = (cosZ * temp2) + (sinX * sinZ);
    arg0[1][2] = (sinZ * temp2) - (sinX * cosZ);
    arg0[2][2] = cosX * cosY;
    arg0[0][3] = 0;
    arg0[1][3] = 0;
    arg0[2][3] = 0;
}

/* Address: 0x801A8D1C | Size: 0x854 | HSD_MtxGetScale */
void HSD_MtxGetScale(Mtx arg0, Vec3* arg1)
{
    f64 scale;

    u8 _[8];

    Vec3 vec1;
    Vec3 vec2;
    Vec3 vec3;
    Vec3 vec4;

    vec1.x = arg0[0][0];
    vec1.y = arg0[1][0];
    vec1.z = arg0[2][0];

    arg1->x = VECMag(&vec1);
    VECNormalize(&vec1, &vec1);

    vec2.x = arg0[0][1];
    vec2.y = arg0[1][1];
    vec2.z = arg0[2][1];

    VECScale(&vec1, &vec4, VECDotProduct(&vec1, &vec2));
    VECSubtract(&vec2, &vec4, &vec2);
    arg1->y = VECMag(&vec2);
    VECNormalize(&vec2, &vec2);

    vec3.x = arg0[0][2];
    vec3.y = arg0[1][2];
    vec3.z = arg0[2][2];

    VECScale(&vec2, &vec4, VECDotProduct(&vec2, &vec3));
    VECSubtract(&vec3, &vec4, &vec3);
    VECScale(&vec1, &vec4, VECDotProduct(&vec1, &vec3));
    VECSubtract(&vec3, &vec4, &vec3);
    arg1->z = VECMag(&vec3);
    VECNormalize(&vec3, &vec3);
    VECCrossProduct(&vec2, &vec3, &vec4);

    if (VECDotProduct(&vec1, &vec4) < 0.0) {
        scale = -1.0;
        arg1->x *= scale;
        arg1->y *= scale;
        arg1->z *= scale;
    }
}

/* Address: 0x801A9570 | Size: 0x1C */
/* Extract translation vector from 3x4 matrix (last column) */
void HSD_MtxGetTranslate(f32 mtx[3][4], f32* vec) {
    vec[0] = mtx[0][3];
    vec[1] = mtx[1][3];
    vec[2] = mtx[2][3];
}

/* Address: 0x801A958C | Size: 0x340 */
/* NOT in melee mtx.c. Two-axis-char dispatch (0x78/0x79/0x7a) building an
 * orthonormal basis via VECNormalize(fn_800A3ADC)+VECCrossProduct(fn_800A3B9C)
 * in axis-order permutations. Needs source TU identification before porting. */
void fn_801A958C(void) {
}

/* Address: 0x801A98CC | Size: 0x524 | HSD_MtxGetRotation */
void HSD_MtxGetRotation(Mtx m, Vec3* vec)
{
    f32 length0;
    f32 length1;
    f32 length2;
    f32 testVal_1;
    f32 val_01;

    length0 = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    if (!(length0 < FLOAT_MIN)) {
        length1 =
            sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
        if (!(length1 < FLOAT_MIN)) {
            length2 = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] +
                            m[2][2] * m[2][2]);
            if (!(length2 < FLOAT_MIN)) {
                testVal_1 = -m[2][0];
                testVal_1 /= length0;

                if (testVal_1 >= 1.0f) {
                    val_01 = M_PI / 2;
                } else if (testVal_1 <= -1) {
                    val_01 = -M_PI / 2;
                } else {
                    val_01 = asinf(testVal_1);
                }

                vec->y = val_01;

                if (cosf(vec->y) >= FLOAT_MIN) {
                    f32 testVal_2_pre = m[2][2] / length2;
                    f32 testVal_3_pre = m[2][1] / length1;

                    vec->x = calcVal(testVal_2_pre, testVal_3_pre);
                    vec->z = calcVal(m[0][0], m[1][0]);
                    return;
                }

                vec->x = calcVal(m[1][1], m[0][1]);
                vec->z = 0;
                return;
            }
        }
    }

    vec->x = 0;
    vec->y = 0;
    vec->z = 0;
}

/* Address: 0x801A9DF0 | Size: 0x560 | HSD_MtxInverseConcat */
void HSD_MtxInverseConcat(Mtx inv, Mtx src, Mtx dest)
{
    Mtx m;
    f32 det;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;
    f32 temp5;
    f32 temp6;
    f32 temp7;
    f32 temp8;
    f32 temp9;
    f32 temp10;
    f32 temp11;
    f32 temp12;
    f32 new_var;

    det = HSD_CalcDeterminantMatrix3x4(inv);

    if (fabsf_bitwise(det) < EPSILON) {
        if (src != dest) {
            MTXCopy(src, dest);
        }
    } else {
        det = 1.0f / det;
        temp1 = ((inv[1][1] * inv[2][2]) - (inv[2][1] * inv[1][2])) * det;
        temp2 = (-((inv[0][1] * inv[2][2]) - (inv[2][1] * inv[0][2]))) * det;
        new_var = inv[1][1];
        temp3 = (-((inv[1][0] * inv[2][2]) - (inv[2][0] * inv[1][2]))) * det;
        temp7 = ((inv[0][1] * inv[1][2]) - (new_var * inv[0][2])) * det;
        temp4 = ((inv[0][0] * inv[2][2]) - (inv[2][0] * inv[0][2])) * det;
        temp8 = (-((inv[0][0] * inv[1][2]) - (inv[1][0] * inv[0][2]))) * det;
        temp5 = ((inv[1][0] * inv[2][1]) - (inv[2][0] * new_var)) * det;
        temp6 = (-((inv[0][0] * inv[2][1]) - (inv[2][0] * inv[0][1]))) * det;
        temp9 = ((inv[0][0] * inv[1][1]) - (inv[1][0] * inv[0][1])) * det;
        temp10 = -((temp7 * inv[2][3]) -
                   (((-temp1) * inv[0][3]) - (temp2 * inv[1][3])));
        temp11 = -((temp8 * inv[2][3]) -
                   (((-temp3) * inv[0][3]) - (temp4 * inv[1][3])));
        temp12 = -((temp9 * inv[2][3]) -
                   (((-temp5) * (new_var = inv[0][3])) - (temp6 * inv[1][3])));

        if (inv == dest || src == dest) {
            m[0][0] =
                temp7 * src[2][0] + (temp1 * src[0][0] + temp2 * src[1][0]);
            m[0][1] =
                temp7 * src[2][1] + (temp1 * src[0][1] + temp2 * src[1][1]);
            m[0][2] =
                temp7 * src[2][2] + (temp1 * src[0][2] + temp2 * src[1][2]);
            m[0][3] = temp7 * src[2][3] +
                      (temp1 * src[0][3] + temp2 * src[1][3]) + temp10;
            m[1][0] =
                temp8 * src[2][0] + (temp3 * src[0][0] + temp4 * src[1][0]);
            m[1][1] =
                temp8 * src[2][1] + (temp3 * src[0][1] + temp4 * src[1][1]);
            m[1][2] =
                temp8 * src[2][2] + (temp3 * src[0][2] + temp4 * src[1][2]);
            m[1][3] = temp8 * src[2][3] +
                      (temp3 * src[0][3] + temp4 * src[1][3]) + temp11;
            m[2][0] =
                temp9 * src[2][0] + (temp5 * src[0][0] + temp6 * src[1][0]);
            m[2][1] =
                temp9 * src[2][1] + (temp5 * src[0][1] + temp6 * src[1][1]);
            m[2][2] =
                temp9 * src[2][2] + (temp5 * src[0][2] + temp6 * src[1][2]);
            m[2][3] = temp9 * src[2][3] +
                      (temp5 * src[0][3] + temp6 * src[1][3]) + temp12;

            MTXCopy(m, dest);
        } else {
            dest[0][0] =
                temp7 * src[2][0] + (temp1 * src[0][0] + temp2 * src[1][0]);
            dest[0][1] =
                temp7 * src[2][1] + (temp1 * src[0][1] + temp2 * src[1][1]);
            dest[0][2] =
                temp7 * src[2][2] + (temp1 * src[0][2] + temp2 * src[1][2]);
            dest[0][3] = temp7 * src[2][3] +
                         (temp1 * src[0][3] + temp2 * src[1][3]) + temp10;
            dest[1][0] =
                temp8 * src[2][0] + (temp3 * src[0][0] + temp4 * src[1][0]);
            dest[1][1] =
                temp8 * src[2][1] + (temp3 * src[0][1] + temp4 * src[1][1]);
            dest[1][2] =
                temp8 * src[2][2] + (temp3 * src[0][2] + temp4 * src[1][2]);
            dest[1][3] = temp8 * src[2][3] +
                         (temp3 * src[0][3] + temp4 * src[1][3]) + temp11;
            dest[2][0] =
                temp9 * src[2][0] + (temp5 * src[0][0] + temp6 * src[1][0]);
            dest[2][1] =
                temp9 * src[2][1] + (temp5 * src[0][1] + temp6 * src[1][1]);
            dest[2][2] =
                temp9 * src[2][2] + (temp5 * src[0][2] + temp6 * src[1][2]);
            dest[2][3] = temp9 * src[2][3] +
                         (temp5 * src[0][3] + temp6 * src[1][3]) + temp12;
        }
    }
}

/* Address: 0x801AA350 | Size: 0xC */
/* Clear vtx desc list head pointer */
extern u32 lbl_8047B2E0;
void fn_801AA350(void) {
    lbl_8047B2E0 = 0;
}
