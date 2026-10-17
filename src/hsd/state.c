/**
 * @file state.c
 * @brief HSD render state cache: pixel-engine and channel modes, material
 *        colour, GX state setters and state invalidation.
 *
 * Retail TU: .text 0x801B25C4 - 0x801B3168.
 *
 * HAL sysdolphin (>= 1.3.0.0) state.c, reconstructed with the Melee
 * decompilation (doldecomp/melee, src/sysdolphin/baselib/state.c) as
 * reference. Functions appear in HAL source order; the library is built with
 * -inline auto,deferred, which emits them in reverse order.
 *
 * Linked as one unit (2026-09-29): .text 0x801B25C4 - 0x801B3168, .data
 * 0x8036CE88 - 0x8036CFA8 (the channel set-ups), .sdata 0x80478C98
 * (dark_matter) and .sdata2 0x8047DE50 (0.0f, 1.0f, 255.0f). HSD_StateSetZMode
 * (HSD_StateSetZMode) is exact only in a shaped form, a title-path rule exception
 * (see the function and docs/RULE_EXCEPTIONS.md). The state words, material
 * state and invalidate table stay in their data units under their address
 * names.
 */
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_tev.h"
#include "hsd/hsd_state.h"

extern void HSD_MulColor(GXColor* a, GXColor* b, GXColor* result);

/* GX */
extern void GXSetDstAlpha(u8 enable, u8 alpha);
extern void GXSetBlendMode(int type, int src_factor, int dst_factor, int op);
extern void GXSetZMode(u32 compareEnable, u32 func, u32 updateEnable);
extern void GXSetZCompLoc(u8 before_tex);   /* GXSetZCompLoc */
extern void GXSetAlphaCompare(int comp0, u8 ref0, int op, int comp1,
                        u8 ref1);         /* GXSetAlphaCompare */
extern void GXSetDither(u8 enable);       /* GXSetDither */

#define GX_BM_NONE 0
#define GX_BM_BLEND 1
#define GX_BL_SRCALPHA 4
#define GX_BL_INVSRCALPHA 5
#define GX_LO_NOOP 15
#define GX_LEQUAL 3
#define GX_GREATER 4
#define GX_ALWAYS 7
#define GX_AOP_AND 0
#define GX_TEXCOORD_NULL 0xFF
#define GX_PASSCLR 4

static GXColor dark_matter = { 0x00, 0x00, 0x00, 0xFF };

/* Channel set-ups used by HSD_SetupChannelMode. */
static HSD_Chan spec_chan = {
    NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 1, 0, 0,
    0, GX_DF_NONE, 0, NULL
};
static HSD_Chan diffuse_chan = {
    NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 1, 0, 0,
    0, 2, 1, NULL
};
static HSD_Chan alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 1, 0, 0, 0, 2, 1, NULL
};
static HSD_Chan vtx_alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0, 1, 0, 2, 1, NULL
};
static HSD_Chan unlit_color_chan = {
    NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 0, 0, 1,
    0, GX_DF_NONE, GX_AF_NONE, NULL
};
static HSD_Chan unlit_alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0x00, 0x00, 0x00, 0xFF }, 0, 0, 1,
    0, GX_DF_NONE, GX_AF_NONE, NULL
};

