/**
 * @file gs_npc_event_candidate_8003037C_r40_80031228_gc20.c
 * @brief fn_80031228 carve, 0x80031228 - 0x80031404.
 *
 * NPC event callback: look the event id up in the first column of the
 * twelve-entry table lbl_80266E90 and, for a Shadow (fusei) or valid
 * event Pokemon, hand it to menuFaceDrawPokemon and show the sprite.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
extern void menuFaceDrawPokemon(u8* r3, u8* r4, void* pokemon);

typedef struct {
    u8 kind;
    u8 arg;
    u16 ids[8];
} NpcPokemonEvent;

extern NpcPokemonEvent lbl_80266E90[];
extern u8 lbl_803A2688[];

void fn_80031228(u8* r3, u8* r4)
{
    void* pokemon;
    s32 i;
    s32 kind;
    s32 arg;

    pokemon = NULL;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(r4 + 0x6) == lbl_80266E90[i].ids[0]) {
            kind = lbl_80266E90[i].kind;
            arg = lbl_80266E90[i].arg;
        }
    }

    switch (kind) {
    case 1:
        pokemon = (void*)heroGetStatus(NULL, 3, (u16)arg);
        break;
    case 2:
        pokemon = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
        break;
    }

    if (pokemonBiosGetFuseiFlag(pokemon) != 0) {
        menuFaceDrawPokemon(r3, r4, pokemon);
        winSpriteSetDisp(r4, 1);
    } else if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
        menuFaceDrawPokemon(r3, r4, pokemon);
        winSpriteSetDisp(r4, 1);
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
