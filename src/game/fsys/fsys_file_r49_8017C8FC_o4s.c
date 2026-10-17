/**
 * @file fsys_file_r49_8017C8FC_o4s.c
 * @brief FSYS status-100 entry dispatch (0x8017C8FC - 0x8017CE7C).
 *
 * Picks the next archive entry to load and starts it: in load mode 3 the
 * entry whose name hash the slot requested, otherwise the first entry whose
 * runtime trailer is idle (state 0) or reloadable (state 4). Plain loads go
 * through fsysLoadEntry, cached reloads through fsysLoadEntryCompressed, and mode-2/7
 * scene reads through fsysStartSceneRead; with nothing started the slot
 * moves to status 2000.
 *
 * Level-0 code like the rest of fsys (see configure.py; the file name
 * keeps its old -O4,s label). fsysStartSceneRead is expanded twice in
 * retail (the compressed and plain arms each have their own stack homes,
 * 0x20..0x34 and 0x8..0x1C); the `(s32)` state tests are the signed
 * compares fsysSlotCheckEntries also uses.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern void fsysLoadEntry(FSYSSlot* slot, FSYSFileEntry* entry, u32 index);
extern void fsysLoadEntryCompressed(FSYSSlot* slot, FSYSFileEntry* entry, u32 index);
extern void fsysInitTOC(FSYSSlot* slot, u32 offset, u32 length, void* callbackA,
                        u32 callbackB, u32 callbackC, const char* externalPath,
                        FSYSFileEntry* entry);

/* Start a scene read of a mode-2/7 entry: from the ARAM cache when it is
 * there (state 4, status 100), else from the disc (state 3, status 301). */
static inline void fsysStartSceneRead(FSYSSlot* slot, FSYSFileEntry* entry, u32 index)
{
    u32* offsets;
    FSYSArchiveHeader* archive;
    u32* tables;
    /* RULE-EXCEPTION(title-path): unused local kept for retail's dead store (0x2C/0x14) — see docs/RULE_EXCEPTIONS.md */
    u32 size;
    char* name;
    u32 cached;
    FSYSSubEntry* sub;
    FSYSFileEntry* current;

    size = (entry->decompressedSize + 0x1F) & ~0x1F;
    archive = slot->archiveData;
    tables = (u32*)((u8*)slot->archiveData + archive->tableOffset);
    sub = slot->currentSub;
    offsets = (u32*)((u8*)slot->archiveData + tables[0]);
    current = (FSYSFileEntry*)((u8*)slot->archiveData + offsets[index]);
    cached = fn_8017F794(slot->fileHandle, current->groupID, current->nameHash);
    if (cached) {
        sub->state = 4;
        slot->status = 100;
    } else {
        sub->state = 3;
        slot->status = 0x12D;
        name = (char*)slot->archiveData + current->dataOffset;
        fsysInitTOC(slot, current->padding04, entry->decompressedSize, NULL,
                    (u32)slot, 0, name, current);
    }
}

/* Address: 0x8017C8FC | size: 0x580 */
s32 fsysStatus100(FSYSSlot* slot)
{
    u32 cached;
    u32 i;
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;
    u32 requestCached;
    u32* tables;
    u32* offsets;

    slot->currentSub = NULL;
    if (slot->loadMode == 3) {
        for (i = 0; i < slot->numEntries; i++) {
            entry = fsysGetEntry(slot, i);
            if (entry->nameHash == slot->requestID) {
                slot->entryIndex = i;
                sub = (FSYSSubEntry*)entry->subEntry;
                requestCached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
                if ((s32)sub->state == 0) {
                    if (!requestCached) {
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                    } else {
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                    }
                } else if ((s32)sub->state == 4 && slot->reloadFlag == 1) {
                    if (!requestCached) {
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                    } else {
                        slot->currentSub = sub;
                        fsysLoadEntryCompressed(slot, entry, i);
                    }
                } else if ((s32)sub->state == 6 && slot->reloadFlag == 1) {
                    if (!requestCached) {
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                    } else {
                        slot->currentSub = sub;
                        fsysLoadEntryCompressed(slot, entry, i);
                    }
                }
                break;
            }
        }
    } else {
        for (i = 0; i < slot->numEntries; i++) {
            entry = fsysGetEntry(slot, i);
            slot->entryIndex = i;
            sub = (FSYSSubEntry*)entry->subEntry;
            cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
            if ((s32)sub->state == 0) {
                tables = (u32*)((u8*)slot->archiveData + slot->field_18);
                offsets = (u32*)((u8*)slot->archiveData + tables[0]);
                entry = (FSYSFileEntry*)((u8*)slot->archiveData + offsets[i]);
                if (entry->decompressedSize == 0) {
                    switch (slot->loadMode) {
                    case 0:
                        sub->state = 6;
                        break;
                    case 2:
                    case 7:
                        sub->state = 6;
                        break;
                    }
                } else {
                    switch (slot->loadMode) {
                    case 0:
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                        break;
                    case 2:
                    case 7:
                        slot->currentSub = sub;
                        if (entry->flags & 0x80000000) {
                            fsysStartSceneRead(slot, entry, i);
                        } else {
                            fsysStartSceneRead(slot, entry, i);
                        }
                        break;
                    }
                    break;
                }
            } else if ((s32)sub->state == 4 && slot->reloadFlag == 1) {
                u32* tables2;
                u32* offsets2;

                tables2 = (u32*)((u8*)slot->archiveData + slot->field_18);
                offsets2 = (u32*)((u8*)slot->archiveData + tables2[0]);
                entry = (FSYSFileEntry*)((u8*)slot->archiveData + offsets2[i]);
                switch (slot->loadMode) {
                case 0:
                    if (!cached) {
                        slot->currentSub = sub;
                        fsysLoadEntry(slot, entry, i);
                    } else {
                        slot->currentSub = sub;
                        fsysLoadEntryCompressed(slot, entry, i);
                    }
                    break;
                case 2:
                case 7:
                    break;
                }
                break;
            }
        }
    }
    if (!slot->currentSub) {
        slot->status = 2000;
        return 1;
    }
    return 0;
}
