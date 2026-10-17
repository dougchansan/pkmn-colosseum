/**
 * @file fsys_file_r48_8017BD34_o2.c
 * @brief FSYS plain-entry read start (0x8017BD34 - 0x8017BFE8).
 *
 * Starts reading an uncompressed entry of an archive through the ARAM
 * cache: mark the entry's trailer reading (state 5) and the slot status
 * 101, allocate its destination (fsysAllocEntry: the group's allocator,
 * else GSres; entries of external archives refresh their size from the
 * disc file first) and queue the cached image's transfer (fn_80180584)
 * with fn_8017A95C as completion callback. fsysLoadEntryCompressed hands compressed
 * entries to the LZSS path and the rest to this function.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas. fsysAllocEntry, fsysUpdateTotalSize and gsResAlloc are
 * fsysLoadEntrySync's helpers (exact there); `size` is stored and never read, as
 * retail's 0x60(r1) store shows.
 *
 * Exact source: fsysAllocEntry's inner `sub` is homed on the stack (0x50),
 * leaving r19 for the cached ARAM address. The post-transfer clear of
 * `cached` is dead, but it gives MWCC the allocation priority retail has.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern void* GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 id, u32 param);
extern u8 fn_80167EF8(const char* path);
extern u32 GSdvdOpen(const char* path);
extern u32 GSdvdGetLength(u32 fileInfo);
extern s32 GSdvdClose(u32 fileInfo);
extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern u32 fn_8017F728(u32 fileHandle, u32 groupID, u32 nameHash);
extern void fn_80180584(void* dst, u32 src, u32 size, void (*callback)(void), void* userData);
extern void fn_8017A95C(void);

/* GSres allocation of an entry buffer, 32-byte aligned (as in fsysLoadEntrySync). */
static inline void* gsResAlloc(u32 size, u32 group, u32 id)
{
    void* p;

    p = GSresAllocResourceAlign(size, 0x20, group, id, 0);
    return p;
}

/* Recompute the archive's total size after an entry changed size (as in
 * fsysLoadEntrySync). */
static inline void fsysUpdateTotalSize(FSYSSlot* slot)
{
    u32 total;
    u32 k;
    FSYSFileEntry* e;
    u8* info;

    total = 0;
    for (k = 0; k < slot->numEntries; k++) {
        e = fsysGetEntry(slot, k);
        total += e->decompressedSize;
    }
    info = (u8*)slot->archiveData + slot->field_18;
    total += *(u32*)(info + 8);
    slot->totalDecompSize = total;
}

/*
 * Destination buffer of a plain entry (as in fsysLoadEntrySync). Entries of
 * archives flagged external (flags bit 0) take their size from the file on
 * disc first.
 */
static inline void* fsysAllocEntry(FSYSSlot* slot, FSYSFileEntry* entry)
{
    u32 size;
    void* buffer;
    FSYSGroup* group;
    void* (*alloc)(u32 fileHandle, u32 nameHash, u32 size);
    FSYSArchiveHeader* archive;

    buffer = NULL;
    group = fsysFindGroup(entry->groupID);
    archive = slot->archiveData;
    size = entry->decompressedSize;
    if (archive->flags & 1) {
        FSYSSubEntry* sub;
        char* name;
        u32 fileSize;

        archive = slot->archiveData;
        name = (char*)slot->archiveData + entry->dataOffset;
        slot->tocBuffer = NULL;
        if (fn_80167EF8(name)) {
            slot->tocBuffer = (void*)GSdvdOpen(name);
        }
        sub = (FSYSSubEntry*)entry->subEntry;
        if (slot->tocBuffer) {
            fileSize = GSdvdGetLength((u32)slot->tocBuffer);
            entry->decompressedSize = fileSize;
            size = entry->decompressedSize;
            fsysUpdateTotalSize(slot);
            GSdvdClose((u32)slot->tocBuffer);
            slot->tocBuffer = NULL;
            sub->ready = 1;
        }
    }
    if (group->alloc) {
        alloc = group->alloc;
        buffer = alloc(slot->fileHandle, entry->nameHash, size);
    } else {
        buffer = gsResAlloc((size + 0x1F) & ~0x1F, slot->fileHandle, entry->nameHash);
    }
    return buffer;
}

/* Address: 0x8017BD34 | size: 0x2B4 */
void fsysLoadEntryUncompressed(FSYSSlot* slot, FSYSFileEntry* entry, u32 index)
{
    FSYSSubEntry* sub;
    u32 cached;
    void* buffer;
    u32 size;
    u32 offset;

    buffer = NULL;
    sub = slot->currentSub;
    sub->state = 5;
    slot->status = 101;
    buffer = fsysAllocEntry(slot, entry);
    size = (entry->decompressedSize + 0x1F) & ~0x1F;
    cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
    offset = fn_8017F728(slot->fileHandle, entry->groupID, entry->nameHash);
    fn_80180584(buffer, cached, offset, fn_8017A95C, slot);
    /* RULE-EXCEPTION(title-path): dead store for register allocation; see docs/RULE_EXCEPTIONS.md. */
    cached = 0;
}
