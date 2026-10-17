/**
 * @file fsys_file_r52_8017DAB8_suffix.c
 * @brief FSYS slot release once every entry has settled
 *        (0x8017DAB8 - 0x8017DB74).
 *
 * fsysSlotStatus2000 (status 2000) calls this for every load mode but 3: walk the
 * archive's entries (recording the one being looked at in
 * slot->entryIndex) and hand the slot to fn_8017D960 only when no entry's
 * trailer is still in flight, i.e. every state is 4, 6 or 7.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py). The first state test reads
 * 0x28(entry) directly: the peephole folds "addi sub,entry,0x28;
 * lwz 0x0(sub)" for the first use only.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern void fn_8017D960(FSYSSlot* slot);

/* Address: 0x8017DAB8 | size: 0xBC */
void fsysSlotCheckEntries(FSYSSlot* slot)
{
    FSYSSubEntry* sub;
    u32 i;
    FSYSFileEntry* entry;
    s32 busy;

    busy = 0;
    for (i = 0; i < slot->numEntries; i++) {
        entry = fsysGetEntry(slot, i);
        slot->entryIndex = i;
        sub = (FSYSSubEntry*)entry->subEntry;
        if ((s32)sub->state != 4 && (s32)sub->state != 6 && (s32)sub->state != 7) {
            busy = 1;
            break;
        }
    }
    if (busy == 0) {
        fn_8017D960(slot);
    }
}
