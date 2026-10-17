/**
 * @file pobj.c
 * @brief HAL sysdolphin pobj.c: polygon objects, 0x801AA608-0x801AD7CC.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/pobj.c) and checked against Colosseum's retail
 * code, the newer sysdolphin. The library is built with deferred inlining,
 * so functions are listed in HAL's order and MWCC emits them in reverse
 * (the retail address order). Functions nothing in the game references are
 * compiled and dead-stripped by the linker as in retail.
 *
 * Colosseum keeps a shape animation's AObj on the PObj itself (and drives it
 * through the class's update method), hides PObjs flagged 0x800, counts
 * vertices and triangles (fn_801ACDAC), and invalidates the vertex cache
 * after loading.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names.
 */
#include "dolphin/gx/GXVert.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_pobj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/object.h"

typedef enum PObjSetupFlag {
    SETUP_NORMAL = 1,
    SETUP_REFLECTION = 2,
    SETUP_HIGHLIGHT = 4,
    SETUP_NORMAL_PROJECTION = 6,
    SETUP_JOINT0 = 1,
    SETUP_JOINT1 = 2,
    SETUP_NONE = 0
} PObjSetupFlag;

#define POBJ_HIDDEN (1 << 11)

#define GX_VA_PNMTXIDX 0
#define GX_VA_TEX0MTXIDX 1
#define GX_VA_TEX1MTXIDX 2
#define GX_VA_TEX2MTXIDX 3
#define GX_VA_TEX3MTXIDX 4
#define GX_VA_TEX4MTXIDX 5
#define GX_VA_TEX5MTXIDX 6
#define GX_VA_TEX6MTXIDX 7
#define GX_VA_TEX7MTXIDX 8
#define GX_VA_POS 9
#define GX_VA_NRM 10
#define GX_VA_CLR0 11
#define GX_VA_CLR1 12
#define GX_VA_TEX0 13
#define GX_VA_TEX1 14
#define GX_VA_TEX2 15
#define GX_VA_TEX3 16
#define GX_VA_TEX4 17
#define GX_VA_TEX5 18
#define GX_VA_TEX6 19
#define GX_VA_TEX7 20
#define GX_VA_NBT 25
#define GX_VA_NULL 0xFF

#define GX_NONE 0
#define GX_DIRECT 1
#define GX_INDEX8 2
#define GX_INDEX16 3

#define GX_POS_XY 0
#define GX_POS_XYZ 1
#define GX_NRM_XYZ 0
#define GX_CLR_RGB 0
#define GX_TEX_S 0

#define GX_U8 0
#define GX_S8 1
#define GX_U16 2
#define GX_S16 3
#define GX_F32 4

#define GX_RGB565 0
#define GX_RGB8 1
#define GX_RGBX8 2
#define GX_RGBA4 3
#define GX_RGBA6 4
#define GX_RGBA8 5

#define GX_QUADS 0x80
#define GX_TRIANGLES 0x90
#define GX_TRIANGLESTRIP 0x98
#define GX_TRIANGLEFAN 0xA0
#define GX_OPCODE_MASK 0xF8
#define GX_VAT_MASK 0x07

#define GX_PNMTX0 0
#define GX_PNMTX1 3
#define GX_TEXMTX0 30
#define GX_TEXMTX1 33
#define GX_MTX3x4 0
#define GX_VTXFMT0 0
#define GX_CULL_NONE 0
#define GX_CULL_FRONT 1
#define GX_CULL_BACK 2

#define TEX_COORD_REFLECTION 1
#define TEX_COORD_HILIGHT 2
#define TEX_COORD_UNK5 5

void OSReport(const char* msg, ...);
void* memset(void* dst, int c, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
void PSMTXCopy(const Mtx src, Mtx dst);
u32 PSMTXInvXpose(const Mtx src, Mtx dst);

/* GXClearVtxDesc */
void GXClearVtxDesc(void);
/* GXSetVtxDesc */
void GXSetVtxDesc(int attr, int type);
/* GXSetVtxAttrFmt */
void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, u8 frac);
/* GXSetArray */
void GXSetArray(int attr, void* base_ptr, u8 stride);
/* GXBegin */
void GXBegin(int type, int vtxfmt, u16 nverts);
/* GXSetCurrentMtx */
void GXSetCurrentMtx(u32 id);
void GXCallDisplayList(void* list, u32 nbytes);
void GXLoadPosMtxImm(Mtx mtx, u32 id);
void GXLoadNrmMtxImm(Mtx mtx, u32 id);
void GXLoadTexMtxImm(Mtx mtx, u32 id, int type);

/* hsdClass */
extern HSD_ClassInfo lbl_8036C638;
/* hsdSearchClassInfo */
HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdNew */
void* fn_80193828(HSD_ClassInfo* info);
/* hsdAllocMemPiece */
void* fn_80193B10(u32 size);
/* hsdFreeMemPiece */
void fn_80193AF0(void* mem, u32 size);
/* HSD_MemAlloc */
void* HSD_MemAlloc(u32 size);
/* HSD_Free */
void HSD_Free(void* ptr);
/* HSD_IsMemoryReleased: nonzero when the pointer lies in freed memory. */
int fn_801A6990(void* ptr);
/* HSD_SListAlloc */
HSD_SList* HSD_SListAlloc(void);
/* HSD_SListRemove */
HSD_SList* HSD_SListRemove(HSD_SList* list);
/* _HSD_NeedCacheInvalidate */
void fn_8019C6EC(u32 flags);
/* HSD_StateSetCullMode */
void HSD_StateSetCullMode(int mode);
/* HSD_PerfCountEnvelopeBlending */
void HSD_PerfCountEnvelopeBlending(s32 n);
void* HSD_IDGetDataFromTable(void* table, u32 id, s32* success);
HSD_JObj* HSD_JObjGetCurrent(void);
HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj* from, u32 mapping);
MtxPtr _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* m, MtxPtr mtx);
s32 HSD_Index2PosNrmMtx(u32 index);
u32 HSD_Index2TexMtx(u32 index);
void HSD_MtxScaledAdd(Mtx src, Mtx dst, Mtx add, f32 scale);

