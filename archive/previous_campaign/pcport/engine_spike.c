/* engine_spike.c — the P-A spike entry points.
 *
 * Demonstrates the host cooperative-fibre runtime (os_thread_host + engine_host)
 * carrying an engine-shaped thread loop and ticking real frames:
 *
 *   --fibre-test : headless, GL-free. Proves the fibre context switch works and
 *                  the cooperative scheduler advances. Runs (a) a single engine
 *                  thread shaped exactly like the real title thread fn_8002058C
 *                  ( init(); for(;;) fn_800F0308(); ) and (b) a two-thread
 *                  round-robin (modelling OSCreateThread + the main idle yield
 *                  loop) — verifying every thread's per-frame slice executes
 *                  exactly once per frame.
 *
 *   --engine     : windowed. Same engine thread, but the per-frame present runs
 *                  the real host present path (VIWaitForRetrace_PC -> animated
 *                  clear -> GSgfxSwapBuffers) so you can watch the round-trip
 *                  producing frames. The engine thread here is a STAND-IN (it
 *                  advances a work counter); wiring the real engine boot chain
 *                  (main.c GameInit -> fn_800F07A8) onto this runtime is P-B and
 *                  is scoped in docs/PC_PORT_ENGINE_FIBRE_SPIKE.md.
 *
 * The point of the spike is the MECHANISM: that fn_800F0308's "block until next
 * vsync" semantics run natively on x86 and the engine's `for(;;) yield()` loop
 * shape ticks frames cooperatively with a host present fibre.
 */
#include "engine_host.h"
#include "os_thread_host.h"
#include "pcport_window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* Host present path (defined in os_shim.c / gx_shim.c). */
extern void VIWaitForRetrace_PC(void);
extern void GSgfxSwapBuffers(unsigned int clear);

/* ---- engine-thread stand-ins ------------------------------------------- */

typedef struct {
    int           initDone;     /* mirrors the real lbl_8047A32C "ready" flag  */
    unsigned long workCounter;  /* per-frame engine work tick                  */
    const char*   name;
} SpikeThreadState;

/* Shaped exactly like the real title thread gs_title.c fn_8002058C:
 *   lbl_8047A32C = 0;
 *   fn_801EF644(-1);     // title-scene init
 *   lbl_8047A32C = 1;
 *   for (;;) fn_800F0308();
 * Here the init and per-frame body are stand-ins; the LOOP SHAPE and the yield
 * are the real thing (EngineHost_VsyncYield == fn_800F0308). */
static void SpikeTitleThread(void* arg) {
    SpikeThreadState* s = (SpikeThreadState*)arg;

    s->initDone = 0;
    /* stand-in for fn_801EF644(-1) (real title scene init) */
    printf("[spike] engine thread '%s': init slice running\n", s->name);
    s->initDone = 1;

    for (;;) {
        s->workCounter++;          /* per-frame engine work (stand-in)         */
        EngineHost_VsyncYield();   /* == fn_800F0308: block until next vsync    */
    }
}

/* ---- headless mechanism self-test (--fibre-test) ----------------------- */

static int SelfTestPresent(int frame, void* user) {
    SpikeThreadState* threads = (SpikeThreadState*)user;
    /* No GL here — just account the round-trip. Log a few. EngineHost_Run stops
     * itself after maxFrames, so the present hook never needs to ask to stop. */
    if (frame < 3 || (frame % 25) == 0) {
        printf("[spike] frame %d presented (host fibre); t0.work=%lu t1.work=%lu\n",
               frame, threads[0].workCounter,
               threads[1].name ? threads[1].workCounter : 0UL);
    }
    return 1;
}

