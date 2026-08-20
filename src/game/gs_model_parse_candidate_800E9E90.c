/** Candidate-only owner for 0x800E9E90 - 0x800EA60C. */
#include "src/game/gs_model_parse.c"

extern void _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
    f32 parent[3][4], HSD_JObj* jobj, f32 dst[3][4]);
extern void PSMTXInverse(f32 src[3][4], f32 dst[3][4]);
extern void PSMTXConcat(f32 left[3][4], f32 right[3][4], f32 dst[3][4]);

#define MODEL_JOBJ_HIDDEN   (1 << 4)
#define MODEL_JOBJ_INSTANCE (1 << 12)

void _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask, BOOL is_visible,
    GSModelPObjDisp disp, void* arg)
{
    HSD_JObj* child;
    HSD_JObj* instance;
    HSD_JObj* grandchild;
    f32 mtx0[3][4];
    f32 mtx1[3][4];
    f32 mtx2[3][4];
    f32 mtx3[3][4];

    if (jobj == NULL) {
        return;
    }

    if (jobj->flags & MODEL_JOBJ_INSTANCE) {
        if (jobj->flags & MODEL_JOBJ_HIDDEN) {
            return;
        }

        HSD_JObjSetupMatrix_800EA664(jobj);
        HSD_JObjSetupMatrix_800EA664(jobj->child);
        PSMTXInverse(jobj->child->matrix, mtx0);
        PSMTXConcat(jobj->matrix, mtx0, mtx0);
        if (obj_mtx != NULL) {
            PSMTXConcat((f32(*)[4])obj_mtx, mtx0, mtx0);
        }

        child = jobj->child;
        if (child == NULL) {
            return;
        }

        if (child->flags & MODEL_JOBJ_INSTANCE) {
            if (child->flags & MODEL_JOBJ_HIDDEN) {
                return;
            }

            HSD_JObjSetupMatrix_800EA664(child);
            HSD_JObjSetupMatrix_800EA664(child->child);
            PSMTXInverse(child->child->matrix, mtx1);
            PSMTXConcat(child->matrix, mtx1, mtx1);
            PSMTXConcat(mtx0, mtx1, mtx1);

            instance = child->child;
            if (instance == NULL) {
                return;
            }
            if (instance->flags & MODEL_JOBJ_INSTANCE) {
                if (instance->flags & MODEL_JOBJ_HIDDEN) {
                    return;
                }
                _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
                    mtx1, instance, mtx2);
                _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                    instance->child, (f32*)mtx2, trsp_mask, is_visible,
                    disp, arg);
                return;
            }

            if (instance->flags & ((u32)trsp_mask << 18)) {
                _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                    instance, (f32*)mtx1, trsp_mask, is_visible, disp, arg);
            }
            if (instance->flags & ((u32)trsp_mask << 28)) {
                grandchild = instance->child;
                while (grandchild != NULL) {
                    _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                        grandchild, (f32*)mtx1, trsp_mask, is_visible,
                        disp, arg);
                    grandchild = grandchild->next;
                }
            }
            return;
        }

        if (child->flags & ((u32)trsp_mask << 18)) {
            _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                child, (f32*)mtx0, trsp_mask, is_visible, disp, arg);
        }
        if (child->flags & ((u32)trsp_mask << 28)) {
            child = child->child;
            while (child != NULL) {
                instance = child;
                if (instance != NULL) {
                    if (instance->flags & MODEL_JOBJ_INSTANCE) {
                        if (!(instance->flags & MODEL_JOBJ_HIDDEN)) {
                            _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
                                mtx0, instance, mtx2);
                            _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                instance->child, (f32*)mtx2, trsp_mask,
                                is_visible, disp, arg);
                        }
                    } else {
                        if (instance->flags & ((u32)trsp_mask << 18)) {
                            _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                                instance, (f32*)mtx0, trsp_mask, is_visible,
                                disp, arg);
                        }
                        if (instance->flags & ((u32)trsp_mask << 28)) {
                            grandchild = instance->child;
                            while (grandchild != NULL) {
                                _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                    grandchild, (f32*)mtx0, trsp_mask,
                                    is_visible, disp, arg);
                                grandchild = grandchild->next;
                            }
                        }
                    }
                }
                child = child->next;
            }
        }
        return;
    }

    if (jobj->flags & ((u32)trsp_mask << 18)) {
        _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
            jobj, obj_mtx, trsp_mask, is_visible, disp, arg);
    }
    if (!(jobj->flags & ((u32)trsp_mask << 28))) {
        return;
    }

    child = jobj->child;
    while (child != NULL) {
        instance = child;
        if (instance != NULL) {
            if (instance->flags & MODEL_JOBJ_INSTANCE) {
                if (!(instance->flags & MODEL_JOBJ_HIDDEN)) {
                    HSD_JObjSetupMatrix_800EA664(instance);
                    HSD_JObjSetupMatrix_800EA664(instance->child);
                    PSMTXInverse(instance->child->matrix, mtx3);
                    PSMTXConcat(instance->matrix, mtx3, mtx3);
                    if (obj_mtx != NULL) {
                        PSMTXConcat((f32(*)[4])obj_mtx, mtx3, mtx3);
                    }

                    grandchild = instance->child;
                    if (grandchild != NULL) {
                        if (grandchild->flags & MODEL_JOBJ_INSTANCE) {
                            if (!(grandchild->flags & MODEL_JOBJ_HIDDEN)) {
                                _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
                                    mtx3, grandchild, mtx2);
                                _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                    grandchild->child, (f32*)mtx2, trsp_mask,
                                    is_visible, disp, arg);
                            }
                        } else {
                            if (grandchild->flags & ((u32)trsp_mask << 18)) {
                                _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                                    grandchild, (f32*)mtx3, trsp_mask,
                                    is_visible, disp, arg);
                            }
                            if (grandchild->flags & ((u32)trsp_mask << 28)) {
                                grandchild = grandchild->child;
                                while (grandchild != NULL) {
                                    _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                        grandchild, (f32*)mtx3, trsp_mask,
                                        is_visible, disp, arg);
                                    grandchild = grandchild->next;
                                }
                            }
                        }
                    }
                }
            } else {
                if (instance->flags & ((u32)trsp_mask << 18)) {
                    _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                        instance, obj_mtx, trsp_mask, is_visible, disp, arg);
                }
                if (instance->flags & ((u32)trsp_mask << 28)) {
                    grandchild = instance->child;
                    while (grandchild != NULL) {
                        instance = grandchild;
                        if (instance != NULL) {
                            if (instance->flags & MODEL_JOBJ_INSTANCE) {
                                if (!(instance->flags & MODEL_JOBJ_HIDDEN)) {
                                    _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
                                        (f32(*)[4])obj_mtx, instance, mtx2);
                                    _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                        instance->child, (f32*)mtx2,
                                        trsp_mask, is_visible, disp, arg);
                                }
                            } else {
                                if (instance->flags & ((u32)trsp_mask << 18)) {
                                    _modelParseJObjDisp__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv_800EA7E4(
                                        instance, obj_mtx, trsp_mask,
                                        is_visible, disp, arg);
                                }
                                if (instance->flags & ((u32)trsp_mask << 28)) {
                                    HSD_JObj* scan = instance->child;
                                    while (scan != NULL) {
                                        _modelParseJObjDispAll__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
                                            scan, obj_mtx, trsp_mask,
                                            is_visible, disp, arg);
                                        scan = scan->next;
                                    }
                                }
                            }
                        }
                        grandchild = grandchild->next;
                    }
                }
            }
        }
        child = child->next;
    }
}
