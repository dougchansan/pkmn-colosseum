/**
 * @file list.c
 * @brief HAL list.c: HSD singly/doubly linked list nodes, 0x801A3E64 -
 *        0x801A4000.
 *
 * The whole translation unit, built with the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly) and no local pragmas. It owns:
 *   .text   0x801A3E64 - 0x801A4000 (jobj.c ends at 0x801A3E64, lobj.c
 *           starts at 0x801A4000)
 *   .bss    0x80465588 - 0x804655E0 (dlist_alloc_data, slist_alloc_data)
 *   .sdata2 0x8047DBA0 - 0x8047DBB8 (__FILE__ "list.c", "prev", "list")
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/list.c), in HAL's order (deferred inlining emits
 * the functions in reverse). Colosseum's list.c has no append functions:
 * their "next" assert string is not in the pool. HSD_SListPrependList is
 * kept for the pool order ("prev" before "list"; the linker strips it, and
 * HSD_SListAllocAndPrepend carries its expansion, assert 202).
 *
 * The symbols keep their address names: lbl_804655B4 is slist_alloc_data,
 * lbl_80465588 dlist_alloc_data, HSD_ListInitAllocData HSD_ListInitAllocData,
 * HSD_SListAlloc HSD_SListAlloc, HSD_SListRemove HSD_SListRemove, and
 * HSD_SListPrepend is HSD_SListAllocAndPrepend.
 */
#include "dolphin/types.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_objalloc.h"
#include "sysdolphin/baselib/debug.h"

extern void* memset(void* dst, int val, u32 size);
extern void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
extern void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);

typedef struct HSD_DList {
    struct HSD_DList* next;
    struct HSD_DList* prev;
    void* data;
} HSD_DList;

HSD_ObjAllocData lbl_804655B4;
HSD_ObjAllocData lbl_80465588;
#define slist_alloc_data lbl_804655B4
#define dlist_alloc_data lbl_80465588

/* HSD_ListInitAllocData */
void HSD_ListInitAllocData(void)
{
    HSD_ObjAllocInit(&slist_alloc_data, sizeof(HSD_SList), 4);
    HSD_ObjAllocInit(&dlist_alloc_data, sizeof(HSD_DList), 4);
}

HSD_ObjAllocData* HSD_SListGetAllocData(void)
{
    return &slist_alloc_data;
}

HSD_ObjAllocData* HSD_DListGetAllocData(void)
{
    return &dlist_alloc_data;
}

/* HSD_SListAlloc */
HSD_SList* HSD_SListAlloc(void)
{
    HSD_SList* list;

    list = HSD_ObjAlloc(HSD_SListGetAllocData());
    HSD_ASSERT(76, list);

    memset(list, 0, sizeof(HSD_SList));
    return list;
}

HSD_SList* HSD_SListPrependList(HSD_SList* list, HSD_SList* prev);

/* HSD_SListAllocAndPrepend */
HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data)
{
    HSD_SList* list;

    list = HSD_SListAlloc();
    list->data = data;

    return HSD_SListPrependList(next, list);
}

HSD_SList* HSD_SListPrependList(HSD_SList* list, HSD_SList* prev)
{
    HSD_ASSERT(202, prev);
    prev->next = list;
    return prev;
}

/* HSD_SListRemove */
HSD_SList* HSD_SListRemove(HSD_SList* list)
{
    HSD_SList* next;

    if (list != NULL) {
        next = list->next;
        HSD_ObjFree(HSD_SListGetAllocData(), list);
        return next;
    }

    return NULL;
}
