#include "dolphin/types.h"

extern u32 lbl_80478BD8;

extern s32 itemGetStatus(u32 entry, u16 id, u16 field, u32 value);

s32 itemIdIsValid(u16 id)
{
    s32 result;

    result = itemGetStatus(0, id, 1, 0);
    if (result == 0) {
        return 0;
    }
    return lbl_80478BD8 > id;
}

s32 itemSlotIsValid(u32 entry)
{
    u16 id;
    u8 valid;

    if (entry == 0) {
        return 0;
    }

    id = (u16)itemGetStatus(entry, 0, 0x1B, 0);
    if (id == 0) {
        return 0;
    }

    if (itemGetStatus(0, id, 1, 0) == 0) {
        valid = 0;
    } else if (id >= lbl_80478BD8) {
        valid = 0;
    } else {
        valid = 1;
    }

    return valid != 0;
}
