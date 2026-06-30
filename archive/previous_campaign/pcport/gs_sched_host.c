/* gs_sched_host.c — see gs_sched_host.h.
 *
 * Faithful host reimplementation of the engine's GStask + GSthread cooperative
 * scheduler (src/game/gs_thread.c), built directly on the os_thread_host fibre
 * primitives. The task-layer logic is reproduced verbatim from the original C
 * (priority-sorted linked list, 1-based IDs, free-slot search, the GStaskRun
 * walk); the GSmem handle allocation is replaced by static pools, and the asm
 * context primitives by host fibres.
 */
#include "gs_sched_host.h"
#include "os_thread_host.h"

#include <stdio.h>
#include <string.h>

/* Static pools replace the GSmem allocations in the original. Sized to the
 * engine's actual usage: GameInit calls GStaskInit(16,4) and the thread count
 * is small. */
#define GS_TASK_POOL_MAX   64
#define GS_THREAD_POOL_MAX 16

/* --- task system globals (mirror gs_thread.c's sbss names) --- */
static u32     gsTaskMaxNormal;
static u32     gsTaskMaxDeferred;
static u32     gsTaskTotal;
static GSTask* gsTaskCurrentRun;
static GSTask* gsTaskListHead;
static GSTask* gsTaskDeferredHead;
static GSTask* gsTaskArray;

/* --- thread system globals --- */
static u32       gsThreadMaxCount;
static GSThread* gsThreadArray;
static GSThread* gsThreadListHead;
static u32       gsThreadFrameCount;
static u8        gsThreadActive;

/* --- host-side backing storage --- */
static GSTask    g_taskPool[GS_TASK_POOL_MAX];
static GSThread  g_threadPool[GS_THREAD_POOL_MAX];
static HostFibre* g_threadFibre[GS_THREAD_POOL_MAX]; /* GSThread slot -> fibre  */

static HostFibre* g_schedFibre;     /* the host/scheduler fibre (main context)  */
static int        g_runningThread;  /* slot index of the thread currently running, or -1 */

void GSched_HostReset(void) {
    memset(g_taskPool, 0, sizeof(g_taskPool));
    memset(g_threadPool, 0, sizeof(g_threadPool));
    memset(g_threadFibre, 0, sizeof(g_threadFibre));

    gsTaskMaxNormal = gsTaskMaxDeferred = gsTaskTotal = 0;
    gsTaskCurrentRun = gsTaskListHead = gsTaskDeferredHead = NULL;
    gsTaskArray = NULL;

    gsThreadMaxCount = 0;
    gsThreadArray = NULL;
    gsThreadListHead = NULL;
    gsThreadFrameCount = 0;
    gsThreadActive = 0;

    g_runningThread = -1;
    g_schedFibre = HostFibre_InitMain();  /* convert this thread to the host fibre */
}

/* ===================================================================
 * TASK LAYER — reproduced from gs_thread.c GStaskInit/Create/Run.
 * =================================================================== */

void GStaskInit(u32 numTasks, u32 numQueues) {
    u32 total = numTasks + numQueues;
    s32 i;

    if (total > GS_TASK_POOL_MAX) {
        fprintf(stderr, "[gs_sched_host] GStaskInit: %u tasks exceeds pool %d\n",
                total, GS_TASK_POOL_MAX);
        total = GS_TASK_POOL_MAX;
    }

    gsTaskMaxNormal   = numTasks;
    gsTaskMaxDeferred = numQueues;
    gsTaskTotal       = total;
    gsTaskCurrentRun  = NULL;
    gsTaskArray       = g_taskPool;

    for (i = 0; i < (s32)total; i++) {
        g_taskPool[i].state = GSTASK_FREE;
    }
    printf("[gs_sched_host] GStaskInit: %u normal + %u deferred = %u slots\n",
           numTasks, numQueues, total);
}

