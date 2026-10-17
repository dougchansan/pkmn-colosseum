#include "dolphin/gx/GXInternal.h"

void GXSetCurrentMtx(u32 index)
{
    GXData* data = gx;

    data->mtxIdx0 = (data->mtxIdx0 & ~0x3FU) | index;
    __GXSetMatrixIndex(0);
}
