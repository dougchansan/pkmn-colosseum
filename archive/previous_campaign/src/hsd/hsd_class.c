/**
 * @file hsd_class.c
 * @brief HSD class system - memory management and class hierarchy.
 *
 * Implements the object-oriented class infrastructure for HSD:
 * - Class initialization and hierarchy management
 * - Memory pool allocation via free lists
 * - Object lifecycle (new/delete)
 * - Class introspection (descendant checks, search)
 *
 * Colosseum address: 0x801938FC (HSD_ClassInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/class.c
 */

#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_object.h"

/* Forward declaration */
static void _hsdClassInfoInit(void);

HSD_ClassInfo hsdClass = { _hsdClassInfoInit };

static HSD_MemoryEntry** memory_list;
static s32 nb_memory_list;

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern int strcmp(const char* s1, const char* s2);
extern void OSReport(const char* fmt, ...);

/* ========================================================================= */
/*  ClassInfoInit                                                            */
/* ========================================================================= */

void ClassInfoInit(HSD_ClassInfo* info)
{
    if ((info->head.flags & 1) == 0) {
        (*info->head.info_init)();
    }
}

/* ========================================================================= */
/*  Memory management                                                        */
/* ========================================================================= */

HSD_MemoryEntry* GetMemoryEntry(s32 idx)
{
    HSD_ASSERT(171, idx >= 0);

    if (idx >= nb_memory_list) {
        if (nb_memory_list == 0) {
            s32 new_nb;
            for (new_nb = 32; idx >= new_nb; new_nb *= 2) {}
            memory_list = (HSD_MemoryEntry**) HSD_MemAlloc(new_nb * 4);
            if (memory_list == NULL) {
                return NULL;
            }
            memset(memory_list, 0, new_nb * 4);
            nb_memory_list = new_nb;
        } else {
            HSD_MemoryEntry** old_list;
            HSD_MemoryEntry** new_list;
            s32 old_nb, new_nb;

            new_nb = nb_memory_list * 2;
            while (idx >= new_nb) {
                new_nb *= 2;
            }

            new_list = HSD_MemAlloc(4 * new_nb);
            if (new_list == NULL) {
                return NULL;
            }

            memcpy(new_list, memory_list, 4 * nb_memory_list);
            memset(&new_list[nb_memory_list], 0,
                   4 * (new_nb - nb_memory_list));

            old_list = memory_list;
            old_nb = ((nb_memory_list * 4 + 31) & ~31);
            memory_list = new_list;
            nb_memory_list = new_nb;

            hsdFreeMemPiece(old_list, old_nb);
        }
    }

    {
        s32 i;
        BOOL found;
        HSD_MemoryEntry* entry;

        if (memory_list[idx] == NULL) {
            entry = HSD_MemAlloc(sizeof(HSD_MemoryEntry));
            if (entry == NULL) {
                return NULL;
            }
            memset(entry, 0, sizeof(HSD_MemoryEntry));
            entry->size = (idx + 1) * 32;
            memory_list[idx] = entry;

            found = FALSE;
            for (i = idx - 1; i >= 0; --i) {
                if (memory_list[i] != NULL) {
                    found = TRUE;
                    entry->next = memory_list[i]->next;
                    memory_list[i]->next = entry;
                    break;
                }
            }
            if (found == FALSE) {
                for (i = idx + 1; i < nb_memory_list; i++) {
                    if (memory_list[i] != NULL) {
                        entry->next = memory_list[i];
                        break;
                    }
                }
            }
        }
        return memory_list[idx];
    }
}

