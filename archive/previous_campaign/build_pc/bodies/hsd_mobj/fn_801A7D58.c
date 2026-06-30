static HSD_TObj* PCPort_MObjCloneTObjChain(HSD_TObj* src)
{
    HSD_TObj* head = NULL;
    HSD_TObj* tail = NULL;

    while (src != NULL) {
        HSD_TObj* clone = HSD_TObjAlloc();
        HSD_Obj initialized_parent;

        if (clone == NULL) {
            HSD_TObjRemoveAll(head);
            return NULL;
        }

        initialized_parent = clone->parent;
        *clone = *src;
        clone->parent = initialized_parent;
        clone->next = NULL;
        clone->aobj = NULL;

        if (head == NULL) {
            head = clone;
        } else {
            tail->next = clone;
        }
        tail = clone;
        src = src->next;
    }

    return head;
}

static void PCPort_MObjCopyCleanup(HSD_MObj* mobj)
{
    if (mobj == NULL) {
        return;
    }
    HSD_TObjRemoveAll(mobj->tobj);
    HSD_Free(mobj->mat);
    HSD_Free(mobj->pe);
    mobj->tobj = NULL;
    mobj->mat = NULL;
    mobj->pe = NULL;
}

s32 fn_801A7D58(HSD_MObj* dst, HSD_MObj* src)
{
    if (dst == NULL || src == NULL || src->mat == NULL) {
        return -1;
    }

    dst->rendermode = src->rendermode;
    dst->tobj = PCPort_MObjCloneTObjChain(src->tobj);
    if (src->tobj != NULL && dst->tobj == NULL) {
        return -1;
    }

    dst->mat = HSD_MaterialAlloc();
    if (dst->mat == NULL) {
        PCPort_MObjCopyCleanup(dst);
        return -1;
    }
    memcpy(dst->mat, src->mat, sizeof(*dst->mat));
    dst->rendermode |= RENDER_TOON;

    dst->pe = NULL;
    if (src->pe != NULL) {
        dst->pe = (HSD_PEDesc*) HSD_MemAlloc((s32) sizeof(*dst->pe));
        if (dst->pe == NULL) {
            PCPort_MObjCopyCleanup(dst);
            return -1;
        }
        memcpy(dst->pe, src->pe, sizeof(*dst->pe));
    }

    dst->aobj = NULL;
    dst->tevdesc = NULL;
    dst->texp = NULL;
    return 0;
}
