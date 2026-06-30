/**
 * @file os_shim.c
 * @brief OS function replacements -- stub implementations.
 *
 * References:
 *   - os_shim.h for full API documentation
 *   - include/dolphin/os/*.h for original SDK interfaces
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "os_shim.h"
#include "pcport_window.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* TODO: Include platform headers when build system is ready
 * #ifdef _WIN32
 * #include <windows.h>
 * #else
 * #include <sys/time.h>
 * #endif
 */

/* =========================================================================
 * Internal state
 * ========================================================================= */

/** Static memory arena (emulates GCN main RAM for GSmem) */
static u8* g_arenaBuffer = NULL;
static void* g_arenaLo = NULL;
static void* g_arenaHi = NULL;

/** High-resolution timer baseline */
static s64 g_timerBaseline = 0;

/** Scheduler disable counter */
static s32 g_schedulerDisableCount = 0;

/** Interrupt state */
static u32 g_interruptsEnabled = 1;

/** VI black screen flag */
static BOOL g_viBlack = 0;

/** Active host window provided by the bootstrap */
static GLFWwindow* g_hostWindow = NULL;

/** Last configured VI dimensions */
static int g_viWidth = 640;
static int g_viHeight = 480;

/** Initialized flag */
static int g_osInitialized = 0;

/* Display descriptor backing storage is provided by the gs_gfx host support
 * layer when that path is linked into the bootstrap.
 */
extern u8 lbl_80466BC0[];

static void UpdateDisplayDescriptor(int width, int height) {
    *(u16*)(lbl_80466BC0 + 4) = (u16)width;
    *(u16*)(lbl_80466BC0 + 6) = (u16)height;
}

/* =========================================================================
 * Initialization / Shutdown
 * ========================================================================= */

void OSInit_PC(void) {
    /* Allocate the memory arena */
    g_arenaBuffer = (u8*)malloc(OS_ARENA_SIZE);
    if (!g_arenaBuffer) {
        printf("[os_shim] FATAL: Failed to allocate %d byte arena\n",
               OS_ARENA_SIZE);
        abort();
    }
    memset(g_arenaBuffer, 0, OS_ARENA_SIZE);
    g_arenaLo = g_arenaBuffer;
    g_arenaHi = g_arenaBuffer + OS_ARENA_SIZE;

    /* TODO: Phase 3a -- Initialize high-resolution timer
     *
     * #ifdef _WIN32
     *   LARGE_INTEGER freq, counter;
     *   QueryPerformanceFrequency(&freq);
     *   QueryPerformanceCounter(&counter);
     *   g_timerBaseline = counter.QuadPart;
     *   // Store freq for later conversion
     * #else
     *   struct timespec ts;
     *   clock_gettime(CLOCK_MONOTONIC, &ts);
     *   g_timerBaseline = (s64)ts.tv_sec * 1000000000LL + ts.tv_nsec;
     * #endif
     */
    g_timerBaseline = (s64)clock();

    g_schedulerDisableCount = 0;
    g_interruptsEnabled = 1;
    g_viBlack = 0;
    g_viWidth = 640;
    g_viHeight = 480;
    g_osInitialized = 1;

    UpdateDisplayDescriptor(g_viWidth, g_viHeight);

    printf("[os_shim] OSInit_PC: Arena %p - %p (%d bytes)\n",
           g_arenaLo, g_arenaHi, OS_ARENA_SIZE);
}

void OSShutdown_PC(void) {
    if (g_arenaBuffer) {
        free(g_arenaBuffer);
        g_arenaBuffer = NULL;
    }
    g_arenaLo = NULL;
    g_arenaHi = NULL;
    g_osInitialized = 0;
}

void PCPort_SetHostWindow(GLFWwindow* window) {
    g_hostWindow = window;
}

GLFWwindow* PCPort_GetHostWindow(void) {
    return g_hostWindow;
}

int PCPort_GetVideoWidth(void) {
    return g_viWidth;
}

int PCPort_GetVideoHeight(void) {
    return g_viHeight;
}

int PCPort_IsVideoBlack(void) {
    return g_viBlack != 0;
}

/* =========================================================================
 * Memory Management
 * ========================================================================= */

void* OSGetArenaHi(void) {
    return g_arenaHi;
}

void* OSGetArenaLo(void) {
    return g_arenaLo;
}

void OSSetArenaHi(void* addr) {
    /* TODO: Phase 3 -- Bounds checking
     *
     * Ensure addr is within the arena:
     *   assert(addr >= g_arenaBuffer && addr <= g_arenaBuffer + OS_ARENA_SIZE);
     */
    g_arenaHi = addr;
}

void OSSetArenaLo(void* addr) {
    /* TODO: Phase 3 -- Bounds checking */
    g_arenaLo = addr;
}

void* OSAlloc_PC(u32 size) {
    /* TODO: Phase 3 -- Consider alignment requirements
     *
     * GCN code often expects 32-byte alignment for DMA transfers.
     * Use _aligned_malloc (MSVC) or posix_memalign (POSIX) if needed.
     *
     * For now, standard malloc with 16-byte alignment is usually sufficient
     * on modern platforms.
     */
    return malloc(size);
}

void OSFree_PC(void* ptr) {
    free(ptr);
}

/* =========================================================================
 * Time Functions
 * ========================================================================= */

