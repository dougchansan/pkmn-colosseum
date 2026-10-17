/**
 * @file mtx.c
 * @brief HAL mtx.c: HSD matrix/vector helpers, 0x801A8478 - 0x801AA350.
 *
 * The whole translation unit, written for the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly) with no local pragmas. Its retail extent:
 *   .text   0x801A8478 - 0x801AA350 (mobj.c ends at 0x801A8478, objalloc.c
 *           starts at 0x801AA350 with _HSD_ObjAllocForgetMemory)
 *   .data   0x8036CBC0 - 0x8036CBF0 (HSD_identityMtx, lbl_8036CBC0: the
 *           only object between mobj.c's and objalloc.c's .data; Melee
 *           keeps it in util.c)
 *   .bss    0x80465620 - 0x80465678 (mtx_alloc_data, vec_alloc_data)
 *   .sdata2 0x8047DC48 - 0x8047DC98 ("mtx.c", "mtx", "vec", then the float
 *           pool; objalloc.c's "data" follows)
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/mtx.c). The functions are written in HAL's order;
 * deferred inlining emits them in reverse, which is retail's address order,
 * and the literal pool comes out in first-use order of that reversed code
 * generation. Melee's HSD_MtxGetScale is kept, unreferenced, after the
 * robust HSD_MtxGetScale (HSD_MtxGetScaleMelee): it is generated between
 * HSD_MkRotationMtx and HSD_MtxGetScale and puts its 0.0 / -1.0 doubles
 * ahead of HSD_MtxGetScale's 1e-10f, 0.5 and 3.0, which is retail's pool
 * order (the robust function's own first use is 1e-10f); the linker
 * strips it. Built as the whole TU with lbl_8047DC90 sized 4 (its last 4
 * bytes are section padding), every section pairs at 100% and every
 * function but HSD_MtxGetScale is exact.
 *
 * Colosseum's HAL version differs from Melee's (all read from retail):
 *  - HSD_MtxSRT and HSD_MtxSRTQuat divide by the parent scale through
 *    HSD_MtxReciprocal (1 / (x +- FLT_EPSILON), expanded six times), and
 *    HSD_MtxSRTQuat folds the inverse parent scale into the rotation;
 *  - HSD_MtxGetScale guards every normalisation with |v|^2 > 1e-10 and
 *    handles degenerate columns; it keeps one float for the squared
 *    length, its inverse root and the column length (a separate inverse
 *    costs an fmr in the first column);
 *  - HSD_MtxInverseConcat (fn_801A9DF0) returns FALSE for a singular
 *    matrix instead of copying src, and tests det == 0;
 *  - HSD_MtxGetRotation (HSD_MtxGetRotation) tests against FLT_MIN and answers
 *    x == 0 with +-pi/2 before atan2 (calcVal);
 *  - HSD_MtxGetRotationMtx (HSD_MtxGetRotationMtx) is new: the orthonormal rotation
 *    of m built from axis `a` first and axis `b` second;
 *  - assert line numbers are those of Colosseum's longer file.
 *
 * Linked as the whole TU (2026-09-29) under a title-path rule exception
 * (docs/RULE_EXCEPTIONS.md): HSD_MtxGetScale is on the recomp's title path
 * (a native_functions entry of the gs-material-init row) and is exact only
 * with the dead copy described next, tagged RULE-EXCEPTION(title-path) in
 * the function. Without it HSD_MtxGetScale is 98.9%. Retail saves
 * and restores f31 (stfd/psq_st, frame 0x70) but never uses it, and the
 * saves reorder the prologue; everything else is exact. MWCC -O1 allocates
 * per variable and runs its dead-code cleanup after register allocation,
 * so a float variable that is live across a call when registers are
 * assigned but whose every instruction is later removed as dead leaves
 * exactly this: a dead copy (t = len; ...call...; u = t;) reproduces the
 * function byte for byte. No natural HAL-side construct that does this
 * has been found (tried: an f64 sign variable, const/initialised locals, a
 * dot-product variable, separate x/y/z temporaries, inline normalise
 * helpers with and without return values, inline parameters); the dead
 * copy spans column 1's PSVECScale.
 *
 * Further evidence (2026-09-27 lane): the phantom is a second variable
 * holding the inverse length across a column's PSVECScale(v, v, inv).
 * A dead copy spanning column 1's or column 2's PSVECScale is byte-exact;
 * one spanning PSVECDotProduct, PSVECCrossProduct or PSVECMag instead
 * recolours len (it coalesces into f31) or the z column's sqrtf result.
 * MSL's sqrtf is not the source: its "volatile float y" form adds a
 * stfs/lfs round trip retail does not have, and const or static const
 * _half/_three locals, with or without the double copy of x, compile
 * identically to the literal form. Kirby Air Ride's HSD_MtxGetScale
 * (wowjinxy/KAR: sq1/sq2/sq3, block-scope invMag, f64 neg) adds an fmr per
 * column and still has no f31. Helpers that normalise a column and return
 * (or drop) the inverse length leave no f31 either: MWCC removes an unused
 * inline return value before allocating registers, and those helpers also
 * reorder the fpclassify stack slots.
 */

