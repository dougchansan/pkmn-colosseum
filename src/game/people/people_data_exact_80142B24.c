/**
 * @file people_data_exact_80142B24.c
 * @brief fightItemCreate_80142B24 (0x80142B24 - 0x80142CF4): sets one field of an item
 *        record (item data kinds 1-10, ball data 11-23, fight/bag records
 *        24-34) by kind.
 *
 * Function-boundary carve of the item/people data TU, built with the
 * group's flags (GC/1.3 -O4,p). It owns its switch's jump table (.data
 * 0x80367D60-0x80367DE8), as people_data_exact_80142CF4.c does for
 * itemGetStatus. The record getters are calls here (defined in other
 * units), and the kind 24-34 record is the first parameter itself.
 */
#include "dolphin/types.h"

extern u8* itemDataBiosGetPtr(u16 idx);
extern u8* itemBallDataBiosGetPtr(u16 idx);
extern void itemDataBiosSetName(u8* p, u32 val);
extern void itemDataBiosSetKind(u8* p, u8 val);
extern void itemDataBiosSetPrice(u8* p, u16 val);
extern void itemDataBiosSetImportant(u8* p, u8 val);
extern void itemDataBiosSetUseful(u8* p, u8 val);
extern void itemDataBiosSetDoc(u8* p, u32 val);
extern void itemDataBiosSetItemSoubiDataId(u8* p, u16 val);
extern void itemDataBiosSetFightUseKoukaDataId(u8* p, u16 val);
extern void itemDataBiosSetUseFriend(u8* p, u32 idx, s8 val);
extern void itemDataBiosSetBuff(u8* p, u32 val);
extern void itemBallDataBiosSetFightKoukaDataId(u8* p, u16 val);
extern void itemBiosSetItemDataId(u8* p, u16 val);
extern void itemBiosSetNum(u8* p, u16 val);
extern void fightItemBiosSetItemDataId(u8* p, u16 val);
extern void fightItemBiosSetTargetDataId(u8* p, u16 val);
extern void fightItemBiosSetCount(u8* p, u32 val);
extern void fightItemBiosSetBuff(u8* p, u32 val);

/* 0x80142B24 | 0x1D0 */
void fightItemCreate_80142B24(u8* target, u16 id, u16 kind, u32 index, u32 value) {

    if (kind == 0 || kind >= 0x23) {
        return;
    }

    if (kind < 0xB) {
        target = itemDataBiosGetPtr(id);
        if (target == NULL) {
            return;
        }
    } else if (kind < 0x18) {
        target = itemBallDataBiosGetPtr(id);
        if (target == NULL) {
            return;
        }
    } else if (target == NULL) {
        return;
    }

    switch (kind) {
    case 1:
        itemDataBiosSetName(target, value);
        break;
    case 2:
        itemDataBiosSetKind(target, value);
        break;
    case 3:
        itemDataBiosSetPrice(target, value);
        break;
    case 4:
        itemDataBiosSetImportant(target, value);
        break;
    case 5:
        itemDataBiosSetUseful(target, value);
        break;
    case 6:
        itemDataBiosSetDoc(target, value);
        break;
    case 7:
        itemDataBiosSetItemSoubiDataId(target, value);
        break;
    case 8:
        itemDataBiosSetFightUseKoukaDataId(target, value);
        break;
    case 9:
        itemDataBiosSetUseFriend(target, index, value);
        break;
    case 10:
        itemDataBiosSetBuff(target, value);
    case 12:
        itemBallDataBiosSetFightKoukaDataId(target, value);
        break;
    case 27:
        itemBiosSetItemDataId(target, value);
        break;
    case 28:
        itemBiosSetNum(target, value);
        break;
    case 30:
        fightItemBiosSetItemDataId(target, value);
        break;
    case 31:
        fightItemBiosSetTargetDataId(target, value);
        break;
    case 32:
        fightItemBiosSetCount(target, value);
        break;
    case 33:
        fightItemBiosSetBuff(target, value);
        break;
    }
}
