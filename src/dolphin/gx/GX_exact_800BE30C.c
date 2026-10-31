#include "dolphin/gx/GXInternal.h"

void GXClearGPMetric(void)
{
    __cpReg[2] = 4;
}

u32 gappReadGPMetric0(void)
{
    u32 hi;
    u32 lo;

    fn_800BE164(&hi, &lo);
    return hi;
}
