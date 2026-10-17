#include "src/game/gs_model_anim.c"

extern void HSD_JObjSetFlags(HSD_JObj*, u32);

void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(HSD_JObj* jobj)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    HSD_JObjSetFlags(jobj, JOBJ_USE_QUATERNION);
    if (jobj->flags & JOBJ_INSTANCE) {
        return;
    }
    for (child = jobj->child; child != NULL; child = child->next) {
        _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(child);
    }
}
