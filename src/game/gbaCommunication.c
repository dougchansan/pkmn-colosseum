/**
 * @file gbaCommunication.c
 * @brief Candidate gbaCommunication suffix, 0x80092FC8 - 0x800980E0.
 *        0x80091DA4 - 0x80092FC8 is gbaCommunication_candidate_80091DA4.c.
 */
#include "dolphin/types.h"
#include "game/gs_material.h"

#define GBA_DATA_OFFSET 0x20
#define GBA_STATE_PORT 0x4338
#define GBA_STATE_TIMEOUT 0x433C
#define GBA_STATE_PHASE 0x4340
#define GBA_THREAD_PRIORITY 8

extern u32 lbl_8047A690;
extern u32 lbl_8047A694;
extern f32 lbl_8047C1D0; /* 0.833333313f -- PAL-adjusted 1-unit wait */
extern f32 lbl_8047C1D4; /* 0.0f */
extern f32 lbl_8047C1D8; /* 1.0f */
extern f32 lbl_8047C1DC; /* 83.3333282f -- PAL-adjusted 100-unit wait */
extern f32 lbl_8047C1E0; /* {41.6666641f, 0.0f} -- PAL-adjusted 50-unit wait */

/* Additional data labels referenced by the ported gba_comm_ext.c /
 * late_game.c bodies below (GBA link-cable state machine + battle-status
 * window helpers living in this same address range per the current
 * object map). */
extern u8 lbl_803FB328[];
extern u8 lbl_803FB338[];
extern u8 lbl_803FB380[];
extern u8 lbl_8047C1E8;
extern u8 lbl_8026F5A8[];
extern u8 lbl_8026F5C0[];
extern u8 lbl_8026F5E4[];
extern u8 lbl_80314F98[];
extern u16 lbl_802EED28[];
extern u32 lbl_802EEEC4[];
extern s32 lbl_802EEFC4[5];

/* Common callees needed by the ported bodies below that are not already
 * declared with a full prototype at the point of use. */
extern void _threadSwitch(void);
extern void* windowSearchID(u32 id);
extern void fn_8009F7B4(void *p);
extern void fn_8009F890(void *p);
extern void fn_800A257C(void *p, u32 b);
extern void fn_800716E8(u32 port, u32 val);
extern void fn_8009FABC(void *p);
extern void fn_800A1E54(void *p, u32 v);
extern void fn_800716C8(u32 port, void *a, void *b);
extern u32 fn_800E202C(void *p);
extern void __assert(const u8 *file, u32 line, const u8 *msg);
extern void fn_800E24B0(u32 status);
extern void fn_800E209C(u32 status);
extern u32 fn_800A13F8(void);
extern void OSYieldThread(void);
extern void std098_mapWarpToMenuRoom(u32 id);
extern void floorSetFadeScript(u32 a, u32 b);
extern u32 GSresGetResource(u32 ctx, u32 id);

/* Storage used by the battle-status window renderer below. */
extern u8 lbl_8047C200;
extern u8 lbl_8047C204;
extern f32 lbl_8047C208;
extern f32 lbl_8047C20C;
extern f32 lbl_8047C210;
extern f32 lbl_8047C214;
extern f32 lbl_8047C218;
extern f32 lbl_8047C21C;
extern u8 lbl_8047C220;
extern u8 lbl_8047C228;
extern f32 lbl_8047C230;
extern f32 lbl_8047C234;
extern f32 lbl_8047C238;
extern void fn_801040F0();
extern void winSpriteSetDisp();
extern void fn_8001E58C();
extern void fn_800FA280();
extern void fn_800FA444();
extern void fn_800FB680();
extern void fn_800FB8C8();
extern void fn_800FBB34();
extern s32 fn_800FAEF8(s32 x, s32 y, u32 color, const void* format, ...);
extern u8 exribbonGetNo(s32 ribbon);
extern void* menuItemBiosGetPtr(u16 id);
extern void* menuSpriteBiosGetPtr(u16 id);

extern u16* windowGetKeyInfo(void);
extern void* pokemonDataBiosGetPtr(u32 id);
extern u8 pokemonBiosGetCatchBallId(void* pokemon);
extern u32 pokemonGetSoubiItemDataId(void* pokemon);
extern u32 pokemonDataBiosGetName(void* bios);
extern u32 GSmsgGetGSchar(u32 id);
extern u32 GSmsgGetRect(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern void windowDrawSprite(s32 x, s32 y, void* win, u32 sprite, u32 data);
extern void windowDrawSprite2(s32 x, s32 y, s16 w, s16 h, s32 color, s32 data, s32 sprite, s32 arg7);
extern void* menuModelRender(void* data);
extern void gfxSetFlags(u32 arg);
extern void gfxClearFlags(u32 arg);
extern void gfxSetPrimitive(u32 arg);
extern void gfxSetVtxDesc(void* arg);
extern void gfxLoadTexture(u32 arg0, void* arg1);
extern void gfxBegin(u32 arg);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
extern void gfxVtxTexCoord(u32 arg0, f32 s, f32 t);
extern void gfxEnd(void);
extern u32 fn_8001D624(void* pokemon, u32 arg);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);

#if !defined(GBA_COMMUNICATION_DECLS_ONLY)

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

typedef struct RibbonSpriteInfo {
    u16 spriteId;
    u16 itemId;
    u32 resource;
    s32 ribbon;
} RibbonSpriteInfo;

extern RibbonSpriteInfo lbl_802EED44[];

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80093B4C(s32 context, void* widget)
{
    s32 index;
    s32 resource;
    s32 column;
    s16 x;
    u8* item;
    u8 value;
    s16 y;
    s32 color;
    u8* sprite;
    s32 row;
    s32 ribbon;

    if (windowSearchID(0x53) == 0) {
        return;
    }
    if (*(void**)(lbl_803FB380 + 0xC) == 0) {
        return;
    }

    color = *(u8*)((u8*)context + 0x8B) | ~0xFF;
    switch (*(s16*)((u8*)widget + 6)) {
    case 0x1F7:
        msgctrlSetValue(0x34, *(u32*)(lbl_803FB380 + 0x1C));
        fn_800FBB34(0, 0, *(s16*)((u8*)widget + 0x54),
                    *(s16*)((u8*)widget + 0x56), color, 0xDD);
        break;

    case 0x1F6:
        fn_800FAEF8(0x78, 0xC8, 0x80FFFF, lbl_8026F5E4,
                    *(s8*)(lbl_803FB380 + 0x1A));
        if (lbl_803FB380[1] != 6) {
            break;
        }

        column = *(s8*)(lbl_803FB380 + 0x1A) % 9;
        row = *(s8*)(lbl_803FB380 + 0x1A) / 9;
        ribbon = *(s8*)(lbl_803FB380 + column * 4 + 0x20 + row);
        if (ribbon < 0 && (u32)ribbon >= 0x20) {
            break;
        }

        fn_800FAEF8(0x78, 0xC8, 0x80FFFF, lbl_8026F5E4, ribbon);
        switch (lbl_802EED44[ribbon].ribbon) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            value = exribbonGetNo(lbl_802EED44[ribbon].ribbon);
            if (value != 0) {
                resource = *(s32*)((u8*)lbl_802EEEC4 + (value - 1) * 4);
            } else {
                resource = 0;
            }
            break;
        case -1:
            resource = lbl_802EED44[ribbon].resource;
            break;
        }
        fn_800FBB34(0, 0, *(s16*)((u8*)widget + 0x54),
                    *(s16*)((u8*)widget + 0x56), color, resource);
        break;

    case 0x1291:
        for (row = 0; row < 4; row++) {
            for (column = 0; column < 9; column++) {
                index = row * 9 + column;
                ribbon = (s8)lbl_803FB380[0x20 + row + column * 4];
                if (ribbon >= 0) {
                    item = menuItemBiosGetPtr(lbl_802EED44[ribbon].itemId);
                    if (index != *(s8*)(lbl_803FB380 + 0x1A)) {
                        windowDrawSprite2(
                            (s16)(*(s16*)((u8*)widget + 0x54) * column / 9),
                            (s16)(*(s16*)((u8*)widget + 0x56) * row / 4),
                            *(s16*)(item + 6), *(s16*)(item + 8), color,
                            context, lbl_802EED44[ribbon].spriteId, 0);
                    }
                }
            }
        }

        if (*(s8*)(lbl_803FB380 + 0x1A) < 0) {
            break;
        }
        column = *(s8*)(lbl_803FB380 + 0x1A) % 9;
        row = *(s8*)(lbl_803FB380 + 0x1A) / 9;
        ribbon = (s8)(lbl_803FB380 + column * 4)[0x20 + row];
        if (ribbon < 0) {
            break;
        }

        item = menuItemBiosGetPtr(lbl_802EED44[ribbon].itemId);
        x = *(s16*)((u8*)widget + 0x54) * column / 9;
        y = *(s16*)((u8*)widget + 0x56) * row / 4;
        sprite = menuSpriteBiosGetPtr(lbl_802EED44[ribbon].spriteId);
        x -= (s16)(*(s16*)(sprite + 0xC) - *(s16*)(item + 6)) / 2;
        y -= (s16)(*(s16*)(sprite + 0xE) - *(s16*)(item + 8)) / 2;
        windowDrawSprite2(x, y, *(s16*)(sprite + 0xC), *(s16*)(sprite + 0xE), color,
                          context, lbl_802EED44[ribbon].spriteId, 0);
        break;
    }
}
#pragma pop

