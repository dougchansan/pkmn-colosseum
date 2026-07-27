/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 dbgMenuGSmemOptimize(void)
{
    u32 value = fn_800E1544();

    GSlogWrite(lbl_80272AB8, value);
    return 0;
}
