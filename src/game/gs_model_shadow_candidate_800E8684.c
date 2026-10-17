/**
 * modelShadowRender__FP10GSgfxLayer (0x800E8684 - 0x800E8EFC), XD shadow.o.
 *
 * Exact and linked (lane D7, 2026-09-29, from lane D4's 99.79% candidate).
 * The unit also owns the function's .sdata2 literal pool, 0x8047CBC0-0x8047CBE8
 * (3.0f, 0.01f, 0.0f, 30.0f, 0.1f, 3000.0f, "shadow", the u32 bias). It is
 * built with -str reuse,readonly so "shadow" lands in that pool. See
 * docs/recon/gs_model_shadow_render_and_etctool_walls.md.
 *
 * Shaping (every item is a RULE-EXCEPTION(title-path), docs/RULE_EXCEPTIONS.md):
 * - `#pragma opt_loop_invariants off`: without it the frontend hoists
 *   &model->bound out of the cast-model loop into r22. The bound argument is
 *   spelt three ways so CSE does not merge the three call sites.
 * - modelShadowSetReceiver nests BoundToSize one level deeper for retail's
 *   stack slots.
 * - Object-list loop: retail colours the searched model as the same web as
 *   the cast-model counter `j` (r27). With `opt_lifetimes off` the model is
 *   stored in `j`, and the slot loop gets its own counter so `i` keeps
 *   retail's split. Backend copy propagation would still fold `j` into the
 *   call-result temporary. It skips a copy whose destination is used by
 *   another move, and each of its four passes strips only the last move of
 *   a chain, so a chain of seven type-changing copies (hop0..listModel)
 *   keeps `j` until the last pass. `opt_dead_assignments off` keeps the chain
 *   intact through the frontend, and all of its moves are gone before
 *   register allocation.
 * - 3.0f, 0.01f and 0.0f are named sdata2 globals, because the linked
 *   800E9358/800E92D8 carves read lbl_8047CBC0/lbl_8047CBC8 by name. The rest
 *   are literals: as named loads, 0.1f changes the float-register order.
 *   Named data comes before the literal pool, so the pool order is retail's.
 */
#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_PREFIX
#include "src/game/gs_model_shadow.c"

typedef struct GSshadowRect {
    GSshadowVec origin;
    GSshadowVec up_v;
    GSshadowVec right_v;
    GSshadowVec eye_v;
    GSshadowVec eye_vn;
    f32 distance;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    int perspective;
} GSshadowRect;

typedef struct GSshadowObj {
    void* objects; /* HSD_SList* */
    void* camera;  /* HSD_CObj* */
    u8 pad_08[0x18];
    u8 intensity; /* 0x20 */
} GSshadowObj;

typedef struct GSshadowList {
    struct GSshadowList* next;
    void* data;
} GSshadowList;

typedef struct GSshadowModel {
    u32 flags;
    u8 pad_04[0x2C];
    GSshadowVec scale; /* 0x30 */
    u8 pad_3C[0x10];
    GSshadowBound bound; /* 0x4C */
} GSshadowModel;

extern u8* lbl_8047AB74;
extern u32 lbl_8047AB78;
extern u32 lbl_8047AB80;
extern u32 lbl_8047AB84;
extern u8 lbl_8047AB94;
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
/* RULE-EXCEPTION(title-path): named pool entries shared with linked carves, zero kept in .sdata2 by pragma - see docs/RULE_EXCEPTIONS.md */
#pragma section ".sdata2"
#pragma explicit_zero_data on
__declspec(section ".sdata2") f32 lbl_8047CBC0 = 3.0f;
__declspec(section ".sdata2") f32 lbl_8047CBC4 = 0.01f;
__declspec(section ".sdata2") f32 lbl_8047CBC8 = 0.0f;
#pragma explicit_zero_data reset
extern const char lbl_80270E98[]; /* "shadow.h" */
extern const f32 lbl_80478AC0[]; /* NaN */
extern void* modelGetRenderJObj(GSmodel* model);
extern GSmodel* GSmodelSearchModelList(void* jobj);
extern void fn_800E3D14(GSmodel* model, GSshadowVec* out);
extern void GSvecAdd(GSshadowVec* dst, GSshadowVec* a, GSshadowVec* b);
extern void set__5GSvecFfff(GSshadowVec* dst, f32 x, f32 y, f32 z);
extern void vecNormalize(GSshadowVec* dst, GSshadowVec* src, f32 scalar);
extern int HSD_ViewingRectCheck(GSshadowRect* rect);
extern void HSD_ViewingRectAddRect(GSshadowRect* rect, GSshadowVec* position, f32 top,
                        f32 bottom, f32 left, f32 right);
extern void HSD_ViewingRectInit(GSshadowRect* rect, GSshadowVec* eye,
                        GSshadowVec* interest, GSshadowVec* up, int perspective);
extern void HSD_ShadowSetViewingRect(GSshadowObj* shadow, f32 top, f32 bottom, f32 left,
                        f32 right);