typedef struct MenuStatusColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} MenuStatusColor;

/* Move in slot `slot` of `pokemon`; slot 4 is the move being learned. */
static inline u16 menuStatusGetMove(u32 pokemon, int slot)
{
    extern u32 pokemonGetStatus();
    extern u8 pokemonWazaCheckValid(u32 pokemon, s32 waza);
    u16 move;

    if ((u16)slot == 4) {
        move = *(u16*)(lbl_803FB380 + 0x18);
    } else {
        move = pokemonGetStatus(pokemon, 0, 0x7F, slot);
        if (pokemonWazaCheckValid(pokemon, slot) == 0) {
            move = 0;
        }
    }
    return move;
}

/* Move-details window renderer (type, power, accuracy, PP, category). */
/* Show the move-detail sprites only while a move slot is selected. */
static inline u8 menuStatusMoveDetailVisible(u8* sprite)
{
    extern void winSpriteSetDisp(u8* sprite, u8 disp);
    u8 visible;

    visible = 1;
    switch (*(s16*)(sprite + 6)) {
    case 0x170:
    case 0x171:
    case 0x172:
    case 0x173:
    case 0x174:
    case 0x175:
    case 0x176:
    case 0x177:
    case 0x178:
    case 0x179:
    case 0x17A:
    case 0x17B:
    case 0x17C:
    case 0x17D:
    case 0x17E:
    case 0x17F:
    case 0x180:
    case 0x181:
    case 0x18B:
    case 0x18C:
    case 0x18D:
    case 0x18E:
    case 0x18F:
    case 0x190:
    case 0x1B8:
    case 0x1B9:
    case 0x1BA:
    case 0x1BB:
    case 0x1BC:
    case 0x1BD:
    case 0x1BE:
    case 0x1BF:
    case 0x1C0:
    case 0x1C1:
    case 0x1C2:
    case 0x1C3:
    case 0x1C4:
    case 0x1C5:
    case 0x1C6:
    case 0x1C7:
    case 0x1C8:
    case 0x1C9:
    case 0x1D3:
    case 0x1D4:
    case 0x1D5:
    case 0x1D6:
    case 0x1D7:
    case 0x1D8:
        switch (lbl_803FB380[1]) {
        case 3:
        case 4:
        case 7:
            if ((s8)lbl_803FB380[2] >= 0 && (s8)lbl_803FB380[2] <= 4) {
                visible = 1;
            } else {
                visible = 0;
            }
            break;
        default:
            visible = 0;
            break;
        }
        winSpriteSetDisp(sprite, visible);
        break;
    }
    return visible;
}

