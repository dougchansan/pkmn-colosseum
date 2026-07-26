/** Candidate-only owner for 0x800EA6D4 - 0x800EA7E4. */
#include "src/game/gs_model_parse.c"

extern void fn_8019D9DC(HSD_JObj*);
extern void fn_800A2EB4(f32 src[3][4], f32 dst[3][4]);
extern void PSMTXConcat(f32 a[3][4], f32 b[3][4], f32 dst[3][4]);

void _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
    f32 parent[3][4], HSD_JObj* jobj, f32 dst[3][4])
{
    HSD_JObj* child;

    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D9DC(jobj);
    }
    child = jobj->child;
    if (child != NULL && HSD_JObjMtxIsDirty(child)) {
        fn_8019D9DC(child);
    }
    fn_800A2EB4(jobj->child->matrix, dst);
    PSMTXConcat(jobj->matrix, dst, dst);
    if (parent != NULL) {
        PSMTXConcat(parent, dst, dst);
    }
}
