#ifndef GAME_GS_MODEL_MATERIAL_INTERNAL_H
#define GAME_GS_MODEL_MATERIAL_INTERNAL_H

s32 GSmodelWalkJObjs(void* model);
s32 GSpartGetMaterialCount(GSpart* part);
void* GSpartGetMaterial(GSpart* part, s32 index);

/*
 * Builds the model's material list on first use: counts the materials of
 * every part, allocates the pointer array and fills it. Returns the count
 * (0 when the model has none); a failed allocation still returns the count.
 * XD names it _modelInitMaterialList__FP8_GSmodel (NXXJ01.map line 6150,
 * GSmodel.o, UNUSED 0xEC: inlined everywhere; StarsMmd/Colo-XD-PBR-symbol-maps).
 * Admitted as a reconstructed helper: the same sequence is expanded in
 * GSmodelSetTextureChange and its siblings (GSmodelSetPEdescr,
 * GSmodelEnableModulation, GSmodelEnableColorSwap, GSmodelEnableEnvMap,
 * GSmodelInitMaterialAlpha, GSmodelSetRenderFlags, GSmodelBlurCreateWork), and its
 * return value is routed through the helper's count and copied to the
 * caller's (mr r24,r26 in GSmodelSetTextureChange), which the in-place
 * forms do not emit.
 */
static inline s32 modelInitMaterialList(GSmodel* model)
{
    void** dst;
    s32 partIndex;
    GSpart* part;
    s32 partCount;
    s32 count;
    s32 materialIndex;
    u32 handle;
    s32 partMaterialCount;

    count = 0;
    partCount = GSmodelWalkJObjs(model);
    for (partIndex = 0; partIndex < partCount; partIndex++) {
        part = GSmodelGetPart(model, partIndex);
        count += GSpartGetMaterialCount(part);
        GSpartFree(part);
    }
    if (count == 0) {
        return 0;
    }
    handle = _toolentryAlloc__FUl(count * sizeof(void*));
    if ((u16)handle != 0) {
        dst = (void**)(model->materialList = (GSmodelMaterialList*)GSmemLock(handle));
        model->materialListHandle = handle;
        model->materialCount = count;
        for (partIndex = 0; partIndex < partCount; partIndex++) {
            part = GSmodelGetPart(model, partIndex);
            partMaterialCount = GSpartGetMaterialCount(part);

            for (materialIndex = 0; materialIndex < partMaterialCount; materialIndex++, dst++) {
                *dst = GSpartGetMaterial(part, materialIndex);
            }
            GSpartFree(part);
        }
    }
    return count;
}

#endif
