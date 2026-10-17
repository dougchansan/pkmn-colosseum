/**
 * @file menuFight_exact_80010294.c
 * @brief Exact menuFight move-info block, 0x80010294 - 0x80010588:
 *        menuFightDrawPP and menuFightDrawType.
 *
 * Standalone copy of the canonical implementations in menuFight.c, built with
 * the parent unit's GC/1.3 no-peephole flags. Both read the fight-menu window
 * work (four 0xC-byte move slots at +4, the fight Pokemon at +0x40).
 */
#include "dolphin/types.h"

extern void* menuSubCalcColor(void*, void*);
extern void windowDrawSprite();
extern u32 GSmsgGetRect();
extern void GSmsgPrintColor();
extern void msgctrlSetValue();

/* Fight-menu window work: the four move slots shown by DrawPP/DrawType. */
typedef struct MenuFightMoveSlot {
    u32 valid;
    u32 unk_04;
    u16 type;
    u8 unk_0A;
    u8 unk_0B;
} MenuFightMoveSlot;

typedef struct MenuFightMoveWork {
    u32 unk_00;
    MenuFightMoveSlot slots[4];
    u8 unk_34[0xc];
    void* fightPokemon;
} MenuFightMoveWork;

/* menuFightDrawPP - 0x80010294 | size: 0x1e8 */
extern u32 windowGetCursor();
void menuFightDrawPP(u8* ctx, u8* npc) {
    extern u8* windowGetAllocPtr(u8* a);
    extern u32 windowGetCursor(u32 val);
    extern void* fightOutPokemonGetPokemonPtr(void* obj);
    extern u32 pokemonGetStatus(void* pokemon, s32 p2, s32 p3, s32 p4);
    extern void GSmsgPrintRight();
    union {
        u16 raw;
        struct {
            s8 x;
            s8 y;
        } pos;
    } cursor;
    MenuFightMoveWork* entries;
    u32 color;
    s32 x;
    u16 status;
    void* pokemon;

    entries = (MenuFightMoveWork*)windowGetAllocPtr(ctx);
    color = (u32)menuSubCalcColor(ctx, npc);
    cursor.raw = windowGetCursor(*(u32*)(ctx + 4)) >> 16;
    x = *(s16*)(npc + 0x54) - (s16)(GSmsgGetRect(0x197) >> 16);
    x = (s16)(x / 2);
    GSmsgPrintColor(x, 0, color, 0x197);

    if (cursor.pos.y >= 0 && cursor.pos.y < 4) {
        if (entries->slots[cursor.pos.y].valid != 0) {
            pokemon = fightOutPokemonGetPokemonPtr(entries->fightPokemon);
            if (pokemon != 0) {
                status = pokemonGetStatus(pokemon, 0, 0x7f, *(s8*)(ctx + 0x95));
            } else {
                status = 0;
            }
            switch (status) {
            case 0x164:
            case 0:
                GSmsgPrintRight(0, 0, x, *(s16*)(npc + 0x56), color, 0x2BE1);
                GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0x2BE1);
                break;
            case 0x165:
                GSmsgPrintRight(0, 0, x, *(s16*)(npc + 0x56), color, 0x2B6D);
                GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0x2B6D);
                break;
            default:
                msgctrlSetValue(0x34, entries->slots[cursor.pos.y].unk_0B);
                GSmsgPrintRight(0, 0, x, *(s16*)(npc + 0x56), color, 0xD2);
                msgctrlSetValue(0x34, entries->slots[cursor.pos.y].unk_0A);
                GSmsgPrintRight(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56), color, 0xD2);
                break;
            }
        }
    }
}

/* menuFightDrawType - 0x8001047C | size: 0x10c */
void menuFightDrawType(u8* arg1) {
    extern void* windowGetAllocPtr(u8* a);
    extern u32 windowGetCursor(u32 val);
    extern void* fightOutPokemonGetPokemonPtr(void* obj);
    extern u32 pokemonGetStatus(void* pokemon, s32 p2, s32 p3, s32 p4);
    extern void windowDrawSprite(s32 p1, s32 p2, u8* p3, u16 p4, s32 p5);
    union {
        u16 raw;
        struct {
            s8 x;
            s8 y;
        } pos;
    } cursor;
    MenuFightMoveWork* participant;
    u16 status;
    u16 type;
    void* pokemon;

    participant = windowGetAllocPtr(arg1);
    cursor.raw = windowGetCursor(*(u32*)(arg1 + 4)) >> 16;
    if (cursor.pos.y >= 0 && cursor.pos.y < 4) {
        if (participant->slots[cursor.pos.y].valid != 0) {
            pokemon = fightOutPokemonGetPokemonPtr(participant->fightPokemon);
            if (pokemon != 0) {
                status = pokemonGetStatus(pokemon, 0, 0x7f, *(s8*)(arg1 + 0x95));
            } else {
                status = 0;
            }
            switch (status) {
            case 0x164:
            case 0:
                type = 0;
                break;
            case 0x165:
                type = 0x5d;
                break;
            default:
                type = participant->slots[cursor.pos.y].type;
                break;
            }
            if (type != 0) {
                windowDrawSprite(0, 2, arg1, type, 0);
            }
        }
    }
}
