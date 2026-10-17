/**
 * @file gs_npc_event_candidate_8003037C_r40_800308D4.c
 * @brief fn_800308D4 carve, 0x800308D4 - 0x80030A44.
 *
 * NPC event callback: look the event id up in the species column of the
 * two-entry table lbl_80266F68, fetch the party (1) or storage (2)
 * Pokemon it names and print message 0x2BD4 followed by its species name.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void GSmsgPrintColor(s32, s32, u32, u16);
extern u32 GSmsgGetRect(u32 msg);
extern u32 pokemonGetStatus(void* pokemon, u32 slot, u16 tableId, u16 flags);
extern void* pokemonDataBiosGetPtr(u16 species);
extern u32 pokemonDataBiosGetName(void* data);
extern s32 GSmsgGetGSchar(u32 msg);

typedef struct {
    u16 kind;
    u16 ids[9];
} NpcPartyEvent;

extern NpcPartyEvent lbl_80266F68[];
extern u8 lbl_803A2688[];
extern u32 lbl_8047A420;
extern u32 lbl_8047A424;

/* Message flags for `npc`: its own flag bits with the colour byte of `owner`. */
static inline u32 npcEventGetMsgFlags(u8* owner, u8* npc)
{
    return (*(u32*)(npc + 0x64) & ~0xFF) | *(u8*)(owner + 0x8B);
}

/* The party (kind 1) or storage (kind 2) Pokemon that the event id of
 * `npc` names in column `column` of lbl_80266F68, or NULL. */
static inline void* npcEventGetPokemon(u8* npc, s32 column)
{
    void* pokemon = NULL;
    s32 kind = 0;
    s32 i;

    for (i = 0; i < 2; i++) {
        if (*(s16*)(npc + 0x6) == lbl_80266F68[i].ids[column]) {
            kind = lbl_80266F68[i].kind;
        }
    }

    switch (kind) {
    case 1:
        pokemon = (void*)heroGetStatus(NULL, 3, (u16)lbl_8047A424);
        break;
    case 2:
        pokemon = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
        break;
    }
    return pokemon;
}

void fn_800308D4(u8* r3, u8* r4)
{
    void* pokemon;
    u32 combined;
    s16 x;

    combined = npcEventGetMsgFlags(r3, r4);
    pokemon = npcEventGetPokemon(r4, 2);
    if (pokemon != NULL) {
        x = GSmsgGetRect(0x2BD4) >> 16;
        GSmsgPrintColor(0, 0, combined, 0x2BD4);
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonDataBiosGetName(
                                  pokemonDataBiosGetPtr(pokemonGetStatus(pokemon, 0, 0x6E, 0)))));
        GSmsgPrintColor(x, 0, combined, 0xE7);
        *(u32*)(r4 + 0x4C) = 0;
        winSpriteSetDisp(r4, 1);
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
