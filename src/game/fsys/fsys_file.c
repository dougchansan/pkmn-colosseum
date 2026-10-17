/**
 * @file fsys_file.c
 * @brief FSYS loaded-entry query and entry load request
 *        (0x8017B07C - 0x8017B1AC).
 *
 * Both functions resolve the archive slot of a file handle through
 * fsysFindSlotByHandle (mode 3). They reference no data.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSSlot* fsysFindSlotByHandle(u32 fileHandle, s32 mode);
extern void fsysStartLoadMode3(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);

/* Address: 0x8017B07C | size: 0xC0
 * Whether the archive loaded for fileHandle holds an entry named nameHash
 * whose runtime trailer has reached state 6 (loaded).
 *
 * The entry lookup is fsysGetEntry, expanded inline (its return value is
 * built in r25 and copied to entry's r28, as in the other expansions). The
 * state test is a switch: at level 0 an if-test branches straight past the
 * store (bne), while retail branches to it (beq) and jumps over it
 * otherwise, which is the level-0 code for a one-case switch. */
s32 fsysIsEntryLoaded(u32 fileHandle, u32 nameHash)
{
    FSYSSlot* slot;
    u32 i;
    FSYSFileEntry* entry;
    s32 found;
    FSYSSubEntry* sub;

    slot = fsysFindSlotByHandle(fileHandle, 3);
    found = 0;
    if (slot) {
        for (i = 0; i < slot->numEntries; i++) {
            entry = fsysGetEntry(slot, i);
            if (entry->nameHash == nameHash) {
                sub = (FSYSSubEntry*)entry->subEntry;
                switch (sub->state) {
                case 6:
                    found = 1;
                    break;
                }
            }
        }
        return found;
    }
    return found;
}

/* Address: 0x8017B13C | size: 0x70
 * Request entry requestID of the archive loaded for fileHandle. */
s32 fsysRequestEntry(u32 fileHandle, u32 requestID)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 3);
    if (slot) {
        slot->requestID = requestID;
        fsysStartLoadMode3(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}
