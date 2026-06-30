#include "hsd/hsd_aobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_memory.h"

#include <string.h>

void HSD_AObjReqAnim(HSD_AObj* aobj, f32 frame) {
    if (aobj != NULL) {
        aobj->curr_frame = frame;
        aobj->flags |= AOBJ_FIRST_PLAY;
        aobj->flags &= ~AOBJ_NO_ANIM;
        HSD_FObjReqAnimAll(aobj->fobj, frame);
    }
}

void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* obj,
                           HSD_ObjUpdateFunc updateFunc) {
    if (aobj == NULL || (aobj->flags & AOBJ_NO_ANIM) != 0u) {
        return;
    }
    HSD_FObjInterpretAnimAll(aobj->fobj, obj, updateFunc, aobj->framerate);
    aobj->curr_frame += aobj->framerate;
}

HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc) {
    HSD_AObj* aobj;

    if (aobjdesc == NULL) {
        return NULL;
    }

    aobj = (HSD_AObj*)HSD_MemAlloc(sizeof(HSD_AObj));
    if (aobj == NULL) {
        return NULL;
    }
    memset(aobj, 0, sizeof(HSD_AObj));
    aobj->flags = aobjdesc->flags;
    aobj->end_frame = aobjdesc->end_frame;
    aobj->fobj = HSD_FObjLoadDesc(aobjdesc->fobjdesc);
    aobj->framerate = 1.0f;
    return aobj;
}

void HSD_AObjRemove(HSD_AObj* aobj) {
    if (aobj != NULL) {
        HSD_FObjRemoveAll(aobj->fobj);
        HSD_Free(aobj);
    }
}
