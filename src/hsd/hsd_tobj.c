/**
 * @file hsd_tobj.c
 * @brief HSD TObj - Texture object implementation.
 *
 * Colosseum address range: 0x801BBAC8 - 0x801BF098
 * Adapted from doldecomp/melee src/sysdolphin/baselib/tobj.c
 *
 * Colosseum emits this translation unit in the reverse of Melee's source
 * order (TObjInfoInit first, the animation entry points last), so the
 * functions below are laid out to match the binary, not tobj.c.
 *
 * The class info, the two file statics, the assert strings and the float
 * constants all live in data sections that are not yet decompiled, so they
 * are reached through their `lbl_` symbol names; the `#define`s below give
 * them their sysdolphin names.
 */

#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_tobj.h"

/* ------------------------------------------------------------------ */
/*  Externals that do not have names in the symbol map yet             */
/* ------------------------------------------------------------------ */

extern void* fn_80193828(HSD_ClassInfo* info); /* hsdNew              */
extern HSD_ClassInfo* fn_80193748(char* name); /* hsdSearchClassInfo  */
extern void* fn_80193B10(s32 size);            /* hsdAllocMemPiece    */
extern void fn_80193AF0(void* mem, s32 size);  /* hsdFreeMemPiece     */
extern void* fn_801A6928(s32 size);            /* HSD_MemAlloc        */
extern void fn_801A6960(void* mem);            /* HSD_Free            */
extern void fn_8019C6EC(u32 mask);             /* HSD_StateInvalidate */

extern void* memset(void* dst, int c, u32 n);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void OSReport(const char* fmt, ...);
extern void HSD_Panic(const char* file, u32 line, const char* msg);

extern HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* desc);
extern void HSD_AObjRemove(HSD_AObj* aobj);
extern void HSD_AObjReqAnim(HSD_AObj* aobj, f32 startframe);
extern void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* obj,
                                  void (*update)(void*, u32, HSD_ObjData*));

/* ------------------------------------------------------------------ */
/*  Class info / data symbols                                          */
/* ------------------------------------------------------------------ */

extern u8 lbl_8036D3F0[]; /* hsdTObj class info */

extern char lbl_80275638[]; /* "sysdolphin_base_library" */
extern char lbl_80275650[]; /* "hsd_tobj" */
extern char lbl_8027565C[]; /* "texmtx index exceed hardware limit (%d).\n" */
extern char lbl_802756C4[]; /* "tobj->repeat_s && tobj->repeat_t" */
extern char lbl_802756E8[]; /* "tobj->imagetbl" */

extern void PSMTXTrans(Mtx m, f32 x, f32 y, f32 z);
extern void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx dst);
extern void PSMTXMultVecSR(const Mtx m, const Vec* src, Vec* dst);
extern void PSVECNormalize(const Vec* src, Vec* dst);
extern void GXLoadTexMtxImm(Mtx m, u32 id, s32 type);
extern void HSD_MkRotationMtx(Mtx m, Vec* rot);
extern void GXSetTexCoordGen(u32 coord, u32 func, u32 src, u32 mtx);
extern void GXSetTexCoordGen2(u32 coord, u32 func, u32 src, u32 mtx,
                              u32 normalize, u32 postmtx);

