#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_SUFFIX
#include "src/game/gs_model_shadow.c"

extern void* modelGetRenderJObj(GSmodel*);
extern void fn_801A3918(void*, void (*)(GSjobjNode*, void*, int), u32);
extern void fn_801B0880(void*, u32);

void modelShadowPrepare__FP8_GSmodelb(GSmodel* model, u8 enable)
{
    void* render;
    u32 i;
    u8 found = FALSE;

    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].model == model) {
            found = TRUE;
            break;
        }
    }
    if (!found) {
        return;
    }
    render = modelGetRenderJObj(model);
    if (!enable) {
        fn_801A3918(render,
                    _modelShadowSetShadowFlag__FP9_HSD_JObjPPvi, 0);
    } else {
        fn_801A3918(render,
                    _modelShadowSetShadowFlag__FP9_HSD_JObjPPvi, 1);
    }
    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].flag && lbl_80401490[i].model == model) {
            fn_801B0880(lbl_80401490[i].obj, (u8)enable);
        }
    }
}
