/**
 * @file gs_part.c
 * @brief GSpart (model part/joint-subtree accessors)
 *
 * Split from gs_range_800E202C.c (0x800EE150-0x800EE928) - one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 *
 * This candidate covers 0x800EE3BC - 0x800EE6B4 (GSpartGetTransform). The
 * exact head, 0x800EE150 - 0x800EE3BC, is linked as gs_part_exact_800EE150.c
 * and the exact tail from GSpartGetMaterial on as gs_part_exact_800EE6B4.c.
 *
 * GSpartGetTransform's loop locals are declared at the top of the
 * function, each loop's node ahead of its index. MWCC numbers locals in
 * declaration order and colours the higher numbers first, so this order
 * gives retail's registers: node r26, index r27 (reusing the parent walk's
 * offset register), walker r24. With block-scope `i`/`node` the index
 * takes r24 instead. See docs/recon/gs_part_get_transform_wall.md.
 */
#include "dolphin/types.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_jobj.h"

typedef struct GSpart {
    u8 inUse;
    u8 _pad;
    u16 index;
    void *model;
    void *jobj;
} GSpart;

extern void GSlogWrite(const char* format, ...);


typedef struct GSpartVec {
    f32 x;
    f32 y;
    f32 z;
} GSpartVec;

extern HSD_JObj* lbl_804018B0[16]; /* parent chain scratch */
extern char lbl_80270F78[];
extern void GSmodelForceAnimTransformUpdate(void*);
extern u8 GSmodelIsBlending(void*);
extern void set__5GSvecFfff(GSpartVec*, f32, f32, f32);
extern void GSvecAdd(GSpartVec*, const GSpartVec*, const GSpartVec*);
extern void fn_800E0108(GSpartVec*, const GSpartVec*, const GSpartVec*);
extern void GSvecCopy(GSpartVec*, const GSpartVec*);
extern void __assert(const char* file, u32 line, const char* condition);
extern const char lbl_8047CCA0[] __attribute__((section(".sdata2"))); /* HSD jobj.h assert file */
extern const char lbl_8047CCA8[] __attribute__((section(".sdata2"))); /* "jobj" */
extern const f32 lbl_8047CCB0; /* 0.0f */
extern const f32 lbl_8047CCB4; /* 1.0f */

/*
 * HSD jobj.h accessors as retail expands them (their assert lines are
 * 605, 748/762/776 and 899/911/923). They are spelled out here because the
 * header versions build their assert strings from __FILE__.
 */
static inline BOOL partJObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 605, lbl_8047CCA8);
    }
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) && (jobj->flags & JOBJ_MTX_DIRTY)) {
        result = TRUE;
    }
    return result;
}

static inline void partJObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !partJObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub(jobj);
}

static inline f32 partJObjGetRotationX(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 748, lbl_8047CCA8);
    }
    return jobj->rotate_x;
}

static inline f32 partJObjGetRotationY(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 762, lbl_8047CCA8);
    }
    return jobj->rotate_y;
}

static inline f32 partJObjGetRotationZ(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 776, lbl_8047CCA8);
    }
    return jobj->rotate_z;
}

static inline f32 partJObjGetScaleX(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 899, lbl_8047CCA8);
    }
    return jobj->scale_x;
}

static inline f32 partJObjGetScaleY(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 911, lbl_8047CCA8);
    }
    return jobj->scale_y;
}

static inline f32 partJObjGetScaleZ(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 923, lbl_8047CCA8);
    }
    return jobj->scale_z;
}

void GSpartGetTransform(GSpart* part, GSpartVec* positionOut,
                        GSpartVec* rotationOut, GSpartVec* scaleOut)
{
    HSD_JObj* rotNode;
    u32 rotIndex;
    HSD_JObj* scaleNode;
    u32 scaleIndex;
    HSD_JObj* jobj = part->jobj;
    GSpartVec position;
    GSpartVec rotation;
    GSpartVec rotate;
    GSpartVec scale;
    GSpartVec scaling;
    u32 count = 0;

    GSmodelForceAnimTransformUpdate(part->model);
    if (!GSmodelIsBlending(part->model)) {
        partJObjSetupMatrix(jobj);
        set__5GSvecFfff(&position, jobj->mtx[0][3], jobj->mtx[1][3], jobj->mtx[2][3]);
    } else if (jobj->flags & JOBJ_JOINT) {
        GSvecCopy(&position, (GSpartVec*)&jobj->translate_x);
    } else {
        set__5GSvecFfff(&position, jobj->mtx[0][3], jobj->mtx[1][3], jobj->mtx[2][3]);
    }

    if (rotationOut != NULL || scaleOut != NULL) {
        while (jobj != NULL) {
            lbl_804018B0[count++] = jobj;
            jobj = HSD_JObjGetParent(jobj);
            if (count >= 16) {
                GSlogWrite(lbl_80270F78);
                jobj = NULL;
            }
        }

        if (rotationOut != NULL) {
            rotIndex = count;
            set__5GSvecFfff(&rotation, lbl_8047CCB0, lbl_8047CCB0, lbl_8047CCB0);
            for (; rotIndex != 0; rotIndex--) {
                rotNode = lbl_804018B0[rotIndex - 1];

                rotate.x = partJObjGetRotationX(rotNode);
                rotate.y = partJObjGetRotationY(rotNode);
                rotate.z = partJObjGetRotationZ(rotNode);
                GSvecAdd(&rotation, &rotation, &rotate);
            }
        }

        if (scaleOut != NULL) {
            scaleIndex = count;
            set__5GSvecFfff(&scale, lbl_8047CCB4, lbl_8047CCB4, lbl_8047CCB4);
            for (; scaleIndex != 0; scaleIndex--) {
                scaleNode = lbl_804018B0[scaleIndex - 1];

                scaling.x = partJObjGetScaleX(scaleNode);
                scaling.y = partJObjGetScaleY(scaleNode);
                scaling.z = partJObjGetScaleZ(scaleNode);
                fn_800E0108(&scale, &scale, &scaling);
            }
        }
    }

    if (positionOut != NULL) {
        GSvecCopy(positionOut, &position);
    }
    if (rotationOut != NULL) {
        GSvecCopy(rotationOut, &rotation);
    }
    if (scaleOut != NULL) {
        GSvecCopy(scaleOut, &scale);
    }
}
