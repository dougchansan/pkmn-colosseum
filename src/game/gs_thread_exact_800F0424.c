/**
 * @file gs_thread_exact_800F0424.c
 * @brief GSthread public thread API, scheduler and exit trampoline.
 *
 * Address range: 0x800F0424 - 0x800F106C (the tail of the retail GSthread
 * translation unit), in address order:
 *   GSthreadUnblock, GSthreadBlock, GSthreadExecuteAll, GSthreadExecuteGroup,
 *   GSthreadClose, GSthreadIsRunning, GSthreadTerminateGroup,
 *   GSthreadTerminate, GSthreadSetArgs, GSthreadCreate, GSthread,
 *   fn_800F0A74 (dispatcher), fn_800F0F4C (exit trampoline).
 *
 * The TU ends at 0x800F106C: the GSthread strings end at 0x80271062 and the
 * next string (lbl_80271068) starts on a fresh 8-byte boundary, and the
 * .sbss globals past 0x8047AC34 are only used by the GS VM code that follows.
 * The .rodata strings and .sbss globals stay extern.
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"

extern void GSlogWrite(const void* fmt, ...);
extern void GSmemFree(u16 handle);           /* GSmemFree */
extern void* fn_800E24B0(u16 handle);          /* GSmemUnlock */
extern void* GSmemLock(u16 handle);          /* GSmemGetPtr */
extern u16 GSmemAlloc(u32 alignment, u32 size);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void threadSaveGPRRegisters(void);
extern void threadSaveFPRRegisters(void);
extern void threadExecute(void);
extern void fn_800F02F4(void);
extern void _threadSwitch(void);
extern void GSscratchFree(void*);
extern u8 GSscratchIsPtr(void*);
extern void GSscratchStore(void*, void*, s32);
extern void GSscratchWaitForCompletion(void);
extern void DCFlushRange(void*, s32);

extern const char lbl_80271008[]; /* "GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n" */
extern const char lbl_80271038[]; /* "GSthread: Init OK, maximum of %d threads\n" */

extern u8 lbl_804019F0[];  /* scheduler context block */
extern u32 lbl_8047AC00;   /* current thread */
extern u32 lbl_8047AC04;   /* next thread */
extern u32 lbl_8047AC08;   /* head of the affinity-sorted thread list */
extern u8 lbl_8047AC0C;    /* reschedule flag */
extern u32 lbl_8047AC10;   /* switch saves/restores FPRs */
extern u32 lbl_8047AC1C;   /* context block for the register save/load primitives */
extern u32 lbl_8047AC20;   /* "run" context */
extern u32 lbl_8047AC24;   /* "next" context */
extern GSThread* lbl_8047AC28; /* thread array */
extern u16 lbl_8047AC2C;   /* thread array GSmem handle */
extern u32 lbl_8047AC30;   /* maximum thread count */

extern void fn_800F0F4C(u32 result);

/* 0x800F0424 | 0x14 */
void GSthreadUnblock(GSThread* thr) {
    thr->suspended = 0;
    lbl_8047AC0C = 1;
}

/* 0x800F0438 | 0x10 */
void GSthreadBlock(u8* p) {
    p[0x9] = 1;
    lbl_8047AC0C = 1;
}

/* thread wake/sleep dispatch -- declared old-style (no prototype) because
 * the real definition of fn_800F0A74 (further below) is itself an old-style
 * K&R definition, which mwcc treats as an unprototyped "(...)" type; a
 * modern prototype here would conflict with that declaration. */
extern void fn_800F0A74();

/* 0x800F0448 | 0x28 */
void GSthreadExecuteAll(void) {
    fn_800F0A74(0, 0);
}

/* 0x800F0470 | 0x24 */
void GSthreadExecuteGroup(u32 priority) {
    fn_800F0A74(priority, 1);
}

/* 0x800F0494 | 0x28 */
u32 GSthreadClose(GSThread* thr) {
    u32 ret;
    if (thr->autoStart == 1) {
        return 0;
    }
    ret = thr->unused;
    thr->active = 0;
    return ret;
}

/* 0x800F04BC | 0x8 */
u8 GSthreadIsRunning(u8* p) { return p[0x14]; }

/* 0x800F04C4 | 0xDC */
/*
 * GSthreadTerminateGroup -- destroy every thread whose priority == @p priority.
 *
 * Walks the global thread list (head lbl_8047AC08); for each matching
 * thread it applies the same destroy/unlink logic as GSthreadTerminate
 * (inlined): current/run threads are deferred via the 0x15 pending flag
 * (+ lbl_8047AC0C reschedule for the run thread); all others are
 * unlinked from the list and have their stack/ctx GSmem handles freed.
 */
