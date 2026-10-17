/**
 * GSmodel blend range 0x800EB5A0 - 0x800EBEEC (XD blend.o:
 * modelCalculateBlendModel, _modelIntpJObjAll, _modelIntpJObj; NXXJ01.map /
 * GXXE01.map, StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af).
 *
 * Shared by two objects: gs_model_bound_r55_800EB5A0_gc13_o2.c defines
 * GS_MODEL_BOUND_EMIT_BLEND_MODEL (modelCalculateBlendModel, 0x800EB5A0) and
 * gs_model_bound_r55_800EB6E0_suffix.c defines GS_MODEL_BOUND_EMIT_INTP
 * (_modelIntpJObjAll and fn_800EB904, 0x800EB6E0). gs_model_bound.c supplies
 * the types only. Both build with GC/1.3.2: its -inline auto expands the
 * recursive _modelIntpJObjAll into itself four levels, as retail does, while
 * GC/1.3 does not inline the recursion at all; every other exact GSmodel
 * function in this lane also compiles identically under GC/1.3.2.
 *
 * fn_800EB904 is Colosseum's _modelIntpJObj: XD's modelIntpJObjTrans/Scale/
 * Rotate bodies, written here through the jobj.h getters and setters, plus
 * the part-anim-mix Euler override.
 */
#define GS_MODEL_BOUND_800EB464_SUFFIX_ACTIVE
#define GS_MODEL_BOUND_DECLARATIONS_ONLY
#include "src/game/gs_model_bound.c"

typedef f32 GSmtx[3][4];

extern u32 lbl_8047ABA0;
extern const char lbl_8047CC40[7];
extern const char lbl_8047CC48[5];
extern void _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
    GSmodel*, struct ModelIntpJObj*, struct ModelIntpJObj*,
    struct ModelIntpJObj*, f32);
extern void fn_800E0560(GSmtx, const GSvec*);
extern void GSmtxMakeXRotation(GSmtx, f32);
extern void GSmtxMakeYRotation(GSmtx, f32);
extern void GSmtxMakeZRotation(GSmtx, f32);
extern void fn_800E042C(GSmtx, const GSvec*);
extern void GSmtxConcat(GSmtx, GSmtx, GSmtx);

typedef struct ModelQuat {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} ModelQuat;

/* The HSD_JObj fields this range touches (Colosseum's 0x88-byte HSD_JObj). */
typedef struct ModelIntpJObj {
    u8 pad_00[8];
    struct ModelIntpJObj* next;   /* 0x08 */
    struct ModelIntpJObj* parent; /* 0x0C */
    struct ModelIntpJObj* child;  /* 0x10 */
    u32 flags;                    /* 0x14 */
    void* u;                      /* 0x18 */
    ModelQuat rotate;             /* 0x1C */
    GSvec scale;                  /* 0x2C */
    GSvec translate;              /* 0x38 */
} ModelIntpJObj;

/*
 * jobj.h inlines (Colosseum's jobj.h: HSD_JObjMtxIsDirty asserts at line 605,
 * HSD_JObjGetMtxPtr at 1148; see include/sysdolphin/baselib/jobj.h). The
 * header's HSD_ASSERT would pool its own "jobj.h"/"jobj" strings, which this
 * carve does not own.
 * RULE-EXCEPTION(title-path): extern named stand-ins for the TU's own pooled
 * assert strings (lbl_8047CC40/lbl_8047CC48) - see docs/RULE_EXCEPTIONS.md
 */
static inline BOOL HSD_JObjMtxIsDirty(ModelIntpJObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x25D, lbl_8047CC48);
    }
    result = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

static inline void HSD_JObjSetupMatrix(ModelIntpJObj* jobj)
{
    if (jobj == NULL || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub((HSDJObj*)jobj);
}

static inline GSmtx* HSD_JObjGetMtxPtr(HSDJObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x47C, lbl_8047CC48);
    }
    HSD_JObjSetupMatrix((ModelIntpJObj*)jobj);
    return (GSmtx*)jobj->matrix;
}

#if defined(GS_MODEL_BOUND_EMIT_INTP)

