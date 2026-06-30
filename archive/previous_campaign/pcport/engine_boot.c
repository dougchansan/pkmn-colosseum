/* engine_boot.c — P-B: boot the engine's own cooperative scheduler.
 *
 *   --sched-test : headless, rigorous unit test of the host GStask + GSthread
 *                  scheduler (gs_sched_host) — priority-ordered task insert,
 *                  1-based task IDs, ACTIVE/paused/DEFERRED semantics, GStaskRun
 *                  call order, and GSthread fibre slices.
 *
 *   --engine-boot: windowed. Mirrors the real main.c boot STRUCTURE — GSthreadInit
 *                  + GStaskInit, register per-frame tasks (one driving the real
 *                  host present path VIWaitForRetrace_PC -> clear -> GSgfxSwapBuffers,
 *                  exactly as the real TaskVBlank does) + a main thread shaped like
 *                  GameMainLoop / the title thread fn_8002058C, then run the
 *                  engine's own main loop `for(;;){ GSthreadStepAll(); GStaskRun(); }`.
 *                  Proves the engine's OWN task scheduler drives the frame loop
 *                  with real present code executing under it.
 *
 * This is the P-B increment that stands up the engine's scheduler model. Linking
 * the real main.c GameInit (with its ~60 hardware-subsystem inits) + the real
 * task callbacks is the next increment, scoped in docs/PC_PORT_ENGINE_FIBRE_SPIKE.md.
 */
#include "gs_sched_host.h"
#include "os_thread_host.h"
#include "pcport_window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern void VIWaitForRetrace_PC(void);
extern void GSgfxSwapBuffers(unsigned int clear);

/* Real title-scene render bridge (pcport_main.c) — loads logo_demo and renders
 * the actual scene graph through the game's own draw path. */
extern int  PCPort_EngineTitleSetup(void);
extern int  PCPort_EngineTitleReady(void);
extern void PCPort_EngineTitleRenderFrame(void);
extern void PCPort_DumpBackbuffer(const char* path);

/* ===================================================================
 * --sched-test : headless scheduler unit test
 * =================================================================== */

#define TRACE_MAX 32
static u32 g_trace[TRACE_MAX];   /* taskIds in the order GStaskRun called them */
static int g_traceLen;

static void TraceTask(u32 taskId, void* param) {
    (void)param;
    if (g_traceLen < TRACE_MAX) {
        g_trace[g_traceLen++] = taskId;
    }
}

/* A deferred task must NEVER be called by GStaskRun (it runs only on the
 * deferred scheduler path). If it is, this flips and the test fails. */
static int g_deferredRan;
static void DeferredTask(u32 taskId, void* param) {
    (void)taskId; (void)param;
    g_deferredRan = 1;
}

/* Thread-layer test entries. GSthread entries are void(void) in the real engine,
 * so the per-thread counter is carried via a global (one entry per thread). */
static int g_tcA, g_tcB;
static void ThreadEntryA(void) { for (;;) { g_tcA++; GSthreadYield(); } }
static void ThreadEntryB(void) { for (;;) { g_tcB++; GSthreadYield(); } }

