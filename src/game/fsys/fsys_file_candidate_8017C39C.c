/**
 * @file fsys_file_candidate_8017C39C.c
 * @brief FSYS status-304 handler (0x8017C39C - 0x8017C414).
 *
 * Entry of the slot status dispatch table lbl_8036C3E0 (status 304): mark
 * the current entry's trailer state 4 (retail stores the same value on the
 * compressed and the plain path) and move the slot to status 100.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

/* Address: 0x8017C39C | size: 0x78 */
s32 fsysStatus304(FSYSSlot* slot)
{
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;

    entry = fsysGetEntry(slot, slot->entryIndex);
    sub = slot->currentSub;
    if (entry->flags & 0x80000000) {
        sub->state = 4;
    } else {
        sub->state = 4;
    }
    slot->status = 100;
    return 0;
}
