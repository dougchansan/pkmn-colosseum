/**
 * @file displayfunc.c
 * @brief HAL sysdolphin displayfunc.c: JObj display, billboards, the
 *        translucency z-sort list and screen erase, 0x80196EB4-0x80198F7C.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/displayfunc.c) and checked against Colosseum's
 * retail code, the newer sysdolphin. The library is built with deferred
 * inlining, so functions are listed in HAL's order and MWCC emits them in
 * reverse (the retail address order). Functions nothing in the game
 * references are compiled and dead-stripped by the linker as in retail.
 *
 * Colosseum's billboard builders normalise with an FLT_EPSILON guard and
 * fall back to copying the matrix when the basis degenerates.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names.
 */
#include "crt/float.h"
#include "crt/math_ppc.h"
#include "dolphin/gx/GX.h"
#include "dolphin/gx/GXVert.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_objalloc.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/mtx.h"
#include "sysdolphin/baselib/object.h"

#define RENDER_SHADOW (1 << 26)

#define DOBJ_HIDDEN 1
#define DOBJ_TRSP_SHIFT 1

typedef struct _GXTexObj {
    u32 dummy[8];
} GXTexObj;

struct HSD_DObj {
    HSD_Class parent;
    HSD_DObj* next;
    HSD_MObj* mobj;
    HSD_PObj* pobj;
    HSD_AObj* aobj;
    u32 flags;
};

struct HSD_DObjInfo {
    HSD_ClassInfo parent;
    void (*disp)(HSD_DObj* dobj, MtxPtr vmtx, MtxPtr pmtx, u32 rendermode);
};

#define HSD_DOBJ_METHOD(o) ((HSD_DObjInfo*) HSD_CLASS_METHOD(o))

void* memset(void* dst, int c, u32 n);
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
void PSMTXCopy(const Mtx src, Mtx dst);
u32 PSMTXInverse(const Mtx src, Mtx inv);
void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
void PSMTXRotRad(Mtx m, char axis, f32 rad);
void PSVECScale(const Vec3* src, Vec3* dst, f32 scale);
void PSVECNormalize(const Vec3* src, Vec3* unit);
f32 PSVECMag(const Vec3* v);
void PSVECCrossProduct(const Vec3* a, const Vec3* b, Vec3* axb);

/* GXInitTexObj */
void GXInitTexObj(GXTexObj* obj, void* image, u16 width, u16 height,
                 int format, int wrap_s, int wrap_t, u8 mipmap);
void GXLoadTexObj(GXTexObj* obj, int id);
/* GXSetNumTexGens */
void GXSetNumTexGens(u8 nTexGens);
/* GXSetTexCoordGen2 */
void GXSetTexCoordGen2(int dst, int func, int src, u32 mtx, u8 normalize,
                 u32 postmtx);
/* GXSetNumTevStages */
void GXSetNumTevStages(u8 nStages);
/* GXSetTevOrder */
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevOp(int id, int mode);
/* GXSetZTexture */
void GXSetZTexture(int op, int fmt, u32 bias);
/* GXSetCullMode */
void GXSetCullMode(int mode);
/* GXSetAlphaCompare */
void GXSetAlphaCompare(int comp0, u8 ref0, int op, int comp1, u8 ref1);
/* GXSetZCompLoc */
void GXSetZCompLoc(u8 before_tex);
void GXSetZMode(u8 compare_enable, int func, u8 update_enable);
void GXSetBlendMode(int type, int src_factor, int dst_factor, int op);
/* GXSetColorUpdate */
void GXSetColorUpdate(u8 update_enable);
/* GXSetAlphaUpdate */
void GXSetAlphaUpdate(u8 update_enable);
/* GXSetNumChans */
void GXSetNumChans(u8 nChans);
/* GXSetChanCtrl */
void GXSetChanCtrl(int chan, u8 enable, int amb_src, int mat_src, u32 light_mask,
                 int diff_fn, int attn_fn);
/* GXClearVtxDesc */
void GXClearVtxDesc(void);
/* GXSetVtxAttrFmt */
void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, u8 frac);
/* GXSetVtxDesc */
void GXSetVtxDesc(int attr, int type);
/* GXBegin */
void GXBegin(int type, int vtxfmt, u16 nverts);
/* GXSetCurrentMtx */
void GXSetCurrentMtx(u32 id);
void GXLoadPosMtxImm(Mtx mtx, u32 id);

