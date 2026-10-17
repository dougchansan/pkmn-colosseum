/**
 * @file gs_model_shadow_candidate_800E9148.c
 * @brief modelShadowPrepare__FP8_GSmodelb (0x800E9148 - 0x800E9288).
 *
 * The source is gs_model_shadow.c (XD shadow.o); only its types and
 * declarations are taken from there.
 */
#define PR410_GS_MODEL_SHADOW_SPLIT
#include "src/game/gs_model_shadow.c"

extern void* modelGetRenderJObj(GSmodel*);
extern void HSD_JObjWalkTree(void*, void (*)(GSjobjNode*, void*, int), u32);
extern void HSD_ShadowSetActive(void*, u32);
extern void _modelShadowSetShadowFlag__FP9_HSD_JObjPPvi(GSjobjNode* jobj,
                                                        void* arg, int unused);

/*
 * Is the model the receiver of any shadow slot? XD's shadow.o has this as
 * _modelShadowIsReceiver__FP8_GSmodel: dead-stripped (UNUSED, size 0x70) in
 * the XD demo linker map NXXJ01.map (StarsMmd/Colo-XD-PBR-symbol-maps
 * @ 6b51d3af, line 6245), where the slot table is _modelShadowReceiveList
 * (5 x 0x58). This body over XD's 5 slots compiles out of line to exactly
 * 0x70 bytes. Size alone does not single it out (XD's slot-returning
 * _modelShadowFindReceiveModel is also 0x70), but retail tests the result
 * as a byte (clrlwi.), which is the bool helper. XD's live
 * modelShadowPrepare (0x800FE3BC, trevor403/xd-asm @ b1087f18) has the same
 * inlined expansion as Colosseum, with the result in the return register.
 * RULE-EXCEPTION(title-path): single-use inline helper whose XD counterpart
 * is dead-stripped, so its calls/body cannot be compared directly (name and
 * size only) - see docs/RULE_EXCEPTIONS.md
 */
static inline u8 modelShadowIsReceiver(GSmodel* model)
{
    u32 i;

    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].model == model) {
            return TRUE;
        }
    }
    return FALSE;
}

void modelShadowPrepare__FP8_GSmodelb(GSmodel* model, u8 enable)
{
    void* render;
    u32 i;

    if (!modelShadowIsReceiver(model)) {
        return;
    }
    render = modelGetRenderJObj(model);
    if (!enable) {
        HSD_JObjWalkTree(render,
                    _modelShadowSetShadowFlag__FP9_HSD_JObjPPvi, 0);
    } else {
        HSD_JObjWalkTree(render,
                    _modelShadowSetShadowFlag__FP9_HSD_JObjPPvi, 1);
    }
    for (i = 0; i < 6; i++) {
        if (lbl_80401490[i].flag && lbl_80401490[i].model == model) {
            HSD_ShadowSetActive(lbl_80401490[i].obj, (u8)enable);
        }
    }
}