OSTime OSGetTime(void) {
    /* TODO: Phase 3a -- High-resolution timer
     *
     * #ifdef _WIN32
     *   LARGE_INTEGER counter, freq;
     *   QueryPerformanceCounter(&counter);
     *   QueryPerformanceFrequency(&freq);
     *   // Convert to GCN ticks (OS_TIMER_CLOCK = 40,500,000 Hz)
     *   s64 elapsed = counter.QuadPart - g_timerBaseline;
     *   return (OSTime)(elapsed * OS_TIMER_CLOCK / freq.QuadPart);
     * #else
     *   struct timespec ts;
     *   clock_gettime(CLOCK_MONOTONIC, &ts);
     *   s64 now = (s64)ts.tv_sec * 1000000000LL + ts.tv_nsec;
     *   s64 elapsed = now - g_timerBaseline;
     *   // Convert nanoseconds to GCN ticks
     *   return (OSTime)(elapsed * OS_TIMER_CLOCK / 1000000000LL);
     * #endif
     */

    /* Stub: use clock() scaled to approximate GCN tick rate */
    s64 now = (s64)clock();
    s64 elapsed = now - g_timerBaseline;
    return (OSTime)(elapsed * OS_TIMER_CLOCK / CLOCKS_PER_SEC);
}

u32 OSGetTick(void) {
    return (u32)(OSGetTime() & 0xFFFFFFFF);
}

u32 OSTicksToMilliseconds(OSTime ticks) {
    return (u32)(ticks / OS_TICKS_PER_MSEC);
}

OSTime OSMillisecondsToTicks(u32 msec) {
    return (OSTime)msec * OS_TICKS_PER_MSEC;
}

/* =========================================================================
 * Cache Functions (all no-ops on PC)
 * ========================================================================= */

void DCFlushRange(void* addr, u32 nBytes) {
    (void)addr; (void)nBytes;
    /* No-op: x86 caches are coherent */
}

void DCInvalidateRange(void* addr, u32 nBytes) {
    (void)addr; (void)nBytes;
}

void DCFlushRangeNoSync(void* addr, u32 nBytes) {
    (void)addr; (void)nBytes;
}

void ICInvalidateRange(void* addr, u32 nBytes) {
    (void)addr; (void)nBytes;
}

void ICFlashInvalidate(void) {
    /* No-op */
}

/* =========================================================================
 * Debug / Report Functions
 * ========================================================================= */

void OSReport(const char* fmt, ...) {
    va_list args;
    printf("[OSReport] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void OSPanic(const char* file, s32 line, const char* fmt, ...) {
    va_list args;
    fprintf(stderr, "\n=== OS PANIC ===\n");
    fprintf(stderr, "File: %s, Line: %d\n", file ? file : "(unknown)", line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n================\n");
    abort();
}

/* =========================================================================
 * Thread Functions
 * ========================================================================= */

u32 OSDisableInterrupts_PC(void) {
    u32 prev = g_interruptsEnabled;
    g_interruptsEnabled = 0;

    /* TODO: Phase 3 -- Enter critical section
     *
     * #ifdef _WIN32
     *   EnterCriticalSection(&g_criticalSection);
     * #else
     *   pthread_mutex_lock(&g_mutex);
     * #endif
     */

    return prev;
}

BOOL OSDisableInterrupts(void) {
    return (BOOL)OSDisableInterrupts_PC();
}

BOOL OSEnableInterrupts(void) {
    BOOL prev = (BOOL)g_interruptsEnabled;
    g_interruptsEnabled = 1;
    return prev;
}

void OSRestoreInterrupts_PC(u32 prev) {
    g_interruptsEnabled = prev;

    /* TODO: Phase 3 -- Leave critical section
     *
     * if (prev) {
     *     #ifdef _WIN32
     *       LeaveCriticalSection(&g_criticalSection);
     *     #else
     *       pthread_mutex_unlock(&g_mutex);
     *     #endif
     * }
     */
}

BOOL OSRestoreInterrupts(BOOL level) {
    BOOL prev = (BOOL)g_interruptsEnabled;
    OSRestoreInterrupts_PC((u32)level);
    return prev;
}

s32 OSDisableScheduler_PC(void) {
    g_schedulerDisableCount++;
    return g_schedulerDisableCount;
}

s32 OSEnableScheduler_PC(void) {
    if (g_schedulerDisableCount > 0)
        g_schedulerDisableCount--;
    return g_schedulerDisableCount;
}

/* =========================================================================
 * VI (Video Interface) Functions
 * ========================================================================= */

void VIConfigure_PC(void* mode) {
    int width = 640;
    int height = 480;

    if (mode != NULL) {
        u8* modeBytes = (u8*)mode;
        u16 modeWidth = *(u16*)(modeBytes + 4);
        u16 modeHeight = *(u16*)(modeBytes + 6);

        if (modeWidth != 0) {
            width = modeWidth;
        }
        if (modeHeight != 0) {
            height = modeHeight;
        }
    }

    g_viWidth = width;
    g_viHeight = height;
    UpdateDisplayDescriptor(width, height);
}

void VISetNextFrameBuffer_PC(void* fb) {
    (void)fb;
    /* No-op on PC -- framebuffer is managed by OpenGL/GLFW */
}

void VIWaitForRetrace_PC(void) {
    PCPort_RunPreRetraceCallback();

    if (g_hostWindow != NULL) {
        glfwPollEvents();
    }
}

void VIFlush_PC(void) {
    if (g_hostWindow != NULL) {
        glfwSetWindowSize(g_hostWindow, g_viWidth, g_viHeight);
    }
}

void VISetBlack_PC(BOOL black) {
    g_viBlack = black;
}


#endif /* __MWERKS__ */
