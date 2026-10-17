/**
 * @file psdisp.c
 * @brief HAL's particle display (sysdolphin psdisp.c) in the Genius Sonority
 *        fork, 0x8016AB94 - 0x8016EC1C.
 *
 * The whole translation unit (linked):
 *   .text   0x8016AB94 - 0x8016EC1C  psappsrt.c ends at 0x8016AB94;
 *           psdisptev.c starts with psSetupTev at 0x8016EC1C
 *   .rodata 0x802738B8 - 0x8027399B  a 20-float block, the billboard
 *           identity matrix, "Particle:setBlendMode:Unknown mode\n",
 *           "psdisp.c" and object.h's ref_INC assert (psSetFog)
 *   .data   0x8036BFC0 - 0x8036BFE0  the quad texture coordinates (32-byte
 *           aligned; 0x8036BFA4-0x8036BFC0 is alignment padding)
 *   .bss    0x80452DE8 - 0x80452EC8  the display matrices
 *   .sbss   0x8047B128 - 0x8047B170  fog, matrix and colour state
 *   .sdata2 0x8047D5C8 - 0x8047D628  the literal pool ("0" for the assert)
 * psFrameNum (.sdata 0x80478C30) is shared and stays extern.
 *
 * Reference: Melee's sysdolphin/baselib/psdisp.c (doldecomp/melee). The fork
 * keeps psDispSub, psDispSubMakePolygon, psDispSubAppSRT,
 * psDispSubPointTrail, psDispSubAPPSRTPoint, setupTevReg and setupChanReg as
 * real functions (Melee inlines them into psDispParticles), draws one point
 * or trail per call instead of batching, passes the particle velocity to
 * psDispSubMakePolygon (which derives the trail's previous position), scales
 * billboards by the generator's scale block, clamps the texture-form trail
 * ratio at 1, and adds psSetFog/psRemoveFog (psSetFog/fn_8016EA88).
 * particleSort lives in pslist.c.
 *
 * Compiler: the particle library flags (pslist.c, particle.c, generator.c,
 * psdisptev.c): GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on
 * -sdata 8 -sdata2 8 -str reuse,readonly, unit-wide, no local pragmas.
 * Functions are defined in reverse address order (deferred generation).
 *
 * The direction tests in psDispSub/psDispSubAppSRT fall back to the
 * particle's own rotation when a projected w is 0 (retail branches from
 * both w tests straight to that else-arm); that is written as a goto to the
 * else-arm's label.
 *
 * Status: psSetFog, psRemoveFog, setupChanReg, setupTevReg,
 * psDispSubAPPSRTPoint, psDispSubPointTrail, psDispSubAppSRT, psDispSub and
 * psDispSubMakePolygon reproduce retail. MakePolygon's appsrt branch goes
 * through calcTornadoLastPosAPPSRT, a separate inline in XD's psdisp.o
 * (NXXJ01.map, UNUSED 0x224; the same body compiles out of line to exactly
 * 0x224 here), whose nested calcTornadoLastPos expansion gives retail's
 * register order. psDispParticles (fn_8016AB94) reproduces retail too: its
 * scalar locals are declared so MWCC numbers them in retail's order (the
 * spilled ones, i/alpha_mode/alpha0_ref/alpha1_ref/needs_setup, get their
 * stack slots in reverse declaration order), the tlut descriptor is an
 * HSD_Tlut that stays in memory (0x40-0x50; see the tagged exception at its
 * use), the multiply blend mode is (DSTCLR, ZERO), and the texture's
 * magnification filter tests the TexInterpNear bit directly.
 *
 * Every function is exact, so the whole unit links as one object with its
 * .rodata, .data, .bss, .sbss and .sdata2 (it replaces the former
 * candidate chunks and the setupTevReg/psSetFog carves).
 */
#include "dolphin/types.h"
#include "dolphin/gx/GXVert.h"
#include "crt/float.h"
#include "crt/math_ppc.h"
#include "hsd/hsd_fog.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_tobj.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/psstructs.h"

typedef enum GXAttnFn { GX_AF_SPEC = 0, GX_AF_SPOT = 1, GX_AF_NONE = 2 } GXAttnFn;

extern void GXSetChanAmbColor(s32 chan, GXColor color); /* GXSetChanAmbColor */
extern void GXSetChanMatColor(s32 chan, GXColor color); /* GXSetChanMatColor */
extern void GXSetTevColor(s32 reg, GXColor color);  /* GXSetTevColor */
extern void HSD_MulColor(GXColor* a, GXColor* b, GXColor* dst);
extern void GXClearVtxDesc(void);                              /* GXClearVtxDesc */
extern void GXSetVtxDesc(s32 attr, s32 type);                /* GXSetVtxDesc */
extern void GXBegin(s32 type, s32 vtxfmt, u16 nverts);  /* GXBegin */
extern void GXSetLineWidth(u8 width, s32 texOffsets);          /* GXSetLineWidth */
extern void GXSetCurrentMtx(u32 id);                            /* GXSetCurrentMtx */
extern void GXSetPointSize(u8 size, s32 texOffsets);           /* GXSetPointSize */
extern void PSMTXCopy(const Mtx src, Mtx dst);
extern void HSD_MtxSRT(Mtx m, Vec* scale, Vec* rot, Vec* trans, Vec* scale2);
extern void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad);
extern void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
extern void psDispSubMakePolygon(HSD_Particle* pp, u8* texform, f32 x, f32 y,
                                 f32 z, f32 vx, f32 vy, f32 vz, f32 rx,
                                 f32 ry, f32 rz, f32 ux, f32 uy, f32 uz);
extern void psDispSubAppSRT(HSD_Particle* pp, u8* texform);
extern void psDispSub(HSD_Particle* pp, u8* texform);
extern void psDispSubAPPSRTPoint(HSD_Particle* pp);
extern HSD_Particle* psDispSubPointTrail(HSD_Particle* pp);
extern void setupChanReg(HSD_Particle* pp);
extern void setupTevReg(HSD_Particle* pp);

extern HSD_Particle* particleSort(s32 linkNo, HSD_Particle** firstPass,
                                  HSD_Particle** secondPass);
extern void psSetupTev(HSD_Particle* pp);
extern void psSetupTevInvalidState(void);
extern void psSetupTevCommon(void);
extern void OSReport(const char* fmt, ...);
extern void PSMTXInverse(const Mtx src, Mtx inv);
extern void HSD_CObjGetViewingMtx(struct HSD_CObj* cobj, Mtx mtx);
extern struct HSD_CObj* HSD_CObjGetCurrent(void);
extern void GXLoadPosMtxImm(const Mtx mtx, u32 id);
extern void GXLoadTexMtxImm(const Mtx mtx, u32 id, u32 type);
extern void GXSetBlendMode(s32 type, s32 src, s32 dst, s32 op);
extern void GXSetZMode(u32 enable, u32 func, u32 update);
extern void GXInitTexObjCI(GXTexObj* obj, void* image, u16 width, u16 height,
                           u32 format, u32 wrap_s, u32 wrap_t, u8 mipmap,
                           u32 tlut_name);
extern void GXLoadTexObj(GXTexObj* obj, u32 id);
extern void GXGetProjectionv(f32* prj);                           /* GXGetProjectionv */
extern void GXEnableTexOffsets(u32 coord, u8 line, u8 point);       /* GXEnableTexOffsets */
extern void HSD_MtxGetRotationMtx(Mtx src, Mtx dst, char axis0, char axis1); /* HSD_MtxGetRotationMtx */
extern void GXSetCullMode(s32 mode);                           /* GXSetCullMode */
extern void GXSetArray(s32 attr, void* base, u8 stride);    /* GXSetArray */
extern void GXSetVtxAttrFmt(s32 fmt, s32 attr, s32 cnt, s32 type, u8 frac); /* GXSetVtxAttrFmt */
extern void GXSetAlphaCompare(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1); /* GXSetAlphaCompare */
extern void GXSetNumChans(u8 num);                             /* GXSetNumChans */
extern void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src,
                        u32 light_mask, s32 diff_fn, GXAttnFn attn_fn); /* GXSetChanCtrl */
extern void GXSetZCompLoc(u8 before_tex);                      /* GXSetZCompLoc */
extern void GXSetTexCoordGen2(u32 dst_coord, u32 func, u32 src_param, u32 mtx,
                        u8 normalize, u32 pt_texmtx);        /* GXSetTexCoordGen2 */
extern void GXInitTexObj(GXTexObj* obj, void* image, u16 width, u16 height,
                        u32 format, u32 wrap_s, u32 wrap_t, u8 mipmap); /* GXInitTexObj */
