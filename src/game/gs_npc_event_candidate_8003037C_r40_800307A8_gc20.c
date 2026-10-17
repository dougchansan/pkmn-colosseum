/**
 * @file gs_npc_event_candidate_8003037C_r40_800307A8_gc20.c
 * @brief fn_800307A8 carve, 0x800307A8 - 0x800308D4.
 *
 * NPC event callback: look the event id up in the level column of the
 * two-entry table lbl_80266F68, fetch the party (1) or storage (2)
 * Pokemon it names and print its level (status 0x7A).
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void GSmsgPrintRect(s32, s32, s16, s16, u32, u16);
extern u32 pokemonGetStatus(void* pokemon, u32 slot, u16 tableId, u16 flags);

typedef struct {
    u16 kind;
    u16 ids[9];
} NpcPartyEvent;

extern NpcPartyEvent lbl_80266F68[];
extern u8 lbl_803A2688[];
extern u32 lbl_8047A420;
extern u32 lbl_8047A424;

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

void fn_800307A8(u8* r3, u8* r4)
{
    u32 combined;
    void* pokemon;

    combined = (*(u32*)(r4 + 0x64) & ~0xFF) | *(u8*)(r3 + 0x8B);
    pokemon = npcEventGetPokemon(r4, 0);
    if (pokemon != NULL) {
        msgctrlSetValue(0x2F, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
        GSmsgPrintRect(0, 0, *(s16*)(r4 + 0x54), *(s16*)(r4 + 0x56), combined, 0x4414);
        winSpriteSetDisp(r4, 1);
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
