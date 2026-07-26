#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_pobj.h"

/* =========================================================================
 * Partial banked source for reserved split unit 0x801AA608 - 0x801AE000.
 * Only the functions that byte-match under GC/1.3 are provided (from backup
 * hsd_pobj_disp.c); the remaining functions (dispatch/vtable reconstructions
 * that are 22-75% source-refinement fleet targets, plus the empty-stub
 * renderers) stay as extracted asm. objdiff pairs by symbol name.
 * ========================================================================= */

/* Helper functions (DTK names). */
extern s32   fn_801A6990(void* obj);
extern void  fn_800B7D3C(void);
extern void  fn_801C27F4(void* aobj, void* pobj, void* method);
extern void  PObjRelease(HSD_Class* object);
extern void  PObjAmnesia(void* pobj);
extern void  PObjSetupMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                          u32 rendermode);
extern s32   PObjLoad(HSD_PObj* pobj, HSD_PObjDesc* desc);
extern void  PObjUpdateFunc(HSD_PObj* pobj, s32 idx, f32* weight_ptr);
extern void  HSD_JObjUnrefThis(HSD_JObj* jobj);
extern void  HSD_JObjRefThis(HSD_JObj* jobj);
extern void* HSD_IDGetDataFromTable(void* table, u32 id, s32* success);
extern void  HSD_AObjRemove(HSD_AObj* aobj);
extern void  fn_801A6960(void* mem);
extern void  fn_80193AF0(void* mem, s32 size);
extern HSD_SList* fn_801A3E64(HSD_SList* list);
extern HSD_SList* fn_801A3F48(void);
extern void* fn_80193B10(s32 size);
extern void* fn_801A6928(s32 size);
extern void fn_8019C6EC(u32 flags);
extern void* memset(void* dst, s32 value, u32 size);
extern void HSD_Panic(const char* file, s32 line, const char* message);
extern HSD_ClassInfo* fn_80193748(const char* class_name);
extern void* fn_80193828(HSD_ClassInfo* info);
extern void __assert(const char* file, u32 line, const char* message);

/* Data / global symbols (DTK names). */
extern u8    lbl_8036CCD0[];         /* PObj class info (data)      */
extern u8    lbl_8036C638[];         /* parent class info           */
extern u8    lbl_80274EE0[];         /* class library name          */
extern u8    lbl_80274EF8[];         /* class name                  */
extern void* lbl_8047B2E8;           /* cached default instance     */
extern void* lbl_8047B2EC;           /* active normal desc          */
extern void* lbl_8047B2F0;           /* active color desc           */
extern u32   lbl_8047B2F4;           /* shape vertex capacity       */
extern u32   lbl_8047B2F8;           /* shape normal capacity       */
extern u32   lbl_8047B2FC;           /* display list marker         */
extern void* lbl_8047B300;           /* display list end marker     */
extern void* lbl_8047B308;           /* active texture desc         */
extern u32   lbl_8047B30C;           /* texture count               */
extern void* lbl_80478C90;           /* RNG default state instance  */
extern void* lbl_80478C94;           /* RNG current state pointer   */
extern char  lbl_8047DCB8;           /* "pobj.c"                    */
extern char  lbl_8047DD10;           /* "pobj"                      */

extern void fn_801ABDD4(HSD_PObj* pobj, f32 vertex_buffer[][3],
                        f32 normal_buffer[][3]);
extern void fn_801AC1F8(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                        f32 dst[9]);
void get_shape_normal_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3]);
void get_shape_vertex_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3]);
extern void SetupEnvelopeModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                  u32 rendermode);
extern void SetupSharedVtxModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                   u32 rendermode);
extern void fn_800BD554(u32 index);
extern void GXLoadPosMtxImm(Mtx mtx, u32 index);
extern void GXLoadNrmMtxImm(Mtx mtx, u32 index);
extern void GXLoadTexMtxImm(Mtx mtx, u32 index, u32 type);
extern u32 PSMTXInvXpose(Mtx src, Mtx dst);
extern void PSMTXCopy(Mtx src, Mtx dst);
extern void PSMTXConcat(Mtx left, Mtx right, Mtx dst);
extern u32 lbl_8036CC40[];
extern HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj* from, u32 mapping);
extern MtxPtr _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* model, MtxPtr mtx);
extern s32 HSD_Index2PosNrmMtx(u32 index);
extern s32 HSD_Index2TexMtx(u32 index);
extern void HSD_MtxScaledAdd(f32* src, f32 scale, f32* add, f32* dst);
extern void fn_801AA5AC(s32 index);
extern const f32 lbl_8047DCC0;
extern char lbl_8047DCC4;
extern char lbl_8047DCCC;
extern char lbl_8047DCD8;
void HSD_PObjGetMtxMark(s32 index, u32* first, u32* second);
void fn_801AB5F8(s32 index, void* ptr, s32 value);
void fn_801AB63C(u32 first, u32 second);

