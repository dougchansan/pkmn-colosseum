/**
 * @file gs_thread.c
 * @brief GSthread -- Genius Sonority cooperative task / thread system.
 *
 * Decompiled from:
 *   fn_800FE9B0 (GStaskInit)
 *   fn_800FE834 (GStaskCreate)
 *   fn_800FE7A0 (GStaskRun)
 *   GSthread (GSthreadInit)
 *   GSthreadCreate (GSthreadCreate)
 *   fn_800FEA74 (GStaskSchedulerThread -- internal)
 *   fn_800FEBA0 (GStaskSwapCallback -- internal)
 *
 * Debug strings:
 *   "GSthread: Init OK, maximum of %d threads"
 *   "GSthreadCreate. Warning: 'usesFPU==FALSE' OK?"
 *
 * The task system and thread system are two separate but related layers:
 *
 * 1. TASKS (lightweight):
 *    - Array of GSTask structs, each 0x18 (24) bytes.
 *    - Kept in a priority-sorted singly-linked list.
 *    - GStaskRun walks the list each frame and calls active callbacks.
 *    - Used for per-frame work: VBlank, pad polling, audio, reset.
 *
 * 2. THREADS (heavier cooperative fibres):
 *    - Array of GSThread structs, each 0x24 (36) bytes.
 *    - Own a GSmem-allocated stack and context block.
 *    - Managed in a priority-sorted doubly-linked list.
 *    - Used for the main game loop and long-running subsystems.
 *
 * Address range: 0x800F07A8 - 0x800FEBA0 (approx.)
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);          /* OSReport */
extern u16   GSmemAllocRaw(u32 size);                    /* fn_800E3534 */
extern void* GSmemGetPtr(u16 handle);                    /* fn_800E27B0 */
extern void* GSmemLock(u16 handle);                      /* fn_800E24B0 */
extern void  GSmemFree(u16 handle);                      /* fn_800E209C */
extern u16   GSmemAlloc(u32 alignment, u32 size);        /* fn_800E2C04 */
extern void  fn_800A263C(void* func, void* arg,
                          void* stackTop, u32 stackSize); /* OSCreateFiber-like */
extern void  OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(void);
extern void  fn_800D30A0(void* callback);                 /* GSgfx register swap callback */
extern void  threadSaveGPRRegisters(void);                /* GSthread context init */
extern void  fn_800F01F0(void);                           /* GSthread FPU context init */
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSscratchFree();
extern void cos();   /* MSL trig (renamed fn_800CDBE0) — referenced by asm incs */

/* ===== String constants (rodata references) ===== */
extern const char lbl_80271008[]; /* "GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n" */
extern const char lbl_80271038[]; /* "GSthread: Init OK, maximum of %d threads\n" */

