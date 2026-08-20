/** Candidate-only owner for the residual model-bound range at 0x800EB5A0. */
#define GS_MODEL_BOUND_800EB464_SUFFIX_ACTIVE
#include "src/game/gs_model_bound.c"

typedef f32 GSmtx[3][4];

extern u32 lbl_8047ABA0;
extern const char lbl_8047CC40[7];
extern const char lbl_8047CC48[5];
extern void _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
    GSmodel*, HSD_JObj*, HSD_JObj*, HSD_JObj*, f32);
extern void fn_800E0560(GSmtx, const GSvec*);
extern void GSmtxMakeXRotation(GSmtx, f32);
extern void GSmtxMakeYRotation(GSmtx, f32);
extern void GSmtxMakeZRotation(GSmtx, f32);
extern void fn_800E042C(GSmtx, const GSvec*);
extern void fn_800E0290(GSmtx, GSmtx, GSmtx);

typedef struct ModelIntpJObj {
    u8 pad_00[8];
    struct ModelIntpJObj* next;
    struct ModelIntpJObj* parent;
    struct ModelIntpJObj* child;
    u32 flags;
    f32 rotation[4];
    GSvec scale;
    GSvec translation;
} ModelIntpJObj;

void _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
    GSmodel* model, HSD_JObj* out, HSD_JObj* from, HSD_JObj* to, f32 blend)
{
    extern void fn_800EB904(GSmodel*, HSD_JObj*, HSD_JObj*, HSD_JObj*, f32);
    ModelIntpJObj *out1, *from1, *to1;
    ModelIntpJObj *out2, *from2, *to2;
    ModelIntpJObj *out3, *from3, *to3;
    ModelIntpJObj *out4, *from4, *to4;
    ModelIntpJObj *out5, *from5, *to5;

    if (out == NULL || from == NULL || to == NULL) {
        return;
    }

    fn_800EB904(model, from, to, out, blend);
    if (((ModelIntpJObj*)out)->flags & 0x1000) return;
    out1 = ((ModelIntpJObj*)out)->child;
    from1 = ((ModelIntpJObj*)from)->child;
    to1 = ((ModelIntpJObj*)to)->child;
    while (out1 != NULL) {
        if (from1 != NULL && to1 != NULL) {
            fn_800EB904(model, (HSD_JObj*)from1, (HSD_JObj*)to1, (HSD_JObj*)out1, blend);
            if (!(out1->flags & 0x1000)) {
                out2 = out1->child; from2 = from1->child; to2 = to1->child;
                while (out2 != NULL) {
                    if (from2 != NULL && to2 != NULL) {
                        fn_800EB904(model, (HSD_JObj*)from2, (HSD_JObj*)to2, (HSD_JObj*)out2, blend);
                        if (!(out2->flags & 0x1000)) {
                            out3 = out2->child; from3 = from2->child; to3 = to2->child;
                            while (out3 != NULL) {
                                if (from3 != NULL && to3 != NULL) {
                                    fn_800EB904(model, (HSD_JObj*)from3, (HSD_JObj*)to3, (HSD_JObj*)out3, blend);
                                    if (!(out3->flags & 0x1000)) {
                                        out4 = out3->child; from4 = from3->child; to4 = to3->child;
                                        while (out4 != NULL) {
                                            if (from4 != NULL && to4 != NULL) {
                                                fn_800EB904(model, (HSD_JObj*)from4, (HSD_JObj*)to4, (HSD_JObj*)out4, blend);
                                                if (!(out4->flags & 0x1000)) {
                                                    out5 = out4->child; from5 = from4->child; to5 = to4->child;
                                                    while (out5 != NULL) {
                                                        _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(model, (HSD_JObj*)out5, (HSD_JObj*)from5, (HSD_JObj*)to5, blend);
                                                        out5 = out5->next; from5 = from5->next; to5 = to5->next;
                                                    }
                                                }
                                            }
                                            out4 = out4->next; from4 = from4->next; to4 = to4->next;
                                        }
                                    }
                                }
                                out3 = out3->next; from3 = from3->next; to3 = to3->next;
                            }
                        }
                    }
                    out2 = out2->next; from2 = from2->next; to2 = to2->next;
                }
            }
        }
        out1 = out1->next; from1 = from1->next; to1 = to1->next;
    }
}

