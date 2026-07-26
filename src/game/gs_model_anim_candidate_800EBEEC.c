#include "src/game/gs_model_anim.c"

extern void fn_8019FE8C(HSD_JObj*, u32);

void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(HSD_JObj* jobj)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    fn_8019FE8C(jobj, JOBJ_USE_QUATERNION);
    if (jobj->flags & JOBJ_INSTANCE) {
        return;
    }
    for (child = jobj->child; child != NULL; child = child->next) {
        _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(child);
    }
}