/* ===== Forward declarations for internal functions ===== */
static void GStaskSchedulerThread(void);  /* fn_800FEA74 */
extern void fn_800FEBA0(void);            /* GStaskSwapCallback */
extern void fn_800F0F4C(u32 arg);          /* GSthread trampoline / entry wrapper */
extern void fn_800AB150(void* buf);
extern u32 fn_800D0F44(u32 buttonIdx);
extern void fn_800AB4FC(void);
extern void fn_800E209C(u16 handle);
extern void fn_800E24B0(void);
extern void fn_800E27B0(void);
extern void fn_800E2C04(void);
extern void fn_800E3534(void);
extern void fn_80080ED8(void);
extern void fn_800DBEB4(u32 a, void* b);
extern void fn_800D5CB8(void);
extern void fn_800D61E4(void);
extern void fn_800D6728(void);
extern void fn_800D67BC(void);
extern void fn_800D6A00(void);
extern void fn_800D7820(void);
extern void fn_800D85D4(void);
extern void fn_800D888C(u32 mask);
extern void fn_800D88DC(void);
extern void fn_800D9ED8(void);
extern void fn_800DC1D4(void);
extern void fn_800DE680(void);
extern void fn_800EF504(void* ctx);
extern void fn_800EF548(void);
extern void fn_801669BC(u32 type);
extern void GStextureCreate(void);
extern void fn_800CDBE0(void);
extern u32 fn_800D3088(void);
extern void fn_800DBF78(void);
extern void fn_800DBFD4(void);
extern void fn_800DC04C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC14C(void);
extern void fn_800DC224(void);
extern void fn_801040F0(void);
extern void fn_80166A28(void);
extern void fn_800D59B8(void);
extern void fn_800D5BA0(void);
extern void fn_800D9D68(u16 a, u16 b, u16 c, u16 d);
extern f64 fn_800CE220(void);
extern void fn_800D7FE4(void* mtx);
extern void fn_800D834C(void);
extern void fn_800D9BD0(f32 a, f32 b, f32 c, f32 d);
extern void fn_800DA028(s32 a);
extern void fn_800DA100(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void fn_800DA1E8(s32 a, s32 b, s32 c);
extern void fn_800DA2BC(s32 a, s32 b, s32 c);
extern void fn_800DA4C4(s32 a, s32 b, s32 c);
extern void fn_800E01F4(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E0218(void* dst, void* a, void* b, void* c);
extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dst, const void* src, u32 n);

/* ===== BSS/SDA symbol externs (for asm{} blocks) ===== */
/* BSS/data/rodata symbols accessed via lis/@ha + addi/@l pairs */
extern u32 lbl_80401C10;
/* .bss symbols */
extern u8  lbl_80401DE0[];
extern u8  lbl_80401E48[];
extern u8  lbl_80402418[];
extern u8  lbl_80402480[];
extern u8  lbl_804024E8[];
/* .data symbols */
extern u8  lbl_80314E08[];
extern u8  lbl_80314F98[];
extern u8  lbl_80315678[];
/* .rodata symbols */
extern u8  lbl_80271300[];
extern u8  lbl_80271500[];
extern u8  lbl_80271700[];
extern u8  lbl_80271730[];
extern u8  lbl_80271754[];
extern u8  lbl_8027177C[];
extern u8  lbl_802717B4[];
extern u8  lbl_802717D4[];
/* .sdata symbol */
extern float lbl_80478AC0;
/* sdata2 (r2) float/double constants used in asm blocks */
extern f64 lbl_8047CCC8;  /* f64 */
extern f32 lbl_8047CCD0;  /* f32 */
extern f32 lbl_8047CCD4;  /* f32 */
extern f32 lbl_8047CCD8;  /* f32 */
extern f32 lbl_8047CCDC;  /* f32 */
extern f64 lbl_8047CCE0;  /* f64 */
extern f64 lbl_8047CCE8;  /* f64 */
extern f64 lbl_8047CCF0;  /* f64 */
extern f64 lbl_8047CCF8;  /* f64 */
extern u32 lbl_8047CD00;  /* u32 (lwz) */
extern u32 lbl_8047CD04;  /* u32 (lwz) */
extern f32 lbl_8047CD08;  /* f32 */
extern f64 lbl_8047CD10;  /* f64 */
extern f64 lbl_8047CD18;  /* f64 */
extern f64 lbl_8047CD20;  /* f64 */
extern f64 lbl_8047CD28;  /* f64 */
extern f32 lbl_8047CD30;  /* f32 */
extern f32 lbl_8047CD34;  /* f32 */
extern f32 lbl_8047CD38;  /* f32 */
extern f32 lbl_8047CD3C;  /* f32 */
extern f32 lbl_8047CD40;  /* f32 */
extern f32 lbl_8047CD44;  /* f32 */
extern f32 lbl_8047CD48;  /* f32 */
extern f32 lbl_8047CD4C;  /* f32 */
extern f64 lbl_8047CD50;  /* f64 */
extern f32 lbl_8047CD58;  /* f32 */
extern f32 lbl_8047CD5C;  /* f32 */
extern f32 lbl_8047CD60;  /* f32 */
extern f32 lbl_8047CD64;  /* f32 */
extern f32 lbl_8047CD68;  /* f32 */
extern f32 lbl_8047CD6C;  /* f32 */
extern f32 lbl_8047CD70;  /* f32 */
extern f32 lbl_8047CD74;  /* f32 */
extern f32 lbl_8047CD78;  /* f32 */
/* sbss (r13) symbols -- task and thread system */
extern u32 lbl_80478B08;
extern u32 lbl_80478B10;
extern u32 lbl_80478B14;
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC08;
extern u32 lbl_8047AC0C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC28;
extern u32 lbl_8047AC2C;
extern u32 lbl_8047AC30;
extern u32 lbl_8047AC34;
extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;
extern u32 lbl_8047AC44;
extern u32 lbl_8047AC48;
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
extern u32 lbl_8047AC54;
extern u16 lbl_8047AC58;
extern u32 lbl_8047AC5C;
extern u32 lbl_8047AC60;
extern u32 lbl_8047AC64;
extern u32 lbl_8047AC68;
extern u32 lbl_8047AC6C;
extern u32 lbl_8047AC70;
extern u32 lbl_8047AC72;
extern u32 lbl_8047AC74;
extern u32 lbl_8047AC78;
extern u32 lbl_8047AC7C;
extern u32 lbl_8047AC80;
extern u32 lbl_8047AC84;
extern u32 lbl_8047AC88;
extern u32 lbl_8047AC8C;
extern u32 lbl_8047AC90;
extern u32 lbl_8047AC94;
extern u32 lbl_8047AC98;
extern u32 lbl_8047AC9C;
/* sdata2 (r2) symbols: CW asm{} does not support sym(r2) syntax.
 * These are handled via numeric offsets in the .inc files.
 * No extern declarations needed. */

/* ===== Global state (sbss) ===== */

/* --- Task system globals --- */
static u32     gsTaskMaxNormal;     /* lbl_8047AC80 : max normal-priority tasks */
static u32     gsTaskMaxDeferred;   /* lbl_8047AC84 : max deferred-queue tasks  */
static u32     gsTaskTotal;         /* lbl_8047AC88 : total task slots           */
static GSTask* gsTaskCurrentRun;    /* lbl_8047AC94 : task currently executing   */
static GSTask* gsTaskListHead;      /* lbl_8047AC98 : head of active task list   */
static GSTask* gsTaskDeferredHead;  /* lbl_8047AC9C : head of deferred list      */
static u16     gsTaskArrayHandle;   /* lbl_8047AC78 : GSmem handle for task array */
static GSTask* gsTaskArray;         /* lbl_8047AC7C : resolved pointer to tasks  */
static u16     gsTaskStackHandle;   /* lbl_8047AC8C : GSmem handle for scheduler stack */
static void*   gsTaskStackPtr;      /* lbl_8047AC90 : scheduler stack base       */

/* --- Thread system globals --- */
static u32       gsThreadMaxCount;    /* lbl_8047AC30 : maximum thread count */
static u16       gsThreadArrayHandle; /* lbl_8047AC2C : GSmem handle for array */
static GSThread* gsThreadArray;       /* lbl_8047AC28 : resolved pointer */
static GSThread* gsThreadListHead;    /* lbl_8047AC08 : head of active thread list */
static u32       gsThreadFrameCount;  /* lbl_8047AC00 : frame counter */
static u8        gsThreadActive;      /* lbl_8047AC0C : flag indicating threads running */
static void*     gsThreadCurrentCtx;  /* lbl_8047AC1C : current thread context pointer */

/* =======================================================================
 *  GStaskInit / fn_800FE9B0
 *  Address: 0x800FE9B0, Size: 0xC4
 *
 *  Allocates the task array and scheduler stack from GSmem.
 *  Sets up the internal scheduler thread using fn_800A263C.
 *
 *  r3 = numTasks (normal-priority), r4 = numQueues (deferred)
 *
 *  Assembly:
 *    total = r3 + r4
 *    allocSize = total * 0x18 (sizeof GSTask)
 *    gsTaskMaxNormal = r3
 *    gsTaskMaxDeferred = r4
 *    gsTaskTotal = total
 *    gsTaskCurrentRun = NULL
 *    handle = GSmemAllocRaw(allocSize)
 *    gsTaskArrayHandle = handle
 *    gsTaskArray = GSmemGetPtr(handle)
 *    // Zero all task slots: store 0 at offset 0x08 of each 0x18-byte entry
 *    // Allocate 0x2000-byte scheduler stack
 *    stackHandle = GSmemAllocRaw(0x2000)
 *    gsTaskStackHandle = stackHandle
 *    gsTaskStackPtr = GSmemGetPtr(stackHandle)
 *    fn_800A263C(GStaskSchedulerThread, NULL, stackTop, 0x1FFC)
 *    fn_800D30A0(GStaskSwapCallback)
 * ======================================================================= */
void GStaskInit(u32 numTasks, u32 numQueues) {
    u32 total;
    u32 allocSize;
    u16 handle;
    s32 i;
    s32 loopCount;
    u32 offset;

    total = numTasks + numQueues;

    gsTaskMaxNormal   = numTasks;
    gsTaskMaxDeferred = numQueues;
    gsTaskTotal       = total;
    gsTaskCurrentRun  = NULL;

    /* Allocate task array: total * 24 bytes */
    allocSize = total * sizeof(GSTask);
    handle = GSmemAllocRaw(allocSize);
    gsTaskArrayHandle = handle;

    if ((handle & 0xFFFF) == 0) {
        return; /* allocation failed */
    }

    gsTaskArray = (GSTask*)GSmemGetPtr(handle & 0xFFFF);

    /* Zero the state field of every task slot */
    offset = 0;
    for (i = 0; i < total; i++) {
        /* Store 0 at offset 0x08 (state field) of each task */
        GSTask* task = (GSTask*)((u32)gsTaskArray + offset);
        task->state = GSTASK_FREE;
        offset += sizeof(GSTask);
    }

    /* Allocate a 0x2000-byte stack for the scheduler co-routine */
    handle = GSmemAllocRaw(0x2000);
    gsTaskStackHandle = handle;
    gsTaskStackPtr = GSmemGetPtr(handle & 0xFFFF);

    /* Create the scheduler fibre:
     * entry = GStaskSchedulerThread
     * arg   = NULL
     * stack = gsTaskStackPtr + 0x1FFC (top of 8KB stack)
     * size  = 0x1FFC */
    fn_800A263C((void*)GStaskSchedulerThread, NULL,
                (void*)((u32)gsTaskStackPtr + 0x1FFC), 0x1FFC);

    /* Register the swap-buffer callback with GSgfx */
    fn_800D30A0((void*)fn_800FEBA0);
}

/* =======================================================================
 *  GStaskCreate / fn_800FE834
 *  Address: 0x800FE834, Size: 0x17C
 *
 *  Creates a task and inserts it into the appropriate list.
 *
 *  r3 = state, r4 = priority, r5 = param, r6 = func
 *
 *  If state == 2 (DEFERRED), search starts from the end of the array
 *  (deferred slots); otherwise search from the beginning (normal slots).
 *
 *  The function finds the first free slot (state == 0), initialises it,
 *  then inserts it into the linked list in priority order.
 *
 *  Returns a 1-based task ID (index = (task - gsTaskArray) / 24 + 1).
 * ======================================================================= */
u32 GStaskCreate(u32 state, u8 priority, void* param, void* func) {
    GSTask* task;
    GSTask* search;
    u32 count;
    s32 i;
    s32 loopCount;

    /* Choose search range based on state */
    if (state == GSTASK_DEFERRED) {
        /* Deferred: start from the end of the normal region */
        task = (GSTask*)((u32)gsTaskArray + gsTaskMaxNormal * sizeof(GSTask));
        count = gsTaskMaxDeferred;
    } else {
        /* Normal: start from the beginning */
        task = gsTaskArray;
        count = gsTaskMaxNormal;
    }

    /* Find a free slot */
    for (i = 0; i < count; i++) {
        if (task->state == GSTASK_FREE) {
            goto found;
        }
        task = (GSTask*)((u32)task + sizeof(GSTask));
    }
    /* No free slot */
    return 0;

found:
    /* Initialise the task */
    task->prev     = NULL;
    task->next     = NULL;
    task->state    = state;
    task->priority = priority;
    task->paused   = 0;
    task->param    = param;
    task->func     = (void (*)(u32, void*))func;

    /* Insert into the appropriate linked list */
    if (gsTaskListHead == NULL) {
        /* First task in the list */
        gsTaskListHead = task;
    } else {
        OSDisableInterrupts();

        if (state == GSTASK_DEFERRED) {
            /* Insert into deferred list */
            task->next = gsTaskDeferredHead;
            gsTaskDeferredHead = task;
        } else {
            /* Insert into active list in priority order */
            GSTask* prev = NULL;
            GSTask* curr = gsTaskListHead;

            /* Walk until we find a task with priority >= ours */
            while (curr->next != NULL) {
                if (curr->priority >= task->priority) {
                    break;
                }
                prev = curr;
                curr = curr->next;
            }

            if (curr->next == NULL && curr->priority < task->priority) {
                /* Append at the end */
                task->prev = curr;
                task->next = NULL;
                curr->next = task;
            } else {
                /* Insert before curr */
                GSTask* prevOfCurr = curr->prev;
                if (prevOfCurr != NULL) {
                    prevOfCurr->next = task;
                }
                task->prev = curr->prev;
                task->next = curr;
                curr->prev = task;

                /* Update head if needed */
                if (gsTaskListHead == curr) {
                    gsTaskListHead = task;
                }
            }
        }

        OSRestoreInterrupts();
    }

    /* Compute 1-based task ID:
     * id = ((task - gsTaskArray) / sizeof(GSTask)) + 1
     * Assembly uses mulhwu with magic constant 0xAAAAAAAB for /24 */
    {
        u32 offset = (u32)task - (u32)gsTaskArray;
        u32 id = (offset / sizeof(GSTask)) + 1;
        return id;
    }
}

/* =======================================================================
 *  GStaskRun / fn_800FE7A0
 *  Address: 0x800FE7A0, Size: 0x94
 *
 *  Iterates the active task list and invokes each active, non-paused
 *  task's callback function.  This is the main cooperative yield point.
 *
 *  For each task where state == 1 (ACTIVE) and paused == 0:
 *    1. Store current task in gsTaskCurrentRun.
 *    2. Compute taskId = ((task - gsTaskArray) / 24) + 1
 *    3. Call task->func(taskId, task->param)
 *  After all tasks, clear gsTaskCurrentRun to NULL.
 * ======================================================================= */
void GStaskRun(void) {
    GSTask* task;
    GSTask* nextTask;

    task = gsTaskListHead;
    while (task != NULL) {
        nextTask = task->next;

        if (task->state == GSTASK_ACTIVE && task->paused == 0) {
            u32 taskId;
            u32 offset;

            gsTaskCurrentRun = task;

            /* Compute 1-based task ID */
            offset = (u32)task - (u32)gsTaskArray;
            taskId = (offset / sizeof(GSTask)) + 1;

            /* Invoke the callback via function pointer */
            task->func(taskId, task->param);
        }

        task = nextTask;
    }

    gsTaskCurrentRun = NULL;
}

/* =======================================================================
 *  GSthreadInit / GSthread
 *  Address: 0x800F09D8, Size: 0x9C
 *
 *  Allocates the thread array from GSmem and zeroes all entries.
 *
 *  r3 = maxThreads
 *
 *  Assembly:
 *    allocSize = maxThreads * 0x24 (sizeof GSThread)
 *    gsThreadMaxCount = maxThreads
 *    handle = GSmemAllocRaw(allocSize)
 *    gsThreadArrayHandle = handle
 *    gsThreadArray = GSmemGetPtr(handle)
 *    // Zero the 'active' byte at offset 0x08 of each 0x24-byte entry
 *    gsThreadFrameCount = 0
 *    gsThreadListHead = NULL
 *    Print "GSthread: Init OK, maximum of %d threads\n"
 * ======================================================================= */
void GSthreadInit(u32 maxThreads) {
    u32 allocSize;
    u16 handle;
    u32 i;
    u32 offset;

    gsThreadMaxCount = maxThreads;

    /* Allocate thread array: maxThreads * 36 bytes */
    allocSize = maxThreads * sizeof(GSThread);
    handle = GSmemAllocRaw(allocSize);
    gsThreadArrayHandle = handle;

    if ((handle & 0xFFFF) == 0) {
        return; /* allocation failed */
    }

    gsThreadArray = (GSThread*)GSmemGetPtr(handle & 0xFFFF);

    /* Zero the 'active' field of every thread slot */
    offset = 0;
    for (i = 0; i < maxThreads; i++) {
        GSThread* thr = (GSThread*)((u32)gsThreadArray + offset);
        thr->active = 0;
        offset += sizeof(GSThread);
    }

    /* Reset frame counter and thread list */
    gsThreadFrameCount = 0;
    gsThreadListHead   = NULL;

    /* Print init message */
    fn_800DD970(lbl_80271038, maxThreads);
}

/* =======================================================================
 *  GSthreadCreate / GSthreadCreate
 *  Address: 0x800F07A8, Size: 0x228
 *
 *  Creates a cooperative thread with its own GSmem-allocated stack.
 *
 *  r3 = affinity, r4 = priority, r5 = stackSize,
 *  r6 = usesFPU, r7 = autoStart, r8 = entryFunc
 *
 *  Assembly:
 *    // Warn if usesFPU == 0
 *    if (usesFPU == 0) fn_800DD970(lbl_80271008);
 *    // Find a free thread slot (active == 0)
 *    thread = NULL;
 *    for each slot in gsThreadArray:
 *      if slot->active == 0: thread = slot; break;
 *    if (thread == NULL) return NULL;
 *    // Allocate stack memory
 *    stackHandle = GSmemAlloc(stackSize, 0x20)  [32-byte aligned]
 *    thread->stackHandle = stackHandle
 *    // Allocate context block: 0x88 or 0x188 depending on usesFPU
 *    ctxSize = usesFPU ? 0x188 : 0x88
 *    ctxHandle = GSmemAllocRaw(ctxSize)
 *    thread->ctxHandle = ctxHandle
 *    // Initialise thread fields
 *    thread->active = 1
 *    thread->priority = priority
 *    thread->stackSize = stackSize
 *    thread->usesFPU = usesFPU
 *    thread->autoStart = autoStart
 *    thread->affinity = affinity
 *    thread->entryFunc = entryFunc
 *    // Resolve pointers
 *    ctx = GSmemGetPtr(ctxHandle)
 *    stack = GSmemGetPtr(stackHandle)
 *    gsThreadCurrentCtx = ctx
 *    // Init context
 *    threadSaveGPRRegisters()
 *    if (usesFPU) fn_800F01F0()
 *    // Set up stack frame
 *    ctx->stackPtr = stackSize - 8
 *    ctx->entryFunc = entryFunc
 *    ctx->trampoline = fn_800F0F4C
 *    stack->sentinel = -1  (stack guard)
 *    // Lock handles (increment refcount)
 *    GSmemLock(ctxHandle)
 *    GSmemLock(stackHandle)
 *    // Insert into priority-sorted thread list
 *    ... (same linked-list logic as tasks)
 *    gsThreadActive = 1
 *    return thread
 * ======================================================================= */
GSThread* GSthreadCreate(u32 affinity, u32 priority, u32 stackSize,
                          u32 usesFPU, u32 autoStart, void* entryFunc) {
    GSThread* thread;
    u16 stackHandle;
    u16 ctxHandle;
    u32 ctxSize;
    void* ctx;
    void* stack;
    u32 i;

    /* Warn if FPU context saving is disabled */
    if (usesFPU == 0) {
        fn_800DD970(lbl_80271008);
    }

    /* Find a free thread slot */
    thread = gsThreadArray;
    for (i = 0; i < gsThreadMaxCount; i++) {
        if (thread->active == 0) {
            goto found;
        }
        thread = (GSThread*)((u32)thread + sizeof(GSThread));
    }
    /* No free slots */
    return NULL;

found:
    /* Allocate stack from GSmem (32-byte aligned) */
    stackHandle = GSmemAlloc(stackSize, 0x20);
    thread->stackHandle = stackHandle;

    if ((stackHandle & 0xFFFF) == 0) {
        return NULL;
    }

    /* Allocate context block.
     * 0x88 bytes for base context, 0x188 if FPU state is included. */
    ctxSize = (usesFPU != 0) ? 0x188 : 0x88;
    ctxHandle = GSmemAllocRaw(ctxSize);
    thread->ctxHandle = ctxHandle;

    if ((ctxHandle & 0xFFFF) == 0) {
        GSmemFree(stackHandle);
        return NULL;
    }

    /* Initialise thread fields */
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

    /* Resolve GSmem handles to raw pointers */
    ctx   = GSmemGetPtr(ctxHandle & 0xFFFF);
    stack = GSmemGetPtr(stackHandle & 0xFFFF);

    gsThreadCurrentCtx = ctx;

    /* Initialise the thread's execution context */
    threadSaveGPRRegisters();
    if (usesFPU != 0) {
        fn_800F01F0(); /* set up FPU save area */
    }

    /* Set up the context's stack pointer and entry point:
     * ctx->stackPtr = stackSize - 8
     * ctx->entry    = entryFunc
     * ctx->trampoline = fn_800F0F4C  (common thread wrapper) */
    {
        u32* ctxWords = (u32*)ctx;
        ctxWords[1] = stackSize - 8;               /* offset 0x04 = stack ptr */
        ctxWords[0x20] = (u32)entryFunc;            /* offset 0x80 = entry    */
        ctxWords[0x21] = (u32)fn_800F0F4C;          /* offset 0x84 = trampoline */
    }

    /* Place a stack sentinel (-1) at the base of the stack */
    *(s32*)stack = -1;

    /* Lock the handles to prevent accidental free */
    GSmemLock(ctxHandle);
    GSmemLock(stackHandle);

    /* Insert into the priority-sorted thread list */
    {
        GSThread* prev = NULL;
        GSThread* curr = gsThreadListHead;

        if (curr == NULL) {
            gsThreadListHead = thread;
        } else {
            /* Walk until we find a thread with affinity >= ours */
            while (curr->next != NULL) {
                if (curr->affinity >= thread->affinity) {
                    break;
                }
                prev = curr;
                curr = curr->next;
            }

            if (curr->next == NULL && curr->affinity < thread->affinity) {
                /* Append at end */
                thread->prev = curr;
                thread->next = NULL;
                curr->next = thread;
            } else {
                /* Insert before curr */
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
    return thread;
}

/* =======================================================================
 *  GStaskSchedulerThread / fn_800FEA74  (INTERNAL)
 *  Address: 0x800FEA74, Size: 0x12C
 *
 *  The scheduler co-routine that runs inside the 8 KB scheduler stack.
 *  It loops forever, executing all active tasks each iteration (same
 *  logic as GStaskRun, but running as a fibre on its own stack).
 * ======================================================================= */
static void GStaskSchedulerThread(void) {
    GSTask* task;
    GSTask* nextTask;

    for (;;) {
        task = gsTaskListHead;
        while (task != NULL) {
            nextTask = task->next;

            if (task->state == GSTASK_DEFERRED && task->paused == 0) {
                u32 taskId;
                u32 offset;

                gsTaskCurrentRun = task;
                offset = (u32)task - (u32)gsTaskArray;
                taskId = (offset / sizeof(GSTask)) + 1;

                task->func(taskId, task->param);
            }

            task = nextTask;
        }

        /* Yield back to the main fibre (implementation is via
         * the cooperative switch in fn_800A263C -- effectively
         * a longjmp back to the caller's context). */
    }
}

/* ===================================================================
 * Generated: 1 pattern-matched + 61 stubs
 * Range: 0x800F8268 - 0x800FEBA0
 * =================================================================== */

/* Forward declarations for all asm-wrapped functions in this block */
extern void fn_800F8268(void);
extern void fn_800F8428(void);
extern void fn_800F8654(void);
extern void fn_800F8A54(void);
extern u32 fn_800F92D4(u32 key);
extern void fn_800F9670(u32 count);
extern void fn_800F96E4(void);
extern u32 fn_800F9AEC(void* outbuf, u16* src, s32 mode);
extern void fn_800F9D04(void);
extern u8* fn_800F9E70(u8* dst, u8* src);
extern void fn_800FA160(void* obj);
extern u32 fn_800FA444(void* obj);
extern void fn_800FAA98(void);
extern void fn_800FAEF8(void);
extern void fn_800FB43C(void);
extern void fn_800FB680(void);
extern void fn_800FB8C8(void);
extern void fn_800FBB34(void);
extern void fn_800FBF10(void);
extern void fn_800FBF74(void);
extern void fn_800FC2A4(void);
extern u32 fn_800FC2A8(void* ptr);
extern void* fn_800FC39C(void* ptr);
extern s32 fn_800FC518(u32 val);
extern void fn_800FC528(void);
extern s32 fn_800FC7E0(void* entry, u8 type, u32 arg);
extern void fn_800FD348(void);
extern void fn_800FD69C(void);
extern void* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(void* obj, u32 key, void** outNode);
extern s32 fn_800FDFE4(const void* str);
extern s32 fn_800FE010(const void* str);
extern void fn_800FE35C(void);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern void fn_800FE4D4(void);
extern void fn_800FE6A0(f32 a, f32 b);
extern void fn_800FE6AC(s16* outA, s16* outB);
extern void fn_800FE6D0(s32 a, s32 b);
extern void fn_800FE6DC(u32 taskId);
extern void fn_800FE6F8(u32 taskId);
extern void fn_800FE714(u32 taskId);
extern void fn_800FE7A0(void);
extern u32 fn_800FE834(s32 state, u8 priority, void* param, void* func);
extern void fn_800FE9B0(u32 numTasks, u32 numQueues);
extern void fn_800FEA74(void);

/* 0x800F8268 | 0x1C0 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F8268(void) {
#include "src/game/gs_thread_fn_800F8268.inc"
}
#else
#pragma optimization_level 2
void fn_800F8268(void) {
    u8 pad[0x50];
    u8* r31;
    u8* r30;
    u8* r28;
    s32 r29;
    u32 btnState;
    s8 ax;

    fn_800AB150(pad);
    r31 = (u8*)&lbl_80401C10;
    r30 = pad;
    for (r29 = 0; r29 < 4; r29++) {
        s32 target = r29 + 1;
        if (*(s32*)r31 == target) {
            r28 = r31;
        } else {
            r28 = r31 + 0x6C;
            if (*(s32*)(r31 + 0x6C) != target) {
                r28 += 0x6C;
                if (*(s32*)(r28) != target) {
                    r28 += 0x6C;
                    if (*(s32*)(r28) != target) {
                        r28 = NULL;
                    }
                }
            }
        }
        if (r28 == NULL) goto next;
        ax = (s8)r30[0xA];
        if (ax == -1) {
            btnState = fn_800D0F44((u32)r29);
            if (btnState == 8) {
                *(u32*)(r28 + 0xC) = 3;
            } else if (btnState == 0x40) {
                *(u32*)(r28 + 0xC) = 4;
            }
            memset(r28 + 0x18, 0, 0xC);
            lbl_8047AC4C = lbl_8047AC4C | ((u32)0x80000000 >> (u32)r29);
        } else if (ax >= 0 && ax < 1) {
            if (*(u32*)(r28 + 0xC) == 3) {
                btnState = fn_800D0F44((u32)r29);
                if (btnState == 0x00900000) {
                    *(u32*)(r28 + 0x4) = 0;
                } else {
                    *(u32*)(r28 + 0x4) = 2;
                }
                *(u32*)(r28 + 0xC) = 0;
            }
            r30[0x3] = (u8)(-(s8)r30[0x3]);
            r30[0x5] = (u8)(-(s8)r30[0x5]);
            memcpy(r28 + 0x18, r30, 0xC);
            lbl_8047AC4C = lbl_8047AC4C & ~((u32)0x80000000 >> (u32)r29);
        }
next:
        r30 += 0xC;
    }
    fn_800F8428();
    lbl_8047AC48++;
}
#endif
#pragma pop

/* 0x800F8428 | 0x22C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800F8428(void) {
#include "src/game/gs_thread_fn_800F8428.inc"
}
#else
void fn_800F8428(void) {
    /* TODO: match -- 556 bytes at 0x800F8428 */
}
#endif
#pragma pop

/* 0x800F8654 | 0x400 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800F8654(void) {
#include "src/game/gs_thread_fn_800F8654.inc"
}
#else
void fn_800F8654(void) {
    /* TODO: match -- 1024 bytes at 0x800F8654 */
}
#endif
#pragma pop

/* 0x800F8A54 | 0x708 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800F8A54(void) {
#include "src/game/gs_thread_fn_800F8A54.inc"
}
#else
void fn_800F8A54(void) {
    /* TODO: match -- 1800 bytes at 0x800F8A54 */
}
#endif
#pragma pop

/* 0x800F915C | 0xB4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F915C(void) {
#include "src/game/gs_thread_fn_800F915C.inc"
}
#else
#pragma optimization_level 2
void fn_800F915C(u32 key) {
    u8* entry;
    u32 i;
    u8 skip;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) == 0 || *(u32*)(entry + 0x8) != key) continue;
        skip = 0;
        if (*(u32*)(entry + 0x10) != 0) {
            if ((u8)((u32 (*)(u32))(*(u32*)(entry + 0x10)))(*(u32*)(entry + 0xC)) == 0) {
                skip = 1;
            }
        }
        if (!skip) {
            if (*(u16*)(entry + 0x0) != 0) {
                GSmemLock(*(u16*)(entry + 0x0));
                GSmemFree(*(u16*)(entry + 0x0));
                *(u16*)(entry + 0x0) = 0;
            }
            *(u32*)(entry + 0x4) = 0;
        }
    }
}
#endif
#pragma pop

/* 0x800F9210 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9210(void) {
#include "src/game/gs_thread_fn_800F9210.inc"
}
#else
#pragma optimization_level 2
void fn_800F9210(u32 key1, u32 key2) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key1 && *(u32*)(entry + 0xC) == key2) {
            if (*(u32*)(entry + 0x10) != 0) {
                if ((u8)((u32 (*)(u32, u32, u32))(*(u32*)(entry + 0x10)))(*(u32*)(entry + 0x4), *(u32*)(entry + 0x8), *(u32*)(entry + 0xC)) == 0) {
                    return;
                }
            }
            if (*(u16*)(entry + 0x0) != 0) {
                GSmemLock(*(u16*)(entry + 0x0));
                GSmemFree(*(u16*)(entry + 0x0));
                *(u16*)(entry + 0x0) = 0;
            }
            *(u32*)(entry + 0x4) = 0;
            return;
        }
    }
}
#endif
#pragma pop

/* 0x800F92D4 | 0x44 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F92D4(void) {
#include "src/game/gs_thread_fn_800F92D4.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F92D4(u32 key) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0xC) == key)
            return *(u32*)(entry + 0x4);
    }
    return 0;
}
#endif
#pragma pop

/* 0x800F9318 | 0x60 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9318(void) {
#include "src/game/gs_thread_fn_800F9318.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 2
u32 fn_800F9318(u32 key1, u32 key2) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key1 && *(u32*)(entry + 0xC) == key2)
            return *(u32*)(entry + 0x4);
    }
    return 0;
}
#pragma pop
#endif
#pragma pop

/* 0x800F9378 | 0xA0 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9378(void) {
#include "src/game/gs_thread_fn_800F9378.inc"
}
#else
#pragma optimization_level 2
void fn_800F9378(u32 fn, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u32 i;

    arr = (u8*)lbl_8047AC5C;
    i = lbl_8047AC60;
    p = arr;
    for (; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return;
    }
    p = arr;
    i = lbl_8047AC60;
    for (; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) == 0) break;
    }
    if (i == 0) return;
    *(u16*)(p + 0x0) = 0;
    *(u32*)(p + 0x4) = fn;
    *(u32*)(p + 0x8) = key1;
    *(u32*)(p + 0xC) = key2;
    *(u32*)(p + 0x10) = val;
}
#endif
#pragma pop

/* 0x800F9418 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9418(void) {
#include "src/game/gs_thread_fn_800F9418.inc"
}
#else
#pragma optimization_level 2
void* fn_800F9418(u32 align, u32 size, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u8* slot;
    u32 i;
    u32 count;

    arr = (u8*)lbl_8047AC5C;
    count = lbl_8047AC60;
    p = arr;
    for (i = count; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return NULL;
    }
    slot = arr;
    for (i = count; i > 0; i--, slot += 0x14) {
        if (*(u32*)(slot + 0x4) == 0) break;
    }
    if (i == 0) return NULL;
    *(u16*)slot = (u16)GSmemAlloc(align, size);
    if (*(u16*)slot == 0) return NULL;
    *(u32*)(slot + 0x4) = (u32)GSmemGetPtr(*(u16*)slot);
    if (*(u32*)(slot + 0x4) == 0) {
        GSmemFree(*(u16*)slot);
        return NULL;
    }
    *(u32*)(slot + 0x8) = key1;
    *(u32*)(slot + 0xC) = key2;
    *(u32*)(slot + 0x10) = val;
    return (void*)*(u32*)(slot + 0x4);
}
#endif
#pragma pop

/* 0x800F9544 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9544(void) {
#include "src/game/gs_thread_fn_800F9544.inc"
}
#else
#pragma optimization_level 2
void* fn_800F9544(u32 size, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u8* slot;
    u32 i;
    u32 count;

    arr = (u8*)lbl_8047AC5C;
    count = lbl_8047AC60;
    p = arr;
    for (i = count; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return NULL;
    }
    slot = arr;
    for (i = count; i > 0; i--, slot += 0x14) {
        if (*(u32*)(slot + 0x4) == 0) break;
    }
    if (i == 0) return NULL;
    *(u16*)slot = (u16)GSmemAllocRaw(size);
    if (*(u16*)slot == 0) return NULL;
    *(u32*)(slot + 0x4) = (u32)GSmemGetPtr(*(u16*)slot);
    if (*(u32*)(slot + 0x4) == 0) {
        GSmemFree(*(u16*)slot);
        return NULL;
    }
    *(u32*)(slot + 0x8) = key1;
    *(u32*)(slot + 0xC) = key2;
    *(u32*)(slot + 0x10) = val;
    return (void*)*(u32*)(slot + 0x4);
}
#endif
#pragma pop

/* 0x800F9670 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9670(void) {
#include "src/game/gs_thread_fn_800F9670.inc"
}
#else
#pragma optimization_level 2
void fn_800F9670(u32 count) {
    u16 handle;
    u32 off;
    u32 i;

    lbl_8047AC60 = count;
    handle = GSmemAllocRaw(count * 0x14);
    lbl_8047AC58 = handle;
    if ((handle & 0xFFFF) == 0) return;
    lbl_8047AC5C = (u32)GSmemGetPtr(handle);
    off = 0;
    i = 0;
    while (i < lbl_8047AC60) {
        u32 storeoff = off + 0x4;
        off += 0x14;
        i++;
        *(u32*)(lbl_8047AC5C + storeoff) = 0;
    }
}
#endif
#pragma pop

/* 0x800F96E4 | 0x408 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800F96E4(void) {
#include "src/game/gs_thread_fn_800F96E4.inc"
}
#else
void fn_800F96E4(void) {
    /* TODO: match -- 1032 bytes at 0x800F96E4 */
}
#endif
#pragma pop

