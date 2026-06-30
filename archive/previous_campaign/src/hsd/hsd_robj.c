/**
 * @file hsd_robj.c
 * @brief HSD RObj - Reference/constraint object implementation.
 *
 * Adapted from doldecomp/melee src/sysdolphin/baselib/robj.c
 */

#include "hsd/hsd_robj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_memory.h"

extern void* memset(void* dst, int val, u32 size);

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_RObjAnimAll(HSD_RObj* robj)
{
    HSD_RObj* r;
    for (r = robj; r != NULL; r = r->next) {
        /* Interpret AObj -> update constraint */
    }
}

void HSD_RObjRemoveAnimAll(HSD_RObj* robj)
{
    HSD_RObj* r;
    for (r = robj; r != NULL; r = r->next) {
        HSD_AObjRemove(r->aobj);
        r->aobj = NULL;
    }
}

void HSD_RObjReqAnimAll(HSD_RObj* robj, f32 startframe)
{
    HSD_RObj* r;
    for (r = robj; r != NULL; r = r->next) {
        HSD_AObjReqAnim(r->aobj, startframe);
    }
}

void HSD_RObjAddAnimAll(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{
    HSD_RObj* r;
    HSD_RObjAnimJoint* a;

    r = robj;
    a = anim;
    while (r != NULL && a != NULL) {
        if (r->aobj != NULL) {
            HSD_AObjRemove(r->aobj);
        }
        r->aobj = HSD_AObjLoadDesc(a->aobjdesc);
        r = r->next;
        a = a->next;
    }
}

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags)
{
    if (robj != NULL) {
        robj->flags = flags;
    }
}

/* ========================================================================= */
/*  Load / resolve                                                           */
/* ========================================================================= */

HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc* desc)
{
    HSD_RObj* first = NULL;
    HSD_RObj* prev = NULL;
    HSD_RObjDesc* d;

    for (d = desc; d != NULL; d = d->next) {
        HSD_RObj* robj = HSD_RObjAlloc();
        robj->flags = d->flags;

        switch (d->flags & ROBJ_TYPE_MASK) {
        case REFTYPE_LIMIT:
            robj->u.limit = d->u.limit;
            break;
        case REFTYPE_IKHINT:
            if (d->u.ik_hint != NULL) {
                robj->u.ik_hint.bone_length = d->u.ik_hint->bone_length;
                robj->u.ik_hint.rotate_x = d->u.ik_hint->rotate_x;
            }
            break;
        default:
            break;
        }

        if (prev != NULL) {
            prev->next = robj;
        } else {
            first = robj;
        }
        prev = robj;
    }
    return first;
}

void HSD_RObjResolveRefs(HSD_RObj* robj, HSD_RObjDesc* desc)
{
    /* Resolve joint/object references post-load */
}

void HSD_RObjResolveRefsAll(HSD_RObj* robj, HSD_RObjDesc* desc)
{
    HSD_RObj* r;
    HSD_RObjDesc* d;

    r = robj;
    d = desc;
    while (r != NULL && d != NULL) {
        HSD_RObjResolveRefs(r, d);
        r = r->next;
        d = d->next;
    }
}

/* ========================================================================= */
/*  Remove / Alloc                                                           */
/* ========================================================================= */

void HSD_RObjRemove(HSD_RObj* robj)
{
    if (robj != NULL) {
        HSD_AObjRemove(robj->aobj);
        HSD_Free(robj);
    }
}

void HSD_RObjRemoveAll(HSD_RObj* robj)
{
    HSD_RObj* next;
    while (robj != NULL) {
        next = robj->next;
        HSD_RObjRemove(robj);
        robj = next;
    }
}

HSD_RObj* HSD_RObjAlloc(void)
{
    HSD_RObj* robj = (HSD_RObj*) HSD_MemAlloc(sizeof(HSD_RObj));
    if (robj != NULL) {
        memset(robj, 0, sizeof(HSD_RObj));
    }
    return robj;
}

void HSD_RObjFree(HSD_RObj* robj)
{
    if (robj != NULL) {
        HSD_Free(robj);
    }
}