/* HSD_PerfCurrentStat (perf.c) */
typedef struct HSD_PerfStat {
    f32 cpu_time;
    f32 draw_time;
    f32 total_time;
    u32 nb_mtx_load;
    u32 env_blend[32];
} HSD_PerfStat;
extern HSD_PerfStat lbl_8036CC40;

static inline void HSD_PerfCountMtxLoad(void)
{
    lbl_8036CC40.nb_mtx_load += 1;
}

static inline void* HSD_IDGetData(u32 id, s32* success)
{
    return HSD_IDGetDataFromTable(NULL, id, success);
}

/* mtx.h */
static inline void HSD_MtxInverseTranspose(Mtx src, Mtx dst)
{
    if (!PSMTXInvXpose(src, dst)) {
        PSMTXCopy(src, dst);
    }
}

static void PObjInfoInit(void);

HSD_PObjInfo hsdPObj = { PObjInfoInit };

/* .sbss is laid out in reverse declaration order. */
static HSD_VtxDescList* prev_vtxdesc;
static HSD_VtxDescList* prev_vtxdesclist_array;
static u32 normal_buffer_size;
static u32 vertex_buffer_size;
static f32 (*normal_buffer)[3];
static f32 (*vertex_buffer)[3];
static HSD_PObjInfo* default_class;

static struct {
    void* obj;
    u32 mark;
} mtx_mark[2];

static inline void HSD_PObjAddAnim(HSD_PObj* pobj, HSD_ShapeAnim* shapeanim)
{
    if (pobj == NULL) {
        return;
    }
    if (pobj->aobj != NULL) {
        HSD_AObjRemove(pobj->aobj);
    }
    pobj->aobj = HSD_AObjLoadDesc(shapeanim->aobjdesc);
}

/* HSD_PObjAddAnimAll */
void HSD_PObjAddAnimAll(HSD_PObj* pobj, HSD_ShapeAnim* shapeanim)
{
    HSD_PObj* po;
    HSD_ShapeAnim* sa;

    if (pobj == NULL || shapeanim == NULL) {
        return;
    }

    for (po = pobj, sa = shapeanim; po != NULL;
         po = po->next, sa = sa != NULL ? sa->next : NULL)
    {
        HSD_PObjAddAnim(po, sa);
    }
}

static inline void HSD_PObjReqAnimByFlags(HSD_PObj* pobj, f32 startframe,
                                          u32 flags)
{
    if (pobj == NULL) {
        return;
    }

    if (flags & POBJ_ANIM) {
        HSD_AObjReqAnim(pobj->aobj, startframe);
    }
}

void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, f32 startframe, u32 flags)
{
    HSD_PObj* pp;

    if (pobj == NULL) {
        return;
    }

    for (pp = pobj; pp != NULL; pp = pp->next) {
        HSD_PObjReqAnimByFlags(pp, startframe, flags);
    }
}

static inline void ShapeSetSetAnimResult(HSD_ShapeSet* shape_set, u32 type,
                                         HSD_ObjData* val)
{
    if (shape_set->flags & SHAPESET_ADDITIVE) {
        shape_set->blend.bp[type - HSD_A_S_W0] = val->fv;
    } else {
        shape_set->blend.bl = val->fv;
    }
}

static void PObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_PObj* pobj = HSD_POBJ(obj);

    if (pobj == NULL) {
        return;
    }

    if (pobj_type(pobj) == POBJ_SHAPEANIM) {
        ShapeSetSetAnimResult(pobj->u.shape_set, type, val);
    }
}

static inline void HSD_PObjAnim(HSD_PObj* pobj)
{
    if (pobj != NULL) {
        HSD_AObjInterpretAnim(pobj->aobj, pobj,
                              (HSD_ObjUpdateFunc) HSD_POBJ_METHOD(pobj)->update);
    }
}

void HSD_PObjAnimAll(HSD_PObj* pobj)
{
    HSD_PObj* pp;

    if (pobj != NULL) {
        for (pp = pobj; pp != NULL; pp = pp->next) {
            HSD_PObjAnim(pp);
        }
    }
}

static inline HSD_Envelope* HSD_EnvelopeAlloc(void)
{
    HSD_Envelope* envelope = fn_80193B10(sizeof(HSD_Envelope));
    HSD_ASSERT(425, envelope);
    memset(envelope, 0, sizeof(HSD_Envelope));
    return envelope;
}

static inline void HSD_EnvelopeFree(HSD_Envelope* env)
{
    if (env != NULL) {
        fn_80193AF0(env, sizeof(HSD_Envelope));
    }
}

static inline void HSD_EnvelopeListFree(HSD_SList* list)
{
    while (list) {
        HSD_Envelope* env = list->data;
        while (env) {
            HSD_Envelope* next = env->next;
            HSD_JObjUnrefThis(env->jobj);
            HSD_EnvelopeFree(env);
            env = next;
        }
        list = HSD_SListRemove(list);
    }
}

static inline HSD_SList* loadEnvelopeDesc(HSD_EnvelopeDesc** edesc_p)
{
    HSD_SList* list = NULL;
    HSD_SList** list_p = &list;

    if (edesc_p == NULL) {
        return NULL;
    }

    while (*edesc_p) {
        HSD_Envelope* envelope = NULL;
        HSD_Envelope** env_p = &envelope;
        HSD_EnvelopeDesc* edesc = *edesc_p;

        while (edesc->joint) {
            *env_p = HSD_EnvelopeAlloc();
            (*env_p)->weight = edesc->weight;
            env_p = &(*env_p)->next;
            edesc++;
        }

        (*list_p) = HSD_SListAlloc();
        (*list_p)->data = envelope;
        list_p = &(*list_p)->next;
        edesc_p++;
    }
    return list;
}

static inline void HSD_ShapeSetFree(HSD_ShapeSet* shape_set)
{
    if (!shape_set) {
        return;
    }
    fn_80193AF0(shape_set, sizeof(HSD_ShapeSet));
}

static inline void HSD_ShapeSetRemove(HSD_ShapeSet* shape_set)
{
    if (shape_set == NULL) {
        return;
    }

    if (shape_set->flags & SHAPESET_ADDITIVE) {
        HSD_Free(shape_set->blend.bp);
    }

    HSD_ShapeSetFree(shape_set);
}

