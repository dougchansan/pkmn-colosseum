#include "dolphin/types.h"
#include "dolphin/mtx.h"

/* =========================================================================
 * Partial banked source for reserved split unit 0x801A86B4 - 0x801AA608.
 * Only the functions that byte-match under GC/1.3 are provided; the rest of
 * the range (including the five float-heavy matrix routines HSD_MtxSRT /
 * 8B94 / 8D1C / 98CC / 9DF0, which are 16-62% source-refinement fleet
 * targets, and the empty-stub dispatchers) stay as extracted asm.
 * ========================================================================= */

/* Object / class helpers (DTK names). */
extern void  fn_801A6960(void* ptr);   /* HSD_MemFree  */
extern void* fn_801A6928(s32 size);    /* HSD_MemAlloc */
extern s32   HSD_GetNbBits(u32 value);
extern void* memset(void* dst, int value, u32 size);
extern void  __assert(const char* file, u32 line, const char* expression);
extern void  hsdInitClassInfo(void* info, void* parent, const char* library,
                              const char* name, u32 size, u32 flags);

/* Data / global symbols (DTK names). */
extern void* lbl_8047B2E0;           /* free-list pool chain head (sbss) */
extern u8    lbl_8036CBF0[];         /* data heap descriptor             */
extern u8    hsdObj[];               /* data class info                  */
extern u8    lbl_8036C638[];         /* data parent class info           */
extern u32   lbl_8036CC40[];         /* performance counters             */
extern const char lbl_80274EC8[];    /* rodata string                    */
extern const char lbl_80274E90[];    /* objalloc.c strings               */
extern const char lbl_8047DC98;      /* non-null allocation data         */
extern const char lbl_8047DCA0;      /* sdata2 string                    */
extern const char lbl_8047DCA8;      /* sdata2 string                    */
extern const char lbl_8047DCB0;      /* sdata2 string                    */

/* ------------------------------------------------------------------------ */
/*  mtx.c - sysdolphin matrix builders                                       */
/*                                                                           */
/*  The retail range keeps the sysdolphin mtx.c bodies; the one Colosseum    */
/*  deviation from the shipped Melee revision is the reciprocal used by      */
/*  HSD_MtxSRT / HSD_MtxSRTQuat, which biases the divisor away from zero     */
/*  by a small epsilon instead of dividing directly.                         */
/* ------------------------------------------------------------------------ */

extern const f32 lbl_80478ACC; /* reciprocal guard epsilon (rodata) */
extern const f32 lbl_80478AC0[];

extern f64 sin(f64 x);
extern f64 cos(f64 x);

/* MSL's math.h defines these float entry points as double-call inlines. */
static inline f32 sinf(f32 x)
{
    f64 r = sin(x);
    return (f32) r;
}

static inline f32 cosf(f32 x)
{
    f64 r = cos(x);
    return (f32) r;
}

/* Address: 0x801A8884 | Size: 0x310 */
void HSD_MtxSRT(f32 m[3][4], Vec* vec1, Vec* vec2, Vec* vec3, Vec* vec4)
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
        f32 temp1 =
            1.0f / (vec4->x >= 0.0f ? vec4->x + lbl_80478ACC
                                    : vec4->x - lbl_80478ACC);
        f32 temp2 =
            1.0f / (vec4->y >= 0.0f ? vec4->y + lbl_80478ACC
                                    : vec4->y - lbl_80478ACC);
        f32 temp3 =
            1.0f / (vec4->z >= 0.0f ? vec4->z + lbl_80478ACC
                                    : vec4->z - lbl_80478ACC);

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

typedef struct HSD_Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} HSD_Quaternion;

extern void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
extern void PSMTXConcat(const f32 a[3][4], const f32 b[3][4],
                        f32 result[3][4]);
extern void PSMTXQuat(f32 m[3][4], const HSD_Quaternion* quat);
extern void PSMTXTrans(f32 m[3][4], f32 x, f32 y, f32 z);
extern f32 PSVECSquareMag(const Vec* vec);
extern f32 PSVECMag(const Vec* vec);
extern f32 PSVECDotProduct(const Vec* a, const Vec* b);
extern void PSVECScale(f32 scale, const Vec* src, Vec* dst);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* dst);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* dst);
extern void PSVECNormalize(const Vec* src, Vec* dst);
extern void PSMTXCopy(const f32 src[3][4], f32 dst[3][4]);
extern f64 asin(f64 x);
extern f64 atan2(f64 y, f64 x);
extern const f32 lbl_8047DC58;
extern const f32 lbl_8047DC5C;
extern const f64 lbl_8047DC60;
extern const f64 lbl_8047DC68;
extern const f32 lbl_8047DC70;
extern const f64 lbl_8047DC78;
extern const f64 lbl_8047DC80;

