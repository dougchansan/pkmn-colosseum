/** Exact nursery status accessors, 0x801ED218 - 0x801ED310. */
#include "dolphin/types.h"

extern u8* savedataGetStatus(u8* arg0, u16 arg1);

u8 daycareGetState(void* arg0)
{
    u8* status;
    u8 result;

    if (arg0 == NULL) {
        status = savedataGetStatus(NULL, 0xB);
    } else {
        status = (u8*)arg0;
    }

    result = status[0];
    return result;
}

s32 daycareGetValue4(void* arg0)
{
    u8* status;

    if (arg0 == NULL) {
        status = savedataGetStatus(NULL, 0xB);
    } else {
        status = (u8*)arg0;
    }

    if (status[0] != 0) {
        return *(s32*)(status + 4);
    }
    return -1;
}

u8 daycareGetValue1(void* arg0)
{
    u8* status;

    if (arg0 == NULL) {
        status = savedataGetStatus(NULL, 0xB);
    } else {
        status = (u8*)arg0;
    }

    if (status[0] != 0) {
        return status[1];
    }
    return 0xFF;
}

u8* sodateyaGetPokemonPtr(void* arg0)
{
    if (arg0 == NULL) {
        arg0 = savedataGetStatus(NULL, 0xB);
    }

    return (u8*)arg0 + 8;
}
