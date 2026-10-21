/**
 * @file pda_exact_80041114.c
 * @brief PDA scene entry helpers, 0x80041114 - 0x800411FC: pdaSceneEnter0,
 *        pdaSceneEnter1, pdaSceneEnter2 and pdaSceneGetWork (the scene-work field at
 *        0x28 of lbl_803A6818).
 */
#include "dolphin/types.h"

extern u8 lbl_803A67FC[];
extern u8 lbl_803A6818[];
extern void GSscene_SetMode(s32 mode);
extern void pdaReadPadInput(void* scene);
extern s32 pdaMemoUpdateScroll(void* work, s32 mode);
extern s32 pdaModelViewerControl(void* work, s32 mode);

void pdaSceneEnter0(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaMemoUpdateScroll(work, 0);
}

void pdaSceneEnter1(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaModelViewerControl(work, 1);
}

void pdaSceneEnter2(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaModelViewerControl(work, 0);
}

s32 pdaSceneGetWork(void)
{
    return *(s32*)(lbl_803A6818 + 0x28);
}
