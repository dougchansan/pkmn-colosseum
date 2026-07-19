/**
 * @file gs_model_main_suffix_800E4AC0.c
 * @brief GSmodel load, material, and null-joint suffix
 *
 * Target range 0x800E4AC0-0x800E8684; original source ordering is preserved.
 */
#include "dolphin/types.h"
#include "game/data/sdata2_8047CAA0.h"
#include "game/gs_model.h"
#include "game/gs_model_anim.h"
#include "game/gs_render.h"

#if !defined(GSMODEL_SUFFIX_ISOLATED)
#define GSMODEL_SUFFIX_ALL
#endif

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























































#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5188)
void* modelGetRenderJObj(GSmodel* model)
{
    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        return model->renderJObjAlt;
    }

    return model->renderJObj;
}
#endif

static inline GSmodel* modelGetFreeSlot(void)
{
    GSmodel* model;
    u32 i;

    model = lbl_8047AB74;
    for (i = 0; i < lbl_8047AB78; i++) {
        if (!(model->flags.raw & GSMODEL_FLAG_ACTIVE)) {
            return model;
        }
        model++;
    }
    return NULL;
}

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E51A4)
GSmodel* _modelLoad(GSmodelResource* resource, GSjobjDesc* joint,
                    void* boundAnim)
{
    GSmodel* model;

    (void)boundAnim;

    model = modelGetFreeSlot();
    if (model == NULL) {
        return NULL;
    }

    memset(model, 0, sizeof(GSmodel));
    model->flags.raw = GSMODEL_FLAG_ACTIVE | GSMODEL_FLAG_LINK_TEX_TO_ANIM;
    model->animCount = 0;
    model->texAnimCount = 0;
    model->centerNullState = 0;
    model->animEndedCallback = NULL;
    model->materialCount = 0;
    model->materialList = NULL;
    model->modulationRefCount = 0;
    model->unk158 = NULL;
    model->unk15C = NULL;
    model->shadowLight = NULL;
    model->unk164 = 0;
    model->transformOverride = 0;
    model->unk118 = NULL;
    model->unk11C = NULL;
    model->gsParticleLinkAttachMode = 1;

    model->resource = resource;
    model->renderJObj = HSD_JObjLoadJoint(joint);
    model->renderJObjAlt = NULL;

    set__5GSvecFfff(&model->position, model->renderJObj->translate.x,
                    model->renderJObj->translate.y,
                    model->renderJObj->translate.z);
    set__5GSvecFfff(&model->rotation, model->renderJObj->rotation.x,
                    model->renderJObj->rotation.y,
                    model->renderJObj->rotation.z);
    set__5GSvecFfff(&model->scale, model->renderJObj->scale.x,
                    model->renderJObj->scale.y, model->renderJObj->scale.z);
    set__5GSvecFfff(&model->boundCenter, model->renderJObj->translate.x,
                    model->renderJObj->translate.y,
                    model->renderJObj->translate.z);

    model->partAnimMix[0].type = 0;
    model->partAnimMix[0].partIndex = -1;
    model->partAnimMix[1].type = 0;
    model->partAnimMix[1].partIndex = -1;
    model->partAnimMix[2].type = 0;
    model->partAnimMix[2].partIndex = -1;
    model->partAnimMix[3].type = 0;
    model->partAnimMix[3].partIndex = -1;

    if (model->resource->shapeAnims != NULL) {
        model->flags.raw |= GSMODEL_FLAG_HAS_SHAPE_ANIM;
    }

    if (model->resource->anims != NULL) {
        model->flags.raw |= GSMODEL_FLAG_HAS_ANIM;
        model->animIndex = -1;
        while (model->resource->anims[model->animCount] != NULL) {
            model->animCount++;
        }
        GSmodelSetAnimIndex(model, 0);
        GSmodelSetAnimRate(model, lbl_8047CB84);
        GSmodelSetAnimType(model, 1);
    }

    if (model->resource->texAnims != NULL) {
        model->flags.raw |= GSMODEL_FLAG_HAS_TEX_ANIM;
        model->texAnimIndex = -1;
        while (model->resource->texAnims[model->texAnimCount] != NULL) {
            model->texAnimCount++;
        }
        GSmodelSetTexAnimIndex(model, 0);
        GSmodelSetTexAnimRate(model, lbl_8047CB84);
        GSmodelSetTexAnimType(model, 1);
    }

    fn_8019147C(&model->bound, &model->boundCenter);
    fn_80191474(&model->bound, &model->rotation);
    fn_8019146C(&model->bound, &model->scale);
    GSmodelRecalculateBound(model);

    model->boundYOffset = lbl_8047CB7C;
    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
    } else {
        GSpart* part = GSmodelGetPart(model, 1);

        if (part == NULL) {
            memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
        } else {
            GSpartGetTransform(part, &model->boundCenter, NULL, NULL);
            GSpartFree(part);
            model->boundCenter.y -= model->boundYOffset;
        }
    }

    model->boundYOffset = model->boundCenter.y;
    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
    } else {
        GSpart* part = GSmodelGetPart(model, 1);

        if (part == NULL) {
            memcpy(&model->boundCenter, &model->position, sizeof(GSvec));
        } else {
            GSpartGetTransform(part, &model->boundCenter, NULL, NULL);
            GSpartFree(part);
            model->boundCenter.y -= model->boundYOffset;
        }
    }

    fn_8019F1C4(model->renderJObj, &model->vertexCount, &model->polygonCount);
    return model;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4D18)