void* hsdAllocMemPiece(s32 size)
{
    HSD_FreeList* temp;
    HSD_MemoryEntry* entry;
    HSD_MemoryEntry* other;
    s32 idx;

    idx = (size + 0x1F) / 32 - 1;
    entry = GetMemoryEntry(idx);
    if (entry == NULL) {
        return NULL;
    }
    if ((temp = entry->free_list) != NULL) {
        entry->free_list = temp->next;
        entry->nb_free -= 1;
        return temp;
    }
    other = entry->next;
    while (other != NULL) {
        if (other->free_list != NULL) {
            HSD_MemoryEntry* remainder_entry;
            HSD_FreeList* block;
            HSD_FreeList* remainder;

            remainder_entry = GetMemoryEntry(
                (s32)(other->size - entry->size + 0x1F) / 32 - 1);
            if (remainder_entry == NULL) {
                return NULL;
            }
            block = other->free_list;
            other->free_list = other->free_list->next;
            other->nb_free -= 1;
            other->nb_alloc -= 1;
            remainder = (HSD_FreeList*)((u8*)block + entry->size);
            remainder->next = remainder_entry->free_list;
            remainder_entry->free_list = remainder;
            remainder_entry->nb_alloc += 1;
            remainder_entry->nb_free += 1;
            entry->nb_alloc += 1;
            return block;
        }
        other = other->next;
    }

    {
        void* mem;
        s32 remainder_idx;
        HSD_MemoryEntry* remainder_entry;

        remainder_idx = (nb_memory_list - idx) - 2;
        if (remainder_idx >= 0) {
            remainder_entry = GetMemoryEntry(remainder_idx);
            if (remainder_entry == NULL) {
                return NULL;
            }
        }
        mem = HSD_MemAlloc(nb_memory_list * 32);
        if (mem == NULL) {
            return NULL;
        }
        if (remainder_idx >= 0) {
            HSD_FreeList* remainder;
            remainder = (HSD_FreeList*)((u8*)mem + entry->size);
            remainder->next = remainder_entry->free_list;
            remainder_entry->free_list = remainder;
            remainder_entry->nb_alloc += 1;
            remainder_entry->nb_free += 1;
        }
        entry->nb_alloc += 1;
        return mem;
    }
}

void hsdFreeMemPiece(void* mem, s32 size)
{
    HSD_MemoryEntry* entry;
    HSD_FreeList* piece = (HSD_FreeList*) mem;

    if (mem != NULL) {
        entry = GetMemoryEntry((size + 31) / 32 - 1);
        piece->next = entry->free_list;
        entry->free_list = piece;
        entry->nb_free += 1;
    }
}

/* ========================================================================= */
/*  Class lifecycle functions                                                */
/* ========================================================================= */

HSD_Class* _hsdClassAlloc(HSD_ClassInfo* info)
{
    HSD_Class* mem_piece = hsdAllocMemPiece(info->head.obj_size);
    if (mem_piece != NULL) {
        info->head.nb_exist += 1;
        if (info->head.nb_exist > info->head.nb_peak) {
            info->head.nb_peak = info->head.nb_exist;
        }
    }
    return mem_piece;
}

int _hsdClassInit(HSD_Class* arg0)
{
    return 0;
}

void _hsdClassRelease(HSD_Class* cls) {}

void _hsdClassDestroy(HSD_Class* cls)
{
    HSD_ClassInfo* info = cls->class_info;
    info->head.nb_exist -= 1;
    hsdFreeMemPiece(cls, info->head.obj_size);
}

void _hsdClassAmnesia(HSD_ClassInfo* info)
{
    info->head.nb_exist = 0;
    info->head.nb_peak = 0;
    if (info == &hsdClass) {
        nb_memory_list = 0;
        memory_list = NULL;
    }
}

/* ========================================================================= */
/*  Class info initialization                                                */
/* ========================================================================= */

static void _hsdClassInfoInit(void)
{
    hsdInitClassInfo(&hsdClass, NULL, "sysdolphin_base_library", "hsd_class",
                     sizeof(HSD_ClassInfo), sizeof(HSD_Class));
    hsdClass.alloc = _hsdClassAlloc;
    hsdClass.init = _hsdClassInit;
    hsdClass.release = _hsdClassRelease;
    hsdClass.destroy = _hsdClassDestroy;
    hsdClass.amnesia = _hsdClassAmnesia;
}

/* ========================================================================= */
/*  hsdNew - create a new object                                             */
/* ========================================================================= */

void* hsdNew(HSD_ClassInfo* i)
{
    HSD_ClassInfo* info = i;
    HSD_Class* cls;

    if (!(info->head.flags & 1)) {
        info->head.info_init();
    }
    cls = info->alloc(info);
    if (cls == NULL) {
        return NULL;
    }
    ClassInfoInit(info);
    memset(cls, 0, info->head.obj_size);
    cls->class_info = info;
    if (info->init(cls) < 0) {
        info->destroy(cls);
        return NULL;
    }
    return cls;
}

/* ========================================================================= */
/*  hsdChangeClass                                                           */
/* ========================================================================= */

