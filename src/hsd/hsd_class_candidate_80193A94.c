#include "hsd/hsd_class.h"

extern void* fn_801A6928(s32 size);

HSD_Class* _hsdClassAlloc(HSD_ClassInfo* info)
{
    HSD_Class* result = (HSD_Class*) fn_801A6928((s32) info->head.obj_size);

    if (result != NULL) {
        info->head.nb_exist += 1;
        if (info->head.nb_exist > info->head.nb_peak) {
            info->head.nb_peak = *(u32 volatile*) &info->head.nb_exist;
        }
    }
    return result;
}