void menuStatusDrawMoveDetail(u8* context, u8* sprite)
{
    extern u32 pokemonGetStatus();
    extern u32 wazaGetStatus(u32, u16, u32, u32);
    extern u16 fn_8010C46C(u16 type);
    extern u8 pokemonWazaGetMaxPP(u32 pokemon, u16 slot);
    extern u16 fn_801EE07C(u16 id);
    extern u32 fn_801EE034(u16 id);
    extern u8 fn_801EE04C(u16 id);
    extern u8 fn_801EE064(u16 id);
    extern u16 fn_801EE0A8(u8 kind);
    extern void fn_8001E58C(s32 x, s32 y, s32 w, s32 h, MenuStatusColor color);
    extern void fn_800FB8C8();
    extern void windowDrawSprite(s32 x, s32 y, void* win, u16 sprite, u32 data);
    s32 color;
    u16 slot;
    u32 pokemon;
    u16 move;
    u32 icon;
    u32 value;
    s16 x;
    s32 level;

    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if (pokemon == 0) {
        return;
    }

    if (menuStatusMoveDetailVisible(sprite) == 0) {
        return;
    }

    color = -0x100 | context[0x8B];
    switch (*(s16*)(sprite + 6)) {
    case 0x59B:
    case 0x59C:
    case 0x59D:
    case 0x59E:
    case 0x12B3:
    case 0x12B4:
    case 0x12B5:
    case 0x12B6:
    case 0x12B7: {
        MenuStatusColor learn = *(MenuStatusColor*)&lbl_8047C200;
        MenuStatusColor cursor = *(MenuStatusColor*)&lbl_8047C204;

        switch (*(s16*)(sprite + 6)) {
        case 0x59B:
            slot = 0;
            break;
        case 0x59C:
            slot = 1;
            break;
        case 0x59D:
            slot = 2;
            break;
        case 0x59E:
            slot = 3;
            break;
        case 0x12B7:
            slot = 0;
            break;
        case 0x12B3:
            slot = 1;
            break;
        case 0x12B4:
            slot = 2;
            break;
        case 0x12B5:
            slot = 3;
            break;
        case 0x12B6:
            slot = 4;
            break;
        }
        switch (lbl_803FB380[1]) {
        case 4:
            if ((s8)lbl_803FB380[3] == (s8)slot) {
                fn_8001E58C(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), learn);
            }
            if ((s8)lbl_803FB380[2] == (s8)slot) {
                fn_8001E58C(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), cursor);
            }
            break;
        case 3:
        case 7:
            if ((s8)lbl_803FB380[2] == (s8)slot) {
                fn_8001E58C(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), cursor);
            }
            break;
        }
        break;
    }
    case 0x182:
    case 0x183:
    case 0x184:
    case 0x185:
    case 0x186:
    case 0x1CA:
    case 0x1CB:
    case 0x1CC:
    case 0x1CD:
        switch (*(s16*)(sprite + 6)) {
        case 0x1CD:
            slot = 0;
            break;
        case 0x1CC:
            slot = 1;
            break;
        case 0x1CB:
            slot = 2;
            break;
        case 0x1CA:
            slot = 3;
            break;
        case 0x186:
            slot = 0;
            break;
        case 0x185:
            slot = 1;
            break;
        case 0x184:
            slot = 2;
            break;
        case 0x183:
            slot = 3;
            break;
        case 0x182:
            slot = 4;
            break;
        }
        move = menuStatusGetMove(pokemon, slot);
        switch (move) {
        case 0:
        case 0x164:
            icon = 0;
            break;
        case 0x165:
            icon = 0x5D;
            break;
        default:
            icon = fn_8010C46C(wazaGetStatus(0, move, 3, 0));
            break;
        }
        if (icon != 0) {
            windowDrawSprite(0, 0, context, icon, 0);
        }
        break;
    case 0x191:
    case 0x192:
    case 0x193:
    case 0x194:
    case 0x195:
    case 0x1D9:
    case 0x1DA:
    case 0x1DB:
    case 0x1DC:
        switch (*(s16*)(sprite + 6)) {
        case 0x1DC:
            slot = 0;
            break;
        case 0x1DB:
            slot = 1;
            break;
        case 0x1DA:
            slot = 2;
            break;
        case 0x1D9:
            slot = 3;
            break;
        case 0x195:
            slot = 0;
            break;
        case 0x194:
            slot = 1;
            break;
        case 0x193:
            slot = 2;
            break;
        case 0x192:
            slot = 3;
            break;
        case 0x191:
            slot = 4;
            break;
        }
        move = menuStatusGetMove(pokemon, slot);
        if (move == 0) {
            fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BE0);
            break;
        }
        value = wazaGetStatus(0, move, 1, 0);
        if (value == 0) {
            break;
        }
        msgctrlSetValue(0x37, GSmsgGetGSchar(value));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xE7);
        break;
    case 0x196:
    case 0x197:
    case 0x198:
    case 0x199:
    case 0x19A:
    case 0x1DD:
    case 0x1DE:
    case 0x1DF:
    case 0x1E0:
        switch (*(s16*)(sprite + 6)) {
        case 0x1E0:
            slot = 0;
            break;
        case 0x1DF:
            slot = 1;
            break;
        case 0x1DE:
            slot = 2;
            break;
        case 0x1DD:
            slot = 3;
            break;
        case 0x19A:
            slot = 0;
            break;
        case 0x199:
            slot = 1;
            break;
        case 0x198:
            slot = 2;
            break;
        case 0x197:
            slot = 3;
            break;
        case 0x196:
            slot = 4;
            break;
        }
        move = menuStatusGetMove(pokemon, slot);
        x = (*(s16*)(sprite + 0x54) - (s16)(GSmsgGetRect(0x2BD4) >> 16)) / 2;
        fn_800FB680(x, 0, color, 0x2BD4);
        switch (move) {
        case 0:
        case 0x164:
            fn_800FB8C8(0, 0, x, *(s16*)(sprite + 0x56), color, 0x2BE1);
            fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BE1);
            break;
        case 0x165:
            fn_800FB8C8(0, 0, x, *(s16*)(sprite + 0x56), color, 0x2B6D);
            fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2B6D);
            break;
        default:
            if (slot == 4) {
                value = wazaGetStatus(0, move, 2, 0);
            } else {
                value = pokemonGetStatus(pokemon, 0, 0x80, slot);
            }
            msgctrlSetValue(0x34, value);
            fn_800FB8C8(0, 0, x, *(s16*)(sprite + 0x56), color, 0xD2);
            if (slot == 4) {
                value = wazaGetStatus(0, move, 2, 0);
            } else {
                value = pokemonWazaGetMaxPP(pokemon, slot);
            }
            msgctrlSetValue(0x34, value);
            fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
            break;
        }
        break;
    case 0x170:
    case 0x1B8:
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        switch (move) {
        case 0:
        case 0x164:
            icon = 0;
            break;
        case 0x165:
            icon = 0x5D;
            break;
        default:
            icon = fn_801EE0A8(wazaGetStatus(0, move, 0x24, 0));
            break;
        }
        if (icon != 0) {
            windowDrawSprite(0, 0, context, icon, 0);
        }
        break;
    case 0x18B:
    case 0x1D3:
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color,
                    fn_801EE034(fn_801EE07C(wazaGetStatus(0, move, 0x23, 0))));
        break;
    case 0x18C:
    case 0x1D4:
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color,
                    wazaGetStatus(0, move, 0x22, 0));
        break;
    case 0x18D:
    case 0x1D5:
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        value = wazaGetStatus(0, move, 6, 0);
        if (value <= 1) {
            fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BE2);
            break;
        }
        msgctrlSetValue(0x34, value);
        fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
        break;
    case 0x18F:
    case 0x1D7:
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        value = wazaGetStatus(0, move, 7, 0);
        if (value <= 1) {
            fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BE2);
            break;
        }
        msgctrlSetValue(0x34, value);
        fn_800FB8C8(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
        break;
    case 0x179:
    case 0x17A:
    case 0x17B:
    case 0x17C:
    case 0x17D:
    case 0x17E:
    case 0x17F:
    case 0x180:
    case 0x1C1:
    case 0x1C2:
    case 0x1C3:
    case 0x1C4:
    case 0x1C5:
    case 0x1C6:
    case 0x1C7:
    case 0x1C8:
        switch (*(s16*)(sprite + 6)) {
        case 0x1C8:
            slot = 1;
            break;
        case 0x1C7:
            slot = 2;
            break;
        case 0x1C6:
            slot = 3;
            break;
        case 0x1C5:
            slot = 4;
            break;
        case 0x1C4:
            slot = 5;
            break;
        case 0x1C3:
            slot = 6;
            break;
        case 0x1C2:
            slot = 7;
            break;
        case 0x1C1:
            slot = 8;
            break;
        case 0x180:
            slot = 1;
            break;
        case 0x17F:
            slot = 2;
            break;
        case 0x17E:
            slot = 3;
            break;
        case 0x17D:
            slot = 4;
            break;
        case 0x17C:
            slot = 5;
            break;
        case 0x17B:
            slot = 6;
            break;
        case 0x17A:
            slot = 7;
            break;
        case 0x179:
            slot = 8;
            break;
        }
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        if (move != 0) {
            level = fn_801EE064(fn_801EE07C(wazaGetStatus(0, move, 0x23, 0)));
        } else {
            level = 0;
        }
        windowDrawSprite(0, 0, context, (level / 10 >= slot) ? (u16)0xF6 : (u16)0xF5, 0);
        break;
    case 0x171:
    case 0x172:
    case 0x173:
    case 0x174:
    case 0x175:
    case 0x176:
    case 0x177:
    case 0x178:
    case 0x1B9:
    case 0x1BA:
    case 0x1BB:
    case 0x1BC:
    case 0x1BD:
    case 0x1BE:
    case 0x1BF:
    case 0x1C0:
        switch (*(s16*)(sprite + 6)) {
        case 0x1C0:
            slot = 1;
            break;
        case 0x1BF:
            slot = 2;
            break;
        case 0x1BE:
            slot = 3;
            break;
        case 0x1BD:
            slot = 4;
            break;
        case 0x1BC:
            slot = 5;
            break;
        case 0x1BB:
            slot = 6;
            break;
        case 0x1BA:
            slot = 7;
            break;
        case 0x1B9:
            slot = 8;
            break;
        case 0x178:
            slot = 1;
            break;
        case 0x177:
            slot = 2;
            break;
        case 0x176:
            slot = 3;
            break;
        case 0x175:
            slot = 4;
            break;
        case 0x174:
            slot = 5;
            break;
        case 0x173:
            slot = 6;
            break;
        case 0x172:
            slot = 7;
            break;
        case 0x171:
            slot = 8;
            break;
        }
        move = menuStatusGetMove(pokemon, (s8)lbl_803FB380[2]);
        if (move != 0) {
            level = fn_801EE04C(fn_801EE07C(wazaGetStatus(0, move, 0x23, 0)));
        } else {
            level = 0;
        }
        windowDrawSprite(0, 0, context, (level / 10 >= slot) ? (u16)0xF7 : (u16)0xF5, 0);
        break;
    }
}



