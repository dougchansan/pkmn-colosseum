/**
 * @file fsys_file_candidate_8017E30C_o2.c
 * @brief fsysLoadEntrySync: load one entry of an already-loaded FSYS archive
 *        synchronously (0x8017E30C - 0x8017EB6C).
 *
 * Finds the entry whose name hash is slot->requestID, fetches its image from
 * the ARAM cache (fn_8017F794 / fn_80180320) and either decodes it (LZSS
 * entries: temporary buffer, group or GSres output buffer, decode, free the
 * temporary) or reads it straight into its group or GSres buffer (external
 * entries first refresh their size from the file on disc), then reports the
 * entry to its group. Returns 0 when the entry or its cached image is
 * missing, 1 otherwise; a failed allocation marks the slot status 100 and
 * the entry state 7.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code; exact (536/536 instructions and relocations) on the
 * unit-wide `-opt level=0` flags with no pragmas.
 *
 * It is the synchronous form of the job pipeline in fsysLZSSJobSetup (move the
 * compressed image aside, allocate the decode buffer), fsysLZSSDecodeJob (LZSS
 * decode into sub->buffer) and fsysLZSSJobFinish (free the image, report the
 * entry), and shares their inline helpers:
 *   fsysGetEntry, fsysFindGroup      fsys_entry.h
 *   fsysEntryDone, fsysFree          as in fsys_file_exact_8017C1D8.c
 *   fsysAllocDecodeBuffer            also expanded in fsysLZSSJobSetup
 *   gsResAlloc                       also in fsysLoadEntryUncompressed, fsysLoadEntry, fsysLZSSJobSetup
 *   fsysAllocEntry, fsysUpdateTotalSize  also in fsysLoadEntryUncompressed, fsysLoadEntry
 *   fsysLZSSInit                     the same sequence as fsysLZSSDecodeJob
 * fsysDecodeLZSS's destination is homed on the stack (0x88) because its
 * argument is fsysDecodeEntry's local: the inline-parameter copy level-0
 * code only makes for another inline's local.
 *
 * `compressed = NULL;` after the temporary image is freed is source, not
 * shaping. The store leaves no instruction (a dead write to a register
 * local); its visible effect is the extra reference that ranks `compressed`
 * (r19) above the decode-buffer and done-callback groups and pushes the
 * size-sum counter onto the stack, as retail has. Evidence that the fsys
 * code clears a pointer right after freeing it:
 *   - every other fsysFree expansion in the level-0 fsys unit is followed
 *     by clearing the freed pointer, where the store is visible because the
 *     pointer lives in memory: GSresReleaseById `slot->archiveData = NULL`
 *     (0x8017B288 li/stw 0x40(r31)), fsysLZSSJobFinish `job->compressed = 0`
 *     (0x8017C25C li/stw 0x38(r22));
 *   - fsysLZSSJobFinish is this path's async twin: fsysLZSSJobSetup moves the image
 *     aside into job->compressed and clears sub->buffer (this function does
 *     `compressed = sub->buffer; sub->buffer = NULL;`, both stores kept),
 *     and fsysLZSSJobFinish frees job->compressed and clears it, the statement
 *     this one mirrors with the local;
 *   - the ARAM-cache code next to fsys does the same with a global
 *     (0x80180A1C: free lbl_8047B1E0, then store 0 to it);
 *   - controlled compiles (GC/1.3, the unit's -opt level=0 flags): a
 *     `p = 0;` after a register local's last use emits nothing, so the
 *     missing instruction is what MWCC does to this statement.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

typedef struct FSYSLZSSHeader {
    u32 magic;
    u32 decompSize;
    u32 compSize;
    u32 field_0C;
} FSYSLZSSHeader;

extern FSYSLZSSHeader lbl_80453FDC; /* LZSS header of the entry being decoded */
extern u8 lbl_80452FC8[];           /* LZSS sliding window */

extern void* memcpy(void* dst, const void* src, u32 n);
extern void DCFlushRange(void* addr, u32 nBytes);
extern u16 fn_800E2B00(u32 size, u32 align);
extern void* GSmemLock(u16 handle);
extern u16 GSmemGetHandleByPtr(void* ptr);
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern void* GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 id, u32 param);
extern void* GSresGetResource(u32 fileHandle, u32 nameHash);
extern u8 fn_80167EF8(const char* path);
extern u32 GSdvdOpen(const char* path);
extern u32 GSdvdGetLength(u32 fileInfo);
extern s32 GSdvdClose(u32 fileInfo);
extern void fn_8017F2C4(void* dst, const void* src, u32 size);
extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern void fn_80180320(void* main, void* aram, u32 size);

/* Temporary (non-resource) allocation; also expanded in fsysLoadEntry. */
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

