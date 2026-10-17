/**
 * @file gs_model_parse.c
 * @brief GSmodel parse (XD GSmodel/parse.c): GSmodelParse down to
 *        _modelParseDObjDisp, 0x800E9E34 - 0x800EAFE4.
 *
 * The TU is built with GC/1.3.2 and -inline auto,deferred. Deferred
 * inlining emits functions in reverse source order, so the file is written
 * bottom-up, as C source is (helpers first), and comes out in retail order:
 * GSmodelParse, _modelParseJObjDispAll, HSD_JObjMtxIsDirty,
 * HSD_JObjSetupMatrix, _modelParseSetupInstanceMtx, _modelParseJObjDisp,
 * _modelParseJObjDispDObj, _modelParseJObjDispSub, _modelParseDObjDisp.
 * XD's parse.o map lists the same order (NXXJ01.map lines 6184-6199).
 *
 * Deferred mode also inlines later functions into earlier ones, and
 * recursive functions into themselves:
 * - _modelParseJObjDispSub carries its _modelParseDObjDisp.
 * - _modelParseJObjDispAll carries its own recursion two levels deep,
 *   _modelParseSetupInstanceMtx, _modelParseJObjDisp and
 *   _modelParseJObjDispDObj. It is written as the plain recursive walk.
 * The jobj.h matrix helpers are real out-of-line functions of this TU
 * (0x800EA60C, 0x800EA664); where the inlining depth runs out, retail
 * calls them.
 *
 * The per-PObj helpers are XD's stripped parse.c inlines (NXXJ01.map lines
 * 6195-6199, all UNUSED: _modelParsePObjDisp 0x334,
 * _modelParsePObjSetupMtx 0x234, _modelParseLoadSharedMatrix 0x154,
 * _modelParseLoadRigidMatrix 0x88; StarsMmd/Colo-XD-PBR-symbol-maps).
 * modelParsePObjDisp is expanded twice (in _modelParseDObjDisp and in the
 * copy inlined into _modelParseJObjDispSub). modelParseLoadRigidMatrix is
 * expanded twice per expansion.
 *
 * The TU's last function, _modelParseLoadEnvelopeMatrix (0x800EAFE4), is
 * its own unit (gs_model_parse_exact_800EAFE4.c) with the TU's .rodata
 * string pool. The TU's .sdata2 assert strings and floats stay extern.
 */
#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

typedef struct HSD_JObj {
    u8 _pad0[0x08];
    struct HSD_JObj* next;
    struct HSD_JObj* parent;
    struct HSD_JObj* child;
    u32 flags;
    u8 _pad18[0x2C];
    f32 matrix[3][4];
    u8 _pad74[0x4];
    f32 (*envelopemtx)[4];
} HSD_JObj;

#define JOBJ_PTCL   (1 << 5)
#define JOBJ_SPLINE (1 << 14)
#define union_type_dobj(o) \
    ((o)->flags & (JOBJ_PTCL | JOBJ_SPLINE) ? FALSE : TRUE)

typedef void (*GSModelPObjDisp)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                               f32 smtx[3][4], void* arg);

#include "hsd/hsd_dobj.h"

extern const u8 lbl_8047CC00[7];
extern const u8 lbl_8047CC08[5];
extern void __assert(const char* file, u32 line, const char* condition);

extern void HSD_JObjSetupMatrixSub(HSD_JObj*);
BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj);
void HSD_JObjSetupMatrix_800EA664(HSD_JObj* jobj);
extern void PSMTXInverse(f32 src[3][4], f32 dst[3][4]);
typedef struct GSmodel GSmodel;
extern void* modelGetRenderJObj(GSmodel* model);
void _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(f32* parent, HSD_JObj* jobj, f32* dst);
void _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask, u8 is_visible, GSModelPObjDisp disp, void* arg);
void _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask, u8 is_visible, GSModelPObjDisp disp, void* arg);
extern f32 lbl_804016A0[12];
extern void fn_800E064C(f32* matrix);
extern void HSD_JObjMakePositionMtx(HSD_JObj*, f32* obj_mtx, f32* out);
void _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* vmtx, f32* pmtx, HSD_TrspMask pass,
    u8 is_visible, GSModelPObjDisp disp, void* arg);
