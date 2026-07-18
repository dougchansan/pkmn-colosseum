/**
 * @file gs_range_801653CC_candidate_80166D48.c
 * @brief Candidate gs-engine range, 0x80166D48 - 0x80166E44.
 */
#include "game/gs_range_801653CC_shared.h"

void fn_80166D48(u32 volume, u32 fade, u32 includeBgm, u32 includeSe)
{
    extern u8 _sndSetVolumeWork(u32, u8);
    u32 offset;
    u32 group;
    u32 i;

    if (includeBgm == 1 && includeSe == 1) {
        group = 0xFF;
    } else if (includeBgm == 1 && includeSe != 1) {
        group = 0xFD;
    } else if (includeBgm != 1 && includeSe == 1) {
        group = 0xFE;
    } else {
        return;
    }

    for (i = 0, offset = 0; i < lbl_8047B0E8; offset += sizeof(GSsndEntry), i++) {
        u32 flags = *(u8*)((u8*)lbl_80478FAC + offset);

        if (((flags >> 5) & 1U) == 1U) {
            switch (group) {
            case 0xFD:
                if (((flags >> 7) & 1U) == 1U) {
                    continue;
                }
                break;
            case 0xFE:
                if (((flags >> 7) & 1U) == 0U) {
                    continue;
                }
                break;
            }
            _sndSetVolumeWork(i, volume);
        }
    }

    sndVolume(volume, fade, group);
}
