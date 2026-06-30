#include "dolphin/os/OSThread.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"

/*
 * OSThread.c - Thread management.
 *
 * Adapted from doldecomp/melee matching implementation.
 * Implements thread initialization, scheduling, and thread queue management.
 *
 * Matches: 0x800A1290 - 0x800A2774
 */

#define ENQUEUE_THREAD(thread, queue, link)       \
    do {                                          \
        OSThread* __prev = (queue)->tail;         \
        if (__prev == NULL) {                     \
            (queue)->head = (thread);             \
        } else {                                  \
            __prev->link.next = (thread);         \
        }                                         \
        (thread)->link.prev = __prev;             \
        (thread)->link.next = 0;                  \
        (queue)->tail = (thread);                 \
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

#define ENQUEUE_THREAD_PRIO(thread, queue, link)       \
    do {                                               \
        OSThread* __prev;                              \
        OSThread* __next;                              \
        for(__next = (queue)->head; __next             \
          && (__next->priority <= (thread)->priority); \
                __next = __next->link.next) ;          \
                                                       \
        if (__next == NULL) {                          \
            ENQUEUE_THREAD(thread, queue, link);       \
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

#define DEQUEUE_HEAD(thread, queue, link)             \
    do {                                              \
        OSThread* __next = thread->link.next;         \
        if (__next == NULL) {                         \
            (queue)->tail = 0;                        \
        } else {                                      \
            __next->link.prev = 0;                    \
        }                                             \
        (queue)->head = __next;                       \
    } while(0);

/* Linker-defined symbols */
extern u8 _stack_end[];
extern u8 _stack_addr[];

/* Memory-mapped pointers */
#define OS_CURRENT_FPU_CONTEXT  (*(OSContext* volatile*)0x800000D8)
#define OS_CURRENT_THREAD       (*(OSThread* volatile*)0x800000E4)

/* Global active thread queue at low memory (0x800000DC) */
static OSThreadQueue* __OSActiveThreadQueue = (OSThreadQueue*)0x800000DC;

/* .bss - names match assembly symbol table */
static OSThreadQueue RunQueue_803FB898[32];
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;
static volatile u32 RunQueueBits_8047A760;
static volatile int RunQueueHint_8047A764;
static s32 Reschedule_8047A768;

static OSSwitchThreadCallback SwitchThreadCallback_804789A8;

/* Forward declarations */
void OSInitThreadQueue(OSThreadQueue* queue);
static void __OSSwitchThread(OSThread* nextThread);
static void SetRun(OSThread* thread);
static void UnsetRun(OSThread* thread);
static OSThread* SetEffectivePriority(OSThread* thread, s32 priority);
static void UpdatePriority(OSThread* thread);
static OSThread* SelectThread(int yield);

/* External functions */
extern void __OSUnlockAllMutex(OSThread* thread);

void __OSThreadInit(void) {
    OSThread* thread = &DefaultThread;
    int prio;

    thread->state = 2;
    thread->attr = 1;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = (u32)-1;
    thread->mutex = NULL;

    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    OS_CURRENT_FPU_CONTEXT = &thread->context;
    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = (u32*)_stack_addr;
    thread->stackEnd = (u32*)_stack_end;
    *(u32*)thread->stackEnd = 0xDEADBABE;

    SwitchThreadCallback_804789A8(OS_CURRENT_THREAD, thread);
    OS_CURRENT_THREAD = thread;
    OSClearStack(0);

    RunQueueBits_8047A760 = 0;
    RunQueueHint_8047A764 = 0;

    for (prio = 0; prio <= 31; prio++) {
        OSInitThreadQueue(&RunQueue_803FB898[prio]);
    }
    OSInitThreadQueue(__OSActiveThreadQueue);

    ENQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);

    OSClearContext(&IdleContext);
    Reschedule_8047A768 = 0;
}

void OSInitThreadQueue(OSThreadQueue* queue) {
    queue->head = queue->tail = NULL;
}

#if 0
asm void fn_800A13F8(void) {
#include "src/dolphin/os/OSThread_fn_800A13F8.inc"
}
#else
OSThread* fn_800A13F8(void) {
    return OS_CURRENT_THREAD;
}
#endif

OSThread* OSGetCurrentThread(void) {
    return OS_CURRENT_THREAD;
}

static void __OSSwitchThread(OSThread* nextThread) {
    OS_CURRENT_THREAD = nextThread;
    OSSetCurrentContext(&nextThread->context);
    OSLoadContext(&nextThread->context);
}

BOOL OSIsThreadSuspended(OSThread* thread) {
    if (thread->suspend > 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL OSIsThreadTerminated(OSThread* thread) {
    return (thread->state == 8 || thread->state == 0) ? TRUE : FALSE;
}

static int __OSIsThreadActive(OSThread* thread) {
    OSThread* active;

    if (thread->state == 0) {
        return 0;
    }

    for (active = __OSActiveThreadQueue->head; active; active = active->linkActive.next) {
        if (thread == active) {
            return 1;
        }
    }
    return 0;
}

s32 OSDisableScheduler(void) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule_8047A768;
    Reschedule_8047A768 = count + 1;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler(void) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule_8047A768;
    Reschedule_8047A768 = count - 1;
    OSRestoreInterrupts(enabled);
    return count;
}

static void SetRun(OSThread* thread) {
    thread->queue = &RunQueue_803FB898[thread->priority];
    ENQUEUE_THREAD(thread, thread->queue, link);
    RunQueueBits_8047A760 |= 1 << (31 - thread->priority);
    RunQueueHint_8047A764 = 1;
}

static void UnsetRun(OSThread* thread) {
    OSThreadQueue* queue;

    queue = thread->queue;
    DEQUEUE_THREAD(thread, queue, link);

    if (!queue->head) {
        RunQueueBits_8047A760 &= ~(1 << (31 - thread->priority));
    }
    thread->queue = NULL;
}

s32 __OSGetEffectivePriority(OSThread* thread) {
    s32 priority = thread->base;
    OSMutex* mutex;

    for (mutex = thread->queueMutex.head; mutex; mutex = mutex->link.next) {
        OSThread* blocked = mutex->queue.head;
        if (blocked && blocked->priority < priority) {
            priority = blocked->priority;
        }
    }
    return priority;
}

static OSThread* SetEffectivePriority(OSThread* thread, s32 priority) {
    switch (thread->state) {
        case 1:
            UnsetRun(thread);
            thread->priority = priority;
            SetRun(thread);
            break;
        case 4:
            DEQUEUE_THREAD(thread, thread->queue, link);
            thread->priority = priority;
            ENQUEUE_THREAD_PRIO(thread, thread->queue, link);
            if (thread->mutex) {
                return thread->mutex->thread;
            }
            break;
        case 2:
            RunQueueHint_8047A764 = 1;
            thread->priority = priority;
            break;
    }
    return NULL;
}

static void UpdatePriority(OSThread* thread) {
    s32 priority;

    while (1) {
        if (thread->suspend > 0) {
            break;
        }
        priority = __OSGetEffectivePriority(thread);
        if (thread->priority == priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
        if (thread == NULL) {
            break;
        }
    }
}

void __OSPromoteThread(OSThread* thread, s32 priority) {
    while (1) {
        if (thread->suspend > 0 || thread->priority <= priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
        if (thread == NULL) {
            break;
        }
    }
}

static OSThread* SelectThread(int yield) {
    OSContext* currentContext;
    OSThread* currentThread;
    OSThread* nextThread;
    s32 priority;
    OSThreadQueue* queue;

    if (Reschedule_8047A768 > 0) {
        return NULL;
    }

    currentContext = OSGetCurrentContext();
    currentThread = OSGetCurrentThread();

    if (currentContext != &currentThread->context) {
        return NULL;
    }

    if (currentThread) {
        if (currentThread->state == 2) {
            if (yield == 0) {
                priority = __cntlzw(RunQueueBits_8047A760);
                if (currentThread->priority <= priority) {
                    return NULL;
                }
            }
            currentThread->state = 1;
            SetRun(currentThread);
        }
        if (!(currentThread->context.state & 2) && (OSSaveContext(&currentThread->context) != 0)) {
            return NULL;
        }
    }

    OS_CURRENT_THREAD = NULL;

    if (RunQueueBits_8047A760 == 0) {
        OSSetCurrentContext(&IdleContext);
        do {
            OSEnableInterrupts();
            while (RunQueueBits_8047A760 == 0) ;
            OSDisableInterrupts();
        } while (RunQueueBits_8047A760 == 0);
        OSClearContext(&IdleContext);
    }

    RunQueueHint_8047A764 = 0;
    priority = __cntlzw(RunQueueBits_8047A760);

    queue = &RunQueue_803FB898[priority];
    nextThread = queue->head;

    DEQUEUE_HEAD(nextThread, queue, link);

    if (!queue->head) {
        RunQueueBits_8047A760 &= ~(1 << (31 - priority));
    }
    nextThread->queue = NULL;
    nextThread->state = 2;
    __OSSwitchThread(nextThread);
    return nextThread;
}

void __OSReschedule(void) {
    if (RunQueueHint_8047A764 != 0) {
        SelectThread(0);
    }
}

#if 0
asm void fn_800A1990(void) {
#include "src/dolphin/os/OSThread_fn_800A1990.inc"
}
#else
void fn_800A1990(void) {
    BOOL enabled = OSDisableInterrupts();
    SelectThread(1);
    OSRestoreInterrupts(enabled);
}
#endif
extern void fn_8009BD84(void);
extern OSErrorHandler __OSErrorTable[];
extern s32 lbl_80478990;
extern void fn_800A1BB4(void);
#if 1
asm void fn_800A19CC(void) {
#include "src/dolphin/os/OSThread_fn_800A19CC.inc"
}
#else
void fn_800A19CC(void) { /* TODO */ }
#endif

void OSYieldThread(void) {
    BOOL enabled = OSDisableInterrupts();

    SelectThread(1);
    OSRestoreInterrupts(enabled);
}

BOOL OSCreateThread(OSThread* thread, void* (*func)(void*), void* param,
                    void* stack, u32 stackSize, s32 priority, u16 attr) {
    BOOL enabled;
    u32 sp;

    if ((priority < 0) || (priority > 31)) {
        return FALSE;
    }

    thread->state = 1;
    thread->attr = attr & 1;
    thread->base = priority;
    thread->priority = priority;
    thread->suspend = 1;
    thread->val = (u32)-1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;
    sp = (u32)stack;
    sp &= ~7;
    sp -= 8;
    ((u32*)sp)[0] = 0;
    ((u32*)sp)[1] = 0;
    OSInitContext(&thread->context, (u32)func, sp);
    thread->context.lr = (u32)OSExitThread;
    thread->context.gpr[3] = (u32)param;
    thread->stackBase = (u32*)stack;
    thread->stackEnd = (u32*)((u32)stack - stackSize);
    *thread->stackEnd = 0xDEADBABE;

    enabled = OSDisableInterrupts();
    ENQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void OSExitThread(void* val) {
    BOOL enabled = OSDisableInterrupts();
    OSThread* currentThread = OSGetCurrentThread();

    OSClearContext(&currentThread->context);
    if (currentThread->attr & 1) {
        DEQUEUE_THREAD(currentThread, __OSActiveThreadQueue, linkActive);
        currentThread->state = 0;
    } else {
        currentThread->state = 8;
        currentThread->val = (u32)val;
    }
    __OSUnlockAllMutex(currentThread);
    OSWakeupThread(&currentThread->queueJoin);
    RunQueueHint_8047A764 = 1;
    if (RunQueueHint_8047A764 != 0) {
        SelectThread(0);
    }
    OSRestoreInterrupts(enabled);
}

void OSCancelThread(OSThread* thread) {
    BOOL enabled = OSDisableInterrupts();

    switch (thread->state) {
        case 1:
            if (thread->suspend <= 0) {
                UnsetRun(thread);
            }
            break;
        case 2:
            RunQueueHint_8047A764 = 1;
            break;
        case 4:
            DEQUEUE_THREAD(thread, thread->queue, link);
            thread->queue = NULL;
            if ((thread->suspend <= 0) && (thread->mutex)) {
                UpdatePriority(thread->mutex->thread);
            }
            break;
        default:
            OSRestoreInterrupts(enabled);
            return;
    }
    OSClearContext(&thread->context);
    if (thread->attr & 1) {
        DEQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);
        thread->state = 0;
    } else {
        thread->state = 8;
    }
    __OSUnlockAllMutex(thread);
    OSWakeupThread(&thread->queueJoin);
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

extern void fn_800A238C(OSThreadQueue* queue);
extern OSThread* fn_800A1528(OSThread* thread, s32 priority);
extern s32 fn_800A14EC(OSThread* thread);
extern void fn_800A1484(OSThread* thread);
#if 1
BOOL fn_800A1E54(OSThread* thread, void* val) {
#pragma peephole on
    extern void fn_800A238C(OSThreadQueue* queue);
    BOOL enabled;
    OSThread* active;
    s32 isActive;

    enabled = OSDisableInterrupts();
    if (!(thread->attr & 1) && thread->state != 8 && thread->queueJoin.head == NULL) {
        fn_800A238C(&thread->queueJoin);
        if (thread->state == 0) {
            isActive = 0;
        } else {
            for (active = ((OSThreadQueue*)0x800000DC)->head; active; active = active->linkActive.next) {
                if (thread == active) { isActive = 1; goto chk; }
            }
            isActive = 0;
          chk:;
        }
        if (isActive == 0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
    }
    if (thread->state == 8) {
        if (val) {
            *(s32*)val = (s32)thread->val;
        }
        DEQUEUE_THREAD(thread, (OSThreadQueue*)0x800000DC, linkActive);
        thread->state = 0;
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}
#else
void fn_800A1E54(void) { /* TODO */ }
#endif
#if 1
asm void fn_800A1F94(void) {
#include "src/dolphin/os/OSThread_fn_800A1F94.inc"
}
#else
void fn_800A1F94(void) { /* TODO */ }
#endif
#if 1
s32 fn_800A221C(OSThread* thread) {
#pragma peephole on
    extern void fn_800A1484(OSThread* thread);
    extern s32 fn_800A14EC(OSThread* thread);
    extern OSThread* fn_800A1528(OSThread* thread, s32 priority);
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend++;
    if (suspendCount == 0) {
        switch (thread->state) {
            case 2:
                RunQueueHint_8047A764 = 1;
                thread->state = 1;
                break;
            case 1:
                fn_800A1484(thread);
                break;
            case 4: {
                OSThread* t;
                DEQUEUE_THREAD(thread, thread->queue, link);
                thread->priority = 32;
                ENQUEUE_THREAD(thread, thread->queue, link);
                if (thread->mutex) {
                    s32 eff;
                    t = thread->mutex->thread;
                    do {
                        if (t->suspend > 0) break;
                        eff = fn_800A14EC(t);
                        if (t->priority == eff) break;
                        t = fn_800A1528(t, eff);
                    } while (t != NULL);
                }
                break;
            }
        }
        if (RunQueueHint_8047A764 != 0) {
            SelectThread(0);
        }
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}
#else
void fn_800A221C(void) { /* TODO */ }
#endif
#if 0
asm void fn_800A238C(void) {
#include "src/dolphin/os/OSThread_fn_800A238C.inc"
}
#else
void fn_800A238C(OSThreadQueue* queue) {
    BOOL enabled = OSDisableInterrupts();
    OSThread* currentThread = OS_CURRENT_THREAD;

    currentThread->state = 4;
    currentThread->queue = queue;
    ENQUEUE_THREAD_PRIO(currentThread, queue, link);
    RunQueueHint_8047A764 = 1;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}
#endif
#if 0
asm void fn_800A2478(void) {
#include "src/dolphin/os/OSThread_fn_800A2478.inc"
}
#else
void fn_800A2478(OSThreadQueue* queue) {
    BOOL enabled;
    OSThread* thread;
    OSThread* next;
    OSThreadQueue* runQueues;
    OSThread* tail;

    enabled = OSDisableInterrupts();
    runQueues = RunQueue_803FB898;

    while ((thread = queue->head) != NULL) {
        next = thread->link.next;
        if (next == NULL) {
            queue->tail = NULL;
        } else {
            next->link.prev = NULL;
        }
        queue->head = next;

        thread->state = 1;
        if (thread->suspend <= 0) {
            thread->queue = &runQueues[thread->priority];
            tail = thread->queue->tail;
            if (tail == NULL) {
                thread->queue->head = thread;
            } else {
                tail->link.next = thread;
            }
            thread->link.prev = tail;
            thread->link.next = NULL;
            thread->queue->tail = thread;
            RunQueueBits_8047A760 |= 1 << (31 - thread->priority);
            RunQueueHint_8047A764 = 1;
        }
    }
    if (RunQueueHint_8047A764 != 0) {
        SelectThread(0);
    }
    OSRestoreInterrupts(enabled);
}
#endif
#if 1
BOOL fn_800A257C(OSThread* thread, s32 priority) {
#pragma peephole on
    extern s32 fn_800A14EC(OSThread* thread);
    extern OSThread* fn_800A1528(OSThread* thread, s32 priority);
    OSThread* t;
    BOOL enabled;
    s32 eff;

    if ((priority < 0) || (priority > 31)) {
        return FALSE;
    }
    enabled = OSDisableInterrupts();

    if (thread->base != priority) {
        thread->base = priority;
        t = thread;
        do {
            if (t->suspend > 0) break;
            eff = fn_800A14EC(t);
            if (t->priority == eff) break;
            t = fn_800A1528(t, eff);
        } while (t != NULL);
        if (RunQueueHint_8047A764 != 0) {
            SelectThread(0);
        }
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}
#else
BOOL fn_800A257C(OSThread* thread, s32 priority) {
    BOOL enabled;

    if ((priority < 0) || (priority > 31)) {
        return FALSE;
    }
    enabled = OSDisableInterrupts();

    if (thread->base != priority) {
        thread->base = priority;
        UpdatePriority(thread);
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}
#endif
#if 1
OSThread* fn_800A263C(void (*idleFunction)(void*), void* param, void* stack, u32 stackSize) {
#pragma peephole on
#pragma opt_common_subs off
    extern BOOL fn_800A19CC(OSThread*, void* (*)(void*), void*, void*, u32, s32, u16);
    extern s32 fn_800A1F94(OSThread*);
    OSThreadQueue* rq = RunQueue_803FB898;
#define IDLE (*(OSThread*)((u8*)rq + 0x100))
    if (idleFunction != NULL) {
        if (IDLE.state == 0) {
            fn_800A19CC(&IDLE, (void* (*)(void*))idleFunction, param, stack, stackSize, 31, 1);
            fn_800A1F94(&IDLE);
            return &IDLE;
        }
    } else if (IDLE.state != 0) {
        OSCancelThread(&IDLE);
    }
    return NULL;
#undef IDLE
}
#else
void fn_800A263C(void) { /* TODO */ }
#endif

BOOL OSJoinThread(OSThread* thread, void* val) {
    BOOL enabled = OSDisableInterrupts();

    if (!(thread->attr & 1) && (thread->state != 8) && (thread->queueJoin.head == NULL)) {
        OSSleepThread(&thread->queueJoin);
        if (__OSIsThreadActive(thread) == 0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
    }
    if (thread->state == 8) {
        if (val) {
            *(s32*)val = (s32)thread->val;
        }
        DEQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);
        thread->state = 0;
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

void OSDetachThread(OSThread* thread) {
    BOOL enabled = OSDisableInterrupts();

    thread->attr |= 1;
    if (thread->state == 8) {
        DEQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);
        thread->state = 0;
    }
    OSWakeupThread(&thread->queueJoin);
    OSRestoreInterrupts(enabled);
}

s32 OSResumeThread(OSThread* thread) {
    BOOL enabled = OSDisableInterrupts();
    s32 suspendCount;

    suspendCount = thread->suspend--;
    if (thread->suspend < 0) {
        thread->suspend = 0;
    } else if (thread->suspend == 0) {
        switch (thread->state) {
            case 1:
                thread->priority = __OSGetEffectivePriority(thread);
                SetRun(thread);
                break;
            case 4:
                DEQUEUE_THREAD(thread, thread->queue, link);
                thread->priority = __OSGetEffectivePriority(thread);
                ENQUEUE_THREAD_PRIO(thread, thread->queue, link);
                if (thread->mutex) {
                    UpdatePriority(thread->mutex->thread);
                }
                break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

s32 OSSuspendThread(OSThread* thread) {
    BOOL enabled = OSDisableInterrupts();
    s32 suspendCount;

    suspendCount = thread->suspend++;
    if (suspendCount == 0) {
        switch (thread->state) {
            case 2:
                RunQueueHint_8047A764 = 1;
                thread->state = 1;
                break;
            case 1:
                UnsetRun(thread);
                break;
            case 4:
                DEQUEUE_THREAD(thread, thread->queue, link);
                thread->priority = 32;
                ENQUEUE_THREAD(thread, thread->queue, link);
                if (thread->mutex) {
                    UpdatePriority(thread->mutex->thread);
                }
                break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

void OSSleepThread(OSThreadQueue* queue) {
    BOOL enabled = OSDisableInterrupts();
    OSThread* currentThread = OSGetCurrentThread();

    currentThread->state = 4;
    currentThread->queue = queue;
    ENQUEUE_THREAD_PRIO(currentThread, queue, link);
    RunQueueHint_8047A764 = 1;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

void OSWakeupThread(OSThreadQueue* queue) {
    BOOL enabled = OSDisableInterrupts();

    while (queue->head) {
        OSThread* thread = queue->head;

        DEQUEUE_HEAD(thread, queue, link);

        thread->state = 1;
        if (thread->suspend <= 0) {
            SetRun(thread);
        }
    }
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

BOOL OSSetThreadPriority(OSThread* thread, s32 priority) {
    BOOL enabled;

    if ((priority < 0) || (priority > 31)) {
        return FALSE;
    }
    enabled = OSDisableInterrupts();

    if (thread->base != priority) {
        thread->base = priority;
        UpdatePriority(thread);
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

s32 OSGetThreadPriority(OSThread* thread) {
    return thread->base;
}

OSThread* OSSetIdleFunction(void (*idleFunction)(void*), void* param,
                            void* stack, u32 stackSize) {
    if (idleFunction) {
        if (IdleThread.state == 0) {
            OSCreateThread(&IdleThread, (void* (*)(void*))idleFunction, param,
                           stack, stackSize, 31, 1);
            OSResumeThread(&IdleThread);
            return &IdleThread;
        }
    } else if (IdleThread.state != 0) {
        OSCancelThread(&IdleThread);
    }
    return NULL;
}

OSThread* OSGetIdleFunction(void) {
    if (IdleThread.state != 0) {
        return &IdleThread;
    }
    return NULL;
}

void OSClearStack(u8 val) {
    u32 sp;
    u32* p;
    u32 pattern;
    u32 count;

    pattern = (val << 24) | (val << 16) | (val << 8) | val;
    sp = OSGetStackPointer();
    p = OS_CURRENT_THREAD->stackEnd + 1;

    if ((u32)p >= sp) {
        return;
    }

    count = (sp - (u32)p + 3) / 4;

    while (count-- > 0) {
        *p++ = pattern;
    }
}

static int CheckThreadQueue(OSThreadQueue* queue) {
    OSThread* thread;

    if ((queue->head != NULL) && (queue->head->link.prev != NULL)) {
        return 0;
    }
    if ((queue->tail != NULL) && (queue->tail->link.next != NULL)) {
        return 0;
    }
    thread = queue->head;
    while (thread) {
        if ((thread->link.next != NULL) && (thread != thread->link.next->link.prev)) {
            return 0;
        }
        if ((thread->link.prev != NULL) && (thread != thread->link.prev->link.next)) {
            return 0;
        }
        thread = thread->link.next;
    }
    return 1;
}

static int IsMember(OSThreadQueue* queue, OSThread* thread) {
    OSThread* member = queue->head;

    while (member) {
        if (thread == member) {
            return 1;
        }
        member = member->link.next;
    }
    return 0;
}

s32 OSCheckActiveThreads(void) {
    OSThread* thread;
    s32 prio;
    s32 cThread;
    BOOL enabled;

    cThread = 0;
    enabled = OSDisableInterrupts();

    for (prio = 0; prio <= 31; prio++) {
        if (RunQueueBits_8047A760 & (1 << (31 - prio))) {
        } else {
        }
    }

    thread = __OSActiveThreadQueue->head;
    while (thread) {
        cThread++;
        switch (thread->state) {
            case 1:
                break;
            case 2:
                break;
            case 4:
                break;
            case 8:
                break;
        }
        thread = thread->linkActive.next;
    }
    OSRestoreInterrupts(enabled);
    return cThread;
}

#if 0
asm void fn_800A1484(void) {
#include "src/dolphin/os/OSThread_fn_800A1484.inc"
}
#else
void fn_800A1484(OSThread* thread) {
    OSThread* next;
    OSThreadQueue* queue;
    OSThread* prev;

    next = thread->link.next;
    queue = thread->queue;
    prev = thread->link.prev;

    if (next == NULL) {
        queue->tail = prev;
    } else {
        next->link.prev = prev;
    }

    if (prev == NULL) {
        queue->head = next;
    } else {
        prev->link.next = next;
    }

    if (!queue->head) {
        u32 bit = 1 << (31 - thread->priority);
        RunQueueBits_8047A760 = RunQueueBits_8047A760 & ~bit;
    }
    thread->queue = NULL;
}
#endif
#if 0
asm void fn_800A14EC(void) {
#include "src/dolphin/os/OSThread_fn_800A14EC.inc"
}
#else
s32 fn_800A14EC(OSThread* thread) {
    s32 priority;
    OSMutex* mutex;

    priority = thread->base;
    mutex = thread->queueMutex.head;
    while (mutex != NULL) {
        OSThread* blocked = mutex->queue.head;
        if (blocked != NULL) {
            if (blocked->priority < priority) {
                priority = blocked->priority;
            }
        }
        mutex = mutex->link.next;
    }
    return priority;
}
#endif
#if 1
asm OSThread* fn_800A1528(OSThread* thread, s32 priority) {
#include "src/dolphin/os/OSThread_fn_800A1528.inc"
}
#else
OSThread* fn_800A1528(OSThread* thread, s32 priority) {
    /* TODO: match -- SetEffectivePriority duplicate at 0x800A1528 */
    return NULL;
}
#endif
#pragma push
#pragma optimization_level 2
#if 1
void fn_800A16E8(OSThread* thread, s32 priority) {
loop:
    if (thread->suspend > 0) return;
    if (thread->priority <= priority) return;
    thread = fn_800A1528(thread, priority);
    if (thread != NULL) goto loop;
}
#else
void fn_800A16E8(OSThread* thread, s32 priority) {
    /* TODO: match -- __OSPromoteThread duplicate at 0x800A16E8 */
}
#endif
#pragma pop
extern void fn_8009F958(OSThread* thread);
#if 1
asm void fn_800A1BB4(void) {
#include "src/dolphin/os/OSThread_fn_800A1BB4.inc"
}
#else
void fn_800A1BB4(void* val) {
    BOOL enabled;
    OSThread* thread;

    enabled = OSDisableInterrupts();
    thread = OS_CURRENT_THREAD;

    OSClearContext(&thread->context);
    if (thread->attr & 1) {
        DEQUEUE_THREAD(thread, __OSActiveThreadQueue, linkActive);
        thread->state = 0;
    } else {
        thread->state = 8;
        thread->val = (u32)val;
    }
    fn_8009F958(thread);
    fn_800A2478(&thread->queueJoin);
    RunQueueHint_8047A764 = 1;
    if (RunQueueHint_8047A764 != 0) {
        SelectThread(0);
    }
    OSRestoreInterrupts(enabled);
}
#endif
