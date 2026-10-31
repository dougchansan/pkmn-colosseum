/**
 * @file menuColosseumBattle_tail.c
 * @brief menuColosseumBattle.cpp suffix candidate, 0x8005D184 - 0x8005D26C.
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
extern void fn_800E24B0(u32);
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
extern u32 pcBoxConfirmRelease(u8);
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

/* Address: 0x8005D184 | Size: 0xE8 */
s32 menu9EDrawSprite(MenuWindow* menu, MenuSprite* sprite) {
    s32 tags[6];
    s32* p;
    s32 i;
    s32 j;
    s32 visible;

    tags[0] = lbl_80267A80[0];
    tags[1] = lbl_80267A80[1];
    tags[2] = lbl_80267A80[2];
    tags[3] = lbl_80267A80[3];
    tags[4] = lbl_80267A80[4];
    tags[5] = lbl_80267A80[5];

    p = tags;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            if (sprite->tag == p[j]) {
                break;
            }
        }
        if (j < 3) break;
        p += 3;
    }
    if (i >= 2) {
        return 0;
    }
    if (menu->cursor == i) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
