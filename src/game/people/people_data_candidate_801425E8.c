/**
 * @file people_data_candidate_801425E8.c
 * @brief itemSlotsCompact (0x801425E8 - 0x80142984, exact): compacts an
 *        item-record array (mode 0/1) and sorts it by item id (mode 1).
 *
 * The record swap is the TU's itemSwapSlots, inlined. The sort compare reads
 * both ids into u16 locals first (entry, then other), so the entry id gets
 * its own callee-saved web (r22) and retail's operand order; the inline
 * one-expression compare evaluated the other record first and shifted the
 * sort loop's register assignment.
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

static inline void peopleSwapEntry(u32* a, u32* b)
{
    volatile u32 saved;
    u32 tmp;

    if (a == NULL) {
        return;
    }
    if (b == NULL) {
        return;
    }
    tmp = (saved = *a);
    *a = *b;
    *b = tmp;
}

void itemSlotsCompact(u32* base, u16 count, u8 mode)
{
    s32 i;
    s32 j;
    u32* entry;
    u32* other;
    u16 a;
    u16 b;

    if (base == NULL) {
        return;
    }
    if (count == 0) {
        return;
    }
    if (mode == 0 || mode == 1) {
        for (i = 0; i < count - 1; i++) {
            entry = &base[i];
            if (!peopleEntryValid(entry)) {
                for (j = i + 1; j < count; j++) {
                    if (peopleEntryValid(&base[j]) == 1) {
                        peopleSwapEntry(entry, &base[j]);
                        break;
                    }
                }
            }
        }
    }
    if (mode == 1) {
        for (i = 0; i < count - 1; i++) {
            entry = &base[i];
            if (peopleEntryValid(entry)) {
                for (j = i + 1; j < count; j++) {
                    other = &base[j];
                    if (peopleEntryValid(other)) {
                        a = itemGetStatus((u32)entry, 0, 0x1B, 0);
                        b = itemGetStatus((u32)other, 0, 0x1B, 0);
                        if (a > b) {
                            peopleSwapEntry(entry, other);
                        }
                    }
                }
            }
        }
    }
}
