/**
 * @file sdk_range_8009A2D8.c
 * @brief dolphin-sdk code, 0x8009A2D8 - 0x8009AFB0 (13 fns). The OSAlarm.c
 *        part (0x8009A27C - 0x8009A92C) links as dolphin/os/OSAlarm.c, which
 *        defines OS_ALARM_FILE and includes this file.
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/PPCArch.h"

typedef struct OSAllocCell {
    struct OSAllocCell* prev;
    struct OSAllocCell* next;
    s32 size;
} OSAllocCell;

typedef struct OSHeapDesc {
    s32 size;
    OSAllocCell* free;
    OSAllocCell* allocated;
} OSHeapDesc;

extern volatile s32 lbl_80478980;
extern OSHeapDesc* lbl_8047A6E8;
extern s32 lbl_8047A6EC;
extern void* lbl_8047A6F0;
extern void* lbl_8047A6F4;
extern const char lbl_80310198[];

typedef struct OSAlarmQueue {
    OSAlarm* head;
    OSAlarm* tail;
} OSAlarmQueue;

#if !defined(SDK_8009A9D8_PREFIX_ACTIVE) && !defined(SDK_EXACT_8009ABD0) && \
    !defined(SDK_8009AC3C_SUFFIX_ACTIVE)
static OSAlarmQueue AlarmQueue;

static void InsertAlarm(OSAlarm* alarm, s64 fire, OSAlarmHandler handler);
static void DecrementerExceptionCallback(u8 exception, OSContext* context);
static void DecrementerExceptionHandler(u8 exception, OSContext* context);

#if defined(OS_ALARM_FILE)
/* 0x8009A27C: the first two functions of OSAlarm.c. */
void OSInitAlarm(void) {
    typedef void (*OSExceptionHandlerFn)(u8 exception, OSContext* context);
    extern OSExceptionHandlerFn __OSGetExceptionHandler(u8 exception);
    extern OSExceptionHandlerFn __OSSetExceptionHandler(u8 exception, OSExceptionHandlerFn handler);

    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler) {
        AlarmQueue.tail = NULL;
        AlarmQueue.head = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler);
    }
}

void OSCreateAlarm(OSAlarm* alarm) {
    alarm->handler = NULL;
    alarm->tag = 0;
}
#endif

static inline void SetTimer(OSAlarm* alarm) {
    s64 delta;

    delta = alarm->fire - __OSGetSystemTime();

    if (delta < 0) {
        PPCMtdec(0);
    } else if (delta < 0x80000000) {
        PPCMtdec((u32)delta);
    } else {
        PPCMtdec(0x7FFFFFFF);
    }
}

static void InsertAlarm(OSAlarm* alarm, s64 fire, OSAlarmHandler handler) {
    OSAlarm* next;
    OSAlarm* prev;

    if (0 < alarm->period) {
        s64 time = __OSGetSystemTime();

        fire = alarm->start;
        if (alarm->start < time) {
            fire += alarm->period * ((time - alarm->start) / alarm->period + 1);
        }
    }

    alarm->handler = handler;
    alarm->fire    = fire;

    for (next = AlarmQueue.head; next; next = next->next) {
        if (next->fire <= fire) {
            continue;
        }

        alarm->prev = next->prev;
        next->prev  = alarm;
        alarm->next = next;
        prev = alarm->prev;

        if (prev) {
            prev->next = alarm;
        } else {
            AlarmQueue.head = alarm;
            SetTimer(alarm);
        }

        return;
    }

    alarm->next = 0;
    prev = AlarmQueue.tail;
    AlarmQueue.tail = alarm;
    alarm->prev = prev;

    if (prev) {
        prev->next = alarm;
    } else {
        AlarmQueue.head = AlarmQueue.tail = alarm;
        SetTimer(alarm);
    }
}

void OSSetAlarm(OSAlarm* alarm, s64 tick, OSAlarmHandler handler) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    alarm->period = 0;
    InsertAlarm(alarm, __OSGetSystemTime() + tick, handler);
    OSRestoreInterrupts(enabled);
}

