/**
 * @file hsd_fobj.c
 * @brief HSD FObj - Function/animation keyframe object implementation.
 *
 * FObj is the lowest-level animation primitive. It stores packed
 * keyframe data and interpolation state for a single animated
 * parameter (e.g., joint rotation X, material alpha, etc.)
 *
 * Adapted from doldecomp/melee src/sysdolphin/baselib/fobj.c
 */

#include "hsd/hsd_fobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_memory.h"

extern void* memset(void* dst, int val, u32 size);

HSD_FObj* HSD_FObjAlloc_Early(void);

/* ========================================================================= */
/*  Remove                                                                   */
/* ========================================================================= */

void HSD_FObjRemove_Early(HSD_FObj* fobj)
{
    if (fobj != NULL) {
        HSD_Free(fobj);
    }
}

void HSD_FObjRemoveAll_Early(HSD_FObj* fobj)
{
    HSD_FObj* next;
    while (fobj != NULL) {
        next = fobj->next;
        HSD_FObjRemove_Early(fobj);
        fobj = next;
    }
}

/* ========================================================================= */
/*  Request animation                                                        */
/* ========================================================================= */

void HSD_FObjReqAnimAll_Early(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* f;
    for (f = fobj; f != NULL; f = f->next) {
        f->ad = f->ad_head;
        f->time = startframe;
        f->flags = 0;
    }
}

/* ========================================================================= */
/*  Load                                                                     */
/* ========================================================================= */

HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    HSD_FObj* first = NULL;
    HSD_FObj* prev = NULL;
    HSD_FObjDesc* d;

    for (d = desc; d != NULL; d = d->next) {
        HSD_FObj* fobj = HSD_FObjAlloc_Early();
        fobj->ad = d->ad;
        fobj->ad_head = d->ad;
        fobj->length = d->length;
        fobj->startframe = (s16) d->startframe;
        fobj->obj_type = d->type;
        fobj->frac_value = d->frac_value;
        fobj->frac_slope = d->frac_slope;
        fobj->time = d->startframe;
        fobj->flags = 0;

        if (prev != NULL) {
            prev->next = fobj;
        } else {
            first = fobj;
        }
        prev = fobj;
    }
    return first;
}

/* ========================================================================= */
/*  Alloc / Free                                                             */
/* ========================================================================= */

HSD_FObj* HSD_FObjAlloc_Early(void)
{
    HSD_FObj* fobj = (HSD_FObj*) HSD_MemAlloc(sizeof(HSD_FObj));
    if (fobj != NULL) {
        memset(fobj, 0, sizeof(HSD_FObj));
    }
    return fobj;
}

void HSD_FObjFree_Early(HSD_FObj* fobj)
{
    if (fobj != NULL) {
        HSD_Free(fobj);
    }
}
