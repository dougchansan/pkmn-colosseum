/**
 * GSmodelBlurCreateWork, 0x800E584C - 0x800E5978 (one-function carve).
 * Builds the material list on the first reference (modelInitMaterialList,
 * XD _modelInitMaterialList) and returns the material list and its count.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void** GSmodelBlurCreateWork(GSmodel* model, u32* count)
{
    if (model->modulationRefCount == 0) {
        *count = modelInitMaterialList(model);
    } else {
        *count = model->materialCount;
    }
    model->modulationRefCount++;
    return (void**)model->materialList;
}
