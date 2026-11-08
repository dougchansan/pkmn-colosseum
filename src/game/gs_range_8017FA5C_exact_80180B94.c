/**
 * @file gs_range_8017FA5C_exact_80180B94.c
 * @brief GSgapp job pool init fsysJobPoolInit (0x80180B94 - 0x80180C78).
 *
 * Allocates count 0x40-byte job records and clears them. Carved out of the
 * 0x8017FA5C - 0x80180C78 retail unit so the exact function can link on its
 * own; built like the rest of it at `-opt level=0` with `-inline deferred`,
 * with no local pragmas.
 */
#include "game/gs_range_8017FA5C_shared.h"

void fsysJobPoolInit(s32 count)
{
    u32 size;
    GsRangePoolElem* entry;
    s32 i;

    size = (count * sizeof(GsRangePoolElem) + 0x1F) & ~0x1F;
    lbl_8047B1E8.count = count;
    lbl_8047B1E8.base = memAlloc(size);
    lbl_8047B1E0 = NULL;
    entry = lbl_8047B1E8.base;
    lbl_8047B1E4 = NULL;
    for (i = 0; i < count; i++) {
        entry->active = 0;
        entry->field_04 = 0;
        entry->callback = NULL;
        entry->nextJob = NULL;
        entry->state = 0;
        entry->slot = NULL;
        entry->subEntry = NULL;
        entry->app = NULL;
        entry->task = NULL;
        entry++;
    }
}