BOOL hsdChangeClass(void* object, void* class_info)
{
    HSD_Obj* obj = (HSD_Obj*) object;
    HSD_ClassInfo* old_info;
    HSD_ClassInfo* new_info;

    HSD_ASSERT(0x249, object);
    HSD_ASSERT(0x24A, class_info);

    old_info = obj->parent.class_info;
    new_info = (HSD_ClassInfo*) class_info;

    if (!(new_info->head.flags & 1)) {
        new_info->head.info_init();
    }

    if (old_info->head.obj_size != new_info->head.obj_size) {
        return FALSE;
    }

    while (old_info->head.parent != NULL &&
           old_info->head.parent->head.obj_size == old_info->head.obj_size)
    {
        old_info = old_info->head.parent;
    }
    while (new_info->head.parent != NULL &&
           new_info->head.parent->head.obj_size == new_info->head.obj_size)
    {
        new_info = new_info->head.parent;
    }

    if (old_info == new_info) {
        HSD_ClassInfo* ci = (HSD_ClassInfo*) class_info;
        obj->parent.class_info->head.nb_exist--;
        ci->head.nb_exist++;
        if (ci->head.nb_exist > ci->head.nb_peak) {
            ci->head.nb_peak = ci->head.nb_exist;
        }
        obj->parent.class_info = ci;
        return TRUE;
    }
    return FALSE;
}

/* ========================================================================= */
/*  Inheritance checks                                                       */
/* ========================================================================= */

BOOL hsdIsDescendantOf(void* info, void* p)
{
    HSD_ClassInfo* cur;
    HSD_ClassInfo* cls = (HSD_ClassInfo*) p;

    if (info == NULL || p == NULL) {
        return FALSE;
    }

    cur = (HSD_ClassInfo*) info;

    if (!(HSD_CLASS_INFO(info)->head.flags & 1)) {
        cur->head.info_init();
    }
    if (!(cls->head.flags & 1)) {
        cls->head.info_init();
    }
    while (cur != NULL) {
        if (cur == cls) {
            return TRUE;
        }
        cur = cur->head.parent;
    }
    return FALSE;
}

BOOL hsdObjIsDescendantOf(HSD_Obj* o, HSD_ClassInfo* p)
{
    HSD_ClassInfo* info;

    if (o == NULL || p == NULL) {
        return FALSE;
    }
    info = o->parent.class_info;
    if (!(p->head.flags & 1)) {
        p->head.info_init();
    }
    while (info != NULL) {
        if (info == p) {
            return TRUE;
        }
        info = info->head.parent;
    }
    return FALSE;
}

/* ========================================================================= */
/*  hsdSearchClassInfo                                                       */
/* ========================================================================= */

HSD_ClassInfo* hsdSearchClassInfo(const char* class_name)
{
    /* Colosseum may not use the hash-based search that Melee uses.
     * Return NULL for now - to be refined when we identify the
     * actual implementation. */
    return NULL;
}

/* ========================================================================= */
/*  hsdForgetClassLibrary                                                    */
/* ========================================================================= */

static void ForgetClassLibraryReal(HSD_ClassInfo* class_info)
{
    HSD_ClassInfo* cur = class_info->head.child;
    HSD_ClassInfo* next;
    while (cur != NULL) {
        next = cur->head.next;
        cur->head.next = NULL;
        ForgetClassLibraryReal(cur);
        cur = next;
    }
    class_info->amnesia(class_info);
    class_info->head.child = NULL;
    class_info->head.parent = NULL;
    class_info->head.flags &= ~1;
}

static void ForgetClassLibraryChild(const char* library_name,
                                    HSD_ClassInfo* class_info)
{
    HSD_ClassInfo** cur = &class_info->head.child;
    while (*cur != NULL) {
        if (strcmp(library_name, (*cur)->head.library_name) == 0) {
            ForgetClassLibraryReal(*cur);
            *cur = (*cur)->head.next;
        } else {
            cur = &(*cur)->head.next;
        }
    }
}

void hsdForgetClassLibrary(const char* library_name)
{
    if (library_name == NULL) {
        library_name = "sysdolphin_base_library";
    }
    if (!(hsdClass.head.flags & 1)) {
        return;
    }
    if (strcmp(library_name, hsdClass.head.library_name) == 0) {
        ForgetClassLibraryReal(&hsdClass);
    } else {
        ForgetClassLibraryChild(library_name, &hsdClass);
    }
}

