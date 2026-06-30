/* os_thread_host.h — host cooperative-fibre primitive layer for the PC port.
 *
 * The GameCube build switches the engine's cooperative threads with PowerPC
 * assembly that saves/restores the full GPR/FPR/SP/LR register file into a
 * per-thread context block (see src/game/gs_texture.c fn_800F0308 and friends,
 * and src/dolphin/os/OSThread.c OSLoadContext/OSSaveContext). That asm cannot
 * run on x86. To host the engine's cooperative scheduler natively we
 * reimplement the context-switch SEMANTICS — "suspend this coroutine, resume
 * that one, preserving its full call stack" — on top of native host fibres.
 *
 * This header is the generic, engine-agnostic primitive. On Windows it is
 * backed by Win32 Fibers (ConvertThreadToFiber / CreateFiber / SwitchToFiber),
 * which are exactly cooperative coroutines: each fibre owns its own stack and
 * register state, and a switch is non-preemptive and explicit. The engine's
 * `fn_800F0308` (per-frame vsync-yield) and OSThread create/switch map directly
 * onto these calls (see engine_host.c).
 *
 * Fallback (not needed on Windows, documented for portability): the same API
 * can be implemented with one OS thread per fibre plus a condition-variable
 * baton so that exactly one fibre runs at a time — a "cooperative threads"
 * emulation. ucontext (POSIX) is the natural Linux/macOS backend.
 *
 * Host-only file: it is never part of the CodeWarrior matching build (only the
 * pcport_link.py BOOT list compiles it), so there is no __MWERKS__ path.
 */
#ifndef PCPORT_OS_THREAD_HOST_H
#define PCPORT_OS_THREAD_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HostFibre HostFibre;

/* Entry point for a fibre. Receives the `arg` passed to HostFibre_Create.
 * A fibre entry should normally never return (the engine threads loop on the
 * vsync-yield forever); if it does, HostFibre_Finish() is invoked and control
 * returns to the host/main fibre. */
typedef void (*HostFibreEntry)(void* arg);

/* Convert the current OS thread into the host/main fibre. Call once, before any
 * HostFibre_Create. Returns the host fibre handle (also retrievable via
 * HostFibre_Main). Idempotent — repeated calls return the same handle. */
HostFibre* HostFibre_InitMain(void);

/* The host/main fibre (the one HostFibre_InitMain converted), or NULL if
 * HostFibre_InitMain has not been called. */
HostFibre* HostFibre_Main(void);

/* Create a cooperative fibre. It is created suspended: `entry` does not begin
 * executing until the first HostFibre_SwitchTo(thisFibre). stackCommit is a
 * hint in bytes (0 => a sensible default). */
HostFibre* HostFibre_Create(HostFibreEntry entry, void* arg, unsigned int stackCommit);

/* Cooperatively transfer execution to `target`. The calling fibre is suspended
 * with its full stack/register state intact; this call returns only when some
 * other fibre later switches back to the caller. Switching to the currently
 * running fibre is a no-op. */
void HostFibre_SwitchTo(HostFibre* target);

/* The fibre currently executing on this thread (NULL before HostFibre_InitMain). */
HostFibre* HostFibre_Current(void);

/* Destroy a fibre created by HostFibre_Create. Must not be the running fibre.
 * Does not free `arg`. */
void HostFibre_Destroy(HostFibre* f);

/* 1 if a real fibre backend is available (Win32 Fibers present), else 0. */
int HostFibre_Available(void);

/* Number of times this fibre has been resumed (switched INTO). Useful for the
 * spike's round-trip accounting. Returns 0 for NULL. */
unsigned long HostFibre_ResumeCount(HostFibre* f);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_OS_THREAD_HOST_H */
