#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_SUFFIX
#include "src/game/gs_model_shadow.c"

extern void* fn_800E5188(GSmodel*);
extern void fn_800E9358(void);
extern void fn_801A3918(void*, void (*)(void), u32);
extern void fn_801B0880(void*, u32);

void modelShadowPrepare__FP8_GSmodelb(GSmodel* model, BOOL enable)
{
    u32 i;
    BOOL found = FALSE;

    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].model == model) {
            found = TRUE;
            break;
        }
    }
    if (!found) {
        return;
    }
    if (enable) {
        fn_801A3918(fn_800E5188(model), fn_800E9358, 1);
    } else {
        fn_801A3918(fn_800E5188(model), fn_800E9358, 0);
    }
    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].flag && lbl_80401490[i].model == model) {
            fn_801B0880(lbl_80401490[i].obj, (u8)enable);
        }
    }
}