/* 0x80091564 | size: 0x210 */
#pragma push
#pragma peephole off
/* Whether the shown Pokemon still belongs to its original trainer. */
static inline u8 menuStatusIsOriginalTrainer(u8* pokemon)
{
    extern u32 pokemonGetStatus();
    extern u8 gamedataGetStatus(u32 id, s32 field);
    extern u32 fightFloorGetGcHeroFightTrainerPtr(s32 index);
    extern u32 fightTrainer_GetHeroPtr(u32 trainer);
    extern u32 std082_flagGetValue(u32 flag);
    extern void* savedataGetStatus();
    extern u32 fn_8006AEEC(void);
    extern u32 heroBiosGetNamePtr(u32 hero);
    extern u32 heroBiosGetRnd(u32 hero);
    extern s32 GScharCmp(u32 a, u32 b);
    u32 hero;
    u32 name;

    hero = *(u32*)(lbl_803FB380 + 8);
    if (pokemon == NULL) {
        return 0;
    }
    if (gamedataGetStatus(pokemonGetStatus(pokemon, 0, 0x70, 0), 2) != 0xB) {
        return 0;
    }
    if (lbl_803FB380[0] & 0x20) {
        if (hero == 0) {
            hero = fightFloorGetGcHeroFightTrainerPtr(0);
        }
        if (hero == 0) {
            return 0;
        }
        hero = fightTrainer_GetHeroPtr(hero);
    } else if (std082_flagGetValue(0x8AE) == 0) {
        hero = (u32)savedataGetStatus(0, 2);
    } else {
        hero = fn_8006AEEC();
    }
    name = heroBiosGetNamePtr(hero);
    hero = heroBiosGetRnd(hero);
    if (hero == pokemonGetStatus(pokemon, 0, 0x75, 0) &&
        GScharCmp(name, pokemonGetStatus(pokemon, 0, 0x76, 0)) == 0) {
        return 1;
    }
    return 0;
}

/* Battle-status detail window renderer. */
void menuStatusDrawPokemonInfo(u8* context, u8* sprite)
{
    extern u32 pokemonGetStatus();
    extern void* pokemonDataBiosGetPtr();
    extern u8 pokemonDataBiosGetZokuseiDataId(void* data, s32 index);
    extern u16 fn_8010C46C();
    extern u8 pokemonGetDarkPokemonLevel(u8* pokemon);
    extern void* pokemonSeikakuDataBiosGetPtr(u8 seikaku);
    extern u32 pokemonSeikakuDataBiosGetName(void* data);
    extern u8 gamedataGetStatus(u32 id, s32 field);
    extern u32 GSmsgGetGSchar(u32 id);
    extern s32 GScharCmp(u32 a, u32 b);
    extern u32 pokemonGetLevelToExp(u8* pokemon, u8 level);
    extern u16 pokemonGetTokuseiDataId(u8* pokemon);
    extern void* pokemonTokuseiDataBiosGetPtr();
    extern u32 pokemonTokuseiDataBiosGetDoc(void* data);
    extern u32 pokemonTokuseiDataBiosGetName(void* data);
    extern u8 pokemonIsDarkPokemon(u8* pokemon);
    extern f32 pokemonGetDp(u8* pokemon);
    extern u32 fn_8011396C(u32 id);
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void winSpriteSetDisp(u8* sprite, u8 disp);
    s32 i;
    void* pokemon_data;
    s32 color;
    u32 value;
    u32 message;
    u32 name;
    u32 next_exp;
    u32 current_exp;
    u32 range;
    s32 x;
    s32 pos;
    u32 count;
    u32 text;
    u8* pokemon;
    u32 divisor;
    u8 level;
    u8 match;
    u16 slot;
    u16 gauge;
    u16 fill;
    f32 ratio;
    f32 dp;

    pokemon = *(u8**)(lbl_803FB380 + 0x0C);
    if (pokemon == NULL) {
        return;
    }

    pokemon_data = pokemonDataBiosGetPtr((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0));
    if (pokemon_data == NULL) {
        return;
    }

    if (menuStatusMoveDetailVisible(sprite) == 0) {
        return;
    }

    color = -0x100 | context[0x8B];
    switch (*(s16*)(sprite + 6)) {
    case 0x13B:
    case 0x57B:
        windowDrawSprite(0, 0, context,
                         fn_8010C46C((u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 0)), 0);
        break;
    case 0x13C:
    case 0x57D:
        value = (u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 0);
        message = (u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 1);
        if (value != message) {
            windowDrawSprite(0, 0, context, fn_8010C46C(message), 0);
        }
        break;
    case 0x142:
    case 0x581:
        if (pokemonGetDarkPokemonLevel(pokemon) < 3) {
            name = 0x934;
        } else {
            name = pokemonSeikakuDataBiosGetName(
                pokemonSeikakuDataBiosGetPtr((u8)pokemonGetStatus(pokemon, 0, 0xBF, 0)));
        }
        msgctrlSetValue(0x55, name);
        switch (name) {
        case 0xC86:
        case 0xC96:
            msgctrlSetValue(0x56, 1);
            break;
        default:
            msgctrlSetValue(0x56, 0x2BD8);
            break;
        }
        count = (u8)pokemonGetStatus(pokemon, 0, 0x72, 0);
        if (count == 0) {
            count = 5;
        }
        msgctrlSetValue(0x34, count);
        if (menuStatusIsOriginalTrainer(pokemon)) {
            switch ((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0)) {
            case 0xC4:
            case 0xC5:
                text = 0x2BE3;
                break;
            default:
                text = 0x2BCD;
                break;
            }
        } else {
            text = 0x2BCD;
        }
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, text);
        break;
    case 0x54E:
    case 0x599:
        message = 0x2BE6;
        value = pokemonGetStatus(pokemon, 0, 0x71, 0);
        name = fn_8011396C(value);
        if (menuStatusIsOriginalTrainer(pokemon)) {
            switch ((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0)) {
            case 0xC4:
            case 0xC5:
                message = 0x2BE4;
                break;
            default:
                if (name != 0) {
                    msgctrlSetValue(0x37, GSmsgGetGSchar(name));
                    message = 0x2BD7;
                }
                break;
            }
        } else {
            switch (gamedataGetStatus(pokemonGetStatus(pokemon, 0, 0x70, 0), 2)) {
            case 0xB:
                if ((s32)pokemonGetStatus(pokemon, 0, 0x75, 0) == 0x911D &&
                    GScharCmp(pokemonGetStatus(pokemon, 0, 0x76, 0),
                              GSmsgGetGSchar(0x12AC)) == 0) {
                    match = 1;
                } else {
                    match = 0;
                }
                if (match) {
                    message = 0x2BE7;
                } else if (value == 0xFF) {
                    message = 0x2BE5;
                } else if (name != 0) {
                    msgctrlSetValue(0x37, GSmsgGetGSchar(name));
                    message = 0x2BD7;
                }
                break;
            case 8:
            case 9:
            case 10:
                if (value == 0xFF) {
                    message = 0x2BE5;
                }
                break;
            }
        }
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, message);
        break;
    case 0x582:
        switch (pokemonGetDarkPokemonLevel(pokemon)) {
        case 0:
            message = 0x2BD9;
            break;
        case 1:
            message = 0x2BDA;
            break;
        case 2:
            message = 0x2BDB;
            break;
        case 3:
            message = 0x2BDC;
            break;
        case 4:
            message = 0x2BDD;
            break;
        case 5:
            message = 0x2BDE;
            break;
        case 6:
            message = 0x2BDF;
            break;
        }
        fn_800FB680(0, 0, color, message);
        break;
    case 0x143:
        level = pokemonGetStatus(pokemon, 0, 0x7A, 0);
        next_exp = pokemonGetLevelToExp(pokemon, level + 1);
        if (next_exp == 0) {
            value = 0;
        } else {
            value = next_exp - pokemonGetStatus(pokemon, 0, 0x79, 0);
        }
        msgctrlSetValue(0x34, value);
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x144:
        msgctrlSetValue(0x34, pokemonGetStatus(pokemon, 0, 0x79, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x147:
    case 0x583:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x8C, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x148:
    case 0x584:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x8B, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x149:
    case 0x585:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x8A, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x14A:
    case 0x586:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x89, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x14B:
    case 0x587:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x88, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x14C:
    case 0x588:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x83, 0));
        fn_800FBB34(0, 0, 0x37, *(s16*)(sprite + 0x56), color, 0xDE);
        fn_800FB680(0x37, 0, color, 0x2BD4);
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, 0x87, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xDE);
        break;
    case 0x153:
    case 0x58F:
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonTokuseiDataBiosGetDoc(
                                  pokemonTokuseiDataBiosGetPtr(pokemonGetTokuseiDataId(pokemon)))));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xCF);
        break;
    case 0x54D:
    case 0x590:
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonTokuseiDataBiosGetName(
                                  pokemonTokuseiDataBiosGetPtr(pokemonGetTokuseiDataId(pokemon)))));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xCF);
        break;
    case 0x154:
    case 0x591:
        x = (s16)(GSmsgGetRect(*(u32*)(sprite + 0x4C)) >> 16);
        if (pokemonIsDarkPokemon(pokemon) == 1) {
            fn_800FB680(x, 0, color, 0x2B70);
            break;
        }
        value = (u16)pokemonGetStatus(pokemon, 0, 0x75, 0);
        pos = x;
        divisor = 10000;
        for (i = 0; i < 5; i++) {
            next_exp = value / divisor;
            value %= divisor;
            divisor /= 10;
            msgctrlSetValue(0x34, next_exp);
            fn_800FB680(pos, 0, color, 0xCA);
            pos += 13;
        }
        break;
    case 0x155:
    case 0x592:
        if (pokemonIsDarkPokemon(pokemon) == 1) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(0x2B70));
        } else {
            msgctrlSetValue(0x37, pokemonGetStatus(pokemon, 0, 0x76, 0));
        }
        fn_800FB680((s16)(GSmsgGetRect(*(u32*)(sprite + 0x4C)) >> 16), 0, color, 0xCF);
        break;
    case 0x158:
        level = pokemonGetStatus(pokemon, 0, 0x7A, 0);
        next_exp = pokemonGetLevelToExp(pokemon, level + 1);
        if (next_exp == 0) {
            break;
        }
        current_exp = pokemonGetLevelToExp(pokemon, level);
        range = next_exp - current_exp;
        value = pokemonGetStatus(pokemon, 0, 0x79, 0) - current_exp;
        windowDrawSprite2(0, 0, (range - 1 + value * *(s16*)(sprite + 0x54)) / range,
                          *(s16*)(sprite + 0x56), color, (s32)context, 0x117, 0);
        break;
    case 0x595:
    case 0x12B8:
    case 0x12B9:
    case 0x12BA:
    case 0x12BB:
        switch (*(s16*)(sprite + 6)) {
        case 0x595:
            slot = 0;
            break;
        case 0x12B8:
            slot = 1;
            break;
        case 0x12B9:
            slot = 2;
            break;
        case 0x12BA:
            slot = 3;
            break;
        case 0x12BB:
            slot = 4;
            break;
        }
        gauge = pokemonGetStatus(pokemon, 0, 0xC4, 0);
        if (gauge == 0) {
            ratio = lbl_8047C208;
        } else {
            dp = pokemonGetDp(pokemon);
            if (dp > gauge) {
                dp = gauge;
            }
            ratio = dp / gauge;
        }
        if (ratio >= lbl_8047C20C) {
            fill = 4;
        } else if (ratio >= lbl_8047C210) {
            fill = 3;
        } else if (ratio >= lbl_8047C214) {
            fill = 2;
        } else if (ratio >= lbl_8047C218) {
            fill = 1;
        } else {
            fill = 0;
        }
        if (fill > slot) {
            windowDrawSprite2(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color,
                              (s32)context, 0x116, 0);
        } else if (fill == slot) {
            ratio -= lbl_8047C218 * fill;
            windowDrawSprite2(0, 0, (u32)(*(s16*)(sprite + 0x54) * (lbl_8047C21C * ratio)),
                              *(s16*)(sprite + 0x56), color, (s32)context, 0x116, 0);
        }
        break;
    }
}



