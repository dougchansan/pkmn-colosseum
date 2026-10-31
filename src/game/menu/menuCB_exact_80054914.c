/**
 * @file menuCB_exact_80054914.c
 * @brief Exact colosseum-battle Pokemon-select island, 0x80054914 - 0x800552D4.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. This source owns the 13 byte-exact functions
 * in target order.
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

extern void* pcBoxGetCursorPokemon(MenuCBPane* pane);
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
s32 pcBoxGetHeldFlag(void);
void pcBoxSetHeldFlag(u32 value);
u32 pcBoxTabNext(void);
u32 pcBoxTabPrev(void);
void menuCursorNormal(MenuCBPane* pane);
MenuCursorItem* windowGetCursorToItem();
void pcBoxSetCursorTarget(s16 x, s16 y, s32 selected);
extern void pcBoxOpenDetail(s32 forward, s32 wait);
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
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling off; dont_inline on; dont_inline reset; push; peephole off; scheduling on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
#pragma dont_inline on
#pragma dont_inline reset
#pragma push
#pragma peephole off
#pragma scheduling on
s32 pcBoxCmdDrawSprite(MenuCBPane* pane, MenuCBPane* sprite) {
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

s32 pcBoxCmdDrawShift(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 2) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 pcBoxCmdDrawPlace(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 1) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 pcBoxCmdDrawMove(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->cursorKind != 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 pcBoxCmdDrawDeposit(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->markKind == 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}

s32 pcBoxCmdDrawWithdraw(MenuCBPane* pane, MenuCBPane* sprite) {
    MenuCBState* state = pane->state;

    if (state->markKind != 0) {
        winSpriteSetDisp(sprite, FALSE);
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; push; scheduling on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop

#pragma push
#pragma scheduling on
#pragma peephole off
s32 pcBoxSelectCommand(s32 context, s32 mode) {
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
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop

#pragma push
#pragma peephole off
s32 pcBoxMainCursor(MenuCBPane* pane) {
    MenuKeyInfo* keys;
    MenuCursorItem* item;
    s32 cursor;
    s32 action;
    s32 previousAction;

    keys = windowGetKeyInfo();
    if (pcBoxCursorBusy() != 0) {
        return 0;
    }
    if (pcBoxTabIsMoving() != 0) {
        return 0;
    }

    if ((keys->buttonsRepeat & 0xC0) != 0) {
        if (pcBoxGetHeldFlag() != 0) {
            cursor = 0;
        } else {
            cursor = 1;
        }
        pcBoxSetHeldFlag(cursor);
    }

    if ((keys->buttons & 0x400) != 0) {
        pcBoxTabNext();
        return 0;
    }
    if ((keys->buttons & 0x200) != 0) {
        pcBoxTabPrev();
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
            pcBoxTabNext();
            return 0;
        }
        if (keys->buttons == 4) {
            pcBoxTabPrev();
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
        pcBoxSetCursorTarget(item->x, item->y, 0);
        return 0;
    }
    return 0;
}
#pragma pop

void pcBoxMainButton(MenuCBPane* pane) {
    windowGetKeyInfo();
    if (pcBoxCursorBusy() == 0) {
        if (pcBoxTabIsMoving() == 0) {
            menuButtonNormal(pane);
        }
    }
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void pcBoxMovePokemon(MenuCBPane* pane) {
    s32 selectedSlot;
    s32 state;
    s32 box;

    state = pcBoxGetHoldState();
    switch (state) {
    case 1:
        if (pcBoxHeldSequenceDone() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = pcBoxGetTab();
        pcBoxCopyToHeld(getPokemon__5PCBOXFScSc(NULL, (s8)box, (s8)selectedSlot));
        delPokemon__5PCBOXFScSc(NULL, (s8)box, (s8)selectedSlot);
        pcBoxSetHoldState(2);
        break;
    case 2:
        if (pcBoxHeldSequenceDone() == 0) {
            break;
        }
        pcBoxSetHoldState(3);
        if ((s32)lbl_8047A560 != 0) {
            pane->flag98 = 1;
        }
        break;
    case 4:
        if (pcBoxHeldSequenceDone() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = pcBoxGetTab();
        setPokemon__5PCBOXFP7PokemonScSc(NULL, pcBoxGetHeld(), (s8)box,
                                          (s8)selectedSlot);
        pcBoxClearHeld();
        pcBoxSetHoldState(5);
        break;
    case 5:
        if (pcBoxHeldSequenceDone() == 0) {
            break;
        }
        pcBoxSetHoldState(0);
        break;
    case 6:
        if (pcBoxHeldSequenceDone() == 0) {
            break;
        }
        box = pane->boxIndex;
        if (box >= 0 && box < 0x20) {
            selectedSlot = lbl_80267398[box].slot;
        }
        box = pcBoxGetTab();
        setPokemon__5PCBOXFP7PokemonScSc(NULL, pcBoxGetHeld(), (s8)box,
                                          (s8)selectedSlot);
        pcBoxClearHeld();
        pcBoxSwapHeldBuffer();
        pcBoxSetHoldState(3);
        break;
    }
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop

#pragma push
#pragma peephole off
s32 pcBoxMainCtrl(MenuCBStatusWork* work) {
    MenuCursorItem* item;
    s32 selected;

    extern MenuCursorItem* windowGetCursorToItem(MenuCBStatusWork* work);
    extern void pcBoxSetCursorTarget(s16 x, s16 y, s32 selected);
    extern void pcBoxUpdateCursor(void);
    extern void pcBoxTabUpdateSlide(void);
    extern s32 windowGetActiveID(void);
    extern void pcBoxMovePokemon(MenuCBStatusWork* work);

    switch (work->state) {
    case 0:
        if (work->initialized == 0) {
            selected = TRUE;
        } else {
            selected = FALSE;
        }
        item = windowGetCursorToItem(work);
        pcBoxSetCursorTarget(item->x, item->y, selected);
        work->initialized = 1;
        break;
    case 2:
        pcBoxUpdateCursor();
        pcBoxTabUpdateSlide();
        if (work->windowId == windowGetActiveID()) {
            pcBoxMovePokemon(work);
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

s32 pcBoxGetGridCell(s32* outSlot, s32 index) {
    MenuCBSlotInfo* info;

    if (index < 0 || index >= 0x20) {
        return 3;
    }

    info = &lbl_80267398[index];
    *outSlot = info->slot;
    return info->kind;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void* pcBoxGetPokemonAt(void* pokemon, s32 direction, s32* moveState)
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
        while (slot < pcBoxGetSlotCount()) {
            candidate = getPokemon__5PCBOXFScSc(NULL, (s8) box, (s8) slot);
            if (pokemonCheckValid(candidate) != 0) {
                break;
            }
            slot++;
        }
        if (slot < pcBoxGetSlotCount()) {
            moveState[2] = slot;
            return candidate;
        }
    }

    return pokemon;
}
#pragma pop
