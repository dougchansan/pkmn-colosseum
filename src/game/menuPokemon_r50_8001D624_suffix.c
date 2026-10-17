/**
 * @file menuPokemon_r50_8001D624_suffix.c
 * @brief menuPokemonGetStatusSprite (0x8001D624 - 0x8001D718): the status-icon ID for a
 *        Pokemon in the menus.
 *
 * Function-boundary carve of menuPokemon.c, text only, GC/2.0 -O4,p with the
 * peephole pass off for the whole unit (the source wrapped the function in
 * `#pragma peephole off`; with the pass on it is 81.1%). Status 0x7B set
 * gives kind 1, otherwise the Joutai sprite 0x3A-0x3E gives kinds 2-6, and
 * the kind indexes one of two u16 ID tables in data_802E4DB0.c.
 */
#include "dolphin/types.h"

extern u8 pokemonGetStatus(void* pokemon, u32 a, u32 id, u32 b);
extern u16 pokemonGetJoutaiMenuSpriteId(void* pokemon);

extern u16 lbl_802E4EB8[8];
extern u16 lbl_802E4EC8[8];

u16 menuPokemonGetStatusSprite(void* pokemon, u8 alt)
{
    u16 kind;

    if (pokemonGetStatus(pokemon, 0, 0x7B, 0) == 1) {
        kind = 1;
    } else {
        switch (pokemonGetJoutaiMenuSpriteId(pokemon)) {
        case 0x3A: kind = 2; break;
        case 0x3B: kind = 3; break;
        case 0x3C: kind = 4; break;
        case 0x3D: kind = 5; break;
        case 0x3E: kind = 6; break;
        default: kind = 0; break;
        }
    }
    if (alt == 0) {
        return lbl_802E4EB8[kind];
    }
    return lbl_802E4EC8[kind];
}
