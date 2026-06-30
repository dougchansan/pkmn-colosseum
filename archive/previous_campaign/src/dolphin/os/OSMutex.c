#include "dolphin/os/OSThread.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * OSMutex.c - Mutual exclusion (mutex) implementation.
 *
 * Adapted from doldecomp/melee matching implementation.
 * Implements priority inheritance mutexes for the Dolphin OS.
 *
 * Matches: 0x8009A92C - 0x8009AFB0
 *   fn_8009A92C (0xAC) - OSInitMutex
 *   fn_8009A9D8 (0xFC) - OSLockMutex
 *   fn_8009AAD4 (0x7C) - OSUnlockMutex
 *   fn_8009AB50 (0x10) - OSTryLockMutex (stub-like)
 *   fn_8009AB60 (0x70) - __OSUnlockAllMutex
 *   fn_8009ABD0 (0x6C) - (internal: CheckMutexQueue)
 *   fn_8009AC3C (0x14) - (internal: CheckDeadLock)
 *   fn_8009AC50 (0x360) - OSInitCond / OSWaitCond / OSSignalCond
 */

#define OS_CURRENT_THREAD (*(OSThread* volatile*)0x800000E4)

/* Internal thread queue manipulation macros - same as OSThread.c */
#define ENQUEUE_THREAD_PRIO(thread, queue, link)       \
    do {                                               \
        OSThread* __prev;                              \
        OSThread* __next;                              \
        for(__next = (queue)->head; __next             \
          && (__next->priority <= (thread)->priority); \
                __next = __next->link.next) ;          \
                                                       \
        if (__next == NULL) {                          \
            OSThread* __tail = (queue)->tail;           \
            if (__tail == NULL) {                      \
                (queue)->head = (thread);              \
            } else {                                   \
                __tail->link.next = (thread);          \
            }                                          \
            (thread)->link.prev = __tail;              \
            (thread)->link.next = 0;                   \
            (queue)->tail = (thread);                  \
        } else {                                       \
            (thread)->link.next = __next;              \
            __prev = __next->link.prev;                \
            __next->link.prev = (thread);              \
            (thread)->link.prev = __prev;              \
            if (__prev == NULL) {                      \
                (queue)->head = (thread);              \
            } else {                                   \
                __prev->link.next = (thread);          \
            }                                          \
        }                                              \
    } while(0);

#define DEQUEUE_THREAD(thread, queue, link)             \
    do {                                                \
        OSThread* __next = (thread)->link.next;         \
        OSThread* __prev = (thread)->link.prev;         \
        if (__next == NULL) {                           \
            (queue)->tail = __prev;                     \
        } else {                                        \
            __next->link.prev = __prev;                 \
        }                                               \
        if (__prev == NULL) {                           \
            (queue)->head = __next;                     \
        } else {                                        \
            __prev->link.next = __next;                 \
        }                                               \
    } while(0);

/*
 * OSInitMutex - Initialize a mutex.
 * 0x8009A92C | size: 0xAC
 */
void OSInitMutex(OSMutex* mutex) {
    OSInitThreadQueue(&mutex->queue);
    mutex->thread = NULL;
    mutex->count = 0;
    mutex->link.next = NULL;
    mutex->link.prev = NULL;
}

/*
 * OSLockMutex - Acquire a mutex, blocking if necessary.
 * 0x8009A9D8 | size: 0xFC
 *
 * If the mutex is already held by the current thread, increments the
 * lock count (recursive locking). If held by another thread, the
 * current thread sleeps on the mutex's thread queue with priority
 * inheritance.
 */
void OSLockMutex(OSMutex* mutex) {
    BOOL enabled;
    OSThread* currentThread;

    enabled = OSDisableInterrupts();
    currentThread = OS_CURRENT_THREAD;

    while (TRUE) {
        if (mutex->thread == NULL) {
            /* Mutex is free; acquire it */
            OSMutexQueue* mq = &currentThread->queueMutex;

            /* Add to thread's owned mutex list */
            mutex->link.prev = mq->tail;
            if (mq->tail != NULL) {
                mq->tail->link.next = mutex;
            } else {
                mq->head = mutex;
            }
            mutex->link.next = NULL;
            mq->tail = mutex;

            mutex->thread = currentThread;
            mutex->count++;
            break;
        } else if (mutex->thread == currentThread) {
            /* Already own it; increment count */
            mutex->count++;
            break;
        } else {
            /* Blocked: priority inheritance */
            currentThread->state = 4;
            currentThread->mutex = mutex;
            currentThread->queue = &mutex->queue;
            ENQUEUE_THREAD_PRIO(currentThread, &mutex->queue, link);
            __OSPromoteThread(mutex->thread, currentThread->priority);
            __OSReschedule();
            currentThread->mutex = NULL;
        }
    }

    OSRestoreInterrupts(enabled);
}

/*
 * OSUnlockMutex - Release a mutex.
 * 0x8009AAD4 | size: 0x7C
 *
 * Decrements the lock count. When it reaches zero, the mutex is
 * released and threads waiting on it are woken.
 */
