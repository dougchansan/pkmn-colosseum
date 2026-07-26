/**
 * @file hsd_mobj.c
 * @brief HSD MObj - material objects.
 *
 * Address range: 0x801A6CA4 - 0x801A8428
 * Ported from the sysdolphin baselib (doldecomp/melee src/sysdolphin/
 * baselib/mobj.c). Colosseum links a newer revision of sysdolphin: the
 * TEV builder honours the RENDER_DIFFUSE_* / RENDER_ALPHA_* channel
 * fields and the animation update function clamps to [0,1].
 */

#include "hsd/hsd_mobj.h"

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_tobj.h"

/* ========================================================================= */
/*  TExp expression builder (hsd_texp.c / hsd_tev.c)                         */
/* ========================================================================= */

#define HSD_TEXP_RAS ((HSD_TExp*) -2)
#define HSD_TEXP_TEX ((HSD_TExp*) -1)
#define HSD_TEXP_ZERO ((HSD_TExp*) 0)

#define HSD_TE_RGB 1
#define HSD_TE_A   5
#define HSD_TE_X   6
#define HSD_TE_0   7
#define HSD_TE_1   8

#define HSD_TE_U8  0
#define HSD_TE_F32 3

#define HSD_TE_TEV 1
#define HSD_TE_ALL 7

#define GX_COLOR1A1 5

/* HSD_TExpTev */
extern HSD_TExp* fn_801B707C(HSD_TExp** list);
/* HSD_TExpOrder */
extern void fn_801B5E40(HSD_TExp* exp, HSD_TObj* tobj, u32 chan);
/* HSD_TExpColorOp */
extern void fn_801B6E74(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
/* HSD_TExpAlphaOp */
extern void fn_801B6CD8(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
/* HSD_TExpColorIn */
extern void fn_801B64EC(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
/* HSD_TExpAlphaIn */
extern void fn_801B5F08(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
/* HSD_TExpFreeList */
extern void fn_801B7178(HSD_TExp* texp, u32 type, int flag);
/* HSD_TExpSetupTev */

extern HSD_TExp* HSD_TExpCnst(void* ptr, u32 comp, u32 type, HSD_TExp** list);
extern u32 HSD_TExpGetType(HSD_TExp* texp);
extern void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc,
                            HSD_TExp** list);
extern void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tevdesc);

#define HSD_TExpTev(list) fn_801B707C(list)
#define HSD_TExpOrder(exp, tobj, chan) fn_801B5E40(exp, tobj, chan)
#define HSD_TExpColorOp(exp, op, bias, scale, clamp) \
    fn_801B6E74(exp, op, bias, scale, clamp)
#define HSD_TExpAlphaOp(exp, op, bias, scale, clamp) \
    fn_801B6CD8(exp, op, bias, scale, clamp)
#define HSD_TExpColorIn(exp, sa, a, sb, b, sc, c, sd, d) \
    fn_801B64EC(exp, sa, a, sb, b, sc, c, sd, d)
#define HSD_TExpAlphaIn(exp, sa, a, sb, b, sc, c, sd, d) \
    fn_801B5F08(exp, sa, a, sb, b, sc, c, sd, d)
#define HSD_TExpFreeList(texp, type, flag) fn_801B7178(texp, type, flag)
#define HSD_TExpSetupTev(tevdesc, texp) fn_801B45A4(tevdesc, texp)

/* ========================================================================= */
/*  Render state (hsd_state.c) and TObj helpers (hsd_tobj.c)                 */
/* ========================================================================= */

/* HSD_StateInitTev */
extern void fn_801B3884(void);
/* HSD_SetMaterialShininess */
extern void fn_801B28B8(f32 shininess);
/* HSD_SetMaterialColor */
extern void fn_801B28C8(GXColor ambient, GXColor diffuse, GXColor specular,
                        f32 alpha);
/* HSD_SetupRenderModeWithCustomPE */
extern void fn_801B294C(u32 rendermode, HSD_PEDesc* pe);
/* HSD_TObjAddNext */
extern void fn_801BBE3C(HSD_TObj* tobj, HSD_TObj* next);


#define HSD_StateInitTev() fn_801B3884()
#define HSD_SetMaterialShininess(s) fn_801B28B8(s)
#define HSD_SetMaterialColor(a, d, s, al) fn_801B28C8(a, d, s, al)
#define HSD_SetupRenderModeWithCustomPE(r, pe) fn_801B294C(r, pe)
#define HSD_TObjAddNext(t, n) fn_801BBE3C(t, n)

/* ========================================================================= */
/*  Class system (hsd_class.c)                                               */
/* ========================================================================= */

/* hsdSearchClassInfo */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdIsDescendantOf */
extern BOOL fn_80193788(void* info, void* p);
/* hsdNew */
extern void* fn_80193828(HSD_ClassInfo* info);
/* hsdAllocMemPiece */
extern void* fn_80193B10(s32 size);

#define hsdSearchClassInfo(n) fn_80193748(n)
#define hsdIsDescendantOf(i, p) fn_80193788(i, p)
#define hsdNew(i) fn_80193828(i)
#define hsdAllocMemPiece(s) fn_80193B10(s)

extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, int val, u32 size);

/* ========================================================================= */
/*  Assertion strings                                                        */
/*                                                                           */
/*  The original TU is HAL's sysdolphin mobj.c: __FILE__ is "mobj.c" and     */
/*  the short expression strings live in .sdata2. Naming them by their       */
/*  final addresses reproduces the @sda21 references of the target.          */
/* ========================================================================= */

extern void __assert(const char* file, u32 line, const char* expr);

extern const char lbl_8047DC18[7]; /* "mobj.c" */
extern const char lbl_8047DC20[5]; /* "tobj" */
extern const char lbl_8047DC28[4]; /* "mat" */
extern const char lbl_8047DC30[5]; /* "mobj" */
extern const char lbl_8047DC38[5]; /* "list" */
extern const char lbl_80274E5C[];  /* "mobj->tevdesc" */
extern const char lbl_80274E6C[];  /* "hsdIsDescendantOf(info, &hsdMObj)" */

#define MOBJ_ASSERT(line, cond, expr) \
    ((cond) ? ((void) 0) : __assert(lbl_8047DC18, line, expr))

/* ========================================================================= */
/*  Globals                                                                  */
/* ========================================================================= */

/* hsdMObj */
extern HSD_MObjInfo lbl_8036CB30;
/* constant alpha fed to HSD_TExpCnst for the RENDER_ALPHA_VTX channel */
extern u8 lbl_80478C88[8];

/* default_class */
extern HSD_ClassInfo* lbl_8047B2D0;
/* tobj_toon */
extern HSD_TObj* lbl_8047B2D8;
/* tobj_shadows */
extern HSD_TObj* lbl_8047B2DC;

#define hsdMObj lbl_8036CB30
#define default_class lbl_8047B2D0
#define tobj_toon lbl_8047B2D8
#define tobj_shadows lbl_8047B2DC

/* ========================================================================= */
/*  Texture list                                                             */
/* ========================================================================= */

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6CA4_6DC4)

