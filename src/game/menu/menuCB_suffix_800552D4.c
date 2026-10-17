/**
 * @file menuCB_suffix_800552D4.c
 * @brief colosseum-battle Pokemon-select suffix, 0x800552D4 - 0x80055E38.
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
extern s32 fn_800573C0(void);
extern s32 fn_800566E8(void);
extern s32 fn_80057E40(MenuCBPane* pane);
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
s32 fn_80057694(void);
void fn_800576A4(u32 value);
u32 fn_800567AC(void);
u32 fn_80056704(void);
void menuCursorNormal(MenuCBPane* pane);
MenuCursorItem* windowGetCursorToItem();
void fn_80057830(s16 x, s16 y, s32 selected);
extern void fn_80054760(s32 forward, s32 wait);
extern void pcBoxRun(void);
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
extern void gfxSetFlags(u32 flags);
extern void fn_800DC0D4(s32, s32, s32, s32, s32);
extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
extern void fn_800DBFD4(s32, s32, s32, s32, s32);
extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
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
void* fn_800551CC(void* pokemon, s32 direction, s32* moveState);
static inline s32 menuCBFindBoxSlot(s32* state)
{
    MenuCBSlotInfo* entry;
    s32 target;
    s32 i;

    target = state[2];
    for (i = 0, entry = lbl_80267398; i < 32; entry++, i++) {
        if (entry->kind == 0 && target == entry->slot) {
            break;
        }
    }
    return i;
}

s32 pcBoxPokemonCommand(s32 box, s32 slot)
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
    extern void fn_80057C9C(void*, void*, s32*);
    extern void* menuItemBiosGetPtr(s32);
    extern void fn_80057830(s16, s16, s32);
    extern MenuCBPane* windowSearchID(s32);
    extern u8 fn_8005D738(u8);
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
    u16 name[0x10];
    s32 moveState[3];
    MenuCBPane* window;
    void* item;
    s32 emptySlots;
    s32 i;
    s32 action;
    s32 mode;
    s32 context;
    s32 answer;
    u32 nameValue;
    u8 canAct;
    u8 mark;
    void* pokemon;
    void* boxPokemon;
    void* alternatePokemon;
    s32 haveCopy;

    haveCopy = 0;
    boxPokemon = getPokemon__5PCBOXFScSc(NULL, box, slot);
    canAct = pokemonCheckValid(boxPokemon);
    mode = 1;

    if (fn_800576B4() != 3) {
        alternatePokemon = NULL;
        if (canAct == 0) {
            mode = 0;
        } else {
            context = 0;
        }
    } else {
        alternatePokemon = fn_800574E0();
        if (canAct == 0) {
            context = 1;
        } else {
            context = 2;
        }
    }
    if (mode == 0) {
        return slot;
    }
    if (alternatePokemon == NULL) {
        pokemon = boxPokemon;
    } else {
        pokemon = alternatePokemon;
    }

    if (fn_80057694() == 0) {
        nameValue = pokemonGetStatus(pokemon, 0, 0x77, 0);
        msgctrlSetValue(0x36, nameValue);
        msgctrlSetValue(0x32, nameValue);
        fn_80058804((void*)0x1B74, 0);
        action = fn_80054B1C(context, 0);
        fn_800587D8();
    } else {
        switch (context) {
        case 0:
            action = 0;
            break;
        case 1:
            action = 1;
            break;
        case 2:
            action = 2;
            break;
        default:
            action = 8;
            break;
        }
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
    case 4:
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
            while (fn_80054680() == 2) {
                _threadSwitch();
            }
            fn_800599AC(1);
            fn_800546C0(0);
            while (fn_80054680() == 3) {
                _threadSwitch();
            }
            break;
        }
        lbl_8047A560 = 1;
        fn_800576C4(1);
        break;
    case 3:
        fadeSet(3, lbl_8047BE98);
        fadeCheck(1);
        fn_8005471C();
        fn_80056A80();
        if (fn_800576B4() == 3) {
            haveCopy = 1;
            pokemonCopy = *(MenuCBPokemonBlob*)fn_800574E0();
        }
        context = fn_80057694();
        fn_80057A38();
        if (alternatePokemon != NULL) {
            moveState[0] = 1;
        } else {
            moveState[0] = 0;
        }
        moveState[1] = box;
        moveState[2] = slot;
        fn_80057C9C(pokemon, fn_800551CC, moveState);
        fn_80056B74((MenuCBPane*)box, 0);
        fn_80054760(0, 0);
        fn_80057A64(haveCopy != 0 ? &pokemonCopy : NULL, context);
        if (moveState[0] == 0) {
            i = menuCBFindBoxSlot(moveState);
            if (i < 32) {
                item = menuItemBiosGetPtr(lbl_80267398[i].itemId);
                fn_80057830(*(s16*)((u8*)item + 2),
                             *(s16*)((u8*)item + 4), 1);
            }
            slot = moveState[2];
            window = windowSearchID(0x93);
            if (window != NULL) {
                window->boxIndex = slot;
            }
        }
        fadeSet(2, lbl_8047BE98);
        fadeCheck(1);
        break;
    case 6:
        fn_80058804((void*)0x1B88, 0);
        mark = fn_8005D738(pokemonBiosGetPcboxMark(pokemon));
        if (mark != 0xFF) {
            pokemonBiosSetPcboxMark(pokemon, mark);
        }
        fn_800587D8();
        break;
    case 7:
        if (fn_80057DE8(pokemon) != 0) {
            fn_80058804((void*)0x1B94, 1);
            break;
        }
        fn_80058804((void*)0x1B91, 0);
        answer = fn_8005D3D0(1);
        fn_800587D8();
        if (answer == 1) {
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

static inline void fn_800558B8_open(u32 box) {
    extern u32 fn_80057B34(u32, u32);
    fn_80057B34(3, box);
    fn_80056B74((MenuCBPane*)box, 0);
}

static inline void fn_800558B8_rebuild(void) {
    extern s32 fn_80057694(void);
    extern void fn_80057A38(void);
    extern u32 fn_80056A78(void);
    extern void fn_80057A64(void*, s32);
    extern s32 fn_8005D26C(void);
    extern void fadeSet(s32, f32);
    extern s32 fadeCheck(s32);
    extern void fn_8005471C(void);
    extern void fn_80056A80(void);
    extern void* fn_800574E0(void);
    extern s32 fn_800576B4(void);
    extern void fn_80058804(void*, s32);
    extern void fn_800587D8(void);
    s32 haveCopy;
    s32 result;
    s32 count;
    MenuCBPokemonBlob pokemonCopy;

    haveCopy = 0;
    fn_80058804((void*)0x1B87, 0);
    result = fn_8005D26C();
    fn_800587D8();
    switch (result) {
    case 0:
        fadeSet(3, lbl_8047BE98);
        fadeCheck(1);
        fn_8005471C();
        fn_80056A80();
        if (fn_800576B4() == 3) {
            haveCopy = 1;
            pokemonCopy = *(MenuCBPokemonBlob*)fn_800574E0();
        }
        count = fn_80057694();
        fn_80057A38();
        fn_800558B8_open(fn_80056A78());
        fn_80054760(0, 0);
        fn_80057A64(haveCopy != 0 ? &pokemonCopy : NULL, count);
        fadeSet(2, lbl_8047BE98);
        fadeCheck(1);
        break;
    }
}

void pcBoxRun(void)
{
    extern void soundStart(s32, s32, s32, s32);
    extern s32 fn_800576B4(void);
    extern void fn_80058804(void*, s32);
    extern s32 fn_8005D3D0(s32);
    extern void fn_800587D8(void);
    extern void fn_800546F0(s32);
    extern s32 fn_80054680(void);
    extern void fn_800599AC(s32);
    extern void fn_800546C0(s32);
    extern u32 fn_80056A78(void);
    extern s32 pcBoxPokemonCommand();
    extern s32 fn_8005D26C(void);
    extern void fadeSet(s32, f32);
    extern s32 fadeCheck(s32);
    extern void fn_8005471C(void);
    extern void fn_80056A80(void);
    extern void* fn_800574E0(void);
    extern s32 fn_80057694(void);
    extern void fn_80057A38(void);
    extern u32 fn_80057B34(u32, u32);
    extern void fn_80057A64(void*, s32);
    s32 selectedSlot;
    s32 slot;
    s32* selectedSlotPtr;
    s32 selection;
    s32 result;

    selectedSlot = 0;
    selectedSlotPtr = &selectedSlot;
    lbl_8047A560 = 0;
    soundStart(0x27, 0, 0xFF, 0);

    for (;;) {
        selection = menuOpenCustom(0x93, 0x10E, (s32)selectedSlotPtr, 0, 1, 0);
        selectedSlotPtr = NULL;

        if (selection == -1) {
            if (fn_800576B4() == 3) {
                fn_80058804((void*)0x1B7E, 1);
                continue;
            }

            fn_80058804((void*)0x1B8B, 0);
            result = fn_8005D3D0(0);
            fn_800587D8();
            switch (result) {
            case 0:
                continue;
            }
            break;
        }

        if ((s32)lbl_8047A560 != 0) {
            fn_800546F0(0);
            while (fn_80054680() == 2) {
                _threadSwitch();
            }
            fn_800599AC(1);
            fn_800546C0(0);
            while (fn_80054680() == 3) {
                _threadSwitch();
            }
            lbl_8047A560 = 0;
            continue;
        }

        if (selection < 0 || selection >= 32) {
            result = 3;
        } else {
            slot = lbl_80267398[selection].slot;
            result = lbl_80267398[selection].kind;
        }

        switch (result) {
        case 2:
            fn_800546F0(0);
            while (fn_80054680() == 2) {
                _threadSwitch();
            }
            fn_800599AC(0);
            fn_800546C0(0);
            while (fn_80054680() == 3) {
                _threadSwitch();
            }
            selectedSlot = 0;
            selectedSlotPtr = &selectedSlot;
            break;
        case 0:
            selectedSlot = pcBoxPokemonCommand(fn_80056A78(), slot);
            selectedSlotPtr = &selectedSlot;
            break;
        case 1:
            fn_800558B8_rebuild();
            break;
        }
    }

    soundStart(0x28, 0, 0xFF, 0);
    menuCloseCustom(0x93, 2, 0);
    menuCloseSync(0x93, 1);
}

void fn_80055B98(MenuCBPane* pane) {
    menuOpenCustom(0x10e, 0x1f, 0, 0, 0, 0);
    fn_80056B74(pane, TRUE);

    while (fn_80055E38(pane) == 0) {
        _threadSwitch();
    }

    fn_80054760(FALSE, TRUE);
    fn_80057A64(NULL, 0);
    pcBoxRun();
    fn_80057A38();
    fn_8005471C();
    fn_80056A80();
}

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

s32 pcBoxDrawBoxNameItem(MenuCBPane* pane, MenuCBPane* sprite) {
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

s32 pcBoxBoxSelectKey(MenuCBPane* pane) {
    MenuKeyInfo* keyInfo;

    keyInfo = windowGetKeyInfo();
    if ((keyInfo->buttonsRepeat & 8) != 0) {
        if (++pane->boxIndex >= (s8)pcboxGetNbPokemonBox()) {
            pane->boxIndex = 0;
        }
    }
    if ((keyInfo->buttonsRepeat & 4) != 0) {
        if (--pane->boxIndex < 0) {
            pane->boxIndex = pcboxGetNbPokemonBox() - 1;
        }
    }
    return 0;
}

void fn_80055DE0(void) {
    menuClose(0xa3);
    menuCloseSync(0xa3, 1);
}

void fn_80055E10(void) {
    menuOpen(0xa3, 1);
}
