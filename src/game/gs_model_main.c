/**
 * @file gs_model_main.c
 * @brief GSmodel update prefix
 *
 * Target range 0x800E3604-0x800E3928; source owns fn_800E3884.
 */
#include "dolphin/types.h"
#include "game/data/sdata2_8047CAA0.h"
#include "game/gs_model.h"
#include "game/gs_model_anim.h"
#include "game/gs_render.h"

#define GSMODEL_FLAG_ACTIVE 0x00000001U
#define GSMODEL_FLAG_VISIBLE 0x00000002U
#define GSMODEL_FLAG_HAS_ANIM 0x00000004U
#define GSMODEL_FLAG_HAS_TEX_ANIM 0x00000008U
#define GSMODEL_FLAG_HAS_SHAPE_ANIM 0x00000010U
#define GSMODEL_FLAG_RENDER_ALT_JOBJ 0x00000080U
#define GSMODEL_FLAG_LINK_TEX_TO_ANIM 0x00002000U
#define GSMODEL_FLAG_PARTICLE_TEXSTAGE 0x00000200U
#define GSMODEL_FLAG_PARTICLE_LINKED 0x00000400U
#define GSMODEL_FLAG_BOUND_CHECK 0x00100000U
#define GSMODEL_FLAG_UPDATE_BOUND 0x00200000U
#define GSMODEL_FLAG_BOUND_VISIBLE 0x00400000U
#define GSMODEL_FLAG_ROOT_NULL_ADDED 0x00020000U
#define GSJOBJ_DESC_FLAG_INSTANCE 0x00001000U
#define JOBJ_MTX_DIRTY 0x00000040U
#define JOBJ_USE_QUATERNION 0x00020000U
#define JOBJ_USER_DEF_MTX 0x00800000U
#define JOBJ_MTX_INDEP_SRT 0x02000000U
#define JOBJ_SET_MTX_FLAGS 0x03800000U

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

typedef struct GScolor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GScolor;

typedef struct GSmaterial {
    u8 _pad0;
    u8 alpha;
} GSmaterial;

typedef struct GSbound {
    u8 _pad00[0x10];
    GSvec centerStart;
    GSvec centerEnd;
    u8 _pad28[0x0C];
} GSbound;

typedef struct GSmodelMaterialList {
    GSmaterial* materials[1];
} GSmodelMaterialList;

typedef struct GSjobjDesc {
    void* className;
    u32 flags;
    struct GSjobjDesc* child;
    struct GSjobjDesc* next;
    void* dobj;
    GSvec rotation;
    GSvec scale;
    GSvec position;
    void* robj;
    void* aobj;
} GSjobjDesc;

typedef struct GSmodelResource {
    GSjobjDesc* joint;
    void** anims;
    void** texAnims;
    void** shapeAnims;
} GSmodelResource;

typedef struct GSpart GSpart;

typedef struct HSDJObj {
    /* 0x000 */ u8 _pad000[0x08];
    /* 0x008 */ struct HSDJObj* next;
    /* 0x00C */ u8 _pad00C[0x04];
    /* 0x010 */ struct HSDJObj* child;
    /* 0x014 */ u32 flags;
    /* 0x018 */ void* data;
    /* 0x01C */ GSvec rotation;
    /* 0x028 */ f32 rotationW;
    /* 0x02C */ GSvec scale;
    /* 0x038 */ GSvec translate;
    /* 0x044 */ f32 matrix[3][4];
} HSDJObj;

typedef union GSmodelFlags {
    u32 raw;
    struct {
        u32 pad00_10 : 11;
        u32 boundCheck : 1;
        u32 pad12_13 : 2;
        u32 rootNullAdded : 1;
        u32 pad15_20 : 6;
        u32 particleLinked : 1;
        u32 particleTexStage : 1;
        u32 pad23 : 1;
        u32 renderAltJObj : 1;
        u32 pad25_29 : 5;
        u32 visible : 1;
        u32 active : 1;
    } bits;
} GSmodelFlags;

