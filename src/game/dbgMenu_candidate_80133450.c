/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 debugMenuColorBarDisp(void)
{
    *(u8*)&lbl_80478820 = 0;
    menuOpenCustom(5, 0, 0, 0, 1, 0);
    menuClose(5);
    *(u8*)&lbl_80478820 = 1;
    return 0;
}
