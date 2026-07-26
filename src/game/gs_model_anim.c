/**
 * @file gs_model_anim.c
 * @brief GSmodel animation prefix, 0x800EBEEC - 0x800EC4D0.
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

#if defined(GS_MODEL_ANIM_800EC0E8_800EC35C)

void GSmodelForceAnimTransformUpdate(GSmodel* model)
{
    u32 flags = model->flags;

    if (!(flags & MODEL_FLAG_FORCE_UPDATE) ||
        !(flags & MODEL_FLAG_SKIP_APPLY)) {
        modelApplyAnimation__FP8_GSmodel(model);
        modelUpdateAttachments__FP8_GSmodel(model);
    }
}
void fn_800EC134(GSmodel* model)
{
    modelApplyAnimation__FP8_GSmodel(model);
}
void GSmodelSetAnimEndedCallback(GSmodel* model,
                                 GSmodelAnimEndedCallback callback, void* arg)
{
    model->anim_ended_callback = callback;
    model->anim_ended_callback_arg = arg;
}
void GSmodelSet60fpsAnimFlag(GSmodel* model, u8 enable)
{
    if (enable) {
        model->flags |= MODEL_FLAG_60FPS_ANIM;
    } else {
        model->flags = model->flags & ~MODEL_FLAG_60FPS_ANIM;
    }
}
void GSmodelLinkTexAnimToAnim(GSmodel* model, u8 enable)
{
    if (enable) {
        model->flags |= MODEL_FLAG_LINK_TEX_TO_ANIM;
    } else {
        model->flags = model->flags & ~MODEL_FLAG_LINK_TEX_TO_ANIM;
    }
}
u32 GSmodelCanTexAnimate(GSmodel* model)
{
    return (model->flags >> 3) & 1;
}
u32 GSmodelCanAnimate(GSmodel* model)
{
    return (model->flags >> 2) & 1;
}
u32 GSmodelHasTexAnimationEnded(GSmodel* model)
{
    return (model->flags >> 15) & 1;
}
void GSmodelStopTexAnimation(GSmodel* model)
{
    model->flags = model->flags & ~MODEL_FLAG_TEX_ANIMATING;
}
void GSmodelStartTexAnimation(GSmodel* model)
{
    u32 flags = model->flags;

    if (flags & MODEL_FLAG_HAS_TEX_ANIM) {
        model->flags = flags | MODEL_FLAG_TEX_ANIMATING;
        model->flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
    }
}
void GSmodelSetTexAnimType(GSmodel* model, u32 type)
{
    HSD_JObj* jobj = model->jobj;

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
void GSmodelSetTexAnimFrame(GSmodel* model, f32 frame)
{
    HSD_JObj* jobj;
    u32 flags = model->flags;

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
void GSmodelSetTexAnimRate(GSmodel* model, f32 rate)
{
    if (model->flags & MODEL_FLAG_HAS_TEX_ANIM) {
        model->tex_anim_rate = rate;
        if (fn_800D37CC() == 50) {
            model->tex_anim_rate *= lbl_8047CC58;
        }
    }
}

#endif

#if defined(GS_MODEL_ANIM_800EC35C_800EC4D0)

void GSmodelSetTexAnimIndex(GSmodel* model, u32 index)
{
    HSD_JObj* jobj = model->jobj;
    u32 flags = model->flags;

    if (!(flags & MODEL_FLAG_HAS_TEX_ANIM)) {
        return;
    }
    if (index >= model->tex_anim_count) {
        return;
    }
    if (index != model->tex_anim_index) {
        if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
            jobj = jobj->child;
        }
        model->tex_anim_index = index;
        fn_801A2B5C(jobj, 0, model->resource->tex_anims[model->tex_anim_index],
                    NULL);
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
        flags = model->flags & ~MODEL_FLAG_TEX_ANIM_ENDED;
        model->flags = flags;
    }

    flags = model->flags;
    jobj = model->jobj;
    if (flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        jobj = jobj->child;
    }

    model->tex_anim_type = model->tex_anim_type;
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

#endif
