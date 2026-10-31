/**
 * @file people_item_getters_exact_80143C50.c
 * @brief Strict item and item-ball accessors, 0x80143C50 - 0x80144064.
 *
 * itemDataBiosCheckExportable (0x8014402C) expands the important-flag getter
 * itemDataGetField1 above it (retail re-tests the pointer inside the expansion),
 * so it links in this unit rather than on its own.
 */
#include "dolphin/types.h"

typedef struct ItemBiosData {
    u16 itemDataId;
    u16 num;
} ItemBiosData;

typedef struct ItemBallDataBios {
    u16 fightKoukaDataId;
    u16 padding_02;
    u32 inWzxDataId;
    u32 openWzxDataId;
    u32 outWzxDataId;
    u32 downinWzxDataId;
    u32 throwWzxDataId;
    u32 snatchAttackWzxDataId;
    u32 snatchBalllandWzxDataId;
    u32 snatchMissWzxDataId;
    u32 snatchPokeoutWzxDataId;
    u32 snatchShakeWzxDataId;
    u32 snatchSnatchWzxDataId;
} ItemBallDataBios;

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

typedef struct WazaMachineData {
    u32 field_00;
    u32 wazaId;
} WazaMachineData;

extern u32 lbl_80478BB8;
extern u32 lbl_80478BD0;
extern ItemBallDataBios lbl_80367AF0[];
extern WazaMachineData lbl_80368018[];

u32 itemBiosGetNum(u8* p)
{
    ItemBiosData* item = (ItemBiosData*)p;
    if (item == NULL) {
        return 0;
    }
    return item->num;
}

u32 itemBiosGetItemDataId(u8* p)
{
    ItemBiosData* item = (ItemBiosData*)p;
    if (item == NULL) {
        return 0;
    }
    return item->itemDataId;
}

u32 itemBallDataBiosGetSnatchSnatchWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchSnatchWzxDataId;
}

u32 itemBallDataBiosGetSnatchShakeWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchShakeWzxDataId;
}

u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchPokeoutWzxDataId;
}

u32 itemBallDataBiosGetSnatchMissWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchMissWzxDataId;
}

u32 itemBallDataBiosGetSnatchBalllandWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchBalllandWzxDataId;
}

u32 itemBallDataBiosGetSnatchAttackWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->snatchAttackWzxDataId;
}

u32 itemBallDataBiosGetThrowWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->throwWzxDataId;
}

u32 itemBallDataBiosGetDowninWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->downinWzxDataId;
}

u32 itemBallDataBiosGetOutWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->outWzxDataId;
}

u32 itemBallDataBiosGetOpenWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->openWzxDataId;
}

u32 itemBallDataBiosGetInWzxDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->inWzxDataId;
}

u32 itemBallDataBiosGetFightKoukaDataId(u8* p)
{
    ItemBallDataBios* data = (ItemBallDataBios*)p;
    if (data == NULL) {
        return 0;
    }
    return data->fightKoukaDataId;
}

u8* itemBallDataBiosGetPtr(u16 idx)
{
    u16 ballId = idx;
    if (ballId >= lbl_80478BB8) {
        return NULL;
    }
    return (u8*)&lbl_80367AF0[ballId];
}

u32 itemDataBiosGetBattleUseFunc(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->battleUseFunc;
}

u32 itemDataBiosGetFieldUseFunc(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->fieldUseFunc;
}

u32 itemDataBiosGetItemEffectParam(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->itemEffectParam;
}

u32 itemDataBiosGetBuff(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->buff;
}

s32 itemDataBiosGetUseFriend(u8* p, u16 idx)
{
    ItemDataBios* item = (ItemDataBios*)p;
    u16 friendIdx;

    if (item == NULL) {
        return 0;
    }
    friendIdx = idx;
    if (friendIdx >= 3) {
        return 0;
    }
    return (s8)((u8)item->useFriend[friendIdx]);
}

u32 itemDataBiosGetKinomiNo(u16 itemId)
{
    u16 itemNo = itemId;
    if (itemNo < 0x85 || itemNo > 0xAF) {
        return 0xFF;
    }
    return (u8)(itemId - 0x85);
}

u32 itemDataBiosGetHidenMachineNo(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    u8 machineNo;

    if (item == NULL) {
        machineNo = 0;
    } else if (item->kind != 4) {
        machineNo = 0xFF;
    } else {
        machineNo = (u8)item->buff;
    }

    if (machineNo == 0xFF) {
        return 0xFF;
    }
    if (machineNo < 0x32 || machineNo >= lbl_80478BD0) {
        return 0xFF;
    }
    return (u8)(machineNo - 0x32);
}

u32 itemDataBiosGetWazaIDByWazaMachineNo(u32 machineNo)
{
    if ((u8)machineNo >= lbl_80478BD0) {
        return 0;
    }
    return (u16)lbl_80368018[(u8)machineNo].wazaId;
}

u32 itemDataBiosGetWazaMachineNo(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    if (item->kind != 4) {
        return 0xFF;
    }
    return (u8)item->buff;
}

u32 itemDataBiosGetFightUseKoukaDataId(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->fightUseKoukaDataId;
}

u32 itemDataBiosGetItemSoubiDataId(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->itemSoubiDataId;
}

u32 itemDataBiosGetDoc(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->doc;
}

u8 itemDataGetField3(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->field_03;
}

u8 itemDataGetField2(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->useful;
}

u8 itemDataGetField1(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->important;
}

u32 itemDataBiosGetCoupon(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->coupon;
}

u32 itemDataBiosGetPrice(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->price;
}

u8 itemDataBiosGetKind(u8* p)
{
    ItemDataBios* item = (ItemDataBios*)p;
    if (item == NULL) {
        return 0;
    }
    return item->kind;
}

s32 itemDataBiosCheckExportable(u8* p)
{
    if (p == NULL) {
        return 0;
    }
    if (itemDataGetField1(p)) {
        return 0;
    }
    return 1;
}
