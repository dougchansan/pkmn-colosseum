/**
 * @file menuCB_range_8005344C.c
 * @brief colosseum-battle Pokemon-select-from-PC screens, 0x8005344C - 0x80055E38.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. PCBOX-centric (getPokemon/setPokemon/delPokemon,
 * pcboxGetNbPokemonBox), menuModelCheck/Render, itemDataBiosGetName,
 * winSpriteSetDisp disp-subs + windowGetKeyInfo ctrl fns. Identity SPECULATIVE
 * (0 XD anchors; structural-family evidence only). All functions asm-only.
 */
#include "dolphin/types.h"

typedef struct MenuCBPane MenuCBPane;
typedef struct MenuCBSlotInfo MenuCBSlotInfo;
typedef struct MenuCBState MenuCBState;
typedef struct MenuKeyInfo MenuKeyInfo;
typedef struct MenuModelWork MenuModelWork;
typedef struct MenuCBLayoutEntry MenuCBLayoutEntry;
typedef struct MenuCBStatusWork MenuCBStatusWork;
typedef struct MenuCursorItem MenuCursorItem;

typedef struct MenuCBPokemonBlob {
    u8 bytes[0x138];
} MenuCBPokemonBlob;

struct MenuCBState {
    s32 markKind;
    s32 cursorKind;
};

struct MenuCBPane {
    u8 pad_00[0x6];
    s16 itemId;
    u8 pad_08[0x44];
    s32 textId;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    u8 pad_58[0x8];
    MenuCBState* state;
    u8 pad_64[0x3];
    u8 alpha;
    u8 pad_68[0x1c];
    s16 originX;
    s16 originY;
    u8 pad_88[0xd];
    s8 boxIndex;
    u8 pad_96;
    s8 previousBoxIndex;
    u8 flag98;
};

struct MenuCBSlotInfo {
    s32 itemId;
    s32 kind;
    s32 slot;
};

struct MenuKeyInfo {
    u16 buttons;
    u16 buttonsPrev;
    u16 buttonsDown;
    u16 buttonsRepeat;
};

struct MenuModelWork {
    u8 pad_00[0x48];
};

struct MenuCBLayoutEntry {
    s32 itemId;
    s16 y;
    s16 pad_06;
};

struct MenuCBStatusWork {
    u8 pad_00;
    s8 state;
    s8 initialized;
    u8 pad_03;
    s32 windowId;
};

struct MenuCursorItem {
    u8 pad_00[2];
    s16 x;
    s16 y;
};

extern f32 lbl_8047A54C;
extern void* lbl_8047A548;
extern f32 lbl_8047A550;
extern f32 lbl_8047A554;
extern f32 lbl_8047A558;
extern const f32 lbl_8047BE60;
extern const f32 lbl_8047BE64;
extern const f32 lbl_8047BE6C;
extern const f32 lbl_8047BE68;
extern const f32 lbl_8047BE84;
extern const f32 lbl_8047BE8C;
extern const f32 lbl_8047BE90;
extern const f32 lbl_8047BE94;
extern const f32 lbl_8047BE80;
extern const f32 lbl_8047BE98;
extern const MenuCBLayoutEntry lbl_802E61E8[17];
extern const u32 lbl_80267350[18];
extern s32 lbl_80267320[6];
extern u8 lbl_80314F98[];
extern u8 lbl_802EF0A8[];
extern MenuCBSlotInfo lbl_80267398[0x20];
extern MenuModelWork lbl_803A9720;
extern u32 lbl_8047A560;