/* HSD_identityMtx */
extern Mtx lbl_8036CBC0;
/* HSD_StateInvalidate */
void HSD_StateInvalidate(int state);
HSD_CObj* HSD_CObjGetCurrent(void);
/* HSD_JObjSetCurrent */
void HSD_JObjSetCurrent(HSD_JObj* jobj);
void HSD_DObjSetCurrent(HSD_DObj* dobj);
/* HSD_LObjSetupSpecularInit */
void HSD_LObjSetupSpecularInit(MtxPtr pmtx);
/* HSD_PObjClearMtxMark */
void HSD_PObjClearMtxMark(void* obj, u32 mark);
f32* HSD_MtxAlloc(void);
void HSD_MtxFree(f32* mtx);
/* HSD_MtxInverseConcat */
void fn_801A9DF0(Mtx inv, Mtx src, Mtx dest);
void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
void* HSD_ObjAlloc(HSD_ObjAllocData* data);
void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);

static inline MtxPtr HSD_CObjGetViewingMtxPtrDirect(HSD_CObj* cobj)
{
    return (MtxPtr) ((u8*) cobj + 0x54);
}

typedef struct _HSD_ZList {
    Mtx pmtx;
    MtxPtr vmtx;
    HSD_JObj* jobj;
    u32 rendermode;

    struct {
        struct _HSD_ZList* texedge;
        struct _HSD_ZList* xlu;
    } sort;

    struct _HSD_ZList* next;
} HSD_ZList;

/* zlist_alloc_data */
HSD_ObjAllocData lbl_80465348;

static void (*sptcl_callback)(s32, s32, s32, HSD_JObj*) = NULL;

static GXColor erase_color = { 0, 0, 0, 0 };

static int zsort_listing = 0;
static int zsort_sorting = 0;

static HSD_ZList* zlist_top = NULL;
static HSD_ZList** zlist_bottom = &zlist_top;

static HSD_ZList* zlist_texedge_top = NULL;
static HSD_ZList** zlist_texedge_bottom = &zlist_texedge_top;
static int zlist_texedge_nb = 0;

static HSD_ZList* zlist_xlu_top = NULL;
static HSD_ZList** zlist_xlu_bottom = &zlist_xlu_top;
static int zlist_xlu_nb = 0;

#define ZLIST_NEXT(list, offset) (*(HSD_ZList**) (((u8*) (list)) + (offset)))

void HSD_ZListInitAllocData(void)
{
    HSD_ObjAllocInit(&lbl_80465348, sizeof(HSD_ZList), 4);
}

static inline HSD_ZList* HSD_ZListAlloc(void)
{
    HSD_ZList* list;

    list = HSD_ObjAlloc(&lbl_80465348);
    memset(&list->vmtx, 0, sizeof(HSD_ZList) - sizeof(Mtx));

    return list;
}

static inline void HSD_ZListFree(HSD_ZList* ptr)
{
    HSD_ObjFree(&lbl_80465348, ptr);
}

static void mkVBillBoardMtx(HSD_JObj* jobj, MtxPtr src, MtxPtr dst)
{
    Vec3 pos, ax, ay, az, uy;
    f32 sx, sz, mag;

    HSD_MtxColVec(src, 3, &pos);
    HSD_MtxColVec(src, 1, &ay);
    PSVECScale(&ay, &uy, 1.0F / (FLT_EPSILON + PSVECMag(&ay)));

    sx = HSD_MtxColMag(src, 0);
    sz = HSD_MtxColMag(src, 2);

    if (jobj->flags & JOBJ_PBILLBOARD) {
        PSVECScale(&pos, &az, -1.0F / (FLT_EPSILON + PSVECMag(&pos)));
        PSVECCrossProduct(&uy, &az, &ax);
    } else {
        PSVECCrossProduct(&uy, &(Vec3){ 0.0F, 0.0F, 1.0F }, &ax);
    }
    mag = PSVECMag(&ax);
    if (!(mag < FLT_EPSILON)) {
        sx /= mag;
        PSVECCrossProduct(&ax, &uy, &az);
        sz /= FLT_EPSILON + PSVECMag(&az);

        dst[0][0] = sx * ax.x;
        dst[1][0] = sx * ax.y;
        dst[2][0] = sx * ax.z;

        HSD_MtxSetColVec(dst, 1, &ay);

        dst[0][2] = sz * az.x;
        dst[1][2] = sz * az.y;
        dst[2][2] = sz * az.z;

        HSD_MtxSetColVec(dst, 3, &pos);
    } else {
        PSMTXCopy(src, dst);
    }
}

