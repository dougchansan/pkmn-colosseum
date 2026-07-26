/**
 * @file hsd_texp.c
 * @brief HSD TExp - Texture expression system and render pipeline.
 *
 * Address range: 0x801B4240 - 0x801BB4C4
 * Contains the TExp compilation system, material render pipeline,
 * and the core rendering dispatch for textures and materials.
 * This is the second half of the TExp system (first half in hsd_tev.c).
 *
 * Decompiled from Melee src/sysdolphin/baselib/texp.c / gobjproc.c
 */

#include "dolphin/types.h"
#include "crt/string.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_objalloc.h"
#include "hsd/hsd_sdata2.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_gobj.h"
#include "hsd/hsd_lobj.h"

/* hsdAllocMemPiece/hsdFreeMemPiece declared in hsd_class.h with s32 */
extern void* hsdNew(HSD_ClassInfo* info);
extern void HSD_JObjDispAll(void* jobj, f32 mtx[3][4], s32 flags);
extern f64 __frsqrte(f64 value);
extern const f32 lbl_80478AC0[];
extern const volatile f32 lbl_8047DE00;
extern const volatile f32 lbl_8047DE18;
extern const volatile f32 lbl_8047DE1C;
extern const volatile f64 lbl_8047DE20;
extern const volatile f64 lbl_8047DE28;
extern const volatile f64 lbl_8047DE30;
extern const volatile f32 lbl_8047DE38;
extern const volatile f32 lbl_8047DE3C;
extern const volatile f32 lbl_8047DE40;
extern const f32 lbl_8047DE50;
extern const f32 lbl_8047DE54;
extern const f32 lbl_8047DE58;

typedef union SplineFloatShape {
    f32 value;
    u32 bits;
} SplineFloatShape;

struct HSD_Shadow {
    void* objects;
    HSD_CObj* camera;
    HSD_TObj* texture;
    f32 scaleS;
    f32 scaleT;
    f32 transS;
    f32 transT;
    BOOL active;
    u8 intensity;
    u8 pad_21[3];
    void* user_data;
};

extern HSD_ObjAllocData lbl_804656E0;

#pragma push
#pragma optimization_level 1
static inline HSD_TObj* makeShadowTObj(void)
{
    HSD_TObj* shadowTObj;

    shadowTObj = HSD_TObjAlloc();
    shadowTObj->src = 0;
    shadowTObj->wrap_s = 0;
    shadowTObj->wrap_t = 0;
    shadowTObj->flags = 0x540103;
    shadowTObj->imagedesc = HSD_ImageDescAlloc();
    return shadowTObj;
}

HSD_Shadow* fn_801B1730(void)
{
    HSD_Shadow* shadow;

    shadow = HSD_ObjAlloc(&lbl_804656E0);
    memset(shadow, 0, sizeof(HSD_Shadow));
    shadow->camera = HSD_CObjAlloc();
    shadow->texture = makeShadowTObj();

    shadow->scaleS = lbl_8047DDF0;
    shadow->scaleT = lbl_8047DDF4;
    shadow->transS = lbl_8047DDF0;
    shadow->transT = lbl_8047DDF0;
    shadow->intensity = 0;

    shadow->texture->imagedesc->format = 0;
    shadow->texture->imagedesc->width = 256;
    shadow->texture->imagedesc->height = 256;
    shadow->texture->imagedesc->image_ptr = NULL;

    HSD_CObjSetViewportfx4(shadow->camera, lbl_8047DDC0,
                           lbl_8047DDF8[0], lbl_8047DDC0,
                           lbl_8047DDF8[0]);
    HSD_CObjSetScissorx4(shadow->camera, 0, 256, 0, 256);

    return shadow;
}
#pragma pop

#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(10000)