typedef struct ModelBlendRule {
    s32 type;
    u32 index;
    GSvec* vector;
} ModelBlendRule;

static BOOL modelIntpJObjNeedsUpdate(ModelIntpJObj* jobj)
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

void fn_800EB904(GSmodel* model, HSD_JObj* from_arg, HSD_JObj* to_arg,
                 HSD_JObj* out_arg, f32 blend)
{
    extern void fn_8019D620(HSD_JObj*);
    extern void fn_801AD7CC(f32* from, f32* to, f32* out, f32 blend);
    extern void fn_801ADAAC(void* src, void* dst);
    extern f32 lbl_8047CC50;
    ModelIntpJObj* from = (ModelIntpJObj*)from_arg;
    ModelIntpJObj* to = (ModelIntpJObj*)to_arg;
    ModelIntpJObj* out = (ModelIntpJObj*)out_arg;
    ModelBlendRule* rules;
    f32 inverse;
    GSvec fromTranslation;
    GSvec toTranslation;
    GSvec blendedTranslation;
    GSvec fromScale;
    GSvec toScale;
    GSvec blendedScale;
    GSvec fromEuler;
    GSvec toEuler;
    f32 fromQuat[4];
    f32 toQuat[4];
    f32 outQuat[4];
    s32 i;

    inverse = lbl_8047CC50 - blend;

    if (from == NULL) {
        __assert(lbl_8047CC40, 0x3E4, lbl_8047CC48);
    }
    fromTranslation = from->translation;
    if (to == NULL) {
        __assert(lbl_8047CC40, 0x3E4, lbl_8047CC48);
    }
    blendedTranslation.x = fromTranslation.x * inverse + to->translation.x * blend;
    blendedTranslation.y = fromTranslation.y * inverse + to->translation.y * blend;
    blendedTranslation.z = fromTranslation.z * inverse + to->translation.z * blend;
    if (out == NULL) {
        __assert(lbl_8047CC40, 0x3A9, lbl_8047CC48);
    }
    out->translation = blendedTranslation;
    if (!(out->flags & 0x02000000) && !modelIntpJObjNeedsUpdate(out)) {
        fn_8019D620(out_arg);
    }

    if (from == NULL) {
        __assert(lbl_8047CC40, 0x351, lbl_8047CC48);
    }
    fromScale = from->scale;
    if (to == NULL) {
        __assert(lbl_8047CC40, 0x351, lbl_8047CC48);
    }
    blendedScale.x = fromScale.x * inverse + to->scale.x * blend;
    blendedScale.y = fromScale.y * inverse + to->scale.y * blend;
    blendedScale.z = fromScale.z * inverse + to->scale.z * blend;
    if (out == NULL) {
        __assert(lbl_8047CC40, 0x316, lbl_8047CC48);
    }
    out->scale = blendedScale;
    if (!(out->flags & 0x02000000) && !modelIntpJObjNeedsUpdate(out)) {
        fn_8019D620(out_arg);
    }

    if (from == NULL) {
        __assert(lbl_8047CC40, 0x2DD, lbl_8047CC48);
    }
    fromQuat[0] = from->rotation[0];
    fromQuat[1] = from->rotation[1];
    fromQuat[2] = from->rotation[2];
    fromQuat[3] = from->rotation[3];
    if (to == NULL) {
        __assert(lbl_8047CC40, 0x2DD, lbl_8047CC48);
    }
    toQuat[0] = to->rotation[0];
    toQuat[1] = to->rotation[1];
    toQuat[2] = to->rotation[2];
    toQuat[3] = to->rotation[3];

    fromEuler.x = from->rotation[0];
    fromEuler.y = from->rotation[1];
    fromEuler.z = from->rotation[2];
    toEuler.x = to->rotation[0];
    toEuler.y = to->rotation[1];
    toEuler.z = to->rotation[2];

    rules = (ModelBlendRule*) ((u8*) model + 0xE4);
    for (i = 0; i < 4; i++) {
        if (rules[i].type == 0 || rules[i].index != lbl_8047ABA0) {
            continue;
        }

        switch (rules[i].type) {
        case 1:
            fromEuler = *rules[i].vector;
            toEuler = *rules[i].vector;
            break;
        case 2:
            fromEuler.x += rules[i].vector->x;
            fromEuler.y += rules[i].vector->y;
            fromEuler.z += rules[i].vector->z;
            toEuler.x += rules[i].vector->x;
            toEuler.y += rules[i].vector->y;
            toEuler.z += rules[i].vector->z;
            break;
        case 3:
            fromEuler.x += rules[i].vector->x;
            fromEuler.y = rules[i].vector->y;
            fromEuler.z += rules[i].vector->z;
            toEuler.x += rules[i].vector->x;
            toEuler.y = rules[i].vector->y;
            toEuler.z += rules[i].vector->z;
            break;
        default:
            break;
        }
        break;
    }

    fn_801ADAAC(&fromEuler, fromQuat);
    fn_801ADAAC(&toEuler, toQuat);
    if (((fromQuat[0] - toQuat[0]) * (fromQuat[0] - toQuat[0]) +
         (fromQuat[1] - toQuat[1]) * (fromQuat[1] - toQuat[1]) +
         (fromQuat[2] - toQuat[2]) * (fromQuat[2] - toQuat[2]) +
         (fromQuat[3] - toQuat[3]) * (fromQuat[3] - toQuat[3])) >
        ((fromQuat[0] + toQuat[0]) * (fromQuat[0] + toQuat[0]) +
         (fromQuat[1] + toQuat[1]) * (fromQuat[1] + toQuat[1]) +
         (fromQuat[2] + toQuat[2]) * (fromQuat[2] + toQuat[2]) +
         (fromQuat[3] + toQuat[3]) * (fromQuat[3] + toQuat[3])))
    {
        toQuat[0] = -toQuat[0];
        toQuat[1] = -toQuat[1];
        toQuat[2] = -toQuat[2];
        toQuat[3] = -toQuat[3];
    }

    fn_801AD7CC(fromQuat, toQuat, outQuat, blend);
    if (out == NULL) {
        __assert(lbl_8047CC40, 0x290, lbl_8047CC48);
    }
    out->rotation[0] = outQuat[0];
    out->rotation[1] = outQuat[1];
    out->rotation[2] = outQuat[2];
    out->rotation[3] = outQuat[3];
    if (!(out->flags & 0x02000000) && !modelIntpJObjNeedsUpdate(out)) {
        fn_8019D620(out_arg);
    }
    lbl_8047ABA0++;
}

