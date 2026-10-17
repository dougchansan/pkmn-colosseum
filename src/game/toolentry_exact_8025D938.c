#include "dolphin/types.h"

extern u8* savedataBattleGetEntryPtr(s32 index);
extern void* heroBiosGetPokemonPtr(void* hero, u32 slot);

void* toolentryTaisenGetEntryPokemonPtr(s32 index, u16 slot)
{
    return heroBiosGetPokemonPtr(savedataBattleGetEntryPtr(index) + 0x2C, slot);
}

void* toolentryTaisenGetPokemonPtr(s32 index, u16 slot)
{
    return heroBiosGetPokemonPtr(savedataBattleGetEntryPtr(index) + 0xB44, slot);
}