static inline f32 splSqrt(f32 value)
{
    volatile SplineFloatShape shape;
    u32 exponent;
    s32 fpclass;

    if (value > lbl_8047DE00) {
        f64 guess = __frsqrte(value);
        guess = lbl_8047DE20 * guess *
                (lbl_8047DE28 - value * (guess * guess));
        guess = lbl_8047DE20 * guess *
                (lbl_8047DE28 - value * (guess * guess));
        guess = lbl_8047DE20 * guess *
                (lbl_8047DE28 - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < lbl_8047DE30) {
        return lbl_80478AC0[0];
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
        break;
    case 0:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

static inline f32 splArcLengthPolynomial(const f32 coeffs[5], f32 t)
{
    f32 t2 = t * t;
    f32 t3 = t2 * t;
    f32 t4 = t3 * t;
    f32 result = (coeffs[0] * t4) + (coeffs[1] * t3) +
                 (coeffs[2] * t2) + (coeffs[3] * t) + coeffs[4];

    if (result < lbl_8047DE00 && result > lbl_8047DE1C) {
        result = lbl_8047DE00;
    }
    return splSqrt(result);
}

static inline f32 splIterateSimpsonsMiddle(const f32 coeffs[5], f32 dx,
                                           f32 t)
{
    f32 sum = lbl_8047DE00;
    s32 i;

    for (i = 2; i <= 8; i++) {
        if (!(i & 1)) {
            sum += lbl_8047DE38 * splArcLengthPolynomial(coeffs, t);
        } else {
            sum += lbl_8047DE3C * splArcLengthPolynomial(coeffs, t);
        }
        t += dx;
    }
    return sum;
}

f32 fn_801B1AD0(const f32 coeffs[5], f32 start, f32 midpoint)
{
    f32 dx = (midpoint - start) * lbl_8047DE18;
    f32 middle = splIterateSimpsonsMiddle(coeffs, dx, start + dx);

    return dx * (middle + splArcLengthPolynomial(coeffs, start) +
                 splArcLengthPolynomial(coeffs, midpoint)) /
           lbl_8047DE40;
}

#pragma pop

typedef struct SplineVec3 {
    f32 x;
    f32 y;
    f32 z;
} SplineVec3;

struct HSD_Spline {
    u8 type;
    u8 pad_01;
    s16 numcv;
    f32 tension;
    SplineVec3* cv;
    f32 totalLength;
    f32* segLength;
    f32 (*segPoly)[5];
};

f32 fn_801B2560(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f)
{
    f32 b2 = b * b;
    f32 a2 = a * a;
    f32 b3 = b2 * b;
    f32 scaled_b2 = lbl_8047DE40 * b2;
    f32 ab2 = b2 * a;
    f32 a2b3 = a2 * b3;
    f32 a2b2 = scaled_b2 * a2;
    f32 weighted = lbl_8047DE3C * a2b3;
    f32 difference = a2b3 - ab2;
    f32 last = a2b3 - ab2;
    f32 middle = difference - ab2;
    f32 first = weighted * a;
    f32 neg_first = -first;

    first -= a2b2;
    b += middle;
    neg_first += a2b2;
    neg_first *= d;
    neg_first = c * first + neg_first;
    neg_first = e * b + neg_first;
    return f * last + neg_first;
}

static inline void ColSplGetCardinalPoint(SplineVec3* p, SplineVec3* cp,
                                          f32 tension, f32 u)
{
    f32 u2 = u * u;
    f32 u3 = u2 * u;
    f32 car0 = tension * (-u3 + 2.0F * u2 - u);
    f32 car1 = ((2.0F - tension) * u3) +
               ((tension - 3.0F) * u2) + 1.0F;
    f32 car2 = ((tension - 2.0F) * u3) +
               ((3.0F - (2.0F * tension)) * u2) + (tension * u);
    f32 car3 = tension * (u3 - u2);

    p->x = (cp[0].x * car0) + (cp[1].x * car1) +
           (cp[2].x * car2) + (cp[3].x * car3);
    p->y = (cp[0].y * car0) + (cp[1].y * car1) +
           (cp[2].y * car2) + (cp[3].y * car3);
    p->z = (cp[0].z * car0) + (cp[1].z * car1) +
           (cp[2].z * car2) + (cp[3].z * car3);
}

static inline void ColSplGetBSplinePoint(SplineVec3* p, SplineVec3* cp,
                                         f32 u)
{
    f32 u2 = u * u;
    f32 u3 = u2 * u;
    f32 u_1 = 1.0F - u;
    f32 k1_6 = 1.0F / 6.0F;
    f32 b0 = k1_6 * u_1 * u_1 * u_1;
    f32 b1 = k1_6 * (4.0F + (3.0F * u3 - 6.0F * u2));
    f32 b2 = k1_6 * (3.0F * (-u3 + u2 + u) + 1.0F);
    f32 b3 = k1_6 * u3;

    p->x = (cp[0].x * b0) + (cp[1].x * b1) +
           (cp[2].x * b2) + (cp[3].x * b3);
    p->y = (cp[0].y * b0) + (cp[1].y * b1) +
           (cp[2].y * b2) + (cp[3].y * b3);
    p->z = (cp[0].z * b0) + (cp[1].z * b1) +
           (cp[2].z * b2) + (cp[3].z * b3);
}

static inline void ColSplGetBezierPoint(SplineVec3* p, SplineVec3* cp,
                                        f32 u)
{
    f32 u_1 = 1.0F - u;
    f32 u2 = u * u;
    f32 u_12 = u_1 * u_1;
    f32 bez0 = u_12 * u_1;
    f32 bez1 = 3.0F * u * u_12;
    f32 bez2 = 3.0F * u2 * u_1;
    f32 bez3 = u2 * u;

    p->x = (cp[0].x * bez0) + (cp[1].x * bez1) +
           (cp[2].x * bez2) + (cp[3].x * bez3);
    p->y = (cp[0].y * bez0) + (cp[1].y * bez1) +
           (cp[2].y * bez2) + (cp[3].y * bez3);
    p->z = (cp[0].z * bez0) + (cp[1].z * bez1) +
           (cp[2].z * bez2) + (cp[3].z * bez3);
}

void fn_801B2038(SplineVec3* p, HSD_Spline* spline, f32 u)
{
    SplineVec3* cp;
    s16 idx;

    if (u < 0.0F || u > 1.0F) {
        return;
    }

    if (u < 1.0F) {
        f32 t = u * (spline->numcv - 1);
        idx = t;
        t -= (f32)idx;
        switch (spline->type) {
        case 0:
            cp = &spline->cv[idx];
            p->x = (t * (cp[1].x - cp[0].x)) + cp[0].x;
            p->y = (t * (cp[1].y - cp[0].y)) + cp[0].y;
            p->z = (t * (cp[1].z - cp[0].z)) + cp[0].z;
            return;
        case 1:
            cp = &spline->cv[idx * 3];
            ColSplGetBezierPoint(p, cp, t);
            return;
        case 2:
            cp = &spline->cv[idx];
            ColSplGetBSplinePoint(p, cp, t);
            return;
        case 3:
            cp = &spline->cv[idx];
            ColSplGetCardinalPoint(p, cp, spline->tension, t);
            return;
        }
    } else {
        idx = spline->numcv - 1;
        switch (spline->type) {
        case 0:
            *p = spline->cv[idx];
            return;
        case 1:
            *p = spline->cv[idx * 3];
            return;
        case 2:
            cp = &spline->cv[idx] - 1;
            ColSplGetBSplinePoint(p, cp, 1.0F);
            return;
        case 3:
            cp = &spline->cv[idx];
            *p = cp[1];
            return;
        }
    }
}

extern u8 lbl_8047B318;
extern u8 lbl_8047B319;
extern u8 lbl_8047B31A;
extern u8 lbl_8047B31B;
extern u8 lbl_8047B31C;
extern u8 lbl_8047B31D;
extern u8 lbl_8047B31E;
extern s32 lbl_8047B320;
extern s32 lbl_8047B324;
extern u8 lbl_8047B328;
extern s32 lbl_8047B32C;
extern u8 lbl_8047B330;
extern s32 lbl_8047B334;
extern u8 lbl_8047B338;
extern s32 lbl_8047B33C;
extern s32 lbl_8047B340;
extern s32 lbl_8047B344;
extern s32 lbl_8047B348;

extern void fn_800BCE30(u8 enable);
extern void fn_800BCE5C(u8 enable);
extern void GXSetDstAlpha(u8 enable, u8 alpha);
extern void GXSetBlendMode(s32 type, s32 src, s32 dst, s32 op);
extern void GXSetZMode(u8 enable, s32 func, u8 update);
extern void fn_800BCEBC(u8 beforeTex);
extern void fn_800BC618(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1);
extern void fn_800BCFDC(u8 enable);

static inline void TExpStateSetColorUpdate(s32 enable)
{
    enable = enable != 0;
    if (lbl_8047B31D != enable) {
        fn_800BCE30(enable);
        lbl_8047B31D = enable;
    }
}

static inline void TExpStateSetAlphaUpdate(s32 enable)
{
    enable = enable != 0;
    if (lbl_8047B31C != enable) {
        fn_800BCE5C(enable);
        lbl_8047B31C = enable;
    }
}

static inline void TExpStateSetDstAlpha(s32 enable, u8 alpha)
{
    enable = enable != 0;
    if (lbl_8047B31B != enable || lbl_8047B31A != alpha) {
        GXSetDstAlpha(enable, alpha);
        lbl_8047B31B = enable;
        lbl_8047B31A = alpha;
    }
}

static inline void TExpStateSetBlendMode(s32 type, s32 src, s32 dst, s32 op)
{
    if (lbl_8047B348 != type || lbl_8047B344 != src ||
        lbl_8047B340 != dst || lbl_8047B33C != op) {
        GXSetBlendMode(type, src, dst, op);
        lbl_8047B348 = type;
        lbl_8047B344 = src;
        lbl_8047B340 = dst;
        lbl_8047B33C = op;
    }
}

static inline void TExpStateSetZMode(s32 enable, s32 func, s32 update)
{
    enable = enable != 0;
    update = update != 0;
    if (lbl_8047B338 != enable || lbl_8047B334 != func ||
        lbl_8047B330 != update) {
        GXSetZMode(enable, func, update);
        lbl_8047B338 = enable;
        lbl_8047B334 = func;
        lbl_8047B330 = update;
    }
}

static inline void TExpStateSetZCompLoc(s32 beforeTex)
{
    beforeTex = beforeTex != 0;
    if (lbl_8047B319 != beforeTex) {
        fn_800BCEBC(beforeTex);
        lbl_8047B319 = beforeTex;
    }
}

static inline void TExpStateSetAlphaCompare(s32 comp0, u8 ref0, s32 op,
                                            s32 comp1, u8 ref1)
{
    if (lbl_8047B32C != comp0 || lbl_8047B328 != ref0 ||
        lbl_8047B324 != op || lbl_8047B320 != comp1 ||
        lbl_8047B31E != ref1) {
        fn_800BC618(comp0, ref0, op, comp1, ref1);
        lbl_8047B32C = comp0;
        lbl_8047B328 = ref0;
        lbl_8047B324 = op;
        lbl_8047B320 = comp1;
        lbl_8047B31E = ref1;
    }
}

static inline void TExpStateSetDither(s32 enable)
{
    enable = enable != 0;
    if (lbl_8047B318 != enable) {
        fn_800BCFDC(enable);
        lbl_8047B318 = enable;
    }
}

typedef struct HSD_TevDesc HSD_TevDesc;
struct HSD_TevDesc {
    /* 0x00 */ HSD_TevDesc* next;
    /* 0x04 */ u32 flag;
    /* 0x08 */ u32 stage;
    /* 0x0C */ u32 coord;
    /* 0x10 */ u32 map;
    /* 0x14 */ u32 color;
    /* 0x18 */ u32 color_op;
    /* 0x1C */ u32 color_a;
    /* 0x20 */ u32 color_b;
    /* 0x24 */ u32 color_c;
    /* 0x28 */ u32 color_d;
    /* 0x2C */ u32 color_scale;
    /* 0x30 */ u32 color_bias;
    /* 0x34 */ u8 color_clamp;
    /* 0x35 */ u8 pad35[3];
    /* 0x38 */ u32 color_tevreg;
    /* 0x3C */ u32 alpha_op;
    /* 0x40 */ u32 alpha_a;
    /* 0x44 */ u32 alpha_b;
    /* 0x48 */ u32 alpha_c;
    /* 0x4C */ u32 alpha_d;
    /* 0x50 */ u32 alpha_scale;
    /* 0x54 */ u32 alpha_bias;
    /* 0x58 */ u8 alpha_clamp;
    /* 0x59 */ u8 pad59[3];
    /* 0x5C */ u32 alpha_tevreg;
    /* 0x60 */ u32 pad60;
    /* 0x64 */ s32 kcolor0;
    /* 0x68 */ s32 kcolor1;
    /* 0x6C */ u32 swap0;
    /* 0x70 */ u32 swap1;
    /* 0x74 */ u32 kr;
    /* 0x78 */ u32 kg;
    /* 0x7C */ u32 kb;
    /* 0x80 */ u32 ka;
};

s32 fn_801B387C(void);
s32 HSD_StateAssignTev(void);
void fn_801B3638(HSD_TevDesc* desc);
void fn_801B3258(void);
void fn_801B3770(void);
void fn_801B3890(void);
void fn_801B2F1C(u32 flags);
void fn_801B29E4(u32 flags, HSD_PEDesc* pe);

#pragma push
#pragma optimization_level 1
static void setupTevMode_last(void)
{
    if (fn_801B387C() == 0) {
        HSD_TevDesc desc;
        desc.flag = 0;
        desc.stage = HSD_StateAssignTev();
        desc.coord = 0xFF;
        desc.map = GX_TEXMAP_NULL;
        desc.color = GX_COLOR0A0;
        desc.color_op = 4;
        fn_801B3638(&desc);
    }
}

void fn_801B294C(u32 flags, HSD_PEDesc* pe)
{
    setupTevMode_last();
    fn_801B29E4(flags, pe);
    fn_801B3258();
    fn_801B3770();
    fn_801B3890();
    fn_801B2F1C(flags);
}
#pragma pop

void fn_801B29E4(u32 flags, HSD_PEDesc* pe)
{
    s32 blendType;
    u8 zUpdate;

    if (pe != NULL) {
        TExpStateSetColorUpdate(pe->flags & 1);
        TExpStateSetAlphaUpdate(pe->flags & 2);
        TExpStateSetDstAlpha(pe->flags & 4, pe->dst_alpha);
        TExpStateSetBlendMode(pe->type, pe->src_factor, pe->dst_factor,
                              pe->logic_op);
        TExpStateSetZMode(pe->flags & 0x10, pe->z_comp,
                          pe->flags & 0x20);
        TExpStateSetZCompLoc(pe->flags & 8);
        TExpStateSetAlphaCompare(pe->alpha_comp0, pe->ref0, pe->alpha_op,
                                 pe->alpha_comp1, pe->ref1);
        TExpStateSetDither(pe->flags & 0x40);
        return;
    }

    TExpStateSetColorUpdate(1);
    TExpStateSetAlphaUpdate(0);
    TExpStateSetDstAlpha(0, 0);
    if ((flags & 0x40000000) != 0) {
        blendType = 1;
    } else {
        blendType = 0;
    }
    TExpStateSetBlendMode(blendType, 4, 5, 0xF);
    zUpdate = (flags & 0x20000000) ? 0 : 1;
    TExpStateSetZMode(1, (flags & 0x08000000) ? 7 : 3, zUpdate);
    if ((flags & 0x20000000) == 0 && (flags & 0x40000000) != 0) {
        TExpStateSetZCompLoc(0);
        TExpStateSetAlphaCompare(4, 0, 0, 4, 0);
    } else {
        TExpStateSetZCompLoc(1);
        TExpStateSetAlphaCompare(7, 0, 0, 7, 0);
    }
    TExpStateSetDither(0);
}

typedef GXColor HsdChanColor;

typedef struct HSD_Chan HSD_Chan;

struct HSD_Chan {
    HSD_Chan* next;
    s32 chan;
    u32 flags;
    HsdChanColor amb_color;
    HsdChanColor mat_color;
    u8 enable;
    u8 pad_15[3];
    s32 amb_src;
    s32 mat_src;
    s32 light_mask;
    s32 diff_fn;
    s32 attn_fn;
    void* aobj;
};

typedef struct HSD_MaterialState {
    HsdChanColor ambient;
    HsdChanColor diffuse;
    HsdChanColor specular;
    u8 alpha;
    u8 pad_0D[3];
    f32 shininess;
} HSD_MaterialState;

typedef struct HSD_ChannelModeState {
    HSD_Chan channels[6];
} HSD_ChannelModeState;

extern HSD_ChannelModeState lbl_8036CE88;
extern HSD_Chan lbl_8036D018[4];
extern HSD_Chan lbl_8036D0D8[4];
extern HSD_MaterialState lbl_80465710;
extern HsdChanColor lbl_80478C98;
extern s32 lbl_8047B360[2];
extern s32 lbl_8047B368[2];
extern void fn_800BA4C8(s32 chan, HsdChanColor color);
extern void fn_800BA5BC(s32 chan, HsdChanColor color);
extern void fn_800BA6F4(s32 chan, u8 enable, s32 ambSrc, s32 matSrc,
                        s32 lightMask, s32 diffFn, s32 attnFn);
extern void HSD_MulColor(HsdChanColor* color0, HsdChanColor* color1,
                         HsdChanColor* result);

void fn_801B28C8(HsdChanColor* ambient, HsdChanColor* diffuse,
                 HsdChanColor* specular, f32 alpha)
{
    lbl_80465710.ambient = *ambient;
    lbl_80465710.diffuse = *diffuse;
    lbl_80465710.specular = *specular;

    if (alpha <= lbl_8047DE50) {
        alpha = lbl_8047DE50;
    } else if (alpha >= lbl_8047DE54) {
        alpha = lbl_8047DE54;
    }
    lbl_80465710.alpha = lbl_8047DE58 * alpha;
}

void fn_801B3D1C(HSD_Chan* ch);
void fn_801B3AE8(s32 chan);
void HSD_StateSetNumChans(s32 n);

#pragma push
#pragma optimization_level 2
void fn_801B2F1C(u32 rendermode)
{
    HSD_Chan* const channels = lbl_8036CE88.channels;
    u32 color_mode = rendermode & 3;
    u32 alpha_mode;
    s32 num_chans = 0;
    s32 alpha_chan = 0;
    HSD_LObj* alpha_light;
    s32 max;
    s32 i;
    HSD_LObj* light;
    u8 alpha;

    if (color_mode == 0) {
        color_mode = 1;
    }
    alpha_mode = rendermode & 0x6000;
    if (alpha_mode == 0) {
        alpha_mode = color_mode << 13;
    }

    if (rendermode & 8) {
        channels[0].light_mask = HSD_LObjGetLightMaskSpecular();
        fn_801B3D1C(&channels[0]);
        num_chans = 1;
        max = HSD_LObjGetNbActive();
        for (i = 0; i < max; i++) {
            light = HSD_LObjGetActiveByIndex(i);
            if (light != NULL) {
                fn_801A6098(light, light->color, lbl_80465710.shininess);
            }
        }
    }

    if (rendermode & 4) {
        alpha_light = HSD_LObjGetActiveByID(0x100);
        if (alpha_light != NULL && (alpha_light->flags & 4)) {
            HSD_MulColor(&lbl_80465710.ambient, &alpha_light->color,
                         &channels[1].amb_color);
        } else {
            channels[1].amb_color = lbl_80478C98;
        }
        channels[1].mat_src = (color_mode >> 1) & 1;
        channels[1].light_mask = HSD_LObjGetLightMaskDiffuse();
        fn_801B3D1C(&channels[1]);

        if (alpha_mode & 0x4000) {
            channels[2].chan = 3;
            fn_801B3D1C(&channels[3]);
            alpha_chan = 1;
        } else {
            channels[2].chan = 2;
        }
        channels[2].light_mask = HSD_LObjGetLightMaskAlpha();
        if (alpha_light != NULL && (alpha_light->flags & 0x10)) {
            alpha = alpha_light->color.a;
        } else {
            alpha = 0;
        }
        if (channels[2].light_mask != 0) {
            channels[2].enable = 1;
            channels[2].mat_color.a = 0xFF;
            channels[2].amb_color.a = alpha;
        } else {
            channels[2].mat_color.a = alpha;
            channels[2].enable = 0;
        }
        fn_801B3D1C(&channels[2]);
    } else {
        channels[4].mat_src = (color_mode >> 1) & 1;
        fn_801B3D1C(&channels[4]);
        channels[5].mat_src = (alpha_mode >> 14) & 1;
        fn_801B3D1C(&channels[5]);
    }

    if (num_chans != 0) {
        if (alpha_chan == 0) {
            fn_801B3AE8(3);
        }
        HSD_StateSetNumChans(2);
    } else if (alpha_chan != 0) {
        fn_801B3AE8(1);
        HSD_StateSetNumChans(2);
    } else {
        fn_801B3AE8(5);
        HSD_StateSetNumChans(1);
    }
}
#pragma pop

static inline s32 ColCompareRGB(HsdChanColor* c0, HsdChanColor* c1)
{
    return ((*(u32*)c0 ^ *(u32*)c1) & 0xFFFFFF00) != 0;
}

static inline s32 ColCompareRGBA(HsdChanColor* c0, HsdChanColor* c1)
{
    return *(u32*)c0 != *(u32*)c1;
}

static inline void ColCopyRGB(HsdChanColor* dst, HsdChanColor* src)
{
    *(u32*)dst = (*(u32*)dst & 0xFF) | (*(u32*)src & 0xFFFFFF00);
}

void fn_801B3D1C(HSD_Chan* ch)
{
    s32 idx;
    s32 chan;
    s32 no;

    if (ch == NULL || ch->chan == 0xFF) {
        return;
    }

    chan = ch->chan;
    idx = chan & 3;
    no = chan & 1;
    if (ch->enable != 0 && ch->amb_src == 0) {
        if (lbl_8047B368[no] != 0) {
            lbl_8047B368[no] = 0;
            fn_800BA4C8(no + 4, ch->amb_color);
            lbl_8036D018[no].amb_color = ch->amb_color;
        } else if (chan == 4 || chan == 5) {
            if (ColCompareRGBA(&ch->amb_color,
                               &lbl_8036D018[no].amb_color)) {
                lbl_8036D018[no].amb_color = ch->amb_color;
                goto set_amb;
            }
        } else if (chan == 0 || chan == 1) {
            if (ColCompareRGB(&ch->amb_color,
                              &lbl_8036D018[no].amb_color)) {
                ColCopyRGB(&lbl_8036D018[no].amb_color, &ch->amb_color);
                goto set_amb;
            }
        } else if (ch->amb_color.a != lbl_8036D018[no].amb_color.a) {
            lbl_8036D018[no].amb_color.a = ch->amb_color.a;
        set_amb:
            fn_800BA4C8(chan, ch->amb_color);
        }
    }

    if (ch->mat_src == 0) {
        if (lbl_8047B360[no] != 0) {
            lbl_8047B360[no] = 0;
            fn_800BA5BC(no + 4, ch->mat_color);
            lbl_8036D018[no].mat_color = ch->mat_color;
        } else if (chan == 4 || chan == 5) {
            if (ColCompareRGBA(&ch->mat_color,
                               &lbl_8036D018[no].mat_color)) {
                lbl_8036D018[no].mat_color = ch->mat_color;
                goto set_mat;
            }
        } else if (chan == 0 || chan == 1) {
            if (ColCompareRGB(&ch->mat_color,
                              &lbl_8036D018[no].mat_color)) {
                ColCopyRGB(&lbl_8036D018[no].mat_color, &ch->mat_color);
                goto set_mat;
            }
        } else if (ch->mat_color.a != lbl_8036D018[no].mat_color.a) {
            lbl_8036D018[no].mat_color.a = ch->mat_color.a;
        set_mat:
            fn_800BA5BC(chan, ch->mat_color);
        }
    }

    if (ch->enable != lbl_8036D018[idx].enable ||
        ch->amb_src != lbl_8036D018[idx].amb_src ||
        ch->mat_src != lbl_8036D018[idx].mat_src ||
        ch->light_mask != lbl_8036D018[idx].light_mask ||
        ch->diff_fn != lbl_8036D018[idx].diff_fn ||
        ch->attn_fn != lbl_8036D018[idx].attn_fn) {
        fn_800BA6F4(chan, ch->enable, ch->amb_src, ch->mat_src,
                    ch->light_mask, ch->diff_fn, ch->attn_fn);
        lbl_8036D018[idx].enable = ch->enable;
        lbl_8036D018[idx].amb_src = ch->amb_src;
        lbl_8036D018[idx].mat_src = ch->mat_src;
        lbl_8036D018[idx].light_mask = ch->light_mask;
        lbl_8036D018[idx].diff_fn = ch->diff_fn;
        lbl_8036D018[idx].attn_fn = ch->attn_fn;
        if (chan == 4 || chan == 5) {
            lbl_8036D018[idx + 2].enable = ch->enable;
            lbl_8036D018[idx + 2].amb_src = ch->amb_src;
            lbl_8036D018[idx + 2].mat_src = ch->mat_src;
            lbl_8036D018[idx + 2].light_mask = ch->light_mask;
            lbl_8036D018[idx + 2].diff_fn = ch->diff_fn;
            lbl_8036D018[idx + 2].attn_fn = ch->attn_fn;
        }
    }
}
extern void HSD_ClearVtxDesc(void);
extern void fn_800B94F0(u32 value);
extern void fn_800BC8C8(u32 value);
extern void fn_800B884C(u32 value);

extern u8 lbl_80465588[];
extern u8 lbl_804655B4[];
extern u8 lbl_8047B350;
extern u8 lbl_8047B351;
extern s32 lbl_8047B34C;
extern s32 lbl_8047B358;
extern u32 lbl_8047B370;

/* GObj system globals */
static HSD_GObj* gobj_list[64];
static HSD_GObj* gobj_render_list[64];
static u32 gobj_num_active;
static u32 gobj_next_id;

/* ========================================================================= */
/*  TExp node management                                                     */
/* ========================================================================= */

/* BSS globals */
extern u8 lbl_80465728[];
extern u8 lbl_80465754[];
extern u8 lbl_80465780[];
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);
extern char lbl_8047DE70;
extern char lbl_8047DE90;
extern char lbl_802753DC[];
extern char lbl_802753A8[];
extern char lbl_802753B4[];
extern char lbl_802755D0[];
extern char lbl_8047DEA0;
extern void* fn_80193B10(s32 size);

/* Address: 0x801B4240 | Size: 0xC */
/* Get pointer to BSS object lbl_80465728 */
void* HSD_ChanGetAllocData(void) {
    return lbl_80465728;
}

/* Address: 0x801B424C | Size: 0xC */
/* Get pointer to BSS object lbl_80465754 */
void* HSD_TevRegGetAllocData(void) {
    return lbl_80465754;
}

/* Address: 0x801B4258 | Size: 0xC */
/* Get pointer to BSS object lbl_80465780 */
void* HSD_RenderGetAllocData(void) {
    return lbl_80465780;
}

void HSD_RenderInitAllocData(void)
{
    HSD_ObjAllocInit(lbl_80465780, 0x1C, 4);
    HSD_ObjAllocInit(lbl_80465754, 0x14, 4);
    HSD_ObjAllocInit(lbl_80465728, 0x30, 4);
}

extern void fn_80193AF0(void* ptr, s32 size);

/*
 * HSD_TExpFreeList - 0x801B42C0 | Size: 0x40
 * Walk a linked list of TExp nodes and free each one (size 0x88).
 */
void HSD_TExpFreeTevDesc(u8* node) {
    u8* next;
    while (node != NULL) {
        next = *(u8**)node;
        fn_80193AF0(node, 0x88);
        node = next;
    }
}

/* ========================================================================= */
/*  TExp compilation                                                         */
/* ========================================================================= */

/*
 * HSD_TExpCollectInputs - 0x801B4300 | Size: 0x2A4
 * Compile pass 1 - collect all texture inputs referenced
 * by the expression tree and map them to GX texture stages.
 */
void HSD_TExpCompile(u8* root, u32* tex_count, u32* ras_count) {
    u32 t_count = 0;
    u32 r_count = 0;

    if (root == NULL) {
        if (tex_count) *tex_count = 0;
        if (ras_count) *ras_count = 0;
        return;
    }

    {
        u8* node = root;
        while (node != NULL) {
            u32 type = *(u32*)(node + 0x0);
            if (type == 1) {
                t_count++;
            } else if (type == 2) {
                r_count++;
            }
            node = *(u8**)(node + 0x8);
        }
    }

    if (tex_count) *tex_count = t_count;
    if (ras_count) *ras_count = r_count;
}

void fn_800BC6F0();
void fn_800BC228();
void fn_800BC290();
void fn_800BC1A0();
void fn_800BC1E4();
void fn_800BC454();
void fn_800BC4C0();
void fn_800BC52C();
void fn_800BC580(u32, u32, u32, u32, u32);
void GXSetTevOp(u32 id, u32 op);

/*
 * HSD_SetupTevStage - 0x801B3638 | Size: 0x138
 */
#pragma optimization_level 1
void fn_801B3638(HSD_TevDesc* desc) {
    fn_800BC6F0(desc->stage, desc->coord, desc->map, desc->color);
    if (desc->flag == 0) {
        GXSetTevOp(desc->stage, desc->color_op);
        fn_800BC52C(desc->stage, 0, 0);
        return;
    }
    fn_800BC228(desc->stage, desc->color_op, desc->color_bias,
                desc->color_scale, desc->color_clamp, desc->color_tevreg);
    fn_800BC1A0(desc->stage, desc->color_a, desc->color_b, desc->color_c,
                desc->color_d);
    fn_800BC290(desc->stage, desc->alpha_op, desc->alpha_bias,
                desc->alpha_scale, desc->alpha_clamp, desc->alpha_tevreg);
    fn_800BC1E4(desc->stage, desc->alpha_a, desc->alpha_b, desc->alpha_c,
                desc->alpha_d);
    fn_800BC580(desc->kcolor0, desc->kr, desc->kg, desc->kb, desc->ka);
    if (desc->kcolor1 != desc->kcolor0) {
        fn_800BC580(desc->kcolor1, desc->kr, desc->kg, desc->kb, desc->ka);
    }
    fn_800BC52C(desc->stage, desc->kcolor0, desc->kcolor1);
    fn_800BC454(desc->stage, desc->swap0);
    fn_800BC4C0(desc->stage, desc->swap1);
}
#pragma optimization_level 4

extern s32 lbl_8047B35C;
extern u32 lbl_8047B370;
void fn_800BA6B0(u8);

typedef struct KColorEntry {
    /* 0x0 */ u32 color0;
    /* 0x4 */ u32 color1;
    /* 0x8 */ s32 dirty;
} KColorEntry;
extern KColorEntry lbl_8036CFE8[4];
void fn_800BC36C(u32 id, void* color);

/* Address: 0x801B3258 | Size: 0xE0 */
#pragma push
#pragma optimization_level 1
void fn_801B3258(void) {
    u32 i;
    u32 tmp[2];
    for (i = 0; i < 4; i++) {
        if (lbl_8036CFE8[i].dirty != 0) {
            KColorEntry* e = &lbl_8036CFE8[i];
            u32 id;
            tmp[0] = e->color0;
            tmp[1] = e->color1;
            switch (i) {
            case 0: id = 1; break;
            case 1: id = 2; break;
            case 2: id = 3; break;
            case 3: id = 0; break;
            default: id = 1; break;
            }
            fn_800BC36C(id, &tmp);
            lbl_8036CFE8[i].dirty = 0;
        }
    }
}
#pragma pop

extern char lbl_8047DE60;
extern char lbl_8047DE68;

/* HSD_Index2TevStage - 0x801B3338 | Size: 0xD0 */
#pragma push
#pragma optimization_level 1
s32 HSD_Index2TevStage(u32 index)
{
    switch (index) {
    case 0:  return 0;
    case 1:  return 1;
    case 2:  return 2;
    case 3:  return 3;
    case 4:  return 4;
    case 5:  return 5;
    case 6:  return 6;
    case 7:  return 7;
    case 8:  return 8;
    case 9:  return 9;
    case 10: return 10;
    case 11: return 11;
    case 12: return 12;
    case 13: return 13;
    case 14: return 14;
    case 15: return 15;
    default:
        __assert((const char*)&lbl_8047DE60, 0x326,
                 (const char*)&lbl_8047DE68);
        return 15;
    }
}
#pragma pop

static inline s32 HSD_TevStage2Num(s32 stage)
{
    switch (stage) {
    case 0:  return 1;
    case 1:  return 2;
    case 2:  return 3;
    case 3:  return 4;
    case 4:  return 5;
    case 5:  return 6;
    case 6:  return 7;
    case 7:  return 8;
    case 8:  return 9;
    case 9:  return 10;
    case 10: return 11;
    case 11: return 12;
    case 12: return 13;
    case 13: return 14;
    case 14: return 15;
    case 15: return 16;
    default:
        __assert((const char*)&lbl_8047DE60, 0x37A,
                 (const char*)&lbl_8047DE68);
        return 0;
    }
}

/*
 * HSD_SetupTevStageAll - 0x801B3408 | Size: 0x230
 * Configure every descriptor and publish the highest active TEV stage count.
 */
void fn_801B3408(HSD_TevDesc* desc)
{
    s32 num = 0;
    HSD_TevDesc* td;

    for (td = desc; td != NULL; td = td->next) {
        s32 stageCount = HSD_TevStage2Num(td->stage);
        if (stageCount > num) {
            num = stageCount;
        }
        fn_801B3638(td);
    }
    lbl_8047B370 = num;
    fn_800BC8C8((u8)lbl_8047B370);
    lbl_8047B370 = 0;
}

/* Address: 0x801B3770 | Size: 0x30 */


/* Address: 0x801B3AA8 | Size: 0x40 */
void HSD_StateSetNumChans(s32 n) {
    if (lbl_8047B35C != n) {
        fn_800BA6B0((u8)n);
        lbl_8047B35C = n;
    }
}

static inline s32 TExpChannel2Num(s32 chan)
{
    switch (chan) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 1;
    case 5:
        return 2;
    case 0xFF:
        return 0;
    default:
        __assert((const char*)&lbl_8047DE60, 0x2F1,
                 (const char*)&lbl_8047DE68);
        return 0;
    }
}

/* Address: 0x801B3998 | Size: 0x110 */
#pragma push
#pragma optimization_level 1
void fn_801B3998(HSD_Chan* chan)
{
    s32 num_chans = 0;

    while (chan != NULL) {
        s32 chan_count = TExpChannel2Num(chan->chan);
        if (chan_count > num_chans) {
            num_chans = chan_count;
        }
        fn_801B3D1C(chan);
        chan = chan->next;
    }
    HSD_StateSetNumChans((u8)num_chans);
}
#pragma pop

/* Address: 0x801B3AE8 | Size: 0x234 */
#pragma push
#pragma optimization_level 1
void fn_801B3AE8(s32 chan)
{
    s32 idx = chan & 3;
    s32 amb_src;
    s32 mat_src;
    s32 diff_fn;
    s32 attn_fn;

    switch (chan) {
    case 4:
    case 5:
        if (lbl_8036D018[idx].enable != 0 ||
            lbl_8036D018[idx + 2].enable != 0) {
            lbl_8036D018[idx].enable =
                lbl_8036D018[idx + 2].enable = 0;
            lbl_8036D018[idx].light_mask =
                lbl_8036D018[idx + 2].light_mask = 0;
            amb_src = lbl_8036D018[idx].amb_src;
            mat_src = lbl_8036D018[idx].mat_src;
            diff_fn = lbl_8036D018[idx].diff_fn;
            attn_fn = lbl_8036D018[idx].attn_fn;
            lbl_8036D018[idx + 2].amb_src = amb_src;
            lbl_8036D018[idx + 2].mat_src = mat_src;
            lbl_8036D018[idx + 2].diff_fn = diff_fn;
            lbl_8036D018[idx + 2].attn_fn = attn_fn;
            fn_800BA6F4(chan, 0, amb_src, mat_src, 0, diff_fn, attn_fn);
        }
        break;
    default:
        if (lbl_8036D018[idx].enable != 0) {
            lbl_8036D018[idx].enable = 0;
            lbl_8036D018[idx].light_mask = 0;
            amb_src = lbl_8036D018[idx].amb_src;
            mat_src = lbl_8036D018[idx].mat_src;
            diff_fn = lbl_8036D018[idx].diff_fn;
            attn_fn = lbl_8036D018[idx].attn_fn;
            fn_800BA6F4(chan, 0, amb_src, mat_src, 0, diff_fn, attn_fn);
        }
        break;
    }
}
#pragma pop

typedef struct ColTExpNode ColTExpNode;

typedef struct ColTEArg {
    u8 type;
    u8 sel;
    u8 arg;
    u8 pad_03;
    ColTExpNode* exp;
} ColTEArg;

struct ColTExpNode {
    s32 type;
    ColTExpNode* next;
    s32 c_ref;
    u8 c_dst;
    u8 c_op;
    u8 c_clamp;
    u8 c_bias;
    u8 c_scale;
    u8 c_range;
    u8 pad_12[2];
    s32 a_ref;
    u8 a_dst;
    u8 a_op;
    u8 a_clamp;
    u8 a_bias;
    u8 a_scale;
    u8 a_range;
    u8 tex_swap;
    u8 ras_swap;
    u8 kcsel;
    u8 kasel;
    u8 pad_22[2];
    s32 input_index[4];
    ColTEArg c_in[4];
    ColTEArg a_in[4];
    HSD_TObj* tex;
    u8 chan;
};

#define COL_TE_ZERO 0
#define COL_TE_TEV 1
#define COL_TE_TEX 2
#define COL_TE_RAS 3
#define COL_TE_CNST 4
#define COL_TE_RGB 1
#define COL_TE_A 5
#define COL_TE_0 7

static inline s32 ColTExpGetType(ColTExpNode* exp)
{
    if (exp == NULL) {
        return COL_TE_ZERO;
    }
    if ((u32) exp == -1U) {
        return COL_TE_TEX;
    }
    if ((u32) exp == -2U) {
        return COL_TE_RAS;
    }
    return exp->type;
}

struct _HSD_TExpTevDesc {
    HSD_TevDesc desc;
    HSD_TObj* tobj;
};

typedef struct ColTECnst {
    s32 type;
    ColTExpNode* next;
    void* val;
    u8 comp;
    u8 ctype;
    u8 reg;
    u8 idx;
    u8 ref;
    u8 range;
} ColTECnst;

typedef struct ColTExpRes {
    s32 failed;
    s32 texmap;
    s32 cnst_remain;
    struct {
        u8 color;
        u8 alpha;
    } reg[8];
    u8 c_ref[4];
    u8 a_ref[4];
    u8 c_use[4];
    u8 a_use[4];
} ColTExpRes;

void fn_801B750C(ColTExpNode* texp, u8 sel);
extern s32 lbl_8036D278[4];
extern s32 lbl_8036D288[4][4];
extern s32 lbl_8036D2C8[4];
extern s32 lbl_8036D2D8[4][4];

extern void fn_800B8E74(void);
extern void fn_800BC3E0(u32 id, GXColor color);
extern void fn_800BC2F8(u32 id, GXColor color);
extern void fn_801B25C4(u32 flags);

void HSD_TExpSetReg(HSD_TExp* texp);

/* HSD_TExpSetupTev - 0x801B45A4 | Size: 0x70 */
void fn_801B45A4(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp) {
    HSD_TExpSetReg(texp);
    while (tevdesc != NULL) {
        if (tevdesc->tobj != NULL) {
            tevdesc->desc.map = tevdesc->tobj->id;
            tevdesc->desc.coord = tevdesc->tobj->coord;
        }
        HSD_StateAssignTev();
        fn_801B3638(&tevdesc->desc);
        tevdesc = (HSD_TExpTevDesc*)tevdesc->desc.next;
    }
}

/*
 * HSD_TExpSetReg - 0x801B4614 | Size: 0x548
 */
void HSD_TExpSetReg(HSD_TExp* texp)
{
    static u32 id1[4] = { 0, 1, 2, 3 };
    static u32 id2[3] = { 1, 2, 3 };
    s32 i;
    GXColor reg[8];
    u32 changed;
    ColTECnst* clist;

    clist = (ColTECnst*) texp;
    changed = 0;
    while (clist != NULL) {
        HSD_ASSERT(0x5AD, clist->type == COL_TE_CNST);
        if (clist->reg < 8) {
            changed |= 1 << clist->reg;
            if (clist->comp == COL_TE_RGB) {
                switch (clist->ctype) {
                case 0: {
                    GXColor col = *(GXColor*) clist->val;
                    col.a = reg[clist->reg].a;
                    reg[clist->reg] = col;
                    break;
                }
                case 2: {
                    u32* ptr = clist->val;
                    reg[clist->reg].r = ptr[0] < 0x100 ? ptr[0] : 0xFF;
                    reg[clist->reg].g = ptr[1] < 0x100 ? ptr[1] : 0xFF;
                    reg[clist->reg].b = ptr[2] < 0x100 ? ptr[2] : 0xFF;
                    break;
                }
                default: {
                    s32 r;
                    s32 g;
                    s32 b;
                    switch (clist->ctype) {
                    case 1: {
                        u16* ptr = clist->val;
                        r = ptr[0];
                        g = ptr[1];
                        b = ptr[2];
                        break;
                    }
                    case 3: {
                        f32* ptr = clist->val;
                        r = 255.0F * ptr[0];
                        g = 255.0F * ptr[1];
                        b = 255.0F * ptr[2];
                        break;
                    }
                    default: {
                        f64* ptr = clist->val;
                        r = 255.0 * ptr[0];
                        g = 255.0 * ptr[1];
                        b = 255.0 * ptr[2];
                        break;
                    }
                    }
                    reg[clist->reg].r = r > 0xFF ? 0xFF : r < 0 ? 0 : r;
                    reg[clist->reg].g = g > 0xFF ? 0xFF : g < 0 ? 0 : g;
                    reg[clist->reg].b = b > 0xFF ? 0xFF : b < 0 ? 0 : b;
                    break;
                }
                }
            } else {
                s32 x;
                u8 val;
                switch (clist->ctype) {
                case 0:
                    x = *(u8*) clist->val;
                    val = x;
                    break;
                case 1:
                    x = *(u16*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                case 2:
                    x = *(u32*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                case 3:
                    x = 255.0F * *(f32*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                default:
                    x = 255.0 * *(f64*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                }
                if (clist->reg < 4) {
                    switch (clist->idx) {
                    case 0:
                        reg[clist->reg].r = val;
                        break;
                    case 1:
                        reg[clist->reg].g = val;
                        break;
                    case 2:
                        reg[clist->reg].b = val;
                        break;
                    default:
                        reg[clist->reg].a = val;
                        break;
                    }
                } else if (clist->idx == 3) {
                    reg[clist->reg].a = val;
                } else {
                    reg[clist->reg].r = val;
                    reg[clist->reg].g = val;
                    reg[clist->reg].b = val;
                }
            }
        }
        clist = (ColTECnst*) clist->next;
    }
    if (changed != 0) {
        fn_800B8E74();
        for (i = 0; i < 4; i++) {
            if (changed & (1 << i)) {
                fn_800BC3E0(id1[i], reg[i]);
            }
        }
        for (i = 4; i < 7; i++) {
            if (changed & (1 << i)) {
                fn_800BC2F8(id2[i - 4], reg[i]);
            }
        }
        fn_801B25C4(0x10);
        fn_800B8E74();
    }
}

/*
 * HSD_TExpOptimize - 0x801B4B5C | Size: 0x564
 * Compile pass 3 - optimize the generated TEV stages.
 * Merges stages where possible, removes redundant operations,
 * and minimizes register usage.
 */
void TExp2TevDesc(ColTExpNode* texp, HSD_TExpTevDesc* desc,
                  s32* init_cprev, s32* init_aprev)
{
    static u32 dst[4] = { 1, 2, 3, 0 };
    ColTExpNode* tev;
    HSD_TevDesc* tevdesc;

    HSD_ASSERT(0x52C, texp);
    HSD_ASSERT(0x52D, desc);
    HSD_ASSERT(0x52E, ColTExpGetType(texp) == COL_TE_TEV);

    tev = texp;
    tevdesc = &desc->desc;
    tevdesc->next = NULL;
    tevdesc->flag = 1;
    desc->tobj = tev->tex;
    if (tev->tex == NULL) {
        tevdesc->coord = 0xFF;
        tevdesc->map = 0xFF;
    }
    tevdesc->color = tev->chan == 0xFF ? 0xFF : tev->chan;
    tevdesc->swap0 = tev->ras_swap == 0xFF ? 0 : tev->ras_swap;
    tevdesc->swap1 = tev->tex_swap == 0xFF ? 0 : tev->tex_swap;
    tevdesc->kcolor0 = tev->kcsel == 0xFF ? 0 : tev->kcsel;
    tevdesc->kcolor1 = tev->kasel == 0xFF ? 0 : tev->kasel;

    if (tev->c_op == 0xFF ||
        (tev->c_ref == 0 && tev->a_op != 8 && tev->a_op != 9 &&
         tev->a_op != 10 && tev->a_op != 11 && tev->a_op != 12 &&
         tev->a_op != 13)) {
        tevdesc->color_op = GX_TEV_ADD;
        tevdesc->color_a = GX_CC_ZERO;
        tevdesc->color_b = GX_CC_ZERO;
        tevdesc->color_c = GX_CC_ZERO;
        if (*init_cprev != 0) {
            *init_cprev = 0;
            tevdesc->color_d = GX_CC_ZERO;
        } else {
            tevdesc->color_d = GX_CC_CPREV;
        }
        tevdesc->color_scale = 0;
        tevdesc->color_bias = 0;
        tevdesc->color_clamp = 0;
        tevdesc->color_tevreg = 0;
    } else {
        tevdesc->color_op = tev->c_op;
        tevdesc->color_a =
            tev->c_in[0].arg == 0xFF ? GX_CC_ZERO : tev->c_in[0].arg;
        tevdesc->color_b =
            tev->c_in[1].arg == 0xFF ? GX_CC_ZERO : tev->c_in[1].arg;
        tevdesc->color_c =
            tev->c_in[2].arg == 0xFF ? GX_CC_ZERO : tev->c_in[2].arg;
        tevdesc->color_d =
            tev->c_in[3].arg == 0xFF ? GX_CC_ZERO : tev->c_in[3].arg;
        tevdesc->color_scale = tev->c_scale == 0xFF ? 0 : tev->c_scale;
        tevdesc->color_bias = tev->c_bias == 0xFF ? 0 : tev->c_bias;
        tevdesc->color_clamp = tev->c_clamp != 0 ? GX_TRUE : GX_FALSE;
        HSD_ASSERT(0x56D, tev->c_dst != 0xFF);
        tevdesc->color_tevreg = dst[tev->c_dst];
        if (tevdesc->color_tevreg == 0) {
            *init_cprev = 0;
        }
    }

    if (tev->a_op == 0xFF || tev->a_ref == 0) {
        tevdesc->alpha_op = GX_TEV_ADD;
        tevdesc->alpha_a = GX_CA_ZERO;
        tevdesc->alpha_b = GX_CA_ZERO;
        tevdesc->alpha_c = GX_CA_ZERO;
        if (*init_aprev != 0) {
            *init_aprev = 0;
            tevdesc->alpha_d = GX_CA_ZERO;
        } else {
            tevdesc->alpha_d = GX_CA_APREV;
        }
        tevdesc->alpha_scale = 0;
        tevdesc->alpha_bias = 0;
        tevdesc->alpha_clamp = 0;
        tevdesc->alpha_tevreg = 0;
    } else {
        tevdesc->alpha_op = tev->a_op;
        tevdesc->alpha_a =
            tev->a_in[0].arg == 0xFF ? GX_CA_ZERO : tev->a_in[0].arg;
        tevdesc->alpha_b =
            tev->a_in[1].arg == 0xFF ? GX_CA_ZERO : tev->a_in[1].arg;
        tevdesc->alpha_c =
            tev->a_in[2].arg == 0xFF ? GX_CA_ZERO : tev->a_in[2].arg;
        tevdesc->alpha_d =
            tev->a_in[3].arg == 0xFF ? GX_CA_ZERO : tev->a_in[3].arg;
        tevdesc->alpha_scale = tev->a_scale == 0xFF ? 0 : tev->a_scale;
        tevdesc->alpha_bias = tev->a_bias == 0xFF ? 0 : tev->a_bias;
        tevdesc->alpha_clamp = tev->a_clamp != 0 ? GX_TRUE : GX_FALSE;
        HSD_ASSERT(0x591, tev->a_dst != 0xFF);
        tevdesc->alpha_tevreg = dst[tev->a_dst];
        if (tevdesc->alpha_tevreg == 0) {
            *init_aprev = 0;
        }
    }
    tevdesc->pad60 = 0;
}

/* ========================================================================= */
/*  Material render pipeline                                                 */
/* ========================================================================= */

/*
 * HSD_MaterialSetupTEV - 0x801B50C0 | Size: 0x790
 */
static inline s32 AssignAlphaRegInline(ColTExpNode* tev, s32 idx,
                                       ColTExpRes* res)
{
    ColTECnst* cnst = (ColTECnst*) tev->a_in[idx].exp;
    s32 i;

    if (cnst->reg != 0xFF) {
        if (cnst->reg < 4) {
            return -1;
        }
        tev->a_in[idx].type = 5;
        tev->a_in[idx].arg = lbl_8036D278[cnst->reg - 4];
        return 0;
    }

    for (i = 4; i < 8; i++) {
        if (res->reg[i].alpha == 0) {
            res->reg[i].alpha = 1;
            cnst->reg = i;
            cnst->idx = 3;
            tev->a_in[idx].type = 5;
            tev->a_in[idx].arg = lbl_8036D278[i - 4];
            return 0;
        }
    }
    return -1;
}

static inline s32 IsThroughColorInline(ColTExpNode* tev)
{
    return tev->c_op == 0 && tev->c_in[0].sel == 7 &&
           tev->c_in[1].sel == 7 && tev->c_bias == 0 && tev->c_scale == 0;
}

static inline s32 IsThroughAlphaInline(ColTExpNode* tev)
{
    return tev->a_op == 0 && tev->a_in[0].sel == 7 &&
           tev->a_in[1].sel == 7 && tev->a_bias == 0 && tev->a_scale == 0;
}

extern s32 fn_801B5850(ColTExpNode*, s32, ColTExpRes*);
extern s32 fn_801B5A00(ColTExpNode*, s32, ColTExpRes*);
extern s32 fn_801B5C94(ColTExpNode*, s32, ColTExpRes*);

s32 fn_801B50C0(ColTExpNode* tev, ColTExpRes* res)
{
    s32 i;
    s32 result;

    if (tev->c_ref > 0) {
        if (tev->kcsel != 0xFF) {
            for (i = 0; i < 4; i++) {
                if (tev->c_in[i].type == COL_TE_CNST) {
                    result = fn_801B5C94(tev, i, res);
                    if (result < 0) {
                        __assert(&lbl_8047DE70, 0x49C, lbl_802753B4);
                        return result;
                    }
                }
            }
        } else if (IsThroughColorInline(tev) &&
                   tev->c_in[3].type == COL_TE_CNST) {
            result = fn_801B5C94(tev, 3, res);
            if (result < 0) {
                result = fn_801B5A00(tev, 3, res);
                if (result < 0) {
                    __assert(&lbl_8047DE70, 0x4A8, lbl_802753B4);
                    return result;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (tev->c_in[i].type == COL_TE_CNST) {
                    result = fn_801B5A00(tev, i, res);
                    if (result < 0) {
                        result = fn_801B5C94(tev, i, res);
                        if (result < 0) {
                            __assert(&lbl_8047DE70, 0x4B5,
                                     lbl_802753B4);
                            return result;
                        }
                    }
                }
            }
            for (; i < 4; i++) {
                if (tev->c_in[i].type == COL_TE_CNST) {
                    result = fn_801B5C94(tev, i, res);
                    if (result < 0) {
                        __assert(&lbl_8047DE70, 0x4BE, lbl_802753B4);
                        return result;
                    }
                }
            }
        }
    }

    if (tev->a_ref > 0) {
        if (tev->kasel != 0xFF) {
            for (i = 0; i < 4; i++) {
                if (tev->a_in[i].type == COL_TE_CNST) {
                    result = AssignAlphaRegInline(tev, i, res);
                    if (result < 0) {
                        __assert(&lbl_8047DE70, 0x4CE, lbl_802753B4);
                        return result;
                    }
                }
            }
        } else if (IsThroughAlphaInline(tev) &&
                   tev->a_in[3].type == COL_TE_CNST) {
            result = AssignAlphaRegInline(tev, 3, res);
            if (result < 0) {
                result = fn_801B5850(tev, 3, res);
                if (result < 0) {
                    __assert(&lbl_8047DE70, 0x4DA, lbl_802753B4);
                    return result;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (tev->a_in[i].type == COL_TE_CNST) {
                    result = fn_801B5850(tev, i, res);
                    if (result < 0) {
                        result = AssignAlphaRegInline(tev, i, res);
                        if (result < 0) {
                            __assert(&lbl_8047DE70, 0x4E7,
                                     lbl_802753B4);
                            return result;
                        }
                    }
                }
            }
            for (; i < 4; i++) {
                if (tev->a_in[i].type == COL_TE_CNST) {
                    result = AssignAlphaRegInline(tev, i, res);
                    if (result < 0) {
                        __assert(&lbl_8047DE70, 0x4F0, lbl_802753B4);
                        return result;
                    }
                }
            }
        }
    }
    return 0;
}

/*
 * HSD_MaterialUnsetTEV - 0x801B5850 | Size: 0x1B0
 * Clean up TEV state after material rendering.
 * Resets TEV stages, disables texture coordinate generation,
 * and frees temporary resources.
 */
s32 fn_801B5850(ColTExpNode* tev, s32 idx, ColTExpRes* res)
{
}

/*
 * HSD_TextureBindForPass - 0x801B5A00 | Size: 0x294
 * Bind textures for a material rendering pass.
 * Loads texture images to GX, configures texture objects,
 * and sets up the texture-to-TEV stage mapping.
 */
s32 fn_801B5A00(ColTExpNode* tev, s32 idx, ColTExpRes* res)
{
    ColTECnst* cnst = (ColTECnst*) tev->c_in[idx].exp;
    s32 i;

    if (cnst->reg != 0xFF) {
        if (cnst->reg >= 4) {
            return -1;
        }
        if (cnst->comp == 6) {
            tev->kcsel = lbl_8036D288[cnst->reg][cnst->idx];
        } else {
            tev->kcsel = lbl_8036D2C8[cnst->reg];
        }
        tev->c_in[idx].type = 6;
        tev->c_in[idx].arg = 14;
        return 0;
    }

    if (cnst->comp == 6) {
        for (i = 1; i < 4; i++) {
            if (res->reg[i].alpha == 0) {
                res->reg[i].alpha = 1;
                cnst->reg = i;
                cnst->idx = 3;
                tev->kcsel = lbl_8036D288[cnst->reg][cnst->idx];
                tev->c_in[idx].type = 6;
                tev->c_in[idx].arg = 14;
                return 0;
            }
        }
        for (i = 0; i < 4; i++) {
            if (res->reg[i].color < 3) {
                cnst->reg = i;
                cnst->idx = res->reg[i].color++;
                tev->kcsel = lbl_8036D288[cnst->reg][cnst->idx];
                tev->c_in[idx].type = 6;
                tev->c_in[idx].arg = 14;
                return 0;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (res->reg[i].color == 0) {
                res->reg[i].color = 3;
                cnst->reg = i;
                cnst->idx = 0;
                tev->kcsel = lbl_8036D2C8[cnst->reg];
                tev->c_in[idx].type = 6;
                tev->c_in[idx].arg = 14;
                return 0;
            }
        }
    }
    return -1;
}

/*
 * HSD_TexCoordMatrixSetup - 0x801B5C94 | Size: 0x1AC
 * Set up texture coordinate transformation matrices.
 * Computes and loads the texture matrix for each active TObj.
 */
s32 fn_801B5C94(ColTExpNode* tev, s32 idx, ColTExpRes* res)
{
    ColTECnst* cnst = (ColTECnst*) tev->c_in[idx].exp;
    s32 i;

    if (cnst->reg != 0xFF) {
        if (cnst->reg < 4) {
            return -1;
        }
        tev->c_in[idx].type = 5;
        if (cnst->comp == 6) {
            tev->c_in[idx].arg = lbl_8036D2C8[cnst->reg - 4];
        } else {
            tev->c_in[idx].arg = lbl_8036D278[cnst->reg - 4];
        }
        return 0;
    }

    if (cnst->comp == 6) {
        for (i = 4; i < 8; i++) {
            if (res->reg[i].alpha == 0) {
                res->reg[i].alpha = 1;
                cnst->reg = i;
                cnst->idx = 3;
                tev->c_in[idx].type = 5;
                tev->c_in[idx].arg = lbl_8036D2C8[i - 4];
                return 0;
            }
        }
    } else {
        for (i = 4; i < 8; i++) {
            if (res->reg[i].color == 0) {
                res->reg[i].color = 3;
                cnst->reg = i;
                cnst->idx = 0;
                tev->c_in[idx].type = 5;
                tev->c_in[idx].arg = lbl_8036D278[i - 4];
                return 0;
            }
        }
    }
    return -1;
}

/*
 * HSD_TextureLODSetup - 0x801B5E40 | Size: 0xC8
 * Configure texture LOD (level of detail) and filter settings.
 */
#pragma optimization_level 1
void fn_801B5E40(u8* exp, u32 value, s32 index) {
    s32 type;

    if (exp == NULL) {
        __assert(&lbl_8047DE70, 0x366, &lbl_8047DE90);
    }
    if (exp != NULL) {
        goto nonnull;
    }
    type = 0;
    goto type_done;
nonnull:
    if ((u32)exp + 0x10000 == 0xFFFF) {
        type = 2;
        goto type_done;
    }
    if ((u32)exp - 0xFFFF0000u == 0xFFFE) {
        type = 3;
        goto type_done;
    }
    type = *(s32*)exp;
type_done:
    if (type != 1) {
        __assert(&lbl_8047DE70, 0x367, lbl_802753DC);
    }
    *(u32*)(exp + 0x74) = value;
    if (index == 0xFF) {
        *(u8*)(exp + 0x78) = 0xFF;
    } else {
        *(u8*)(exp + 0x78) = index;
    }
}
#pragma optimization_level 4

/* HSD_TExpAlphaIn */
void fn_801B5F08(ColTExpNode* texp, u32 sel_a, ColTExpNode* exp_a,
                 u32 sel_b, ColTExpNode* exp_b, u32 sel_c,
                 ColTExpNode* exp_c, u32 sel_d, ColTExpNode* exp_d) {
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x356, &lbl_8047DE90);
    }
    if (ColTExpGetType(texp) != COL_TE_TEV) {
        __assert(&lbl_8047DE70, 0x357, lbl_802753DC);
    }
}

/*
 * HSD_FullTextureSetup - 0x801B600C | Size: 0x4E0
 * Complete texture setup pipeline. Initializes GXTexObj,
 * configures wrap mode, filter settings, and loads the texture.
 * This is a large function because it handles all texture formats,
 * mipmap chains, and special texture types (TLUT, CI).
 */
void fn_801B600C(HSD_TObj* tobj, u32 map_id) {
    if (tobj == NULL) {
        return;
    }

    /* 1. Initialize texture object from image descriptor */
    if (tobj->imagedesc != NULL) {
        /* GXInitTexObj */
    }

    /* 2. Configure wrap mode */
    /* GXInitTexObjWrapMode */

    /* 3. Configure filter */
    /* GXInitTexObjFilterMode */

    /* 4. Configure LOD */
    if (tobj->lod != NULL) {
        /* GXInitTexObjLOD */
    }

    /* 5. Load TLUT if CI format */
    if (tobj->tlut != NULL) {
        /* GXLoadTlut */
    }

    /* 6. Load texture to GX */
    /* GXLoadTexObj */
}

/* HSD_TExpColorIn */
void fn_801B64EC(ColTExpNode* texp, u32 sel_a, ColTExpNode* exp_a,
                 u32 sel_b, ColTExpNode* exp_b, u32 sel_c,
                 ColTExpNode* exp_c, u32 sel_d, ColTExpNode* exp_d) {
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x2D4, &lbl_8047DE90);
    }
    if (ColTExpGetType(texp) != COL_TE_TEV) {
        __assert(&lbl_8047DE70, 0x2D5, lbl_802753DC);
    }
}

/*
 * HSD_TexCoordGenSetup - 0x801B65F0 | Size: 0x6E8
 * Set up texture coordinate generation for all active textures.
 * This is a very large function because it handles all texcoord
 * generation sources: UV, reflection, highlight, shadow, toon,
 * and gradation mapping.
 */
void fn_801B65F0(ColTExpNode* tev, u32 sel, ColTExpNode* exp, s32 idx)
{
    ColTEArg prev = tev->c_in[idx];
    u8 ksel = 0xFF;

    tev->c_in[idx].type = ColTExpGetType(exp);
    tev->c_in[idx].sel = sel;
    tev->c_in[idx].exp = exp;
    tev->c_in[idx].arg = 0xFF;

    switch (sel) {
    case 7:
        tev->c_in[idx].arg = 15;
        tev->c_in[idx].type = COL_TE_ZERO;
        tev->c_in[idx].exp = NULL;
        break;
    case 8:
        tev->c_in[idx].arg = 14;
        tev->c_in[idx].type = 5;
        tev->c_in[idx].exp = NULL;
        break;
    case 12:
        tev->c_in[idx].arg = 13;
        tev->c_in[idx].type = 5;
        tev->c_in[idx].exp = NULL;
        break;
    case 9:
        ksel = 7;
        goto konst;
    case 10:
        ksel = 6;
        goto konst;
    case 11:
        ksel = 5;
        goto konst;
    case 13:
        ksel = 3;
        goto konst;
    case 14:
        ksel = 2;
        goto konst;
    case 15:
        ksel = 1;
    konst:
        tev->c_in[idx].arg = 14;
        if (tev->kcsel == 0xFF) {
            tev->kcsel = ksel;
        } else if (tev->kcsel == ksel) {
            HSD_Panic(&lbl_8047DE70, 0x218, lbl_802753A8 + 0x114);
        }
        tev->c_in[idx].type = 6;
        break;
    default:
        switch (tev->c_in[idx].type) {
        case COL_TE_ZERO:
            tev->c_in[idx].sel = 7;
            tev->c_in[idx].arg = 15;
            break;
        case COL_TE_TEV: {
            u8 input_sel;
            if (!(sel == 1 || sel == 5)) {
                __assert(&lbl_8047DE70, 0x228, &lbl_8047DEA0);
            }
            if (!(idx == 3 || sel != 1 || exp->c_clamp)) {
                __assert(&lbl_8047DE70, 0x22A, &lbl_8047DEA0);
            }
            if (!(idx == 3 || sel != 5 || exp->a_clamp)) {
                __assert(&lbl_8047DE70, 0x22B, &lbl_8047DEA0);
            }
            input_sel = tev->c_in[idx].sel;
            if (ColTExpGetType(tev->c_in[idx].exp) == COL_TE_TEV) {
                if (input_sel == 1) {
                    tev->c_in[idx].exp->c_ref += 1;
                } else {
                    tev->c_in[idx].exp->a_ref += 1;
                }
            } else if (ColTExpGetType(tev->c_in[idx].exp) == COL_TE_CNST) {
                ((ColTECnst*) tev->c_in[idx].exp)->ref += 1;
            }
            break;
        }
        case COL_TE_CNST: {
            u8 input_sel;
            tev->c_in[idx].sel = ((ColTECnst*) exp)->comp;
            input_sel = tev->c_in[idx].sel;
            if (ColTExpGetType(tev->c_in[idx].exp) == COL_TE_TEV) {
                if (input_sel == 1) {
                    tev->c_in[idx].exp->c_ref += 1;
                } else {
                    tev->c_in[idx].exp->a_ref += 1;
                }
            } else if (ColTExpGetType(tev->c_in[idx].exp) == COL_TE_CNST) {
                ((ColTECnst*) tev->c_in[idx].exp)->ref += 1;
            }
            break;
        }
        case COL_TE_TEX:
            switch (sel) {
            case 1:
                tev->c_in[idx].arg = 8;
                ksel = 0;
                break;
            case 2:
                tev->c_in[idx].arg = 8;
                ksel = 1;
                break;
            case 3:
                tev->c_in[idx].arg = 8;
                ksel = 2;
                break;
            case 4:
                tev->c_in[idx].arg = 8;
                ksel = 3;
                break;
            case 5:
                tev->c_in[idx].arg = 9;
                break;
            default:
                __assert(&lbl_8047DE70, 0x24E, &lbl_8047DEA0);
                break;
            }
            if (tev->tex_swap == 0xFF) {
                tev->tex_swap = ksel;
            } else if (!(ksel == 0xFF || tev->tex_swap == ksel)) {
                __assert(&lbl_8047DE70, 0x253, &lbl_8047DEA0);
            }
            break;
        case COL_TE_RAS:
            switch (sel) {
            case 1:
                tev->c_in[idx].arg = 10;
                ksel = 0;
                break;
            case 2:
                tev->c_in[idx].arg = 10;
                ksel = 1;
                break;
            case 3:
                tev->c_in[idx].arg = 10;
                ksel = 2;
                break;
            case 4:
                tev->c_in[idx].arg = 10;
                ksel = 3;
                break;
            case 5:
                tev->c_in[idx].arg = 11;
                break;
            default:
                __assert(&lbl_8047DE70, 0x27E, &lbl_8047DEA0);
                break;
            }
            if (tev->ras_swap == 0xFF) {
                tev->ras_swap = ksel;
            } else if (!(ksel == 0xFF || tev->ras_swap == ksel)) {
                __assert(&lbl_8047DE70, 0x283, &lbl_8047DEA0);
            }
            break;
        default:
            __assert(&lbl_8047DE70, 0x295, &lbl_8047DEA0);
            break;
        }
        break;
    }
    fn_801B750C(prev.exp, prev.sel);
}

/* ========================================================================= */
/*  TObj rendering helpers                                                   */
/* ========================================================================= */

/* HSD_TExpAlphaOp */
void fn_801B6CD8(ColTExpNode* texp, u32 op, u32 bias, u32 scale, u8 clamp) {
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x1D3, &lbl_8047DE90);
    }
    if (ColTExpGetType(texp) != COL_TE_TEV) {
        __assert(&lbl_8047DE70, 0x1D4, lbl_802753DC);
    }

    texp->a_op = op;
    texp->a_clamp = clamp != 0;
    if (op == 0 || op == 1) {
        texp->a_bias = bias;
        texp->a_scale = scale;
    } else {
        texp->a_bias = 0;
        texp->a_scale = 0;
    }
}

/* Set the four direct color-input selectors on a TEV expression. */
void fn_801B6DC0(ColTExpNode* texp, u32 input_a, u32 input_b,
                 u32 input_c, u32 input_d) {
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x1BB, &lbl_8047DE90);
    }
    if (ColTExpGetType(texp) != COL_TE_TEV) {
        __assert(&lbl_8047DE70, 0x1BC, lbl_802753DC);
    }
    texp->input_index[0] = input_a;
    texp->input_index[1] = input_b;
    texp->input_index[2] = input_c;
    texp->input_index[3] = input_d;
}

