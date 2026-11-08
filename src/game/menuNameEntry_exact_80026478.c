/**
 * @file menuNameEntry_exact_80026478.c
 * @brief Gender-symbol visibility callbacks for the selected Pokemon.
 */
#include "dolphin/types.h"

extern void* heroGetStatus(s32, s32, u32);
extern u8 pokemonCheckValid(void);
extern u8 pokemonGetSex(void* pokemon);

s32 menuNameEntryDrawGender(void* owner, u8* draw)
{
    void* context;
    void* pokemon;
    u8 visible;

    context = *(void**)((u8*)owner + 0x60);
    visible = 0;
    if (*(s32*)((u8*)context + 0x1C) != 2) {
        draw[0x67] = 0;
        return 0;
    }
    pokemon = heroGetStatus(0, 3, (u16)*(u32*)((u8*)context + 0x20));
    if ((u8)pokemonCheckValid() == 0) {
        goto done;
    }
    if ((u32)(pokemonGetSex(pokemon) & 0xFF) != 1) {
        goto done;
    }
    visible = 0xFF;
done:
    draw[0x67] = visible;
    return 0;
}

s32 menuNameEntryDrawGender2(void* owner, u8* draw)
{
    void* context;
    void* pokemon;
    u8 visible;

    context = *(void**)((u8*)owner + 0x60);
    visible = 0;
    if (*(s32*)((u8*)context + 0x1C) != 2) {
        draw[0x67] = 0;
        return 0;
    }
    pokemon = heroGetStatus(0, 3, (u16)*(u32*)((u8*)context + 0x20));
    if ((u8)pokemonCheckValid() == 0) {
        goto done;
    }
    if ((u32)(pokemonGetSex(pokemon) & 0xFF) != 0) {
        goto done;
    }
    visible = 0xFF;
done:
    draw[0x67] = visible;
    return 0;
}
