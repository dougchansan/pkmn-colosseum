/**
 * @file dobj.c
 * @brief HAL sysdolphin display objects, 0x80198F7C-0x8019970C.
 *
 * Reunites the Count-Sheep campaign's three matching DObj text owners with
 * their class info, small globals and compiler-owned string pools.
 * HAL source order and interfaces were checked against doldecomp/melee,
 * src/sysdolphin/baselib/dobj.c at
 * 598987cf347c125c7416517af18bf5c1dc3f15a1.
 *
 * Colosseum's newer HAL version also interprets the DObj's own AObj through
 * its update method and counts vertices through HSD_PObjCountVertices.
 * Preserve those behaviors and Colosseum's assertion line numbers.
 *
 * The GC/1.3.2 library flags use deferred inlining: HAL source order emits
 * the reverse function order present in the image. External ABI symbols
 * retain their address names. No separately owned literal stand-ins remain.
 *
 * The existing DObjLoad register-allocation exception is retained pending
 * a whole-unit compiler comparison of the natural HAL call/helper form;
 * see docs/RULE_EXCEPTIONS.md. This reunification introduces no new shaping.
 */
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"
#include "sysdolphin/baselib/debug.h"

extern void OSReport(const char* fmt, ...);
extern HSD_ClassInfo* fn_80193748(const char* class_name); /* hsdSearchClassInfo */
extern void* fn_80193828(HSD_ClassInfo* info);             /* hsdNew */
extern void fn_801ACDAC(HSD_PObj* pobj, s32* total_a, s32* total_b);
extern void HSD_PObjAddAnimAll(HSD_PObj* pobj, HSD_ShapeAnim* shapeanim);
extern void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, f32 startframe, void* flags);
extern void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, f32 startframe, void* flags);
extern HSD_ClassInfo lbl_8036C638; /* hsdClass */

void DObjInfoInit(void);
int DObjLoad(HSD_DObj* dobj, HSD_DObjDesc* desc);
inline HSD_DObj* HSD_DObjAlloc(void);

HSD_DObjInfo lbl_8036C7A0 = { DObjInfoInit }; /* hsdDObj */

/* MWCC allocates uninitialized globals in reverse definition order. */
HSD_DObj* lbl_8047B264;          /* current_dobj */
HSD_ClassInfo* lbl_8047B260;     /* default_class */

void HSD_DObjSetCurrent(HSD_DObj* dobj)
{
    lbl_8047B264 = dobj;
}

inline void HSD_DObjAddAnim(HSD_DObj* dobj, HSD_MatAnim* mat_anim,
                                   HSD_ShapeAnimDObj* sh_anim)
{
    if (dobj == NULL) {
        return;
    }

    HSD_PObjAddAnimAll(dobj->pobj, sh_anim != NULL ? sh_anim->shapeanim : NULL);
    HSD_MObjAddAnim(dobj->mobj, mat_anim);
}

void HSD_DObjAddAnimAll(HSD_DObj* dobj, void* matanim, void* shapeanimdobj)
{
    HSD_DObj* dp;
    HSD_MatAnim* ma;
    HSD_ShapeAnimDObj* sd;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj, ma = matanim, sd = shapeanimdobj; dp != NULL;
         dp = dp->next, ma = next_p(ma), sd = next_p(sd))
    {
        HSD_DObjAddAnim(dp, ma, sd);
    }
}

static inline void HSD_DObjReqAnimByFlags(HSD_DObj* dobj, f32 startframe,
                                          void* flags)
{
    if (dobj == NULL) {
        return;
    }

    HSD_PObjReqAnimAllByFlags(dobj->pobj, startframe, flags);
    HSD_MObjReqAnimByFlags(dobj->mobj, startframe, flags);
}

void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, f32 startframe, void* flags)
{
    HSD_DObj* dp;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj; dp != NULL; dp = dp->next) {
        HSD_DObjReqAnimByFlags(dp, startframe, flags);
    }
}

inline void HSD_DObjAnim(HSD_DObj* dobj)
{
    if (dobj == NULL) {
        return;
    }

    HSD_AObjInterpretAnim(dobj->aobj, dobj, HSD_DOBJ_METHOD(dobj)->update);
    HSD_PObjAnimAll(dobj->pobj);
    HSD_MObjAnim(dobj->mobj);
}

void HSD_DObjAnimAll(HSD_DObj* dobj)
{
    HSD_DObj* dp;

    if (dobj == NULL) {
        return;
    }

    for (dp = dobj; dp != NULL; dp = dp->next) {
        HSD_DObjAnim(dp);
    }
}

