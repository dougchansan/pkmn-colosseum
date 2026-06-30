#include "hsd/hsd_jobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_robj.h"

#include <string.h>

HSD_JObj* HSD_JObjAlloc(void) {
    HSD_JObj* jobj = (HSD_JObj*)HSD_MemAlloc(sizeof(HSD_JObj));
    if (jobj != NULL) {
        memset(jobj, 0, sizeof(HSD_JObj));
    }
    return jobj;
}

void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj) {
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    jobj->flags |= JOBJ_MTX_DIRTY;
    for (child = jobj->child; child != NULL; child = child->next) {
        HSD_JObjSetMtxDirtySub(child);
    }
}

void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame) {
    if (jobj == NULL) {
        return;
    }
    HSD_AObjReqAnim(jobj->aobj, frame);
    HSD_RObjReqAnimAll(jobj->robj, frame);
    HSD_DObjReqAnimAll(jobj->u.dobj, frame);
    HSD_JObjReqAnimAll(jobj->child, frame);
    HSD_JObjReqAnimAll(jobj->next, frame);
}

void HSD_JObjAddAnimAll(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                        HSD_MatAnimJoint* matanimjoint,
                        HSD_ShapeAnimJoint* shapeanimjoint) {
    if (jobj == NULL) {
        return;
    }
    if (animjoint != NULL) {
        HSD_AObjRemove(jobj->aobj);
        jobj->aobj = HSD_AObjLoadDesc(animjoint->aobjdesc);
        HSD_RObjAddAnimAll(jobj->robj, animjoint->robj_anim);
    }
    if (jobj->u.dobj != NULL && matanimjoint != NULL) {
        HSD_DObjAddAnimAll(jobj->u.dobj, matanimjoint->matanim,
                           shapeanimjoint != NULL ?
                               shapeanimjoint->shapeanimdobj :
                               NULL);
    }
    HSD_JObjAddAnimAll(jobj->child,
                       animjoint != NULL ? animjoint->child : NULL,
                       matanimjoint != NULL ? matanimjoint->child : NULL,
                       shapeanimjoint != NULL ? shapeanimjoint->child : NULL);
    HSD_JObjAddAnimAll(jobj->next,
                       animjoint != NULL ? animjoint->next : NULL,
                       matanimjoint != NULL ? matanimjoint->next : NULL,
                       shapeanimjoint != NULL ? shapeanimjoint->next : NULL);
}

void HSD_JObjRemoveAll(HSD_JObj* jobj) {
    if (jobj == NULL) {
        return;
    }
    HSD_JObjRemoveAll(jobj->child);
    HSD_JObjRemoveAll(jobj->next);
    HSD_AObjRemove(jobj->aobj);
    HSD_RObjRemoveAll(jobj->robj);
    HSD_DObjRemoveAll(jobj->u.dobj);
    HSD_Free(jobj);
}
