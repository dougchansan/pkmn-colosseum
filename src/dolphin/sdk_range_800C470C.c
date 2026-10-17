/**
 * @file sdk_range_800C470C.c
 * @brief __sys_free (GCN_mem_alloc.c), 0x800C4D8C - 0x800C4E44, built as
 *        dolphin/sdk_r52_800C4D8C_suffix.c.
 */
#include "dolphin/types.h"

/* The runtime.c routines that used to sit here (__save_fpr .. __cvt_dbl_usll,
 * 0x800C470C - 0x800C4D8C) are the linked crt/runtime.c. */

/* GCN_mem_alloc.c (Metrowerks runtime), as in tww / pikmin2. */
extern s32 lbl_80478980; /* __OSCurrHeap */
extern char lbl_8026FE70[];
extern char lbl_8026FEA8[];
extern void OSReport(const char* format, ...);
extern void* OSGetArenaLo(void);
extern void* OSGetArenaHi(void);
extern void OSSetArenaLo(void* addr);
extern void* OSInitAlloc(void* arenaLo, void* arenaHi, s32 maxHeaps); /* OSInitAlloc */
extern s32 OSCreateHeap(void* start, void* end);                     /* OSCreateHeap */
extern s32 OSSetCurrentHeap(s32 heap);                                    /* OSSetCurrentHeap */
extern void OSFreeToHeap(s32 heap, void* ptr);                        /* OSFreeToHeap */

inline static void InitDefaultHeap(void)
{
    void* arenaLo;
    void* arenaHi;

    OSReport(lbl_8026FE70);
    OSReport(lbl_8026FEA8);

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();

    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
    OSSetArenaLo(arenaLo);

    arenaLo = (void*)(((u32)arenaLo + 31) & ~31);
    arenaHi = (void*)((u32)arenaHi & ~31);

    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaLo = arenaHi);
}

void __sys_free(void* ptr)
{
    if (lbl_80478980 == -1) {
        InitDefaultHeap();
    }

    OSFreeToHeap(lbl_80478980, ptr);
}