#define HSD_MTX_FLOAT_ONE  lbl_8047DC58
#define HSD_MTX_FLOAT_ZERO lbl_8047DC5C
#define HSD_MTX_DOUBLE_ZERO lbl_8047DC60
#define HSD_MTX_DOUBLE_NEG_ONE lbl_8047DC68
#define HSD_MTX_FLOAT_EPS  lbl_8047DC70
#define HSD_MTX_DOUBLE_HALF lbl_8047DC78
#define HSD_MTX_DOUBLE_THREE lbl_8047DC80

void HSD_MtxSRTQuat(f32 m[3][4], Vec* scale, HSD_Quaternion* rotate,
                    Vec* translate, Vec* parent_scale)
{
    f32 temp[3][4];

    PSMTXScale(m, scale->x, scale->y, scale->z);
    if (parent_scale != NULL) {
        PSMTXScale(temp, parent_scale->x, parent_scale->y, parent_scale->z);
        PSMTXConcat(temp, m, m);
    }
    PSMTXQuat(temp, rotate);
    PSMTXConcat(temp, m, m);
    if (parent_scale != NULL) {
        PSMTXScale(temp, 1.0F / parent_scale->x,
                   1.0F / parent_scale->y,
                   1.0F / parent_scale->z);
        PSMTXConcat(temp, m, m);
    }
    PSMTXTrans(temp, translate->x, translate->y, translate->z);
    PSMTXConcat(temp, m, m);
}

/* Address: 0x801A8B94 | Size: 0x188 */
void HSD_MkRotationMtx(f32 arg0[3][4], Vec* arg1)
{
    f32 sinX = sinf(arg1->x);
    f32 cosX = cosf(arg1->x);
    f32 sinY = sinf(arg1->y);
    f32 cosY = cosf(arg1->y);
    f32 sinZ = sinf(arg1->z);
    f32 cosZ = cosf(arg1->z);

    arg0[0][0] = cosY * cosZ;
    arg0[1][0] = cosY * sinZ;
    arg0[2][0] = -sinY;
    arg0[0][1] = (cosZ * (sinX * sinY)) - (cosX * sinZ);
    arg0[1][1] = (sinZ * (sinX * sinY)) + (cosX * cosZ);
    arg0[2][1] = sinX * cosY;
    arg0[0][2] = (cosZ * (cosX * sinY)) + (sinX * sinZ);
    arg0[1][2] = (sinZ * (cosX * sinY)) - (sinX * cosZ);
    arg0[2][2] = cosX * cosY;
    arg0[0][3] = 0.0f;
    arg0[1][3] = 0.0f;
    arg0[2][3] = 0.0f;
}