GSmodel* GSmodelLoad(GSmodelResource* resource, void* unused, void* boundAnim)
{
    (void)unused;
    return _modelLoad(resource, resource->joint, boundAnim);
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4D3C)
void GSmodelInit(u32 count)
{
    u16 handle;

    lbl_8047AB78 = count;
    handle = _toolentryAlloc__FUl(count * sizeof(GSmodel));
    lbl_8047AB70 = handle;

    if (handle != 0) {
        u32 i;

        lbl_8047AB74 = fn_800E27B0(handle);

        i = 0;
        while (i < lbl_8047AB78) {
            lbl_8047AB74[i].flags.raw = 0;
            i++;
        }

        modelShadowInit__Fv();
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4BF4)
void GSmodelFree(GSmodel* model)
{
    modelShadowFreeModelList__FP8_GSmodel(model);
    fn_800E85E8(model);
    GSmodelRemoveNull(model);

    if (model->centerNullState != 0) {
        model->centerNullState = 1;
        modelRemoveCenterNull(model);
    }

    fn_801A05EC(model->renderJObj);

    if (model->renderJObjAlt != NULL) {
        fn_801A05EC(model->renderJObjAlt);
        fn_801A05EC(model->jobj10);
        fn_801A05EC(model->jobj14);
    }

    if (model->linkedGSparticleBank != NULL) {
        fn_80118874(model->linkedGSparticleBank, 1);
    }

    memset(model, 0, sizeof(GSmodel));
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4C98)
GSmodel* GSmodelClone(GSmodel* model)
{
    GSjobjDesc desc;
    GSmodelResource* resource;
    GSjobjDesc* root;
#if !defined(GSMODEL_SUFFIX_ALL)
    f32 one;
    f32 zero;

    zero = lbl_8047CB7C;
    one = lbl_8047CB80;
#endif
    desc.className = NULL;
    resource = model->resource;
    root = resource->joint;
    desc.flags = root->flags | GSJOBJ_DESC_FLAG_INSTANCE;
    desc.child = root;
    desc.next = NULL;
    desc.dobj = NULL;
#if defined(GSMODEL_SUFFIX_ALL)
    desc.rotation.x = 0.0f;
    desc.rotation.y = 0.0f;
    desc.rotation.z = 0.0f;
    desc.scale.x = 1.0f;
    desc.scale.y = 1.0f;
    desc.scale.z = 1.0f;
    desc.position.x = 0.0f;
    desc.position.y = 0.0f;
    desc.position.z = 0.0f;
#else
    desc.rotation.x = zero;
    desc.rotation.y = zero;
    desc.rotation.z = zero;
    desc.scale.x = one;
    desc.scale.y = one;
    desc.scale.z = one;
    desc.position.x = zero;
    desc.position.y = zero;
    desc.position.z = zero;
#endif
    desc.robj = NULL;
    desc.aobj = NULL;

    return _modelLoad(resource, &desc, root);
}
#endif





#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4DB0)
void modelSetScl(GSmodel* model, GSvec* scale)
{
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
}
#endif





#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E4E8C)
void modelSetRot(GSmodel* model, GSvec* rotation)
{
    HSDJObj* jobj;
    f32 value;

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
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E50A8)
void modelSetPos(GSmodel* model, GSvec* position)
{
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
}
#endif





#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E7290)
void GSmodelGetRootPosition(GSmodel* model, GSvec* out)
{
    HSDJObj* jobj;

    jobj = model->renderJObj;
    if (model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED) {
        HSDJObj* child;

        child = jobj->child;
        if (child == NULL) {
            __assert(lbl_8047CB9C, 0x3E4, lbl_8047CBA4);
        }

        if (out == NULL) {
            __assert(lbl_8047CB9C, 0x3E5, lbl_80270E60);
        }

        *out = child->translate;
    } else {
        clear__5GSvecFv(out);
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5978)
u32 GSmodelIsModulationEnabled(GSmodel* model)
{
    void* material;

    if (model->materialCount == 0) {
        return 0;
    }

    material = model->materialList->materials[0];
    if (material == NULL) {
        return 0;
    }

    return GSmaterialGetEnabledExtensions(material) & 1;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E59C8)
void GSmodelGetModulationColor(GSmodel* model, GScolor* color)
{
    u8 enabled;

    if (model->materialCount == 0) {
        enabled = 0;
    } else {
        void* material;

        material = model->materialList->materials[0];
        if (material == NULL) {
            enabled = 0;
        } else if (GSmaterialGetEnabledExtensions(material) & 1) {
            enabled = 1;
        } else {
            enabled = 0;
        }
    }

    if (enabled) {
        GSmaterialGetModulate(model->materialList->materials[0], color);
    } else {
        color->b = 0x7f;
        color->g = 0x7f;
        color->r = 0x7f;
        color->a = 0xff;
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5B68)
void GSmodelSetModulationColor(GSmodel* model, const GScolor* color)
{
    u32 i;
    u16 count;
    GSmodelMaterialList* materialEntry;

    i = 0;
    count = model->materialCount;
    materialEntry = model->materialList;

    while ((u16)i < count) {
        void* material;

        material = materialEntry->materials[0];
        if (material != NULL) {
            GSmaterialSetModulate(material, color);
        }

        i++;
        materialEntry++;
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5FAC)
u32 GSmodelIsEnvMapEnabled(GSmodel* model)
{
    void* material;

    if (model->materialCount == 0) {
        return 0;
    }

    material = model->materialList->materials[0];
    if (material == NULL) {
        return 0;
    }

    return (GSmaterialGetEnabledExtensions(material) >> 2) & 1;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E60F0)
void GSmodelSetEnvMapBlendValue(GSmodel* model, f32 value)
{
    u8 enabled;

    if (model->materialCount == 0) {
        enabled = 0;
    } else {
        void* material;

        material = model->materialList->materials[0];
        if (material == NULL) {
            enabled = 0;
        } else if (GSmaterialGetEnabledExtensions(material) & 4) {
            enabled = 1;
        } else {
            enabled = 0;
        }
    }

    if (enabled) {
        u32 i;
        s32 count;

        count = model->materialCount;
        if (count != 0) {
            GSmodelMaterialList* materialEntry;

            materialEntry = model->materialList;
            i = 0;
            while ((s32)i < count) {
                void* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    GSmaterialSetEnvMapBlendValue(material, value);
                }

                i++;
                materialEntry++;
            }
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5550)
void GSmodelResetTextureChange(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        u32 handle;
        s32 i;
        s32 count;
        GSmodelMaterialList* materialEntry;

        materialEntry = model->materialList;
        count = model->materialCount;
        i = 0;
        model->modulationRefCount = refs - 1;

        while (i < count) {
            void* material;

            material = materialEntry->materials[0];
            if (material != NULL) {
                GSmaterialResetTexture(material);
            }

            i++;
            materialEntry++;
        }

        if (model->modulationRefCount == 0) {
            count = model->materialCount;
            i = 0;
            materialEntry = model->materialList;

            while (i < count) {
                void* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                materialEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5790)
void fn_800E5790(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        GSmodelMaterialList* materialEntry;
        s32 count;
        s32 i;
        u32 handle;

        model->modulationRefCount = refs - 1;

        if (model->modulationRefCount == 0) {
            count = model->materialCount;
            i = 0;
            materialEntry = model->materialList;

            while (i < count) {
                GSmaterial* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                materialEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5A74)
void GSmodelDisableModulation(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        GSmodelMaterialList* materialEntry;
        s32 count;

        materialEntry = model->materialList;
        count = model->materialCount;
        model->modulationRefCount = refs - 1;

        if (model->modulationRefCount == 0) {
            s32 cleanupCount;
            GSmodelMaterialList* cleanupEntry;
            s32 i;
            u32 handle;

            cleanupCount = model->materialCount;
            i = 0;
            cleanupEntry = model->materialList;

            while (i < cleanupCount) {
                GSmaterial* material;

                material = cleanupEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                cleanupEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        } else {
            s32 i;

            i = 0;
            while (i < count) {
                GSmaterial* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    GSmaterialDisableExtension(material, 1);
                }

                i++;
                materialEntry++;
            }
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5D40)
void GSmodelDisableColorSwap(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        GSmodelMaterialList* materialEntry;
        s32 count;

        materialEntry = model->materialList;
        count = model->materialCount;
        model->modulationRefCount = refs - 1;

        if (model->modulationRefCount == 0) {
            s32 cleanupCount;
            GSmodelMaterialList* cleanupEntry;
            s32 i;
            u32 handle;

            cleanupCount = model->materialCount;
            i = 0;
            cleanupEntry = model->materialList;

            while (i < cleanupCount) {
                GSmaterial* material;

                material = cleanupEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                cleanupEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        } else {
            s32 i;

            i = 0;
            while (i < count) {
                GSmaterial* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    GSmaterialDisableExtension(material, 2);
                }

                i++;
                materialEntry++;
            }
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E5FFC)
void GSmodelDisableEnvMap(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        GSmodelMaterialList* materialEntry;
        s32 count;

        materialEntry = model->materialList;
        count = model->materialCount;
        model->modulationRefCount = refs - 1;

        if (model->modulationRefCount == 0) {
            s32 cleanupCount;
            GSmodelMaterialList* cleanupEntry;
            s32 i;
            u32 handle;

            cleanupCount = model->materialCount;
            i = 0;
            cleanupEntry = model->materialList;

            while (i < cleanupCount) {
                GSmaterial* material;

                material = cleanupEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                cleanupEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        } else {
            s32 i;

            i = 0;
            while (i < count) {
                GSmaterial* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    GSmaterialDisableExtension(material, 4);
                }

                i++;
                materialEntry++;
            }
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E638C)
void GSmodelResetPEdescr(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        u32 handle;
        s32 i;
        s32 count;
        GSmodelMaterialList* materialEntry;

        materialEntry = model->materialList;
        count = model->materialCount;
        i = 0;
        model->modulationRefCount = refs - 1;

        while (i < count) {
            void* material;

            material = materialEntry->materials[0];
            if (material != NULL) {
                GSmaterialResetPEdescr(material);
            }

            i++;
            materialEntry++;
        }

        if (model->modulationRefCount == 0) {
            count = model->materialCount;
            i = 0;
            materialEntry = model->materialList;

            while (i < count) {
                void* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                materialEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E65CC)
void GSmodelResetMaterialAlpha(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        u32 handle;
        s32 i;
        s32 count;
        GSmodelMaterialList* materialEntry;

        materialEntry = model->materialList;
        count = model->materialCount;
        i = 0;
        model->modulationRefCount = refs - 1;

        while (i < count) {
            void* material;

            material = materialEntry->materials[0];
            if (material != NULL) {
                GSmaterialResetAlpha(material);
            }

            i++;
            materialEntry++;
        }

        if (model->modulationRefCount == 0) {
            count = model->materialCount;
            i = 0;
            materialEntry = model->materialList;

            while (i < count) {
                void* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                materialEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E6804)
void GSmodelSetMaterialAlpha(GSmodel* model, f32 alpha)
{
    if (model->modulationRefCount != 0) {
        s32 i;
        s32 count;
        GSmodelMaterialList* materialEntry;

        count = model->materialCount;
        i = 0;
        materialEntry = model->materialList;

        while (i < count) {
            GSmaterial* material;

            material = materialEntry->materials[0];
            if (material != NULL) {
                GSmaterialSetAlpha(material, ((f32)material->alpha * alpha) / 255.0f);
            }

            i++;
            materialEntry++;
        }
    }
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E68D8)
void GSmodelResetRenderFlags(GSmodel* model)
{
    u16 refs;

    refs = model->modulationRefCount;
    if (refs != 0) {
        u32 handle;
        s32 i;
        s32 count;
        GSmodelMaterialList* materialEntry;

        materialEntry = model->materialList;
        count = model->materialCount;
        i = 0;
        model->modulationRefCount = refs - 1;

        while (i < count) {
            void* material;

            material = materialEntry->materials[0];
            if (material != NULL) {
                GSmaterialResetFlags(material);
            }

            i++;
            materialEntry++;
        }

        if (model->modulationRefCount == 0) {
            count = model->materialCount;
            i = 0;
            materialEntry = model->materialList;

            while (i < count) {
                void* material;

                material = materialEntry->materials[0];
                if (material != NULL) {
                    fn_800DF608(material);
                }

                i++;
                materialEntry++;
            }

            handle = model->materialListHandle;
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }

            model->materialListHandle = 0;
            model->materialCount = 0;
            model->materialList = NULL;
        }
    }
}
#endif

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

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E732C)
void GSmodelAddNull(GSmodel* model, const GSvec* position,
                    const GSvec* rotation, const GSvec* scale)
{
    HSDJObj* root;
    HSDJObj* null;
    GSvec zero;
    GSvec one;

    if (model->flags.raw & GSMODEL_FLAG_RENDER_ALT_JOBJ) {
        return;
    }

    root = model->renderJObj;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    one.x = 1.0f;
    one.y = 1.0f;
    one.z = 1.0f;

    if (!(model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED)) {
        GSvec oldPosition;
        GSvec oldRotation;
        GSvec oldScale;

        null = fn_8019F718();
        if (null == NULL) {
            return;
        }

        if (root == NULL) {
            __assert(lbl_8047CB9C, 0x3E4, lbl_8047CBA4);
        }
        oldPosition = root->translate;

        if (root == NULL) {
            __assert(lbl_8047CB9C, 0x2EC, lbl_8047CBA4);
        }
        oldRotation.x = root->rotation.x;
        if (root == NULL) {
            __assert(lbl_8047CB9C, 0x2FA, lbl_8047CBA4);
        }
        oldRotation.y = root->rotation.y;
        if (root == NULL) {
            __assert(lbl_8047CB9C, 0x308, lbl_8047CBA4);
        }
        oldRotation.z = root->rotation.z;

        if (root == NULL) {
            __assert(lbl_8047CB9C, 0x351, lbl_8047CBA4);
        }
        oldScale = root->scale;

        GSMODEL_JOBJ_SET_POSITION(root, position != NULL ? position : &zero);
        GSMODEL_JOBJ_SET_ROTATION(root, rotation != NULL ? rotation : &zero);
        GSMODEL_JOBJ_SET_SCALE(root, scale != NULL ? scale : &one);

        if (root != NULL && !(root->flags & JOBJ_USER_DEF_MTX) &&
            (root->flags & JOBJ_MTX_DIRTY)) {
            fn_8019D9DC(root);
        }

        HSD_JObjAddChild(null, root);
        model->flags.raw |= GSMODEL_FLAG_ROOT_NULL_ADDED;
        model->renderJObj = null;
        fn_8019FE8C(null, HSD_JObjGetFlags(root));

        GSMODEL_JOBJ_SET_POSITION(null, &oldPosition);
        GSMODEL_JOBJ_SET_ROTATION(null, &oldRotation);
        GSMODEL_JOBJ_SET_SCALE(null, &oldScale);

        if (root != NULL && !(root->flags & JOBJ_USER_DEF_MTX) &&
            (root->flags & JOBJ_MTX_DIRTY)) {
            fn_8019D9DC(root);
        }
        if (null != NULL && !(null->flags & JOBJ_USER_DEF_MTX) &&
            (null->flags & JOBJ_MTX_DIRTY)) {
            fn_8019D9DC(null);
        }
    } else {
        null = root != NULL ? root->child : NULL;
        GSMODEL_JOBJ_SET_POSITION(null, position != NULL ? position : &zero);
        GSMODEL_JOBJ_SET_ROTATION(null, rotation != NULL ? rotation : &zero);
        GSMODEL_JOBJ_SET_SCALE(null, scale != NULL ? scale : &one);

        if (null != NULL && !(null->flags & JOBJ_USER_DEF_MTX) &&
            (null->flags & JOBJ_MTX_DIRTY)) {
            fn_8019D9DC(null);
        }
    }

    GSmodelSetAnimIndex(model, model->animIndex);
    GSmodelSetAnimType(model, model->animType);
    GSmodelStartAnimation(model);
}
#endif

#undef GSMODEL_JOBJ_SET_SCALE
#undef GSMODEL_JOBJ_SET_ROTATION
#undef GSMODEL_JOBJ_SET_ROTATION_COMPONENT
#undef GSMODEL_JOBJ_SET_POSITION
#undef GSMODEL_JOBJ_FLUSH

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E85E8)
void fn_800E85E8(GSmodel* model)
{
    u32 handle;
    s32 i;
    s32 count;
    GSmodelMaterialList* materialEntry;

    i = 0;
    count = model->materialCount;
    materialEntry = model->materialList;

    while (i < count) {
        void* material;

        material = materialEntry->materials[0];
        if (material != NULL) {
            fn_800DF608(material);
        }

        i++;
        materialEntry++;
    }

    handle = model->materialListHandle;
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }

    model->materialListHandle = 0;
    model->materialCount = 0;
    model->materialList = NULL;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E6B20)
void modelRemoveCenterNull(GSmodel* model)
{
    HSDJObj* jobj;
    HSDJObj* cur;

    if (model->centerNullState == 0) {
        return;
    }

    model->centerNullState--;
    if (model->centerNullState != 0) {
        return;
    }

    jobj = model->renderJObj;
    if (model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED) {
        jobj = jobj->child;
    }

    cur = (jobj == NULL) ? NULL : jobj->child;
    if (cur == NULL) {
        return;
    }
    for (; ((cur == NULL) ? NULL : cur->next) != NULL;
         cur = (cur == NULL) ? NULL : cur->next) {
    }
    fn_801A02B0(cur);
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E6BC8)
u8 GSmodelCenterNull(GSmodel* model)
{
    HSDJObj* jobj;
    HSDJObj* null;
    GSvec position;

    if (model->centerNullState == 0) {
        jobj = model->renderJObj;
        null = fn_8019F718();
        if (null == NULL) {
            return 0;
        }

        if (model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED) {
            jobj = jobj->child;
        }
        jobj = jobj == NULL ? NULL : jobj->child;

        if (jobj == NULL) {
            HSD_JObjAddChild(jobj, null);
        } else {
            while ((jobj == NULL ? NULL : jobj->next) != NULL) {
                jobj = jobj == NULL ? NULL : jobj->next;
            }
            HSD_JObjAddNext(jobj, null);
        }

        fn_8019FE8C(null, HSD_JObjGetFlags(jobj));
        GSlerpGetLinearInterpolationVector(&position, &model->bound.centerStart,
                                           &model->bound.centerEnd,
                                           lbl_8047CB98);

        if (null == NULL) {
            __assert(lbl_8047CB9C, 0x3A9, lbl_8047CBA4);
        }
        if (&position == NULL) {
            __assert(lbl_8047CB9C, 0x3AA, lbl_80270E60);
        }
        null->translate = position;

        if (!(null->flags & JOBJ_MTX_INDEP_SRT)) {
            if (null != NULL) {
                s32 dirty;
                u32 flags;

                if (null == NULL) {
                    __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
                }
                flags = null->flags;
                dirty = 0;
                if (!(flags & JOBJ_USER_DEF_MTX)) {
                    if (flags & JOBJ_MTX_DIRTY) {
                        dirty = 1;
                    }
                }
                if (dirty == 0) {
                    fn_8019D620(null);
                }
            }
        }

        if (null != NULL) {
            s32 dirty;
            u32 flags;

            if (null == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }
            flags = null->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }
            if (dirty != 0) {
                fn_8019D9DC(null);
            }
        }
    }

    model->centerNullState++;
    return 1;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E6DC0)
u32 GSmodelIsRootNullAdded(GSmodel* model)
{
    return (model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED) != 0;
}
#endif

#if defined(GSMODEL_SUFFIX_ALL) || defined(GSMODEL_SUFFIX_SELECT_800E6DCC)
void GSmodelRemoveNull(GSmodel* model)
{
    HSDJObj* child;
    HSDJObj* root;
    HSDJObj* newRoot;
    GSvec position;
    GSvec scale;
    f32 rotX;
    f32 rotY;
    f32 rotZ;

    if (!(model->flags.raw & GSMODEL_FLAG_ROOT_NULL_ADDED)) {
        return;
    }

    root = model->renderJObj;
    child = (root == NULL) ? NULL : root->child;

    if (root == NULL) {
        __assert(lbl_8047CB9C, 0x3E4, lbl_8047CBA4);
    }

    if (&position == NULL) {
        __assert(lbl_8047CB9C, 0x3E5, lbl_80270E60);
    }
    position = root->translate;

    if (root == NULL) {
        __assert(lbl_8047CB9C, 0x2EC, lbl_8047CBA4);
    }
    rotX = root->rotation.x;

    if (root == NULL) {
        __assert(lbl_8047CB9C, 0x2FA, lbl_8047CBA4);
    }
    rotY = root->rotation.y;

    if (root == NULL) {
        __assert(lbl_8047CB9C, 0x308, lbl_8047CBA4);
    }
    rotZ = root->rotation.z;

    if (root == NULL) {
        __assert(lbl_8047CB9C, 0x351, lbl_8047CBA4);
    }
    if (&scale == NULL) {
        __assert(lbl_8047CB9C, 0x352, lbl_8047CBAC);
    }
    scale = root->scale;

    if (child == NULL) {
        __assert(lbl_8047CB9C, 0x3A9, lbl_8047CBA4);
    }
    if (&position == NULL) {
        __assert(lbl_8047CB9C, 0x3AA, lbl_80270E60);
    }
    child->translate = position;
    if (!(child->flags & JOBJ_MTX_INDEP_SRT)) {
        if (child != NULL) {
            s32 dirty;
            u32 flags;

            if (child == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }

            flags = child->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }

            if (dirty == 0) {
                fn_8019D620(child);
            }
        }
    }

    if (child == NULL) {
        __assert(lbl_8047CB9C, 0x2A4, lbl_8047CBA4);
    }
    if (child->flags & JOBJ_USE_QUATERNION) {
        __assert(lbl_8047CB9C, 0x2A5, lbl_80270E6C);
    }
    child->rotation.x = rotX;
    if (!(child->flags & JOBJ_MTX_INDEP_SRT)) {
        if (child != NULL) {
            s32 dirty;
            u32 flags;

            if (child == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }

            flags = child->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }

            if (dirty == 0) {
                fn_8019D620(child);
            }
        }
    }

    if (child == NULL) {
        __assert(lbl_8047CB9C, 0x2B8, lbl_8047CBA4);
    }
    if (child->flags & JOBJ_USE_QUATERNION) {
        __assert(lbl_8047CB9C, 0x2B9, lbl_80270E6C);
    }
    child->rotation.y = rotY;
    if (!(child->flags & JOBJ_MTX_INDEP_SRT)) {
        if (child != NULL) {
            s32 dirty;
            u32 flags;

            if (child == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }

            flags = child->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }

            if (dirty == 0) {
                fn_8019D620(child);
            }
        }
    }

    if (child == NULL) {
        __assert(lbl_8047CB9C, 0x2CC, lbl_8047CBA4);
    }
    if (child->flags & JOBJ_USE_QUATERNION) {
        __assert(lbl_8047CB9C, 0x2CD, lbl_80270E6C);
    }
    child->rotation.z = rotZ;
    if (!(child->flags & JOBJ_MTX_INDEP_SRT)) {
        if (child != NULL) {
            s32 dirty;
            u32 flags;

            if (child == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }

            flags = child->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }

            if (dirty == 0) {
                fn_8019D620(child);
            }
        }
    }

    if (child == NULL) {
        __assert(lbl_8047CB9C, 0x316, lbl_8047CBA4);
    }
    if (&scale == NULL) {
        __assert(lbl_8047CB9C, 0x317, lbl_8047CBAC);
    }
    child->scale = scale;
    if (!(child->flags & JOBJ_MTX_INDEP_SRT)) {
        if (child != NULL) {
            s32 dirty;
            u32 flags;

            if (child == NULL) {
                __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
            }

            flags = child->flags;
            dirty = 0;
            if (!(flags & JOBJ_USER_DEF_MTX)) {
                if (flags & JOBJ_MTX_DIRTY) {
                    dirty = 1;
                }
            }

            if (dirty == 0) {
                fn_8019D620(child);
            }
        }
    }

    newRoot = fn_801A02B0(root);

    if (child != NULL) {
        s32 dirty;
        u32 flags;

        if (child == NULL) {
            __assert(lbl_8047CB9C, 0x25D, lbl_8047CBA4);
        }

        flags = child->flags;
        dirty = 0;
        if (!(flags & JOBJ_USER_DEF_MTX)) {
            if (flags & JOBJ_MTX_DIRTY) {
                dirty = 1;
            }
        }

        if (dirty != 0) {
            fn_8019D9DC(child);
        }
    }

    model->flags.raw &= ~GSMODEL_FLAG_ROOT_NULL_ADDED;
    model->renderJObj = newRoot;
}
#endif
