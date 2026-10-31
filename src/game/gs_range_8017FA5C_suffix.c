/**
 * @file gs_range_8017FA5C_suffix.c
 * @brief gs small-block heap, 0x8017FB08 - 0x8017FDB0: K&R-style free.
 *
 * Matching carve of the 0x8017FA5C - 0x80180C78 retail unit, built like the
 * rest of it at `-opt level=0` with `-inline deferred`.
 *
 * At level 0 the frontend gives locals the callee-saved registers ranked by
 * reference weight, then declaration order. memAbsorbNext's adjacency test
 * uses its own NULL-initialised end/data locals, like gsSmallHeapFree's: their
 * two initial zero stores (retail +0xF4..+0x108) and the stack homes
 * 0x18/0x1C are those locals. Retail also ranks both inline list heads above
 * gsSmallHeapFree's end/data pair; one extra reference to each head (see the
 * title-path exception tags) gives that ranking, and the function is exact.
 */
#include "game/gs_range_8017FA5C_shared.h"

static inline GsRangeMemNode* memFindBlock(void* data)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        if (block->data == data) {
            return block;
        }
        block++;
    }
    return NULL;
}

static inline GsRangeMemNode* memFindPrev(GsRangeMemNode* target)
{
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* a = NULL;
    GsRangeMemNode* b = NULL;
    GsRangeMemNode* p;

    /* RULE-EXCEPTION(title-path): no-op statement used only for register priority — see docs/RULE_EXCEPTIONS.md */
    (void)head;
    for (p = head->next;; p = p->next) {
        /* RULE-EXCEPTION(title-path): pure copies used only for register/stack layout — see docs/RULE_EXCEPTIONS.md */
        a = target;
        b = p->next;
        if (a == b) {
            return p;
        }
    }
}

static inline GsRangeMemNode* memAbsorbNext(GsRangeMemNode* block)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    /* RULE-EXCEPTION(title-path): no-op statement used only for register priority — see docs/RULE_EXCEPTIONS.md */
    (void)head;
    if (!block) {
        return NULL;
    }
    for (p = head->next;; p = p->next) {
        end = (u8*)block->data + block->size;
        data = p->data;
        if (end == data) {
            block->size += p->size;
            p->data = NULL;
            prev = memFindPrev(p);
            prev->next = p->next;
            if (p == lbl_8047B1D0) {
                lbl_8047B1D0 = p->next;
            }
            p->previous = NULL;
            p->next = NULL;
            return p;
        }
        if (p == lbl_8047B1D0) {
            break;
        }
    }
    return NULL;
}


void gsSmallHeapFree(void* allocation)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* bp;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!allocation) {
        return;
    }
    bp = memFindBlock(allocation);
    if (!bp) {
        return;
    }
    for (p = lbl_8047B1D0; !(bp > p && bp < p->next); p = p->next) {
        if (p >= p->next && (bp > p || bp < p->next)) {
            break;
        }
    }
    if (bp->previous) {
        end = (u8*)bp->previous->data + bp->previous->size;
        prev = bp->previous;
    }
    data = bp->data;
    if (end == data) {
        bp->previous->size += bp->size;
        bp->data = NULL;
        bp->next = NULL;
        bp->previous = NULL;
        while (memAbsorbNext(prev)) {
        }
        return;
    }
    if (p->next->data == (u8*)bp->data + bp->size) {
        bp->size += p->next->size;
        bp->next = p->next->next;
        p->next->data = NULL;
    } else {
        bp->next = p->next;
    }
    if (bp->data == (u8*)p->data + p->size) {
        p->size += bp->size;
        p->next = bp->next;
        bp->data = NULL;
    } else {
        p->next = bp;
    }
    lbl_8047B1D0 = p;
}
