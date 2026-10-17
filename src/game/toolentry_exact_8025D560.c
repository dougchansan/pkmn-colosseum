/** Exact tool-entry party-order accessors, 0x8025D560 - 0x8025D644. */
#include "dolphin/types.h"

typedef struct BattleFieldAccessor {
    u8 unk_00[8];
    u32 values[6];
    u32 count;
} BattleFieldAccessor;

extern BattleFieldAccessor* savedataBattleGetEntryPtr(s32 index);

u32 toolentryTaisengetEtnryPokemonOrderNum(s32 index)
{
    return savedataBattleGetEntryPtr(index)->count;
}

u32 toolentryTaisenDeleteEtnryPokemonOrder(s32 trainerIndex, u32 slot, u32 param)
{
    BattleFieldAccessor* entry;
    s32 index;

    entry = savedataBattleGetEntryPtr(trainerIndex);
    index = entry->count - 1;
    if ((index < 0) || (index > 6)) {
        return 0;
    }
    entry->values[index] = (u32)-1;
    entry->count--;
    return entry->count;
}

u32 toolentryTaisenSetEtnryPokemonOrderGBA(s32 index, s32 count, u32* src)
{
    BattleFieldAccessor* dst;
    s32 i;

    dst = savedataBattleGetEntryPtr(index);
    for (i = 0; i < count; i++) {
        dst->values[i] = src[i];
    }
    dst->count = i;
    return i;
}