u32 GStaskCreate(u32 state, u8 priority, void* param, void* func) {
    GSTask* task;
    u32 count;
    s32 i;

    if (state == GSTASK_DEFERRED) {
        task = (GSTask*)((u32)gsTaskArray + gsTaskMaxNormal * sizeof(GSTask));
        count = gsTaskMaxDeferred;
    } else {
        task = gsTaskArray;
        count = gsTaskMaxNormal;
    }

    for (i = 0; i < (s32)count; i++) {
        if (task->state == GSTASK_FREE) {
            goto found;
        }
        task = (GSTask*)((u32)task + sizeof(GSTask));
    }
    return 0; /* no free slot */

found:
    task->prev     = NULL;
    task->next     = NULL;
    task->state    = state;
    task->priority = priority;
    task->paused   = 0;
    task->param    = param;
    task->func     = (void (*)(u32, void*))func;

    if (gsTaskListHead == NULL) {
        gsTaskListHead = task;
    } else if (state == GSTASK_DEFERRED) {
        task->next = gsTaskDeferredHead;
        gsTaskDeferredHead = task;
    } else {
        /* Insert into the active list in priority order (lower runs first). */
        GSTask* curr = gsTaskListHead;
        while (curr->next != NULL) {
            if (curr->priority >= task->priority) {
                break;
            }
            curr = curr->next;
        }
        if (curr->next == NULL && curr->priority < task->priority) {
            task->prev = curr;
            task->next = NULL;
            curr->next = task;
        } else {
            GSTask* prevOfCurr = curr->prev;
            if (prevOfCurr != NULL) {
                prevOfCurr->next = task;
            }
            task->prev = curr->prev;
            task->next = curr;
            curr->prev = task;
            if (gsTaskListHead == curr) {
                gsTaskListHead = task;
            }
        }
    }

    return (u32)(((u32)task - (u32)gsTaskArray) / sizeof(GSTask)) + 1;
}

void GStaskRun(void) {
    GSTask* task = gsTaskListHead;
    GSTask* nextTask;

    while (task != NULL) {
        nextTask = task->next;
        if (task->state == GSTASK_ACTIVE && task->paused == 0) {
            u32 taskId = (u32)(((u32)task - (u32)gsTaskArray) / sizeof(GSTask)) + 1;
            gsTaskCurrentRun = task;
            task->func(taskId, task->param);
        }
        task = nextTask;
    }
    gsTaskCurrentRun = NULL;
    gsThreadFrameCount++;
}

/* ===================================================================
 * THREAD LAYER — GSthreadInit/Create on host fibres; GSthreadYield is
 * the host equivalent of the asm vsync-yield fn_800F0308.
 * =================================================================== */

void GSthreadInit(u32 maxThreads) {
    u32 i;
    if (maxThreads > GS_THREAD_POOL_MAX) {
        fprintf(stderr, "[gs_sched_host] GSthreadInit: %u threads exceeds pool %d\n",
                maxThreads, GS_THREAD_POOL_MAX);
        maxThreads = GS_THREAD_POOL_MAX;
    }
    gsThreadMaxCount = maxThreads;
    gsThreadArray = g_threadPool;
    for (i = 0; i < maxThreads; i++) {
        g_threadPool[i].active = 0;
    }
    gsThreadFrameCount = 0;
    gsThreadListHead = NULL;
    printf("[gs_sched_host] GSthread: Init OK, maximum of %u threads\n", maxThreads);

    if (g_schedFibre == NULL) {
        g_schedFibre = HostFibre_InitMain();
    }
}

/* Fibre trampoline: run the GSthread entry, then (should it ever return) park
 * the thread and keep yielding so the scheduler stops resuming it. */
static void GSthread_FibreEntry(void* param) {
    GSThread* thread = (GSThread*)param;
    if (thread != NULL && thread->entryFunc != NULL) {
        void (*entry)(void) = (void (*)(void))thread->entryFunc;
        entry();
    }
    if (thread != NULL) {
        thread->active = 0;  /* entry returned: thread is done */
    }
    for (;;) {
        GSthreadYield();
    }
}