extern void GXInitTexObjLOD(GXTexObj* obj, u32 min_filt, u32 mag_filt,
                        f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp,
                        u8 do_edge_lod, u32 max_aniso);      /* GXInitTexObjLOD */
extern void GXInitTlutObj(GXTlutObj* tlut_obj, void* lut, u32 fmt,
                        u16 n_entries);                      /* GXInitTlutObj */
extern void GXLoadTlut(GXTlutObj* tlut_obj, u32 tlut_name); /* GXLoadTlut */
extern void HSD_StateInvalidate(u32 state);                          /* HSD_StateInvalidate */

#define GXGetProjectionv GXGetProjectionv
#define GXEnableTexOffsets GXEnableTexOffsets
#define HSD_MtxGetRotationMtx HSD_MtxGetRotationMtx
#define GXSetCullMode GXSetCullMode
#define GXSetArray GXSetArray
#define GXSetVtxAttrFmt GXSetVtxAttrFmt
#define GXSetAlphaCompare GXSetAlphaCompare
#define GXSetZCompLoc GXSetZCompLoc
#define GXInitTexObj GXInitTexObj
#define GXInitTexObjLOD GXInitTexObjLOD
#define GXInitTlutObj GXInitTlutObj
#define GXLoadTlut GXLoadTlut
#define HSD_StateInvalidate HSD_StateInvalidate

extern HSD_PSFormGroup** lbl_804528C8[PS_NUM_BANK]; /* particle.c: form groups */
extern HSD_PSTexGroup** lbl_804529C8[PS_NUM_BANK];  /* particle.c: texture groups */

#define GXClearVtxDesc GXClearVtxDesc
#define GXSetVtxDesc GXSetVtxDesc
#define GXBegin GXBegin
#define GXSetLineWidth GXSetLineWidth
#define GXSetCurrentMtx GXSetCurrentMtx
#define GXSetPointSize GXSetPointSize

#define GXSetTevColor GXSetTevColor

#define GX_COLOR0 0
#define GX_TEVREG0 1
#define GX_TEVREG1 2
#define GX_TEVREG2 3
#define GX_MAX_LIGHT 0x100
#define GX_PNMTX0 0
#define GX_LINES 0xA8
#define GX_POINTS 0xB8
#define GX_QUADS 0x80
#define GX_VTXFMT4 4
#define GX_VTXFMT5 5
#define GX_VTXFMT0 0
#define GX_VTXFMT1 1
#define GX_VTXFMT2 2
#define GX_VTXFMT3 3
#define GX_TO_ONE 5
#define GX_VA_POS 9
#define GX_VA_CLR0 11
#define GX_VA_TEX0 13
#define GX_DIRECT 1
#define GX_INDEX8 2
#define GX_PNMTX1 3
#define GX_COLOR0A0 4
#define GX_COLOR1A1 5
#define GX_ALPHA0 2
#define GX_SRC_REG 0
#define GX_SRC_VTX 1
#define GX_DF_NONE 0
#define GX_TRUE 1
#define GX_FALSE 0
#define GX_TEXCOORD0 0
#define GX_CULL_BACK 2
#define GX_POS_XYZ 1
#define GX_CLR_RGBA 1
#define GX_TEX_ST 1
#define GX_F32 4
#define GX_U8 0
#define GX_RGBA8 5
#define GX_BM_BLEND 1
#define GX_BM_SUBTRACT 3
#define GX_BL_ZERO 0
#define GX_BL_ONE 1
#define GX_BL_SRCCLR 2
#define GX_BL_DSTCLR 2
#define GX_BL_SRCALPHA 4
#define GX_BL_INVSRCALPHA 5
#define GX_LO_CLEAR 0
#define GX_LEQUAL 3
#define GX_MIRROR 2
#define GX_CLAMP 0
#define GX_TEXMTX0 30
#define GX_MTX2x4 1
#define GX_TG_MTX2x4 1
#define GX_TG_TEX0 4
#define GX_PTIDENTITY 125
#define GX_TLUT0 0
#define GX_TEXMAP0 0
#define GX_NEAR 0
#define GX_LINEAR 1
#define GX_ANISO_1 0
#define GX_TF_I4 0
#define GX_TF_I8 1
#define GX_TF_IA4 2
#define GX_TF_IA8 3
#define GX_TF_RGB565 4
#define GX_TF_RGB5A3 5
#define GX_TF_RGBA8 6
#define GX_TF_C4 8
#define GX_TF_C8 9
#define GX_TF_CMPR 14

#define Tornado (1 << 2)
#define TexEdge (1 << 3)
#define ComTLUT (1 << 4)
#define MirrorS (1 << 5)
#define MirrorT (1 << 6)
#define TexInterpNear (1 << 9)
#define DispFog (1 << 24)
#define NoZComp (1 << 28)
#define Hidden (1 << 29)
#define DispPoint (1 << 30)
#define DirVec (1 << 21)
#define PrimEnv (1 << 7)
#define DispTexture (1 << 10)
#define TexFlipS (1 << 18)
#define TexFlipT (1 << 19)
#define Trail (1 << 20)
#define DispLighting (1u << 31)

/* .sbss, defined in reverse address order (the object lays small data out
 * last-defined first). */
s32 lbl_8047B168;           /* prevPointSize */
s32 lbl_8047B164;           /* prevLineWidth */
/* the billboard axes, right + up and right - up per component */
f32 lbl_8047B160, lbl_8047B15C, lbl_8047B158, lbl_8047B154, lbl_8047B150,
    lbl_8047B14C;
s32 lbl_8047B148;           /* prevBlendMode */
u32 lbl_8047B144;           /* prevChanCtrl */
GXColor lbl_8047B140;       /* prevChanMat */
GXColor lbl_8047B13C;       /* prevChanAmb */
GXColor lbl_8047B138;       /* prevColorPrim */
GXColor lbl_8047B134;       /* prevColorEnv */
GXColor lbl_8047B130;       /* prevColorMat */
s32 lbl_8047B12C[1];        /* current position matrix */
HSD_Fog* lbl_8047B128;      /* psFog */

#define prevChanCtrl lbl_8047B144
#define prevBlendMode lbl_8047B148

/* .data: the texture coordinates of the four quad corners (GX_VA_TEX0 array). */
u8 lbl_8036BFC0[0x20] __attribute__((aligned(32))) = {
    0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
    0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0,
};

#define psFog lbl_8047B128
#define prevColorMat lbl_8047B130
#define prevColorEnv lbl_8047B134
#define prevColorPrim lbl_8047B138
#define prevChanAmb lbl_8047B13C
#define prevChanMat lbl_8047B140
#define psCurrentMtx lbl_8047B12C[0]
#define prevLineWidth lbl_8047B164
#define prevPointSize lbl_8047B168

#define PS_APPSTATUS_ONCE 1
#define PS_APPSTATUS_STILL 2

extern u8 lbl_80478C30; /* psFrameNum */
/* .bss, defined in reverse address order (deferred generation lays pooled
 * data out last-defined first). */
Mtx lbl_80452E94;     /* vmtx: the camera's viewing matrix */
Mtx lbl_80452E64;     /* rvmtx: its inverse */
f32 lbl_80452E48[7];  /* prj: the projection parameters */
Mtx lbl_80452E18;     /* pvmtx */
Mtx lbl_80452DE8;

#define pvmtx lbl_80452E18
#define prj lbl_80452E48
#define rvmtx lbl_80452E64
#define vmtx lbl_80452E94

#define psFrameNum lbl_80478C30

static inline void psSetCurrentMtx(s32 idx)
{
    if (psCurrentMtx != idx) {
        psCurrentMtx = idx;
        GXSetCurrentMtx(idx);
    }
}

static inline void setVtxDesc(s32 fmt)
{
    GXClearVtxDesc();
    switch (fmt) {
    case 0:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        return;
    case 1:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        return;
    case 2:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        return;
    case 3:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        return;
    case 4:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        return;
    case 5:
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        return;
    }
}

static inline void calcTornadoLastPos(HSD_Particle* pp, f32* x, f32* y, f32* z)
{
    f32 px, py, pz;
    f32 sina, sinb, cosa, cosb;
    HSD_Generator* gp;
    f32 radius;
    f32 vx0;
    f32 vz0;

    gp = pp->gen;
    if (gp == NULL) {
        *x = pp->pos.x;
        *y = pp->pos.y;
        *z = pp->pos.z;
        return;
    }

    sina = sinf(pp->grav);
    sinb = sinf(pp->fric);
    cosa = cosf(pp->grav);
    cosb = cosf(pp->fric);

    vz0 = pp->vel.z - gp->aux.tornado.vel;
    vx0 = pp->vel.x - gp->grav;

    radius = gp->radius < 0.0f ? -gp->radius : gp->radius;
    radius += vz0 * tanf(gp->angle < 0.0f ? -gp->angle : gp->angle);
    radius *= pp->vel.y;
    px = radius * cosf(vx0);
    py = radius * sinf(vx0);
    pz = vz0;

    *x = px * cosb + pz * sinb + gp->pos.x;
    *y = -px * sina * sinb + py * cosa + pz * sina * cosb + gp->pos.y;
    *z = -px * cosa * sinb - py * sina + pz * cosa * cosb + gp->pos.z;
}

