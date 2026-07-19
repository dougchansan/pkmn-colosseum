#include "dolphin/types.h"

extern u8* lbl_8047B610;

static inline u32 fn_80219E10_checkMove(u16 move)
{
    extern u16 lbl_80279FA0[];
    u16 entry;
    s32 index;

    if (move == 0) {
        return 0;
    }
    if (move == 0x165) {
        return 0;
    }
    if (move == 0x163) {
        return 0;
    }
    index = 0;
    while (1) {
        entry = lbl_80279FA0[index];
        if (entry == 0xffff) {
            break;
        }
        if (move == entry) {
            break;
        }
        index++;
    }
    if (entry != 0xffff) {
        return 0;
    }
    return 1;
}

#pragma optimize_for_size on
void fn_80219E10(void)
{
    extern u32 fn_8000814C();
    extern u32 fn_800E0C54();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 pokemonGetStatus();
    extern void fightWazaSetUseWazaStatus();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern u16 lbl_80279FA0[];
    extern u32 lbl_80379BFF[];
    u16 floorId;
    u32 attacker;
    u32 status;
    u32 count;
    u32 offset;
    u32 move;
    u32 allowed;
    u32 random;
    u32 selected;
    u32 result;
    u32 i;
    u16 local_338[404];

    floorId = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    status = pokemonGetStatus(attacker, 0, 0xd9, 0);

    for (i = 0; i < 400; i++) {
        local_338[i] = 0;
    }

    count = 0;
    offset = 0;
    for (move = 0; move < 0x163; move++) {
        if (move != 0) {
            allowed = fn_80219E10_checkMove((u16)move);
            if ((u8)allowed != 0) {
                count++;
                local_338[offset] = (u16)move;
                offset++;
            }
        }
    }

    random = (u16)fn_800E0C54();
    selected = local_338[random % count];
    result = fn_8000814C();
    if ((u16)result != 0) {
        selected = result;
    }

    lbl_8047B618 &= 0xfffffbff;
    fightWazaSetUseWazaStatus(status, selected);
    {
        u32 callResult =
            fn_8022B2CC(attacker, selected, floorId, 0, 1, 1, (void*)0xffffffff);
        result = callResult;
    }
    fightFloorSetStatus(0, 0, 0x43, 0, result);
    result = wazaGetStatus(0, selected, 9, 0);
    lbl_8047B610 = (u8*)lbl_80379BFF[(u16)result];
}
#pragma optimize_for_size reset