extern HSD_TExp* fn_801B707C(HSD_TExp** list);
extern void fn_801B5E40(HSD_TExp* exp, HSD_TObj* tobj, u32 chan);
extern void fn_801B6E74(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
extern void fn_801B6CD8(HSD_TExp* exp, u32 op, u32 bias, u32 scale,
                        u32 clamp);
extern void fn_801B64EC(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
extern void fn_801B5F08(HSD_TExp* exp, u32 sel_a, HSD_TExp* a, u32 sel_b,
                        HSD_TExp* b, u32 sel_c, HSD_TExp* c, u32 sel_d,
                        HSD_TExp* d);
extern HSD_TExp* HSD_TExpCnst(void* ptr, u32 comp, u32 type,
                              HSD_TExp** list);

#define HSD_TEXP_TEX ((HSD_TExp*) -1)
#define HSD_TEXP_ZERO ((HSD_TExp*) 0)
#define HSD_TE_RGB 1
#define HSD_TE_A 5
#define HSD_TE_X 6
#define HSD_TE_0 7
#define HSD_TE_1 8
#define HSD_TE_4_8 12
#define HSD_TE_U8 0
#define HSD_TE_F32 3
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

/* .sdata2 strings */
extern const char lbl_8047DEB0[7]; /* __FILE__ */
extern const char lbl_8047DEB8[8]; /* "idesc" */
extern const char lbl_8047DEC0[4]; /* "tev" */
extern const char lbl_8047DEC4[8]; /* "tlut" */
extern const char lbl_8047DECC[4]; /* "new" */
extern const char lbl_8047DED0[4]; /* "0" */
extern const char lbl_8047DED4[4]; /* "" */
extern const char lbl_8047DF10[8]; /* "tobj" */
extern const char lbl_8047DEE8[8]; /* "cobj" */
extern Mtx lbl_8036D43C;

#define HSD_TOBJ_FILE lbl_8047DEB0

#define hsdTObjInfo HSD_TOBJ_INFO(lbl_8036D3F0)
#define hsdTObjClass HSD_CLASS_INFO(lbl_8036D3F0)

/* ------------------------------------------------------------------ */
/*  File statics.  Named for their addresses so that the relocations   */
/*  land on the original .sbss symbols.                                */
/* ------------------------------------------------------------------ */

static HSD_ClassInfo* lbl_8047B378 = NULL; /* default_class */
static HSD_TObj* lbl_8047B37C = NULL;      /* tobj_head */

#define default_class lbl_8047B378
#define tobj_head lbl_8047B37C

/* ------------------------------------------------------------------ */
/*  sysdolphin's small deallocators.  They are always inlined; keeping  */
/*  them as functions is what makes the caller load the pointer once.   */
/* ------------------------------------------------------------------ */

static inline void HSD_TlutFree(HSD_Tlut* tlut)
{
    fn_80193AF0(tlut, sizeof(HSD_Tlut));
}

static inline void HSD_TlutRemove(HSD_Tlut* tlut)
{
    if (tlut != NULL) {
        HSD_TlutFree(tlut);
    }
}

static inline void HSD_TObjTevFree(HSD_TObjTev* tev)
{
    fn_80193AF0(tev, sizeof(HSD_TObjTev));
}

static inline void HSD_TObjTevRemove(HSD_TObjTev* tev)
{
    if (tev != NULL) {
        HSD_TObjTevFree(tev);
    }
}

static inline HSD_Tlut* HSD_TlutAlloc(void)
{
    HSD_Tlut* tlut = fn_80193B10(sizeof(HSD_Tlut));
    if (tlut == NULL) {
        __assert(HSD_TOBJ_FILE, 0x8A1, lbl_8047DEC4);
    }
    memset(tlut, 0, sizeof(HSD_Tlut));
    return tlut;
}

static inline HSD_Tlut* HSD_TlutLoadDesc(HSD_TlutDesc* tlutdesc)
{
    if (tlutdesc != NULL) {
        HSD_Tlut* tlut = HSD_TlutAlloc();
        memcpy(tlut, tlutdesc, sizeof(HSD_Tlut));
        return tlut;
    }
    return NULL;
}

static inline HSD_TObjTev* HSD_TObjTevAlloc(void)
{
    HSD_TObjTev* tev = fn_80193B10(sizeof(HSD_TObjTev));
    if (tev == NULL) {
        __assert(HSD_TOBJ_FILE, 0x8CC, lbl_8047DEC0);
    }
    memset(tev, 0, sizeof(HSD_TObjTev));
    return tev;
}

static inline HSD_TObjTev* HSD_TObjTevLoadDesc(HSD_TObjTevDesc* tevdesc)
{
    if (tevdesc != NULL) {
        HSD_TObjTev* tev = HSD_TObjTevAlloc();
        memcpy(tev, tevdesc, sizeof(HSD_TObjTev));
        return tev;
    }
    return NULL;
}

/* ------------------------------------------------------------------ */

static void TObjInfoInit(void);
static void TObjAmnesia(HSD_ClassInfo* info);
static void TObjRelease(HSD_Class* o);
static int TObjLoad(HSD_TObj* tobj, HSD_TObjDesc* td);
static void MakeTextureMtx(HSD_TObj* tobj);
static void MakeColorGenTExp(u32 lightmap, HSD_TObj* tobj, HSD_TExp** c,
                             HSD_TExp** a, HSD_TExp** list, int repeat);
static void TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                         HSD_TExp** c, HSD_TExp** a, HSD_TExp** list);
static void TObjSetupMtx(HSD_TObj* tobj);

/** TObjInit - the class `init` method (0x801BBCE0). */
int fn_801BBCE0(HSD_TObj* tobj);
/** HSD_TObjInsert - splice `next` in behind `tobj` (0x801BBE3C). */
void fn_801BBE3C(HSD_TObj* tobj, HSD_TObj* next);
/** TObjUpdateFunc - the class `update` method (0x801BE85C). */
void fn_801BE85C(void* obj, u32 type, HSD_ObjData* val);

/* ========================================================================= */
/*  0x801BBAC8 | 0xEC  TObjInfoInit                                          */
/* ========================================================================= */
#pragma push
#pragma optimization_level 0
static void TObjInfoInit(void)
{
    hsdInitClassInfo(hsdTObjClass, HSD_CLASS_INFO(&hsdObj),
                     lbl_80275638, lbl_80275650,
                     sizeof(HSD_TObjInfo), sizeof(HSD_TObj));

    hsdTObjClass->init = (int (*)(HSD_Class*)) fn_801BBCE0;
    hsdTObjClass->release = TObjRelease;
    hsdTObjClass->amnesia = TObjAmnesia;
    hsdTObjInfo->load = TObjLoad;
    hsdTObjInfo->make_texp = TObjMakeTExp;
    hsdTObjInfo->make_mtx = MakeTextureMtx;
    hsdTObjInfo->update = fn_801BE85C;
}
#pragma pop

/* ========================================================================= */
/*  0x801BBBB4 | 0x60  TObjAmnesia                                           */
/* ========================================================================= */
#pragma push
#pragma optimization_level 4
static void TObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == hsdTObjClass) {
        tobj_head = NULL;
    }
    hsdTObjClass->head.parent->amnesia(info);
}
#pragma pop

/* ========================================================================= */
/*  0x801BBC14 | 0xCC  TObjRelease                                           */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
static void TObjRelease(HSD_Class* o)
{
    HSD_TObj* tobj = HSD_TOBJ(o);

    HSD_AObjRemove(tobj->aobj);
    HSD_TlutRemove(tobj->tlut);
    HSD_TObjTevRemove(tobj->tev);

    if (tobj->tluttbl != NULL) {
        int i;
        for (i = 0; tobj->tluttbl[i] != NULL; i++) {
            HSD_TlutRemove(tobj->tluttbl[i]);
        }
        fn_801A6960(tobj->tluttbl);
    }

    hsdTObjClass->head.parent->release(o);
}
#pragma pop

/* ========================================================================= */
/*  0x801BBCE0 | 0x5C  fn_801BBCE0 -- TObjInit (class `init` method)         */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
int fn_801BBCE0(HSD_TObj* tobj)
{
    int result = hsdTObjClass->head.parent->init((HSD_Class*) tobj);
    if (result >= 0) {
        tobj->anim_id = TOBJ_ID_NULL;
        result = 0;
    }
    return result;
}
#pragma pop

/* ========================================================================= */
/*  0x801BBD3C | 0x24  HSD_ImageDescFree                                     */
/* ========================================================================= */
#pragma push
#pragma optimization_level 4
void HSD_ImageDescFree(HSD_ImageDesc* idesc)
{
    fn_80193AF0(idesc, sizeof(HSD_ImageDesc));
}
#pragma pop

/* ========================================================================= */
/*  0x801BBD60 | 0x24  HSD_ImageDescRemove                                   */
/* ========================================================================= */
#pragma push
#pragma optimization_level 4
void HSD_ImageDescRemove(HSD_ImageDesc* idesc)
{
    fn_80193AF0(idesc, sizeof(HSD_ImageDesc));
}
#pragma pop