#pragma pop
/* 0x80091DA4 - 0x80092C90: defined in gbaCommunication_candidate_80091DA4.c.
 * The peephole pass stayed off from fn_80092140 onward; keep that state for
 * the code below. */

#endif

#if !defined(GBA_COMMUNICATION_REMAINDER_80092FC8)

/* 0x80092FC8 | size: 0x198 */
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
s32 fn_80092FC8(s32 channel, void* requestValue, void* requestContext)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 4;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x30004;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
        *(void**)(requestWork + 0x4348) = requestContext;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif

/* 0x80093160 | size: 0x190 */
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
s32 fn_80093160(s32 channel, void* requestValue)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 2;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x30002;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif

/* 0x800932F0 | size: 0x1F4 */
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
s32 fn_800932F0(s32 channel, const char* primary, const char* secondary)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern u32 strlen(const char* string);
    extern char* strcpy(char* dst, const char* src);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u32 primaryLength;
    u32 secondaryLength;
    u8* allocated;
    u8* work;
    s32 started;
    s32 commandStarted;
    u8* commandWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    commandWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    commandStarted = 0;
    primaryLength = strlen(primary);
    if (secondary != NULL) {
        secondaryLength = strlen(secondary);
    } else {
        secondaryLength = 0;
    }

    if (primaryLength >= 0x7F || secondaryLength >= 0x7F) {
        commandStarted = 0;
        goto done;
    }

    fn_8009F7B4(commandWork);
    if (*(s32*)(commandWork + GBA_STATE_PHASE) == 0) {
        commandStarted++;
        *(s32*)(commandWork + GBA_STATE_PHASE) = commandStarted;
        *(u32*)(commandWork + GBA_STATE_TIMEOUT) = 0x30001;
        strcpy((char*)(commandWork + 0x4344), primary);
        if (secondary != NULL) {
            strcpy((char*)(commandWork + 0x43C4), secondary);
        } else {
            commandWork[0x43C4] = 0;
        }
    }
    fn_8009F890(commandWork);
    fn_800A257C(commandWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (commandStarted != 0) {
        fn_8009FABC(commandWork + 0x18);
    }

done:
    return commandStarted;
}
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif

/* 0x800934E4 | size: 0x90 */
s32 fn_800934E4(s32 channel)
{
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
    s32 idle;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work != NULL) {
        fn_8009F7B4(work);
        idle = (*(u32*)(work + GBA_STATE_PHASE) == 0);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    } else {
        idle = 1;
    }

    return idle;
}

/* 0x80093574 | size: 0x9C */
u32 fn_80093574(s32 channel)
{
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            return status;
        }
    }
}

/* 0x80093610 | size: 0x88 */
u32 fn_80093610(s32 channel)
{
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    fn_8009F7B4(work);
    status = *(u32*)(work + GBA_STATE_TIMEOUT);
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);

    return status;
}

/* 0x80093698 | size: 0x15C */
s32 fn_80093698(s32 channel)
{
#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)
#endif
    u32 slot;
    u32 status;
    u8* work;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 1;
    }

    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 1);
    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            break;
        }
    }

    fn_8009F7B4(work);
    *(u32*)(work + GBA_STATE_PHASE) = 0xD;
    *(u32*)(work + GBA_STATE_TIMEOUT) = 0x3000D;
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    fn_8009FABC(work + 0x18);
    fn_800A1E54(work + GBA_DATA_OFFSET, 0);
    fn_800716C8(*(s32*)(work + GBA_STATE_PORT), NULL, NULL);
    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 0);

    status = fn_800E202C(*(u8**)((u8*)lbl_803FB328 + slot));
    if ((status & 0xFFFF) == 0) {
        __assert(lbl_8026F5A8, 0x1E6, &lbl_8047C1E8);
    }
    fn_800E24B0(status);
    fn_800E209C(status);
    *(u8**)((u8*)lbl_803FB328 + slot) = NULL;

    return 1;
}

#endif

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