extern f32 lbl_804016D0[24];

extern void HSD_JObjSetCurrent(HSD_JObj* jobj);
extern void HSD_PObjClearMtxMark(u32 first, u32 second);
extern void HSD_DObjSetCurrent(HSD_DObj* dobj);
extern HSD_JObj* HSD_JObjGetCurrent(void);
extern void HSD_PObjGetMtxMark(s32 index, u32* object, u32* mark);
extern void HSD_PObjSetMtxMark(s32 index, void* object, s32 mark);
extern void PSMTXConcat(f32* left, f32* right, f32* out);
extern void fn_800E0628(void* dst, void* src);
void _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
    HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* matrices);

static inline void modelParseLoadRigidMatrix(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    HSD_JObj* current;
    u32 marked_object;
    u32 mark;

    current = HSD_JObjGetCurrent();
    HSD_PObjGetMtxMark(0, &marked_object, &mark);
    if (marked_object != (u32)current || mark != 1) {
        HSD_PObjSetMtxMark(0, current, 1);
        fn_800E0628(mtx, pmtx);
    }
}

static inline void modelParseLoadSharedMatrix(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    HSD_JObj* current;
    u32 marked_object;
    u32 mark;
    u8 setup_current = FALSE;
    u8 setup_joint = FALSE;
    f32 matrix[12];

    current = HSD_JObjGetCurrent();
    HSD_PObjGetMtxMark(0, &marked_object, &mark);
    if (marked_object != (u32)current && mark != 1) {
        setup_current = TRUE;
    }
    HSD_PObjSetMtxMark(0, current, 1);
    HSD_PObjGetMtxMark(1, &marked_object, &mark);
    if (marked_object != (u32)pobj->u.jobj && mark != 1) {
        setup_joint = TRUE;
    }
    HSD_PObjSetMtxMark(1, pobj->u.jobj, 1);
    if (setup_current || setup_joint) {
        if (setup_current) {
            fn_800E0628(mtx, pmtx);
        }
        if (setup_joint) {
            HSD_JObjSetupMatrix_800EA664(pobj->u.jobj);
            PSMTXConcat(vmtx, (f32*)((u8*)pobj->u.jobj + 0x44), matrix);
            fn_800E0628(mtx + 12, matrix);
        }
    }
}

static inline void modelParsePObjSetupMtx(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    switch (pobj->flags & 0x3000) {
    case 0:
        if (pobj->u.jobj == NULL) {
            modelParseLoadRigidMatrix(pobj, vmtx, pmtx, mtx);
        } else {
            modelParseLoadSharedMatrix(pobj, vmtx, pmtx, mtx);
        }
        break;
    case 0x1000:
        modelParseLoadRigidMatrix(pobj, vmtx, pmtx, mtx);
        break;
    case 0x2000:
        _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(pobj, vmtx, pmtx, mtx);
        break;
    }
}

static inline void modelParsePObjDisp(HSD_PObj* pobj, f32* vmtx, f32* pmtx, u8 is_visible,
                                      GSModelPObjDisp disp, void* arg)
{
    if (pobj->flags & 0x800) {
        return;
    }
    if (is_visible) {
        modelParsePObjSetupMtx(pobj, vmtx, pmtx, lbl_804016D0);
    }
    if ((pobj->flags & 0x3000) == 0x1000) {
        disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, NULL, arg);
    } else if (disp != NULL) {
        if (is_visible) {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, (f32(*)[4])lbl_804016D0, arg);
        } else {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, NULL, arg);
        }
    }
}

