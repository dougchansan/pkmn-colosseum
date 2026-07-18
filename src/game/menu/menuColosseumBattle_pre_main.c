/**
 * @file menuColosseumBattle_pre_main.c
 * @brief menuColosseumBattle.cpp pre-main range, 0x80058DCC - 0x800599AC.
 */
#include "dolphin/types.h"
#include "game/data/rodata_80267398.h"
#include "game/menu/menu.h"
#include "game/menu/menu_name_entry.h"

typedef struct MenuKeyInfo {
    u8 pad_00[4];
    u16 buttons;
    u16 repeatButtons;
} MenuKeyInfo;

typedef struct MenuWindow {
    u8 pad_00[0x4C];
    s32 nextMenu;
    u8 pad_50[0x10];
    void* partyState;
    u8 pad_64[0x31];
    s8 cursor;
    u8 pad_96;
    s8 previousCursor;
    u8 accepted;
    u8 canceled;
} MenuWindow;

typedef struct MenuSprite {
    u8 pad_00[6];
    s16 tag;
} MenuSprite;

typedef struct MenuCursorItem {
    u8 pad_00[2];
    s16 field_02;
    s16 field_04;
} MenuCursorItem;

typedef struct MenuSeTable {
    u16 field_00;
    u16 field_02;
    u16 field_04;
    u16 field_06;
    u16 field_08;
} MenuSeTable;

typedef struct ColosseumBattleConnectState {
    s32 active;
    s32 connected;
    s32 busyRequest;
    s32 reserved;
} ColosseumBattleConnectState;

typedef struct ColosseumSaveWork {
    s32 battleKind;
    u8 pad_04[0x18];
    u8 exitPending;
} ColosseumSaveWork;

typedef struct ColosseumMenuHeap {
    u8 pad_0000[0x4314];
    void* resourceHandle;
} ColosseumMenuHeap;

typedef struct ColosseumMessageBuffer {
    u16 text[0x24];
} ColosseumMessageBuffer;

typedef struct ColosseumRosterRow {
    s32 mode;
    s32 menuId;
    s32 partyIndex;
    s32 action;
} ColosseumRosterRow;

typedef union ColosseumBitMasks {
    u32 word;
    u8 bytes[4];
} ColosseumBitMasks;

typedef struct ColosseumUnownMenuIds {
    u32 normal;
    u32 shiny;
} ColosseumUnownMenuIds;

typedef struct ColosseumPokemonBlob {
    u8 bytes[0x138];
} ColosseumPokemonBlob;

