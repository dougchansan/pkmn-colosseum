/**
 * @file gs_model_anim_exact_800EC4D0.c
 * @brief Exact GSmodel animation accessor/apply island, 0x800EC4D0 - 0x800ED4D4.
 *
 * Mechanical address partition of the recovered GSmodel animation source.
 */
#include "dolphin/types.h"
#include "game/gs_model_anim.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_jobj.h"

#define MODEL_FLAG_IN_USE              0x00000001
#define MODEL_FLAG_HAS_ANIM            0x00000004
#define MODEL_FLAG_HAS_TEX_ANIM        0x00000008
#define MODEL_FLAG_HAS_SHAPE_ANIM      0x00000010
#define MODEL_FLAG_ANIMATING           0x00000020
#define MODEL_FLAG_TEX_ANIMATING       0x00000040
#define MODEL_FLAG_BLENDING            0x00000080
#define MODEL_FLAG_PAUSED              0x00000100
#define MODEL_FLAG_60FPS_ANIM          0x00000800
#define MODEL_FLAG_SKIP_APPLY          0x00001000
#define MODEL_FLAG_LINK_TEX_TO_ANIM    0x00002000
#define MODEL_FLAG_ANIM_ENDED          0x00004000
#define MODEL_FLAG_TEX_ANIM_ENDED      0x00008000
#define MODEL_FLAG_FORCE_UPDATE        0x00010000
#define MODEL_FLAG_USE_JOBJ_CHILD      0x00020000
#define MODEL_FLAG_ATTACHMENT_ONCE     0x00040000
#define MODEL_FLAG_UPDATE_ATTACHMENT   0x00080000

typedef struct GSmodel GSmodel;
typedef struct GSpart GSpart;
typedef void (*GSmodelAnimEndedCallback)(GSmodel*, void*);

typedef struct GSmodelResource {
    void* header;
    void** anims;
    void** tex_anims;
    void** shape_anims;
} GSmodelResource;

typedef struct GSmodelPartAnimMix {
    s32 type;
    s32 part_index;
    f32* value;
} GSmodelPartAnimMix;

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

typedef struct GSquat {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} GSquat;

struct GSmodel {
    u32 flags;
    GSmodelResource* resource;
    HSD_JObj* jobj;
    u8 pad0C[0x10 - 0x0C];
    HSD_JObj* blend_jobj_a;
    HSD_JObj* blend_jobj_b;
    u8 pad18[0x84 - 0x18];
    u32 anim_count;
    u32 tex_anim_count;
    u32 anim_type;
    u32 anim_index;
    f32 anim_rate;
    f32 anim_frame;
    f32 anim_req_frame;
    f32 anim_end_frame;
    u32 tex_anim_type;
    u32 tex_anim_index;
    f32 tex_anim_rate;
    f32 tex_anim_frame;
    f32 tex_anim_req_frame;
    f32 tex_anim_end_frame;
    u32 blend_anim_index_a;
    u32 blend_anim_index_b;
    f32 blend_anim_frame_a;
    f32 blend_anim_frame_b;
    f32 blend_anim_end_frame_a;
    f32 blend_anim_end_frame_b;
    f32 blend_factor;
    f32 blend_frame_scale;
    GSmodelAnimEndedCallback anim_ended_callback;
    void* anim_ended_callback_arg;
    GSmodelPartAnimMix part_anim_mixes[4];
    union {
        struct {
            s32 force_fractional_frames;
            void* fractional_frame_data;
        } animation;
        struct {
            s32 attachment_type;
            GSmodel* attachment_model;
            s32 attachment_part_index;
            GSvec attachment_position;
            GSvec attachment_rotation;
            GSvec attachment_scale;
        } attachment;
    } tail;
    u8 pad144[0x170 - 0x144];
};

#define force_fractional_frames tail.animation.force_fractional_frames
#define fractional_frame_data tail.animation.fractional_frame_data
#define attachment_type tail.attachment.attachment_type
#define attachment_model tail.attachment.attachment_model
#define attachment_part_index tail.attachment.attachment_part_index
#define attachment_position tail.attachment.attachment_position
#define attachment_rotation tail.attachment.attachment_rotation
#define attachment_scale tail.attachment.attachment_scale

