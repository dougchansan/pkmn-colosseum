/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 dbgMenuSetVideoModeEurgb60(void)
{
    GSgfxSetVideoMode_800D37D4(2, 2, 0, 2, 0, 0);
    return 0;
}