/* Address: 0x801AA608 | Size: 0xC8  -- PObj class info init */
#pragma push
#pragma optimization_level 1
void HSD_PObjInit(void)
{
    hsdInitClassInfo((HSD_ClassInfo*) lbl_8036CCD0,
                     (HSD_ClassInfo*) lbl_8036C638, (char*) lbl_80274EE0,
                     (char*) lbl_80274EF8, 0x4C, 0x1C);

    *(void**) ((u8*) lbl_8036CCD0 + 0x30) = (void*) PObjRelease;
    *(void**) ((u8*) lbl_8036CCD0 + 0x38) = (void*) PObjAmnesia;
    *(void**) ((u8*) lbl_8036CCD0 + 0x3C) = (void*) HSD_PObjDisp;
    *(void**) ((u8*) lbl_8036CCD0 + 0x40) = (void*) PObjSetupMtx;
    *(void**) ((u8*) lbl_8036CCD0 + 0x44) = (void*) PObjLoad;
    *(void**) ((u8*) lbl_8036CCD0 + 0x48) = (void*) PObjUpdateFunc;
}
#pragma pop

static inline void PObjFreeEnvelopeList(HSD_SList* list)
{
    while (list != NULL) {
        HSD_Envelope* envelope = list->data;

        while (envelope != NULL) {
            HSD_Envelope* next = envelope->next;
            HSD_JObjUnrefThis(envelope->jobj);
            fn_80193AF0(envelope, sizeof(HSD_Envelope));
            envelope = next;
        }
        list = fn_801A3E64(list);
    }
}

static inline void PObjFreeShapeSet(HSD_ShapeSet* shapeSet)
{
    if (shapeSet == NULL) {
        return;
    }
    if (shapeSet->flags & 2) {
        fn_801A6960(shapeSet->blend.bp);
    }
    fn_80193AF0(shapeSet, sizeof(HSD_ShapeSet));
}

void PObjRelease(HSD_Class* object)
{
    HSD_PObj* pobj = HSD_POBJ(object);

    if (pobj->aobj != NULL) {
        HSD_AObjRemove(pobj->aobj);
    }
    switch (pobj->flags & 0x3000) {
    case 0x1000:
        PObjFreeShapeSet(pobj->u.shape_set);
        break;
    case 0x2000:
        PObjFreeEnvelopeList(pobj->u.envelope_list);
        break;
    case 0:
        HSD_JObjUnrefThis(pobj->u.jobj);
        break;
    default:
        break;
    }
    ((HSD_ClassInfo*)lbl_8036CCD0)->head.parent->release(object);
}

typedef enum PObjSetupFlag {
    SETUP_NORMAL = 1,
    SETUP_REFLECTION = 2,
    SETUP_HIGHLIGHT = 4,
    SETUP_NORMAL_PROJECTION = 6,
} PObjSetupFlag;

static inline void HSD_PerfCountMtxLoad(void)
{
    lbl_8036CC40[3]++;
}

static inline void HSD_MtxInverseTranspose(Mtx src, Mtx dst)
{
    if (PSMTXInvXpose(src, dst) == 0) {
        PSMTXCopy(src, dst);
    }
}

static inline PObjSetupFlag GetSetupFlags(HSD_JObj* jobj, u32 rendermode)
{
    PObjSetupFlag flags = 0;

    if (!(rendermode & 0x04000000)) {
        if (jobj->flags & JOBJ_LIGHTING) {
            flags |= SETUP_NORMAL;
        }
        if (_HSD_TObjGetCurrentByType(NULL, 1) != NULL) {
            flags |= SETUP_NORMAL | SETUP_REFLECTION;
        }
        if (_HSD_TObjGetCurrentByType(NULL, 5) != NULL) {
            flags |= SETUP_NORMAL | SETUP_HIGHLIGHT;
        }
        if (_HSD_TObjGetCurrentByType(NULL, 2) != NULL) {
            flags |= SETUP_NORMAL | SETUP_HIGHLIGHT;
        }
    }
    return flags;
}

static inline void SetupRigidModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                                      u32 rendermode)
{
    HSD_JObj* jobj = HSD_JObjGetCurrent();
    Mtx normal_mtx;
    PObjSetupFlag flags;
    void* marked_obj;
    u32 mark;

    HSD_PObjGetMtxMark(0, (u32*) &marked_obj, &mark);
    if (marked_obj == jobj && mark == HSD_MTX_RIGID) {
        return;
    }
    fn_801AB5F8(0, jobj, HSD_MTX_RIGID);

    fn_800BD554(0);
    GXLoadPosMtxImm(pmtx, 0);
    HSD_PerfCountMtxLoad();

    flags = GetSetupFlags(jobj, rendermode);
    if (flags & SETUP_NORMAL) {
        HSD_MtxInverseTranspose(pmtx, normal_mtx);
        if (jobj->flags & JOBJ_LIGHTING) {
            GXLoadNrmMtxImm(normal_mtx, 0);
            HSD_PerfCountMtxLoad();
        }
        if (flags & SETUP_NORMAL_PROJECTION) {
            GXLoadTexMtxImm(normal_mtx, 30, 0);
            HSD_PerfCountMtxLoad();
        }
    }
}

void PObjSetupMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    switch (pobj->flags & 0x3000) {
    case 0:
        if (pobj->u.jobj == NULL) {
            SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
        } else {
            SetupSharedVtxModelMtx(pobj, vmtx, pmtx, rendermode);
        }
        break;
    case 0x1000:
        SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
        break;
    case 0x2000:
        SetupEnvelopeModelMtx(pobj, vmtx, pmtx, rendermode);
        break;
    }
}

void SetupSharedVtxModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                            u32 rendermode)
{
    HSD_JObj* jobj = HSD_JObjGetCurrent();
    Mtx normal0;
    Mtx normal1;
    Mtx model;
    u32 flags = 0;
    void* marked_obj;
    u32 mark;

    HSD_PObjGetMtxMark(0, (u32*) &marked_obj, &mark);
    if (marked_obj != jobj && mark != HSD_MTX_RIGID) {
        flags |= 1;
    }
    HSD_PObjGetMtxMark(1, (u32*) &marked_obj, &mark);
    if (marked_obj != pobj->u.jobj && mark != HSD_MTX_RIGID) {
        flags |= 2;
    }
    if (flags == 0) {
        return;
    }

    flags |= GetSetupFlags(jobj, rendermode);
    if (flags | 1) {
        fn_800BD554(0);
        GXLoadPosMtxImm(pmtx, 0);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(pmtx, normal0);
            if (jobj->flags & JOBJ_LIGHTING) {
                GXLoadNrmMtxImm(normal0, 0);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(normal0, 30, 0);
                HSD_PerfCountMtxLoad();
            }
        }
    }
    if (flags | 2) {
        HSD_JObjSetupMatrix(pobj->u.jobj);
        PSMTXConcat(vmtx, pobj->u.jobj->mtx, model);
        GXLoadPosMtxImm(model, 3);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(model, normal1);
            if (jobj->flags & JOBJ_LIGHTING) {
                GXLoadNrmMtxImm(normal1, 3);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(normal1, 33, 0);
                HSD_PerfCountMtxLoad();
            }
        }
    }
}

void SetupEnvelopeModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx,
                           u32 rendermode)
{
    HSD_JObj* jobj = HSD_JObjGetCurrent();
    HSD_SList* list;
    s32 matrix_index;
    MtxPtr right;
    Mtx envelope_mtx;
    PObjSetupFlag flags;

    fn_801AB63C(0, HSD_MTX_ENVELOPE);
    flags = GetSetupFlags(jobj, rendermode);
    right = _HSD_mkEnvelopeModelNodeMtx(jobj, envelope_mtx);

    for (matrix_index = 0, list = pobj->u.envelope_list;
         matrix_index < 10 && list != NULL;
         matrix_index++, list = list->next)
    {
        Mtx matrix;
        Mtx temp;
        MtxPtr matrix_ptr;
        HSD_Envelope* envelope = list->data;
        s32 matrix_number = HSD_Index2PosNrmMtx(matrix_index);
        s32 blend_count = 0;

        if (envelope == NULL) {
            __assert(&lbl_8047DCB8, 0x71E,
                     (char*) lbl_80274EE0 + 0x124);
        }
        if (envelope->weight >= lbl_8047DCC0) {
            HSD_JObjSetupMatrix(envelope->jobj);
            if (right != NULL) {
                PSMTXConcat(envelope->jobj->mtx,
                            (MtxPtr) envelope->jobj->envelopemtx, matrix);
                matrix_ptr = matrix;
            } else {
                matrix_ptr = envelope->jobj->mtx;
            }
        } else {
            matrix[0][0] = matrix[0][1] = matrix[0][2] = matrix[0][3] =
            matrix[1][0] = matrix[1][1] = matrix[1][2] = matrix[1][3] =
            matrix[2][0] = matrix[2][1] = matrix[2][2] = matrix[2][3] = 0.0f;

            while (envelope != NULL) {
                HSD_JObj* envelope_jobj;

                if (envelope->jobj == NULL) {
                    __assert(&lbl_8047DCB8, 0x732, &lbl_8047DCCC);
                }
                envelope_jobj = envelope->jobj;
                HSD_JObjSetupMatrix(envelope_jobj);
                if (envelope_jobj->mtx == NULL) {
                    __assert(&lbl_8047DCB8, 0x735, &lbl_8047DCD8);
                }
                if (envelope_jobj->envelopemtx == NULL) {
                    __assert(&lbl_8047DCB8, 0x736,
                             (char*) lbl_80274EE0 + 0x158);
                }
                PSMTXConcat(envelope_jobj->mtx,
                            (MtxPtr) envelope_jobj->envelopemtx, temp);
                HSD_MtxScaledAdd((f32*) temp, envelope->weight,
                                 (f32*) matrix, (f32*) matrix);
                blend_count++;
                envelope = envelope->next;
            }
            matrix_ptr = matrix;
        }
        fn_801AA5AC(blend_count);
        if (right != NULL) {
            PSMTXConcat(matrix_ptr, right, matrix);
        }
        PSMTXConcat(vmtx, matrix_ptr, temp);
        GXLoadPosMtxImm(temp, matrix_number);
        HSD_PerfCountMtxLoad();

        if (flags & SETUP_NORMAL) {
            HSD_MtxInverseTranspose(temp, matrix);
            if (jobj->flags & JOBJ_LIGHTING) {
                GXLoadNrmMtxImm(matrix, matrix_number);
                HSD_PerfCountMtxLoad();
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GXLoadTexMtxImm(matrix, HSD_Index2TexMtx(matrix_index), 0);
                HSD_PerfCountMtxLoad();
            }
        }
    }
}

