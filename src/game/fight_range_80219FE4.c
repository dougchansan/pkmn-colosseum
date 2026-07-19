/**
 * @file fight_range_80219FE4.c
 * @brief Fight-sequence move filter, 0x80219FE4 - 0x8021A054.
 */
#include "dolphin/types.h"

extern u16 lbl_80279FA0[];

#pragma dont_inline on
#pragma optimize_for_size on
u32 fightSeqCheckYubiwohuruWazaDataId(u16 move_id) {
    u16 entry;
    s32 index;

    if (move_id == 0) {
        return 0;
    }
    if (move_id == 0x165) {
        return 0;
    }
    if (move_id == 0x163) {
        return 0;
    }
    index = 0;
    while (1) {
        entry = lbl_80279FA0[index];
        if (entry == 0xffff) {
            break;
        }
        if (move_id == entry) {
            break;
        }
        index = index + 1;
    }
    if (entry != 0xffff) {
        return 0;
    }
    return 1;
}
#pragma optimize_for_size reset
#pragma dont_inline reset
