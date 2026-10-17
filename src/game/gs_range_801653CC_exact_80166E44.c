/**
 * @file gs_range_801653CC_exact_80166E44.c
 * @brief Byte-exact gs-engine range, 0x80166E44 - 0x80166E88.
 */
#include "game/gs_range_801653CC_shared.h"

void soundShutdown(void)
{
    if (fn_8015FFD4() == 1) {
        soundMasterVolume(0, 0, 1, 1);
        sndQuit();
    }
}