/* 0x800F9AEC | 0x118 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9AEC(void) {
#include "src/game/gs_thread_fn_800F9AEC.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F9AEC(void* outbuf, u16* src, s32 mode) {
    u8* out;
    register u16* table;
    register s32 idx;
    register u32 count;
    register u16* p;
    register u32 ch;
    u32 outch;

    out = (u8*)outbuf;

    switch (mode) {
    case 1:
        break;
    case 7:
    case 9:
    default:
        goto use_second_table;
    }

    count = 0;
    if (src != NULL) goto first_have_src;
    goto done_first;
first_have_src:
        table = (u16*)lbl_80271300;
        goto first_cond;
first_loop:
        p = table;
        idx = 0;
        goto first_scan_check;
first_scan_next:
        idx++;
        p++;
        if (idx >= 0x100) {
            idx = 0xb7;
            goto first_found;
        }
first_scan_check:
        if ((u32)ch != *p) goto first_scan_next;
first_found:
        if (out != NULL) {
            outch = (u8)idx;
            *out = outch;
            out++;
        }
        count++;
        src++;
first_cond:
        ch = *src;
        if (ch != 0) goto first_loop;
done_first:
    return count;

use_second_table:
    count = 0;
    if (src != NULL) goto second_have_src;
    goto done_second;
second_have_src:
    table = (u16*)lbl_80271500;
    goto second_cond;
second_loop:
    p = table;
    idx = 0;
    goto second_scan_check;
second_scan_next:
    idx++;
    p++;
    if (idx >= 0x100) {
        idx = 0xb7;
        goto second_found;
    }
second_scan_check:
    if ((u32)ch != *p) goto second_scan_next;
second_found:
    if (out != NULL) {
        outch = (u8)idx;
        *out = outch;
        out++;
    }
    count++;
    src++;
second_cond:
    ch = *src;
    if (ch != 0) goto second_loop;
done_second:
    return count;
}
#endif
#pragma pop

/* 0x800F9C04 | 0x100 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9C04(void) {
#include "src/game/gs_thread_fn_800F9C04.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F9C04(void* outbuf, u8* src, u32 count, u32 mode) {
    u16* out;
    u16* table;
    u32 total;
    u8 b;

    out = (u16*)outbuf;
    total = 0;

    if (mode == 1) {
        if (src == NULL) {
            if (out != NULL) *out = 0;
            return total;
        }
        table = (u16*)lbl_80271300;
        while (count > 0 && (b = *src) != 0xFF) {
            if (out != NULL) {
                *out = table[b];
                out++;
            }
            total++;
            src++;
            count--;
        }
        if (out != NULL) *out = 0;
    } else {
        if (src == NULL) {
            if (out != NULL) *out = 0;
            return total;
        }
        table = (u16*)lbl_80271500;
        while (count > 0 && (b = *src) != 0xFF) {
            if (out != NULL) {
                *out = table[b];
                out++;
            }
            total++;
            src++;
            count--;
        }
        if (out != NULL) *out = 0;
    }
    return total;
}
#endif
#pragma pop

/* 0x800F9D04 | 0x20 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9D04(void) {
#include "src/game/gs_thread_fn_800F9D04.inc"
}
#else
#pragma optimization_level 2
void fn_800F9D04(void) {
    fn_80080ED8();
}
#endif
#pragma pop

/* 0x800F9D24 | 0x14C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9D24(void) {
#include "src/game/gs_thread_fn_800F9D24.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 2
void* fn_800F9D24(u16* dst, u16* src, s32 maxlen) {
    s32 r;
    s32 i;

    if (maxlen <= 0) return dst;
    r = (fn_800FE010(src) + 1) >> 1;
    if (r >= maxlen) r = maxlen - 1;
    memcpy(dst, src, r * 2);
    i = r;
    r = maxlen - r;
    if (r <= 0) return dst;
    while (r > 0) {
        dst[i] = 0;
        i++;
        r--;
    }
    return dst;
}
#pragma pop
#endif
#pragma pop

/* 0x800F9E70 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9E70(void) {
#include "src/game/gs_thread_fn_800F9E70.inc"
}
#else
#pragma peephole off
u8* fn_800F9E70(u8* dst, u8* src) {
    extern u32 fn_800FE010(u8* a);
    if (dst == NULL) { return NULL; }
    if (src == NULL) { *(u16*)dst = 0; }
    else { memcpy(dst, src, fn_800FE010(src)); }
    return dst;
}
#pragma peephole on
#endif
#pragma pop

/* 0x800F9EE4 | 0x180 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9EE4(void) {
#include "src/game/gs_thread_fn_800F9EE4.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F9EE4(void* str1, void* str2) {
    s32 len1;
    s32 len2;
    u16* p1;
    u16* p2;
    u32 i;
    u16 c1;
    u16 c2;
    s32 n;

    len1 = fn_800FDFE4(str1);
    len2 = fn_800FDFE4(str2);

    if (len1 == len2) {
        p1 = (u16*)str1;
        p2 = (u16*)str2;
        i = 0;
        n = len1;
        while (n > 0) {
            c1 = *p1;
            c2 = *p2;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return 0;
    } else if (len1 > len2) {
        p1 = (u16*)str2;
        p2 = (u16*)str1;
        i = 0;
        n = len2;
        while (n > 0) {
            c1 = *p2;
            c2 = *p1;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return 1;
    } else {
        p1 = (u16*)str2;
        p2 = (u16*)str1;
        i = 0;
        n = len1;
        while (n > 0) {
            c1 = *p2;
            c2 = *p1;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return -1;
    }
}
#endif
#pragma pop

/* 0x800FA064 | 0xFC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA064(void) {
#include "src/game/gs_thread_fn_800FA064.inc"
}
#else
#pragma optimization_level 2
void fn_800FA064(void* obj) {
    u8* o;
    u32 rv;
    s16 r5;
    u8 type;
    f32 base;
    f64 bias;

    o = (u8*)obj;
    if (*(s16*)(o + 0x18) == 0) return;
    rv = fn_800FA444(*(void**)(o + 0x1C));
    r5 = (s16)(rv >> 16);
    type = *(u8*)(o + 0x4A);
    base = *(f32*)(o + 0x4);
    bias = lbl_8047CD10;

    if (type == 0) {
        *(f32*)(o + 0xC) = base;
    } else if (type == 1) {
        s16 target;
        s32 diff;
        target = *(s16*)(o + 0x18);
        diff = (s32)(target - r5);
        diff = (diff + (diff >> 31)) >> 1;
        *(f32*)(o + 0xC) = base + (f32)((f64)diff - bias + bias);
    } else if (type < 4) {
        s16 target;
        target = *(s16*)(o + 0x18);
        *(f32*)(o + 0xC) = base + (f32)target - (f32)r5;
    }
}
#endif
#pragma pop

/* 0x800FA160 | 0x5C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA160(void) {
#include "src/game/gs_thread_fn_800FA160.inc"
}
#else
#pragma optimization_level 2
#pragma peephole off
void fn_800FA160(void* obj) {
    u8 clr[8];
    u32 init = lbl_8047CD04;
    u32 color = *(u32*)((u8*)obj + 0x24);
    *(u32*)(&clr[4]) = init;
    clr[4] = (u8)(color >> 24);
    clr[5] = (u8)((color >> 16) & 0xFF);
    clr[6] = (u8)((color >> 8) & 0xFF);
    clr[7] = (u8)(color & 0xFF);
    *(u32*)(&clr[0]) = *(u32*)(&clr[4]);
    fn_800DBEB4(0, &clr[0]);
}
#pragma peephole on
#endif
#pragma pop

/* 0x800FA1BC | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA1BC(void) {
#include "src/game/gs_thread_fn_800FA1BC.inc"
}
#else
#pragma optimization_level 2
void fn_800FA1BC(void* obj) {
    u8* o;
    u8* head;
    u32 count;
    u32 offset;
    u8* arr;
    u8* entry;
    u16 val;

    o = (u8*)obj;
    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    if ((s32)count <= 0) return;
    offset = 0;
    do {
        arr = (u8*)*(u32*)(head + 0x24);
        entry = arr + offset;
        if (*(u16*)entry == *(u16*)(o + 0x20)) {
            *(u8*)(o + 0x22) = entry[2];
            *(u8*)(o + 0x23) = entry[3];
            val = *(u16*)(o + 0x20);
            if (val == 0) {
                *(u8*)(o + 0x42) = 0xB;
                return;
            } else if (val == 1) {
                *(u8*)(o + 0x42) = 6;
                return;
            } else {
                *(s8*)(o + 0x42) = (s8)(s32)(lbl_8047CD20 * ((f64)(u32)entry[3] - lbl_8047CD28) + lbl_8047CD18);
                return;
            }
        }
        offset += 8;
        count--;
    } while (count > 0);
}
#endif
#pragma pop

/* 0x800FA280 | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA280(void) {
#include "src/game/gs_thread_fn_800FA280.inc"
}
#else
#pragma optimization_level 2
void* fn_800FA280(u32 key) {
    u8* head;
    u16 group;
    u32 sub;
    u8* node;
    u32 lo;
    u32 hi;
    u32 mid;
    u32 count;
    u8* arr;
    u32 val;

    if (key == 0) return NULL;

    head = (u8*)lbl_80478B08;
    group = (u16)(key >> 20);
    sub = key & 0xFFFFF;

    node = (u8*)*(u32*)(head + 0x8);
    while (node != NULL) {
        if (*(u16*)(node + 0x0) == group) {
            count = *(u16*)(node + 0x4);
            arr = node + 0x10;
            lo = 0;
            hi = count;
            while (lo < hi) {
                mid = (lo + hi) >> 1;
                val = *(u32*)(arr + mid * 8);
                if (val == sub) {
                    u32 offset = *(u32*)(arr + mid * 8 + 4);
                    return node + offset;
                }
                if (val < sub) lo = mid + 1;
                else hi = mid;
            }
        }
        node = (u8*)*(u32*)(node + 0x8);
    }
    return NULL;
}
#endif
#pragma pop

/* 0x800FA314 | 0xBC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA314(void) {
#include "src/game/gs_thread_fn_800FA314.inc"
}
#else
#pragma optimization_level 2
s32 fn_800FA314(u32 key) {
    u8* head;
    u16 group;
    u32 sub;
    u8* node;
    u32 lo;
    u32 hi;
    u32 mid;
    u32 count;
    u8* arr;
    u32 val;
    void* result;

    if (key == 0) return fn_800FDFE4(NULL);

    head = (u8*)lbl_80478B08;
    group = (u16)(key >> 20);
    sub = key & 0xFFFFF;

    node = (u8*)*(u32*)(head + 0x8);
    result = NULL;
    while (node != NULL) {
        if (*(u16*)(node + 0x0) == group) {
            count = *(u16*)(node + 0x4);
            arr = node + 0x10;
            lo = 0;
            hi = count;
            while (lo < hi) {
                mid = (lo + hi) >> 1;
                val = *(u32*)(arr + mid * 8);
                if (val == sub) {
                    u32 offset = *(u32*)(arr + mid * 8 + 4);
                    result = node + offset;
                    return fn_800FDFE4(result);
                }
                if (val < sub) lo = mid + 1;
                else hi = mid;
            }
        }
        node = (u8*)*(u32*)(node + 0x8);
    }
    return fn_800FDFE4(NULL);
}
#endif
#pragma pop

/* 0x800FA3D0 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FA3D0(void) {
#include "src/game/gs_thread_fn_800FA3D0.inc"
}
#else
#pragma optimization_level 2
s32 fn_800FA3D0(u32 key) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;

    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        entry = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(entry + 0x0) != 0 && *(u32*)(entry + 0x1C) == key) {
            if (*(u8*)(entry + 0x0) == 1) return 1;
            return 0;
        }
    }
    return 0;
}
#endif
#pragma pop

/* 0x800FA444 | 0x654 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm u32 fn_800FA444(void* obj) {
#include "src/game/gs_thread_fn_800FA444.inc"
}
#else
u32 fn_800FA444(void* obj) {
    /* TODO: match -- 1620 bytes at 0x800FA444 */
}
#endif
#pragma pop

