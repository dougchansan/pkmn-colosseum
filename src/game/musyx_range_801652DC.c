/**
 * @file musyx_range_801652DC.c
 * @brief musyx code, 0x801652DC - 0x801653BC (1 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

void ReverbHICallback(s32* left, s32* right, s32* surround, u8* reverb)
{
    extern f32 lbl_8047D4F0;
    extern f32 lbl_8047D4F4;
    extern f32 lbl_8047D538;
    extern void ReverbHIFilterBlock(s32* left, s32* right, f32 cross, f32 invCross);
    extern void HandleReverb(s32* samples, u8* reverb, s32 channel);
    u8 channel;

    for (channel = 0; channel < 3; channel++) {
        switch (channel) {
        case 0:
            if (lbl_8047D4F0 != *(f32*)(reverb + 0x1A8)) {
                ReverbHIFilterBlock(left, right,
                            lbl_8047D538 * *(f32*)(reverb + 0x1A8),
                            lbl_8047D4F4 -
                                (lbl_8047D538 * *(f32*)(reverb + 0x1A8)));
            }
            HandleReverb(left, reverb, 0);
            break;
        case 1:
            HandleReverb(right, reverb, 1);
            break;
        case 2:
            HandleReverb(surround, reverb, 2);
            break;
        }
    }
}
