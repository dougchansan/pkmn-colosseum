#include "dolphin/types.h"

extern u8 lbl_80478B28;
extern void fn_800BD91C(s32, s32);
extern void fn_80101B34(u32);

typedef void (*GappBreakPointCallback)(u32 token);
extern GappBreakPointCallback lbl_8047ACF4;
extern GappBreakPointCallback
GXSetDrawSyncCallback(GappBreakPointCallback callback);

void fn_80101FB8(u8 param)
{
    if (param < 1) {
        param = 1;
    }
    lbl_80478B28 = param;
    lbl_8047ACF4 = GXSetDrawSyncCallback(fn_80101B34);
    fn_800BD91C(0x22, 0x16);
}
