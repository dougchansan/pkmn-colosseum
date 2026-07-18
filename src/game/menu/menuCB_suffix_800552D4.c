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
void* fn_800551CC(void* pokemon, s32 direction, s32* moveState);
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
