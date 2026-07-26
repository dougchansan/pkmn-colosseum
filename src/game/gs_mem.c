/**
 * @file gs_mem.c
 * @brief Genius Sonority's handle-based heap allocator.
 */
#include "dolphin/types.h"
#include "game/gs_mem.h"

extern u8 lbl_80478AF0;
extern u8 lbl_8047AB28;
extern u32 lbl_8047AB2C;
extern GSmemBlock* lbl_8047AB30;
extern GSmemEntry* lbl_8047AB34;
extern GSmemEntry* lbl_8047AB38;
extern u32 lbl_8047AB3C;
extern u32 lbl_8047AB48;
extern u32 lbl_8047AB4C;
extern u32 lbl_8047AB50;
extern u32 lbl_8047AB54;
extern u32 lbl_8047AB58;
extern u32 lbl_8047AB5C;
extern u32 lbl_8047AB60;
extern void* lbl_8047AB64;
extern void* lbl_8047AB68;
extern char lbl_80270658[];
extern char lbl_80270D78[];
extern char lbl_80270DD0[];
extern char lbl_80270DFC[];

extern void GSlogWrite(const char* format, ...);
extern void* memset(void* dest, int value, u32 length);
extern void DCFlushRange(void* address, u32 length);

static inline GSmemEntry* GSmemEntryFromHandle(u16 handle)
{
    if (handle == 0) {
        return NULL;
    }
    return lbl_8047AB34 - (handle - 1);
}

static inline u16 GSmemEntryHandle(GSmemEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return ((u32)lbl_8047AB34 - (u32)entry) / sizeof(GSmemEntry) + 1;
}

static inline BOOL GSmemGuardsAreValid(GSmemEntry* entry)
{
    u8* start = entry->data;
    u8* end = start + entry->size - 4;

    return start[0] == 0 && start[1] == 0 && start[2] == 0 &&
           start[3] == 0 && end[0] == 0 && end[1] == 0 &&
           end[2] == 0 && end[3] == 0;
}

static inline void GSmemClearGuards(GSmemEntry* entry)
{
    u8* start = entry->data;
    u8* end = start + entry->size - 4;

    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    start[3] = 0;
    end[0] = 0;
    end[1] = 0;
    end[2] = 0;
    end[3] = 0;
}

static inline u16 GSmemChecksum(GSmemEntry* entry)
{
    u8* bytes = entry->data;
    u16* halves = entry->data;
    u32 halfCount = entry->size >> 1;
    u32 checksum = 0x3D94;

    while (halfCount-- != 0) {
        checksum += *halves++;
    }
    if ((entry->size & 1) != 0) {
        bytes = (u8*)halves;
        checksum += *bytes;
    }
    return checksum;
}

static inline GSmemEntry* GSmemFindFreeEntry(void)
{
    GSmemEntry* entry;

    for (entry = lbl_8047AB34; entry >= lbl_8047AB38; entry--) {
        if (entry->handle == 0) {
            return entry;
        }
    }
    return NULL;
}

static inline GSmemEntry* GSmemGrowEntryTable(void)
{
    GSmemBlock* block = lbl_8047AB30;
    GSmemEntry* entry;
    u32 bytes;

    if (block == NULL) {
        return NULL;
    }
    while (block->next != NULL) {
        block = block->next;
    }
    if ((u8*)block + block->size != (u8*)lbl_8047AB38) {
        return NULL;
    }

    bytes = block->size;
    if (bytes > 0x4000) {
        bytes = 0x4000;
        block->size -= bytes;
    } else {
        bytes &= ~0xF;
        if (bytes == 0) {
            return NULL;
        }
        if (block->prev != NULL) {
            block->prev->next = NULL;
        } else {
            lbl_8047AB30 = NULL;
        }
    }

    entry = lbl_8047AB38 - 1;
    lbl_8047AB38 = (GSmemEntry*)((u8*)lbl_8047AB38 - bytes);
    {
        GSmemEntry* clear = entry;
        while (clear >= lbl_8047AB38) {
            clear->handle = 0;
            clear--;
        }
    }
    lbl_8047AB50++;
    return entry;
}

u16 fn_800E202C(void* ptr)
{
    GSmemEntry* entry;

    if (lbl_8047AB28 == 1) {
        ptr = (u8*)ptr - 4;
    }
    entry = lbl_8047AB34;
    while (entry >= lbl_8047AB38) {
        if (entry->handle != 0 && entry->data == ptr) {
            goto found;
        }
        entry--;
    }
    entry = NULL;
found:
    if (entry == NULL) {
        return 0;
    }
    return entry->handle;
}

