/**
 * @file fsys_file_candidate_8017DB74_gc20.c
 * @brief FSYS archive header/TOC stage (0x8017DB74 - 0x8017DEA4).
 *
 * The slot's first 0x40 bytes hold the archive header just read. Cache the
 * header in ARAM (making room in the cache by evicting the least recently
 * used archives first), allocate the archive's TOC buffer, then either copy
 * the TOC from the ARAM cache (slot status 4) or read the rest of it from
 * the disc (status 3, fsysDVDReadCallback completes it).
 *
 * The ARAM cache eviction (fsysCacheMakeRoom and its handle-table helpers)
 * is expanded five times in the level-0 fsys code: here, twice in
 * fsysStatus4 and twice in fsysInitTOC (find_inline_expansions.py block
 * 0x8017DC88 0x8017DD50: the handle removal scores 0.86 at the four other
 * sites, the difference being register numbers and stack slots).
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas. `x = x | 0x40000000` is the form that emits retail's oris
 * (the compound `|=` on a u32 field emits lis/or at level 0). The bare
 * `cmplwi` after the TOC allocation is a release-build assert, as in
 * gs_range_8017FA5C_exact_80180320.c.
 *
 * Exact (2026-09-29) under two title-path exceptions: stack homes of
 * spilled inline variables follow the frontend's temporary numbering (a
 * higher number takes a lower offset), and fsysCacheOldestHandle needs a
 * statement-form body to number its result variable where retail has it;
 * a constant `((void)0, 1)` condition gives that body without emitting
 * code (the parser does not fold a comma expression, the backend does).
 * `tocSize = tocSize;` gives tocSize the extra level-0 reference weight
 * that ranks it above the inlined handle-table pointer (r28/r27).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSFileHandle* lbl_8047B1B8; /* handle table, LRU order */
extern s32 lbl_8047B1BC;             /* handle table fill count */

extern void* memcpy(void* dst, const void* src, u32 size);
extern void DCFlushRange(void* addr, u32 nBytes);
extern u16 GSmemAlloc(u32 size, u32 align);
extern void* GSmemLock(u16 handle);
extern void GSdvdReadAsync(u32 fileInfo, void* dst, u32 length, u32 offset,
                        void (*callback)(s32 result));
extern u32 fn_8017F794(u32 fileHandle, u32 key1, u32 key2);
extern void fn_8017F800(s32 handleID);
extern u32 fn_8017FA5C(void);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 key1, u32 key2);
extern void fn_80180320(void* main, void* aram, u32 size);
extern void fn_80180450(void* main, void* aram, u32 size);
extern void fsysDVDReadCallback(s32 result);

/* A release-build assert: the test survives, the handler is compiled out
 * (as in gs_range_8017FA5C_exact_80180320.c). */
#define GS_ASSERT(cond) ((cond) ? (void)0 : (void)0)

/* Heap allocation, 32-byte aligned (as in fsys_system_8017AAA4.c). */
static inline void* fsysAlloc(u32 size)
{
    u32 alignedSize;
    u16 handle;

    alignedSize = (size + 0x1F) & ~0x1F;
    handle = GSmemAlloc(alignedSize, 0x20);
    if (handle != 0) {
        return GSmemLock(handle);
    }
    return NULL;
}

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
 * this through a call-site result variable (stack 0x20, numbered before
 * `table` 0x1C and `id` 0x18), i.e. MWCC inlined it as a statement body; a
 * lone trailing return is inlined as a comma expression whose temporaries
 * the frontend numbers last (0xC/0x8 here).
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
 * is substituted into both tests, so an aligned size is recomputed at each
 * (fsysStatus4 passes slot->totalDecompSize, which is reloaded instead).
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

/* Address: 0x8017DB74 | size: 0x330 */
void fsysSlotReadNextEntry(FSYSSlot* slot)
{
    u8 loaded;
    u32 offset;
    u32 numEntries;
    u32 tocSize;
    u32 headerSize;
    u32 cached;
    u32 tocCached;

    loaded = 0;
    DCFlushRange(slot, 0x40);
    numEntries = slot->numEntries;
    tocSize = slot->field_1C;
    /* RULE-EXCEPTION(title-path): self-assignment used only for register priority — see docs/RULE_EXCEPTIONS.md */
    tocSize = tocSize;
    if (slot->loadMode == 7) {
        slot->field_10 = slot->field_10 | 0x40000000;
    }
    headerSize = 0x40;
    cached = fn_8017F794(slot->fileHandle, 0, 0);
    if (!cached) {
        fsysCacheMakeRoom(slot, (headerSize + 0x1F) & ~0x1F);
        cached = fn_8017F928((headerSize + 0x1F) & ~0x1F, slot->fileHandle, 0, 0);
        if (cached) {
            fn_80180450(slot, (void*)cached, (headerSize + 0x1F) & ~0x1F);
        }
    }
    slot->archiveData = fsysAlloc((tocSize + 0x1F) & ~0x1F);
    GS_ASSERT(slot->archiveData);
    tocCached = fn_8017F794(slot->fileHandle, 0, 1);
    if (tocCached) {
        fn_80180320(slot->archiveData, (void*)tocCached, (tocSize + 0x1F) & ~0x1F);
        DCFlushRange(slot->archiveData, (tocSize + 0x1F) & ~0x1F);
        slot->status = 4;
        loaded = 1;
    }
    if (!loaded) {
        slot->status = 3;
        memcpy(slot->archiveData, slot, 0x40);
        offset = 0x40;
        GSdvdReadAsync(slot->fileInfo0, (u8*)slot->archiveData + 0x40,
                    (tocSize - 0x40 + 0x1F) & ~0x1F, offset, fsysDVDReadCallback);
    }
}
