/**
 * @file mobj.c
 * @brief HAL mobj.c: HSD material objects (MObj), 0x801A6A34 - 0x801A8478.
 *
 * The whole translation unit, built with the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly) and no local pragmas. It owns:
 *   .text   0x801A6A34 - 0x801A8478 (memory.c ends at 0x801A6A34, mtx.c
 *           starts at 0x801A8478 with HSD_MtxInitAllocData)
 *   .rodata 0x80274E38 - 0x80274E90 ("sysdolphin_base_library", "hsd_mobj",
 *           "mobj->tevdesc", "hsdIsDescendantOf(info, &hsdMObj)"; objalloc.c's
 *           strings follow)
 *   .data   0x8036CB30 - 0x8036CBBC (hsdMObj, then MObjUpdateFunc's switch
 *           table; mtx.c's HSD_identityMtx is next, at 0x8036CBC0)
 *   .sdata  0x80478C88 - 0x80478C90 (MObjMakeTExp's constant alpha 0xFF)
 *   .sbss   0x8047B2D0 - 0x8047B2E0 (default_class, current_mobj, tobj_toon,
 *           tobj_shadows)
 *   .sdata2 0x8047DC18 - 0x8047DC48 (__FILE__ "mobj.c", the short assert
 *           strings and the float pool; mtx.c's starts at 0x8047DC48)
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/mobj.c). The functions are written in HAL's order;
 * deferred inlining emits them in reverse, which is retail's address order,
 * and the literal pools come out in first-use order of that reversed code
 * generation. Melee's HSD_MObjAlloc and HSD_MaterialAlloc are kept for that
 * reason: nothing calls them out of line (HSD_MObjLoadDesc and MObjLoad carry
 * their expansions, asserts 1098 "mobj" and 1126 "mat"), but compiled as
 * functions between HSD_MObjRemove and HSD_MObjAddShadowTexture they put
 * "mat", 1.0f and "mobj" ahead of MObjMakeTExp's "list", as in retail. The
 * linker strips them, as in Melee.
 *
 * Colosseum's HAL version differs from Melee's in these places (all read
 * from retail):
 *  - MObjInfoInit also installs MObjUpdateFunc (update) and HSD_MObjUnset
 *    (unset);
 *  - MObjMakeTExp honours the RENDER_DIFFUSE_* / RENDER_ALPHA_* channel
 *    fields (the vertex-alpha channel feeds a constant 0xFF alpha);
 *  - MObjUpdateFunc clamps every colour and alpha value to [0, 1] with the
 *    same fcmpo sequence in all thirteen cases (recovered as the static
 *    inline MObjClamp01, policy "repeated expansion"; it leaves no symbol)
 *    and checks mat / pe for NULL;
 *  - fn_801A6DA0 (push a TObj onto the material) and
 *    HSD_MObjAddTObjNext are new; HSD_MObjSetToonTextureImage is gone;
 *  - assert line numbers are those of Colosseum's longer file.
 *
 * The symbols keep their address names: lbl_8036CB30 is hsdMObj,
 * lbl_8036C638 hsdClass, lbl_8047B2D0 default_class, lbl_8047B2D4
 * current_mobj, lbl_8047B2D8 tobj_toon, lbl_8047B2DC tobj_shadows.
 */

