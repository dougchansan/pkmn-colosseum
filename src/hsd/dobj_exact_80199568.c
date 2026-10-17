/**
 * @file dobj_exact_80199568.c
 * @brief sysdolphin dobj.c, animation tail: .text 0x80199568-0x8019970C.
 *
 * HSD_DObjAnimAll, HSD_DObjReqAnimAllByFlags, HSD_DObjAddAnimAll and
 * HSD_DObjSetCurrent. Built with the sysdolphin library flags (GC/1.3.2
 * -O4,p -O1 -inline auto,deferred -use_lmw_stmw on). Deferred inlining emits
 * functions in reverse definition order, so they are listed here in
 * HAL/Melee source order (last address first).
 *
 * The per-DObj helpers HSD_DObjAnim, HSD_DObjReqAnimByFlags and
 * HSD_DObjAddAnim have no retail symbol: each loop above carries the
 * helper's own NULL guard re-tested inside the loop (an inline fingerprint),
 * so they are recovered as static inline.
 *
 * Text-only unit carved from dobj.c (.text 0x80198F7C-0x8019970C):
 * current_dobj (.sbss 0x8047B264) stays extern. DObjLoad (0x801993A4) is
 * its own linked unit, src/hsd/dobj_exact_801993A4.c.
 */

#include "hsd/hsd_dobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"

/* HSD_PObjAddAnimAll */
extern void HSD_PObjAddAnimAll(HSD_PObj* pobj, HSD_ShapeAnim* shapeanim);
extern void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, f32 startframe,
                                      void* flags);
extern void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, f32 startframe,
                                   void* flags);

/* current_dobj */
extern HSD_DObj* lbl_8047B264;

void HSD_DObjSetCurrent(HSD_DObj* dobj)
{
    lbl_8047B264 = dobj;
}

static inline void HSD_DObjAddAnim(HSD_DObj* dobj, HSD_MatAnim* mat_anim,
                                   HSD_ShapeAnimDObj* sh_anim)
{
    if (dobj == NULL) {
        return;
    }

    HSD_PObjAddAnimAll(dobj->pobj, sh_anim != NULL ? sh_anim->shapeanim : NULL);
    HSD_MObjAddAnim(dobj->mobj, mat_anim);
}

void HSD_DObjAddAnimAll(HSD_DObj* dobj, void* matanim, void* shapeanimdobj)
{
    HSD_DObj* dp;
    HSD_MatAnim* ma;
    HSD_ShapeAnimDObj* sd;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj, ma = matanim, sd = shapeanimdobj; dp != NULL;
         dp = dp->next, ma = next_p(ma), sd = next_p(sd))
    {
        HSD_DObjAddAnim(dp, ma, sd);
    }
}

static inline void HSD_DObjReqAnimByFlags(HSD_DObj* dobj, f32 startframe,
                                          void* flags)
{
    if (dobj == NULL) {
        return;
    }

    HSD_PObjReqAnimAllByFlags(dobj->pobj, startframe, flags);
    HSD_MObjReqAnimByFlags(dobj->mobj, startframe, flags);
}

void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, f32 startframe, void* flags)
{
    HSD_DObj* dp;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj; dp != NULL; dp = dp->next) {
        HSD_DObjReqAnimByFlags(dp, startframe, flags);
    }
}

static inline void HSD_DObjAnim(HSD_DObj* dobj)
{
    if (dobj == NULL) {
        return;
    }

    HSD_AObjInterpretAnim(dobj->aobj, dobj, HSD_DOBJ_METHOD(dobj)->update);
    HSD_PObjAnimAll(dobj->pobj);
    HSD_MObjAnim(dobj->mobj);
}

void HSD_DObjAnimAll(HSD_DObj* dobj)
{
    HSD_DObj* dp;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj; dp != NULL; dp = dp->next) {
        HSD_DObjAnim(dp);
    }
}
