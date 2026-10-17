/**
 * @file gs_math_random_exact_800E0C54.c
 * @brief Exact integer RNG and seed helpers.
 *
 * Address range: 0x800E0C54 - 0x800E0CA0.
 */

#include "dolphin/types.h"

extern u64 OSGetTime(void);
extern u32 HSD_Rand(void);
extern u32* lbl_80478C94;

u16 GSrand(void)
{
    return HSD_Rand();
}

void fn_800E0C78(void)
{
    u64 time = OSGetTime();

    *lbl_80478C94 = (u32)time;
}
