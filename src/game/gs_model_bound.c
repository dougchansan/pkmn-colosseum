/**
 * @file gs_model_bound.c
 * @brief GSmodel bound-recalc + anim-blend interpolation (XD anim.c sub-range)
 *
 * Split from gs_range_800E202C.c (0x800EB268-0x800EBEEC) - one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

typedef struct GSbound {
    u8 _pad[0x34];
} GSbound;

typedef struct HSDJObj {
    u8 _pad0[0x14];
    u32 flags;
} HSDJObj;

typedef struct GSmodelResource {
    HSD_Joint* joint;
} GSmodelResource;

typedef struct GSmodel {
    u32 flags;
    GSmodelResource* resource;
    HSDJObj* renderJObj;
    HSD_JObj* blendJObj;
    HSD_JObj* blendJObjA;
    HSD_JObj* blendJObjB;
    u8 _pad18[0x34];
    GSbound bound;
    u8 _pad80[0xC4];
    void* linkedGSparticleBank;
} GSmodel;

typedef void (*GSModelPObjDisp)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                               f32 smtx[3][4], void* arg);

extern const f32 lbl_8047CC28;
extern const char lbl_8047CC2C[7];
extern const char lbl_8047CC34[5];

extern void __assert(const char* file, u32 line, const char* condition);
extern HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint);
extern void GSmodelSetAnimIndex(GSmodel* model, u32 index);
extern void fn_80191460(GSbound* bound);
extern void GSmodelSetAnimFrame(GSmodel* model, f32 frame);
extern void fn_800EC134(GSmodel* model);
extern void fn_8019D9DC(HSDJObj* jobj);
extern void GSmodelParse(GSmodel* model, BOOL is_visible, GSModelPObjDisp disp,
                         void* arg);
extern void _modelBoundPObj__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv(
    HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4], f32 smtx[3][4], void* arg);

#if !defined(GS_MODEL_BOUND_800EB464_SUFFIX_ACTIVE)
static inline BOOL HSD_JObjMtxIsDirty(HSDJObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(lbl_8047CC2C, 0x25D, lbl_8047CC34);
    }
    result = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

void fn_800EB268(GSmodel* model, u32 anim_index)
{
    void* particle_bank;
    HSDJObj* jobj;

    GSmodelSetAnimIndex(model, anim_index);
    fn_80191460(&model->bound);
    particle_bank = model->linkedGSparticleBank;
    model->linkedGSparticleBank = NULL;
    if (model->flags & 4) {
        GSmodelSetAnimFrame(model, lbl_8047CC28);
        fn_800EC134(model);
        jobj = model->renderJObj;
        if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
            fn_8019D9DC(jobj);
        }
    }
    GSmodelParse(
        model, TRUE,
        _modelBoundPObj__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv, model);
    model->linkedGSparticleBank = particle_bank;
}

void GSmodelRecalculateBound(GSmodel* model)
{
    HSDJObj* jobj;
    void* particle_bank;

    fn_80191460(&model->bound);
    particle_bank = model->linkedGSparticleBank;
    model->linkedGSparticleBank = NULL;
    if (model->flags & 4) {
        GSmodelSetAnimFrame(model, lbl_8047CC28);
        fn_800EC134(model);
        jobj = model->renderJObj;
        if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
            fn_8019D9DC(jobj);
        }
    }
    GSmodelParse(
        model, TRUE,
        _modelBoundPObj__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv, model);
    model->linkedGSparticleBank = particle_bank;
}
#endif

#if defined(GS_MODEL_BOUND_800EB464_SUFFIX_ACTIVE)
void _modelBoundVertex__FUlPvPv(u32 flags, void* vertex, void* arg)
{
    typedef f32 GSmtx[3][4];
    typedef struct GSvec {
        f32 x;
        f32 y;
        f32 z;
    } GSvec;
    typedef struct ModelBoundArgs {
        GSmodel* model;
        GSmtx* matrix;
        GSmtx* matrices;
        u32 flags;
    } ModelBoundArgs;
    extern u32 lbl_8047AB98;
    extern void set__5GSvecFfff(GSvec* vec, f32 x, f32 y, f32 z);
    extern void GSvecTransform(GSvec* dst, GSmtx* matrix, GSvec* src);
    extern GSbound* GSmodelGetBound(GSmodel* model);
    extern void fn_80191358(GSbound* bound, f32 x, f32 y, f32 z);
    ModelBoundArgs* args = arg;

    if (flags & 1) {
        lbl_8047AB98 = *(u8*)vertex / 3;
    }
    if (flags & 2) {
        GSmtx* matrix;
        GSvec position;

        if (args->flags & 1) {
            matrix = &args->matrices[lbl_8047AB98];
        } else {
            matrix = args->matrix;
        }
        set__5GSvecFfff(&position, ((f32*)vertex)[0], ((f32*)vertex)[1],
                        ((f32*)vertex)[2]);
        GSvecTransform(&position, matrix, &position);
        fn_80191358(GSmodelGetBound(args->model), position.x, position.y,
                    position.z);
    }
}

void _modelBoundBeginSurface__F13GSgfxPrimTypeUsUlPv(s32 prim, u16 count, u32 attr, void *ctx) {
    (void)prim;
    (void)count;
    *(u32 *)((u8 *)ctx + 0xC) = attr;
}

void GSmodelEnableAnimBlend(GSmodel* model)
{
    extern void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(HSD_JObj* jobj);

    if (model->blendJObj == NULL && !(model->flags & 0x20000)) {
        model->blendJObj = HSD_JObjLoadJoint(model->resource->joint);
        model->blendJObjA = HSD_JObjLoadJoint(model->resource->joint);
        model->blendJObjB = HSD_JObjLoadJoint(model->resource->joint);
        _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(model->blendJObj);
    }
}
#endif
