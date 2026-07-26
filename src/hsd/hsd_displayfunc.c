/**
 * @file hsd_displayfunc.c
 * @brief HSD displayfunc.c -- Envelope and billboard matrix construction.
 *
 * Decompiled from:
 *   _HSD_mkEnvelopeModelNodeMtx (envelope-model node matrix setup)
 *   HSD_JObjMakePositionMtx (JObj billboard position-matrix construction)
 *
 * Source file reference: "displayfunc.c" (rodata string at lbl_802746DC)
 *
 * These functions build matrices used by HSD's JObj/DObj renderer:
 *
 * 1. _HSD_mkEnvelopeModelNodeMtx: Walks a JObj's parent chain to find
 *    its skeleton or skeleton-root node, then builds the inverse envelope
 *    matrix needed to transform the model node into skeleton space.
 *
 * 2. HSD_JObjMakePositionMtx: Concatenates the view and JObj matrices,
 *    then applies the appropriate billboard transform selected by the
 *    JOBJ_BILLBOARD_FIELD bits (0xE00). A non-billboard JObj uses the
 *    concatenated matrix directly.
 *
 * These are part of the HAL SysDolphin library, customised for
 * Pokemon Colosseum's rendering pipeline.
 *
 * Assertion strings:
 *   "displayfunc.c" (file name for assert)
 *   "unkown type of billboard."  (sic -- typo in original)
 *
 * Address range: 0x80197A64 - 0x80197C70
 */

#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "crt/math.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_jobj.h"

/* ===== External functions ===== */
extern void __assert(const char* file, u32 line, const char* msg); /* HSD_Halt / assert */
extern void HSD_Panic(const char* file, u32 line, const char* msg);
extern void PSMTXInverse(const Mtx srcMtx, Mtx dstMtx);
extern void PSMTXConcat(const Mtx srcMtx, const Mtx jointMtx, Mtx dstMtx);
extern void PSMTXScale(Mtx mtx, f32 x, f32 y, f32 z);
extern void PSMTXRotRad(Mtx mtx, char axis, f32 radians);
extern void fn_801A9DF0(void* a, void* b, void* c);                  /* HSD_MtxInverseConcat */

/* Billboard subroutines - fn_80198038/fn_801985E0/fn_80198B20 are defined below. */
void fn_80197C70(HSD_JObj* jobj, Mtx src, Mtx dst); /* rotated billboard */

/* ===== String constants (rodata) ===== */
extern const char lbl_802746DC[]; /* "displayfunc.c" */
extern const char lbl_802746EC[]; /* "unkown type of billboard.\n" */

/* ===== SDA2 assertion expression strings ===== */
extern const char lbl_8047D9E8[] __attribute__((section(".sdata2"))); /* "jobj" */
extern const char lbl_8047D9F0[] __attribute__((section(".sdata2"))); /* "x" */

/* ========================================================================
 * HSD_JObj fields used by _HSD_mkEnvelopeModelNodeMtx:
 *
 * offset 0x0C: HSD_JObj* parent
 * offset 0x14: u32 flags
 * offset 0x44: Mtx local-to-world matrix
 * offset 0x78: MtxPtr envelope matrix
 * ======================================================================== */

typedef struct DisplayFuncVec {
    f32 x;
    f32 y;
    f32 z;
} DisplayFuncVec;

extern void PSVECScale(void*, void*, f32);
extern f32 PSVECMag(void*);
extern void PSVECCrossProduct(void*, void*, void*);
extern const DisplayFuncVec lbl_802746D0; /* { 0.0f, 0.0f, 1.0f } */
extern f32 lbl_80478ACC;

#define DISPLAYFUNC_FLAG_2000 0x2000
#define DISPLAYFUNC_MTX(mtx, row, col) (((f32*) (mtx))[(row) * 4 + (col)])
#define DISPLAYFUNC_LOAD_COLUMN(dst, mtx, col) \
    do {                                       \
        (dst).x = DISPLAYFUNC_MTX(mtx, 0, col); \
        (dst).y = DISPLAYFUNC_MTX(mtx, 1, col); \
        (dst).z = DISPLAYFUNC_MTX(mtx, 2, col); \
    } while (0)
#define DISPLAYFUNC_STORE_COLUMN(dst, col, src, scale)      \
    do {                                                    \
        DISPLAYFUNC_MTX(dst, 0, col) = (src).x * (scale);   \
        DISPLAYFUNC_MTX(dst, 1, col) = (src).y * (scale);   \
        DISPLAYFUNC_MTX(dst, 2, col) = (src).z * (scale);   \
    } while (0)
