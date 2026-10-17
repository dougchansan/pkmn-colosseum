/**
 * @file gs_model_parse_exact_800EAFE4.c
 * @brief GSmodel parse: _modelParseLoadEnvelopeMatrix, 0x800EAFE4 - 0x800EB268.
 *
 * Loads the envelope (skinned) matrices of a PObj, after HSD pobj.c's
 * SetupEnvelopeModelMtx. XD's parse.o has the same function at the same
 * size (GXXE01.map: 0x800FC624, 0x284) with the same register allocation.
 * Built like the parse TU (GC/1.3.2, strings in .rodata/.sdata2). The unit
 * owns the TU's .rodata string pool (0x80270EB8 - 0x80270EE8: "envelope",
 * "envelope->jobj", "jp->envelopemtx"); written as literals they give
 * retail's base-plus-offset addressing. The short .sdata2 strings and the
 * floats stay extern, as in gs_model_parse.c.
 *
 * HSD_JObjMtxIsDirty returns its condition as one expression. The
 * "result = FALSE; if (...) result = TRUE;" form gives the same
 * instructions, but its result local is a frontend temporary numbered
 * before the string-pool base, so the base is coloured last (r25) instead
 * of first (r31).
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

typedef void (*GSModelPObjDisp)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                               f32 smtx[3][4], void* arg);

#include "hsd/hsd_dobj.h"

extern const u8 lbl_8047CC00[7];
extern const u8 lbl_8047CC08[5];
extern void __assert(const char* file, u32 line, const char* condition);

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert((const char*)lbl_8047CC00, 0x25d, (const char*)lbl_8047CC08);
    }
    return !(jobj->flags & 0x00800000) && (jobj->flags & 0x40);
}

extern void HSD_JObjSetupMatrixSub(HSD_JObj*);
static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub(jobj);
}

extern const f32 lbl_8047CC18;
extern const f32 lbl_8047CC1C;
extern void HSD_PObjClearMtxMark(u32 first, u32 second);
extern HSD_JObj* HSD_JObjGetCurrent(void);
extern void PSMTXConcat(f32* left, f32* right, f32* out);
extern void fn_800E0628(void* dst, void* src);

void _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
    HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    extern f32* _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* jobj, f32* matrix);
    extern void HSD_MtxScaledAdd(f32* src, f32 scale, f32* add, f32* out);
    extern const char lbl_8047CC10[8];
    extern const char lbl_8047CC20[8];
    HSD_JObj* jobj;
    HSD_SList* list;
    s32 idx;
    f32* right;
    f32 nmtx[12];

    jobj = HSD_JObjGetCurrent();
    HSD_PObjClearMtxMark(0, 2);
    right = _HSD_mkEnvelopeModelNodeMtx(jobj, nmtx);

    for (idx = 0, list = pobj->u.envelope_list; idx < 10 && list != NULL;
         idx++, list = list->next) {
        f32 m[12];
        f32 tmp[12];
        f32* mtxp;
        HSD_Envelope* envelope = list->data;

        if (envelope == NULL) {
            __assert(lbl_8047CC10, 0x65, "envelope");
        }
        if (envelope->weight >= lbl_8047CC18) {
            HSD_JObjSetupMatrix(envelope->jobj);
            if (right != NULL) {
                PSMTXConcat((f32*)envelope->jobj->matrix,
                            (f32*)envelope->jobj->envelopemtx, m);
                mtxp = m;
            } else {
                mtxp = (f32*)envelope->jobj->matrix;
            }
        } else {
            m[0] = m[1] = m[2] = m[3] = m[4] = m[5] = m[6] = m[7] =
                m[8] = m[9] = m[10] = m[11] = lbl_8047CC1C;
            while (envelope != NULL) {
                HSD_JObj* jp;

                if (envelope->jobj == NULL) {
                    __assert(lbl_8047CC10, 0x7E, "envelope->jobj");
                }
                jp = envelope->jobj;
                HSD_JObjSetupMatrix(jp);
                if ((f32*)jp->matrix == NULL) {
                    __assert(lbl_8047CC10, 0x81, lbl_8047CC20);
                }
                if ((f32*)jp->envelopemtx == NULL) {
                    __assert(lbl_8047CC10, 0x82, "jp->envelopemtx");
                }
                PSMTXConcat((f32*)jp->matrix, (f32*)jp->envelopemtx, tmp);
                HSD_MtxScaledAdd(tmp, envelope->weight, m, m);
                envelope = envelope->next;
            }
            mtxp = m;
        }
        if (right != NULL) {
            PSMTXConcat(mtxp, right, m);
        }
        PSMTXConcat(vmtx, mtxp, tmp);
        fn_800E0628(mtx + idx * 12, tmp);
    }
}