static void mkHBillBoardMtx(HSD_JObj* jobj, MtxPtr src, MtxPtr dst)
{
    Vec3 pos, ax, ay, az, ux;
    f32 sy, sz, mag;

    HSD_MtxColVec(src, 3, &pos);
    HSD_MtxColVec(src, 0, &ax);
    PSVECScale(&ax, &ux, 1.0F / (FLT_EPSILON + PSVECMag(&ax)));

    sy = HSD_MtxColMag(src, 1);
    sz = HSD_MtxColMag(src, 2);

    if (jobj->flags & JOBJ_PBILLBOARD) {
        ay.y = FLT_EPSILON + sqrtf(pos.x * pos.x + pos.z * pos.z);
        ay.x = pos.x * (-pos.y / ay.y);
        ay.z = pos.z * (-pos.y / ay.y);
        PSVECNormalize(&ay, &ay);
    } else {
        ay.x = 0.0F;
        ay.y = 1.0F;
        ay.z = 0.0F;
    }
    PSVECCrossProduct(&ux, &ay, &az);
    mag = PSVECMag(&az);
    if (!(mag < FLT_EPSILON)) {
        sz /= mag;
        PSVECCrossProduct(&az, &ux, &ay);
        sy /= FLT_EPSILON + PSVECMag(&ay);

        HSD_MtxSetColVec(dst, 0, &ax);

        dst[0][1] = sy * ay.x;
        dst[1][1] = sy * ay.y;
        dst[2][1] = sy * ay.z;

        dst[0][2] = sz * az.x;
        dst[1][2] = sz * az.y;
        dst[2][2] = sz * az.z;

        HSD_MtxSetColVec(dst, 3, &pos);
    } else {
        PSMTXCopy(src, dst);
    }
}

static void mkBillBoardMtx(HSD_JObj* jobj, MtxPtr src, MtxPtr dst)
{
    Vec3 ax, ay, az, pos;
    f32 sx, sy, sz, mag;

    sx = HSD_MtxColMag(src, 0);
    sz = HSD_MtxColMag(src, 2);

    HSD_MtxColVec(src, 1, &ay);
    sy = PSVECMag(&ay);
    HSD_MtxColVec(src, 3, &pos);

    if (jobj->flags & JOBJ_PBILLBOARD) {
        PSVECScale(&pos, &az, -1.0F / (FLT_EPSILON + PSVECMag(&pos)));
    } else {
        az.x = 0.0F;
        az.y = 0.0F;
        az.z = 1.0F;
    }
    PSVECScale(&ay, &ay, 1.0F / (sy + FLT_EPSILON));
    PSVECCrossProduct(&ay, &az, &ax);
    mag = PSVECMag(&ax);
    if (mag >= FLT_EPSILON) {
        sx /= mag;
        PSVECCrossProduct(&az, &ax, &ay);
        sy /= FLT_EPSILON + PSVECMag(&ay);
    } else {
        ay.y = FLT_EPSILON + sqrtf(az.x * az.x + az.z * az.z);
        ay.x = az.x * (-az.y / ay.y);
        ay.z = az.z * (-az.y / ay.y);
        PSVECCrossProduct(&ay, &az, &ax);
        sx /= FLT_EPSILON + PSVECMag(&ax);
    }

    dst[0][0] = sx * ax.x;
    dst[1][0] = sx * ax.y;
    dst[2][0] = sx * ax.z;

    dst[0][1] = sy * ay.x;
    dst[1][1] = sy * ay.y;
    dst[2][1] = sy * ay.z;

    dst[0][2] = sz * az.x;
    dst[1][2] = sz * az.y;
    dst[2][2] = sz * az.z;

    HSD_MtxSetColVec(dst, 3, &pos);
}

/* mkRBillBoardMtx */
void fn_80197C70(HSD_JObj* jobj, MtxPtr src, MtxPtr dst)
{
    Mtx rot, scl;
    f32 sx, sy, sz;

    sx = HSD_MtxColMag(src, 0);
    sy = HSD_MtxColMag(src, 1);
    sz = HSD_MtxColMag(src, 2);
    PSMTXScale(scl, sx, sy, sz);
    PSMTXRotRad(rot, 'z', jobj->rotate.z);
    rot[0][3] = src[0][3];
    rot[1][3] = src[1][3];
    rot[2][3] = src[2][3];
    PSMTXConcat(rot, scl, dst);
}