/* HSD_SetupChannelMode */
void HSD_SetupChannelMode(u32 rendermode)
{
    u32 color_mode;
    u32 alpha_mode;
    int specular = 0;
    int vtx_alpha = 0;
    HSD_LObj* lobj;
    int i;
    int max;
    u8 alpha;

    color_mode = rendermode & 3;
    if (color_mode == 0) {
        color_mode = 1;
    }
    alpha_mode = rendermode & 0x6000;
    if (alpha_mode == 0) {
        alpha_mode = color_mode << 13;
    }

    if (rendermode & 8) {
        spec_chan.light_mask = HSD_LObjGetLightMaskSpecular();
        HSD_SetupChannel(&spec_chan);
        specular = 1;
        max = HSD_LObjGetNbActive();
        for (i = 0; i < max; i++) {
            HSD_LObj* l = HSD_LObjGetActiveByIndex(i);
            if (l != NULL) {
                HSD_LObjSetupSpecular(l, l->color, lbl_80465710.shininess);
            }
        }
    }

    if (rendermode & 4) {
        lobj = HSD_LObjGetActiveByID(0x100);
        if (lobj != NULL && (lobj->flags & 4)) {
            HSD_MulColor(&lbl_80465710.ambient, &lobj->color, &diffuse_chan.amb_color);
        } else {
            diffuse_chan.amb_color = dark_matter;
        }
        diffuse_chan.mat_src = (color_mode >> 1) & 1;
        diffuse_chan.light_mask = HSD_LObjGetLightMaskDiffuse();
        HSD_SetupChannel(&diffuse_chan);

        if (alpha_mode & 0x4000) {
            alpha_chan.chan = GX_ALPHA1;
            vtx_alpha = 1;
            HSD_SetupChannel(&vtx_alpha_chan);
        } else {
            alpha_chan.chan = GX_ALPHA0;
        }
        alpha_chan.light_mask = HSD_LObjGetLightMaskAlpha();
        if (lobj != NULL && (lobj->flags & 0x10)) {
            alpha = lobj->color.a;
        } else {
            alpha = 0;
        }
        if (alpha_chan.light_mask != 0) {
            alpha_chan.enable = GX_ENABLE;
            alpha_chan.mat_color.a = 0xFF;
            alpha_chan.amb_color.a = alpha;
        } else {
            alpha_chan.enable = GX_DISABLE;
            alpha_chan.mat_color.a = alpha;
        }
        HSD_SetupChannel(&alpha_chan);
    } else {
        unlit_color_chan.mat_src = (color_mode >> 1) & 1;
        HSD_SetupChannel(&unlit_color_chan);
        unlit_alpha_chan.mat_src = (alpha_mode >> 14) & 1;
        HSD_SetupChannel(&unlit_alpha_chan);
    }

    if (specular) {
        if (!vtx_alpha) {
            fn_801B3AE8(GX_ALPHA1);
        }
        HSD_StateSetNumChans(2);
    } else if (vtx_alpha) {
        fn_801B3AE8(GX_COLOR1);
        HSD_StateSetNumChans(2);
    } else {
        fn_801B3AE8(GX_COLOR1A1);
        HSD_StateSetNumChans(1);
    }
}

static inline void HSD_StateSetBlendMode(int type, int src_factor,
                                         int dst_factor, int op);
static inline void HSD_StateSetAlphaCompare(int comp0, u8 ref0, int op,
                                            int comp1, u8 ref1);
static inline void HSD_StateSetDstAlpha(int enable, u8 alpha);
static inline void HSD_StateSetZCompLoc(int before_tex);
static inline void HSD_StateSetDither(int enable);

/* HSD_SetupPEMode */
void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc* pe)
{

    if (pe != NULL) {
        HSD_StateSetColorUpdate(pe->flags & 1);
        HSD_StateSetAlphaUpdate(pe->flags & 2);
        HSD_StateSetDstAlpha(pe->flags & 4, pe->dst_alpha);
        HSD_StateSetBlendMode(pe->type, pe->src_factor, pe->dst_factor,
                              pe->logic_op);
        HSD_StateSetZMode(pe->flags & 0x10, pe->z_comp, pe->flags & 0x20);
        HSD_StateSetZCompLoc(pe->flags & 8);
        HSD_StateSetAlphaCompare(pe->alpha_comp0, pe->ref0, pe->alpha_op,
                                 pe->alpha_comp1, pe->ref1);
        HSD_StateSetDither(pe->flags & 0x40);
        return;
    }
    HSD_StateSetColorUpdate(1);
    HSD_StateSetAlphaUpdate(0);
    HSD_StateSetDstAlpha(0, 0);
    HSD_StateSetBlendMode((rendermode & RENDER_XLU) ? GX_BM_BLEND : GX_BM_NONE,
                          GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    HSD_StateSetZMode(1, (rendermode & RENDER_ZMODE_ALWAYS) ? GX_ALWAYS : GX_LEQUAL,
                (GXBool) ((rendermode & RENDER_NO_ZUPDATE) ? GX_FALSE : GX_TRUE));
    if (!(rendermode & RENDER_NO_ZUPDATE) && (rendermode & RENDER_XLU)) {
        HSD_StateSetZCompLoc(0);
        HSD_StateSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
    } else {
        HSD_StateSetZCompLoc(1);
        HSD_StateSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
    HSD_StateSetDither(0);
}

static inline void setupTevMode_last(void)
{
    if (HSD_StateGetNumTevStages() == 0) {
        HSD_TevDesc tevdesc;
        tevdesc.flags = 0;
        tevdesc.stage = HSD_StateAssignTev();
        tevdesc.coord = GX_TEXCOORD_NULL;
        tevdesc.map = GX_TEXMAP_NULL;
        tevdesc.color = GX_COLOR0A0;
        tevdesc.u.tevop.tevmode = GX_PASSCLR;
        HSD_SetupTevStage(&tevdesc);
    }
}

/* HSD_SetupRenderModeWithCustomPE */
void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc* pe)
{
    setupTevMode_last();
    HSD_SetupPEMode(rendermode, pe);
    HSD_SetTevRegAll();
    HSD_StateSetNumTevStages();
    HSD_StateSetNumTexGens();
    HSD_SetupChannelMode(rendermode);
}

/* 0x801B2878 - 0x801B294C */
extern void GXSetCullMode(int mode); /* GXSetCullMode */

/* HSD_SetMaterialColor */
void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha)
{
    lbl_80465710.ambient = ambient;
    lbl_80465710.diffuse = diffuse;
    lbl_80465710.specular = specular;
    if (alpha <= 0.0F) {
        alpha = 0.0F;
    } else if (alpha >= 1.0F) {
        alpha = 1.0F;
    }
    lbl_80465710.alpha = 255.0F * alpha;
}