#include "crt/float.h"
#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_objalloc.h"

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

#define M_PI 3.14159265358979323846

extern f64 asin(f64 x);

extern void PSMTXCopy(const Mtx src, Mtx dst);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
extern void PSMTXQuat(Mtx m, const Quaternion* q);
extern void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
extern void PSVECScale(const Vec* src, Vec* dst, f32 scale);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* ab);
extern void PSVECNormalize(const Vec* src, Vec* unit);
extern f32 PSVECSquareMag(const Vec* v);
extern f32 PSVECMag(const Vec* v);
extern f32 PSVECDotProduct(const Vec* a, const Vec* b);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* axb);

extern void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
extern void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);

/* HSD_identityMtx (util.c in Melee) */
Mtx lbl_8036CBC0 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

/* vec_alloc_data / mtx_alloc_data. MWCC emits these .bss objects in
 * reverse declaration order, so mtx_alloc_data lands first (0x80465620),
 * as in retail; list.c's pair is declared the same way. */
HSD_ObjAllocData lbl_8046564C;
HSD_ObjAllocData lbl_80465620;

/* HSD_MtxInverseConcat */
BOOL fn_801A9DF0(Mtx inv, Mtx src, Mtx dest)
{
    Mtx m;
    f32 temp1;
    f32 temp2;
    f32 temp7;
    f32 temp3;
    f32 temp4;
    f32 temp8;
    f32 temp5;
    f32 temp6;
    f32 temp9;
    f32 det;
    f32 temp10;
    f32 temp11;
    f32 temp12;

    det = inv[0][0] * inv[1][1] * inv[2][2] + inv[0][1] * inv[1][2] * inv[2][0] +
          inv[0][2] * inv[1][0] * inv[2][1] - inv[2][0] * inv[1][1] * inv[0][2] -
          inv[1][0] * inv[0][1] * inv[2][2] - inv[0][0] * inv[2][1] * inv[1][2];
    if (det == 0.0f) {
        return FALSE;
    }

    det = 1.0f / det;
    temp1 = ((inv[1][1] * inv[2][2]) - (inv[2][1] * inv[1][2])) * det;
    temp2 = (-((inv[0][1] * inv[2][2]) - (inv[2][1] * inv[0][2]))) * det;
    temp7 = ((inv[0][1] * inv[1][2]) - (inv[1][1] * inv[0][2])) * det;
    temp3 = (-((inv[1][0] * inv[2][2]) - (inv[2][0] * inv[1][2]))) * det;
    temp4 = ((inv[0][0] * inv[2][2]) - (inv[2][0] * inv[0][2])) * det;
    temp8 = (-((inv[0][0] * inv[1][2]) - (inv[1][0] * inv[0][2]))) * det;
    temp5 = ((inv[1][0] * inv[2][1]) - (inv[2][0] * inv[1][1])) * det;
    temp6 = (-((inv[0][0] * inv[2][1]) - (inv[2][0] * inv[0][1]))) * det;
    temp9 = ((inv[0][0] * inv[1][1]) - (inv[1][0] * inv[0][1])) * det;
    temp10 = -((temp7 * inv[2][3]) -
               (((-temp1) * inv[0][3]) - (temp2 * inv[1][3])));
    temp11 = -((temp8 * inv[2][3]) -
               (((-temp3) * inv[0][3]) - (temp4 * inv[1][3])));
    temp12 = -((temp9 * inv[2][3]) -
               (((-temp5) * inv[0][3]) - (temp6 * inv[1][3])));

    if (inv == dest || src == dest) {
        m[0][0] = temp7 * src[2][0] + (temp1 * src[0][0] + temp2 * src[1][0]);
        m[0][1] = temp7 * src[2][1] + (temp1 * src[0][1] + temp2 * src[1][1]);
        m[0][2] = temp7 * src[2][2] + (temp1 * src[0][2] + temp2 * src[1][2]);
        m[0][3] = temp7 * src[2][3] + (temp1 * src[0][3] + temp2 * src[1][3]) +
                  temp10;
        m[1][0] = temp8 * src[2][0] + (temp3 * src[0][0] + temp4 * src[1][0]);
        m[1][1] = temp8 * src[2][1] + (temp3 * src[0][1] + temp4 * src[1][1]);
        m[1][2] = temp8 * src[2][2] + (temp3 * src[0][2] + temp4 * src[1][2]);
        m[1][3] = temp8 * src[2][3] + (temp3 * src[0][3] + temp4 * src[1][3]) +
                  temp11;
        m[2][0] = temp9 * src[2][0] + (temp5 * src[0][0] + temp6 * src[1][0]);
        m[2][1] = temp9 * src[2][1] + (temp5 * src[0][1] + temp6 * src[1][1]);
        m[2][2] = temp9 * src[2][2] + (temp5 * src[0][2] + temp6 * src[1][2]);
        m[2][3] = temp9 * src[2][3] + (temp5 * src[0][3] + temp6 * src[1][3]) +
                  temp12;
        PSMTXCopy(m, dest);
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
    return TRUE;
}

static inline f32 calcVal(f32 x, f32 y)
{
    if (x == 0.0f) {
        if (y >= 0.0f) {
            return M_PI / 2;
        } else {
            return -M_PI / 2;
        }
    } else {
        return atan2f(y, x);
    }
}

/* HSD_MtxGetRotation */
void HSD_MtxGetRotation(Mtx m, Vec3* vec)
{
    f32 length0;
    f32 length1;
    f32 length2;
    f32 testVal_1;
    f32 val_01;

    length0 = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    if (!(length0 < FLT_MIN)) {
        length1 =
            sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
        if (!(length1 < FLT_MIN)) {
            length2 = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] +
                            m[2][2] * m[2][2]);
            if (!(length2 < FLT_MIN)) {
                testVal_1 = -m[2][0];
                testVal_1 /= length0;

                if (testVal_1 >= 1.0f) {
                    val_01 = M_PI / 2;
                } else if (testVal_1 <= -1.0f) {
                    val_01 = -M_PI / 2;
                } else {
                    val_01 = asin(testVal_1);
                }

                vec->y = val_01;

                if (cosf(vec->y) >= FLT_MIN) {
                    f32 testVal_2_pre = m[2][2] / length2;
                    f32 testVal_3_pre = m[2][1] / length1;

                    vec->x = calcVal(testVal_2_pre, testVal_3_pre);
                    vec->z = calcVal(m[0][0], m[1][0]);
                    return;
                }

                vec->x = calcVal(m[1][1], m[0][1]);
                vec->z = 0.0f;
                return;
            }
        }
    }

    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
}