void OSCancelAlarm(OSAlarm* alarm) {
    OSAlarm* next;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (alarm->handler == 0) {
        OSRestoreInterrupts(enabled);
        return;
    }

    next = alarm->next;
    if (next == 0) {
        AlarmQueue.tail = alarm->prev;
    } else {
        next->prev = alarm->prev;
    }
    if (alarm->prev) {
        alarm->prev->next = next;
    } else {
        AlarmQueue.head = next;
        if (next) {
            SetTimer(next);
        }
    }
    alarm->handler = 0;
    OSRestoreInterrupts(enabled);
}

static void DecrementerExceptionCallback(u8 exception, OSContext* context) {
    OSAlarm*        alarm;
    OSAlarm*        next;
    OSAlarmHandler  handler;
    s64             time;
    OSContext       exceptionContext;

    time  = __OSGetSystemTime();
    alarm = AlarmQueue.head;

    if (alarm == 0) {
        OSLoadContext(context);
    }

    if (time < alarm->fire) {
        SetTimer(alarm);
        OSLoadContext(context);
    }

    next = alarm->next;
    AlarmQueue.head = next;
    if (next == 0) {
        AlarmQueue.tail = 0;
    } else {
        next->prev = 0;
    }

    handler = alarm->handler;
    alarm->handler = 0;

    if (0 < alarm->period) {
        InsertAlarm(alarm, 0, handler);
    }

    if (AlarmQueue.head) {
        SetTimer(AlarmQueue.head);
    }

    OSDisableScheduler();
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    handler(alarm, context);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
    OSEnableScheduler();
    __OSReschedule();
    OSLoadContext(context);
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_alarm.md.
 * It saves the remaining GPRs and the GQRs into the context and tail-branches
 * to DecrementerExceptionCallback (a declared branch target). */
static asm void DecrementerExceptionHandler(register u8 exception, register OSContext* context) {
    nofralloc

    stw     r0, 0x0(context)
    stw     r1, 0x4(context)
    stw     r2, 0x8(context)
    stmw    r6, 0x18(context)
    mfspr   r0, GQR1
    stw     r0, 0x1A8(context)
    mfspr   r0, GQR2
    stw     r0, 0x1AC(context)
    mfspr   r0, GQR3
    stw     r0, 0x1B0(context)
    mfspr   r0, GQR4
    stw     r0, 0x1B4(context)
    mfspr   r0, GQR5
    stw     r0, 0x1B8(context)
    mfspr   r0, GQR6
    stw     r0, 0x1BC(context)
    mfspr   r0, GQR7
    stw     r0, 0x1C0(context)
    stwu    r1, -0x8(r1)
    b       DecrementerExceptionCallback
}
#endif

extern OSAllocCell* DLInsert(OSAllocCell* head, OSAllocCell* cell);

#if defined(SDK_8009A9D8_PREFIX_ACTIVE)
static OSAllocCell* DLAddFront(OSAllocCell* list, OSAllocCell* cell) {
    cell->next = list;
    cell->prev = 0;
    if (list) {
        list->prev = cell;
    }
    return cell;
}

static OSAllocCell* DLExtract(OSAllocCell* list, OSAllocCell* cell) {
    if (cell->next) {
        cell->next->prev = cell->prev;
    }
    if (cell->prev == 0) {
        return cell->next;
    }
    cell->prev->next = cell->next;
    return list;
}

void* OSAllocFromHeap(s32 heap, u32 size) {
    OSHeapDesc* hd;
    OSAllocCell* cell;
    OSAllocCell* newCell;
    s32 leftoverSize;

    hd = &lbl_8047A6E8[heap];
    size += 0x20;
    size = (size + 0x1F) & ~0x1F;

    for (cell = hd->free; cell != 0; cell = cell->next) {
        if ((s32)size <= cell->size) {
            break;
        }
    }
    if (cell == 0) {
        return 0;
    }

    leftoverSize = cell->size - size;
    if ((u32)leftoverSize < 0x40) {
        hd->free = DLExtract(hd->free, cell);
    } else {
        cell->size = size;
        newCell = (OSAllocCell*)((u8*)cell + size);
        newCell->size = leftoverSize;
        newCell->prev = cell->prev;
        newCell->next = cell->next;
        if (newCell->next != 0) {
            newCell->next->prev = newCell;
        }
        if (newCell->prev != 0) {
            newCell->prev->next = newCell;
        } else {
            hd->free = newCell;
        }
    }

    hd->allocated = DLAddFront(hd->allocated, cell);
    return (u8*)cell + 0x20;
}

void OSFreeToHeap(u32 idx, void* ptr) {
    OSHeapDesc* hd = &lbl_8047A6E8[idx];
    OSAllocCell* cell = (OSAllocCell*)((u8*)ptr - 0x20);

    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = DLInsert(hd->free, cell);
}

u32 OSSetCurrentHeap(u32 xfb) {
    u32 previous = lbl_80478980;

    lbl_80478980 = xfb;
    return previous;
}

void* OSInitAlloc(void* start, void* end, s32 count) {
    u32 arraySize;
    s32 i;
    OSHeapDesc* hd;

    arraySize = count * sizeof(OSHeapDesc);
    lbl_8047A6E8 = start;
    lbl_8047A6EC = count;
    for (i = 0; i < lbl_8047A6EC; i++) {
        hd = &lbl_8047A6E8[i];
        hd->size = -1;
        hd->free = hd->allocated = 0;
    }

    lbl_80478980 = -1;
    start = (void*)((u32)lbl_8047A6E8 + arraySize);
    start = (void*)(((u32)start + 0x1F) & ~0x1F);
    lbl_8047A6F0 = start;
    lbl_8047A6F4 = (void*)((u32)end & ~0x1F);
    return start;
}
#endif

#if defined(SDK_EXACT_8009ABD0)
s32 OSCreateHeap(u32 start, u32 end) {
    s32 i;
    OSHeapDesc* arr = lbl_8047A6E8;
    s32 count;
    u32 alignedStart = (start + 0x1F) & ~0x1F;
    u32 alignedEnd = end & ~0x1F;

    count = lbl_8047A6EC;
    for (i = 0; i < count; arr++, i++) {
        if (arr->size < 0) {
            OSAllocCell* blk = (OSAllocCell*)alignedStart;

            arr->size = alignedEnd - alignedStart;
            blk->prev = 0;
            blk->next = 0;
            blk->size = arr->size;
            arr->free = blk;
            arr->allocated = 0;
            return i;
        }
    }
    return -1;
}
#endif

#if defined(SDK_8009AC3C_SUFFIX_ACTIVE)
void OSDestroyHeap(u32 xfb) {
    lbl_8047A6E8[xfb].size = -1;
}
#endif

#if defined(SDK_8009AC50_CANDIDATE_ACTIVE)
/* OSAlloc.c's OSCheckHeap. Its failure messages (and the stripped
 * OSDumpHeap's) are this object's string run, lbl_80310198 (.data
 * 0x80310198-0x80310530), which the unit owns. */
#define InRange_8009AC50(cell, start, end) \
    ((u32)(start) <= (u32)(cell) && (u32)(cell) < (u32)(end))
#define OFFSET_8009AC50(n, a) (((u32)(n)) & ((a) - 1))
#define CHECK_8009AC50(line, msg, condition)                 \
    if (!(condition)) {                                      \
        OSReport("OSCheckHeap: Failed " msg " in %d", line); \
        return -1;                                           \
    }

s32 OSCheckHeap(s32 heap)
{
    OSHeapDesc* hd;
    OSAllocCell* cell;
    s32 total = 0;
    s32 free = 0;

    CHECK_8009AC50(893, "HeapArray", lbl_8047A6E8);
    CHECK_8009AC50(894, "0 <= heap && heap < NumHeaps", 0 <= heap && heap < lbl_8047A6EC);

    hd = &lbl_8047A6E8[heap];
    CHECK_8009AC50(897, "0 <= hd->size", 0 <= hd->size);

    CHECK_8009AC50(899, "hd->allocated == NULL || hd->allocated->prev == NULL", hd->allocated == NULL || hd->allocated->prev == NULL);

    for (cell = hd->allocated; cell; cell = cell->next) {
        CHECK_8009AC50(902, "InRange(cell, ArenaStart, ArenaEnd)", InRange_8009AC50(cell, lbl_8047A6F0, lbl_8047A6F4));
        CHECK_8009AC50(903, "OFFSET(cell, ALIGNMENT) == 0", OFFSET_8009AC50(cell, 32) == 0);
        CHECK_8009AC50(904, "cell->next == NULL || cell->next->prev == cell", cell->next == NULL || cell->next->prev == cell);
        CHECK_8009AC50(905, "MINOBJSIZE <= cell->size", 64u <= cell->size);
        CHECK_8009AC50(906, "OFFSET(cell->size, ALIGNMENT) == 0", OFFSET_8009AC50(cell->size, 32) == 0);

        total += cell->size;
        CHECK_8009AC50(909, "0 < total && total <= hd->size", 0 < total && total <= hd->size);
    }

    CHECK_8009AC50(917, "hd->free == NULL || hd->free->prev == NULL", hd->free == NULL || hd->free->prev == NULL);

    for (cell = hd->free; cell; cell = cell->next) {
        CHECK_8009AC50(920, "InRange(cell, ArenaStart, ArenaEnd)", InRange_8009AC50(cell, lbl_8047A6F0, lbl_8047A6F4));
        CHECK_8009AC50(921, "OFFSET(cell, ALIGNMENT) == 0", OFFSET_8009AC50(cell, 32) == 0);
        CHECK_8009AC50(922, "cell->next == NULL || cell->next->prev == cell", cell->next == NULL || cell->next->prev == cell);
        CHECK_8009AC50(923, "MINOBJSIZE <= cell->size", 64u <= cell->size);
        CHECK_8009AC50(924, "OFFSET(cell->size, ALIGNMENT) == 0", OFFSET_8009AC50(cell->size, 32) == 0);
        CHECK_8009AC50(925, "cell->next == NULL || (char*) cell + cell->size < (char*) cell->next", cell->next == NULL || (char*)cell + cell->size < (char*)cell->next);

        total += cell->size;
        free += cell->size - 32u;
        CHECK_8009AC50(929, "0 < total && total <= hd->size", 0 < total && total <= hd->size);
    }

    CHECK_8009AC50(936, "total == hd->size", total == hd->size);

    return free;
}

/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function — see docs/RULE_EXCEPTIONS.md
 * OSAlloc.c's OSDumpHeap (Dolphin SDK; release build, asserts compiled out).
 * Retail strips it, but its five report strings stay in the object's .data
 * right after OSCheckHeap's (0x803104B4-0x80310530 inside lbl_80310198), so
 * the unit only owns its whole string run with it present. */
void OSDumpHeap(s32 heap)
{
    OSHeapDesc* hd;
    OSAllocCell* cell;

    OSReport("\nOSDumpHeap(%d):\n", heap);
    hd = &lbl_8047A6E8[heap];
    if (hd->size < 0) {
        OSReport("--------Inactive\n");
        return;
    }
    OSReport("addr\tsize\t\tend\tprev\tnext\n");
    OSReport("--------Allocated\n");
    for (cell = hd->allocated; cell; cell = cell->next) {
        OSReport("%x\t%d\t%x\t%x\t%x\n", cell, cell->size,
                 (char*)cell + cell->size, cell->prev, cell->next);
    }
    OSReport("--------Free\n");
    for (cell = hd->free; cell; cell = cell->next) {
        OSReport("%x\t%d\t%x\t%x\t%x\n", cell, cell->size,
                 (char*)cell + cell->size, cell->prev, cell->next);
    }
}
#endif
