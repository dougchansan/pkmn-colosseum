/**
 * @file people_item_tail_exact_80144064.c
 * @brief Strict item-data tail, 0x80144064 - 0x801441A8.
 */
#include "dolphin/types.h"

typedef struct PeopleFieldEntry {
    f32 field_00;
    u8 flags_04;
    s8 field_05;
    s8 field_06;
    s8 field_07;
    u32 field_08;
    u8 field_0C;
    u8 field_0D;
    u8 field_0E;
    u8 field_0F;
    f32 posX;
    f32 posY;
    f32 posZ;
    f32 rotAngle;
    f32 scale;
    u32 modelRef;
} PeopleFieldEntry;

typedef struct ItemDataBios {
    u8 kind;
    u8 important;
    u8 useful;
    u8 field_03;
    u8 itemEffectParam;
    u8 padding_05;
    u16 price;
    u16 coupon;
    u16 itemSoubiDataId;
    u16 fightUseKoukaDataId;
    u16 padding_0E;
    u32 name;
    u32 doc;
    u32 buff;
    u32 fieldUseFunc;
    u32 battleUseFunc;
    s8 useFriend[3];
    u8 padding_27;
} ItemDataBios;

extern u32 lbl_80478BD8;
extern u32 lbl_80478BB0;
extern u16 lbl_803681E8[];
extern PeopleFieldEntry lbl_80363CE8[];

extern void* fightFloorGetNowPtr(void);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void* memcpy(void* dst, const void* src, u32 size);
extern s32 fn_80144574(void*, void*, void*, void*, void*);
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);

s32 itemDataBiosCheckImportable(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->name != 0;
}

u32 itemDataBiosGetName(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->name;
}

u8* itemDataBiosGetPtr(u16 index)
{
    u16 slot;

    if (index >= lbl_80478BD8) {
        return NULL;
    }
    slot = lbl_803681E8[index];
    if (slot >= lbl_80478BB0) {
        return NULL;
    }
    return (u8*)&lbl_80363CE8[slot];
}

s32 itemUse2PokemonSimulation(
    void* arg0, void* arg1, void* arg2, void* arg3, void* arg4)
{
    void* savedFightFloor;
    void* fightFloor;
    u16 memHandle;
    s32 result;

    fightFloor = fightFloorGetNowPtr();
    if (fightFloor == NULL) {
        return 0;
    }

    memHandle = _toolentryAlloc__FUl(0xA4E8);
    savedFightFloor = GSmemLock(memHandle);
    memcpy(savedFightFloor, fightFloor, 0xA4E8);
    result = fn_80144574(arg0, arg1, arg2, arg3, arg4);
    memcpy(fightFloor, savedFightFloor, 0xA4E8);
    fn_800E24B0(memHandle);
    GSmemFree(memHandle);
    return result;
}
