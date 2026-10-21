#include "dolphin/gx/GXInternal.h"

void GXSetProjectionv(f32* projection)
{
    GXData* data;
    u32 type;

    type = __cvt_fp2unsigned(projection[0]);
    data = gx;
    data->field_420 = type;
    data->field_424 = projection[1];
    data->field_428 = projection[2];
    data->field_42C = projection[3];
    data->field_430 = projection[4];
    data->field_434 = projection[5];
    data->field_438 = projection[6];

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x00061020;
    GX_FIFO_F32 = data->field_424;
    GX_FIFO_F32 = data->field_428;
    GX_FIFO_F32 = data->field_42C;
    GX_FIFO_F32 = data->field_430;
    GX_FIFO_F32 = data->field_434;
    GX_FIFO_F32 = data->field_438;
    GX_FIFO_U32 = data->field_420;
    data->status.half.field_002 = 1;
}
