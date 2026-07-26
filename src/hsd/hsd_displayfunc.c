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

/* 0x80198038 | 0x5A8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
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

#if 0
asm void fn_80198038(void* dobj, void* mtx, void* renderState) {
#include "src/hsd/hsd_displayfunc_fn_80198038.inc"
}
#else
#pragma optimization_level 4
void fn_80198038(void* dobj, void* mtx, void* renderState)
{
    DisplayFuncVec col0;
    DisplayFuncVec col1;
    DisplayFuncVec col1Work;
    DisplayFuncVec col2;
    DisplayFuncVec trans;
    DisplayFuncVec basis;
    DisplayFuncVec cross;
    f32 col0Len;
    f32 col1Len;
    f32 col2Len;
    f32 crossLen;
    f32 denom;
    f32 scale;
    f32 horizLen;

    DISPLAYFUNC_LOAD_COLUMN(col0, mtx, 0);
    DISPLAYFUNC_LOAD_COLUMN(col1, mtx, 1);
    DISPLAYFUNC_LOAD_COLUMN(col2, mtx, 2);
    DISPLAYFUNC_LOAD_TRANSLATION(trans, mtx);

    col0Len = PSVECMag(&col0);
    col2Len = PSVECMag(&col2);
    col1Len = PSVECMag(&col1);
    col1Work = col1;

    if (((HSD_DObj*) dobj)->flags & DISPLAYFUNC_FLAG_2000) {
        scale = -1.0f / (lbl_80478ACC + PSVECMag(&trans));
        PSVECScale(&trans, &basis, scale);
    } else {
        basis = lbl_802746D0;
    }

    scale = 1.0f / (lbl_80478ACC + col1Len);
    PSVECScale(&col1Work, &col1Work, scale);
    PSVECCrossProduct(&col1Work, &basis, &cross);
    crossLen = PSVECMag(&cross);

    if (crossLen >= lbl_80478ACC) {
        col0Len /= crossLen;
        PSVECCrossProduct(&basis, &cross, &col1Work);
        col1Len /= lbl_80478ACC + PSVECMag(&col1Work);
    } else {
        DisplayFuncVec flat;

        DISPLAYFUNC_SET_VEC(flat, basis.x, 0.0f, basis.z);
        horizLen = PSVECMag(&flat);
        denom = lbl_80478ACC + horizLen;
        scale = -basis.y / denom;
        DISPLAYFUNC_SET_VEC(col1Work, basis.x * scale, denom, basis.z * scale);
        PSVECCrossProduct(&col1Work, &basis, &cross);
        col0Len /= lbl_80478ACC + PSVECMag(&cross);
    }

    DISPLAYFUNC_STORE_COLUMN(renderState, 0, cross, col0Len);
    DISPLAYFUNC_STORE_COLUMN(renderState, 1, col1Work, col1Len);
    DISPLAYFUNC_STORE_COLUMN(renderState, 2, basis, col2Len);
    DISPLAYFUNC_STORE_TRANSLATION(renderState, trans);
}
#endif
#pragma pop

/* 0x801985E0 | 0x540 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void PSMTXCopy(void*, void*);
extern void PSVECNormalize(void*, void*);
#if 0
asm void fn_801985E0(void* dobj, void* mtx, void* renderState) {
#include "src/hsd/hsd_displayfunc_fn_801985E0.inc"
}
#else
#pragma optimization_level 4
void fn_801985E0(void* dobj, void* mtx, void* renderState)
{
    DisplayFuncVec col0;
    DisplayFuncVec col0Unit;
    DisplayFuncVec col1;
    DisplayFuncVec col2;
    DisplayFuncVec trans;
    DisplayFuncVec basis;
    DisplayFuncVec cross;
    f32 col1Len;
    f32 col2Len;
    f32 crossLen;
    f32 basisLen;
    f32 denom;
    f32 scale;

    DISPLAYFUNC_LOAD_COLUMN(col0, mtx, 0);
    DISPLAYFUNC_LOAD_COLUMN(col1, mtx, 1);
    DISPLAYFUNC_LOAD_COLUMN(col2, mtx, 2);
    DISPLAYFUNC_LOAD_TRANSLATION(trans, mtx);

    scale = 1.0f / (lbl_80478ACC + PSVECMag(&col0));
    PSVECScale(&col0, &col0Unit, scale);
    col1Len = PSVECMag(&col1);
    col2Len = PSVECMag(&col2);

    if (((HSD_DObj*) dobj)->flags & DISPLAYFUNC_FLAG_2000) {
        DisplayFuncVec flat;

        DISPLAYFUNC_SET_VEC(flat, trans.x, 0.0f, trans.z);
        basisLen = PSVECMag(&flat);
        denom = lbl_80478ACC + basisLen;
        scale = -trans.y / denom;
        DISPLAYFUNC_SET_VEC(basis, trans.x * scale, denom, trans.z * scale);
        PSVECNormalize(&basis, &basis);
    } else {
        DISPLAYFUNC_SET_VEC(basis, 0.0f, 1.0f, 0.0f);
    }

    PSVECCrossProduct(&col0Unit, &basis, &cross);
    crossLen = PSVECMag(&cross);

    if (crossLen >= lbl_80478ACC) {
        col2Len /= crossLen;
        PSVECCrossProduct(&cross, &col0Unit, &basis);
        col1Len /= lbl_80478ACC + PSVECMag(&basis);

        DISPLAYFUNC_STORE_COLUMN(renderState, 0, col0, 1.0f);
        DISPLAYFUNC_STORE_COLUMN(renderState, 1, basis, col1Len);
        DISPLAYFUNC_STORE_COLUMN(renderState, 2, cross, col2Len);
        DISPLAYFUNC_STORE_TRANSLATION(renderState, trans);
    } else {
        PSMTXCopy(mtx, renderState);
    }
}
#endif
#pragma pop

/* 0x80198B20 | 0x42C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80198B20(void* dobj, void* mtx, void* renderState) {
#include "src/hsd/hsd_displayfunc_fn_80198B20.inc"
}
#else
#pragma optimization_level 4
void fn_80198B20(void* dobj, void* mtx, void* renderState)
{
    DisplayFuncVec col0;
    DisplayFuncVec col1;
    DisplayFuncVec col1Unit;
    DisplayFuncVec col2;
    DisplayFuncVec trans;
    DisplayFuncVec basis;
    DisplayFuncVec cross;
    f32 col0Len;
    f32 col2Len;
    f32 crossLen;
    f32 basisLen;
    f32 scale;

    DISPLAYFUNC_LOAD_COLUMN(col1, mtx, 1);
    DISPLAYFUNC_LOAD_COLUMN(col0, mtx, 0);
    DISPLAYFUNC_LOAD_COLUMN(col2, mtx, 2);
    DISPLAYFUNC_LOAD_TRANSLATION(trans, mtx);

    scale = 1.0f / (lbl_80478ACC + PSVECMag(&col1));
    PSVECScale(&col1, &col1Unit, scale);
    col0Len = PSVECMag(&col0);
    col2Len = PSVECMag(&col2);

    if (((HSD_DObj*) dobj)->flags & DISPLAYFUNC_FLAG_2000) {
        scale = -1.0f / (lbl_80478ACC + PSVECMag(&trans));
        PSVECScale(&trans, &basis, scale);
    } else {
        basis = lbl_802746D0;
    }

    PSVECCrossProduct(&col1Unit, &basis, &cross);
    crossLen = PSVECMag(&cross);

    if (crossLen >= lbl_80478ACC) {
        col0Len /= crossLen;
        PSVECCrossProduct(&cross, &col1Unit, &basis);
        basisLen = PSVECMag(&basis);
        col2Len /= lbl_80478ACC + basisLen;

        DISPLAYFUNC_STORE_COLUMN(renderState, 0, cross, col0Len);
        DISPLAYFUNC_STORE_COLUMN(renderState, 1, col1, 1.0f);
        DISPLAYFUNC_STORE_COLUMN(renderState, 2, basis, col2Len);
        DISPLAYFUNC_STORE_TRANSLATION(renderState, trans);
    } else {
        PSMTXCopy(mtx, renderState);
    }
}
#endif
#pragma pop

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