void GSthreadTerminateGroup(u32 priority) {
    GSThread* thr;
    for (thr = (GSThread*)lbl_8047AC08; thr != NULL; thr = thr->next) {
        if (thr->priority == priority) {
            if (thr == (GSThread*)lbl_8047AC00 || thr == (GSThread*)lbl_8047AC04) {
                thr->pad1 = 1;
                if (thr == (GSThread*)lbl_8047AC04) {
                    lbl_8047AC0C = 1;
                }
            } else {
                thr->pad0 = 0;
                thr->active = 0;
                if (thr->prev != NULL) {
                    thr->prev->next = thr->next;
                }
                if (thr->next != NULL) {
                    thr->next->prev = thr->prev;
                }
                if ((GSThread*)lbl_8047AC08 == thr) {
                    lbl_8047AC08 = (u32)thr->next;
                }
                GSmemFree(thr->stackHandle);
                GSmemFree(thr->ctxHandle);
            }
        }
    }
}

/* 0x800F05A0 | 0xB4 */
/*
 * GSthreadTerminate -- destroy / unregister a cooperative thread (GSThread).
 *
 * If @p thr is the current thread (lbl_8047AC00) or the run thread
 * (lbl_8047AC04) it cannot be torn down in place, so a destroy-pending
 * flag at offset 0x15 is raised instead; when it is the run thread the
 * global reschedule flag (lbl_8047AC0C) is set so the dispatcher re-enters.
 *
 * Otherwise the thread is unlinked from the doubly-linked thread list,
 * the list head (lbl_8047AC08) is fixed up if it pointed at this thread,
 * and both the stack and context GSmem handles are released via
 * GSmemFree (GSmemFree).
 *
 * Parameter kept as u32 (not GSThread*) to match the other GSthreadTerminate
 * extern declarations/call sites (fn_800F716C, fn_800F7274), which pass a
 * raw GSThread* value stored in a u32 slot.
 */
void GSthreadTerminate(u32 ctxArg) {
    GSThread* thr = (GSThread*)ctxArg;
    if (thr == (GSThread*)lbl_8047AC00 || thr == (GSThread*)lbl_8047AC04) {
        thr->pad1 = 1;
        if (thr == (GSThread*)lbl_8047AC04) {
            lbl_8047AC0C = 1;
        }
    } else {
        thr->pad0 = 0;
        thr->active = 0;
        if (thr->prev != NULL) {
            thr->prev->next = thr->next;
        }
        if (thr->next != NULL) {
            thr->next->prev = thr->prev;
        }
        if ((GSThread*)lbl_8047AC08 == thr) {
            lbl_8047AC08 = (u32)thr->next;
        }
        GSmemFree(thr->stackHandle);
        GSmemFree(thr->ctxHandle);
    }
}

typedef struct GSthreadVaInfo {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} GSthreadVaInfo;
typedef GSthreadVaInfo GSthreadVaList[1];

#define GS_THREAD_VA_START(ap, last) \
    ((void)(last), __builtin_va_info(&(ap)))

extern u32* __va_arg(void* ap, u32 type);

/* 0x800F0654 | 0x154 */
void GSthreadSetArgs(void* threadPtr, s32 count, ...)
{
    GSThread* thread = threadPtr;
    GSthreadVaList args;
    u32* stack;
    GSThreadCtx* context;
    s32 i;
    s32 directCount;
    u32 reg;
    u32 stackIndex;

    if (thread->pad0 == 1) {
        return;
    }

    stack = GSmemLock(thread->stackHandle);
    context = GSmemLock(thread->ctxHandle);
    GS_THREAD_VA_START(args, count);

    /* The first eight arguments go in the saved r3-r10 slots... */
    reg = 3;
    directCount = 8;
    if (count < 8) {
        directCount = count;
    }
    for (i = 0; i < directCount; i++) {
        context->gpr[reg++] = *__va_arg(args, 1);
    }

    /* ...the rest are spilled to the parameter area of the new thread's
     * initial stack frame (8 bytes above its saved stack pointer). */
    if (count > 8) {
        context->gpr[1] -= (count - i) * sizeof(u32);
        stackIndex = (context->gpr[1] >> 2) + 2;
        for (i = 8; i < count; i++) {
            stack[stackIndex++] = *__va_arg(args, 1);
        }
    }

    fn_800E24B0(thread->stackHandle);
    fn_800E24B0(thread->ctxHandle);
}

