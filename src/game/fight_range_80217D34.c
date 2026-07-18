#include "dolphin/types.h"

extern u8* lbl_8047B610;

#pragma optimize_for_size on
void fn_80217D34(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u8 fn_802026E4();
    u8 index;
    u32 target;
    u32 statusId;
    s8 value;

    index = lbl_8047B610[1];
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    switch ((u8)(s8)index) {
    case 1:
        statusId = 0xe6;
        break;
    case 2:
        statusId = 0xe7;
        break;
    case 3:
        statusId = 0xea;
        break;
    case 4:
        statusId = 0xe8;
        break;
    case 5:
        statusId = 0xe9;
        break;
    case 6:
        statusId = 0xeb;
        break;
    case 7:
        statusId = 0xec;
        break;
    case 0:
    default:
        statusId = 0;
        break;
    }
    value = pokemonGetStatus(target, 0, statusId, 0);
    if (fn_802026E4(target, 9) == 1 && value >= 12) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 += 6;
    }
}
#pragma optimize_for_size reset
