/**
 * @file people_data_exact_80142368.c
 * @brief itemSlotsFind (0x80142368 - 0x801425E8): finds an item record in an
 *        array - the first free slot (mode 2), the first record of an item
 *        id with count below a limit (mode 1), or the first record of an id.
 *
 * Function-boundary carve of the item/people data TU (people_data.c), GC/1.3
 * -O4,p, text only. The id and record validity tests are the TU's inlines,
 * each returning a materialised u8 as in retail.
 */
#include "dolphin/types.h"

extern s32 itemGetStatus(u32, u16, u16, u32);
extern u32 lbl_80478BD8; /* gPeopleFieldCount */

static inline u8 peopleItemIdValid(u16 id)
{
    if (itemGetStatus(0, id, 1, 0) == 0) {
        return 0;
    }
    if (id >= lbl_80478BD8) {
        return 0;
    }
    return 1;
}

static inline u8 peopleEntryValid(u32* entry)
{
    u16 id;

    if (entry == NULL) {
        return 0;
    }
    id = itemGetStatus((u32)entry, 0, 0x1B, 0);
    if (id == 0) {
        return 0;
    }
    if (!peopleItemIdValid(id)) {
        return 0;
    }
    return 1;
}

static inline u8 peopleEntryIs(u32* entry, u16 id)
{
    if (entry == NULL) {
        return 0;
    }
    if (!peopleEntryValid(entry)) {
        return 0;
    }
    if (itemGetStatus((u32)entry, 0, 0x1B, 0) == id) {
        return 1;
    }
    return 0;
}

u32* itemSlotsFind(u32* base, u16 count, u16 id, u8 mode, u16 limit)
{
    u16 i;

    if (base == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (mode == 2) {
            if (!peopleEntryValid(&base[i])) {
                return &base[i];
            }
        } else if (peopleEntryIs(&base[i], id)) {
            if (mode == 1) {
                if (itemGetStatus((u32)&base[i], 0, 0x1C, 0) < limit) {
                    return &base[i];
                }
            } else {
                return &base[i];
            }
        }
    }
    return NULL;
}