static inline s32 mtxScaleFpClassifyf(f32 value)
{
    switch (*(s32*) &value & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*) &value & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (*(s32*) &value & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 mtxScaleSqrtf(f32 value)
{
    f64 guess;

    if (value > HSD_MTX_FLOAT_ZERO) {
        guess = __frsqrte(value);
        guess = HSD_MTX_DOUBLE_HALF * guess *
                (HSD_MTX_DOUBLE_THREE - value * (guess * guess));
        guess = HSD_MTX_DOUBLE_HALF * guess *
                (HSD_MTX_DOUBLE_THREE - value * (guess * guess));
        guess = HSD_MTX_DOUBLE_HALF * guess *
                (HSD_MTX_DOUBLE_THREE - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < HSD_MTX_DOUBLE_ZERO) {
        return lbl_80478AC0[0];
    }
    if (mtxScaleFpClassifyf(value) == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

void HSD_MtxGetScale(f32 m[3][4], Vec* scale)
{
    Vec x;
    Vec y;
    Vec z;
    Vec projection;
    f32 x_sq;
    f32 y_sq;
    f32 z_sq;

    x.x = m[0][0];
    x.y = m[1][0];
    x.z = m[2][0];

    x_sq = PSVECSquareMag(&x);
    if (x_sq > HSD_MTX_FLOAT_EPS) {
        f32 x_inv_mag = mtxScaleSqrtf(HSD_MTX_FLOAT_ONE / x_sq);
        scale->x = HSD_MTX_FLOAT_ONE / x_inv_mag;
        PSVECScale(x_inv_mag, &x, &x);

        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];
        PSVECScale(PSVECDotProduct(&x, &y), &x, &projection);
        PSVECSubtract(&y, &projection, &y);

        y_sq = PSVECSquareMag(&y);
        if (y_sq > HSD_MTX_FLOAT_EPS) {
            f32 y_inv_mag = mtxScaleSqrtf(HSD_MTX_FLOAT_ONE / y_sq);
            scale->y = HSD_MTX_FLOAT_ONE / y_inv_mag;
            PSVECScale(y_inv_mag, &y, &y);

            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(PSVECDotProduct(&y, &z), &y, &projection);
            PSVECSubtract(&z, &projection, &z);
            PSVECScale(PSVECDotProduct(&x, &z), &x, &projection);
            PSVECSubtract(&z, &projection, &z);

            z_sq = PSVECSquareMag(&z);
            if (z_sq > HSD_MTX_FLOAT_EPS) {
                f64 negative;

                scale->z = mtxScaleSqrtf(z_sq);
                PSVECCrossProduct(&y, &z, &projection);
                if (PSVECDotProduct(&x, &projection) <
                    HSD_MTX_DOUBLE_ZERO) {
                    scale->x =
                        (f32) (scale->x *
                               (negative = HSD_MTX_DOUBLE_NEG_ONE));
                    scale->y = (f32) (scale->y * negative);
                    scale->z = (f32) (scale->z * negative);
                }
            } else {
                scale->z = HSD_MTX_FLOAT_ZERO;
            }
        } else {
            scale->y = HSD_MTX_FLOAT_ZERO;
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(PSVECDotProduct(&x, &z), &x, &projection);
            PSVECSubtract(&z, &projection, &z);

            z_sq = PSVECSquareMag(&z);
            if (z_sq > HSD_MTX_FLOAT_EPS) {
                scale->z = mtxScaleSqrtf(z_sq);
            } else {
                scale->z = HSD_MTX_FLOAT_ZERO;
            }
        }
    } else {
        scale->x = HSD_MTX_FLOAT_ZERO;
        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];

        y_sq = PSVECSquareMag(&y);
        if (y_sq > HSD_MTX_FLOAT_EPS) {
            f32 y_inv_mag = mtxScaleSqrtf(HSD_MTX_FLOAT_ONE / y_sq);
            scale->y = HSD_MTX_FLOAT_ONE / y_inv_mag;
            PSVECScale(y_inv_mag, &y, &y);

            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(PSVECDotProduct(&y, &z), &y, &projection);
            PSVECSubtract(&z, &projection, &z);
            scale->z = PSVECMag(&z);
        } else {
            scale->y = HSD_MTX_FLOAT_ZERO;
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            scale->z = PSVECMag(&z);
        }
    }
}

/* Address: 0x801A9570 | Size: 0x1C  -- already-banked (GC/1.3, calibration) */
void HSD_MtxGetTranslate(f32 mtx[3][4], f32* vec) {
    vec[0] = mtx[0][3];
    vec[1] = mtx[1][3];
    vec[2] = mtx[2][3];
}

/* Address: 0x801A958C | Size: 0x340 -- HSD_MtxGetRotationMtx */
void fn_801A958C(f32 src[3][4], f32 dst[3][4], char axis0, char axis1)
{
    Vec x;
    Vec y;
    Vec z;
    f32 zero;

    switch (axis0) {
    case 'x':
    case 'X':
        x.x = src[0][0];
        x.y = src[1][0];
        x.z = src[2][0];
        PSVECNormalize(&x, &x);
        switch (axis1) {
        case 'z':
        case 'Z':
            z.x = src[0][2];
            z.y = src[1][2];
            z.z = src[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&x, &y, &z);
            break;
        default:
            y.x = src[0][1];
            y.y = src[1][1];
            y.z = src[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&z, &x, &y);
            break;
        }
        break;
    case 'y':
    case 'Y':
        y.x = src[0][1];
        y.y = src[1][1];
        y.z = src[2][1];
        PSVECNormalize(&y, &y);
        switch (axis1) {
        case 'x':
        case 'X':
            x.x = src[0][0];
            x.y = src[1][0];
            x.z = src[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&y, &z, &x);
            break;
        default:
            z.x = src[0][2];
            z.y = src[1][2];
            z.z = src[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&x, &y, &z);
            break;
        }
        break;
    default:
        z.x = src[0][2];
        z.y = src[1][2];
        z.z = src[2][2];
        PSVECNormalize(&z, &z);
        switch (axis1) {
        case 'y':
        case 'Y':
            y.x = src[0][1];
            y.y = src[1][1];
            y.z = src[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&z, &x, &y);
            break;
        default:
            x.x = src[0][0];
            x.y = src[1][0];
            x.z = src[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&y, &z, &x);
            break;
        }
        break;
    }

    zero = 0.0f;
    dst[0][0] = x.x;
    dst[1][0] = x.y;
    dst[2][0] = x.z;
    dst[0][1] = y.x;
    dst[1][1] = y.y;
    dst[2][1] = y.z;
    dst[0][2] = z.x;
    dst[1][2] = z.y;
    dst[2][2] = z.z;
    dst[0][3] = zero;
    dst[1][3] = zero;
    dst[2][3] = zero;
}

static inline f32 mtxCalcVal(f32 x, f32 y)
{
    if (x == 0.0f) {
        return y >= 0.0f ? 1.5707963267948966f : -1.5707963267948966f;
    }
    return (f32) atan2(y, x);
}

typedef union MtxFloatShape {
    f32 value;
    u32 bits;
} MtxFloatShape;

/* MSL math.h's inline sqrtf, retained here because this TU inlined it. */
static inline f32 mtxSqrtf(f32 value)
{
    MtxFloatShape shape;
    f64 guess;
    s32 exponent;
    s32 fpclass;

    if (value > 0.0f) {
        guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < 0.0) {
        return lbl_80478AC0[0];
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 1 : 2;
        break;
    case 0:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 5 : 3;
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

/* Address: 0x801A98CC | Size: 0x524 -- HSD_MtxGetRotation */
void fn_801A98CC(f32 m[3][4], Vec* vec)
{
    f32 length0;
    f32 length1;
    f32 length2;
    f32 test;
    f32 angle;

    length0 = mtxSqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] +
                       m[2][0] * m[2][0]);
    if (!(length0 < 1.1754943E-38f)) {
        length1 = mtxSqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] +
                           m[2][1] * m[2][1]);
        if (!(length1 < 1.1754943E-38f)) {
            length2 = mtxSqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] +
                               m[2][2] * m[2][2]);
            if (!(length2 < 1.1754943E-38f)) {
                test = -m[2][0] / length0;
                if (test >= 1.0f) {
                    angle = 1.5707963267948966f;
                } else if (test <= -1.0f) {
                    angle = -1.5707963267948966f;
                } else {
                    angle = (f32) asin(test);
                }
                vec->y = angle;

                if ((f32) cos(vec->y) >= 1.1754943E-38f) {
                    f32 y = m[2][1] / length1;
                    f32 x = m[2][2] / length2;

                    vec->x = mtxCalcVal(x, y);
                    vec->z = mtxCalcVal(m[0][0], m[1][0]);
                    return;
                }

                vec->x = mtxCalcVal(m[1][1], m[0][1]);
                vec->z = 0.0f;
                return;
            }
        }
    }

    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
}