static inline HSD_ShapeSet* loadShapeSetDesc(HSD_ShapeSetDesc* sdesc)
{
    HSD_ShapeSet* shape_set;
    int i;

    shape_set = fn_80193B10(sizeof(HSD_ShapeSet));
    HSD_ASSERT(482, shape_set);
    memset(shape_set, 0, sizeof(HSD_ShapeSet));
    shape_set->flags = sdesc->flags;
    shape_set->nb_shape = sdesc->nb_shape;
    shape_set->nb_vertex_index = sdesc->nb_vertex_index;
    shape_set->vertex_desc = sdesc->vertex_desc;
    shape_set->vertex_idx_list = sdesc->vertex_idx_list;
    shape_set->nb_normal_index = sdesc->nb_normal_index;
    shape_set->normal_desc = sdesc->normal_desc;
    shape_set->normal_idx_list = sdesc->normal_idx_list;
    if (shape_set->flags & SHAPESET_ADDITIVE) {
        shape_set->blend.bp = HSD_MemAlloc(shape_set->nb_shape * sizeof(f32));
        for (i = 0; i < shape_set->nb_shape; i++) {
            shape_set->blend.bp[i] = 0.0F;
        }
    } else {
        shape_set->blend.bl = 0.0F;
    }
    return shape_set;
}

HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* pobjdesc);

static s32 PObjLoad(HSD_Class* o, HSD_PObjDesc* desc)
{
    HSD_PObj* pobj = HSD_POBJ(o);

    pobj->next = HSD_PObjLoadDesc(desc->next);
    pobj->verts = desc->verts;
    pobj->flags = desc->flags;
    pobj->n_display = desc->n_display;
    pobj->display = desc->display;
    switch (pobj_type(pobj)) {
    case POBJ_SHAPEANIM:
        pobj->u.shape_set = loadShapeSetDesc(desc->u.shape_set);
        break;

    case POBJ_ENVELOPE:
        pobj->u.envelope_list = loadEnvelopeDesc(desc->u.envelope_p);
        break;

    case POBJ_SKIN:
        break;

    default:
        HSD_Panic(__FILE__, 554, "pobj: unexected type.\n");
    }

    fn_8019C6EC(1);

    return 0;
}

static inline HSD_PObj* HSD_PObjAlloc(void);

HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* pobjdesc)
{
    if (pobjdesc != NULL) {
        HSD_PObj* pobj;
        HSD_ClassInfo* info;

        if (!pobjdesc->class_name ||
            !(info = fn_80193748(pobjdesc->class_name)))
        {
            pobj = HSD_PObjAlloc();
        } else {
            pobj = fn_80193828(info);
            HSD_ASSERT(583, pobj);
        }
        HSD_POBJ_METHOD(pobj)->load(pobj, pobjdesc);
        return pobj;
    } else {
        return NULL;
    }
}

static inline void HSD_PObjRemove(HSD_PObj* pobj)
{
    hsdDelete(pobj);
}

void HSD_PObjRemoveAll(HSD_PObj* pobj)
{
    HSD_PObj* next;

    while (pobj != NULL) {
        next = pobj->next;
        HSD_PObjRemove(pobj);
        pobj = next;
    }
}

static inline HSD_PObjInfo* HSD_PObjGetDefaultClass(void)
{
    return default_class ? default_class : &hsdPObj;
}

void HSD_PObjSetDefaultClass(HSD_PObjInfo* info)
{
    if (info != NULL) {
        /* Dead-stripped in retail; the line number is not recoverable. */
        HSD_ASSERT(651, hsdIsDescendantOf(info, &hsdPObj));
    }
    default_class = info;
}

static inline HSD_PObj* HSD_PObjAlloc(void)
{
    HSD_PObj* pobj = fn_80193828(&HSD_PObjGetDefaultClass()->parent);
    HSD_ASSERT(681, pobj);
    return pobj;
}

static inline void resolveEnvelope(HSD_SList* list,
                                   HSD_EnvelopeDesc** edesc_p)
{
    if (list == NULL || edesc_p == NULL) {
        return;
    }

    for (; list && *edesc_p; list = list->next, edesc_p++) {
        HSD_Envelope* env = list->data;
        HSD_EnvelopeDesc* edesc = *edesc_p;

        while (env && edesc->joint) {
            HSD_JObjUnrefThis(env->jobj);
            env->jobj = HSD_IDGetData((u32) edesc->joint, NULL);
            HSD_ASSERT(714, env->jobj);
            HSD_JObjRefThis(env->jobj);
            env = env->next;
            edesc++;
        }
    }
}

static inline void HSD_PObjResolveRefs(HSD_PObj* pobj, HSD_PObjDesc* pdesc)
{
    if (!pobj || !pdesc) {
        return;
    }

    switch (pobj_type(pobj)) {
    case POBJ_ENVELOPE:
        resolveEnvelope(pobj->u.envelope_list, pdesc->u.envelope_p);
        break;

    case POBJ_SKIN:
        HSD_JObjUnrefThis(pobj->u.jobj);
        pobj->u.jobj = NULL;
        if (pdesc->u.joint != NULL) {
            pobj->u.jobj = HSD_IDGetData((u32) pdesc->u.joint, NULL);
            HSD_ASSERT(741, pobj->u.jobj);
            HSD_JObjRefThis(pobj->u.jobj);
        }
        break;

    default:
        break;
    }
}

void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObjDesc* pdesc)
{
    for (; pobj != NULL && pdesc != NULL;
         pobj = pobj->next, pdesc = pdesc->next)
    {
        HSD_PObjResolveRefs(pobj, pdesc);
    }
}

/*
 * Colosseum addition (HAL name unknown): totals the vertices and triangles
 * of a PObj list. Each PObj's vertex size comes from its descriptor list;
 * a direct attribute in an unsupported format ends that PObj's count,
 * which retail branches straight to the next PObj for.
 */