GSThread* GSthreadCreate(u32 affinity, u32 priority, u32 stackSize,
                         u32 usesFPU, u32 autoStart, void* entryFunc) {
    GSThread* thread;
    u32 i;
    int slot;

    /* Find a free thread slot (active == 0). */
    thread = gsThreadArray;
    for (i = 0; i < gsThreadMaxCount; i++) {
        if (thread->active == 0) {
            goto found;
        }
        thread = (GSThread*)((u32)thread + sizeof(GSThread));
    }
    return NULL; /* no free slot */

found:
    slot = (int)(((u32)thread - (u32)gsThreadArray) / sizeof(GSThread));

    thread->active    = 1;
    thread->priority  = priority;
    thread->stackSize = stackSize;
    thread->suspended = 0;
    thread->sleeping  = 0;
    thread->usesFPU   = (u8)usesFPU;
    thread->entryFunc = entryFunc;
    thread->prev      = NULL;
    thread->next      = NULL;
    thread->affinity  = (u8)affinity;
    thread->autoStart = (u8)autoStart;

    /* Back the GSthread with a host fibre (replaces GSmem stack/ctx + the asm
     * fn_800F015C context init). The fibre is created suspended; GSthreadStepAll
     * resumes it cooperatively each frame. */
    g_threadFibre[slot] = HostFibre_Create(GSthread_FibreEntry, thread,
                                           stackSize ? stackSize : (256 * 1024));
    if (g_threadFibre[slot] == NULL) {
        thread->active = 0;
        return NULL;
    }

    /* Insert into the affinity-sorted thread list (verbatim from the original). */
    {
        GSThread* curr = gsThreadListHead;
        if (curr == NULL) {
            gsThreadListHead = thread;
        } else {
            while (curr->next != NULL) {
                if (curr->affinity >= thread->affinity) {
                    break;
                }
                curr = curr->next;
            }
            if (curr->next == NULL && curr->affinity < thread->affinity) {
                thread->prev = curr;
                thread->next = NULL;
                curr->next = thread;
            } else {
                GSThread* prevOfCurr = curr->prev;
                if (prevOfCurr != NULL) {
                    prevOfCurr->next = thread;
                }
                thread->prev = curr->prev;
                thread->next = curr;
                curr->prev = thread;
                if (gsThreadListHead == curr) {
                    gsThreadListHead = thread;
                }
            }
        }
    }

    gsThreadActive = 1;
    printf("[gs_sched_host] GSthreadCreate: slot %d (affinity=%u priority=%u entry=%p)\n",
           slot, affinity, priority, entryFunc);
    return thread;
}

void GSthreadYield(void) {
    /* fn_800F0308 host equivalent: suspend the running GSthread fibre and return
     * to the scheduler/host fibre. Resumes (returns) next frame. */
    if (g_schedFibre != NULL) {
        HostFibre_SwitchTo(g_schedFibre);
    }
}

int GSthreadStepAll(void) {
    GSThread* thread = gsThreadListHead;
    int live = 0;

    while (thread != NULL) {
        GSThread* nextThread = thread->next;
        if (thread->active && !thread->suspended) {
            int slot = (int)(((u32)thread - (u32)gsThreadArray) / sizeof(GSThread));
            if (g_threadFibre[slot] != NULL) {
                g_runningThread = slot;
                HostFibre_SwitchTo(g_threadFibre[slot]); /* runs its slice */
                g_runningThread = -1;
                if (thread->active) {
                    live++;
                }
            }
        }
        thread = nextThread;
    }
    return live;
}

/* ===================================================================
 * Introspection.
 * =================================================================== */

int GSched_TaskCount(void) {
    int n = 0;
    u32 i;
    for (i = 0; i < gsTaskTotal; i++) {
        if (g_taskPool[i].state != GSTASK_FREE) {
            n++;
        }
    }
    return n;
}

int GSched_ThreadCount(void) {
    int n = 0;
    u32 i;
    for (i = 0; i < gsThreadMaxCount; i++) {
        if (g_threadPool[i].active) {
            n++;
        }
    }
    return n;
}

unsigned int GSched_FrameCount(void) {
    return gsThreadFrameCount;
}

/* ===================================================================
 * fn_ aliases — so real engine TUs (main.c GameInit, etc.) that call the
 * scheduler by address-name bind to THIS host implementation instead of the
 * pcport_link.py auto-stubs. Thin forwarders.
 * =================================================================== */

void fn_800FE9B0(u32 numTasks, u32 numQueues) { GStaskInit(numTasks, numQueues); }
u32  fn_800FE834(u32 state, u8 priority, void* param, void* func) {
    return GStaskCreate(state, priority, param, func);
}
void fn_800FE7A0(void) { GStaskRun(); }
GSThread* fn_800F07A8(u32 affinity, u32 priority, u32 stackSize,
                      u32 usesFPU, u32 autoStart, void* entryFunc) {
    return GSthreadCreate(affinity, priority, stackSize, usesFPU, autoStart, entryFunc);
}
/* fn_800F0308 — the per-frame vsync-yield the real engine thread bodies call
 * (e.g. gs_title.c fn_8002058C's `for(;;) fn_800F0308();`). Aliased to the host
 * GSthreadYield so that when a real engine TU is made host-linkable (Track D) and
 * its thread runs on our scheduler, its yields route through the host fibre layer
 * instead of the auto-stub. Safe no-op when no host scheduler fibre is live
 * (GSthreadYield guards on g_schedFibre), so it does not affect the --menu path. */
void fn_800F0308(void) { GSthreadYield(); }
/* NOTE: fn_800F09D8 (GSthreadInit) is intentionally NOT aliased — the decomp
 * annotations conflict (main.c labels 0x800F09D8 as render-timing, gs_thread.c
 * as GSthreadInit). Call GSthreadInit() directly from host boot code instead. */