struct GSpart {
    u8 pad00[8];
    HSD_JObj* jobj;
};

extern GSmodel* lbl_8047AB74;
extern u32 lbl_8047AB78;

extern const f32 lbl_8047CC58;
extern const f32 lbl_8047CC5C;
extern const f32 lbl_8047CC60;
extern const f32 lbl_8047CC64;
extern const f32 lbl_8047CC78; /* defined SDATA2 in src/game/data/sdata2_8047CBE0.c = 0.0001f */
extern char lbl_8047CC90[] __attribute__((section(".sdata2")));
extern char lbl_8047CC98[] __attribute__((section(".sdata2")));
extern char lbl_8047CC68[] __attribute__((section(".sdata2")));
extern char lbl_8047CC70[] __attribute__((section(".sdata2")));
extern char lbl_80270EE8[];

void HSD_ForeachAnim(void* obj, u32 type, u32 mask, void* func, u32 arg_type, ...);
void fn_801A32A0(HSD_JObj* jobj, u32 flags, f32 frame);
void fn_801A2B5C(HSD_JObj* jobj, void* anim, void* mat_anim,
                 void* shape_anim);
void modelCalculateBlendModel__FP8_GSmodelf(GSmodel* model);
f32 fn_800ED8C4(u32 type, u32 fractional_frames, f32 frame,
                f32 requested_frame, f32 end_frame, f32 rate);
s32 fn_800D37CC(void);
void _modelGetAObjFunc__FP9_HSD_AObjPv(HSD_AObj* aobj, HSD_AObj** out);
void _modelResetPartAnimMixes__FP8_GSmodel(GSmodel* model);
void _modelSetLoopFlag__FP9_HSD_AObjUl(HSD_AObj* aobj, u32 enable);
void _modelGetEndFrame(HSD_AObj* aobj, f32* end_frame);
void fn_800ED6E4(GSmodel* model, u8 tex_anim);
void fn_800ED7E4(GSmodel* model, u8 tex_anim, f32 delta);
GSpart* GSmodelGetPart(GSmodel* model, s32 index);
void GSpartGetTransform(GSpart* part, GSvec* position, GSvec* rotation,
                        GSvec* scale);
void GSpartFree(GSpart* part);
void fn_800E06EC(GSquat* quaternion, GSvec* rotation);
void GSvecTransformQuat(GSvec* dst, GSquat* quaternion, GSvec* src);
void GSvecAdd(GSvec* dst, GSvec* lhs, GSvec* rhs);
void fn_800E0108(GSvec* dst, GSvec* lhs, GSvec* rhs);
void modelSetPos(GSmodel* model, GSvec* position);
void modelSetRot(GSmodel* model, GSvec* rotation);
void modelSetScl(GSmodel* model, GSvec* scale);
void fn_8019D620(HSD_JObj* jobj);
void fn_8019D9DC(HSD_JObj* jobj);

typedef struct GSmodelAnimEndedInfo {
    u32 event_flags;
    u32 anim_index;
    void* arg;
} GSmodelAnimEndedInfo;

