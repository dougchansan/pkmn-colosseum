/**
 * @file people_data_exact_80140ACC.c
 * @brief heroItemChangeCount (0x80140ACC - 0x80141308): takes `amount` of item `id`
 *        out of an item-record array, either from the record at `index` or,
 *        when index < 0, from successive records of that id until the
 *        amount is covered. Emptied records are cleared and the array is
 *        re-sorted (itemSlotsCompact). Returns the amount still owed, or -1.
 *
 * Function-boundary carve of the item/people data TU (people_data.c), GC/1.3
 * -O4,p, text only. The validity tests are the TU's u8-materialised inlines
 * (as in people_data_exact_80142368.c); the record search, take, count and
 * clear helpers never exist out of line. The take mirrors itemSlotsAdd's add:
 * id checks, then a count helper that re-checks the record.
 */
#include "dolphin/types.h"

extern s32 itemGetStatus(u32, u16, u16, u32);
extern void fightItemCreate_80142B24(void*, u32, u16, u32, u32);
extern void itemSlotsCompact(u32* base, u16 count, u8 mode);
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

static inline u32* peopleFindEntry(u32* base, u16 count, u16 id)
{
    u16 i;

    if (base == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (peopleEntryIs(&base[i], id)) {
            return &base[i];
        }
    }
    return NULL;
}

static inline void peopleEntryClear(u32* entry)
{
    if (entry == NULL) {
        return;
    }
    fightItemCreate_80142B24(entry, 0, 0x1B, 0, 0);
    fightItemCreate_80142B24(entry, 0, 0x1C, 0, 0);
}

static inline s32 peopleEntrySubCount(u32* entry, u16 amount)
{
    s32 left;
    s32 rest;

    if (entry == NULL) {
        return -1;
    }
    if (!peopleEntryValid(entry)) {
        return -1;
    }
    left = (u16)itemGetStatus((u32)entry, 0, 0x1C, 0) - amount;
    if (left < 0) {
        rest = amount - left;
        left = 0;
    } else {
        rest = 0;
    }
    fightItemCreate_80142B24(entry, 0, 0x1C, 0, (u16)left);
    if (left <= 0) {
        peopleEntryClear(entry);
    }
    return rest;
}

static inline s32 peopleEntryTake(u32* entry, u16 id, u16 amount)
{
    if (entry == NULL) {
        return -1;
    }
    if (!peopleItemIdValid(id)) {
        return -1;
    }
    if (!peopleEntryIs(entry, id)) {
        return -1;
    }
    return peopleEntrySubCount(entry, amount);
}

s32 heroItemChangeCount(u32* base, u16 count, u16 id, u16 amount, s16 index, u16 unused, u8 sortMode)
{
    u32* entry;
    s32 rest;
    s32 remaining;
    u8 needsSort;

    if (base == NULL) {
        return -1;
    }
    if (!peopleItemIdValid(id)) {
        return -1;
    }
    needsSort = 0;
    if (index < 0) {
        remaining = amount;
        do {
            entry = peopleFindEntry(base, count, id);
            if (entry == NULL) {
                break;
            }
            if ((rest = peopleEntryTake(entry, id, remaining)) < 0) {
                return rest;
            }
            if (!peopleEntryValid(entry)) {
                needsSort = 1;
            }
            remaining = rest;
        } while (rest > 0);
    } else {
        entry = &base[index];
        remaining = peopleEntryTake(entry, id, (u16)amount);
        if (!peopleEntryValid(entry)) {
            needsSort = 1;
        }
    }
    if (needsSort == 1) {
        itemSlotsCompact(base, count, sortMode);
    }
    return remaining;
}
