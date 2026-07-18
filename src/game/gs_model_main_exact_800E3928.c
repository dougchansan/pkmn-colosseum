/**
 * @file gs_model_main_exact_800E3928.c
 * @brief GSmodel exact model-list, bounds, transforms, and detach island
 *
 * Exact target range 0x800E3928-0x800E4AC0; functions follow target order.
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

static inline void modelUpdateBoundCenter(GSmodel* model)
{
    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
    } else {
        GSpart* part;

        part = GSmodelGetPart(model, 1);
        if (part == NULL) {
            memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
        } else {
            GSpartGetTransform(part, &model->boundCenter, NULL, NULL);
            GSpartFree(part);
            model->boundCenter.y -= model->boundYOffset;
        }
    }
}

void fn_800E3928(void* unused)
{
    u32 i;
    GSmodel* model;

    i = 0;
    while (i < lbl_8047AB78) {
        u32 flags;

        model = &lbl_8047AB74[i];
        flags = model->flags.raw;
        if (flags & GSMODEL_FLAG_ACTIVE) {
            if (flags & GSMODEL_FLAG_VISIBLE) {
                if (flags & GSMODEL_FLAG_BOUND_CHECK) {
                    u8 boundVisible;

                    boundVisible = fn_80191118(&model->bound) != 0;
                    if (boundVisible == 1) {
                        model->flags.raw |= GSMODEL_FLAG_BOUND_VISIBLE;
                    } else {
                        model->flags.raw &= ~GSMODEL_FLAG_BOUND_VISIBLE;
                    }
                } else {
                    model->flags.raw |= GSMODEL_FLAG_BOUND_VISIBLE;
                }
            } else {
                model->flags.raw &= ~GSMODEL_FLAG_BOUND_VISIBLE;
            }
        }
        i++;
    }

    i = 0;
    while (i < lbl_8047AB78) {
        u32 flags;

        model = &lbl_8047AB74[i];
        flags = model->flags.raw;
        if ((flags & GSMODEL_FLAG_ACTIVE) &&
            (flags & (GSMODEL_FLAG_BOUND_VISIBLE |
                      GSMODEL_FLAG_PARTICLE_LINKED))) {
            modelApplyAnimation__FP8_GSmodel(model);
        }
        i++;
    }

    i = 0;
    while (i < lbl_8047AB78) {
        model = &lbl_8047AB74[i];
        if (model->flags.raw & GSMODEL_FLAG_ACTIVE) {
            u32 flags;

            modelUpdateAttachments__FP8_GSmodel(model);
            modelUpdateBoundCenter(model);

            flags = model->flags.raw;
            if ((flags & GSMODEL_FLAG_BOUND_VISIBLE) &&
                (flags & GSMODEL_FLAG_UPDATE_BOUND)) {
                fn_80190E60(&model->bound);
            }
        }
        i++;
    }
}

GSmodel* fn_800E3B08(u32 index)
{
    GSmodel* model;

    if (index >= lbl_8047AB78) {
        return NULL;
    }

    model = &lbl_8047AB74[index];
    if (model->flags.raw & GSMODEL_FLAG_ACTIVE) {
        return model;
    }

    return NULL;
}

u32 fn_800E3B3C(void)
{
    return lbl_8047AB78;
}

void fn_800E3B44(GSmodel* model, u8 enable)
{
    if (enable) {
        model->flags.raw |= GSMODEL_FLAG_PARTICLE_LINKED;
        return;
    }

    model->flags.raw &= ~GSMODEL_FLAG_PARTICLE_LINKED;
}

GSmodel* GSmodelSearchModelList(void* jobj)
{
    u32 i;
    GSmodel* model;
    u32 flags;

    i = 0;
    model = lbl_8047AB74;

    while (i < lbl_8047AB78) {
        flags = model->flags.raw;
        if (flags & GSMODEL_FLAG_ACTIVE) {
            void* renderJObj;

            if (flags & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
                renderJObj = model->renderJObjAlt;
            } else {
                renderJObj = model->renderJObj;
            }

            if (renderJObj == jobj) {
                return model;
            }
        }

        model++;
        i++;
    }

    return NULL;
}

void GSmodelDestroyLinkedParticles(GSmodel* model)
{
    void* bank;

    bank = model->linkedGSparticleBank;
    if (bank != NULL) {
        fn_80118874(bank, 1);
    }
}

u32 GSmodelGetGSparticleLinkAttachMode(GSmodel* model)
{
    return model->gsParticleLinkAttachMode;
}

void* GSmodelGetLinkedGSparticleBank(GSmodel* model)
{
    return model->linkedGSparticleBank;
}

void GSmodelSetGSparticleLinkAttachMode(GSmodel* model, u32 mode)
{
    model->gsParticleLinkAttachMode = mode;
}

void GSmodelLinkToGSparticleBank(GSmodel* model, void* bank)
{
    if (model->linkedGSparticleBank != bank) {
        model->linkedGSparticleBank = bank;
        if (model->linkedGSparticleBank != NULL) {
            GSmodelSetAnimFrame(model, model->animFrame);
            fn_800EC134(model);
        }
    }
}

GSbound* GSmodelGetBound(GSmodel* model)
{
    return &model->bound;
}

u32 fn_800E3C64(GSmodel* model)
{
    return fn_80191118(&model->bound) != 0;
}

void GSmodelSetBoundCheck(GSmodel* model, u8 enable)
{
    if (enable) {
        model->flags.raw |= GSMODEL_FLAG_BOUND_CHECK;
        return;
    }

    model->flags.raw &= ~GSMODEL_FLAG_BOUND_CHECK;
}

u32 fn_800E3CBC(GSmodel* model)
{
    return (model->flags.raw & GSMODEL_FLAG_PARTICLE_TEXSTAGE) != 0;
}

void fn_800E3CC8(GSmodel* model, u8 enable)
{
    if (enable) {
        model->flags.raw |= GSMODEL_FLAG_PARTICLE_TEXSTAGE;
        return;
    }

    model->flags.raw &= ~GSMODEL_FLAG_PARTICLE_TEXSTAGE;
}

GSvec* GSmodelGetScalePtr(GSmodel* model)
{
    return &model->scale;
}

GSvec* GSmodelGetRotationPtr(GSmodel* model)
{
    return &model->rotation;
}

GSvec* GSmodelGetPositionPtr(GSmodel* model)
{
    return &model->position;
}

u32 GSmodelGetVisibility(GSmodel* model)
{
    return (model->flags.raw & GSMODEL_FLAG_VISIBLE) != 0;
}

void fn_800E3D14(GSmodel* model, GSvec* out)
{
    GSvecCopy(out, &model->boundCenter);
}

void GSmodelGetScale(GSmodel* model, GSvec* out)
{
    GSvecCopy(out, &model->scale);
}

void GSmodelGetRotation(GSmodel* model, GSvec* out)
{
    GSvecCopy(out, &model->rotation);
}

void GSmodelGetPosition(GSmodel* model, GSvec* out)
{
    GSvecCopy(out, &model->position);
}

void fn_800E3DC4(GSmodel* model, const GSvec* rotation)
{
    GSvec newRotation;
    HSDJObj* jobj;
    f32 value;

    GSvecCopy(&newRotation, &model->rotation);
    GSvecAdd(&newRotation, &newRotation, rotation);

    if (model->transformOverride == 0) {
        GSvecCopy(&model->rotation, &newRotation);

        jobj = model->renderJObj;
        value = newRotation.x;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x2A4, lbl_8047CB68);
        }

        if (jobj->flags & JOBJ_USE_QUATERNION) {
            __assert(lbl_8047CB60, 0x2A5, lbl_80270E28);
        }

        jobj->rotation.x = value;
        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }

        jobj = model->renderJObj;
        value = newRotation.y;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x2B8, lbl_8047CB68);
        }

        if (jobj->flags & JOBJ_USE_QUATERNION) {
            __assert(lbl_8047CB60, 0x2B9, lbl_80270E28);
        }

        jobj->rotation.y = value;
        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }

        {
            HSDJObj* zJObj;

            zJObj = model->renderJObj;
            value = newRotation.z;
            if (zJObj == NULL) {
                __assert(lbl_8047CB60, 0x2CC, lbl_8047CB68);
            }

            if (zJObj->flags & JOBJ_USE_QUATERNION) {
                __assert(lbl_8047CB60, 0x2CD, lbl_80270E28);
            }

            zJObj->rotation.z = value;
            if (!(zJObj->flags & JOBJ_MTX_INDEP_SRT)) {
                if (zJObj != NULL) {
                    s32 dirty;
                    u32 flags;

                    if (zJObj == NULL) {
                        __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                    }

                    flags = zJObj->flags;
                    dirty = 0;
                    if (!(flags & JOBJ_USER_DEF_MTX)) {
                        if (flags & JOBJ_MTX_DIRTY) {
                            dirty = 1;
                        }
                    }

                    if (dirty == 0) {
                        fn_8019D620(zJObj);
                    }
                }
            }
        }
    } else {
        GSvecCopy(&model->overrideRotation, &newRotation);
    }
}

void GSmodelSetVisibility(GSmodel* model, u8 visible)
{
    if (visible) {
        model->flags.raw |= GSMODEL_FLAG_VISIBLE;
        if (model->linkedGSparticleBank != NULL) {
            fn_80118C20(model->linkedGSparticleBank, 1);
        }
    } else {
        model->flags.raw &= ~GSMODEL_FLAG_VISIBLE;
        if (model->linkedGSparticleBank != NULL) {
            fn_80118C20(model->linkedGSparticleBank, 0);
        }
    }
}

void GSmodelSetScale(GSmodel* model, GSvec* scale)
{
    if (model->transformOverride == 0) {
        HSDJObj* jobj;

        GSvecCopy(&model->scale, scale);

        jobj = model->renderJObj;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x316, lbl_8047CB68);
        }

        if (scale == NULL) {
            __assert(lbl_8047CB60, 0x317, lbl_8047CB70);
        }

        jobj->scale = *scale;

        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }
    } else {
        GSvecCopy(&model->overrideScale, scale);
    }
}

void GSmodelSetRotation(GSmodel* model, GSvec* rotation)
{
    HSDJObj* jobj;
    f32 value;

    if (model->transformOverride == 0) {
        GSvecCopy(&model->rotation, rotation);

        jobj = model->renderJObj;
        value = rotation->x;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x2A4, lbl_8047CB68);
        }

        if (jobj->flags & JOBJ_USE_QUATERNION) {
            __assert(lbl_8047CB60, 0x2A5, lbl_80270E28);
        }

        jobj->rotation.x = value;
        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }

        jobj = model->renderJObj;
        value = rotation->y;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x2B8, lbl_8047CB68);
        }

        if (jobj->flags & JOBJ_USE_QUATERNION) {
            __assert(lbl_8047CB60, 0x2B9, lbl_80270E28);
        }

        jobj->rotation.y = value;
        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }

        {
            HSDJObj* zJObj;

            zJObj = model->renderJObj;
            value = rotation->z;
            if (zJObj == NULL) {
                __assert(lbl_8047CB60, 0x2CC, lbl_8047CB68);
            }

            if (zJObj->flags & JOBJ_USE_QUATERNION) {
                __assert(lbl_8047CB60, 0x2CD, lbl_80270E28);
            }

            zJObj->rotation.z = value;
            if (!(zJObj->flags & JOBJ_MTX_INDEP_SRT)) {
                if (zJObj != NULL) {
                s32 dirty;
                u32 flags;

                if (zJObj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = zJObj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(zJObj);
                }
            }
        }
        }
    } else {
        GSvecCopy(&model->overrideRotation, rotation);
    }
}

void GSmodelSetPosition(GSmodel* model, GSvec* position)
{
    if (model->transformOverride == 0) {
        HSDJObj* jobj;

        GSvecCopy(&model->position, position);

        jobj = model->renderJObj;
        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x3A9, lbl_8047CB68);
        }

        if (position == NULL) {
            __assert(lbl_8047CB60, 0x3AA, lbl_80270E50);
        }

        jobj->translate = *position;

        if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
            if (jobj != NULL) {
                s32 dirty;
                u32 flags;

                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                }

                flags = jobj->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }

                if (dirty == 0) {
                    fn_8019D620(jobj);
                }
            }
        }
    } else {
        GSvecCopy(&model->overridePosition, position);
    }

    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
    } else {
        GSpart* part;

        part = GSmodelGetPart(model, 1);
        if (part == NULL) {
            memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
        } else {
            GSpartGetTransform(part, &model->boundCenter, NULL, NULL);
            GSpartFree(part);
            model->boundCenter.y -= model->boundYOffset;
        }
    }
}

void* GSmodelGetMatrixPtr(GSmodel* model)
{
    HSDJObj* jobj;

    jobj = model->renderJObj;
    if (jobj != NULL) {
        s32 dirty;
        u32 flags;

        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
        }

        flags = jobj->flags;
        dirty = 0;
        if (!(flags & JOBJ_USER_DEF_MTX)) {
            if (flags & JOBJ_MTX_DIRTY) {
                dirty = 1;
            }
        }

        if (dirty != 0) {
            fn_8019D9DC(jobj);
        }
    }

    return &model->renderJObj->matrix;
}

void GSmodelSetMatrix(GSmodel* model, const f32 matrix[3][4])
{
    HSDJObj* jobj;

    jobj = model->renderJObj;
    if (jobj == NULL) {
        __assert(lbl_8047CB60, 0x495, lbl_8047CB68);
    }

    if (matrix == NULL) {
        __assert(lbl_8047CB60, 0x496, lbl_8047CB78);
    }

    PSMTXCopy(matrix, jobj->matrix);
    jobj->flags |= JOBJ_SET_MTX_FLAGS;

    if (jobj != NULL) {
        s32 dirty;
        u32 flags;

        if (jobj == NULL) {
            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
        }

        flags = jobj->flags;
        dirty = 0;
        if (!(flags & JOBJ_USER_DEF_MTX)) {
            if (flags & JOBJ_MTX_DIRTY) {
                dirty = 1;
            }
        }

        if (dirty == 0) {
            fn_8019D620(jobj);
        }
    }
}

void GSmodelDetachFromGSpart(GSmodel* model, u8 applyTransform)
{
    HSDJObj* jobj;
    f32 value;

    if (model->transformOverride != 0) {
        *(u32*)((u8*)model + 0x118) = 0;
        *(u32*)((u8*)model + 0x11C) = 0;
        model->flags.raw &= ~0xC0000U;
        model->transformOverride = 0;

        if (applyTransform != 0) {
            if (model->transformOverride == 0) {
                GSvecCopy(&model->position, &model->overridePosition);

                jobj = model->renderJObj;
                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x3A9, lbl_8047CB68);
                }
                if (&model->overridePosition == NULL) {
                    __assert(lbl_8047CB60, 0x3AA, lbl_80270E50);
                }
                jobj->translate = model->overridePosition;
                if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
                    if (jobj != NULL) {
                        s32 dirty;
                        u32 flags;

                        if (jobj == NULL) {
                            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                        }
                        flags = jobj->flags;
                        dirty = 0;
                        if (!(flags & JOBJ_USER_DEF_MTX) &&
                            (flags & JOBJ_MTX_DIRTY)) {
                            dirty = 1;
                        }
                        if (dirty == 0) {
                            fn_8019D620(jobj);
                        }
                    }
                }
            } else {
                GSvecCopy(&model->overridePosition, &model->overridePosition);
            }

            if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
                memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
            } else {
                GSpart* part;

                part = GSmodelGetPart(model, 1);
                if (part == NULL) {
                    memcpy(&model->boundCenter, &model->position,
                           sizeof(GSvec));
                } else {
                    GSpartGetTransform(part, &model->boundCenter, NULL, NULL);
                    GSpartFree(part);
                    model->boundCenter.y -= model->boundYOffset;
                }
            }

            if (model->transformOverride == 0) {
                GSvecCopy(&model->rotation, &model->overrideRotation);

                jobj = model->renderJObj;
                value = model->overrideRotation.x;
                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x2A4, lbl_8047CB68);
                }
                if (jobj->flags & JOBJ_USE_QUATERNION) {
                    __assert(lbl_8047CB60, 0x2A5, lbl_80270E28);
                }
                jobj->rotation.x = value;
                if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
                    if (jobj != NULL) {
                        s32 dirty;
                        u32 flags;

                        if (jobj == NULL) {
                            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                        }
                        flags = jobj->flags;
                        dirty = 0;
                        if (!(flags & JOBJ_USER_DEF_MTX) &&
                            (flags & JOBJ_MTX_DIRTY)) {
                            dirty = 1;
                        }
                        if (dirty == 0) {
                            fn_8019D620(jobj);
                        }
                    }
                }

                jobj = model->renderJObj;
                value = model->overrideRotation.y;
                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x2B8, lbl_8047CB68);
                }
                if (jobj->flags & JOBJ_USE_QUATERNION) {
                    __assert(lbl_8047CB60, 0x2B9, lbl_80270E28);
                }
                jobj->rotation.y = value;
                if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
                    if (jobj != NULL) {
                        s32 dirty;
                        u32 flags;

                        if (jobj == NULL) {
                            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                        }
                        flags = jobj->flags;
                        dirty = 0;
                        if (!(flags & JOBJ_USER_DEF_MTX) &&
                            (flags & JOBJ_MTX_DIRTY)) {
                            dirty = 1;
                        }
                        if (dirty == 0) {
                            fn_8019D620(jobj);
                        }
                    }
                }

                jobj = model->renderJObj;
                value = model->overrideRotation.z;
                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x2CC, lbl_8047CB68);
                }
                if (jobj->flags & JOBJ_USE_QUATERNION) {
                    __assert(lbl_8047CB60, 0x2CD, lbl_80270E28);
                }
                jobj->rotation.z = value;
                if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
                    if (jobj != NULL) {
                        s32 dirty;
                        u32 flags;

                        if (jobj == NULL) {
                            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                        }
                        flags = jobj->flags;
                        dirty = 0;
                        if (!(flags & JOBJ_USER_DEF_MTX) &&
                            (flags & JOBJ_MTX_DIRTY)) {
                            dirty = 1;
                        }
                        if (dirty == 0) {
                            fn_8019D620(jobj);
                        }
                    }
                }
            } else {
                GSvecCopy(&model->overrideRotation, &model->overrideRotation);
            }

            if (model->transformOverride == 0) {
                GSvecCopy(&model->scale, &model->overrideScale);

                jobj = model->renderJObj;
                if (jobj == NULL) {
                    __assert(lbl_8047CB60, 0x316, lbl_8047CB68);
                }
                if (&model->overrideScale == NULL) {
                    __assert(lbl_8047CB60, 0x317, lbl_8047CB70);
                }
                jobj->scale = model->overrideScale;
                if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
                    if (jobj != NULL) {
                        s32 dirty;
                        u32 flags;

                        if (jobj == NULL) {
                            __assert(lbl_8047CB60, 0x25D, lbl_8047CB68);
                        }
                        flags = jobj->flags;
                        dirty = 0;
                        if (!(flags & JOBJ_USER_DEF_MTX) &&
                            (flags & JOBJ_MTX_DIRTY)) {
                            dirty = 1;
                        }
                        if (dirty == 0) {
                            fn_8019D620(jobj);
                        }
                    }
                }
            } else {
                GSvecCopy(&model->overrideScale, &model->overrideScale);
            }
        }
    }
}
