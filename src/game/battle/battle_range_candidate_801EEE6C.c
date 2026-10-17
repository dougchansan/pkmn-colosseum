#include "dolphin/types.h"

extern u8* darkPokemonSaveGetPtr(u32 data);

void darkPokemonSaveSet4A6(u16 id, u16 value)
{
    u8* data = darkPokemonSaveGetPtr(0);

    if (data != NULL) {
        *(u16*)(data + id * 0x18 + 0x4A6) = value;
    }
}