#define MODEL_RESET_PART_ANIM_MIX_DIRTY(jobj_)                                \
    do {                                                                      \
        HSD_JObj* dirty_jobj = (jobj_);                                        \
        s32 dirty;                                                            \
        if (!(dirty_jobj->flags & JOBJ_MTX_INDEP_SRT)) {                      \
            if (dirty_jobj != NULL) {                                         \
                if (dirty_jobj == NULL) {                                     \
                    __assert(lbl_8047CC68, 0x25D, lbl_8047CC70);              \
                }                                                             \
                dirty = 0;                                                    \
                if (!(dirty_jobj->flags & JOBJ_USER_DEF_MTX)) {               \
                    if (dirty_jobj->flags & JOBJ_MTX_DIRTY) {                 \
                        dirty = 1;                                            \
                    }                                                         \
                }                                                             \
                if (dirty == 0) {                                             \
                    fn_8019D620(dirty_jobj);                                  \
                }                                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

#define MODEL_RESET_PART_ANIM_MIX_SET_ROT(jobj_, value_, null_line_,          \
                                          spline_line_, field_)               \
    do {                                                                      \
        HSD_JObj* set_jobj = (jobj_);                                          \
        f32 set_value = (value_);                                             \
        if (set_jobj == NULL) {                                               \
            __assert(lbl_8047CC68, (null_line_), lbl_8047CC70);               \
        }                                                                     \
        if (set_jobj->flags & JOBJ_USE_QUATERNION) {                          \
            __assert(lbl_8047CC68, (spline_line_), lbl_80270EE8);             \
        }                                                                     \
        set_jobj->field_ = set_value;                                         \
        MODEL_RESET_PART_ANIM_MIX_DIRTY(set_jobj);                            \
    } while (0)

#define MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj_, value_, null_line_, field_)  \
    do {                                                                      \
        HSD_JObj* add_jobj = (jobj_);                                          \
        f32 add_value = (value_);                                             \
        if (add_jobj == NULL) {                                               \
            __assert(lbl_8047CC68, (null_line_), lbl_8047CC70);               \
        }                                                                     \
        add_jobj->field_ += add_value;                                        \
        MODEL_RESET_PART_ANIM_MIX_DIRTY(add_jobj);                            \
    } while (0)

#undef MODEL_RESET_PART_ANIM_MIX_ADD_ROT
#undef MODEL_RESET_PART_ANIM_MIX_SET_ROT
#undef MODEL_RESET_PART_ANIM_MIX_DIRTY

/* sModelJObjCount / sModelJObjLastIndex are .sbss globals owned by the
 * gs_model_anim split (config/GC6E01/symbols.txt). fn_800EE0E8 uses them
 * as a shared counter/scratch during a JObj traversal driven by fn_800EE20C. */
extern s32 sModelJObjCount;
extern s32 sModelJObjLastIndex;
typedef void (*GSmodelAObjApplyFunc)(HSD_AObj* aobj, void* arg);
void fn_801A3918(HSD_JObj* jobj, GSmodelAObjApplyFunc func, u32 arg);
void fn_800EE20C(HSD_AObj* aobj, void* arg);

#pragma force_active on
void GSmodelGetFrameCount(GSmodel* model, f32* anim_frames, f32* tex_frames)
{
    if (!(model->flags & MODEL_FLAG_BLENDING)) {
        if (anim_frames != NULL) {
            *anim_frames = lbl_8047CC60 + model->anim_end_frame;
        }
        if (tex_frames != NULL) {
            *tex_frames = lbl_8047CC64;
        }
    } else {
        if (anim_frames != NULL) {
            *anim_frames = lbl_8047CC60 + model->blend_anim_end_frame_a;
        }
        if (tex_frames != NULL) {
            *tex_frames = lbl_8047CC60 + model->blend_anim_end_frame_b;
        }
    }
}
f32 GSmodelGetAnimFrame(GSmodel* model)
{
    u32 flags = model->flags;

    if (!(flags & MODEL_FLAG_HAS_ANIM)) {
        return lbl_8047CC5C;
    }
    if (flags & MODEL_FLAG_BLENDING) {
        return model->blend_anim_frame_b;
    }
    return model->anim_frame;
}
u32 GSmodelGetAnimType(GSmodel* model)
{
    return model->anim_type;
}
f32 GSmodelGetAnimRate(GSmodel* model)
{
    return model->anim_rate;
}
void GSmodelGetAnimIndex(GSmodel* model, u32* index_a, u32* index_b)
{
    if (!(model->flags & MODEL_FLAG_BLENDING)) {
        *index_a = model->anim_index;
        *index_b = -1;
    } else {
        *index_a = model->blend_anim_index_a;
        *index_b = model->blend_anim_index_b;
    }
}
u32 GSmodelIsBlending(GSmodel* model)
{
    return (model->flags >> 7) & 1;
}
void GSmodelSetBlendFactor(GSmodel* model, f32 factor)
{
    u32 flags = model->flags;

    if (flags & MODEL_FLAG_HAS_ANIM) {
        if (flags & MODEL_FLAG_BLENDING) {
            goto clamp_factor;
        }
    }
    return;

clamp_factor:
    if (factor < lbl_8047CC5C) {
        factor = lbl_8047CC5C;
    } else if (factor > lbl_8047CC60) {
        factor = lbl_8047CC60;
    }
    model->blend_factor = factor;
}
void GSmodelSetAnimBlend(GSmodel* model, u32 index_a, u32 index_b)
{
    HSD_JObj* jobj;
    u32 flags;
    u32 type;

    flags = model->flags;
    if (!(flags & MODEL_FLAG_HAS_ANIM)) {
        return;
    }
    if (index_a > model->anim_count || index_b > model->anim_count) {
        return;
    }

    model->flags = flags | MODEL_FLAG_BLENDING;
    model->blend_anim_index_a = index_a;
    model->blend_anim_index_b = index_b;
    fn_801A2B5C(model->blend_jobj_a,
                model->resource->anims[model->blend_anim_index_a], NULL,
                NULL);
    fn_801A2B5C(model->blend_jobj_b,
                model->resource->anims[model->blend_anim_index_b], NULL,
                NULL);

    model->blend_factor = lbl_8047CC5C;
    model->blend_anim_frame_a = lbl_8047CC5C;
    model->blend_anim_frame_b = lbl_8047CC5C;
    fn_801A32A0(model->blend_jobj_a, 0x1CB, model->blend_anim_frame_a);
    fn_801A32A0(model->blend_jobj_b, 0x1CB, model->blend_anim_frame_b);
    HSD_JObjAnimAll(model->blend_jobj_a);
    HSD_JObjAnimAll(model->blend_jobj_b);

    model->blend_anim_end_frame_a = lbl_8047CC5C;
    HSD_ForeachAnim(model->blend_jobj_a, 6, 0x9B2F,
                    (void*)_modelGetEndFrame, 2,
                    &model->blend_anim_end_frame_a);
    model->blend_anim_end_frame_b = lbl_8047CC5C;
    HSD_ForeachAnim(model->blend_jobj_b, 6, 0x9B2F,
                    (void*)_modelGetEndFrame, 2,
                    &model->blend_anim_end_frame_b);
    model->blend_frame_scale =
        (lbl_8047CC60 + model->blend_anim_end_frame_a) /
        (lbl_8047CC60 + model->blend_anim_end_frame_b);

    flags = model->flags;
    if (flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        jobj = model->jobj;
        if ((flags & MODEL_FLAG_HAS_TEX_ANIM) &&
            index_b < model->tex_anim_count) {
            if (index_b != model->tex_anim_index) {
                if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                    jobj = jobj->child;
                }
                model->tex_anim_index = index_b;
                fn_801A2B5C(
                    jobj, NULL,
                    model->resource->tex_anims[model->tex_anim_index], NULL);
                model->tex_anim_end_frame = lbl_8047CC5C;
                HSD_ForeachAnim(jobj, 6, 0x64DB,
                                (void*)_modelGetEndFrame, 2,
                                &model->tex_anim_end_frame);
            }

            flags = model->flags;
            jobj = model->jobj;
            if (flags & MODEL_FLAG_HAS_TEX_ANIM) {
                if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                    jobj = jobj->child;
                }
                model->tex_anim_frame = lbl_8047CC5C;
                model->tex_anim_req_frame = lbl_8047CC5C;
                fn_801A32A0(jobj, 0x634, model->tex_anim_req_frame);
                model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
            }

            type = model->tex_anim_type;
            flags = model->flags;
            jobj = model->jobj;
            if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                jobj = jobj->child;
            }
            model->tex_anim_type = type;
            switch (model->tex_anim_type) {
            case 0:
                HSD_ForeachAnim(
                    jobj, 6, 0x64DB,
                    (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
                break;
            case 1:
                HSD_ForeachAnim(
                    jobj, 6, 0x64DB,
                    (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
                break;
            }
        }
    }

    model->flags = model->flags & ~MODEL_FLAG_SKIP_APPLY;
}
void GSmodelSetBlendAnimFrameForce(GSmodel* model, f32 frame_a, f32 frame_b)
{
    model->blend_frame_scale = lbl_8047CC5C;
    model->blend_anim_frame_a = frame_a;
    model->blend_anim_frame_b = frame_b;
}
void GSmodelAllUnpauseAnimation(void)
{
    u32 i;

    for (i = 0; i < lbl_8047AB78; i++) {
        if (lbl_8047AB74[i].flags & MODEL_FLAG_IN_USE) {
            lbl_8047AB74[i].flags &= ~(u32)MODEL_FLAG_PAUSED;
        }
    }
}
void GSmodelAllPauseAnimation(void)
{
    u32 i;

    for (i = 0; i < lbl_8047AB78; i++) {
        if (lbl_8047AB74[i].flags & MODEL_FLAG_IN_USE) {
            lbl_8047AB74[i].flags |= MODEL_FLAG_PAUSED;
        }
    }
}
u32 GSmodelHasAnimationEnded(GSmodel* model)
{
    return (model->flags >> 14) & 1;
}
u32 GSmodelIsAnimating(GSmodel* model)
{
    return (model->flags >> 5) & 1;
}
void GSmodelStopAnimation(GSmodel* model)
{
    model->flags = model->flags & ~MODEL_FLAG_ANIMATING;

    if (model->flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        model->flags = model->flags & ~MODEL_FLAG_TEX_ANIMATING;
    }
}
void GSmodelStartAnimation(GSmodel* model)
{
    u32 flags = model->flags;

    if (!(flags & MODEL_FLAG_HAS_ANIM)) {
        return;
    }

    model->flags = flags | MODEL_FLAG_ANIMATING;
    model->flags = model->flags & ~MODEL_FLAG_ANIM_ENDED;

    flags = model->flags;
    if (!(flags & MODEL_FLAG_LINK_TEX_TO_ANIM)) {
        return;
    }
    if (!(flags & MODEL_FLAG_HAS_TEX_ANIM)) {
        return;
    }

    model->flags = flags | MODEL_FLAG_TEX_ANIMATING;
    model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
}
void GSmodelSetAnimRate(GSmodel* model, f32 rate)
{
    if (model->flags & MODEL_FLAG_HAS_ANIM) {
        model->anim_rate = rate;
        if (fn_800D37CC() == 50) {
            model->anim_rate *= lbl_8047CC58;
        }

        if ((model->flags & MODEL_FLAG_LINK_TEX_TO_ANIM) &&
            (model->flags & MODEL_FLAG_HAS_TEX_ANIM)) {
            model->tex_anim_rate = rate;
            if (fn_800D37CC() == 50) {
                model->tex_anim_rate *= lbl_8047CC58;
            }
        }
    }
}
void GSmodelSetAnimFrame(GSmodel* model, f32 frame)
{
    HSD_JObj* jobj;
    u32 flags = model->flags;

    jobj = model->jobj;
    if (!(flags & MODEL_FLAG_HAS_ANIM)) {
        return;
    }

    if (!(flags & MODEL_FLAG_BLENDING)) {
        if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->anim_frame = frame;
        model->anim_req_frame = frame;
        fn_801A32A0(jobj, 0x1CB, model->anim_req_frame);
    } else {
        model->blend_anim_frame_a = frame * model->blend_frame_scale;
        model->blend_anim_frame_b = frame;
        fn_801A32A0(model->blend_jobj_a, 0x1CB, model->blend_anim_frame_a);
        fn_801A32A0(model->blend_jobj_b, 0x1CB, model->blend_anim_frame_b);
    }

    model->flags &= ~(MODEL_FLAG_SKIP_APPLY | MODEL_FLAG_ANIM_ENDED);

    flags = model->flags;
    if (flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        jobj = model->jobj;
        if (flags & MODEL_FLAG_HAS_TEX_ANIM) {
            if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                jobj = jobj->child;
            }
            model->tex_anim_frame = frame;
            model->tex_anim_req_frame = frame;
            fn_801A32A0(jobj, 0x634, model->tex_anim_req_frame);
            model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
        }
    }
}
void GSmodelSetAnimType(GSmodel* model, u32 type)
{
    HSD_JObj* jobj;

    jobj = model->jobj;
    if (model->flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        jobj = jobj->child;
    }

    model->anim_type = type;
    switch (model->anim_type) {
    case 0:
        HSD_ForeachAnim(jobj, 6, 0x9B2F,
                        (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
        break;
    case 1:
        HSD_ForeachAnim(jobj, 6, 0x9B2F,
                        (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
        break;
    }

    if (model->flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        jobj = model->jobj;
        if (model->flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->tex_anim_type = type;
        switch (model->tex_anim_type) {
        case 0:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
            break;
        case 1:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
            break;
        }
    }
}
void GSmodelSetAnimIndex(GSmodel* model, u32 index)
{
    HSD_JObj* jobj = model->jobj;
    u32 flags = model->flags;
    u32 type;

    if (!(flags & MODEL_FLAG_HAS_ANIM)) {
        return;
    }
    if (index >= model->anim_count) {
        return;
    }

    if (index != model->anim_index || (flags & MODEL_FLAG_BLENDING)) {
        void* shape_anim;

        if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->flags = ((volatile GSmodel*)model)->flags &
                       ~MODEL_FLAG_BLENDING;
        model->anim_index = index;
        if (model->flags & MODEL_FLAG_HAS_SHAPE_ANIM) {
            shape_anim = model->resource->shape_anims[model->anim_index];
        } else {
            shape_anim = NULL;
        }
        fn_801A2B5C(jobj, model->resource->anims[model->anim_index], NULL,
                    shape_anim);
        model->anim_end_frame = lbl_8047CC5C;
        HSD_ForeachAnim(jobj, 6, 0x9B2F, (void*)_modelGetEndFrame, 2,
                        &model->anim_end_frame);
    }

    flags = model->flags;
    jobj = model->jobj;
    if (flags & MODEL_FLAG_HAS_ANIM) {
        if (!(flags & MODEL_FLAG_BLENDING)) {
            if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                jobj = jobj->child;
            }
            model->anim_frame = lbl_8047CC5C;
            model->anim_req_frame = lbl_8047CC5C;
            fn_801A32A0(jobj, 0x1CB, model->anim_req_frame);
        } else {
            model->blend_anim_frame_a =
                lbl_8047CC5C * model->blend_frame_scale;
            model->blend_anim_frame_b = lbl_8047CC5C;
            fn_801A32A0(model->blend_jobj_a, 0x1CB,
                        model->blend_anim_frame_a);
            fn_801A32A0(model->blend_jobj_b, 0x1CB,
                        model->blend_anim_frame_b);
        }
        model->flags &= ~(MODEL_FLAG_SKIP_APPLY | MODEL_FLAG_ANIM_ENDED);

        flags = model->flags;
        if (flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
            jobj = model->jobj;
            if (flags & MODEL_FLAG_HAS_TEX_ANIM) {
                if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                    jobj = jobj->child;
                }
                model->tex_anim_frame = lbl_8047CC5C;
                model->tex_anim_req_frame = lbl_8047CC5C;
                fn_801A32A0(jobj, 0x634, model->tex_anim_req_frame);
                model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
            }
        }
    }

    type = model->anim_type;
    flags = model->flags;
    jobj = model->jobj;
    if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        jobj = jobj->child;
    }
    model->anim_type = type;
    switch (model->anim_type) {
    case 0:
        HSD_ForeachAnim(jobj, 6, 0x9B2F,
                        (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
        break;
    case 1:
        HSD_ForeachAnim(jobj, 6, 0x9B2F,
                        (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
        break;
    }
    if (model->flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        jobj = model->jobj;
        if (model->flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->tex_anim_type = type;
        switch (model->tex_anim_type) {
        case 0:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
            break;
        case 1:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
            break;
        }
    }

    flags = model->flags;
    if (flags & MODEL_FLAG_LINK_TEX_TO_ANIM) {
        jobj = model->jobj;
        if (!(flags & MODEL_FLAG_HAS_TEX_ANIM) ||
            index >= model->tex_anim_count) {
            goto done;
        }
        if (index != model->tex_anim_index) {
            if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                jobj = jobj->child;
            }
            model->tex_anim_index = index;
            fn_801A2B5C(jobj, NULL,
                        model->resource->tex_anims[model->tex_anim_index], NULL);
            model->tex_anim_end_frame = lbl_8047CC5C;
            HSD_ForeachAnim(jobj, 6, 0x64DB, (void*)_modelGetEndFrame, 2,
                            &model->tex_anim_end_frame);
        }

        flags = model->flags;
        jobj = model->jobj;
        if (flags & MODEL_FLAG_HAS_TEX_ANIM) {
            if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
                jobj = jobj->child;
            }
            model->tex_anim_frame = lbl_8047CC5C;
            model->tex_anim_req_frame = lbl_8047CC5C;
            fn_801A32A0(jobj, 0x634, model->tex_anim_req_frame);
            model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
        }

        type = model->tex_anim_type;
        flags = model->flags;
        jobj = model->jobj;
        if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->tex_anim_type = type;
        switch (model->tex_anim_type) {
        case 0:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 0);
            break;
        case 1:
            HSD_ForeachAnim(jobj, 6, 0x64DB,
                            (void*)_modelSetLoopFlag__FP9_HSD_AObjUl, 3, 1);
            break;
        }
    }

done:
    model->flags = model->flags & ~MODEL_FLAG_SKIP_APPLY;
}
void GSmodelAdvanceAnimation(GSmodel* model, f32 delta)
{
    u32 flags = model->flags;

    if (flags & MODEL_FLAG_PAUSED) {
        return;
    }

    if (flags & MODEL_FLAG_ANIM_ENDED) {
        fn_800ED6E4(model, FALSE);
    }
    if (model->flags & MODEL_FLAG_TEX_ANIM_ENDED) {
        fn_800ED6E4(model, TRUE);
    }

    flags = model->flags;
    if (flags & MODEL_FLAG_BLENDING) {
        if (flags & MODEL_FLAG_ANIMATING) {
            fn_800ED7E4(model, FALSE, delta * model->anim_rate);
            model->blend_anim_frame_a =
                model->blend_anim_frame_b * model->blend_frame_scale;
        }
    } else {
        if (flags & MODEL_FLAG_ANIMATING) {
            fn_800ED7E4(model, FALSE, delta * model->anim_rate);
        }
        if (model->flags & MODEL_FLAG_TEX_ANIMATING) {
            fn_800ED7E4(model, TRUE, delta * model->tex_anim_rate);
        }
    }

    model->flags &= ~(MODEL_FLAG_FORCE_UPDATE | MODEL_FLAG_SKIP_APPLY);
}
void modelApplyAnimation__FP8_GSmodel(GSmodel* model)
{
    HSD_JObj* jobj;
    HSD_AObj* anim_aobj;
    HSD_AObj* tex_anim_aobj;
    f32 anim_rate;
    f32 tex_anim_rate;
    u32 flags;
    u32 fractional_frames;

    flags = model->flags;
    if (flags & MODEL_FLAG_SKIP_APPLY) {
        return;
    }

    if (flags & MODEL_FLAG_BLENDING) {
        fn_801A32A0(model->blend_jobj_a, 0x1CB,
                    model->blend_anim_frame_a);
        fn_801A32A0(model->blend_jobj_b, 0x1CB,
                    model->blend_anim_frame_b);
        HSD_JObjAnimAll(model->blend_jobj_a);
        HSD_JObjAnimAll(model->blend_jobj_b);
        modelCalculateBlendModel__FP8_GSmodelf(model);
        goto done;
    }

    jobj = model->jobj;
    if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        jobj = jobj->child;
    }

    anim_rate = lbl_8047CC5C;
    tex_anim_rate = anim_rate;
    if (flags & MODEL_FLAG_ANIMATING) {
        if ((flags & MODEL_FLAG_60FPS_ANIM) == MODEL_FLAG_60FPS_ANIM ||
            (model->force_fractional_frames != 0 &&
             model->fractional_frame_data != NULL)) {
            fractional_frames = TRUE;
        } else {
            fractional_frames = FALSE;
        }
        anim_rate = fn_800ED8C4(model->anim_type, fractional_frames,
                                model->anim_frame, model->anim_req_frame,
                                model->anim_end_frame, model->anim_rate);
    }

    flags = model->flags;
    if (flags & MODEL_FLAG_TEX_ANIMATING) {
        if ((flags & MODEL_FLAG_60FPS_ANIM) == MODEL_FLAG_60FPS_ANIM ||
            (model->force_fractional_frames != 0 &&
             model->fractional_frame_data != NULL)) {
            fractional_frames = TRUE;
        } else {
            fractional_frames = FALSE;
        }
        tex_anim_rate = fn_800ED8C4(
            model->tex_anim_type, fractional_frames, model->tex_anim_frame,
            model->tex_anim_req_frame, model->tex_anim_end_frame,
            model->tex_anim_rate);
    }

    HSD_ForeachAnim(jobj, 6, 0x9B2F, (void*)HSD_AObjSetRate, 1,
                    anim_rate);
    HSD_ForeachAnim(jobj, 6, 0x64DB, (void*)HSD_AObjSetRate, 1,
                    tex_anim_rate);
    HSD_JObjAnimAll(jobj);

    if (model->flags & MODEL_FLAG_ANIMATING) {
        anim_aobj = NULL;
        HSD_ForeachAnim(jobj, 6, 0x20,
                        (void*)_modelGetAObjFunc__FP9_HSD_AObjPv, 2,
                        &anim_aobj);
        model->anim_req_frame = anim_aobj->curr_frame;
    }

    if (model->flags & MODEL_FLAG_TEX_ANIMATING) {
        tex_anim_aobj = NULL;
        HSD_ForeachAnim(jobj, 6, 0x480,
                        (void*)_modelGetAObjFunc__FP9_HSD_AObjPv, 2,
                        &tex_anim_aobj);
        model->tex_anim_req_frame = tex_anim_aobj->curr_frame;
    }

    if (model->flags & MODEL_FLAG_ANIMATING) {
        _modelResetPartAnimMixes__FP8_GSmodel(model);
    }

    if (model->flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        jobj = model->jobj->child;
        if (jobj != NULL) {
            s32 dirty;
            u32 jobj_flags;

            if (jobj == NULL) {
                __assert(lbl_8047CC68, 0x25D, lbl_8047CC70);
            }
            jobj_flags = jobj->flags;
            dirty = FALSE;
            if (!(jobj_flags & JOBJ_USER_DEF_MTX)) {
                if (jobj_flags & JOBJ_MTX_DIRTY) {
                    dirty = TRUE;
                }
            }
            if (dirty != FALSE) {
                fn_8019D9DC(jobj);
            }
        }
    }

    jobj = model->jobj;
    if (jobj != NULL) {
        s32 dirty;
        u32 jobj_flags;

        if (jobj == NULL) {
            __assert(lbl_8047CC68, 0x25D, lbl_8047CC70);
        }
        jobj_flags = jobj->flags;
        dirty = FALSE;
        if (!(jobj_flags & JOBJ_USER_DEF_MTX)) {
            if (jobj_flags & JOBJ_MTX_DIRTY) {
                dirty = TRUE;
            }
        }
        if (dirty != FALSE) {
            fn_8019D9DC(jobj);
        }
    }

done:
    model->flags |= MODEL_FLAG_SKIP_APPLY;
}
#pragma force_active off