/* XD's psdisp.o keeps this as its own (stripped) function next to
 * calcTornadoLastPos: NXXJ01.map lists calcTornadoLastPosAPPSRT (UNUSED,
 * 0x224) and calcTornadoLastPos (UNUSED, 0x1C8). As a separate inline, its
 * nested calcTornadoLastPos expansion creates its locals after the direct
 * expansion's (breadth-first), which gives retail's register order. */
static inline void calcTornadoLastPosAPPSRT(HSD_Particle* pp, f32* x, f32* y,
                                            f32* z)
{
    Mtx mtx;
    Vec v;

    calcTornadoLastPos(pp, x, y, z);
    HSD_MtxSRT(mtx, &pp->appsrt->scale, &pp->appsrt->rot,
               &pp->appsrt->translate, NULL);
    v.x = *x;
    v.y = *y;
    v.z = *z;
    PSMTXMultVec(mtx, &v, &v);
    *x = v.x;
    *y = v.y;
    *z = v.z;
}

#include "sysdolphin/baselib/psdisp_color.h"

static inline void getClrTrail(HSD_Particle* pp, GXColor* color)
{
    GXColor env_color;

    switch (pp->kind & (DispLighting | PrimEnv)) {
    case 0:
    case DispLighting:
        getColorPrimEnv(pp, color, &env_color);
        break;
    case PrimEnv:
    case DispLighting | PrimEnv:
        color->r = color->g = color->b = color->a = 0xFF;
        break;
    }
}

/* psSetFog */
void psSetFog(HSD_Fog* fog)
{
    if (psFog != NULL) {
        HSD_FogUnref(psFog);
    }
    if (fog != NULL) {
        ref_INC(fog);
    }
    psFog = fog;
}

/* psRemoveFog */
void fn_8016EA88(void)
{
    if (psFog != NULL) {
        HSD_FogUnref(psFog);
        psFog = NULL;
    }
}

void setupChanReg(HSD_Particle* pp)
{
    GXColor prim_color;
    GXColor amb_color;
    GXColor mat_color;
    HSD_LObj* lobj;

    if (pp->kind & DispLighting) {
        getColorMatAmb(pp, &mat_color, &amb_color);
        if (pp->kind & PrimEnv) {
            prim_color.r = prim_color.g = prim_color.b = 0xFF;
        } else {
            getColorPrimEnv(pp, &prim_color, &mat_color);
            amb_color.r = (u8) ((amb_color.r * prim_color.r) >> 8);
            amb_color.g = (u8) ((amb_color.g * prim_color.g) >> 8);
            amb_color.b = (u8) ((amb_color.b * prim_color.b) >> 8);
        }
        if (prim_color.r != prevChanMat.r || prim_color.g != prevChanMat.g ||
            prim_color.b != prevChanMat.b)
        {
            prevChanMat = prim_color;
            GXSetChanMatColor(GX_COLOR0, prevChanMat);
        }
        lobj = HSD_LObjGetActiveByID(GX_MAX_LIGHT);
        if (lobj != NULL) {
            HSD_MulColor(&amb_color, &lobj->color, &amb_color);
        } else {
            amb_color.r = amb_color.g = amb_color.b = 0;
        }
        if (amb_color.r != prevChanAmb.r || amb_color.g != prevChanAmb.g ||
            amb_color.b != prevChanAmb.b)
        {
            prevChanAmb = amb_color;
            GXSetChanAmbColor(GX_COLOR0, prevChanAmb);
        }
    }
}

void setupTevReg(HSD_Particle* pp)
{
    GXColor prim_color;
    GXColor env_color;
    GXColor mat_color;
    GXColor amb_color;

    getColorPrimEnv(pp, &prim_color, &env_color);
    if ((pp->kind & PrimEnv) ||
        (!(pp->kind & DispLighting) && !(pp->kind & Trail)))
    {
        if (prevColorPrim.r != prim_color.r ||
            prevColorPrim.g != prim_color.g ||
            prevColorPrim.b != prim_color.b || prevColorPrim.a != prim_color.a)
        {
            prevColorPrim = prim_color;
            GXSetTevColor(GX_TEVREG0, prevColorPrim);
        }
        if (pp->kind & PrimEnv) {
            if (prevColorEnv.r != env_color.r ||
                prevColorEnv.g != env_color.g ||
                prevColorEnv.b != env_color.b || prevColorEnv.a != env_color.a)
            {
                prevColorEnv = env_color;
                GXSetTevColor(GX_TEVREG1, prevColorEnv);
            }
        } else if (prevColorEnv.r != 0 || prevColorEnv.g != 0 ||
                   prevColorEnv.b != 0 || prevColorEnv.a != 0)
        {
            prevColorEnv.r = prevColorEnv.g = prevColorEnv.b = prevColorEnv.a =
                0;
            GXSetTevColor(GX_TEVREG1, prevColorEnv);
        }
    }
    if (pp->kind & DispLighting) {
        getColorMatAmb(pp, &mat_color, &amb_color);
        if (pp->kind & PrimEnv) {
            if (prevColorMat.r != mat_color.r ||
                prevColorMat.g != mat_color.g ||
                prevColorMat.b != mat_color.b || prevColorMat.a != mat_color.a)
            {
                prevColorMat = mat_color;
                GXSetTevColor(GX_TEVREG2, prevColorMat);
            }
        } else {
            mat_color.a = (u8) ((mat_color.a * prim_color.a) >> 8);
            if (prevColorMat.r != mat_color.r ||
                prevColorMat.g != mat_color.g ||
                prevColorMat.b != mat_color.b || prevColorMat.a != mat_color.a)
            {
                prevColorMat = mat_color;
                GXSetTevColor(GX_TEVREG2, prevColorMat);
            }
        }
    }
}

void psDispSubAPPSRTPoint(HSD_Particle* pp)
{
    Mtx mtx;
    Mtx srt;
    Vec v;
    Vec tv;
    f32 cur_x, cur_y, cur_z;
    f32 prev_x, prev_y, prev_z;
    f32 vx, vy, vz;
    u8 w;

    psSetCurrentMtx(GX_PNMTX0);
    if (pp->appsrt->frameNum != psFrameNum) {
        if (pp->appsrt->status != PS_APPSTATUS_STILL) {
            HSD_MtxSRT(pp->appsrt->mmtx, &pp->appsrt->scale, &pp->appsrt->rot,
                       &pp->appsrt->translate, NULL);
        }
        if (pp->appsrt->status == PS_APPSTATUS_ONCE) {
            pp->appsrt->status = PS_APPSTATUS_STILL;
        }
    }
    pp->appsrt->frameNum = psFrameNum;
    PSMTXCopy(pp->appsrt->mmtx, mtx);
    mtx[0][3] -= pp->appsrt->translate.x;
    mtx[1][3] -= pp->appsrt->translate.y;
    mtx[2][3] -= pp->appsrt->translate.z;
    v.x = pp->vel.x;
    v.y = pp->vel.y;
    v.z = pp->vel.z;
    PSMTXMultVec(mtx, &v, &v);
    if (pp->appsrt->x72 != 0) {
        PSMTXMultVec(lbl_80452DE8, &v, &v);
    }
    vx = v.x;
    vy = v.y;
    vz = v.z;
    v.x = pp->pos.x;
    v.y = pp->pos.y;
    v.z = pp->pos.z;
    if (pp->appsrt->x72 != 0) {
        PSMTXMultVec(mtx, &v, &v);
        PSMTXMultVec(lbl_80452DE8, &v, &v);
        v.x += pp->appsrt->translate.x;
        v.y += pp->appsrt->translate.y;
        v.z += pp->appsrt->translate.z;
    } else {
        PSMTXMultVec(pp->appsrt->mmtx, &v, &v);
    }
    cur_x = v.x;
    cur_y = v.y;
    cur_z = v.z;
    if (pp->kind & Tornado) {
        calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
        HSD_MtxSRT(srt, &pp->appsrt->scale, &pp->appsrt->rot,
                   &pp->appsrt->translate, NULL);
        tv.x = prev_x;
        tv.y = prev_y;
        tv.z = prev_z;
        PSMTXMultVec(srt, &tv, &tv);
        prev_x = tv.x;
        prev_y = tv.y;
        prev_z = tv.z;
    } else {
        prev_x = cur_x - vx;
        prev_y = cur_y - vy;
        prev_z = cur_z - vz;
    }

    w = (pp->size > 42.5) ? 255.0f : 6.0f * pp->size;
    if (pp->kind & Trail) {
        GXColor color;

        if (prevLineWidth != (s32) w) {
            prevLineWidth = w;
            GXSetLineWidth(w, GX_TO_ONE);
        }
        getClrTrail(pp, &color);
        if (pp->kind & DispTexture) {
            setVtxDesc(2);
            GXBegin(GX_LINES, GX_VTXFMT2, 2);
        } else {
            setVtxDesc(3);
            GXBegin(GX_LINES, GX_VTXFMT3, 2);
        }
        GXPosition3f32(prev_x, prev_y, prev_z);
        GXColor4u8(color.r, color.g, color.b, (u8) ((f32) color.a * pp->trail));
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(0);
        }
        GXPosition3f32(cur_x, cur_y, cur_z);
        GXColor4u8(color.r, color.g, color.b, color.a);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(1);
        }
    } else {
        if (prevPointSize != (s32) w) {
            prevPointSize = w;
            GXSetPointSize(w, GX_TO_ONE);
        }
        if (pp->kind & DispTexture) {
            setVtxDesc(0);
            GXBegin(GX_POINTS, GX_VTXFMT0, 1);
        } else {
            setVtxDesc(1);
            GXBegin(GX_POINTS, GX_VTXFMT1, 1);
        }
        GXPosition3f32(cur_x, cur_y, cur_z);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(1);
        }
    }
}