s32 fn_800E209C(u16 handle)
{
    GSmemEntry* entry;
    GSmemBlock* before;
    GSmemBlock* after;
    GSmemBlock* released;
    s32 result = 0;

    if (handle == 0) {
        GSlogWrite(lbl_80270658 + 0x5A4, handle);
        lbl_8047AB3C = 1;
        return 1;
    }

    entry = GSmemEntryFromHandle(handle);
    if (entry->handle != handle) {
        GSlogWrite(lbl_80270658 + 0x5A4, handle);
        lbl_8047AB3C = 1;
        return 1;
    }
    if (entry->refCount != 0) {
        GSlogWrite(lbl_80270658 + 0x5C8, handle);
        result = 8;
    }

    if (lbl_8047AB28 == 1) {
        if (!GSmemGuardsAreValid(entry)) {
            GSlogWrite(lbl_80270658 + 0x5FC, handle);
            lbl_8047AB3C = 7;
            result = 7;
        }
        if (entry->pad != GSmemChecksum(entry)) {
            GSlogWrite(lbl_80270658 + 0x62C, handle);
            lbl_8047AB3C = 6;
        }
    }

    entry->handle = 0;
    released = entry->data;
    before = NULL;
    after = lbl_8047AB30;
    while (after != NULL && after < released) {
        before = after;
        after = after->next;
    }

    released->prev = before;
    released->next = after;
    released->size = entry->size;
    if (before != NULL) {
        before->next = released;
    } else {
        lbl_8047AB30 = released;
    }
    if (after != NULL) {
        after->prev = released;
    }

    if (released->next != NULL &&
        (u8*)released + released->size == (u8*)released->next) {
        after = released->next;
        released->size += after->size;
        released->next = after->next;
        if (released->next != NULL) {
            released->next->prev = released;
        }
    }
    if (released->prev != NULL &&
        (u8*)released->prev + released->prev->size == (u8*)released) {
        before = released->prev;
        before->size += released->size;
        before->next = released->next;
        if (released->next != NULL) {
            released->next->prev = before;
        }
    }

    lbl_8047AB54++;
    lbl_8047AB4C--;
    return result;
}

s32 fn_800E24B0(u16 handle)
{
    GSmemEntry* entry;
    s32 result = 0;

    if (handle == 0) {
        GSlogWrite(lbl_80270658 + 0x660, handle);
        lbl_8047AB3C = 1;
        return 1;
    }
    entry = GSmemEntryFromHandle(handle);
    if (entry->handle != handle) {
        GSlogWrite(lbl_80270658 + 0x660, handle);
        lbl_8047AB3C = 1;
        return 1;
    }
    if (entry->refCount == 0) {
        GSlogWrite(lbl_80270658 + 0x684, handle);
        lbl_8047AB3C = 5;
        return 5;
    }

    lbl_8047AB3C = 0;
    if (lbl_8047AB28 == 1) {
        if (!GSmemGuardsAreValid(entry)) {
            GSlogWrite(lbl_80270658 + 0x5FC, handle);
            lbl_8047AB3C = 7;
            result = 7;
            GSmemClearGuards(entry);
        }
        if (entry->refCount == 1) {
            entry->pad = GSmemChecksum(entry);
        }
    }

    entry->refCount--;
    lbl_8047AB58++;
    lbl_8047AB48--;
    return result;
}

void* fn_800E27B0(u16 handle)
{
    GSmemEntry* entry;

    if (handle == 0) {
        GSlogWrite(lbl_80270658 + 0x6D0, handle);
        lbl_8047AB3C = 1;
        return NULL;
    }
    entry = GSmemEntryFromHandle(handle);
    if (entry->handle != handle) {
        GSlogWrite(lbl_80270658 + 0x6D0, handle);
        lbl_8047AB3C = 1;
        return NULL;
    }
    if (entry->refCount == 0xFFFF) {
        GSlogWrite(lbl_80270658 + 0x6F4, handle);
        lbl_8047AB3C = 4;
        return NULL;
    }

    lbl_8047AB3C = 0;
    if (lbl_8047AB28 == 1) {
        if (!GSmemGuardsAreValid(entry)) {
            GSlogWrite(lbl_80270658 + 0x5FC, handle);
            lbl_8047AB3C = 7;
            GSmemClearGuards(entry);
        }
        if (entry->refCount == 0 && entry->pad != GSmemChecksum(entry)) {
            GSlogWrite(lbl_80270658 + 0x62C, handle);
            lbl_8047AB3C = 6;
        }
    }

    entry->refCount++;
    lbl_8047AB5C++;
    lbl_8047AB48++;
    if (lbl_8047AB28 == 1) {
        return (u8*)entry->data + 4;
    }
    return entry->data;
}