void fn_801ACDAC(HSD_PObj* pobj, u32* nb_vtx, u32* nb_tri)
{
    u32 vtx = 0;
    u32 tri = 0;
    HSD_VtxDescList* desc;
    int vtx_size;
    int comp_size;
    int i;
    int length;
    u8* dl;

    for (; pobj != NULL; pobj = pobj->next) {
        vtx_size = 0;
        for (desc = pobj->verts; desc->attr != GX_VA_NULL; desc++) {
            switch (desc->attr_type) {
            case GX_INDEX8:
                vtx_size += 1;
                break;
            case GX_INDEX16:
                vtx_size += 2;
                break;
            case GX_DIRECT:
                if (desc->attr == GX_VA_CLR0 || desc->attr == GX_VA_CLR1) {
                    switch (desc->comp_type) {
                    case GX_RGB565:
                    case GX_RGBA4:
                        comp_size = 2;
                        break;
                    case GX_RGB8:
                    case GX_RGBA6:
                        comp_size = 3;
                        break;
                    case GX_RGBX8:
                    case GX_RGBA8:
                        comp_size = 4;
                        break;
                    }
                } else {
                    switch (desc->comp_type) {
                    case GX_U8:
                    case GX_S8:
                        comp_size = 1;
                        break;
                    case GX_U16:
                    case GX_S16:
                        comp_size = 2;
                        break;
                    case GX_F32:
                        comp_size = 4;
                        break;
                    default:
                        goto next;
                    }
                }
                switch (desc->attr) {
                case GX_VA_PNMTXIDX:
                    vtx_size += 1;
                    break;
                case GX_VA_POS:
                    if (desc->comp_cnt == GX_POS_XY) {
                        vtx_size += comp_size * 2;
                    } else {
                        vtx_size += comp_size * 3;
                    }
                    break;
                case GX_VA_NRM:
                    if (desc->comp_cnt == GX_NRM_XYZ) {
                        vtx_size += comp_size * 3;
                    }
                    break;
                case GX_VA_CLR0:
                case GX_VA_CLR1:
                    vtx_size += comp_size;
                    break;
                case GX_VA_TEX0:
                case GX_VA_TEX1:
                case GX_VA_TEX2:
                case GX_VA_TEX3:
                case GX_VA_TEX4:
                case GX_VA_TEX5:
                case GX_VA_TEX6:
                case GX_VA_TEX7:
                    if (desc->comp_cnt == GX_TEX_S) {
                        vtx_size += comp_size;
                    } else {
                        vtx_size += comp_size * 2;
                    }
                    break;
                default:
                    goto next;
                }
                break;
            }
        }

        dl = pobj->display;
        length = pobj->n_display << 5;
        if (dl == NULL) {
            continue;
        }
        for (i = 0; i < length;) {
            int prim = dl[i] & GX_OPCODE_MASK;
            u32 n;

            i++;
            if (prim == 0) {
                continue;
            }
            n = ((u16) dl[i] << 8) + (u16) dl[i + 1];
            i += n * vtx_size + 2;
            vtx += n;
            switch (prim) {
            case GX_TRIANGLES:
                tri += n / 3;
                break;
            case GX_TRIANGLESTRIP:
            case GX_TRIANGLEFAN:
                tri += n - 2;
                break;
            case GX_QUADS:
                tri += n / 2;
                break;
            }
        }
    next:;
    }
    if (nb_vtx != NULL) {
        *nb_vtx = vtx;
    }
    if (nb_tri != NULL) {
        *nb_tri = tri;
    }
}

void HSD_ClearVtxDesc(void)
{
    GXClearVtxDesc();
    prev_vtxdesclist_array = 0;
    prev_vtxdesc = 0;
}

static inline void setupArrayDesc(HSD_VtxDescList* desc_list)
{
    HSD_VtxDescList* desc;

    if (prev_vtxdesclist_array != desc_list) {
        for (desc = desc_list; desc->attr != GX_VA_NULL; desc++) {
            if (desc->attr_type != GX_DIRECT) {
                GXSetArray(desc->attr, desc->vertex, desc->stride);
            }
        }
        prev_vtxdesclist_array = desc_list;
    }
}

static inline void setupVtxDesc(HSD_PObj* pobj)
{
    HSD_VtxDescList* desc;

    if (prev_vtxdesc != pobj->verts) {
        GXClearVtxDesc();
        for (desc = pobj->verts; desc->attr != GX_VA_NULL; desc++) {
            GXSetVtxDesc(desc->attr, desc->attr_type);
            switch (desc->attr) {
            case GX_VA_PNMTXIDX:
            case GX_VA_TEX0MTXIDX:
            case GX_VA_TEX1MTXIDX:
            case GX_VA_TEX2MTXIDX:
            case GX_VA_TEX3MTXIDX:
            case GX_VA_TEX4MTXIDX:
            case GX_VA_TEX5MTXIDX:
            case GX_VA_TEX6MTXIDX:
            case GX_VA_TEX7MTXIDX:
                break;
            default:
                GXSetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt,
                            desc->comp_type, desc->frac);
            }
        }
        prev_vtxdesc = pobj->verts;
    }
}

static inline void setupShapeAnimArrayDesc(HSD_VtxDescList* desc_list)
{
    HSD_VtxDescList* desc;

    for (desc = desc_list; desc->attr != GX_VA_NULL; desc++) {
        if (desc->attr_type != GX_DIRECT) {
            switch (desc->attr) {
            case GX_VA_POS:
            case GX_VA_NRM:
            case GX_VA_NBT:
                break;
            default:
                GXSetArray(desc->attr, desc->vertex, desc->stride);
            }
        }
    }
    prev_vtxdesclist_array = NULL;
}

static inline void setupShapeAnimVtxDesc(HSD_PObj* pobj)
{
    HSD_VtxDescList* desc;

    GXClearVtxDesc();
    for (desc = pobj->verts; desc->attr != GX_VA_NULL; desc++) {
        switch (desc->attr) {
        case GX_VA_NRM:
        case GX_VA_POS:
        case GX_VA_NBT:
            GXSetVtxDesc(desc->attr, GX_DIRECT);
            GXSetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt, GX_F32, 0);
            break;

        case GX_VA_PNMTXIDX:
        case GX_VA_TEX0MTXIDX:
        case GX_VA_TEX1MTXIDX:
        case GX_VA_TEX2MTXIDX:
        case GX_VA_TEX3MTXIDX:
        case GX_VA_TEX4MTXIDX:
        case GX_VA_TEX5MTXIDX:
        case GX_VA_TEX6MTXIDX:
        case GX_VA_TEX7MTXIDX:
            GXSetVtxDesc(desc->attr, desc->attr_type);
            break;

        default:
            GXSetVtxDesc(desc->attr, desc->attr_type);
            GXSetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt,
                        desc->comp_type, desc->frac);
        }
    }
    prev_vtxdesc = NULL;
}

