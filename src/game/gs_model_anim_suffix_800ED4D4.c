/**
 * @file gs_model_anim_suffix_800ED4D4.c
 * @brief GSmodel animation suffix, 0x800ED4D4 - 0x800EE150.
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


/* sModelJObjCount / sModelJObjLastIndex are .sbss globals owned by the
 * gs_model_anim split (config/GC6E01/symbols.txt). fn_800EE0E8 uses them
 * as a shared counter/scratch during a JObj traversal driven by fn_800EE20C. */
extern s32 sModelJObjCount;
extern s32 sModelJObjLastIndex;
typedef void (*GSmodelAObjApplyFunc)(HSD_AObj* aobj, void* arg);
void fn_801A3918(HSD_JObj* jobj, GSmodelAObjApplyFunc func, u32 arg);
void fn_800EE20C(HSD_AObj* aobj, void* arg);


void modelUpdateAttachments__FP8_GSmodel(GSmodel* model)
{
    GSvec position;
    GSvec rotation;
    GSvec scale;
    GSquat quaternion;
    GSvec transformed_position;
    GSpart* part;
    u32 flags;

    if (model->attachment_type != 0 &&
        (model->flags & MODEL_FLAG_UPDATE_ATTACHMENT)) {
        part = GSmodelGetPart(model->attachment_model,
                              model->attachment_part_index);
        GSpartGetTransform(part, &position, &rotation, &scale);
        GSpartFree(part);

        fn_800E06EC(&quaternion, &rotation);
        GSvecTransformQuat(&transformed_position, &quaternion,
                           &model->attachment_position);
        GSvecAdd(&position, &position, &transformed_position);
        GSvecAdd(&rotation, &rotation, &model->attachment_rotation);
        fn_800E0108(&scale, &scale, &model->attachment_scale);

        modelSetPos(model, &model->attachment_position);
        modelSetRot(model, &model->attachment_rotation);
        modelSetScl(model, &model->attachment_scale);

        switch (model->attachment_type) {
        case 1:
            modelSetPos(model, &position);
            break;
        case 2:
            modelSetRot(model, &rotation);
            break;
        case 3:
            modelSetScl(model, &scale);
            break;
        case 4:
            modelSetPos(model, &position);
            modelSetRot(model, &rotation);
            break;
        case 5:
            modelSetRot(model, &rotation);
            modelSetScl(model, &scale);
            break;
        case 6:
            modelSetPos(model, &position);
            modelSetScl(model, &scale);
            break;
        case 7:
            modelSetPos(model, &position);
            modelSetRot(model, &rotation);
            modelSetScl(model, &scale);
            break;
        }

        flags = model->flags;
        if (flags & MODEL_FLAG_ATTACHMENT_ONCE) {
            model->flags = flags & ~MODEL_FLAG_UPDATE_ATTACHMENT;
        } else {
            model->flags = flags | MODEL_FLAG_UPDATE_ATTACHMENT;
        }
    }

    model->flags = model->flags | MODEL_FLAG_FORCE_UPDATE;
}
u32 modelProcessAt60fps__FP8_GSmodel(GSmodel* model)
{
    if ((model->flags & MODEL_FLAG_60FPS_ANIM) == MODEL_FLAG_60FPS_ANIM) {
        goto yes;
    }
    if (model->attachment_type == 0) {
        goto no;
    }
    if (model->attachment_model == NULL) {
        goto no;
    }
yes:
    return TRUE;
no:
    return FALSE;
}
void fn_800ED6E4(GSmodel* model, u8 tex_anim)
{
    u32 end_flag_mask;
    u32 anim_flag_mask;
    s32 anim_type;
    u32 anim_index;
    GSmodelAnimEndedInfo info;

    if (!(model->flags & MODEL_FLAG_BLENDING)) {
        if (!tex_anim) {
            anim_index = model->anim_index;
            end_flag_mask = MODEL_FLAG_ANIM_ENDED;
            anim_type = model->anim_type;
            anim_flag_mask = MODEL_FLAG_ANIMATING;
        } else {
            end_flag_mask = MODEL_FLAG_TEX_ANIM_ENDED;
            anim_index = model->tex_anim_index;
            anim_type = model->tex_anim_type;
            anim_flag_mask = MODEL_FLAG_TEX_ANIMATING;
        }
    } else if (!tex_anim) {
        anim_index = model->blend_anim_index_b;
        end_flag_mask = MODEL_FLAG_ANIM_ENDED;
        anim_type = model->anim_type;
        anim_flag_mask = MODEL_FLAG_ANIMATING;
    }

    switch (anim_type) {
    case 0:
        model->flags &= ~anim_flag_mask;
        break;
    case 1:
        model->flags &= ~end_flag_mask;
        break;
    }

    if (model->anim_ended_callback != NULL) {
        info.event_flags = 0;
        if (tex_anim == 0) {
            info.event_flags |= 1;
        } else {
            info.event_flags |= 2;
        }
        if (anim_type == 1) {
            info.event_flags |= 4;
        }
        info.anim_index = anim_index;
        info.arg = model->anim_ended_callback_arg;
        model->anim_ended_callback(model, &info);
    }
}
void fn_800ED7E4(GSmodel* model, u8 tex_anim, f32 delta)
{
    u32 ended_flag;
    f32* frame_ptr;
    f32* req_ptr;
    s32 type;
    f32 end_frame;

    if (!(model->flags & MODEL_FLAG_BLENDING)) {
        if (!tex_anim) {
            end_frame = model->anim_end_frame;
            frame_ptr = &model->anim_frame;
            type = model->anim_type;
            req_ptr = &model->anim_req_frame;
            ended_flag = MODEL_FLAG_ANIM_ENDED;
        } else {
            end_frame = model->tex_anim_end_frame;
            type = model->tex_anim_type;
            frame_ptr = &model->tex_anim_frame;
            req_ptr = &model->tex_anim_req_frame;
            ended_flag = MODEL_FLAG_TEX_ANIM_ENDED;
        }
    } else if (!tex_anim) {
        frame_ptr = &model->blend_anim_frame_b;
        end_frame = model->blend_anim_end_frame_b;
        type = model->anim_type;
        req_ptr = frame_ptr;
        ended_flag = MODEL_FLAG_ANIM_ENDED;
    }

    *frame_ptr += delta;
    if (*frame_ptr >= end_frame - lbl_8047CC78) {
        switch (type) {
        case 0:
            *frame_ptr = end_frame;
            break;
        case 1:
            *frame_ptr -= end_frame;
            break;
        }
    }
    if (*req_ptr >= end_frame - lbl_8047CC78) {
        if (type == 0) {
            model->flags |= ended_flag;
        }
    }
}
void _modelResetPartAnimMixes__FP8_GSmodel(GSmodel* model)
{
    GSmodelPartAnimMix* mix;
    s32 i;
    GSpart* part;
    HSD_JObj* jobj;

    mix = model->part_anim_mixes;
    for (i = 3; i >= 0; i--, mix++) {
        if (mix->type == 0) {
            continue;
        }

        part = GSmodelGetPart(model, mix->part_index);
        if (part == NULL) {
            continue;
        }

        switch (mix->type) {
        case 1:
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_SET_ROT(jobj, mix->value[0], 0x2A4,
                                              0x2A5, rotate_x);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_SET_ROT(jobj, mix->value[1], 0x2B8,
                                              0x2B9, rotate_y);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_SET_ROT(jobj, mix->value[2], 0x2CC,
                                              0x2CD, rotate_z);
            break;
        case 2:
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj, mix->value[0], 0x412,
                                             rotate_x);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj, mix->value[1], 0x41D,
                                             rotate_y);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj, mix->value[2], 0x428,
                                             rotate_z);
            break;
        case 3:
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj, mix->value[0], 0x412,
                                             rotate_x);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_SET_ROT(jobj, mix->value[1], 0x2B8,
                                              0x2B9, rotate_y);
            jobj = part->jobj;
            MODEL_RESET_PART_ANIM_MIX_ADD_ROT(jobj, mix->value[2], 0x428,
                                             rotate_z);
            break;
        }

        GSpartFree(part);
    }
}
void _modelGetAObjFunc__FP9_HSD_AObjPv(HSD_AObj* aobj, HSD_AObj** out)
{
    if (aobj != NULL) {
        *out = aobj;
    }
}
void _modelSetLoopFlag__FP9_HSD_AObjUl(HSD_AObj* aobj, u32 enable)
{
    if (enable == 0) {
        HSD_AObjClearFlags(aobj, AOBJ_LOOP);
    } else {
        HSD_AObjSetFlags(aobj, AOBJ_LOOP);
    }
}
void _modelGetEndFrame(HSD_AObj* aobj, f32* end_frame)
{
    if (aobj == NULL) {
        __assert(lbl_8047CC90, 0xAB, lbl_8047CC98);
    }

    if (aobj->end_frame > *end_frame) {
        *end_frame = aobj->end_frame;
    }
}
s32 fn_800EE0E8(GSmodel* model)
{
    sModelJObjCount = 0;
    sModelJObjLastIndex = -1;
    fn_801A3918(model->jobj, fn_800EE20C, 0);
    if (model->flags & MODEL_FLAG_USE_JOBJ_CHILD) {
        sModelJObjCount = sModelJObjCount - 1;
    }
    return sModelJObjCount;
}
#undef MODEL_RESET_PART_ANIM_MIX_ADD_ROT
#undef MODEL_RESET_PART_ANIM_MIX_SET_ROT
#undef MODEL_RESET_PART_ANIM_MIX_DIRTY
