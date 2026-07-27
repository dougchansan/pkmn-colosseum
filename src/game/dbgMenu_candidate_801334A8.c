/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 debugMenuShadowBorderDisp(void)
{
    u8 enabled = (u8)((__cntlzw(lbl_8047AED9) >> 5) & 0xFF);

    lbl_8047AED9 = enabled;
    GSmodelSetShadowDebug(enabled);
    return 0;
}