/* 0x801A6CA4 | 0x64 */
void HSD_MObjAddShadowTexture(HSD_TObj* tobj)
{
    HSD_TObj* cur;
    HSD_TObj** shadowList;
    HSD_TObj** list;

    MOBJ_ASSERT(1173, tobj, lbl_8047DC20);

    for (cur = tobj_shadows; cur != NULL; cur = cur->next) {
        if (cur == tobj) {
            return;
        }
    }

    shadowList = &tobj_shadows;
    if ((tobj && tobj) && tobj) {
    }
    list = shadowList;
    tobj->next = *list;
    tobj_shadows = tobj;
}

/* 0x801A6D08 | 0x54 */
void HSD_MObjRemove(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_CLASS_METHOD(mobj)->release((HSD_Class*) mobj);
        HSD_CLASS_METHOD(mobj)->destroy((HSD_Class*) mobj);
    }
}

/* 0x801A6D5C | 0x44 */
void HSD_MObjAddTObjNext(HSD_MObj* mobj, HSD_TObj* tobj, HSD_TObj* next)
{
    if (mobj == NULL || tobj == NULL || next == NULL) {
        return;
    }
    HSD_TObjAddNext(tobj, next);
}

/* 0x801A6DA0 | 0x24 */
void fn_801A6DA0(HSD_MObj* mobj, HSD_TObj* tobj)
{
    if (mobj == NULL || tobj == NULL) {
        return;
    }
    tobj->next = mobj->tobj;
    mobj->tobj = tobj;
}