/* HSD_TExpColorOp */
void fn_801B6E74(ColTExpNode* texp, u32 op, u32 bias, u32 scale, u8 clamp) {
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x1A3, &lbl_8047DE90);
    }
    if (ColTExpGetType(texp) != COL_TE_TEV) {
        __assert(&lbl_8047DE70, 0x1A4, lbl_802753DC);
    }

    texp->c_op = op;
    texp->c_clamp = clamp != 0;
    if (op == 0 || op == 1) {
        texp->c_bias = bias;
        texp->c_scale = scale;
    } else {
        texp->c_bias = 0;
        texp->c_scale = 0;
    }
}

ColTExpNode* HSD_TExpCnst(void* value, s32 component, s32 type,
                          ColTExpNode** list) {
    ColTECnst* current;
    ColTECnst* constant;

    if (list == NULL) {
        __assert(&lbl_8047DE70, 0x173, lbl_802753A8);
    }

    for (current = (ColTECnst*)*list; current != NULL;
         current = (ColTECnst*)current->next) {
        if (current->type == COL_TE_CNST && current->val == value &&
            current->comp == component) {
            if (current->ctype != type) {
                __assert(&lbl_8047DE70, 0x17A, lbl_802755D0);
            }
            return (ColTExpNode*)current;
        }
    }

    if (component == COL_TE_0) {
        return NULL;
    }

    constant = fn_80193B10(0x14);
    if (constant == NULL) {
        __assert(&lbl_8047DE70, 0x47, &lbl_8047DE90);
    }
    constant->type = COL_TE_CNST;
    constant->next = *list;
    *list = (ColTExpNode*)constant;
    constant->ref = 0;
    constant->val = value;
    constant->comp = component;
    constant->ctype = type;
    constant->reg = 0xFF;
    constant->idx = 0xFF;
    constant->range = 0;
    return (ColTExpNode*)constant;
}