#undef GS_THREAD_VA_START

/* =======================================================================
 *  GSthreadCreate
 *  Address: 0x800F07A8, Size: 0x230
 *
 *  Creates a cooperative thread: claims a free slot, allocates its stack
 *  (GSmemAlloc, 32-byte aligned) and register context block
 *  (_toolentryAlloc__FUl; 0x88 bytes, or 0x188 with FPU state), seeds the
 *  context so the thread starts in fn_800F0F4C, and links it into the
 *  affinity-sorted thread list.
 * ======================================================================= */
/*
 * Return the first unused slot in the thread array, or NULL when every slot
 * is taken. Inlined into GSthreadCreate: the target materialises the NULL
 * result (li r31, 0) on loop exhaustion and then re-tests it in the caller's
 * own NULL check, and the found path branches straight to that re-test.
 * Same search idiom as floorFindDataEntry in gs_floor.c.
 */
static inline GSThread* threadFindFreeSlot(void) {
    GSThread* thread;
    u32 count;

    thread = lbl_8047AC28;
    for (count = lbl_8047AC30; count != 0; count--) {
        if (thread->active == 0) {
            return thread;
        }
        thread++;
    }
    return NULL;
}

GSThread* GSthreadCreate(u32 affinity, u32 priority, u32 stackSize,
                         u8 usesFPU, u32 autoStart, void* entryFunc) {
    GSThread* thread;
    GSThread* curr;
    GSThread* next;
    GSThreadCtx* ctx;
    s32* stack;

    if (usesFPU == 0) {
        GSlogWrite(lbl_80271008);
    }

    thread = threadFindFreeSlot();
    if (thread == NULL) {
        return NULL;
    }

    thread->stackHandle = GSmemAlloc(stackSize, 0x20);
    if (thread->stackHandle == 0) {
        return NULL;
    }

    thread->ctxHandle = _toolentryAlloc__FUl(usesFPU ? 0x188 : 0x88);
    if (thread->ctxHandle == 0) {
        GSmemFree(thread->stackHandle);
        return NULL;
    }

    thread->active = 1;
    thread->priority = priority;
    thread->stackSize = stackSize;
    thread->pad0 = 0;
    thread->pad1 = 0;
    thread->usesFPU = usesFPU;
    thread->entryFunc = entryFunc;
    thread->suspended = 0;
    thread->sleeping = 0;
    thread->affinity = affinity;
    thread->prev = NULL;
    thread->next = NULL;
    thread->autoStart = autoStart;

    /* Build the initial register context: the new thread starts in the
     * fn_800F0F4C trampoline with entryFunc in the saved LR slot and its
     * stack pointer 8 bytes below the top of its stack. */
    ctx = GSmemLock(thread->ctxHandle);
    stack = GSmemLock(thread->stackHandle);

    lbl_8047AC1C = (u32)ctx;
    threadSaveGPRRegisters();
    if (thread->usesFPU) {
        threadSaveFPRRegisters();
    }

    ctx->gpr[1] = thread->stackSize - 8;
    ctx->lr = (u32)thread->entryFunc;
    ctx->ctr = (u32)fn_800F0F4C;
    *stack = -1; /* stack-overflow sentinel */

    fn_800E24B0(thread->ctxHandle);
    fn_800E24B0(thread->stackHandle);

    /* Insert into the thread list, kept sorted by ascending affinity. */
    curr = (GSThread*)lbl_8047AC08;
    if (curr == NULL) {
        lbl_8047AC08 = (u32)thread;
    } else {
        while ((next = curr->next) != NULL && curr->affinity < thread->affinity) {
            curr = next;
        }

        if (next == NULL && curr->affinity < thread->affinity) {
            thread->prev = curr;
            thread->next = NULL;
            curr->next = thread;
        } else {
            if (curr->prev != NULL) {
                curr->prev->next = thread;
            }
            thread->prev = curr->prev;
            thread->next = curr;
            curr->prev = thread;
            if ((GSThread*)lbl_8047AC08 == curr) {
                lbl_8047AC08 = (u32)thread;
            }
        }
    }

    lbl_8047AC0C = 1;
    return thread;
}

/* =======================================================================
 *  GSthread
 *  Address: 0x800F09D8, Size: 0x9C
 *
 *  Allocates the cooperative thread pool from GSmem (maxThreads * 0x24
 *  bytes), clears each slot's active flag, resets the current thread and
 *  the thread list head and logs
 *  "GSthread: Init OK, maximum of %d threads\n".
 * ======================================================================= */
