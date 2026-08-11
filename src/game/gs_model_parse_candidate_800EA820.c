/** Candidate-only owner for 0x800EA820 - 0x800EB268. */
#include "src/game/gs_model_parse.c"
#include "hsd/hsd_dobj.h"

extern f32 lbl_804016A0[12];
extern void fn_800E064C(f32* matrix);
extern void fn_8019D9DC(HSD_JObj*);
extern void fn_80197B6C(HSD_JObj*, f32* obj_mtx, f32* out);
extern void fn_800EA960(HSD_JObj*, f32* obj_mtx, f32* vmtx,
                       u32 pass, BOOL is_visible, GSModelPObjDisp disp,
                       void* arg);
extern f32 lbl_804016D0[24];
extern const f32 lbl_8047CC18;
extern const f32 lbl_8047CC1C;

void _modelParseJObjDispDObj__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask,
    BOOL is_visible, GSModelPObjDisp disp, void* arg)
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
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D9DC(jobj);
    }
    if (obj_mtx == NULL) {
        fn_800E064C(lbl_804016A0);
        obj_mtx = lbl_804016A0;
    }
    fn_80197B6C(jobj, obj_mtx, vmtx);
    if (passes & 0x00040000) {
        fn_800EA960(jobj, obj_mtx, vmtx, 1, is_visible, disp, arg);
    }
    if (passes & 0x00100000) {
        fn_800EA960(jobj, obj_mtx, vmtx, 4, is_visible, disp, arg);
    }
    if (passes & 0x00080000) {
        fn_800EA960(jobj, obj_mtx, vmtx, 2, is_visible, disp, arg);
    }
}

void _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_DObj* dobj, f32* vmtx, f32* pmtx, BOOL is_visible,
    GSModelPObjDisp disp, void* arg)
{
    extern HSD_JObj* HSD_JObjGetCurrent(void);
    extern void HSD_PObjGetMtxMark(s32 index, u32* object, u32* mark);
    extern void fn_801AB5F8(s32 index, void* object, s32 mark);
    extern void PSMTXConcat(f32* left, f32* right, f32* out);
    extern void fn_800E0628(void* dst, void* src);
    extern void _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
        HSD_PObj*, f32*, f32*, f32*);
    HSD_PObj* pobj;
    HSD_JObj* current;
    u32 markedObject;
    u32 mark;
    u8 setupCurrent;
    u8 setupJoint;
    f32 matrix[12];

    for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
        if (pobj->flags & 0x800) {
            continue;
        }

        if (is_visible) {
            switch (pobj->flags & 0x3000) {
            case 0:
                if (pobj->u.jobj == NULL) {
                    current = HSD_JObjGetCurrent();
                    HSD_PObjGetMtxMark(0, &markedObject, &mark);
                    if (markedObject != (u32)current || mark != 1) {
                        fn_801AB5F8(0, current, 1);
                        fn_800E0628(lbl_804016D0, pmtx);
                    }
                } else {
                    current = HSD_JObjGetCurrent();
                    setupCurrent = setupJoint = FALSE;
                    HSD_PObjGetMtxMark(0, &markedObject, &mark);
                    if (markedObject != (u32)current && mark != 1) {
                        setupCurrent = TRUE;
                    }
                    fn_801AB5F8(0, current, 1);
                    HSD_PObjGetMtxMark(1, &markedObject, &mark);
                    if (markedObject != (u32)pobj->u.jobj && mark != 1) {
                        setupJoint = TRUE;
                    }
                    fn_801AB5F8(1, pobj->u.jobj, 1);
                    if (setupCurrent || setupJoint) {
                        if (setupCurrent) {
                            fn_800E0628(lbl_804016D0, pmtx);
                        }
                        if (setupJoint) {
                            if (pobj->u.jobj != NULL &&
                                HSD_JObjMtxIsDirty(pobj->u.jobj)) {
                                fn_8019D9DC(pobj->u.jobj);
                            }
                            PSMTXConcat(vmtx,
                                        (f32*)((u8*)pobj->u.jobj + 0x44),
                                        matrix);
                            fn_800E0628(lbl_804016D0 + 12, matrix);
                        }
                    }
                }
                break;
            case 0x1000:
                current = HSD_JObjGetCurrent();
                HSD_PObjGetMtxMark(0, &markedObject, &mark);
                if (markedObject != (u32)current || mark != 1) {
                    fn_801AB5F8(0, current, 1);
                    fn_800E0628(lbl_804016D0, pmtx);
                }
                break;
            case 0x2000:
                _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
                    pobj, vmtx, pmtx, lbl_804016D0);
                break;
            }
        }

        if ((pobj->flags & 0x3000) == 0x1000) {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, NULL, arg);
        } else if (disp != NULL) {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx,
                 is_visible ? (f32(*)[4])lbl_804016D0 : NULL, arg);
        }
    }
}

