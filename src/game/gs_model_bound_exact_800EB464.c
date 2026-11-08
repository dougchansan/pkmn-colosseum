/**
 * @file gs_model_bound_exact_800EB464.c
 * @brief Strict model-bound callbacks and animation-blend setup,
 *        0x800EB464 - 0x800EB5A0.
 */

#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

typedef f32 GSmtx[3][4];

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

typedef struct GSbound {
    u8 _pad[0x34];
} GSbound;

typedef struct GSmodelResource {
    HSD_Joint* joint;
} GSmodelResource;

typedef struct GSmodel {
    u32 flags;
    GSmodelResource* resource;
    void* renderJObj;
    HSD_JObj* blendJObj;
    HSD_JObj* blendJObjA;
    HSD_JObj* blendJObjB;
    u8 _pad18[0x34];
    GSbound bound;
    u8 _pad80[0xC4];
    void* linkedGSparticleBank;
} GSmodel;

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
extern void GSboundAddPoint(GSbound* bound, f32 x, f32 y, f32 z);
extern HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint);
extern void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(HSD_JObj* jobj);

void _modelBoundVertex__FUlPvPv(u32 flags, void* vertex, void* arg)
{
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
        GSboundAddPoint(GSmodelGetBound(args->model), position.x, position.y,
                    position.z);
    }
}

void _modelBoundBeginSurface__F13GSgfxPrimTypeUsUlPv(
    s32 prim, u16 count, u32 attr, void* arg)
{
    ModelBoundArgs* args = arg;

    args->flags = attr;
}

void GSmodelEnableAnimBlend(GSmodel* model)
{
    if (model->blendJObj == NULL && !(model->flags & 0x20000)) {
        model->blendJObj = HSD_JObjLoadJoint(model->resource->joint);
        model->blendJObjA = HSD_JObjLoadJoint(model->resource->joint);
        model->blendJObjB = HSD_JObjLoadJoint(model->resource->joint);
        _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(model->blendJObj);
    }
}
