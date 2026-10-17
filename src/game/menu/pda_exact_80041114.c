/**
 * @file pda_exact_80041114.c
 * @brief PDA scene entry helpers, 0x80041114 - 0x800411FC: fn_80041114,
 *        fn_8004115C, fn_800411A4 and fn_800411EC (the scene-work field at
 *        0x28 of lbl_803A6818).
 */
#include "dolphin/types.h"

extern u8 lbl_803A67FC[];
extern u8 lbl_803A6818[];
extern void GSscene_SetMode(s32 mode);
extern void pdaReadPadInput(void* scene);
extern s32 fn_80041E48(void* work, s32 mode);
extern s32 fn_80042658(void* work, s32 mode);

void fn_80041114(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    fn_80041E48(work, 0);
}

void fn_8004115C(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    fn_80042658(work, 1);
}

void fn_800411A4(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    fn_80042658(work, 0);
}

s32 fn_800411EC(void)
{
    return *(s32*)(lbl_803A6818 + 0x28);
}
