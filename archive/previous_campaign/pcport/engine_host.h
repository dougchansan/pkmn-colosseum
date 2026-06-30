/* engine_host.h — host reimplementation of the Pokémon Colosseum engine's
 * cooperative scheduler, on top of the generic Win32-fibre layer (os_thread_host).
 *
 * WHAT THIS REPLACES (read-only references — do not edit those files):
 *   - src/game/gs_texture.c fn_800F0308  : the per-frame vsync-yield the whole
 *     engine blocks on (asm context switch: save LR/SP/GPR/FPR into a GSThreadCtx
 *     obtained from fn_800F015C, then load the scheduler-selected next ctx from
 *     fn_800F0030 and return into it). Its host equivalent is EngineHost_VsyncYield.
 *   - src/dolphin/os/OSThread.c OSCreateThread / __OSThreadSwitch / OSLoadContext /
 *     OSSaveContext : the low-level thread create + context switch. OSCreateThread
 *     maps to EngineHost_CreateThread (CreateFiber); __OSThreadSwitch maps to a
 *     HostFibre_SwitchTo.
 *
 * KEY INSIGHT: on PowerPC the engine had to hand-save the register file because a
 * coroutine switch is "set SP/LR/GPRs to the other stack." On x86 with native
 * fibres, SwitchToFiber preserves the entire CPU + stack state automatically, so
 * we only reimplement the SCHEDULING semantics (who runs next), not the register
 * marshalling. The GSThreadCtx GPR/FPR fields are irrelevant on host; each engine
 * thread-context simply maps 1:1 to a host fibre.
 *
 * SPIKE MODEL (single OS thread, cooperative):
 *   [host/present fibre]  <-->  [engine thread fibre(s)]
 *   The host fibre is the scheduler/present driver. Each engine thread runs its
 *   per-frame slice then calls EngineHost_VsyncYield() to hand control back. Once
 *   every registered engine thread has yielded, the host presents one frame and
 *   resumes them for the next. This is the faithful shape of the real engine's
 *   `for(;;) fn_800F0308()` thread loops (e.g. gs_title.c fn_8002058C) driven by
 *   the GSgfx swap callback at vsync.
 */
#ifndef PCPORT_ENGINE_HOST_H
#define PCPORT_ENGINE_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EngineThread EngineThread;

/* An engine thread body. Mirrors a GameCube thread entry (e.g. fn_8002058C):
 * typically `do_init(); for(;;) EngineHost_VsyncYield();`. */
typedef void (*EngineThreadFunc)(void* arg);

/* Per-frame present hook, called on the HOST fibre once per ticked frame, after
 * all engine threads have yielded. `frame` is the 0-based frame index. Return 0
 * to request the run loop stop (e.g. window closed). */
typedef int (*EngineFramePresent)(int frame, void* user);

/* Initialise the engine-host scheduler. Converts the current OS thread into the
 * host/present fibre. Returns 1 on success, 0 if the fibre backend is missing. */
int EngineHost_Init(void);

/* Create an engine thread (a cooperative fibre). It is created suspended and
 * begins executing `func(arg)` the first time the scheduler resumes it.
 * stackSize 0 => default. Returns NULL on failure. */
EngineThread* EngineHost_CreateThread(EngineThreadFunc func, void* arg,
                                      unsigned int stackSize);

/* Host equivalent of fn_800F0308 (the vsync-yield). Called from WITHIN an engine
 * thread: suspends it and returns control to the host/scheduler fibre. The call
 * returns (the thread resumes) on the next frame. */
void EngineHost_VsyncYield(void);

/* Run the cooperative loop for up to maxFrames (<=0 => until present asks to
 * stop). Each frame: resume every created engine thread in registration order
 * until each yields, then call `present(frame, user)` on the host fibre. Returns
 * the number of frames actually ticked. */
int EngineHost_Run(int maxFrames, EngineFramePresent present, void* user);

/* The frame index currently being processed (incremented after each present). */
int EngineHost_FrameCount(void);

/* True once EngineHost_Init has run and the fibre backend is live. */
int EngineHost_Ready(void);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_ENGINE_HOST_H */
