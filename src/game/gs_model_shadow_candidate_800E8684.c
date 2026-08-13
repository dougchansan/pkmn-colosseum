#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_PREFIX
#include "src/game/gs_model_shadow.c"

typedef struct GSshadowVec {
    f32 x;
    f32 y;
    f32 z;
} GSshadowVec;

typedef struct GSshadowBound {
    u8 pad_00[0xC];
    GSshadowVec* scale;
} GSshadowBound;

extern u8* lbl_8047AB74;
extern u32 lbl_8047AB78;
extern u32 lbl_8047AB80;
extern u32 lbl_8047AB84;
extern u8 lbl_8047AB94;
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
extern f32 lbl_8047CBCC;
extern f32 lbl_8047CBC0;
extern f32 lbl_8047CBC4;
extern f32 lbl_8047CBC8;
extern f64 lbl_8047CBE0;
extern f32 lbl_8047CBD0;
extern f32 lbl_8047CBD4;
extern f32 lbl_8047CBD8;
extern void* modelGetRenderJObj(GSmodel* model);
extern void fn_800E3D14(GSmodel* model, GSshadowVec* out);
extern void GSvecAdd(void* dst, void* a, void* b);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E00AC(void* dst, void* src, f32 scalar);
extern void* ObjInfoInit(void* bound, GSshadowVec* out);
extern void* HSD_ViewingRectCheck(void* rect);
extern void fn_801B0408(void* rect, void* eyePos, void* interest, void* up, u32 persp);
extern void fn_801B04E0(void* shadow, f32 top, f32 bottom, f32 left, f32 right);
extern void fn_801B06DC(void* obj);
extern void fn_801B073C(void* list, void* object);
extern void fn_801B07D4(void* list, void* object);
extern void fn_801B0A98(void* shadow, void* lightData, f32 value);
extern void fn_801B0BD8(void* shadow);
extern void fn_801B0EB8(void* shadow);
extern void fn_801B1524(void* shadow, u16 width, u16 height);
extern void HSD_ShadowInit(void* shadow);
extern void HSD_CObjSetProjectionType(void* cobj, s32 type);
extern void HSD_CObjSetInterest(void* cobj, GSshadowVec* interest);
extern void HSD_CObjSetNear(void* cobj, f32 value);
extern void HSD_CObjSetFar(void* cobj, f32 value);
extern f32 HSD_CObjGetEyeDistance(void* cobj);
extern void HSD_CObjGetEyePosition(void* cobj, GSshadowVec* out);
extern void HSD_CObjGetUpVector(void* cobj, GSshadowVec* out);
extern void fn_8019C6FC(void);
extern void fn_8019C708(u32 arg);
extern u32 __cvt_fp2unsigned(f64 value);
extern f64 ceil(f64);
extern void __assert(const char*, s32, const char*);
extern const char lbl_80270E98[];
extern GSshadowSlot*
_modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);
extern void
_modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);

