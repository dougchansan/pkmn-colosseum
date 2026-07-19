/**
 * @file fight_range_exact_8021A2C0.c
 * @brief Exact pure-C fight move-copy rejection predicate.
 */
#include "dolphin/types.h"

#pragma optimize_for_size on
u32 fightSeqMonomaneNGCheck(u16 r3)
{
    extern u16 lbl_80279FA0[];
    u16 uVar1;
    int iVar2;

    if (r3 == 0) {
        return 1;
    }
    if (r3 == 0x165) {
        return 1;
    }
    if (r3 == 0x163) {
        return 1;
    }
    iVar2 = 0;
    while ((uVar1 = lbl_80279FA0[iVar2]) != 0xfffe) {
        if (r3 == uVar1) {
            break;
        }
        iVar2 = iVar2 + 1;
    }
    return (u8)(uVar1 != 0xfffe);
}
#pragma optimize_for_size reset