extern void* fn_80057270(MenuCBPane* pane);
extern s32 fn_800573C0(void);
extern s32 fn_800566E8(void);
extern s32 fn_80057E40(MenuCBPane* pane);
extern void fn_800FB680(s32 x, s32 y, s32 color, u32 msgId);
extern u32 GSmsgGetGSchar(u32 msgId);
extern u32 GSmsgGetRect(u32 msgId);
extern void* itemDataBiosGetPtr(u16 itemId);
extern u32 itemDataBiosGetName(void* itemData);
extern s32 menuCloseCustom(s32 menuId, s32 mode, s32 wait);
extern s32 menuCloseSync(s32 menuId, s32 wait);
extern void menuClose(s32 menuId);
extern void menuOpen(s32 menuId, s32 wait);
extern s32 menuOpenCustom(s32 menuId, s32 owner, s32 arg2, s32 arg3, s32 arg4, s32 arg5, ...);
extern void menuButtonNormal(MenuCBPane* pane);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);
extern void msgctrlSetValue(s32 id, u32 value);
extern void _threadSwitch(void);
extern u8 pokemonBiosGetLevel(void* pokemon);
extern u8 pokemonBiosGetPcboxMark(void* pokemon);
extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern u32 pokemonDataBiosGetName(void* pokemonData);
extern void* pokemonDataBiosGetPtr(u16 dataId);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern void* pcboxGetPokemonBoxName(void* pcbox, s8 box);
extern s8 pcboxGetNbPokemonBox(void);
extern s8 getPokemonBoxNbUsedSlot__5PCBOXFSc(void* pcbox, s8 box);
extern void winSpriteSetDisp(MenuCBPane* pane, u8 enable);
extern MenuKeyInfo* windowGetKeyInfo(void);
extern void fn_8010A420(MenuModelWork* work);
s32 fn_80057694(void);
void fn_800576A4(u32 value);
u32 fn_800567AC(void);
u32 fn_80056704(void);
void menuCursorNormal(MenuCBPane* pane);
MenuCursorItem* windowGetCursorToItem();
void fn_80057830(s16 x, s16 y, s32 selected);
extern void fn_80054760(s32 forward, s32 wait);
extern void fn_800558B8(void);
extern void fn_80056A80(void);
extern void fn_80056B74(MenuCBPane* pane, s32 enabled);
extern void fn_80057A38(void);
extern void fn_80057A64(void* pokemon, s32 arg1);
extern s8 fn_801347D8(void);
extern s32 fn_80055E38(MenuCBPane* pane);
extern void fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void* heroGetStatus(void* hero, s32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern s32 fn_80057DE8(void* pokemon);
extern void* fn_80057F94(void* pokemon);
extern void fn_800D88DC(u32 flags);
extern void fn_800DC0D4(s32, s32, s32, s32, s32);
extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
extern void fn_800DBFD4(s32, s32, s32, s32, s32);
extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void fn_800D7820(void* resource);
extern void fn_800D85D4(s32 slot, void* texture);
extern void fn_800D6A00(s32 mode);
extern void fn_800D67BC(s32 mode);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32 slot, f32 xScale, f32 yScale);
extern void fn_800D6728(void);
extern void fn_800D888C(u32 flags);
extern void* menuSpriteBiosGetPtr(s32 id);
extern void windowDrawSprite2(s32, s32, s16, s16, s32, void*, s32, s32);
s32 fn_800576B4(void);
s32 fn_80057538(void);
s32 fn_80056A78(void);
void* getPokemon__5PCBOXFScSc(void*, s8, s8);
void delPokemon__5PCBOXFScSc(void*, s8, s8);
void setPokemon__5PCBOXFP7PokemonScSc(void*, void*, s8, s8);
void fn_800574FC(void* pokemon);
void* fn_800574E0(void);
void fn_800574A8(void);
void fn_80057400(void);
void fn_800576C4(s32 state);

