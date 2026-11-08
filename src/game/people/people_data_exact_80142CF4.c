/*
 * people_data TU: itemGetStatus, the item/ball/equipment/bag-entry field
 * query, with its switch jump table (.data 0x80367DE8-0x80367E70).
 */
#include "dolphin/types.h"

extern u8* itemDataBiosGetPtr(u16 id);
extern u8* itemBallDataBiosGetPtr(u16 id);
extern u8* itemSoubiDataBiosGetPtr(u16 id);
extern u32 itemDataBiosGetName(u8* p);
extern u8 itemDataBiosGetKind(u8* p);
extern u32 itemDataBiosGetPrice(u8* p);
extern u8 itemDataGetField1(u8* p);
extern u8 itemDataGetField2(u8* p);
extern u32 itemDataBiosGetDoc(u8* p);
extern u32 itemDataBiosGetItemSoubiDataId(u8* p);
extern u32 itemDataBiosGetFightUseKoukaDataId(u8* p);
extern s32 itemDataBiosGetUseFriend(u8* p, u16 index);
extern u32 itemDataBiosGetBuff(u8* p);
extern u32 itemBallDataBiosGetFightKoukaDataId(u8* p);
extern u32 itemBallDataBiosGetInWzxDataId(u8* p);
extern u32 itemBallDataBiosGetOpenWzxDataId(u8* p);
extern u32 itemBallDataBiosGetOutWzxDataId(u8* p);
extern u32 itemBallDataBiosGetDowninWzxDataId(u8* p);
extern u32 itemBallDataBiosGetThrowWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchAttackWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchBalllandWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchMissWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchShakeWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchSnatchWzxDataId(u8* p);
extern u16 itemSoubiDataBiosGetFightKoukaDataId(u8* p);
extern u32 itemBiosGetItemDataId(u8* p);
extern u32 itemBiosGetNum(u8* p);
extern u16 fightItemBiosGetItemDataId(u8* p);
extern u16 fightItemBiosGetTargetDataId(u8* p);
extern u32 fightItemBiosGetCount(u8* p);
extern u32 fightItemBiosGetBuff(u8* p);

/*
 * Read field `field` of an item record: fields 1-10 come from item `id`'s
 * item data, 11-23 from its ball data, 24-25 from its equipment data, and
 * 26-33 from the bag/battle item record `record`. `arg` is the friend
 * index for field 9. Unknown fields read as 0.
 */
s32 itemGetStatus(u8* record, u16 id, u16 field, u16 arg)
{
    u8* data;

    if (field == 0 || field >= 0x23) {
        return 0;
    }

    if (field < 0xB) {
        data = itemDataBiosGetPtr(id);
        if (data == NULL) {
            return 0;
        }
    } else if (field < 0x18) {
        data = itemBallDataBiosGetPtr(id);
        if (data == NULL) {
            return 0;
        }
    } else if (field < 0x1A) {
        data = itemSoubiDataBiosGetPtr(id);
        if (data == NULL) {
            return 0;
        }
    } else {
        if (record == NULL) {
            return 0;
        }
        data = record;
    }

    switch (field) {
    case 1:
        return itemDataBiosGetName(data);
    case 2:
        return itemDataBiosGetKind(data);
    case 3:
        return (u16)itemDataBiosGetPrice(data);
    case 4:
        return itemDataGetField1(data);
    case 5:
        return itemDataGetField2(data);
    case 6:
        return itemDataBiosGetDoc(data);
    case 7:
        return (u16)itemDataBiosGetItemSoubiDataId(data);
    case 8:
        return (u16)itemDataBiosGetFightUseKoukaDataId(data);
    case 9:
        return (s8)itemDataBiosGetUseFriend(data, arg);
    case 10:
        return itemDataBiosGetBuff(data);
    case 12:
        return (u16)itemBallDataBiosGetFightKoukaDataId(data);
    case 13:
        return itemBallDataBiosGetInWzxDataId(data);
    case 14:
        return itemBallDataBiosGetOpenWzxDataId(data);
    case 15:
        return itemBallDataBiosGetOutWzxDataId(data);
    case 16:
        return itemBallDataBiosGetDowninWzxDataId(data);
    case 17:
        return itemBallDataBiosGetThrowWzxDataId(data);
    case 18:
        return itemBallDataBiosGetSnatchAttackWzxDataId(data);
    case 19:
        return itemBallDataBiosGetSnatchBalllandWzxDataId(data);
    case 20:
        return itemBallDataBiosGetSnatchMissWzxDataId(data);
    case 21:
        return itemBallDataBiosGetSnatchPokeoutWzxDataId(data);
    case 22:
        return itemBallDataBiosGetSnatchShakeWzxDataId(data);
    case 23:
        return itemBallDataBiosGetSnatchSnatchWzxDataId(data);
    case 25:
        return itemSoubiDataBiosGetFightKoukaDataId(data);
    case 27:
        return (u16)itemBiosGetItemDataId(data);
    case 28:
        return (u16)itemBiosGetNum(data);
    case 30:
        return fightItemBiosGetItemDataId(data);
    case 31:
        return fightItemBiosGetTargetDataId(data);
    case 32:
        return fightItemBiosGetCount(data);
    case 33:
        return fightItemBiosGetBuff(data);
    default:
        return 0;
    }
}
