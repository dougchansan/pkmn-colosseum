/**
 * @file objalloc.c
 * @brief sysdolphin objalloc.c - object allocation bookkeeping.
 *
 * Retail extent (the whole TU):
 *   .text   0x801AA350-0x801AA568  _HSD_ObjAllocForgetMemory .. HSD_ObjSetHeap
 *   .rodata 0x80274E90-0x80274EC8  "objalloc.c", "align <= 32",
 *                                  "HSD_GetNbBits(align) == 1"
 *   .data   0x8036CBF0-0x8036CC00  obj_heap = { 0, 0, -1, -1 }
 *   .sdata2 0x8047DC98-0x8047DCA0  "data"
 *   .sbss   0x8047B2E0-0x8047B2E8  alloc_datas
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on -str reuse,readonly). Deferred
 * inlining emits functions in reverse definition order, so they are listed
 * in HAL/Melee source order (HSD_ObjSetHeap first). With -str readonly the
 * assert file name and the long expressions pool in .rodata and the short
 * "data" lands in .sdata2, exactly as retail.
 *
 * Colosseum's sysdolphin (>= 1.3.0.0) no longer carves objects out of free
 * lists: HSD_ObjAlloc only enforces the object count limit, tracks
 * used/peak and forwards to the memory callbacks (HSD_MemAlloc /
 * HSD_Free, HSD_MemAlloc / HSD_Free); HSD_ObjFree drops the count.
 * HSD_ObjAllocAddFree and the heap-limit logic of Melee's version are gone.
 */

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_objalloc.h"

typedef struct objheap {
    u32 top;
    u32 curr;
    u32 size;
    u32 remain;
} objheap;

extern void* memset(void* dst, int val, u32 n);
extern s32 HSD_GetNbBits(u32 x);
extern void* HSD_MemAlloc(u32 size); /* HSD_MemAlloc */
extern void HSD_Free(void* ptr); /* HSD_Free */

static objheap obj_heap = { 0, 0, -1, -1 };

static HSD_ObjAllocData* alloc_datas;

void HSD_ObjSetHeap(u32 size, void* ptr)
{
    obj_heap.curr = (u32) ptr;
    obj_heap.top = (u32) ptr;
    obj_heap.remain = size;
    obj_heap.size = size;
}

void* HSD_ObjAlloc(HSD_ObjAllocData* data)
{
    if (data->num_limit_flag && data->used >= data->numLimit) {
        return NULL;
    }
    data->used += 1;
    if (data->used > data->peak) {
        data->peak = data->used;
    }
    return HSD_MemAlloc(data->size);
}

void HSD_ObjFree(HSD_ObjAllocData* data, void* obj)
{
    data->used -= 1;
    HSD_Free(obj);
}

static inline void removeAll(HSD_ObjAllocData* data)
{
    HSD_ObjAllocData** cur = &alloc_datas;

    while (*cur != NULL) {
        if (*cur == data) {
            *cur = (*cur)->next;
        } else {
            cur = &(*cur)->next;
        }
    }
}

void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align)
{
    HSD_ASSERT(430, data);
    if (data != NULL) {
        removeAll(data);
    } else {
        alloc_datas = NULL;
    }
    memset(data, 0, sizeof(HSD_ObjAllocData));
    data->numLimit = -1;
    data->heapLimitSize = 0;
    data->heapLimitNum = -1;
    HSD_ASSERT(441, align <= 32);
    HSD_ASSERT(442, HSD_GetNbBits(align) == 1);
    data->align = align;
    data->size = (data->align - 1 + size) & ~(data->align - 1);
    data->next = alloc_datas;
    alloc_datas = data;
}

void _HSD_ObjAllocForgetMemory(void)
{
    alloc_datas = NULL;
}