extern void fn_8019D620(ModelIntpJObj* jobj);        /* HSD_JObjSetMtxDirtySub */
extern void EulerToQuat(GSvec* euler, ModelQuat* q); /* EulerToQuat */
extern void fn_801AD7CC(ModelQuat* from, ModelQuat* to, ModelQuat* out,
                        f32 t);                      /* QuatSlerp */
/* RULE-EXCEPTION(title-path): extern named stand-in for the TU's pooled
 * 1.0f - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047CC50; /* 1.0f */

static inline void HSD_JObjSetMtxDirty(ModelIntpJObj* jobj)
{
    if (jobj != NULL && !HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D620(jobj);
    }
}

static inline void HSD_JObjGetRotation(ModelIntpJObj* jobj, ModelQuat* quat)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x2DD, lbl_8047CC48);
    }
    *quat = jobj->rotate;
}

static inline void HSD_JObjSetRotation(ModelIntpJObj* jobj, ModelQuat* quat)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x290, lbl_8047CC48);
    }
    jobj->rotate = *quat;
    if (!(jobj->flags & 0x02000000)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjGetScale(ModelIntpJObj* jobj, GSvec* scale)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x351, lbl_8047CC48);
    }
    *scale = jobj->scale;
}

static inline void HSD_JObjSetScale(ModelIntpJObj* jobj, GSvec* scale)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x316, lbl_8047CC48);
    }
    jobj->scale = *scale;
    if (!(jobj->flags & 0x02000000)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjGetTranslation(ModelIntpJObj* jobj, GSvec* translate)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x3E4, lbl_8047CC48);
    }
    *translate = jobj->translate;
}

static inline void HSD_JObjSetTranslate(ModelIntpJObj* jobj, GSvec* translate)
{
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x3A9, lbl_8047CC48);
    }
    jobj->translate = *translate;
    if (!(jobj->flags & 0x02000000)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

typedef struct ModelPartAnimMix {
    s32 type;
    u32 index;
    GSvec* value;
} ModelPartAnimMix;

void fn_800EB904(GSmodel* model, ModelIntpJObj* from, ModelIntpJObj* to,
                 ModelIntpJObj* out, f32 blend);

void _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
    GSmodel* model, ModelIntpJObj* from, ModelIntpJObj* to, ModelIntpJObj* out,
    f32 blend)
{
    ModelIntpJObj* f;
    ModelIntpJObj* t;
    ModelIntpJObj* o;

    if (from == NULL || to == NULL || out == NULL) {
        return;
    }
    fn_800EB904(model, from, to, out, blend);
    if (!(from->flags & 0x1000)) {
        f = from->child;
        t = to->child;
        o = out->child;
        while (f != NULL) {
            _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
                model, f, t, o, blend);
            f = f->next;
            t = t->next;
            o = o->next;
        }
    }
}

