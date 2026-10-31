/**
 * @file people_data_r49_80140588_o4s.c
 * @brief peopleFieldGetPlacementOffset (0x80140588 - 0x80140A9C): the room left for item
 *        `dataIndex` in an item-record array. Without `accumulate` it is
 *        `extent` minus the matching record's count (or `extent` for a free
 *        record, 0 when neither exists); with it, the sum over all records.
 *
 * Function-boundary carve of the item/people data TU (people_data.c), GC/1.3
 * -O4,p like its exact siblings (the unit name predates the -O4,p finding),
 * text only. The validity, match and search helpers are the TU's
 * u8-materialised inlines, copied from people_data.c; keep the two in step.
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
    id = itemGetStatus((u32)entry, 0, 0x1b, 0);
    if (id == 0) {
        return 0;
    }
    if (!peopleItemIdValid(id)) {
        return 0;
    }
    return 1;
}

static inline u8 peopleEntryMatches(u32* entry, u16 id)
{
    if (entry == NULL) {
        return 0;
    }
    if (!peopleEntryValid(entry)) {
        return 0;
    }
    if (id == itemGetStatus((u32)entry, 0, 0x1b, 0)) {
        return 1;
    }
    return 0;
}

static inline u32* peopleFindEntry(u32* entries, u16 count, u16 id)
{
    u16 i;

    if (entries == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (peopleEntryMatches(&entries[i], id)) {
            return &entries[i];
        }
    }
    return NULL;
}

static inline u32* peopleFindFreeEntry(u32* entries, u16 count, u16 id)
{
    u16 i;

    if (entries == NULL) {
        return NULL;
    }
    if (!peopleItemIdValid(id)) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (!peopleEntryValid(&entries[i])) {
            return &entries[i];
        }
    }
    return NULL;
}

/* Resolve the placement offset for a field-person record group. */
s32 peopleFieldGetPlacementOffset(u32* entries, u16 count, u16 dataIndex, u16 extent,
                u8 accumulate)
{
    u32* entry;
    s32 result;
    u16 i;

    if (entries == NULL) {
        return -1;
    }
    if (!peopleItemIdValid(dataIndex)) {
        return -1;
    }

    if (accumulate == 0) {
        entry = peopleFindEntry(entries, count, dataIndex);
        if (entry != NULL) {
            result = extent - itemGetStatus((u32)entry, 0, 0x1c, 0);
        } else if (peopleFindFreeEntry(entries, count, dataIndex) != NULL) {
            result = extent;
        } else {
            result = 0;
        }
    } else {
        result = 0;
        for (i = 0; i < count; i++) {
            entry = &entries[i];
            if (!peopleEntryValid(entry)) {
                result += extent;
            } else if (peopleEntryMatches(entry, dataIndex) == 1) {
                result += extent - itemGetStatus((u32)entry, 0, 0x1c, 0);
            }
        }
    }

    return result;
}