HSD_Particle* psDispSubPointTrail(HSD_Particle* pp)
{
    f32 x, y, z;
    GXColor color;
    u8 w;

    psSetCurrentMtx(GX_PNMTX0);
    w = (pp->size > 42.5) ? 255.0f : 6.0f * pp->size;
    if (prevLineWidth != (s32) w) {
        prevLineWidth = w;
        GXSetLineWidth(w, GX_TO_ONE);
    }
    if (pp->kind & Tornado) {
        calcTornadoLastPos(pp, &x, &y, &z);
    } else {
        x = pp->pos.x - pp->vel.x;
        y = pp->pos.y - pp->vel.y;
        z = pp->pos.z - pp->vel.z;
    }
    getClrTrail(pp, &color);
    if (pp->kind & DispTexture) {
        setVtxDesc(2);
        GXBegin(GX_LINES, GX_VTXFMT2, 2);
    } else {
        setVtxDesc(3);
        GXBegin(GX_LINES, GX_VTXFMT3, 2);
    }
    GXPosition3f32(x, y, z);
    GXColor4u8(color.r, color.g, color.b, (u8) ((f32) color.a * pp->trail));
    if (pp->kind & DispTexture) {
        GXTexCoord1x8(0);
    }
    GXPosition3f32(pp->pos.x, pp->pos.y, pp->pos.z);
    GXColor4u8(color.r, color.g, color.b, color.a);
    if (pp->kind & DispTexture) {
        GXTexCoord1x8(1);
    }
    return pp;
}

void psDispSubAppSRT(HSD_Particle* pp, u8* texform)
{
    Mtx mtx;
    Vec v;
    f32 right_x;
    f32 up_x;
    f32 right_y;
    f32 up_y;
    f32 right_z;
    f32 up_z;
    f32 x;
    f32 y;
    f32 z;
    f32 vx;
    f32 vy;
    f32 vz;
    f32 angle;
    f32 dir_x;
    f32 dir_y;
    HSD_Generator* gp;

    if (pp->appsrt->frameNum != psFrameNum) {
        if (pp->appsrt->status != PS_APPSTATUS_STILL) {
            HSD_MtxSRT(pp->appsrt->mmtx, &pp->appsrt->scale, &pp->appsrt->rot,
                       &pp->appsrt->translate, NULL);
        }
        if (pp->appsrt->status == PS_APPSTATUS_ONCE) {
            pp->appsrt->status = PS_APPSTATUS_STILL;
        }
    }
    pp->appsrt->frameNum = psFrameNum;
    PSMTXCopy(pp->appsrt->mmtx, mtx);
    mtx[0][3] -= pp->appsrt->translate.x;
    mtx[1][3] -= pp->appsrt->translate.y;
    mtx[2][3] -= pp->appsrt->translate.z;
    v.x = pp->vel.x;
    v.y = pp->vel.y;
    v.z = pp->vel.z;
    PSMTXMultVec(mtx, &v, &v);
    if (pp->appsrt->x72 != 0 && !(pp->kind & Tornado)) {
        PSMTXMultVec(lbl_80452DE8, &v, &v);
    }
    vx = v.x;
    vy = v.y;
    vz = v.z;
    v.x = pp->pos.x;
    v.y = pp->pos.y;
    v.z = pp->pos.z;
    if (pp->appsrt->x72 != 0) {
        PSMTXMultVec(mtx, &v, &v);
        PSMTXMultVec(lbl_80452DE8, &v, &v);
        v.x += pp->appsrt->translate.x;
        v.y += pp->appsrt->translate.y;
        v.z += pp->appsrt->translate.z;
    } else {
        PSMTXMultVec(pp->appsrt->mmtx, &v, &v);
    }
    x = v.x;
    y = v.y;
    z = v.z;
    if (texform != NULL) {
        Mtx smtx;

        PSMTXScale(smtx, pp->appsrt->scale.x, pp->appsrt->scale.y, 1.0f);
        PSMTXConcat(rvmtx, smtx, smtx);
        right_x = smtx[0][0] * pp->size;
        up_x = -smtx[0][1] * pp->size;
        right_y = smtx[1][0] * pp->size;
        up_y = -smtx[1][1] * pp->size;
        right_z = smtx[2][0] * pp->size;
        up_z = -smtx[2][1] * pp->size;
    } else {
        Vec r;
        Vec u;

        r.x = pp->appsrt->scale.x + vmtx[0][3];
        r.y = pp->appsrt->scale.y + vmtx[1][3];
        r.z = vmtx[2][3];
        u.x = pp->appsrt->scale.x + vmtx[0][3];
        u.y = -pp->appsrt->scale.y + vmtx[1][3];
        u.z = vmtx[2][3];
        PSMTXMultVec(rvmtx, &r, &r);
        right_x = r.x * pp->size;
        right_y = r.y * pp->size;
        right_z = r.z * pp->size;
        PSMTXMultVec(rvmtx, &u, &u);
        up_x = u.x * pp->size;
        up_y = u.y * pp->size;
        up_z = u.z * pp->size;
    }
    gp = pp->gen;
    if (gp != NULL && (gp->posFlags & 0x20)) {
        right_x *= gp->scale.x;
        up_x *= gp->scale.x;
        right_y *= gp->scale.y;
        up_y *= gp->scale.y;
        right_z *= gp->scale.z;
        up_z *= gp->scale.z;
    }
    if ((pp->kind & Trail) || (pp->kind & DirVec)) {
        if (0.0f == prj[0]) {
            f32 prev_x;
            f32 prev_y;
            f32 prev_z;
            f32 w1inv;
            f32 w1;
            f32 w0inv;
            f32 w0;

            if (pp->kind & Tornado) {
                calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
                v.x = prev_x;
                v.y = prev_y;
                v.z = prev_z;
                if (pp->appsrt->x72 != 0) {
                    PSMTXMultVec(mtx, &v, &v);
                    PSMTXMultVec(lbl_80452DE8, &v, &v);
                    v.x += pp->appsrt->translate.x;
                    v.y += pp->appsrt->translate.y;
                    v.z += pp->appsrt->translate.z;
                } else {
                    PSMTXMultVec(pp->appsrt->mmtx, &v, &v);
                }
                prev_x = v.x;
                prev_y = v.y;
                prev_z = v.z;
            } else {
                prev_x = x - vx;
                prev_y = y - vy;
                prev_z = z - vz;
            }
            w0 = vmtx[2][3] + (vmtx[2][2] * z + (vmtx[2][0] * x + vmtx[2][1] * y));
            if (0.0f == w0) {
                goto no_direction;
            }
            w0inv = -1.0f / w0;
            w1 = vmtx[2][3] + (vmtx[2][2] * prev_z +
                               (vmtx[2][0] * prev_x + vmtx[2][1] * prev_y));
            if (0.0f == w1) {
                goto no_direction;
            }
            w1inv = -1.0f / w1;
            dir_x = w0inv * (pvmtx[0][3] + (pvmtx[0][2] * z +
                                            (pvmtx[0][0] * x + pvmtx[0][1] * y))) -
                    w1inv * (pvmtx[0][3] + (pvmtx[0][2] * prev_z +
                                            (pvmtx[0][0] * prev_x +
                                             pvmtx[0][1] * prev_y)));
            dir_y = w0inv * (pvmtx[1][3] + (pvmtx[1][2] * z +
                                            (pvmtx[1][0] * x + pvmtx[1][1] * y))) -
                    w1inv * (pvmtx[1][3] + (pvmtx[1][2] * prev_z +
                                            (pvmtx[1][0] * prev_x +
                                             pvmtx[1][1] * prev_y)));
        } else if (pp->kind & Tornado) {
            f32 prev_x;
            f32 prev_y;
            f32 prev_z;
            f32 dx;
            f32 dy;
            f32 dz;

            calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
            v.x = prev_x;
            v.y = prev_y;
            v.z = prev_z;
            if (pp->appsrt->x72 != 0) {
                PSMTXMultVec(mtx, &v, &v);
                PSMTXMultVec(lbl_80452DE8, &v, &v);
                v.x += pp->appsrt->translate.x;
                v.y += pp->appsrt->translate.y;
                v.z += pp->appsrt->translate.z;
            } else {
                PSMTXMultVec(pp->appsrt->mmtx, &v, &v);
            }
            dx = x - v.x;
            dy = y - v.y;
            dz = z - v.z;
            dir_x = pvmtx[0][2] * dz + (pvmtx[0][0] * dx + pvmtx[0][1] * dy);
            dir_y = pvmtx[1][2] * dz + (pvmtx[1][0] * dx + pvmtx[1][1] * dy);
        } else {
            dir_x = pvmtx[0][2] * vz + (pvmtx[0][0] * vx + pvmtx[0][1] * vy);
            dir_y = pvmtx[1][2] * vz + (pvmtx[1][0] * vx + pvmtx[1][1] * vy);
        }
        if (fabs(dir_y) < FLT_MIN) {
            angle = (dir_x >= 0.0f) ? 1.5707964f : -1.5707964f;
        } else {
            angle = atan2f(dir_x, dir_y);
        }
        if (pp->kind & DirVec) {
            angle += pp->rotate;
        }
    } else {
    no_direction:
        angle = pp->rotate;
    }
    if (fabs(angle) > 0.01) {
        Mtx rmtx;
        Vec axis;
        f32 rx = right_x;
        f32 ry = right_y;
        f32 rz = right_z;
        f32 ux = up_x;
        f32 uy = up_y;
        f32 uz = up_z;

        axis.x = ry * uz - rz * uy;
        axis.y = rz * ux - rx * uz;
        axis.z = rx * uy - ry * ux;
        PSMTXRotAxisRad(rmtx, &axis, angle);
        right_x = rmtx[0][2] * rz + (rmtx[0][0] * rx + rmtx[0][1] * ry);
        right_y = rmtx[1][2] * rz + (rmtx[1][0] * rx + rmtx[1][1] * ry);
        right_z = rmtx[2][2] * rz + (rmtx[2][0] * rx + rmtx[2][1] * ry);
        up_x = rmtx[0][2] * uz + (rmtx[0][0] * ux + rmtx[0][1] * uy);
        up_y = rmtx[1][2] * uz + (rmtx[1][0] * ux + rmtx[1][1] * uy);
        up_z = rmtx[2][2] * uz + (rmtx[2][0] * ux + rmtx[2][1] * uy);
    }
    psDispSubMakePolygon(pp, texform, x, y, z, vx, vy, vz, right_x, right_y,
                         right_z, up_x, up_y, up_z);
}

