/** Exact tool-entry hero pointer accessor, 0x8025D914 - 0x8025D938. */
#include "dolphin/types.h"

extern u8* savedataBattleGetEntryPtr(s32 index);

void* toolentryTaisenGetHeroPtr(s32 index)
{
    return savedataBattleGetEntryPtr(index) + 0xB44;
}
