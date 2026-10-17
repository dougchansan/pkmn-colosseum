/**
 * @file gs_range_exact_801DF160.c
 * @brief wazaSequenceOwnerFindAnim, 0x801DF160 - 0x801DF1D0.
 *
 * Function-boundary carve of gs_range_801DE698.c: no jump table, no pooled
 * constant, no data. GC/1.3 -O4,p like the range, no pragmas.
 */
#include "dolphin/types.h"

/* The first free-slot value in the model's active animation table: the
 * alternate table at +0xD40 when flag 2 is set with more than 16 frames,
 * unless that table is marked 1. */
s32 wazaSequenceOwnerFindAnim(u8* obj) {
    u8* base;
    u8* table;
    s32* entry;
    s32 count;

    base = *(u8**)(obj + 0x2C);
    table = base;
    if ((obj[0x18] & 2) == 2 && *(u16*)(obj + 0x14) > 0x10) {
        table = base + 0xD40;
        if (*(s32*)(table + 0x94) == 1) {
            table = base;
        }
    }

    count = *(s32*)(table + 4);
    entry = (s32*)(table + 0x8C);
    while (count-- > 0) {
        if (entry[0] == 0) {
            return entry[1];
        }
        entry += 2;
    }
    return 0;
}
