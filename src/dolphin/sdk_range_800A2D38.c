/**
 * @file sdk_range_800A2D38.c
 * @brief dolphin-sdk code, 0x800A2D38 - 0x800A3EB0 (37 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). Functions not present here remain asm-only until
 * matched; the range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef f32 Mtx[3][4];
typedef f32 Mtx44[4][4];

typedef struct Vec {
    f32 x;
    f32 y;
    f32 z;
} Vec;

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

extern const f32 lbl_8047C288;
extern const f32 lbl_8047C28C;
extern const f32 lbl_8047C2A8;
extern const f32 lbl_8047C2AC;
extern const f32 lbl_8047C2B0;
extern const f32 lbl_8047C2B4;
extern const f32 lbl_8047C2B8;
extern const f32 lbl_8047C2BC;

extern f32 tanf(f32 x);

#if !defined(SDK_800A37CC_SUFFIX_ACTIVE)
void PSMTXIdentity(Mtx m)
{
    f32 zero = lbl_8047C28C;
    f32 one = lbl_8047C288;

    m[0][2] = zero;
    m[0][3] = zero;
    m[1][2] = zero;
    m[1][3] = zero;
    m[2][0] = zero;
    m[2][1] = zero;
    m[0][0] = one;
    m[0][1] = zero;
    m[1][0] = zero;
    m[1][1] = one;
    m[2][2] = one;
    m[2][3] = zero;
}

void PSMTXCopy(const Mtx src, Mtx dst)
{
    const f64* srcPairs = (const f64*)src;
    f64* dstPairs = (f64*)dst;

    dstPairs[0] = srcPairs[0];
    dstPairs[1] = srcPairs[1];
    dstPairs[2] = srcPairs[2];
    dstPairs[3] = srcPairs[3];
    dstPairs[4] = srcPairs[4];
    dstPairs[5] = srcPairs[5];
}

#pragma peephole off
void PSMTXRotRad(Mtx m, char axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    extern void PSMTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA);
    f32 sinA;
    f32 cosA;

    sinA = sinf(rad);
    cosA = cosf(rad);
    PSMTXRotTrig(m, axis, sinA, cosA);
}
#pragma peephole reset

void PSMTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA)
{
    f32 zero = lbl_8047C28C;
    f32 one = lbl_8047C288;

    if ((axis | 0x20) == 'x') {
        m[0][0] = one;
        m[0][1] = zero;
        m[0][2] = zero;
        m[0][3] = zero;
        m[1][0] = zero;
        m[1][1] = cosA;
        m[1][2] = -sinA;
        m[1][3] = zero;
        m[2][0] = zero;
        m[2][1] = sinA;
        m[2][2] = cosA;
        m[2][3] = zero;
    } else if ((axis | 0x20) == 'y') {
        m[0][0] = cosA;
        m[0][1] = zero;
        m[0][2] = sinA;
        m[0][3] = zero;
        m[1][0] = zero;
        m[1][1] = one;
        m[1][2] = zero;
        m[1][3] = zero;
        m[2][0] = -sinA;
        m[2][1] = zero;
        m[2][2] = cosA;
        m[2][3] = zero;
    } else if ((axis | 0x20) == 'z') {
        m[0][0] = cosA;
        m[0][1] = -sinA;
        m[0][2] = zero;
        m[0][3] = zero;
        m[1][0] = sinA;
        m[1][1] = cosA;
        m[1][2] = zero;
        m[1][3] = zero;
        m[2][0] = zero;
        m[2][1] = zero;
        m[2][2] = one;
        m[2][3] = zero;
    }
}

void __PSMTXRotAxisRadInternal(Mtx m, const Vec* axis, f32 sinA, f32 cosA)
{
    f32 x;
    f32 y;
    f32 z;
    f32 mag;
    f32 scale;
    f32 t;

    mag = axis->x * axis->x + axis->y * axis->y + axis->z * axis->z;
    scale = (f32)__frsqrte(mag);
    scale = scale * 0.5f * (3.0f - mag * scale * scale);
    x = axis->x * scale;
    y = axis->y * scale;
    z = axis->z * scale;
    t = 1.0f - cosA;

    m[0][0] = t * x * x + cosA;
    m[0][1] = t * x * y - sinA * z;
    m[0][2] = t * x * z + sinA * y;
    m[0][3] = 0.0f;

    m[1][0] = t * x * y + sinA * z;
    m[1][1] = t * y * y + cosA;
    m[1][2] = t * y * z - sinA * x;
    m[1][3] = 0.0f;

    m[2][0] = t * x * z - sinA * y;
    m[2][1] = t * y * z + sinA * x;
    m[2][2] = t * z * z + cosA;
    m[2][3] = 0.0f;
}

#pragma peephole off
void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    f32 sinA;
    f32 cosA;

    sinA = sinf(rad);
    cosA = cosf(rad);
    __PSMTXRotAxisRadInternal(m, axis, sinA, cosA);
}
#pragma peephole reset

void PSMTXTrans(Mtx m, f32 xT, f32 yT, f32 zT)
{
    f32 zero = lbl_8047C28C;
    f32 one = lbl_8047C288;

    m[0][3] = xT;
    m[1][3] = yT;
    *(f64*)&m[0][1] = 0.0;
    *(f64*)&m[2][0] = 0.0;
    m[1][0] = zero;
    m[1][1] = one;
    m[1][2] = zero;
    m[2][2] = one;
    m[2][3] = zT;
    m[0][0] = one;
}

void PSMTXTransApply(const Mtx src, Mtx dst, f32 xT, f32 yT, f32 zT)
{
    const f64* srcPairs = (const f64*)src;
    f64* dstPairs = (f64*)dst;

    dstPairs[0] = srcPairs[0];
    dstPairs[1] = srcPairs[1];
    dstPairs[2] = srcPairs[2];
    dstPairs[3] = srcPairs[3];
    dstPairs[4] = srcPairs[4];
    dstPairs[5] = srcPairs[5];
    dst[0][3] = src[0][3] + xT;
    dst[1][3] = src[1][3] + yT;
    dst[2][3] = src[2][3] + zT;
}

void PSMTXScale(Mtx m, f32 xS, f32 yS, f32 zS)
{
    f32 zero = lbl_8047C28C;

    m[0][0] = xS;
    *(f64*)&m[0][1] = 0.0;
    *(f64*)&m[0][3] = 0.0;
    m[1][1] = yS;
    *(f64*)&m[1][2] = 0.0;
    *(f64*)&m[2][0] = 0.0;
    m[2][2] = zS;
    m[2][3] = zero;
}

#pragma optimize_for_size on
void PSMTXScaleApply(const Mtx src, Mtx dst, f32 xS, f32 yS, f32 zS)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        dst[0][i] = src[0][i] * xS;
        dst[1][i] = src[1][i] * yS;
        dst[2][i] = src[2][i] * zS;
    }
}
#pragma optimize_for_size reset

void C_MTXLookAt(Mtx m, const Vec* cameraPosition, const Vec* cameraUp,
                 const Vec* target)
{
    extern void PSVECNormalize(const Vec* source, Vec* destination);
    extern void PSVECCrossProduct(const Vec* first, const Vec* second, Vec* destination);
    Vec look;
    Vec right;
    Vec up;

    look.x = cameraPosition->x - target->x;
    look.y = cameraPosition->y - target->y;
    look.z = cameraPosition->z - target->z;
    PSVECNormalize(&look, &look);
    PSVECCrossProduct(cameraUp, &look, &right);
    PSVECNormalize(&right, &right);
    PSVECCrossProduct(&look, &right, &up);

    m[0][0] = right.x;
    m[0][1] = right.y;
    m[0][2] = right.z;
    m[0][3] = -(cameraPosition->x * right.x + cameraPosition->y * right.y +
                  cameraPosition->z * right.z);

    m[1][0] = up.x;
    m[1][1] = up.y;
    m[1][2] = up.z;
    m[1][3] = -(cameraPosition->x * up.x + cameraPosition->y * up.y +
                  cameraPosition->z * up.z);

    m[2][0] = look.x;
    m[2][1] = look.y;
    m[2][2] = look.z;
    m[2][3] = -(cameraPosition->x * look.x + cameraPosition->y * look.y +
                  cameraPosition->z * look.z);
}

#pragma peephole off
void C_MTXLightFrustum(Mtx m, f32 top, f32 bottom, f32 left, f32 right, f32 near,
                       f32 scaleS, f32 scaleT, f32 transS, f32 transT)
{
    extern const f32 lbl_8047C298;
    extern const f32 lbl_8047C29C;
    f32 tmp;

    tmp = lbl_8047C288 / (right - left);
    m[0][0] = (lbl_8047C298 * near) * tmp * scaleS;
    m[0][1] = lbl_8047C28C;
    m[0][2] = (right + left) * tmp * scaleS - transS;
    m[0][3] = lbl_8047C28C;

    tmp = lbl_8047C288 / (top - bottom);
    m[1][0] = lbl_8047C28C;
    m[1][1] = (lbl_8047C298 * near) * tmp * scaleT;
    m[1][2] = (top + bottom) * tmp * scaleT - transT;
    m[1][3] = lbl_8047C28C;

    m[2][0] = lbl_8047C28C;
    m[2][1] = lbl_8047C28C;
    m[2][2] = lbl_8047C29C;
    m[2][3] = lbl_8047C28C;
}
#pragma peephole reset

#pragma peephole off
void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT,
                           f32 transS, f32 transT)
{
    extern const f32 lbl_8047C290;
    extern const f32 lbl_8047C29C;
    extern const f32 lbl_8047C2A0;
    f32 cot;

    fovY = lbl_8047C290 * fovY;
    fovY = lbl_8047C2A0 * fovY;
    cot = lbl_8047C288 / tanf(fovY);

    m[0][0] = scaleS * (cot / aspect);
    m[0][1] = lbl_8047C28C;
    m[0][2] = -transS;
    m[0][3] = lbl_8047C28C;

    m[1][0] = lbl_8047C28C;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = lbl_8047C28C;

    m[2][0] = lbl_8047C28C;
    m[2][1] = lbl_8047C28C;
    m[2][2] = lbl_8047C29C;
    m[2][3] = lbl_8047C28C;
}
#pragma peephole reset
#endif

#if defined(SDK_800A37CC_SUFFIX_ACTIVE)
#if !defined(SDK_CQUATSLERP_ONLY)
void PSMTXMultVec(const Mtx m, const Vec* src, Vec* dst)
{
    const f64* mPairs = (const f64*)m;
    const f64* srcPairs = (const f64*)src;
    f64 xy = srcPairs[0];
    f32 z = src->z;

    dst->x = (f32)(mPairs[0] * xy + mPairs[1] * (f64)z);
    dst->y = (f32)(mPairs[2] * xy + mPairs[3] * (f64)z);
    dst->z = (f32)(mPairs[4] * xy + mPairs[5] * (f64)z);
}

void PSMTXMultVecSR(const Mtx m, const Vec* src, Vec* dst)
{
    const f64* mPairs = (const f64*)m;
    const f64* srcPairs = (const f64*)src;
    f64 xy = srcPairs[0];
    f32 z = src->z;

    dst->x = (f32)(mPairs[0] * xy + mPairs[1] * (f64)z);
    dst->y = (f32)(mPairs[2] * xy + mPairs[3] * (f64)z);
    dst->z = (f32)(mPairs[4] * xy + mPairs[5] * (f64)z);
}

void C_MTXFrustum(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far)
{
    f32 scaledNear;
    f32 depthScale;
    f32 verticalScale;
    f32 horizontalScale;

    horizontalScale = lbl_8047C2A8 / (right - left);
    scaledNear = lbl_8047C2AC * near;
    verticalScale = lbl_8047C2A8 / (top - bottom);
    depthScale = lbl_8047C2A8 / (far - near);

    m[0][0] = scaledNear * horizontalScale;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = horizontalScale * (right + left);
    m[0][3] = lbl_8047C2B0;

    m[1][0] = lbl_8047C2B0;
    m[1][1] = scaledNear * verticalScale;
    m[1][2] = verticalScale * (top + bottom);
    m[1][3] = lbl_8047C2B0;

    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = -near * depthScale;
    m[2][3] = depthScale * -(far * near);

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B4;
    m[3][3] = lbl_8047C2B0;
}

void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 near, f32 far)
{
    f32 cot;
    f32 tmp;

    fovY = lbl_8047C2B8 * fovY;
    fovY = lbl_8047C2BC * fovY;
    cot = lbl_8047C2A8 / tanf(fovY);
    tmp = lbl_8047C2A8 / (far - near);

    m[0][0] = cot / aspect;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = lbl_8047C2B0;
    m[0][3] = lbl_8047C2B0;

    m[1][0] = lbl_8047C2B0;
    m[1][1] = cot;
    m[1][2] = lbl_8047C2B0;
    m[1][3] = lbl_8047C2B0;

    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = -near * tmp;
    m[2][3] = -(far * near) * tmp;

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B4;
    m[3][3] = lbl_8047C2B0;
}

void C_MTXOrtho(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far)
{
    f32 tmp;

    tmp = lbl_8047C2A8 / (right - left);
    m[0][0] = lbl_8047C2AC * tmp;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = lbl_8047C2B0;
    m[0][3] = -(right + left) * tmp;

    tmp = lbl_8047C2A8 / (top - bottom);
    m[1][0] = lbl_8047C2B0;
    m[1][1] = lbl_8047C2AC * tmp;
    m[1][2] = lbl_8047C2B0;
    m[1][3] = -(top + bottom) * tmp;

    tmp = lbl_8047C2A8 / (far - near);
    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = lbl_8047C2B4 * tmp;
    m[2][3] = -far * tmp;

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B0;
    m[3][3] = lbl_8047C2A8;
}

void PSVECAdd(Vec* lhs, Vec* rhs, Vec* dst)
{
    const f64* lhsPairs = (const f64*)lhs;
    const f64* rhsPairs = (const f64*)rhs;
    f64* dstPairs = (f64*)dst;

    dstPairs[0] = lhsPairs[0] + rhsPairs[0];
    dst->z = lhs->z + rhs->z;
}

void PSVECSubtract(Vec* lhs, Vec* rhs, Vec* dst)
{
    const f64* lhsPairs = (const f64*)lhs;
    const f64* rhsPairs = (const f64*)rhs;
    f64* dstPairs = (f64*)dst;

    dstPairs[0] = lhsPairs[0] - rhsPairs[0];
    dstPairs[1] = lhsPairs[1] - rhsPairs[1];
}

#pragma peephole off
void PSVECScale(Vec* src, f32 scale, Vec* dst)
{
    const f64* srcPairs = (const f64*)src;
    f64* dstPairs = (f64*)dst;
    f64 scale64;

    scale64 = (f64)scale;
    dstPairs[0] = srcPairs[0] * scale64;
    dstPairs[1] = srcPairs[1] * scale64;
}
#pragma peephole reset

#pragma scheduling off
void PSVECNormalize(Vec* src, Vec* dst)
{
    extern const f32 lbl_8047C2C0;
    extern const f32 lbl_8047C2C4;
    f32 half = lbl_8047C2C0;
    f32 three = lbl_8047C2C4;
    const f64* srcPairs = (const f64*)src;
    f64* dstPairs = (f64*)dst;
    f64 xy;
    f32 z;
    f32 mag;
    f32 scale;

    xy = srcPairs[0];
    z = src->z;
    mag = (f32)(xy * xy + (f64)(z * z));
    scale = (f32)__frsqrte(mag);
    scale = (scale * half) * (three - mag * scale * scale);
    dstPairs[0] = xy * (f64)scale;
    dst->z = z * scale;
}
#pragma scheduling reset

#pragma scheduling off
f32 PSVECMag(Vec* src)
{
    extern const f32 lbl_8047C2C0;
    extern const f32 lbl_8047C2C4;
    f32 half = lbl_8047C2C0;
    f32 three = lbl_8047C2C4;
    const f64* srcPairs = (const f64*)src;
    f64 xy;
    f32 z;
    f32 mag;
    f32 scale;

    xy = srcPairs[0];
    z = src->z;
    mag = (f32)(xy * xy + (f64)(z * z));
    if (mag != 0.0f) {
        scale = (f32)__frsqrte(mag);
        scale = (scale * half) * (three - mag * scale * scale);
        mag *= scale;
    }
    return mag;
}
#pragma scheduling reset

f32 PSVECSquareMag(Vec* src)
{
    const f64* srcPairs = (const f64*)src;
    f64 xy;

    xy = srcPairs[1] * srcPairs[1];
    return (f32)(srcPairs[0] * srcPairs[0] + xy);
}

#pragma peephole off
f32 PSVECDotProduct(Vec* lhs, Vec* rhs)
{
    const f64* lhsPairs = (const f64*)lhs;
    const f64* rhsPairs = (const f64*)rhs;
    f64 xy;

    xy = lhsPairs[1] * rhsPairs[1];
    return (f32)(lhsPairs[0] * rhsPairs[0] + xy);
}
#pragma peephole reset

#pragma peephole off
void PSVECCrossProduct(Vec* lhs, Vec* rhs, Vec* dst)
{
    f32 x;
    f32 y;
    f32 z;

    x = lhs->y * rhs->z - lhs->z * rhs->y;
    y = lhs->z * rhs->x - lhs->x * rhs->z;
    z = lhs->x * rhs->y - lhs->y * rhs->x;

    dst->x = x;
    dst->y = y;
    dst->z = z;
}
#pragma peephole reset

#pragma peephole off
f32 PSVECSquareDistance(Vec* lhs, Vec* rhs)
{
    const f64* lhsPairs = (const f64*)lhs;
    const f64* rhsPairs = (const f64*)rhs;
    register f64 xy;
    register f64 tmp;

    xy = lhsPairs[1] - rhsPairs[1];
    tmp = lhsPairs[0] - rhsPairs[0];
    return (f32)(tmp * tmp + xy * xy);
}
#pragma peephole reset

#pragma peephole off
f32 PSVECDistance(Vec* lhs, Vec* rhs)
{
    extern const f32 lbl_8047C2C0;
    extern const f32 lbl_8047C2C4;
    const f64* lhsPairs = (const f64*)lhs;
    const f64* rhsPairs = (const f64*)rhs;
    f64 xy;
    f64 tmp;
    f32 mag;
    f32 half;
    f32 three;
    f32 scale;

    xy = lhsPairs[1] - rhsPairs[1];
    tmp = lhsPairs[0] - rhsPairs[0];
    mag = (f32)(tmp * tmp + xy * xy);
    half = lbl_8047C2C0;
    if (mag != half - half) {
        scale = (f32)__frsqrte(mag);
        three = lbl_8047C2C4;
        scale = (scale * half) * (three - mag * scale * scale);
        mag *= scale;
    }
    return mag;
}
#pragma peephole reset

void PSQUATMultiply(const Quaternion* lhs, const Quaternion* rhs, Quaternion* dst)
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;

    x = lhs->w * rhs->x + lhs->x * rhs->w + lhs->y * rhs->z - lhs->z * rhs->y;
    y = lhs->w * rhs->y - lhs->x * rhs->z + lhs->y * rhs->w + lhs->z * rhs->x;
    z = lhs->w * rhs->z + lhs->x * rhs->y - lhs->y * rhs->x + lhs->z * rhs->w;
    w = lhs->w * rhs->w - lhs->x * rhs->x - lhs->y * rhs->y - lhs->z * rhs->z;

    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = w;
}

#pragma dont_inline on
void C_QUATRotAxisRad(Quaternion* quat, const Vec* axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    extern const f32 lbl_8047C2D0;
    Vec normalized;
    f32 sinHalf;
    f32 cosHalf;

    PSVECNormalize((Vec*)axis, &normalized);
    rad = lbl_8047C2D0 * rad;
    sinHalf = sinf(rad);
    cosHalf = cosf(rad);
    quat->x = sinHalf * normalized.x;
    quat->y = sinHalf * normalized.y;
    quat->z = sinHalf * normalized.z;
    quat->w = cosHalf;
}
#pragma dont_inline reset
#endif

#if !defined(SDK_CQUATSLERP_EXCLUDE)
void C_QUATSlerp(const Quaternion* p, const Quaternion* q, Quaternion* r, f32 t)
{
    extern f32 acosf(f32 x);
    extern f32 sinf(f32 x);
    extern const f32 lbl_8047C2C8;
    extern const f32 lbl_8047C2CC;
    extern const f32 lbl_8047C2D4;
    f32 theta;
    f32 sinTheta;
    f32 cosTheta;
    f32 pScale;
    f32 qScale;

    cosTheta = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;
    qScale = lbl_8047C2CC;

    if (cosTheta < lbl_8047C2C8) {
        cosTheta = -cosTheta;
        qScale = -qScale;
    }

    if (cosTheta <= lbl_8047C2D4) {
        theta = acosf(cosTheta);
        sinTheta = sinf(theta);
        pScale = sinf((lbl_8047C2CC - t) * theta) / sinTheta;
        qScale *= sinf(t * theta) / sinTheta;
    } else {
        pScale = lbl_8047C2CC - t;
        qScale *= t;
    }

    r->x = pScale * p->x + qScale * q->x;
    r->y = pScale * p->y + qScale * q->y;
    r->z = pScale * p->z + qScale * q->z;
    r->w = pScale * p->w + qScale * q->w;
}
#endif
#endif
