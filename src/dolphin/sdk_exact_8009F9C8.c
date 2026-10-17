/* Canonical Dolphin condition-variable helpers in retail function order. */
#include "dolphin/types.h"

typedef struct OSThreadQueue {
    void* head;
    void* tail;
} OSThreadQueue;

typedef struct OSMutex OSMutex;
typedef struct OSCurThread OSCurThread;

struct OSMutex {
    OSThreadQueue queue;
    OSCurThread* thread;
    s32 count;
    OSMutex* next;
    OSMutex* prev;
};

struct OSCurThread {
    u8 pad_000[0x2D0];
    s32 priority;
    s32 base;
    u8 pad_2D8[0x2F0 - 0x2D8];
    OSMutex* mutex;
    OSMutex* queueMutexHead;
    OSMutex* queueMutexTail;
};

extern void OSInitThreadQueue(OSThreadQueue* queue);
extern void OSWakeupThread(void* queue);
extern BOOL OSDisableInterrupts(void);
extern void OSRestoreInterrupts(BOOL enabled);
extern s32 OSDisableScheduler(void);
extern s32 OSEnableScheduler(void);
extern void OSSleepThread(OSThreadQueue* queue);
extern s32 __OSGetEffectivePriority(OSCurThread* thread);
extern OSCurThread* OSGetCurrentThread(void);
extern void OSLockMutex(OSMutex* mutex);

void OSInitCond(void* queue)
{
    OSInitThreadQueue((OSThreadQueue*) queue);
}

void OSWaitCond(OSThreadQueue* queue, OSMutex* mutex)
{
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

void OSSignalCond(void* queue)
{
    OSWakeupThread(queue);
}
