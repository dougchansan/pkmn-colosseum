/**
 * @file sdk_range_8009F77C.c
 * @brief dolphin-sdk code, 0x8009F77C - 0x8009FAEC (8 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef struct {
    void* head;
    void* tail;
} OSThreadQueue;

typedef struct {
    OSThreadQueue queue;
    void* unk_08;
    void* unk_0C;
} fn_8009F77C_Worker;

extern void OSInitThreadQueue(OSThreadQueue* queue);
extern void OSWakeupThread(void* queue);

#if defined(SDK_8009F77C_PREFIX_ACTIVE)
void OSInitMutex(fn_8009F77C_Worker* arg) {
    OSInitThreadQueue(&arg->queue);
    arg->unk_08 = 0;
    arg->unk_0C = 0;
}
#endif

#if defined(SDK_8009F9C8_SUFFIX_ACTIVE)
void OSInitCond(void* queue) {
    OSInitThreadQueue((OSThreadQueue*)queue);
}

void OSSignalCond(void* queue) {
    OSWakeupThread(queue);
}
#endif

/* --- OSMutex / OSCond family, 0x8009F7B4 - 0x8009FAEC --- */

typedef struct OSMutex OSMutex;
typedef struct OSCurThread OSCurThread;

struct OSMutex {
    OSThreadQueue queue; /* 0x00 */
    OSCurThread* thread; /* 0x08 */
    s32 count;           /* 0x0C */
    OSMutex* next;        /* 0x10 */
    OSMutex* prev;        /* 0x14 */
};

/* Partial current-thread view: only the fields this range touches. */
struct OSCurThread {
    u8 pad_000[0x2D0];
    s32 priority;               /* 0x2D0 */
    s32 base;                   /* 0x2D4 */
    u8 pad_2D8[0x2F0 - 0x2D8];
    OSMutex* mutex;             /* 0x2F0 */
    OSMutex* queueMutexHead;    /* 0x2F4 */
    OSMutex* queueMutexTail;    /* 0x2F8 */
};

extern BOOL OSDisableInterrupts(void);
extern void OSRestoreInterrupts(BOOL enabled);
extern s32 OSDisableScheduler(void);
extern s32 OSEnableScheduler(void);
extern void OSSleepThread(OSThreadQueue* queue);
extern s32 __OSGetEffectivePriority(OSCurThread* thread);
extern OSCurThread* OSGetCurrentThread(void);
extern void __OSPromoteThread(OSCurThread* thread, s32 priority);

#if defined(SDK_8009F77C_PREFIX_ACTIVE)
void OSLockMutex(OSMutex* mutex) {
    BOOL enabled;
    OSCurThread* current;
    OSCurThread* owner;

    enabled = OSDisableInterrupts();
    current = OSGetCurrentThread();

    for (;;) {
        OSMutex* tail;

        owner = mutex->thread;
        if (owner == NULL) {
            mutex->thread = current;
            mutex->count++;

            tail = current->queueMutexTail;
            if (tail == NULL) {
                current->queueMutexHead = mutex;
            } else {
                tail->next = mutex;
            }
            mutex->prev = tail;
            mutex->next = NULL;
            current->queueMutexTail = mutex;
            break;
        } else if (owner == current) {
            mutex->count++;
            break;
        } else {
            current->mutex = mutex;
            __OSPromoteThread(mutex->thread, current->priority);
            OSSleepThread(&mutex->queue);
            current->mutex = NULL;
        }
    }

    OSRestoreInterrupts(enabled);
}

void OSUnlockMutex(OSMutex* mutex) {
    BOOL enabled;
    OSCurThread* current;

    enabled = OSDisableInterrupts();
    current = OSGetCurrentThread();

    if (mutex->thread == current && --mutex->count == 0) {
        OSMutex* next = mutex->next;
        OSMutex* prev = mutex->prev;

        if (next == NULL) {
            current->queueMutexTail = prev;
        } else {
            next->prev = prev;
        }
        if (prev == NULL) {
            current->queueMutexHead = next;
        } else {
            prev->next = next;
        }

        mutex->thread = NULL;

        if (current->priority < current->base) {
            current->priority = __OSGetEffectivePriority(current);
        }

        OSWakeupThread(&mutex->queue);
    }

    OSRestoreInterrupts(enabled);
}
#endif

#if defined(SDK_EXACT_8009F958)
void __OSUnlockAllMutex(OSCurThread* thread) {
    OSMutex* mutex;

    while (thread->queueMutexHead) {
        OSMutex* next;

        mutex = thread->queueMutexHead;
        next = mutex->next;
        if (next == NULL) {
            thread->queueMutexTail = NULL;
        } else {
            next->prev = NULL;
        }
        thread->queueMutexHead = next;
        mutex->count = 0;
        mutex->thread = NULL;
        OSWakeupThread(&mutex->queue);
    }
}
#endif

#if defined(SDK_8009F9C8_SUFFIX_ACTIVE)
void OSWaitCond(OSThreadQueue* queue, OSMutex* mutex) {
    BOOL enabled;
    OSCurThread* current;
    s32 count;

    enabled = OSDisableInterrupts();
    current = OSGetCurrentThread();

    if (mutex->thread == current) {
        OSMutex* next;
        OSMutex* prev;

        count = mutex->count;
        mutex->count = 0;

        next = mutex->next;
        prev = mutex->prev;

        if (next == NULL) {
            current->queueMutexTail = prev;
        } else {
            next->prev = prev;
        }
        if (prev == NULL) {
            current->queueMutexHead = next;
        } else {
            prev->next = next;
        }

        mutex->thread = NULL;

        if (current->priority < current->base) {
            current->priority = __OSGetEffectivePriority(current);
        }

        OSDisableScheduler();
        OSWakeupThread(&mutex->queue);
        OSEnableScheduler();

        OSSleepThread(queue);
        OSLockMutex(mutex);
        mutex->count = count;
    }

    OSRestoreInterrupts(enabled);
}
#endif
