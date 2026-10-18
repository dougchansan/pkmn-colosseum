#include "dolphin/types.h"

extern u32 fightFloorGetGcHeroFightTrainerPtr(s32 side);
extern u32 fightTrainerGetValidFightPokemonPtr(u32 trainer, u16 slot);
extern u32 pokemonGetStatus();
extern u32 pokemonCheckValid();
extern u32 windowGetParam(u8* window, s32 index);
extern u32 menuPokemonGetStatusSprite(u32 pokemon, s32 kind);
extern s32 GSmsgPrintRect(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key);
extern void GSmsgPrintColor();
extern void windowDrawSprite();
extern void* menuSubCalcColor(void*, void*);
extern u32 GSmsgGetRect();
extern void msgctrlSetValue();

static inline u32 menuFightGetSecretPokemon(u32 trainer, s32 slot)
{
    u32 pokemon;
    u16 index = slot;

    if (index >= 6) {
        return 0;
    }
    if (trainer == 0) {
        trainer = fightFloorGetGcHeroFightTrainerPtr(0);
    }
    if (trainer == 0) {
        return 0;
    }
    pokemon = fightTrainerGetValidFightPokemonPtr(trainer, index);
    if (pokemon == 0) {
        return 0;
    }
    pokemon = pokemonGetStatus(pokemon, 0, 0xCC, 0);
    if ((u8)pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    return pokemon;
}

void menuFightDrawSecretPokemon(u8* ctx, u8* npc)
{
    s32 slot;
    u32 trainer;
    u32 pokemon;
    s32 color;
    s32 y;

    slot = -1;
    trainer = windowGetParam(ctx, 0);
    switch (*(s16*)(npc + 6)) {
    case 0x121A:
    case 0x1220:
    case 0x1226:
    case 0x122C:
        slot = 0;
        break;
    case 0x1219:
    case 0x121F:
    case 0x1225:
    case 0x122B:
        slot = 1;
        break;
    case 0x1218:
    case 0x121E:
    case 0x1224:
    case 0x122A:
        slot = 2;
        break;
    case 0x1217:
    case 0x121D:
    case 0x1223:
    case 0x1229:
        slot = 3;
        break;
    case 0x1216:
    case 0x121C:
    case 0x1222:
    case 0x1228:
        slot = 4;
        break;
    case 0x1215:
    case 0x121B:
    case 0x1221:
    case 0x1227:
        slot = 5;
        break;
    }
    if (slot < 0) {
        return;
    }
    pokemon = menuFightGetSecretPokemon(trainer, slot);
    if (pokemon == 0) {
        return;
    }
    color = (s32)menuSubCalcColor(ctx, npc);
    switch (*(s16*)(npc + 6)) {
    case 0x1215:
    case 0x1216:
    case 0x1217:
    case 0x1218:
    case 0x1219:
    case 0x121A:
        windowDrawSprite(0, 0, ctx, menuPokemonGetStatusSprite(pokemon, 1), 0);
        break;
    case 0x121B:
    case 0x121C:
    case 0x121D:
    case 0x121E:
    case 0x121F:
    case 0x1220:
        y = (s16)((s16)(GSmsgGetRect(0x1A8) >> 16) + 2);
        GSmsgPrintColor(0, 0, color, 0x1A8);
        y += (s16)((*(s16*)(npc + 0x54) - y - (s16)(GSmsgGetRect(0x197) >> 16)) / 2);
        GSmsgPrintColor(y, 0, color, 0x197);
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x83, 0));
        GSmsgPrintRect(0, 0, y, *(s16*)(npc + 0x56), color, 0xDE);
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x87, 0));
        GSmsgPrintRect(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xDE);
        break;
    case 0x1221:
    case 0x1222:
    case 0x1223:
    case 0x1224:
    case 0x1225:
    case 0x1226:
        y = (s16)(GSmsgGetRect(0x1A7) >> 16);
        GSmsgPrintColor(0, 0, color, 0x1A7);
        msgctrlSetValue(0x34, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
        GSmsgPrintRect(y, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xD2);
        break;
    case 0x1227:
    case 0x1228:
    case 0x1229:
    case 0x122A:
    case 0x122B:
    case 0x122C: {
        u32 value = pokemonGetStatus(pokemon, 0, 0x77, 0);
        msgctrlSetValue(0x37, value);
        GSmsgPrintRect(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xE7);
        break;
    }
    }
}