s32 GSthread(u32 maxThreads) {
    u32 handle;
    u32 i;

    lbl_8047AC30 = maxThreads;
    handle = _toolentryAlloc__FUl(maxThreads * 0x24);
    lbl_8047AC2C = handle;
    if (handle == 0) {
        return 0;
    }
    lbl_8047AC28 = GSmemLock(handle);

    for (i = 0; i < lbl_8047AC30; i++) {
        lbl_8047AC28[i].active = 0;
    }

    lbl_8047AC00 = 0;
    lbl_8047AC08 = 0;
    GSlogWrite(lbl_80271038, lbl_8047AC30);
    return 1;
}

/* 0x800F0A74 | 0x4D8 */
/*
 * Return the first runnable thread after @p from (or from the list head when
 * @p from is NULL) -- active, not suspended, not sleeping, not pending
 * destruction and, when @p groupOnly is set, of priority @p priority.
 * Inlined four times into fn_800F0A74: every expansion materialises the NULL
 * result (li rN, 0) on list exhaustion and re-tests it in the caller, and the
 * found path branches straight to that re-test (same idiom as
 * threadFindFreeSlot above).
 */
static inline GSThread* threadFindRunnable(GSThread* from, u32 priority, u8 groupOnly) {
    GSThread* thread;

    if (from == NULL) {
        thread = (GSThread*)lbl_8047AC08;
    } else {
        thread = from->next;
    }
    while (thread != NULL) {
        if (thread->active != 0 && thread->suspended == 0 && thread->sleeping == 0 && thread->pad1 == 0) {
            if (groupOnly == 0 || thread->priority == priority) {
                return thread;
            }
        }
        thread = thread->next;
    }
    return NULL;
}

/*
 * Thread dispatcher behind GSthreadExecuteAll/GSthreadExecuteGroup: run every
 * runnable thread (only those of priority @p arg0 when @p arg1 is set) once.
 * The next thread is looked up and its stack fetched before the current one
 * runs; if a thread raised the reschedule flag (lbl_8047AC0C) and that
 * prefetched choice is now stale, it is released (destroying it if it was
 * marked for destruction meanwhile) and the search is repeated. nextStack is
 * only assigned when a next thread exists -- the target has no initialiser.
 */
