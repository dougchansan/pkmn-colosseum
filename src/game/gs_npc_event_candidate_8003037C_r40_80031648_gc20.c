/**
 * @file gs_npc_event_candidate_8003037C_r40_80031648_gc20.c
 * @brief fn_80031648 carve, 0x80031648 - 0x800318D8.
 *
 * NPC event callback: look the event id up in the name column of the
 * twelve-entry table lbl_80266E90 and print, centred on the sprite, the
 * Pokemon's name: message 0x56C for a Shadow (fusei) Pokemon, 0x56B for
 * an egg, otherwise its nickname.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void GSmsgPrintColor(s32, s32, u32, u16);
extern u32 GSmsgGetRect(u32 msg);
extern s32 GSmsgGetGSchar(u32 msg);
extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
extern u8 pokemonBiosGetTamagoFlag(void* pokemon);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);

typedef struct {
    u8 kind;
    u8 arg;
    u16 ids[8];
} NpcPokemonEvent;

extern NpcPokemonEvent lbl_80266E90[];
extern u8 lbl_803A2688[];

void fn_80031648(u8* r3, u8* r4)
{
    void* pokemon;
    s32 i;
    s32 kind;
    s32 arg;
    s16 width;
    void* name;

    pokemon = NULL;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(r4 + 0x6) == lbl_80266E90[i].ids[4]) {
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
        msgctrlSetValue(0x37, GSmsgGetGSchar(0x56C));
        width = GSmsgGetRect(0xE7) >> 16;
        GSmsgPrintColor((s16)(*(s16*)(r4 + 0x54) / 2 - width / 2), 0, -1, 0xE7);
        winSpriteSetDisp(r4, 1);
    } else if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
        if (pokemonBiosGetTamagoFlag(pokemon) != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(0x56B));
        } else {
            name = pokemonBiosGetNicknamePtr(pokemon);
            msgctrlSetValue(0x37, (s32)name);
        }
        width = GSmsgGetRect(0xE7) >> 16;
        GSmsgPrintColor((s16)(*(s16*)(r4 + 0x54) / 2 - width / 2), 0, -1, 0xE7);
        winSpriteSetDisp(r4, 1);
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