/* HSD_MtxGetRotationMtx: the pure rotation of m, orthonormalised starting
 * from axis `a`, with axis `b` as the secondary one. */
void HSD_MtxGetRotationMtx(Mtx m, Mtx dst, char a, char b)
{
    Vec3 x;
    Vec3 y;
    Vec3 z;

    switch (a) {
    case 'x':
    case 'X':
        x.x = m[0][0];
        x.y = m[1][0];
        x.z = m[2][0];
        PSVECNormalize(&x, &x);
        switch (b) {
        case 'z':
        case 'Z':
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&x, &y, &z);
            break;
        default:
            y.x = m[0][1];
            y.y = m[1][1];
            y.z = m[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&z, &x, &y);
            break;
        }
        break;
    case 'y':
    case 'Y':
        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];
        PSVECNormalize(&y, &y);
        switch (b) {
        case 'x':
        case 'X':
            x.x = m[0][0];
            x.y = m[1][0];
            x.z = m[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&y, &z, &x);
            break;
        default:
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&x, &y, &z);
            break;
        }
        break;
    default:
        z.x = m[0][2];
        z.y = m[1][2];
        z.z = m[2][2];
        PSVECNormalize(&z, &z);
        switch (b) {
        case 'y':
        case 'Y':
            y.x = m[0][1];
            y.y = m[1][1];
            y.z = m[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&z, &x, &y);
            break;
        default:
            x.x = m[0][0];
            x.y = m[1][0];
            x.z = m[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&y, &z, &x);
            break;
        }
        break;
    }

    dst[0][0] = x.x;
    dst[1][0] = x.y;
    dst[2][0] = x.z;
    dst[0][1] = y.x;
    dst[1][1] = y.y;
    dst[2][1] = y.z;
    dst[0][2] = z.x;
    dst[1][2] = z.y;
    dst[2][2] = z.z;
    dst[0][3] = 0.0f;
    dst[1][3] = 0.0f;
    dst[2][3] = 0.0f;
}

