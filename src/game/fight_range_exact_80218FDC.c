#include "dolphin/types.h"

extern u32 wazaGetStatus(void* ptr, u16 dataId, u16 status, u32 index);

u32 fightSeqRendouWazaCheck(u16 id)
{
    if (id == 0 || id == 0x165 || id == 0xd6 || id == 0x112 || id == 0x77 ||
        id == 0x76) {
        return 1;
    }
    return 0;
}

u8 fightAiIsSpecialEffect097(u16 dataId)
{
    u16 effect = (u16)wazaGetStatus(0, dataId, 9, 0);

    if (effect == 0x91 || effect == 0x27 || effect == 0x4b ||
        effect == 0x97 || effect == 0x9b || effect == 0x1a) {
        return 1;
    }
    return 0;
}