extern void fn_801B06DC(GSshadowObj* shadow);
extern void HSD_ShadowDeleteObject(GSshadowObj* shadow, void* object);
extern void HSD_ShadowAddObject(GSshadowObj* shadow, void* object);
extern void HSD_ShadowSetLight(GSshadowObj* shadow, void* lightData, f32 value);
extern void HSD_ShadowEndRender(GSshadowObj* shadow);
extern void HSD_ShadowStartRender(GSshadowObj* shadow);
extern void HSD_ShadowSetSize(GSshadowObj* shadow, u16 width, u16 height);
extern void HSD_ShadowInit(GSshadowObj* shadow);
extern void HSD_CObjSetProjectionType(void* cobj, s32 type);
extern void HSD_CObjSetInterest(void* cobj, GSshadowVec* interest);
extern void HSD_CObjSetNear(void* cobj, f32 value);
extern void HSD_CObjSetFar(void* cobj, f32 value);
extern f32 HSD_CObjGetEyeDistance(void* cobj);
extern void HSD_CObjGetEyePosition(void* cobj, GSshadowVec* out);
extern void HSD_CObjGetUpVector(void* cobj, GSshadowVec* out);
extern void fn_8019C6FC(void);
extern void HSD_StartRender(u32 arg);
extern void __assert(const char*, s32, const char*);
extern GSshadowSlot*
_modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);
extern void
_modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);

/* shadow.h (assert line 107). */
static inline void HSD_ShadowSetIntensity(GSshadowObj* shadow, u8 intensity)
{
    if (shadow == NULL) {
        __assert(lbl_80270E98, 107, "shadow");
    }
    shadow->intensity = intensity;
}

/* XD: _modelShadowInitReceiveList__Fv (UNUSED, 0x70). Expanding it inline
 * gives retail's separate zero and counter registers for the first loop. */
static inline void modelShadowInitReceiveList(void)
{
    u32 i;
    u32 j;

    for (i = 0; i < 6; i++) {
        lbl_80401490[i].flag = 0;
        lbl_80401490[i].model = NULL;
        for (j = 0; j < 16; j++) {
            lbl_80401490[i].receivers[j] = NULL;
        }
    }
}

/* Single-use helper: nesting BoundToSize one inline level deeper gives
 * retail's stack order for its three GSvec temporaries. */
/* RULE-EXCEPTION(title-path): single-use inline helper justified by stack order - see docs/RULE_EXCEPTIONS.md */
static inline void modelShadowSetReceiver(GSshadowSlot* slot, s32 index,
                                          GSmodel* receiveModel,
                                          GSshadowBound* bound)
{
    u32 size;

    slot->receivers[index] = receiveModel;
    size = modelShadowBoundToSize(bound);
    if (size < slot->minSize) {
        slot->minSize = size;
    }
    if (size > slot->maxSize) {
        slot->maxSize = size;
    }
}