void fn_800EB904(GSmodel* model, ModelIntpJObj* from, ModelIntpJObj* to,
                 ModelIntpJObj* out, f32 blend)
{
    s32 i;
    ModelPartAnimMix* mix;
    f32 inverse;
    GSvec vec;
    GSvec fromVec;
    GSvec toVec;
    ModelQuat quat;
    ModelQuat fromQuat;
    ModelQuat toQuat;

    inverse = lbl_8047CC50 - blend;

    HSD_JObjGetTranslation(from, &fromVec);
    HSD_JObjGetTranslation(to, &toVec);
    vec.x = fromVec.x * inverse + toVec.x * blend;
    vec.y = fromVec.y * inverse + toVec.y * blend;
    vec.z = fromVec.z * inverse + toVec.z * blend;
    HSD_JObjSetTranslate(out, &vec);

    HSD_JObjGetScale(from, &fromVec);
    HSD_JObjGetScale(to, &toVec);
    vec.x = fromVec.x * inverse + toVec.x * blend;
    vec.y = fromVec.y * inverse + toVec.y * blend;
    vec.z = fromVec.z * inverse + toVec.z * blend;
    HSD_JObjSetScale(out, &vec);

    HSD_JObjGetRotation(from, &fromQuat);
    HSD_JObjGetRotation(to, &toQuat);
    fromVec.x = fromQuat.x;
    fromVec.y = fromQuat.y;
    fromVec.z = fromQuat.z;
    toVec.x = toQuat.x;
    toVec.y = toQuat.y;
    toVec.z = toQuat.z;

    mix = (ModelPartAnimMix*)((u8*)model + 0xE4);
    for (i = 4; i-- != 0; mix++) {
        if (mix->type != 0 && mix->index == lbl_8047ABA0) {
            switch (mix->type) {
            case 1:
                fromVec.x = mix->value->x;
                fromVec.y = mix->value->y;
                fromVec.z = mix->value->z;
                toVec.x = mix->value->x;
                toVec.y = mix->value->y;
                toVec.z = mix->value->z;
                break;
            case 2:
                fromVec.x += mix->value->x;
                fromVec.y += mix->value->y;
                fromVec.z += mix->value->z;
                toVec.x += mix->value->x;
                toVec.y += mix->value->y;
                toVec.z += mix->value->z;
                break;
            case 3:
                fromVec.x += mix->value->x;
                fromVec.y = mix->value->y;
                fromVec.z += mix->value->z;
                toVec.x += mix->value->x;
                toVec.y = mix->value->y;
                toVec.z += mix->value->z;
                break;
            }
            i = 0;
        }
    }

    EulerToQuat(&fromVec, &fromQuat);
    EulerToQuat(&toVec, &toQuat);
    if ((fromQuat.x - toQuat.x) * (fromQuat.x - toQuat.x) +
            (fromQuat.y - toQuat.y) * (fromQuat.y - toQuat.y) +
            (fromQuat.z - toQuat.z) * (fromQuat.z - toQuat.z) +
            (fromQuat.w - toQuat.w) * (fromQuat.w - toQuat.w) >
        (fromQuat.x + toQuat.x) * (fromQuat.x + toQuat.x) +
            (fromQuat.y + toQuat.y) * (fromQuat.y + toQuat.y) +
            (fromQuat.z + toQuat.z) * (fromQuat.z + toQuat.z) +
            (fromQuat.w + toQuat.w) * (fromQuat.w + toQuat.w))
    {
        toQuat.x = -toQuat.x;
        toQuat.y = -toQuat.y;
        toQuat.z = -toQuat.z;
        toQuat.w = -toQuat.w;
    }
    fn_801AD7CC(&fromQuat, &toQuat, &quat, blend);
    HSD_JObjSetRotation(out, &quat);
    lbl_8047ABA0++;
}

#endif /* GS_MODEL_BOUND_EMIT_INTP */

#if defined(GS_MODEL_BOUND_EMIT_BLEND_MODEL)
void modelCalculateBlendModel__FP8_GSmodelf(GSmodel* model, f32 unused)
{
    GSmtx translation;
    GSmtx rotate_x;
    GSmtx rotate_y;
    GSmtx rotate_z;
    GSmtx scale;
    GSmtx* matrix;

    (void)unused;
    lbl_8047ABA0 = 0;
    _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
        model, (struct ModelIntpJObj*)model->blendJObjA,
        (struct ModelIntpJObj*)model->blendJObjB,
        (struct ModelIntpJObj*)model->blendJObj, model->blendFactor);
    matrix = HSD_JObjGetMtxPtr((HSDJObj*)model->blendJObj);
    fn_800E0560(translation, &model->position);
    GSmtxMakeXRotation(rotate_x, model->rotation.x);
    GSmtxMakeYRotation(rotate_y, model->rotation.y);
    GSmtxMakeZRotation(rotate_z, model->rotation.z);
    fn_800E042C(scale, &model->scale);
    GSmtxConcat(*matrix, *matrix, scale);
    GSmtxConcat(*matrix, *matrix, translation);
    GSmtxConcat(*matrix, *matrix, rotate_x);
    GSmtxConcat(*matrix, *matrix, rotate_y);
    GSmtxConcat(*matrix, *matrix, rotate_z);
}
#endif /* GS_MODEL_BOUND_EMIT_BLEND_MODEL */
