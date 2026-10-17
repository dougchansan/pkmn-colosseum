/**
 * @file fsys_file_r48_8017B4BC_prefix.c
 * @brief FSYS entry name-hash query and entry flag decode
 *        (0x8017B4BC - 0x8017B5C0).
 *
 * fsysFindLoadedArchive scans the slot array for the loaded archive of fileHandle and
 * returns the name hash of its entry `index` (0 when no slot holds it).
 * fsysEntryGetType extracts bits 9-14 of an entry word.
 *
 * Level-0 code like the rest of fsys (see configure.py; this object used to
 * be scored at -O1 from fsys_file_candidates.c).
 *
 * Retail computes the archive's name-table pointer for every slot it visits
 * and stores it to the stack without reading it back, and stores a second
 * copy of the archive pointer before the entry lookup the same way. Written
 * in place, the unused name-table walk is removed as dead code; only an
 * inline expansion keeps it, with the result built in r25 and stored once
 * after the null-check join (the helper's return temp). Its typed header
 * view saves the same unused register (r23) that fsysGetEntry's does
 * (r19 here; see fsys_entry.h). The 13 register locals use r19-r31, so the
 * name hash is homed on the stack.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSManager lbl_80453FEC;
extern FSYSSlot* lbl_8047B1B4; /* slot array */

/* Name table of the archive loaded in slot: the second offset of the
 * archive's table-offset pair at +0x18. */
static inline char* fsysGetNameTable(FSYSSlot* slot)
{
    u8* base;
    FSYSArchiveHeader* archive;
    u32* tables;

    base = (u8*)slot->archiveData;
    if (base) {
        archive = (FSYSArchiveHeader*)base;
        tables = (u32*)(base + archive->tableOffset);
        return (char*)base + tables[1];
    }
    return NULL;
}

/* Address: 0x8017B4BC | size: 0xE8 */
u32 fsysFindLoadedArchive(u32 fileHandle, u32 index)
{
    u32 i;
    FSYSSlot* slot;
    u8 found;
    FSYSFileEntry* entry;
    u32 hash;
    FSYSArchiveHeader* archive;
    char* names;

    slot = lbl_8047B1B4;
    for (i = 0, slot = lbl_8047B1B4; i < lbl_80453FEC.maxSlots; slot++, i++) {
        names = fsysGetNameTable(slot);
        if (slot->status != FSYS_STATUS_FREE && slot->fileHandle == fileHandle &&
            slot->padding05C != 0) {
            /* RULE-EXCEPTION(user-approved): dead stores (archive, names) kept for retail's stack stores — see docs/RULE_EXCEPTIONS.md */
            archive = slot->archiveData;
            entry = fsysGetEntry(slot, index);
            hash = entry->nameHash;
            found = 1;
            goto done;
        }
    }
    found = 0;
done:
    if (found) {
        return hash;
    }
    return 0;
}

/* Address: 0x8017B5A4 | size: 0x1C */
u32 fsysEntryGetType(u32 val)
{
    u32 result;

    result = (val >> 9) & 0x3F;
    return result;
}
