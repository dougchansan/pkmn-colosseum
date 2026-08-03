/** Residual retail-only GSmodelSetRenderFlags island at 0x800E69C4. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

s32 GSmaterialGetFlags(void* material);
void GSmaterialSetFlags(void* material, s32 flags);

void GSmodelSetRenderFlags(GSmodel* model, s32 flags)
{
    u16 count = GSmodelAcquireMaterials(model);
    void** materials = (void**)model->materialList;
    s32 i;

    for (i = 0; i < count; i++, materials++) {
        if (*materials != NULL) {
            GSmaterialSetFlags(*materials,
                               flags | GSmaterialGetFlags(*materials));
        }
    }
}
