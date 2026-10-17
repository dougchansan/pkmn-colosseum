/**
 * @file gs_range_8017FA5C_exact_8017FDB0.c
 * @brief gs small-block heap allocator fn_8017FDB0 (0x8017FDB0 - 0x801800F8).
 *
 * K&R-style malloc over the lbl_80455070 block descriptors: first fit on
 * the circular free list, splitting the tail off a larger block, and
 * growing from the lbl_80455048 bump arena (morecore) when the walk wraps.
 * When every descriptor is in use it flushes the handle cache
 * (fsysFlushCachedHandles) and retries.
 *
 * Carved out of the 0x8017FA5C - 0x80180C78 retail unit so the exact
 * function can link on its own; built like the rest of it at
 * `-opt level=0` with `-inline deferred`, with no local pragmas.
 */
#include "game/gs_range_8017FA5C_shared.h"

static inline GsRangeMemNode* memNewBlock(void* data)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        if (!block->data) {
            block->data = data;
            return block;
        }
        block++;
    }
    return NULL;
}

static inline void* memArenaGrow(u32 size)
{
    u8* cursor = lbl_80455048.cursor[lbl_80455048.cursorIndex];
    u32 alignedSize = (size + 0x1F) & ~0x1F;

    if (lbl_80455048.remaining >= alignedSize && lbl_80455048.cursorIndex < 7) {
        lbl_80455048.cursorIndex++;
        lbl_80455048.cursor[lbl_80455048.cursorIndex] = cursor + alignedSize;
        lbl_80455048.remaining -= alignedSize;
        return cursor;
    }
    return NULL;
}

static inline GsRangeMemNode* memMoreCore(u32 size)
{
    u8* cursor;
    GsRangeMemNode* block;

    if (size < 0x20) {
        size = 0x20;
    }
    cursor = memArenaGrow(size);
    if (!cursor) {
        return NULL;
    }
    block = memNewBlock(cursor);
    if (!block) {
        fsysFlushCachedHandles();
        block = memNewBlock(cursor);
    }
    block->size = size;
    fn_8017FB08(block->data);
    return lbl_8047B1D0;
}

void* fn_8017FDB0(u32 size)
{
    GsRangeMemNode* p;
    GsRangeMemNode* prevp;
    GsRangeMemNode* orig;
    u32 alignedSize;
    s32 count = 0;

    alignedSize = (size + 0x1F) & ~0x1F;
    if (!(prevp = lbl_8047B1D0)) {
        lbl_80465070.next = lbl_8047B1D0 = prevp = &lbl_80465070;
        lbl_80465070.size = 0;
    }
    for (count = 0, p = prevp->next;; prevp = p, p = p->next) {
        count++;
        if ((u32)p <= 0x80000000) {
            return NULL;
        }
        if (p->size >= alignedSize) {
            if (p->size == alignedSize) {
                prevp->next = p->next;
            } else {
                orig = p;
                p->size -= alignedSize;
                p = (GsRangeMemNode*)((u8*)p->data +
                                      p->size / sizeof(GsRangeMemNode) * sizeof(GsRangeMemNode));
                p = memNewBlock(p);
                if (!p) {
                    fsysFlushCachedHandles();
                    p = memNewBlock(p);
                }
                p->previous = orig;
                p->size = alignedSize;
            }
            lbl_8047B1D0 = prevp;
            return p->data;
        }
        if (p == lbl_8047B1D0) {
            if (!(p = memMoreCore(alignedSize))) {
                return NULL;
            }
        }
    }
}
