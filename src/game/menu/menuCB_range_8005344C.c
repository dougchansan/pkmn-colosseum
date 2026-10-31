/**
 * @file menuCB_range_8005344C.c
 * @brief colosseum-battle Pokemon-select prefix, 0x8005344C - 0x80054914.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. PCBOX-centric (getPokemon/setPokemon/delPokemon,
 * pcboxGetNbPokemonBox), menuModelCheck/Render, itemDataBiosGetName,
 * winSpriteSetDisp disp-subs + windowGetKeyInfo ctrl fns. Identity SPECULATIVE
 * (0 XD anchors; structural-family evidence only).
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
typedef struct MenuCBResourceLayout MenuCBResourceLayout;
typedef struct MenuItemBios MenuItemBios;

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

struct MenuCBResourceLayout {
    s32 itemId;
    s16 y;
    u8 pad_06[0x16];
};

struct MenuItemBios {
    u8 pad_00[6];
    s16 x;
    s16 y;
};

extern f32 lbl_8047A54C;
extern void* lbl_8047A548;
extern f32 lbl_8047A550;
extern f32 lbl_8047A554;
extern f32 lbl_8047A558;
extern f32 lbl_8047A540;
extern u32 lbl_8047A544;
extern MenuCBLayoutEntry lbl_802E61E8[17];
extern const u32 lbl_80267350[18];
extern s32 lbl_80267320[6];
extern u8 lbl_80314F98[];
extern u8 lbl_80314E08[];
extern u8 lbl_802EF0A8[];
extern MenuCBPokemonBlob lbl_803A95E8;
extern MenuCBSlotInfo lbl_80267398[0x20];
extern MenuModelWork lbl_803A9720;
extern u32 lbl_8047A560;
extern s32 lbl_804788F8;

extern void* pcBoxGetCursorPokemon();
extern s32 pcBoxCursorBusy(void);
extern s32 pcBoxTabIsMoving(void);
extern s32 pcBoxStreamGetState(MenuCBPane* pane);
extern void GSmsgPrintColor(s32 x, s32 y, s32 color, u32 msgId);
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
extern void menuModelTerm(MenuModelWork* work);
extern s32 menuModelCheck(MenuModelWork* work, u8 wait);
extern s32 menuPokemonModelIsLoaded(MenuModelWork* work, void* pokemon);
extern void* menuModelRender(MenuModelWork* work);
extern void menuPokemonModelRequest(MenuModelWork* work, void* pokemon);
s32 pcBoxGetHeldFlag(void);
void pcBoxSetHeldFlag(u32 value);
u32 pcBoxTabNext(void);
u32 pcBoxTabPrev(void);
void menuCursorNormal(MenuCBPane* pane);
MenuCursorItem* windowGetCursorToItem();
void pcBoxSetCursorTarget(s16 x, s16 y, s32 selected);
extern void pcBoxOpenDetail(s32 forward, s32 wait);
extern MenuItemBios* menuItemBiosGetPtr(s32 itemId);
extern void menuModelInit(MenuModelWork* work, s16 x, s16 y);
extern void pokemonInit(MenuCBPokemonBlob* pokemon);
extern void pcBoxRun(void);
extern void pcBoxCloseTabs(void);
extern void pcBoxOpenTabs(MenuCBPane* pane, s32 enabled);
extern void pcBoxCloseHeldWindow(void);
extern void pcBoxOpenHeldWindow(void* pokemon, s32 arg1);
extern s8 pcBoxGetSlotCount(void);
extern s32 pcBoxTabMenuIdle(MenuCBPane* pane);
extern void spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void* heroGetStatus(void* hero, s32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern s32 pcBoxIsShadowPokemon(void* pokemon);
extern void* pcBoxGetIconResource(void* pokemon);
extern void gfxSetFlags(u32 flags);
extern void gfxSetTevAlphaIn(s32, s32, s32, s32, s32);
extern void gfxSetTevAlphaOp(s32, s32, s32, s32, s32, s32);
extern void gfxSetTevColorIn(s32, s32, s32, s32, s32);
extern void gfxSetTevColorOp(s32, s32, s32, s32, s32, s32);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void gfxSetVtxDesc(void* resource);
extern void gfxLoadTexture(s32 slot, void* texture);
extern void gfxSetPrimitive(s32 mode);
extern void gfxBegin(s32 mode);
extern void gfxVtxPos2s16(s16 x, s16 y);
extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
extern void gfxVtxTexCoord(s32 slot, f32 xScale, f32 yScale);
extern void gfxEnd(void);
extern void gfxClearFlags(u32 flags);
extern void* menuSpriteBiosGetPtr(s32 id);
extern void windowDrawSprite2(s32, s32, s16, s16, s32, void*, s32, s32);
s32 pcBoxGetHoldState(void);
s32 pcBoxHeldSequenceDone(void);
s32 pcBoxGetTab(void);
void* getPokemon__5PCBOXFScSc(void*, s8, s8);
void delPokemon__5PCBOXFScSc(void*, s8, s8);
void setPokemon__5PCBOXFP7PokemonScSc(void*, void*, s8, s8);
void pcBoxCopyToHeld(void* pokemon);
void* pcBoxGetHeld(void);
void pcBoxClearHeld(void);
void pcBoxSwapHeldBuffer(void);
void pcBoxSetHoldState(s32 state);


s32 pcBoxDetailDrawSprite(MenuCBPane* pane, MenuCBPane* sprite) {
    s16 position;
    s16 x;
    s16 y;
    s32 visible;
    MenuCBPane* window;

    extern s32 pcBoxHeldWindowOpen(MenuCBPane* pane);
    extern void* windowSearchID(s32 id);
    extern s32 pcBoxGetDetailCell(s16* position, s32 box);
    extern void pcBoxGetCursorPos(s16* x, s16* y);

    visible = FALSE;
    if (pcBoxHeldWindowOpen(pane) != 0) {
        window = windowSearchID(0x94);
        if (window != NULL && pcBoxGetDetailCell(&position, window->boxIndex) == 0) {
            visible = TRUE;
            pcBoxGetCursorPos(&x, &y);
            sprite->x = x + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 2);
            sprite->y = y + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 4);
        }
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}

typedef struct MenuCBTweenFrame {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s32 alpha;
} MenuCBTweenFrame;

extern const MenuCBTweenFrame lbl_80267338[2];

s32 pcBoxDetailDrawSprite2(MenuCBPane* pane, MenuCBPane* sprite)
{
    MenuCBTweenFrame frames[2];
    f32 amount;
    s32 visible;
    f32 fx, fy, fw, fh, fa;
    extern s32 pcBoxStreamGetState();

    frames[0] = lbl_80267338[0];
    frames[1] = lbl_80267338[1];
    if (pcBoxStreamGetState() != 2) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, visible);
    if (visible) {
        amount = lbl_8047A540;
        fx = frames[0].x + amount * (frames[1].x - frames[0].x);
        fy = frames[0].y + amount * (frames[1].y - frames[0].y);
        fw = frames[0].width + amount * (frames[1].width - frames[0].width);
        fh = frames[0].height + amount * (frames[1].height - frames[0].height);
        fa = frames[0].alpha + amount * (frames[1].alpha - frames[0].alpha);
        sprite->x = fx;
        sprite->y = fy;
        sprite->width = fw;
        sprite->height = fh;
        sprite->alpha = fa;
    }
    return 0;
}

s32 pcBoxDetailDrawSprite3(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;

    if (pcBoxStreamGetState(pane) != 2) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}

u32 pcBoxDrawModel(u32 unused, u8* pane) {
    u32 texture;
    s32 alpha;
    s32 height;
    MenuCBPokemonBlob* pokemon;
    f32 heightScale;
    f32 fade;
    extern void gfxVtxPos2s16();
    extern s32 pcBoxStreamGetState();

    texture = 0;
    if (pcBoxStreamGetState() != 2) {
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetPrimitive(6);
        gfxSetVtxDesc(lbl_80314E08);

        if ((heightScale = 2.0f * lbl_8047A540) > 1.0f) {
            heightScale = 1.0f;
        }
        height = (s32)((f32)(s32)*(s16*)(pane + 0x56) * heightScale);

        if ((fade = 2.0f * (lbl_8047A540 - 0.5f)) < 0.0f) {
            fade = 0.0f;
        }
        alpha = (s32)(102.0f * (1.0f - fade));

        gfxBegin(4);
        gfxVtxPos2s16(0, 0);
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0);
        gfxVtxPos2s16(*(s16*)(pane + 0x54), 0);
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0);
        gfxVtxPos2s16(*(s16*)(pane + 0x54), height);
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, alpha);
        gfxVtxPos2s16(0, height);
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, alpha);
        gfxEnd();
        return 0;
    }

    pokemon = pcBoxGetCursorPokemon();
    if (pokemon == NULL) {
        return 0;
    }

    if ((u8)menuModelCheck(&lbl_803A9720, 0) == 0) {
        if ((u8)menuPokemonModelIsLoaded(&lbl_803A9720, pokemon) != 0) {
            texture = (u32)menuModelRender(&lbl_803A9720);
        } else {
            menuPokemonModelRequest(&lbl_803A9720, pokemon);
            lbl_803A95E8 = *pokemon;
        }
    }

    if (texture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, (void*)texture);
        gfxBegin(2);
        gfxVtxPos2s16(0, 0);
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, 0.0f, 0.0f);
        gfxVtxPos2s16(*(s16*)(pane + 0x54), *(s16*)(pane + 0x56));
        gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, 1.0f, 1.0f);
        gfxEnd();
    }

    return 0;
}

s32 pcBoxDrawMark1(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = pcBoxGetCursorPokemon(pane);
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

s32 pcBoxDrawMark2(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = pcBoxGetCursorPokemon(pane);
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

s32 pcBoxDrawMark3(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = pcBoxGetCursorPokemon(pane);
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

s32 pcBoxDrawMark4(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = pcBoxGetCursorPokemon(pane);
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

s32 pcBoxDrawItemName(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    u16 itemId;
    void* itemData;
    s32 result;

    pokemon = pcBoxGetCursorPokemon(pane);
    if (pokemon != NULL) {
        itemId = pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            itemData = itemDataBiosGetPtr(itemId);
            if (itemData != NULL) {
                msgctrlSetValue(0x37, GSmsgGetGSchar(itemDataBiosGetName(itemData)));
                GSmsgPrintColor(0, 0, -1, 0xe7);
            }
        }
    }
    result = 0;
    sprite->textId = result;
    return result;
}

s32 pcBoxDrawItemIcon(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;
    u32 itemId;

    visible = FALSE;
    pokemon = pcBoxGetCursorPokemon(pane);
    if (pokemon != NULL) {
        itemId = (u16)pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 pcBoxDrawGender(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 textId;
    void* pokemon;

    textId = 0;
    pokemon = pcBoxGetCursorPokemon(pane);
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

s32 pcBoxDrawSpecies(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    s32 result;

    pokemon = pcBoxGetCursorPokemon(pane);
    if (pokemon != NULL) {
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonDataBiosGetName(
                                     pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))));
        GSmsgPrintColor(0, 0, -1, 0xe7);
    }
    result = 0;
    sprite->textId = result;
    return result;
}

s32 pcBoxDrawLevel(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    u32 level;
    s32 textId;
    u32 rect;

    textId = 0;
    pokemon = pcBoxGetCursorPokemon(pane);
    if (pokemon != NULL) {
        level = pokemonBiosGetLevel(pokemon);
        if ((s32)level < 100) {
            textId += 2;
        } else {
            textId += 3;
        }
        rect = GSmsgGetRect(0x1b82);
        GSmsgPrintColor(sprite->width - (textId * 15) - (rect >> 16), 0, -1, 0x1b82);
        msgctrlSetValue(0x34, level);
        textId = 0xde;
    }
    sprite->textId = textId;
    return 0;
}

s32 pcBoxDrawNickname(MenuCBPane* pane)
{
    void* pokemon;
    void* nickname;

    pokemon = pcBoxGetCursorPokemon(pane);
    if (pokemon == NULL) {
        return 0;
    }
    nickname = pokemonBiosGetNicknamePtr(pokemon);
    msgctrlSetValue(0x37, (u32)nickname);
    GSmsgPrintColor(0, 0, -1, 0xe7);
    return 0;
}

/*
 * Not referenced by the game, so the linker strips it. Retail's .sdata2 pool
 * (0x8047BE58-0x8047BE98) has the unsigned int-to-float bias at 0x8047BE78,
 * ahead of pcBoxDrawPartyIcons's first literal (-383.0f at 0x8047BE80), so an earlier
 * function in this TU converted an unsigned value to float. Nothing but that
 * pool position is known about it.
 */