/* ========================================================================= */
/*  0x801BBD84 | 0x58  HSD_ImageDescAlloc                                    */
/* ========================================================================= */
#pragma push
#pragma optimization_level 4
HSD_ImageDesc* HSD_ImageDescAlloc(void)
{
    HSD_ImageDesc* idesc = fn_80193B10(sizeof(HSD_ImageDesc));
    if (idesc == NULL) {
        __assert(HSD_TOBJ_FILE, 0x8F7, lbl_8047DEB8);
    }
    memset(idesc, 0, sizeof(HSD_ImageDesc));
    return idesc;
}
#pragma pop

/* ========================================================================= */
/*  0x801BBDDC | 0x60  HSD_TObjAlloc                                         */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
HSD_TObj* HSD_TObjAlloc(void)
{
    HSD_TObj* tobj =
        fn_80193828(default_class != NULL ? default_class : hsdTObjClass);
    if (tobj == NULL) {
        __assert(HSD_TOBJ_FILE, 0x884, lbl_8047DECC);
    }
    return tobj;
}
#pragma pop

/* ========================================================================= */
/*  0x801BBE3C | 0x24  fn_801BBE3C -- HSD_TObjInsert                         */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
void fn_801BBE3C(HSD_TObj* tobj, HSD_TObj* next)
{
    if (tobj == NULL || next == NULL) {
        return;
    }
    next->next = tobj->next;
    tobj->next = next;
}
#pragma pop

/* ========================================================================= */
/*  0x801BBE60 | 0x74  HSD_TObjRemoveAll                                     */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
void HSD_TObjRemoveAll(HSD_TObj* tobj)
{
    while (tobj != NULL) {
        HSD_TObj* next = tobj->next;
        hsdDelete(tobj);
        tobj = next;
    }
}

static inline void setupTextureCoordGen(HSD_TObj* tobj)
{
    switch (tobj_coord(tobj)) {
    case TEX_COORD_SHADOW:
        GXSetTexCoordGen2(tobj->coord, GX_TG_MTX3x4, GX_TG_POS, 0,
                          GX_DISABLE, tobj->mtxid);
        break;
    case TEX_COORD_REFLECTION:
    case TEX_COORD_HILIGHT:
        GXSetTexCoordGen2(tobj->coord, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0,
                          GX_ENABLE, tobj->mtxid);
        break;
    default:
        if (tobj_bump(tobj)) {
            GXSetTexCoordGen(tobj->coord, GX_TG_MTX2x4, tobj->src,
                             tobj->mtxid);
        } else {
            GXSetTexCoordGen2(tobj->coord, GX_TG_MTX2x4, tobj->src,
                              GX_IDENTITY, GX_DISABLE, tobj->mtxid);
        }
        break;
    }
}

static inline void setupTextureCoordGenBump(HSD_TObj* bump)
{
    static const u32 func[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
    u32 mask = HSD_LObjGetLightMaskDiffuse();
    int i;

    for (i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            break;
        }
    }
    if (i >= 8) {
        i = 0;
    }
    GXSetTexCoordGen(bump->coord + 1, func[i], bump->coord + 12,
                     GX_IDENTITY);
}

void fn_801BDA58(HSD_TObj* tobj)
{
    for (; tobj != NULL; tobj = tobj->next) {
        if (tobj->id == GX_TEXMAP_NULL) {
            continue;
        }
        if (tobj_bump(tobj)) {
            setupTextureCoordGen(tobj);
            setupTextureCoordGenBump(tobj);
        } else if (tobj_coord(tobj) == TEX_COORD_TOON) {
            GXSetTexCoordGen(tobj->coord, GX_TG_SRTG, tobj->src,
                             GX_IDENTITY);
        } else {
            setupTextureCoordGen(tobj);
        }
    }
}
#pragma pop

/* ========================================================================= */
/*  0x801BBED4 | 0x54  HSD_TObjRemove                                        */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
void HSD_TObjRemove(HSD_TObj* tobj)
{
    hsdDelete(tobj);
}
#pragma pop

/* ========================================================================= */
/*  0x801BBF28 | 0xBC  HSD_Index2TexMtx                                      */
/* ========================================================================= */
#pragma push
#pragma optimization_level 1
u32 HSD_Index2TexMtx(u32 index)
{
    switch (index) {
    case 0:
        return GX_TEXMTX0;
    case 1:
        return GX_TEXMTX0 + 3;
    case 2:
        return GX_TEXMTX0 + 6;
    case 3:
        return GX_TEXMTX0 + 9;
    case 4:
        return GX_TEXMTX0 + 12;
    case 5:
        return GX_TEXMTX0 + 15;
    case 6:
        return GX_TEXMTX0 + 18;
    case 7:
        return GX_TEXMTX0 + 21;
    case 8:
        return GX_TEXMTX0 + 24;
    case 9:
        return GX_TEXMTX9;
    case 10:
        return GX_IDENTITY;
    default:
        OSReport(lbl_8027565C, index);
        HSD_Panic(HSD_TOBJ_FILE, 0x7E1, lbl_8047DED4);
    }
    return GX_IDENTITY;
}
#pragma pop

/* ========================================================================= */
/*  0x801BE490 | 0x3C  _HSD_TObjGetCurrentByType                             */
/* ========================================================================= */
#pragma push
#pragma optimization_level 4
HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj* from, u32 mapping)
{
    HSD_TObj* tp;

    if (from == NULL) {
        tp = tobj_head;
    } else {
        tp = from->next;
    }

    for (; tp != NULL; tp = tp->next) {
        if (tobj_coord(tp) == mapping) {
            goto END;
        }
    }

    tp = NULL;
END:
    return tp;
}
#pragma pop