/* 0x800937F4 | size: 0x310 */
#pragma push
#pragma peephole off
s32 fn_800937F4(void* arg0)
{
    extern void fn_8009F9E8();
    extern s32 fn_80073E8C();
    extern s32 fn_80073E84();
    extern s32 fn_80074324();
    extern s32 fn_800745B4();
    extern s32 fn_80073690();
    extern void fn_800895A4();
    extern s32 fn_80071E34();
    extern void fn_80089380();
    extern s32 fn_80089D30();
    extern s32 fn_80089CA8();
    extern s32 fn_80089C84();
    extern u64 OSGetTime(void);
    extern f32 lbl_8047C1F0;

    u8* p;
    s32 result;
    s32 status;
    s32 state;
    void* arg;
    u64 start;
    u64 now;
    f32 rate;
    u8 readBuffer[0xD8];
    u8 statusBuffer[0x278];

    p = arg0;
    result = 0;
    for (;;) {
        fn_8009F7B4(p);
        if (*(s32*)(p + GBA_STATE_PHASE) != 0xD) {
            *(s32*)(p + GBA_STATE_TIMEOUT) = result;
            *(s32*)(p + GBA_STATE_PHASE) = 0;
            while (*(s32*)(p + GBA_STATE_PHASE) == 0) {
                fn_8009F9E8(p + 0x18, p);
            }
        }

        state = *(s32*)(p + GBA_STATE_PHASE);
        fn_8009F890(p);
        status = 0;

        switch (state) {
        case 0:
        case 3:
            break;
        case 1:
            if ((s8)p[0x43C4] != 0) {
                arg = p + 0x43C4;
            } else {
                arg = NULL;
            }
            status = fn_80073E8C(p + 0x4344, arg);
            if (status == 0) {
                while (fn_80073E84() == 0) {
                    fn_800A257C((void*)fn_800A13F8(), 0x10);
                    OSYieldThread();
                }
                result = 1;
            }
            break;
        case 2:
            start = OSGetTime();
            if (*(s32*)(p + 0x4344) == 0) {
                rate = lbl_8047C1F0;
            } else {
                rate = lbl_8047C1F0;
            }
            while ((status = fn_80074324(*(s32*)(p + GBA_STATE_PORT))) != 0) {
                if (status == 0x3E8) {
                    goto case2Done;
                }
                now = OSGetTime() - start;
                if ((s32)(u32)(u64)(
                        rate * (f32)(*(u32*)0x800000F8 >> 2))
                    <= (s32)(u32)now) {
                    result = 0x20002;
                    goto case2Done;
                }
                fn_800A257C((void*)fn_800A13F8(), 0x10);
                OSYieldThread();
            }
            status = fn_800745B4(*(s32*)(p + GBA_STATE_PORT),
                                 *(s32*)(p + 0x4344));
            if (status == 0) {
                result = 2;
            }
        case2Done:
            break;
        case 4:
            status = fn_80073690(*(s32*)(p + GBA_STATE_PORT), statusBuffer);
            if (status == 0) {
                fn_800895A4(*(s32*)(p + 0x4344), statusBuffer);
                result = 4;
                **(u32**)(p + 0x4348) =
                    (statusBuffer[3] << 24) | (statusBuffer[2] << 16)
                    | (statusBuffer[1] << 8) | statusBuffer[0];
            }
            break;
        case 5:
            result = 5;
            break;
        case 6:
            result = 6;
            break;
        case 7:
            result = 7;
            break;
        case 8:
            result = 8;
            break;
        case 9:
            result = 9;
            break;
        case 10:
            result = 10;
            break;
        case 11:
            status = fn_80071E34(*(s32*)(p + GBA_STATE_PORT), readBuffer);
            if (status == 0) {
                fn_80089380(*(s32*)(p + 0x4344), readBuffer);
                result = 11;
            }
            break;
        case 12:
            status = fn_80089D30(*(s32*)(p + GBA_STATE_PORT) + 1,
                                  p + 0x4344);
            if (status == 0) {
                for (;;) {
                    status = fn_80089CA8(*(s32*)(p + GBA_STATE_PORT) + 1);
                    if (status == 0) {
                        status = fn_80089C84(*(s32*)(p + GBA_STATE_PORT) + 1);
                    }
                    if (status >= 0) {
                        break;
                    }
                    fn_800A257C((void*)fn_800A13F8(), 0x10);
                    OSYieldThread();
                }
                if (status == 0) {
                    result = 12;
                }
            }
            break;
        case 13:
            return 0;
        }

        if (status != 0) {
            result = (state & 0xFFFF) | 0x10000;
        }
    }
}
#pragma pop

/* 0x80093B04 | size: 0x48 */
void fn_80093B04(u32 a, u32 b) {
    u32 r31;
    u32 result;
    r31 = b;
    result = fn_800A13F8();
    if (r31 != 0) {
        if (r31 != result) return;
    }
    fn_800A257C((void*)result, 0x10);
    OSYieldThread();
    return;
}

/* 0x80093F2C | size: 0x38 */
void menuPokemonStatusCtrlRibbon(void) {
    extern void fn_80093F64();
    u8 *r4 = (u8*)&lbl_803FB380;
    u32 r3 = *(u32*)(r4 + 0xC);

    if (r3 != 0) {
        fn_80093F64(r3, r4 + 0x1c);
    }
    return;
}

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;

extern RibbonGroupDescriptor lbl_802EEFD8[10];
extern RibbonGroupDescriptor lbl_802EF000[7];
extern u32 pokemonGetStatus();

/* Build the four-column ribbon grid used by the Pokemon status window. */
void fn_80093F64(u8* pokemon, PokemonRibbonGrid* grid)
{
    s32 ribbon;
    u32 groupIndex;
    s32 outputIndex;
    s32 row;
    s32 column;
    s32 count;

    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            grid->ribbon[row][column] = -1;
        }
    }

    outputIndex = 0;
    for (groupIndex = 0; groupIndex < 10; groupIndex++) {
        /* RULE-EXCEPTION(user-approved): count doubles as the per-group ribbon limit for register allocation — see docs/RULE_EXCEPTIONS.md */
        count = pokemonGetStatus(pokemon, 0, lbl_802EEFD8[groupIndex].selector, 0);
        if (count > lbl_802EEFD8[groupIndex].maximum) {
            count = lbl_802EEFD8[groupIndex].maximum;
        }
        for (ribbon = 0; ribbon < count; ribbon++) {
            /* RULE-EXCEPTION(user-approved): row and column reused as staging scalars for the grid index - see docs/RULE_EXCEPTIONS.md */
            row = outputIndex % 9;
            column = outputIndex / 9;
            grid->ribbon[row][column] = ribbon + lbl_802EEFD8[groupIndex].firstRibbon;
            outputIndex++;
        }
    }

    outputIndex = 0;
    for (groupIndex = 0; groupIndex < 7; groupIndex++) {
        count = pokemonGetStatus(pokemon, 0, lbl_802EF000[groupIndex].selector, 0);
        if (count > lbl_802EF000[groupIndex].maximum) {
            count = lbl_802EF000[groupIndex].maximum;
        }
        for (ribbon = 0; ribbon < count; ribbon++) {
            grid->ribbon[outputIndex][3] = ribbon + lbl_802EF000[groupIndex].firstRibbon;
            outputIndex++;
        }
    }

    count = 0;
    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            if (grid->ribbon[row][column] >= 0) {
                count++;
            }
        }
    }
    grid->count = count;
}

/* 0x800965C8 | size: 0x680. Linked from gbaCommunication_candidate_800965C8_gc20.c
 * (lane D18), which copies this body; keep the two in step. */
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
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047C230, lbl_8047C230);
        fn_800D61E4(*(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
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
        windowDrawSprite(0, 0, window, fn_8001D624(pokemon, 1), 0);
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
            fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BD3);
        }
        break;
    case 0x552: {
        s32 x = (s16)(GSmsgGetRect(*(u32*)(sprite + 0x4C)) >> 16);
        msgctrlSetValue(0x34, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
        fn_800FBB34(x, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
        break;
    }
    case 0x554:
        ball = pokemonDataBiosGetName(bios);
        msg = (s16)(GSmsgGetRect(0x2BD4) >> 16);
        fn_800FB680(0, 0, color, 0x2BD4);
        if (ball != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(ball));
            fn_800FB680(msg, 0, color, 0xE7);
        }
        break;
    case 0x555: {
        u32 sexMsg;

        msgctrlSetValue(0x37, pokemonGetStatus(pokemon, 0, 0x77, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xE7);
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
            fn_800FB680(value, 0, color, 0xCF);
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
            fn_800FB680(0, 0, color, msg);
        }
        break;
    default:
        break;
    }
}