void HSD_MtxGetTranslate(Mtx mat, Vec3* vec)
{
    vec->x = mat[0][3];
    vec->y = mat[1][3];
    vec->z = mat[2][3];
}

void HSD_MtxGetScale(Mtx m, Vec3* scale)
{
    Vec3 x;
    Vec3 y;
    Vec3 z;
    Vec3 proj;
    f32 len;
    f32 inv; /* RULE-EXCEPTION(title-path): dead copy, register allocation only - see docs/RULE_EXCEPTIONS.md */

    x.x = m[0][0];
    x.y = m[1][0];
    x.z = m[2][0];
    len = PSVECSquareMag(&x);
    if (len > 1e-10f) {
        len = sqrtf(1.0f / len);
        scale->x = 1.0f / len;
        inv = len; /* RULE-EXCEPTION(title-path): dead copy - see docs/RULE_EXCEPTIONS.md */
        PSVECScale(&x, &x, len);
        len = inv; /* RULE-EXCEPTION(title-path): dead copy - see docs/RULE_EXCEPTIONS.md */

        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];
        PSVECScale(&x, &proj, PSVECDotProduct(&x, &y));
        PSVECSubtract(&y, &proj, &y);
        len = PSVECSquareMag(&y);
        if (len > 1e-10f) {
            len = sqrtf(1.0f / len);
            scale->y = 1.0f / len;
            PSVECScale(&y, &y, len);

            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(&y, &proj, PSVECDotProduct(&y, &z));
            PSVECSubtract(&z, &proj, &z);
            PSVECScale(&x, &proj, PSVECDotProduct(&x, &z));
            PSVECSubtract(&z, &proj, &z);
            len = PSVECSquareMag(&z);
            if (len > 1e-10f) {
                scale->z = sqrtf(len);
                PSVECCrossProduct(&y, &z, &proj);
                if (PSVECDotProduct(&x, &proj) < 0.0) {
                    scale->x *= -1.0;
                    scale->y *= -1.0;
                    scale->z *= -1.0;
                }
            } else {
                scale->z = 0.0f;
            }
        } else {
            scale->y = 0.0f;
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(&x, &proj, PSVECDotProduct(&x, &z));
            PSVECSubtract(&z, &proj, &z);
            len = PSVECSquareMag(&z);
            if (len > 1e-10f) {
                scale->z = sqrtf(len);
            } else {
                scale->z = 0.0f;
            }
        }
    } else {
        scale->x = 0.0f;
        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];
        len = PSVECSquareMag(&y);
        if (len > 1e-10f) {
            len = sqrtf(1.0f / len);
            scale->y = 1.0f / len;
            PSVECScale(&y, &y, len);

            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECScale(&y, &proj, PSVECDotProduct(&y, &z));
            PSVECSubtract(&z, &proj, &z);
            scale->z = PSVECMag(&z);
        } else {
            scale->y = 0.0f;
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            scale->z = PSVECMag(&z);
        }
    }
}

