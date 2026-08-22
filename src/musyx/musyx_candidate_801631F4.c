/**
 * @file musyx_candidate_801631F4.c
 * @brief Standalone owner for fn_801631F4 at 0x801631F4.
 */

#include "musyx/runtime/hw_dspctrl.h"

u32 fn_801631F4(u32 index)
{
    u8 v = lbl_8047B024[index].state;
    u32 diff = 1 - v;
    return (u32)(diff == 0);
}