static inline f32 mtxDeterminant(f32 m[3][4])
{
    f32 det;

    det = m[2][0] * (m[0][1] * m[1][2]);
    det += m[2][2] * (m[0][0] * m[1][1]);
    det += m[2][1] * (m[0][2] * m[1][0]);
    det -= m[0][2] * (m[2][0] * m[1][1]);
    det -= m[2][2] * (m[1][0] * m[0][1]);
    det -= m[1][2] * (m[0][0] * m[2][1]);
    return det;
}

/* Address: 0x801A9DF0 | Size: 0x560 -- HSD_MtxInverseConcat */
BOOL fn_801A9DF0(f32 inv[3][4], f32 src[3][4], f32 dst[3][4])
{
    f32 result[3][4];
    f32 det;
    f32 i00;
    f32 i01;
    f32 i02;
    f32 i10;
    f32 i11;
    f32 i12;
    f32 i20;
    f32 i21;
    f32 i22;
    f32 tx;
    f32 ty;
    f32 tz;

    det = mtxDeterminant(inv);
    if (det == 0.0f) {
        return FALSE;
    }

    det = 1.0f / det;
    i00 = (inv[1][1] * inv[2][2] - inv[2][1] * inv[1][2]) * det;
    i01 = -(inv[0][1] * inv[2][2] - inv[2][1] * inv[0][2]) * det;
    i10 = -(inv[1][0] * inv[2][2] - inv[2][0] * inv[1][2]) * det;
    i02 = (inv[0][1] * inv[1][2] - inv[1][1] * inv[0][2]) * det;
    i11 = (inv[0][0] * inv[2][2] - inv[2][0] * inv[0][2]) * det;
    i12 = -(inv[0][0] * inv[1][2] - inv[1][0] * inv[0][2]) * det;
    i20 = (inv[1][0] * inv[2][1] - inv[2][0] * inv[1][1]) * det;
    i21 = -(inv[0][0] * inv[2][1] - inv[2][0] * inv[0][1]) * det;
    i22 = (inv[0][0] * inv[1][1] - inv[1][0] * inv[0][1]) * det;

    tx = -(i02 * inv[2][3] - (-i00 * inv[0][3] - i01 * inv[1][3]));
    ty = -(i12 * inv[2][3] - (-i10 * inv[0][3] - i11 * inv[1][3]));
    tz = -(i22 * inv[2][3] - (-i20 * inv[0][3] - i21 * inv[1][3]));

    if (inv == dst || src == dst) {
        result[0][0] =
            i02 * src[2][0] + (i00 * src[0][0] + i01 * src[1][0]);
        result[0][1] =
            i02 * src[2][1] + (i00 * src[0][1] + i01 * src[1][1]);
        result[0][2] =
            i02 * src[2][2] + (i00 * src[0][2] + i01 * src[1][2]);
        result[0][3] = i02 * src[2][3] +
                       (i00 * src[0][3] + i01 * src[1][3]) + tx;
        result[1][0] =
            i12 * src[2][0] + (i10 * src[0][0] + i11 * src[1][0]);
        result[1][1] =
            i12 * src[2][1] + (i10 * src[0][1] + i11 * src[1][1]);
        result[1][2] =
            i12 * src[2][2] + (i10 * src[0][2] + i11 * src[1][2]);
        result[1][3] = i12 * src[2][3] +
                       (i10 * src[0][3] + i11 * src[1][3]) + ty;
        result[2][0] =
            i22 * src[2][0] + (i20 * src[0][0] + i21 * src[1][0]);
        result[2][1] =
            i22 * src[2][1] + (i20 * src[0][1] + i21 * src[1][1]);
        result[2][2] =
            i22 * src[2][2] + (i20 * src[0][2] + i21 * src[1][2]);
        result[2][3] = i22 * src[2][3] +
                       (i20 * src[0][3] + i21 * src[1][3]) + tz;
        PSMTXCopy(result, dst);
    } else {
        dst[0][0] =
            i02 * src[2][0] + (i00 * src[0][0] + i01 * src[1][0]);
        dst[0][1] =
            i02 * src[2][1] + (i00 * src[0][1] + i01 * src[1][1]);
        dst[0][2] =
            i02 * src[2][2] + (i00 * src[0][2] + i01 * src[1][2]);
        dst[0][3] = i02 * src[2][3] +
                    (i00 * src[0][3] + i01 * src[1][3]) + tx;
        dst[1][0] =
            i12 * src[2][0] + (i10 * src[0][0] + i11 * src[1][0]);
        dst[1][1] =
            i12 * src[2][1] + (i10 * src[0][1] + i11 * src[1][1]);
        dst[1][2] =
            i12 * src[2][2] + (i10 * src[0][2] + i11 * src[1][2]);
        dst[1][3] = i12 * src[2][3] +
                    (i10 * src[0][3] + i11 * src[1][3]) + ty;
        dst[2][0] =
            i22 * src[2][0] + (i20 * src[0][0] + i21 * src[1][0]);
        dst[2][1] =
            i22 * src[2][1] + (i20 * src[0][1] + i21 * src[1][1]);
        dst[2][2] =
            i22 * src[2][2] + (i20 * src[0][2] + i21 * src[1][2]);
        dst[2][3] = i22 * src[2][3] +
                    (i20 * src[0][3] + i21 * src[1][3]) + tz;
    }
    return TRUE;
}