#endif

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6DC4_6DDC)

/* 0x801A6DC4 | 0x18 */
HSD_TObj* HSD_MObjGetTObj(HSD_MObj* mobj)
{
    if (mobj == NULL) {
        return NULL;
    }
    return mobj->tobj;
}

#endif

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6DDC_6E00)

/* 0x801A6DDC | 0x24 */
void HSD_MObjSetAlpha(HSD_MObj* mobj, f32 alpha)
{
    if (mobj == NULL || mobj->mat == NULL) {
        return;
    }
    mobj->mat->alpha = alpha;
}

#endif

/* ========================================================================= */
/*  Setup                                                                    */
/* ========================================================================= */

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6E00_6E24)

/* 0x801A6E00 | 0x24 */
void HSD_MObjUnset(HSD_MObj* mobj, u32 rendermode)
{
    HSD_TObjSetup(NULL);
}


#endif

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6E24_6F78)

/* 0x801A6E24 | 0x154 */
void HSD_MObjSetup(HSD_MObj* mobj, u32 rendermode)
{
    HSD_TObj* tobj;
    HSD_TObj** tail;

    HSD_StateInitTev();
    rendermode = mobj->rendermode;
    HSD_SetMaterialColor(mobj->mat->ambient, mobj->mat->diffuse,
                         mobj->mat->specular, mobj->mat->alpha);
    if (rendermode & RENDER_SPECULAR) {
        HSD_SetMaterialShininess(mobj->mat->shininess);
    }

    tobj = mobj->tobj;
    tail = NULL;

    if ((rendermode & RENDER_SHADOW) && tobj_shadows != NULL) {
        tail = &tobj;
        while (*tail != NULL) {
            tail = &(*tail)->next;
        }
        *tail = tobj_shadows;
    }
    if ((rendermode & RENDER_TOON) && tobj_toon != NULL &&
        tobj_toon->imagedesc != NULL)
    {
        tobj_toon->next = tobj;
        tobj = tobj_toon;
    }
    HSD_TObjSetup(tobj);
    HSD_TObjSetupTextureCoordGen(tobj);
    HSD_MOBJ_METHOD(mobj)->setup_tev(mobj, tobj, rendermode);
    HSD_SetupRenderModeWithCustomPE(rendermode, mobj->pe);
    if (tail != NULL) {
        *tail = NULL;
    }
}

#endif

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6F78_6FF0)

/* 0x801A6F78 | 0x78 */
void MObjSetupTev(HSD_MObj* mobj, HSD_TObj* tobj, u32 rendermode)
{
    MOBJ_ASSERT(798, mobj->tevdesc, lbl_80274E5C);
    HSD_TExpSetupTev(mobj->tevdesc, mobj->texp);
    HSD_TObjSetupVolatileTev(tobj, rendermode);
}

#endif

/* ========================================================================= */
/*  TEV compilation                                                          */
/* ========================================================================= */

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_6FF0_7E3C)

/* 0x801A6FF0 | 0x138 */
void HSD_MObjCompileTev(HSD_MObj* mobj)
{
    HSD_TObj* tobj;
    HSD_TObj** tail;
    HSD_TExp* texp;

    tail = NULL;
    if (mobj != NULL) {
        if (mobj->tevdesc != NULL) {
            HSD_TExpFreeTevDesc(mobj->tevdesc);
            mobj->tevdesc = NULL;
        }
        if (mobj->texp != NULL) {
            HSD_TExpFreeList(mobj->texp, HSD_TE_ALL, 1);
            mobj->texp = NULL;
        }
        tobj = mobj->tobj;
        if (mobj->rendermode & RENDER_SHADOW) {
            if (tobj_shadows != NULL) {
                tail = &tobj;
                while (*tail != NULL) {
                    tail = &(*tail)->next;
                }
                *tail = tobj_shadows;
            }
        }
        if (mobj->rendermode & RENDER_TOON) {
            if (tobj_toon != NULL && tobj_toon->imagedesc != NULL) {
                tobj_toon->next = tobj;
                tobj = tobj_toon;
            }
        }
        HSD_TObjAssignResources(tobj);
        texp = HSD_MOBJ_METHOD(mobj)->make_texp(mobj, tobj, &mobj->texp);
        HSD_TExpCompile(texp, &mobj->tevdesc, &mobj->texp);
        if (tail != NULL) {
            *tail = NULL;
        }
    }
}

