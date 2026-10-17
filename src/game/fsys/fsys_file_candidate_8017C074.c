/**
 * @file fsys_file_candidate_8017C074.c
 * @brief FSYS LZSS job start (0x8017C074 - 0x8017C1D8).
 *
 * Moves the compressed image of an entry aside into the job
 * (job->compressed, freed by fsysLZSSJobFinish when the decode finishes) and
 * allocates the entry's output buffer: the group's allocator, else GSres.
 * It is the asynchronous form of the first half of fsysLoadEntrySync's LZSS path
 * (`compressed = sub->buffer; sub->buffer = NULL;` then
 * `sub->buffer = fsysAllocDecodeBuffer(...)`), and shares its helpers.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas.
 *
 * Retail keeps `job` in a register (mr r18,r6) and homes
 * fsysAllocDecodeBuffer's `alloc` on the stack (0x30). Level-0 code gives
 * registers r31..r17 (15 at most) to the variables with the most
 * references, ties going to the earlier declaration (controlled compiles,
 * GC/1.3 -opt level=0), so retail's `job` has one reference more than its
 * single store. Without the dead `job = NULL;` at the end the function is
 * 90.8% (`job` homed at 0xc, `alloc` kept).
 *
 * RULE-EXCEPTION(title-path), user directive 2026-09-28: that dead store
 * is applied although nothing in the target shows such a statement (no
 * free precedes it, unlike fsysLoadEntrySync's free-then-clear); a clean fix
 * needs the real statement that gives `job` its extra reference.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"
#include "game/gs_range_8017FA5C_shared.h"

extern void* GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 id, u32 param);

/* GSres allocation of an entry buffer, 32-byte aligned (as in fsysLoadEntrySync). */
static inline void* gsResAlloc(u32 size, u32 group, u32 id)
{
    void* p;

    p = GSresAllocResourceAlign(size, 0x20, group, id, 0);
    return p;
}

/* Output buffer of an LZSS entry: the group's allocator, else GSres (as in
 * fsysLoadEntrySync). */
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

/* Address: 0x8017C074 | size: 0x164 */
void fsysLZSSJobSetup(FSYSSlot* slot, FSYSSubEntry* sub, u32 index, GsRangePoolElem* job)
{
    FSYSFileEntry* entry;

    entry = fsysGetEntry(slot, index);
    job->compressed = sub->buffer;
    sub->buffer = NULL;
    sub->buffer = fsysAllocDecodeBuffer(slot, entry, sub);
    /* RULE-EXCEPTION(title-path): dead store whose only effect is register ranking (gives `job` the extra reference that keeps it in r18) - see docs/RULE_EXCEPTIONS.md */
    job = NULL;
}
