/** Exact tool-entry resource cleanup, 0x8025CD64 - 0x8025CDB8. */
#include "dolphin/types.h"

extern u32 lbl_8047B650;
extern u32 GSmemGetHandleByPtr();
extern void GSmemFree();
extern void fn_800E24B0();

void toolentryTaisenFreePokemonData(void* ctx, u32 slot, u32 param)
{
    u32 handle;
    u32 result;

    handle = lbl_8047B650;
    if (handle != 0) {
        result = GSmemGetHandleByPtr(handle);
        if ((result & 0xFFFF) != 0) {
            fn_800E24B0(result);
            GSmemFree(result);
        }
        lbl_8047B650 = 0;
    }
}