#include "hsd/hsd_mobj.h"

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_debug.h"

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
extern HSD_TExp* HSD_TExpTev(HSD_TExp** list);
/* HSD_TExpOrder */
extern void HSD_TExpOrder(HSD_TExp* exp, HSD_TObj* tobj, u32 chan);
/* HSD_TExpColorOp */
extern void HSD_TExpColorOp(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
/* HSD_TExpAlphaOp */
extern void HSD_TExpAlphaOp(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
/* HSD_TExpColorIn */
extern void HSD_TExpColorIn(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
/* HSD_TExpAlphaIn */
extern void HSD_TExpAlphaIn(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
/* HSD_TExpFreeList */
extern void HSD_TExpFreeList(HSD_TExp* texp, u32 type, int flag);
/* HSD_TExpSetupTev */

extern HSD_TExp* HSD_TExpCnst(void* ptr, u32 comp, u32 type, HSD_TExp** list);
extern s32 HSD_TExpGetType(HSD_TExp* texp);
extern void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc,
                            HSD_TExp** list);
extern void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tevdesc);

#define HSD_TExpFreeList(texp, type, flag) HSD_TExpFreeList(texp, type, flag)
/* HSD_TExpSetupTev */
extern void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp);

/* ========================================================================= */
/*  Render state (hsd_state.c) and TObj helpers (hsd_tobj.c)                 */
/* ========================================================================= */

/* HSD_StateInitTev */
extern void HSD_StateInitTev(void);
/* HSD_SetMaterialShininess */
extern void HSD_SetMaterialShininess(f32 shininess);
/* HSD_SetMaterialColor */
extern void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular,
                        f32 alpha);
/* HSD_SetupRenderModeWithCustomPE */
extern void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc* pe);
/* HSD_TObjAddNext */
extern void fn_801BBE3C(HSD_TObj* tobj, HSD_TObj* next);

#define HSD_StateInitTev() HSD_StateInitTev()
#define HSD_SetMaterialShininess(s) HSD_SetMaterialShininess(s)
#define HSD_TObjAddNext(t, n) fn_801BBE3C(t, n)

/* ========================================================================= */
/*  Class system (hsd_class.c)                                               */
/* ========================================================================= */

/* hsdSearchClassInfo */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdIsDescendantOf */
extern BOOL hsdIsDescendantOf(void* info, void* p);
/* hsdNew */
extern void* fn_80193828(HSD_ClassInfo* info);
/* hsdAllocMemPiece */
extern void* fn_80193B10(s32 size);

#define hsdSearchClassInfo(n) fn_80193748(n)
#define hsdIsDescendantOf(i, p) hsdIsDescendantOf(i, p)
#define hsdNew(i) fn_80193828(i)
#define hsdAllocMemPiece(s) fn_80193B10(s)

extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, int val, u32 size);

extern void fn_80193AF0(void* mem, s32 size); /* hsdFreeMemPiece */

extern HSD_ClassInfo lbl_8036C638; /* hsdClass */

void MObjInfoInit(void);
HSD_MObj* HSD_MObjAlloc(void);
HSD_Material* HSD_MaterialAlloc(void);

/* hsdMObj */
HSD_MObjInfo lbl_8036CB30 = { MObjInfoInit };
#define hsdMObj lbl_8036CB30

/* default_class, current_mobj, tobj_toon, tobj_shadows */
HSD_TObj* lbl_8047B2DC;
HSD_TObj* lbl_8047B2D8;
HSD_MObj* lbl_8047B2D4;
HSD_ClassInfo* lbl_8047B2D0;
#define default_class lbl_8047B2D0
#define current_mobj lbl_8047B2D4
#define tobj_toon lbl_8047B2D8
#define tobj_shadows lbl_8047B2DC

/* 0x801A8470 | 0x8 */
void HSD_MObjSetCurrent(HSD_MObj* mobj)
{
    current_mobj = mobj;
}

/* 0x801A8458 | 0x18 */
u32 HSD_MObjGetFlags(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        return mobj->rendermode;
    }
    return 0;
}

/* 0x801A8440 | 0x18 */
void HSD_MObjSetFlags(HSD_MObj* mobj, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode |= flags;
}

