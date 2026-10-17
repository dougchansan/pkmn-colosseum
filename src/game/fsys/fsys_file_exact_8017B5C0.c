/**
 * @file fsys_file_exact_8017B5C0.c
 * @brief Start an asynchronous read of one compressed FSYS entry.
 *
 * The compressed path expands the same temporary allocation helper used by
 * fsysLoadEntrySync. The uncompressed path continues in fsysLoadEntryUncompressed.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern u16 fn_800E2B00(u32 size, u32 alignment);
extern void* GSmemLock(u16 handle);
extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern u32 fn_8017F728(u32 fileHandle, u32 groupID, u32 nameHash);
extern void fn_80180584(void* dst, void* src, u32 size, void (*callback)(void), void* userData);
extern void fn_8017A814(void);
extern void fsysLoadEntryUncompressed();

static inline void* fsysAllocTemp(u32 size)
{
    u32 alignedSize;
    u16 handle;

    alignedSize = (size + 0x1F) & ~0x1F;
    handle = fn_800E2B00(alignedSize, 0x20);
    if (handle != 0) {
        return GSmemLock(handle);
    }
    return NULL;
}

static inline void fsysReadCompressed(FSYSSlot* slot, FSYSFileEntry* entry)
{
    FSYSSubEntry* sub;
    /* RULE-EXCEPTION(title-path): dead stack store with no behavioral role; see docs/RULE_EXCEPTIONS.md. */
    void* zero = NULL;
    u32 size;
    u32 offset;
    /* RULE-EXCEPTION(title-path): dead initializer for register order; see docs/RULE_EXCEPTIONS.md. */
    u32 cached = 0;

    sub = slot->currentSub;
    sub->state = 5;
    slot->status = 0x65;
    size = (entry->decompressedSize + 0x1F) & ~0x1Fu;
    sub->buffer = fsysAllocTemp(size);
    cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
    offset = fn_8017F728(slot->fileHandle, entry->groupID, entry->nameHash);
    fn_80180584(sub->buffer, (void*)cached, offset, fn_8017A814, slot);
}

/* Address: 0x8017B5C0 | size: 0xF8 */
void fsysLoadEntryCompressed(FSYSSlot* slot, FSYSFileEntry* entry, u32 index)
{
    if (entry->flags & 0x80000000) {
        fsysReadCompressed(slot, entry);
    } else {
        fsysLoadEntryUncompressed(slot, entry, index);
    }
}