/* Melee's HSD_MtxGetScale, kept unchanged next to the robust version above
 * (see the file header). Nothing references it and the linker strips it,
 * but it is compiled: its two double literals (0.0 and -1.0) come before
 * HSD_MtxGetScale's in the .sdata2 pool, as in retail. */
void HSD_MtxGetScaleMelee(Mtx arg0, Vec3* arg1)
{
    f64 scale;
    Vec3 vec1;
    Vec3 vec2;
    Vec3 vec3;
    Vec3 vec4;

    vec1.x = arg0[0][0];
    vec1.y = arg0[1][0];
    vec1.z = arg0[2][0];

    arg1->x = PSVECMag(&vec1);
    PSVECNormalize(&vec1, &vec1);

    vec2.x = arg0[0][1];
    vec2.y = arg0[1][1];
    vec2.z = arg0[2][1];

    PSVECScale(&vec1, &vec4, PSVECDotProduct(&vec1, &vec2));
    PSVECSubtract(&vec2, &vec4, &vec2);
    arg1->y = PSVECMag(&vec2);
    PSVECNormalize(&vec2, &vec2);

    vec3.x = arg0[0][2];
    vec3.y = arg0[1][2];
    vec3.z = arg0[2][2];

    PSVECScale(&vec2, &vec4, PSVECDotProduct(&vec2, &vec3));
    PSVECSubtract(&vec3, &vec4, &vec3);
    PSVECScale(&vec1, &vec4, PSVECDotProduct(&vec1, &vec3));
    PSVECSubtract(&vec3, &vec4, &vec3);
    arg1->z = PSVECMag(&vec3);
    PSVECNormalize(&vec3, &vec3);
    PSVECCrossProduct(&vec2, &vec3, &vec4);

    if (PSVECDotProduct(&vec1, &vec4) < 0.0) {
        scale = -1.0;
        arg1->x *= scale;
        arg1->y *= scale;
        arg1->z *= scale;
    }
}

void HSD_MkRotationMtx(Mtx m, Vec3* r)
{
    f32 sinX;
    f32 sinY;
    f32 sinZ;
    f32 cosX;
    f32 cosY;
    f32 cosZ;

    sinX = sinf(r->x);
    cosX = cosf(r->x);
    sinY = sinf(r->y);
    cosY = cosf(r->y);
    sinZ = sinf(r->z);
    cosZ = cosf(r->z);

    m[0][0] = cosY * cosZ;
    m[1][0] = cosY * sinZ;
    m[2][0] = -sinY;
    m[0][1] = (cosZ * (sinX * sinY)) - (cosX * sinZ);
    m[1][1] = (sinZ * (sinX * sinY)) + (cosX * cosZ);
    m[2][1] = sinX * cosY;
    m[0][2] = (cosZ * (cosX * sinY)) + (sinX * sinZ);
    m[1][2] = (sinZ * (cosX * sinY)) - (sinX * cosZ);
    m[2][2] = cosX * cosY;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
}

/* 1 / x with the divisor pushed away from zero by FLT_EPSILON. */
static inline f32 HSD_MtxReciprocal(f32 x)
{
    if (x >= 0.0f) {
        return 1.0f / (x + FLT_EPSILON);
    } else {
        return 1.0f / (x - FLT_EPSILON);
    }
}

