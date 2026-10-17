/**
 * @file memory.c
 * @brief HAL sysdolphin memory.c: the library's allocator front end,
 *        0x801A6928-0x801A6A34.
 *
 * Melee's memory.c (doldecomp/melee, src/sysdolphin/baselib/memory.c) wraps
 * OSAllocFromHeap/OSFreeToHeap on HSD_GetHeap(). Colosseum's sysdolphin
 * (>= 1.3.0.0) routes every allocation through a table of callbacks
 * instead, installed by _HSD_MemSetCallbacks: initialize.c registers its
 * GSmem-backed defaults (_HSD_Mem*DefaultCB) unless the game passed its own
 * through HSD_SetInitParameter(HSD_INIT_MEMORY_CALLBACKS, ...). The table's
 * type name and the assert line come from retail's assertion
 * ("memory.c", 20, "size == sizeof(__mem_cb)").
 *
 * The library is built with deferred inlining, so functions are listed in
 * HAL's order and MWCC emits them in reverse (the retail address order).
 * The allocator entry points keep their dtk names because many objects
 * link against them; the comments give the HAL names.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/debug.h"

typedef struct __mem_cb {
    void* (*alloc)(u32 size, u32 align, u32 flags);
    void (*free)(void* ptr);
    void (*clear)(void);
    u32 (*get_remain)(void);
    BOOL (*check_own)(void* ptr);
} __mem_cb;

/* HAL: the callback table (0x14 bytes). */
__mem_cb lbl_80465608;

void _HSD_MemSetCallbacks(__mem_cb* cb, u32 size)
{
    HSD_ASSERT(20, size == sizeof(__mem_cb));
    lbl_80465608 = *cb;
}

/* HAL: HSD_MemCheckOwn */
BOOL fn_801A6990(void* ptr)
{
    return lbl_80465608.check_own(ptr);
}

/* HAL: HSD_Free */
void HSD_Free(void* ptr)
{
    lbl_80465608.free(ptr);
}

/* HAL: HSD_MemAlloc */
void* HSD_MemAlloc(u32 size)
{
    return lbl_80465608.alloc(size, 32, 0);
}