#define DISPLAYFUNC_LOAD_TRANSLATION(dst, mtx) DISPLAYFUNC_LOAD_COLUMN(dst, mtx, 3)
#define DISPLAYFUNC_STORE_TRANSLATION(dst, src)       \
    do {                                             \
        DISPLAYFUNC_MTX(dst, 0, 3) = (src).x;        \
        DISPLAYFUNC_MTX(dst, 1, 3) = (src).y;        \
        DISPLAYFUNC_MTX(dst, 2, 3) = (src).z;        \
    } while (0)
#define DISPLAYFUNC_SET_VEC(dst, vx, vy, vz) \
    do {                                     \
        (dst).x = (vx);                      \
        (dst).y = (vy);                      \
        (dst).z = (vz);                      \
    } while (0)

inline s32 displayfunc_fpclassifyf(f32 value)
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

inline f32 displayfunc_sqrtf(f32 value)
{
    if (value > 0.0F) {
        f64 guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < 0.0) {
        return lbl_80478AC0[0];
    }
    if (displayfunc_fpclassifyf(value) == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

static inline f32 displayfuncMtxColMag(Mtx mtx, int col)
{
    return displayfunc_sqrtf(mtx[0][col] * mtx[0][col] +
                             mtx[1][col] * mtx[1][col] +
                             mtx[2][col] * mtx[2][col]);
}

/* 0x80198038 | 0x5A8 -- mkBillBoardMtx */
void fn_80198038(HSD_JObj* jobj, Mtx src, Mtx dst)
{
    DisplayFuncVec x;
    DisplayFuncVec y;
    DisplayFuncVec z;
    DisplayFuncVec position;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 magnitude;

    sx = displayfuncMtxColMag(src, 0);
    sz = displayfuncMtxColMag(src, 2);
    y.x = src[0][1];
    y.y = src[1][1];
    y.z = src[2][1];
    sy = PSVECMag(&y);
    position.x = src[0][3];
    position.y = src[1][3];
    position.z = src[2][3];

    if (jobj->flags & DISPLAYFUNC_FLAG_2000) {
        magnitude = PSVECMag(&position);
        PSVECScale(&position, &z, -1.0F / (lbl_80478ACC + magnitude));
    } else {
        z = lbl_802746D0;
    }

    PSVECScale(&y, &y, 1.0F / (sy + lbl_80478ACC));
    PSVECCrossProduct(&y, &z, &x);
    magnitude = PSVECMag(&x);
    if (magnitude >= lbl_80478ACC) {
        sx /= magnitude;
        PSVECCrossProduct(&z, &x, &y);
        sy /= lbl_80478ACC + PSVECMag(&y);
        dst[0][0] = sx * x.x;
        dst[1][0] = sx * x.y;
        dst[2][0] = sx * x.z;
        dst[0][1] = sy * y.x;
        dst[1][1] = sy * y.y;
        dst[2][1] = sy * y.z;
    } else {
        DisplayFuncVec y2;
        f32 scale;

        magnitude = displayfunc_sqrtf(z.x * z.x + z.z * z.z);
        scale = -z.y / (lbl_80478ACC + magnitude);
        y2.x = z.x * scale;
        y2.y = lbl_80478ACC + magnitude;
        y2.z = z.z * scale;
        PSVECCrossProduct(&y2, &z, &x);
        sx /= lbl_80478ACC + PSVECMag(&x);
        dst[0][0] = sx * x.x;
        dst[1][0] = sx * x.y;
        dst[2][0] = sx * x.z;
        dst[0][1] = y2.x;
        dst[1][1] = y2.y;
        dst[2][1] = y2.z;
    }

    dst[0][2] = sz * z.x;
    dst[1][2] = sz * z.y;
    dst[2][2] = sz * z.z;
    dst[0][3] = position.x;
    dst[1][3] = position.y;
    dst[2][3] = position.z;
}

extern void PSMTXCopy(void*, void*);
extern void PSVECNormalize(void*, void*);

/* 0x801985E0 | 0x540 -- mkHBillBoardMtx */
void fn_801985E0(HSD_JObj* jobj, Mtx src, Mtx dst)
{
    DisplayFuncVec position;
    DisplayFuncVec x;
    DisplayFuncVec x_unit;
    DisplayFuncVec y;
    DisplayFuncVec z;
    f32 sy;
    f32 sz;
    f32 magnitude;

    position.x = src[0][3];
    position.y = src[1][3];
    position.z = src[2][3];
    x.x = src[0][0];
    x.y = src[1][0];
    x.z = src[2][0];
    magnitude = PSVECMag(&x);
    PSVECScale(&x, &x_unit, 1.0F / (lbl_80478ACC + magnitude));
    sy = displayfuncMtxColMag(src, 1);
    sz = displayfuncMtxColMag(src, 2);

    if (jobj->flags & DISPLAYFUNC_FLAG_2000) {
        f32 horizontal;
        f32 scale;

        horizontal =
            displayfunc_sqrtf(position.x * position.x + position.z * position.z);
        scale = -position.y / (lbl_80478ACC + horizontal);
        y.x = position.x * scale;
        y.y = lbl_80478ACC + horizontal;
        y.z = position.z * scale;
        PSVECNormalize(&y, &y);
    } else {
        y.x = 0.0F;
        y.y = 1.0F;
        y.z = 0.0F;
    }

    PSVECCrossProduct(&x_unit, &y, &z);
    magnitude = PSVECMag(&z);
    if (magnitude < lbl_80478ACC) {
        PSMTXCopy(src, dst);
    } else {
        sz /= magnitude;
        PSVECCrossProduct(&z, &x_unit, &y);
        sy /= lbl_80478ACC + PSVECMag(&y);
        dst[0][0] = x.x;
        dst[1][0] = x.y;
        dst[2][0] = x.z;
        dst[0][1] = sy * y.x;
        dst[1][1] = sy * y.y;
        dst[2][1] = sy * y.z;
        dst[0][2] = sz * z.x;
        dst[1][2] = sz * z.y;
        dst[2][2] = sz * z.z;
        dst[0][3] = position.x;
        dst[1][3] = position.y;
        dst[2][3] = position.z;
    }
}

/* 0x80198B20 | 0x42C -- mkVBillBoardMtx */
void fn_80198B20(HSD_JObj* jobj, Mtx src, Mtx dst)
{
    DisplayFuncVec position;
    DisplayFuncVec y;
    DisplayFuncVec y_unit;
    DisplayFuncVec x;
    DisplayFuncVec z;
    f32 sx;
    f32 sz;
    f32 magnitude;

    position.x = src[0][3];
    position.y = src[1][3];
    position.z = src[2][3];
    y.x = src[0][1];
    y.y = src[1][1];
    y.z = src[2][1];
    magnitude = PSVECMag(&y);
    PSVECScale(&y, &y_unit, 1.0F / (lbl_80478ACC + magnitude));
    sx = displayfuncMtxColMag(src, 0);
    sz = displayfuncMtxColMag(src, 2);

    if (jobj->flags & DISPLAYFUNC_FLAG_2000) {
        magnitude = PSVECMag(&position);
        PSVECScale(&position, &z, -1.0F / (lbl_80478ACC + magnitude));
        PSVECCrossProduct(&y_unit, &z, &x);
    } else {
        z = lbl_802746D0;
        PSVECCrossProduct(&y_unit, &z, &x);
    }

    magnitude = PSVECMag(&x);
    if (magnitude < lbl_80478ACC) {
        PSMTXCopy(src, dst);
    } else {
        sx /= magnitude;
        PSVECCrossProduct(&x, &y_unit, &z);
        sz /= lbl_80478ACC + PSVECMag(&z);
        dst[0][0] = sx * x.x;
        dst[1][0] = sx * x.y;
        dst[2][0] = sx * x.z;
        dst[0][1] = y.x;
        dst[1][1] = y.y;
        dst[2][1] = y.z;
        dst[0][2] = sz * z.x;
        dst[1][2] = sz * z.y;
        dst[2][2] = sz * z.z;
        dst[0][3] = position.x;
        dst[1][3] = position.y;
        dst[2][3] = position.z;
    }
}

/* 0x80198F4C | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);
extern u8 lbl_80465348[];
#if 0
asm void HSD_ZListInitAllocData(void) {
#include "src/hsd/hsd_displayfunc_fn_80198F4C.inc"
}
#else
#pragma optimization_level 4
void HSD_ZListInitAllocData(void) {
    HSD_ObjAllocInit(lbl_80465348, 0x48, 4);
}
#endif
#pragma pop

/* Inlined from HSD_JObjFindSkeleton in the original displayfunc.c TU. */
static inline HSD_JObj* displayfuncFindSkeleton(HSD_JObj* jobj)
{
    HSD_JObj* skeleton;

    ((skeleton = jobj) != NULL)
        ? (void) 0
        : __assert(lbl_802746DC, 0x184, lbl_8047D9E8);
    for (; skeleton != NULL; skeleton = skeleton->parent) {
        if (skeleton->flags & (JOBJ_SKELETON | JOBJ_SKELETON_ROOT)) {
            return skeleton;
        }
    }
    return NULL;
}

/* 0x80197A64 | 0x108 */
MtxPtr _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* model, MtxPtr mtx)
{
    HSD_JObj* skeleton;
    Mtx concat;

    if (model->flags & JOBJ_SKELETON_ROOT) {
        return NULL;
    }

    skeleton = displayfuncFindSkeleton(model);
    if (skeleton == NULL) {
        __assert(lbl_802746DC, 0x1D4, lbl_8047D9F0);
    }

    if (skeleton == model) {
        PSMTXInverse((MtxPtr) skeleton->envelopemtx, mtx);
    } else if (skeleton->flags & JOBJ_SKELETON_ROOT) {
        fn_801A9DF0(skeleton->mtx, model->mtx, mtx);
    } else {
        PSMTXConcat(skeleton->mtx, (MtxPtr) skeleton->envelopemtx, concat);
        fn_801A9DF0(concat, model->mtx, mtx);
    }

    return mtx;
}

/* =======================================================================
 *  HSD_JObjMakePositionMtx
 *  Address: 0x80197B6C, Size: 0x104
 *
 *  Build a JObj position matrix, accounting for billboard orientation.
 *
 *  r3 = jobj
 *  r4 = viewMtx (the camera/view matrix)
 *  r5 = positionMtx (output matrix)
 *
 *  Algorithm:
 *    1. Check billboard flags (bits selected by 0xE00 at offset 0x14)
 *    2. If no billboard flags are set, concatenate viewMtx * jobj->mtx
 *       directly into positionMtx.
 *    3. Else:
 *         Concatenate into localMtx and dispatch the billboard subtype:
 *           0x200 -> fn_80198038 (billboard)
 *           0x400 -> fn_80198B20 (vertical billboard)
 *           0x600 -> fn_801985E0 (horizontal billboard)
 *           0x800 -> fn_80197C70 (rotated billboard)
 *         Unknown subtypes panic with the retail billboard diagnostic.
 * ======================================================================= */
#pragma push
#pragma dont_inline on
void HSD_JObjMakePositionMtx(HSD_JObj* jobj, Mtx viewMtx, Mtx positionMtx) {
    Mtx localMtx;

    if (jobj->flags & JOBJ_BILLBOARD_FIELD) {
        PSMTXConcat(viewMtx, jobj->mtx, localMtx);
        switch (jobj->flags & JOBJ_BILLBOARD_FIELD) {
        case JOBJ_BILLBOARD:
            fn_80198038(jobj, localMtx, positionMtx);
            break;
        case JOBJ_VBILLBOARD:
            fn_80198B20(jobj, localMtx, positionMtx);
            break;
        case JOBJ_HBILLBOARD:
            fn_801985E0(jobj, localMtx, positionMtx);
            break;
        case JOBJ_RBILLBOARD:
            fn_80197C70(jobj, localMtx, positionMtx);
            break;
        default:
            HSD_Panic(lbl_802746DC, 0x170, lbl_802746EC);
            break;
        }
    } else {
        PSMTXConcat(viewMtx, jobj->mtx, positionMtx);
    }
}
#pragma pop

void fn_80197C70(HSD_JObj* jobj, Mtx src, Mtx dst)
{
    Mtx rot;
    Mtx scale;
    f32 sx;
    f32 sy;
    f32 sz;

    sx = displayfuncMtxColMag(src, 0);
    sy = displayfuncMtxColMag(src, 1);
    sz = displayfuncMtxColMag(src, 2);
    PSMTXScale(scale, sx, sy, sz);
    PSMTXRotRad(rot, 'z', jobj->rotate_z);
    rot[0][3] = src[0][3];
    rot[1][3] = src[1][3];
    rot[2][3] = src[2][3];
    PSMTXConcat(rot, scale, dst);
}