typedef struct GSmodel {
    /* 0x000 */ GSmodelFlags flags;
    /* 0x004 */ GSmodelResource* resource;
    /* 0x008 */ HSDJObj* renderJObj;
    /* 0x00C */ HSDJObj* renderJObjAlt;
    /* 0x010 */ HSDJObj* jobj10;
    /* 0x014 */ HSDJObj* jobj14;
    /* 0x018 */ GSvec position;
    /* 0x024 */ GSvec rotation;
    /* 0x030 */ GSvec scale;
    /* 0x03C */ GSvec boundCenter;
    /* 0x048 */ f32 boundYOffset;
    /* 0x04C */ GSbound bound;
    /* 0x080 */ u8 centerNullState;
    /* 0x081 */ u8 _pad081[0x03];
    /* 0x084 */ u32 animCount;
    /* 0x088 */ u32 texAnimCount;
    /* 0x08C */ u32 animType;
    /* 0x090 */ u32 animIndex;
    /* 0x094 */ f32 animRate;
    /* 0x098 */ f32 animFrame;
    /* 0x09C */ f32 animRequestedFrame;
    /* 0x0A0 */ f32 animEndFrame;
    /* 0x0A4 */ u32 texAnimType;
    /* 0x0A8 */ u32 texAnimIndex;
    /* 0x0AC */ f32 texAnimRate;
    /* 0x0B0 */ f32 texAnimFrame;
    /* 0x0B4 */ f32 texAnimRequestedFrame;
    /* 0x0B8 */ f32 texAnimEndFrame;
    /* 0x0BC */ u8 _pad0BC[0x20];
    /* 0x0DC */ void* animEndedCallback;
    /* 0x0E0 */ void* animEndedCallbackArg;
    /* 0x0E4 */ struct {
        s32 type;
        s32 partIndex;
        f32* value;
    } partAnimMix[4];
    /* 0x114 */ s32 transformOverride;
    /* 0x118 */ void* unk118;
    /* 0x11C */ void* unk11C;
    /* 0x120 */ GSvec overridePosition;
    /* 0x12C */ GSvec overrideRotation;
    /* 0x138 */ GSvec overrideScale;
    /* 0x144 */ void* linkedGSparticleBank;
    /* 0x148 */ u32 gsParticleLinkAttachMode;
    /* 0x14C */ GSmodelMaterialList* materialList;
    /* 0x150 */ u16 materialCount;
    /* 0x152 */ u16 modulationRefCount;
    /* 0x154 */ u16 materialListHandle;
    /* 0x156 */ u8 _pad156[0x02];
    /* 0x158 */ void* unk158;
    /* 0x15C */ void* unk15C;
    /* 0x160 */ void* shadowLight;
    /* 0x164 */ u16 unk164;
    /* 0x166 */ u8 _pad166[0x02];
    /* 0x168 */ s32 vertexCount;
    /* 0x16C */ s32 polygonCount;
} GSmodel;

extern GSmodel* lbl_8047AB74;
extern u32 lbl_8047AB78;
extern u16 lbl_8047AB70;

extern void GSvecCopy(GSvec* dst, const GSvec* src);
extern void GSmodelSetAnimFrame(GSmodel* model, f32 frame);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u32 handle);
extern void modelShadowInit__Fv(void);
extern void fn_800EC134(GSmodel* model);
extern void fn_80118874(void* bank, u32 destroyChildren);
extern void fn_80118C20(void* bank, u32 visible);
extern u32 fn_80191118(GSbound* bound);
extern u32 GSmaterialGetEnabledExtensions(void* material);
extern void GSmaterialGetModulate(void* material, GScolor* color);
extern void GSmaterialSetModulate(void* material, const GScolor* color);
extern void GSmaterialSetEnvMapBlendValue(void* material, f32 value);
extern void GSmaterialSetAlpha(void* material, f32 alpha);
extern void GSmaterialDisableExtension(void* material, u32 extension);
extern void GSmaterialResetTexture(void* material);
extern void GSmaterialResetPEdescr(void* material);
extern void GSmaterialResetAlpha(void* material);
extern void GSmaterialResetFlags(void* material);
extern void fn_800DF608(void* material);
extern void fn_800E24B0(u32 handle);
extern void fn_800E209C(u32 handle);
extern void modelShadowFreeModelList__FP8_GSmodel(GSmodel* model);
extern void GSmodelRemoveNull(GSmodel* model);
extern void modelRemoveCenterNull(GSmodel* model);
extern HSDJObj* fn_801A02B0(HSDJObj* jobj);
extern void fn_801A05EC(void* object);
extern void* memset(void* dst, int value, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void fn_8019D9DC(HSDJObj* jobj);
extern void __assert(const char* file, u32 line, const char* message);
extern char lbl_8047CB60[] __attribute__((section(".sdata2")));
extern char lbl_8047CB68[] __attribute__((section(".sdata2")));
extern char lbl_8047CB70[] __attribute__((section(".sdata2")));
extern char lbl_8047CB78[] __attribute__((section(".sdata2")));
extern char lbl_8047CB9C[] __attribute__((section(".sdata2")));
extern char lbl_8047CBA4[] __attribute__((section(".sdata2")));
extern char lbl_8047CBAC[] __attribute__((section(".sdata2")));
extern char lbl_80270E6C[];
extern char lbl_80270E28[];
extern char lbl_80270E50[];
extern char lbl_80270E60[];
extern void clear__5GSvecFv(GSvec* vec);
extern u8 modelProcessAt60fps__FP8_GSmodel(GSmodel* model);
extern void GSmodelAdvanceAnimation(GSmodel* model, f32 frames);
extern void PSMTXCopy(const f32 src[3][4], f32 dst[3][4]);
extern void fn_8019D620(HSDJObj* jobj);
extern GSpart* GSmodelGetPart(GSmodel* model, s32 index);
extern void GSpartGetTransform(GSpart* part, GSvec* position, GSvec* rotation, GSvec* scale);
extern void GSpartFree(GSpart* part);
extern HSDJObj* fn_8019F718(void);
extern void HSD_JObjAddChild(HSDJObj* parent, HSDJObj* child);
extern u32 HSD_JObjGetFlags(HSDJObj* jobj);
extern void fn_8019FE8C(HSDJObj* jobj, u32 flags);
extern void GSmodelSetAnimIndex(GSmodel* model, u32 index);
extern void GSmodelSetAnimType(GSmodel* model, u32 type);
extern void GSmodelStartAnimation(GSmodel* model);

void fn_800E85E8(GSmodel* model);







void fn_800E3884(u32 frames, u8 phase)
{
    u32 i;

    i = 0;
    while (i < lbl_8047AB78) {
        GSmodel* model;

        model = &lbl_8047AB74[i];
        if (model->flags.raw & GSMODEL_FLAG_ACTIVE) {
            if (modelProcessAt60fps__FP8_GSmodel(model) == phase) {
                GSmodelAdvanceAnimation(model, (f32)frames);
            }
        }

        i++;
    }
}













































































































#define GSMODEL_JOBJ_FLUSH(jobj)                                                     \
    do {                                                                            \
        if (!((jobj)->flags & JOBJ_MTX_INDEP_SRT)) {                                \
            if ((jobj) != NULL) {                                                   \
                s32 dirty_ = 0;                                                     \
                u32 flags_;                                                         \
                if ((jobj) == NULL) {                                               \
                    __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);                    \
                }                                                                   \
                flags_ = (jobj)->flags;                                             \
                if (!(flags_ & JOBJ_USER_DEF_MTX) && (flags_ & JOBJ_MTX_DIRTY)) {  \
                    dirty_ = 1;                                                     \
                }                                                                   \
                if (dirty_ == 0) {                                                  \
                    fn_8019D620(jobj);                                              \
                }                                                                   \
            }                                                                       \
        }                                                                           \
    } while (0)