/* ========================================================================= */
/*  0x801BE4CC | 0xCC  HSD_TObjLoadDesc                                      */
/* ========================================================================= */
HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc* td)
{
    if (td != NULL) {
        HSD_TObj* tobj;
        HSD_ClassInfo* info;

        if (td->class_name == NULL ||
            (info = fn_80193748(td->class_name)) == NULL)
        {
            tobj = HSD_TObjAlloc();
        } else {
            tobj = fn_80193828(info);
            if (tobj == NULL) {
                __assert(HSD_TOBJ_FILE, 0x1ED, lbl_8047DF10);
            }
        }
        HSD_TOBJ_METHOD(tobj)->load(tobj, td);
        return tobj;
    } else {
        return NULL;
    }
}

/* ========================================================================= */
/*  0x801BE598 | 0x268  TObjLoad                                             */
/* ========================================================================= */
static int TObjLoad(HSD_TObj* tobj, HSD_TObjDesc* td)
{
    tobj->next = HSD_TObjLoadDesc(td->next);
    tobj->anim_id = (u16) td->id;
    tobj->src = td->src;
    tobj->mtxid = GX_IDENTITY;
    tobj->rotate.x = td->rotate.x;
    tobj->rotate.y = td->rotate.y;
    tobj->rotate.z = td->rotate.z;
    tobj->scale = td->scale;
    tobj->translate = td->translate;
    tobj->wrap_s = td->wrap_s;
    tobj->wrap_t = td->wrap_t;
    tobj->repeat_s = td->repeat_s;
    tobj->repeat_t = td->repeat_t;
    tobj->flags = td->blend_flags;
    tobj->blending = td->blending;
    tobj->magFilt = td->magFilt;
    tobj->imagedesc = td->imagedesc;
    tobj->tlut = HSD_TlutLoadDesc(td->tlutdesc);
    tobj->lod = td->lod;
    tobj->aobj = NULL;
    tobj->flags |= TEX_MTX_DIRTY;
    tobj->tlut_no = (u8) -1;
    tobj->tev = HSD_TObjTevLoadDesc(td->tev);

    fn_8019C6EC(2);

    return 0;
}

/* ========================================================================= */
/*  0x801BE800 | 0x5C  fn_801BE800 -- HSD_TObjAnimAll                        */
/* ========================================================================= */

static inline void HSD_TObjAnim(HSD_TObj* tobj)
{
    if (tobj == NULL) {
        return;
    }
    HSD_AObjInterpretAnim(tobj->aobj, tobj, HSD_TOBJ_METHOD(tobj)->update);
}

void fn_801BE800(HSD_TObj* tobj)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjAnim(tp);
        }
    }
}

/* --- Non-linked CodeCandidate placeholders. These remain intentionally
       incomplete until their target bodies are decompiled. --- */

/* ========================================================================= */
/*  0x801BE2B4 | 0x1DC  MakeTextureMtx                                       */
/* ========================================================================= */

/** 1.0e-10f, the .sdata2 constant at 0x8047DF00. */
#define TOBJ_SCALE_EPSILON 1.0e-10F

static void MakeTextureMtx(HSD_TObj* tobj)
{
    Vec scale;
    Mtx m;
    Vec trans;
    Quaternion rot;

    int no_assert = 0;

    if (tobj->repeat_s && tobj->repeat_t) {
        no_assert = 1;
    }

    if (!no_assert) {
        __assert(HSD_TOBJ_FILE, 0x267, lbl_802756C4);
    }

    scale.x = (f32) __fabs(tobj->scale.x) < TOBJ_SCALE_EPSILON
                  ? 0.0F
                  : (f32) tobj->repeat_s / tobj->scale.x;
    scale.y = (f32) __fabs(tobj->scale.y) < TOBJ_SCALE_EPSILON
                  ? 0.0F
                  : (f32) tobj->repeat_t / tobj->scale.y;
    scale.z = tobj->scale.z;
    rot.x = tobj->rotate.x;
    rot.y = tobj->rotate.y;
    rot.z = -tobj->rotate.z;
    trans.x = -tobj->translate.x;
    trans.y =
        -(tobj->translate.y + (tobj->wrap_t == GX_MIRROR
                                   ? 1.0F / (tobj->repeat_t / tobj->scale.y)
                                   : 0.0F));
    trans.z = tobj->translate.z;

    PSMTXTrans(tobj->mtx, trans.x, trans.y, trans.z);
    HSD_MkRotationMtx(m, (Vec*) &rot);
    PSMTXConcat(m, tobj->mtx, tobj->mtx);
    PSMTXScale(m, scale.x, scale.y, scale.z);
    PSMTXConcat(m, tobj->mtx, tobj->mtx);
}

static inline u32 HSD_Index2TexMap(u32 index)
{
    switch (index) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    case 5:
        return 5;
    case 6:
        return 6;
    case 7:
        return 7;
    default:
        __assert(HSD_TOBJ_FILE, 0x794, lbl_8047DED0);
    }
    return 0;
}

static inline u32 HSD_Index2TexCoord(u32 index)
{
    switch (index) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    case 5:
        return 5;
    case 6:
        return 6;
    case 7:
        return 7;
    default:
        __assert(HSD_TOBJ_FILE, 0x807, lbl_8047DED0);
    }
    return 0;
}

static inline u32 HSD_TexMapID2PTTexMtx(u32 id)
{
    switch (id) {
    case 0:
        return 64;
    case 1:
        return 67;
    case 2:
        return 70;
    case 3:
        return 73;
    case 4:
        return 76;
    case 5:
        return 79;
    case 6:
        return 82;
    case 7:
        return 85;
    default:
        __assert(HSD_TOBJ_FILE, 0x258, lbl_8047DED0);
    }
    return 64;
}

