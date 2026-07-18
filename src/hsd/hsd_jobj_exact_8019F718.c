/**
 * @file hsd_jobj_exact_8019F718.c
 * @brief Exact pure-C HSD JObj flag-management island.
 *
 * Address range: 0x8019F718 - 0x8019FF74 (8 functions).
 */

#include "dolphin/mtx.h"

#define iref_DEC hsd_inline_iref_DEC
#define ref_INC hsd_inline_ref_INC
#include "hsd/hsd_jobj.h"
#undef iref_DEC
#undef ref_INC
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_dobj.h"

extern HSD_ClassInfo* lbl_8047B298;
extern u8 lbl_8036C8E0[];
extern char lbl_8047DB20;
extern char lbl_8047DB34;
extern char lbl_8047DB3C;
extern void __assert();

HSD_JObj* fn_80193828(HSD_ClassInfo*);
extern void fn_8019D620(HSD_JObj*);
BOOL fn_8019D980(HSD_JObj* jobj);
void fn_8019F778(HSD_JObj* jobj);
void fn_8019F7F0(HSD_JObj* jobj, u32 flags);
void fn_8019FAEC(HSD_JObj* jobj, u32 flags);
void fn_8019FB90(HSD_JObj* jobj, u32 flags);
void fn_8019FE8C(HSD_JObj* jobj, u32 flags);

/* 0x8019F718 | 0x60 */
#pragma push
#pragma optimization_level 1
HSD_JObj* fn_8019F718(void)
{
    HSD_JObj* jobj;

    jobj = fn_80193828(lbl_8047B298 ? lbl_8047B298
                                     : (HSD_ClassInfo*) lbl_8036C8E0);
    if (jobj == NULL) {
        __assert(&lbl_8047DB20, 0x7DF, &lbl_8047DB3C);
    }

    return jobj;
}
#pragma pop

/* 0x8019F778 | 0x78 */
#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void fn_8019F778(HSD_JObj* jobj) {
    s32 result;
    if (!jobj) return;
    if (!jobj) __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
    result = 0;
    if (!(jobj->flags & 0x00800000)) {
        if (jobj->flags & 0x00000040) {
            result = 1;
        }
    }
    switch (result) {
    case 0:
        fn_8019D620(jobj);
        break;
    }
}
#pragma dont_inline reset
#pragma pop

/* 0x8019F7F0 | 0x2FC */
#pragma push
#pragma optimization_level 1
#pragma use_lmw_stmw on
#pragma inline_depth(5)
#pragma inline_max_size(10000)

static inline BOOL JObjMtxIsDirtyForClearFlags(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(&lbl_8047DB34, 0x25D, &lbl_8047DB3C);
    }
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) &&
        (jobj->flags & JOBJ_MTX_DIRTY))
    {
        result = TRUE;
    }
    return result;
}

static inline void JObjSetMtxDirtyForClearFlags(HSD_JObj* jobj)
{
    if (jobj != NULL && !JObjMtxIsDirtyForClearFlags(jobj)) {
        fn_8019D620(jobj);
    }
}

static inline void JObjClearFlagsInline(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        JObjSetMtxDirtyForClearFlags(jobj);
    }
    jobj->flags &= ~flags;
}

static inline void JObjClearFlagsUsingMtxCheck(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        if (jobj != NULL && !fn_8019D980(jobj)) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags &= ~flags;
}

static inline void JObjClearFlagsUsingDirtyCall(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        fn_8019F778(jobj);
    }
    jobj->flags &= ~flags;
}

static inline void JObjClearFlagsAllLevel5(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    fn_8019FAEC(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            fn_8019F7F0(child, flags);
        }
    }
}

static inline void JObjClearFlagsAllLevel4(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjClearFlagsUsingDirtyCall(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjClearFlagsAllLevel5(child, flags);
        }
    }
}

static inline void JObjClearFlagsAllLevel3(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjClearFlagsUsingMtxCheck(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjClearFlagsAllLevel4(child, flags);
        }
    }
}

static inline void JObjClearFlagsAllLevel2(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjClearFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjClearFlagsAllLevel3(child, flags);
        }
    }
}

static inline void JObjClearFlagsAllLevel1(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjClearFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjClearFlagsAllLevel2(child, flags);
        }
    }
}

void fn_8019F7F0(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjClearFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjClearFlagsAllLevel1(child, flags);
        }
    }
}
#pragma pop

