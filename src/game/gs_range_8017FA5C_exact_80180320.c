/**
 * @file gs_range_8017FA5C_exact_80180320.c
 * @brief ARQ transfer queue: DMA wrappers, busy poll and completion callback
 *        (0x80180320 - 0x8018094C).
 *
 * Part of the 0x8017FA5C - 0x801812C4 retail unit, carved so its exact
 * functions can link; built like the rest of it at `-opt level=0` with
 * `-inline deferred` (deferred inlining emits a unit's functions in reverse
 * source order, so they are written from the highest address down) and no
 * local pragmas.
 *
 * Inlining evidence:
 * - arqRead / arqWrite are the request bodies: their result travels through
 *   a return temporary (r27, "mr r27,r31 / li r27,0") that the wrapper then
 *   copies to its own local, which only an inline expansion produces;
 *   arqAlloc, expanded in both, leaves the same pattern in r26.
 * - The synchronous wrappers are the asynchronous ones plus a busy wait.
 *   fsysMainToARAMSync expands fsysARQPost and fsysARAMToMainSync expands fsysARAMToMainAsync
 *   with NULL callbacks (`-inline auto` inlines these same-unit calls), and
 *   both expand fsysARQIsBusy in the wait loop ("mr r29,rN; lwz r0,0x24(rN)",
 *   the typed view of its handle). fsysARQIsBusy therefore has to live in the
 *   same object as its callers.
 *
 * GS_ASSERT: every wrapper copies `aram` with a record-form "mr. rN,r4"
 * whose CR0 result nothing reads; the size test that decides the branch is
 * a separate cmplwi. That dead compare is what a conditional with two empty
 * arms leaves at level 0: a release-build assert whose failure handler is
 * compiled out but whose test survives. Without it `aram` also loses the
 * extra reference that ranks its register above `main`'s.
 */
#include "game/gs_range_8017FA5C_shared.h"

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern void ARQPostRequest(void* request, u32 owner, u32 type, u32 priority,
                           u32 source, u32 dest, u32 length,
                           void (*callback)(u32 request));

#define GS_ASSERT(cond) ((cond) ? (void)0 : (void)0)

/* Address: 0x801808E4 | size: 0x68
 * ARQ completion callback: mark the entry idle, run the user callback,
 * release the entry and flush the transferred range. */
void fsysARQCallback2(GsRangeARQEntry* entry)
{
    GsRangeARQEntry* e = entry;

    e->mode = 0;
    if (e->callback) {
        e->callback((void*)e->flush, e->callbackArg);
    }
    e->state = 0;
    DCFlushRange(e->src, e->size);
}

/* Address: 0x801808B4 | size: 0x30
 * Nonzero while the request is still in flight. */
s32 fsysARQIsBusy(void* handle)
{
    GsRangeARQEntry* entry = handle;

    if (entry->mode != 1) {
        entry->state = 0;
    }
    return entry->state;
}

static inline GsRangeARQEntry* arqAlloc(void)
{
    GsRangeARQEntry* entry;
    u32 i;

    entry = lbl_8047B1D4;
    for (i = 0; i < lbl_8047B1D8; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            return entry;
        }
        entry++;
    }
    return NULL;
}

/* Main memory -> ARAM. */
static inline GsRangeARQEntry* arqWrite(void* main, void* aram, u32 size,
                                        void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry;
    BOOL level;
    u32 alignedSize;

    GS_ASSERT(aram);
    if (size != 0) {
        entry = arqAlloc();
        level = OSDisableInterrupts();
        entry->flush = 0;
        entry->mode = 1;
        entry->callback = callback;
        entry->callbackArg = arg;
        alignedSize = (size + 0x1F) & ~0x1F;
        entry->src = main;
        entry->dst = aram;
        entry->size = alignedSize;
        DCFlushRange(main, size);
        ARQPostRequest(entry, (u32)entry, 0, 0, (u32)main, (u32)aram,
                       (size + 0x1F) & ~0x1F, (void (*)(u32))fsysARQCallback2);
        OSRestoreInterrupts(level);
        return entry;
    }
    return NULL;
}

/* ARAM -> main memory. */
static inline GsRangeARQEntry* arqRead(void* main, void* aram, u32 size,
                                       void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry;
    BOOL level;
    u32 alignedSize;

    GS_ASSERT(aram);
    if (size != 0) {
        entry = arqAlloc();
        level = OSDisableInterrupts();
        entry->flush = 1;
        entry->mode = 1;
        entry->callback = callback;
        entry->callbackArg = arg;
        alignedSize = (size + 0x1F) & ~0x1F;
        entry->src = main;
        entry->dst = aram;
        entry->size = alignedSize;
        DCFlushRange(main, size);
        ARQPostRequest(entry, (u32)entry, 1, 0, (u32)aram, (u32)main, alignedSize,
                       (void (*)(u32))fsysARQCallback2);
        OSRestoreInterrupts(level);
        return entry;
    }
    return NULL;
}

/* Address: 0x801807A8 | size: 0x10C */
GsRangeARQEntry* fsysARQPost(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, NULL, NULL);
    return entry;
}

/* Address: 0x80180694 | size: 0x114 */
GsRangeARQEntry* fsysMainToARAMAsync(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, callback, arg);
    return entry;
}

/* Address: 0x80180584 | size: 0x110 */
GsRangeARQEntry* fsysARAMToMainAsync(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqRead(main, aram, size, callback, arg);
    return entry;
}

/* Address: 0x80180450 | size: 0x134
 * Synchronous main memory -> ARAM copy. */
void fsysMainToARAMSync(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = fsysARQPost(main, aram, size);

    while (fsysARQIsBusy(entry)) {
    }
}

/* Address: 0x80180320 | size: 0x130
 * Synchronous ARAM -> main memory copy. */
void fsysARAMToMainSync(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = fsysARAMToMainAsync(main, aram, size, NULL, NULL);

    while (fsysARQIsBusy(entry)) {
    }
}