/* 0x801A8428 | 0x18 */
void HSD_MObjClearFlags(HSD_MObj* mobj, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode &= ~flags;
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

/* The [0, 1] clamp expanded in every colour and alpha case below. */
static inline f32 MObjClamp01(f32 value)
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
            mobj->mat->ambient.r = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_AMBIENT_G:
        if (mobj->mat != NULL) {
            mobj->mat->ambient.g = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_AMBIENT_B:
        if (mobj->mat != NULL) {
            mobj->mat->ambient.b = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_R:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.r = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_G:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.g = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_DIFFUSE_B:
        if (mobj->mat != NULL) {
            mobj->mat->diffuse.b = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_ALPHA:
        if (mobj->mat != NULL) {
            mobj->mat->alpha = MObjClamp01(1.0F - val->fv);
        }
        break;
    case HSD_A_M_SPECULAR_R:
        if (mobj->mat != NULL) {
            mobj->mat->specular.r = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_SPECULAR_G:
        if (mobj->mat != NULL) {
            mobj->mat->specular.g = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_SPECULAR_B:
        if (mobj->mat != NULL) {
            mobj->mat->specular.b = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_PE_REF0:
        if (mobj->pe != NULL) {
            mobj->pe->ref0 = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_PE_REF1:
        if (mobj->pe != NULL) {
            mobj->pe->ref1 = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    case HSD_A_M_PE_DSTALPHA:
        if (mobj->pe != NULL) {
            mobj->pe->dst_alpha = (u8) (255.0F * MObjClamp01(val->fv));
        }
        break;
    }
}

/* 0x801A7E3C | 0x48 */
void HSD_MObjAnim(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_AObjInterpretAnim(mobj->aobj, mobj, HSD_MOBJ_METHOD(mobj)->update);
        HSD_TObjAnimAll(mobj->tobj);
    }
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

/* 0x801A7CFC | 0x5C */
void HSD_MObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info != NULL) {
        HSD_ASSERT(334, hsdIsDescendantOf(info, &hsdMObj));
    }
    default_class = info;
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
            HSD_ASSERT(373, mobj);
        }

        HSD_MOBJ_METHOD(mobj)->load(mobj, mobjdesc);
        HSD_MObjCompileTev(mobj);

        return mobj;
    } else {
        return NULL;
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
    s32 diffuse_bits;
    s32 alpha_bits;
    static u8 one = 0xFF;

    HSD_ASSERT(395, list);

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
        switch (diffuse_bits) {
        case RENDER_DIFFUSE_VTX:
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_1,
                            HSD_TEXP_ZERO);
            break;
        default:
            cnst = HSD_TExpCnst(&mobj->mat->diffuse, HSD_TE_RGB, HSD_TE_U8,
                                list);
            HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_RGB, cnst);
            break;
        }
        switch (alpha_bits) {
        case RENDER_ALPHA_VTX:
            cnst = HSD_TExpCnst(&one, HSD_TE_X, HSD_TE_U8, list);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X,
                            cnst);
            break;
        default:
            cnst = HSD_TExpCnst(&mobj->mat->alpha, HSD_TE_X, HSD_TE_F32,
                                list);
            HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X,
                            cnst);
            break;
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

/* 0x801A6F78 | 0x78 */
void MObjSetupTev(HSD_MObj* mobj, HSD_TObj* tobj, u32 rendermode)
{
    HSD_ASSERT(798, mobj->tevdesc);
    HSD_TExpSetupTev(mobj->tevdesc, mobj->texp);
    HSD_TObjSetupVolatileTev(tobj, rendermode);
}

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

/* 0x801A6E00 | 0x24 */
void HSD_MObjUnset(HSD_MObj* mobj, u32 rendermode)
{
    HSD_TObjSetup(NULL);
}

/* 0x801A6DDC | 0x24 */
void HSD_MObjSetAlpha(HSD_MObj* mobj, f32 alpha)
{
    if (mobj == NULL || mobj->mat == NULL) {
        return;
    }
    mobj->mat->alpha = alpha;
}

/* 0x801A6DC4 | 0x18 */
HSD_TObj* HSD_MObjGetTObj(HSD_MObj* mobj)
{
    if (mobj == NULL) {
        return NULL;
    }
    return mobj->tobj;
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

/* 0x801A6D5C | 0x44 */
void HSD_MObjAddTObjNext(HSD_MObj* mobj, HSD_TObj* tobj, HSD_TObj* next)
{
    if (mobj == NULL || tobj == NULL || next == NULL) {
        return;
    }
    HSD_TObjAddNext(tobj, next);
}

/* 0x801A6D08 | 0x54 */
void HSD_MObjRemove(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_CLASS_METHOD(mobj)->release((HSD_Class*) mobj);
        HSD_CLASS_METHOD(mobj)->destroy((HSD_Class*) mobj);
    }
}

HSD_MObj* HSD_MObjAlloc(void)
{
    HSD_MObj* mobj = hsdNew(default_class != NULL ? default_class
                                                  : HSD_CLASS_INFO(&hsdMObj));

    HSD_ASSERT(1098, mobj);
    return mobj;
}

HSD_Material* HSD_MaterialAlloc(void)
{
    HSD_Material* mat = hsdAllocMemPiece(sizeof(HSD_Material));

    HSD_ASSERT(1126, mat);
    memset(mat, 0, sizeof(HSD_Material));
    mat->alpha = 1.0F;
    return mat;
}

/* 0x801A6CA4 | 0x64 */
void HSD_MObjAddShadowTexture(HSD_TObj* tobj)
{
    HSD_TObj* cur;

    HSD_ASSERT(1173, tobj);
    for (cur = tobj_shadows; cur != NULL; cur = cur->next) {
        if (cur == tobj) {
            return;
        }
    }
    tobj->next = tobj_shadows;
    tobj_shadows = tobj;
}

void HSD_MObjDeleteShadowTexture(HSD_TObj* tobj)
{
    if (tobj != NULL) {
        HSD_TObj** cur = &tobj_shadows;

        while (*cur != NULL) {
            if (*cur == tobj) {
                *cur = tobj->next;
                tobj->next = NULL;
                return;
            }
            cur = &(*cur)->next;
        }
    } else {
        HSD_TObj* next;

        while (tobj_shadows != NULL) {
            next = tobj_shadows->next;
            tobj_shadows->next = NULL;
            tobj_shadows = next;
        }
    }
}

void MObjRelease(HSD_Class* o)
{
    HSD_MObj* mobj = HSD_MOBJ(o);

    HSD_AObjRemove(mobj->aobj);
    fn_80193AF0(mobj->mat, sizeof(HSD_Material));
    HSD_TObjRemoveAll(mobj->tobj);

    if (mobj->tevdesc != NULL) {
        HSD_TExpFreeTevDesc(mobj->tevdesc);
    }
    if (mobj->texp != NULL) {
        HSD_TExpFreeList(mobj->texp, HSD_TE_ALL, 1);
    }
    if (mobj->pe != NULL) {
        fn_80193AF0(mobj->pe, sizeof(HSD_PEDesc));
    }
    HSD_PARENT_INFO(&hsdMObj)->release(o);
}

void MObjAmnesia(HSD_ClassInfo* info)
{
    if (info == default_class) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdMObj)) {
        tobj_toon = NULL;
        tobj_shadows = NULL;
    }
    HSD_PARENT_INFO(&hsdMObj)->amnesia(info);
}

void MObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdMObj), &lbl_8036C638,
                     "sysdolphin_base_library", "hsd_mobj",
                     sizeof(HSD_MObjInfo), sizeof(HSD_MObj));

    HSD_CLASS_INFO(&hsdMObj)->release = MObjRelease;
    HSD_CLASS_INFO(&hsdMObj)->amnesia = MObjAmnesia;
    HSD_MOBJ_INFO(&hsdMObj)->setup = HSD_MObjSetup;
    HSD_MOBJ_INFO(&hsdMObj)->unset = HSD_MObjUnset;
    HSD_MOBJ_INFO(&hsdMObj)->load = MObjLoad;
    HSD_MOBJ_INFO(&hsdMObj)->make_texp = MObjMakeTExp;
    HSD_MOBJ_INFO(&hsdMObj)->setup_tev = MObjSetupTev;
    HSD_MOBJ_INFO(&hsdMObj)->update = MObjUpdateFunc;
}