#pragma push
#pragma peephole off
s32 fn_8005344C(MenuCBPane* pane, MenuCBPane* sprite) {
    s16 position;
    s16 x;
    s16 y;
    s32 visible;
    MenuCBPane* window;

    extern s32 fn_80057A08(MenuCBPane* pane);
    extern void* windowSearchID(s32 id);
    extern s32 fn_80058F08(s16* position, s32 box);
    extern void fn_80057094(s16* x, s16* y);

    visible = FALSE;
    if (fn_80057A08(pane) != 0) {
        window = windowSearchID(0x94);
        if (window != NULL && fn_80058F08(&position, window->boxIndex) == 0) {
            visible = TRUE;
            fn_80057094(&x, &y);
            sprite->x = x + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 2);
            sprite->y = y + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 4);
        }
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053728(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;

    if (fn_80057E40(pane) != 2) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053A60(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 8;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053AC8(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 4;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053B30(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 2;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053B98(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 1;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053C00(MenuCBPane* pane, MenuCBPane* sprite) {
    void* pokemon;
    u16 itemId;
    void* itemData;
    s32 result;

    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        itemId = pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            itemData = itemDataBiosGetPtr(itemId);
            if (itemData != NULL) {
                msgctrlSetValue(0x37, GSmsgGetGSchar(itemDataBiosGetName(itemData)));
                fn_800FB680(0, 0, -1, 0xe7);
            }
        }
    }
    result = 0;
    sprite->textId = result;
    return result;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053C84(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;
    void* pokemon;
    u32 itemId;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        itemId = (u16)pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053CE8(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 textId;
    void* pokemon;

    textId = 0;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        switch ((u8)menuSubGetPokemonSexForDisp(pokemon)) {
        case 0:
            textId = 0xd67;
            break;
        case 1:
            textId = 0xd68;
            break;
        case 2:
            break;
        }
    }
    sprite->textId = textId;
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053D64(MenuCBPane* pane, MenuCBPane* sprite) {
    void* pokemon;
    s32 result;

    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonDataBiosGetName(pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))));
        fn_800FB680(0, 0, -1, 0xe7);
    }
    result = 0;
    sprite->textId = result;
    return result;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053DD4(MenuCBPane* pane, MenuCBPane* sprite) {
    void* pokemon;
    u32 level;
    s32 textId;
    u32 rect;

    textId = 0;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        level = pokemonBiosGetLevel(pokemon);
        if ((s32)level < 100) {
            textId += 2;
        } else {
            textId += 3;
        }
        rect = GSmsgGetRect(0x1b82);
        fn_800FB680(sprite->width - (textId * 15) - (rect >> 16), 0, -1, 0x1b82);
        msgctrlSetValue(0x34, level);
        textId = 0xde;
    }
    sprite->textId = textId;
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80054420(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 i;
    s32 scaled;
    s16 offset;

    for (i = 0; i < 17; i++) {
        if (*(s16*)((u8*)sprite + 6) == lbl_802E61E8[i].itemId) {
            break;
        }
    }
    if (i >= 17) {
        return 0;
    }
    offset = lbl_802E61E8[i].y;
    scaled = (s32)(lbl_8047BE80 * lbl_8047A558);
    sprite->y = scaled + offset;
    return 0;
}
#pragma pop

s32 fn_80053E7C(MenuCBPane* pane) {
    void* pokemon;
    void* nickname;

    pokemon = fn_80057270(pane);
    if (pokemon == NULL) {
        return 0;
    }
    nickname = pokemonBiosGetNicknamePtr(pokemon);
    msgctrlSetValue(0x37, (u32)nickname);
    fn_800FB680(0, 0, -1, 0xe7);
    return 0;
}

#pragma push
#pragma peephole off
s32 fn_80053ED8(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 index;
    s32 partySlot;
    s32 drawFallback;
    void* pokemon;
    void* texture;
    s32 alpha;
    s16 width;
    s16 height;
    s16 x;
    s16 y;
    s16 insetX;
    s16 insetY;
    f32 t;
    f32 scaleS0;
    f32 scaleS1;
    f32 scaleT0;
    f32 scaleT1;

    drawFallback = TRUE;

    for (index = 0; index < 17; index++) {
        if (sprite->y == lbl_802E61E8[index].itemId) {
            break;
        }
    }

    if (index >= 17) {
        return 0;
    }

    sprite->y = (s16)((s32)(lbl_8047BE80 * lbl_8047A558) + lbl_802E61E8[index].y);
    fn_800FE6D0((s16)(pane->originX + sprite->x), (s16)(pane->originY + sprite->y));
    spriteSetEnv();

    texture = NULL;
    partySlot = 0;
    if (sprite->y != (s32)lbl_80267320[0]) {
        partySlot = 1;
        if (sprite->y != (s32)lbl_80267320[1]) {
            partySlot = 2;
            if (sprite->y != (s32)lbl_80267320[2]) {
                partySlot = 3;
                if (sprite->y != (s32)lbl_80267320[3]) {
                    partySlot = 4;
                    if (sprite->y != (s32)lbl_80267320[4]) {
                        partySlot = 5;
                        if (sprite->y != (s32)lbl_80267320[5]) {
                            partySlot = 6;
                        }
                    }
                }
            }
        }
    }
    if (partySlot >= 6) {
        partySlot = -1;
    }

    if (partySlot >= 0) {
        pokemon = heroGetStatus(0, 3, (u16)partySlot);
        if (pokemon != NULL) {
            if ((u8)pokemonCheckValid(pokemon) != 0) {
                texture = fn_80057F94(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (texture != NULL) {
        if (fn_80057DE8(pokemon) != 0) {
            if (lbl_8047A550 < lbl_8047BE6C) {
                t = lbl_8047BE64 * lbl_8047A550;
            } else {
                t = lbl_8047BE60 - (lbl_8047BE64 * (lbl_8047A550 - lbl_8047BE6C));
            }
            alpha = (s32)(lbl_8047BE84 * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A54C < lbl_8047BE60 && lbl_8047A548 == (void*)partySlot) {
            s32 scaledW;
            s32 scaledH;

            t = lbl_8047BE60 - lbl_8047A54C;
            scaledW = (s32)(t * sprite->width);
            scaledH = (s32)(t * sprite->height);
            width = (s16)scaledW;
            height = (s16)scaledH;
            insetX = (s16)((sprite->width - (s16)scaledW) / 2);
            insetY = (s16)((sprite->height - (s16)scaledH) / 2);
        } else {
            width = sprite->width;
            height = sprite->height;
            insetX = 0;
            insetY = 0;
        }

        x = sprite->width;
        y = sprite->height;
        fn_800D88DC(-0x7ffffffe);
        fn_800DC0D4(0, 0xf, 0xb, 0xa, 8);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        fn_800DC04C(0, 0, 0, 0, 1, 0);

        scaleS0 = lbl_8047BE68 / (f32)GStextureGetXsize(texture);
        scaleS1 = (f32)x / (f32)GStextureGetXsize(texture);
        scaleT0 = lbl_8047BE68 / (f32)GStextureGetYsize(texture);
        scaleT1 = (f32)y / (f32)GStextureGetYsize(texture);

        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, texture);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(insetX, insetY);
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS0, scaleT0);
        fn_800D61E4((s16)(insetX + width), (s16)(insetY + height));
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS1, scaleT1);
        fn_800D6728();
        fn_800D888C(0x80000000);
    } else if (drawFallback != FALSE) {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);
        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane, 0x232, 0);
    }

    return 0;
}
#pragma pop


s32 fn_8005464C(void) {
    return !(lbl_8047A54C >= lbl_8047BE8C);
}

void fn_80054670(void* ptr) {
    lbl_8047A548 = ptr;
    lbl_8047A54C = lbl_8047BE68;
}

s32 fn_80054680(void) {
    if (lbl_8047A554 > lbl_8047BE68) {
        return 3;
    }
    if (lbl_8047A554 < lbl_8047BE68) {
        return 2;
    }
    return lbl_8047A558 >= lbl_8047BE60;
}

void fn_800546C0(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = lbl_8047BE60;
        lbl_8047A554 = lbl_8047BE68;
        return;
    }

    lbl_8047A558 = lbl_8047BE68;
    lbl_8047A554 = lbl_8047BE90;
}

void fn_800546F0(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = lbl_8047BE68;
        lbl_8047A554 = lbl_8047BE68;
        return;
    }

    lbl_8047A558 = lbl_8047BE60;
    lbl_8047A554 = lbl_8047BE94;
}

