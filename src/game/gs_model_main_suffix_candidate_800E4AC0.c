/** Residual retail-only GSmodelAttachToGSpart island at 0x800E4AC0. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"

typedef struct GSmodelAttachPart {
    u16 _pad0;
    u16 partIndex;
    GSmodel* model;
} GSmodelAttachPart;

void fn_800E01D0(GSvec* dst, const GSvec* src);
void fn_800E01F4(GSvec* dst, f32 x, f32 y, f32 z);

void GSmodelAttachToGSpart(GSmodel* model, GSmodelAttachPart* part,
                           s32 attachmentType, s32 updateOnce, u8 resetTransform)
{
    u32 flags;

    if (model->transformOverride != 0) {
        return;
    }
    if (attachmentType == 0) {
        return;
    }

    model->unk118 = part->model;
    model->unk11C = (void*)(u32)part->partIndex;
    if (resetTransform == 1) {
        fn_800E01F4(&model->overridePosition, lbl_8047CB7C,
                    lbl_8047CB7C, lbl_8047CB7C);
        fn_800E01F4(&model->overrideRotation, lbl_8047CB7C,
                    lbl_8047CB7C, lbl_8047CB7C);
        fn_800E01F4(&model->overrideScale, lbl_8047CB80,
                    lbl_8047CB80, lbl_8047CB80);
    } else {
        fn_800E01D0(&model->overridePosition, &model->position);
        fn_800E01D0(&model->overrideRotation, &model->rotation);
        fn_800E01D0(&model->overrideScale, &model->scale);
    }

    fn_800E01F4(&model->position, lbl_8047CB7C, lbl_8047CB7C,
                lbl_8047CB7C);
    fn_800E01F4(&model->rotation, lbl_8047CB7C, lbl_8047CB7C,
                lbl_8047CB7C);
    fn_800E01F4(&model->scale, lbl_8047CB80, lbl_8047CB80,
                lbl_8047CB80);
    model->transformOverride = attachmentType;

    flags = model->flags.raw | 0x80000;
    model->flags.raw = flags;
    if ((u8)updateOnce != 0) {
        flags = model->flags.raw | 0x40000;
        model->flags.raw = flags;
    }
}