/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function; its body is inferred from the .sdata2 pool layout only and exists to place the unsigned bias before -383.0f — see docs/RULE_EXCEPTIONS.md */
f32 menuCBGetTextureWidth(void* texture) {
    return (f32)GStextureGetXsize(texture);
}

/* The party slot whose item id a sprite shows, or -1. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (pcBoxDrawPartyIcons) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuCBGetPartySlot(s32 itemId) {
    s32 slot;

    for (slot = 0; slot < 6; slot++) {
        if (itemId == lbl_80267320[slot]) {
            break;
        }
    }
    if (slot >= 6) {
        slot = -1;
    }
    return slot;
}

s32 pcBoxDrawPartyIcons(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 index;
    s32 partySlot;
    s32 drawFallback;
    void* texture;
    void* pokemon;
    s32 alpha;
    s16 x;
    s16 y;
    s16 insetX;
    s16 insetY;
    s32 width;
    s32 height;
    f32 t;
    f32 scaleS0;
    f32 scaleT1;
    f32 scaleT0;
    f32 scaleS1;

    drawFallback = TRUE;

    for (index = 0; index < 17; index++) {
        if (sprite->itemId == lbl_802E61E8[index].itemId) {
            break;
        }
    }

    if (index >= 17) {
        return 0;
    }

    sprite->y = (s16)((s32)(-383.0f * lbl_8047A558) + lbl_802E61E8[index].y);
    spriteSetOrigin((s16)(pane->originX + sprite->x), (s16)(pane->originY + sprite->y));
    spriteSetEnv();

    texture = NULL;
    partySlot = menuCBGetPartySlot(sprite->itemId);

    if (partySlot >= 0) {
        pokemon = heroGetStatus(0, 3, (u16)partySlot);
        if (pokemon != NULL) {
            if ((u8)pokemonCheckValid(pokemon) != 0) {
                texture = pcBoxGetIconResource(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (texture != NULL) {
        if (pcBoxIsShadowPokemon(pokemon) != 0) {
            if (lbl_8047A550 < 0.5f) {
                t = 2.0f * lbl_8047A550;
            } else {
                t = 1.0f - (2.0f * (lbl_8047A550 - 0.5f));
            }
            alpha = (s32)(255.0f * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A54C < 1.0f && (s32)lbl_8047A548 == partySlot) {
            t = 1.0f - lbl_8047A54C;
            width = (s32)(t * sprite->width);
            height = (s32)(t * sprite->height);
            insetX = (s16)((sprite->width - (s16)width) / 2);
            insetY = (s16)((sprite->height - (s16)height) / 2);
        } else {
            width = sprite->width;
            height = sprite->height;
            insetX = 0;
            insetY = 0;
        }

        y = sprite->height;
        x = sprite->width;
        gfxSetFlags(-0x7ffffffe);
        gfxSetTevAlphaIn(0, 0xf, 0xb, 0xa, 8);
        gfxSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        gfxSetTevColorIn(0, 7, 7, 7, 4);
        gfxSetTevColorOp(0, 0, 0, 0, 1, 0);

        scaleS0 = 0.0f / (f32)GStextureGetXsize(texture);
        scaleS1 = (f32)x / (f32)GStextureGetXsize(texture);
        scaleT0 = 0.0f / (f32)GStextureGetYsize(texture);
        scaleT1 = (f32)y / (f32)GStextureGetYsize(texture);

        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxSetPrimitive(7);
        gfxBegin(2);
        gfxVtxPos2s16(insetX, insetY);
        gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
        gfxVtxTexCoord(0, scaleS0, scaleT0);
        /* RULE-EXCEPTION(user-approved): codegen-only (s32) casts drop a second extsh of the insets — see docs/RULE_EXCEPTIONS.md */
        gfxVtxPos2s16((s16)((s32)insetX + width), (s16)((s32)insetY + height));
        gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
        gfxVtxTexCoord(0, scaleS1, scaleT1);
        gfxEnd();
        gfxClearFlags(0x80000000);
    } else if (drawFallback != FALSE) {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);
        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane, 0x232, 0);
    }

    return 0;
}