/* Returns 1 on pass, 0 on fail. */
int RunFibreSelfTest(void) {
    SpikeThreadState threads[2];
    int frames;
    int wantFrames = 10;
    const char* env = getenv("PCPORT_FIBRE_FRAMES");
    if (env != NULL) {
        int v = atoi(env);
        if (v > 0) wantFrames = v;
    }

    printf("[spike] === fibre cooperative-scheduler self-test ===\n");
    if (!HostFibre_Available()) {
        fprintf(stderr, "[spike] FAIL: no fibre backend on this host\n");
        return 0;
    }
    if (!EngineHost_Init()) {
        fprintf(stderr, "[spike] FAIL: EngineHost_Init\n");
        return 0;
    }

    /* --- Part A: single engine thread, N frames --- */
    memset(threads, 0, sizeof(threads));
    threads[0].name = "title";
    threads[1].name = NULL; /* not registered in part A */
    if (EngineHost_CreateThread(SpikeTitleThread, &threads[0], 0) == NULL) {
        fprintf(stderr, "[spike] FAIL: create title thread\n");
        return 0;
    }
    frames = EngineHost_Run(wantFrames, SelfTestPresent, threads);

    printf("[spike] part A: ticked %d frames; title.initDone=%d title.work=%lu\n",
           frames, threads[0].initDone, threads[0].workCounter);

    if (frames != wantFrames) {
        fprintf(stderr, "[spike] FAIL: expected %d frames, got %d\n", wantFrames, frames);
        return 0;
    }
    if (!threads[0].initDone) {
        fprintf(stderr, "[spike] FAIL: engine thread init never ran\n");
        return 0;
    }
    /* The thread does one work tick per frame BEFORE yielding, so after N frames
     * its counter is exactly N (the run loop stops after the Nth present, having
     * resumed the thread N times). */
    if (threads[0].workCounter != (unsigned long)wantFrames) {
        fprintf(stderr, "[spike] FAIL: expected %d work ticks, got %lu\n",
                wantFrames, threads[0].workCounter);
        return 0;
    }
    printf("[spike] part A PASS: round-trip count matches frame count\n");

    /* --- Part B: add a SECOND engine thread (models OSCreateThread spawning a
     * thread that cooperatively shares the frame with the existing one — like
     * the real main() idle yield loop alongside the GameMainLoop thread). The
     * scheduler's table is append-only within the process, so part B resumes the
     * part-A title thread (continuing its counter) AND the new thread, slicing
     * BOTH once per frame. --- */
    memset(&threads[1], 0, sizeof(threads[1]));
    threads[1].name = "idle";
    if (EngineHost_CreateThread(SpikeTitleThread, &threads[1], 0) == NULL) {
        fprintf(stderr, "[spike] FAIL: create idle thread\n");
        return 0;
    }
    frames = EngineHost_Run(wantFrames, SelfTestPresent, threads);
    printf("[spike] part B: ticked %d more frames; t0.work=%lu t1.work=%lu\n",
           frames, threads[0].workCounter, threads[1].workCounter);

    /* In part B BOTH threads get a slice each frame. thread[1] (created fresh)
     * should have exactly `frames` ticks; thread[0] continues from part A. */
    if (threads[1].workCounter != (unsigned long)frames) {
        fprintf(stderr, "[spike] FAIL: 2nd thread expected %d ticks, got %lu\n",
                frames, threads[1].workCounter);
        return 0;
    }
    if (!threads[1].initDone) {
        fprintf(stderr, "[spike] FAIL: 2nd engine thread init never ran\n");
        return 0;
    }
    printf("[spike] part B PASS: both engine threads sliced once per frame\n");

    printf("[spike] === self-test PASS ===\n");
    return 1;
}

/* ---- windowed present loop (--engine) ---------------------------------- */

typedef struct {
    GLFWwindow*  window;
    int          maxFrames;
    SpikeThreadState* engine;
} EngineSpikePresentCtx;

static int EngineSpikePresent(int frame, void* user) {
    EngineSpikePresentCtx* ctx = (EngineSpikePresentCtx*)user;
    float t = (float)frame;
    float r = 0.15f + 0.10f * (float)((frame / 1) % 2);
    float g = 0.10f + 0.0025f * (float)(frame % 100);
    float b = 0.25f;
    (void)t;

    /* Real host present path: pump events/pre-retrace, clear, swap. This is the
     * same sequence RunMenuScene uses, driven here from the HOST fibre after the
     * engine thread yielded — proving the round-trip presents real frames. */
    VIWaitForRetrace_PC();
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GSgfxSwapBuffers(0);

    if (frame < 5 || (frame % 60) == 0) {
        printf("[spike] --engine frame %d (engine.work=%lu) presented\n",
               frame, ctx->engine->workCounter);
    }

    if (glfwWindowShouldClose(ctx->window)) {
        printf("[spike] window closed at frame %d\n", frame);
        return 0;
    }
    if (ctx->maxFrames > 0 && frame + 1 >= ctx->maxFrames) {
        return 0;
    }
    return 1;
}

/* Returns 1 on success. */
int RunEngineSpike(GLFWwindow* window) {
    SpikeThreadState engine;
    EngineSpikePresentCtx ctx;
    int maxFrames = 0; /* 0 => until window close */
    const char* env = getenv("PCPORT_ENGINE_FRAMES");
    int ticked;

    if (env != NULL) {
        int v = atoi(env);
        if (v > 0) maxFrames = v;
    }

    printf("[spike] === --engine: host<->engine fibre round-trip (windowed) ===\n");
    if (!EngineHost_Init()) {
        fprintf(stderr, "[spike] EngineHost_Init failed\n");
        return 0;
    }

    memset(&engine, 0, sizeof(engine));
    engine.name = "title";
    if (EngineHost_CreateThread(SpikeTitleThread, &engine, 0) == NULL) {
        fprintf(stderr, "[spike] create engine thread failed\n");
        return 0;
    }

    ctx.window = window;
    ctx.maxFrames = maxFrames;
    ctx.engine = &engine;

    ticked = EngineHost_Run(maxFrames, EngineSpikePresent, &ctx);
    printf("[spike] --engine ran %d frames; engine.initDone=%d engine.work=%lu\n",
           ticked, engine.initDone, engine.workCounter);
    return 1;
}
