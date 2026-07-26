#ifndef GAME_GS_MODEL_MATERIAL_INTERNAL_H
#define GAME_GS_MODEL_MATERIAL_INTERNAL_H

u16 fn_800E3534(u32 size);
void* fn_800E27B0(u32 handle);
s32 fn_800EE0E8(void* model);
void* fn_800EE150(void* model, s32 index);
s32 fn_800EE758(void* part);
void* fn_800EE6B4(void* part, s32 index);
void fn_800EE828(void* part);

static inline u16 GSmodelAcquireMaterials(GSmodel* model)
{
    s32 count;
    s32 partCount;
    s32 partIndex;

    if (model->modulationRefCount == 0) {
        count = 0;
        partCount = fn_800EE0E8(model);
        for (partIndex = 0; partIndex < partCount; partIndex++) {
            void* part = fn_800EE150(model, partIndex);
            count += fn_800EE758(part);
            fn_800EE828(part);
        }

        if (count != 0) {
            u16 handle = fn_800E3534(count * sizeof(void*));

            if (handle != 0) {
                void** dst = fn_800E27B0(handle);

                model->materialList = (GSmodelMaterialList*)dst;
                model->materialListHandle = handle;
                model->materialCount = count;
                for (partIndex = 0; partIndex < partCount; partIndex++) {
                    void* part = fn_800EE150(model, partIndex);
                    s32 partMaterialCount = fn_800EE758(part);
                    s32 materialIndex;

                    for (materialIndex = 0; materialIndex < partMaterialCount;
                         materialIndex++) {
                        *dst++ = fn_800EE6B4(part, materialIndex);
                    }
                    fn_800EE828(part);
                }
            } else {
                count = 0;
            }
        }
    }

    model->modulationRefCount++;
    return model->materialCount;
}

#endif
