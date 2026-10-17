#include "dolphin/gx/GXInternal.h"

void GXSetZCompLoc(u32 value)
{
    GXData* data = gx;

    data->field_1DC = (data->field_1DC & ~0x40U) |
                      ((value & 0xFF) << 6);
    GX_BP_REG(data->field_1DC);
    data->status.half.field_002 = 0;
}

void GXSetPixelFmt(s32 pixelFmt, u32 zFmt)
{
    GXData* data = gx;
    u32 old = data->field_1DC;
    u32 value;
    u8 isY8;

    data->field_1DC = (data->field_1DC & ~7U) | lbl_80313608[pixelFmt];
    data->field_1DC = (data->field_1DC & ~0x38U) | (zFmt << 3);

    value = data->field_1DC;
    if (old != value) {
        GX_BP_REG(value);
        if (pixelFmt == 2) {
            isY8 = 1;
        } else {
            isY8 = 0;
        }
        data = gx;
        data->genMode = (data->genMode & ~0x200U) | (isY8 << 9);
        data->dirtyState |= 4;
    }

    if (lbl_80313608[pixelFmt] == 4) {
        data = gx;
        data->dstAlpha = (data->dstAlpha & ~0x600U) |
                         (((pixelFmt - 4) << 9) & 0x600U);
        data->dstAlpha = (data->dstAlpha & 0xFFFFFFU) | 0x42000000U;
        GX_BP_REG(data->dstAlpha);
    }

    gx->status.half.field_002 = 0;
}