void HSD_MtxSRT(Mtx m, Vec3* s, Vec3* r, Vec3* t, Vec3* ps)
{
    f32 sinX = sinf(r->x);
    f32 cosX = cosf(r->x);
    f32 sinY = sinf(r->y);
    f32 cosY = cosf(r->y);
    f32 sinZ = sinf(r->z);
    f32 cosZ = cosf(r->z);
    f32 sx_2;
    f32 sy_2;
    f32 sz_2;
    f32 sx_1;
    f32 sy_1;
    f32 sz_1;
    f32 sx;
    f32 sy;
    f32 sz;

    sx_2 = sx_1 = sx = s->x;
    sy_2 = sy_1 = sy = s->y;
    sz_2 = sz_1 = sz = s->z;

    if (ps != NULL) {
        f32 ix = HSD_MtxReciprocal(ps->x);
        f32 iy = HSD_MtxReciprocal(ps->y);
        f32 iz = HSD_MtxReciprocal(ps->z);

        sy_2 *= ps->y * ix;
        sz_2 *= ps->z * ix;
        sx_1 *= ps->x * iy;
        sz_1 *= ps->z * iy;
        sx *= ps->x * iz;
        sy *= ps->y * iz;
    }

    m[0][0] = cosZ * (sx_2 * cosY);
    m[1][0] = sinZ * (sx_1 * cosY);
    m[2][0] = -sx * sinY;
    m[0][1] = sy_2 * ((cosZ * (sinX * sinY)) - (cosX * sinZ));
    m[1][1] = sy_1 * ((sinZ * (sinX * sinY)) + (cosX * cosZ));
    m[2][1] = cosY * (sy * sinX);
    m[0][2] = sz_2 * ((cosZ * (cosX * sinY)) + (sinX * sinZ));
    m[1][2] = sz_1 * ((sinZ * (cosX * sinY)) - (sinX * cosZ));
    m[2][2] = cosY * (sz * cosX);
    m[0][3] = t->x;
    m[1][3] = t->y;
    m[2][3] = t->z;
}

void HSD_MtxSRTQuat(Mtx m, Vec3* s, Quaternion* r, Vec3* t, Vec3* ps)
{
    Mtx rot;

    PSMTXScale(m, s->x, s->y, s->z);
    PSMTXQuat(rot, r);
    if (ps != NULL) {
        f32 ix = HSD_MtxReciprocal(ps->x);
        f32 iy = HSD_MtxReciprocal(ps->y);
        f32 iz = HSD_MtxReciprocal(ps->z);

        rot[0][1] *= ps->y * ix;
        rot[0][2] *= ps->y * ix;
        rot[1][0] *= ps->x * iy;
        rot[1][2] *= ps->z * iy;
        rot[2][0] *= ps->x * iz;
        rot[2][1] *= ps->y * iz;
    }
    PSMTXConcat(rot, m, m);
    m[0][3] = t->x;
    m[1][3] = t->y;
    m[2][3] = t->z;
}

void HSD_MtxScaledAdd(Mtx a, Mtx b, Mtx dst, f32 scale)
{
    f32* arr0 = &a[0][0];
    f32* arr1 = &b[0][0];
    f32* arr2 = &dst[0][0];

    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);

    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);

    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
    *arr2++ = *arr1++ + (scale * *arr0++);
}

void* HSD_VecAlloc(void)
{
    void* vec = HSD_ObjAlloc(&lbl_8046564C);

    HSD_ASSERT(887, vec);
    return vec;
}

void HSD_VecFree(void* vec)
{
    if (vec != NULL) {
        HSD_ObjFree(&lbl_8046564C, vec);
    }
}

void* HSD_MtxAlloc(void)
{
    void* mtx = HSD_ObjAlloc(&lbl_80465620);

    HSD_ASSERT(918, mtx);
    return mtx;
}

void HSD_MtxFree(void* mtx)
{
    if (mtx != NULL) {
        HSD_ObjFree(&lbl_80465620, mtx);
    }
}

HSD_ObjAllocData* HSD_VecGetAllocData(void)
{
    return &lbl_8046564C;
}

void HSD_VecInitAllocData(void)
{
    HSD_ObjAllocInit(&lbl_8046564C, sizeof(Vec), 4);
}

HSD_ObjAllocData* HSD_MtxGetAllocData(void)
{
    return &lbl_80465620;
}

void HSD_MtxInitAllocData(void)
{
    HSD_ObjAllocInit(&lbl_80465620, sizeof(Mtx), 4);
}
