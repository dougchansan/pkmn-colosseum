/**
 * @file os_shim.h
 * @brief OS function replacements for the PC port.
 *
 * Replaces Dolphin SDK OS functions with standard C library / platform
 * equivalents:
 *   - OSAlloc / OSFree -> malloc / free
 *   - OSGetTime / OSGetTick -> clock / QueryPerformanceCounter
 *   - DCFlushRange / DCInvalidateRange -> no-op (PC cache is coherent)
 *   - OSReport -> printf
 *   - OSThread -> pthreads or C11 threads
 *   - OSInit -> PC initialization sequence
 *   - OSGetArenaHi/Lo -> static memory pool
 *
 * Also provides VI (Video Interface) replacements:
 *   - VIConfigure -> GLFW window setup
 *   - VISetNextFrameBuffer -> no-op (handled by gx_shim GXCopyDisp)
 *   - VIWaitForRetrace -> glfwSwapInterval + main loop sync
 *
 * References:
 *   - include/dolphin/os/OS.h
 *   - include/dolphin/os/OSTime.h
 *   - include/dolphin/os/OSCache.h
 *   - include/dolphin/os/OSThread.h
 *   - include/dolphin/os/OSMemory.h
 *   - include/dolphin/vi/VI.h
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_OS_SHIM_H
#define PCPORT_OS_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard types */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long    u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed long      s32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float          f32;
typedef double         f64;
typedef int            BOOL;

/* GCN type aliases used by the SDK */
typedef s64 OSTime;
typedef s32 OSHeapHandle;

/* =========================================================================
 * Constants
 * ========================================================================= */

/** GCN bus clock frequency (used to convert OSTime to real time) */
#define OS_BUS_CLOCK         162000000
/** GCN timer frequency (bus clock / 4) */
#define OS_TIMER_CLOCK       (OS_BUS_CLOCK / 4)
/** Ticks per second */
#define OS_TICKS_PER_SEC     OS_TIMER_CLOCK
/** Ticks per millisecond */
#define OS_TICKS_PER_MSEC    (OS_TIMER_CLOCK / 1000)

/** Size of the emulated OS arena (24 MB, matching GCN main RAM) */
#define OS_ARENA_SIZE        (24 * 1024 * 1024)

/* =========================================================================
 * OS Initialization
 * ========================================================================= */

/**
 * OSInit_PC -- Initialize the OS shim.
 *
 * Sets up:
 *   - Static memory arena (OS_ARENA_SIZE bytes)
 *   - High-resolution timer baseline
 *   - Console output
 *
 * Must be called before any other OS functions.
 */
void OSInit_PC(void);

/**
 * OSShutdown_PC -- Clean up the OS shim.
 *
 * Frees the memory arena and releases resources.
 */
void OSShutdown_PC(void);

/* =========================================================================
 * Memory Management
 *
 * The game uses OSGetArenaHi/Lo to define the memory region for GSmem.
 * On PC, we allocate a static buffer and expose it through these APIs.
 * ========================================================================= */

/**
 * OSGetArenaHi -- Get the top of the available memory arena.
 *
 * @return  Pointer to the top of the arena.
 *
 * On GCN: Returns the high-water mark of the OS arena.
 * On PC: Returns (arenaBase + OS_ARENA_SIZE).
 */
void* OSGetArenaHi(void);

/**
 * OSGetArenaLo -- Get the bottom of the available memory arena.
 *
 * @return  Pointer to the bottom of the arena.
 *
 * On GCN: Returns the low-water mark of the OS arena.
 * On PC: Returns the base of the static arena buffer.
 */
void* OSGetArenaLo(void);

/**
 * OSSetArenaHi -- Set the top of the memory arena.
 *
 * @param addr  New arena high address.
 *
 * On PC: Update the high-water mark (bounds checking only).
 */
void OSSetArenaHi(void* addr);

/**
 * OSSetArenaLo -- Set the bottom of the memory arena.
 *
 * @param addr  New arena low address.
 *
 * On PC: Update the low-water mark.
 */
void OSSetArenaLo(void* addr);

/**
 * OSAlloc_PC -- Allocate memory from the OS heap.
 *
 * @param size  Number of bytes to allocate.
 * @return      Pointer to allocated memory, or NULL on failure.
 *
 * Thin wrapper around malloc().
 */
void* OSAlloc_PC(u32 size);

/**
 * OSFree_PC -- Free memory allocated by OSAlloc_PC.
 *
 * @param ptr  Pointer to free.
 *
 * Thin wrapper around free().
 */
void OSFree_PC(void* ptr);

/* =========================================================================
 * Time Functions
 *
 * On GCN, OSGetTime returns the time base register value (TB),
 * which increments at OS_TIMER_CLOCK (40.5 MHz).
 * On PC, we use high-resolution timers scaled to match.
 * ========================================================================= */

/**
 * OSGetTime -- Get the current time in GCN time base ticks.
 *
 * @return  Current time in ticks (at OS_TIMER_CLOCK frequency).
 *
 * On PC: Uses QueryPerformanceCounter (Windows) or clock_gettime (POSIX),
 * scaled to match GCN tick frequency for game code compatibility.
 */
OSTime OSGetTime(void);