/* 0x801A7128 | 0x9FC */
HSD_TExp* MObjMakeTExp(HSD_MObj* mobj, HSD_TObj* tobj_top, HSD_TExp** list)
{
    HSD_TExp* diff;
    HSD_TExp* spec;
    HSD_TExp* ext;
    HSD_TExp* alpha;
    HSD_TExp* exp;
    HSD_TExp* cnst;
    HSD_TObj* tobj;
    HSD_TObj* toon = NULL;
    u32 done = 0;
    u32 diffuse_bits;
    u32 alpha_bits;

    MOBJ_ASSERT(395, list, lbl_8047DC38);

    *list = NULL;
    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if (tobj_coord(tobj) == TEX_COORD_TOON) {
            toon = tobj;
        }
    }

    diffuse_bits = mobj->rendermode & RENDER_DIFFUSE_BITS;
    if (diffuse_bits == RENDER_DIFFUSE_MAT0) {
        diffuse_bits = RENDER_DIFFUSE_MAT;
    }
    alpha_bits = mobj->rendermode & RENDER_ALPHA_BITS;
    if (alpha_bits == RENDER_ALPHA_COMPAT) {
        alpha_bits = diffuse_bits << RENDER_ALPHA_SHIFT;
    }

    exp = HSD_TExpTev(list);

    if (mobj->rendermode & RENDER_DIFFUSE) {
        if (diffuse_bits == RENDER_DIFFUSE_VTX) {
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_1,
                            HSD_TEXP_ZERO);
        } else {
            cnst = HSD_TExpCnst(&mobj->mat->diffuse, HSD_TE_RGB, HSD_TE_U8,
                                list);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB, cnst);
        }
        if (alpha_bits == RENDER_ALPHA_VTX) {
            cnst = HSD_TExpCnst(lbl_80478C88, HSD_TE_X, HSD_TE_U8, list);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X,
                            cnst);
        } else {
            cnst = HSD_TExpCnst(&mobj->mat->alpha, HSD_TE_X, HSD_TE_F32,
                                list);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X,
                            cnst);
        }
    } else {
        switch (diffuse_bits) {
        case RENDER_DIFFUSE_MAT:
            cnst = HSD_TExpCnst(&mobj->mat->diffuse, HSD_TE_RGB, HSD_TE_U8,
                                list);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB, cnst);
            break;
        case RENDER_DIFFUSE_VTX:
            HSD_TExpOrder(exp, toon, GX_COLOR0A0);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB,
                            toon != NULL ? HSD_TEXP_TEX : HSD_TEXP_RAS);
            break;
        default:
            cnst = HSD_TExpCnst(&mobj->mat->diffuse, HSD_TE_RGB, HSD_TE_U8,
                                list);
            HSD_TExpOrder(exp, toon, GX_COLOR0A0);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_RGB,
                            toon != NULL ? HSD_TEXP_TEX : HSD_TEXP_RAS,
                            HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB, cnst);
            break;
        }
        switch (alpha_bits) {
        case RENDER_ALPHA_MAT:
            cnst = HSD_TExpCnst(&mobj->mat->alpha, HSD_TE_X, HSD_TE_F32,
                                list);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X,
                            cnst);
            break;
        case RENDER_ALPHA_VTX:
            HSD_TExpOrder(exp, toon, GX_COLOR0A0);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A,
                            HSD_TEXP_RAS);
            break;
        default:
            cnst = HSD_TExpCnst(&mobj->mat->alpha, HSD_TE_X, HSD_TE_F32,
                                list);
            HSD_TExpOrder(exp, toon, GX_COLOR0A0);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A,
                            HSD_TEXP_RAS, HSD_TE_X, cnst, HSD_TE_0,
                            HSD_TEXP_ZERO);
            break;
        }
    }

    diff = exp;
    alpha = exp;

    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if ((tobj->flags & (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT)) &&
            tobj->id != GX_TEXMAP_NULL)
        {
            HSD_TOBJ_METHOD(tobj)->make_texp(
                tobj, (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT), done,
                &diff, &alpha, list);
        }
    }
    done |= (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT);

    if (mobj->rendermode & RENDER_DIFFUSE) {
        if (alpha_bits & RENDER_ALPHA_VTX) {
            exp = HSD_TExpTev(list);
            HSD_TExpOrder(exp, NULL, GX_COLOR1A1);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB, diff);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_A, alpha, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_A, HSD_TEXP_RAS, HSD_TE_0, HSD_TEXP_ZERO);
            diff = exp;
            alpha = exp;
        }
        exp = HSD_TExpTev(list);
        if (toon != NULL) {
            HSD_TExpOrder(exp, toon, GX_COLOR0A0);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, diff,
                            HSD_TE_RGB, HSD_TEXP_TEX, HSD_TE_0,
                            HSD_TEXP_ZERO);
        } else {
            HSD_TExpOrder(exp, NULL, GX_COLOR0A0);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, diff,
                            HSD_TE_RGB, HSD_TEXP_RAS, HSD_TE_0,
                            HSD_TEXP_ZERO);
        }
        diff = exp;
        if (alpha_bits & RENDER_ALPHA_VTX) {
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A, alpha,
                            HSD_TE_A, HSD_TEXP_RAS, HSD_TE_0, HSD_TEXP_ZERO);
        } else {
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_A, alpha, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_A, HSD_TEXP_RAS, HSD_TE_0, HSD_TEXP_ZERO);
        }
        alpha = exp;
    }

    if (mobj->rendermode & RENDER_SPECULAR) {
        cnst = HSD_TExpCnst(&mobj->mat->specular, HSD_TE_RGB, HSD_TE_U8,
                            list);
        exp = HSD_TExpTev(list);
        HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                        GX_ENABLE);
        HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                        HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, cnst);
        spec = exp;

        for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
            if ((tobj->flags & TEX_LIGHTMAP_SPECULAR) &&
                tobj->id != GX_TEXMAP_NULL)
            {
                HSD_TOBJ_METHOD(tobj)->make_texp(tobj, TEX_LIGHTMAP_SPECULAR,
                                                 done, &spec, &alpha, list);
            }
        }
        done |= TEX_LIGHTMAP_SPECULAR;

        exp = HSD_TExpTev(list);
        HSD_TExpOrder(exp, NULL, GX_COLOR1A1);
        HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                        GX_ENABLE);
        HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, spec,
                        HSD_TE_RGB, HSD_TEXP_RAS, HSD_TE_0, HSD_TEXP_ZERO);
        spec = exp;

        exp = HSD_TExpTev(list);
        HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                        GX_ENABLE);
        HSD_TExpColorIn(exp, HSD_TE_RGB, spec, HSD_TE_0, HSD_TEXP_ZERO,
                        HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, diff);
        diff = exp;
    }

    ext = diff;

    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if ((tobj->flags & TEX_LIGHTMAP_EXT) && tobj->id != GX_TEXMAP_NULL) {
            HSD_TOBJ_METHOD(tobj)->make_texp(tobj, TEX_LIGHTMAP_EXT, done,
                                             &ext, &alpha, list);
        }
    }

    if (ext != alpha || HSD_TExpGetType(ext) != HSD_TE_TEV ||
        HSD_TExpGetType(alpha) != HSD_TE_TEV)
    {
        exp = HSD_TExpTev(list);
        HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                        GX_ENABLE);
        HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                        HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, ext);
        HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                        GX_ENABLE);
        HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                        HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A, alpha);
        return exp;
    }

    return ext;
}

