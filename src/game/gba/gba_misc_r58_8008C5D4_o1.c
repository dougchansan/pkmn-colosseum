/**
 * gba_misc 0x8008C5D4 - 0x8008C6FC: GameCube status condition to GBA
 * status word. Same code as gbaPokemonConditonFromGC in gba_misc.c, kept
 * as a standalone unit (like the gba_misc_exact_* siblings) so it can be
 * linked.
 */
#include "dolphin/types.h"

extern s16 pokemonGetJoutaiStack(void* pokemon, s32 kind);
extern s8 pokemonGetJoutaiKaisuu(void* pokemon, s32 kind);
extern u8 pokemonIsJoutaiDataId(void* pokemon, s32 kind);

/* 0x8008C5D4 | size: 0x128 */
u16 gbaPokemonConditonFromGC(void* pokemon) {
    u16 status = 0;

    if (pokemonIsJoutaiDataId(pokemon, 4) != 0) {
        status = (pokemonGetJoutaiStack(pokemon, 4) << 8) | 0x80;
    } else if (pokemonIsJoutaiDataId(pokemon, 5) != 0) {
        status |= 0x40;
    } else if (pokemonIsJoutaiDataId(pokemon, 7) != 0) {
        status |= 0x20;
    } else if (pokemonIsJoutaiDataId(pokemon, 6) != 0) {
        status |= 0x10;
    } else if (pokemonIsJoutaiDataId(pokemon, 3) != 0) {
        status |= 0x08;
    } else if (pokemonIsJoutaiDataId(pokemon, 8) != 0) {
        status = (s16)pokemonGetJoutaiKaisuu(pokemon, 8);
    }
    return status;
}
