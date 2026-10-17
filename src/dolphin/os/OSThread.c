/**
 * @file OSThread.c
 * @brief Dolphin SDK OSThread.c, 0x800A128C - 0x800A2778, linked as one TU
 *        with its data: .bss RunQueue/IdleThread/DefaultThread/IdleContext
 *        (0x803FB898-0x803FC290), .sbss RunQueueBits/RunQueueHint/Reschedule
 *        (0x8047A760-0x8047A76C) and .sdata SwitchThreadCallback (0x804789A8).
 *
 * __OSThreadInit, SelectThread and OSCreateThread follow the SDK's
 * OSThread.c as decompiled in XD:
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/os/OSThread.c
 */
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

OSThreadQueue __OSActiveThreadQueue : (0x800000DC);
OSThread* __OSCurrentThread : (0x800000E4);
OSThread* __gUnkThread1 : (0x800000D8);
extern u8 _stack_end[];
extern u8 _stack_addr[];
#define OSCurrentThread __OSCurrentThread

extern OSErrorHandler __OSErrorTable[OS_ERROR_MAX];
extern u32 lbl_80478990; /* __OSFpscrEnableBits */

static OSThreadQueue RunQueue[32];
static volatile u32 RunQueueBits;
static volatile int RunQueueHint_8047A764;
static int Reschedule;
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;


void UnsetRun(OSThread* thread);
static OSThread* SelectThread(BOOL yield);

#define AddTail(queue, thread, link)                                           \
    do {                                                                       \
        OSThread* __prev;                                                      \
        __prev = (queue)->tail;                                                \
        if (__prev == NULL) {                                                  \
            (queue)->head = (thread);                                          \
        } else {                                                               \
            __prev->link.next = (thread);                                      \
        }                                                                      \
        (thread)->link.prev = __prev;                                          \
        (thread)->link.next = NULL;                                            \
        (queue)->tail = (thread);                                              \
    } while (0)

#define AddPrio(queue, thread, link)                                           \
    do {                                                                       \
        OSThread* __next;                                                      \
        OSThread* __prev;                                                      \
        for (__next = (queue)->head;                                           \
             __next != NULL && __next->priority <= (thread)->priority;         \
             __next = __next->link.next) {                                     \
            ;                                                                  \
        }                                                                      \
        if (__next == NULL) {                                                  \
            AddTail(queue, thread, link);                                      \
        } else {                                                               \
            (thread)->link.next = __next;                                      \
            __prev = __next->link.prev;                                        \
            __next->link.prev = (thread);                                      \
            (thread)->link.prev = __prev;                                      \
            if (__prev == NULL) {                                              \
                (queue)->head = (thread);                                      \
            } else {                                                           \
                __prev->link.next = (thread);                                  \
            }                                                                  \
        }                                                                      \
    } while (0)

#define RemoveItem(queue, thread, link)                                        \
    do {                                                                       \
        OSThread* __next;                                                      \
        OSThread* __prev;                                                      \
        __next = (thread)->link.next;                                          \
        __prev = (thread)->link.prev;                                          \
        if (__next == NULL) {                                                  \
            (queue)->tail = __prev;                                            \
        } else {                                                               \
            __next->link.prev = __prev;                                        \
        }                                                                      \
        if (__prev == NULL) {                                                  \
            (queue)->head = __next;                                            \
        } else {                                                               \
            __prev->link.next = __next;                                        \
        }                                                                      \
    } while (0)

#define RemoveHead(queue, thread, link)                                        \
    do {                                                                       \
        OSThread* __next;                                                      \
        (thread) = (queue)->head;                                              \
        __next = (thread)->link.next;                                          \
        if (__next == NULL) {                                                  \
            (queue)->tail = NULL;                                              \
        } else {                                                               \
            __next->link.prev = NULL;                                          \
        }                                                                      \
        (queue)->head = __next;                                                \
    } while (0)

#define SetRun(thread)                                                         \
    do {                                                                       \
        (thread)->queue = &RunQueue[(thread)->priority];                       \
        AddTail((thread)->queue, (thread), link);                              \
        RunQueueBits |= 1 << (31 - (thread)->priority);                        \
        RunQueueHint_8047A764 = TRUE;                                          \
    } while (0)

s32 DefaultSwitchThreadCallback(OSThread* from, OSThread* to)
{
}

static OSSwitchThreadCallback SwitchThreadCallback = DefaultSwitchThreadCallback;

