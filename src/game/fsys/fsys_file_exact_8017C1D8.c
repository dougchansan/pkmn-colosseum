/**
 * @file fsys_file_exact_8017C1D8.c
 * @brief FSYS LZSS job completion (0x8017C1D8 - 0x8017C394).
 *
 * Called by the GSgapp job pool (fn_8018094C) once the background LZSS
 * decode of an entry has finished: free the compressed image the job kept
 * (fsysLZSSJobSetup moved it there), then either mark the entry failed (state 7)
 * when no output buffer was allocated, or mark it done (state 6), flush the
 * resource and report it to the entry's group callback.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): exact with that unit-wide flag
 * and no local pragmas.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"
#include "game/gs_range_8017FA5C_shared.h"

extern u16 GSmemGetHandleByPtr(void* ptr);
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern void* GSresGetResource(u32 fileHandle, u32 nameHash);

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

/*
 * Report a loaded entry to its group. Retail also stores two locals that
 * nothing reads: a word zeroed on entry (stw r4,0x1c(r1)) and the entry's
 * compressed size (stw r3,0x18(r1)); level-0 code keeps both stores.
 */
static inline void fsysEntryDone(FSYSSlot* slot, FSYSFileEntry* entry)
{
    FSYSGroup* group;
    void* resource;
    void (*done)(u32 fileHandle, u32 nameHash, u32 size);
    u32 size;
    u32 unused;

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

/* Address: 0x8017C1D8 | size: 0x1BC */
void fsysLZSSJobFinish(FSYSSlot* slot, FSYSSubEntry* sub, u32 index, GsRangePoolElem* job)
{
    FSYSFileEntry* entry;

    entry = fsysGetEntry(slot, index);
    fsysFree(job->compressed);
    job->compressed = 0;
    slot->padding100 = 0;
    if (!sub->buffer) {
        sub->state = 7;
        return;
    }
    sub->state = 6;
    fsysEntryDone(slot, entry);
}