void HSD_JObjMakePositionMtx(HSD_JObj* jobj, MtxPtr vmtx, MtxPtr pmtx)
{
    Mtx mtx;

    if (jobj->flags & JOBJ_BILLBOARD_FIELD) {
        PSMTXConcat(vmtx, jobj->mtx, mtx);
        switch (jobj->flags & JOBJ_BILLBOARD_FIELD) {
        case JOBJ_BILLBOARD:
            mkBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_VBILLBOARD:
            mkVBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_HBILLBOARD:
            mkHBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_RBILLBOARD:
            fn_80197C70(jobj, mtx, pmtx);
            break;
        default:
            HSD_Panic(__FILE__, 368, "unkown type of billboard.\n");
        }
    } else {
        PSMTXConcat(vmtx, jobj->mtx, pmtx);
    }
}

HSD_JObj* HSD_JObjFindSkeleton(HSD_JObj* jobj)
{
    HSD_ASSERT(388, jobj);
    for (; jobj; jobj = jobj->parent) {
        if (jobj->flags & (JOBJ_SKELETON | JOBJ_SKELETON_ROOT)) {
            return jobj;
        }
    }
    return NULL;
}

MtxPtr _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* m, MtxPtr mtx)
{
    if (m->flags & JOBJ_SKELETON_ROOT) {
        return NULL;
    } else {
        HSD_JObj* x = HSD_JObjFindSkeleton(m);
        HSD_ASSERT(468, x);

        if (x == m) {
            PSMTXInverse(x->envelopemtx, mtx);
        } else if (x->flags & JOBJ_SKELETON_ROOT) {
            fn_801A9DF0(x->mtx, m->mtx, mtx);
        } else {
            Mtx n;
            PSMTXConcat(x->mtx, x->envelopemtx, n);
            fn_801A9DF0(n, m->mtx, mtx);
        }

        return mtx;
    }
}

/* HSD_JObjDispSub */
void HSD_JObjDispDObj(HSD_JObj* jobj, MtxPtr vmtx, MtxPtr pmtx,
                 HSD_TrspMask trsp_mask, u32 rendermode)
{
    HSD_DObj* dobj;
    u32 dobj_trsp;

    HSD_JObjSetCurrent(jobj);

    dobj_trsp = trsp_mask << DOBJ_TRSP_SHIFT;

    if (!(rendermode & RENDER_SHADOW)) {
        if (jobj->flags & JOBJ_SPECULAR) {
            HSD_LObjSetupSpecularInit(pmtx);
        }
    }

    HSD_PObjClearMtxMark(NULL, 0);
    for (dobj = jobj->u.dobj; dobj; dobj = dobj->next) {
        if (dobj->flags & DOBJ_HIDDEN) {
            continue;
        }

        if (dobj->flags & dobj_trsp) {
            HSD_DObjSetCurrent(dobj);
            HSD_DOBJ_METHOD(dobj)->disp(dobj, vmtx, pmtx, rendermode);
        }
    }
    HSD_DObjSetCurrent(NULL);
    HSD_JObjSetCurrent(NULL);
}