void _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_DObj* dobj, f32* vmtx, f32* pmtx, u8 is_visible,
    GSModelPObjDisp disp, void* arg)
{
    HSD_PObj* pobj;

    for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
        modelParsePObjDisp(pobj, vmtx, pmtx, is_visible, disp, arg);
    }
}

void _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* vmtx, f32* pmtx, HSD_TrspMask pass,
    u8 is_visible, GSModelPObjDisp disp, void* arg)
{
    HSD_DObj* dobj;
    u32 pass_mask;

    HSD_JObjSetCurrent(jobj);
    pass_mask = (u32)pass << 1;
    HSD_PObjClearMtxMark(0, 0);
    for (dobj = *(HSD_DObj**)((u8*)jobj + 0x18); dobj != NULL; dobj = dobj->next) {
        if ((dobj->flags & 1) == 0 && (dobj->flags & pass_mask) != 0) {
            HSD_DObjSetCurrent(dobj);
            _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(dobj, vmtx, pmtx, is_visible, disp, arg);
        }
    }
    HSD_DObjSetCurrent(NULL);
    HSD_JObjSetCurrent(NULL);
}

void _modelParseJObjDispDObj__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask,
    u8 is_visible, GSModelPObjDisp disp, void* arg)
{
    f32 vmtx[12];
    u32 passes;

    if (jobj->flags & 0x10) {
        return;
    }
    passes = jobj->flags & ((u32)trsp_mask << 18);
    if (passes == 0) {
        return;
    }
    HSD_JObjSetupMatrix_800EA664(jobj);
    if (obj_mtx == NULL) {
        fn_800E064C(lbl_804016A0);
        obj_mtx = lbl_804016A0;
    }
    HSD_JObjMakePositionMtx(jobj, obj_mtx, vmtx);
    if (passes & 0x00040000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 1, is_visible, disp, arg);
    }
    if (passes & 0x00100000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 4, is_visible, disp, arg);
    }
    if (passes & 0x00080000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 2, is_visible, disp, arg);
    }
}

void _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask, u8 is_visible,
    GSModelPObjDisp disp, void* arg)
{
    if (jobj != NULL && union_type_dobj(jobj)) {
        _modelParseJObjDispDObj__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
            jobj, obj_mtx, trsp_mask, is_visible, disp, arg);
    }
}

void _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
    f32* parent, HSD_JObj* jobj, f32* dst)
{
    HSD_JObjSetupMatrix_800EA664(jobj);
    HSD_JObjSetupMatrix_800EA664(jobj->child);
    PSMTXInverse(jobj->child->matrix, (f32(*)[4])dst);
    PSMTXConcat((f32*)jobj->matrix, dst, dst);
    if (parent) {
        PSMTXConcat(parent, dst, dst);
    }
}

void HSD_JObjSetupMatrix_800EA664(HSD_JObj* jobj)
{
    if (jobj == NULL || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub(jobj);
}

BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert((const char*)lbl_8047CC00, 0x25d, (const char*)lbl_8047CC08);
    }
    result = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

void _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask, u8 is_visible,
    GSModelPObjDisp disp, void* arg)
{
    f32 mtx[12];
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    if (jobj->flags & 0x1000) {
        if (jobj->flags & 0x10) {
            return;
        }
        _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(obj_mtx, jobj, mtx);
        _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj->child, mtx, trsp_mask, is_visible, disp, arg);
    } else {
        if (jobj->flags & ((u32)trsp_mask << 18)) {
            _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                jobj, obj_mtx, trsp_mask, is_visible, disp, arg);
        }
        if (jobj->flags & ((u32)trsp_mask << 28)) {
            for (jobj = jobj->child; jobj != NULL; jobj = jobj->next) {
                _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, trsp_mask, is_visible, disp, arg);
            }
        }
    }
}

void GSmodelParse(GSmodel* model, u8 is_visible, GSModelPObjDisp disp,
                  void* arg)
{
    _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
        modelGetRenderJObj(model), NULL, 7, is_visible, disp, arg);
}
