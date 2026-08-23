#include "dolphin/types.h"

typedef struct {
    u8 data[0x154];
} CopyBlock_0x154;

extern void* pokemonGetStatus(void* context, s32 arg1, s32 status, s32 arg3);
extern u32 pokemonSetStatus(void* context, s32 arg1, s32 status, s32 arg3, u32 value);

u32 fightOutPokemonUseHensinBuff(void* context)
{
    CopyBlock_0x154* src;
    CopyBlock_0x154* dest;

    if (context == NULL) {
        return 0;
    }
    src = (CopyBlock_0x154*) pokemonGetStatus(context, 0, 0xD5, 0);
    dest = (CopyBlock_0x154*) pokemonGetStatus(context, 0, 0xD7, 0);
    if (src == NULL) {
        return 0;
    }
    if (dest == NULL) {
        return 0;
    }
    *dest = *src;
    pokemonSetStatus(context, 0, 0xD6, 0, (u32) dest);
    return 1;
}

u8 fightOutPokemonIsUseHensinBuff(void* trainer)
{
    void* a;
    void* b;

    if (trainer == NULL) {
        return 0;
    }
    a = pokemonGetStatus(trainer, 0, 0xD6, 0);
    b = pokemonGetStatus(trainer, 0, 0xD7, 0);
    return (u8) (a == b);
}
