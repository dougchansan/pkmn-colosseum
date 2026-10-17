/**
 * @file gs_mem.h
 * @brief GSmem -- Genius Sonority's handle-based heap allocator.
 *
 * GSmem manages one region carved from the Dolphin OS arena (main() passes
 * arenaLo .. arenaLo + 0xE80000).  Free memory is a doubly linked,
 * address-ordered list of GSmemBlock headers stored in the free space
 * itself.  Every allocation is described by a 16-byte GSmemEntry in a
 * handle table that grows downward from the top of the region; callers hold
 * the 1-based table index (the "handle") and lock it to get a pointer.
 *
 * Retail symbols (behaviour recovered from src/game/gs_mem.c and the
 * "GSmem: ..." log strings):
 *   GSmemGetHandleByPtr          pointer -> handle lookup
 *   GSmemFree          free a handle
 *   fn_800E24B0          unlock (decrement the lock count)
 *   GSmemLock          lock (increment the lock count, return pointer)
 *   fn_800E2AF8          always returns 1
 *   fn_800E2B00          aligned allocation from the top of a free block
 *   GSmemAlloc          aligned allocation (first/best/worst fit)
 *   GSmemShrink          place an allocation at a fixed address
 *   _toolentryAlloc__FUl 32-byte aligned allocation (GSmemAlloc)
 *   fn_800E3560          select the fit strategy (GSMEM_FIT_*)
 *   GSmemInit            initialise the heap
 *
 * The allocation functions are declared by each caller: several of them
 * need the handle as a full word (see src/game/gs_res.c), so no shared
 * prototype is given here.
 */
#ifndef GS_MEM_H
#define GS_MEM_H

#include "dolphin/types.h"

/* Header of a free block; lives at the start of the free memory itself. */
typedef struct GSmemBlock {
    /* 0x00 */ struct GSmemBlock* prev; /* lower-addressed free block      */
    /* 0x04 */ struct GSmemBlock* next; /* higher-addressed free block     */
    /* 0x08 */ u32 size;                /* bytes in the block, header incl. */
} GSmemBlock;

/* Handle table entry.  Entry N (handle N) sits at tableTop - (N - 1). */
typedef struct GSmemEntry {
    /* 0x00 */ u16 handle;    /* own handle while in use, 0 when free      */
    /* 0x02 */ u16 lockCount; /* number of outstanding locks               */
    /* 0x04 */ void* data;    /* start of the allocation                   */
    /* 0x08 */ u32 size;      /* bytes owned by the allocation             */
    /* 0x0C */ u16 align;     /* requested alignment, 0xFFFF if none       */
    /* 0x0E */ u16 checksum;  /* guard mode: checksum taken at last unlock */
} GSmemEntry;

/* Fit strategies for GSmemAlloc (stored in lbl_8047AB2C). */
#define GSMEM_FIT_FIRST 0
#define GSMEM_FIT_BEST  1
#define GSMEM_FIT_WORST 2

void GSmemInit(u32 guardMode, void* start, void* end);

#endif /* GS_MEM_H */
