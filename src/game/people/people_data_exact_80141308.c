/**
 * @file people_data_exact_80141308.c
 * @brief itemSlotsAdd (0x80141308 - 0x80142368): adds `amount` of item `id`
 *        to an item-record array, capping each record at `max`. Without
 *        `force` it tops up the first record of that id (if below max) or
 *        fills a free record; with `force` it either tops up the record at
 *        `index` or, when index < 0, keeps topping up records below max and
 *        filling free ones until the amount is placed. New records trigger
 *        a re-sort (itemSlotsCompact). Returns the amount left over, or -1.
 *
 * Function-boundary carve of the item/people data TU (people_data.c), GC/1.3
 * -O4,p, text only. The validity tests are the TU's u8-materialised inlines.
 * The three record searches are itemSlotsFind's modes (0, 1 and 2) written
 * out, and the add/put/count helpers mirror heroItemChangeCount's take; none of the
 * helpers exists out of line.
 */
#include "dolphin/types.h"

extern s32 itemGetStatus(void*, u16, u16, u32);
extern void fightItemCreate_80142B24(void*, u32, u16, u32, u32);
extern void itemSlotsCompact(u32* base, u16 count, u8 mode);
extern u32 lbl_80478BD8; /* gPeopleFieldCount */

static inline u8 peopleItemIdValid(u16 id)
{
    if (itemGetStatus(NULL, id, 1, 0) == 0) {
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
    id = itemGetStatus(entry, 0, 0x1B, 0);
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
    if (itemGetStatus(entry, 0, 0x1B, 0) == id) {
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

static inline u32* peopleFindEntryBelow(u32* base, u16 count, u16 id, u16 limit)
{
    u16 i;

    if (base == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (peopleEntryIs(&base[i], id) && itemGetStatus(&base[i], 0, 0x1C, 0) < limit) {
            return &base[i];
        }
    }
    return NULL;
}

static inline u32* peopleFindFreeEntry(u32* base, u16 count, u16 id)
{
    u16 i;

    if (base == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (!peopleEntryValid(&base[i])) {
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

static inline s32 peopleEntryAddCount(u32* entry, u16 amount, u16 max)
{
    s32 sum;
    s32 rest;
    u16 num;

    if (entry == NULL) {
        return -1;
    }
    if (!peopleEntryValid(entry)) {
        return -1;
    }
    num = itemGetStatus(entry, 0, 0x1C, 0);
    sum = num + amount;
    if (sum > max) {
        rest = sum - max;
        sum = max;
    } else {
        rest = 0;
    }
    fightItemCreate_80142B24(entry, 0, 0x1C, 0, (u16)sum);
    return rest;
}

static inline s32 peopleEntryAdd(u32* entry, u16 id, u16 amount, u16 max)
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
    return peopleEntryAddCount(entry, amount, max);
}

static inline s32 peopleEntryPut(u32* entry, u16 id, u16 amount, u16 max)
{
    s32 rest;

    if (entry == NULL) {
        return -1;
    }
    if (!peopleItemIdValid(id)) {
        return -1;
    }
    peopleEntryClear(entry);
    fightItemCreate_80142B24(entry, 0, 0x1B, 0, id);
    rest = peopleEntryAddCount(entry, amount, max);
    if (rest < 0) {
        return rest;
    }
    return rest;
}

s32 itemSlotsAdd(u32* base, u16 count, u16 id, u16 amount, s16 index, u16 max, u8 sortMode, u8 force)
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
    remaining = (u16)amount;
    needsSort = 0;
    if (!force) {
        entry = peopleFindEntry(base, count, id);
        if (entry != NULL) {
            if (itemGetStatus(entry, 0, 0x1C, 0) < max) {
                if ((rest = peopleEntryAdd(entry, id, remaining, max)) < 0) {
                    return rest;
                }
                remaining = rest;
            }
        } else {
            entry = peopleFindFreeEntry(base, count, id);
            if (entry != NULL) {
                if ((rest = peopleEntryPut(entry, id, remaining, max)) < 0) {
                    return rest;
                }
                remaining = rest;
                needsSort = 1;
            }
        }
    } else if (index < 0) {
        for (;;) {
            entry = peopleFindEntryBelow(base, count, id, max);
            if (entry != NULL) {
                if ((rest = peopleEntryAdd(entry, id, remaining, max)) < 0) {
                    return rest;
                }
                remaining = rest;
            } else {
                entry = peopleFindFreeEntry(base, count, id);
                if (entry == NULL) {
                    break;
                }
                if ((rest = peopleEntryPut(entry, id, remaining, max)) < 0) {
                    return rest;
                }
                needsSort = 1;
                remaining = rest;
            }
            if (remaining <= 0) {
                break;
            }
        }
    } else {
        remaining = peopleEntryAdd(&base[index], id, (u16)amount, max);
    }
    if (needsSort == 1) {
        itemSlotsCompact(base, count, sortMode);
    }
    return remaining;
}
