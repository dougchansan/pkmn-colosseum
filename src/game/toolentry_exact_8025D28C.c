/** Exact tool-entry trainer accessors, 0x8025D28C - 0x8025D364. */
#include "dolphin/types.h"

extern u8* savedataBattleGetEntryPtr(s32 index);

u16 toolentryTaisenGetTrainerDataID(s32 index)
{
    return *(u16*)savedataBattleGetEntryPtr(index);
}

u32 toolentryTaisenGetControlerType(s32 index)
{
    return *(u32*)(savedataBattleGetEntryPtr(index) + 0x24);
}

u32 toolentryGetTrainerSamllFaceResID(s32 index, u32 param1, u32 param2)
{
    extern u32 lbl_80478E04;
    extern u32 fn_801FCBA4(u8* trainer);
    extern u8* fightTrainerDataBiosGetPtr(u16 id);
    u8* trainer;
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    id = *(u16*)savedataBattleGetEntryPtr(index);
    if (id == 0) {
        return 0;
    }
    trainer = fightTrainerDataBiosGetPtr(id);
    offset = fn_801FCBA4(trainer);
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        ret = entry[3];
        if (ret == 0) {
            return 0xF941200;
        }
        return ret;
    }
    ret = entry[4];
    if (ret == 0) {
        return 0xF8F1200;
    }
    return ret;
}
