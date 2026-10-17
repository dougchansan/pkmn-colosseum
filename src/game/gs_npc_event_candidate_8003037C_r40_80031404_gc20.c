/**
 * @file gs_npc_event_candidate_8003037C_r40_80031404_gc20.c
 * @brief fn_80031404 carve, 0x80031404 - 0x80031648.
 *
 * NPC event callback: look the event id up in the gender column of the
 * twelve-entry table lbl_80266E90 and print the gender symbol of the
 * valid event Pokemon it names (none for an egg).
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void GSmsgPrintColor(s32, s32, u32, u16);
extern s32 GSmsgGetGSchar(u32 msg);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);
extern u8 pokemonBiosGetTamagoFlag(void* pokemon);

typedef struct {
    u8 kind;
    u8 arg;
    u16 ids[8];
} NpcPokemonEvent;

extern NpcPokemonEvent lbl_80266E90[];
extern u8 lbl_803A2688[];

void fn_80031404(u8* r3, u8* r4)
{
    void* pokemon;
    u32 msg;
    u32 combined;
    s32 i;
    s32 kind;
    s32 arg;

    combined = (*(u32*)(r4 + 0x64) & ~0xFF) | *(u8*)(r3 + 0x8B);
    pokemon = NULL;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(r4 + 0x6) == lbl_80266E90[i].ids[5]) {
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

    if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
        switch (menuSubGetPokemonSexForDisp(pokemon)) {
        case 0:
            msg = 0xD67;
            break;
        case 1:
            msg = 0xD68;
            break;
        case 2:
        default:
            msg = 0;
            break;
        }
        if (pokemonBiosGetTamagoFlag(pokemon) == 1) {
            msg = 0;
        }
        if (msg != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(msg));
            GSmsgPrintColor(2, 0, combined, 0xCF);
            winSpriteSetDisp(r4, 1);
        } else {
            winSpriteSetDisp(r4, 0);
        }
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
