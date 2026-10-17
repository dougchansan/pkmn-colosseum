#include "dolphin/types.h"

typedef struct GSmodel {
    u8 pad_000[0x158];
    u32 shadowVtxCount;
    void* shadowVtxBuffer;
    void* shadowLight;
    u16 shadowVtxHandle;
} GSmodel;

extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);

void modelShadowFreeModelList__FP8_GSmodel(GSmodel* model)
{
    if (model->shadowVtxHandle != 0) {
        fn_800E24B0(model->shadowVtxHandle);
        GSmemFree(model->shadowVtxHandle);
        model->shadowVtxHandle = 0;
        model->shadowVtxCount = 0;
        model->shadowVtxBuffer = NULL;
    }
}
