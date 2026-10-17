#include "dolphin/gx/GXInternal.h"

void GXSetViewport(f32 left, f32 top, f32 width, f32 height, f32 nearz, f32 farz)
{
    GXSetViewportJitter(left, top, width, height, nearz, farz, 1);
}

void GXGetViewportv(f32* projection)
{
    GXData* data = gx;

    projection[0] = data->projection[0];
    projection[1] = data->projection[1];
    projection[2] = data->projection[2];
    projection[3] = data->projection[3];
    projection[4] = data->projection[4];
    projection[5] = data->projection[5];
}
