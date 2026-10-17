/**
 * @file class.c
 * @brief HAL class.c: the HSD base class (hsdClass) and class-info helpers,
 *        0x80193748 - 0x80193C24.
 *
 * The whole translation unit, built with the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly) and no local pragmas. It owns:
 *   .text   0x80193748 - 0x80193C24
 *   .rodata 0x80274590 - 0x80274628 ("sysdolphin_base_library", "hsd_class",
 *           the two hsdInitClassInfo assert expressions)
 *   .data   0x8036C638 - 0x8036C678 (hsdClass)
 *   .sbss   0x8047B220 - 0x8047B230 (memory_list, nb_memory_list,
 *           current_hash)
 *   .sdata2 0x8047D950 - 0x8047D958 (__FILE__, "class.c")
 *
 * The functions are written in HAL's (Melee's) order; deferred inlining
 * emits them in reverse, which is retail's address order. Colosseum's HAL
 * library no longer has the memory-piece allocator: hsdAllocMemPiece and
 * hsdFreeMemPiece forward to HSD_MemAlloc / HSD_Free. Everything Colosseum
 * never references (hsdChangeClass, hsdObjIsDescendantOf, the
 * ForgetClassLibrary family, the class-stat dumps) is dead-stripped from the
 * retail image and is therefore not reproduced here.
 *
 * ClassInfoInit is auto-inlined into its callers and, with nothing else
 * referencing it, dead-stripped at link time, so it never reaches the binary.
 * hsdInitClassInfo is auto-inlined into _hsdClassInfoInit with a NULL parent
 * (retail has no call there), and hsdAllocMemPiece / hsdFreeMemPiece into
 * _hsdClassAlloc / _hsdClassDestroy.
 *
 * hsdIsDescendantOf copies its arguments into locals only after the NULL
 * checks, which is what keeps retail's register use: the checks test the
 * argument registers, then the class walk runs in r31/r30.
 *
 * ClassInfoInitGet is a reconstructed inline (it ensures the class info is
 * initialised and returns it). hsdNew carries its fingerprint: after the
 * allocation, retail copies the class info into a second callee-saved
 * register (mr r31,r29) and makes the info_init call, the obj_size load and
 * the init call through that copy, while the class_info store and the
 * destroy call keep using the argument (r29). That is the helper's return
 * value in its own home register; ClassInfoInit (void) cannot produce it,
 * nor can a local copy, which MWCC propagates away. Melee's retail
 * hsdChangeClass has the same shape (its decompilation spells it as
 * HSD_PushClassInfo followed by the init check), so it is a HAL construct.
 *
 * The symbols keep their address names: lbl_8036C638 is hsdClass,
 * lbl_8047B220 memory_list, lbl_8047B224 nb_memory_list, lbl_8047B228
 * current_hash, fn_80193B10 hsdAllocMemPiece, fn_80193AF0 hsdFreeMemPiece,
 * fn_801938FC _hsdClassInfoInit, fn_80193828 hsdNew, hsdIsDescendantOf
 * hsdIsDescendantOf, fn_80193748 hsdSearchClassInfo.
 */
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_hash.h"

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* HSD_MemAlloc(s32 size); /* HSD_MemAlloc */
extern void HSD_Free(void* mem); /* HSD_Free */

void fn_801938FC(void);

HSD_ClassInfo lbl_8036C638 = { fn_801938FC }; /* hsdClass */

HSD_Hash* lbl_8047B228;               /* current_hash */
s32 lbl_8047B224;                     /* nb_memory_list */
HSD_MemoryEntry** lbl_8047B220;       /* memory_list */

void ClassInfoInit(HSD_ClassInfo* info)
{
    if ((info->head.flags & 1) == 0) {
        (*info->head.info_init)();
    }
}

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
        HSD_ASSERT(103, class_info->head.obj_size >= parent_info->head.obj_size);
        HSD_ASSERT(104, class_info->head.info_size >= parent_info->head.info_size);
        memcpy(&class_info->alloc, &parent_info->alloc,
               parent_info->head.info_size - sizeof(HSD_ClassInfoHead));
        class_info->head.next = parent_info->head.child;
        parent_info->head.child = class_info;
    }
}

void* fn_80193B10(s32 size)
{
    return HSD_MemAlloc(size);
}

void fn_80193AF0(void* mem, s32 size)
{
    HSD_Free(mem);
}

HSD_Class* _hsdClassAlloc(HSD_ClassInfo* info)
{
    HSD_Class* mem_piece = fn_80193B10(info->head.obj_size);

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
    fn_80193AF0(cls, info->head.obj_size);
}

void _hsdClassAmnesia(HSD_ClassInfo* info)
{
    info->head.nb_exist = 0;
    info->head.nb_peak = 0;
    if (info == &lbl_8036C638) {
        lbl_8047B224 = 0;
        lbl_8047B220 = NULL;
        lbl_8047B228 = NULL;
    }
}

void fn_801938FC(void)
{
    hsdInitClassInfo(&lbl_8036C638, NULL, "sysdolphin_base_library",
                     "hsd_class", sizeof(HSD_ClassInfo), sizeof(HSD_Class));
    lbl_8036C638.alloc = _hsdClassAlloc;
    lbl_8036C638.init = _hsdClassInit;
    lbl_8036C638.release = _hsdClassRelease;
    lbl_8036C638.destroy = _hsdClassDestroy;
    lbl_8036C638.amnesia = _hsdClassAmnesia;
}

static inline HSD_ClassInfo* ClassInfoInitGet(HSD_ClassInfo* info)
{
    HSD_ClassInfo* ret = info;

    if (!(ret->head.flags & 1)) {
        ret->head.info_init();
    }
    return ret;
}

void* fn_80193828(HSD_ClassInfo* info)
{
    HSD_ClassInfo* ci;
    HSD_Class* cls;

    ClassInfoInit(info);
    cls = info->alloc(info);
    if (cls == NULL) {
        return NULL;
    }
    ci = ClassInfoInitGet(info);
    memset(cls, 0, ci->head.obj_size);
    cls->class_info = info;
    if (ci->init(cls) < 0) {
        info->destroy(cls);
        return NULL;
    }
    return cls;
}

BOOL hsdIsDescendantOf(void* info, void* p)
{
    HSD_ClassInfo* c;
    HSD_ClassInfo* parent;

    if (info == NULL || p == NULL) {
        return FALSE;
    }
    c = info;
    parent = p;
    ClassInfoInit(c);
    ClassInfoInit(parent);
    while (c != NULL) {
        if (c == parent) {
            return TRUE;
        }
        c = c->head.parent;
    }
    return FALSE;
}

HSD_ClassInfo* fn_80193748(const char* class_name)
{
    if (lbl_8047B228 != NULL) {
        return HSD_HashSearch(lbl_8047B228, (void*) class_name, 0);
    }
    return NULL;
}