static void get_shape_vertex_xyz(HSD_ShapeSet* shape_set, int shape_id,
                                 int arrayidx, f32 dst[3])
{
    u8* index_array = shape_set->vertex_idx_list[shape_id];
    int idx;
    void* src_base;

    if (shape_set->vertex_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    HSD_ASSERT(1082, shape_set->vertex_desc->comp_cnt == GX_POS_XYZ);
    src_base = ((u8*) shape_set->vertex_desc->vertex) +
               idx * shape_set->vertex_desc->stride;

    if (shape_set->vertex_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        int decimal_point = 1 << shape_set->vertex_desc->frac;
        switch (shape_set->vertex_desc->comp_type) {
        case GX_U8:
            {
                u8* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;

        case GX_S8:
            {
                s8* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;

        case GX_U16:
            {
                u16* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;

        case GX_S16:
            {
                s16* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;

        default:
            HSD_Panic(__FILE__, 1125, "unexpected vertex type.\n");
        }
    }
}

static void get_shape_normal_xyz(HSD_ShapeSet* shape_set, int shape_id,
                                 int arrayidx, f32 dst[3])
{
    u8* index_array = shape_set->normal_idx_list[shape_id];
    int idx;
    void* src_base;

    if (shape_set->normal_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    HSD_ASSERT(1145, shape_set->normal_desc->comp_cnt == GX_NRM_XYZ);
    src_base = ((u8*) shape_set->normal_desc->vertex) +
               idx * shape_set->normal_desc->stride;

    if (shape_set->normal_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        int decimal_point = 1 << shape_set->normal_desc->frac;
        switch (shape_set->normal_desc->comp_type) {
        case GX_U8:
            {
                u8* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;
        case GX_S8:
            {
                s8* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;
        case GX_U16:
            {
                u16* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;
        case GX_S16:
            {
                s16* src = src_base;
                dst[0] = (f32) src[0] / decimal_point;
                dst[1] = (f32) src[1] / decimal_point;
                dst[2] = (f32) src[2] / decimal_point;
            }
            break;
        default:
            HSD_Panic(__FILE__, 1188, "unexpected normal type.");
        }
    }
}

static void get_shape_nbt_xyz(HSD_ShapeSet* shape_set, int shape_id,
                              int arrayidx, f32* dst)
{
    u8* index_array = shape_set->normal_idx_list[shape_id];
    int i, idx;
    void* src_base;

    HSD_ASSERT(1201, shape_set->normal_desc->attr == GX_VA_NBT);

    if (shape_set->normal_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    HSD_ASSERT(1210, shape_set->normal_desc->comp_cnt == GX_NRM_XYZ);

    src_base = ((u8*) shape_set->normal_desc->vertex) +
               idx * shape_set->normal_desc->stride;

    if (shape_set->normal_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[9]));
    } else {
        int decimal_point = 1 << shape_set->normal_desc->frac;
        switch (shape_set->normal_desc->comp_type) {
        case GX_U8:
            for (i = 0; i < 9; i++) {
                dst[i] = (f32) ((u8*) src_base)[i] / decimal_point;
            }
            break;
        case GX_S8:
            for (i = 0; i < 9; i++) {
                dst[i] = (f32) ((s8*) src_base)[i] / decimal_point;
            }
            break;
        case GX_U16:
            for (i = 0; i < 9; i++) {
                dst[i] = (f32) ((u16*) src_base)[i] / decimal_point;
            }
            break;
        case GX_S16:
            for (i = 0; i < 9; i++) {
                dst[i] = (f32) ((s16*) src_base)[i] / decimal_point;
            }
            break;
        default:
            HSD_Panic(__FILE__, 1241, "unexpected normal type.");
        }
    }
}

static void interpretShapeAnimDisplayList(HSD_PObj* pobj, f32 (*vertex)[3],
                                          f32 (*normal)[3])
{
    u8* dl = pobj->display;
    int length = pobj->n_display << 5;
    int l;

    for (l = 0; l + 3 < length;) {
        int n = dl[1] << 8 | dl[2];
        int m = 3;
        int i, j;

        if ((dl[0] & GX_OPCODE_MASK) == 0) {
            break;
        }
        GXBegin(dl[0] & GX_OPCODE_MASK, dl[0] & GX_VAT_MASK, n);
        for (i = 0; i < n; i++) {
            for (j = 0;; j++) {
                HSD_VtxDescList* desc = &pobj->verts[j];
                if (desc->attr == GX_VA_NULL) {
                    break;
                } else {
                    u16 idx = dl[m++];
                    switch (desc->attr) {
                    case GX_VA_PNMTXIDX:
                    case GX_VA_TEX0MTXIDX:
                    case GX_VA_TEX1MTXIDX:
                    case GX_VA_TEX2MTXIDX:
                    case GX_VA_TEX3MTXIDX:
                    case GX_VA_TEX4MTXIDX:
                    case GX_VA_TEX5MTXIDX:
                    case GX_VA_TEX6MTXIDX:
                    case GX_VA_TEX7MTXIDX:
                        GXTexCoord1u8(idx);
                        break;

                    case GX_VA_POS:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        GXPosition3f32(vertex[idx][0], vertex[idx][1],
                                       vertex[idx][2]);
                        break;

                    case GX_VA_NRM:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        GXNormal3f32(normal[idx][0], normal[idx][1],
                                     normal[idx][2]);
                        break;

                    case GX_VA_NBT:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        idx *= 3;
                        GXNormal3f32(normal[idx + 0][0], normal[idx + 0][1],
                                     normal[idx + 0][2]);
                        GXNormal3f32(normal[idx + 1][0], normal[idx + 1][1],
                                     normal[idx + 1][2]);
                        GXNormal3f32(normal[idx + 2][0], normal[idx + 2][1],
                                     normal[idx + 2][2]);
                        break;

                    case GX_VA_TEX0:
                    case GX_VA_TEX1:
                    case GX_VA_TEX2:
                    case GX_VA_TEX3:
                    case GX_VA_TEX4:
                    case GX_VA_TEX5:
                    case GX_VA_TEX6:
                    case GX_VA_TEX7:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                            GXTexCoord1x16(idx);
                        } else {
                            GXTexCoord1x8(idx);
                        }
                        break;

                    case GX_VA_CLR0:
                    case GX_VA_CLR1:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                            GXColor1x16(idx);
                        } else if (desc->attr_type == GX_INDEX8) {
                            GXColor1x8(idx);
                        } else {
                            switch (desc->comp_type) {
                            case GX_RGB565:
                            case GX_RGBA4:
                                GXColor1u16((idx << 8) | dl[m++]);
                                break;
                            case GX_RGB8:
                            case GX_RGBA6:
                                GXColor3u8(idx, dl[m], dl[m + 1]);
                                m += 2;
                                break;
                            case GX_RGBA8:
                            case GX_RGBX8:
                                GXColor4u8(idx, dl[m], dl[m + 1], dl[m + 2]);
                                m += 3;
                                break;
                            }
                        }
                        break;
                    default:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        OSReport("attr(%d) is not supported by sysdolphin\n",
                                 desc->attr);
                        break;
                    }
                }
            }
        }
        GXEnd();
        l += m;
        dl += m;
    }
}

#define pobj_min(x, y) (x < y ? x : y)
#define pobj_max(x, y) ((x) > (y) ? (x) : (y))

static void drawShapeAnim(HSD_PObj* pobj)
{
    HSD_ShapeSet* shape_set = pobj->u.shape_set;
    f32 blend;
    int shape_id, i;
    int blend_nbt;

    if (vertex_buffer_size == 0) {
        vertex_buffer_size = HSD_DEFAULT_MAX_SHAPE_VERTICES;
        vertex_buffer = HSD_MemAlloc(vertex_buffer_size * sizeof(f32[3]));
    }
    HSD_ASSERT(1387, vertex_buffer_size >= shape_set->nb_vertex_index);
    if (shape_set->normal_desc && normal_buffer_size == 0) {
        normal_buffer_size = HSD_DEFAULT_MAX_SHAPE_NORMALS;
        normal_buffer = HSD_MemAlloc(normal_buffer_size * sizeof(f32[3]));
    }

    if (shape_set->normal_desc) {
        if (shape_set->normal_desc->attr == GX_VA_NRM) {
            HSD_ASSERT(1396, normal_buffer_size >= shape_set->nb_normal_index);
            blend_nbt = 0;
        } else {
            HSD_ASSERT(1399, normal_buffer_size >= shape_set->nb_normal_index * 3);
            blend_nbt = 1;
        }
    }

    if (shape_set->flags & SHAPESET_AVERAGE) {
        blend = shape_set->blend.bl;
        shape_id = pobj_min(((int) blend < 0 ? 0 : (int) blend),
                            shape_set->nb_shape - 1);
        blend = pobj_min(pobj_max(0.0, blend - (f32) shape_id), 1.0F);
        for (i = 0; i < shape_set->nb_vertex_index; i++) {
            f32 s0[3], s1[3];

            get_shape_vertex_xyz(shape_set, shape_id, i, s0);
            get_shape_vertex_xyz(
                shape_set, pobj_min(shape_id + 1, shape_set->nb_shape - 1), i,
                s1);
            vertex_buffer[i][0] = (s1[0] - s0[0]) * blend + s0[0];
            vertex_buffer[i][1] = (s1[1] - s0[1]) * blend + s0[1];
            vertex_buffer[i][2] = (s1[2] - s0[2]) * blend + s0[2];
        }
        if (shape_set->nb_normal_index) {
            if (blend_nbt) {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 s0[9], s1[9];
                    int j, idx = i * 3;

                    get_shape_nbt_xyz(shape_set, shape_id, i, s0);
                    get_shape_nbt_xyz(
                        shape_set,
                        pobj_min(shape_id + 1, shape_set->nb_shape - 1), i,
                        s1);
                    for (j = 0; j < 9; j++) {
                        normal_buffer[idx][j] =
                            (s1[j] - s0[j]) * blend + s0[j];
                    }
                }
            } else {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 s0[3], s1[3];

                    get_shape_normal_xyz(shape_set, shape_id, i, s0);
                    get_shape_normal_xyz(
                        shape_set,
                        pobj_min(shape_id + 1, shape_set->nb_shape - 1), i,
                        s1);
                    normal_buffer[i][0] = (s1[0] - s0[0]) * blend + s0[0];
                    normal_buffer[i][1] = (s1[1] - s0[1]) * blend + s0[1];
                    normal_buffer[i][2] = (s1[2] - s0[2]) * blend + s0[2];
                }
            }
        }
    } else {
        int j;
        f32* blend_bp;
        blend_bp = shape_set->blend.bp;
        for (i = 0; i < shape_set->nb_vertex_index; i++) {
            get_shape_vertex_xyz(shape_set, 0, i, vertex_buffer[i]);
            for (j = 0; j < shape_set->nb_shape; j++) {
                f32 b = pobj_max(0.0, blend_bp[j]);
                f32 s[3];

                get_shape_vertex_xyz(shape_set, j + 1, i, s);
                vertex_buffer[i][0] += s[0] * b;
                vertex_buffer[i][1] += s[1] * b;
                vertex_buffer[i][2] += s[2] * b;
            }
        }
        if (shape_set->nb_normal_index) {
            if (blend_nbt) {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    s32 idx = i * 3;
                    get_shape_nbt_xyz(shape_set, 0, i, normal_buffer[idx]);
                    for (j = 0; j < shape_set->nb_shape; j++) {
                        f32 b = pobj_max(0.0, blend_bp[j]);
                        f32 s[9];
                        int k;

                        get_shape_nbt_xyz(shape_set, j + 1, i, s);
                        for (k = 0; k < 9; k++) {
                            normal_buffer[idx][k] += s[k] * b;
                        }
                    }
                }
            } else {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    get_shape_normal_xyz(shape_set, 0, i, normal_buffer[i]);
                    for (j = 0; j < shape_set->nb_shape; j++) {
                        f32 b = pobj_max(0.0, blend_bp[j]);
                        f32 s[3];

                        get_shape_normal_xyz(shape_set, j + 1, i, s);
                        normal_buffer[i][0] += s[0] * b;
                        normal_buffer[i][1] += s[1] * b;
                        normal_buffer[i][2] += s[2] * b;
                    }
                }
            }
        }
    }
    interpretShapeAnimDisplayList(pobj, vertex_buffer, normal_buffer);
}

/* HSD_PObjClearMtxMark */
void HSD_PObjClearMtxMark(void* obj, u32 mark)
{
    int i;

    for (i = 0; i < 2; i++) {
        mtx_mark[i].obj = obj;
        mtx_mark[i].mark = mark;
    }
}

/* HSD_PObjSetMtxMark */
void HSD_PObjSetMtxMark(int idx, void* obj, u32 mark)
{
    if (idx >= 2) {
        return;
    }

    if (0 <= idx && idx < 2) {
    } else {
        mtx_mark[idx].obj = obj;
        mtx_mark[idx].mark = mark;
    }
}

void HSD_PObjGetMtxMark(int idx, void** obj, u32* mark)
{
    HSD_ASSERT(1635, obj);
    HSD_ASSERT(1636, mark);

    if (idx < 0 || 2 <= idx) {
        *obj = NULL;
        *mark = 0;
    } else {
        *obj = mtx_mark[idx].obj;
        *mark = mtx_mark[idx].mark;
    }
}

static inline PObjSetupFlag GetSetupFlags(HSD_JObj* jobj)
{
    PObjSetupFlag flags = SETUP_NONE;

    if (jobj->flags & 0x80) {
        flags |= SETUP_NORMAL;
    }

    if (_HSD_TObjGetCurrentByType(NULL, TEX_COORD_REFLECTION)) {
        flags |= SETUP_NORMAL | SETUP_REFLECTION;
    }

    if (_HSD_TObjGetCurrentByType(NULL, TEX_COORD_UNK5)) {
        flags |= SETUP_NORMAL | SETUP_REFLECTION;
    }

    if (_HSD_TObjGetCurrentByType(NULL, TEX_COORD_HILIGHT)) {
        flags |= SETUP_NORMAL | SETUP_HIGHLIGHT;
    }

    return flags;
}

static inline void SetupRigidModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                      u32 rendermode)
{
    HSD_JObj* jobj;
    Mtx n;
    PObjSetupFlag flags;

    jobj = HSD_JObjGetCurrent();

    {
        void* obj;
        u32 mark;

        HSD_PObjGetMtxMark(0, &obj, &mark);
        if (obj == jobj && mark == HSD_MTX_RIGID) {
            return;
        }
        HSD_PObjSetMtxMark(0, jobj, HSD_MTX_RIGID);
    }

    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadPosMtxImm(pmtx, GX_PNMTX0);
    HSD_PerfCountMtxLoad();

    flags = GetSetupFlags(jobj);

    if (flags & SETUP_NORMAL) {
        HSD_MtxInverseTranspose(pmtx, n);
        if (jobj->flags & 0x80) {
            GXLoadNrmMtxImm(n, GX_PNMTX0);
            HSD_PerfCountMtxLoad();
        }
        if (flags & SETUP_NORMAL_PROJECTION) {
            GXLoadTexMtxImm(n, GX_TEXMTX0, GX_MTX3x4);
            HSD_PerfCountMtxLoad();
        }
    }
}

static void SetupSharedVtxModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                   u32 rendermode)
{
    HSD_JObj* jobj;
    Mtx n0, n1, m;
    PObjSetupFlag flags = SETUP_NONE;

    jobj = HSD_JObjGetCurrent();
    {
        void* obj;
        u32 mark;

        HSD_PObjGetMtxMark(0, &obj, &mark);
        if (obj != jobj && mark != HSD_MTX_RIGID) {
            flags |= SETUP_JOINT0;
        }

        HSD_PObjGetMtxMark(1, &obj, &mark);
        if (obj != pobj->u.jobj && mark != HSD_MTX_RIGID) {
            flags |= SETUP_JOINT1;
        }
    }

    if (flags == SETUP_NONE) {
        return;
    }

    flags |= GetSetupFlags(jobj);

    if (flags | SETUP_JOINT0) {
        GXSetCurrentMtx(GX_PNMTX0);
        GXLoadPosMtxImm(pmtx, GX_PNMTX0);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(pmtx, n0);
            if (jobj->flags & 0x80) {
                GXLoadNrmMtxImm(n0, GX_PNMTX0);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(n0, GX_TEXMTX0, GX_MTX3x4);
                HSD_PerfCountMtxLoad();
            }
        }
    }
    if (flags | SETUP_JOINT1) {
        HSD_JObjSetupMatrix(pobj->u.jobj);
        PSMTXConcat(vmtx, pobj->u.jobj->mtx, m);
        GXLoadPosMtxImm(m, GX_PNMTX1);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(m, n1);
            if (jobj->flags & 0x80) {
                GXLoadNrmMtxImm(n1, GX_PNMTX1);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(n1, GX_TEXMTX1, GX_MTX3x4);
                HSD_PerfCountMtxLoad();
            }
        }
    }
}