s32 HSD_TObjAssignResources(HSD_TObj* tobj_top)
{
    HSD_TObj* tobj;
    u32 texmap_no = 0;
    u32 texcoord_no = 0;
    u32 limit = 8;
    HSD_TObj* bump = NULL;
    HSD_TObj* toon = NULL;

    if (tobj_top == NULL) {
        return 0;
    }

    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if (tobj_coord(tobj) == TEX_COORD_TOON) {
            toon = tobj;
        } else if (tobj_bump(tobj)) {
            bump = tobj;
        }
    }

    if (toon != NULL) {
        limit--;
    }
    if (bump != NULL) {
        limit -= 2;
    }

    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if (tobj_coord(tobj) == TEX_COORD_TOON) {
            if (tobj != toon) {
                tobj->id = GX_TEXMAP_NULL;
            }
        } else if (tobj_bump(tobj)) {
            if (tobj != bump) {
                tobj->id = GX_TEXMAP_NULL;
            }
        } else if (texmap_no < limit) {
            tobj->id = HSD_Index2TexMap(texmap_no++);
            tobj->mtxid = HSD_TexMapID2PTTexMtx(tobj->id);
            switch (tobj_coord(tobj)) {
            case TEX_COORD_REFLECTION:
            case TEX_COORD_HILIGHT:
            case TEX_COORD_SHADOW:
                tobj->coord = HSD_Index2TexCoord(texcoord_no++);
                break;
            }
        } else {
            tobj->id = GX_TEXMAP_NULL;
        }
    }

    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if (tobj->id != GX_TEXMAP_NULL && !tobj_bump(tobj) &&
            tobj_coord(tobj) == TEX_COORD_UV)
        {
            tobj->coord = HSD_Index2TexCoord(texcoord_no++);
        }
    }

    if (bump != NULL) {
        bump->id = HSD_Index2TexMap(texmap_no++);
        bump->mtxid = GX_TEXMTX9;
        bump->coord = HSD_Index2TexCoord(texcoord_no);
        texcoord_no += 2;
    }
    if (toon != NULL) {
        toon->id = HSD_Index2TexMap(texmap_no++);
        toon->coord = HSD_Index2TexCoord(texcoord_no++);
    }

    return texcoord_no;
}

static void TObjSetupMtx(HSD_TObj* tobj)
{
    int i;

    if (tobj_coord(tobj) == TEX_COORD_TOON) {
        return;
    }

    if (tobj->flags & TEX_MTX_DIRTY) {
        HSD_TOBJ_METHOD(tobj)->make_mtx(tobj);
        tobj->flags &= ~TEX_MTX_DIRTY;
    }

    switch (tobj_coord(tobj)) {
    case TEX_COORD_REFLECTION: {
        Mtx mtx;

        for (i = 0; i < 3; i++) {
            mtx[i][0] = 0.5F * tobj->mtx[i][0];
            mtx[i][1] = -0.5F * tobj->mtx[i][1];
            mtx[i][2] = 0.0F;
            mtx[i][3] = 0.5F * tobj->mtx[i][0] +
                        0.5F * tobj->mtx[i][1] + tobj->mtx[i][2] +
                        tobj->mtx[i][3];
        }
        GXLoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
        break;
    }

    case TEX_COORD_HILIGHT: {
        HSD_LObj* lobj = HSD_LObjGetCurrentByType(LOBJ_INFINITE);

        if (lobj != NULL) {
            HSD_CObj* cobj;
            Vec ldir;
            Vec half;
            Mtx mtx;
            MtxPtr vmtx;

            cobj = HSD_CObjGetCurrent();
            if (cobj == NULL) {
                __assert(HSD_TOBJ_FILE, 0x2A8, lbl_8047DEE8);
            }
            vmtx = (MtxPtr) HSD_CObjGetViewingMtxPtrDirect(cobj);
            HSD_LObjGetLightVector(lobj, &ldir);
            PSMTXMultVecSR(vmtx, &ldir, &ldir);
            ldir.z += -1.0F;
            PSVECNormalize(&ldir, &half);
            half.x *= -0.5;
            half.y *= -0.5;
            half.z *= -0.5;

            mtx[0][0] = tobj->mtx[0][0] * half.x;
            mtx[0][1] = tobj->mtx[0][0] * half.y;
            mtx[0][2] = tobj->mtx[0][0] * half.z;
            mtx[0][3] = tobj->mtx[0][0] * 0.5F + tobj->mtx[0][3];
            mtx[1][0] = tobj->mtx[1][0] * half.x;
            mtx[1][1] = tobj->mtx[1][0] * half.y;
            mtx[1][2] = tobj->mtx[1][0] * half.z;
            mtx[1][3] = tobj->mtx[1][0] * 0.5F + tobj->mtx[1][3];
            mtx[2][0] = mtx[2][1] = mtx[2][2] = 0.0F;
            mtx[2][3] = 1.0F;
            GXLoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
        } else {
            GXLoadTexMtxImm(lbl_8036D43C, tobj->mtxid, GX_MTX3x4);
        }
        break;
    }

    case TEX_COORD_SHADOW: {
        HSD_CObj* cobj = HSD_CObjGetCurrent();
        Mtx mtx;

        PSMTXConcat(tobj->mtx,
                    (MtxPtr) HSD_CObjGetInvViewingMtxPtrDirect(cobj), mtx);
        GXLoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
        break;
    }

    default:
        if (tobj_bump(tobj)) {
            GXLoadTexMtxImm(tobj->mtx, tobj->mtxid, GX_MTX2x4);
        } else {
            GXLoadTexMtxImm(tobj->mtx, tobj->mtxid, GX_MTX3x4);
        }
        break;
    }
}

