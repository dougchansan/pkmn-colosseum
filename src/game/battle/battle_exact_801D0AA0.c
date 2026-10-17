/** Exact party-ball/status helpers, 0x801D0AA0 - 0x801D0C30. */
#include "dolphin/types.h"

u8 heroGetPokemonBallId(u32 index)
{
    extern u32 savedataGetStatus(u8* data, u16 index);
    extern void* heroBiosGetPokemonPtr(u8* status, u16 index);
    extern u8 pokemonCheckValid(u8* pokemon);
    extern u8 pokemonBiosGetCatchBallId(u8* pokemon);
    u8* pokemon;

    pokemon = heroBiosGetPokemonPtr((u8*)savedataGetStatus(NULL, 2),
                                    (u16)index);
    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    return pokemonBiosGetCatchBallId(pokemon);
}

u16 fn_801D0AFC(s32 mode)
{
    extern u16 lbl_8047B3D8;
    extern u32 savedataGetStatus(u8* data, u16 index);
    extern void* heroBiosGetPokemonPtr(u8* status, u16 index);
    extern u8 pokemonCheckValid(u8* pokemon);
    extern u32 pokemonGetStatus(u8* pokemon, u32 value, u32 mode,
                                u32 unused);
    extern u8 pokemonBiosGetCatchBallId(u8* pokemon);
    extern void pokemonAllKaihuku(u8* pokemon);
    u32 i;
    u32 status;
    u8 valid;
    u8* pokemon;

    lbl_8047B3D8 = i = 0;
    while ((u16)i < 6) {
        valid = 1;
        pokemon = heroBiosGetPokemonPtr((u8*)savedataGetStatus(NULL, 2),
                                        (u16)i);
        if (!pokemonCheckValid(pokemon)) {
            valid = 0;
        } else {
            status = pokemonGetStatus(pokemon, 0, 0x6E, 0);
            pokemonGetStatus(pokemon, (u16)status, 1, 0);
            pokemonBiosGetCatchBallId(pokemon);
            pokemonAllKaihuku(pokemon);
        }
        if (valid) {
            lbl_8047B3D8++;
        }
        i++;
    }
    return lbl_8047B3D8;
}

void std083_unk_801d0bd0(s32 arg0, s32 arg1, s32 mode)
{
    extern void pokemonCenterHealSequence(void);
    extern void pokemonCenterBallMachineSequence(s32 arg0, s32 arg1);

    switch (mode) {
    case 0:
        pokemonCenterBallMachineSequence(arg0, arg1);
        break;
    case 1:
        pokemonCenterHealSequence();
        break;
    case 2:
        pokemonCenterHealSequence();
        break;
    case 3:
        pokemonCenterHealSequence();
        break;
    }
}
