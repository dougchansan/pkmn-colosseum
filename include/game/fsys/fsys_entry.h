#ifndef GAME_FSYS_FSYS_ENTRY_H
#define GAME_FSYS_FSYS_ENTRY_H

#include "game/fsys/fsys.h"

/*
 * fsysGetEntry -- the archive entry index -> FSYSFileEntry lookup the fsys
 * code expands inline (fsysFindLoadedArchive, fsysLZSSJobSetup, fsysLZSSJobFinish, fsysStatus304,
 * fsysSlotCheckEntries, ...): the archive's table-offset pair at +0x18 gives the
 * per-entry offset list, and entry `index` lives at base + offsets[index].
 *
 * Every expansion in the level-0 fsys code saves one non-volatile register
 * that no instruction uses: the typed header view `archive` of the byte
 * pointer `base` is a plain copy, which the peephole pass folds into its
 * uses after register allocation. Writing the lookup through `base` alone
 * (or through `archive` alone) leaves the other registers one slot off.
 */
static inline FSYSFileEntry* fsysGetEntry(FSYSSlot* slot, u32 index)
{
    u8* base;
    FSYSArchiveHeader* archive;
    u32* tables;
    u32* offsets;

    base = (u8*)slot->archiveData;
    if (base) {
        archive = (FSYSArchiveHeader*)base;
        tables = (u32*)(base + archive->tableOffset);
        offsets = (u32*)(base + tables[0]);
        return (FSYSFileEntry*)(base + offsets[index]);
    }
    return NULL;
}

/*
 * Per-group load callbacks (lbl_8036C2A0, lbl_80478C48 entries): alloc
 * supplies the destination buffer for an entry of the group, done is told
 * when the entry has been loaded.
 */
typedef struct FSYSGroup {
    u32 field_00;
    u32 groupID;
    void* (*alloc)(u32 fileHandle, u32 nameHash, u32 size);
    void (*done)(u32 fileHandle, u32 nameHash, u32 size);
} FSYSGroup;

extern FSYSGroup lbl_8036C2A0[];
extern s32 lbl_80478C48;

/*
 * fsysFindGroup -- expanded inline in fsysLoadEntry, fsysLoadEntryUncompressed,
 * fsysLZSSJobSetup and fsysLZSSJobFinish. At level 0 the count and the groupID
 * parameter are single-use values homed on the stack.
 */
static inline FSYSGroup* fsysFindGroup(u32 groupID)
{
    FSYSGroup* group;
    s32 i;
    s32 count;

    group = lbl_8036C2A0;
    count = lbl_80478C48;
    for (i = 0; i < count; i++) {
        if (group->groupID == groupID) {
            return group;
        }
        group++;
    }
    return NULL;
}

#endif /* GAME_FSYS_FSYS_ENTRY_H */
