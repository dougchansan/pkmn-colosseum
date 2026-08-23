#include "hsd/hsd_class.h"

void hsdDelete_801A0CE8(void* object)
{
    void (*new_var)(HSD_Class* c);
    HSD_Class* o;

    if ((o = (HSD_Class*) object) != ((void*) 0)) {
        if (!o) {
            o = (HSD_Class*) object;
        }
        new_var = ((HSD_Class*) o)->class_info->release;
        new_var(o);
        ((HSD_Class*) o)->class_info->destroy(o);
    }
}