/* Same expansion as fsysFree in fsys_slot_8017B1CC.c. */
static inline void fsysFree(void* ptr)
{
    u16 handle;

    handle = GSmemGetHandleByPtr(ptr);
    if (handle != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
}

/* GSres allocation of an entry buffer, 32-byte aligned. */
static inline void* gsResAlloc(u32 size, u32 group, u32 id)
{
    void* p;

    p = GSresAllocResourceAlign(size, 0x20, group, id, 0);
    return p;
}

/* Output buffer of an LZSS entry: the group's allocator, else GSres. */
static inline void* fsysAllocDecodeBuffer(FSYSSlot* slot, FSYSFileEntry* entry, FSYSSubEntry* sub)
{
    FSYSGroup* group;
    void* buffer;
    void* (*alloc)(u32 fileHandle, u32 nameHash, u32 size);

    group = fsysFindGroup(entry->groupID);
    if (group->alloc) {
        alloc = group->alloc;
        buffer = alloc(slot->fileHandle, entry->nameHash, entry->compressedSize);
    } else {
        buffer = gsResAlloc((entry->compressedSize + 0x1F) & ~0x1F, slot->field_08, entry->nameHash);
    }
    sub->buffer = buffer;
    return buffer;
}

/* Take the LZSS header of `src` and clear the sliding window. */
static inline void fsysLZSSInit(void* src)
{
    s32 i;

    memcpy(&lbl_80453FDC, src, 0x10);
    for (i = 0; i < 0xFEE; i++) {
        lbl_80452FC8[i] = 0;
    }
}

static inline void fsysDecodeLZSS(void* dst, void* src)
{
    u8 decoded;

    decoded = 0;
    fsysLZSSInit(src);
    fn_8017F2C4(dst, src, lbl_80453FDC.decompSize);
    DCFlushRange(dst, lbl_80453FDC.decompSize);
    decoded = 1;
}

static inline void* fsysDecodeEntry(FSYSSubEntry* sub, void* src)
{
    void* out;

    out = sub->buffer;
    if (out) {
        fsysDecodeLZSS(out, src);
    }
    return out;
}

/*
 * fsysEntryDone of fsys_file_exact_8017C1D8.c. The level-0 stack slots of
 * its two expansions here (size, unused, group, done, resource from the
 * bottom up) give its declaration order; fsysLZSSJobFinish is exact with this
 * order as well.
 */
static inline void fsysEntryDone(FSYSSlot* slot, FSYSFileEntry* entry)
{
    u32 size;
    u32 unused;
    FSYSGroup* group;
    void (*done)(u32 fileHandle, u32 nameHash, u32 size);
    void* resource;

    unused = 0;
    group = fsysFindGroup(entry->groupID);
    size = entry->compressedSize;
    if (group->done) {
        resource = GSresGetResource(slot->fileHandle, entry->nameHash);
        if (resource) {
            if (entry->flags & 0x80000000) {
                DCFlushRange(resource, entry->compressedSize);
            } else {
                DCFlushRange(resource, entry->decompressedSize);
            }
        }
        done = group->done;
        if (entry->flags & 0x80000000) {
            done(slot->fileHandle, entry->nameHash, entry->compressedSize);
        } else {
            done(slot->fileHandle, entry->nameHash, entry->decompressedSize);
        }
    }
}

/* Recompute the archive's total size after an entry changed size. */
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
 * Destination buffer of a plain entry. Entries of archives flagged external
 * (flags bit 0) take their size from the file on disc first.
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

/* Address: 0x8017E30C | size: 0x860 */
s32 fsysLoadEntrySync(FSYSSlot* slot)
{
    u32 i;
    s32 found;
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;
    void* cached;

    found = 0;
    if (slot->archiveData) {
        for (i = 0; i < slot->numEntries; i++) {
            entry = fsysGetEntry(slot, i);
            if (entry->nameHash == slot->requestID) {
                sub = (FSYSSubEntry*)entry->subEntry;
                found = 1;
                break;
            }
        }
    }
    if (found) {
        cached = (void*)fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
        if (cached) {
            if (entry->flags & 0x80000000) {
                u32 size;
                void* compressed;

                size = entry->decompressedSize;
                sub->buffer = fsysAllocTemp(size);
                fn_80180320(sub->buffer, cached, size);
                DCFlushRange(sub->buffer, size);
                compressed = sub->buffer;
                sub->buffer = NULL;
                sub->buffer = fsysAllocDecodeBuffer(slot, entry, sub);
                fsysDecodeEntry(sub, compressed);
                if (!sub->buffer) {
                    slot->status = 100;
                    sub->state = 7;
                } else {
                    fsysFree(compressed);
                    compressed = NULL;
                    fsysEntryDone(slot, entry);
                }
            } else {
                sub->buffer = fsysAllocEntry(slot, entry);
                if (!sub->buffer) {
                    slot->status = 100;
                    sub->state = 7;
                } else {
                    fn_80180320(sub->buffer, cached, entry->decompressedSize);
                    DCFlushRange(sub->buffer, entry->decompressedSize);
                    fsysEntryDone(slot, entry);
                }
            }
            return 1;
        }
        return 0;
    }
    return 0;
}