void psDispSubMakePolygon(HSD_Particle* pp, u8* texform, f32 x, f32 y, f32 z,
                          f32 vx, f32 vy, f32 vz, f32 rx, f32 ry, f32 rz,
                          f32 ux, f32 uy, f32 uz)
{
    GXColor color;

    psSetCurrentMtx(GX_PNMTX0);
    if (pp->kind & Trail) {
        f32 prev_x;
        f32 prev_y;
        f32 prev_z;

        if (pp->kind & Tornado) {
            if (pp->appsrt != NULL) {
                calcTornadoLastPosAPPSRT(pp, &prev_x, &prev_y, &prev_z);
            } else {
                calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
            }
        } else {
            prev_x = x - vx;
            prev_y = y - vy;
            prev_z = z - vz;
        }
        getClrTrail(pp, &color);
        if (texform == NULL) {
            if (pp->kind & DispTexture) {
                setVtxDesc(2);
                GXBegin(GX_QUADS, GX_VTXFMT2, 4);
            } else {
                setVtxDesc(3);
                GXBegin(GX_QUADS, GX_VTXFMT3, 4);
            }
            GXPosition3f32(prev_x - rx, prev_y - ry, prev_z - rz);
            GXColor4u8(color.r, color.g, color.b,
                       (u8) ((f32) color.a * pp->trail));
            if (pp->kind & DispTexture) {
                GXTexCoord1x8((pp->kind >> 16) & 0xC);
            }
            GXPosition3f32(x - ux, y - uy, z - uz);
            GXColor4u8(color.r, color.g, color.b, color.a);
            if (pp->kind & DispTexture) {
                GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 1);
            }
            GXPosition3f32(x + rx, y + ry, z + rz);
            GXColor4u8(color.r, color.g, color.b, color.a);
            if (pp->kind & DispTexture) {
                GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 2);
            }
            GXPosition3f32(prev_x + ux, prev_y + uy, prev_z + uz);
            GXColor4u8(color.r, color.g, color.b,
                       (u8) ((f32) color.a * pp->trail));
            if (pp->kind & DispTexture) {
                GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 3);
            }
        } else {
            f32 trail_alpha = 255.0f * (1.0f - pp->trail);
            f32 up_len = sqrtf(ux * ux + uy * uy + uz * uz);

            if (!(fabs(up_len) < FLT_MIN)) {
                f32 dy;
                f32 dx;
                f32 dz;
                f32 ratio;
                u32 num;

                dx = x - prev_x;
                dy = y - prev_y;
                dz = z - prev_z;
                dx *= dx;
                dy *= dy;
                dz *= dz;
                ratio = sqrtf(dz + (dx + dy)) / up_len;

                if (ratio < 1.0f) {
                    ratio = 1.0f;
                }
                ux *= ratio;
                uy *= ratio;
                uz *= ratio;
                num = *(u32*) texform;
                texform += 4;
                for (; num != 0; num--) {
                    u8 prim = texform[0];
                    u8 count = texform[1];
                    s32 i;

                    texform += 4;
                    if (pp->kind & DispTexture) {
                        setVtxDesc(5);
                        GXBegin(prim, GX_VTXFMT5, count);
                    } else {
                        setVtxDesc(3);
                        GXBegin(prim, GX_VTXFMT3, count);
                    }
                    for (i = 0; i < count; i++) {
                        f32 s;
                        f32 t;
                        f32 sx;
                        f32 tx;
                        s32 alpha;

                        s = *(f32*) &texform[0];
                        sx = 2.0f * (s - 0.5f);
                        if (pp->kind & TexFlipS) {
                            s = 1.0f - s;
                        }
                        t = *(f32*) &texform[4];
                        texform += 8;
                        alpha = 255.0f - t * trail_alpha;
                        if (alpha < 0) {
                            alpha = 0;
                        }
                        if (alpha > 0xFF) {
                            alpha = 0xFF;
                        }
                        tx = 2.0f * (t - 0.5f);
                        if (pp->kind & TexFlipT) {
                            t = 1.0f - t;
                        }
                        GXPosition3f32(ux * tx + (rx * sx + x),
                                       uy * tx + (ry * sx + y),
                                       uz * tx + (rz * sx + z));
                        GXColor4u8(color.r, color.g, color.b, alpha);
                        if (pp->kind & DispTexture) {
                            GXTexCoord2f32(s, t);
                        }
                    }
                }
            }
        }
    } else if (texform == NULL) {
        if (pp->kind & DispTexture) {
            setVtxDesc(0);
            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        } else {
            setVtxDesc(1);
            GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        }
        GXPosition3f32(x - rx, y - ry, z - rz);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8((pp->kind >> 16) & 0xC);
        }
        GXPosition3f32(x - ux, y - uy, z - uz);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 1);
        }
        GXPosition3f32(x + rx, y + ry, z + rz);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 2);
        }
        GXPosition3f32(x + ux, y + uy, z + uz);
        if (pp->kind & DispTexture) {
            GXTexCoord1x8(((pp->kind >> 16) & 0xC) + 3);
        }
    } else {
        u32 num = *(u32*) texform;

        texform += 4;
        for (; num != 0; num--) {
            u8 prim = texform[0];
            u8 count = texform[1];
            s32 i;

            texform += 4;
            if (pp->kind & DispTexture) {
                setVtxDesc(4);
                GXBegin(prim, GX_VTXFMT4, count);
            } else {
                setVtxDesc(1);
                GXBegin(prim, GX_VTXFMT1, count);
            }
            for (i = 0; i < count; i++) {
                f32 s;
                f32 t;
                f32 sx;
                f32 tx;

                s = *(f32*) &texform[0];
                sx = 2.0f * (s - 0.5f);
                if (pp->kind & TexFlipS) {
                    s = 1.0f - s;
                }
                t = *(f32*) &texform[4];
                texform += 8;
                tx = 2.0f * (t - 0.5f);
                if (pp->kind & TexFlipT) {
                    t = 1.0f - t;
                }
                GXPosition3f32(ux * tx + (rx * sx + x), uy * tx + (ry * sx + y),
                               uz * tx + (rz * sx + z));
                if (pp->kind & DispTexture) {
                    GXTexCoord2f32(s, t);
                }
            }
        }
    }
}

