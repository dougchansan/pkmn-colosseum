/** Residual retail-only GSmodelSetTextureChange island at 0x800E563C. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialSetTexture(void* material, void* textureChange);

void GSmodelSetTextureChange(GSmodel* model, void* textureChange)
{
    u16 count = GSmodelAcquireMaterials(model);
    void** materials = (void**)model->materialList;
    s32 i;

    for (i = 0; i < count; i++, materials++) {
        if (*materials != NULL) {
            GSmaterialSetTexture(*materials, textureChange);
        }
    }
}