/* Address: 0x801AA6D0 | Size: 0xB8  -- PObj remove */
#pragma push
#pragma optimization_level 1
void PObjAmnesia(void* pobj)
{
    void* p = pobj;

    if (p == lbl_8047B2E8) {
        lbl_8047B2E8 = NULL;
    }

    if (p == (void*)lbl_8036CCD0) {
        s32 r;

        r = fn_801A6990(lbl_8047B2EC);
        if (r != 0) {
            lbl_8047B2EC = NULL;
            lbl_8047B2F4 = 0;
        }

        r = fn_801A6990(lbl_8047B2F0);
        if (r != 0) {
            lbl_8047B2F0 = NULL;
            lbl_8047B2F8 = 0;
        }

        lbl_8047B2FC = 0;
        lbl_8047B300 = NULL;
    }

    {
        void** ci = *(void***)((u8*)lbl_8036CCD0 + 0x14);
        ((void(*)(void*))ci[0x38/4])(p);
    }
}
#pragma pop

/* Address: 0x801ACD7C | Size: 0x30  -- Draw sync + clear display list state */
#pragma push
#pragma optimization_level 1
void HSD_ClearVtxDesc(void)
{
    fn_800B7D3C();
    lbl_8047B2FC = 0;
    lbl_8047B300 = NULL;
}
#pragma pop

static inline void resolveEnvelopeRefs(HSD_SList* list,
                                       HSD_EnvelopeDesc** desc_list)
{
    for (; list != NULL && *desc_list != NULL;
         list = list->next, desc_list++)
    {
        HSD_Envelope* envelope = list->data;
        HSD_EnvelopeDesc* desc = *desc_list;

        while (envelope != NULL && desc->joint != NULL) {
            HSD_JObjUnrefThis(envelope->jobj);
            envelope->jobj = HSD_IDGetDataFromTable(
                NULL, (u32) desc->joint, NULL);
            if (envelope->jobj == NULL) {
                __assert(&lbl_8047DCB8, 0x2E0, &lbl_8047DD10);
            }
            HSD_JObjRefThis(envelope->jobj);
            envelope = envelope->next;
            desc++;
        }
    }
}

static inline void resolvePObjRefs(HSD_PObj* pobj, HSD_PObjDesc* desc)
{
    if (pobj == NULL || desc == NULL) {
        return;
    }
    switch (pobj->flags & 0x3000) {
    case 0x2000:
        resolveEnvelopeRefs(pobj->u.envelope_list, desc->u.envelope_p);
        break;
    case 0:
        HSD_JObjUnrefThis(pobj->u.jobj);
        pobj->u.jobj = NULL;
        if (desc->u.joint != NULL) {
            pobj->u.jobj = HSD_IDGetDataFromTable(
                NULL, (u32) desc->u.joint, NULL);
            if (pobj->u.jobj == NULL) {
                __assert(&lbl_8047DCB8, 0x2FB, &lbl_8047DD10);
            }
            HSD_JObjRefThis(pobj->u.jobj);
        }
        break;
    }
}

void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObjDesc* desc)
{
    for (; pobj != NULL && desc != NULL;
         pobj = pobj->next, desc = desc->next)
    {
        resolvePObjRefs(pobj, desc);
    }
}

/* Address: 0x801AD214 | Size: 0x74  -- Walk pobj list, vtable[0x30] + [0x34] */
#pragma push
#pragma peephole off
void HSD_PObjRemoveAll(HSD_PObj* pobj)
{
    HSD_PObj* next;
    HSD_PObj* cur = pobj;

    while (cur != NULL) {
        next = cur->next;
        if (cur != NULL) {
            void** vtbl = *(void***)cur;
            ((void(*)(void*))vtbl[0x30 / 4])(cur);
            vtbl = *(void***)cur;
            ((void(*)(void*))vtbl[0x34 / 4])(cur);
        }
        cur = next;
    }
}
#pragma pop

/* Address: 0x801AD288 | Size: 0xCC  -- Load PObj descriptor */
HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* desc)
{
    HSD_ClassInfo* info;
    HSD_PObj* pobj;

    if (desc == NULL) {
        goto return_null;
    }

    if (*(volatile u32*) &desc->class_name != 0) {
        info = fn_80193748(desc->class_name);
        if (info != NULL) {
            goto alloc_named;
        }
    }

    if (*(volatile u32*) &lbl_8047B2E8 != 0) {
        info = lbl_8047B2E8;
    } else {
        info = (HSD_ClassInfo*) lbl_8036CCD0;
    }
    pobj = fn_80193828(info);
    if (pobj == NULL) {
        __assert(&lbl_8047DCB8, 0x2A9, &lbl_8047DD10);
    }
    goto load;

alloc_named:
    pobj = fn_80193828(info);
    if (pobj == NULL) {
        __assert(&lbl_8047DCB8, 0x247, &lbl_8047DD10);
    }

load:
    HSD_POBJ_METHOD(pobj)->load(pobj, desc);
    return pobj;

return_null:
    return NULL;
}

