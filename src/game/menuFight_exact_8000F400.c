/**
 * @file menuFight_exact_8000F400.c
 * @brief menuFightDrawSecretWazaDoc, 0x8000F400 - 0x8000F768.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c). GC/1.3
 * -O4,p with the TU's unit-wide -opt nopeephole. The unit owns its switch
 * table, retail's jumptable_802E4CA8 (.data 0x802E4CA8 - 0x802E4CD8).
 */
#include "dolphin/types.h"

extern void* menuSubCalcColor(void*, void*);
extern void  winSpriteSetDisp(void* npc, s32 direction);
extern void  windowDrawSprite();
extern u32   GSmsgGetRect();
extern void  msgctrlSetValue();

#pragma push
#pragma opt_propagation off
void menuFightDrawSecretWazaDoc(u8* ctx, u8* npc) {
    extern u32 windowSearchID(void);
    extern u8* windowGetAllocPtr(void);
    extern u32 windowGetParam(u8* a, s32 b);
    extern u32 fightOutPokemonGetPokemonPtr(void*);
    extern u32 pokemonGetStatus();
    extern u32 pokemonCheckValid(void);
    extern u32 wazaGetStatus();
    extern s32 GSmsgPrintRect(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key);
    extern void GSmsgPrintRight();
    extern void GSmsgPrintColor();
    s16 delta;
    u8* party;
    s32 id;
    u8* state;
    s32 value;
    u32 battle;
    s32 color;
    s16 y0;
    s16 y1;
    u16 result;

    value = -1;
    windowGetParam(ctx, 0);
    if (windowSearchID() == 0) return;
    party = windowGetAllocPtr();
    if (party == NULL) return;
    state = (u8*)windowGetParam(ctx, 1);
    if (*(s32*)state < 0) return;
    battle = fightOutPokemonGetPokemonPtr(*(void**)(party + 0x40));
    if (battle == 0) return;
    result = pokemonGetStatus(battle, 0, 0x7F, (u16)*(s32*)state);
    color = (s32)menuSubCalcColor(ctx, npc);
    id = *(s16*)(npc + 6);

    switch (id) {
    case 0x11F4:
    case 0x11F5:
    case 0x11F6:
    case 0x11F7:
        switch (id) {
        case 0x11F7:
            value = 0;
            break;
        case 0x11F4:
            value = 1;
            break;
        case 0x11F6:
            value = 2;
            break;
        case 0x11F5:
            value = 3;
            break;
        default:
            break;
        }
        if (value == *(s32*)state) {
            winSpriteSetDisp(npc, 1);
        } else {
            winSpriteSetDisp(npc, 0);
        }
        break;
    case 0x11F8: {
        u8* entry = party + (*(s32*)state * 0xC);

        windowDrawSprite(0, 0, ctx,
                         *(u16*)(entry + 0xC), 0);
        break;
    }
    case 0x11F9:
        value = wazaGetStatus(0, (u16)result, 0x22, 0);
        GSmsgPrintRect(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, value);
        break;
    case 0x11FA:
        value = wazaGetStatus(0, (u16)result, 6, 0);
        if ((u32)value <= 1) {
            GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0x2BE2);
        } else {
            msgctrlSetValue(0x34, value);
            GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xD2);
        }
        break;
    case 0x11FC:
        value = wazaGetStatus(0, (u16)result, 7, 0);
        if ((u32)value <= 1) {
            GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0x2BE2);
        } else {
            msgctrlSetValue(0x34, value);
            GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xD2);
        }
        break;
    case 0x11FF: {
        u8* entry = party + (*(s32*)state * 0xC);

        msgctrlSetValue(0x37, *(u32*)(entry + 4));
        GSmsgPrintColor(0, 0, color, 0xCF);
        break;
    }
    case 0x11FE: {
        u8* entry;

        y0 = (s16)(GSmsgGetRect(0x1A4) >> 16);
        delta = (s16)(*(s16*)(npc + 0x54) - y0);
        GSmsgPrintColor(0, 0, color, 0x1A4);
        y1 = (s16)((delta - (s16)(GSmsgGetRect(0x197) >> 16)) / 2);
        GSmsgPrintColor(y0 + y1, 0, color, 0x197);
        entry = party + (*(s32*)state * 0xC);
        msgctrlSetValue(0x34, *(u8*)(entry + 0xF));
        GSmsgPrintRect(y0, 0, y1, *(s16*)(npc + 0x56), color, 0xDE);
        entry = party + (*(s32*)state * 0xC);
        msgctrlSetValue(0x34, *(u8*)(entry + 0xE));
        GSmsgPrintRect(y0, 0, delta, *(s16*)(npc + 0x56), color, 0xDE);
        break;
    }
    default:
        break;
    }
}
#pragma pop
