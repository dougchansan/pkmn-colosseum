/**
 * @file sdk_range_8009A2D8.c
 * @brief dolphin-sdk code, 0x8009A2D8 - 0x8009AFB0 (13 fns).
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

static void SetTimer(OSAlarm* alarm) {
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
#endif

extern OSAllocCell* fn_8009A92C(OSAllocCell* head, OSAllocCell* cell);

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

void* fn_8009A9D8(s32 heap, u32 size) {
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

void fn_8009AAD4(u32 idx, void* ptr) {
    OSHeapDesc* hd = &lbl_8047A6E8[idx];
    OSAllocCell* cell = (OSAllocCell*)((u8*)ptr - 0x20);

    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = fn_8009A92C(hd->free, cell);
}

u32 fn_8009AB50(u32 xfb) {
    u32 previous = lbl_80478980;

    lbl_80478980 = xfb;
    return previous;
}

void* fn_8009AB60(void* start, void* end, s32 count) {
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
s32 fn_8009ABD0(u32 start, u32 end) {
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
void fn_8009AC3C(u32 xfb) {
    lbl_8047A6E8[xfb].size = -1;
}
#endif

#if defined(SDK_8009AC50_CANDIDATE_ACTIVE)
s32 fn_8009AC50(s32 heap)
{
    OSHeapDesc* hd;
    OSAllocCell* cell;
    s32 total;
    s32 freeSpace;
    void* arenaStart;
    void* arenaEnd;

    total = 0;
    freeSpace = 0;

    if (lbl_8047A6E8 == NULL) {
        OSReport((const char*) lbl_80310198, 0x37D);
        return -1;
    }
    if (heap < 0 || heap >= lbl_8047A6EC) {
        OSReport((const char*) lbl_80310198 + 0x24, 0x37E);
        return -1;
    }

    hd = &lbl_8047A6E8[heap];
    if (hd->size <= 0) {
        OSReport((const char*) lbl_80310198 + 0x5C, 0x381);
        return -1;
    }
    if (hd->allocated != NULL && hd->allocated->prev != NULL) {
        OSReport((const char*) lbl_80310198 + 0x84, 0x383);
        return -1;
    }

    arenaStart = lbl_8047A6F0;
    arenaEnd = lbl_8047A6F4;
    cell = hd->allocated;
    while (cell != NULL) {
        if (cell < (OSAllocCell*) arenaStart || cell >= (OSAllocCell*) arenaEnd) {
            OSReport((const char*) lbl_80310198 + 0xD4, 0x386);
            return -1;
        }
        if (((u32) cell & 0x1F) != 0) {
            OSReport((const char*) lbl_80310198 + 0x114, 0x387);
            return -1;
        }
        if (cell->next != NULL && cell->next->prev != cell) {
            OSReport((const char*) lbl_80310198 + 0x14C, 0x388);
            return -1;
        }
        if (cell->size < 0x40) {
            OSReport((const char*) lbl_80310198 + 0x198, 0x389);
            return -1;
        }
        if ((cell->size & 0x1F) != 0) {
            OSReport((const char*) lbl_80310198 + 0x1CC, 0x38A);
            return -1;
        }
        total += cell->size;
        if (total <= 0 || total > hd->size) {
            OSReport((const char*) lbl_80310198 + 0x20C, 0x38D);
            return -1;
        }
        cell = cell->next;
    }

    if (hd->free != NULL && hd->free->prev != NULL) {
        OSReport((const char*) lbl_80310198 + 0x248, 0x395);
        return -1;
    }

    cell = hd->free;
    while (cell != NULL) {
        if (cell < (OSAllocCell*) arenaStart || cell >= (OSAllocCell*) arenaEnd) {
            OSReport((const char*) lbl_80310198 + 0xD4, 0x398);
            return -1;
        }
        if (((u32) cell & 0x1F) != 0) {
            OSReport((const char*) lbl_80310198 + 0x114, 0x399);
            return -1;
        }
        if (cell->next != NULL && cell->next->prev != cell) {
            OSReport((const char*) lbl_80310198 + 0x14C, 0x39A);
            return -1;
        }
        if (cell->size < 0x40) {
            OSReport((const char*) lbl_80310198 + 0x198, 0x39B);
            return -1;
        }
        if ((cell->size & 0x1F) != 0) {
            OSReport((const char*) lbl_80310198 + 0x1CC, 0x39C);
            return -1;
        }
        if (cell->next != NULL &&
            (u8*) cell + cell->size >= (u8*) cell->next)
        {
            OSReport((const char*) lbl_80310198 + 0x290, 0x39D);
            return -1;
        }

        total += cell->size;
        freeSpace += cell->size - 0x20;
        if (total <= 0 || total > hd->size) {
            OSReport((const char*) lbl_80310198 + 0x20C, 0x3A1);
            return -1;
        }
        cell = cell->next;
    }

    if (total != hd->size) {
        OSReport((const char*) lbl_80310198 + 0x2F0, 0x3A8);
        return -1;
    }
    return freeSpace;
}
#endif