void psDispSub(HSD_Particle* pp, u8* texform)
{
    f32 x;
    f32 y;
    f32 z;
    f32 right_x;
    f32 up_x;
    f32 right_y;
    f32 up_y;
    f32 right_z;
    f32 up_z;
    f32 angle;
    f32 dir_x;
    f32 dir_y;
    HSD_Generator* gp;

    x = pp->pos.x;
    y = pp->pos.y;
    z = pp->pos.z;
    if (texform != NULL) {
        right_x = rvmtx[0][0] * pp->size;
        up_x = -rvmtx[0][1] * pp->size;
        right_y = rvmtx[1][0] * pp->size;
        up_y = -rvmtx[1][1] * pp->size;
        right_z = rvmtx[2][0] * pp->size;
        up_z = -rvmtx[2][1] * pp->size;
    } else {
        right_x = lbl_8047B160 * pp->size;
        up_x = lbl_8047B15C * pp->size;
        right_y = lbl_8047B158 * pp->size;
        up_y = lbl_8047B154 * pp->size;
        right_z = lbl_8047B150 * pp->size;
        up_z = lbl_8047B14C * pp->size;
    }
    gp = pp->gen;
    if (gp != NULL && (gp->posFlags & 0x20)) {
        right_x *= gp->scale.x;
        up_x *= gp->scale.x;
        right_y *= gp->scale.y;
        up_y *= gp->scale.y;
        right_z *= gp->scale.z;
        up_z *= gp->scale.z;
    }
    if ((pp->kind & Trail) || (pp->kind & DirVec)) {
        if (0.0f == prj[0]) {
            f32 prev_x;
            f32 prev_y;
            f32 prev_z;
            f32 w1inv;
            f32 w1;
            f32 w0inv;
            f32 w0;

            if (pp->kind & Tornado) {
                calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
            } else {
                prev_x = pp->pos.x - pp->vel.x;
                prev_y = pp->pos.y - pp->vel.y;
                prev_z = pp->pos.z - pp->vel.z;
            }
            w0 = vmtx[2][3] +
                 (vmtx[2][2] * pp->pos.z +
                  (vmtx[2][0] * pp->pos.x + vmtx[2][1] * pp->pos.y));
            if (0.0f == w0) {
                goto no_direction;
            }
            w0inv = -1.0f / w0;
            w1 = vmtx[2][3] + (vmtx[2][2] * prev_z +
                               (vmtx[2][0] * prev_x + vmtx[2][1] * prev_y));
            if (0.0f == w1) {
                goto no_direction;
            }
            w1inv = -1.0f / w1;
            dir_x = w0inv * (pvmtx[0][3] + (pvmtx[0][2] * pp->pos.z +
                                            (pvmtx[0][0] * pp->pos.x +
                                             pvmtx[0][1] * pp->pos.y))) -
                    w1inv * (pvmtx[0][3] + (pvmtx[0][2] * prev_z +
                                            (pvmtx[0][0] * prev_x +
                                             pvmtx[0][1] * prev_y)));
            dir_y = w0inv * (pvmtx[1][3] + (pvmtx[1][2] * pp->pos.z +
                                            (pvmtx[1][0] * pp->pos.x +
                                             pvmtx[1][1] * pp->pos.y))) -
                    w1inv * (pvmtx[1][3] + (pvmtx[1][2] * prev_z +
                                            (pvmtx[1][0] * prev_x +
                                             pvmtx[1][1] * prev_y)));
        } else if (pp->kind & Tornado) {
            f32 prev_x;
            f32 prev_y;
            f32 prev_z;
            f32 dx;
            f32 dy;
            f32 dz;

            calcTornadoLastPos(pp, &prev_x, &prev_y, &prev_z);
            dx = pp->pos.x - prev_x;
            dy = pp->pos.y - prev_y;
            dz = pp->pos.z - prev_z;
            dir_x = pvmtx[0][2] * dz + (pvmtx[0][0] * dx + pvmtx[0][1] * dy);
            dir_y = pvmtx[1][2] * dz + (pvmtx[1][0] * dx + pvmtx[1][1] * dy);
        } else {
            dir_x = pvmtx[0][2] * pp->vel.z +
                    (pvmtx[0][0] * pp->vel.x + pvmtx[0][1] * pp->vel.y);
            dir_y = pvmtx[1][2] * pp->vel.z +
                    (pvmtx[1][0] * pp->vel.x + pvmtx[1][1] * pp->vel.y);
        }
        if (fabs(dir_y) < FLT_MIN) {
            angle = (dir_x >= 0.0f) ? 1.5707964f : -1.5707964f;
        } else {
            angle = atan2f(dir_x, dir_y);
        }
        if (pp->kind & DirVec) {
            angle += pp->rotate;
        }
    } else {
    no_direction:
        angle = pp->rotate;
    }
    if (fabs(angle) > 0.01) {
        Mtx mtx;
        Vec axis;
        f32 rx = right_x;
        f32 ry = right_y;
        f32 rz = right_z;
        f32 ux = up_x;
        f32 uy = up_y;
        f32 uz = up_z;

        axis.x = ry * uz - rz * uy;
        axis.y = rz * ux - rx * uz;
        axis.z = rx * uy - ry * ux;
        PSMTXRotAxisRad(mtx, &axis, angle);
        right_x = mtx[0][2] * rz + (mtx[0][0] * rx + mtx[0][1] * ry);
        right_y = mtx[1][2] * rz + (mtx[1][0] * rx + mtx[1][1] * ry);
        right_z = mtx[2][2] * rz + (mtx[2][0] * rx + mtx[2][1] * ry);
        up_x = mtx[0][2] * uz + (mtx[0][0] * ux + mtx[0][1] * uy);
        up_y = mtx[1][2] * uz + (mtx[1][0] * ux + mtx[1][1] * uy);
        up_z = mtx[2][2] * uz + (mtx[2][0] * ux + mtx[2][1] * uy);
    }
    psDispSubMakePolygon(pp, texform, x, y, z, pp->vel.x, pp->vel.y,
                         pp->vel.z, right_x, right_y, right_z, up_x, up_y,
                         up_z);
}

typedef struct psdisp_Mtx {
    Mtx mtx;
} psdisp_Mtx;

static const f32 lbl_802738B8[20] = {
    1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
    1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
};

static const psdisp_Mtx psBillboardMtx = {
    { { 1.0F, 0.0F, 0.0F, 0.0F },
      { 0.0F, 1.0F, 0.0F, 0.0F },
      { 0.0F, 0.0F, 1.0F, 0.0F } },
};

static inline void psSetColor(GXColor* color, u8 value)
{
    color->r = value;
    color->g = value;
    color->b = value;
    color->a = value;
}