/* HSD_SetMaterialShininess */
void HSD_SetMaterialShininess(f32 shininess)
{
    lbl_80465710.shininess = shininess;
}

/* HSD_StateSetCullMode */
void HSD_StateSetCullMode(int mode)
{
    if (lbl_8047B34C != mode) {
        GXSetCullMode(mode);
        lbl_8047B34C = mode;
    }
}

static inline void HSD_StateSetBlendMode(int type, int src_factor,
                                         int dst_factor, int op)
{
    if (lbl_8047B348 != type || lbl_8047B344 != src_factor ||
        lbl_8047B340 != dst_factor || lbl_8047B33C != op)
    {
        GXSetBlendMode(type, src_factor, dst_factor, op);
        lbl_8047B348 = type;
        lbl_8047B344 = src_factor;
        lbl_8047B340 = dst_factor;
        lbl_8047B33C = op;
    }
}

/*
 * HSD_StateSetZMode
 *
 * Exact only in this shaped form (2026-09-29), a title-path rule exception
 * (HSD_StateSetZMode is a native_functions entry of the recomp's gs-gfx-init row):
 * the normalised flags are int locals and the cache compares cast them back
 * to GXBool. Retail computes the update flag's `or` into r0 after the
 * z_enable compare; the natural GXBool locals compute it into r4 before the
 * state load (88.72%). GXSetZMode is declared with the SDK's u32 parameters
 * as in this project's GX source (sdk_range_800BB30C.c) and its other
 * callers; with u8 parameters three instructions still differ. The same body
 * is exact where HSD_SetupPEMode expands it twice. Ruled out earlier (strict
 * forms), each on the full TU and on the function alone:
 * - compilers GC/1.0-1.2.5n (worse), 1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1, 2.5,
 *   2.6, 2.7 (all the same 6 instructions off), 3.0a3-3.0a5.2 (worse);
 *   -O0/-O2/-O3/-O4 with ,p and ,s, -inline auto/noauto/smart/off/deferred,
 *   -opt nopeephole/noschedule, -proc 750/740/7400/603e/604/601/generic,
 *   -sdata, -use_lmw_stmw off, -fp_contract off, C++;
 * - callee and global types, parameter types, Melee's shape, casts, local
 *   orders, early returns, store orders, deferred-inlining contexts (about
 *   1,000 generated variants).
 */
void HSD_StateSetZMode(int enable, int func, int update)
{
    int z_enable = enable != 0;
    int z_update = update != 0;

    /* RULE-EXCEPTION(title-path): casts whose only effect is register allocation - see docs/RULE_EXCEPTIONS.md */
    if (lbl_8047B338 != (GXBool) z_enable || lbl_8047B334 != func ||
        lbl_8047B330 != (GXBool) z_update)
    {
        GXSetZMode(z_enable, func, z_update);
        lbl_8047B338 = z_enable;
        lbl_8047B334 = func;
        lbl_8047B330 = z_update;
    }
}

