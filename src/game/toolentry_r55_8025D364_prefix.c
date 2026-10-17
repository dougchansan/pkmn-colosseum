/**
 * @file toolentry_r55_8025D364_prefix.c
 * @brief toolentryGetTrainerBicFaceResID (0x8025D364 - 0x8025D3F4): the
 *        big-face resource ID of the entry's trainer.
 *
 * Function-boundary carve of toolentry.c, text only, GC/1.3 -O4,p. Retail
 * calls savedataBattleGetEntryPtr on both arms of the context test (the NULL arm passes
 * the NULL context through), so the body keeps both calls. The same body is
 * in toolentry.c.
 */
#include "dolphin/types.h"

u32 toolentryGetTrainerBicFaceResID(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478E04;
    extern void* savedataBattleGetEntryPtr(void*);
    extern u32 fn_801FCBA4(void);
    extern void fightTrainerDataBiosGetPtr(u32);
    u16 id16;
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    if ((s32)ctx != 0) {
        id16 = *(u16*)savedataBattleGetEntryPtr(ctx);
    } else {
        id16 = *(u16*)savedataBattleGetEntryPtr(ctx);
    }
    id = id16;
    if (id == 0) {
        return 0;
    }
    fightTrainerDataBiosGetPtr(id);
    offset = fn_801FCBA4();
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        return entry[1];
    }
    ret = entry[2];
    if (ret == 0) {
        return 0xf991200;
    }
    return ret;
}