void OSUnlockMutex(OSMutex* mutex) {
    BOOL enabled;
    OSThread* currentThread;

    enabled = OSDisableInterrupts();
    currentThread = OS_CURRENT_THREAD;

    if (mutex->thread == currentThread && --mutex->count == 0) {
        /* Remove from thread's owned mutex list */
        OSMutexQueue* mq = &currentThread->queueMutex;

        if (mutex->link.next != NULL) {
            mutex->link.next->link.prev = mutex->link.prev;
        } else {
            mq->tail = mutex->link.prev;
        }
        if (mutex->link.prev != NULL) {
            mutex->link.prev->link.next = mutex->link.next;
        } else {
            mq->head = mutex->link.next;
        }

        mutex->thread = NULL;

        /* Recalculate priority after releasing mutex */
        {
            s32 priority = __OSGetEffectivePriority(currentThread);
            if (currentThread->priority != priority) {
                currentThread->priority = priority;
            }
        }

        /* Wake up the highest-priority waiter */
        OSWakeupThread(&mutex->queue);
    }

    OSRestoreInterrupts(enabled);
}

/*
 * OSTryLockMutex - Try to acquire a mutex without blocking.
 * 0x8009AB50 | size: 0x10
 *
 * Returns TRUE if the mutex was acquired, FALSE if it was already held
 * by another thread.
 */
BOOL OSTryLockMutex(OSMutex* mutex) {
    BOOL enabled;
    BOOL result = FALSE;
    OSThread* currentThread;

    enabled = OSDisableInterrupts();
    currentThread = OS_CURRENT_THREAD;

    if (mutex->thread == NULL) {
        OSMutexQueue* mq = &currentThread->queueMutex;

        mutex->link.prev = mq->tail;
        if (mq->tail != NULL) {
            mq->tail->link.next = mutex;
        } else {
            mq->head = mutex;
        }
        mutex->link.next = NULL;
        mq->tail = mutex;

        mutex->thread = currentThread;
        mutex->count++;
        result = TRUE;
    } else if (mutex->thread == currentThread) {
        mutex->count++;
        result = TRUE;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * __OSUnlockAllMutex - Release all mutexes held by a thread.
 * 0x8009AB60 | size: 0x70
 *
 * Called when a thread is canceled or exits to release all its mutexes.
 */
void __OSUnlockAllMutex(OSThread* thread) {
    OSMutex* mutex;

    while ((mutex = thread->queueMutex.head) != NULL) {
        /* Remove from thread's mutex list */
        thread->queueMutex.head = mutex->link.next;
        if (mutex->link.next != NULL) {
            mutex->link.next->link.prev = NULL;
        } else {
            thread->queueMutex.tail = NULL;
        }

        /* Release the mutex */
        mutex->thread = NULL;
        mutex->count = 0;

        /* Wake up waiters */
        OSWakeupThread(&mutex->queue);
    }
}

/*
 * OSInitCond - Initialize a condition variable.
 * Part of fn_8009AC50 (0x360)
 *
 * Condition variables allow threads to wait for a condition to become
 * true while holding a mutex.
 */
typedef struct OSCond {
    OSThreadQueue queue;
} OSCond;

void OSInitCond(OSCond* cond) {
    OSInitThreadQueue(&cond->queue);
}

/*
 * OSWaitCond - Wait on a condition variable.
 *
 * Atomically releases the mutex and sleeps on the condition's queue.
 * When woken, re-acquires the mutex before returning.
 */
void OSWaitCond(OSCond* cond, OSMutex* mutex) {
    BOOL enabled;
    OSThread* currentThread;
    s32 count;

    enabled = OSDisableInterrupts();
    currentThread = OS_CURRENT_THREAD;

    if (mutex->thread == currentThread) {
        count = mutex->count;
        mutex->count = 0;

        /* Remove mutex from thread's list */
        {
            OSMutexQueue* mq = &currentThread->queueMutex;

            if (mutex->link.next != NULL) {
                mutex->link.next->link.prev = mutex->link.prev;
            } else {
                mq->tail = mutex->link.prev;
            }
            if (mutex->link.prev != NULL) {
                mutex->link.prev->link.next = mutex->link.next;
            } else {
                mq->head = mutex->link.next;
            }
        }

        mutex->thread = NULL;

        /* Recalculate priority */
        {
            s32 priority = __OSGetEffectivePriority(currentThread);
            if (currentThread->priority != priority) {
                currentThread->priority = priority;
            }
        }

        /* Wake any threads waiting on the mutex */
        OSWakeupThread(&mutex->queue);

        /* Sleep on the condition variable */
        OSSleepThread(&cond->queue);

        /* Re-acquire the mutex */
        OSLockMutex(mutex);
        mutex->count = count;
    }

    OSRestoreInterrupts(enabled);
}

/*
 * OSSignalCond - Signal a condition variable.
 *
 * Wakes up one or all threads waiting on the condition variable.
 */
void OSSignalCond(OSCond* cond) {
    OSWakeupThread(&cond->queue);
}
