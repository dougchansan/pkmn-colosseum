/** Residual retail-only GSmodelEnableEnvMap island at 0x800E61BC. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialSetEnvMapParams(void* material, f64 blend, void* texture,
                               void* matrix, void* light);
void GSmaterialEnableExtension(void* material, s32 mode);

void GSmodelEnableEnvMap(GSmodel* model, void* texture, void* matrix,
                         void* light, f32 blend)
{
    u16 count;
    void** materials;
    s32 i;

    if (model->materialCount != 0) {
        void* first = model->materialList->materials[0];
        if (first != NULL && (GSmaterialGetEnabledExtensions(first) & 4) != 0) {
            return;
        }
    }

    count = GSmodelAcquireMaterials(model);
    materials = (void**)model->materialList;
    for (i = 0; i < count; i++, materials++) {
        if (*materials != NULL) {
            GSmaterialSetEnvMapParams(*materials, blend, texture, matrix,
                                      light);
            GSmaterialEnableExtension(*materials, 4);
        }
    }
}