/* Address: 0x801AA350 | Size: 0xC  -- already-banked (GC/1.3, calibration) */
void _HSD_ObjAllocForgetMemory(void) {
    lbl_8047B2E0 = 0;
}

/* Address: 0x801AA35C | Size: 0x13C */
void HSD_ObjAllocInit(void* data, u32 size, u32 align)
{
    void** current;

    if (data == NULL) {
        __assert(lbl_80274E90, 0x1AE, &lbl_8047DC98);
    }

    if (data != NULL) {
        current = &lbl_8047B2E0;
        while (*current != NULL) {
            if (*current == data) {
                *current = *(void**)((u8*)*current + 0x28);
                break;
            }
            current = (void**)((u8*)*current + 0x28);
        }
    } else {
        lbl_8047B2E0 = NULL;
    }

    memset(data, 0, 0x2C);
    *(s32*)((u8*)data + 0x14) = -1;
    *(u32*)((u8*)data + 0x18) = 0;
    *(s32*)((u8*)data + 0x1C) = -1;

    if (align > 0x20) {
        __assert(lbl_80274E90, 0x1B9, lbl_80274E90 + 0xC);
    }
    if (HSD_GetNbBits(align) != 1) {
        __assert(lbl_80274E90, 0x1BA, lbl_80274E90 + 0x18);
    }

    *(u32*)((u8*)data + 0x24) = align;
    *(u32*)((u8*)data + 0x20) = (size + align - 1) & ~(align - 1);
    *(void**)((u8*)data + 0x28) = lbl_8047B2E0;
    lbl_8047B2E0 = data;
}