static inline HSD_Envelope* HSD_EnvelopeAlloc(void)
{
    HSD_Envelope* envelope = fn_80193B10(sizeof(HSD_Envelope));

    if (envelope == NULL) {
        __assert(&lbl_8047DCB8, 0x1A9, (char*) lbl_80274EE0 + 0x38);
    }
    memset(envelope, 0, sizeof(HSD_Envelope));
    return envelope;
}

static inline HSD_SList* loadEnvelopeDesc(HSD_EnvelopeDesc** desc_list)
{
    HSD_SList* list = NULL;
    HSD_SList** list_ptr = &list;

    if (desc_list == NULL) {
        return NULL;
    }

    while (*desc_list != NULL) {
        HSD_Envelope* envelope = NULL;
        HSD_Envelope** envelope_ptr = &envelope;
        HSD_EnvelopeDesc* desc = *desc_list;

        while (desc->joint != NULL) {
            *envelope_ptr = HSD_EnvelopeAlloc();
            (*envelope_ptr)->weight = desc->weight;
            envelope_ptr = &(*envelope_ptr)->next;
            desc++;
        }

        *list_ptr = fn_801A3F48();
        (*list_ptr)->data = envelope;
        list_ptr = &(*list_ptr)->next;
        desc_list++;
    }
    return list;
}

static inline HSD_ShapeSet* loadShapeSetDesc(HSD_ShapeSetDesc* desc)
{
    s32 i;
    HSD_ShapeSet* shape_set = fn_80193B10(sizeof(HSD_ShapeSet));

    if (shape_set == NULL) {
        __assert(&lbl_8047DCB8, 0x1E2, (char*) lbl_80274EE0 + 0x264);
    }
    memset(shape_set, 0, sizeof(HSD_ShapeSet));
    shape_set->flags = desc->flags;
    shape_set->nb_shape = desc->nb_shape;
    shape_set->nb_vertex_index = desc->nb_vertex_index;
    shape_set->vertex_desc = desc->vertex_desc;
    shape_set->vertex_idx_list = desc->vertex_idx_list;
    shape_set->nb_normal_index = desc->nb_normal_index;
    shape_set->normal_desc = desc->normal_desc;
    shape_set->normal_idx_list = desc->normal_idx_list;
    if (shape_set->flags & 2) {
        shape_set->blend.bp =
            fn_801A6928(shape_set->nb_shape * sizeof(f32));
        for (i = 0; i < shape_set->nb_shape; i++) {
            shape_set->blend.bp[i] = 0.0f;
        }
    } else {
        shape_set->blend.bl = 0.0f;
    }
    shape_set->aobj = NULL;
    return shape_set;
}

s32 PObjLoad(HSD_PObj* pobj, HSD_PObjDesc* desc)
{
    pobj->next = HSD_PObjLoadDesc(desc->next);
    pobj->verts = desc->verts;
    pobj->flags = desc->flags;
    pobj->n_display = desc->n_display;
    pobj->display = desc->display;

    switch (pobj->flags & 0x3000) {
    case 0x1000:
        pobj->u.shape_set = loadShapeSetDesc(desc->u.shape_set);
        break;
    case 0x2000:
        pobj->u.envelope_list = loadEnvelopeDesc(desc->u.envelope_p);
        break;
    case 0:
        break;
    default:
        HSD_Panic(&lbl_8047DCB8, 0x22A, (char*) lbl_80274EE0 + 0x270);
    }

    fn_8019C6EC(1);
    return 0;
}

/* Address: 0x801AD61C | Size: 0x5C  -- Walk pobj list, call reqAnim */
void HSD_PObjAnimAll(HSD_PObj* pobj)
{
    HSD_PObj* cur;

    if (pobj == NULL) {
        return;
    }

    cur = pobj;
    while (cur != NULL) {
        if (cur != NULL) {
            void** vtbl = *(void***)cur;
            fn_801C27F4(*(void**)((u8*)cur + 0x18), cur, vtbl[0x48 / 4]);
        }
        cur = cur->next;
    }
}

/* Address: 0x801AD678 | Size: 0x4C  -- Set shape blend weight */
void PObjUpdateFunc(HSD_PObj* pobj, s32 idx, f32* weight_ptr)
{
    HSD_PObj* p = pobj;
    HSD_ShapeSet* shapeset;

    if (p == NULL) return;

    if ((p->flags & 0x3000) != 0x1000) return;

    shapeset = p->u.shape_set;

    if (shapeset->flags & 0x2) {
        shapeset->blend.bp[idx - 2] = *weight_ptr;
    } else {
        shapeset->blend.bl = *weight_ptr;
    }
}

