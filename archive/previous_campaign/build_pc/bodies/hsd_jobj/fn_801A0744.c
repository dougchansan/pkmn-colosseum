static HSD_JObj* PCPort_JObj0744Find(HSD_JObj* jobj,
                                     HSD_Joint* joint,
                                     HSD_Joint* target)
{
    HSD_JObj* found;

    while (jobj != NULL && joint != NULL) {
        if (joint == target) {
            return jobj;
        }
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            found = PCPort_JObj0744Find(jobj->child, joint->child, target);
            if (found != NULL) {
                return found;
            }
        }
        jobj = jobj->next;
        joint = joint->next;
    }
    return NULL;
}

static void PCPort_JObj0744Resolve(HSD_JObj* jobj,
                                   HSD_Joint* joint,
                                   HSD_JObj* root,
                                   HSD_Joint* root_joint)
{
    while (jobj != NULL && joint != NULL) {
        if (jobj->robj != NULL && joint->robjdesc != NULL) {
            HSD_RObjResolveRefsAll(jobj->robj, joint->robjdesc);
        }

        if (jobj->flags & JOBJ_INSTANCE) {
            if (joint->child != NULL) {
                jobj->child = PCPort_JObj0744Find(root, root_joint, joint->child);
            } else {
                jobj->child = NULL;
            }
        }

        if (union_type_dobj(jobj) && jobj->u.dobj != NULL &&
            joint->u.dobjdesc != NULL)
        {
            HSD_DObjResolveRefsAll(jobj->u.dobj, joint->u.dobjdesc);
        }

        if (!(jobj->flags & JOBJ_INSTANCE)) {
            PCPort_JObj0744Resolve(jobj->child, joint->child, root, root_joint);
        }

        jobj = jobj->next;
        joint = joint->next;
    }
}

void fn_801A0744(HSD_JObj* jobj, HSD_Joint* joint)
{
    PCPort_JObj0744Resolve(jobj, joint, jobj, joint);
}
