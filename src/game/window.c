/**
 * @file window.c
 * @brief windowAllocMemory (0x80103FFC - 0x801040A0) of the window TU.
 *
 * The window TU is built with -opt nopeephole (configure.py; see
 * window_exact_80104318.c), so no local pragma is needed.
 */
#include "dolphin/types.h"

typedef struct WindowWork {
    /* 0x00 */ u8 _00[0xAC];
    /* 0xAC */ u16 memoryHandle;
    /* 0xB0 */ void* memory;
} WindowWork;

extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void* fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);

/* 0x80103FFC | 0xA4: free the window's memory block, then allocate size
 * bytes (none when size <= 0). */
void* windowAllocMemory(WindowWork* window, s32 size)
{
    if (window == NULL) {
        return NULL;
    }
    if (window->memoryHandle != 0) {
        fn_800E24B0(window->memoryHandle);
        GSmemFree(window->memoryHandle);
        window->memory = NULL;
    }
    if (size <= 0) {
        return NULL;
    }
    window->memoryHandle = _toolentryAlloc__FUl(size);
    if (window->memoryHandle != 0) {
        window->memory = GSmemLock(window->memoryHandle);
    } else {
        return NULL;
    }
    return window->memory;
}
