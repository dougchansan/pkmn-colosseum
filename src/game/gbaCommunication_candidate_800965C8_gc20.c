/**
 * @file gbaCommunication_candidate_800965C8_gc20.c
 * @brief fn_800965C8 (0x800965C8 - 0x80096C48): the Pokemon-status window's
 *        sprite callback (ball, markings, held item, name, sex, state
 *        icons).
 *
 * Function-boundary carve, text only, built as the unit was scored
 * (GC/2.0 -O3, -use_lmw_stmw off) with the peephole pass off, which
 * gbaCommunication.c's file-level `#pragma peephole off` (from fn_80093574
 * on) gives it there. The body is gbaCommunication.c's; keep
 * the two in step. The data it reads (the 0.0f/1.0f at lbl_8047C230 and
 * lbl_8047C208, the ball-sprite table and the render state) stays extern.
 *
 * Matching notes (lane D18):
 * - windowDrawSprite takes the sprite id as u32, as its linked definition
 *   does (window_exact_801040F0.c); a u16 prototype adds clrlwi at both
 *   computed call sites.
 * - the markings case tests a u8 copy of the status (signed test of the
 *   low nibble, unsigned test of the whole byte);
 * - the sex switch has cases 0, 1 and 2 (retail's range compare is 3);
 * - color is -0x100 | window[0x8B] and state is read through an s8 pointer;
 * - the name and its x position reuse ball and msg (named block locals add
 *   two copies).
 * RULE-EXCEPTION(user-approved): per-unit compiler flag and reused locals -
 * see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern u8 lbl_803FB338[];
extern u8 lbl_803FB380[];
extern u8 lbl_80314F98[];
extern u16 lbl_802EED28[];
extern f32 lbl_8047C208;
extern f32 lbl_8047C230;

extern void winSpriteSetDisp();
extern void GSmsgPrintColor();
extern void GSmsgPrintRect();
extern void* pokemonDataBiosGetPtr(u32 id);
extern u8 pokemonBiosGetCatchBallId(void* pokemon);
extern u32 pokemonGetSoubiItemDataId(void* pokemon);
extern u32 pokemonDataBiosGetName(void* bios);
extern u32 GSmsgGetGSchar(u32 id);
extern u32 GSmsgGetRect(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern void windowDrawSprite(s32 x, s32 y, void* win, u32 sprite, u32 data);
extern void* menuModelRender(void* data);
extern void gfxSetFlags(u32 arg);
extern void gfxClearFlags(u32 arg);
extern void gfxSetPrimitive(u32 arg);
extern void gfxSetVtxDesc(void* arg);
extern void gfxLoadTexture(u32 arg0, void* arg1);
extern void gfxBegin(u32 arg);
extern void gfxVtxPos2s16(s32 x, s32 y);
extern void gfxVtxColor4u8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
extern void gfxVtxTexCoord(u32 arg0, f32 s, f32 t);
extern void gfxEnd(void);
extern u32 menuPokemonGetStatusSprite(void* pokemon, u32 arg);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);
extern u32 pokemonGetStatus();

/* 0x800965C8 | size: 0x680 */
void fn_800965C8(void* window, u8* sprite) {
    register s32 color;
    register void* pokemon;
    void* bios;
    s32 state;
    s32 value;
    u32 msg;
    u32 ball;

    pokemon = *(void**)(lbl_803FB380 + 0x0C);
    if (pokemon == NULL) {
        return;
    }

    bios = pokemonDataBiosGetPtr((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0));
    if (bios == NULL) {
        return;
    }

    color = -0x100 | (s32)((u8*)window)[0x8B];
    state = ((s8*)window)[0x95];

    switch (*(s16*)(sprite + 0x06)) {
    case 0xE7: {
        void* texture = menuModelRender(lbl_803FB338);
        if (texture == NULL) {
            return;
        }
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxBegin(2);
        gfxVtxPos2s16(0, 0);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047C230, lbl_8047C230);
        gfxVtxPos2s16(*(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047C208, lbl_8047C208);
        gfxEnd();
        break;
    }
    case 0x107:
    case 0x108:
    case 0x109:
    case 0x10A: {
        u32 mask = 0;
        value = (u8)pokemonGetStatus(pokemon, 0, 0xBB, 0);
        switch (*(s16*)(sprite + 0x06)) {
        case 0x107:
            mask = 8;
            break;
        case 0x108:
            mask = 4;
            break;
        case 0x109:
            mask = 2;
            break;
        case 0x10A:
            mask = 1;
            break;
        }
        value &= mask;
        winSpriteSetDisp(sprite, (u8)value);
        break;
    }
    case 0x10B:
        ball = pokemonBiosGetCatchBallId(pokemon);
        if (ball < 13) {
            windowDrawSprite(0, 0, window, lbl_802EED28[ball], 0);
        }
        break;
    case 0x10C:
        {
            u8 status = pokemonGetStatus(pokemon, 0, 0xB5, 0);

            if ((status & 0xF) != 0) {
                msg = 0xE8;
            } else if (status != 0) {
                msg = 0xE7;
            } else {
                msg = 0;
            }
        }
        windowDrawSprite(0, 0, window, msg, 0);
        break;
    case 0x10D:
        windowDrawSprite(0, 0, window, menuPokemonGetStatusSprite(pokemon, 1), 0);
        break;
    case 0x10E:
        value = pokemonGetSoubiItemDataId(pokemon);
        if ((u16)value != 0) {
            value = 1;
        } else {
            value = 0;
        }
        winSpriteSetDisp(sprite, (u8)value);
        break;
    case 0x551:
        value = pokemonGetSoubiItemDataId(pokemon);
        if ((u16)value != 0) {
            msgctrlSetValue(0x2D, (u16)value);
            GSmsgPrintRect(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BD3);
        }
        break;
    case 0x552: {
        s32 x = (s16)(GSmsgGetRect(*(u32*)(sprite + 0x4C)) >> 16);
        msgctrlSetValue(0x34, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
        GSmsgPrintRect(x, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
        break;
    }
    case 0x554:
        /* RULE-EXCEPTION(user-approved): ball and msg reused for the name and
         * its x position - see docs/RULE_EXCEPTIONS.md */
        ball = pokemonDataBiosGetName(bios);
        msg = (s16)(GSmsgGetRect(0x2BD4) >> 16);
        GSmsgPrintColor(0, 0, color, 0x2BD4);
        if (ball != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(ball));
            GSmsgPrintColor(msg, 0, color, 0xE7);
        }
        break;
    case 0x555: {
        u32 sexMsg;

        msgctrlSetValue(0x37, pokemonGetStatus(pokemon, 0, 0x77, 0));
        GSmsgPrintRect(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xE7);
        value = (s16)(GSmsgGetRect(0xE7) >> 16);
        switch ((u8)menuSubGetPokemonSexForDisp(pokemon)) {
        case 0:
            sexMsg = 0xD67;
            break;
        case 1:
            sexMsg = 0xD68;
            break;
        case 2:
        default:
            sexMsg = 0;
            break;
        }
        if (sexMsg != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(sexMsg));
            GSmsgPrintColor(value, 0, color, 0xCF);
        }
        break;
    }
    case 0xF3:
    case 0x110:
        value = state == 0;
        winSpriteSetDisp(sprite, (u8)value);
        break;
    case 0xF5:
    case 0x112:
        value = state == 1;
        winSpriteSetDisp(sprite, (u8)value);
        break;
    case 0xF4:
    case 0x111:
        value = state == 2;
        winSpriteSetDisp(sprite, (u8)value);
        break;
    case 0x106: {
        u32 disp = 0;
        switch (lbl_803FB380[1]) {
        case 2:
        case 4:
            disp = 1;
            break;
        case 3:
            if (lbl_803FB380[0] & 2) {
                disp = 1;
            }
            break;
        case 5:
            if (*(s32*)(lbl_803FB380 + 0x1C) > 0) {
                disp = 1;
            }
            break;
        case 1:
        case 6:
        case 7:
        default:
            break;
        }
        winSpriteSetDisp(sprite, disp);
        break;
    }
    case 0x598:
        msg = 0;
        switch (lbl_803FB380[1]) {
        case 2:
            msg = 0x2BCF;
            break;
        case 5:
            if (*(s32*)(lbl_803FB380 + 0x1C) > 0) {
                msg = 0x2BD2;
            }
            break;
        case 3:
            if (lbl_803FB380[0] & 2) {
                msg = 0x2BD0;
            }
            break;
        case 4:
            msg = 0x2BD0;
            break;
        case 1:
        case 6:
        case 7:
        default:
            break;
        }
        if (msg != 0) {
            GSmsgPrintColor(0, 0, color, msg);
        }
        break;
    default:
        break;
    }
}