void _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* vmtx, f32* pmtx, HSD_TrspMask pass,
    BOOL is_visible, GSModelPObjDisp disp, void* arg)
{
    extern void fn_8019F024(HSD_JObj* jobj);
    extern void fn_801AB63C(u32 first, u32 second);
    extern void HSD_DObjSetCurrent(HSD_DObj* dobj);
    HSD_DObj* dobj;

    fn_8019F024(jobj);
    fn_801AB63C(0, 0);
    for (dobj = *(HSD_DObj**)((u8*)jobj + 0x18);
         dobj != NULL; dobj = dobj->next) {
        if ((dobj->flags & 1) == 0 &&
            (dobj->flags & ((u32)pass << 1)) != 0) {
            HSD_DObjSetCurrent(dobj);
            _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                dobj, vmtx, pmtx, is_visible, disp, arg);
        }
    }
    HSD_DObjSetCurrent(NULL);
    fn_8019F024(NULL);
}

void _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
    HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* matrices)
{
    extern HSD_JObj* HSD_JObjGetCurrent(void);
    extern void fn_801AB63C(u32 first, u32 second);
    extern f32* _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* jobj, f32* matrix);
    extern void PSMTXConcat(f32* left, f32* right, f32* out);
    extern void HSD_MtxScaledAdd(f32* src, f32 scale, f32* add, f32* out);
    extern void fn_800E0628(void* dst, void* src);
    extern const char lbl_80270EB8[];
    extern const char lbl_8047CC10[];
    HSD_JObj* current;
    HSD_SList* list;
    HSD_Envelope* envelope;
    HSD_JObj* jobj;
    f32 nodeMatrix[12];
    f32 matrix[12];
    f32 temp[12];
    f32* node;
    f32* source;
    s32 index;

    (void)pmtx;
    current = HSD_JObjGetCurrent();
    fn_801AB63C(0, 2);
    node = _HSD_mkEnvelopeModelNodeMtx(current, nodeMatrix);

    list = pobj->u.envelope_list;
    for (index = 0; index < 10 && list != NULL;
         index++, list = list->next, matrices += 12) {
        envelope = list->data;
        if (envelope == NULL) {
            __assert(lbl_8047CC10, 0x65, lbl_80270EB8);
        }

        if (envelope->weight >= lbl_8047CC18) {
            jobj = envelope->jobj;
            if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
                fn_8019D9DC(jobj);
            }
            if (node != NULL) {
                PSMTXConcat((f32*)((u8*)jobj + 0x44),
                            *(f32**)((u8*)jobj + 0x78), matrix);
                source = matrix;
            } else {
                source = (f32*)((u8*)jobj + 0x44);
            }
        } else {
            matrix[0] = matrix[1] = matrix[2] = matrix[3] =
                matrix[4] = matrix[5] = matrix[6] = matrix[7] =
                matrix[8] = matrix[9] = matrix[10] = matrix[11] =
                    lbl_8047CC1C;
            while (envelope != NULL) {
                jobj = envelope->jobj;
                if (jobj == NULL) {
                    __assert(lbl_8047CC10, 0x7E, lbl_80270EB8 + 0xC);
                }
                if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
                    fn_8019D9DC(jobj);
                }
                if ((f32*)((u8*)jobj + 0x44) == NULL) {
                    __assert(lbl_8047CC10, 0x81, lbl_80270EB8 + 0x1C);
                }
                if (*(f32**)((u8*)jobj + 0x78) == NULL) {
                    __assert(lbl_8047CC10, 0x82, lbl_80270EB8 + 0x1C);
                }
                PSMTXConcat((f32*)((u8*)jobj + 0x44),
                            *(f32**)((u8*)jobj + 0x78), temp);
                HSD_MtxScaledAdd(temp, envelope->weight, matrix, matrix);
                envelope = envelope->next;
            }
            source = matrix;
        }

        if (node != NULL) {
            PSMTXConcat(source, node, matrix);
            source = matrix;
        }
        PSMTXConcat(vmtx, source, temp);
        fn_800E0628(matrices, temp);
    }
}