/* HSD_TExpTev */
ColTExpNode* fn_801B707C(ColTExpNode** list) {
    ColTExpNode* texp;
    s32 i;

    if (list == NULL) {
        __assert(&lbl_8047DE70, 0x127, lbl_802753A8);
    }

    texp = fn_80193B10(0x7C);
    if (texp == NULL) {
        __assert(&lbl_8047DE70, 0x3F, &lbl_8047DE90);
    }
    memset(texp, 0xFF, 0x7C);
    texp->type = COL_TE_TEV;
    texp->next = *list;
    *list = texp;
    texp->c_ref = 0;
    texp->a_ref = 0;
    texp->input_index[0] = 0;
    texp->input_index[1] = 1;
    texp->input_index[2] = 2;
    texp->input_index[3] = 3;
    texp->tex = NULL;
    for (i = 0; i < 4; i++) {
        texp->c_in[i].exp = NULL;
        texp->a_in[i].exp = NULL;
    }
    return texp;
}

extern char lbl_8047DEA0;
void fn_801B750C(ColTExpNode* texp, u8 sel);

static inline void ColTExpFree(ColTExpNode* texp)
{
    switch (ColTExpGetType(texp)) {
    case COL_TE_TEV:
        fn_80193AF0(texp, 0x7C);
        break;
    case COL_TE_CNST:
        fn_80193AF0(texp, 0x14);
        break;
    }
}

