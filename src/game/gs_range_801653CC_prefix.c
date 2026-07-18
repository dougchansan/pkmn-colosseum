/**
 * @file gs_range_801653CC_prefix.c
 * @brief Candidate gs-engine range, 0x801653CC - 0x801654E0.
 */
#include "game/gs_range_801653CC_shared.h"

u32 fn_801653CC(u32 id, void* buffer, u32 size)
{
    GSsndEntry* entry;
    void* resource;
    void* wave;
    u32 waveIndex;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    entry = &lbl_80478FAC[id];
    waveIndex = entry->waveIndex;
    if (waveIndex < *lbl_80478E30) {
        resource = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
        if (resource != 0) {
            wave = lbl_80478E34[waveIndex].data;
            if (wave != 0) {
                fn_80165EE0(wave, resource, 0x10000);
                fn_80166B3C(id, 0, 0x408);
                fn_80166A50(id, (u32)buffer, size, 0);
                while (fn_801666BC(id) == 2) {
                    _threadSwitch();
                }
                fn_80166B18(id);
            }
            fn_800F9210(0, 0x408);
        }
    }

    return 1;
}
