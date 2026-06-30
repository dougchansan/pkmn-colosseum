void fn_80194400(HSD_CObj* cobj, HSD_RectS16* rect)
{
    if (cobj == NULL) {
        return;
    }

    cobj->viewport.xmin = (f32) rect->xmin;
    cobj->viewport.xmax = (f32) rect->xmax;
    cobj->viewport.ymin = (f32) rect->ymin;
    cobj->viewport.ymax = (f32) rect->ymax;
}