static void TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                         HSD_TExp** c, HSD_TExp** a, HSD_TExp** list)
{
    HSD_TExp *e0, *e1;
    HSD_TExp *c_src, *a_src;
    u32 c_sel, a_sel;
    int repeat = lightmap_done & tobj_lightmap(tobj);

    c_src = HSD_TEXP_TEX;
    c_sel = HSD_TE_RGB;
    a_src = HSD_TEXP_TEX;
    a_sel = HSD_TE_A;
    e0 = HSD_TExpTev(list);

    if (tobj->tev &&
        (tobj->tev->active & (TOBJ_TEVREG_ACTIVE_COLOR_TEV |
                              TOBJ_TEVREG_ACTIVE_ALPHA_TEV))) {
        MakeColorGenTExp(lightmap, tobj, &c_src, &a_src, list, repeat);
    }

    HSD_TExpOrder(e0, tobj, GX_COLOR_NULL);
    switch (tobj_colormap(tobj)) {
    case TEX_COLORMAP_ALPHA_MASK:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_RGB, *c, c_sel, c_src, a_sel, a_src,
                        HSD_TE_0, HSD_TEXP_ZERO);
        break;
    case TEX_COLORMAP_RGB_MASK:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_RGB, *c, c_sel, c_src, c_sel, c_src,
                        HSD_TE_0, HSD_TEXP_ZERO);
        break;
    case TEX_COLORMAP_BLEND:
        e1 = HSD_TExpCnst(&tobj->blending, HSD_TE_X, HSD_TE_F32, list);
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_RGB, *c, c_sel, c_src, HSD_TE_X, e1,
                        HSD_TE_0, HSD_TEXP_ZERO);
        break;
    case TEX_COLORMAP_MODULATE:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB, *c, c_sel,
                        c_src, HSD_TE_0, HSD_TEXP_ZERO);
        break;
    case TEX_COLORMAP_REPLACE:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                        HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, c_sel, c_src);
        break;
    case TEX_COLORMAP_NONE:
    case TEX_COLORMAP_PASS:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                        HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_RGB,
                        *c);
        break;
    case TEX_COLORMAP_ADD:
        HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, c_sel, c_src, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                        HSD_TEXP_ZERO, HSD_TE_RGB, *c);
        break;
    case TEX_COLORMAP_SUB:
        HSD_TExpColorOp(e0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
        HSD_TExpColorIn(e0, c_sel, c_src, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                        HSD_TEXP_ZERO, HSD_TE_RGB, *c);
        break;
    default:
        HSD_ASSERT(1372, 0);
        break;
    }
    *c = e0;

    if (!repeat) {
        switch (tobj_alphamap(tobj)) {
        case TEX_ALPHAMAP_ALPHA_MASK:
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, HSD_TE_A, *a, a_sel, a_src, a_sel, a_src,
                            HSD_TE_0, HSD_TEXP_ZERO);
            break;
        case TEX_ALPHAMAP_BLEND:
            e1 = HSD_TExpCnst(&tobj->blending, HSD_TE_X, HSD_TE_F32, list);
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, HSD_TE_A, *a, a_sel, a_src, HSD_TE_X, e1,
                            HSD_TE_0, HSD_TEXP_ZERO);
            break;
        case TEX_ALPHAMAP_MODULATE:
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A, *a, a_sel,
                            a_src, HSD_TE_0, HSD_TEXP_ZERO);
            break;
        case TEX_ALPHAMAP_REPLACE:
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, a_sel,
                            a_src);
            break;
        case TEX_ALPHAMAP_NONE:
        case TEX_ALPHAMAP_PASS:
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                            HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A,
                            *a);
            break;
        case TEX_ALPHAMAP_ADD:
            HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, a_sel, a_src, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A, *a);
            break;
        case TEX_ALPHAMAP_SUB:
            HSD_TExpAlphaOp(e0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1,
                            GX_ENABLE);
            HSD_TExpAlphaIn(e0, a_sel, a_src, HSD_TE_0, HSD_TEXP_ZERO,
                            HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_A, *a);
            break;
        default:
            HSD_ASSERT(1430, 0);
            break;
        }
        *a = e0;
    }
}

