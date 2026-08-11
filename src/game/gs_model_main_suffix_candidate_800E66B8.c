/** Residual retail-only GSmodelInitMaterialAlpha island at 0x800E66B8. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialStoreAlpha(void* material);

void GSmodelInitMaterialAlpha(GSmodel* model)
{
    u16 count = GSmodelAcquireMaterials(model);
    void** materials = (void**)model->materialList;
    s32 i;

    for (i = 0; i < count; i++) {
        if (materials[i] != NULL) {
            GSmaterialStoreAlpha(materials[i]);
        }
    }
}