/* ===== External function declarations (menuColosseumBattleMain only) ===== */
extern void fn_800347B8(void);
extern void fn_800347C4(void);
extern void fn_800347E8(s32);
extern void fn_8003480C(s32);
s32 fn_800566E8(void);
extern s32 fn_800573C0(void);
s32 fn_80057694(void);
void fn_800576A4(s32);
void fn_80057830(s32, s32, s32);
extern u8 fn_8006B8E8(void);
extern u32 fn_800E202C(void*);
extern void fn_800E209C(u32);
extern void fn_800E24B0(u32);
extern void* fn_800E27B0(u32);
extern void fn_800F96E4(void*, s32, void*);
extern void fn_8002D91C();
extern void fn_80062948(void);
extern void fn_80069C0C();
extern void fn_8006A76C();
extern void fn_8006A79C();
extern void fn_8006A7AC();
extern void fn_8006A7BC();
extern void fn_8006A7C8();
extern void fn_8006A7E0();
extern void fn_8006A7E8();
extern void fn_8006A7F0();
extern void fn_8006A81C();
extern void fn_8006A824();
extern void fn_8006AC28();
extern void fn_8006ADB4();
extern void fn_8006AF44();
extern void fn_8006AFC4();
extern void fn_8006AFE4();
extern void fn_8006B09C();
extern void fn_8006B4AC();
extern void fn_8006B51C();
extern void fn_8006B8F0();
extern void fn_8006B8FC();
extern void fn_8006E0CC();
extern void fn_800FF58C(s32);
extern void fn_80071160();
extern void fn_80071344();
extern void fn_80071398();
extern void fn_800714C8();
extern void fn_800715BC();
extern void fn_8007162C();
extern void fn_800776E4();
extern void fn_80077E80();
extern void fn_80077EA4();
extern void fn_800849B4();
extern void fn_80088964();
extern void fn_80088C60();
extern void fn_800889A4();
extern void fn_80088D84();
extern void fn_80089028();
extern void fn_80092C90();
extern void fn_80093574();
extern void menuGetCursorItemID();
extern void menuSubOpenYesNo();
extern void menuClose();
extern void menuCloseCustom();
extern s32 menuCloseSync(s32, s32);
extern void menuIsCheck();
extern s32 menuOpen(s32, s32);
extern s32  fn_8010264C();
extern void menuSetPosition();
extern u8*  fn_80104704(u32);
MenuCursorItem* windowGetCursorToItem(MenuWindow*);
extern u8*  windowGetKeyInfo(void);
extern void winMsgOpen();
extern void winMsgClose();
extern void fn_80108518();
extern void floorLink();
extern void* savedataGetStatus();
extern void fn_80129384();
extern void fn_801293FC();
extern void heroInit();
extern void heroBiosSetHomePlace();
extern u32  heroBiosGetRnd(void*);
extern u8*  heroBiosGetNamePtr(void*);
extern void heroBiosCopy();
extern void heroMoveSyncWithHero();
extern void msgctrlSetValue();
extern void gamedatasaveGetStatus();
extern void fn_80166A28();
extern void fn_80166A50(s32, s32, s32, s32);
extern void fn_8019075C();
extern void fn_801CB9D8(void*);
extern u8 pokemonCheckValid(void*);
extern u8 pokemonIsDarkPokemon(void*);
extern void __assert();
extern void _threadSwitch();
extern void _flagSet(s32, s32);
extern void floorChangePos();
extern s32  GScharCmp(void*, void*);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void menuButtonNormal(void*);
extern void winSpriteSetDisp(void*, u32);
extern void fadeCheck(s32);
extern void toolentryTaisenFreePokemonData(void);
extern void scriptStoreTemochiPokemon(s32);
extern u8 scriptCheckTemochiPokemon(s32);
extern void* getPokemon__5PCBOXFScSc(void*, s8, s8);
extern s8 pcboxGetNbPokemonBox(void);
extern s8 fn_801347D8(void);
extern void* heroGetStatus(void*, s32, u16);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern u32 pokemonGetStatus(void*, u32, u32, u32);
extern u8 pokemonGetAnnonKatati(u32);
extern u32 _toolentryAlloc__FUl(u32);
extern u32 fn_800FF560(void);
extern u32 GSthreadCreate(u32, u32, u32, u32, u32, void*);
extern void fn_80057E70(void);
extern void fn_8017B3E4(s32);
extern s32 fn_8017B2CC(s32);
extern void fn_8017B1CC(s32);
extern void fn_800F915C(s32);
extern void fn_80055B98(s32);
extern void fn_80190528(s32);
extern s32 fn_800576B4(void);
extern void* fn_800574E0(void);
extern void fn_800576C4(s32);
extern void fn_80057458(void*);
extern s32 fn_80054B1C(s32, s32);
extern void fn_80055DE0(void);
extern s32 fn_80055E10(void);
extern s8 pcboxGetPokemonBoxNbEmptySlot(s32, s8);
extern void pcboxAddPokemon(s32, void*, s8);
extern void pokemonInit(void*);
extern void fadeSet(s32, f32);
extern s32 fn_80056A78(void);
extern void fn_80056A80(void);
extern s32 fn_80057C9C(void*, void*, s32*);
extern void fn_80056B74(s32, s32);
extern void fn_80054760(s32, s32);
extern void fn_80057A38(void);
extern void fn_80057A64(void*, s32);
extern void* menuItemBiosGetPtr(s32);
extern u8 pokemonBiosGetPcboxMark(void*);
extern void pokemonBiosSetPcboxMark(void*, u8);
extern s32 fn_8005D3D0(s32);
extern void GScharCpy(void*, void*);
extern void fn_80054670(s32);
extern s32 fn_8005464C(void);
extern void fn_8005744C(void);
extern s32 fn_80057428(void);
extern void fn_800574A8(void);
extern void* windowSearchID(s32);
extern u32 fn_8005D738(u8);
extern void fn_80058804(void*, s32);
extern void fn_800587D8(void);
extern void fn_8005471C(void);