/* Address: 0x801AD6C4 | Size: 0x74  -- Request PObj animation by flags */
#pragma push
#pragma optimization_level 1
void HSD_PObjReqAnimAllByFlags(f32 val, HSD_PObj* pobj, u32 flags)
{
    HSD_PObj* cur;

    if (pobj == NULL) {
        return;
    }

    cur = pobj;
    while (cur != NULL) {
        if (cur != NULL) {
            if (flags & 0x8) {
                HSD_AObjReqAnim(*(void**)((u8*)cur + 0x18), val);
            }
        }
        cur = cur->next;
    }
}
#pragma pop

/* Address: 0x801AD738 | Size: 0x94  -- Add shape animation to PObj list */
void fn_801AD738(HSD_PObj* pobj, HSD_ShapeAnim* anim)
{
    HSD_PObj* p;
    HSD_ShapeAnim* a;

    if (pobj == NULL || anim == NULL) {
        return;
    }

    p = pobj;
    a = anim;
    while (p != NULL) {
        if (p != NULL) {
            if (*(HSD_AObj* volatile*) ((u8*) p + 0x18) != NULL) {
                HSD_AObjRemove(*(HSD_AObj**) ((u8*) p + 0x18));
            }
            *(HSD_AObj**) ((u8*) p + 0x18) =
                HSD_AObjLoadDesc(a->aobjdesc);
        }
        p = p->next;
        if (a != NULL) {
            a = a->next;
        } else {
            a = NULL;
        }
    }
}

/* Address: 0x801AB538 | Size: 0xC0  -- Get matrix-mark pair */
#pragma push
#pragma optimization_level 1
void HSD_PObjGetMtxMark(s32 index, u32* first, u32* second)
{
    extern void __assert(const char*, s32, const char*);
    extern char lbl_8047DCB8;
    extern char lbl_8047DCE0;
    extern char lbl_8047DCE4;
    extern u32 lbl_80465678[];

    if (first == NULL) {
        __assert(&lbl_8047DCB8, 0x663, &lbl_8047DCE0);
    }
    if (second == NULL) {
        __assert(&lbl_8047DCB8, 0x664, &lbl_8047DCE4);
    }

    if (index < 0 || index >= 2) {
        *first = 0;
        *second = 0;
    } else {
        *first = lbl_80465678[2 * index];
        *second = lbl_80465678[2 * index + 1];
    }
}
#pragma pop

/* Address: 0x801AB5F8 | Size: 0x44  -- Set one mtx-mark slot */
#pragma push
#pragma optimization_level 1
void fn_801AB5F8(s32 index, void* ptr, s32 value)
{
    extern u8 lbl_80465678[];
    s32 i = index;

    if (i >= 2) {
        return;
    }
    if (i < 0) {
        goto store;
    }
    if (i < 2) {
        return;
    }

store:
    *(void**) ((u8*) lbl_80465678 + (u32) i * 8) = ptr;
    *(s32*) ((u8*) lbl_80465678 + (u32) i * 8 + 4) = value;
}
#pragma pop

/* Address: 0x801AB63C | Size: 0x40  -- Set both mtx-mark slots */
#pragma push
#pragma optimization_level 1
void fn_801AB63C(u32 first, u32 second)
{
    extern u8 lbl_80465678[];
    s32 i;

    for (i = 0; i < 2; i++) {
        ((u32*) lbl_80465678)[2 * i] = first;
        ((u32*) lbl_80465678)[2 * i + 1] = second;
    }
}
#pragma pop

#define POBJ_MIN(x, y) ((x) < (y) ? (x) : (y))
#define POBJ_MAX(x, y) ((x) > (y) ? (x) : (y))

