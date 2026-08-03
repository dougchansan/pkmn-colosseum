#ifndef GAME_GS_MODEL_MATERIAL_INTERNAL_H
#define GAME_GS_MODEL_MATERIAL_INTERNAL_H

s32 fn_800EE0E8(void* model);
s32 GSpartGetMaterialCount(GSpart* part);
void* GSpartGetMaterial(GSpart* part, s32 index);

static inline u16 GSmodelAcquireMaterials(GSmodel* model)
{
    s32 count;
    s32 partCount;
    s32 partIndex;

    if (model->modulationRefCount == 0) {
        count = 0;
        partCount = fn_800EE0E8(model);
        for (partIndex = 0; partIndex < partCount; partIndex++) {
            GSpart* part = GSmodelGetPart(model, partIndex);
            count += GSpartGetMaterialCount(part);
            GSpartFree(part);
        }

        if (count != 0) {
            u32 handle = _toolentryAlloc__FUl(count * sizeof(void*));

            if (handle != 0) {
                void** dst = fn_800E27B0(handle);

                model->materialList = (GSmodelMaterialList*)dst;
                model->materialListHandle = handle;
                model->materialCount = count;
                for (partIndex = 0; partIndex < partCount; partIndex++) {
                    GSpart* part = GSmodelGetPart(model, partIndex);
                    s32 partMaterialCount = GSpartGetMaterialCount(part);
                    s32 materialIndex;

                    for (materialIndex = 0; materialIndex < partMaterialCount;) {
                        *dst = GSpartGetMaterial(part, materialIndex);
                        materialIndex++;
                        dst++;
                    }
                    GSpartFree(part);
                }
            } else {
                count = 0;
            }
        }
    }

    count = model->materialCount;
    model->modulationRefCount++;
    return count;
}

#endif