/* HSD_TExpFreeList */
ColTExpNode* fn_801B7178(ColTExpNode* list, s32 type, s32 all) {
    ColTExpNode** handle;
    ColTExpNode* current;
    ColTExpNode* next;

    handle = &list;
    if (all != 0) {
        while (*handle != NULL) {
            if (type == COL_TE_0 || type == (*handle)->type) {
                switch ((*handle)->type) {
                case COL_TE_TEV:
                case COL_TE_CNST:
                    next = (*handle)->next;
                    ColTExpFree(*handle);
                    *handle = next;
                    continue;
                default:
                    __assert(&lbl_8047DE70, 0xDC, &lbl_8047DEA0);
                    break;
                }
            }
            handle = &(*handle)->next;
        }
    } else {
        if (type == COL_TE_0 || type == COL_TE_TEV) {
            for (current = list; current != NULL; current = current->next) {
                if (current->type == COL_TE_TEV &&
                    current->c_ref == 0 && current->c_ref == 0) {
                    fn_801B750C(current, COL_TE_RGB);
                    fn_801B750C(current, COL_TE_A);
                }
            }
        }

        while (*handle != NULL) {
            if (type == COL_TE_0 || type == (*handle)->type) {
                switch ((*handle)->type) {
                case COL_TE_TEV:
                    if ((*handle)->c_ref != 0 || (*handle)->a_ref != 0) {
                        break;
                    }
                    next = (*handle)->next;
                    ColTExpFree(*handle);
                    *handle = next;
                    continue;
                case COL_TE_CNST:
                    if (((ColTECnst*)*handle)->ref != 0) {
                        break;
                    }
                    next = (*handle)->next;
                    ColTExpFree(*handle);
                    *handle = next;
                    continue;
                default:
                    __assert(&lbl_8047DE70, 0x10C, &lbl_8047DEA0);
                    break;
                }
            }
            handle = &(*handle)->next;
        }
    }
    return list;
}

/*
 * HSD_TExpUnref - 0x801B750C | Size: 0x6C8
 * Drop one reference on a TExp node.  Once both the colour and the alpha
 * reference counts have reached zero the node's own inputs are released as
 * well, so a dead sub-graph unwinds in a single sweep.
 */
void fn_801B750C(ColTExpNode* texp, u8 sel) {
    s32 i;

    switch (ColTExpGetType(texp)) {
    case COL_TE_TEV:
        if (sel == COL_TE_RGB) {
            if (texp->c_ref != 0) {
                texp->c_ref -= 1;
            }
        } else {
            if (texp->a_ref != 0) {
                texp->a_ref -= 1;
            }
        }
        if (texp->c_ref == 0 && texp->a_ref == 0) {
            for (i = 0; i < 4; i++) {
                fn_801B750C(texp->c_in[i].exp, texp->c_in[i].sel);
                fn_801B750C(texp->a_in[i].exp, texp->a_in[i].sel);
            }
        }
        break;
    case COL_TE_CNST:
        if (texp->c_scale != 0) {
            texp->c_scale -= 1;
        }
        break;
    }
}

