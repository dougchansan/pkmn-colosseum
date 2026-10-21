#include "dolphin/types.h"

extern void HSD_SetPixelFormat(s32 mode, s32 value);
extern s32 GSgfxVideoVsyncRate;

void fn_800D377C(s32 mode)
{
    switch (mode) {
    case 1:
        HSD_SetPixelFormat(0, 0);
        break;
    case 2:
        HSD_SetPixelFormat(1, 0);
        break;
    }
}

s32 GSgfxGetVsyncRate(void)
{
    return GSgfxVideoVsyncRate;
}