void modelCalculateBlendModel__FP8_GSmodelf(GSmodel* model, f32 unused)
{
    HSDJObj* jobj;
    GSmtx translation;
    GSmtx rotate_x;
    GSmtx rotate_y;
    GSmtx rotate_z;
    GSmtx scale;
    GSmtx* matrix;

    (void)unused;
    lbl_8047ABA0 = 0;
    _modelIntpJObjAll__FP8_GSmodelP9_HSD_JObjP9_HSD_JObjP9_HSD_JObjff(
        model, model->blendJObjA, model->blendJObjB, model->blendJObj,
        model->blendFactor);
    jobj = (HSDJObj*)model->blendJObj;
    if (jobj == NULL) {
        __assert(lbl_8047CC40, 0x47C, lbl_8047CC48);
    }
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D9DC(jobj);
    }
    matrix = (GSmtx*)jobj->matrix;
    fn_800E0560(translation, &model->position);
    GSmtxMakeXRotation(rotate_x, model->rotation.x);
    GSmtxMakeYRotation(rotate_y, model->rotation.y);
    GSmtxMakeZRotation(rotate_z, model->rotation.z);
    fn_800E042C(scale, &model->scale);
    fn_800E0290(*matrix, *matrix, scale);
    fn_800E0290(*matrix, *matrix, translation);
    fn_800E0290(*matrix, *matrix, rotate_x);
    fn_800E0290(*matrix, *matrix, rotate_y);
    fn_800E0290(*matrix, *matrix, rotate_z);
}