s32 pcBoxMenuDrawExit(MenuCBPane* pane, MenuCBPane* sprite) {
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
    scaled = (s32)(-383.0f * lbl_8047A558);
    sprite->y = scaled + offset;
    return 0;
}


s32 pcBoxDetailCtrl(u8* ctx) {
    extern void winSeqSetMenu(s32, s32);

    switch ((s8)ctx[1]) {
    case 0:
        if ((s8)ctx[2] != 0) { break; }
        lbl_8047A550 = 0.0f;
        lbl_8047A54C = 1.0f;
        lbl_8047A540 = 0.0f;
        if ((s32)lbl_8047A544 != 0) {
            winSeqSetMenu(0x8f, 0xff);
        }
        ctx[2] = 1;
        break;
    case 2:
        if (lbl_8047A554 > 0.0f) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 >= 1.0f) {
                lbl_8047A558 = 1.0f;
                lbl_8047A554 = 0.0f;
            }
        }
        if (lbl_8047A554 < 0.0f) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 <= 0.0f) {
                lbl_8047A558 = 0.0f;
                lbl_8047A554 = 0.0f;
            }
        }
        lbl_8047A550 += 0.016666668f;
        if (lbl_8047A550 >= 1.0f) {
            lbl_8047A550 -= 1.0f;
        }
        if (lbl_8047A54C < 1.0f) {
            lbl_8047A54C += 0.016666668f;
            if (lbl_8047A54C > 1.0f) {
                lbl_8047A54C = 1.0f;
            }
        }
        lbl_8047A540 += 0.016666668f;
        if (lbl_8047A540 >= 1.0f) {
            lbl_8047A540 -= 1.0f;
        }
        break;
    case 3:
        if ((s8)ctx[2] != 0) { break; }
        winSeqSetMenu(0x8f, 0x103);
        ctx[2] = 1;
        break;
    }
    return 0;
}


