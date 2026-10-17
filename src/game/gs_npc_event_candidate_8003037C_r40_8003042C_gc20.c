/**
 * @file gs_npc_event_candidate_8003037C_r40_8003042C_gc20.c
 * @brief fn_8003042C carve, 0x8003042C - 0x80030574.
 *
 * NPC event callback: look the event id up in the two-entry table
 * lbl_80266F68, fetch the party (1) or storage (2) Pokemon it names and,
 * when that Pokemon holds an item, print the item's name.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void GSmsgPrintColor(s32, s32, u32, u16);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern void* itemDataBiosGetPtr(u16 itemId);
extern u32 itemDataBiosGetName(void* item);
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

void fn_8003042C(u8* r3, u8* r4)
{
    u32 combined;
    void* pokemon;

    combined = npcEventGetMsgFlags(r3, r4);
    pokemon = npcEventGetPokemon(r4, 8);
    if (pokemon != NULL) {
        u16 item = pokemonGetSoubiItemDataId(pokemon);

        if (item != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(itemDataBiosGetName(itemDataBiosGetPtr(item))));
            GSmsgPrintColor(0, 0, combined, 0xE7);
            *(u32*)(r4 + 0x4C) = 0;
            winSpriteSetDisp(r4, 1);
        } else {
            winSpriteSetDisp(r4, 0);
        }
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
