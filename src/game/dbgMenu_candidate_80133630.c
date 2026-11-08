/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

extern void gappSetDrawSyncEnable(u32 enabled);

u32 dbgMenuToggleDrawSync(void)
{
    u32 leadingZeroes = __cntlzw(lbl_8047AED4);

    lbl_8047AED4 = leadingZeroes >> 5;
    gappSetDrawSyncEnable(leadingZeroes >> 5);
    return 0;
}