s32 menuPartySlotFadeDone(void) {
    return !(lbl_8047A54C >= 0.8f);
}

void menuPartySlotFadeStart(void* ptr) {
    lbl_8047A548 = ptr;
    lbl_8047A54C = 0.0f;
}

s32 pcBoxScrollGetState(void) {
    if (lbl_8047A554 > 0.0f) {
        return 3;
    }
    if (lbl_8047A554 < 0.0f) {
        return 2;
    }
    return lbl_8047A558 >= 1.0f;
}

void pcBoxScrollStartIn(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = 1.0f;
        lbl_8047A554 = 0.0f;
        return;
    }

    lbl_8047A558 = 0.0f;
    lbl_8047A554 = 0.033333335f;
}

void pcBoxScrollStartOut(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = 0.0f;
        lbl_8047A554 = 0.0f;
        return;
    }

    lbl_8047A558 = 1.0f;
    lbl_8047A554 = -0.033333335f;
}

void pcBoxCloseDetail(void) {
    menuCloseCustom(0x8f, 2, 0);
    menuCloseSync(0x8f, 1);
    menuModelTerm(&lbl_803A9720);
}

/* Refresh the 17 layout rows' y from the resource layout table. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (pcBoxOpenDetail) — see docs/RULE_EXCEPTIONS.md */
static inline void menuCBLoadLayoutY(void) {
    int i;

    for (i = 0; i < 17; i++) {
        lbl_802E61E8[i].y =
            ((MenuCBResourceLayout*)lbl_802EF0A8)[lbl_802E61E8[i].itemId].y;
    }
}

void pcBoxOpenDetail(s32 forward, s32 wait) {
    MenuItemBios* item;

    lbl_8047A544 = wait;
    if (lbl_804788F8 != 0) {
        menuCBLoadLayoutY();
        lbl_804788F8 = 0;
    }

    if (forward == 0) {
        lbl_8047A558 = 1.0f;
        lbl_8047A554 = 0.0f;
    } else {
        lbl_8047A558 = 0.0f;
        lbl_8047A554 = 0.0f;
    }

    item = menuItemBiosGetPtr(0xD34);
    menuModelInit(&lbl_803A9720, item->x, item->y);
    pokemonInit(&lbl_803A95E8);
    menuOpenCustom(0x8F, 0x1F, 0, 0, 0, 0);
}