#pragma push
#pragma scheduling off
#pragma dont_inline on
void fn_8005471C(void) {
    menuCloseCustom(0x8f, 2, 0);
    menuCloseSync(0x8f, 1);
    fn_8010A420(&lbl_803A9720);
}
#pragma dont_inline reset

#pragma push
#pragma peephole off
#pragma scheduling on
s32 fn_80054914(MenuCBPane* pane, MenuCBPane* sprite) {
    typedef struct MessageIdTable {
        s32 ids[6][3];
    } MessageIdTable;
    const MessageIdTable* source;
    MessageIdTable table;
    s32 column;
    s32 row;
    s16 id;
    u8 visible;

    source = (const MessageIdTable*)lbl_80267350;
    table = *source;

    for (row = 0; row < 6; row++) {
        column = 0;
        id = *(s16*)((u8*)sprite + 6);
        for (; column < 3; column++) {
            if (*(s16*)((u8*)sprite + 6) == table.ids[row][column]) {
                break;
            }
        }
        if (column < 3) {
            break;
        }
    }
    if (row >= 6) {
        return 0;
    }
    if (pane->boxIndex == row) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop

s32 fn_800549F0(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 2) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 fn_80054A2C(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 1) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 fn_80054A68(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 fn_80054AA4(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->markKind == 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 fn_80054AE0(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->markKind != 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma scheduling on
#pragma peephole off
s32 fn_80054B1C(s32 context, s32 mode) {
    s32 result;
    u32 args[2];

    s32 windowGetActiveID(void);

    args[0] = (u32)mode;
    args[1] = (u32)context;
    result = menuOpenCustom(0x9c, windowGetActiveID(), 0, 0, 1, 1, args);
    menuClose(0x9c);
    menuCloseSync(0x9c, 1);

    switch (result) {
    case 0:
        switch (context) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 2;
        default:
            return 0;
        }
    case 1:
        return 3;
    case 2:
        if (mode != 0) {
            return 5;
        }
        return 4;
    case 3:
        return 6;
    case 4:
        return 7;
    default:
        return 8;
    }
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80054C44(MenuCBPane* pane) {
    MenuKeyInfo* keys;
    MenuCursorItem* item;
    s32 cursor;
    s32 action;
    s32 previousAction;

    keys = windowGetKeyInfo();
    if (fn_800573C0() != 0) {
        return 0;
    }
    if (fn_800566E8() != 0) {
        return 0;
    }

    if ((keys->buttonsRepeat & 0xC0) != 0) {
        if (fn_80057694() != 0) {
            cursor = 0;
        } else {
            cursor = 1;
        }
        fn_800576A4(cursor);
    }

    if ((keys->buttons & 0x400) != 0) {
        fn_800567AC();
        return 0;
    }
    if ((keys->buttons & 0x200) != 0) {
        fn_80056704();
        return 0;
    }

    cursor = pane->boxIndex;
    if (cursor < 0 || cursor >= 32) {
        action = 3;
    } else {
        action = lbl_80267398[cursor].kind;
    }

    if (action == 1) {
        if (keys->buttons == 8) {
            fn_800567AC();
            return 0;
        }
        if (keys->buttons == 4) {
            fn_80056704();
            return 0;
        }
    }

    if (action == 2) {
        if ((keys->buttonsRepeat & 5) != 0) {
            return 0;
        }
        if ((keys->buttonsRepeat & 10) != 0) {
            pane->boxIndex = 30;
        }
    }

    cursor = (u8)pane->boxIndex;
    if ((s8)cursor == pane->previousBoxIndex) {
        menuCursorNormal(pane);

        cursor = pane->previousBoxIndex;
        if (cursor < 0 || cursor >= 32) {
            previousAction = 3;
        } else {
            previousAction = lbl_80267398[cursor].kind;
        }

        cursor = pane->boxIndex;
        if (cursor < 0 || cursor >= 32) {
            action = 3;
        } else {
            action = lbl_80267398[cursor].kind;
        }

        if (previousAction == 0 && action == 2) {
            pane->boxIndex = 30;
        }
    }

    cursor = (u8)pane->boxIndex;
    if ((s8)cursor != pane->previousBoxIndex) {
        item = windowGetCursorToItem(pane);
        fn_80057830(item->x, item->y, 0);
        return 0;
    }
    return 0;
}
#pragma pop

void fn_80054E7C(MenuCBPane* pane) {
    windowGetKeyInfo();
    if (fn_800573C0() == 0) {
        if (fn_800566E8() == 0) {
            menuButtonNormal(pane);
        }
    }
}

#pragma push
#pragma peephole off
void fn_80054EC8(MenuCBPane* pane) {
    s32 selectedSlot;
    s32 state;
    s32 box;

    state = fn_800576B4();
    switch (state) {
    case 1:
        if (fn_80057538() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = fn_80056A78();
        fn_800574FC(getPokemon__5PCBOXFScSc(NULL, (s8)box, (s8)selectedSlot));
        delPokemon__5PCBOXFScSc(NULL, (s8)box, (s8)selectedSlot);
        fn_800576C4(2);
        break;
    case 2:
        if (fn_80057538() == 0) {
            break;
        }
        fn_800576C4(3);
        if ((s32)lbl_8047A560 != 0) {
            pane->flag98 = 1;
        }
        break;
    case 4:
        if (fn_80057538() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = fn_80056A78();
        setPokemon__5PCBOXFP7PokemonScSc(NULL, fn_800574E0(), (s8)box,
                                          (s8)selectedSlot);
        fn_800574A8();
        fn_800576C4(5);
        break;
    case 5:
        if (fn_80057538() == 0) {
            break;
        }
        fn_800576C4(0);
        break;
    case 6:
        if (fn_80057538() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = fn_80056A78();
        setPokemon__5PCBOXFP7PokemonScSc(NULL, fn_800574E0(), (s8)box,
                                          (s8)selectedSlot);
        fn_800574A8();
        fn_80057400();
        fn_800576C4(3);
        break;
    }
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_800550B4(MenuCBStatusWork* work) {
    MenuCursorItem* item;
    s32 selected;

    extern MenuCursorItem* windowGetCursorToItem(MenuCBStatusWork* work);
    extern void fn_80057830(s16 x, s16 y, s32 selected);
    extern void fn_80057948(void);
    extern void fn_80056854(void);
    extern s32 windowGetActiveID(void);
    extern void fn_80054EC8(MenuCBStatusWork* work);

    switch (work->state) {
    case 0:
        if (work->initialized == 0) {
            selected = TRUE;
        } else {
            selected = FALSE;
        }
        item = windowGetCursorToItem(work);
        fn_80057830(item->x, item->y, selected);
        work->initialized = 1;
        break;
    case 2:
        fn_80057948();
        fn_80056854();
        if (work->windowId == windowGetActiveID()) {
            fn_80054EC8(work);
        }
        break;
    case 3:
        if (work->initialized == 0) {
            work->initialized = 1;
        }
        break;
    }
    return 0;
}
#pragma pop

s32 fn_80055194(s32* outSlot, s32 index) {
    MenuCBSlotInfo* info;

    if (index < 0 || index >= 0x20) {
        return 3;
    }

    info = &lbl_80267398[index];
    *outSlot = info->slot;
    return info->kind;
}

#pragma push
#pragma peephole off
void* fn_800551CC(void* pokemon, s32 direction, s32* moveState)
{
    extern void* getPokemon__5PCBOXFScSc(void*, s8, s8);
    s32 active;
    s32 box;
    void* candidate;
    s32 slot;

    active = moveState[0];
    box = moveState[1];
    if (active != 0) {
        return pokemon;
    }

    if (direction == 1) {
        slot = moveState[2] - 1;
        while (slot >= 0) {
            candidate = getPokemon__5PCBOXFScSc(NULL, (s8) box, (s8) slot);
            if (pokemonCheckValid(candidate) != 0) {
                break;
            }
            slot--;
        }
        if (slot >= 0) {
            moveState[2] = slot;
            return candidate;
        }
    }

    if (direction == 2) {
        slot = moveState[2] + 1;
        while (slot < fn_801347D8()) {
            candidate = getPokemon__5PCBOXFScSc(NULL, (s8) box, (s8) slot);
            if (pokemonCheckValid(candidate) != 0) {
                break;
            }
            slot++;
        }
        if (slot < fn_801347D8()) {
            moveState[2] = slot;
            return candidate;
        }
    }

    return pokemon;
}
#pragma pop

s32 fn_800552D4(s8 box, s8 slot)
{
    extern void* getPokemon__5PCBOXFScSc(void*, s8, s8);
    extern void delPokemon__5PCBOXFScSc(void*, s8, s8);
    extern u8 pokemonCheckValid(void*);
    extern s32 fn_800576B4(void);
    extern void* fn_800574E0(void);
    extern s32 fn_80057694(void);
    extern u32 pokemonGetStatus(void*, s32, s32, s32);
    extern void fn_80058804(void*, s32);
    extern s32 fn_80054B1C(s32, s32);
    extern void fn_800587D8(void);
    extern void fn_800576C4(s32);
    extern void fn_80057458(void*);
    extern void* heroGetStatus(void*, s32, u16);
    extern void fn_800546F0(s32);
    extern s32 fn_80054680(void);
    extern void fn_800599AC(s32);
    extern void fn_800546C0(s32);
    extern void fadeSet(s32, f32);
    extern s32 fadeCheck(s32);
    extern void fn_8005471C(void);
    extern s32 fn_80057C9C(void*, void*, s32*);
    extern void* menuItemBiosGetPtr(s32);
    extern void fn_80057830(s16, s16, s32);
    extern MenuCBPane* windowSearchID(s32);
    extern u32 fn_8005D738(u8);
    extern void pokemonBiosSetPcboxMark(void*, u8);
    extern s32 fn_80057DE8(void*);
    extern s32 fn_8005D3D0(s32);
    extern void GScharCpy(void*, void*);
    extern void fn_800566D8(s32);
    extern s32 fn_800566B4(void);
    extern void fn_8005744C(void);
    extern s32 fn_80057428(void);
    extern void fn_800574A8(void);
    MenuCBPokemonBlob pokemonCopy;
    u16 name[0xE];
    s32 moveState[3];
    void* boxPokemon;
    void* alternatePokemon;
    void* pokemon;
    MenuCBPane* window;
    void* item;
    s32 canAct;
    s32 context;
    s32 action;
    s32 emptySlots;
    s32 i;
    s32 haveCopy;
    s32 mode;
    u32 nameValue;
    u8 mark;

    boxPokemon = getPokemon__5PCBOXFScSc(NULL, box, slot);
    canAct = pokemonCheckValid(boxPokemon);
    alternatePokemon = NULL;
    context = 0;
    mode = 1;

    if (fn_800576B4() != 3) {
        if (canAct == 0) {
            mode = 0;
        }
    } else {
        alternatePokemon = fn_800574E0();
        context = canAct != 0 ? 2 : 1;
    }
    if (mode == 0) {
        return slot;
    }
    pokemon = alternatePokemon != NULL ? alternatePokemon : boxPokemon;

    if (fn_80057694() == 0) {
        nameValue = pokemonGetStatus(pokemon, 0, 0x77, 0);
        msgctrlSetValue(0x36, nameValue);
        msgctrlSetValue(0x32, nameValue);
        fn_80058804((void*)0x1B74, 0);
        action = fn_80054B1C(context, 0);
        fn_800587D8();
    } else {
        action = context >= 0 && context < 3 ? context : 8;
    }

    switch (action) {
    case 0:
        fn_800576C4(1);
        break;
    case 1:
        fn_800576C4(4);
        break;
    case 2:
        fn_80057458(boxPokemon);
        delPokemon__5PCBOXFScSc(NULL, box, slot);
        fn_800576C4(6);
        break;
    case 3:
        emptySlots = 6;
        for (i = 0; i < 6; i++) {
            if (pokemonCheckValid(heroGetStatus(NULL, 3, (u16)i)) != 0) {
                emptySlots--;
            }
        }
        if (emptySlots <= 0) {
            fn_80058804((void*)0x1B7D, 1);
            break;
        }
        if (alternatePokemon != NULL) {
            fn_800546F0(0);
            while (fn_80054680() != 2) {
                _threadSwitch();
            }
            fn_800599AC(1);
            fn_800546C0(0);
            while (fn_80054680() != 3) {
                _threadSwitch();
            }
        }
        lbl_8047A560 = 1;
        fn_800576C4(1);
        break;
    case 4:
        fadeSet(3, lbl_8047BE98);
        fadeCheck(1);
        fn_8005471C();
        fn_80056A80();
        haveCopy = 0;
        if (fn_800576B4() == 3) {
            haveCopy = 1;
            pokemonCopy = *(MenuCBPokemonBlob*)fn_800574E0();
        }
        context = fn_80057694();
        fn_80057A38();
        moveState[0] = alternatePokemon != NULL;
        moveState[1] = box;
        moveState[2] = slot;
        fn_80057C9C(pokemon, fn_800551CC, moveState);
        fn_80056B74((MenuCBPane*)(s32)box, 0);
        fn_80054760(0, 0);
        fn_80057A64(haveCopy != 0 ? &pokemonCopy : NULL, context);
        if (moveState[0] == 0) {
            for (i = 0; i < 32; i++) {
                if (lbl_80267398[i].kind == 0 &&
                    lbl_80267398[i].slot == moveState[2]) {
                    break;
                }
            }
            if (i < 32) {
                item = menuItemBiosGetPtr(lbl_80267398[i].itemId);
                fn_80057830(*(s16*)((u8*)item + 2),
                             *(s16*)((u8*)item + 4), 1);
            }
            slot = (s8)moveState[2];
            window = windowSearchID(0x93);
            if (window != NULL) {
                window->boxIndex = slot;
            }
        }
        fadeSet(2, lbl_8047BE98);
        fadeCheck(1);
        break;
    case 5:
        fn_80058804((void*)0x1B88, 0);
        mark = fn_8005D738(pokemonBiosGetPcboxMark(pokemon));
        if (mark != 0xFF) {
            pokemonBiosSetPcboxMark(pokemon, mark);
        }
        fn_800587D8();
        break;
    case 6:
        if (fn_80057DE8(pokemon) != 0) {
            fn_80058804((void*)0x1B94, 1);
            break;
        }
        fn_80058804((void*)0x1B91, 0);
        action = fn_8005D3D0(1);
        fn_800587D8();
        if (action == 1) {
            break;
        }
        nameValue = pokemonGetStatus(pokemon, 0, 0x77, 0);
        GScharCpy(name, (void*)nameValue);
        if (alternatePokemon == NULL) {
            fn_800566D8(slot);
            while (fn_800566B4() != 0) {
                _threadSwitch();
            }
            delPokemon__5PCBOXFScSc(NULL, box, slot);
        } else {
            fn_8005744C();
            while (fn_80057428() != 0) {
                _threadSwitch();
            }
            fn_800574A8();
            fn_800576C4(0);
        }
        msgctrlSetValue(0x36, (u32)name);
        fn_80058804((void*)0x1B92, 1);
        fn_80058804((void*)0x1B93, 1);
        break;
    }

    return slot;
}


#pragma push
#pragma peephole off
void fn_80055B98(MenuCBPane* pane) {
    menuOpenCustom(0x10e, 0x1f, 0, 0, 0, 0);
    fn_80056B74(pane, TRUE);

    while (fn_80055E38(pane) == 0) {
        _threadSwitch();
    }

    fn_80054760(FALSE, TRUE);
    fn_80057A64(NULL, 0);
    fn_800558B8();
    fn_80057A38();
    fn_8005471C();
    fn_80056A80();
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80055C2C(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 box;
    s32 capacity;
    s32 used;

    box = pane->boxIndex;
    if (box < 0 || box >= pcboxGetNbPokemonBox()) {
        return 0;
    }

    capacity = fn_801347D8();
    used = getPokemonBoxNbUsedSlot__5PCBOXFSc(NULL, box);
    if (used < 0) {
        return 0;
    }

    msgctrlSetValue(0x34, used);
    msgctrlSetValue(0x35, capacity);
    sprite->textId = 0x1b7f;
    return 0;
}
#pragma pop

s32 fn_80055CD4(MenuCBPane* pane, MenuCBPane* sprite) {
    u8 box;
    u32 name;
    s32 result;

    box = pane->boxIndex;
    name = (u32)pcboxGetPokemonBoxName(NULL, (s8)box);
    if (name == 0) {
        return 0;
    }

    msgctrlSetValue(0x37, name);
    result = 0;
    sprite->textId = 0xcf;
    return result;
}

#pragma push
#pragma peephole off
#pragma scheduling off
s32 fn_80055D34(MenuCBPane* pane) {
    MenuKeyInfo* keyInfo;
    s8 count;
    s32 box;

    keyInfo = windowGetKeyInfo();
    if ((keyInfo->buttonsRepeat & 8) != 0) {
        count = pcboxGetNbPokemonBox();
        box = (u8)pane->boxIndex + 1;
        pane->boxIndex = box;
        if ((s8)box >= count) {
            pane->boxIndex = 0;
        }
    }
    if ((keyInfo->buttonsRepeat & 4) != 0) {
        box = (u8)pane->boxIndex - 1;
        pane->boxIndex = box;
        if ((s8)box < 0) {
            pane->boxIndex = pcboxGetNbPokemonBox() - 1;
        }
    }
    return 0;
}
#pragma pop

#pragma push
#pragma scheduling off
void fn_80055DE0(void) {
    menuClose(0xa3);
    menuCloseSync(0xa3, 1);
}

void fn_80055E10(void) {
    menuOpen(0xa3, 1);
}
#pragma pop
