/* engine_host.c — see engine_host.h.
 *
 * Reimplements the engine's cooperative scheduler semantics on host fibres.
 * The host/main fibre is the scheduler + present driver; engine threads are
 * fibres that yield back to it via EngineHost_VsyncYield (the fn_800F0308
 * host equivalent).
 */
#include "engine_host.h"
#include "os_thread_host.h"

#include <stdio.h>
#include <stdlib.h>

#define ENGINE_HOST_MAX_THREADS 16
#define ENGINE_HOST_DEFAULT_STACK (256 * 1024)

struct EngineThread {
    HostFibre*       fibre;
    EngineThreadFunc func;
    void*            arg;
    int              started;   /* has the scheduler resumed it at least once   */
    int              finished;  /* entry returned (engine threads normally loop) */
    unsigned long    sliceCount;/* per-frame slices executed (yield round-trips) */
};

static int           g_ready = 0;
static HostFibre*    g_hostFibre = NULL;
static EngineThread  g_threads[ENGINE_HOST_MAX_THREADS];
static int           g_threadCount = 0;
static int           g_frame = 0;

/* The thread whose slice is currently executing (so VsyncYield knows who is
 * handing control back to the host). Set by the scheduler before resuming a
 * thread; cleared while the host fibre runs. */
static EngineThread* g_running = NULL;

int EngineHost_Init(void) {
    if (g_ready) {
        return 1;
    }
    if (!HostFibre_Available()) {
        fprintf(stderr, "[engine_host] no fibre backend available on this host\n");
        return 0;
    }
    g_hostFibre = HostFibre_InitMain();
    if (g_hostFibre == NULL) {
        fprintf(stderr, "[engine_host] HostFibre_InitMain failed\n");
        return 0;
    }
    g_threadCount = 0;
    g_frame = 0;
    g_running = NULL;
    g_ready = 1;
    printf("[engine_host] cooperative-fibre scheduler initialised (host fibre live)\n");
    return 1;
}

int EngineHost_Ready(void) {
    return g_ready;
}

/* Trampoline body for an engine thread fibre: run the user func, mark finished.
 * Engine threads normally never return (they loop on VsyncYield forever); if one
 * does, we mark it finished and yield back so the scheduler stops resuming it. */
static void EngineHost_ThreadTrampoline(void* param) {
    EngineThread* t = (EngineThread*)param;
    if (t != NULL && t->func != NULL) {
        t->func(t->arg);
    }
    if (t != NULL) {
        t->finished = 1;
    }
    /* Hand control back to the host; the scheduler will not resume a finished
     * thread again. Loop in case we are ever resumed after finishing. */
    for (;;) {
        HostFibre_SwitchTo(g_hostFibre);
    }
}

EngineThread* EngineHost_CreateThread(EngineThreadFunc func, void* arg,
                                      unsigned int stackSize) {
    if (!g_ready && !EngineHost_Init()) {
        return NULL;
    }
    if (g_threadCount >= ENGINE_HOST_MAX_THREADS) {
        fprintf(stderr, "[engine_host] thread table full (%d)\n", ENGINE_HOST_MAX_THREADS);
        return NULL;
    }

    EngineThread* t = &g_threads[g_threadCount];
    t->func = func;
    t->arg = arg;
    t->started = 0;
    t->finished = 0;
    t->sliceCount = 0;
    t->fibre = HostFibre_Create(EngineHost_ThreadTrampoline, t,
                                stackSize ? stackSize : ENGINE_HOST_DEFAULT_STACK);
    if (t->fibre == NULL) {
        fprintf(stderr, "[engine_host] HostFibre_Create failed\n");
        return NULL;
    }
    g_threadCount++;
    printf("[engine_host] created engine thread %d (fibre=%p)\n",
           g_threadCount - 1, (void*)t->fibre);
    return t;
}

void EngineHost_VsyncYield(void) {
    /* Called from inside an engine thread. Record the slice and hand control
     * back to the host/scheduler fibre. Control returns here next frame. This is
     * the exact host analogue of fn_800F0308: "block until the next vsync." */
    if (g_running != NULL) {
        g_running->sliceCount++;
    }
    HostFibre_SwitchTo(g_hostFibre);
}

int EngineHost_FrameCount(void) {
    return g_frame;
}

int EngineHost_Run(int maxFrames, EngineFramePresent present, void* user) {
    int ticked = 0;

    if (!g_ready) {
        fprintf(stderr, "[engine_host] EngineHost_Run before Init\n");
        return 0;
    }
    if (g_threadCount == 0) {
        fprintf(stderr, "[engine_host] EngineHost_Run with no engine threads\n");
        return 0;
    }

    for (;;) {
        int i;
        int liveThisFrame = 0;

        if (maxFrames > 0 && ticked >= maxFrames) {
            break;
        }

        /* Resume each engine thread for its per-frame slice. Each runs until it
         * calls EngineHost_VsyncYield (or finishes). This is the cooperative
         * round-robin: control only moves when a thread explicitly yields. */
        for (i = 0; i < g_threadCount; i++) {
            EngineThread* t = &g_threads[i];
            if (t->finished) {
                continue;
            }
            t->started = 1;
            g_running = t;
            HostFibre_SwitchTo(t->fibre);   /* runs the thread's slice           */
            g_running = NULL;               /* back on the host fibre            */
            if (!t->finished) {
                liveThisFrame++;
            }
        }

        /* Present one frame on the host fibre (VIWaitForRetrace/swap/poll). */
        if (present != NULL) {
            if (!present(g_frame, user)) {
                ticked++;
                g_frame++;
                break;  /* present asked to stop (window closed / cap reached) */
            }
        }

        ticked++;
        g_frame++;

        if (liveThisFrame == 0) {
            /* All engine threads have finished; nothing left to drive. */
            printf("[engine_host] all engine threads finished after %d frames\n", ticked);
            break;
        }
    }

    return ticked;
}