static void SetupEnvelopeModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                  u32 rendermode)
{
    HSD_JObj* jobj;
    HSD_SList* list;
    int MtxIdx = 0;
    MtxPtr right;
    Mtx mtx;
    PObjSetupFlag flags = SETUP_NONE;

    jobj = HSD_JObjGetCurrent();
    HSD_PObjClearMtxMark(NULL, HSD_MTX_ENVELOPE);
    flags = GetSetupFlags(jobj);
    right = _HSD_mkEnvelopeModelNodeMtx(jobj, mtx);

    for (MtxIdx = 0, list = pobj->u.envelope_list; MtxIdx < 10 && list;
         MtxIdx++, list = list->next)
    {
        Mtx mtx, tmp;
        MtxPtr mtxp;
        HSD_Envelope* envelope = list->data;
        s32 mtx_no = HSD_Index2PosNrmMtx(MtxIdx);
        int perf = 0;

        HSD_ASSERT(1822, envelope);
        if (envelope->weight >= 1.0F) {
            HSD_JObjSetupMatrix(envelope->jobj);
            if (right) {
                PSMTXConcat(envelope->jobj->mtx, envelope->jobj->envelopemtx,
                            mtx);
                mtxp = mtx;
            } else {
                mtxp = envelope->jobj->mtx;
            }
        } else {
            mtx[0][0] = mtx[0][1] = mtx[0][2] = mtx[0][3] = mtx[1][0] =
                mtx[1][1] = mtx[1][2] = mtx[1][3] = mtx[2][0] = mtx[2][1] =
                    mtx[2][2] = mtx[2][3] = 0.0F;
            while (envelope) {
                HSD_JObj* jp;

                HSD_ASSERT(1842, envelope->jobj);
                jp = envelope->jobj;
                HSD_JObjSetupMatrix(jp);
                HSD_ASSERT(1845, jp->mtx);
                HSD_ASSERT(1846, jp->envelopemtx);

                PSMTXConcat(jp->mtx, jp->envelopemtx, tmp);
                HSD_MtxScaledAdd(tmp, mtx, mtx, envelope->weight);
                perf++;
                envelope = envelope->next;
            }
            mtxp = mtx;
        }
        HSD_PerfCountEnvelopeBlending(perf);
        if (right) {
            PSMTXConcat(mtxp, right, mtx);
        }
        PSMTXConcat(vmtx, mtxp, tmp);
        GXLoadPosMtxImm(tmp, mtx_no);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(tmp, mtx);
            if (jobj->flags & 0x80) {
                GXLoadNrmMtxImm(mtx, mtx_no);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(mtx, HSD_Index2TexMtx(MtxIdx), GX_MTX3x4);
                HSD_PerfCountMtxLoad();
            }
        }
    }
}

