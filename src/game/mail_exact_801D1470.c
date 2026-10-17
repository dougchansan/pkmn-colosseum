#include "game/battle/battle_waza_types.h"

u32 mailGetNbMailData(void)
{
    return *lbl_80478E98;
}

s32 mailDataBiosGet28(s32 idx)
{
    WazaEntry* entry;

    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return -1;
    return entry->resourceID;
}

s32 mailDataBiosGet24(s32 idx)
{
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return -1;
    return entry->field_24;
}

s32 mailDataBiosGet20(s32 idx)
{
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return -1;
    return entry->field_20;
}

s32 mailGetSendCondition(s32 idx)
{
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return -1;
    return entry->sendCondition;
}

u32 mailGetSendCondType(s32 idx)
{
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return 0xFF;
    return entry->sendCondType;
}

u32 mailGetSendRate(s32 idx)
{
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) return 0xFFFF;
    return entry->sendRate;
}

u32 mailGetMailCount(void)
{
    return lbl_80478CB8;
}

u32 mailGetEntryField4(u32 idx)
{
    s32 slot = idx & 0xFF;
    if (slot >= lbl_80478CB8) {
        return 0;
    }
    return lbl_8036E0E0[slot * 2 + 1];
}

u32 mailGetEntryField0(u32 idx)
{
    s32 slot = idx & 0xFF;
    if (slot >= lbl_80478CB8) {
        return 0;
    }
    return lbl_8036E0E0[slot * 2];
}