static inline void HSD_StateSetAlphaCompare(int comp0, u8 ref0, int op,
                                            int comp1, u8 ref1)
{
    if (lbl_8047B32C != comp0 || lbl_8047B328 != ref0 ||
        lbl_8047B324 != op || lbl_8047B320 != comp1 ||
        lbl_8047B31E != ref1)
    {
        GXSetAlphaCompare(comp0, ref0, op, comp1, ref1);
        lbl_8047B32C = comp0;
        lbl_8047B328 = ref0;
        lbl_8047B324 = op;
        lbl_8047B320 = comp1;
        lbl_8047B31E = ref1;
    }
}

static inline void HSD_StateSetDstAlpha(int enable, u8 alpha)
{
    GXBool dst_enable = enable != 0;

    if (lbl_8047B31B != dst_enable || lbl_8047B31A != alpha) {
        GXSetDstAlpha(dst_enable, alpha);
        lbl_8047B31B = dst_enable;
        lbl_8047B31A = alpha;
    }
}

static inline void HSD_StateSetZCompLoc(int before_tex)
{
    GXBool before = before_tex != 0;

    if (lbl_8047B319 != before) {
        GXSetZCompLoc(before);
        lbl_8047B319 = before;
    }
}

static inline void HSD_StateSetDither(int enable)
{
    GXBool dither = enable != 0;

    if (lbl_8047B318 != dither) {
        GXSetDither(dither);
        lbl_8047B318 = dither;
    }
}

/* 0x801B25C4 - 0x801B27DC */
extern void HSD_ClearVtxDesc(void);

extern void GXSetColorUpdate(u8 enable); /* GXSetColorUpdate */
extern void GXSetAlphaUpdate(u8 enable); /* GXSetAlphaUpdate */

/* HSD_StateSetColorUpdate */
void HSD_StateSetColorUpdate(int enable)
{
    GXBool update = enable != 0;

    if (lbl_8047B31D != update) {
        GXSetColorUpdate(update);
        lbl_8047B31D = update;
    }
}

/* HSD_StateSetAlphaUpdate */
void HSD_StateSetAlphaUpdate(int enable)
{
    GXBool update = enable != 0;

    if (lbl_8047B31C != update) {
        GXSetAlphaUpdate(update);
        lbl_8047B31C = update;
    }
}

/*
 * _HSD_StateInvalidatePrimitive. Retail stores the line width twice (0, then
 * 0xFF); nothing else reads these two bytes.
 */
void _HSD_StateInvalidatePrimitive(void)
{
    lbl_8047B351 = 0;
    lbl_8047B350 = 0;
    lbl_8047B34C = -1;
    lbl_8047B351 = -1;
}

/* _HSD_StateInvalidateVtxAttr */
void _HSD_StateInvalidateVtxAttr(void)
{
    HSD_ClearVtxDesc();
}

/* _HSD_StateInvalidateRenderMode */
void _HSD_StateInvalidateRenderMode(void)
{
    lbl_8047B348 = -1;
    lbl_8047B344 = -1;
    lbl_8047B340 = -1;
    lbl_8047B33C = -1;
    lbl_8047B338 = -1;
    lbl_8047B334 = -1;
    lbl_8047B330 = -1;
    lbl_8047B32C = -1;
    lbl_8047B328 = 0;
    lbl_8047B324 = -1;
    lbl_8047B320 = -1;
    lbl_8047B31E = 0;
    lbl_8047B31D = -1;
    lbl_8047B31C = -1;
    lbl_8047B31B = -1;
    lbl_8047B31A = 0;
    lbl_8047B319 = -1;
    lbl_8047B318 = -1;
}

/* HSD_StateInvalidate */
void HSD_StateInvalidate(int mask)
{
    int i;

    for (i = 0; lbl_8036CFA8[i].mask != HSD_STATE_NONE; i++) {
        if (mask & lbl_8036CFA8[i].mask) {
            lbl_8036CFA8[i].func();
        }
    }
}