/* HSD_JObjDispDObj */
void fn_80197784(HSD_JObj* jobj, MtxPtr vmtx, HSD_TrspMask trsp_mask,
                 u32 rendermode)
{
    HSD_CObj* cobj;
    Mtx mtx;

    if ((jobj->flags & JOBJ_HIDDEN) == 0) {
        u32 xlu_bits = jobj->flags & (trsp_mask << JOBJ_TRSP_SHIFT);
        if (xlu_bits != 0) {
            HSD_JObjSetupMatrix(jobj);

            if (vmtx == NULL) {
                cobj = HSD_CObjGetCurrent();
                vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);
            }

            HSD_JOBJ_METHOD(jobj)->make_pmtx(jobj, vmtx, mtx);
            if ((xlu_bits & JOBJ_OPA) != 0) {
                HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx, HSD_TRSP_OPA,
                                            rendermode);
            }
            if (zsort_listing == 0) {
                if ((xlu_bits & JOBJ_TEXEDGE) != 0) {
                    HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx,
                                                HSD_TRSP_TEXEDGE, rendermode);
                }
                if ((xlu_bits & JOBJ_XLU) != 0) {
                    HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx, HSD_TRSP_XLU,
                                                rendermode);
                }
            } else {
                if ((xlu_bits & (JOBJ_TEXEDGE | JOBJ_XLU)) != 0) {
                    HSD_ZList* zlist;

                    zlist = HSD_ZListAlloc();
                    PSMTXCopy(mtx, zlist->pmtx);
                    if (vmtx != NULL) {
                        zlist->vmtx = (MtxPtr) HSD_MtxAlloc();
                        PSMTXCopy(vmtx, zlist->vmtx);
                    }
                    zlist->jobj = jobj;
                    zlist->rendermode = rendermode;
                    *zlist_bottom = zlist;
                    zlist_bottom = &zlist->next;
                    if ((xlu_bits & JOBJ_TEXEDGE) != 0) {
                        *zlist_texedge_bottom = zlist;
                        zlist_texedge_bottom = &zlist->sort.texedge;
                        zlist_texedge_nb += 1;
                    }
                    if ((xlu_bits & JOBJ_XLU) != 0) {
                        *zlist_xlu_bottom = zlist;
                        zlist_xlu_bottom = &zlist->sort.xlu;
                        zlist_xlu_nb += 1;
                    }
                }
            }
        }
    }
}

static HSD_ZList* zlist_sort(HSD_ZList* list, s32 nb, s32 offset)
{
    HSD_ZList *fore, *hind, **ptr;
    int nb_fore, nb_hind;
    int i;

    if (nb <= 1) {
        if (list != NULL) {
            ZLIST_NEXT(list, offset) = NULL;
        }
        return list;
    }

    nb_fore = nb / 2;
    nb_hind = nb - nb_fore;

    hind = list;
    for (i = 0; i < nb_fore; i++) {
        hind = ZLIST_NEXT(hind, offset);
    }

    fore = zlist_sort(list, nb_fore, offset);
    hind = zlist_sort(hind, nb_hind, offset);

    list = NULL;
    ptr = &list;

    while (fore != NULL && hind != NULL) {
        if (fore->pmtx[2][3] <= hind->pmtx[2][3]) {
            *ptr = fore;
            fore = ZLIST_NEXT(fore, offset);
        } else {
            *ptr = hind;
            hind = ZLIST_NEXT(hind, offset);
        }
        ptr = &ZLIST_NEXT(*ptr, offset);
    }

    if (fore != NULL) {
        *ptr = fore;
    } else if (hind != NULL) {
        *ptr = hind;
    }

    return list;
}

/* _HSD_ZListSort */
void _HSD_ZListSort(void)
{
    if (zsort_sorting) {
        zlist_texedge_top =
            zlist_sort(zlist_texedge_top, zlist_texedge_nb, 0x3C);
        zlist_xlu_top = zlist_sort(zlist_xlu_top, zlist_xlu_nb, 0x40);
    }
}

void _HSD_ZListClear(void);

/* _HSD_ZListDisp */
void _HSD_ZListDisp(void)
{
    HSD_ZList* list;
    MtxPtr vmtx;
    HSD_CObj* cobj;

    cobj = HSD_CObjGetCurrent();
    vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);

    list = zlist_texedge_top;
    while (list != NULL) {
        HSD_JOBJ_METHOD(list->jobj)
            ->disp(list->jobj, (list->vmtx) ? list->vmtx : vmtx, list->pmtx,
                   HSD_TRSP_TEXEDGE, list->rendermode);
        list = list->sort.texedge;
    }

    list = zlist_xlu_top;
    while (list != NULL) {
        HSD_JOBJ_METHOD(list->jobj)
            ->disp(list->jobj, (list->vmtx) ? list->vmtx : vmtx, list->pmtx,
                   HSD_TRSP_XLU, list->rendermode);
        list = list->sort.xlu;
    }

    _HSD_ZListClear();
}

/* _HSD_ZListClear */
void _HSD_ZListClear(void)
{
    HSD_ZList* list = zlist_top;

    while (list != NULL) {
        HSD_ZList* next = list->next;
        if (list->vmtx) {
            HSD_MtxFree((f32*) list->vmtx);
        }
        HSD_ZListFree(list);
        list = next;
    }
    zlist_top = NULL;
    zlist_bottom = &zlist_top;

    zlist_texedge_top = NULL;
    zlist_texedge_bottom = &zlist_texedge_top;
    zlist_texedge_nb = 0;

    zlist_xlu_top = NULL;
    zlist_xlu_bottom = &zlist_xlu_top;
    zlist_xlu_nb = 0;
}