/* 0x80096C48 | size: 0x10C */
void fn_80096C48(u32 unused, u8* dst) {
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } ColorTriple;

    ColorTriple state0;
    ColorTriple state1;
    ColorTriple state2;
    register u8* out;
    register ColorTriple* triple;
    u8* obj;
    s32 state;

    out = dst;
    state0 = *(ColorTriple*)(lbl_8026F5C0 + 0x00);
    state1 = *(ColorTriple*)(lbl_8026F5C0 + 0x0C);
    state2 = *(ColorTriple*)(lbl_8026F5C0 + 0x18);

    obj = windowSearchID(0x53);
    if (obj == NULL) {
        return;
    }

    state = (s8)obj[0x95];
    switch (state) {
    case 0:
        triple = &state0;
        break;
    case 1:
        triple = &state1;
        break;
    case 2:
        triple = &state2;
        break;
    }

    out[0x64] = triple->x;
    out[0x65] = triple->y;
    out[0x66] = triple->z;
}

/* Handle confirm/cancel input for the linked-Pokemon status menu. */
void fn_80096D54(u8* menu)
{
    extern void soundStartSE(s32 id);
    extern void pokemonWazaReplace();

    u16* keys;
    void* pokemon;
    PokemonStatusMenuWork* work;
    register s8* ribbonRow;
    register s32 ribbonColumn;
    register s32 ribbonIndex;
    s8 ribbon;

    keys = windowGetKeyInfo();
    if (keys[2] & 0x10) {
        work = (PokemonStatusMenuWork*)lbl_803FB380;
        switch (work->state) {
        case 0:
        case 1:
        case 6:
        case 8:
            break;
        case 2:
            work->selection = 0;
            work->state = 3;
            break;
        case 3:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
                    soundStartSE(0x26);
                } else if (lbl_803FB380[0] & 2) {
                    work->state = 4;
                    lbl_803FB380[3] = lbl_803FB380[2];
                }
            }
            break;
        case 4:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                pokemonWazaReplace(pokemon, work->previousSelection,
                                   work->selection);
            }
            *(s8*)(lbl_803FB380 + 3) = -1;
            work->state = 3;
            break;
        case 5:
            if ((s32)work->ribbons.count > 0) {
                for (ribbon = 0; ribbon < 36; ribbon++) {
                    ribbonIndex = ribbon % 9;
                    ribbonColumn = ribbon % 4;
                    ribbonRow = (s8*)work + ribbonIndex * 4;
                    ribbonRow += ribbonColumn;
                    if (ribbonRow[0x20] >= 0) {
                        break;
                    }
                }
                work->state = 6;
                *(s8*)(lbl_803FB380 + 0x1A) = ribbon;
            }
            break;
        case 7:
            if (work->flags & 0x10) {
                menu[0x98] = 1;
                work->result = work->selection;
            }
            break;
        }
    } else if (keys[2] & 0x20) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 8:
            break;
        case 1:
        case 2:
        case 5:
        case 7:
            menu[0x98] = 1;
            menu[0x99] = 1;
            lbl_803FB380[1] = 8;
            break;
        case 3:
            lbl_803FB380[1] = 2;
            break;
        case 4:
            lbl_803FB380[1] = 3;
            break;
        case 6:
            *(s8*)(lbl_803FB380 + 0x1A) = -1;
            lbl_803FB380[1] = 5;
            break;
        }
    }
}

/* Handle input for the linked-Pokemon status submenus. */
extern void menuPlaySe(s32 menu, s32 se);
extern void fn_80109C88(void*, u32);
extern u8 pokemonWazaCheckValid(u32 pokemon, s32 waza);

/* Whether move slot `move` of the shown Pokemon can be selected. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusMoveValid(s32 move)
{
    u32 pokemon;
    u16 valid;

    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if ((u16)move == 4) {
        valid = *(u16*)(lbl_803FB380 + 0x18);
    } else {
        valid = pokemonGetStatus(pokemon, 0, 0x7F, move);
        if (pokemonWazaCheckValid(pokemon, move) == 0) {
            valid = 0;
        }
    }
    return valid;
}

/* Move the cursor over the move list. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusMoveInput(u8* menu, u16 input, s8 limit)
{
    s8 moveSelection;

    moveSelection = *(s8*)(lbl_803FB380 + 2);
    if (input & 1) {
        moveSelection--;
    } else if (input & 2) {
        moveSelection++;
    }
    if (moveSelection >= limit) {
        moveSelection = (s8)(limit - 1);
    }
    if (moveSelection < 0) {
        moveSelection = 0;
    }
    if (menuStatusMoveValid(moveSelection) != 0 &&
        moveSelection != *(s8*)(lbl_803FB380 + 2)) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 2) = moveSelection;
    }
}

/* Move the cursor over the 9x4 ribbon grid. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInputBody(u8* menu, u16 input)
{
    s32 previous;
    s32 row;
    s32 column;
    s32 scan;
    s8 selection;
    u8* cell;
    s8 current;
    s32 inner;

    current = *(s8*)(lbl_803FB380 + 0x1A);
    column = current % 9;
    row = current / 9;
    if (input & 1) {
        scan = row;
        while (scan-- > 0) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = -1;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 2) {
        scan = row;
        while (++scan < 4) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = 5;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 8) {
        previous = column;
        column++;
        if (column >= 9) {
            column = 8;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    } else if (input & 4) {
        previous = column;
        column--;
        if (column < 0) {
            column = 0;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    }
    selection = column + row * 9;
    if (selection != current) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 0x1A) = selection;
    }
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInput(u8* menu, u16 input)
{
    menuStatusRibbonInputBody(menu, input);
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusGetTrigger(void)
{
    return windowGetKeyInfo()[3];
}

void fn_80096FA0(u8* menu)
{
    typedef u32 (*StatusChangeCallback)(u32, s32, s32);

    s32 input;
    s32 action;
    s8 limit;
    u8 tabSelection;
    u32 result;

    action = 0;
    if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
        limit = 5;
    } else {
        limit = 4;
    }
    input = menuStatusGetTrigger();

    switch (lbl_803FB380[1]) {
        case 1:
        case 2:
        case 5: {
            u16 trigger = input;

            tabSelection = menu[0x95];
            if (trigger & 8) {
                tabSelection++;
            } else if (trigger & 4) {
                tabSelection--;
            }
            if ((s8)tabSelection > 2) {
                tabSelection = 2;
            }
            if ((s8)tabSelection < 0) {
                tabSelection = 0;
            }
            menu[0x95] = tabSelection;
            switch ((s8)tabSelection) {
            case 0:
                lbl_803FB380[1] = 1;
                break;
            case 1:
                lbl_803FB380[1] = 2;
                break;
            case 2:
                lbl_803FB380[1] = 5;
                break;
            }
            if (trigger & 1) {
                action = 1;
            } else if (trigger & 2) {
                action = 2;
            }
            if (action != 0 && *(u32*)(lbl_803FB380 + 0x10) != 0) {
                result = (*(StatusChangeCallback*)(lbl_803FB380 + 0x10))(
                    *(u32*)(lbl_803FB380 + 0x0C), action,
                    *(s32*)(lbl_803FB380 + 0x14));
                if (*(u32*)(lbl_803FB380 + 0x0C) != result) {
                    if (result != 0) {
                        menuPlaySe(*(s32*)(menu + 4), 1);
                        fn_80109C88(lbl_803FB338, result);
                    }
                    *(u32*)(lbl_803FB380 + 0x0C) = result;
                }
            }
            break;
        }

        case 3:
        case 4:
        case 7:
            menuStatusMoveInput(menu, input, limit);
            break;

        case 6:
            menuStatusRibbonInput(menu, input);
            break;

        case 0:
        case 8:
            break;
    }
}

typedef struct MenuStatusPage {
    s8 group;
    s8 page;
} MenuStatusPage;

extern u8 menuIsCheck(s32 menuId);
extern void menuOpen(s32 menuId, s32 arg1);
extern void menuClose(s32 menuId);

/* Open the status submenu for the current page and close the others. */
static inline void menuStatusSyncPage(u8* menu)
{
    MenuStatusPage page;
    u32 pokemon;
    u32 index;
    s32 menuId;

    page = *(MenuStatusPage*)(menu + 0x94);
    menuId = 0;
    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if (pokemon == 0) {
        return;
    }

    switch (page.page) {
    case 0:
        if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
            menuId = 0x55;
        } else {
            menuId = 0x54;
        }
        break;
    case 1:
        if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
            menuId = 0x56;
        } else {
            menuId = 0x57;
        }
        break;
    case 2:
        menuId = 0x58;
        break;
    }

    for (index = 0; index < 5; index++) {
        if (menuId == lbl_802EEFC4[index]) {
            if (menuIsCheck(menuId) == 0) {
                menuOpen(menuId, 0);
            }
        } else if (menuIsCheck(lbl_802EEFC4[index]) != 0) {
            menuClose(lbl_802EEFC4[index]);
        }
    }
}

