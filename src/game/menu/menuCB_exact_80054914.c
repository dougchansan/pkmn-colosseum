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
#pragma scheduling off
#pragma dont_inline on
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