void fn_800F0A74(arg0, arg1)
    u32 arg0;
    u8 arg1;
{
    GSThread *next;
    GSThread *dead;
    GSThreadCtx *ctx;
    u8 *stack;
    u8 *scratchStack;
    u8 *nextStack;
    GSThread *thread;

    thread = threadFindRunnable(NULL, arg0, arg1);
    if (thread == NULL) {
        return;
    }

    ctx = (GSThreadCtx *)GSmemLock(thread->ctxHandle);
    stack = (u8 *)GSmemLock(thread->stackHandle);
    scratchStack = stack;

    while (thread != NULL) {
        next = threadFindRunnable(thread, arg0, arg1);
        lbl_8047AC04 = (u32)next;
        if (next != NULL) {
            nextStack = (u8 *)GSmemLock(next->stackHandle);
        }

        lbl_8047AC00 = (u32)thread;
        thread->pad0 = 1;
        ctx->gpr[1] = (u32)(scratchStack + ctx->gpr[1]);
        ((GSThreadCtx *)lbl_804019F0)->ctr = (u32)fn_800F02F4;
        lbl_8047AC24 = (u32)&lbl_804019F0;
        lbl_8047AC20 = (u32)ctx;
        lbl_8047AC10 = thread->usesFPU;
        threadExecute();
        ctx->gpr[1] -= (u32)scratchStack;
        lbl_8047AC00 = 0;

        if (thread->pad1 != 0) {
            if (GSscratchIsPtr(scratchStack) != 0) {
                GSscratchFree(scratchStack);
            }
            fn_800E24B0(thread->ctxHandle);
            fn_800E24B0(thread->stackHandle);

            if ((u32)thread == lbl_8047AC00 || (u32)thread == lbl_8047AC04) {
                thread->pad1 = 1;
                if ((u32)thread == lbl_8047AC04) {
                    lbl_8047AC0C = 1;
                }
            } else {
                thread->pad0 = 0;
                thread->active = 0;
                if (thread->prev != NULL) {
                    thread->prev->next = thread->next;
                }
                if (thread->next != NULL) {
                    thread->next->prev = thread->prev;
                }
                if ((GSThread *)lbl_8047AC08 == thread) {
                    lbl_8047AC08 = (u32)thread->next;
                }
                GSmemFree(thread->stackHandle);
                GSmemFree(thread->ctxHandle);
            }
        } else {
            if (GSscratchIsPtr(scratchStack) != 0) {
                DCFlushRange(stack, thread->stackSize);
                GSscratchStore(stack, scratchStack, thread->stackSize);
                GSscratchWaitForCompletion();
                GSscratchFree(scratchStack);
            }
            fn_800E24B0(thread->ctxHandle);
            fn_800E24B0(thread->stackHandle);
        }

        if (lbl_8047AC0C == 0 || (GSThread *)lbl_8047AC04 == threadFindRunnable(thread, arg0, arg1)) {
            next = (GSThread *)lbl_8047AC04;
            thread = next;
            if (next != NULL) {
                scratchStack = nextStack;
                stack = nextStack;
                ctx = (GSThreadCtx *)GSmemLock(next->ctxHandle);
            }
        } else {
            if (GSscratchIsPtr(nextStack) != 0) {
                GSscratchWaitForCompletion();
                GSscratchFree(nextStack);
            }
            if ((GSThread *)lbl_8047AC04 != NULL) {
                fn_800E24B0(((GSThread *)lbl_8047AC04)->stackHandle);
                dead = (GSThread *)lbl_8047AC04;
                if (dead->pad1 != 0) {
                    lbl_8047AC04 = 0;
                    if ((u32)dead == lbl_8047AC00 || (u32)dead == lbl_8047AC04) {
                        dead->pad1 = 1;
                        if ((u32)dead == lbl_8047AC04) {
                            lbl_8047AC0C = 1;
                        }
                    } else {
                        dead->pad0 = 0;
                        dead->active = 0;
                        if (dead->prev != NULL) {
                            dead->prev->next = dead->next;
                        }
                        if (dead->next != NULL) {
                            dead->next->prev = dead->prev;
                        }
                        if ((GSThread *)lbl_8047AC08 == dead) {
                            lbl_8047AC08 = (u32)dead->next;
                        }
                        GSmemFree(dead->stackHandle);
                        GSmemFree(dead->ctxHandle);
                    }
                }
            }
            thread = threadFindRunnable(thread, arg0, arg1);
            lbl_8047AC04 = (u32)thread;
            if (thread != NULL) {
                ctx = (GSThreadCtx *)GSmemLock(thread->ctxHandle);
                stack = (u8 *)GSmemLock(thread->stackHandle);
                scratchStack = stack;
            }
        }

        lbl_8047AC0C = 0;
    }
}

/* 0x800F0F4C | 0x120 */
/*
 * Thread exit trampoline: GSthreadCreate seeds ctx->ctr with this function,
 * so a thread's entry function returns here with its result in r3. The
 * result is kept in the thread slot for GSthreadClose. An autoStart thread
 * is then destroyed exactly as GSthreadTerminate would do it; any other
 * thread is only marked finished and unlinked, leaving its slot and GSmem
 * handles for GSthreadClose. Either way control switches away for good.
 */
void fn_800F0F4C(u32 result) {
    GSThread* thread = (GSThread*)lbl_8047AC00;

    thread->unused = result;
    if (thread->autoStart == 1) {
        if (thread == (GSThread*)lbl_8047AC00 || thread == (GSThread*)lbl_8047AC04) {
            thread->pad1 = 1;
            if (thread == (GSThread*)lbl_8047AC04) {
                lbl_8047AC0C = 1;
            }
        } else {
            thread->pad0 = 0;
            thread->active = 0;
            if (thread->prev != NULL) {
                thread->prev->next = thread->next;
            }
            if (thread->next != NULL) {
                thread->next->prev = thread->prev;
            }
            if ((GSThread*)lbl_8047AC08 == thread) {
                lbl_8047AC08 = (u32)thread->next;
            }
            GSmemFree(thread->stackHandle);
            GSmemFree(thread->ctxHandle);
        }
    } else {
        thread->pad1 = 1;
        thread->pad0 = 0;
        if (thread->prev != NULL) {
            thread->prev->next = thread->next;
        }
        if (thread->next != NULL) {
            thread->next->prev = thread->prev;
        }
        if ((GSThread*)lbl_8047AC08 == thread) {
            lbl_8047AC08 = (u32)thread->next;
        }
        lbl_8047AC0C = 1;
    }
    _threadSwitch();
}