void modelShadowRender__FP10GSgfxLayer(void* layer)
{
    GSshadowSlot* slot;
    GSmodel* receiveModel;
    GSmodel* castModel;
    GSlight* light;
    GSshadowVec pos;
    GSshadowVec avgPos;
    GSshadowVec dims;
    u32 i;
    u32 j;
    u32 activeCount;
    u32 valid;
    f32 largest;
    f32 size;

    (void)layer;

    for (i = 0; i < 6; i++) {
        slot = &lbl_80401490[i];
        slot->model = NULL;
        slot->light = NULL;
        slot->receivers[0] = NULL;
        slot->receivers[1] = NULL;
        slot->receivers[2] = NULL;
        slot->receivers[3] = NULL;
        slot->receivers[4] = NULL;
        slot->receivers[5] = NULL;
        slot->receivers[6] = NULL;
        slot->receivers[7] = NULL;
        slot->receivers[8] = NULL;
        slot->receivers[9] = NULL;
        slot->receivers[10] = NULL;
        slot->receivers[11] = NULL;
        slot->receivers[12] = NULL;
        slot->receivers[13] = NULL;
        slot->receivers[14] = NULL;
        slot->receivers[15] = NULL;
        slot->minSize = 0;
        slot->maxSize = 0;
        slot->flag = 0;
    }

    for (i = 0; i < lbl_8047AB78; i++) {
        receiveModel = (GSmodel*)(lbl_8047AB74 + i * 0x170);
        valid = 0;
        if ((*(u32*)receiveModel & 1) == 0) {
            continue;
        }
        if ((*(u32*)receiveModel & 0x2) != 0 &&
            (*(u32*)receiveModel & 0x00400000) != 0 &&
            (*(u32*)receiveModel & 0x10000000) != 0) {
            valid = 1;
        }
        if ((*(u32*)receiveModel & 0x20000000) != 0) {
            valid = 1;
        }
        if ((*(u32*)receiveModel & 0x400) != 0 &&
            (*(u32*)receiveModel & 0x2) != 0) {
            valid = 1;
        }
        if (!valid) {
            continue;
        }

        for (j = 0; j < receiveModel->shadowVtxCount; j++) {
            light = receiveModel->shadowLight;
            castModel = ((GSmodel**)receiveModel->shadowVtxBuffer)[j];
            if (light == NULL || castModel == NULL) {
                continue;
            }

            if (((*(f32*)((u8*)receiveModel + 0x30) +
                  *(f32*)((u8*)receiveModel + 0x34) +
                  *(f32*)((u8*)receiveModel + 0x38)) / lbl_8047CBC0) <
                lbl_8047CBC4) {
                continue;
            }

            slot = _modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                castModel, receiveModel, light, (GSshadowBound*)((u8*)receiveModel + 0x4C));
            if (slot == NULL) {
                _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                    castModel, receiveModel, light, (GSshadowBound*)((u8*)receiveModel + 0x4C));
                continue;
            }

            for (activeCount = 0; activeCount < 16; activeCount++) {
                if (slot->receivers[activeCount] == receiveModel) {
                    break;
                }
            }
            if (activeCount == 16) {
                _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                    castModel, receiveModel, light, (GSshadowBound*)((u8*)receiveModel + 0x4C));
                continue;
            }

            ObjInfoInit((u8*)receiveModel + 0x4C, &dims);
            largest = dims.x;
            if (dims.y > largest) {
                largest = dims.y;
            }
            if (dims.z > largest) {
                largest = dims.z;
            }
            size = ceil((largest *
                        ((((GSshadowBound*)((u8*)receiveModel + 0x4C))->scale->x +
                          ((GSshadowBound*)((u8*)receiveModel + 0x4C))->scale->y +
                          ((GSshadowBound*)((u8*)receiveModel + 0x4C))->scale->z) /
                         lbl_8047CBC0)));
            if ((u32)size < slot->minSize) {
                slot->minSize = (u32)size;
            }
            if ((u32)size > slot->maxSize) {
                slot->maxSize = (u32)size;
            }
        }
    }

    for (i = 0; i < 6; i++) {
        slot = &lbl_80401490[i];
        if (slot->model == NULL) {
            fn_801B06DC(slot->obj);
            slot->flag = 0;
            continue;
        }
        if ((*(u32*)slot->model & 1) == 0) {
            continue;
        }

        set__5GSvecFfff(&avgPos, lbl_8047CBC8, lbl_8047CBC8, lbl_8047CBC8);
        activeCount = 0;
        for (j = 0; j < 16; j++) {
            castModel = slot->receivers[j];
            if (castModel != NULL && ((*(u32*)castModel & 1) != 0)) {
                fn_801B07D4(slot->obj, modelGetRenderJObj(castModel));
                fn_800E3D14(castModel, &pos);
                GSvecAdd(&avgPos, &avgPos, &pos);
                activeCount++;
            }
        }
        if (activeCount == 0) {
            slot->flag = 0;
            continue;
        }

        if ((*(u16*)(*(u8**)((u8*)slot->light + 0x0C) + 0x08) & 3) == 1) {
            HSD_CObjSetProjectionType(*(u8**)((u8*)slot->obj + 0x04), 3);
        } else if ((*(u16*)(*(u8**)((u8*)slot->light + 0x0C) + 0x08) & 3) < 4) {
            HSD_CObjSetProjectionType(*(u8**)((u8*)slot->obj + 0x04), 2);
        }

        fn_800E00AC(&avgPos, &avgPos, (f32)activeCount);
        HSD_CObjSetInterest(*(u8**)((u8*)slot->obj + 0x04), &avgPos);
        fn_801B0A98(slot->obj, *(u8**)((u8*)slot->light + 0x0C), lbl_8047CBCC);
        HSD_CObjSetNear(*(u8**)((u8*)slot->obj + 0x04), lbl_8047CBD0);
        HSD_CObjSetFar(*(u8**)((u8*)slot->obj + 0x04), lbl_8047CBD4);

        if (HSD_CObjGetEyeDistance(*(u8**)((u8*)slot->obj + 0x04)) == 0.0f ||
            HSD_CObjGetEyeDistance(*(u8**)((u8*)slot->obj + 0x04)) < lbl_8047CBD0) {
            fn_801B073C(slot->obj, NULL);
            continue;
        }

        HSD_CObjGetEyePosition(*(u8**)((u8*)slot->obj + 0x04), &pos);
        HSD_CObjGetUpVector(*(u8**)((u8*)slot->obj + 0x04), &dims);
        fn_801B0408((u8*)(((u8*)slot->obj) + 0x50), &pos, &avgPos, &dims,
                    (((*(u16*)(*(u8**)((u8*)slot->light + 0x0C) + 0x08) & 3) == 1) ? 0 : 1));

        if (!HSD_ViewingRectCheck((u8*)slot->obj + 0x50)) {
            size = slot->maxSize + lbl_8047AB84;
            if (size < lbl_8047CBD0) {
                size = lbl_8047CBD0;
            }
            fn_801B04E0(slot->obj, size, -size, size, -size);
        } else {
            fn_801B04E0(slot->obj,
                        *(f32*)((u8*)slot->obj + 0x90),
                        *(f32*)((u8*)slot->obj + 0x94),
                        *(f32*)((u8*)slot->obj + 0x98),
                        *(f32*)((u8*)slot->obj + 0x9C));
        }

        ((u8*)slot->obj)[0x20] = 0xFF - lbl_8047AB94;
        fn_801B1524(slot->obj, (u16)lbl_8047AB90, (u16)lbl_8047AB8C);
        slot->flag = 1;
        fn_8019C708(3);
        HSD_ShadowInit(slot->obj);
        fn_801B0EB8(slot->obj);
        fn_801B0BD8(slot->obj);
        fn_8019C6FC();
        fn_801B073C(slot->obj, NULL);
    }
}