static void MakeColorGenTExp(u32 lightmap, HSD_TObj* tobj, HSD_TExp** c,
                             HSD_TExp** a, HSD_TExp** list, int repeat)
{
    HSD_TObjTev* tev = tobj->tev;
    HSD_TExp *e0, *tmp;
    HSD_TExp *konst_rgb, *konst_r, *konst_g, *konst_b, *konst_a;
    HSD_TExp *reg0_rgb, *reg0_a, *reg1_rgb, *reg1_a;
    u8* in;
    u32 sel[4];
    HSD_TExp* exp[4];
    int use_k_rgb = 0, use_k_r = 0, use_k_g = 0, use_k_b = 0;
    int use_k_a = 0, use_reg0_rgb = 0, use_reg0_a = 0;
    int use_reg1_rgb = 0, use_reg1_a = 0;
    int i;

    (void) lightmap;
    (void) repeat;
    in = &tev->color_a;
    for (i = 0; i < 4; i++) {
        switch (in[i]) {
        case TOBJ_TEV_CC_KONST_RGB: use_k_rgb = 1; break;
        case TOBJ_TEV_CC_KONST_RRR: use_k_r = 1; break;
        case TOBJ_TEV_CC_KONST_GGG: use_k_g = 1; break;
        case TOBJ_TEV_CC_KONST_BBB: use_k_b = 1; break;
        case TOBJ_TEV_CC_KONST_AAA: use_k_a = 1; break;
        case TOBJ_TEV_CC_TEX0_RGB: use_reg0_rgb = 1; break;
        case TOBJ_TEV_CC_TEX0_AAA: use_reg0_a = 1; break;
        case TOBJ_TEV_CC_TEX1_RGB: use_reg1_rgb = 1; break;
        case TOBJ_TEV_CC_TEX1_AAA: use_reg1_a = 1; break;
        }
    }
    in = &tev->alpha_a;
    for (i = 0; i < 4; i++) {
        switch (in[i]) {
        case TOBJ_TEV_CA_KONST_R: use_k_r = 1; break;
        case TOBJ_TEV_CA_KONST_G: use_k_g = 1; break;
        case TOBJ_TEV_CA_KONST_B: use_k_b = 1; break;
        case TOBJ_TEV_CA_KONST_A: use_k_a = 1; break;
        case TOBJ_TEV_CA_TEX0_A: use_reg0_a = 1; break;
        case TOBJ_TEV_CA_TEX1_A: use_reg1_a = 1; break;
        }
    }
    if (use_k_rgb)
        konst_rgb = HSD_TExpCnst(&tev->konst, HSD_TE_RGB, HSD_TE_U8, list);
    if (use_k_r)
        konst_r = HSD_TExpCnst(&tev->konst.r, HSD_TE_X, HSD_TE_U8, list);
    if (use_k_g)
        konst_g = HSD_TExpCnst(&tev->konst.g, HSD_TE_X, HSD_TE_U8, list);
    if (use_k_b)
        konst_b = HSD_TExpCnst(&tev->konst.b, HSD_TE_X, HSD_TE_U8, list);
    if (use_k_a)
        konst_a = HSD_TExpCnst(&tev->konst.a, HSD_TE_X, HSD_TE_U8, list);
    if (use_reg0_rgb)
        reg0_rgb = HSD_TExpCnst(&tev->tev0, HSD_TE_RGB, HSD_TE_U8, list);
    if (use_reg0_a)
        reg0_a = HSD_TExpCnst(&tev->tev0.a, HSD_TE_X, HSD_TE_U8, list);
    if (use_reg1_rgb)
        reg1_rgb = HSD_TExpCnst(&tev->tev1, HSD_TE_RGB, HSD_TE_U8, list);
    if (use_reg1_a)
        reg1_a = HSD_TExpCnst(&tev->tev1.a, HSD_TE_X, HSD_TE_U8, list);

    e0 = HSD_TExpTev(list);
    HSD_TExpOrder(e0, tobj, GX_COLOR_NULL);
    if (tev->active & TOBJ_TEVREG_ACTIVE_COLOR_TEV) {
        in = &tev->color_a;
        for (i = 0; i < 4; i++) {
            switch (in[i]) {
            case GX_CC_ZERO: sel[i] = HSD_TE_0; exp[i] = HSD_TEXP_ZERO; break;
            case GX_CC_ONE: sel[i] = HSD_TE_1; exp[i] = HSD_TEXP_ZERO; break;
            case GX_CC_HALF: sel[i] = HSD_TE_4_8; exp[i] = HSD_TEXP_ZERO; break;
            case GX_CC_TEXC: sel[i] = HSD_TE_RGB; exp[i] = HSD_TEXP_TEX; break;
            case GX_CC_TEXA: sel[i] = HSD_TE_A; exp[i] = HSD_TEXP_TEX; break;
            case TOBJ_TEV_CC_KONST_RGB: sel[i] = HSD_TE_RGB; exp[i] = konst_rgb; break;
            case TOBJ_TEV_CC_KONST_RRR: sel[i] = HSD_TE_X; exp[i] = konst_r; break;
            case TOBJ_TEV_CC_KONST_GGG: sel[i] = HSD_TE_X; exp[i] = konst_g; break;
            case TOBJ_TEV_CC_KONST_BBB: sel[i] = HSD_TE_X; exp[i] = konst_b; break;
            case TOBJ_TEV_CC_KONST_AAA: sel[i] = HSD_TE_X; exp[i] = konst_a; break;
            case TOBJ_TEV_CC_TEX0_RGB:
            case TOBJ_TEV_CC_TEX0_AAA:
            case TOBJ_TEV_CC_TEX1_RGB:
            case TOBJ_TEV_CC_TEX1_AAA:
                tmp = HSD_TExpTev(list);
                HSD_TExpOrder(tmp, NULL, GX_COLOR_NULL);
                HSD_TExpColorOp(tmp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                                GX_ENABLE);
                if (in[i] == TOBJ_TEV_CC_TEX0_RGB ||
                    in[i] == TOBJ_TEV_CC_TEX1_RGB) {
                    HSD_TExp* reg = in[i] == TOBJ_TEV_CC_TEX0_RGB ?
                                    reg0_rgb : reg1_rgb;
                    HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                                    HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                                    HSD_TE_RGB, reg);
                } else {
                    HSD_TExp* reg = in[i] == TOBJ_TEV_CC_TEX0_AAA ?
                                    reg0_a : reg1_a;
                    HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                                    HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                                    HSD_TE_X, reg);
                }
                sel[i] = HSD_TE_RGB;
                exp[i] = tmp;
                break;
            default: HSD_ASSERT(0x4A7, 0); break;
            }
        }
        HSD_TExpColorOp(e0, tev->color_op, tev->color_bias, tev->color_scale,
                        tev->color_clamp);
        HSD_TExpColorIn(e0, sel[0], exp[0], sel[1], exp[1], sel[2], exp[2],
                        sel[3], exp[3]);
        *c = e0;
    }
    if (tev->active & TOBJ_TEVREG_ACTIVE_ALPHA_TEV) {
        in = &tev->alpha_a;
        for (i = 0; i < 4; i++) {
            switch (in[i]) {
            case GX_CA_ZERO: sel[i] = HSD_TE_0; exp[i] = HSD_TEXP_ZERO; break;
            case GX_CA_TEXA: sel[i] = HSD_TE_A; exp[i] = HSD_TEXP_TEX; break;
            case TOBJ_TEV_CA_KONST_R: sel[i] = HSD_TE_X; exp[i] = konst_r; break;
            case TOBJ_TEV_CA_KONST_G: sel[i] = HSD_TE_X; exp[i] = konst_g; break;
            case TOBJ_TEV_CA_KONST_B: sel[i] = HSD_TE_X; exp[i] = konst_b; break;
            case TOBJ_TEV_CA_KONST_A: sel[i] = HSD_TE_X; exp[i] = konst_a; break;
            case TOBJ_TEV_CA_TEX0_A:
            case TOBJ_TEV_CA_TEX1_A:
                tmp = HSD_TExpTev(list);
                HSD_TExpOrder(tmp, NULL, GX_COLOR_NULL);
                HSD_TExpAlphaOp(tmp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                                GX_ENABLE);
                HSD_TExpAlphaIn(tmp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0,
                                HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO,
                                HSD_TE_X, in[i] == TOBJ_TEV_CA_TEX0_A ?
                                          reg0_a : reg1_a);
                sel[i] = HSD_TE_A;
                exp[i] = tmp;
                break;
            default: HSD_ASSERT(0x4F0, 0); break;
            }
        }
        HSD_TExpAlphaOp(e0, tev->alpha_op, tev->alpha_bias, tev->alpha_scale,
                        tev->alpha_clamp);
        HSD_TExpAlphaIn(e0, sel[0], exp[0], sel[1], exp[1], sel[2], exp[2],
                        sel[3], exp[3]);
        *a = e0;
    }
}

/* ========================================================================= */
/*  0x801BE85C | 0x60C  fn_801BE85C -- TObjUpdateFunc                        */
/* ========================================================================= */