/* 0x800FAA98 | 0x460 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FAA98(void) {
#include "src/game/gs_thread_fn_800FAA98.inc"
}
#else
void fn_800FAA98(void) {
    /* TODO: match -- 1120 bytes at 0x800FAA98 */
}
#endif
#pragma pop

/* 0x800FAEF8 | 0x544 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FAEF8(void) {
#include "src/game/gs_thread_fn_800FAEF8.inc"
}
#else
void fn_800FAEF8(void) {
    /* TODO: match -- 1348 bytes at 0x800FAEF8 */
}
#endif
#pragma pop

/* 0x800FB43C | 0x244 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FB43C(void) {
#include "src/game/gs_thread_fn_800FB43C.inc"
}
#else
void fn_800FB43C(void) {
    /* TODO: match -- 580 bytes at 0x800FB43C */
}
#endif
#pragma pop

/* 0x800FB680 | 0x248 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FB680(void) {
#include "src/game/gs_thread_fn_800FB680.inc"
}
#else
void fn_800FB680(void) {
    /* TODO: match -- 584 bytes at 0x800FB680 */
}
#endif
#pragma pop

/* 0x800FB8C8 | 0x26C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FB8C8(void) {
#include "src/game/gs_thread_fn_800FB8C8.inc"
}
#else
void fn_800FB8C8(void) {
    /* TODO: match -- 620 bytes at 0x800FB8C8 */
}
#endif
#pragma pop

/* 0x800FBB34 | 0x254 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FBB34(void) {
#include "src/game/gs_thread_fn_800FBB34.inc"
}
#else
void fn_800FBB34(void) {
    /* TODO: match -- 596 bytes at 0x800FBB34 */
}
#endif
#pragma pop

/* 0x800FBD88 | 0xF4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FBD88(void) {
#include "src/game/gs_thread_fn_800FBD88.inc"
}
#else
#pragma optimization_level 2
void fn_800FBD88(u32 key) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;
    u8 type;
    u32 r3;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;
    entry = NULL;
    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        u8* e = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(e + 0x0) != 0 && *(u32*)(e + 0x1C) == key) {
            entry = e;
            break;
        }
    }
    if (entry == NULL) return;
    type = *(u8*)(entry + 0x3);
    r3 = 0;
    if (type == 1) r3 = 0x57;
    else if (type == 2) r3 = 0x58;
    else if (type == 3) r3 = 0x59;
    else if (type == 4) r3 = 0x497;
    else if (type == 5) r3 = 0x498;
    if (r3 != 0) fn_801669BC(r3);
    *(u8*)(entry + 0x0) = 0;
}
#endif
#pragma pop

/* 0x800FBE7C | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FBE7C(void) {
#include "src/game/gs_thread_fn_800FBE7C.inc"
}
#else
#pragma optimization_level 2
s32 fn_800FBE7C(u32 key, u32 r4arg) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;
    entry = NULL;
    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        u8* e = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(e + 0x0) != 0 && *(u32*)(e + 0x1C) == key) {
            entry = e;
            break;
        }
    }
    if (entry == NULL) return -1;
    return fn_800FC7E0(entry, *(u8*)(entry + 0x44), r4arg);
}
#endif
#pragma pop

/* 0x800FBF10 | 0x64 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FBF10(void) {
#include "src/game/gs_thread_fn_800FBF10.inc"
}
#else
void fn_800FBF10(void) {
    extern u32 lbl_80478B08;
    extern void fn_800EF504(u32 val);
    u8* ptr;
    s8 idx;
    ptr = (u8*)lbl_80478B08;
    idx = (s8)ptr[0x1d];
    ptr += (s32)idx * 4;
    fn_800EF504(*(u32*)(ptr + 0xc));
    *(u16*)((u8*)lbl_80478B08 + 0x18) = 2;
    *(u16*)((u8*)lbl_80478B08 + 0x1a) = 1;
    ptr = (u8*)lbl_80478B08;
    idx = (s8)(ptr[0x1d] ^ 1);
    ptr[0x1d] = (u8)idx;
}
#endif
#pragma pop

/* 0x800FBF74 | 0x25C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FBF74(void) {
#include "src/game/gs_thread_fn_800FBF74.inc"
}
#else
void fn_800FBF74(void) {
    /* TODO: match -- 604 bytes at 0x800FBF74 */
}
#endif
#pragma pop

/* 0x800FC1D0 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FC1D0(void) {
#include "src/game/gs_thread_fn_800FC1D0.inc"
}
#else
#pragma optimization_level 2
s32 fn_800FC1D0(u32* item) {
    u32* head;
    u32* p;

    head = (u32*)lbl_80478B08;
    if ((u32*)head[2] == NULL) return -1;
    p = (u32*)head[2];
    while (p != NULL) {
        if (p == item) {
            if (p[3] != 0) ((u32*)p[3])[2] = p[2];
            else head[2] = p[2];
            if (p[2] != 0) ((u32*)p[2])[3] = p[3];
            break;
        }
        p = (u32*)p[2];
    }
    return 0;
}
#endif
#pragma pop

/* 0x800FC244 | 0x60 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FC244(void) {
#include "src/game/gs_thread_fn_800FC244.inc"
}
#else
#pragma optimization_level 2
void fn_800FC244(u32* item) {
    u32* head;
    u32* p;

    head = (u32*)lbl_80478B08;
    p = (u32*)head[2];
    if (p == NULL) {
        head[2] = (u32)item;
        item[2] = 0;
        item[3] = 0;
        return;
    }
    while (1) {
        if (p == item) return;
        if (p[2] == 0) {
            p[2] = (u32)item;
            item[2] = 0;
            item[3] = (u32)p;
            return;
        }
        p = (u32*)p[2];
    }
}
#endif
#pragma pop

/* 0x800FC2A4 | 0x4 | void_stub */
#if 0
asm void fn_800FC2A4(void) {
#include "src/game/gs_thread_fn_800FC2A4.inc"
}
#else
void fn_800FC2A4(void) {
}
#endif

/* 0x800FC2A8 | 0xF4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FC2A8(void* ptr) {
#include "src/game/gs_thread_fn_800FC2A8.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 fn_800FC2A8(void* ptr) {
    u8* p;
    register s32 offset;
    register u8* head;
    register s32 count;
    register u8* entry;
    register u8* nodePrev;
    register u8* node;
    register s32 idx;

    p = (u8*)ptr;
tail:
    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    idx = 0;
    offset = 0;
    for (; idx < count; offset += 8, idx++) {
        entry = (u8*)*(u32*)(head + 0x24) + offset;
        if (*(u32*)(entry + 0x4) != 0) {
            if (*(u16*)entry == *(u16*)p) break;
        }
    }
    if (idx != count) {
        node = (u8*)*(u32*)(entry + 0x4);
        while (node != NULL) {
            if (node == p + 8) {
                nodePrev = (u8*)*(u32*)(node + 0xC);
                if (nodePrev == NULL && *(u32*)(node + 0x8) == 0) {
                    *(u16*)entry = 0xFFFF;
                    *(u32*)(entry + 0x4) = 0;
                } else {
                    if (nodePrev != NULL) {
                        *(u32*)(nodePrev + 0x8) = *(u32*)(node + 0x8);
                    } else {
                        *(u32*)(entry + 0x4) = *(u32*)(node + 0x8);
                    }
                    if (*(u32*)(node + 0x8) != 0) {
                        *(u32*)(*(u32*)(node + 0x8) + 0xC) = *(u32*)(node + 0xC);
                    }
                }
                break;
            }
            node = (u8*)*(u32*)(node + 0x8);
        }
    }
    if (*(u32*)(p + 0x4) != 0) {
        p += *(u32*)(p + 0x4);
        goto tail;
    }
    return 0;
}
#endif
#pragma pop

/* 0x800FC39C | 0x17C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void* fn_800FC39C(void* ptr) {
#include "src/game/gs_thread_fn_800FC39C.inc"
}
#else
#pragma optimization_level 2
void* fn_800FC39C(void* ptr) {
    u8* p;
    u8* orig;
    u8* head;
    u8* entry;
    u8* node;
    u8* node2;
    u16 key;
    u16 count;
    u16 idx;
    u32 offset;

    orig = (u8*)ptr;
    p = orig;
loop:
    key = *(u16*)p;
    if (key == 0xFFFF) return NULL;

    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    idx = 0;
    offset = 0;
    entry = NULL;
    while (count > 0) {
        entry = (u8*)*(u32*)(head + 0x24) + offset;
        if (*(u32*)(entry + 0x4) != 0) {
            if (*(u16*)entry == key) break;
        }
        offset += 8;
        idx++;
        count--;
        entry = NULL;
    }
    if (entry == NULL) {
        /* No matching occupied slot. Find free slot for new key. */
        count = *(u16*)(head + 0x4);
        idx = 0;
        offset = 0;
        entry = NULL;
        while (count > 0) {
            entry = (u8*)*(u32*)(head + 0x24) + offset;
            if (*(u32*)(entry + 0x4) == 0) {
                /* Insert into free slot */
                *(u32*)(entry + 0x0) = *(u32*)(p + 0x0);
                *(u32*)(entry + 0x4) = (u32)(p + 0x8);
                *(u32*)(p + 0x8 + 0x8) = 0;
                *(u32*)(p + 0x8 + 0xC) = 0;
                break;
            }
            offset += 8;
            idx++;
            count--;
            entry = NULL;
        }
        if (entry == NULL) {
            fn_800DD970((const char*)lbl_8027177C, *(u16*)p);
        }
    } else {
        /* Found occupied slot with matching key; insert node into list */
        node = p + 8;
        node2 = (u8*)*(u32*)(entry + 0x4);
        while (1) {
            if (node2 == node) return NULL;
            if (*(u32*)(node2 + 0x8) == 0) {
                *(u32*)(node2 + 0x8) = (u32)node;
                *(u32*)(node + 0x8) = 0;
                *(u32*)(node + 0xC) = (u32)node2;
                break;
            }
            node2 = (u8*)*(u32*)(node2 + 0x8);
        }
    }
    if (*(u32*)(p + 0x4) != 0) {
        p += *(u32*)(p + 0x4);
        goto loop;
    }
    return orig;
}
#endif
#pragma pop

/* 0x800FC518 | 0x10 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FC518(void) {
#include "src/game/gs_thread_fn_800FC518.inc"
}
#else
#pragma optimization_level 2
s32 fn_800FC518(u32 val) {
    *(u32*)((u8*)lbl_80478B08 + 0x28) = val;
    return 0;
}
#endif
#pragma pop

/* 0x800FC528 | 0x2B8 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FC528(void) {
#include "src/game/gs_thread_fn_800FC528.inc"
}
#else
void fn_800FC528(void) {
    /* TODO: match -- 696 bytes at 0x800FC528 */
}
#endif
#pragma pop

/* 0x800FC7E0 | 0xB68 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm s32 fn_800FC7E0(void* entry, u8 type, u32 arg) {
#include "src/game/gs_thread_fn_800FC7E0.inc"
}
#else
s32 fn_800FC7E0(void* entry, u8 type, u32 arg) {
    /* TODO: match -- 2920 bytes at 0x800FC7E0 */
    return 0;
}
#endif
#pragma pop

/* 0x800FD348 | 0x354 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FD348(void) {
#include "src/game/gs_thread_fn_800FD348.inc"
}
#else
void fn_800FD348(void) {
    /* TODO: match -- 852 bytes at 0x800FD348 */
}
#endif
#pragma pop

/* 0x800FD69C | 0x880 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FD69C(void) {
#include "src/game/gs_thread_fn_800FD69C.inc"
}
#else
void fn_800FD69C(void) {
    /* TODO: match -- 2176 bytes at 0x800FD69C */
}
#endif
#pragma pop

