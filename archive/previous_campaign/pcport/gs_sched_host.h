/* gs_sched_host.h — host reimplementation of the engine's GSthread cooperative
 * scheduler (the GStask + GSthread layers from src/game/gs_thread.c).
 *
 * P-B (engine-hosting). The real gs_thread.c cannot be host-linked — it is a
 * 153 KB grab-bag TU with 86 still-asm functions, and its C-active scheduler
 * functions sit on the GSmem allocator + the asm context primitives (see
 * docs/PC_PORT_ENGINE_FIBRE_SPIKE.md §4). So we host-REIMPLEMENT its small,
 * well-understood scheduler surface here, semantically faithful to the original:
 *
 *   - the TASK layer (GStaskInit/GStaskCreate/GStaskRun) is pure C linked-list
 *     logic in the original — reproduced verbatim, minus the GSmem handles
 *     (static pools instead). This is the engine's dominant per-frame mechanism:
 *     each frame GStaskRun walks a priority-sorted list and calls every active
 *     task's func(taskId, param).
 *   - the THREAD layer (GSthreadInit/GSthreadCreate) maps each GSthread onto a
 *     host fibre (engine_host / os_thread_host); GSthreadYield is the host
 *     equivalent of the asm vsync-yield fn_800F0308.
 *
 * It uses the REAL structs/enums from <game/gs_thread.h> (GSTask 0x18, GSThread
 * 0x24, GSTASK_FREE/ACTIVE/DEFERRED) so it is a drop-in for engine callers, and
 * exports fn_ aliases (fn_800FE9B0/834/7A0, fn_800F09D8/07A8) so that when real
 * engine TUs are linked they bind to this host scheduler instead of the
 * auto-generated stubs.
 *
 * Host-only file (never in the CodeWarrior matching build).
 */
#ifndef PCPORT_GS_SCHED_HOST_H
#define PCPORT_GS_SCHED_HOST_H

#include "game/gs_thread.h"   /* GSTask, GSThread, GSTASK_*, the real API protos */

#ifdef __cplusplus
extern "C" {
#endif

/* Reset all host scheduler state (task + thread pools, list heads, frame
 * counter). Call before GStaskInit/GSthreadInit to start a clean boot. */
void GSched_HostReset(void);

/* Host equivalent of the asm vsync-yield fn_800F0308: called from inside a
 * GSthread body to suspend it until the next frame (switches the fibre back to
 * the host/scheduler). */
void GSthreadYield(void);

/* Run one per-frame slice of every active GSthread (resume each fibre until it
 * yields). Returns the number of threads still live. Mirrors how the engine
 * steps its thread list each frame. */
int GSthreadStepAll(void);

/* Introspection for tests/diagnostics. */
int GSched_TaskCount(void);          /* number of non-free task slots in use */
int GSched_ThreadCount(void);        /* number of active GSthreads           */
unsigned int GSched_FrameCount(void);/* GStaskRun invocations so far         */

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_GS_SCHED_HOST_H */