/**
 * Colosseum clamps every animated TObj scalar into [0, 1] before it is used.
 * The two limits and the 255.0f scale are the .sdata2 constants at
 * 0x8047DEE0, 0x8047DEE4 and 0x8047DF18.
 */
static inline f32 tobj_clamp01(f32 x)
{
    if (x <= 0.0F) {
        x = 0.0F;
    } else if (x >= 1.0F) {
        x = 1.0F;
    }
    return x;
}

static inline u8 tobj_color8(f32 x)
{
    return (u8) (255.0F * tobj_clamp01(x));
}

void fn_801BE85C(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_TObj* tobj = obj;

    if (tobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_T_TIMG: {
        int n;
        if (tobj->imagetbl == NULL) {
            __assert(HSD_TOBJ_FILE, 0x116, lbl_802756E8);
        }
        n = (int) val->fv;
        if (tobj->imagetbl[n] != NULL) {
            tobj->imagedesc = tobj->imagetbl[n];
        }
    } break;
    case HSD_A_T_TCLT:
        if (tobj->tluttbl != NULL) {
            tobj->tlut_no = (u8) val->fv;
        }
        break;
    case HSD_A_T_BLEND:
        tobj->blending = tobj_clamp01(val->fv);
        break;
    case HSD_A_T_ROTX:
        tobj->rotate.x = val->fv;
        goto mtxdirty;
    case HSD_A_T_ROTY:
        tobj->rotate.y = val->fv;
        goto mtxdirty;
    case HSD_A_T_ROTZ:
        tobj->rotate.z = val->fv;
        goto mtxdirty;
    case HSD_A_T_TRAU:
        tobj->translate.x = val->fv;
        goto mtxdirty;
    case HSD_A_T_TRAV:
        tobj->translate.y = val->fv;
        goto mtxdirty;
    case HSD_A_T_SCAU:
        tobj->scale.x = val->fv;
        goto mtxdirty;
    case HSD_A_T_SCAV:
        tobj->scale.y = val->fv;
        goto mtxdirty;
    mtxdirty:
        tobj->flags |= TEX_MTX_DIRTY;
        break;
    case HSD_A_T_LOD_BIAS:
        if (tobj->lod != NULL) {
            tobj->lod->LODBias = val->fv;
        }
        break;
    case HSD_A_T_KONST_R:
        if (tobj->tev != NULL) {
            tobj->tev->konst.r = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_KONST_G:
        if (tobj->tev != NULL) {
            tobj->tev->konst.g = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_KONST_B:
        if (tobj->tev != NULL) {
            tobj->tev->konst.b = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_KONST_A:
        if (tobj->tev != NULL) {
            tobj->tev->konst.a = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV0_R:
        if (tobj->tev != NULL) {
            tobj->tev->tev0.r = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV0_G:
        if (tobj->tev != NULL) {
            tobj->tev->tev0.g = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV0_B:
        if (tobj->tev != NULL) {
            tobj->tev->tev0.b = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV0_A:
        if (tobj->tev != NULL) {
            tobj->tev->tev0.a = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV1_R:
        if (tobj->tev != NULL) {
            tobj->tev->tev1.r = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV1_G:
        if (tobj->tev != NULL) {
            tobj->tev->tev1.g = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV1_B:
        if (tobj->tev != NULL) {
            tobj->tev->tev1.b = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TEV1_A:
        if (tobj->tev != NULL) {
            tobj->tev->tev1.a = tobj_color8(val->fv);
        }
        break;
    case HSD_A_T_TS_BLEND:
        tobj->blending = tobj_clamp01(val->fv);
        break;
    }
}

/* ========================================================================= */
/*  0x801BEE68 | 0x74  fn_801BEE68 -- HSD_TObjReqAnimAllByFlags              */
/* ========================================================================= */

static inline void HSD_TObjReqAnimByFlags(HSD_TObj* tobj, f32 startframe,
                                          u32 flags)
{
    if (tobj != NULL) {
        if (flags & TOBJ_ANIM) {
            HSD_AObjReqAnim(tobj->aobj, startframe);
        }
    }
}

void fn_801BEE68(HSD_TObj* tobj, f32 startframe, u32 flags)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjReqAnimByFlags(tp, startframe, flags);
        }
    }
}

/* ========================================================================= */
/*  0x801BEEDC | 0x1BC  fn_801BEEDC -- HSD_TObjAddAnimAll                    */
/* ========================================================================= */

static inline HSD_TexAnim* lookupTextureAnim(s32 id, HSD_TexAnim* texanim)
{
    HSD_TexAnim* ta;

    for (ta = texanim; ta != NULL; ta = ta->next) {
        if ((s32) ta->id == id) {
            return ta;
        }
    }
    return NULL;
}

static inline void HSD_TObjAddAnim(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    s32 i;
    HSD_TexAnim* ta;

    if (tobj != NULL) {
        if ((ta = lookupTextureAnim(tobj->anim_id, texanim)) != NULL) {
            if (tobj->aobj != NULL) {
                HSD_AObjRemove(tobj->aobj);
            }
            tobj->aobj = HSD_AObjLoadDesc(ta->aobjdesc);
            tobj->imagetbl = ta->imagetbl;

            if (tobj->tluttbl != NULL) {
                for (i = 0; tobj->tluttbl[i] != NULL; i++) {
                    HSD_TlutRemove(tobj->tluttbl[i]);
                }
                fn_801A6960(tobj->tluttbl);
            }

            if (ta->n_tluttbl != 0) {
                tobj->tluttbl = (HSD_Tlut**) fn_801A6928(
                    (s32) sizeof(HSD_Tlut*) * (ta->n_tluttbl + 1));
                for (i = 0; i < ta->n_tluttbl; i++) {
                    tobj->tluttbl[i] = HSD_TlutLoadDesc(ta->tluttbl[i]);
                }
                tobj->tluttbl[i] = NULL;
            } else {
                tobj->tluttbl = NULL;
            }
            tobj->tlut_no = (u8) -1;
        }
    }
}

void fn_801BEEDC(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjAddAnim(tp, texanim);
        }
    }
}
