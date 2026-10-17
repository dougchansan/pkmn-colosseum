/** Exact tool-entry party-order initializer, 0x8025D744 - 0x8025D788. */
#include "dolphin/types.h"

extern u8* savedataBattleGetEntryPtr(s32 index);

u32 toolentryTaisenInitPokemonOrder(s32 index, u32 slot, u32 param)
{
    u8* base;
    u32 i;

    base = savedataBattleGetEntryPtr(index);
    *(u32*)(base + 0x20) = 0;
    for (i = 0; i < 6; i++) {
        *(u32*)(base + 0x8 + i * 4) = (u32)-1;
    }
    return (u32)base;
}
