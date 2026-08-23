#include "hsd/hsd_aobj.h"
#include "hsd/hsd_tobj.h"

static inline void HSD_TObjReqAnimByFlags(HSD_TObj* tobj, f32 startframe,
                                          u32 flags)
{
    if (tobj != NULL) {
        if (flags & TOBJ_ANIM) {
            HSD_AObjReqAnim(tobj->aobj, startframe);
        }
    }
}

void fn_801BEE68(HSD_TObj* tobj, f32 startframe, u32 flags)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjReqAnimByFlags(tp, startframe, flags);
        }
    }
}