/* ===== SDA globals ===== */
extern s32 lbl_8047A598;
extern s32 lbl_8047A59C;
extern void* lbl_8047A590;
extern u8* lbl_8047A5A0;
extern u8  lbl_8047A5A8;
extern f32 lbl_8047BF18;
extern u8  lbl_8047BF1C;
extern u8  lbl_8047BF20;
extern u8  lbl_8047BF24;
extern const u32 lbl_8047BF30;
extern const u32 lbl_8047BF34;
extern const u32 lbl_8047BF38;
extern const u32 lbl_8047BF3C;
extern const ColosseumBitMasks lbl_8047BF40;
extern const char lbl_8047BF28;
extern const f32 lbl_8047BF10;

/* ===== Rodata / data labels ===== */
extern u8 lbl_80267840[];
extern char lbl_802678D8[];
extern const ColosseumUnownMenuIds lbl_802676F0[28];
extern const ColosseumRosterRow lbl_802677D0[7];
extern const s32 lbl_80267A80[6];
extern const s32 lbl_80267A98[6];
extern const s32 lbl_80267AB0[18];
extern u8 lbl_803A9A08[];
extern u8 lbl_803A9A18[];

/* ===== Function implementations ===== */

/* Address: 0x80058F08 | Size: 0x38 */
s32 fn_80058F08(s32* partyIndex, s32 cursor) {
    const ColosseumRosterRow* row;

    if (cursor < 0 || cursor >= 7) {
        return 2;
    }

    row = &lbl_802677D0[cursor];
    *partyIndex = row->partyIndex;
    return row->menuId;
}

/* Address: 0x80058F40 | Size: 0xF4 */
#pragma push
#pragma peephole off
void* fn_80058F40(void* dflt, s32 dir, s32* state) {
    extern void* heroGetStatus(void*, s32, u16);
    void* hero;
    s32 i;

    if (state[0] != 0) {
        return dflt;
    }
    if (dir == 1) {
        for (i = state[1] - 1; i >= 0; i--) {
            u32 v;
            hero = heroGetStatus(NULL, 3, (u16)i);
            v = pokemonCheckValid(hero);
            if (v != 0) {
                break;
            }
        }
        if (i >= 0) {
            state[1] = i;
            return hero;
        }
    }
    if (dir == 2) {
        for (i = state[1] + 1; i < 6; i++) {
            u32 v;
            hero = heroGetStatus(NULL, 3, (u16)i);
            v = pokemonCheckValid(hero);
            if (v != 0) {
                break;
            }
        }
        if (i < 6) {
            state[1] = i;
            return hero;
        }
    }
    return dflt;
}
#pragma pop