/* RULE-EXCEPTION(title-path): local compiler-control pragmas (opt_loop_invariants, opt_lifetimes, opt_dead_assignments) - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_loop_invariants off
#pragma opt_lifetimes off
#pragma opt_dead_assignments off
void modelShadowRender__FP10GSgfxLayer(void* layer)
{
    u32 slotIndex; /* RULE-EXCEPTION(title-path): slot-loop counter split from i under opt_lifetimes off - see docs/RULE_EXCEPTIONS.md */
    GSmodel* receiver;
    /* RULE-EXCEPTION(title-path): copy chain that delays copy propagation - see docs/RULE_EXCEPTIONS.md */
    GSmodel* hop0;
    u32 hop1;
    GSmodel* hop2;
    u32 hop3;
    GSmodel* hop4;
    u32 hop5;
    GSmodel* listModel;
    u32 i;
    u32 j;
    GSmodel* model;
    u32 count;
    GSshadowSlot* slot;
    u32 k;
    GSmodel* castModel;
    GSlight* light;
    GSshadowList* list;
    s32 index;
    u8 valid;
    GSshadowRect rect;
    GSshadowVec avg;
    GSshadowVec pos;
    GSshadowVec up;
    f32 size;

    (void)layer;

    modelShadowInitReceiveList();

    for (i = 0; i < lbl_8047AB78; i++) {
        model = (GSmodel*)(lbl_8047AB74 + i * 0x170);
        valid = FALSE;
        if (!(*(u32*)model & 1)) {
            continue;
        }
        if ((*(u32*)model & 2) && (*(u32*)model & 0x00400000) &&
            (*(u32*)model & 0x10000000))
        {
            valid = TRUE;
        }
        if (*(u32*)model & 0x20000000) {
            valid = TRUE;
        }
        if ((*(u32*)model & 0x400) && (*(u32*)model & 2)) {
            valid = TRUE;
        }
        if (!valid) {
            continue;
        }
        for (j = 0; j < model->shadowVtxCount; j++) {
            light = model->shadowLight;
            castModel = ((GSmodel**)model->shadowVtxBuffer)[j];
            if (light == NULL) {
                continue;
            }
            if (modelShadowGetAvgScl(&((GSshadowModel*)model)->scale) <
                lbl_8047CBC4)
            {
                continue;
            }
            /* RULE-EXCEPTION(title-path): bound spelt three ways to defeat CSE - see docs/RULE_EXCEPTIONS.md */
            slot = _modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                castModel, model, light, (GSshadowBound*)((u8*)model + 0x4cU));
            if (slot != NULL) {
                if (modelShadowFindCastModel(slot, model) == -1) {
                    index = modelShadowFindCastModel(slot, NULL);
                    if (index == -1) {
                        _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                            castModel, model, light,
                            (GSshadowBound*)((s32)model + 0x4c));
                    } else {
                        modelShadowSetReceiver(slot, index, model,
                                               &((GSshadowModel*)model)->bound);
                    }
                }
            } else {
                _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                    castModel, model, light, (GSshadowBound*)((u32)model + 0x4c));
            }
        }
    }

    for (slotIndex = 0; slotIndex < 6; slotIndex++) {
        slot = &lbl_80401490[slotIndex];
        if (slot->model == NULL) {
            fn_801B06DC(slot->obj);
            slot->flag = 0;
            continue;
        }
        if (!(*(u32*)slot->model & 1)) {
            continue;
        }
        count = 0;
        set__5GSvecFfff(&avg, lbl_8047CBC8, lbl_8047CBC8, lbl_8047CBC8);
        for (k = 0; k < 16; k++) {
            receiver = slot->receivers[k];
            if (receiver != NULL && (*(u32*)receiver & 1)) {
                HSD_ShadowAddObject(slot->obj, modelGetRenderJObj(receiver));
                fn_800E3D14(receiver, &pos);
                GSvecAdd(&avg, &avg, &pos);
                count++;
            }
        }
        if (count == 0) {
            slot->flag = 0;
            continue;
        }
        switch (slot->light->classObj->flags & 3) {
        case 1:
            HSD_CObjSetProjectionType(((GSshadowObj*)slot->obj)->camera, 3);
            break;
        case 2:
        case 3:
            HSD_CObjSetProjectionType(((GSshadowObj*)slot->obj)->camera, 2);
            break;
        }
        vecNormalize(&avg, &avg, count);
        HSD_CObjSetInterest(((GSshadowObj*)slot->obj)->camera, &avg);
        HSD_ShadowSetLight(slot->obj, slot->light->classObj, 30.0f);
        HSD_CObjSetNear(((GSshadowObj*)slot->obj)->camera, 0.1f);
        HSD_CObjSetFar(((GSshadowObj*)slot->obj)->camera, 3000.0f);
        size = HSD_CObjGetEyeDistance(((GSshadowObj*)slot->obj)->camera);
        if (size == lbl_80478AC0[0] || size < 0.1f) {
            HSD_ShadowDeleteObject(slot->obj, NULL);
            return;
        }
        HSD_CObjGetEyePosition(((GSshadowObj*)slot->obj)->camera, &pos);
        HSD_CObjGetUpVector(((GSshadowObj*)slot->obj)->camera, &up);
        HSD_ViewingRectInit(&rect, &pos, &avg, &up,
                    (slot->light->classObj->flags & 3) != 1);
        for (list = ((GSshadowObj*)slot->obj)->objects; list != NULL;
             list = list->next)
        {
            /* RULE-EXCEPTION(title-path): model held in j, then a chain of
             * seven copies, so j keeps retail's r27 - see docs/RULE_EXCEPTIONS.md */
            j = (u32)GSmodelSearchModelList(list->data);
            if ((GSmodel*)j == NULL) {
                continue;
            }
            hop0 = (GSmodel*)j;
            hop1 = (u32)hop0;
            hop2 = (GSmodel*)hop1;
            hop3 = (u32)hop2;
            hop4 = (GSmodel*)hop3;
            hop5 = (u32)hop4;
            listModel = (GSmodel*)hop5;
            fn_800E3D14(listModel, &pos);
            if (count == 1) {
                size = modelShadowBoundToSize(
                           &((GSshadowModel*)listModel)->bound) +
                       lbl_8047AB80;
            } else {
                size = modelShadowBoundToSize(
                           &((GSshadowModel*)listModel)->bound) +
                       lbl_8047AB84;
            }
            if (size < 0.1f) {
                size = 0.1f;
            }
            HSD_ViewingRectAddRect(&rect, &pos, size, -size, -size, size);
        }
        if (HSD_ViewingRectCheck(&rect) != 0) {
            HSD_ShadowSetViewingRect(slot->obj, rect.top, rect.bottom, rect.left,
                        rect.right);
        } else {
            size = slot->maxSize + lbl_8047AB84;
            if (size < 0.1f) {
                size = 0.1f;
            }
            HSD_ShadowSetViewingRect(slot->obj, size, -size, -size, size);
        }
        HSD_ShadowSetIntensity(slot->obj, 0xFF - lbl_8047AB94);
        HSD_ShadowSetSize(slot->obj, lbl_8047AB90, lbl_8047AB8C);
        slot->flag = 1;
        HSD_StartRender(3);
        HSD_ShadowInit(slot->obj);
        HSD_ShadowStartRender(slot->obj);
        HSD_ShadowEndRender(slot->obj);
        fn_8019C6FC();
        HSD_ShadowDeleteObject(slot->obj, NULL);
    }
}
#pragma pop
