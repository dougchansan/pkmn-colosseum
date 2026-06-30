void fn_801A6E24(HSD_MObj* mobj)
{
    HSD_TObj* tobj_top;
    HSD_TObj** shadow_link;
    HSD_TObj* shadow_tobj;
    HSD_TObj* toon_tobj;
    HSD_TObj* saved_toon_next;

    if (mobj == NULL || mobj->mat == NULL) {
        return;
    }

    fn_801B3884();

    tobj_top = mobj->tobj;
    shadow_link = NULL;
    shadow_tobj = NULL;
    toon_tobj = NULL;
    saved_toon_next = NULL;

    if ((mobj->rendermode & RENDER_SHADOW) && lbl_8047B2DC != 0) {
        shadow_tobj = (HSD_TObj*) lbl_8047B2DC;
        if (tobj_top == NULL) {
            tobj_top = shadow_tobj;
        } else {
            shadow_link = &tobj_top;
            while (*shadow_link != NULL) {
                shadow_link = &(*shadow_link)->next;
            }
            *shadow_link = shadow_tobj;
        }
    }

    if ((mobj->rendermode & RENDER_TOON) && lbl_8047B2D8 != 0) {
        toon_tobj = (HSD_TObj*) lbl_8047B2D8;
        if (toon_tobj->imagedesc != NULL) {
            saved_toon_next = toon_tobj->next;
            toon_tobj->next = tobj_top;
            tobj_top = toon_tobj;
        } else {
            toon_tobj = NULL;
        }
    }

    fn_801BBFE4(tobj_top);
    fn_801BDA58(tobj_top);

    mobj->texp = NULL;
    if (HSD_MOBJ_METHOD(mobj)->make_texp != NULL) {
        HSD_MOBJ_METHOD(mobj)->make_texp(mobj, tobj_top, &mobj->texp);
    }

    if (toon_tobj != NULL) {
        toon_tobj->next = saved_toon_next;
    }
    if (shadow_link != NULL) {
        *shadow_link = NULL;
    }
}
