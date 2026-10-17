#include "dolphin/gx/GXInternal.h"

void GXSetTevIndirect(u32 dstCoord, u32 function, u32 sourceParam, u32 matrix,
                 u32 normalize, u32 postMatrix, u32 normalizeColor,
                 u8 bias, u8 arg8, u32 arg9)
{
    GXData* data = gx;
    u32 command;
    u32 value = function;

    value = (value & ~0xCU) | (sourceParam << 2);
    value = (value & ~0x70U) | (matrix << 4);
    value = (value & ~0x180U) | (arg9 << 7);
    value = (value & ~0x1E00U) | (normalize << 9);
    value = (value & ~0xE000U) | (postMatrix << 13);
    dstCoord += 0x10;
    value = (value & ~0x70000U) | (normalizeColor << 16);
    value = (value & ~0x80000U) | (arg8 << 19);
    value = (value & ~0x100000U) | (bias << 20);

    command = value & 0xFFFFFF;
    command |= dstCoord << 24;
    GX_BP_REG(command);
    data->status.half.field_002 = 0;
}
