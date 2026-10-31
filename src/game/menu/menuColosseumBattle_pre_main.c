/**
 * @file menuColosseumBattle_pre_main.c
 * @brief menuColosseumBattle.cpp pre-main candidate, 0x80059034 - 0x800599AC.
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
extern s32 pcBoxIsShadowPokemon(void* pokemon);
extern void cardeSetRecordFlag(void);
extern void cardeBuzzer_800347C4(void);
extern void cardeBuzzer_800347E8(s32);
extern void cardeBuzzer_8003480C(s32);
s32 pcBoxTabIsMoving(void);
extern s32 pcBoxCursorBusy(void);
s32 pcBoxGetHeldFlag(void);
void pcBoxSetHeldFlag(s32);
void pcBoxSetCursorTarget(s32, s32, s32);
extern u8 menuColosseumBattleGetFlag(void);
extern u32 GSmemGetHandleByPtr(void*);
extern void GSmemFree(u32);
extern void GSmemUnlock(u32);
extern void* GSmemLock(u32);
extern void GSmsgGetText(void*, s32, void*);
extern void shopRunAtLocation();
extern void menuTaisenRun(void);
extern void menuCBBattleSetupFight();
extern void memcardIsUnavailable();
extern void savedataBattleClearFlagC98B();
extern void savedataBattleSetFlagC98B();
extern void savedataBattleGetFlagC98B();
extern void savedataBattleGetParty();
extern void savedataBattleSetController();
extern void savedataBattleGetController();
extern void savedataBattleCopyPlayer();
extern void savedataBattleSetTrainerData();
extern void menuCBBiosInitHero();
extern void fn_8006AC28();
extern void savedataBattleSetCouponRun();
extern void savedataBattleSetSavedHero();
extern void savedataBattleGetSavedHero();
extern void savedataBattleGetEntryById();
extern void savedataBattleGetEntryPtr();
extern void menuSetRuleType();
extern void savedataBattleGetRuleSlotPtr();
extern void menuColosseumBattleClearFlag();
extern void menuColosseumBattleSetFlag();
extern void menuWaitFaceSprites();
extern void floorRequestChange(s32);
extern void menuFindLiveSaveSlot();
extern void _menuReopenTop();
extern void fn_80071398();
extern void fn_800714C8();
extern void fn_800715BC();
extern void _menuPeekStackDepth();
extern void menuRuleCheckPartyAndRule();
extern void menuRuleCopy();
extern void menuRuleEqual();
extern void cardeTransferUI();
extern void menuMemcardTaskA();
extern void menuSaveLoadCommitA();
extern void menuMemcardTaskB();
extern void menuSaveLoadCommitB();
extern void gbaGetConnectionMode();
extern void gbaCommStartSendPokemon();
extern void gbaCommWaitDone();
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
extern void soundStartSE();
extern void soundStart(s32, s32, s32, s32);
extern void fn_8019075C();
extern void std113_mapObjectUnk_8018db68(void*);
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
extern s8 pcBoxGetSlotCount(void);
extern void* heroGetStatus(void*, s32, u16);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern u32 pokemonGetStatus(void*, u32, u32, u32);
extern u8 pokemonGetAnnonKatati(u32);
extern u32 _toolentryAlloc__FUl(u32);
extern u32 floorGetCurrentContext(void);
extern u32 GSthreadCreate(u32, u32, u32, u32, u32, void*);
extern void pcBoxStreamModels(void);
extern void fsysStartLoad(s32);
extern s32 GSresGroupGetState(s32);
extern void GSresReleaseById(s32);
extern void GSresReleaseOwner(s32);
extern void pcBoxOpen(s32);
extern void std078_flagSetFalse(s32);
extern s32 pcBoxGetHoldState(void);
extern void* pcBoxGetHeld(void);
extern void pcBoxSetHoldState(s32);
extern void pcBoxCopyToOtherHeld(void*);
extern s32 pcBoxSelectCommand(s32, s32);
extern void menuPartyCloseMenuA3(void);
extern s32 menuPartyOpenMenuA3(void);
extern s8 pcboxGetPokemonBoxNbEmptySlot(s32, s8);
extern void pcboxAddPokemon(s32, void*, s8);
extern void pokemonInit(void*);
extern void fadeSet(s32, f32);
extern s32 pcBoxGetTab(void);
extern void pcBoxCloseTabs(void);
extern void pcBoxRunStatus(void*, void*, s32*);
extern void* pcBoxStepPartyPokemon(void*, s32, s32*);
extern void pcBoxOpenTabs(s32, s32);
extern void pcBoxOpenDetail(s32, s32);
extern void pcBoxCloseHeldWindow(void);
extern void pcBoxOpenHeldWindow(void*, s32);
extern void* menuItemBiosGetPtr(s32);
extern u8 pokemonBiosGetPcboxMark(void*);
extern void pokemonBiosSetPcboxMark(void*, u8);
extern s32 pcBoxSelectMode(s32);
extern void GScharCpy(void*, void*);
extern void menuPartySlotFadeStart(s32);
extern s32 menuPartySlotFadeDone(void);
extern void pcBoxHeldFadeStart(void);
extern s32 pcBoxHeldFadeDone(void);
extern void pcBoxClearHeld(void);
extern void* windowSearchID(s32);
extern u8 pcBoxConfirmRelease(u8);
extern void pcBoxShowMessage(void*, s32);
extern void pcBoxCloseMessage(void);
extern void pcBoxCloseDetail(void);

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

static inline s32 countMarkedParty(void)
{
    void* partyPokemon;
    s32 i;
    s32 count;

    count = 0;
    for (i = 0; i < 6; i++) {
        partyPokemon = heroGetStatus(0, 3, (u16)i);
        if (partyPokemon == 0) {
            break;
        }
        if (pokemonCheckValid(partyPokemon) &&
            (s32)pokemonGetStatus(partyPokemon, 0, 0x83, 0) > 0) {
            count++;
        }
    }
    return count;
}

static inline void closePartyGap(s32 slot)
{
    void* dst;
    void* src;
    s32 i;

    if (slot >= 0 && slot < 6) {
        for (i = slot + 1; i < 6; i++) {
            src = heroGetStatus(0, 3, (u16)i);
            if (src == 0) {
                break;
            }
            dst = heroGetStatus(0, 3, (u16)(i - 1));
            if (dst == 0) {
                break;
            }
            *(ColosseumPokemonBlob*)dst = *(ColosseumPokemonBlob*)src;
        }
        if (i >= 6) {
            pokemonInit(heroGetStatus(0, 3, 5));
        }
    }
}

static inline s32 selectPcBox(void)
{
    s32 box;

    while (1) {
        box = menuPartyOpenMenuA3();
        pcBoxCloseMessage();
        if (box < 0 || box >= pcboxGetNbPokemonBox()) {
            box = -1;
            break;
        }
        if (pcboxGetPokemonBoxNbEmptySlot(0, (s8)box) > 0) {
            break;
        }
        pcBoxShowMessage((void*)0x1B81, 1);
        pcBoxShowMessage((void*)0x1B80, 0);
    }
    return box;
}

static inline s32 findRosterRow(s32 partyIndex)
{
    s32 i;

    for (i = 0; i < 7; i++) {
        if (lbl_802677D0[i].menuId == 0 && lbl_802677D0[i].partyIndex == partyIndex) {
            break;
        }
    }
    return i;
}

/* Handle an action selected for one party slot. */
s32 menuPartySlotAction(s32 slot)
{
    ColosseumPokemonBlob pokemonCopy;
    u8 pokemonName[0x20];
    s32 navigation[2];
    void* heroPokemon;
    void* alternatePokemon;
    void* pokemon;
    s32 battleMode;
    s32 context;
    s32 count;
    s32 i;
    s32 box;
    s32 selection;
    s32 haveCopy;
    u8 pokemonValid;
    u32 name;
    u8 mark;

    haveCopy = 0;
    heroPokemon = heroGetStatus(0, 3, (u16)slot);
    pokemonValid = pokemonCheckValid(heroPokemon);
    battleMode = 1;

    if (pcBoxGetHoldState() != 3) {
        alternatePokemon = 0;
        if (!pokemonValid) {
            battleMode = 0;
        } else {
            context = 0;
        }
    } else {
        alternatePokemon = pcBoxGetHeld();
        if (!pokemonValid) {
            context = 1;
        } else {
            context = 2;
        }
    }

    if (battleMode == 0) {
        return slot;
    }
    pokemon = alternatePokemon == 0 ? heroPokemon : alternatePokemon;

    if (pcBoxGetHeldFlag() == 0) {
        name = pokemonGetStatus(pokemon, 0, 0x77, 0);
        msgctrlSetValue(0x36, name);
        msgctrlSetValue(0x32, name);
        pcBoxShowMessage((void*)0x1B74, 0);
        battleMode = pcBoxSelectCommand(context, 1);
        pcBoxCloseMessage();
    } else {
        switch (context) {
        case 0:
            battleMode = 0;
            break;
        case 1:
            battleMode = 1;
            break;
        case 2:
            battleMode = 2;
            break;
        default:
            battleMode = 8;
            break;
        }
    }

    switch (battleMode) {
    case 0:
        if ((s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            count = countMarkedParty();
            if (count <= 1) {
                pcBoxShowMessage((void*)0x1B90, 1);
                break;
            }
        }
        pcBoxSetHoldState(1);
        break;

    case 1:
        pcBoxSetHoldState(4);
        break;

    case 2:
        if ((s32)pokemonGetStatus(alternatePokemon, 0, 0x83, 0) <= 0 &&
            (s32)pokemonGetStatus(heroPokemon, 0, 0x83, 0) > 0) {
            count = countMarkedParty();
            if (count < 2) {
                pcBoxShowMessage((void*)0x1B90, 1);
                break;
            }
        }
        pcBoxCopyToOtherHeld(heroPokemon);
        pokemonInit(heroPokemon);
        pcBoxSetHoldState(6);
        break;

    case 5:
        if (pcBoxGetHoldState() == 0 &&
            (s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            count = countMarkedParty();
            if (count <= 1) {
                pcBoxShowMessage((void*)0x1B90, 1);
                break;
            }
        }

        pcBoxShowMessage((void*)0x1B80, 0);
        box = selectPcBox();
        menuPartyCloseMenuA3();
        if (box < 0) {
            break;
        }

        pcboxAddPokemon(0, pokemon, (s8)box);
        if (pcBoxGetHoldState() == 3) {
            pcBoxClearHeld();
            pcBoxSetHoldState(0);
            break;
        }
        closePartyGap(slot);
        break;

    case 3:
        fadeSet(3, lbl_8047BF10);
        fadeCheck(1);
        pcBoxCloseDetail();
        selection = pcBoxGetTab();
        pcBoxCloseTabs();

        if (pcBoxGetHoldState() == 3) {
            haveCopy = 1;
            pokemonCopy = *(ColosseumPokemonBlob*)pcBoxGetHeld();
        }
        context = pcBoxGetHeldFlag();
        pcBoxCloseHeldWindow();

        navigation[0] = alternatePokemon != 0 ? 1 : 0;
        navigation[1] = slot;
        pcBoxRunStatus(pokemon, pcBoxStepPartyPokemon, navigation);
        pcBoxOpenTabs(selection, 0);
        pcBoxOpenDetail(1, 0);
        pcBoxOpenHeldWindow(haveCopy ? &pokemonCopy : 0, context);

        if (navigation[0] == 0) {
            i = findRosterRow(navigation[1]);
            if (i < 7) {
                MenuSprite* item = (MenuSprite*)menuItemBiosGetPtr(
                    lbl_802677D0[i].mode);
                pcBoxSetCursorTarget(*(s16*)((u8*)item + 2),
                            *(s16*)((u8*)item + 4), 1);
            }
            slot = navigation[1];
            {
                MenuWindow* targetWindow = (MenuWindow*)windowSearchID(0x94);
                if (targetWindow != 0) {
                    targetWindow->cursor = (s8)slot;
                }
            }
        }
        fadeSet(2, lbl_8047BF10);
        fadeCheck(1);
        break;

    case 6:
        pcBoxShowMessage((void*)0x1B88, 0);
        mark = pcBoxConfirmRelease(pokemonBiosGetPcboxMark(pokemon));
        if (mark != 0xFF) {
            pokemonBiosSetPcboxMark(pokemon, mark);
        }
        pcBoxCloseMessage();
        break;

    case 7:
        if (pcBoxIsShadowPokemon(pokemon)) {
            pcBoxShowMessage((void*)0x1B94, 1);
            break;
        }
        if (alternatePokemon == 0 &&
            (s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            count = countMarkedParty();
            if (count <= 1) {
                pcBoxShowMessage((void*)0x1B90, 1);
                break;
            }
        }

        pcBoxShowMessage((void*)0x1B91, 0);
        selection = pcBoxSelectMode(1);
        pcBoxCloseMessage();
        if (selection == 1) {
            break;
        }

        GScharCpy(pokemonName,
                  (void*)pokemonGetStatus(pokemon, 0, 0x77, 0));
        if (alternatePokemon == 0) {
            menuPartySlotFadeStart(slot);
            while (menuPartySlotFadeDone() != 0) {
                _threadSwitch();
            }
            if (slot >= 0 && slot < 6) {
                pokemonInit(heroGetStatus(0, 3, (u16)slot));
            }
        } else {
            pcBoxHeldFadeStart();
            while (pcBoxHeldFadeDone() != 0) {
                _threadSwitch();
            }
            pcBoxClearHeld();
            pcBoxSetHoldState(0);
        }

        msgctrlSetValue(0x36, pokemonName);
        pcBoxShowMessage((void*)0x1B92, 1);
        pcBoxShowMessage((void*)0x1B93, 1);
        if (alternatePokemon == 0) {
            closePartyGap(slot);
        }
        break;

    case 4:
        break;
    }

    return slot;
}
