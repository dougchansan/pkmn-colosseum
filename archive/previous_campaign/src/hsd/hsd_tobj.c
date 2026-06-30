/**
 * @file hsd_tobj.c
 * @brief HSD TObj - Texture object implementation.
 *
 * Colosseum address: 0x801BBAC8 (HSD_TObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/tobj.c
 */

#include "hsd/hsd_tobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"

static void TObjInfoInit(void);

#ifdef PCPORT
void PCPort_TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                         HSD_TExp** c, HSD_TExp** a, HSD_TExp** list);
#endif

HSD_TObjInfo hsdTObj = { TObjInfoInit };

static HSD_TObjInfo* default_class = NULL;

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_TObjRemoveAnimAll(HSD_TObj* tobj)
{
    HSD_TObj* t;
    for (t = tobj; t != NULL; t = t->next) {
        HSD_AObjRemove(t->aobj);
        t->aobj = NULL;
    }
}

void HSD_TObjAddAnim(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    if (tobj == NULL || texanim == NULL) {
        return;
    }
    if (tobj->aobj != NULL) {
        HSD_AObjRemove(tobj->aobj);
    }
    tobj->aobj = HSD_AObjLoadDesc(texanim->aobjdesc);
    tobj->imagetbl = texanim->imagetbl;
    tobj->tluttbl = texanim->tluttbl;
}

void HSD_TObjAddAnimAll(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    HSD_TObj* t;
    HSD_TexAnim* a;

    t = tobj;
    a = texanim;
    while (t != NULL && a != NULL) {
        HSD_TObjAddAnim(t, a);
        t = t->next;
        a = a->next;
    }
}

void HSD_TObjReqAnim(HSD_TObj* tobj, f32 startframe)
{
    if (tobj != NULL) {
        HSD_AObjReqAnim(tobj->aobj, startframe);
    }
}

void HSD_TObjReqAnimAll(HSD_TObj* tobj, f32 startframe)
{
    HSD_TObj* t;
    for (t = tobj; t != NULL; t = t->next) {
        HSD_TObjReqAnim(t, startframe);
    }
}

void HSD_TObjAnim(HSD_TObj* tobj)
{
    /* Interpret AObj -> update texture transform/image */
}

void HSD_TObjAnimAll(HSD_TObj* tobj)
{
    HSD_TObj* t;
    for (t = tobj; t != NULL; t = t->next) {
        HSD_TObjAnim(t);
    }
}

/* ========================================================================= */
/*  Accessors                                                                */
/* ========================================================================= */

HSD_TObj* HSD_TObjGetNext(HSD_TObj* tobj)
{
    if (tobj == NULL) {
        return NULL;
    }
    return tobj->next;
}

/* ========================================================================= */
/*  Load                                                                     */
/* ========================================================================= */

static int TObjLoad(HSD_TObj* tobj, HSD_TObjDesc* desc)
{
    tobj->id = desc->id;
    tobj->src = desc->src;
    tobj->rotate_x = desc->rotate_x;
    tobj->rotate_y = desc->rotate_y;
    tobj->rotate_z = desc->rotate_z;
    tobj->scale_x = desc->scale_x;
    tobj->scale_y = desc->scale_y;
    tobj->scale_z = desc->scale_z;
    tobj->translate_x = desc->translate_x;
    tobj->translate_y = desc->translate_y;
    tobj->translate_z = desc->translate_z;
    tobj->wrap_s = desc->wrap_s;
    tobj->wrap_t = desc->wrap_t;
    tobj->repeat_s = desc->repeat_s;
    tobj->repeat_t = desc->repeat_t;
    tobj->flags = desc->blend_flags;
    tobj->blending = desc->blending;
    tobj->magFilt = desc->magFilt;
    tobj->imagedesc = desc->imagedesc;
    return 0;
}

HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc* td)
{
    HSD_TObj* tobj = NULL;
    HSD_TObj* first = NULL;
    HSD_TObj* prev = NULL;
    HSD_TObjDesc* d;

    for (d = td; d != NULL; d = d->next) {
        tobj = HSD_TObjAlloc();
        HSD_TOBJ_METHOD(tobj)->load(tobj, d);
        if (prev != NULL) {
            prev->next = tobj;
        } else {
            first = tobj;
        }
        prev = tobj;
    }
    return first;
}

/* ========================================================================= */
/*  Remove / Alloc                                                           */
/* ========================================================================= */

void HSD_TObjRemove(HSD_TObj* tobj)
{
    if (tobj != NULL) {
        HSD_OBJECT_METHOD(tobj)->release((HSD_Class*) tobj);
        HSD_OBJECT_METHOD(tobj)->destroy((HSD_Class*) tobj);
    }
}

void HSD_TObjRemoveAll(HSD_TObj* tobj)
{
    HSD_TObj* next;
    while (tobj != NULL) {
        next = tobj->next;
        HSD_TObjRemove(tobj);
        tobj = next;
    }
}

HSD_TObj* HSD_TObjAlloc(void)
{
    HSD_TObj* tobj;
    tobj = (HSD_TObj*) hsdNew(
        default_class ? (HSD_ClassInfo*) default_class
                      : &hsdTObj.parent.parent);
    HSD_ASSERT(0, tobj);
    return tobj;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

static void TObjRelease(HSD_Class* o)
{
    HSD_TObj* tobj = (HSD_TObj*) o;
    HSD_AObjRemove(tobj->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdTObj)->release(o);
}

static void TObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&hsdTObj)->amnesia(info);
}

static void TObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdTObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_tobj",
                     sizeof(HSD_TObjInfo), sizeof(HSD_TObj));
    HSD_CLASS_INFO(&hsdTObj)->release = TObjRelease;
    HSD_CLASS_INFO(&hsdTObj)->amnesia = TObjAmnesia;
    HSD_TOBJ_INFO(&hsdTObj)->load = TObjLoad;
#ifdef PCPORT
    HSD_TOBJ_INFO(&hsdTObj)->make_texp = PCPort_TObjMakeTExp;
#endif
}
