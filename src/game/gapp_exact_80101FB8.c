#include "dolphin/types.h"

extern u8 lbl_80478B28;
extern void GXSetGPMetric(s32, s32);
extern void gappDrawSyncCallback(u32);

typedef void (*GappBreakPointCallback)(u32 token);
extern GappBreakPointCallback lbl_8047ACF4;
extern GappBreakPointCallback
GXSetDrawSyncCallback(GappBreakPointCallback callback);

void gappLoadMeterInit(u8 param)
{
    if (param < 1) {
        param = 1;
    }
    lbl_80478B28 = param;
    lbl_8047ACF4 = GXSetDrawSyncCallback(gappDrawSyncCallback);
    GXSetGPMetric(0x22, 0x16);
}