/* Handle an action selected for one party slot. */
s32 fn_80059034(s32 slot)
{
    ColosseumPokemonBlob pokemonCopy;
    u8 pokemonName[0x1C];
    s32 navigation[2];
    void* heroPokemon;
    void* alternatePokemon;
    void* pokemon;
    void* partyPokemon;
    s32 battleMode;
    s32 context;
    s32 action;
    s32 count;
    s32 i;
    s32 box;
    s32 selection;
    s32 haveCopy;
    u32 name;

    heroPokemon = heroGetStatus(0, 3, (u16)slot);
    alternatePokemon = 0;
    battleMode = 1;

    if (fn_800576B4() != 3) {
        context = 0;
        if (!pokemonCheckValid(heroPokemon)) {
            battleMode = 0;
        }
    } else {
        alternatePokemon = fn_800574E0();
        context = pokemonCheckValid(heroPokemon) ? 2 : 1;
    }

    if (battleMode == 0) {
        return slot;
    }
    pokemon = alternatePokemon != 0 ? alternatePokemon : heroPokemon;

    if (fn_80057694() == 0) {
        name = pokemonGetStatus(pokemon, 0, 0x77, 0);
        msgctrlSetValue(0x36, name);
        msgctrlSetValue(0x32, name);
        fn_80058804((void*)0x1B74, 0);
        action = fn_80054B1C(context, 1);
        fn_800587D8();
    } else {
        action = context >= 0 && context < 3 ? context : 8;
    }

#define COUNT_MARKED_PARTY(result)                                           \
    do {                                                                     \
        (result) = 0;                                                        \
        for (i = 0; i < 6; i++) {                                           \
            partyPokemon = heroGetStatus(0, 3, (u16)i);                     \
            if (partyPokemon == 0) {                                        \
                break;                                                       \
            }                                                                \
            if (pokemonCheckValid(partyPokemon) &&                           \
                (s32)pokemonGetStatus(partyPokemon, 0, 0x83, 0) > 0) {      \
                (result)++;                                                  \
            }                                                                \
        }                                                                    \
    } while (0)

    switch (action) {
    case 0:
        if ((s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            COUNT_MARKED_PARTY(count);
            if (count <= 1) {
                fn_80058804((void*)0x1B90, 1);
                break;
            }
        }
        fn_800576C4(1);
        break;

    case 1:
        fn_800576C4(4);
        break;

    case 2:
        if ((s32)pokemonGetStatus(alternatePokemon, 0, 0x83, 0) <= 0 &&
            (s32)pokemonGetStatus(heroPokemon, 0, 0x83, 0) > 0) {
            COUNT_MARKED_PARTY(count);
            if (count < 2) {
                fn_80058804((void*)0x1B90, 1);
                break;
            }
        }
        fn_80057458(heroPokemon);
        pokemonInit(heroPokemon);
        fn_800576C4(6);
        break;

    case 3:
        fadeSet(3, lbl_8047BF10);
        fadeCheck(1);
        fn_8005471C();
        selection = fn_80056A78();
        fn_80056A80();

        haveCopy = 0;
        if (fn_800576B4() == 3) {
            haveCopy = 1;
            pokemonCopy = *(ColosseumPokemonBlob*)fn_800574E0();
        }
        context = fn_80057694();
        fn_80057A38();

        navigation[0] = alternatePokemon != 0;
        navigation[1] = slot;
        fn_80057C9C(pokemon, fn_80058F40, navigation);
        fn_80056B74(selection, 0);
        fn_80054760(1, 0);
        fn_80057A64(haveCopy ? &pokemonCopy : 0, context);

        if (navigation[0] == 0) {
            for (i = 0; i < 7; i++) {
                if (lbl_802677D0[i].menuId == 0 &&
                    lbl_802677D0[i].partyIndex == navigation[1]) {
                    break;
                }
            }
            if (i < 7) {
                MenuSprite* item = (MenuSprite*)menuItemBiosGetPtr(
                    lbl_802677D0[i].mode);
                fn_80057830(*(s16*)((u8*)item + 2),
                            *(s16*)((u8*)item + 4), 1);
            }
            {
                u8* targetWindow = (u8*)windowSearchID(0x94);
                if (targetWindow != 0) {
                    targetWindow[0x95] = (s8)navigation[1];
                }
            }
        }
        fadeSet(2, lbl_8047BF10);
        fadeCheck(1);
        break;

    case 4:
        break;

    case 5:
        if (fn_800576B4() == 0 &&
            (s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            COUNT_MARKED_PARTY(count);
            if (count <= 1) {
                fn_80058804((void*)0x1B90, 1);
                break;
            }
        }

        fn_80058804((void*)0x1B80, 0);
        do {
            box = fn_80055E10();
            fn_800587D8();
            if (box < 0 || box >= pcboxGetNbPokemonBox()) {
                box = -1;
                break;
            }
            if (pcboxGetPokemonBoxNbEmptySlot(0, (s8)box) <= 0) {
                fn_80058804((void*)0x1B81, 1);
                fn_80058804((void*)0x1B80, 0);
            }
        } while (pcboxGetPokemonBoxNbEmptySlot(0, (s8)box) <= 0);
        fn_80055DE0();
        if (box < 0) {
            break;
        }

        pcboxAddPokemon(0, pokemon, (s8)box);
        if (fn_800576B4() == 3) {
            fn_800574A8();
            fn_800576C4(0);
            break;
        }
        if (slot >= 0 && slot < 6) {
            for (i = slot + 1; i < 6; i++) {
                partyPokemon = heroGetStatus(0, 3, (u16)i);
                if (partyPokemon == 0) {
                    break;
                }
                heroPokemon = heroGetStatus(0, 3, (u16)(i - 1));
                if (heroPokemon == 0) {
                    break;
                }
                *(ColosseumPokemonBlob*)heroPokemon =
                    *(ColosseumPokemonBlob*)partyPokemon;
            }
            if (i == 6) {
                pokemonInit(heroGetStatus(0, 3, 5));
            }
        }
        break;

    case 6:
        fn_80058804((void*)0x1B88, 0);
        name = pokemonBiosGetPcboxMark(pokemon);
        name = fn_8005D738((u8)name);
        if ((u8)name != 0xFF) {
            pokemonBiosSetPcboxMark(pokemon, (u8)name);
        }
        fn_800587D8();
        break;

    case 7:
        if (fn_80057DE8(pokemon)) {
            fn_80058804((void*)0x1B94, 1);
            break;
        }
        if (alternatePokemon == 0 &&
            (s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            COUNT_MARKED_PARTY(count);
            if (count <= 1) {
                fn_80058804((void*)0x1B90, 1);
                break;
            }
        }

        fn_80058804((void*)0x1B91, 0);
        selection = fn_8005D3D0(1);
        fn_800587D8();
        if (selection == 1) {
            break;
        }

        GScharCpy(pokemonName,
                  (void*)pokemonGetStatus(pokemon, 0, 0x77, 0));
        if (alternatePokemon == 0) {
            fn_80054670(slot);
            while (fn_8005464C() != 0) {
                _threadSwitch();
            }
            if (slot >= 0 && slot < 6) {
                pokemonInit(heroGetStatus(0, 3, (u16)slot));
            }
        } else {
            fn_8005744C();
            while (fn_80057428() != 0) {
                _threadSwitch();
            }
            fn_800574A8();
            fn_800576C4(0);
        }

        msgctrlSetValue(0x36, pokemonName);
        fn_80058804((void*)0x1B92, 1);
        fn_80058804((void*)0x1B93, 1);
        if (alternatePokemon == 0 && slot >= 0 && slot < 6) {
            for (i = slot + 1; i < 6; i++) {
                partyPokemon = heroGetStatus(0, 3, (u16)i);
                if (partyPokemon == 0) {
                    break;
                }
                heroPokemon = heroGetStatus(0, 3, (u16)(i - 1));
                if (heroPokemon == 0) {
                    break;
                }
                *(ColosseumPokemonBlob*)heroPokemon =
                    *(ColosseumPokemonBlob*)partyPokemon;
            }
            if (i == 6) {
                pokemonInit(heroGetStatus(0, 3, 5));
            }
        }
        break;
    }

#undef COUNT_MARKED_PARTY
    return slot;
}
