/** Candidate-only owner for 0x800EA820 - 0x800EB268. */
#include "src/game/gs_model_parse.c"

extern f32 lbl_804016A0[12];
extern void fn_800E064C(f32* matrix);
extern void fn_8019D9DC(HSD_JObj*);
extern void fn_80197B6C(HSD_JObj*, f32* obj_mtx, f32* out);
extern void fn_800EA960(HSD_JObj*, f32* obj_mtx, f32* vmtx,
                       u32 pass, BOOL is_visible, GSModelPObjDisp disp,
                       void* arg);

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