static void PObjSetupMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    switch (pobj_type(pobj)) {
    case POBJ_SKIN:
        if (!pobj->u.jobj) {
            SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
        } else {
            SetupSharedVtxModelMtx(pobj, vmtx, pmtx, rendermode);
        }
        break;
    case POBJ_SHAPEANIM:
        SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
        break;
    case POBJ_ENVELOPE:
        SetupEnvelopeModelMtx(pobj, vmtx, pmtx, rendermode);
        break;
    }
}

static inline void PObjDispSimplePrimitive(HSD_PObj* pobj, u32 rendermode)
{
    setupArrayDesc(pobj->verts);
    setupVtxDesc(pobj);

    GXCallDisplayList(pobj->display, pobj->n_display << 5);
}

static inline void PObjDispShapeAnim(HSD_PObj* pobj, u32 rendermode)
{
    setupShapeAnimArrayDesc(pobj->verts);
    setupShapeAnimVtxDesc(pobj);

    HSD_ASSERT(1921, pobj->u.shape_set);
    drawShapeAnim(pobj);
}

void HSD_PObjDisp(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    if (pobj->flags & POBJ_HIDDEN) {
        return;
    }

    switch (pobj->flags & (POBJ_CULLFRONT | POBJ_CULLBACK)) {
    case 0x0:
        HSD_StateSetCullMode(GX_CULL_NONE);
        break;
    case POBJ_CULLFRONT:
        HSD_StateSetCullMode(GX_CULL_FRONT);
        break;
    case POBJ_CULLBACK:
        HSD_StateSetCullMode(GX_CULL_BACK);
        break;
    case POBJ_CULLFRONT | POBJ_CULLBACK:
        return;
    }

    HSD_POBJ_METHOD(pobj)->setup_mtx(pobj, vmtx, pmtx, rendermode);
    if (pobj_type(pobj) == POBJ_SHAPEANIM) {
        PObjDispShapeAnim(pobj, rendermode);
    } else {
        PObjDispSimplePrimitive(pobj, rendermode);
    }
}