void drawShapeAnim(HSD_PObj* pobj)
{
    HSD_ShapeSet* shape_set = pobj->u.shape_set;
    f32 (*vertex_buffer)[3];
    f32 (*normal_buffer)[3];
    f32 blend;
    s32 shape_id;
    s32 i;
    s32 blend_nbt;

    if (lbl_8047B2F4 == 0) {
        lbl_8047B2F4 = 2000;
        lbl_8047B2EC = fn_801A6928(lbl_8047B2F4 * sizeof(f32[3]));
    }
    if (lbl_8047B2F4 < (u32) shape_set->nb_vertex_index) {
        __assert(&lbl_8047DCB8, 0x56B, (char*) lbl_80274EE0 + 0x64);
    }
    if (shape_set->normal_desc != NULL && lbl_8047B2F8 == 0) {
        lbl_8047B2F8 = 2000;
        lbl_8047B2F0 = fn_801A6928(lbl_8047B2F8 * sizeof(f32[3]));
    }

    if (shape_set->normal_desc != NULL) {
        if (shape_set->normal_desc->attr == 10) {
            if (lbl_8047B2F8 < (u32) shape_set->nb_normal_index) {
                __assert(&lbl_8047DCB8, 0x574,
                         (char*) lbl_80274EE0 + 0x98);
            }
            blend_nbt = 0;
        } else {
            if (lbl_8047B2F8 < (u32) shape_set->nb_normal_index * 3) {
                __assert(&lbl_8047DCB8, 0x577,
                         (char*) lbl_80274EE0 + 0xCC);
            }
            blend_nbt = 1;
        }
    }

    vertex_buffer = lbl_8047B2EC;
    normal_buffer = lbl_8047B2F0;
    if (shape_set->flags & 1) {
        blend = shape_set->blend.bl;
        shape_id = POBJ_MIN(POBJ_MAX(0, (s32) blend),
                            shape_set->nb_shape - 1);
        blend = POBJ_MIN(POBJ_MAX(0.0f, blend - (f32) shape_id), 1.0f);
        for (i = 0; i < shape_set->nb_vertex_index; i++) {
            f32 shape0[3];
            f32 shape1[3];

            get_shape_vertex_xyz(shape_set, shape_id, i, shape0);
            get_shape_vertex_xyz(
                shape_set,
                POBJ_MIN(shape_id + 1, shape_set->nb_shape - 1),
                i, shape1);
            vertex_buffer[i][0] =
                (shape1[0] - shape0[0]) * blend + shape0[0];
            vertex_buffer[i][1] =
                (shape1[1] - shape0[1]) * blend + shape0[1];
            vertex_buffer[i][2] =
                (shape1[2] - shape0[2]) * blend + shape0[2];
        }
        if (shape_set->nb_normal_index != 0) {
            if (blend_nbt != 0) {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 shape0[9];
                    f32 shape1[9];
                    s32 j;
                    s32 idx = i * 3;

                    fn_801AC1F8(shape_set, shape_id, i, shape0);
                    fn_801AC1F8(
                        shape_set,
                        POBJ_MIN(shape_id + 1, shape_set->nb_shape - 1),
                        i, shape1);
                    for (j = 0; j < 9; j++) {
                        normal_buffer[idx][j] =
                            (shape1[j] - shape0[j]) * blend + shape0[j];
                    }
                }
            } else {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 shape0[3];
                    f32 shape1[3];

                    get_shape_normal_xyz(shape_set, shape_id, i, shape0);
                    get_shape_normal_xyz(
                        shape_set,
                        POBJ_MIN(shape_id + 1, shape_set->nb_shape - 1),
                        i, shape1);
                    normal_buffer[i][0] =
                        (shape1[0] - shape0[0]) * blend + shape0[0];
                    normal_buffer[i][1] =
                        (shape1[1] - shape0[1]) * blend + shape0[1];
                    normal_buffer[i][2] =
                        (shape1[2] - shape0[2]) * blend + shape0[2];
                }
            }
        }
    } else {
        s32 j;
        f32* weights = shape_set->blend.bp;

        for (i = 0; i < shape_set->nb_vertex_index; i++) {
            get_shape_vertex_xyz(shape_set, 0, i, vertex_buffer[i]);
            for (j = 0; j < shape_set->nb_shape; j++) {
                f32 weight = POBJ_MAX(0.0f, weights[j]);
                f32 shape[3];

                get_shape_vertex_xyz(shape_set, j + 1, i, shape);
                vertex_buffer[i][0] += shape[0] * weight;
                vertex_buffer[i][1] += shape[1] * weight;
                vertex_buffer[i][2] += shape[2] * weight;
            }
        }
        if (shape_set->nb_normal_index != 0) {
            if (blend_nbt != 0) {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    s32 idx = i * 3;

                    fn_801AC1F8(shape_set, 0, i, normal_buffer[idx]);
                    for (j = 0; j < shape_set->nb_shape; j++) {
                        f32 weight = POBJ_MAX(0.0f, weights[j]);
                        f32 shape[9];
                        s32 k;

                        fn_801AC1F8(shape_set, j + 1, i, shape);
                        for (k = 0; k < 9; k++) {
                            normal_buffer[idx][k] += shape[k] * weight;
                        }
                    }
                }
            } else {
                for (i = 0; i < shape_set->nb_normal_index; i++) {
                    get_shape_normal_xyz(shape_set, 0, i, normal_buffer[i]);
                    for (j = 0; j < shape_set->nb_shape; j++) {
                        f32 weight = POBJ_MAX(0.0f, weights[j]);
                        f32 shape[3];

                        get_shape_normal_xyz(shape_set, j + 1, i, shape);
                        normal_buffer[i][0] += shape[0] * weight;
                        normal_buffer[i][1] += shape[1] * weight;
                        normal_buffer[i][2] += shape[2] * weight;
                    }
                }
            }
        }
    }
    fn_801ABDD4(pobj, vertex_buffer, normal_buffer);
}

/* Shape-anim source decoders. The retail range keeps the sysdolphin
 * pobj.c bodies (assert/panic lines 1145/1188 and 1082/1125), so the
 * component fetch below mirrors that source one-for-one. */