static inline void psSetupVtxFormat(s32 fmt, BOOL has_color, BOOL has_texture,
                                    s32 texture_type)
{
    GXSetVtxAttrFmt(fmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    if (has_color) {
        GXSetVtxAttrFmt(fmt, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    }
    if (has_texture) {
        GXSetVtxAttrFmt(fmt, GX_VA_TEX0, GX_TEX_ST, texture_type, 0);
    }
}

/* psDispParticles */
void psDispMain(u32 target_link, u32 sw)
{
    u32 prev_zmode;
    HSD_Particle* pp;
    s32 i;
    void* prev_image;
    u32 prev_mirror;
    s32 alpha_mode;
    u8 alpha0_ref;
    u8 alpha1_ref;
    s32 needs_setup;
    void* prev_tlut;
    HSD_Tlut tlut_obj;
    GXTexObj texobj;
    HSD_Particle* sorted_particles;
    HSD_Particle* non_edge_particles;
    psdisp_Mtx billboard_mtx;
    GXTlutObj gx_tlut_obj;
    u32 prev_tex_interp_near;
    u32 prev_kind;

    alpha_mode = 0;
    prev_tex_interp_near = 0;
    alpha0_ref = 0;
    alpha1_ref = 0xFF;
    needs_setup = 1;
    if (sw == 0) {
        if (psFrameNum < 0xFF) {
            psFrameNum += 1;
        } else {
            psFrameNum = 1;
        }
        return;
    }
    for (i = 0; i < PS_NUM_LINK; i++) {
        if (!(target_link & (1 << i))) {
            continue;
        }
        particleSort(i, &sorted_particles, &non_edge_particles);
        switch (sw) {
        case 1:
            pp = sorted_particles;
            break;
        case 2:
            pp = non_edge_particles;
            break;
        default:
            continue;
        }
        while (pp != NULL) {
            HSD_PSFormGroup* form_group;
            u8* form;

            if (sw == 1 && !(pp->kind & TexEdge)) {
                break;
            }
            if (!(pp->size < FLT_EPSILON) && !(pp->kind & Hidden)) {
                if (needs_setup != 0) {
                    prev_tlut = NULL;
                    prevPointSize = -1;
                    prev_image = NULL;
                    prevLineWidth = -1;
                    prevChanCtrl = -1;
                    psSetupTevInvalidState();
                    prev_kind &= ~DispFog;
                    prev_mirror = -1;
                    prev_zmode = -1;
                    HSD_FogSet(NULL);
                    prevChanMat.r = prevChanMat.g = prevChanMat.b = 0xFF;
                    prevChanAmb.r = prevChanAmb.g = prevChanAmb.b = 0xFF;
                    prevChanMat.a = prevChanAmb.a = 0xFF;
                    GXSetChanMatColor(GX_COLOR0A0, prevChanMat);
                    GXSetChanAmbColor(GX_COLOR0A0, prevChanAmb);
                    psSetupTevInvalidState();
                    psSetupTevCommon();
                    psSetColor(&prevColorPrim, 0xFF);
                    psSetColor(&prevColorEnv, 0);
                    psSetColor(&prevColorMat, 0xFF);
                    GXSetTevColor(GX_TEVREG0, prevColorPrim);
                    GXSetTevColor(GX_TEVREG1, prevColorEnv);
                    GXSetTevColor(GX_TEVREG2, prevColorMat);
                    prevBlendMode = -1;
                    GXSetZCompLoc(GX_FALSE);
                    HSD_CObjGetViewingMtx(HSD_CObjGetCurrent(), vmtx);
                    PSMTXInverse(vmtx, rvmtx);
                    GXGetProjectionv(prj);
                    if (0.0f == prj[0]) {
                        pvmtx[0][0] = prj[1] * vmtx[0][0] + prj[2] * vmtx[2][0];
                        pvmtx[0][1] = prj[1] * vmtx[0][1] + prj[2] * vmtx[2][1];
                        pvmtx[0][2] = prj[1] * vmtx[0][2] + prj[2] * vmtx[2][2];
                        pvmtx[0][3] = prj[1] * vmtx[0][3] + prj[2] * vmtx[2][3];
                        pvmtx[1][0] = prj[3] * vmtx[1][0] + prj[4] * vmtx[2][0];
                        pvmtx[1][1] = prj[3] * vmtx[1][1] + prj[4] * vmtx[2][1];
                        pvmtx[1][2] = prj[3] * vmtx[1][2] + prj[4] * vmtx[2][2];
                        pvmtx[1][3] = prj[3] * vmtx[1][3] + prj[4] * vmtx[2][3];
                    } else {
                        pvmtx[0][0] = prj[1] * vmtx[0][0] + prj[2];
                        pvmtx[0][1] = prj[1] * vmtx[0][1] + prj[2];
                        pvmtx[0][2] = prj[1] * vmtx[0][2] + prj[2];
                        pvmtx[0][3] = prj[1] * vmtx[0][3] + prj[2];
                        pvmtx[1][0] = prj[3] * vmtx[1][0] + prj[4];
                        pvmtx[1][1] = prj[3] * vmtx[1][1] + prj[4];
                        pvmtx[1][2] = prj[3] * vmtx[1][2] + prj[4];
                        pvmtx[1][3] = prj[3] * vmtx[1][3] + prj[4];
                    }
                    lbl_8047B160 = rvmtx[0][0] + rvmtx[0][1];
                    lbl_8047B15C = rvmtx[0][0] - rvmtx[0][1];
                    lbl_8047B158 = rvmtx[1][0] + rvmtx[1][1];
                    lbl_8047B154 = rvmtx[1][0] - rvmtx[1][1];
                    lbl_8047B150 = rvmtx[2][0] + rvmtx[2][1];
                    lbl_8047B14C = rvmtx[2][0] - rvmtx[2][1];
                    GXLoadPosMtxImm(vmtx, GX_PNMTX0);
                    billboard_mtx = psBillboardMtx;
                    GXLoadPosMtxImm(billboard_mtx.mtx, GX_PNMTX1);
                    psCurrentMtx = GX_PNMTX1;
                    psSetCurrentMtx(GX_PNMTX0);
                    GXEnableTexOffsets(GX_TEXCOORD0, GX_TRUE, GX_TRUE);
                    HSD_MtxGetRotationMtx(rvmtx, lbl_80452DE8, 'z', 'x');
                    GXSetCullMode(GX_CULL_BACK);
                    GXSetArray(GX_VA_TEX0, lbl_8036BFC0, 2);
                    psSetupVtxFormat(GX_VTXFMT0, FALSE, TRUE, GX_U8);
                    psSetupVtxFormat(GX_VTXFMT1, FALSE, FALSE, GX_U8);
                    psSetupVtxFormat(GX_VTXFMT2, TRUE, TRUE, GX_U8);
                    psSetupVtxFormat(GX_VTXFMT3, TRUE, FALSE, GX_U8);
                    psSetupVtxFormat(GX_VTXFMT4, FALSE, TRUE, GX_F32);
                    psSetupVtxFormat(GX_VTXFMT5, TRUE, TRUE, GX_F32);
                    needs_setup = 0;
                }

                {
                    s32 blend_mode = (pp->kind >> 22) & 3;

                    if (prevBlendMode != blend_mode) {
                        prevBlendMode = blend_mode;
                        switch (blend_mode) {
                        case 0:
                            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA,
                                           GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                            break;
                        case 1:
                            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA,
                                           GX_BL_ONE, GX_LO_CLEAR);
                            break;
                        case 2:
                            GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA,
                                           GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                            break;
                        case 3:
                            GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTCLR,
                                           GX_BL_ZERO, GX_LO_CLEAR);
                            break;
                        default:
                            OSReport("Particle:setBlendMode:Unknown mode\n");
                            break;
                        }
                    }
                }

                {
                    u8 alpha0;
                    u8 alpha1;

                    if (pp->aCmpCount != 0) {
                        s32 scale = (65536 * pp->aCmpRemain) / pp->aCmpCount;

                        alpha0 = ((pp->aCmpParam1Target << 16) +
                                  scale * (pp->aCmpParam1 -
                                           pp->aCmpParam1Target)) >>
                                 16;
                        alpha1 = ((pp->aCmpParam2Target << 16) +
                                  scale * (pp->aCmpParam2 -
                                           pp->aCmpParam2Target)) >>
                                 16;
                    } else {
                        alpha0 = pp->aCmpParam1;
                        alpha1 = pp->aCmpParam2;
                    }
                    if (alpha_mode != pp->aCmpMode || alpha0_ref != alpha0 ||
                        alpha1_ref != alpha1)
                    {
                        alpha_mode = pp->aCmpMode;
                        alpha0_ref = alpha0;
                        alpha1_ref = alpha1;
                        GXSetAlphaCompare((alpha_mode >> 3) & 7, alpha0_ref,
                                          (alpha_mode >> 6) & 3, alpha_mode & 7,
                                          alpha1_ref);
                    }
                }

                psSetupTev(pp);
                {
                    u32 chan_state = pp->kind & (DispLighting | Trail);

                    if (chan_state != prevChanCtrl) {
                        prevChanCtrl = chan_state;
                        GXSetNumChans(1);
                        switch (prevChanCtrl) {
                        case Trail:
                            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX,
                                          GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
                            break;
                        case DispLighting:
                            GXSetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG,
                                          GX_SRC_REG,
                                          HSD_LObjGetLightMaskDiffuse(),
                                          GX_DF_NONE,
                                          HSD_LObjGetLightMaskAttnFunc()
                                              ? GX_AF_SPOT
                                              : GX_AF_NONE);
                            GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG,
                                          GX_SRC_REG, 0, GX_DF_NONE,
                                          GX_AF_NONE);
                            break;
                        case DispLighting | Trail:
                            GXSetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG,
                                          GX_SRC_REG,
                                          HSD_LObjGetLightMaskDiffuse(),
                                          GX_DF_NONE,
                                          HSD_LObjGetLightMaskAttnFunc()
                                              ? GX_AF_SPOT
                                              : GX_AF_NONE);
                            GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_VTX,
                                          GX_SRC_VTX, 0, GX_DF_NONE,
                                          GX_AF_NONE);
                            break;
                        default:
                            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG,
                                          GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
                            break;
                        }
                        GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG,
                                      GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
                    }
                }
                setupChanReg(pp);
                setupTevReg(pp);
                if ((pp->kind & (NoZComp | TexEdge)) != prev_zmode) {
                    prev_zmode = pp->kind & (NoZComp | TexEdge);
                    if (prev_zmode & TexEdge) {
                        GXSetZMode((pp->kind & NoZComp) ? GX_FALSE : GX_TRUE, GX_LEQUAL, GX_TRUE);
                    } else {
                        GXSetZMode((pp->kind & NoZComp) ? GX_FALSE : GX_TRUE, GX_LEQUAL, GX_FALSE);
                    }
                }
                if ((pp->kind ^ prev_kind) & DispFog) {
                    if (pp->kind & DispFog) {
                        HSD_FogSet(psFog);
                    } else {
                        HSD_FogSet(NULL);
                    }
                }

                if (lbl_804528C8[pp->bank] != NULL &&
                    (form_group = lbl_804528C8[pp->bank][pp->texGroup]) != NULL &&
                    form_group->formTable != NULL)
                {
                    form = form_group->formTable[pp->poseNum];
                } else {
                    form = NULL;
                }

                if (pp->kind & DispTexture) {
                    HSD_PSTexGroup* tex_group;
                    u8** tex_table;
                    s32 fmt;
                    u32 width;
                    u32 height;
                    void* image;
                    s32 wrap_s;
                    s32 wrap_t;
                    f32 scale_s;
                    f32 scale_t;

                    if (pp->kind & MirrorS) {
                        scale_s = 2.0f;
                        wrap_s = GX_MIRROR;
                    } else {
                        scale_s = 1.0f;
                        wrap_s = GX_CLAMP;
                    }
                    if (pp->kind & MirrorT) {
                        scale_t = 2.0f;
                        wrap_t = GX_MIRROR;
                    } else {
                        scale_t = 1.0f;
                        wrap_t = GX_CLAMP;
                    }
                    if ((pp->kind & (MirrorS | MirrorT)) != prev_mirror) {
                        Mtx temp_mtx;

                        prev_mirror = pp->kind & (MirrorS | MirrorT);
                        prev_image = NULL;
                        PSMTXScale(temp_mtx, scale_s, scale_t, 1.0f);
                        if (pp->kind & MirrorT) {
                            temp_mtx[1][3] = 1.0f;
                        }
                        GXLoadTexMtxImm(temp_mtx, GX_TEXMTX0, GX_MTX2x4);
                        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0,
                                          GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
                    }
                    tex_group = lbl_804529C8[pp->bank][pp->texGroup];
                    if (tex_group != NULL) {
                        fmt = tex_group->fmt;
                        tex_table = tex_group->texTable;
                        width = tex_group->width;
                        height = tex_group->height;
                    } else {
                        fmt = 0;
                        height = 0;
                        width = 0;
                        tex_table = NULL;
                    }
                    if (tex_table != NULL) {
                        image = tex_table[pp->poseNum];
                    } else {
                        image = NULL;
                    }
                    if (fmt == GX_TF_C4 || fmt == GX_TF_C8) {
                        if (tex_table != NULL) {
                            void** palettes = (void**) &tex_table[tex_group->num];

                            if (palettes != NULL) {
                                void* tlut;

                                if (pp->palNum != 0xFF) {
                                    tlut = palettes[pp->palNum];
                                } else if (!(pp->kind & ComTLUT)) {
                                    tlut = palettes[pp->poseNum];
                                } else {
                                    tlut = palettes[0];
                                }
                                if (tlut != prev_tlut) {
                                    /* RULE-EXCEPTION(title-path): pointer alias
                                     * used only to keep tlut_obj in memory (retail
                                     * and XD store fmt/tlut_name at 0x44/0x48 and
                                     * reload them) — see docs/RULE_EXCEPTIONS.md */
                                    HSD_Tlut* t = &tlut_obj;

                                    t->fmt = tex_group->tlutfmt;
                                    t->tlut_name = GX_TLUT0;
                                    t->n_entries =
                                        (fmt == GX_TF_C4) ? 0x10 : 0x100;
                                    GXInitTlutObj(&gx_tlut_obj, tlut, t->fmt,
                                                  t->n_entries);
                                    GXLoadTlut(&gx_tlut_obj, t->tlut_name);
                                }
                                prev_image = NULL;
                            }
                        }
                    }
                    if (prev_image != image && image != NULL) {
                        prev_image = image;
                        switch (fmt) {
                        case GX_TF_C4:
                        case GX_TF_C8:
                            GXInitTexObjCI(&texobj, image, width, height, fmt,
                                           wrap_s, wrap_t, GX_FALSE, GX_TLUT0);
                            break;
                        case GX_TF_I4:
                        case GX_TF_I8:
                        case GX_TF_IA4:
                        case GX_TF_IA8:
                        case GX_TF_RGB565:
                        case GX_TF_RGB5A3:
                        case GX_TF_RGBA8:
                        case GX_TF_CMPR:
                            GXInitTexObj(&texobj, image, width, height, fmt,
                                         wrap_s, wrap_t, GX_FALSE);
                            break;
                        default:
                            HSD_ASSERT(1935, 0);
                            break;
                        }
                        prev_tex_interp_near = pp->kind & TexInterpNear;
                        GXInitTexObjLOD(&texobj,
                                        (prev_tex_interp_near != 0) ? GX_NEAR
                                                                    : GX_LINEAR,
                                        ((pp->kind >> 9) & 1) ? GX_NEAR : GX_LINEAR,
                                        0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE,
                                        GX_ANISO_1);
                        GXLoadTexObj(&texobj, GX_TEXMAP0);
                    }
                    if (prev_tex_interp_near != (pp->kind & TexInterpNear)) {
                        prev_tex_interp_near = pp->kind & TexInterpNear;
                        GXInitTexObjLOD(&texobj,
                                        (prev_tex_interp_near != 0) ? GX_NEAR
                                                                    : GX_LINEAR,
                                        (prev_tex_interp_near != 0) ? GX_NEAR
                                                                    : GX_LINEAR,
                                        0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE,
                                        GX_ANISO_1);
                        GXLoadTexObj(&texobj, GX_TEXMAP0);
                    }
                }

                if (pp->kind & DispPoint) {
                    if (pp->appsrt != NULL) {
                        psDispSubAPPSRTPoint(pp);
                    } else if (pp->kind & Trail) {
                        pp = psDispSubPointTrail(pp);
                    } else {
                        u8 w;

                        psSetCurrentMtx(GX_PNMTX0);
                        w = (pp->size > 42.5) ? 255.0f : 6.0f * pp->size;
                        if (prevPointSize != (s32) w) {
                            prevPointSize = w;
                            GXSetPointSize(w, GX_TO_ONE);
                        }
                        if (pp->kind & DispTexture) {
                            setVtxDesc(0);
                            GXBegin(GX_POINTS, GX_VTXFMT0, 1);
                        } else {
                            setVtxDesc(1);
                            GXBegin(GX_POINTS, GX_VTXFMT1, 1);
                        }
                        GXPosition3f32(pp->pos.x, pp->pos.y, pp->pos.z);
                        if (pp->kind & DispTexture) {
                            GXTexCoord1x8(1);
                        }
                    }
                } else if (pp->appsrt != NULL) {
                    psDispSubAppSRT(pp, form);
                } else {
                    psDispSub(pp, form);
                }
            }
            prev_kind = pp->kind;
            pp = pp->next;
        }
    }
    if (needs_setup == 0) {
        HSD_StateInvalidate(-1);
    }
}
