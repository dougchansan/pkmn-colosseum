HSD_MObj* fn_801A7B24(void* desc)
{
    HSD_MObj* mobj;
    HSD_TObj* tobj_top;
    HSD_TObj** shadow_link;
    HSD_TObj* shadow_tobj;
    HSD_TObj* toon_tobj;
    HSD_TObj* saved_toon_next;

    if (desc == NULL) {
        return NULL;
    }
    if (*(void**) desc != NULL) {
        mobj = fn_80193748(*(void**) desc);
        if (mobj != NULL) {
            goto found;
        }
    }
    if (lbl_8047B2D0 != 0) {
        mobj = fn_80193828((HSD_ClassInfo*) lbl_8047B2D0);
    } else {
        mobj = fn_80193828((HSD_ClassInfo*) lbl_8036CB30);
    }
    if (mobj == NULL) {
        fn_80196E10(&lbl_8047DC18, 0x44a, &lbl_8047DC30);
    }
    goto call_load;

found:
    mobj = fn_80193828((HSD_ClassInfo*) mobj);
    if (mobj == NULL) {
        fn_80196E10(&lbl_8047DC18, 0x175, &lbl_8047DC30);
    }

call_load:
    if (mobj == NULL) {
        return NULL;
    }
    if (HSD_MOBJ_METHOD(mobj)->load != NULL) {
        HSD_MOBJ_METHOD(mobj)->load(mobj, (HSD_MObjDesc*) desc);
    }

    if (mobj->tevdesc != NULL) {
        fn_801B42C0(mobj->tevdesc);
        mobj->tevdesc = NULL;
    }
    mobj->texp = NULL;

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

    fn_801BC33C(tobj_top);
    if (HSD_MOBJ_METHOD(mobj)->make_texp != NULL) {
        HSD_MOBJ_METHOD(mobj)->make_texp(mobj, tobj_top, &mobj->texp);
    }
    fn_801B4300(&mobj->tevdesc, &mobj->texp);

    if (toon_tobj != NULL) {
        toon_tobj->next = saved_toon_next;
    }
    if (shadow_link != NULL) {
        *shadow_link = NULL;
    }
    return mobj;
}