s32 fn_800E2AF8(void)
{
    return 1;
}

static u16 fn_800E2DB0(void* address, u32 size)
{
    GSmemBlock* block;
    GSmemBlock* suffix;
    GSmemEntry* entry;
    GSmemEntry* previousEntry;
    u8* allocation = address;
    u32 allocationSize;
    u32 prefixSize;
    u32 suffixSize;

    if (size == 0) {
        return 0;
    }

    allocationSize = (((size + 0x1F) & ~0x1F) + 3) & ~3;
    if (lbl_8047AB28 != 0) {
        allocation -= 4;
        allocationSize += 8;
    }
    if (allocationSize < sizeof(GSmemBlock)) {
        allocationSize = sizeof(GSmemBlock);
    }

    block = lbl_8047AB30;
    while (block != NULL) {
        if (allocation >= (u8*)block &&
            allocation <= (u8*)block + block->size) {
            break;
        }
        block = block->next;
    }
    if (block == NULL ||
        allocation + allocationSize > (u8*)block + block->size) {
        lbl_8047AB3C = 2;
        return 0;
    }

    entry = GSmemFindFreeEntry();
    if (entry == NULL) {
        entry = GSmemGrowEntryTable();
    }
    if (entry == NULL) {
        lbl_8047AB3C = 3;
        return 0;
    }

    prefixSize = allocation - (u8*)block;
    suffixSize = block->size - prefixSize - allocationSize;
    suffix = (GSmemBlock*)(allocation + allocationSize);
    if (suffixSize < sizeof(GSmemBlock)) {
        suffix = NULL;
    }

    if (prefixSize < sizeof(GSmemBlock)) {
        previousEntry = NULL;
        {
            GSmemEntry* scan;
            for (scan = lbl_8047AB34; scan >= lbl_8047AB38; scan--) {
                if (scan->handle != 0 && scan->data < block &&
                    (previousEntry == NULL ||
                     (u32)block - (u32)scan->data <
                         (u32)block - (u32)previousEntry->data)) {
                    previousEntry = scan;
                }
            }
        }

        if (previousEntry == NULL) {
            if (prefixSize != 0) {
                GSlogWrite(lbl_80270D78, prefixSize, allocation);
            }
        } else if (prefixSize != 0) {
            previousEntry->size += prefixSize;
            if (lbl_8047AB28 != 0) {
                GSmemClearGuards(previousEntry);
                if (previousEntry->refCount == 0) {
                    previousEntry->pad = GSmemChecksum(previousEntry);
                }
            }
        }

        if (block->prev != NULL) {
            block->prev->next = suffix != NULL ? suffix : block->next;
        } else {
            lbl_8047AB30 = suffix != NULL ? suffix : block->next;
        }
        if (suffix != NULL) {
            suffix->prev = block->prev;
            suffix->next = block->next;
            suffix->size = suffixSize;
        }
        if (block->next != NULL) {
            block->next->prev = suffix != NULL ? suffix : block->prev;
        }
        block = NULL;
    } else {
        block->size = prefixSize;
        if (suffix != NULL) {
            suffix->prev = block;
            suffix->next = block->next;
            suffix->size = suffixSize;
            if (block->next != NULL) {
                block->next->prev = suffix;
            }
            block->next = suffix;
        } else {
            allocationSize += suffixSize;
        }
    }

    entry->handle = GSmemEntryHandle(entry);
    entry->refCount = 0;
    entry->data = allocation;
    entry->size = allocationSize;
    entry->align = 0xFFFF;
    entry->pad = 0;

    if (lbl_80478AF0 == 1) {
        memset(entry->data, 0, entry->size);
        DCFlushRange(entry->data, entry->size);
    }
    if (lbl_8047AB28 != 0) {
        GSmemClearGuards(entry);
        entry->pad = GSmemChecksum(entry);
    }

    lbl_8047AB3C = 0;
    lbl_8047AB60++;
    lbl_8047AB4C++;
    return entry->handle;
}