/* ========================================================================= */
/*  TExp graph helpers                                                       */
/* ========================================================================= */

/*
 * HSD_TExpRef - 0x801B7BD4 | Size: 0x8C
 */
#pragma push
#pragma optimization_level 1
void fn_801B7BD4(ColTExpNode* exp, s32 sel) {
    s32 type = ColTExpGetType(exp);

    switch (type) {
    case COL_TE_TEV:
        if ((u8) sel == TRUE) {
            exp->c_ref += 1;
            return;
        } else {
            exp->a_ref += 1;
            return;
        }
        break;
    case COL_TE_CNST:
        exp->c_scale += 1;
        break;
    }
}
#pragma pop

/* ========================================================================= */
/*  GObj render callbacks                                                    */
/* ========================================================================= */

/*
 * HSD_TExpGetType - 0x801B7C60 | Size: 0x40
 * Get the type of a TExp expression.
 * Returns HSD_TE_ZERO for NULL, HSD_TE_TEX for -1, HSD_TE_RAS for -2,
 * otherwise returns the type field from the expression.
 */
#pragma optimization_level 1
s32 HSD_TExpGetType(u8* texp) {
    if (texp == NULL) {
        return 0;
    }
    if ((u32)texp + 0x10000 == 0xFFFF) {
        return 2;
    }
    if ((u32)texp + 0x10000 == 0xFFFE) {
        return 3;
    }
    return *(s32*)texp;
}
#pragma optimization_level 4

/*
 * HSD_GObjRenderSorted - 0x801B7CA0 | Size: 0x384
 * Full scene render with sorting.
 * Walks the render list, sorts by priority, and dispatches render callbacks.
 */
void fn_801B7CA0(u32 pass) {
    u32 i;

    for (i = 0; i < 64; i++) {
        HSD_GObj* gobj = gobj_render_list[i];
        while (gobj != NULL) {
            if (gobj->render_cb != NULL) {
                gobj->render_cb(gobj, pass);
            }
            gobj = gobj->next_gx;
        }
    }
}

/* ========================================================================= */
/*  TExp DAG construction                                                    */
/* ========================================================================= */

typedef struct HSD_TExpDag {
    void* tev;
    u8 idx;
    u8 nb_dep;
    u8 nb_ref;
    u8 dist;
    struct HSD_TExpDag* depend[8];
} HSD_TExpDag;

extern s32 HSD_TExpGetType(u8* texp);
extern void CalcDistance(u8** nodes, s32* dist, u8* root, s32 num, s32 val);

/*
 * HSD_TExpMakeDag - 0x801B8024 | Size: 0x480
 * Build a DAG from a texture expression tree.
 * Returns the number of nodes in the DAG.
 */
s32 HSD_TExpMakeDag(u8* root, HSD_TExpDag* list) {
    u8* nodes[32];
    s32 dist[32];
    s32 num, saved_num, i, j, k, l, m, last;
    u8* cur;
    u8* exp;
    u8 type;
    HSD_TExpDag* dag;

    HSD_ASSERT(0xEE, HSD_TExpGetType(root) == 1);

    num = 0;
    nodes[num] = root;
    num++;
    j = 0;

    while (j < num) {
        cur = nodes[j];

        for (i = 0; i < 4; i++) {
            type = *(u8*)(cur + 0x34 + i * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x38 + i * 8);
                for (k = 0; k < num; k++) {
                    if (nodes[k] == exp) break;
                }
                if (k >= num) {
                    nodes[num] = exp;
                    num++;
                }
            }
        }

        for (i = 0; i < 4; i++) {
            type = *(u8*)(cur + 0x54 + i * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x58 + i * 8);
                for (k = 0; k < num; k++) {
                    if (nodes[k] == exp) break;
                }
                if (k >= num) {
                    nodes[num] = exp;
                    num++;
                }
            }
        }

        j++;
    }

    saved_num = num;

    for (i = 0; i < saved_num; i++) {
        dist[i] = -1;
    }

    CalcDistance(nodes, dist, nodes[0], saved_num, 0);

    for (i = 0; i < saved_num; i++) {
        for (j = i + 1; j < saved_num; j++) {
            if (dist[j - 1] > dist[j]) {
                u8* tmp_node;
                s32 tmp_dist;

                tmp_node = nodes[j - 1];
                nodes[j - 1] = nodes[j];
                nodes[j] = tmp_node;

                tmp_dist = dist[j - 1];
                dist[j - 1] = dist[j];
                dist[j] = tmp_dist;
            }
        }
    }

    last = saved_num - 1;
    for (i = last; i >= 0; i--) {
        dag = &list[i];
        cur = nodes[i];

        dag->tev = cur;
        dag->idx = (u8)i;
        dag->nb_dep = 0;
        dag->nb_ref = 0;

        for (j = 0; j < 4; j++) {
            type = *(u8*)(cur + 0x34 + j * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x38 + j * 8);
                for (l = i; l < saved_num; l++) {
                    if (exp == nodes[l]) {
                        for (m = 0; m < dag->nb_dep; m++) {
                            if (dag->depend[m] == &list[l]) break;
                        }
                        if (m >= dag->nb_dep) {
                            dag->depend[dag->nb_dep] = &list[l];
                            dag->nb_dep++;
                            list[l].nb_ref++;
                        }
                        break;
                    }
                }
            }
        }

        for (j = 0; j < 4; j++) {
            type = *(u8*)(cur + 0x54 + j * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x58 + j * 8);
                for (l = i; l < saved_num; l++) {
                    if (exp == nodes[l]) {
                        for (m = 0; m < dag->nb_dep; m++) {
                            if (dag->depend[m] == &list[l]) break;
                        }
                        if (m >= dag->nb_dep) {
                            dag->depend[dag->nb_dep] = &list[l];
                            dag->nb_dep++;
                            list[l].nb_ref++;
                        }
                        break;
                    }
                }
            }
        }
    }

    return saved_num;
}

/*
 * CalcDistance - 0x801B84A4 | Size: 0x518
 * TExp DAG distance computation helper.
 * 3-level unrolled recursive depth assignment.
 */
void CalcDistance(u8** nodes, s32* dist, u8* root, s32 num, s32 val) {
    s32 i, j, k;
    u8* exp1;
    u8* exp2;
    s32 val1;
    s32 val2;

    for (i = 0; i < num; i++) {
        if (nodes[i] == root) {
            if (dist[i] >= val) return;
            dist[i] = val;
            val++;
            i = 0;
            goto level1_check;
        }
    }
    return;

level1_check:
    if (i >= 4) return;

    if (*(u8*)(root + 0x34 + i * 8) == 1) {
        exp1 = *(u8**)(root + 0x38 + i * 8);
        val1 = val;

        for (j = 0; j < num; j++) {
            if (nodes[j] == exp1) {
                if (dist[j] >= val1) goto level1_a_in;
                dist[j] = val1;
                val1++;
                j = 0;
                goto level2_c_in_check;
            }
        }
        goto level1_a_in;

level2_c_in_check:
        if (j >= 4) goto level1_a_in;

        if (*(u8*)(exp1 + 0x34 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x38 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2_c_in_a_in;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3_c_in_check;
                }
            }
            goto level2_c_in_a_in;

level3_c_in_check:
            if (k >= 4) goto level2_c_in_a_in;
            if (*(u8*)(exp2 + 0x34 + k * 8) == 1) {
                CalcDistance(nodes, dist, *(u8**)(exp2 + 0x38 + k * 8), num, val2);
            }
            k++;
            goto level3_c_in_check;
        }

level2_c_in_a_in:
        if (*(u8*)(exp1 + 0x54 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x58 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2_a_in_next;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3_a_in_check;
                }
            }
            goto level2_a_in_next;

level3_a_in_check:
            if (k >= 4) goto level2_a_in_next;
            if (*(u8*)(exp2 + 0x54 + k * 8) == 1) {
                CalcDistance(nodes, dist, *(u8**)(exp2 + 0x58 + k * 8), num, val2);
            }
            k++;
            goto level3_a_in_check;
        }

level2_a_in_next:
        j++;
        goto level2_c_in_check;
    }

level1_a_in:
    if (*(u8*)(root + 0x54 + i * 8) == 1) {
        exp1 = *(u8**)(root + 0x58 + i * 8);
        val1 = val;

        for (j = 0; j < num; j++) {
            if (nodes[j] == exp1) {
                if (dist[j] >= val1) goto level1_next;
                dist[j] = val1;
                val1++;
                j = 0;
                goto level2a_c_in_check;
            }
        }
        goto level1_next;

level2a_c_in_check:
        if (j >= 4) goto level1_next;

        if (*(u8*)(exp1 + 0x34 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x38 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2a_c_in_a_in;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3a_c_in_check;
                }
            }
            goto level2a_c_in_a_in;

level3a_c_in_check:
            if (k >= 4) goto level2a_c_in_a_in;
            if (*(u8*)(exp2 + 0x34 + k * 8) == 1) {
                CalcDistance(nodes, dist, *(u8**)(exp2 + 0x38 + k * 8), num, val2);
            }
            k++;
            goto level3a_c_in_check;
        }

level2a_c_in_a_in:
        if (*(u8*)(exp1 + 0x54 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x58 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2a_a_in_next;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3a_a_in_check;
                }
            }
            goto level2a_a_in_next;

level3a_a_in_check:
            if (k >= 4) goto level2a_a_in_next;
            if (*(u8*)(exp2 + 0x54 + k * 8) == 1) {
                CalcDistance(nodes, dist, *(u8**)(exp2 + 0x58 + k * 8), num, val2);
            }
            k++;
            goto level3a_a_in_check;
        }

level2a_a_in_next:
        j++;
        goto level2a_c_in_check;
    }

level1_next:
    i++;
    goto level1_check;
}

/*
 * GObj_RenderLinkManagement - 0x801B89BC | Size: 0x1C8
 * Manage render links for game objects.
 */
void fn_801B89BC(HSD_GObj* gobj, u8 gx_link) {
    if (gobj == NULL) {
        return;
    }

    /* Unlink from current render list */
    if (gobj->prev_gx != NULL) {
        gobj->prev_gx->next_gx = gobj->next_gx;
    } else if (gobj->gx_link < 64) {
        gobj_render_list[gobj->gx_link] = gobj->next_gx;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj->prev_gx;
    }

    /* Link into new render list */
    gobj->gx_link = gx_link;
    if (gx_link < 64) {
        gobj->next_gx = gobj_render_list[gx_link];
        gobj->prev_gx = NULL;
        if (gobj_render_list[gx_link] != NULL) {
            gobj_render_list[gx_link]->prev_gx = gobj;
        }
        gobj_render_list[gx_link] = gobj;
    }
}

/*
 * GObj_ProcessLinkManagement - 0x801B8B84 | Size: 0x1D8
 * Manage process links for game objects.
 */
void fn_801B8B84(HSD_GObj* gobj, u8 p_link, u8 priority) {
    if (gobj == NULL) {
        return;
    }

    /* Unlink from current process list */
    if (gobj->prev != NULL) {
        gobj->prev->next = gobj->next;
    } else if (gobj->p_link < 64) {
        gobj_list[gobj->p_link] = gobj->next;
    }
    if (gobj->next != NULL) {
        gobj->next->prev = gobj->prev;
    }

    /* Update link info */
    gobj->p_link = p_link;
    gobj->p_priority = priority;

    /* Link into new process list */
    if (p_link < 64) {
        gobj->next = gobj_list[p_link];
        gobj->prev = NULL;
        if (gobj_list[p_link] != NULL) {
            gobj_list[p_link]->prev = gobj;
        }
        gobj_list[p_link] = gobj;
    }
}

/*
 * GObj_Destroy - 0x801B8D5C | Size: 0x25C
 */
void fn_801B8D5C(HSD_GObj* gobj) {
    HSD_GObjProc* proc;

    if (gobj == NULL) {
        return;
    }

    if (gobj->user_data != NULL && gobj->user_data_remove_func != NULL) {
        gobj->user_data_remove_func(gobj->user_data);
    }
    proc = gobj->proc;
    while (proc != NULL) {
        HSD_GObjProc* next = proc->next;
        hsdFreeMemPiece(proc, sizeof(HSD_GObjProc));
        proc = next;
    }
    if (gobj->prev != NULL) {
        gobj->prev->next = gobj->next;
    } else if (gobj->p_link < 64) {
        gobj_list[gobj->p_link] = gobj->next;
    }
    if (gobj->next != NULL) {
        gobj->next->prev = gobj->prev;
    }
    if (gobj->prev_gx != NULL) {
        gobj->prev_gx->next_gx = gobj->next_gx;
    } else if (gobj->gx_link < 64) {
        gobj_render_list[gobj->gx_link] = gobj->next_gx;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj->prev_gx;
    }
    gobj_num_active--;
    hsdFreeMemPiece(gobj, sizeof(HSD_GObj));
}

/*
 * The neutral TEV argument used whenever an input slot is cleared:
 * { type = HSD_TE_ZERO, sel = HSD_TE_0, arg = 0xFF, exp = NULL }.
 */
extern ColTEArg lbl_80478CA0;

s32 fn_801B9320(ColTExpNode* tev);
void fn_801B9048(u32 pass);
s32 fn_801BAC8C(ColTExpNode* tev);
s32 fn_801BB4C4(ColTExpNode* tev);

/*
 * HSD_TExpSimplify - 0x801B8FB8 | Size: 0x90
 * Run the three peephole passes over one TEV node until they report no
 * further change, then fold the clamp/range flags.
 */
s32 HSD_TExpSimplify(ColTExpNode* texp) {
    s32 res = 0;

    if (((s32 (*)(ColTExpNode*)) HSD_TExpGetType)(texp) != COL_TE_TEV) {
        return 0;
    }
    if (fn_801BB4C4(texp) != 0) {
        res = 1;
    }
    if (fn_801BAC8C(texp) != 0) {
        res = 1;
    }
    if (fn_801B9320(texp) != 0) {
        res = 1;
    }
    return res;
}

/*
 * 0x801B9048 | Size: 0x2D8
 * Propagate the clamp/range flags of the source expressions onto this node.
 */
void fn_801B9048(u32 pass) {
    u32 i;

    for (i = 0; i < 64; i++) {
        HSD_GObj* gobj = gobj_render_list[i];
        while (gobj != NULL) {
            if (gobj->render_cb != NULL) {
                gobj->render_cb(gobj, pass);
            }
            gobj = gobj->next_gx;
        }
    }
}

/* Keep this as a real call: exposing the same-TU body perturbs the retail
 * register allocation in the large simplifier below. */
#define TEXP_REF(exp, sel)                                                   \
    ((void (*)(ColTExpNode*, s32)) fn_801B7BD4)((exp), (sel))
#define TEXP_UNREF(exp, sel)                                                 \
    ((void (*)(ColTExpNode*, u8)) fn_801B750C)((exp), (sel))

/*
 * SimplifyByMerge - 0x801B9320 | Size: 0x196C
 * Fold a compatible child TEV stage into its parent.  Color and alpha are
 * handled independently, and the pass repeats until no further fold occurs.
 */