/* Select and synchronize the Pokemon-status submenu for the current page. */
s32 fn_800973EC(u8* menu)
{
    switch ((s8)menu[1]) {
    case 0:
        *(s8*)(menu + 0x97) = -1;
        lbl_803FB380[1] = 0;
        lbl_803FB380[2] = menu[0x95];
        *(s8*)(lbl_803FB380 + 3) = -1;
        *(s8*)(lbl_803FB380 + 0x1A) = -1;
        if (lbl_803FB380[0] & 4) {
            menu[0x95] = 0;
            lbl_803FB380[1] = 1;
        } else {
            menu[0x95] = 1;
            lbl_803FB380[1] = 7;
        }
        menuStatusSyncPage(menu);
        break;
    case 1:
        break;
    case 2:
        menuStatusSyncPage(menu);
        break;
    }

    return 0;
}
/* 0x80097BBC | size: 0x114 */
s32 fn_80097BBC(u8 chan) {
    extern void* savedataGetStatus();
    extern void* heroBiosGetPokemonPtr();
    extern int pokemonCheckValid();
    extern int fn_8010B560();
    void* entity;
    void* mgr;

    entity = NULL;
    if (chan < 6) {
        mgr = savedataGetStatus(0, 2);
        if (mgr != 0) {
            entity = heroBiosGetPokemonPtr(mgr, chan);
            if ((u8)pokemonCheckValid() == 0) {
                entity = NULL;
            }
        }
    }
    if (entity == 0) {
        return -1;
    }
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x11;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = (u32)entity;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(u32*)(lbl_803FB380 + 0x4) = -1;
    std098_mapWarpToMenuRoom(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x8009769C | size: 0x350 */
u32 fn_8009769C(u8 flags, u32 arg1, s32 pokemon, u16 arg3, u32 arg4, u32 arg5) {
    extern void menuModelInit(void* model, s32 width, s32 height);
    extern void fn_80109C88(void* model, u32 pokemon);
    extern void fadeCheck(s32 wait);
    extern void fadeSet(s32 mode, f32 value);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern u32 pokemonGetStatus(u32 pokemon, u32 index, u32 status, s32 slot);
    extern u8 pokemonWazaCheckValid(u32 pokemon, s32 slot);
    extern s32 wazaGetStatus(u32 data, u16 index, u32 status, u32 arg3);
    extern void winMsgOpen(s32 slot, s32 message, s32 arg2, s32 arg3);
    extern void winMsgClose(s32 slot);
    extern s32 menuIsCheck(s32 menu);
    extern void menuCloseCustom(s32 menu, s32 mode, s32 wait);
    extern void fn_800FF660(void);
    extern void fn_8010A420(void* model);

    u32 cursor;
    s32 result;
    u16 move;
    u32 selectedPokemon;
    u8 currentFlags;
    u8* state;

    menuModelInit(lbl_803FB338, 0xC8, 0xB4);
    fn_80109C88(lbl_803FB338, pokemon);

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(2, lbl_8047C234);
        } else {
            fadeSet(2, lbl_8047C238);
        }
    }

    cursor = 0;
    state = lbl_803FB380;
    for (;;) {
        result = menuOpenCustom(0x53, 0, &cursor, 0, 1, 0);
        if (result == -1) {
            *(s32*)(state + 4) = result;
            break;
        }

        pokemon = *(s8*)(state + 2);
        *(u32*)(state + 4) = *(s8*)(state + 2);
        if (!(state[0] & 0x40)) {
            break;
        }

        selectedPokemon = *(u32*)(state + 0xC);
        if ((u16)pokemon == 4) {
            move = *(u16*)(state + 0x18);
        } else {
            move = (u16)pokemonGetStatus(selectedPokemon, 0, 0x7F, pokemon);
            if ((u8)pokemonWazaCheckValid(selectedPokemon, pokemon) == 0) {
                move = 0;
            }
        }

        if (wazaGetStatus(0, move, 0x19, 0) == 0) {
            break;
        }
        winMsgOpen(2, 0x2BE9, 1, 0);
        winMsgClose(1);
    }

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(3, lbl_8047C234);
        } else {
            fadeSet(3, lbl_8047C238);
        }
        fadeCheck(1);
    }

    currentFlags = lbl_803FB380[0];
    if ((currentFlags & 1) && !(currentFlags & 8)) {
        fadeCheck(1);
        fadeSet(3, lbl_8047C238);
        fadeCheck(1);
    }

    if ((u8)menuIsCheck(0x54) != 0) {
        menuCloseCustom(0x54, 0, 0);
    }
    if ((u8)menuIsCheck(0x55) != 0) {
        menuCloseCustom(0x55, 0, 0);
    }
    if ((u8)menuIsCheck(0x57) != 0) {
        menuCloseCustom(0x57, 0, 0);
    }
    if ((u8)menuIsCheck(0x56) != 0) {
        menuCloseCustom(0x56, 0, 0);
    }
    if ((u8)menuIsCheck(0x58) != 0) {
        menuCloseCustom(0x58, 0, 0);
    }
    menuCloseCustom(0x53, 0, 1);

    if (lbl_803FB380[0] & 1) {
        fn_800FF660();
        if (lbl_803FB380[0] & 8) {
            floorSetFadeScript(0, 0);
        }
    }

    fn_8010A420(lbl_803FB338);
    _threadSwitch();
    return *(u32*)(state + 4);
}

/* 0x800979EC | size: 0x4C */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void menuPokemonStatus(void) {
    *(u32*)(lbl_803FB380 + 4) = fn_8009769C(
        lbl_803FB380[0],
        *(u32*)(lbl_803FB380 + 8),
        *(u32*)(lbl_803FB380 + 0xC),
        *(u16*)(lbl_803FB380 + 0x18),
        *(u32*)(lbl_803FB380 + 0x10),
        *(u32*)(lbl_803FB380 + 0x14));
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/* 0x80097FCC | size: 0x4 */
void fn_80097FCC(void) {
}

/* 0x80097FD0 | size: 0x28 */
void fn_80097FD0(void) {
    extern int std093_getCurrentGroupID();
    GSresGetResource(std093_getCurrentGroupID(), 0x12670000);
}

/* 0x80097FF8 | size: 0x4 */
void fn_80097FF8(void) {
}

/* 0x80097A38 | size: 0xCC */
s32 fn_80097A38(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x59;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    std098_mapWarpToMenuRoom(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097B04 | size: 0xB8 */
s32 fn_80097B04(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x58;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                arg1, *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097CD0 | size: 0xC4 */
s32 fn_80097CD0(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xc;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097D94 | size: 0xC4 */
s32 fn_80097D94(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xe;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097E58 | size: 0xB0 */
s32 fn_80097E58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xac;
    *(u32*)(lbl_803FB380 + 0x8) = arg0;
    *(u32*)(lbl_803FB380 + 0xC) = arg1;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg2;
    *(u32*)(lbl_803FB380 + 0x14) = arg3;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097F08 | size: 0xC4 */
s32 fn_80097F08(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x8e;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* PPCMfmsr .. PPCMtwpar (0x80097FFC - 0x800980E0) are the SDK PPCArch.c
 * primitives; they link from dolphin/os/PPCArch.c. */

#endif

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

/* 0x80092E38: defined in gbaCommunication_candidate_80091DA4.c. */

#endif

#endif /* GBA_COMMUNICATION_DECLS_ONLY */