static void PObjRelease(HSD_Class* o)
{
    HSD_PObj* pobj = HSD_POBJ(o);

    if (pobj->aobj != NULL) {
        HSD_AObjRemove(pobj->aobj);
    }

    switch (pobj_type(pobj)) {
    case POBJ_SHAPEANIM:
        HSD_ShapeSetRemove(pobj->u.shape_set);
        break;
    case POBJ_ENVELOPE:
        HSD_EnvelopeListFree(pobj->u.envelope_list);
        break;
    case POBJ_SKIN:
        HSD_JObjUnrefThis(pobj->u.jobj);
        break;
    default:
        break;
    }
    HSD_PARENT_INFO(&hsdPObj)->release(o);
}

static void PObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdPObj)) {
        if (fn_801A6990(vertex_buffer)) {
            vertex_buffer = NULL;
            vertex_buffer_size = 0;
        }
        if (fn_801A6990(normal_buffer)) {
            normal_buffer = NULL;
            normal_buffer_size = 0;
        }
        prev_vtxdesclist_array = NULL;
        prev_vtxdesc = NULL;
    }
    HSD_PARENT_INFO(&hsdPObj)->amnesia(info);
}

static void PObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdPObj), &lbl_8036C638,
                     "sysdolphin_base_library", "hsd_pobj",
                     sizeof(HSD_PObjInfo), sizeof(HSD_PObj));
    HSD_CLASS_INFO(&hsdPObj)->release = PObjRelease;
    HSD_CLASS_INFO(&hsdPObj)->amnesia = PObjAmnesia;
    HSD_POBJ_INFO(&hsdPObj)->disp = HSD_PObjDisp;
    HSD_POBJ_INFO(&hsdPObj)->setup_mtx = PObjSetupMtx;
    HSD_POBJ_INFO(&hsdPObj)->load = (s32 (*)(HSD_PObj*, HSD_PObjDesc*)) PObjLoad;
    HSD_POBJ_INFO(&hsdPObj)->update = PObjUpdateFunc;
}