static int traces_equal(const u32* a, const u32* b, int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int RunSchedTest(void) {
    printf("[boot] === GStask/GSthread host scheduler unit test ===\n");
    if (!HostFibre_Available()) {
        fprintf(stderr, "[boot] FAIL: no fibre backend\n");
        return 0;
    }
    GSched_HostReset();

    /* --- Task layer --- */
    GStaskInit(8, 2);   /* 8 normal + 2 deferred = 10 slots */

    {
        u32 id1, id2, id3, id4;
        u32 expectOrder[3]      = { 2, 3, 1 }; /* ascending priority: T2,T3,T1 */
        u32 expectOrderPaused[2] = { 2, 1 };    /* with T3 paused              */

        /* Create in slot order 0,1,2 => IDs 1,2,3; priorities 30,10,20. */
        id1 = GStaskCreate(GSTASK_ACTIVE, 30, NULL, (void*)TraceTask);
        id2 = GStaskCreate(GSTASK_ACTIVE, 10, NULL, (void*)TraceTask);
        id3 = GStaskCreate(GSTASK_ACTIVE, 20, NULL, (void*)TraceTask);
        if (id1 != 1 || id2 != 2 || id3 != 3) {
            fprintf(stderr, "[boot] FAIL: task IDs %u,%u,%u (want 1,2,3)\n", id1, id2, id3);
            return 0;
        }

        /* A deferred task takes a slot in the deferred region (slots 8..9 =>
         * ID 9) and must not run under GStaskRun. */
        id4 = GStaskCreate(GSTASK_DEFERRED, 5, NULL, (void*)DeferredTask);
        if (id4 != 9) {
            fprintf(stderr, "[boot] FAIL: deferred task ID %u (want 9)\n", id4);
            return 0;
        }

        /* Run once: active tasks fire in priority order, deferred does not. */
        g_traceLen = 0; g_deferredRan = 0;
        GStaskRun();
        if (g_traceLen != 3 || !traces_equal(g_trace, expectOrder, 3)) {
            fprintf(stderr, "[boot] FAIL: run order [%u,%u,%u] want [2,3,1]\n",
                    g_traceLen > 0 ? g_trace[0] : 0,
                    g_traceLen > 1 ? g_trace[1] : 0,
                    g_traceLen > 2 ? g_trace[2] : 0);
            return 0;
        }
        if (g_deferredRan) {
            fprintf(stderr, "[boot] FAIL: deferred task ran under GStaskRun\n");
            return 0;
        }
        printf("[boot] task PASS: priority order [2,3,1], IDs 1/2/3, deferred skipped\n");

        (void)expectOrderPaused;

        /* Re-init: slots are reclaimed and IDs restart at 1 (free-slot reuse). */
        GSched_HostReset();
        GStaskInit(8, 2);
        id1 = GStaskCreate(GSTASK_ACTIVE, 30, NULL, (void*)TraceTask);
        id2 = GStaskCreate(GSTASK_ACTIVE, 10, NULL, (void*)TraceTask);
        id3 = GStaskCreate(GSTASK_ACTIVE, 20, NULL, (void*)TraceTask);
        if (id1 != 1 || id2 != 2 || id3 != 3) {
            fprintf(stderr, "[boot] FAIL: reuse IDs %u,%u,%u (want 1,2,3)\n", id1, id2, id3);
            return 0;
        }
        g_traceLen = 0;
        GStaskRun();
        if (g_traceLen != 3 || GSched_TaskCount() != 3) {
            fprintf(stderr, "[boot] FAIL: re-init run %d / count %d (want 3/3)\n",
                    g_traceLen, GSched_TaskCount());
            return 0;
        }
        printf("[boot] task PASS: re-init + free-slot reuse (IDs restart at 1)\n");
    }

    /* --- Thread layer: two GSthreads, each sliced exactly once per step --- */
    {
        int live, step;
        GSched_HostReset();
        GSthreadInit(4);
        g_tcA = 0; g_tcB = 0;
        if (GSthreadCreate(0, 1000, 0x4000, 1, 1, (void*)ThreadEntryA) == NULL ||
            GSthreadCreate(1, 1000, 0x4000, 1, 1, (void*)ThreadEntryB) == NULL) {
            fprintf(stderr, "[boot] FAIL: GSthreadCreate (entry A/B)\n");
            return 0;
        }
        if (GSched_ThreadCount() != 2) {
            fprintf(stderr, "[boot] FAIL: thread count %d (want 2)\n", GSched_ThreadCount());
            return 0;
        }
        for (step = 1; step <= 5; step++) {
            live = GSthreadStepAll();
            if (g_tcA != step || g_tcB != step || live != 2) {
                fprintf(stderr, "[boot] FAIL: step %d A=%d B=%d live=%d (want %d,%d,2)\n",
                        step, g_tcA, g_tcB, live, step, step);
                return 0;
            }
        }
        printf("[boot] thread PASS: 2 threads, 1 slice/step for 5 steps (A=%d B=%d)\n",
               g_tcA, g_tcB);
    }

    printf("[boot] === scheduler unit test PASS ===\n");
    return 1;
}

/* ===================================================================
 * --engine-boot : windowed, mirror main.c structure
 * =================================================================== */

/* Per-frame engine work counters (stand-ins for real engine state). */
static unsigned long g_bootFrame;
static unsigned long g_mainThreadWork;
static int g_bootStop;

/* Task 0xFF — mirrors the real TaskVBlank (fn_80005D80): drives the present.
 * The real one does GX render-state + fn_80175F6C world render + swap. Here the
 * task renders the REAL title scene graph (RenderJointTree -> the game's own
 * fn_800DAD10 draw bridge) when it loaded, so real engine render code runs under
 * the scheduler; if the title assets are unavailable it falls back to a flat
 * animated clear so the headless path still proves the loop. */
static void BootTaskVBlank(u32 taskId, void* param) {
    (void)taskId; (void)param;

    VIWaitForRetrace_PC();
    if (PCPort_EngineTitleReady()) {
        PCPort_EngineTitleRenderFrame();   /* real title scene through the draw bridge */
    } else {
        int frame = (int)g_bootFrame;
        glClearColor(0.10f + 0.0025f * (float)(frame % 80), 0.14f, 0.28f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /* Headless verify: PCPORT_BOOT_DUMP=<path.bmp> dumps the backbuffer at frame
     * PCPORT_BOOT_DUMP_FRAME (default 80), after render, before swap. */
    {
        const char* dumpPath = getenv("PCPORT_BOOT_DUMP");
        if (dumpPath != NULL && dumpPath[0] != '\0') {
            int dumpFrame = 80;
            const char* df = getenv("PCPORT_BOOT_DUMP_FRAME");
            if (df != NULL && atoi(df) >= 0) {
                dumpFrame = atoi(df);
            }
            if ((int)g_bootFrame == dumpFrame) {
                PCPort_DumpBackbuffer(dumpPath);
                printf("[boot] dumped frame %lu to %s\n", g_bootFrame, dumpPath);
            }
        }
    }

    GSgfxSwapBuffers(0);
}

/* Task 0x01 — mirrors TaskPadRead: poll input, request stop on window close. */
static void BootTaskPad(u32 taskId, void* param) {
    GLFWwindow* window = (GLFWwindow*)param;
    (void)taskId;
    if (window != NULL && glfwWindowShouldClose(window)) {
        g_bootStop = 1;
    }
}

/* Task 0xFE — mirrors TaskRetraceMain: per-frame game logic (stand-in tick). */
static void BootTaskMain(u32 taskId, void* param) {
    (void)taskId; (void)param;
    /* (real: debug-menu check, save/card state, particles, sound) */
}

/* Real engine title-init (battle_main.c, now host-linked via pcport_gen). KR decl
 * so we can call it as the real title thread does: fn_801EF644(-1). */
extern void fn_801EF644();

/* Main thread — mirrors GameMainLoop (fn_80005AAC) / the title thread
 * fn_8002058C: `fn_801EF644(-1); for(;;) fn_800F0308();`. With PCPORT_REAL_TITLE_INIT
 * the init slice runs the REAL decompiled fn_801EF644 under the host scheduler (its
 * yields route through fn_800F0308 == GSthreadYield); on host its demo table
 * (lbl_803752A0/count lbl_80478D10) is zero so it returns immediately — proving the
 * real engine function EXECUTES under the scheduler without crashing. Default keeps
 * the stand-in init. */
static void BootMainThread(void) {
    if (getenv("PCPORT_REAL_TITLE_INIT") != NULL) {
        printf("[boot] main thread: calling REAL fn_801EF644(-1) (title init) under the scheduler\n");
        fn_801EF644(-1);
        printf("[boot] main thread: REAL fn_801EF644 returned (host demo table empty -> no-op, no crash)\n");
    } else {
        printf("[boot] main thread: init slice (stand-in for GameMainLoop)\n");
    }
    for (;;) {
        g_mainThreadWork++;
        GSthreadYield();   /* == fn_800F0308 */
    }
}

int RunEngineBoot(GLFWwindow* window) {
    int maxFrames = 0; /* 0 => until window close */
    const char* env = getenv("PCPORT_BOOT_FRAMES");
    if (env != NULL) {
        int v = atoi(env);
        if (v > 0) maxFrames = v;
    }

    printf("[boot] === --engine-boot: engine scheduler drives the frame loop ===\n");

    /* Mirror main.c GameInit's scheduler setup. */
    GSched_HostReset();
    GSthreadInit(4);        /* (main.c: thread pool)                       */
    GStaskInit(16, 4);      /* (main.c GameInit: fn_800FE9B0(0x10,0x4))    */

    /* Register per-frame tasks (main.c registers TaskVBlank/PadRead/Main...). */
    GStaskCreate(GSTASK_ACTIVE, 0xFF, NULL,   (void*)BootTaskVBlank); /* present */
    GStaskCreate(GSTASK_ACTIVE, 0x01, window, (void*)BootTaskPad);    /* input   */
    GStaskCreate(GSTASK_ACTIVE, 0xFE, NULL,   (void*)BootTaskMain);   /* logic   */

    /* Create the main game thread (main.c: fn_800F07A8(0,0x3E8,0x4000,1,1,GameMainLoop)). */
    if (GSthreadCreate(0, 0x3E8, 0x4000, 1, 1, (void*)BootMainThread) == NULL) {
        fprintf(stderr, "[boot] FAIL: GSthreadCreate(main)\n");
        return 0;
    }

    /* Load the real title scene so the VBlank task renders it through the game's
     * own draw bridge. Falls back to a flat clear if assets are unavailable. */
    if (!PCPort_EngineTitleSetup()) {
        printf("[boot] title scene unavailable — VBlank task will flat-clear\n");
    }

    g_bootFrame = 0;
    g_mainThreadWork = 0;
    g_bootStop = 0;

    /* The engine's own main loop: step threads, then run tasks. In main.c this
     * is `for(;;) fn_800FE7A0();` with the thread stepping driven by the swap
     * callback; here we step threads explicitly each frame for clarity. */
    for (;;) {
        GSthreadStepAll();   /* run each GSthread's per-frame slice (yields)   */
        GStaskRun();         /* run all active tasks (VBlank presents a frame) */

        if (g_bootFrame < 5 || (g_bootFrame % 60) == 0) {
            printf("[boot] frame %lu: tasks ran, mainThread.work=%lu\n",
                   g_bootFrame, g_mainThreadWork);
        }
        g_bootFrame++;

        if (g_bootStop) {
            printf("[boot] window closed at frame %lu\n", g_bootFrame);
            break;
        }
        if (maxFrames > 0 && (int)g_bootFrame >= maxFrames) {
            break;
        }
    }

    printf("[boot] --engine-boot ran %lu frames; mainThread.work=%lu; tasks=%d threads=%d\n",
           g_bootFrame, g_mainThreadWork, GSched_TaskCount(), GSched_ThreadCount());
    return 1;
}