/* 0x8019FAEC | 0xA4 */
#pragma push
#pragma optimization_level 0
#pragma optimization_level 1
void fn_8019FAEC(HSD_JObj* jobj, u32 flags) {
    s32 result;
    if (!jobj) return;
    if (((jobj->flags ^ flags) & 0x8) && jobj != NULL) {
        if (!jobj) __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
        result = 0;
        if (!(jobj->flags & 0x00800000)) {
            if (jobj->flags & 0x00000040) {
                result = 1;
            }
        }
        if (result == 0) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags &= ~flags;
}
#pragma pop

/* 0x8019FB90 | 0x2FC */
#pragma push
#pragma optimization_level 1
/*
 * The target saves r25-r31 with stmw/lmw and expands five recursive helper
 * levels before the remaining call. Keep these compiler controls scoped to
 * this reconstruction; changing the save mode or either limit changes code.
 */
#pragma use_lmw_stmw on
#pragma inline_depth(5)
#pragma inline_max_size(10000)

static inline BOOL JObjMtxIsDirtyForSetFlags(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(&lbl_8047DB34, 0x25D, &lbl_8047DB3C);
    }
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) &&
        (jobj->flags & JOBJ_MTX_DIRTY))
    {
        result = TRUE;
    }
    return result;
}

static inline void JObjSetMtxDirtyForSetFlags(HSD_JObj* jobj)
{
    if (jobj != NULL && !JObjMtxIsDirtyForSetFlags(jobj)) {
        fn_8019D620(jobj);
    }
}

static inline void JObjSetFlagsInline(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        JObjSetMtxDirtyForSetFlags(jobj);
    }
    jobj->flags |= flags;
}

static inline void JObjSetFlagsUsingMtxCheck(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        if (jobj != NULL && !fn_8019D980(jobj)) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags |= flags;
}

static inline void JObjSetFlagsUsingDirtyCall(HSD_JObj* jobj, u32 flags)
{
    if (jobj == NULL) {
        return;
    }
    if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
        fn_8019F778(jobj);
    }
    jobj->flags |= flags;
}

static inline void JObjSetFlagsAllLevel5(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    fn_8019FE8C(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            fn_8019FB90(child, flags);
        }
    }
}

static inline void JObjSetFlagsAllLevel4(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjSetFlagsUsingDirtyCall(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjSetFlagsAllLevel5(child, flags);
        }
    }
}

static inline void JObjSetFlagsAllLevel3(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjSetFlagsUsingMtxCheck(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjSetFlagsAllLevel4(child, flags);
        }
    }
}

static inline void JObjSetFlagsAllLevel2(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjSetFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjSetFlagsAllLevel3(child, flags);
        }
    }
}

static inline void JObjSetFlagsAllLevel1(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjSetFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjSetFlagsAllLevel2(child, flags);
        }
    }
}

void fn_8019FB90(HSD_JObj* jobj, u32 flags)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    JObjSetFlagsInline(jobj, flags);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        for (child = jobj->child; child != NULL; child = child->next) {
            JObjSetFlagsAllLevel1(child, flags);
        }
    }
}
#pragma pop

/* 0x8019FE8C | 0xA4 */
#pragma push
#pragma optimization_level 0
#pragma optimization_level 1
void fn_8019FE8C(HSD_JObj* jobj, u32 flags) {
    s32 result;
    if (!jobj) return;
    if (((jobj->flags ^ flags) & 0x8) && jobj != NULL) {
        if (!jobj) __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
        result = 0;
        if (!(jobj->flags & 0x00800000)) {
            if (jobj->flags & 0x00000040) {
                result = 1;
            }
        }
        if (result == 0) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags |= flags;
}
#pragma pop

/* 0x8019FF30 | 0x18 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma optimization_level 4
u32 HSD_JObjGetFlags(HSD_JObj* jobj) {
    if (jobj != NULL) {
        return jobj->flags;
    }
    return 0;
}
#pragma pop

/* 0x8019FF48 | 0x2C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma optimization_level 4
HSD_DObj* fn_8019FF48(HSD_JObj* jobj) {
    if (jobj == NULL) {
        goto end;
    }
    if (union_type_dobj(jobj)) {
        goto ok;
    }
end:
    return NULL;
ok:
    return jobj->u.dobj;
}
#pragma pop