/* ========================================================================= */
/*  Loading                                                                  */
/* ========================================================================= */

static HSD_MObj* HSD_MObjAlloc(void)
{
    HSD_MObj* mobj =
        hsdNew(default_class != NULL ? default_class : &hsdMObj.parent);
    MOBJ_ASSERT(1098, mobj, lbl_8047DC30);
    return mobj;
}

/* 0x801A7B24 | 0x1D8 */
HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc* mobjdesc)
{
    if (mobjdesc != NULL) {
        HSD_MObj* mobj;
        HSD_ClassInfo* info;

        if (mobjdesc->class_name == NULL ||
            (info = hsdSearchClassInfo(mobjdesc->class_name)) == NULL)
        {
            mobj = HSD_MObjAlloc();
        } else {
            mobj = hsdNew(info);
            MOBJ_ASSERT(373, mobj, lbl_8047DC30);
        }

        HSD_MOBJ_METHOD(mobj)->load(mobj, mobjdesc);
        HSD_MObjCompileTev(mobj);

        return mobj;
    } else {
        return NULL;
    }
}

/* 0x801A7CFC | 0x5C */
void HSD_MObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info != NULL) {
        MOBJ_ASSERT(334, hsdIsDescendantOf(info, &hsdMObj), lbl_80274E6C);
    }
    default_class = info;
}