u16 fn_800E2B00(u32 size, u32 alignment)
{
    GSmemBlock* block;
    void* address;
    u32 allocationSize;
    u32 mask;
    u16 handle;

    if (size == 0) {
        return 0;
    }
    alignment = (alignment + 3) & 0xFFFC;
    if (alignment == 0 || (alignment & 0x1F) != 0) {
        return 0;
    }

    size = (size + 0x1F) & ~0x1F;
    allocationSize = (size + 3) & ~3;
    if (lbl_8047AB28 != 0) {
        allocationSize += 8;
    }
    if (allocationSize < sizeof(GSmemBlock)) {
        allocationSize = sizeof(GSmemBlock);
    }
    mask = ~(alignment - 1);
    address = NULL;

    for (block = lbl_8047AB30; block != NULL; block = block->next) {
        if (block->size >= allocationSize) {
            u8* candidate =
                (u8*)(((u32)block + block->size - allocationSize) & mask);
            if (candidate >= (u8*)block) {
                address = candidate;
            }
        }
    }
    if (address == NULL) {
        lbl_8047AB3C = 2;
        return 0;
    }

    handle = fn_800E2DB0(address, size);
    GSmemEntryFromHandle(handle)->align = alignment;
    return handle;
}

u16 fn_800E2C04(u32 size, u32 alignment)
{
    GSmemBlock* block;
    GSmemBlock* selected;
    u8* address;
    u32 allocationSize;
    u32 mask;
    u16 handle;

    if (size == 0) {
        return 0;
    }
    alignment = (alignment + 3) & 0xFFFC;
    if (alignment == 0 || (alignment & 0x1F) != 0) {
        return 0;
    }

    size = (size + 0x1F) & ~0x1F;
    allocationSize = (size + 3) & ~3;
    if (lbl_8047AB28 != 0) {
        allocationSize += 8;
    }
    allocationSize += alignment - 1;
    if (allocationSize < sizeof(GSmemBlock)) {
        allocationSize = sizeof(GSmemBlock);
    }
    mask = ~(alignment - 1);

    selected = lbl_8047AB30;
    while (selected != NULL) {
        address = (u8*)(((u32)selected + alignment - 1) & mask);
        if (address + allocationSize <
            (u8*)selected + selected->size) {
            break;
        }
        selected = selected->next;
    }

    if (lbl_8047AB2C == GSMEM_FIT_BEST ||
        lbl_8047AB2C == GSMEM_FIT_WORST) {
        for (block = selected; block != NULL; block = block->next) {
            u8* candidate =
                (u8*)(((u32)block + alignment - 1) & mask);
            if (candidate + allocationSize <
                    (u8*)block + block->size &&
                ((lbl_8047AB2C == GSMEM_FIT_BEST &&
                  block->size < selected->size) ||
                 (lbl_8047AB2C == GSMEM_FIT_WORST &&
                  block->size > selected->size))) {
                selected = block;
                address = candidate;
            }
        }
    }

    if (selected == NULL) {
        lbl_8047AB3C = 2;
        return 0;
    }
    handle = fn_800E2DB0(address, size);
    GSmemEntryFromHandle(handle)->align = alignment;
    return handle;
}

void fn_800E3560(u32 value)
{
    lbl_8047AB2C = value;
}

u16 _toolentryAlloc__FUl(u32 size)
{
    size = (size + 0x1F) & ~0x1F;
    return fn_800E2C04(size, 0x20);
}

void GSmemInit(u32 heapId, void* start, void* end)
{
    GSmemBlock* block;
    GSmemEntry* entry;
    void* alignedEnd = (void*)((u32)end & ~0x1F);
    void* alignedStart = (void*)(((u32)start + 0x1F) & ~0x1F);

    lbl_8047AB28 = heapId;
    lbl_8047AB68 = alignedStart;
    lbl_8047AB64 = alignedEnd;
    lbl_8047AB60 = 0;
    lbl_8047AB5C = 0;
    lbl_8047AB58 = 0;
    lbl_8047AB54 = 0;
    lbl_8047AB50 = 0;
    lbl_8047AB4C = 0;
    lbl_8047AB48 = 0;

    entry = (GSmemEntry*)(((u32)end & ~0x1F) - sizeof(GSmemEntry));
    lbl_8047AB34 = entry;
    lbl_8047AB38 = entry;
    entry->handle = 0;

    block = lbl_8047AB68;
    block->prev = NULL;
    block->next = NULL;
    block->size = (u8*)lbl_8047AB38 - (u8*)lbl_8047AB68;
    lbl_8047AB30 = block;
    GSlogWrite(lbl_80270DFC, lbl_8047AB68, lbl_8047AB64);
}
