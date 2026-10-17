/**
 * @file fsys_file_candidate_8017CED8.c
 * @brief FSYS archive TOC stage done (0x8017CED8 - 0x8017D3A0).
 *
 * Once an archive's TOC is in memory: cache it in ARAM unless it is cached
 * already (making room first), make room in the ARAM cache for the whole
 * archive, move the archive's handle to the most recently used end of the
 * handle table (fsysSlotRefreshHandle), reset every entry's runtime trailer, and set
 * the slot to status 100.
 *
 * The ARAM cache eviction helpers are those of
 * fsys_file_candidate_8017DB74_gc20.c (expanded there once and here twice).
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas. The entry loop counts with an s32 against a saved count,
 * as retail's cmpw and 0x64(r1) show.
 *
 * Exact (2026-09-29) under title-path exceptions. Stack homes of spilled
 * inline variables follow the frontend's temporary numbering (a higher
 * number takes a lower offset; first-level expansions are numbered before
 * the helpers they call). fsysCacheOldestHandle needs a statement-form body
 * for retail's numbering, and the trailer-reset loop is a first-level
 * helper whose fsysGetEntry expansion is numbered last. The full-archive
 * eviction re-reads totalDecompSize at both tests.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSFileHandle* lbl_8047B1B8; /* handle table, LRU order */
extern s32 lbl_8047B1BC;             /* handle table fill count */

extern void DCFlushRange(void* addr, u32 nBytes);
extern u32 fn_8017F794(u32 fileHandle, u32 key1, u32 key2);
extern void fn_8017F800(s32 handleID);
extern u32 fn_8017FA5C(void);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 key1, u32 key2);
extern void fn_80180450(void* main, void* aram, u32 size);
extern FSYSFileHandle* fsysSlotRefreshHandle(FSYSSlot* slot);

/* The slot's archive handle if the handle table holds it, else -1. */
static inline s32 fsysCacheFindHandle(FSYSSlot* slot)
{
    FSYSFileHandle* table;
    s32 i;

    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == (s32)slot->field_08) {
            return table->handleID;
        }
        table++;
    }
    return -1;
}

/*
 * The least recently used handle (the table's first entry). Retail expands
 * it through a call-site result variable numbered before `table` and `id`
 * (as fsys_file_candidate_8017DB74_gc20.c documents), i.e. as a statement
 * body, not as the comma expression a lone trailing return gives.
 */
static inline s32 fsysCacheOldestHandle(void)
{
    s32 id;
    FSYSFileHandle* table;

    table = lbl_8047B1B8;
    id = table->handleID;
    /* RULE-EXCEPTION(title-path): constant condition used only to force statement inlining — see docs/RULE_EXCEPTIONS.md */
    if (((void)0, 1)) {
        return id;
    }
    return -1;
}

/* Drop handleID from the handle table; -1 when it is not there. */
static inline s32 fsysCacheRemoveHandle(s32 handleID)
{
    FSYSFileHandle* table;
    s32 i;
    s32 found;
    s32 index;

    found = 0;
    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == handleID) {
            table->handleID = -1;
            found = 1;
            index = i;
            break;
        }
        table++;
    }
    if (found) {
        table = lbl_8047B1B8;
        for (i = 0; i < lbl_8047B1BC - 1; i++) {
            if (i >= index) {
                table[i] = table[i + 1];
            }
        }
        lbl_8047B1BC--;
        table[lbl_8047B1BC].handleID = -1;
        return 0;
    }
    return -1;
}

/*
 * Evict least recently used archives from the ARAM cache until `size`
 * bytes are free, unless the slot's own archive is cached already. `size`
 * is substituted into both tests, so an aligned size is recomputed at each.
 */
static inline void fsysCacheMakeRoom(FSYSSlot* slot, u32 size)
{
    u32 freeSize;
    s32 n;
    s32 handleID;

    freeSize = fn_8017FA5C();
    if (size > freeSize && fsysCacheFindHandle(slot) < 0) {
        for (n = 0;; n++) {
            handleID = fsysCacheOldestHandle();
            if (handleID < 0) {
                break;
            }
            fn_8017F800(handleID);
            if (fsysCacheRemoveHandle(handleID) < 0) {
                break;
            }
            freeSize = fn_8017FA5C();
            if (size <= freeSize) {
                break;
            }
        }
    }
}

/* The full-archive pass reads the size again after each cache eviction. */
static inline void fsysCacheMakeRoomTotal(FSYSSlot* slot)
{
    u32 freeSize;
    s32 n;
    s32 handleID;

    freeSize = fn_8017FA5C();
    if (slot->totalDecompSize > freeSize && fsysCacheFindHandle(slot) < 0) {
        for (n = 0;; n++) {
            handleID = fsysCacheOldestHandle();
            if (handleID < 0) {
                break;
            }
            fn_8017F800(handleID);
            if (fsysCacheRemoveHandle(handleID) < 0) {
                break;
            }
            freeSize = fn_8017FA5C();
            if (slot->totalDecompSize <= freeSize) {
                break;
            }
        }
    }
}

/*
 * Reset every entry's runtime trailer. Retail numbers this loop's count and
 * entry homes (0x64/0x68) with the eviction helpers' locals and the
 * fsysGetEntry expansion (0x8..0x18) after both evictions: the lookup is
 * expanded one inline level down, inside this helper.
 * RULE-EXCEPTION(title-path): single-use inline helper evidenced only by stack numbering — see docs/RULE_EXCEPTIONS.md
 */
static inline void fsysResetEntryTrailers(FSYSSlot* slot)
{
    s32 count;
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;
    s32 i;

    count = slot->numEntries;
    for (i = 0; i < count; i++) {
        entry = fsysGetEntry(slot, i);
        sub = (FSYSSubEntry*)entry->subEntry;
        sub->buffer = NULL;
        sub->state = 0;
        sub->ready = 0;
    }
}

/* Address: 0x8017CED8 | size: 0x4C8 */
s32 fsysStatus4(FSYSSlot* slot)
{
    u32 tocSize;
    u32 cached;

    tocSize = slot->field_1C;
    cached = fn_8017F794(slot->fileHandle, 0, 1);
    if (!cached) {
        DCFlushRange(slot->archiveData, (tocSize + 0x1F) & ~0x1F);
        fsysCacheMakeRoom(slot, (tocSize + 0x1F) & ~0x1F);
        cached = fn_8017F928((tocSize + 0x1F) & ~0x1F, slot->fileHandle, 0, 1);
        if (cached) {
            fn_80180450(slot->archiveData, (void*)cached, (tocSize + 0x1F) & ~0x1F);
        }
    }
    fsysCacheMakeRoomTotal(slot);
    fsysSlotRefreshHandle(slot);
    fsysResetEntryTrailers(slot);
    slot->padding05C = 1;
    slot->status = 100;
    return 0;
}