static HSD_Material* HSD_MaterialAlloc(void)
{
    HSD_Material* mat = hsdAllocMemPiece(sizeof(HSD_Material));
    MOBJ_ASSERT(1126, mat, lbl_8047DC28);
    memset(mat, 0, sizeof(HSD_Material));
    mat->alpha = 1.0F;
    return mat;
}

/* 0x801A7D58 | 0xE4 */
int MObjLoad(HSD_MObj* mobj, HSD_MObjDesc* desc)
{
    mobj->rendermode = desc->rendermode;
    mobj->tobj = HSD_TObjLoadDesc(desc->texdesc);
    mobj->mat = HSD_MaterialAlloc();
    memcpy(mobj->mat, desc->mat, sizeof(HSD_Material));
    mobj->rendermode |= RENDER_TOON;
    if (desc->pedesc != NULL) {
        mobj->pe = hsdAllocMemPiece(sizeof(HSD_PEDesc));
        memcpy(mobj->pe, desc->pedesc, sizeof(HSD_PEDesc));
    }
    mobj->aobj = NULL;
    return 0;
}

#endif

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_7E3C_7E84)

/* 0x801A7E3C | 0x48 */
void HSD_MObjAnim(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_AObjInterpretAnim(mobj->aobj, mobj, HSD_MOBJ_METHOD(mobj)->update);
        HSD_TObjAnimAll(mobj->tobj);
    }
}

#endif

#if !defined(PR409_HSD_MOBJ_SPLIT) || defined(PR409_HSD_MOBJ_7E84_8428)

static f32 MObjClampAnim(f32 value)
{
    if (value <= 0.0F) {
        return 0.0F;
    }
    if (value >= 1.0F) {
        return 1.0F;
    }
    return value;
}

/* 0x801A7E84 | 0x4D0 */
void MObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_MObj* mobj = obj;

    if (mobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_M_AMBIENT_R:
        if (mobj->mat != NULL) {
            mobj->mat->ambient.r = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_AMBIENT_G:
        if (mobj->mat != NULL) {
            mobj->mat->ambient.g = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_AMBIENT_B:
        if (mobj->mat != NULL) {
            mobj->mat->ambient.b = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_R:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.r = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_G:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.g = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_B:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.b = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_ALPHA:
        if (mobj->mat != NULL) {
            mobj->mat->alpha = MObjClampAnim(1.0F - val->fv);
        }
        break;
    case HSD_A_M_SPECULAR_R:
        if (mobj->mat != NULL) {
            mobj->mat->specular.r = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_SPECULAR_G:
        if (mobj->mat != NULL) {
            mobj->mat->specular.g = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_SPECULAR_B:
        if (mobj->mat != NULL) {
            mobj->mat->specular.b = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_PE_REF0:
        if (mobj->pe != NULL) {
            mobj->pe->ref0 = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_PE_REF1:
        if (mobj->pe != NULL) {
            mobj->pe->ref1 = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    case HSD_A_M_PE_DSTALPHA:
        if (mobj->pe != NULL) {
            mobj->pe->dst_alpha = (u8) (255.0F * MObjClampAnim(val->fv));
        }
        break;
    }
}

/* 0x801A8354 | 0x68 */
void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, f32 startframe, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    if (flags & MOBJ_ANIM) {
        HSD_AObjReqAnim(mobj->aobj, startframe);
    }
    HSD_TObjReqAnimAllByFlags(mobj->tobj, startframe, flags);
}

/* 0x801A83BC | 0x6C */
void HSD_MObjAddAnim(HSD_MObj* mobj, HSD_MatAnim* matanim)
{
    if (mobj == NULL) {
        return;
    }
    if (matanim == NULL) {
        return;
    }
    if (mobj->aobj != NULL) {
        HSD_AObjRemove(mobj->aobj);
    }
    mobj->aobj = HSD_AObjLoadDesc(matanim->aobjdesc);
    HSD_TObjAddAnimAll(mobj->tobj, matanim->texanim);
}

#endif