/* 0x800FDF1C | 0xC8 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(void* obj, u32 key, void** outNode) {
#include "src/game/gs_thread_fn_800FDF1C.inc"
}
#else
#pragma optimization_level 2
void* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(void* obj, u32 key, void** outNode) {
    u8* head;
    u32 count;
    u32 offset;
    u32 idx;
    u8* entry;
    u8* e;
    u8* node;
    u32 lo;
    u32 hi;
    u32 mid;
    u16 nodeCount;
    u8* arr;
    u16 ek;

    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    offset = 0;
    idx = 0;
    entry = NULL;
    while ((s32)count > 0 && idx < count) {
        e = (u8*)*(u32*)(head + 0x24) + offset;
        if (*(u16*)e == *(u16*)((u8*)obj + 0x20)) {
            entry = e;
            break;
        }
        offset += 8;
        idx++;
    }
    if (idx == count) return NULL;

    node = (u8*)*(u32*)(entry + 0x4);
    key = key & 0xFFFF;
    while (node != NULL) {
        nodeCount = *(u16*)node;
        arr = node + 0x10;
        lo = 0;
        hi = nodeCount;
        while (lo < hi) {
            mid = (lo + hi) >> 1;
            ek = *(u16*)(arr + mid * 8);
            if (ek == (u16)key) {
                if (outNode != NULL) { *outNode = (void*)node; }
                return arr + mid * 8;
            }
            if (ek < (u16)key) lo = mid + 1;
            else hi = mid;
        }
        node = (u8*)*(u32*)(node + 0x8);
    }
    return NULL;
}
#endif
#pragma pop

/* 0x800FDFE4 | 0x2C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm s32 fn_800FDFE4(const void* str) {
#include "src/game/gs_thread_fn_800FDFE4.inc"
}
#else
#pragma optimization_level 4
s32 fn_800FDFE4(const void* str) {
    s32 r;
    r = fn_800FE010(str);
    return ((u32)(r + 1) >> 1) - 1;
}
#endif
#pragma pop

/* 0x800FE010 | 0x34C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm s32 fn_800FE010(const void* str) {
#include "src/game/gs_thread_fn_800FE010.inc"
}
#else
s32 fn_800FE010(const void* str) {
    /* TODO: match -- 844 bytes at 0x800FE010 */
    return 0;
}
#endif
#pragma pop

/* 0x800FE35C | 0x30 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE35C(void) {
#include "src/game/gs_thread_fn_800FE35C.inc"
}
#else
#pragma optimization_level 2
#pragma scheduling off
void fn_800FE35C(void) {
    fn_800D9D68(0, 0, 0x27F, 0x1DF);
}
#pragma scheduling on
#endif
#pragma pop

/* 0x800FE38C | 0x148 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2) {
#include "src/game/gs_thread_fn_800FE38C.inc"
}
#else
#pragma optimization_level 2
#pragma scheduling on
#pragma peephole off
#pragma push
#pragma optimization_level 3
void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2) {
    s32 ax, ay, bx, by;
    f32 scale_x, scale_y;
    s32 cy2, cx2, cy1, cx1;

    ax = (s32)(*(s16*)&lbl_8047AC70) + x1;
    ay = (s32)(*(s16*)&lbl_8047AC72) + y1;
    bx = ax + x2;
    by = ay + y2;
    scale_x = *(f32*)&lbl_80478B10;
    scale_y = *(f32*)&lbl_80478B14;
    cx1 = (s32)((f32)ax * scale_x);
    cy1 = (s32)((f32)ay * scale_y);
    cx2 = (s32)((f32)bx * scale_x);
    cy2 = (s32)((f32)by * scale_y);
    if (cx1 >= 0x280) cx1 = 0x27F;
    if (cy1 >= 0x1E0) cy1 = 0x1DF;
    if (cx2 >= 0x280) cx2 = 0x27F;
    if (cy2 >= 0x1E0) cy2 = 0x1DF;
    if (cx1 < 0) cx1 = 0;
    if (cy1 < 0) cy1 = 0;
    if (cx2 < 0) cx2 = 0;
    if (cy2 < 0) cy2 = 0;
    fn_800D9D68(cx1, cy1, cx2, cy2);
}
#pragma pop
#endif
#pragma pop

/* 0x800FE4D4 | 0x1CC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE4D4(void) {
#include "src/game/gs_thread_fn_800FE4D4.inc"
}
#else
#pragma optimization_level 2
void fn_800FE4D4(void) {
    f32 v0[3];
    f32 v1[3];
    f32 v2[3];
    f32 mtx[12];
    f32 sx;
    f32 sy;
    f32 x;
    f32 y;
    f32 z;
    f32 t;

    sx = lbl_8047CD58 / *(f32*)&lbl_80478B10;
    sy = lbl_8047CD5C / *(f32*)&lbl_80478B14;
    x = sx * lbl_8047CD60;
    y = sy * lbl_8047CD60;
    t = (f32)fn_800CE220();
    z = y / t;

    fn_800E01F4(v0,
                x - (f32)(s32)*(s16*)&lbl_8047AC70,
                y - (f32)(s32)*(s16*)&lbl_8047AC72,
                z);
    fn_800E01F4(v1,
                x - (f32)(s32)*(s16*)&lbl_8047AC70,
                y - (f32)(s32)*(s16*)&lbl_8047AC72,
                lbl_8047CD68);
    fn_800E01F4(v2, lbl_8047CD68, lbl_8047CD6C, lbl_8047CD68);
    fn_800E0218(mtx, v0, v2, v1);

    fn_800D9BD0(lbl_8047CD70, -(sx / sy), lbl_8047CD74, lbl_8047CD78);
    fn_800D834C();
    fn_800D7FE4(mtx);
    fn_800DA4C4(1, 6, 7);
    fn_800D888C(0x80000000);
    fn_800DA2BC(2, 2, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    fn_800DA1E8(0, 2, 0);
    fn_800DA028(0);
}
#endif
#pragma pop

/* 0x800FE6A0 | 0xC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE6A0(void) {
#include "src/game/gs_thread_fn_800FE6A0.inc"
}
#else
#pragma optimization_level 2
void fn_800FE6A0(f32 a, f32 b) {
    *(f32*)&lbl_80478B10 = a;
    *(f32*)&lbl_80478B14 = b;
}
#endif
#pragma pop

/* 0x800FE6AC | 0x24 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE6AC(void) {
#include "src/game/gs_thread_fn_800FE6AC.inc"
}
#else
#pragma optimization_level 2
void fn_800FE6AC(s16* outA, s16* outB) {
    if (outA != (void*)0) {
        *outA = *(s16*)&lbl_8047AC70;
    }
    if (outB != (void*)0) {
        *outB = *(s16*)&lbl_8047AC72;
    }
}
#endif
#pragma pop

/* 0x800FE6D0 | 0xC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE6D0(void) {
#include "src/game/gs_thread_fn_800FE6D0.inc"
}
#else
#pragma optimization_level 2
void fn_800FE6D0(s32 a, s32 b) {
    *(u16*)&lbl_8047AC70 = (u16)a;
    *(u16*)&lbl_8047AC72 = (u16)b;
}
#endif
#pragma pop

/* 0x800FE6DC | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE6DC(void) {
#include "src/game/gs_thread_fn_800FE6DC.inc"
}
#else
#pragma optimization_level 2
void fn_800FE6DC(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 0;
}
#endif
#pragma pop

/* 0x800FE6F8 | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE6F8(void) {
#include "src/game/gs_thread_fn_800FE6F8.inc"
}
#else
#pragma optimization_level 2
void fn_800FE6F8(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 1;
}
#endif
#pragma pop

/* 0x800FE714 | 0x8C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE714(s32 taskId) {
#include "src/game/gs_thread_fn_800FE714.inc"
}
#else
void fn_800FE714(u32 taskId) {
    extern u32 lbl_8047AC7C;
    extern u32 lbl_8047AC98;
    extern u32 OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(u32 state);
    u8* node;
    u32 state;
    u32 prev;
    u32 next;
    node = (u8*)lbl_8047AC7C + (taskId - 1) * 0x18;
    state = OSDisableInterrupts();
    prev = *(u32*)(node + 0);
    if (prev != 0) { *(u32*)(prev + 4) = *(u32*)(node + 4); }
    next = *(u32*)(node + 4);
    if (next != 0) { *(u32*)(next + 0) = *(u32*)(node + 0); }
    if (lbl_8047AC98 == (u32)node) { lbl_8047AC98 = *(u32*)(node + 4); }
    *(u32*)(node + 0) = 0;
    *(u32*)(node + 4) = 0;
    OSRestoreInterrupts(state);
    *(u32*)(node + 8) = 0;
}
#endif
#pragma pop

/* 0x800FE7A0 | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FE7A0(void) {
#include "src/game/gs_thread_fn_800FE7A0.inc"
}
#else
#pragma optimization_level 2
void fn_800FE7A0(void) {
    u32* task;
    u32* next;
    u32 taskId;
    task = (u32*)lbl_8047AC98;
    while (task != NULL) {
        next = (u32*)task[1];
        if (task[2] == 1 && ((u8*)task)[0xD] == 0) {
            lbl_8047AC94 = (u32)task;
            taskId = ((u32)task - lbl_8047AC7C) / 0x18 + 1;
            ((void (*)(u32, void*))task[5])(taskId, (void*)task[4]);
        }
        task = next;
    }
    lbl_8047AC94 = 0;
}
#endif
#pragma pop

/* 0x800FE834 | 0x17C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm u32 fn_800FE834(s32 state, u8 priority, void* param, void* func) {
#include "src/game/gs_thread_fn_800FE834.inc"
}
#else
#pragma optimization_level 2
#pragma scheduling on
u32 fn_800FE834(s32 state, u8 priority, void* param, void* func) {
    u8* task;
    u32 count;
    u32 i;
    u8* curr;
    u8* prev;
    u8* next;

    if (state == 2) {
        task = (u8*)lbl_8047AC7C + lbl_8047AC80 * 0x18;
        count = lbl_8047AC84;
    } else {
        task = (u8*)lbl_8047AC7C;
        count = lbl_8047AC80;
    }
    i = count;
    if (i != 0) {
        do {
            if (*(s32*)(task + 0x8) == 0) goto found;
            task += 0x18;
            i--;
        } while (i != 0);
    }
    task = NULL;
found:
    if (task == NULL) {
        return 0;
    }
    *(u32*)(task + 0x0) = 0;
    *(u32*)(task + 0x4) = 0;
    *(u32*)(task + 0x8) = state;
    *(u8*)(task + 0xC) = priority;
    *(u8*)(task + 0xD) = 0;
    *(u32*)(task + 0x10) = (u32)param;
    *(u32*)(task + 0x14) = (u32)func;
    if (lbl_8047AC98 == 0) {
        lbl_8047AC98 = (u32)task;
    } else {
        OSDisableInterrupts();
        if (*(s32*)(task + 0x8) == 2) {
            *(u32*)(task + 0x4) = lbl_8047AC9C;
            lbl_8047AC9C = (u32)task;
        } else {
            curr = (u8*)lbl_8047AC98;
            for (;;) {
                next = (u8*)*(u32*)(curr + 0x4);
                if (next == NULL) break;
                if ((u32)*(u8*)(curr + 0xC) >= (u32)*(u8*)(task + 0xC)) break;
                curr = next;
            }
            if (next == NULL && (u32)*(u8*)(curr + 0xC) < (u32)*(u8*)(task + 0xC)) {
                *(u32*)(task + 0x0) = (u32)curr;
                *(u32*)(task + 0x4) = 0;
                *(u32*)(curr + 0x4) = (u32)task;
            } else {
                prev = (u8*)*(u32*)(curr + 0x0);
                if (prev != NULL) *(u32*)(prev + 0x4) = (u32)task;
                *(u32*)(task + 0x0) = *(u32*)(curr + 0x0);
                *(u32*)(task + 0x4) = (u32)curr;
                *(u32*)(curr + 0x0) = (u32)task;
                if (lbl_8047AC98 == (u32)curr) lbl_8047AC98 = (u32)task;
            }
        }
        OSRestoreInterrupts();
    }
    return ((u32)task - lbl_8047AC7C) / 0x18 + 1;
}
#endif
#pragma pop

/* 0x800FE9B0 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 1
asm void fn_800FE9B0(u32 numTasks, u32 numQueues) {
#include "src/game/gs_thread_fn_800FE9B0.inc"
}
#else
#pragma optimization_level 2
void fn_800FE9B0(u32 numTasks, u32 numQueues) {
    u32 total;
    u32 i;
    u32 offset;
    u16 handle;

    total = numTasks + numQueues;
    lbl_8047AC80 = numTasks;
    lbl_8047AC84 = numQueues;
    lbl_8047AC88 = total;
    lbl_8047AC94 = 0;
    handle = GSmemAllocRaw(total * 0x18);
    lbl_8047AC78 = handle;
    if ((u16)handle == 0) return;
    lbl_8047AC7C = (u32)GSmemGetPtr(handle);
    offset = 8;
    i = 0;
    while (i < total) {
        *(u32*)(lbl_8047AC7C + offset) = 0;
        offset += 0x18;
        i++;
    }
    handle = GSmemAllocRaw(0x2000);
    lbl_8047AC8C = handle;
    lbl_8047AC90 = (u32)GSmemGetPtr(handle);
    fn_800A263C((void*)fn_800FEA74, NULL,
                (void*)(lbl_8047AC90 + 0x1FFC), 0x1FFC);
    fn_800D30A0((void*)fn_800FEBA0);
}
#endif
#pragma pop

/* 0x800FEA74 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800FEA74(void) {
#include "src/game/gs_thread_fn_800FEA74.inc"
}
#else
#pragma optimization_level 2
static void fn_800FEA74(void) {
    u32* task;
    u32* next;
    u32* pend;
    u32* next_pend;
    u8* curr;
    u8* prev;
    u32 taskId;

    for (;;) {
        task = (u32*)lbl_8047AC98;
        while (task != NULL) {
            next = (u32*)task[1];
            if (task[2] == 2 && ((u8*)task)[0xD] == 0) {
                lbl_8047AC94 = (u32)task;
                taskId = ((u32)task - lbl_8047AC7C) / 0x18 + 1;
                ((void(*)(u32, void*))task[5])(taskId, (void*)task[4]);
            }
            task = next;
        }
        lbl_8047AC94 = 0;
        OSDisableInterrupts();
        pend = (u32*)lbl_8047AC9C;
        while (pend != NULL) {
            next_pend = (u32*)pend[1];
            curr = (u8*)lbl_8047AC98;
            while (*(u32*)(curr + 0x4) != 0) {
                if (*(u8*)(curr + 0xC) >= *(u8*)((u8*)pend + 0xC)) break;
                curr = (u8*)*(u32*)(curr + 0x4);
            }
            if (*(u32*)(curr + 0x4) == 0 && *(u8*)(curr + 0xC) < *(u8*)((u8*)pend + 0xC)) {
                *(u32*)((u8*)pend + 0x0) = (u32)curr;
                *(u32*)((u8*)pend + 0x4) = 0;
                *(u32*)(curr + 0x4) = (u32)pend;
            } else {
                prev = (u8*)*(u32*)(curr + 0x0);
                if (prev != NULL) *(u32*)(prev + 0x4) = (u32)pend;
                *(u32*)((u8*)pend + 0x0) = *(u32*)(curr + 0x0);
                *(u32*)((u8*)pend + 0x4) = (u32)curr;
                *(u32*)(curr + 0x0) = (u32)pend;
                if (lbl_8047AC98 == (u32)curr) lbl_8047AC98 = (u32)pend;
            }
            pend = next_pend;
        }
        lbl_8047AC9C = 0;
        OSRestoreInterrupts();
    }
}
#endif
#pragma pop

/* 0x800F0A74 | 0x4D8 */
extern void fn_800F02F4(void);
extern void threadExecute(void);
extern void fn_800EEA50(void);
extern void fn_800EEB34(void);
extern void GSscratchStore(void);
extern void fn_800EEA6C(void);
extern void DCFlushRange();
extern u32 lbl_8047AC08;
extern u8 lbl_804019F0[];
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC0C;
#if 1
asm void fn_800F0A74(void) {
#include "src/game/gs_thread_fn_800F0A74.inc"
}
#else
void fn_800F0A74(void) {
    /* TODO: match -- 1240 bytes at 0x800F0A74 */
}
#endif