/* HSD_JObjDisp */
void HSD_JObjDisp(HSD_JObj* jobj, MtxPtr vmtx, HSD_TrspMask trsp_mask,
                 u32 rendermode)
{
    if (jobj != NULL) {
        if (union_type_dobj(jobj)) {
            fn_80197784(jobj, vmtx, trsp_mask, rendermode);
        } else if (union_type_ptcl(jobj) && sptcl_callback != NULL) {
            HSD_SList* sp;
            for (sp = jobj->u.ptcl; sp != NULL; sp = sp->next) {
                if ((((u32) sp->data) & 0x80000000) != 0) {
                    u32 offset = (((u32) sp->data) >> JOBJ_PTCL_OFFSET_SHIFT) &
                                 JOBJ_PTCL_OFFSET_MASK;
                    u32 bank = (u32) sp->data;
                    bank &= JOBJ_PTCL_BANK_MASK;
                    (*sptcl_callback)(0, bank, offset, jobj);
                }
                sp->data = (void*) ((u32) sp->data & JOBJ_PTCL_ACTIVE);
            }
        }
    }
}

/* HSD_JObjSetSPtclCallback */
void HSD_JObjSetSPtclCallback(void (*func)(s32, s32, s32, HSD_JObj*))
{
    sptcl_callback = func;
}

void HSD_SetEraseColor(u8 r, u8 g, u8 b, u8 a)
{
    erase_color.r = r;
    erase_color.g = g;
    erase_color.b = b;
    erase_color.a = a;
}

void HSD_EraseRect(f32 top, f32 bottom, f32 left, f32 right, f32 z,
                   int enable_color, int enable_alpha, int enable_depth)
{
    GXTexObj texobj;
    static u8 depth_image[128] __attribute__((aligned(32))) = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    };
    GXColor color;
    GXColor tmp;

    if (!enable_color && !enable_alpha && !enable_depth) {
        return;
    }

    if (enable_depth) {
        GXInitTexObj(&texobj, depth_image, 4, 4, 0x11, 1, 1, 0);
        GXLoadTexObj(&texobj, 0);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 4);
        GXSetZTexture(2, 0x11, 0);
    } else {
        GXSetNumTexGens(0);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOp(0, 4);
    }

    GXSetCullMode(0);

    GXSetAlphaCompare(7, 0, 1, 7, 0);
    GXSetZCompLoc(1);
    GXSetZMode(1, 7, enable_depth ? 1 : 0);
    GXSetBlendMode(2, 1, 0, 3);
    GXSetColorUpdate(enable_color ? 1 : 0);
    GXSetAlphaUpdate(enable_alpha ? 1 : 0);

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);

    GXClearVtxDesc();
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 0, 0);
    GXLoadPosMtxImm(lbl_8036CBC0, 0);
    GXSetCurrentMtx(0);
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);

    tmp = erase_color;
    color = tmp;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(left, top, z);
    GXColor4u8(color.r, color.g, color.b, color.a);
    GXTexCoord2u8(0, 0);
    GXPosition3f32(right, top, z);
    GXColor4u8(color.r, color.g, color.b, color.a);
    GXTexCoord2u8(1, 0);
    GXPosition3f32(right, bottom, z);
    GXColor4u8(color.r, color.g, color.b, color.a);
    GXTexCoord2u8(1, 1);
    GXPosition3f32(left, bottom, z);
    GXColor4u8(color.r, color.g, color.b, color.a);
    GXTexCoord2u8(0, 1);
    GXEnd();

    GXSetZTexture(0, 0x11, 0);

    HSD_StateInvalidate(-1);
}

void _HSD_DispForgetMemory(void* lo, void* hi)
{
    zlist_top = NULL;
    zlist_bottom = &zlist_top;

    zlist_texedge_top = NULL;
    zlist_texedge_bottom = &zlist_texedge_top;
    zlist_texedge_nb = 0;

    zlist_xlu_top = NULL;
    zlist_xlu_bottom = &zlist_xlu_top;
    zlist_xlu_nb = 0;
}
