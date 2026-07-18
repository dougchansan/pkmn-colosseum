/**
 * @file gs_range_801653CC_candidate_80165D0C.c
 * @brief Candidate gs-engine range, 0x80165D0C - 0x80165DEC.
 */
#include "game/gs_range_801653CC_shared.h"

void fn_80165D0C(u32 frames)
{
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern const f32 lbl_8047D544;
    f32 duration = (f32)frames / lbl_8047D544;
    f32 elapsed = lbl_8047D540;

    while (elapsed < duration) {
        _threadSwitch();
        elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}