/* Address: 0x801AA498 | Size: 0x34  -- HSD_ObjFree (from backup hsd_pobj_disp.c) */
#pragma push
#pragma optimization_level 1
void HSD_ObjFree(void* list, void* data)
{
    void* l = list;
    *(u32*)((u8*)l + 0x8) = *(u32*)((u8*)l + 0x8) - 1;
    fn_801A6960(data);
}
#pragma pop

/* Address: 0x801AA4CC | Size: 0x6C  -- HSD_ObjAlloc (from backup hsd_pobj_disp.c) */
#pragma push
#pragma optimization_level 1
void* HSD_ObjAlloc(void* list)
{
    void* l = list;

    if ((*(u8*)l & 0x80) >> 7) {
        if (*(u32*)((u8*)l + 0x8) >= *(u32*)((u8*)l + 0x14)) {
            return NULL;
        }
    }

    *(u32*)((u8*)l + 0x8) += 1;
    if (*(u32*)((u8*)l + 0x8) > *(u32*)((u8*)l + 0x10)) {
        *(u32*)((u8*)l + 0x10) = *(u32*)((u8*)l + 0x8);
    }

    return fn_801A6928(*(u32*)((u8*)l + 0x20));
}
#pragma pop

/* Address: 0x801AA538 | Size: 0x30  -- HSD_ObjSetHeap (from backup hsd_pobj_disp.c) */
#pragma push
#pragma optimization_level 0
void HSD_ObjSetHeap(void* a, void* b)
{
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x4) = b;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x0) = b;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0xc) = a;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x8) = a;
}
#pragma pop

/* Address: 0x801AA568 | Size: 0x44  -- PObj class info init (small)
 * (from backup hsd_pobj_disp.c) */
void ObjInfoInit_801AA568(void)
{
    hsdInitClassInfo(hsdObj,
                     (void*) lbl_8036C638, (char*) lbl_80274EC8,
                     (char*) &lbl_8047DCA0, 0x3c, 0x8);
}

/* Address: 0x801AA5AC | Size: 0x5C */
void fn_801AA5AC(s32 n)
{
    if (n >= 32) {
        __assert(&lbl_8047DCA8, 0xA4, &lbl_8047DCB0);
    }
    lbl_8036CC40[n + 4] += 1;
}