#define GSMODEL_JOBJ_SET_POSITION(jobj, value)                      \
    do {                                                            \
        const GSvec* value_ = (value);                              \
        if ((jobj) == NULL) {                                       \
            __assert(lbl_8047CB9C, 0x3A9, lbl_8047CBA4);            \
        }                                                           \
        if (value_ == NULL) {                                       \
            __assert(lbl_8047CB9C, 0x3AA, lbl_80270E60);            \
        }                                                           \
        (jobj)->translate = *value_;                                \
        GSMODEL_JOBJ_FLUSH(jobj);                                   \
    } while (0)

#define GSMODEL_JOBJ_SET_ROTATION_COMPONENT(jobj, member, value, line, quat_line) \
    do {                                                                           \
        if ((jobj) == NULL) {                                                      \
            __assert(lbl_8047CB9C, line, lbl_8047CBA4);                            \
        }                                                                          \
        if ((jobj)->flags & JOBJ_USE_QUATERNION) {                                 \
            __assert(lbl_8047CB9C, quat_line, lbl_80270E6C);                       \
        }                                                                          \
        (jobj)->rotation.member = (value);                                         \
        GSMODEL_JOBJ_FLUSH(jobj);                                                  \
    } while (0)

#define GSMODEL_JOBJ_SET_ROTATION(jobj, value)                                  \
    do {                                                                        \
        const GSvec* value_ = (value);                                          \
        GSMODEL_JOBJ_SET_ROTATION_COMPONENT(jobj, x, value_->x, 0x2A4, 0x2A5); \
        GSMODEL_JOBJ_SET_ROTATION_COMPONENT(jobj, y, value_->y, 0x2B8, 0x2B9); \
        GSMODEL_JOBJ_SET_ROTATION_COMPONENT(jobj, z, value_->z, 0x2CC, 0x2CD); \
    } while (0)

#define GSMODEL_JOBJ_SET_SCALE(jobj, value)                       \
    do {                                                          \
        const GSvec* value_ = (value);                            \
        if ((jobj) == NULL) {                                     \
            __assert(lbl_8047CB9C, 0x316, lbl_8047CBA4);          \
        }                                                         \
        if (value_ == NULL) {                                     \
            __assert(lbl_8047CB9C, 0x317, lbl_8047CBAC);          \
        }                                                         \
        (jobj)->scale = *value_;                                  \
        GSMODEL_JOBJ_FLUSH(jobj);                                 \
    } while (0)



#undef GSMODEL_JOBJ_SET_SCALE
#undef GSMODEL_JOBJ_SET_ROTATION
#undef GSMODEL_JOBJ_SET_ROTATION_COMPONENT
#undef GSMODEL_JOBJ_SET_POSITION
#undef GSMODEL_JOBJ_FLUSH
