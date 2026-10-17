#include "dolphin/gx/GXInternal.h"

void GXSetFieldMode(u32 fieldMode, u32 halfAspectRatio)
{
    GXData* data = gx;

    data->lpSize = (data->lpSize & ~0x400000U) |
                   ((halfAspectRatio & 0xFFU) << 22);
    GX_BP_REG(data->lpSize);
    __GXFlushTextureState();
    GX_BP_REG(0x68000000U | (fieldMode & 0xFFU));
    __GXFlushTextureState();
}

void __GXSetRange(void)
{
}

void GXCallDisplayList(void* list, u32 nbytes)
{
    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }
    if (gx->status.word == 0) {
        __GXSendFlushPrim();
    }
    GX_FIFO_U8 = 0x40;
    GX_FIFO_U32 = (u32) list;
    GX_FIFO_U32 = nbytes;
}
