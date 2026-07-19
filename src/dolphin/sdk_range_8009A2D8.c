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

typedef struct {
    s32 unk0;
    u32 unk4;
    u32 unk8;
} AlarmCallback;

extern u32 lbl_80478980;
extern AlarmCallback* lbl_8047A6E8;
extern s32 lbl_8047A6EC;
extern u32 lbl_8047A6F0;
extern u32 lbl_8047A6F4;

typedef struct OSAlarmQueue {
    OSAlarm* head;
    OSAlarm* tail;
} OSAlarmQueue;

#if !defined(SDK_8009A9D8_SUFFIX_ACTIVE)
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

extern AlarmCallback* fn_8009A92C(AlarmCallback* head, AlarmCallback* blk,
                                   void* unused);

#if defined(SDK_8009A9D8_SUFFIX_ACTIVE)
void* fn_8009A9D8(u32 idx, u32 size) {
    AlarmCallback* rec = &lbl_8047A6E8[idx];
    AlarmCallback* node = (AlarmCallback*)rec->unk4;
    u32 reqSize = (size + 0x3F) & ~0x1F;

    while (node != 0) {
        if (reqSize <= node->unk8) {
            break;
        }
        node = (AlarmCallback*)node->unk4;
    }
    if (node == 0) {
        return 0;
    }

    if (node->unk8 - reqSize < 0x40) {
        AlarmCallback* next = (AlarmCallback*)node->unk4;
        u32 newHead = rec->unk4;

        if (next != 0) {
            next->unk0 = node->unk0;
        }
        if ((u32)node->unk0 == 0) {
            newHead = node->unk4;
        } else {
            ((AlarmCallback*)node->unk0)->unk4 = node->unk4;
        }
        rec->unk4 = newHead;
    } else {
        AlarmCallback* newblk;
        u32 remainder = node->unk8 - reqSize;

        node->unk8 = reqSize;
        newblk = (AlarmCallback*)((u8*)node + reqSize);
        newblk->unk8 = remainder;
        newblk->unk0 = node->unk0;
        newblk->unk4 = node->unk4;
        if (newblk->unk4 != 0) {
            ((AlarmCallback*)newblk->unk4)->unk0 = (s32)newblk;
        }
        if ((u32)newblk->unk0 != 0) {
            ((AlarmCallback*)newblk->unk0)->unk4 = (u32)newblk;
        } else {
            rec->unk4 = (u32)newblk;
        }
    }

    node->unk4 = rec->unk8;
    node->unk0 = 0;
    if (rec->unk8 != 0) {
        ((AlarmCallback*)rec->unk8)->unk0 = (s32)node;
    }
    rec->unk8 = (u32)node;

    return (u8*)node + 0x20;
}

void fn_8009AAD4(u32 idx, void* ptr) {
    AlarmCallback* rec = &lbl_8047A6E8[idx];
    AlarmCallback* header = (AlarmCallback*)((u8*)ptr - 0x20);
    AlarmCallback* next = (AlarmCallback*)header->unk4;
    u32 newHead = rec->unk8;

    if (next != 0) {
        next->unk0 = header->unk0;
    }
    if ((u32)header->unk0 == 0) {
        newHead = header->unk4;
    } else {
        ((AlarmCallback*)header->unk0)->unk4 = header->unk4;
    }
    rec->unk8 = newHead;

    rec->unk4 = (u32)fn_8009A92C((AlarmCallback*)rec->unk4, header, (void*)newHead);
}

u32 fn_8009AB50(u32 xfb) {
    u32 previous = lbl_80478980;

    lbl_80478980 = xfb;
    return previous;
}

#pragma optimize_for_size on
void fn_8009AB60(AlarmCallback* arr, void* end, s32 count) {
    s32 i;
    u32 size;

    lbl_8047A6E8 = arr;
    lbl_8047A6EC = count;
    size = count * 0xC;
    for (i = 0; i < lbl_8047A6EC; i++) {
        AlarmCallback* p = &lbl_8047A6E8[i];
        p->unk0 = -1;
        p->unk8 = 0;
        p->unk4 = 0;
    }

    lbl_8047A6F4 = (u32)end & ~0x1F;
    lbl_80478980 = -1;
    lbl_8047A6F0 = ((u32)lbl_8047A6E8 + size + 0x1F) & ~0x1F;
}
#pragma optimize_for_size reset

s32 fn_8009ABD0(u32 start, u32 end) {
    s32 i;
    AlarmCallback* arr = lbl_8047A6E8;
    s32 count;
    u32 alignedStart = (start + 0x1F) & ~0x1F;
    u32 alignedEnd = end & ~0x1F;

    count = lbl_8047A6EC;
    for (i = 0; i < count; arr++, i++) {
        if (arr->unk0 < 0) {
            AlarmCallback* blk = (AlarmCallback*)alignedStart;

            arr->unk0 = alignedEnd - alignedStart;
            blk->unk0 = 0;
            blk->unk4 = 0;
            blk->unk8 = arr->unk0;
            arr->unk4 = alignedStart;
            arr->unk8 = 0;
            return i;
        }
    }
    return -1;
}

void fn_8009AC3C(u32 xfb) {
    lbl_8047A6E8[xfb].unk0 = -1;
}
#endif