/* 0x80193A1C | 0x3C */
extern u32 lbl_8047B224;
extern u32 lbl_8047B220;
extern u32 lbl_8047B228;
extern u8 lbl_8036C638[64];
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
void fn_80193A1C(HSD_ClassInfo* info) {
    info->head.nb_exist = 0;
    info->head.nb_peak = 0;
    if (info == (HSD_ClassInfo*)lbl_8036C638) {
        lbl_8047B224 = 0;
        lbl_8047B220 = 0;
        lbl_8047B228 = 0;
    }
}
#pragma pop

/* 0x80193A58 | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801A6960(void* cls);
#if 0
asm void fn_80193A58(void) {
#include "src/hsd/hsd_class_fn_80193A58.inc"
}
#else
#pragma optimization_level 4
void fn_80193A58(HSD_Class* cls) {
    HSD_ClassInfo* info = cls->class_info;
    info->head.nb_exist -= 1;
    fn_801A6960(cls);
}
#endif
#pragma pop

/* 0x80193A88 | 0x4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80193A88(void) {
#include "src/hsd/hsd_class_fn_80193A88.inc"
}
#else
#pragma optimization_level 4
void fn_80193A88(HSD_Class* cls) {}
#endif
#pragma pop

/* 0x80193A8C | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80193A8C(void) {
#include "src/hsd/hsd_class_fn_80193A8C.inc"
}
#else
#pragma optimization_level 4
int fn_80193A8C(HSD_Class* cls) { return 0; }
#endif
#pragma pop

/* 0x80193A94 | 0x5C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* fn_801A6928(s32 size);
#if 0
asm void fn_80193A94(void) {
#include "src/hsd/hsd_class_fn_80193A94.inc"
}
#else
#pragma optimization_level 4
HSD_Class* fn_80193A94(HSD_ClassInfo* info) {
    HSD_Class* result = (HSD_Class*) fn_801A6928((s32) info->head.obj_size);
    if (result != NULL) {
        info->head.nb_exist += 1;
        if (info->head.nb_exist > info->head.nb_peak) {
            info->head.nb_peak = *(u32 volatile*)&info->head.nb_exist;
        }
    }
    return result;
}
#endif
#pragma pop

/* 0x80193AF0 | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80193AF0(void) {
#include "src/hsd/hsd_class_fn_80193AF0.inc"
}
#else
#pragma optimization_level 4
void fn_80193AF0(void* cls) {
    fn_801A6960(cls);
}
#endif
#pragma pop

/* 0x80193B10 | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80193B10(void) {
#include "src/hsd/hsd_class_fn_80193B10.inc"
}
#else
#pragma optimization_level 4
void fn_80193B10(HSD_ClassInfo* info) {
    fn_801A6928((s32)(u32)info);
}
#endif
#pragma pop

/* 0x80193B30 | 0xF4 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern char lbl_8047D950;
extern char lbl_802745B4[];
extern char lbl_802745EC[];
#if 0
asm void hsdInitClassInfo(void) {
#include "src/hsd/hsd_class_fn_80193B30.inc"
}
#else
void hsdInitClassInfo(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info,
                      char* base_class_library, char* type, s32 info_size,
                      s32 class_size)
{
    class_info->head.flags = 1;
    class_info->head.library_name = base_class_library;
    class_info->head.class_name = type;
    class_info->head.obj_size = (s16) class_size;
    class_info->head.info_size = (s16) info_size;
    class_info->head.parent = parent_info;
    class_info->head.child = NULL;
    class_info->head.next = NULL;
    class_info->head.nb_exist = 0;
    class_info->head.nb_peak = 0;

    if (parent_info != NULL) {
        if ((parent_info->head.flags & 1) == 0) {
            (*parent_info->head.info_init)();
        }
        if (class_info->head.obj_size < parent_info->head.obj_size) {
            __assert(&lbl_8047D950, 0x67, lbl_802745B4);
        }
        if (class_info->head.info_size < parent_info->head.info_size) {
            __assert(&lbl_8047D950, 0x68, lbl_802745EC);
        }
        memcpy(&class_info->alloc, &parent_info->alloc,
               parent_info->head.info_size - 0x28);
        class_info->head.next = parent_info->head.child;
        parent_info->head.child = class_info;
    }
}
#endif
#pragma pop