/* 0x800F106C | 0x7C */
extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;
#if 1
asm void fn_800F106C(void) {
#include "src/game/gs_thread_fn_800F106C.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F106C(void) {
    u32 (*f)(u32,u32,u32,u32,u32,u32,u32,u32,f32,f32,f32,f32,f32,f32,f32,f32);
    f32* fa;
    u32* ia;
    f = (u32(*)(u32,u32,u32,u32,u32,u32,u32,u32,f32,f32,f32,f32,f32,f32,f32,f32))lbl_8047AC38;
    fa = (f32*)lbl_8047AC3C;
    ia = (u32*)lbl_8047AC40;
    return f(ia[0],ia[1],ia[2],ia[3],ia[4],ia[5],ia[6],ia[7],
             fa[0],fa[1],fa[2],fa[3],fa[4],fa[5],fa[6],fa[7]);
}
#endif

/* 0x800F10E8 | 0x2E8 */
extern void fn_800DD38C(const char* fmt, ...);
extern u8 lbl_80271068[];
extern u32 lbl_80478B00;
extern u32 lbl_8047AC38;
extern u8 lbl_80401BD8[];
extern u8 lbl_80401BB8[];
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;
extern u8 lbl_8027107C[];
#if 1
asm void fn_800F10E8(void) {
#include "src/game/gs_thread_fn_800F10E8.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F10E8(void* obj) {
    u8* p;
    u32 r31;
    u32 r30;
    u32 r29;
    u8* r28;
    u8* baseArr;
    u8* entry;
    u32 stackVal0;
    u32 stackVal1;
    u32 n;
    u32 stackBase;
    s32 i;
    u32 floatIdx;
    u32 intIdx;

    p = (u8*)obj;
    /* read opcode u16 from stream, advance ptr+2 */
    r31 = (u32)*(u16*)(*(u32*)(p+0x14));
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;
    r31 = (u32)*(u16*)(*(u32*)(p+0x14));
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;

    /* push current stack top */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
        *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = *(u32*)(p+0x1C);
    }
    /* push r31 */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
        *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = r31;
    }

    /* compute stack base = count - r31 - 3, update 0x1c */
    r30 = 0;
    stackBase = *(u32*)(p+0x28) - r31 - 3;
    *(u32*)(p+0x1C) = stackBase;

    /* look up function in node table */
    entry = (u8*)*(u32*)((u8*)*(u32*)lbl_80478B00 + 0x10) + stackBase * 0xC;
    lbl_8047AC38 = *(u32*)(entry + 0x0);
    if (lbl_8047AC38 == 0) goto skip_call;

    /* clear float/int arg buffers */
    memset(lbl_80401BD8, 0, 0x20);
    memset(lbl_80401BB8, 0, 0x20);

    /* count args = min(count - stackBase - 3, 8) */
    n = *(u32*)(p+0x28) - stackBase - 3;
    if (n > 8) n = 8;

    floatIdx = 0;
    intIdx = 0;
    for (i = 0; (u32)i < n; i++) {
        if (*(u8*)(entry + 0x4 + (u32)i) == 2) {
            /* float arg */
            *(f32*)((u8*)lbl_80401BB8 + floatIdx) = *(f32*)(p + 0x6C + (stackBase + (u32)i + 1)*4);
            floatIdx += 4;
        } else {
            /* int arg */
            *(u32*)((u8*)lbl_80401BD8 + intIdx) = *(u32*)(p + 0x6C + (stackBase + (u32)i + 1)*4);
            intIdx += 4;
        }
    }
    lbl_8047AC3C = (u32)lbl_80401BB8;
    lbl_8047AC40 = (u32)lbl_80401BD8;
    r30 = fn_800F106C();

skip_call:
    /* store return value on stack at stackBase */
    *(u32*)(p + 0x68 + *(u32*)(p+0x1C)*4) = r30;

    /* pop to stackVal1 (pop once) */
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal1 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }

    /* pop to stackVal0 */
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal0 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal0 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }

    /* restore 0x1c and loop popping stackVal0 times */
    *(u32*)(p+0x1C) = stackVal0;
    r28 = 0;
    for (i = 0; i < (s32)stackVal1; i++) {
        if (*(u32*)(p+0x28) <= 0) {
            fn_800DD38C((const char*)lbl_8027107C);
        } else {
            *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        }
    }
    return 1;
}
#endif

/* 0x800F13D0 | 0x2F0 */
extern void _threadSwitch(void);
#if 0
asm void fn_800F13D0(void) {
#include "src/game/gs_thread_fn_800F13D0.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F13D0(void* obj) {
    u8* p;
    u32 r29;
    u32 r30;
    u32 r28;
    u32 stackVal0;
    u32 stackVal1;
    s32 r28b;
    u8* errStr;

    p = (u8*)obj;
    /* read two u16 from stream */
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;
    r29 = (u32)*(u16*)(*(u32*)(p+0x14));
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;

    /* push 0x1C and r29 to stack */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4) = *(u32*)(p+0x1C);
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
    }
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4) = r29;
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
    }

    /* stackBase = count - (r29 + 2) */
    *(u32*)(p+0x1C) = *(u32*)(p+0x28) - (r29 + 2);
    r28 = *(u32*)(p + 0x6C + *(u32*)(p+0x1C)*4);
    *(u8*)(p+0x4) = 2;
    r30 = 0;

    for (;;) {
        if (r30 >= r28) break;
        /* state check */
        if (*(u8*)(p+0x4) == 4) *(u8*)(p+0x4) = 3;
        if (*(u8*)(p+0x4) == 3) {
            /* pop stackVal1 */
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)lbl_8027107C);
                stackVal1 = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                stackVal1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
            /* pop stackVal0 */
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)lbl_8027107C);
                stackVal0 = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                stackVal0 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
            errStr = (u8*)lbl_8027107C;
            r28b = 0;
            *(u32*)(p+0x1C) = stackVal0;
            while (r28b < (s32)stackVal1) {
                if (*(u32*)(p+0x28) <= 0) {
                    fn_800DD38C((const char*)errStr);
                } else {
                    *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                }
                r28b++;
            }
            return 0;
        }
        _threadSwitch();
        r30 += fn_800D3088();
    }

    /* exit loop: state=1, pop twice, drain */
    *(u8*)(p+0x4) = 1;
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal1 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal0 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal0 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }
    *(u32*)(p+0x1C) = stackVal0;
    r28b = 0;
    errStr = (u8*)lbl_8027107C;
    while (r28b < (s32)stackVal1) {
        if (*(u32*)(p+0x28) <= 0) {
            fn_800DD38C((const char*)errStr);
        } else {
            *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        }
        r28b++;
    }
    return 1;
}
#endif

/* 0x800F16C0 | 0x34C */
extern s32 fn_800C8520(u8* buf, const char* fmt, ...);
extern u8 lbl_80401AB8[];
extern u8 lbl_80401A78[];
extern u8 lbl_8047CCB8[];
#if 0
asm void fn_800F16C0(void) {
#include "src/game/gs_thread_fn_800F16C0.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F16C0(void* obj) {
    u8* p;
    u32 r30;
    u8* r26;
    u8* r27;
    u32 r28;
    s32 r29;
    u32 r27val;
    u32 stackVal0;
    u32 stackVal1;
    s32 r28b;
    u8* errStr;
    u8* outEnd;
    u8 ch;
    s8 sch;
    u8 fmtBuf[4];
    u32 fmtLen;
    u32 argVal;

    p = (u8*)obj;
    /* read two u16 from stream */
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;
    r30 = (u32)*(u16*)(*(u32*)(p+0x14));
    *(u32*)(p+0x14) = *(u32*)(p+0x14) + 2;

    /* push 0x1C and r30 to stack */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4) = *(u32*)(p+0x1C);
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
    }
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4) = r30;
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
    }

    /* stackBase = count - (r30 + 2) */
    *(u32*)(p+0x1C) = *(u32*)(p+0x28) - (r30 + 2);
    r26 = lbl_80401AB8;
    r28 = 1;
    r27val = *(u32*)(p + 0x6C + *(u32*)(p+0x1C)*4);
    outEnd = lbl_80401AB8 + 0xFF;
    r27 = (u8*)r27val;

    for (;;) {
        ch = *r27;
        sch = (s8)ch;
        if (sch == 0) break;
        if (sch == 0x25) { /* '%' */
            argVal = *(u32*)(p + 0x6C + (*(u32*)(p+0x1C) + r28)*4);
            r28++;
            fmtLen = 0;
            for (;;) {
                lbl_80401A78[fmtLen] = *r27;
                fmtLen++;
                ch = *r27;
                sch = (s8)ch;
                if (sch == 'd' || sch == 'x' || sch == 'c') {
                    lbl_80401A78[fmtLen] = 0;
                    r29 = fn_800C8520(r26, (const char*)lbl_80401A78, argVal);
                    r26 += r29;
                    break;
                } else if (sch == 'f') {
                    lbl_80401A78[fmtLen] = 0;
                    r29 = fn_800C8520(r26, (const char*)lbl_80401A78, *(f32*)&argVal);
                    r26 += r29;
                    break;
                } else if (sch == 's') {
                    lbl_80401A78[fmtLen] = 0;
                    r29 = fn_800C8520(r26, (const char*)lbl_80401A78, argVal);
                    r26 += r29;
                    break;
                } else if (sch == 0) {
                    break;
                }
                r27++;
            }
        } else if (sch == 0x5C) { /* '\' */
            if ((s8)r27[1] == 'n') {
                r29 = fn_800C8520(r26, (const char*)lbl_8047CCB8);
                r27++;
                r26 += r29;
            } else {
                *r26 = ch;
                r26++;
            }
        } else {
            *r26 = ch;
            r26++;
        }
        r27++;
        if (r26 >= outEnd) break;
    }

    /* null-terminate and print */
    *r26 = 0;
    fn_800DD970((const char*)lbl_80401AB8);

    /* pop twice, drain */
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal1 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }
    if (*(u32*)(p+0x28) <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        stackVal0 = *(u32*)(p+0x6C);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        stackVal0 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
    }
    *(u32*)(p+0x1C) = stackVal0;
    errStr = (u8*)lbl_8027107C;
    r28b = 0;
    while (r28b < (s32)stackVal1) {
        if (*(u32*)(p+0x28) <= 0) {
            fn_800DD38C((const char*)errStr);
        } else {
            *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
        }
        r28b++;
    }
    return 1;
}
#endif

/* 0x800F1A0C | 0x42C */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCBC;
#if 1
asm void fn_800F1A0C(void) {
#include "src/game/gs_thread_fn_800F1A0C.inc"
}
#else
void fn_800F1A0C(void) {
    /* TODO: match -- 1068 bytes at 0x800F1A0C */
}
#endif

/* 0x800F1E38 | 0x42C */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCBC;
#if 1
asm void fn_800F1E38(void) {
#include "src/game/gs_thread_fn_800F1E38.inc"
}
#else
void fn_800F1E38(void) {
    /* TODO: match -- 1068 bytes at 0x800F1E38 */
}
#endif

/* 0x800F2264 | 0x290 */
#if 1
asm void fn_800F2264(void) {
#include "src/game/gs_thread_fn_800F2264.inc"
}
#else
void fn_800F2264(void) {
    /* TODO: match -- 656 bytes at 0x800F2264 */
}
#endif

/* 0x800F24F4 | 0x2E0 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 1
asm void fn_800F24F4(void) {
#include "src/game/gs_thread_fn_800F24F4.inc"
}
#else
void fn_800F24F4(void) {
    /* TODO: match -- 736 bytes at 0x800F24F4 */
}
#endif

/* 0x800F27D4 | 0x414 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F27D4(void) {
#include "src/game/gs_thread_fn_800F27D4.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F27D4(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* NEQ */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = (val2 != val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 != f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 != f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 != f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F2BE8 | 0x410 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F2BE8(void) {
#include "src/game/gs_thread_fn_800F2BE8.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F2BE8(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* EQ */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = (val2 == val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 == f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 == f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 == f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F2FF8 | 0x420 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F2FF8(void) {
#include "src/game/gs_thread_fn_800F2FF8.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F2FF8(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* GE: result=1 when (s32)val2 >= (s32)val1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = ((s32)val2 >= (s32)val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 >= f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 >= f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 >= f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F3418 | 0x418 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F3418(void) {
#include "src/game/gs_thread_fn_800F3418.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F3418(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* GT: result=1 when (s32)val2 > (s32)val1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = ((s32)val2 > (s32)val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 > f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 > f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 > f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F3830 | 0x420 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F3830(void) {
#include "src/game/gs_thread_fn_800F3830.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F3830(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* LE: result=1 when (s32)val2 <= (s32)val1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = ((s32)val2 <= (s32)val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 <= f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 <= f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 <= f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F3C50 | 0x418 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F3C50(void) {
#include "src/game/gs_thread_fn_800F3C50.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F3C50(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* LT: result=1 when (s32)val2 < (s32)val1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = ((s32)val2 < (s32)val1) ? 1 : 0;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            result = (f1 < f0) ? 1 : 0;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            result = (f2 < f0) ? 1 : 0;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            result = (f1 < f0) ? 1 : 0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F4068 | 0x3D8 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F4068(void) {
#include "src/game/gs_thread_fn_800F4068.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F4068(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* SUB: op2 - op1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = val2 - val1;
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            f32 tmp = f1 - f0; result = *(u32*)&tmp;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            f32 tmp = f2 - f0; result = *(u32*)&tmp;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            f32 tmp = f1 - f0; result = *(u32*)&tmp;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F4440 | 0x3D8 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F4440(void) {
#include "src/game/gs_thread_fn_800F4440.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F4440(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 r4;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;

    /* read 2 operand bytes */
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp;
        sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp;
        sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;

    /* resolve op1 (r28) */
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)(r31+0x14));
                val1 = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) {
                    val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                } else {
                    val1 = *(u32*)(p+0x18) + idx*4;
                }
            } else {
                u32 rawptr;
                if (r30 & 0x40) {
                    rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                } else {
                    rawptr = *(u32*)(p+0x18) + idx*4;
                }
                if (r30 & 0x100) {
                    val1 = rawptr;
                } else {
                    val1 = *(u32*)rawptr;
                }
            }
        }
    }

    /* resolve op2 (r29) */
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)(r31+0x14));
                val2 = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) {
                    val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                } else {
                    val2 = *(u32*)(p+0x18) + idx*4;
                }
            } else {
                u32 rawptr;
                if (r30 & 0x40) {
                    rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                } else {
                    rawptr = *(u32*)(p+0x18) + idx*4;
                }
                if (r30 & 0x100) {
                    val2 = rawptr;
                } else {
                    val2 = *(u32*)rawptr;
                }
            }
        }
    }

    /* compute: ADD */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            /* int + int */
            result = val2 + val1;
        } else {
            /* int val2 + float val1 */
            f32 f1 = (f32)(s32)val2;
            f32 f0 = *(f32*)&val1;
            result = *(u32*)&(f32){f0 + f1}; /* fix */
            {
                f32 tmp = f0 + f1;
                result = *(u32*)&tmp;
            }
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            /* float val2 + int val1 */
            f32 f0 = *(f32*)&val2;
            f32 f1 = (f32)(s32)val1;
            f32 tmp = f0 + f1;
            result = *(u32*)&tmp;
        } else {
            /* float + float */
            f32 f1 = *(f32*)&val2;
            f32 f0 = *(f32*)&val1;
            f32 tmp = f1 + f0;
            result = *(u32*)&tmp;
        }
    }

    /* push result */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)r31);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
        *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result;
    }
    return 1;
}
#endif