/* Address: 0x801AC4BC | Size: 0x460  -- fetch one shape-anim normal */
void get_shape_normal_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3])
{
    extern void __assert(const char*, s32, const char*);
    extern void HSD_Panic(const char*, s32, const char*);
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern char lbl_8047DCB8;

    u8* index_array = shape_set->normal_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if ((s32) shape_set->normal_desc->attr_type == 3) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    if ((s32) shape_set->normal_desc->comp_cnt != 0) {
        __assert(&lbl_8047DCB8, 1145,
                 "shape_set->normal_desc->comp_cnt == GX_NRM_XYZ");
    }

    src_base = ((u8*) shape_set->normal_desc->vertex) +
               idx * shape_set->normal_desc->stride;

    if ((s32) shape_set->normal_desc->comp_type == 4) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->normal_desc->frac;
        switch ((s32) shape_set->normal_desc->comp_type) {
        case 0: {
            u8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 1: {
            s8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 2: {
            u16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 3: {
            s16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        default:
            HSD_Panic(&lbl_8047DCB8, 1188, "unexpected normal type.");
        }
    }
}

/* Address: 0x801AC91C | Size: 0x460  -- fetch one shape-anim vertex */
void get_shape_vertex_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3])
{
    extern void __assert(const char*, s32, const char*);
    extern void HSD_Panic(const char*, s32, const char*);
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern char lbl_8047DCB8;

    u8* index_array = shape_set->vertex_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if ((s32) shape_set->vertex_desc->attr_type == 3) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    if ((s32) shape_set->vertex_desc->comp_cnt != 1) {
        __assert(&lbl_8047DCB8, 1082,
                 "shape_set->vertex_desc->comp_cnt == GX_POS_XYZ");
    }

    src_base = ((u8*) shape_set->vertex_desc->vertex) +
               idx * shape_set->vertex_desc->stride;

    if ((s32) shape_set->vertex_desc->comp_type == 4) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->vertex_desc->frac;
        switch ((s32) shape_set->vertex_desc->comp_type) {
        case 0: {
            u8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 1: {
            s8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 2: {
            u16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 3: {
            s16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        default:
            HSD_Panic(&lbl_8047DCB8, 1125, "unexpected vertex type.\n");
        }
    }
}

typedef struct EulerVec {
    f32 x;
    f32 y;
    f32 z;
} EulerVec;

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

extern const f32 lbl_8047DD1C;
extern double cos(double x);
extern double sin(double x);

s32 fn_801ADAAC(EulerVec* euler, Quaternion* q)
{
    f32 cx;
    f32 cy;
    f32 cz;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 cc;
    f32 ss;

    cx = cos(lbl_8047DD1C * euler->x);
    cy = cos(lbl_8047DD1C * euler->y);
    cz = cos(lbl_8047DD1C * euler->z);
    sx = sin(lbl_8047DD1C * euler->x);
    sy = sin(lbl_8047DD1C * euler->y);
    sz = sin(lbl_8047DD1C * euler->z);

    ss = sy * sz;
    cc = cy * cz;
    q->w = cx * cc + sx * ss;
    q->x = sx * cc - cx * ss;
    q->y = cz * (cx * sy) + sz * (sx * cy);
    q->z = sz * (cx * cy) - cz * (sx * sy);

    return 1;
}

/* Address: 0x801ADC08 | Size: 0x34  -- Forget RNG memory state */
void _HSD_RandForgetMemory(void)
{
    s32 r = fn_801A6990(lbl_80478C94);
    if (r != 0) {
        lbl_80478C94 = &lbl_80478C90;
    }
}

/* Address: 0x801ADC3C | Size: 0x40  -- Bounded RNG */
#pragma push
#pragma peephole off
s32 fn_801ADC3C(s32 max)
{
    s32* state;
    volatile s32* state_v;
    s32 rand;

    state = (s32*) *(void* volatile*) &lbl_80478C94;
    state_v = (s32*) *(void* volatile*) &lbl_80478C94;
    rand = (*state * 0x343fd) + 0x269EC3;
    *state_v = rand;

    return (s32)(((s32)max * (s32)(*(u32*) *(void* volatile*) &lbl_80478C94 >> 16)) / 0x10000);
}
#pragma pop

/* Address: 0x801ADC7C | Size: 0x5C  -- LCG next, normalized float return */
#pragma push
#pragma peephole off
f32 fn_801ADC7C(void)
{
    extern const f32 lbl_8047DD40;
    u32* state;
    volatile u32* state_v;
    u32 next;

    state = (u32*) lbl_80478C94;
    state_v = (u32*) lbl_80478C94;
    next = (*state * 0x343FD) + 0x269EC3;
    *state_v = next;

    return (f32) (*(u32*) lbl_80478C94 >> 16) / lbl_8047DD40;
}
#pragma pop

/* Address: 0x801ADCD8 | Size: 0x34  -- LCG next, u16 return */
#pragma push
#pragma peephole off
u32 fn_801ADCD8(void)
{
    u32* state;
    volatile u32* state_v;
    u32 next;

    state = (u32*) *(void* volatile*) &lbl_80478C94;
    state_v = (u32*) *(void* volatile*) &lbl_80478C94;
    next = (*state * 0x343fd) + 0x269ec3;
    *state_v = next;

    return *(u32*) *(void* volatile*) &lbl_80478C94 >> 16;
}
#pragma pop

/* Address: 0x801ADD0C | Size: 0x3C  -- Deactivate texture anim state */
#pragma push
#pragma optimization_level 1
void _HSD_RObjForgetMemory(void)
{
    s32 r = fn_801A6990(lbl_8047B308);
    if (r != 0) {
        lbl_8047B308 = NULL;
        lbl_8047B30C = 0;
    }
}
#pragma pop