#pragma push
#pragma use_lmw_stmw on
s32 fn_801B9320(ColTExpNode* tev)
{
    ColTExpNode* child;
    s32 bias;
    s32 result;
    s32 merged;
    s32 i;
    s32 conflict;
    u8 type;
    u8 child_sel;
    u8 new_op;
    ColTEArg tmp_arg;

    result = 0;
    do {
        merged = 0;

        if (tev->a_op == 0xFF || tev->a_op == 0xE || tev->a_op == 0xF ||
            tev->a_op <= 1)
        {
            if ((tev->c_op == 0 || tev->c_op == 1) &&
                tev->c_in[1].sel == COL_TE_0 &&
                tev->c_in[2].sel == COL_TE_0 &&
                HSD_TExpGetType((u8*) tev->c_in[0].exp) != COL_TE_CNST &&
                HSD_TExpGetType((u8*) tev->c_in[3].exp) != COL_TE_CNST)
            {
                if (tev->c_op == 0 && tev->c_in[3].type == COL_TE_TEV &&
                    ((tev->c_in[3].sel == COL_TE_RGB &&
                      tev->c_in[3].exp->c_clamp != 0) ||
                     (tev->c_in[3].sel == COL_TE_A &&
                      tev->c_in[3].exp->a_clamp != 0)))
                {
                    type = tev->c_in[0].type;
                    switch (type) {
                    case COL_TE_TEX:
                    case COL_TE_RAS:
                        tmp_arg = tev->c_in[0];
                        tev->c_in[0] = tev->c_in[3];
                        tev->c_in[3] = tmp_arg;
                        break;
                    }
                }

                switch (tev->c_in[0].type) {
                case COL_TE_TEV:
                    if (tev->c_in[0].sel == COL_TE_RGB) {
                        child = tev->c_in[0].exp;
                        child_sel = tev->c_in[0].sel;
                        if ((child->c_op == 0 || child->c_op == 1) &&
                            child->c_in[3].sel == COL_TE_0 &&
                            child->c_scale == 0)
                        {
                            if (tev->tex != NULL && child->tex != NULL &&
                                tev->tex != child->tex)
                            {
                                conflict = 1;
                            } else if (tev->chan != 0xFF &&
                                       child->chan != 0xFF &&
                                       tev->chan != child->chan)
                            {
                                conflict = 1;
                            } else {
                                conflict = 0;
                            }
                            if (conflict == 0) {
                                switch ((s32) child->c_bias) {
                                case 1:
                                    bias = 1;
                                    break;
                                case 2:
                                    bias = -1;
                                    break;
                                default:
                                    bias = 0;
                                    break;
                                }
                                if (child->c_op == 1) {
                                    bias = -bias;
                                }
                                switch ((s32) tev->c_bias) {
                                case 1:
                                    bias += 1;
                                    break;
                                case 2:
                                    bias -= 1;
                                    break;
                                }
                                switch (bias) {
                                case 0:
                                    tev->c_bias = 0;
                                    merged = 1;
                                    break;
                                case 1:
                                    tev->c_bias = 1;
                                    merged = 1;
                                    break;
                                case -1:
                                    tev->c_bias = 2;
                                    merged = 1;
                                    break;
                                default:
                                    merged = 0;
                                    break;
                                }
                                if (merged != 0) {
                                    if (child->c_op == 1) {
                                        if (tev->c_op == 0) {
                                            new_op = 1;
                                        } else {
                                            new_op = 0;
                                        }
                                        tev->c_op = new_op;
                                    }
                                    for (i = 0; i < 3; i++) {
                                        tev->c_in[i] = child->c_in[i];
                                        TEXP_REF(tev->c_in[i].exp,
                                                 tev->c_in[i].sel);
                                    }
                                    if (tev->tex == NULL) {
                                        tev->tex = child->tex;
                                    }
                                    if (tev->chan == 0xFF) {
                                        tev->chan = child->chan;
                                    }
                                    if (tev->tex_swap == 0xFF) {
                                        tev->tex_swap = child->tex_swap;
                                    }
                                    if (tev->ras_swap == 0xFF) {
                                        tev->ras_swap = child->ras_swap;
                                    }
                                    TEXP_UNREF(child, child_sel);
                                }
                            }
                        }
                    }
                    break;
                case COL_TE_ZERO:
                    if (tev->c_in[3].type == COL_TE_TEV) {
                        child_sel = tev->c_in[3].sel;
                        if (child_sel == COL_TE_RGB) {
                            child = tev->c_in[3].exp;
                            if (child->c_scale == 0 &&
                                (tev->c_bias == 0 ||
                                 tev->c_bias != child->c_bias))
                            {
                                if (tev->tex != NULL && child->tex != NULL &&
                                    tev->tex != child->tex)
                                {
                                    conflict = 1;
                                } else if (tev->chan != 0xFF &&
                                           child->chan != 0xFF &&
                                           tev->chan != child->chan)
                                {
                                    conflict = 1;
                                } else {
                                    conflict = 0;
                                }
                                if (conflict == 0) {
                                    merged = 1;
                                    for (i = 0; i < 4; i++) {
                                        tev->c_in[i] = child->c_in[i];
                                        TEXP_REF(tev->c_in[i].exp,
                                                 tev->c_in[i].sel);
                                    }
                                    tev->c_op = child->c_op;
                                    switch ((s32) child->c_bias) {
                                    case 1:
                                        bias = 1;
                                        break;
                                    case 2:
                                        bias = -1;
                                        break;
                                    default:
                                        bias = 0;
                                        break;
                                    }
                                    if (child->c_op == 1) {
                                        bias = -bias;
                                    }
                                    switch ((s32) tev->c_bias) {
                                    case 1:
                                        bias += 1;
                                        break;
                                    case 2:
                                        bias -= 1;
                                        break;
                                    }
                                    switch (bias) {
                                    case 1:
                                        tev->c_bias = 1;
                                        break;
                                    case -1:
                                        tev->c_bias = 2;
                                        break;
                                    default:
                                    case 0:
                                        tev->c_bias = 0;
                                        break;
                                    }
                                    if (tev->c_clamp == 0xFF ||
                                        tev->c_clamp == 0)
                                    {
                                        tev->c_clamp = child->c_clamp;
                                    }
                                    if (tev->tex == NULL) {
                                        tev->tex = child->tex;
                                    }
                                    if (tev->chan == 0xFF) {
                                        tev->chan = child->chan;
                                    }
                                    if (tev->tex_swap == 0xFF) {
                                        tev->tex_swap = child->tex_swap;
                                    }
                                    if (tev->ras_swap == 0xFF) {
                                        tev->ras_swap = child->ras_swap;
                                    }
                                    TEXP_UNREF(child, child_sel);
                                }
                            }
                        }
                    }
                    break;
                }
            }

            if ((tev->a_op == 0 || tev->a_op == 1) &&
                tev->a_in[1].sel == COL_TE_0 &&
                tev->a_in[2].sel == COL_TE_0 &&
                HSD_TExpGetType((u8*) tev->a_in[0].exp) != COL_TE_CNST &&
                HSD_TExpGetType((u8*) tev->a_in[3].exp) != COL_TE_CNST)
            {
                if (tev->a_op == 0 && tev->a_in[3].type == COL_TE_TEV &&
                    tev->a_in[3].exp->a_clamp != 0)
                {
                    type = tev->a_in[0].type;
                    switch ((s32) type) {
                    case COL_TE_TEX:
                    case COL_TE_RAS:
                        tmp_arg = tev->a_in[0];
                        tev->a_in[0] = tev->a_in[3];
                        tev->a_in[3] = tmp_arg;
                        break;
                    }
                }

                switch (tev->a_in[0].type) {
                case COL_TE_TEV:
                    child = tev->a_in[0].exp;
                    child_sel = tev->a_in[0].sel;
                    if ((child->a_op == 0 || child->a_op == 1) &&
                        child->a_in[3].sel == COL_TE_0 &&
                        child->a_scale == 0)
                    {
                        if (tev->tex != NULL && child->tex != NULL &&
                            tev->tex != child->tex)
                        {
                            conflict = 1;
                        } else if (tev->chan != 0xFF &&
                                   child->chan != 0xFF &&
                                   tev->chan != child->chan)
                        {
                            conflict = 1;
                        } else {
                            conflict = 0;
                        }
                        if (conflict == 0) {
                            switch ((s32) child->a_bias) {
                            case 1:
                                bias = 1;
                                break;
                            case 2:
                                bias = -1;
                                break;
                            default:
                                bias = 0;
                                break;
                            }
                            if (child->a_op == 1) {
                                bias = -bias;
                            }
                            switch ((s32) tev->a_bias) {
                            case 1:
                                bias += 1;
                                break;
                            case 2:
                                bias -= 1;
                                break;
                            }
                            switch (bias) {
                            case 0:
                                tev->a_bias = 0;
                                merged = 1;
                                break;
                            case 1:
                                tev->a_bias = 1;
                                merged = 1;
                                break;
                            case -1:
                                tev->a_bias = 2;
                                merged = 1;
                                break;
                            default:
                                merged = 0;
                                break;
                            }
                            if (merged != 0) {
                                if (child->a_op == 1) {
                                    if (tev->a_op == 0) {
                                        new_op = 1;
                                    } else {
                                        new_op = 0;
                                    }
                                    tev->a_op = new_op;
                                }
                                for (i = 0; i < 3; i++) {
                                    tev->a_in[i] = child->a_in[i];
                                    TEXP_REF(tev->a_in[i].exp,
                                             tev->a_in[i].sel);
                                }
                                if (tev->tex == NULL) {
                                    tev->tex = child->tex;
                                }
                                if (tev->chan == 0xFF) {
                                    tev->chan = child->chan;
                                }
                                TEXP_UNREF(child, child_sel);
                            }
                        }
                    }
                    break;
                case COL_TE_ZERO:
                    if (tev->a_in[3].type == COL_TE_TEV) {
                        child = tev->a_in[3].exp;
                        child_sel = tev->a_in[3].sel;
                        if (child->a_scale == 0 &&
                            (tev->a_bias == 0 ||
                             tev->a_bias != child->a_bias))
                        {
                            if (tev->tex != NULL && child->tex != NULL &&
                                tev->tex != child->tex)
                            {
                                conflict = 1;
                            } else if (tev->chan != 0xFF &&
                                       child->chan != 0xFF &&
                                       tev->chan != child->chan)
                            {
                                conflict = 1;
                            } else {
                                conflict = 0;
                            }
                            if (conflict == 0) {
                                merged = 1;
                                for (i = 0; i < 4; i++) {
                                    tev->a_in[i] = child->a_in[i];
                                    TEXP_REF(tev->a_in[i].exp,
                                             tev->a_in[i].sel);
                                }
                                tev->a_op = child->a_op;
                                switch ((s32) child->a_bias) {
                                case 1:
                                    bias = 1;
                                    break;
                                case 2:
                                    bias = -1;
                                    break;
                                default:
                                    bias = 0;
                                    break;
                                }
                                if (child->a_op == 1) {
                                    bias = -bias;
                                }
                                switch ((s32) tev->a_bias) {
                                case 1:
                                    bias += 1;
                                    break;
                                case 2:
                                    bias -= 1;
                                    break;
                                }
                                switch (bias) {
                                case 1:
                                    tev->a_bias = 1;
                                    break;
                                case -1:
                                    tev->a_bias = 2;
                                    break;
                                default:
                                case 0:
                                    tev->a_bias = 0;
                                    break;
                                }
                                if (tev->a_clamp == 0xFF ||
                                    tev->a_clamp == 0)
                                {
                                    tev->a_clamp = child->a_clamp;
                                }
                                if (tev->tex == NULL) {
                                    tev->tex = child->tex;
                                }
                                if (tev->chan == 0xFF) {
                                    tev->chan = child->chan;
                                }
                                TEXP_UNREF(child, child_sel);
                            }
                        }
                    }
                    break;
                }
            }
        }

        if (merged != 0) {
            result = 1;
        }
    } while (merged != 0);

    return result;
}
#pragma pop

/*
 * SimplifyThis - 0x801BAC8C | Size: 0x838
 * Local peephole over a single TEV stage: drop unreferenced halves, collapse
 * multiply/compare forms whose operands became constant, and re-run until the
 * node stops changing.
 */
