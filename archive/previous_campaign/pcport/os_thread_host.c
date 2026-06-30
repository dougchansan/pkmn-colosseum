/* os_thread_host.c — Win32-fibre implementation of the host cooperative-fibre
 * primitive layer declared in os_thread_host.h.
 *
 * Win32 Fibers are the clean fit for the GameCube engine's cooperative
 * scheduler: each fibre owns its own stack and CPU register state, and
 * SwitchToFiber performs an explicit, non-preemptive context switch that
 * preserves the suspended fibre's entire call stack — exactly the semantics of
 * the engine's PowerPC `fn_800F0308` vsync-yield, minus the manual register
 * save/restore (the OS does that for us on x86).
 */
#include "os_thread_host.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

struct HostFibre {
    void*          handle;     /* LPVOID from CreateFiber / ConvertThreadToFiber */
    HostFibreEntry entry;      /* user entry (NULL for the host/main fibre)      */
    void*          arg;        /* user arg                                       */
    int            isMain;     /* 1 for the converted host thread fibre          */
    unsigned long  resumeCount;/* times switched INTO this fibre                 */
    HostFibre*     returnTo;   /* fibre to resume if entry returns               */
};

/* Per-process state. The port is single-windowed/single-engine-threaded, so a
 * thread-local "current fibre" pointer is sufficient; the engine scheduler runs
 * on one OS thread. */
static HostFibre* g_mainFibre    = NULL;
static HostFibre* g_currentFibre = NULL;

HostFibre* HostFibre_InitMain(void) {
    if (g_mainFibre != NULL) {
        return g_mainFibre;
    }

    HostFibre* f = (HostFibre*)calloc(1, sizeof(HostFibre));
    if (f == NULL) {
        return NULL;
    }

    /* If this thread is already a fibre (e.g. converted elsewhere), reuse it;
     * otherwise convert. FIBER_FLAG_FLOAT_SWITCH preserves x87/SSE float state
     * across switches — the engine threads do heavy FP work. */
    if (IsThreadAFiber()) {
        f->handle = GetCurrentFiber();
    } else {
        f->handle = ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
        if (f->handle == NULL) {
            f->handle = ConvertThreadToFiber(NULL); /* fallback w/o float flag */
        }
    }
    if (f->handle == NULL) {
        free(f);
        return NULL;
    }

    f->isMain = 1;
    f->resumeCount = 1; /* the host fibre is "running" from the start */
    g_mainFibre = f;
    g_currentFibre = f;
    return f;
}

HostFibre* HostFibre_Main(void) {
    return g_mainFibre;
}

static void WINAPI HostFibre_Trampoline(void* param) {
    HostFibre* self = (HostFibre*)param;
    if (self != NULL && self->entry != NULL) {
        self->entry(self->arg);
    }
    /* Entry returned (engine threads normally never do). Hand control back to
     * the designated return fibre — the host/main fibre by default. A fibre
     * must never simply "fall off the end"; that would terminate the thread. */
    {
        HostFibre* back = (self != NULL && self->returnTo != NULL)
                              ? self->returnTo
                              : g_mainFibre;
        for (;;) {
            if (back != NULL) {
                HostFibre_SwitchTo(back);
            }
            /* If we are ever resumed again after returning, just yield back. */
        }
    }
}

HostFibre* HostFibre_Create(HostFibreEntry entry, void* arg, unsigned int stackCommit) {
    if (g_mainFibre == NULL) {
        /* The host fibre must exist first so a returning entry has somewhere to
         * go and SwitchToFiber has a valid current fibre. */
        if (HostFibre_InitMain() == NULL) {
            return NULL;
        }
    }

    HostFibre* f = (HostFibre*)calloc(1, sizeof(HostFibre));
    if (f == NULL) {
        return NULL;
    }
    f->entry = entry;
    f->arg = arg;
    f->isMain = 0;
    f->resumeCount = 0;
    f->returnTo = g_mainFibre;

    /* dwStackCommitSize as the reserve hint; CreateFiberEx with FLOAT_SWITCH. */
    f->handle = CreateFiberEx((SIZE_T)stackCommit, 0, FIBER_FLAG_FLOAT_SWITCH,
                              (LPFIBER_START_ROUTINE)HostFibre_Trampoline, f);
    if (f->handle == NULL) {
        f->handle = CreateFiber((SIZE_T)stackCommit,
                                (LPFIBER_START_ROUTINE)HostFibre_Trampoline, f);
    }
    if (f->handle == NULL) {
        free(f);
        return NULL;
    }
    return f;
}

void HostFibre_SwitchTo(HostFibre* target) {
    if (target == NULL || target == g_currentFibre) {
        return;
    }
    g_currentFibre = target;
    target->resumeCount++;
    SwitchToFiber(target->handle);
    /* When we get back here, we are running as the fibre that issued the switch
     * away from us; restore the "current" pointer is handled by whoever resumed
     * us setting g_currentFibre to themselves before SwitchToFiber. */
}

HostFibre* HostFibre_Current(void) {
    return g_currentFibre;
}

void HostFibre_Destroy(HostFibre* f) {
    if (f == NULL || f == g_currentFibre || f->isMain) {
        return;
    }
    if (f->handle != NULL) {
        DeleteFiber(f->handle);
    }
    free(f);
}

int HostFibre_Available(void) {
    return 1;
}

unsigned long HostFibre_ResumeCount(HostFibre* f) {
    return f != NULL ? f->resumeCount : 0UL;
}

#else /* !_WIN32 ---------------------------------------------------------- */

/* Non-Windows hosts: a ucontext (POSIX) or thread+condvar backend would go
 * here. The port currently targets Windows only, so these are unimplemented
 * and HostFibre_Available() reports 0 so callers can degrade gracefully. */

struct HostFibre { int dummy; };

HostFibre* HostFibre_InitMain(void) { return NULL; }
HostFibre* HostFibre_Main(void) { return NULL; }
HostFibre* HostFibre_Create(HostFibreEntry e, void* a, unsigned int s) {
    (void)e; (void)a; (void)s; return NULL;
}
void HostFibre_SwitchTo(HostFibre* t) { (void)t; }
HostFibre* HostFibre_Current(void) { return NULL; }
void HostFibre_Destroy(HostFibre* f) { (void)f; }
int HostFibre_Available(void) { return 0; }
unsigned long HostFibre_ResumeCount(HostFibre* f) { (void)f; return 0UL; }

#endif
