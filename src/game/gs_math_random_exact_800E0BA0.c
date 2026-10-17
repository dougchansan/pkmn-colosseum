/**
 * @file gs_math_random_exact_800E0BA0.c
 * @brief Exact floating-point random helpers.
 *
 * Address range: 0x800E0BA0 - 0x800E0C04.
 */

#include "dolphin/types.h"

extern f32 HSD_Randf(void);
extern f32 lbl_8047CB10;

f32 fn_800E0BA0(void)
{
    f32 a = HSD_Randf();
    f32 b = HSD_Randf();

    return b + a - lbl_8047CB10;
}

f32 GSrandf(void)
{
    return HSD_Randf();
}
