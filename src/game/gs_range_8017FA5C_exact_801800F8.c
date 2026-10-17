/**
 * @file gs_range_8017FA5C_exact_801800F8.c
 * @brief gs heap / handle-cache / ARQ-table initialisation fsysCacheInit
 *        (0x801800F8 - 0x80180320) and the unit's pooled .bss
 *        (0x80454018 - 0x80455070).
 *
 * Carved out of the 0x8017FA5C - 0x80180C78 retail unit, built like the
 * rest of it at `-opt level=0` with `-inline deferred` (see configure.py).
 *
 * Pooled .bss. The queue, the cache area and the arena are defined in this
 * unit, so MWCC (1.3.2 and later, also at optimisation level 0) addresses
 * all three from one base: retail loads lbl_80454018 once into r29 and
 * reaches each object with a fresh `addi rX,r29,<object offset>`, leaving
 * the member offset in the store (addi r10,r29,0x20; stw r0,0xc(r10)).
 * lbl_80455070 (the block descriptors) is defined in a later part of the
 * unit, so it keeps its own relocation. Functions that touch only one of
 * these objects (the cache helpers, the allocator) address it directly.
 *
 * .bss order. Normal inlining lays pooled .bss out in the order the code
 * first references each object, which here would put the cache (first
 * touched by the pre-call cache.nodes = NULL) ahead of the queue. Under
 * `-inline deferred`, inlining and code generation wait for the end of the
 * unit, and the objects are laid out in reverse definition order. Defining
 * arena, cache area, queue in that order gives retail's queue +0, cache
 * +0x20, arena +0x1030 with no extra references.
 *
 * Registers. The cache set-up is an inline helper, so its allocation's
 * handle and result (r21/r22) rank below the ARQ table's (r23/r24), and it
 * takes the size through a parameter so memAlloc receives it with no copy
 * (mr r3,r18). The three loops use three separate counters (r26, r25,
 * r28). `heap` is the arena block that fn_8017FDB0 returns and fn_8017FB08
 * frees onto the list; MWCC gives it r20 and then forwards the call result
 * straight into r3, so r20 is saved but unused, as in retail. The r14-r16
 * "constants" are ordinary temporaries spilled once r3-r12 hold pooled
 * bases.
 */
#include "game/gs_range_8017FA5C_shared.h"

GsRangeArena lbl_80455048;
GsRangeCacheArea lbl_80454038;
GsRangeQueue lbl_80454018;

static inline void cacheInit(u32 size)
{
    GsRangeCacheNode* node;
    s32 i;

    lbl_80454038.cache.nodes = NULL;
    lbl_80454038.cache.nodes = memAlloc(size);
    lbl_80454038.cache.last = NULL;
    lbl_80454038.cache.count = 0;
    lbl_80454038.cache.capacity = 0x400;
    lbl_80454018.field_10 = 0;
    lbl_80454018.field_14 = 0;
    lbl_80454018.field_18 = 0;
    lbl_80454018.field_00 = 0;
    lbl_80454018.field_04 = 0;
    lbl_80454018.field_08 = 0;
    lbl_80454018.field_0C = 0;
    node = lbl_80454038.cache.nodes;
    for (i = 0; i < 0x400; i++) {
        node->size = 0;
        node->data = node->prev = node->next = NULL;
        node->active = 0;
        node++;
    }
    node = lbl_80454038.cache.nodes;
    node->size = 0;
    node->next = node + 1;
    lbl_80454038.cache.last = node;
}

static inline void memBlocksClear(void)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        block->data = NULL;
        block++;
    }
}

void fsysCacheInit(u32 entryCount, u8* arena, u32 arenaSize)
{
    GsRangeARQEntry* entry;
    u32 j;
    void* heap;
    u32 entrySize;
    u32 cacheSize;

    cacheSize = 0x8000;
    cacheInit(cacheSize);
    memBlocksClear();
    lbl_8047B1D8 = entryCount;
    entrySize = (entryCount * sizeof(GsRangeARQEntry) + 0x1F) & ~0x1F;
    lbl_8047B1D4 = memAlloc(entrySize);
    entry = lbl_8047B1D4;
    for (j = 0; j < lbl_8047B1D8; j++) {
        entry->state = 0;
        entry->mode = 0;
        entry->callback = NULL;
        entry->callbackArg = NULL;
        entry->index = j;
        entry++;
    }
    lbl_8047B1D0 = NULL;
    lbl_80455048.cursorIndex = 0;
    lbl_80455048.remaining = arenaSize;
    lbl_80455048.cursor[0] = arena;
    heap = fn_8017FDB0(arenaSize);
    fn_8017FB08(heap);
}
