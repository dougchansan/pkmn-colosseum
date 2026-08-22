#include "dolphin/types.h"

extern void fadeSetFunctionOnly(s32 arg0);

extern s32 fadeEffectHookFunction_Doku(s32 slot, f32 x, f32 y, f32 z, f32 rot,
                                       f32 scale);
extern void fn_801C4CB8(void);
extern f32 fn_801C54FC(void);
extern void fn_801C5530(void);
extern f32 fadeEffectHookFunction_carde(void);
extern void fn_801C55D8(void);
extern s32 fn_801C6908(s32);
extern void fn_801C5F6C(void);
extern void fn_801C5ED0(void);
extern void fn_801C5898(void);

f32 fadeEffectHookFunction_Doku_Init(s32 slot)
{
    fadeSetFunctionOnly((s32)fadeEffectHookFunction_Doku);
}

f32 fadeEffectHookFunction_trainer_Init(s32 slot)
{
    fadeSetFunctionOnly((s32)fn_801C4CB8);
}

f32 fadeEffectHookFunction_fadein_Init(s32 slot)
{
    fadeSetFunctionOnly((s32)fn_801C54FC);
}

void fadeEffectHookFunction_fadeout_in_Init(s32 slot, f32 x)
{
    fadeSetFunctionOnly((s32)fn_801C5530);
}

void fadeEffectHookFunction_carde_Init(s32 slot, f32 y)
{
    fadeSetFunctionOnly((s32)fadeEffectHookFunction_carde);
}

void fadeEffectHookFunction_boss_Init(s32 slot, f32 z)
{
    fadeSetFunctionOnly((s32)fn_801C55D8);
}

void fadeEffectHookFunction_yoko_or_tate_or_ball_Init(s32 slot, f32 x, f32 y,
                                                      f32 z)
{
    s32 result = fn_801C6908(3);

    switch (result) {
    case 0:
        fadeSetFunctionOnly((s32)fn_801C5F6C);
        break;
    case 1:
        fadeSetFunctionOnly((s32)fn_801C5ED0);
        break;
    case 2:
    default:
        fadeSetFunctionOnly((s32)fn_801C5898);
        break;
    }
}

f32 fadeEffectHookFunction_ball_Init(s32 slot)
{
    fadeSetFunctionOnly((s32)fn_801C5898);
}

void fadeEffectHookFunction_yoko_or_tate_Init(s32 slot, f32 rotation)
{
    s32 result = fn_801C6908(2);
    switch (result) {
    case 0:
        fadeSetFunctionOnly((s32)fn_801C5F6C);
        break;
    case 1:
    default:
        fadeSetFunctionOnly((s32)fn_801C5ED0);
        break;
    }
}

f32 fadeEffectHookFunction_tate_Init(s32 slot)
{
    fadeSetFunctionOnly((s32)fn_801C5ED0);
}

void fadeEffectHookFunction_yoko_Init(s32 slot, f32 scale)
{
    fadeSetFunctionOnly((s32)fn_801C5F6C);
}