/* 0x800F4818 | 0x420 */
extern void fn_800CE318(void);
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F4818(void) {
#include "src/game/gs_thread_fn_800F4818.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F4818(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* MOD: op2 % op1, op1 must be nonzero */
    if (val1 == 0) {
        fn_800DD38C((const char*)(r31+0xD8));
        result = 0;
    } else {
        if ((r29 & 0x3F) == 2) {
            if ((r28 & 0x3F) == 2) {
                result = (u32)((s32)val2 % (s32)val1);
            } else {
                f32 f2 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
                fn_800CE318();
                result = *(u32*)&f0;
            }
        } else {
            f32 f2 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            fn_800CE318();
            result = *(u32*)&f0;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F4C38 | 0x3F4 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F4C38(void) {
#include "src/game/gs_thread_fn_800F4C38.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F4C38(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* DIV: op2 / op1, op1 must be nonzero */
    if (val1 == 0) {
        fn_800DD38C((const char*)(r31+0xD8));
        result = 0;
    } else {
        if ((r29 & 0x3F) == 2) {
            if ((r28 & 0x3F) == 2) {
                result = (u32)((s32)val2 / (s32)val1);
            } else {
                f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
                f32 tmp = f1 / f0; result = *(u32*)&tmp;
            }
        } else {
            if ((r28 & 0x3F) == 2) {
                f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
                f32 tmp = f2 / f0; result = *(u32*)&tmp;
            } else {
                f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
                f32 tmp = f1 / f0; result = *(u32*)&tmp;
            }
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F502C | 0x3D8 */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCC0;
#if 0
asm void fn_800F502C(void) {
#include "src/game/gs_thread_fn_800F502C.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F502C(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* MUL: op2 * op1 */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = (u32)((s32)val2 * (s32)val1);
        } else {
            f32 f1 = (f32)(s32)val2; f32 f0 = *(f32*)&val1;
            f32 tmp = f1 * f0; result = *(u32*)&tmp;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            f32 f2 = *(f32*)&val2; f32 f0 = (f32)(s32)val1;
            f32 tmp = f2 * f0; result = *(u32*)&tmp;
        } else {
            f32 f1 = *(f32*)&val2; f32 f0 = *(f32*)&val1;
            f32 tmp = f1 * f0; result = *(u32*)&tmp;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F5404 | 0x1D8 */
extern u32 lbl_8047E710;
#if 0
asm void fn_800F5404(void) {
#include "src/game/gs_thread_fn_800F5404.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F5404(void* obj) {
    u8* p;
    u8* r31;
    u32 r30;
    u32 r29;
    u32 r4;
    u32 result;
    u32 def;
    u32 idx;
    u32 ptr;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    r29 = p - p; /* obj, keep p */
    r29 = (u32)p;

    /* read 1 byte operand descriptor */
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r4 = (u32)*sp;
        *(u32*)(p+0x14) = (u32)(sp+1);
    }
    def = lbl_8047E710;

    if (r4 == 0) {
        /* invalid operand */
        fn_800DD38C((const char*)(r31+0x28), (u32)(r4 & 0xFFFF));
        result = def;
    } else {
        r30 = r4 & 0xFFFF;
        if (r4 & 0x80) {
            /* pop from stack */
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)(r31+0x14));
                result = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                result = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
        } else {
            /* index-based address resolution */
            idx = r30 & 0x3F; /* clear upper bits */
            if (r30 & 0x20) {
                if (r30 & 0x40) {
                    /* local stack relative, store address */
                    result = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx) * 4);
                } else {
                    /* external array, store address */
                    result = *(u32*)(p+0x18) + idx * 4;
                }
            } else {
                u32 rawptr;
                if (r30 & 0x40) {
                    rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx) * 4);
                } else {
                    rawptr = *(u32*)(p+0x18) + idx * 4;
                }
                if (r30 & 0x100) {
                    result = rawptr;
                } else {
                    result = *(u32*)rawptr;
                }
            }
        }
    }

    /* push result to stack */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)r31);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
        *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result;
    }
    return 1;
}
#endif

/* 0x800F55DC | 0x214 */
#if 0
asm void fn_800F55DC(void) {
#include "src/game/gs_thread_fn_800F55DC.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F55DC(void* obj) {
    u8* p;
    u8* r31;
    u32 r30;
    u32 r29;
    u32 r4;
    u32 result;
    u32 def;
    u32 idx;
    u32 negResult;
    f32 fval;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;

    /* read 1 byte operand descriptor */
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r29 = (u32)*sp;
        *(u32*)(p+0x14) = (u32)(sp+1);
    }
    def = lbl_8047E710;

    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        result = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) {
                fn_800DD38C((const char*)(r31+0x14));
                result = *(u32*)(p+0x6C);
            } else {
                *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1;
                result = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4);
            }
        } else {
            u32 idx2 = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) {
                    result = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx2)*4);
                } else {
                    result = *(u32*)(p+0x18) + idx2*4;
                }
            } else {
                u32 rawptr;
                if (r30 & 0x40) {
                    rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx2)*4);
                } else {
                    rawptr = *(u32*)(p+0x18) + idx2*4;
                }
                if (r30 & 0x100) {
                    result = rawptr;
                } else {
                    result = *(u32*)rawptr;
                }
            }
        }
    }

    /* negate based on type (bits 0-5 of r29) */
    if ((r29 & 0x3F) == 2) {
        /* integer negate */
        negResult = (u32)(-(s32)result);
    } else {
        /* float negate */
        fval = -(*(f32*)&result);
        negResult = *(u32*)&fval;
    }

    /* push negated result */
    if (*(u32*)(p+0x28) > 0x40) {
        fn_800DD38C((const char*)r31);
    } else {
        *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1;
        *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = negResult;
    }
    return 1;
}
#endif

/* 0x800F57F0 | 0x24C */
extern u32 lbl_8047E710;
extern u32 lbl_8047CCBC;
extern u32 lbl_8047CCC0;
#if 1
asm void fn_800F57F0(void) {
#include "src/game/gs_thread_fn_800F57F0.inc"
}
#else
void fn_800F57F0(void) {
    /* TODO: match -- 588 bytes at 0x800F57F0 */
}
#endif

/* 0x800F5A3C | 0x264 */
extern u32 lbl_8047E710;
#if 1
asm void fn_800F5A3C(void) {
#include "src/game/gs_thread_fn_800F5A3C.inc"
}
#else
void fn_800F5A3C(void) {
    /* TODO: match -- 612 bytes at 0x800F5A3C */
}
#endif

/* 0x800F5CA0 | 0x24C */
extern u32 lbl_8047E710;
#if 1
asm void fn_800F5CA0(void) {
#include "src/game/gs_thread_fn_800F5CA0.inc"
}
#else
void fn_800F5CA0(void) {
    /* TODO: match -- 588 bytes at 0x800F5CA0 */
}
#endif

/* 0x800F5EEC | 0x3D0 */
extern u32 lbl_8047E710;
#if 0
asm void fn_800F5EEC(void) {
#include "src/game/gs_thread_fn_800F5EEC.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F5EEC(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* OR */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = val2 | val1;
        } else {
            result = val2 | (u32)(s32)*(f32*)&val1;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            result = (u32)(s32)*(f32*)&val2 | val1;
        } else {
            result = (u32)(s32)*(f32*)&val2 | (u32)(s32)*(f32*)&val1;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F62BC | 0x3D0 */
extern u32 lbl_8047E710;
#if 0
asm void fn_800F62BC(void) {
#include "src/game/gs_thread_fn_800F62BC.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F62BC(void* obj) {
    u8* p;
    u8* r31;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 def;
    u32 val1;
    u32 val2;
    u32 result;

    p = (u8*)obj;
    r31 = (u8*)lbl_80271068;
    {
        u8* sp = (u8*)*(u32*)(p+0x14);
        r28 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
        r29 = (u32)*sp; sp++;
        *(u32*)(p+0x14) = (u32)sp;
    }
    def = lbl_8047E710;
    if (r28 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r28 & 0xFFFF));
        val1 = def;
    } else {
        r30 = r28 & 0xFFFF;
        if (r28 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val1 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val1 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val1 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val1 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val1 = rawptr;
                else val1 = *(u32*)rawptr;
            }
        }
    }
    def = lbl_8047E710;
    if (r29 == 0) {
        fn_800DD38C((const char*)(r31+0x28), (u32)(r29 & 0xFFFF));
        val2 = def;
    } else {
        r30 = r29 & 0xFFFF;
        if (r29 & 0x80) {
            if (*(u32*)(p+0x28) <= 0) { fn_800DD38C((const char*)(r31+0x14)); val2 = *(u32*)(p+0x6C); }
            else { *(u32*)(p+0x28) = *(u32*)(p+0x28) - 1; val2 = *(u32*)(p + 0x6C + *(u32*)(p+0x28)*4); }
        } else {
            u32 idx = r30 & 0x3F;
            if (r30 & 0x20) {
                if (r30 & 0x40) val2 = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else val2 = *(u32*)(p+0x18) + idx*4;
            } else {
                u32 rawptr;
                if (r30 & 0x40) rawptr = (u32)(p + 0x6C + (*(u32*)(p+0x1C) + idx)*4);
                else rawptr = *(u32*)(p+0x18) + idx*4;
                if (r30 & 0x100) val2 = rawptr;
                else val2 = *(u32*)rawptr;
            }
        }
    }
    /* AND */
    if ((r29 & 0x3F) == 2) {
        if ((r28 & 0x3F) == 2) {
            result = val2 & val1;
        } else {
            result = val2 & (u32)(s32)*(f32*)&val1;
        }
    } else {
        if ((r28 & 0x3F) == 2) {
            result = (u32)(s32)*(f32*)&val2 & val1;
        } else {
            result = (u32)(s32)*(f32*)&val2 & (u32)(s32)*(f32*)&val1;
        }
    }
    if (*(u32*)(p+0x28) > 0x40) { fn_800DD38C((const char*)r31); }
    else { *(u32*)(p+0x28) = *(u32*)(p+0x28) + 1; *(u32*)(p + 0x6C + (*(u32*)(p+0x28)-1)*4) = result; }
    return 1;
}
#endif

/* 0x800F668C | 0x80 */
#if 0
asm s32 fn_800F668C(void* obj) {
#include "src/game/gs_thread_fn_800F668C.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F668C(void* obj) {
    u8* p = (u8*)obj;
    s32 count;
    u32 val;

    *(u32*)(p + 0x14) = *(u32*)(p + 0x14) + 1;
    count = *(s32*)(p + 0x28);
    val = *(u32*)*(u32*)(p + 0x14);
    if (count > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(s32*)(p + 0x28) = count + 1;
        ((u32*)(p + 0x6C))[count] = val;
    }
    *(u32*)(p + 0x14) = *(u32*)(p + 0x14) + 4;
    return 1;
}
#endif

/* 0x800F670C | 0xA0 */
#if 0
asm s32 fn_800F670C(void* obj) {
#include "src/game/gs_thread_fn_800F670C.inc"
}
#else
#pragma optimization_level 4
s32 fn_800F670C(u8* ptr) {
    volatile u32 cond;
    volatile u32 val;
    s32 count = *(s32*)(ptr + 0x28);
    if (count <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        val = *(u32*)(ptr + 0x6c);
    } else {
        count--;
        *(s32*)(ptr + 0x28) = count;
        val = ((u32*)(ptr + 0x6c))[count];
    }
    if ((s32)(cond = val) != 0) {
        *(u32*)(ptr + 0x14) = *(u32*)(ptr + 0x14) + 4;
    } else {
        *(u32*)(ptr + 0x14) = *(u32*)ptr + **(u32**)(ptr + 0x14);
    }
    return 1;
}
#endif

/* 0x800F67AC | 0x1C */
#if 0
asm void fn_800F67AC(void) {
#include "src/game/gs_thread_fn_800F67AC.inc"
}
#else
#pragma optimization_level 2
#pragma push
#pragma optimization_level 3
s32 fn_800F67AC(u32* ptr) {
    u32* p = (u32*)ptr[5];
    ptr[5] = ptr[0] + *p;
    return 1;
}
#pragma pop
#endif

/* 0x800F67C8 | 0x184 */
#if 0
asm s32 fn_800F67C8(void* obj) {
#include "src/game/gs_thread_fn_800F67C8.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F67C8(void* obj) {
    u8* p;
    s32 count;
    volatile u32 val1;
    volatile u32 val2;
    volatile u32 val3;
    s32 i;
    s32 loopCount;
    const char* errStr;
    u8* stackPtr;

    p = (u8*)obj;

    /* Pop 1 (into val1) */
    count = *(s32*)(p + 0x28);
    if ((s32)count <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        val1 = *(u32*)(p + 0x6C);
    } else {
        count = count - 1;
        *(s32*)(p + 0x28) = count;
        stackPtr = p + count * 4;
        val1 = *(u32*)(stackPtr + 0x6C);
    }

    /* Pop 2 (into val2) → store to obj->0x1C */
    count = *(s32*)(p + 0x28);
    if ((s32)count <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        val2 = *(u32*)(p + 0x6C);
    } else {
        count = count - 1;
        *(s32*)(p + 0x28) = count;
        stackPtr = p + count * 4;
        val2 = *(u32*)(stackPtr + 0x6C);
    }
    *(u32*)(p + 0x1C) = val2;

    /* Pop 3 (return address) → store to obj->0x14 */
    count = *(s32*)(p + 0x28);
    if ((s32)count <= 0) {
        fn_800DD38C((const char*)lbl_8027107C);
        val3 = *(u32*)(p + 0x6C);
    } else {
        count = count - 1;
        *(s32*)(p + 0x28) = count;
        stackPtr = p + count * 4;
        val3 = *(u32*)(stackPtr + 0x6C);
    }
    *(u32*)(p + 0x14) = val3;

    /* Drain val1 entries from stack */
    loopCount = val1;
    errStr = (const char*)lbl_8027107C;
    for (i = 0; i < loopCount; i++) {
        count = *(s32*)(p + 0x28);
        if ((s32)count <= 0) {
            fn_800DD38C(errStr);
        } else {
            *(s32*)(p + 0x28) = count - 1;
        }
    }

    if (*(u32*)(p + 0x14) == 0) {
        *(u8*)(p + 0x4) = 3;
        return 0;
    }
    return 1;
}
#endif

/* 0x800F694C | 0x168 */
extern u8 lbl_8027115C[];
#if 0
asm s32 fn_800F694C(void* obj) {
#include "src/game/gs_thread_fn_800F694C.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F694C(void* obj) {
    u8* p;
    u8* ptr;
    u32 funcIdx;
    u32 argCount;
    u32 stackBase;
    s32 count;
    u32 value;
    u8* head;

    p = (u8*)obj;
    ptr = (u8*)*(volatile u32*)(p + 0x14);
    funcIdx = *(u32*)ptr;
    *(volatile u32*)(p + 0x14) = (u32)(ptr + 4);
    ptr = (u8*)*(volatile u32*)(p + 0x14);
    argCount = *(u16*)ptr;
    *(volatile u32*)(p + 0x14) = (u32)(ptr + 2);
    ptr = (u8*)*(volatile u32*)(p + 0x14);
    stackBase = *(u16*)ptr;
    *(volatile u32*)(p + 0x14) = (u32)(ptr + 2);

    /* Push current ptr (return address) */
    count = *(s32*)(p + 0x28);
    value = *(u32*)(p + 0x14);
    if (count > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x28) = count + 1;
        *(u32*)(p + 0x6C + count * 4) = value;
    }

    /* Push obj->0x1C */
    count = *(s32*)(p + 0x28);
    value = *(u32*)(p + 0x1C);
    if (count > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x28) = count + 1;
        *(u32*)(p + 0x6C + count * 4) = value;
    }

    /* Push stackBase (argCount) */
    count = *(s32*)(p + 0x28);
    if (count > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(u32*)(p + 0x28) = count + 1;
        *(u32*)(p + 0x6C + count * 4) = (u32)stackBase;
    }

    /* Update obj->0x1C = obj->0x28 - (argCount + stackBase + 3) */
    *(u32*)(p + 0x1C) = *(u32*)(p + 0x28) - ((u32)argCount + (u32)stackBase + 3);

    /* Look up jump target via funcIdx in head table */
    head = (u8*)*(u32*)(p + 0x0);
    if (funcIdx >= (u32)*(u16*)(head + 0x4)) {
        fn_800DD38C((const char*)lbl_8027115C, funcIdx);
    } else {
        *(u32*)(p + 0x14) = (u32)head + *(u32*)(head + funcIdx * 4 + 0x18);
    }
    return 1;
}
#endif