/**
 * OSGetTick -- Get the lower 32 bits of the time base.
 *
 * @return  Current tick count (lower 32 bits).
 */
u32 OSGetTick(void);

/**
 * OSTicksToMilliseconds -- Convert GCN ticks to milliseconds.
 *
 * @param ticks  Number of GCN ticks.
 * @return       Equivalent milliseconds.
 */
u32 OSTicksToMilliseconds(OSTime ticks);

/**
 * OSMillisecondsToTicks -- Convert milliseconds to GCN ticks.
 *
 * @param msec  Milliseconds.
 * @return      Equivalent GCN ticks.
 */
OSTime OSMillisecondsToTicks(u32 msec);

/* =========================================================================
 * Cache Functions
 *
 * No-ops on PC -- x86 caches are coherent and do not require
 * explicit management.
 * ========================================================================= */

/** DCFlushRange -- Flush data cache. No-op on PC. */
void DCFlushRange(void* addr, u32 nBytes);

/** DCInvalidateRange -- Invalidate data cache. No-op on PC. */
void DCInvalidateRange(void* addr, u32 nBytes);

/** DCFlushRangeNoSync -- Flush without sync. No-op on PC. */
void DCFlushRangeNoSync(void* addr, u32 nBytes);

/** ICInvalidateRange -- Invalidate instruction cache. No-op on PC. */
void ICInvalidateRange(void* addr, u32 nBytes);

/** ICFlashInvalidate -- Flash-invalidate instruction cache. No-op on PC. */
void ICFlashInvalidate(void);

/* =========================================================================
 * Debug / Report Functions
 * ========================================================================= */

/**
 * OSReport -- Print a debug message.
 *
 * @param fmt  printf-style format string.
 * @param ...  Format arguments.
 *
 * On PC: Maps to printf() with a "[OSReport]" prefix.
 */
void OSReport(const char* fmt, ...);

/**
 * OSPanic -- Fatal error with file/line info.
 *
 * @param file  Source file name.
 * @param line  Source line number.
 * @param fmt   Error message format string.
 * @param ...   Format arguments.
 *
 * On PC: Print the error, call abort().
 */
void OSPanic(const char* file, s32 line, const char* fmt, ...);

/* =========================================================================
 * Thread Functions
 *
 * The game uses threads for:
 *   - DVD async I/O
 *   - Sound streaming
 *   - Script interpreter
 *
 * On PC, we can use platform threads (pthreads / Win32) or C11 threads.
 * For the initial port, these can be simplified or single-threaded.
 * ========================================================================= */

/**
 * OSDisableInterrupts_PC -- Disable "interrupts" (enter critical section).
 *
 * @return  Previous interrupt state (for restore).
 *
 * On PC: Enter a mutex or use a flag to prevent context switches.
 */
u32 OSDisableInterrupts_PC(void);

/**
 * OSRestoreInterrupts_PC -- Restore interrupt state.
 *
 * @param prev  Previous state from OSDisableInterrupts_PC.
 */
void OSRestoreInterrupts_PC(u32 prev);

/* Thin SDK-name wrappers used by decompiled host-compiled translation units. */
BOOL OSDisableInterrupts(void);
BOOL OSEnableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

/**
 * OSDisableScheduler_PC -- Disable the thread scheduler.
 *
 * @return  Previous scheduler state.
 *
 * On PC: Increment a scheduler-disable counter.
 */
s32 OSDisableScheduler_PC(void);

/**
 * OSEnableScheduler_PC -- Enable the thread scheduler.
 *
 * @return  New scheduler state.
 */
s32 OSEnableScheduler_PC(void);

/* =========================================================================
 * VI (Video Interface) Functions
 *
 * The game uses VI functions for video mode setup. On PC, these
 * translate to GLFW window configuration.
 * ========================================================================= */

/**
 * VIConfigure_PC -- Configure the video output mode.
 *
 * @param mode  Pointer to a video mode structure.
 *
 * On PC: Set GLFW window size based on the requested resolution.
 * Default: 640x480 for NTSC, 640x576 for PAL.
 */
void VIConfigure_PC(void* mode);

/**
 * VISetNextFrameBuffer_PC -- Set the next framebuffer for display.
 *
 * @param fb  Pointer to the framebuffer.
 *
 * On PC: No-op -- framebuffer management is handled by GLFW/OpenGL.
 */
void VISetNextFrameBuffer_PC(void* fb);

/**
 * VIWaitForRetrace_PC -- Wait for vertical retrace (VSync).
 *
 * On PC: If VSync is enabled via glfwSwapInterval(1), this just
 * returns. The actual VSync happens in glfwSwapBuffers (GXCopyDisp).
 */
void VIWaitForRetrace_PC(void);

/**
 * VIFlush_PC -- Flush VI settings to hardware.
 *
 * On PC: No-op.
 */
void VIFlush_PC(void);

/**
 * VISetBlack_PC -- Set the screen to black (during transitions).
 *
 * @param black  TRUE to black out, FALSE to show normal output.
 *
 * On PC: Set a flag that causes the main loop to skip rendering
 * (or clear to black).
 */
void VISetBlack_PC(BOOL black);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_OS_SHIM_H */