int DObjLoad(HSD_DObj* dobj_arg, HSD_DObjDesc* desc_arg)
{
    HSD_DObjDesc* desc;
    HSD_DObj* dobj;
    HSD_DObjDesc* subdesc;
    HSD_DObj* sub;
    HSD_ClassInfo* info;

    /* RULE-EXCEPTION(user-approved): parameter copies used only for register allocation - see docs/RULE_EXCEPTIONS.md */
    dobj = dobj_arg;
    desc = desc_arg;
    subdesc = desc->next;
    if (subdesc == NULL) {
        sub = NULL;
    } else {
        if (subdesc->class_name == NULL
            || (info = fn_80193748(subdesc->class_name)) == NULL)
        {
            info = lbl_8047B260 ? lbl_8047B260 : (HSD_ClassInfo*) &lbl_8036C7A0;
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(__FILE__, 0x214, "dobj");
            }
        } else {
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(__FILE__, 0x181, "dobj");
            }
        }
        /* RULE-EXCEPTION(user-approved): parameter re-copy used only for register allocation - see docs/RULE_EXCEPTIONS.md */
        dobj = dobj_arg;
        {
            void (**vtbl)(void) = (void (**)(void)) HSD_CLASS_METHOD(sub);
            ((int (*)(HSD_DObj*, HSD_DObjDesc*)) vtbl[0x40 / 4])(sub, subdesc);
        }
    }
    dobj->next = sub;
    dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
    dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);

    if (dobj->mobj != NULL) {
        u32 type = dobj->mobj->rendermode;
        type = type & 0x60000000;
        switch (type) {
        case 0x00000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x02;
            }
            break;
        case 0x40000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x08;
            }
            break;
        case 0x60000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x04;
            }
            break;
        default:
            OSReport("mobj has unexpected blending flags (0x%x).", dobj->mobj->rendermode);
            HSD_Panic(__FILE__, 0x13F, "");
            break;
        }
    }
    return 0;
}

HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    HSD_DObj* dobj;
    HSD_ClassInfo* info;

    if (desc == NULL) {
        return NULL;
    }

    if (desc->class_name == NULL ||
        (info = fn_80193748(desc->class_name)) == NULL)
    {
        dobj = HSD_DObjAlloc();
    } else {
        dobj = HSD_DOBJ(fn_80193828(info));
        if (dobj == NULL) {
            __assert(__FILE__, 385, "dobj");
        }
    }
    HSD_DOBJ_METHOD(dobj)->load(dobj, desc);

    return dobj;
}

void HSD_DObjRemoveAll(HSD_DObj* dobj)
{
    HSD_DObj* next;

    for (; dobj != NULL; dobj = next) {
        next = dobj->next;
        hsdDelete(dobj);
    }
}

inline HSD_DObj* HSD_DObjAlloc(void)
{
    HSD_DObj* dobj = fn_80193828(lbl_8047B260 != NULL
                                     ? lbl_8047B260
                                     : HSD_CLASS_INFO(&lbl_8036C7A0));
    if (dobj == NULL) {
        __assert(__FILE__, 532, "dobj");
    }
    return dobj;
}

inline void HSD_DObjResolveRefs(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    if (dobj == NULL || desc == NULL) {
        return;
    }
    HSD_PObjResolveRefsAll(dobj->pobj, desc->pobjdesc);
}

void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    for (; dobj != NULL && desc != NULL; dobj = dobj->next, desc = desc->next)
    {
        HSD_DObjResolveRefs(dobj, desc);
    }
}

void HSD_DObjCountVertices(HSD_DObj* dobj, s32* total_a, s32* total_b)
{
    s32 sum_a = 0;
    s32 sum_b = 0;

    for (; dobj != NULL; dobj = dobj->next) {
        s32 a;
        s32 b;

        fn_801ACDAC(dobj->pobj, &a, &b);
        sum_a += a;
        sum_b += b;
    }
    if (total_a != NULL) {
        *total_a = sum_a;
    }
    if (total_b != NULL) {
        *total_b = sum_b;
    }
}

void HSD_DObjDisp(HSD_DObj* dobj, f32 vmtx[3][4], f32 pmtx[3][4],
                  u32 rendermode)
{
    HSD_PObj* p;

    HSD_MObjSetCurrent(dobj->mobj);
    if ((rendermode & RENDER_SHADOW) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->setup(dobj->mobj, rendermode);
    }
    for (p = dobj->pobj; p != NULL; p = p->next) {
        HSD_POBJ_METHOD(p)->disp(p, vmtx, pmtx, rendermode);
    }
    if ((rendermode & RENDER_SHADOW) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->unset(dobj->mobj, rendermode);
    }
    HSD_MObjSetCurrent(NULL);
}

static void DObjRelease(HSD_Class* o)
{
    HSD_DObj* dobj = HSD_DOBJ(o);

    HSD_MObjRemove(dobj->mobj);
    HSD_PObjRemoveAll(dobj->pobj);
    HSD_AObjRemove(dobj->aobj);

    HSD_PARENT_INFO(&lbl_8036C7A0)->release(o);
}

static void DObjAmnesia(HSD_ClassInfo* info)
{
    if (info == lbl_8047B260) {
        lbl_8047B260 = NULL;
    }
    HSD_PARENT_INFO(&lbl_8036C7A0)->amnesia(info);
}

void DObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036C7A0), &lbl_8036C638,
                     "sysdolphin_base_library", "hsd_dobj", sizeof(HSD_DObjInfo),
                     sizeof(HSD_DObj));

    HSD_CLASS_INFO(&lbl_8036C7A0)->release = DObjRelease;
    HSD_CLASS_INFO(&lbl_8036C7A0)->amnesia = DObjAmnesia;
    HSD_DOBJ_INFO(&lbl_8036C7A0)->disp = HSD_DObjDisp;
    HSD_DOBJ_INFO(&lbl_8036C7A0)->load = DObjLoad;
}