void __OSThreadInit(void)
{
    OSThread* thread = &DefaultThread;
    s32 prio;

    thread->state = OS_THREAD_STATE_RUNNING;
    thread->attr = OS_THREAD_ATTR_DETACH;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = -1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    __gUnkThread1 = thread;
    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = (u32*)_stack_addr;
    thread->stackEnd = (u32*)_stack_end;
    *thread->stackEnd = OS_THREAD_STACK_MAGIC;
    SwitchThreadCallback(__OSCurrentThread, thread);
    __OSCurrentThread = thread;
    OSClearStack(0);
    RunQueueBits = 0;
    RunQueueHint_8047A764 = 0;

    for (prio = 0; prio <= 31; prio++) {
        OSInitThreadQueue(&RunQueue[prio]);
    }
    OSInitThreadQueue(&__OSActiveThreadQueue);
    AddTail(&__OSActiveThreadQueue, thread, linkActive);
    OSClearContext(&IdleContext);
    Reschedule = 0;
}

void OSInitThreadQueue(OSThreadQueue* queue)
{
    queue->head = queue->tail = NULL;
}

OSThread* OSGetCurrentThread(void)
{
    return __OSCurrentThread;
}

s32 OSDisableScheduler(void)
{
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule++;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler(void)
{
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule--;
    OSRestoreInterrupts(enabled);
    return count;
}

/* RULE-EXCEPTION(user-approved): local auto_inline pragma keeps UnsetRun out of line as in retail — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma auto_inline off
void UnsetRun(OSThread* thread)
{
    OSThreadQueue* queue;

    queue = thread->queue;
    RemoveItem(queue, thread, link);
    if (queue->head == NULL) {
        RunQueueBits &= ~(1 << (31 - thread->priority));
    }
    thread->queue = NULL;
}
#pragma pop

s32 __OSGetEffectivePriority(OSThread* thread)
{
    s32 priority;
    OSMutex* mutex;
    OSThread* blocked;

    priority = thread->base;
    for (mutex = thread->queueMutex.head; mutex != NULL; mutex = mutex->link.next) {
        blocked = mutex->queue.head;
        if (blocked != NULL && blocked->priority < priority) {
            priority = blocked->priority;
        }
    }
    return priority;
}

static OSThread* SetEffectivePriority(OSThread* thread, s32 priority)
{
    switch (thread->state) {
    case OS_THREAD_STATE_READY:
        UnsetRun(thread);
        thread->priority = priority;
        SetRun(thread);
        break;

    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->priority = priority;
        AddPrio(thread->queue, thread, link);
        if (thread->mutex != NULL) {
            return thread->mutex->thread;
        }
        break;

    case OS_THREAD_STATE_RUNNING:
        RunQueueHint_8047A764 = TRUE;
        thread->priority = priority;
        break;
    }
    return NULL;
}

void __OSPromoteThread(OSThread* thread, s32 priority)
{
    do {
        if (thread->suspend > 0) {
            break;
        }
        if (thread->priority <= priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
    } while (thread != NULL);
}

static inline void UpdatePriority(OSThread* thread)
{
    s32 priority;

    do {
        if (thread->suspend > 0) {
            break;
        }
        priority = __OSGetEffectivePriority(thread);
        if (thread->priority == priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
    } while (thread != NULL);
}

static inline void __OSSwitchThread(OSThread* nextThread)
{
    SwitchThreadCallback(__OSCurrentThread, nextThread);
    __OSCurrentThread = nextThread;
    OSSetCurrentContext(&nextThread->context);
    OSLoadContext(&nextThread->context);
}

static OSThread* SelectThread(BOOL yield)
{
    OSContext* currentContext;
    OSThread* currentThread;
    OSThread* nextThread;
    s32 priority;
    OSThreadQueue* queue;

    if (Reschedule > 0) {
        return NULL;
    }

    currentContext = OSGetCurrentContext();
    currentThread = OSGetCurrentThread();
    if (currentContext != &currentThread->context) {
        return NULL;
    }

    if (currentThread) {
        if (currentThread->state == OS_THREAD_STATE_RUNNING) {
            if (yield == 0) {
                priority = __cntlzw(RunQueueBits);
                if (currentThread->priority <= priority) {
                    return NULL;
                }
            }
            currentThread->state = OS_THREAD_STATE_READY;
            SetRun(currentThread);
        }
        if (!(currentThread->context.state & OS_CONTEXT_STATE_EXC) &&
            OSSaveContext(&currentThread->context) != 0) {
            return NULL;
        }
    }

    if (RunQueueBits == 0) {
        SwitchThreadCallback(__OSCurrentThread, NULL);
        __OSCurrentThread = NULL;
        OSSetCurrentContext(&IdleContext);
        do {
            OSEnableInterrupts();
            while (RunQueueBits == 0) {
                ;
            }
            OSDisableInterrupts();
        } while (RunQueueBits == 0);
        OSClearContext(&IdleContext);
    }

    RunQueueHint_8047A764 = FALSE;
    priority = __cntlzw(RunQueueBits);
    queue = &RunQueue[priority];
    nextThread = queue->head;
    {
        OSThread* __next = nextThread->link.next;
        if (__next == NULL) {
            queue->tail = NULL;
        } else {
            __next->link.prev = NULL;
        }
        queue->head = __next;
    }
    if (!queue->head) {
        RunQueueBits &= ~(1 << (31 - priority));
    }
    nextThread->queue = NULL;
    nextThread->state = OS_THREAD_STATE_RUNNING;
    __OSSwitchThread(nextThread);
    return nextThread;
}

void __OSReschedule(void)
{
    if (RunQueueHint_8047A764) {
        SelectThread(FALSE);
    }
}

void OSYieldThread(void)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SelectThread(TRUE);
    OSRestoreInterrupts(enabled);
}

BOOL OSCreateThread(OSThread* thread, void* (*func)(void*), void* param, void* stack,
                    u32 stackSize, s32 priority, u16 attr)
{
    BOOL enabled;
    u32 sp;
    OSContext* context;
    int i;

    if (priority < OS_PRIORITY_MIN || OS_PRIORITY_MAX < priority) {
        return FALSE;
    }

    context = &thread->context;
    thread->state = OS_THREAD_STATE_READY;
    thread->attr = attr & OS_THREAD_ATTR_DETACH;
    thread->base = priority;
    thread->priority = priority;
    thread->suspend = 1;
    thread->val = -1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    OSInitThreadQueue((OSThreadQueue*)&thread->queueMutex);
    sp = (u32)stack;
    sp &= ~7;
    sp -= 8;
    ((u32*)sp)[0] = 0;
    ((u32*)sp)[1] = 0;
    OSInitContext(context, (u32)func, sp);
    thread->context.lr = (u32)&OSExitThread;
    thread->context.gpr[3] = (u32)param;
    thread->stackBase = (u32*)stack;
    thread->stackEnd = (u32*)((u32)stack - stackSize);
    *thread->stackEnd = OS_THREAD_STACK_MAGIC;
    thread->error = 0;
    for (i = 0; i < 2; i++) {
        thread->specific[i] = NULL;
    }
    enabled = OSDisableInterrupts();
    if (__OSErrorTable[OS_ERROR_FPE] != NULL) {
        thread->context.srr1 |= 0x900;
        thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
        thread->context.fpscr = (lbl_80478990 & 0xF8) | 4;
        for (i = 0; i < 32; i++) {
            *(u64*)&thread->context.fpr[i] = 0xFFFFFFFFFFFFFFFFULL;
            *(u64*)&thread->context.psf[i] = 0xFFFFFFFFFFFFFFFFULL;
        }
    }
    AddTail(&__OSActiveThreadQueue, thread, linkActive);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void OSExitThread(void* val)
{
    OSThread* currentThread;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    currentThread = OSCurrentThread;
    OSClearContext(&currentThread->context);
    if (currentThread->attr & OS_THREAD_ATTR_DETACH) {
        RemoveItem(&__OSActiveThreadQueue, currentThread, linkActive);
        currentThread->state = OS_THREAD_STATE_EXITED;
    } else {
        currentThread->state = OS_THREAD_STATE_MORIBUND;
        currentThread->val = (u32)val;
    }
    __OSUnlockAllMutex(currentThread);
    OSWakeupThread(&currentThread->queueJoin);
    RunQueueHint_8047A764 = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

void OSCancelThread(OSThread* thread)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    switch (thread->state) {
    case OS_THREAD_STATE_READY:
        if (thread->suspend > 0) {
            break;
        }
        UnsetRun(thread);
        break;

    case OS_THREAD_STATE_RUNNING:
        RunQueueHint_8047A764 = TRUE;
        break;

    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->queue = NULL;
        if (thread->suspend > 0) {
            break;
        }
        if (thread->mutex != NULL) {
            UpdatePriority(thread->mutex->thread);
        }
        break;

    default:
        OSRestoreInterrupts(enabled);
        return;
    }

    OSClearContext(&thread->context);
    if (thread->attr & OS_THREAD_ATTR_DETACH) {
        RemoveItem(&__OSActiveThreadQueue, thread, linkActive);
        thread->state = OS_THREAD_STATE_EXITED;
    } else {
        thread->state = OS_THREAD_STATE_MORIBUND;
    }
    __OSUnlockAllMutex(thread);
    OSWakeupThread(&thread->queueJoin);
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

static inline BOOL IsThreadActive(OSThread* thread)
{
    OSThread* active;

    if (thread->state == OS_THREAD_STATE_EXITED) {
        return FALSE;
    }
    for (active = __OSActiveThreadQueue.head; active != NULL;
         active = active->linkActive.next) {
        if (thread == active) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL fn_800A1E54(OSThread* thread, u32* val)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(thread->attr & OS_THREAD_ATTR_DETACH) &&
        thread->state != OS_THREAD_STATE_MORIBUND && thread->queueJoin.head == NULL) {
        OSSleepThread(&thread->queueJoin);
        if (!IsThreadActive(thread)) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
    }
    if (thread->state == OS_THREAD_STATE_MORIBUND) {
        if (val != NULL) {
            *val = thread->val;
        }
        RemoveItem(&__OSActiveThreadQueue, thread, linkActive);
        thread->state = OS_THREAD_STATE_EXITED;
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

s32 OSResumeThread(OSThread* thread)
{
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend--;
    if (thread->suspend < 0) {
        thread->suspend = 0;
    } else if (thread->suspend == 0) {
        switch (thread->state) {
        case OS_THREAD_STATE_READY:
            thread->priority = __OSGetEffectivePriority(thread);
            SetRun(thread);
            break;

        case OS_THREAD_STATE_WAITING:
            RemoveItem(thread->queue, thread, link);
            thread->priority = __OSGetEffectivePriority(thread);
            AddPrio(thread->queue, thread, link);
            if (thread->mutex != NULL) {
                UpdatePriority(thread->mutex->thread);
            }
            break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

s32 OSSuspendThread(OSThread* thread)
{
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend++;
    if (suspendCount == 0) {
        switch (thread->state) {
        case OS_THREAD_STATE_RUNNING:
            RunQueueHint_8047A764 = TRUE;
            thread->state = OS_THREAD_STATE_READY;
            break;

        case OS_THREAD_STATE_READY:
            UnsetRun(thread);
            break;

        case OS_THREAD_STATE_WAITING:
            RemoveItem(thread->queue, thread, link);
            thread->priority = 32;
            AddTail(thread->queue, thread, link);
            if (thread->mutex != NULL) {
                UpdatePriority(thread->mutex->thread);
            }
            break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

void OSSleepThread(OSThreadQueue* queue)
{
    BOOL enabled;
    OSThread* currentThread;

    enabled = OSDisableInterrupts();
    currentThread = OSCurrentThread;
    currentThread->state = OS_THREAD_STATE_WAITING;
    currentThread->queue = queue;
    AddPrio(queue, currentThread, link);
    RunQueueHint_8047A764 = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

void OSWakeupThread(OSThreadQueue* queue)
{
    BOOL enabled;
    OSThread* thread;

    enabled = OSDisableInterrupts();
    while (queue->head != NULL) {
        RemoveHead(queue, thread, link);
        thread->state = OS_THREAD_STATE_READY;
        if (!(thread->suspend > 0)) {
            SetRun(thread);
        }
    }
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

BOOL OSSetThreadPriority(OSThread* thread, s32 priority)
{
    BOOL enabled;

    if (priority < OS_PRIORITY_MIN || OS_PRIORITY_MAX < priority) {
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

OSThread* OSSetIdleFunction(void (*idleFunction)(void*), void* param, void* stack,
                            u32 stackSize)
{
    if (idleFunction) {
        if (IdleThread.state == OS_THREAD_STATE_EXITED) {
            OSCreateThread(&IdleThread, (void* (*)(void*))idleFunction, param,
                           stack, stackSize, OS_PRIORITY_IDLE,
                           OS_THREAD_ATTR_DETACH);
            OSResumeThread(&IdleThread);
            return &IdleThread;
        }
    } else if (IdleThread.state != OS_THREAD_STATE_EXITED) {
        OSCancelThread(&IdleThread);
    }
    return NULL;
}

void OSClearStack(u8 val)
{
    u32 sp;
    u32* p;
    u32 pattern;

    pattern = (val << 24) | (val << 16) | (val << 8) | val;
    sp = OSGetStackPointer();
    p = OSCurrentThread->stackEnd + 1;
    while (p < (u32*)sp) {
        *p++ = pattern;
    }
}
