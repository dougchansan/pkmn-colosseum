#include "dolphin/types.h"

u8 fightIsWazaThunderInRain(u32 move_id)
{
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern s32 fightFloorGetNowTenkouDataId(void*, u32);
    u16 value = (u16)wazaGetStatus(0, move_id, 9, 0);

    if ((u8)fightFloorGetNowTenkouDataId(0, 1) == 2 && value == 0x98) {
        return 1;
    }
    return 0;
}

s32 fn_80229BD8(s32 arg)
{
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    u16 value = (u16)wazaGetStatus(0, arg, 9, 0);

    if (value == 0x11 || value == 0x4E) {
        return 1;
    }
    return 0;
}