/* 0x800F6AB4 | 0xA0 */
#if 1
asm s32 fn_800F6AB4(void* obj) {
#include "src/game/gs_thread_fn_800F6AB4.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F6AB4(void* obj) {
    u8* p;
    u8* ptr;
    u16 n;
    u32 count;
    u32 i;

    p = (u8*)obj;
    ptr = (u8*)*(u32*)(p + 0x14) + 1;
    *(u32*)(p + 0x14) = (u32)ptr;
    n = *(u16*)ptr;
    ptr += 2;
    *(u32*)(p + 0x14) = (u32)ptr;
    for (i = 0; i < (u32)n; i++) {
        count = *(u32*)(p + 0x28);
        if ((s32)count <= 0) {
            fn_800DD38C((const char*)lbl_8027107C);
        } else {
            *(u32*)(p + 0x28) = count - 1;
        }
    }
    return 1;
}
#endif

/* 0x800F6B54 | 0x58 */
#if 0
asm s32 fn_800F6B54(void* obj) {
#include "src/game/gs_thread_fn_800F6B54.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F6B54(u8* ptr) {
    s32 count = *(s32*)(ptr + 0x28);
    if (count > 0x40) {
        fn_800DD38C((const char*)lbl_80271068);
    } else {
        *(s32*)(ptr + 0x28) = count + 1;
        ptr = ptr + count * 4;
        *(s32*)(ptr + 0x6c) = -1;
    }
    return 1;
}
#endif

/* 0x800F6BAC | 0x10 */
#if 0
asm void fn_800F6BAC(void) {
#include "src/game/gs_thread_fn_800F6BAC.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F6BAC(u8* ptr) {
    ptr[4] = 3;
    return 0;
}
#endif

/* 0x800F6BBC | 0x8 */
#if 0
asm void fn_800F6BBC(void) {
#include "src/game/gs_thread_fn_800F6BBC.inc"
}
#else
#pragma optimization_level 2
s32 fn_800F6BBC(void) {
    return 1;
}
#endif

/* 0x800F6BC4 | 0x154 */
extern u8 lbl_803155D0[];
#if 0
asm u32 fn_800F6BC4(void* obj) {
#include "src/game/gs_thread_fn_800F6BC4.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F6BC4(void* obj) {
    u8* p;
    u8* strBase;
    u8* ip;
    u32 state;
    s32 opcode;
    void (*dispatch)(void*);
    u32 count;
    volatile u32 ret;
    volatile u32 val;
    u32 delay;

    p = (u8*)obj;
    strBase = lbl_80271068;
    for (;;) {
        state = (u32)*(u8*)(p + 0x4);
        if (state == 0) {
            fn_800DD970((const char*)(strBase + 0x120), *(u32*)(p + 0x8));
            goto done;
        }
        if (state == 3) {
            *(u8*)(p + 0x4) = 0;
            goto done;
        }
        ip = (u8*)*(u32*)(p + 0x14);
        *(u32*)(p + 0x14) = (u32)(ip + 1);
        opcode = (u32)*ip;
        if (opcode >= 0x26) {
            fn_800DD38C((const char*)(strBase + 0x150));
        } else {
            dispatch = (void (*)(void*))*(u32*)((u8*)lbl_803155D0 + (opcode & 0xFF) * 4);
            if (dispatch != NULL) {
                dispatch(obj);
            }
        }
        delay = *(u32*)(p + 0x28);
        if ((s32)delay > 0) {
            for (delay--; (s32)delay >= 0; delay--) {
            }
        }
    }
done:
    if (*(u8*)(p + 0x4) == 4) {
        *(u8*)(p + 0x4) = 0;
    }
    count = *(u32*)(p + 0x28);
    if ((s32)count <= 0) {
        fn_800DD38C((const char*)(strBase + 0x14));
        val = *(u32*)(p + 0x6C);
    } else {
        count--;
        *(u32*)(p + 0x28) = count;
        val = *(u32*)(p + 0x6C + count * 4);
    }
    {
        void (*cb)(void*, u32) = (void (*)(void*, u32))*(u32*)(p + 0x10);
        ret = val;
        if (cb != NULL) cb(obj, val);
    }
    return ret;
}
#endif

/* 0x800F6D18 | 0x350 */
extern void __va_arg();
extern u32 lbl_80478B00;
#if 1
asm void* fn_800F6D18(void* callback, u32 arg, void* list) {
#include "src/game/gs_thread_fn_800F6D18.inc"
}
#else
void* fn_800F6D18(void* callback, u32 arg, void* list) {
    /* TODO: match -- 848 bytes at 0x800F6D18 */
    return 0;
}
#endif

/* 0x800F7068 | 0xA0 */
extern u32 lbl_80478B00;
#if 0
asm s32 fn_800F7068(u16 key, u8 flag) {
#include "src/game/gs_thread_fn_800F7068.inc"
}
#else
s32 fn_800F7068(u16 key, u8 flag) {
    u32 k = (u32)(u16)key;
    u32 f = (u32)(u8)flag;
    u8* entry;
    u32 offset;
    u8* head;
    s32 i;

    for (;;) {
        head = (u8*)lbl_80478B00;
        offset = 0;
        i = (s32)*(u16*)head;
        if (i > 0) {
            do {
                entry = (u8*)*(u32*)(head + 0xC) + offset;
                if (*(u8*)(entry + 0x4) != 0 && (u32)*(u16*)(entry + 0x6) == k) {
                    goto found;
                }
                offset += 0x16C;
            } while (--i != 0);
        }
        entry = NULL;
    found:
        if (entry == NULL) return 0;
        if (f != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}
#endif
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC0C;
extern u32 lbl_8047AC08;
#if 0
asm void fn_800F0F4C(u32 arg) {
#include "src/game/gs_thread_fn_800F0F4C.inc"
}
#else
#pragma optimization_level 2
void fn_800F0F4C(u32 arg) {
    u8* obj;

    obj = (u8*)lbl_8047AC00;
    *(u32*)(obj + 0x18) = arg;
    if (*(u8*)(obj + 0x17) == 1) {
        if ((u32)obj == lbl_8047AC00 || (u32)obj == lbl_8047AC04) {
            *(u8*)(obj + 0x15) = 1;
            if ((u32)obj == lbl_8047AC04) {
                *(u8*)&lbl_8047AC0C = 1;
            }
        } else {
            *(u8*)(obj + 0x14) = 0;
            *(u8*)(obj + 0x8) = 0;
            if (*(u32*)(obj + 0x0) != 0) *(u32*)(*(u32*)(obj+0x0)+0x4) = *(u32*)(obj+0x4);
            if (*(u32*)(obj + 0x4) != 0) *(u32*)(*(u32*)(obj+0x4)+0x0) = *(u32*)(obj+0x0);
            if (lbl_8047AC08 == (u32)obj) lbl_8047AC08 = *(u32*)(obj+0x4);
            fn_800E209C(*(u16*)(obj + 0x20));
            fn_800E209C(*(u16*)(obj + 0x22));
        }
    } else {
        *(u8*)(obj + 0x15) = 1;
        *(u8*)(obj + 0x14) = 0;
        if (*(u32*)(obj + 0x0) != 0) *(u32*)(*(u32*)(obj+0x0)+0x4) = *(u32*)(obj+0x4);
        if (*(u32*)(obj + 0x4) != 0) *(u32*)(*(u32*)(obj+0x4)+0x0) = *(u32*)(obj+0x0);
        if (lbl_8047AC08 == (u32)obj) lbl_8047AC08 = *(u32*)(obj+0x4);
        *(u8*)&lbl_8047AC0C = 1;
    }
    _threadSwitch();
}
#endif
extern u32 lbl_80478B00;
#if 1
asm u32 fn_800F7108(u16 key) {
#include "src/game/gs_thread_fn_800F7108.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F7108(u16 key) {
    u8* head;
    u16 count;
    u8* entry;
    u8* e;
    u32 offset;
    u32 k;

    k = (u32)(u16)key;
    head = (u8*)lbl_80478B00;
    count = *(u16*)head;
    offset = 0;
    entry = NULL;
    while (count > 0) {
        e = (u8*)*(u32*)(head + 0xC) + offset;
        if (*(u8*)(e + 0x4) != 0 && (u32)*(u16*)(e + 0x6) == k) {
            entry = e;
            break;
        }
        offset += 0x16C;
        count--;
    }
    if (entry == NULL) return 0;
    return *(u32*)(entry + 0xC);
}
#endif
extern u32 fn_800F0374(u32 ctx);
extern void fn_800F05A0(u32 ctx);
extern u32 lbl_80478B00;
#if 1
asm u32 fn_800F716C(u32 key) {
#include "src/game/gs_thread_fn_800F716C.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F716C(u32 key) {
    u8* head;
    u8* head2;
    u8* entry;
    u8* e;
    u16 total;
    u16 total2;
    u32 offset;
    u32 offset2;
    u16 subkey;
    u32 i;

    i = 0;
    offset = 0;
    for (;;) {
        head = (u8*)lbl_80478B00;
        total = *(u16*)head;
        if ((s32)i >= (s32)total) break;
        entry = (u8*)*(u32*)(head + 0xC) + offset;
        if (*(u8*)(entry + 0x4) != 0 && *(u32*)(entry + 0xC) != 0) {
            if (fn_800F0374(*(u32*)(entry + 0xC)) == key) {
                head2 = (u8*)lbl_80478B00;
                subkey = *(u16*)(entry + 0x6);
                total2 = *(u16*)head2;
                offset2 = 0;
                e = NULL;
                while (total2 > 0) {
                    e = (u8*)*(u32*)(head2 + 0xC) + offset2;
                    if (*(u8*)(e + 0x4) != 0 && *(u16*)(e + 0x6) == subkey) break;
                    offset2 += 0x16C;
                    total2--;
                    e = NULL;
                }
                if (e != NULL) {
                    *(u8*)(e + 0x4) = 4;
                    if (*(u32*)(e + 0xC) != 0) {
                        fn_800F05A0(*(u32*)(e + 0xC));
                        *(u32*)(e + 0xC) = 0;
                    }
                }
            }
        }
        offset += 0x16C;
        i++;
    }
    return 0;
}
#endif
extern u32 lbl_80478B00;
#if 1
asm u32 fn_800F7274(u16 key) {
#include "src/game/gs_thread_fn_800F7274.inc"
}
#else
#pragma optimization_level 2
u32 fn_800F7274(u16 key) {
    u8* head;
    u16 count;
    u8* entry;
    u8* e;
    u32 offset;
    u32 k;

    k = (u32)(u16)key;
    head = (u8*)lbl_80478B00;
    count = *(u16*)head;
    offset = 0;
    entry = NULL;
    while (count > 0) {
        e = (u8*)*(u32*)(head + 0xC) + offset;
        if (*(u8*)(e + 0x4) != 0 && (u32)*(u16*)(e + 0x6) == k) {
            entry = e;
            break;
        }
        offset += 0x16C;
        count--;
    }
    if (entry == NULL) return 0;
    *(u8*)(entry + 0x4) = 4;
    if (*(u32*)(entry + 0xC) != 0) {
        fn_800F05A0(*(u32*)(entry + 0xC));
        *(u32*)(entry + 0xC) = 0;
    }
    return 0;
}
#endif
extern u32 fn_800FF560(void);
extern GSThread* GSthreadCreate(u32, u32, u32, u32, u32, void*);
extern void fn_800F0654(void*, s32, ...);
extern u8 lbl_80271294[];
extern u8 lbl_80315668[];
typedef struct ThreadVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} ThreadVaList;
typedef ThreadVaList ThreadVaListArray[1];
#if 0
asm u32 fn_800F7318(void) {
#include "src/game/gs_thread_fn_800F7318.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma push
#pragma optimization_level 1
u32 fn_800F7318(u32 r27, void* callback, u32 r28, u32 r29, u32 r30, u32 r8, ...) {
    ThreadVaListArray list;
    register void* listPtr;
    u8* entry;
    void* thread;
    u32 current;

    *(u32*)list = 0x06000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    listPtr = list;
    entry = fn_800F6D18(callback, r8, listPtr);
    current = fn_800FF560();
    if (entry == NULL) {
        return 0;
    }
    thread = GSthreadCreate(r27, current, r28, 1, r29, fn_800F6BC4);
    if (thread != NULL) {
        *(void**)(entry + 0xC) = thread;
        *(u32*)(entry + 0x10) = r30;
        fn_800F0654(thread, 1, entry);
    } else {
        fn_800DD970((const char*)lbl_80271294, lbl_80315668);
    }
    return *(u16*)(entry + 0x6);
}
#pragma pop
#endif
#if 0
asm u32 fn_800F7434(void* callback, u32 arg, ...) {
#include "src/game/gs_thread_fn_800F7434.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 fn_800F7434(void* callback, u32 arg, ...) {
    ThreadVaListArray list;
    register void* listPtr;
    u8* entry;
    u8* strBase;
    u8* ip;
    u32 state;
    u32 opcode;
    void (*dispatch)(void*);
    u32 count;
    volatile u32 ret;
    volatile u32 val;
    u32 delay;

    strBase = lbl_80271068;
    *(u32*)list = 0x02000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x28);
    list[0].reg_save_area = (u32*)((u8*)list - 0x68);
    listPtr = list;
    entry = fn_800F6D18(callback, arg, listPtr);
    if (entry == NULL) {
        return 0;
    }

    for (;;) {
        state = (u32)*(u8*)(entry + 0x4);
        if (state == 0) {
            fn_800DD970((const char*)(strBase + 0x120), *(u32*)(entry + 0x8));
            goto done;
        }
        if (state == 3) {
            *(u8*)(entry + 0x4) = 0;
            goto done;
        }
        ip = (u8*)*(u32*)(entry + 0x14);
        *(u32*)(entry + 0x14) = (u32)(ip + 1);
        opcode = (u32)*ip;
        if (opcode >= 0x26) {
            fn_800DD38C((const char*)(strBase + 0x150));
        } else {
            dispatch = (void (*)(void*))*(u32*)((u8*)lbl_803155D0 + (opcode & 0xFF) * 4);
            if (dispatch != NULL) {
                dispatch(entry);
            }
        }
        delay = *(u32*)(entry + 0x28);
        if ((s32)delay > 0) {
            for (delay--; (s32)delay >= 0; delay--) {
            }
        }
    }
done:
    if (*(u8*)(entry + 0x4) == 4) {
        *(u8*)(entry + 0x4) = 0;
    }
    count = *(u32*)(entry + 0x28);
    if ((s32)count <= 0) {
        fn_800DD38C((const char*)(strBase + 0x14));
        val = *(u32*)(entry + 0x6C);
    } else {
        count--;
        *(u32*)(entry + 0x28) = count;
        val = *(u32*)(entry + 0x6C + count * 4);
    }
    {
        void (*cb)(void*, u32) = (void (*)(void*, u32))*(u32*)(entry + 0x10);
        ret = val;
        if (cb != NULL) cb(entry, val);
    }
    return ret;
}
#endif
