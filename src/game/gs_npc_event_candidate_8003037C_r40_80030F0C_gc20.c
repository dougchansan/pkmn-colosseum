/**
 * @file gs_npc_event_candidate_8003037C_r40_80030F0C_gc20.c
 * @brief gbaTradeMenuDrawSprite2 carve, 0x80030F0C - 0x80031188.
 *
 * NPC event callback: look the event id up in the two Shadow columns of
 * the twelve-entry table lbl_80266E90 and show the sprite when the
 * Pokemon's dark flag fits the column (second column: dark, first
 * column: not dark), for a Shadow (fusei) or valid event Pokemon.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
extern u8 pokemonBiosGetDarkFlag(void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);

typedef struct {
    u8 kind;
    u8 arg;
    u16 ids[8];
} NpcPokemonEvent;

extern NpcPokemonEvent lbl_80266E90[];
extern u8 lbl_803A2688[];

void gbaTradeMenuDrawSprite2(u8* r3, u8* r4)
{
    void* pokemon;
    u8 column;
    s32 i;
    s32 kind;
    s32 arg;

    pokemon = NULL;
    column = 0;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(r4 + 0x6) == lbl_80266E90[i].ids[1]) {
            kind = lbl_80266E90[i].kind;
            column = 1;
            arg = lbl_80266E90[i].arg;
        } else if (*(s16*)(r4 + 0x6) == lbl_80266E90[i].ids[2]) {
            kind = lbl_80266E90[i].kind;
            column = 2;
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
        if (pokemonBiosGetDarkFlag(pokemon) == 1) {
            if (column == 2) {
                winSpriteSetDisp(r4, 1);
                return;
            }
        } else if (column == 1) {
            winSpriteSetDisp(r4, 1);
            return;
        }
    }
    if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
        if (pokemonBiosGetDarkFlag(pokemon) == 1) {
            if (column == 2) {
                winSpriteSetDisp(r4, 1);
                return;
            }
        } else if (column == 1) {
            winSpriteSetDisp(r4, 1);
            return;
        }
    }
    winSpriteSetDisp(r4, 0);
}
