/** Residual retail-only GSmodelEnableColorSwap island at 0x800E5E34. */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialSetColorChannels(void* material, void* color0, void* color1,
                                void* color2, void* color3);
void GSmaterialEnableExtension(void* material, s32 mode);

void GSmodelEnableColorSwap(GSmodel* model, void* color0,
                            void* color1, void* color2, void* color3)
{
    u16 count = GSmodelAcquireMaterials(model);
    void** materials = (void**)model->materialList;
    s32 i;

    for (i = 0; i < count; i++, materials++) {
        if (*materials != NULL) {
            GSmaterialSetColorChannels(*materials, color0, color1, color2,
                                       color3);
            GSmaterialEnableExtension(*materials, 2);
        }
    }
}