s32 fn_801BAC8C(ColTExpNode* tev)
{
    s32 color_tex;
    s32 alpha_tex;
    s32 color_ras;
    s32 alpha_ras;
    s32 result;
    s32 changed;
    s32 i;

    result = 0;
    do {
        color_tex = -1;
        alpha_tex = -1;
        color_ras = -1;
        alpha_ras = -1;

        for (i = 0; i < 4; i++) {
            switch (tev->c_in[i].type) {
            case COL_TE_TEX:
                color_tex = i;
                break;
            case COL_TE_RAS:
                color_ras = i;
                break;
            }
            switch (tev->a_in[i].type) {
            case COL_TE_TEX:
                alpha_tex = i;
                break;
            case COL_TE_RAS:
                alpha_ras = i;
                break;
            }
        }

        if (color_tex == -1 && alpha_tex == -1) {
            tev->tex = NULL;
            tev->tex_swap = 0xFF;
        }
        if (color_ras == -1 && alpha_ras == -1) {
            tev->chan = 0xFF;
            tev->ras_swap = 0xFF;
        }

        changed = 0;
        if (tev->a_op == 0xFF || tev->a_op == 0xE || tev->a_op == 0xF ||
            tev->a_op == 0 || tev->a_op == 1)
        {
            if (tev->c_op != 0xFF && tev->c_ref == 0) {
                tev->c_op = 0xFF;
                for (i = 0; i < 4; i++) {
                    TEXP_UNREF(tev->c_in[i].exp, tev->c_in[i].sel);
                    tev->c_in[i] = lbl_80478CA0;
                }
                changed = 1;
            }

            switch (tev->c_op) {
            case 0:
            case 1:
                if (tev->c_in[2].sel == COL_TE_0) {
                    if (tev->c_in[1].sel != COL_TE_0) {
                        TEXP_UNREF(tev->c_in[1].exp, tev->c_in[1].sel);
                        changed = 1;
                        tev->c_in[1] = lbl_80478CA0;
                    }
                    if (tev->c_op == 0 && tev->c_in[3].sel == COL_TE_0) {
                        changed = 1;
                        tev->c_in[3] = tev->c_in[0];
                        tev->c_in[0] = lbl_80478CA0;
                        tev->c_clamp = 1;
                    }
                }
                if (tev->c_in[2].sel == 8) {
                    if (tev->c_in[0].sel != COL_TE_0) {
                        TEXP_UNREF(tev->c_in[0].exp, tev->c_in[0].sel);
                        changed = 1;
                        tev->c_in[0] = lbl_80478CA0;
                    }
                    if (tev->c_op == 0 && tev->c_in[3].sel == COL_TE_0) {
                        changed = 1;
                        tev->c_in[3] = tev->c_in[1];
                        tev->c_in[1] = lbl_80478CA0;
                        tev->c_in[2] = lbl_80478CA0;
                    }
                }
                if (tev->c_in[0].sel == COL_TE_0 &&
                    tev->c_in[1].sel == COL_TE_0 &&
                    tev->c_in[3].sel == COL_TE_0 && tev->c_bias == 0)
                {
                    tev->c_op = 0xFF;
                    TEXP_UNREF(tev->c_in[2].exp, tev->c_in[2].sel);
                    changed = 1;
                    tev->c_in[2] = lbl_80478CA0;
                }
                break;
            case 8:
            case 10:
            case 12:
            case 14:
                if (tev->c_in[2].sel == COL_TE_0) {
                    tev->c_op = 0;
                    TEXP_UNREF(tev->c_in[0].exp, tev->c_in[0].sel);
                    tev->c_in[0] = lbl_80478CA0;
                    TEXP_UNREF(tev->c_in[1].exp, tev->c_in[1].sel);
                    changed = 1;
                    tev->c_in[1] = lbl_80478CA0;
                } else if (tev->c_in[0].sel == COL_TE_0) {
                    tev->c_op = 0;
                    TEXP_UNREF(tev->c_in[1].exp, tev->c_in[1].sel);
                    tev->c_in[1] = lbl_80478CA0;
                    TEXP_UNREF(tev->c_in[2].exp, tev->c_in[2].sel);
                    changed = 1;
                    tev->c_in[2] = lbl_80478CA0;
                }
                break;
            case 9:
            case 11:
            case 13:
            case 15:
                if (tev->c_in[2].sel == COL_TE_0) {
                    tev->c_op = 0;
                    TEXP_UNREF(tev->c_in[0].exp, tev->c_in[0].sel);
                    tev->c_in[0] = lbl_80478CA0;
                    TEXP_UNREF(tev->c_in[1].exp, tev->c_in[1].sel);
                    changed = 1;
                    tev->c_in[1] = lbl_80478CA0;
                } else if (tev->c_in[0].sel == COL_TE_0 &&
                           tev->c_in[1].sel == COL_TE_0)
                {
                    tev->c_op = 0;
                    changed = 1;
                    tev->c_in[0] = tev->c_in[2];
                    tev->c_in[2] = lbl_80478CA0;
                }
                break;
            }
        }

        if (tev->a_op != 0xFF && tev->a_ref == 0) {
            tev->a_op = 0xFF;
            for (i = 0; i < 4; i++) {
                TEXP_UNREF(tev->a_in[i].exp, tev->a_in[i].sel);
                tev->a_in[i] = lbl_80478CA0;
            }
            changed = 1;
        }

        switch (tev->a_op) {
        case 0:
        case 1:
            if (tev->a_in[2].sel == COL_TE_0) {
                if (tev->a_in[1].sel != COL_TE_0) {
                    TEXP_UNREF(tev->a_in[1].exp, tev->a_in[1].sel);
                    changed = 1;
                    tev->a_in[1] = lbl_80478CA0;
                }
                if (tev->a_op == 0 && tev->a_in[3].sel == COL_TE_0) {
                    changed = 1;
                    tev->a_in[3] = tev->a_in[0];
                    tev->a_in[0] = lbl_80478CA0;
                }
            }
            if (tev->a_in[2].sel == 8) {
                if (tev->a_in[0].sel != COL_TE_0) {
                    TEXP_UNREF(tev->a_in[0].exp, tev->a_in[0].sel);
                    changed = 1;
                    tev->a_in[0] = lbl_80478CA0;
                }
                if (tev->a_op == 0 && tev->a_in[3].sel == COL_TE_0) {
                    changed = 1;
                    tev->a_in[3] = tev->a_in[1];
                    tev->a_in[1] = lbl_80478CA0;
                    tev->a_in[2] = lbl_80478CA0;
                }
            }
            if (tev->a_in[0].sel == COL_TE_0 &&
                tev->a_in[1].sel == COL_TE_0 && tev->a_in[3].sel == COL_TE_0)
            {
                tev->a_op = 0xFF;
                changed = 1;
            }
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            if (tev->a_in[2].sel == COL_TE_0) {
                tev->a_op = 0;
                TEXP_UNREF(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = lbl_80478CA0;
                TEXP_UNREF(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = lbl_80478CA0;
            }
            break;
        case 14:
            if (tev->a_in[2].sel == COL_TE_0) {
                tev->a_op = 0;
                TEXP_UNREF(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = lbl_80478CA0;
                TEXP_UNREF(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = lbl_80478CA0;
            } else if (tev->a_in[0].sel == COL_TE_0) {
                tev->a_op = 0;
                TEXP_UNREF(tev->a_in[1].exp, tev->a_in[1].sel);
                tev->a_in[1] = lbl_80478CA0;
                TEXP_UNREF(tev->a_in[2].exp, tev->a_in[2].sel);
                changed = 1;
                tev->a_in[2] = lbl_80478CA0;
            }
            break;
        case 15:
            if (tev->a_in[2].sel == COL_TE_0) {
                tev->a_op = 0;
                TEXP_UNREF(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = lbl_80478CA0;
                TEXP_UNREF(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = lbl_80478CA0;
            } else if (tev->a_in[0].sel == COL_TE_0 &&
                       tev->a_in[1].sel == COL_TE_0)
            {
                tev->a_op = 0;
                changed = 1;
                tev->a_in[0] = tev->a_in[2];
                tev->a_in[2] = lbl_80478CA0;
            }
            break;
        }

        if (changed != 0) {
            result = 1;
        }
    } while (changed != 0);

    return result;
}

/*
 * SimplifySrc - 0x801BB4C4 | Size: 0x604
 * Simplify every TEV-typed input of this node, then splice a pass-through
 * source directly into this node's input slot.
 */
s32 fn_801BB4C4(ColTExpNode* tev)
{
    ColTExpNode* src;
    u8 sel;
    s32 result;
    s32 i;

    result = 0;

    for (i = 0; i < 4; i++) {
        if (tev->c_in[i].type == COL_TE_TEV) {
            src = tev->c_in[i].exp;
            sel = tev->c_in[i].sel;
            if (HSD_TExpSimplify(src) != 0) {
                result = 1;
            }
            if (sel == COL_TE_RGB) {
                switch (src->c_op) {
                case 0xFF:
                    TEXP_UNREF(src, sel);
                    result = 1;
                    tev->c_in[i] = lbl_80478CA0;
                    break;
                case 0:
                    if (src->c_in[0].sel == COL_TE_0 &&
                        src->c_in[1].sel == COL_TE_0 && src->c_bias == 0 &&
                        src->c_scale == 0)
                    {
                        switch (src->c_in[3].type) {
                        case COL_TE_TEV:
                            if (src->c_in[3].exp->c_clamp != 0 ||
                                src->c_clamp == 0)
                            {
                                tev->c_in[i] = src->c_in[3];
                                TEXP_REF(tev->c_in[i].exp, tev->c_in[i].sel);
                                TEXP_UNREF(src, sel);
                                result = 1;
                            }
                            break;
                        case COL_TE_TEX:
                            if ((tev->tex == NULL || tev->tex == src->tex) &&
                                (tev->tex_swap == 0xFF ||
                                 src->tex_swap == 0xFF ||
                                 tev->tex_swap == src->tex_swap))
                            {
                                tev->c_in[i] = src->c_in[3];
                                tev->tex = src->tex;
                                if (tev->tex_swap == 0xFF) {
                                    tev->tex_swap = src->tex_swap;
                                }
                                TEXP_UNREF(src, sel);
                                result = 1;
                            }
                            break;
                        case COL_TE_RAS:
                            if ((tev->chan == 0xFF ||
                                 tev->chan == src->chan) &&
                                (tev->ras_swap == 0xFF ||
                                 src->ras_swap == 0xFF ||
                                 tev->ras_swap == src->ras_swap))
                            {
                                tev->c_in[i] = src->c_in[3];
                                tev->chan = src->chan;
                                if (tev->ras_swap == 0xFF) {
                                    tev->ras_swap = src->ras_swap;
                                }
                                TEXP_UNREF(src, sel);
                                result = 1;
                            }
                            break;
                        }
                    }
                    break;
                }
            } else {
                switch (src->a_op) {
                case 0xFF:
                    TEXP_UNREF(src, sel);
                    result = 1;
                    tev->c_in[i] = lbl_80478CA0;
                    break;
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        if (tev->a_in[i].type == COL_TE_TEV) {
            src = tev->a_in[i].exp;
            sel = tev->a_in[i].sel;
            if (HSD_TExpSimplify(src) != 0) {
                result = 1;
            }
            switch (src->a_op) {
            case 0xFF:
                TEXP_UNREF(src, sel);
                result = 1;
                tev->a_in[i] = lbl_80478CA0;
                break;
            case 0:
                if (src->a_in[0].sel == COL_TE_0 &&
                    src->a_in[1].sel == COL_TE_0 && src->a_bias == 0 &&
                    src->a_scale == 0)
                {
                    switch (src->a_in[3].type) {
                    case COL_TE_TEV:
                        tev->a_in[i] = src->a_in[3];
                        TEXP_REF(tev->a_in[i].exp, tev->a_in[i].sel);
                        TEXP_UNREF(src, sel);
                        result = 1;
                        break;
                    case COL_TE_TEX:
                        if (tev->tex == NULL || tev->tex == src->tex) {
                            tev->a_in[i] = src->a_in[3];
                            tev->tex = src->tex;
                            TEXP_UNREF(src, sel);
                            result = 1;
                        }
                        break;
                    case COL_TE_RAS:
                        if (tev->chan == 0xFF || tev->chan == src->chan) {
                            tev->a_in[i] = src->a_in[3];
                            tev->chan = src->chan;
                            TEXP_UNREF(src, sel);
                            result = 1;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return result;
}

#undef TEXP_UNREF
#undef TEXP_REF

/* 0x801B1854 | 0x30 */
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);
void fn_801B1854(void) {
    HSD_ObjAllocInit(&lbl_804656E0, 0x28, 4);
}

/* 0x801B1884 | 0xC */
void* HSD_ShadowGetAllocData(void) {
    return &lbl_804656E0;
}

extern f32 fn_801B18D8(HSD_Spline* spline, f32 distance);

void splArcLengthPoint(SplineVec3* p, HSD_Spline* spline, f32 distance) {
    fn_801B2038(p, spline, fn_801B18D8(spline, distance));
}

typedef void (*HSD_TevStateFn)(void);

typedef struct HSD_TevStateCallback {
    s32 mask;
    HSD_TevStateFn callback;
} HSD_TevStateCallback;

extern HSD_TevStateCallback lbl_8036CFA8[];

/* 0x801B25C4 | 0x90 */
#pragma push
#pragma optimization_level 1
void fn_801B25C4(u32 flags) {
    s32 i;

    for (i = 0; lbl_8036CFA8[i].mask != 0; i++) {
        if ((flags & lbl_8036CFA8[i].mask) != 0) {
            lbl_8036CFA8[i].callback();
        }
    }
}
#pragma pop

/* 0x801B2654 | 0xA4 */
#pragma push
#pragma optimization_level 1
#pragma use_lmw_stmw on
void fn_801B2654(void) {
    lbl_8047B348 = -1;
    lbl_8047B344 = -1;
    lbl_8047B340 = -1;
    lbl_8047B33C = -1;
    lbl_8047B338 = 0xFF;
    lbl_8047B334 = -1;
    lbl_8047B330 = 0xFF;
    lbl_8047B32C = -1;
    lbl_8047B328 = 0;
    lbl_8047B324 = -1;
    lbl_8047B320 = -1;
    lbl_8047B31E = 0;
    lbl_8047B31D = 0xFF;
    lbl_8047B31C = 0xFF;
    lbl_8047B31B = 0xFF;
    lbl_8047B31A = 0;
    lbl_8047B319 = 0xFF;
    lbl_8047B318 = 0xFF;
}
#pragma pop

/* 0x801B26F8 | 0x20 */
void fn_801B26F8(void) {
    HSD_ClearVtxDesc();
}

/* 0x801B2718 | 0x24 */
#pragma push
#pragma optimization_level 1
void fn_801B2718(void) {
    lbl_8047B351 = 0;
    lbl_8047B350 = 0;
    lbl_8047B34C = -1;
    lbl_8047B351 = 0xFF;
}
#pragma pop

/* 0x801B273C | 0x50 */
#pragma push
#pragma optimization_level 1
void fn_801B273C(s32 enable) {
    u8 normalized = enable ? 1 : 0;

    if (lbl_8047B31C != normalized) {
        fn_800BCE5C(normalized);
        lbl_8047B31C = normalized;
    }
}
#pragma pop

/* 0x801B278C | 0x50 */
#pragma push
#pragma optimization_level 1
void fn_801B278C(s32 enable) {
    u8 normalized = enable ? 1 : 0;

    if (lbl_8047B31D != normalized) {
        fn_800BCE30(normalized);
        lbl_8047B31D = normalized;
    }
}
#pragma pop

/* 0x801B27DC | 0x9C */
void fn_801B27DC(u32 enable, s32 func, u32 update) {
    u32 enable_state = enable != 0;
    u32 update_state = update != 0;

    if ((u8)lbl_8047B338 != (u8)enable_state ||
        lbl_8047B334 != func ||
        (u8)lbl_8047B330 != (u8)update_state) {
        GXSetZMode(enable_state, func, update_state);
        lbl_8047B338 = enable_state;
        lbl_8047B334 = func;
        lbl_8047B330 = update_state;
    }
}

/* 0x801B2878 | 0x40 */
#pragma push
#pragma optimization_level 1
void fn_801B2878(s32 value) {
    if (lbl_8047B34C != value) {
        fn_800B94F0(value);
        lbl_8047B34C = value;
    }
}
#pragma pop

/* 0x801B28B8 | 0x10 */
void fn_801B28B8(f32 value) {
    lbl_80465710.shininess = value;
}

/* 0x801B3168 | 0xC */
void fn_801B3168(void) {
    lbl_8047B358 = 0;
}

/* 0x801B3174 | 0x30 */
#pragma push
#pragma optimization_level 1
void fn_801B3174(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        lbl_8036CFE8[i].dirty = 0;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
/* 0x801B31A4 | 0x50 */
void fn_801B31A4(void) {
    extern void fn_800BBC34(s32 stage);
    extern void fn_800BBC0C(s32 count);
    s32 i;

    for (i = 0; i < 16; i++) {
        fn_800BBC34(i);
    }
    fn_800BBC0C(0);
    lbl_8047B370 = 0;
}
#pragma pop

/* 0x801B31F4 | 0x64 */
#pragma push
#pragma optimization_level 1
void fn_801B31F4(void)
{
    memcpy(&lbl_8036D018, &lbl_8036D0D8, sizeof(lbl_8036D018));
    lbl_8047B368[0] = 1;
    lbl_8047B368[1] = 1;
    lbl_8047B360[0] = 1;
    lbl_8047B360[1] = 1;
    lbl_8047B35C = -1;
}
#pragma pop

/* 0x801B3770 | 0x30 */
void fn_801B3770(void) {
    fn_800BC8C8((u32)(u8)lbl_8047B370);
    lbl_8047B370 = 0;
}

/* 0x801B37A0 | 0xDC */
#pragma push
#pragma optimization_level 1
s32 HSD_StateAssignTev(void) {
    switch (lbl_8047B370++) {
    case 0:  return 0;
    case 1:  return 1;
    case 2:  return 2;
    case 3:  return 3;
    case 4:  return 4;
    case 5:  return 5;
    case 6:  return 6;
    case 7:  return 7;
    case 8:  return 8;
    case 9:  return 9;
    case 10: return 10;
    case 11: return 11;
    case 12: return 12;
    case 13: return 13;
    case 14: return 14;
    case 15: return 15;
    default:
        __assert((const char*)&lbl_8047DE60, 0x326, (const char*)&lbl_8047DE68);
        return 15;
    }
}
#pragma pop

s32 fn_801B387C(void) {
    return lbl_8047B370;
}

/* 0x801B3884 | 0xC */
void fn_801B3884(void) {
    lbl_8047B370 = 0;
}

/* 0x801B3890 | 0x30 */
void fn_801B3890(void) {
    fn_800B884C((u32)(u8)lbl_8047B358);
    lbl_8047B358 = 0;
}

static inline s32 HSD_TexCoordID2Num(s32 coord)
{
    switch (coord) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    case 6:
        return 7;
    case 7:
        return 8;
    case 0xFF:
        return 0;
    default:
        __assert((const char*)&lbl_8047DE60, 0x453,
                 (const char*)&lbl_8047DE68);
        return 0;
    }
}

void HSD_StateRegisterTexGen(s32 coord)
{
    s32 num = HSD_TexCoordID2Num(coord);

    if (num > lbl_8047B358) {
        lbl_8047B358 = num;
    }
}
