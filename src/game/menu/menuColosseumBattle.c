/**
 * @file menuColosseumBattle.c
 * @brief menuColosseumBattle.cpp main/exit range, 0x80059BDC - 0x8005CEE8.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. XD has __sinit_menuColosseumBattle_cpp;
 * locals _colosseumbattlemenu_duel/_teamselect/_playerdecide/_gbac_connect_job,
 * menuColosseumBattleMain/Exit/Init, menuCB_FlashStart/End,
 * menuCB_SubWinDisp_GCENTRY (XD 0x800443E4-0x80046594).
 *
 * DIRECT filename proof: __assert first-arg string 'menuColosseumBattle.c'
 * at .data 0x802678D8 referenced by fn_8005CCD0 and fn_8005CF2C.
 *
 * menuColosseumBattleMain (below) is reintroduced from the previous
 * campaign's ui_core.c (archive/previous_campaign/src/game/ui/ui_core.c,
 * commits 745775c5 and 9f9727ef) through the current dtk-template pipeline:
 * ported into this unit's split and re-verified against this unit's own
 * compiler flags (GC/1.3, -use_lmw_stmw on, -sdata 8, -sdata2 8), not
 * copied wholesale.
 */
#include "dolphin/types.h"
#include "game/data/rodata_80267398.h"
#include "game/menu/menu.h"
#include "game/menu/menu_name_entry.h"
#include "game/menu/menu_middle.h"

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

typedef struct ColosseumBattleMenuData {
    s32 commands[6];
    s32 partyCommands[6];
    s32 battleKinds[7];
} ColosseumBattleMenuData;

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

typedef struct ColosseumBattleRules {
    u8 pad_0000[0xC9DC];
    u8 rules[7 * 0x54];
} ColosseumBattleRules;

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
extern u8 menuColosseumBattleGetFlag(void);
extern u32 GSmemGetHandleByPtr(void*);
extern void GSmemFree(u32);
extern void fn_800E24B0(u32);
extern void* GSmemLock(u32);
extern void GSmsgGetText(void*, s32, void*);
extern void fn_8002D91C();
extern void menuTaisenRun(void);
extern void menuCBBattleSetupFight();
extern void fn_8006A76C();
extern void savedataBattleClearFlagC98B();
extern void savedataBattleSetFlagC98B();
extern void savedataBattleGetFlagC98B();
extern void fn_8006A7C8();
extern void fn_8006A7E0();
extern void fn_8006A7E8();
extern void fn_8006A7F0();
extern void fn_8006A81C();
extern void menuCBBiosInitHero();
extern void menuCBBios_InitTrainer();
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
extern void fn_80071160();
extern void fn_80071344();
extern void fn_80071398();
extern void fn_800714C8();
extern void fn_800715BC();
extern void fn_8007162C();
extern void menuRuleCheckPartyAndRule();
extern void fn_80077E80();
extern void fn_80077EA4();
extern void fn_800849B4();
extern void fn_80088964();
extern void menuSaveLoadCommitA();
extern void fn_800889A4();
extern void fn_80088D84();
extern void fn_80089028();
extern void gbaCommStartSendPokemon();
extern void fn_80093574();
extern void menuGetCursorItemID();
extern void menuSubOpenYesNo();
extern void menuClose();
extern void menuCloseCustom();
extern s32 menuCloseSync(s32, s32);
extern void menuIsCheck();
extern s32 menuOpen(s32, s32);
extern void menuSetPosition();
extern u8*  fn_80104704(u32);
MenuCursorItem* windowGetCursorToItem(MenuWindow*);
extern u8*  windowGetKeyInfo(void);
extern void winMsgOpen();
extern void winMsgClose();
extern void fn_80108518();
extern void floorLink();
extern void* savedataGetStatus();
extern void heroDecPokecoupon();
extern void heroAddPokecoupon();
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
extern s8 fn_801347D8(void);
extern void* heroGetStatus(void*, s32, u16);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern u32 pokemonGetStatus(void*, u32, u32, u32);
extern u8 pokemonGetAnnonKatati(u32);
extern u32 _toolentryAlloc__FUl(u32);
extern u32 floorGetCurrentContext(void);
extern u32 GSthreadCreate(u32, u32, u32, u32, u32, void*);
extern void fn_80057E70(void);
extern void fn_8017B3E4(s32);
extern s32 fn_8017B2CC(s32);
extern void GSresReleaseById(s32);
extern void fn_800F915C(s32);
extern void fn_80055B98(s32);
extern void std078_flagSetFalse(s32);
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
extern void fn_80057C9C(void*, void*, s32*);
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
#if !defined(MENU_COLOSSEUM_BATTLE_MAIN_ONLY)
extern u8 lbl_80267840[];
extern char lbl_802678D8[];
#endif
extern const ColosseumUnownMenuIds lbl_802676F0[28];
extern const ColosseumRosterRow lbl_802677D0[7];
extern const s32 lbl_80267A80[6];
extern const s32 lbl_80267A98[6];
extern const s32 lbl_80267AB0[18];
extern u8 lbl_803A9A08[];
extern u8 lbl_803A9A18[];

/* ===== Function implementations ===== */

#if defined(MENU_COLOSSEUM_BATTLE_MAIN_ONLY)
/* menuColosseumBattle.c's .rodata at 0x80267840..0x80267A5A: MWCC pools these
 * tables and assert strings off one base register.
 * RULE-EXCEPTION(user-approved): named shared-pool stand-ins — see
 * docs/RULE_EXCEPTIONS.md. The command table and the strings at 0x802678D8 and
 * 0x80267A20 are global under their symbols.txt labels because linked sibling
 * units of the same original TU reference them; the other assert messages are
 * named statics so the pool keeps retail order around lbl_80267A20. */
/* menuCBCommands */
const s32 lbl_80267840[6] = {0xAD, -1, -1, 0xA8, 0xAF, -1};
static const s32 menuCBPartyCommands[6] = {0xAE, 0xAF, 0xAD, 0xB0, 0xB9, -1};
static const s32 menuCBBattleKinds[7] = {1, 1, 1, 1, 2, 2, 2};
static const ColosseumRosterRow menuCBRosterRows[4] = {
    {0, 1, 0, 0x01020000},
    {0, 1, 1, 0x02030000},
    {2, 2, 0, 0x01020304},
    {2, 2, 1, 0x01020304},
};
static const u8 menuCBPlayerOrders[3][4] = {{0, 1, 2, 3}, {0, 2, 1, 3}, {0, 3, 1, 2}};
const char lbl_802678D8[] = "menuColosseumBattle.c";
static const char menuCBMsgFloorStack[] = "0 == GSfloorGetStackPointer()";
static const char menuCBMsgStoryMode[] = "SYSTEM_GAMEMODE_STORY == GSflagGet(FLAG_system_gamemode)";
static const char menuCBMsgResumeTrainer[] = "menuCBBios_SaveDataAvailable(&_CB) && menuCBBios_GetRegisteredTrainer(&_CB)";
static const char menuCBMsgColosseumMode[] = "BATTLEMODE_COLOSSEUM == _CB.m_eBattleMode";
static const char menuCBMsgYamaColosseum[] = "COLOSSEUM_BATTLEYAMA == _CB.m_eColosseum";
static const char menuCBMsgYamaMode[] = "BATTLEMODE_BATTLEYAMA100 == _CB.m_eBattleMode";
const char lbl_80267A20[] = "MSG_ID_dummy != msg";
static const char menuCBMsgMultiType[] = "BATTLETYPE_MULTI != _CB.m_eBattleType";

/* RULE-EXCEPTION(user-approved): single-use inline helper boundaries chosen for
 * register allocation (menuCBOpenParty, menuCBInitTrainers, menuCBInitRecord,
 * menuCBCanResume, menuCBResumeColosseum, menuCBResumeYama, menuCBWindowWork,
 * menuCBEditRules) — see docs/RULE_EXCEPTIONS.md. */
static inline void menuCBOpenParty(s32 menu, u8* party, void* rules, void* resume)
{
    extern s32 menuIsCheck(s32);
    extern s32 fn_8006A7E8(void*);
    extern void* fn_8006A7C8(void*);
    extern s32 menuOpenCustom(s32, s32, void*, s32, s32, s32, ...);

    if ((u8)menuIsCheck(menu)) {
        menuClose(menu);
        while ((u8)menuIsCheck(menu)) {
            _threadSwitch();
        }
    }
    menuOpenCustom(menu, 0, NULL, 0x10, 0, 4,
                   fn_8006A7C8(party), fn_8006A7E8(party), rules, resume);
}

/* Both trainer-selection paths repeat this initialization sequence. */
static inline void menuCBInitTrainers(void)
{
    u32 i;
    for (i = 0; i < 4; i++) {
        memset(lbl_8047A5A0 + 0x1660 + i * 0xB18, 0, 0xB18);
        heroInit(lbl_8047A5A0 + 0x1660 + i * 0xB18);
    }
}
static inline void menuCBInitRecord(s32* buf, s32 a, s32 b, s32 c, s32 d, s32 e)
{
    memset(buf, 0, 0x50);
    buf[0] = a;
    buf[1] = b;
    buf[2] = c;
    buf[3] = d;
    buf[4] = e;
    buf[5] = 0;
    buf[6] = 5;
}
static inline s32 menuCBCanResume(void)
{
    extern s32 savedataBattleGetFlagC98B(void*);
    extern u8* savedataBattleGetSavedHero(void*);
    s32 ok = 0;
    if ((u8)savedataBattleGetFlagC98B(savedataGetStatus(0, 0xE)) != 0) {
        if (savedataBattleGetSavedHero(savedataGetStatus(0, 0xE)) != 0) {
            ok = 1;
        }
    }
    return ok;
}
static inline void menuCBResumeColosseum(u8* rec)
{

    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0) = *(s32*)(rec - 0x3674);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 4) = *(s32*)(rec - 0x3670);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 8) = *(s32*)(rec - 0x366C);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0xC) = *(s32*)(rec - 0x3668);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x10) = *(s32*)(rec - 0x3664);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x14) = *(s32*)(rec - 0x3660);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x18) = *(s32*)(rec - 0x365C);
    if (*(u32*)(rec - 0x3660) == 0) {
        savedataBattleSetCouponRun(0);
    }
    if (*(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0) != 0) {
        __assert(lbl_802678D8, 0xAB, menuCBMsgColosseumMode);
    }
    _flagSet(0x8AE, (*(s32*)(rec - 0x3670) == 0) ? 1 : 2);
    menuCBBattleSetupFight(((u8*)savedataGetStatus(0, 0xE)));
}
static inline s32 menuCBResumeYama(u8* rec)
{

    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0) = *(s32*)(rec - 0x3674);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 4) = *(s32*)(rec - 0x3670);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 8) = *(s32*)(rec - 0x366C);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0xC) = *(s32*)(rec - 0x3668);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x10) = *(s32*)(rec - 0x3664);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x14) = *(s32*)(rec - 0x3660);
    *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x18) = *(s32*)(rec - 0x365C);
    if (*(u32*)(rec - 0x3660) == 0) {
        savedataBattleSetCouponRun(0);
    }
    if (*(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0xC) != 6) {
        __assert(lbl_802678D8, 0x81, menuCBMsgYamaColosseum);
    }
    if (*(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0) != 1) {
        __assert(lbl_802678D8, 0x82, menuCBMsgYamaMode);
    }
    _flagSet(0x8AE, (*(s32*)(rec - 0x3670) == 0) ? 1 : 2);
    _flagSet(0xB59, *(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 0x14));
    _flagSet(0xAFC, 0);
    _flagSet(0xB11, 0);
    _flagSet(0xDE1, 0);
    heroMoveSyncWithHero();
    _flagSet(0xAFC, *(s32*)(rec - 0x3634));
    _flagSet(0xB11, *(s32*)(rec - 0x362C));
    _flagSet(0xDE1, *(s32*)(rec - 0x3628));
    return *(s32*)(rec - 0x3654);
}
static inline u8* menuCBWindowWork(void)
{
    extern s32 fn_8007162C(void);
    extern void* windowSearchID(s32);
    u8* npc = windowSearchID((u16)fn_8007162C());
    if (npc != NULL) {
        return *(u8**)(npc + 0x20);
    }
    return NULL;
}
static inline s32 menuCBEditRules(s32 cmd)
{
    extern s32 fn_8007162C(void);
    extern s32 _menuPop(void);
    extern s32 menuOpenCustom(s32, s32, void*, s32, s32, s32, ...);
    extern u8* savedataBattleGetRulePtr(void);
    extern u8* savedataBattleGetRuleSlotPtr(s32);
    extern s32 menuGetCursorItemID(s32);
    extern s32 fn_80088964(void);
    extern s32 fn_80077EA4(void*, void*);
    extern s32 memcardGetFlag41(void);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);

    s32 r = menuOpenCustom(fn_8007162C(), 0, NULL, 0x10, 1, 1, lbl_8047A5A0 + 0x42C0);
    if (r < 0) {
        cmd = -1;
        return cmd;
    }
    switch (r) {
    case 5:
        _menuPop();
        cmd = 0xC0;
        break;
    case 6:
        if (menuGetCursorItemID(fn_8007162C()) == 0x9FC) {
            fn_80077E80(lbl_8047A5A0 + 0x42C0, savedataBattleGetRuleSlotPtr(0));
            *(s16*)(lbl_8047A5A0 + 0x42C6) = 6;
            break;
        }
        goto c1_apply;
    case 7:
    c1_apply:
        if ((u8)fn_80077EA4(lbl_8047A5A0 + 0x42C0, savedataBattleGetRulePtr()) == 0) {
            fn_80077E80(&((ColosseumBattleRules*)((u8*)savedataGetStatus(0, 0xE)))->rules[*(s32*)(((u8*)savedataGetStatus(0, 0xE)) + 8) * 0x54], lbl_8047A5A0 + 0x42C0);
            for (;;) {
                if ((u8)memcardGetFlag41() == 0) {
                    break;
                }
                winMsgOpen(2, 0x44B1, 1, 0);
                if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                    break;
                }
                if (fn_80088964() >= 0) {
                    break;
                }
            }
            winMsgClose(1);
        }
        cmd = -1;
        break;
    default:
        break;
    }
    return cmd;

}

/* Address: 0x80059BDC | Size: 0x30F4 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragmas — see
 * docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
#pragma opt_loop_invariants off
#pragma opt_common_subs off
void menuColosseumBattleMain(void) {
    extern s32 fn_8007162C(void);
    extern s32 fn_80071344(void);
    extern s32 _menuPop_80071398();
    extern s32 _menuPop(void);
    extern void _menuPush(s32);
    extern s32 fn_80071160(void);
    extern s32 menuOpenCustom(s32, s32, void*, s32, s32, s32, ...);
    extern s32 menuIsCheck(s32);
    extern s32 GSinputIsConnected(s32);
    extern s32 fn_8008ABA0(s32);
    extern u8* savedataBattleGetSavedHero(void*);
    extern s32 fn_8006A7E8(void*);
    extern void* fn_8006A7C8(void*);
    extern s32 savedataBattleGetFlagC98B(void*);
    extern void fn_8006A81C(void*, u32);
    extern s32 fn_8006A76C(void*);
    extern u8* savedataBattleGetRulePtr(void);
    extern u8* savedataBattleGetRuleSlotPtr(s32);
    extern u8* savedataBattleGetEntryPtr(s32);
    extern u8* savedataBattleGetEntryById(s32);
    extern s32 savedataBattleGetCouponRun(void);
    extern u32 fn_800FF540(void);
    extern u32 std082_flagGetValue(s32);
    extern void* windowSearchID(s32);
    extern s32 windowGetActiveID(void);
    extern u8* windowGetKeyInfo(void);
    extern s32 fn_80076054(void*, void*);
    extern s32 GScharCmp(void*, void*);
    extern s32 fn_80089028(void);
    extern u32 fn_80093574(s32);
    extern s32 fn_800849B4(s32, s32, void*, void*);
    extern u32 heroBiosGetRnd(void*);
    extern u8* heroBiosGetNamePtr(void*);
    extern s32 menuGetCursorItemID(s32);
    extern u32 gamedatasaveGetStatus(s32, s32);
    extern s32 menuSaveLoadCommitA(void);
    extern s32 fn_80088964(void);
    extern s32 fn_80088D84(void);
    extern s32 fn_800889A4(void);
    extern s32 menuRuleCheckPartyAndRule(void*);
    extern s32 fn_80077EA4(void*, void*);
    extern s32 memcardGetFlag41(void);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void floorChangePos(u32, s32, f32, f32, f32);
    typedef struct SaveImage { u32 data[0x330B]; } SaveImage;
#define WORKP ((u8*)savedataGetStatus(0, 0xE))
    u8* playerWindowWork;
    s32 prevCmd;
    s32 first;
    u32 floorId;
    u8 posFlag;
    s32 warpArg;
    s32 cmd;
    f32 posX;
    f32 posY;
    f32 posZ;

    savedataGetStatus(0, 0xE);
    posFlag = 0;
    warpArg = 0;
    prevCmd = fn_8007162C();
    floorId = 0;
    if (fn_800FF540() != 0) {
        __assert(lbl_802678D8, 0x267, menuCBMsgFloorStack);
    }
    if (std082_flagGetValue(0x8AE) != 0) {
        __assert(lbl_802678D8, 0x268, menuCBMsgStoryMode);
    }
    while (fn_8007162C() > 0) {
        s32 mode;
        first = fn_8007162C();
        cmd = fn_8007162C();
        menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, first);
        switch (fn_8007162C()) {
        case 0xAA: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            cmd = lbl_80267840[v];
            switch (v) {
            case 1:
                floorId = 1;
                break;
            }
            break;
        }
        case 0xA8: {

            switch (fn_80071344()) {
            case 0:
                cmd = 0xAC;
                *(s32*)(WORKP + 4) = 0;
                *(s32*)(WORKP + 0x10) = 0;
                *(s32*)(WORKP + 0) = 0;
                fn_8006A7E0(WORKP + 0x59A8, 0);
                break;
            case 1:
                cmd = 0xB3;
                *(s32*)(WORKP + 4) = 2;
                *(s32*)(WORKP + 0x10) = 4;
                *(s32*)(WORKP + 0) = 2;
                break;
            case 2:
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0xAC: {
            s32 selection;
            menuSetRuleType(0);
            {
                s32 v = fn_80071344();
                if (v < 0) {
                    cmd = -1;
                    break;
                }
                selection = menuCBPartyCommands[v];
            }
            if (savedataBattleGetSavedHero(WORKP) == 0 && (selection == 0xAE || selection == 0xAF)) {
                winMsgOpen(2, 0x3BFE, 1, 0);
                winMsgClose(1);
                break;
            }
            switch (selection) {
            case 0xAE: {
                s32 t = 0;
                *(s32*)(WORKP + 0) = t;
                cmd = selection;
                break;
            }
            case 0xAF: {

                *(s32*)(WORKP + 0) = 1;
                *(s32*)(WORKP + 0xC) = 6;
                *(s32*)(WORKP + 8) = 2;
                cmd = selection;
                break;
            }
            case 0xAD:
                if ((u8)savedataBattleGetFlagC98B(WORKP) == 0 || savedataBattleGetSavedHero(WORKP) == 0) {
                    winMsgOpen(2, 0x4415, 1, 0);
                    winMsgClose(1);
                    break;
                }
                cmd = selection;
                break;
            default:
                cmd = selection;
                break;
            }
            break;
        }
        case 0xAD: {
            u8* st;
            s32 r;
            if (menuCBCanResume() == 0) {
                __assert(lbl_802678D8, 0x30F, menuCBMsgResumeTrainer);
            }
            menuCBOpenParty(0xC8, savedataBattleGetSavedHero(WORKP), 0, WORKP + 0xC98C);
            if ((u8)menuIsCheck(0xD6)) {
                menuClose(0xD6);
                while ((u8)menuIsCheck(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeAd = 1;
                r = menuOpenCustom(0xD6, 0, &modeAd, 0x10, 1, 4, 0x3DB0, 0x3DB1, 0x3D89, 0);
            }
            menuClose(0xD6);
            if (r != 0) {
                menuCloseCustom(0xC8, 0, 0);
                menuWaitFaceSprites();
                cmd = -1;
                break;
            }
            fn_8006A7F0(savedataBattleGetEntryById(1), savedataBattleGetSavedHero(WORKP));
            if ((u8)fn_8006A76C(WORKP) != 0) {
                savedataBattleClearFlagC98B(WORKP);
            } else {
                s32 wasOpen = savedataBattleGetFlagC98B(WORKP);
                savedataBattleClearFlagC98B(WORKP);
                winMsgOpen(2, 0x44D9, 1, 0);
                winMsgClose(1);
                if (menuSaveLoadCommitA() < 0) {
                    if ((u8)wasOpen != 0) {
                        savedataBattleSetFlagC98B(WORKP);
                    }
                    menuCloseCustom(0xC8, 0, 0);
                    menuWaitFaceSprites();
                    cmd = -1;
                    break;
                }
            }
            if (*(s32*)(WORKP + 0xC98C) != 1) {
                menuCBResumeColosseum(WORKP + 0x10000);
                cmd = 0xD1;
            } else {
                floorId = menuCBResumeYama(WORKP + 0x10000);
                posX = *(f32*)(WORKP + 0xC9B4);
                posY = *(f32*)(WORKP + 0xC9B8);
                posZ = *(f32*)(WORKP + 0xC9BC);
                warpArg = (s32)(lbl_8047BF18 * *(f32*)(WORKP + 0xC9C4));
                posFlag = 1;
                cmd = 0x105;
            }
            menuCloseCustom(0xC8, 0, 0);
            menuWaitFaceSprites();
            break;
        }
        case 0xAE: {

            s32 v;
            *(s32*)(WORKP + 8) = 2;
            *(s32*)(WORKP + 0) = 0;
            v = fn_80071344();
            switch (v) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                *(s32*)(WORKP + 0xC) = v;
                menuSetRuleType(menuCBBattleKinds[v]);
                cmd = 0xAF;
                break;
            case 6:
                cmd = 0xC0;
                break;
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0xAF: {
            u8 playerMode;
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            if (v == 2) {
                cmd = -1;
                break;
            }
            *(s32*)(WORKP + 4) = v;
            if (*(s32*)(WORKP + 0) == 3) {

                s32 r;
                playerMode = (u8)(v == 1);
                playerWindowWork = menuCBWindowWork();
                winSetSequence(playerWindowWork + 0xC, 0x1CE);
                r = menuOpenCustom(0xBC, 0, NULL, 0x10, 1, 1, playerMode);
                switch (r) {
                case 0:
                case 1:
                case 2:
                case 3: {

                    *(s32*)(WORKP + 0x10) = r;
                    *(s32*)(WORKP + 8) = 0;
                    *(s32*)(WORKP + 0xC) = 0;
                    cmd = 0xCC;
                    menuCloseCustom(0xBC, 0, 0);
                    break;
                }
                default:
                    menuCloseCustom(0xBC, 0, 1);
                    winSetSequence(playerWindowWork + 0xC, 0x1CA);
                    break;
                }
                break;
            }
            if (*(s32*)(WORKP + 0x10) == 4) {
                cmd = 0xBF;
            } else {
                cmd = 0xB1;
            }
            break;
        }
        case 0xEB:
            __assert(lbl_802678D8, 0x3C4, (char*)&lbl_8047BF1C);
            break;
        case 0xB0:
            switch (fn_80071344()) {
            case 0:
                if ((u8)memcardGetFlag41() == 0) {
                    winMsgOpen(2, 0x44EB, 1, 0);
                    winMsgClose(1);
                    break;
                }
                if (gamedatasaveGetStatus(0, 4) == 0) {
                    winMsgOpen(2, 0x444D, 1, 0);
                    winMsgClose(1);
                    break;
                }
                heroBiosCopy(lbl_8047A5A0 + 0x1660, savedataGetStatus(0, 2));
                cmd = 0xEE;
                break;
            case 1:
                cmd = 0xED;
                break;
            default:
                cmd = -1;
                break;
            }
            break;
        case 0xEE: {
            s32 trainer;
            trainer = (s32)(lbl_8047A5A0 + 0x1660);
            menuSetRuleType(2);
            if ((u8)menuRuleCheckPartyAndRule((u8*)trainer) == 0) {
                s32 m;
                menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xF5);
                menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, trainer, 0, savedataBattleGetRulePtr(), 0);
                menuSetPosition(0xDA, 0, -0x28);
                m = fn_80076054((u8*)trainer, savedataBattleGetRulePtr());
                if ((u16)m == 0) {
                    __assert(lbl_802678D8, 0x1BB, lbl_80267A20);
                }
                soundStartSE(0x26);
                winMsgOpen(7, (u16)m, 1, 0);
                winMsgOpen(7, 0x440A, 1, 0);
                menuCloseCustom(0xDA, 0, 0);
                menuCloseCustom(0xBE, 0, 1);
                menuCloseCustom(0xDA, 0, 1);
                menuWaitFaceSprites();
                menuSetRuleType(0);
                winMsgClose(1);
                _menuPop();
                cmd = -1;
                break;
            }
            menuSetRuleType(0);
            heroBiosSetHomePlace((u8*)trainer, 0);
            menuCBBios_InitTrainer(lbl_8047A5A0, 0);
            menuCBBiosInitHero(lbl_8047A5A0, (u8*)trainer);
            _menuPop();
            cmd = 0xB2;
            break;
        }
        case 0xED: {
            s32 r;
            u8* arr[4];
            menuCBInitTrainers();
            arr[0] = NULL;
            arr[1] = lbl_8047A5A0 + 0x1660;
            arr[2] = NULL;
            arr[3] = NULL;
            menuSetRuleType(2);
            r = fn_800849B4(0, 2, arr, NULL);
            menuSetRuleType(0);
            if (r < 0) {
                _menuPop();
                cmd = -1;
                break;
            }
            menuCBBios_InitTrainer(lbl_8047A5A0, 0);
            menuCBBiosInitHero(lbl_8047A5A0, lbl_8047A5A0 + 0x1660);
            _menuPop();
            cmd = 0xB2;
            break;
        }
        case 0xB2: {
            s32 answer;
            s32 r;
            menuCBOpenParty(0xDA, lbl_8047A5A0, 0, 0);
            if ((u8)menuIsCheck(0xD6)) {
                menuClose(0xD6);
                while ((u8)menuIsCheck(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeB2 = 0;
                r = menuOpenCustom(0xD6, 0, &modeB2, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            menuClose(0xD6);
            if (r == 0) {
                if (savedataBattleGetSavedHero(WORKP) != 0) {
                    u8* p;
                    menuCloseCustom(0xDA, 0, 0);
                    menuWaitFaceSprites();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xEB);
                    menuCBOpenParty(0xDA, savedataBattleGetSavedHero(WORKP), 0, 0);
                    if ((u8)menuIsCheck(0xD6)) {
                        menuClose(0xD6);
                        while ((u8)menuIsCheck(0xD6)) {
                            _threadSwitch();
                        }
                    }
                    {
                        u32 modeB2b = 1;
                        r = menuOpenCustom(0xD6, 0, &modeB2b, 0x10, 1, 4, 0x3D47, 0x3D49, 0x3C54, -0x28);
                    }
                    menuClose(0xD6);
                    if (r != 0) {
                        goto b2_cancel;
                    }
                    if ((u8)savedataBattleGetFlagC98B(WORKP) != 0) {
                        winMsgOpen(2, 0x44C2, 1, 0);
                        answer = menuSubOpenYesNo(0, 0x3C, 0x9E, 1);
                        winMsgClose(1);
                        if (answer != 0) {
                            goto b2_cancel;
                        }
                    }
                    menuCloseCustom(0xDA, 0, 0);
                    menuWaitFaceSprites();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, first);
                    {
                        menuCBOpenParty(0xDA, lbl_8047A5A0, 0, 0);
                    }
                }
                memcpy(lbl_8047A5A0 + 0x4318, savedataGetStatus(0, 0xE), 0xCC2C);
                savedataBattleSetSavedHero(savedataGetStatus(0, 0xE), lbl_8047A5A0);
                if ((u8)memcardGetFlag41() == 0) {
                    soundStartSE(0x26);
                    winMsgOpen(2, 0x3C60, 1, 0);
                    if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                } else if (fn_800889A4() < 0) {
                    memcpy(savedataGetStatus(0, 0xE), lbl_8047A5A0 + 0x4318, 0xCC2C);
                    if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                } else if (fn_8006A7E8(lbl_8047A5A0) == 0) {
                    winMsgOpen(2, 0x3C5E, 1, 0);
                } else {
                    winMsgOpen(2, 0x3D44, 1, 0);
                }
                menuCloseCustom(0xDA, 0, 0);
                menuWaitFaceSprites();
                _menuPop();
                winMsgClose(1);
                cmd = -1;
                break;
            }
        b2_cancel:
            if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                winMsgOpen(2, 0x3D55, 1, 0);
                menuCloseCustom(0xDA, 0, 0);
                menuWaitFaceSprites();
                winMsgClose(1);
            }
            cmd = -1;
            break;
        }
        case 0xCC: {
            u8* p;
            s32 r;
            menuCBBattleSetupFight(savedataGetStatus(0, 0xE));
            menuCBOpenParty(0xDA, savedataBattleGetEntryPtr(0), 0, 0);
            if ((u8)menuIsCheck(0xD6)) {
                menuClose(0xD6);
                while ((u8)menuIsCheck(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeCc = 0;
                r = menuOpenCustom(0xD6, 0, &modeCc, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            menuClose(0xD6);
            menuCloseCustom(0xDA, 0, 0);
            menuWaitFaceSprites();
            if (r != 0) {
                cmd = -1;
                break;
            }
            cmd = 0xD1;
            break;
        }
        case 0xB1: {
            s32 previousState;
            s32 r;
            s32 m;
            u8* wm;
            s32 party;
            party = (s32)savedataBattleGetSavedHero(WORKP);
            cmd = party;
            if (*(s32*)(WORKP + 4) == 2) {
                __assert(lbl_802678D8, 0x4B9, menuCBMsgMultiType);
            }
            if ((u32)cmd == 0) {
                __assert(lbl_802678D8, 0x4BA, (char*)&lbl_8047BF20);
            }
            wm = savedataBattleGetRulePtr();
            menuCBOpenParty(0xDA, (u8*)(void*)cmd, wm, 0);
            m = fn_80076054((u8*)cmd + 0xB44, savedataBattleGetRulePtr());
            if ((u16)m != 0) {
                winMsgOpen(1, (u16)m, 1, 0);
                winMsgClose(1);
                menuCloseCustom(0xDA, 0, 0);
                menuWaitFaceSprites();
                _menuPop();
                cmd = -1;
                break;
            }
            if ((u8)menuIsCheck(0xD6)) {
                menuClose(0xD6);
                while ((u8)menuIsCheck(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeB1 = 0;
                r = menuOpenCustom(0xD6, 0, &modeB1, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            menuClose(0xD6);
            if (r != 0) {
                menuCloseCustom(0xDA, 0, 0);
                menuWaitFaceSprites();
                cmd = -1;
                break;
            }
            {
                u8* dst = lbl_8047A5A0 + 0x4318;
                SaveImage* dstSaveImage = (SaveImage*)dst;
                SaveImage* srcSaveImage = (SaveImage*)savedataGetStatus(0, 0xE);

                *dstSaveImage = *srcSaveImage;
                previousState = (s32)savedataBattleGetSavedHero(dst);
                if ((u8)savedataBattleGetFlagC98B(dst) != 0 && (u32)previousState != 0) {
                    menuCloseCustom(0xDA, 0, 0);
                    menuWaitFaceSprites();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xD7);
                    menuCBOpenParty(0xC8, (u8*)(void*)previousState, 0, dst + 0xC98C);
                    if ((u8)menuIsCheck(0xD6)) {
                        menuClose(0xD6);
                        while ((u8)menuIsCheck(0xD6)) {
                            _threadSwitch();
                        }
                    }
                    {
                        u32 modeB1b = 1;
                        r = menuOpenCustom(0xD6, 0, &modeB1b, 0x10, 1, 4, 0x3D47, 0x3D49, 0x44C8, 0);
                    }
                    menuClose(0xD6);
                    menuCloseCustom(0xC8, 0, 0);
                    menuWaitFaceSprites();
                    menuCloseCustom(0xBE, 0, 1);
                    if (r != 0) {
                        cmd = -1;
                        break;
                    }
                    if ((u8)fn_8006A76C(WORKP) == 0) {
                        u8* wm2;
                        menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, first);
                        wm2 = savedataBattleGetRulePtr();
                        menuCBOpenParty(0xDA, (u8*)(void*)cmd, wm2, 0);
                    }
                }
            }
            if ((u8)fn_8006A76C(WORKP) != 0) {
                savedataBattleClearFlagC98B(WORKP);
            } else {
                s32 wasOpen = savedataBattleGetFlagC98B(WORKP);
                savedataBattleClearFlagC98B(WORKP);
                if (menuSaveLoadCommitA() < 0) {
                    if ((u8)wasOpen != 0) {
                        savedataBattleSetFlagC98B(WORKP);
                    }
                    menuCloseCustom(0xDA, 0, 0);
                    menuWaitFaceSprites();
                    cmd = -1;
                    break;
                }
            }
            {
                s32 buf[20];

                menuCBInitRecord(buf, *(s32*)(WORKP + 0), *(s32*)(WORKP + 4), *(s32*)(WORKP + 8),
                                 *(s32*)(WORKP + 0xC), *(s32*)(WORKP + 0x10));
                if (*(s32*)(WORKP + 0) == 1) {
                    *(s32*)(WORKP + 0) = buf[0];
                    *(s32*)(WORKP + 4) = buf[1];
                    *(s32*)(WORKP + 8) = buf[2];
                    *(s32*)(WORKP + 0xC) = buf[3];
                    *(s32*)(WORKP + 0x10) = buf[4];
                    *(s32*)(WORKP + 0x14) = buf[5];
                    *(s32*)(WORKP + 0x18) = buf[6];
                    if ((u32)buf[5] == 0) {
                        savedataBattleSetCouponRun(0);
                    }
                    if (*(s32*)(WORKP + 0xC) != 6) {
                        __assert(lbl_802678D8, 0x81, menuCBMsgYamaColosseum);
                    }
                    if (*(s32*)(WORKP + 0) != 1) {
                        __assert(lbl_802678D8, 0x82, menuCBMsgYamaMode);
                    }
                    _flagSet(0x8AE, (buf[1] == 0) ? 1 : 2);
                    _flagSet(0xB59, *(s32*)(WORKP + 0x14));
                    _flagSet(0xAFC, 0);
                    _flagSet(0xB11, 0);
                    _flagSet(0xDE1, 0);
                    heroMoveSyncWithHero();
                    floorId = 0x4C;
                    cmd = 0x105;
                } else {
                    *(s32*)(WORKP + 0) = buf[0];
                    *(s32*)(WORKP + 4) = buf[1];
                    *(s32*)(WORKP + 8) = buf[2];
                    *(s32*)(WORKP + 0xC) = buf[3];
                    *(s32*)(WORKP + 0x10) = buf[4];
                    *(s32*)(WORKP + 0x14) = buf[5];
                    *(s32*)(WORKP + 0x18) = buf[6];
                    if ((u32)buf[5] == 0) {
                        savedataBattleSetCouponRun(0);
                    }
                    if (*(s32*)(WORKP + 0) != 0) {
                        __assert(lbl_802678D8, 0xAB, menuCBMsgColosseumMode);
                    }
                    _flagSet(0x8AE, (buf[1] == 0) ? 1 : 2);
                    menuCBBattleSetupFight(WORKP);
                    cmd = 0xD1;
                }
            }
            menuCloseCustom(0xDA, 0, 0);
            menuWaitFaceSprites();
            break;
        }
        case 0xB3: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            if (v < 4) {
                s8* e;
                s32* st;

                e = (s8*)&menuCBRosterRows[v];
                st = (s32*)(e + 8);
                if (*st == 0) {
                    if ((u8)memcardGetFlag41() == 0) {
                        winMsgOpen(2, 0x44EA, 1, 0);
                        winMsgClose(1);
                        break;
                    }
                    if (gamedatasaveGetStatus(0, 4) == 0) {
                        winMsgOpen(2, 0x44DB, 1, 0);
                        winMsgClose(1);
                        break;
                    }
                }
                *(s32*)(WORKP + 4) = *(s32*)e;
                *(s32*)(WORKP + 0xC) = *(s32*)(e + 4);
                fn_8006A7E0(WORKP + 0x59A8, *st);
                fn_8006A81C(WORKP + 0x59A8, e[0xC]);
                {
                    u32 offset;
                    s8* p;
                    u32 i;
                    p = e + 1;
                    i = 1;
                    offset = 0x1660;
                    for (; i < 4; i++) {
                        fn_8006A7E0((WORKP + offset + 0x59A8), 1);
                        fn_8006A81C((WORKP + offset + 0x59A8), p[0xC]);
                        offset += 0x1660;
                        p += 1;
                    }
                }
                if (*(s32*)(WORKP + 4) == 2) {
                    cmd = 0xBF;
                } else {
                    cmd = 0xAF;
                }
                break;
            }
            cmd = -1;
            break;
        }
        case 0xC0: {
            s32 r;
            mode = 0;
            if (prevCmd == 0xC1) {
                mode = 7;
            } else {
                fn_80077E80(lbl_8047A5A0 + 0x42C0, savedataBattleGetRulePtr());
            }
            for (;;) {
                r = menuOpenCustom(fn_8007162C(), 0, &mode, 0x10, 1, 1, lbl_8047A5A0 + 0x42C0);
                if (r < 0) {
                    cmd = -1;
                    goto c0_end;
                }
                switch (r) {
                case 3:
                    if (*(s32*)(lbl_8047A5A0 + 0x42C8) == 2) {
                        u8 tmp[0x3C];
                        s32 r2;
                        memcpy(tmp, lbl_8047A5A0 + 0x42D8, 0x3C);
                        r2 = menuOpenCustom(0xB4, 0, NULL, 0x10, 1, 1, tmp);
                        menuClose(0xB4);
                        if (r2 >= 0) {
                            memcpy(lbl_8047A5A0 + 0x42D8, tmp, 0x3C);
                        }
                        mode = 3;
                        continue;
                    }
                    goto c0_end;
                case 8:
                    _menuPop();
                    cmd = 0xC1;
                    goto c0_end;
                default:
                    goto c0_end;
                }
            }
        c0_end:
            break;
        }
        case 0xC1: {
            cmd = menuCBEditRules(cmd);
            break;
        }
        case 0xB4:
            __assert(lbl_802678D8, 0x5F5, (char*)&lbl_8047BF1C);
            break;
        case 0xBF: {
            s32 v = fn_80071344();
            switch (v) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                *(s32*)(WORKP + 8) = v;
                if (*(u16*)(windowGetKeyInfo() + 4) & 0x400) {
                    cmd = 0xC0;
                } else {
                    cmd = 0xC2;
                }
                break;
            default:
                if (*(s32*)(WORKP + 4) == 2) {
                    cmd = _menuPop_80071398(0xB3);
                } else {
                    cmd = _menuPop_80071398(0xAF);
                }
                break;
            }
            break;
        }
        case 0xC2:
            switch (fn_80071344()) {
            case 0:
                _menuPop();
                if (*(s32*)(WORKP + 0x10) != 4) {
                    cmd = 0xB1;
                    break;
                }
                if ((u8)fn_80089028() != 0) {
                    cmd = 0xE4;
                    break;
                }
                cmd = 0xB6;
                break;
            case 1:
            default:
                cmd = _menuPop_80071398(0xB3);
                break;
            }
            break;
        case 0xE4: {
            s32 mode2;
            u8* arr[4];
            s32 r;
            menuCBInitTrainers();
            switch (*(s32*)(WORKP + 4)) {
            case 0:
            case 1:
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    mode2 = 0;
                    arr[0] = lbl_8047A5A0 + 0x1660;
                    arr[1] = lbl_8047A5A0 + 0x2178;
                    arr[2] = NULL;
                    arr[3] = NULL;
                } else {
                    arr[0] = NULL;
                    arr[1] = lbl_8047A5A0 + 0x1660;
                    arr[2] = lbl_8047A5A0 + 0x2178;
                    arr[3] = NULL;
                    mode2 = 1;
                }
                break;
            case 2:
            default:
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    mode2 = 2;
                } else {
                    mode2 = 3;
                }
                arr[0] = lbl_8047A5A0 + 0x1660;
                arr[1] = lbl_8047A5A0 + 0x2178;
                arr[2] = lbl_8047A5A0 + 0x2C90;
                arr[3] = lbl_8047A5A0 + 0x37A8;
                break;
            }
            r = fn_800849B4(mode2, 0x1A, arr, NULL);
            if (r < 0) {
                cmd = _menuPop_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 0x59AC) == 0) {
                heroBiosSetHomePlace(lbl_8047A5A0 + 0x1660, 0);
            }
            switch (*(s32*)(WORKP + 4)) {
            case 0:
            case 1:
                menuCBBiosInitHero(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                menuCBBiosInitHero(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    fn_8006A81C(WORKP + 0x59A8, 1);
                    fn_8006A81C(WORKP + 0x7008, 2);
                } else {
                    fn_8006A81C(WORKP + 0x59A8, 2);
                    fn_8006A81C(WORKP + 0x7008, 3);
                }
                fn_8006A81C(WORKP + 0x8668, 0);
                fn_8006A81C(WORKP + 0x9CC8, 0);
                break;
            case 2:
            default:
                menuCBBiosInitHero(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                menuCBBiosInitHero(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
                menuCBBiosInitHero(WORKP + 0x8668, lbl_8047A5A0 + 0x2C90);
                menuCBBiosInitHero(WORKP + 0x9CC8, lbl_8047A5A0 + 0x37A8);
                fn_8006A81C(WORKP + 0x59A8, 1);
                fn_8006A81C(WORKP + 0x7008, 2);
                fn_8006A81C(WORKP + 0x8668, 3);
                fn_8006A81C(WORKP + 0x9CC8, 4);
                break;
            }
            cmd = 0xB6;
            break;
        }
        case 0xB6: {
            s32 v;
            s32 r;
            {
                u32 i;
                for (i = 0; i < 4; i++) {
                    *(u8*)(WORKP + i * 0x1660 + 0x7005) = 0;
                }
            }
            v = fn_80071344();
            if (v < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        winMsgOpen(2, 0x4445, 1, 0);
                        while ((u8)GSinputIsConnected(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)GSinputIsConnected(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = _menuPop_80071398(0xB3);
                break;
            }
            r = menuOpen(0xD0, 1);
            menuClose(0xD0);
            if (r < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        winMsgOpen(2, 0x4445, 1, 0);
                        while ((u8)GSinputIsConnected(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)GSinputIsConnected(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = _menuPop_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 4) != 2) {
                menuCBBattleSetupFight(WORKP);
                cmd = 0xD1;
                break;
            }
            cmd = 0xB5;
            break;
        }
        case 0xB5: {
            s32 v = fn_80071344();
            cmd = 0xD1;
            switch (v) {
            case 0:
            case 1:
            case 2: {
                u8* e = (u8*)menuCBPlayerOrders;

                e += v * 4;
                *(s32*)(WORKP + 0x59D0) = (s8)e[0];
                *(s32*)(WORKP + 0x7030) = (s8)e[1];
                *(s32*)(WORKP + 0x8690) = (s8)e[2];
                *(s32*)(WORKP + 0x9CF0) = (s8)e[3];
                menuCBBattleSetupFight(WORKP);
                goto b5_done;
            }
            case -1:
            case 3:
            default: {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        winMsgOpen(2, 0x4445, 1, 0);
                        while ((u8)GSinputIsConnected(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)GSinputIsConnected(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = _menuPop_80071398(0xB3);
                break;
            }
            }
        b5_done:
            break;
        }
        case 0xD1:
            menuColosseumBattleSetFlag();
            floorId = 0x397;
            break;
        case 0xB8:
            savedataGetStatus(0, 0xE);
            menuTaisenRun();
            cmd = _menuPop_80071398();
            break;
        case 0xB9: {
            s32 r = menuOpen(0xB9, 1);
            menuCloseCustom(0xB9, 0, 0);
            menuCloseCustom(0xBE, 0, 1);
            switch (r) {
            case 0:
                if ((u8)memcardGetFlag41() == 0) {
                    winMsgOpen(2, 0x44EA, 1, 0);
                    winMsgClose(1);
                    break;
                }
                if (gamedatasaveGetStatus(0, 4) == 0) {
                    winMsgOpen(2, 0x44DB, 1, 0);
                    winMsgClose(1);
                    break;
                }
                fn_8002D91C(0xB);
                break;
            case 1:
                if ((u8)fn_8008ABA0(1) != 0) {
                    winMsgOpen(2, 0x4445, 1, 0);
                    while ((u8)GSinputIsConnected(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                fn_8002D91C(0xC);
                break;
            case -1:
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0x105: {
            s32 couponAmount;
            u32 coins;
            s32 st;
            if ((u8)savedataBattleGetFlagC98B(WORKP) == 0) {
                coins = savedataBattleGetCouponRun();
                cmd = (s32)savedataBattleGetSavedHero(WORKP);
                if ((u32)cmd == 0) {
                    __assert(lbl_802678D8, 0x72C, (char*)&lbl_8047BF24);
                }
                if (coins == 0) {
                    goto quit105;
                }
                savedataBattleSetCouponRun(0);
                if (fn_8006A7E8((void*)cmd) == 0) {
                    if ((u8)fn_8006A76C(WORKP) != 0) {
                        goto quit105;
                    }
                    couponAmount = coins;
                    heroAddPokecoupon(0, coins);
                    for (;;) {
                        winMsgOpen(2, 0x3C03, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) == 0) {
                            if (fn_80088D84() >= 0) {
                                goto quit105;
                            }
                            continue;
                        }
                        winMsgOpen(2, 0x3D54, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 1) != 0) {
                            continue;
                        }
                        heroDecPokecoupon(0, couponAmount);
                        goto quit105;
                    }
                }
                for (;;) {
                    winMsgOpen(7, 0x3C23, 1, 0);
                    if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                        goto ask105;
                    }
                    {
                        u8* arr2[4];
                        u32 money[0x36];
                        s32 r;
                        u32 h;
                        u32 savedH;
                        arr2[0] = NULL;
                        arr2[1] = lbl_8047A5A0 + 0x1660;
                        arr2[2] = NULL;
                        arr2[3] = NULL;
                        r = fn_800849B4(0, 0x40, arr2, money);
                        if (r < 0) {
                            continue;
                        }
                        h = heroBiosGetRnd(((MenuMiddleTrainerSlot*)cmd)->heroCopy);
                        savedH = heroBiosGetRnd(lbl_8047A5A0 + 0x1660);
                        if (savedH != h) {
                            goto mismatch105;
                        }
                        if (GScharCmp(heroBiosGetNamePtr(lbl_8047A5A0 + 0x1660), heroBiosGetNamePtr((void*)(cmd + 0xB44))) != 0) {
                            goto mismatch105;
                        }
                        winMsgOpen(7, 0x3D51, 0, 1);
                        money[0] += coins;
                        money[1] += coins;
                        if (money[0] > 9999999) {
                            money[0] = 9999999;
                        }
                        if (money[1] > 9999999) {
                            money[1] = 9999999;
                        }
                        fn_80093574(1);
                        gbaCommStartSendPokemon(1, money, 0);
                        if ((s32)fn_80093574(1) == 0xC) {
                            winMsgOpen(7, 0x3D52, 1, 0);
                            break;
                        }
                        winMsgOpen(7, 0x3D53, 1, 0);
                        continue;
                    }
                mismatch105:
                    winMsgOpen(7, 0x44DA, 1, 0);
                    continue;
                ask105:
                    winMsgOpen(7, 0x3D54, 1, 0);
                    if (menuSubOpenYesNo(0, 0x3C, 0x9E, 1) != 0) {
                        continue;
                    }
                    break;
                }
                if ((u8)fn_8006A76C(savedataGetStatus(0, 0xE)) == 0) {
                    do {
                        winMsgOpen(2, 0x44EC, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                            break;
                        }
                    } while (menuSaveLoadCommitA() < 0);
                }
            quit105:
                winMsgClose(1);
            }
            st = *(s32*)(WORKP + 0);
            switch (st) {
            case 0:
                cmd = _menuPop_80071398(0xAE);
                break;
            case 1:
            default:
                cmd = _menuPop_80071398(0xAC);
                break;
            }
            break;
        }
        default:
            break;
        }
        if ((u8)GSinputIsConnected(1) == 0 && (u8)fn_8008ABA0(1) == 0) {
            winMsgOpen(2, 0x3C4F, 1, 0);
            while ((u8)GSinputIsConnected(1) == 0) {
                _threadSwitch();
            }
            winMsgClose(1);
        }
        prevCmd = first;
        if (cmd < 0) {
            s32 r = _menuPop();
            menuCloseCustom(0xBE, 0, 1);
            if (r < 0) {
                goto done;
            }
        } else if (cmd != fn_8007162C()) {
            s32 r = fn_8007162C();
            s32 active = windowGetActiveID();
            if (active == r) {
                menuCloseCustom(fn_8007162C(), 0, 0);
            }
            menuCloseCustom(0xBE, 0, 1);
            _menuPush(cmd);
        }
        if (floorId != 0) {
            s32 r = fn_8007162C();
            s32 active = windowGetActiveID();
            if (active != r) {
                goto done;
            }
            menuCloseCustom(fn_8007162C(), 0, 0);
            menuCloseCustom(0xBE, 0, 1);
            goto done;
        }
    }
done:
    if (posFlag != 0) {
        menuColosseumBattleSetFlag();
        floorChangePos(floorId, warpArg, posX, posY, posZ);
    } else {
        if (floorId == 0) {
            floorId = 0x3A1;
        }
        if (floorId == 0x3A1) {
            menuColosseumBattleClearFlag();
            if ((u8)GSinputIsConnected(1) == 0) {
                winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                while ((u8)GSinputIsConnected(1) == 0) {
                    _threadSwitch();
                }
                winMsgClose(1);
            }
        } else {
            menuColosseumBattleSetFlag();
        }
        floorLink(floorId, 0);
    }
#undef WORKP
}
#pragma pop
#endif

#if defined(MENU_COLOSSEUM_BATTLE_EXIT_ONLY)
/* Address: 0x8005CCD0 | Size: 0xB8 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (opt_propagation off) - keeps the cleared exit byte live across savedataGetStatus - see docs/RULE_EXCEPTIONS.md */
#pragma opt_propagation off
void menuColosseumBattleExit(void) {
    u32 handle;
    u8 pending;

    fadeCheck(1);
    if (menuColosseumBattleGetFlag() == 0) {
        ColosseumSaveWork* work;

        pending = 0;
        work = (ColosseumSaveWork*)savedataGetStatus(0, 0xE);
        work->exitPending = pending;
        _flagSet(0x8AE, 0);
    }

    menuClose(0xD3);
    toolentryTaisenFreePokemonData();
    std113_mapObjectUnk_8018db68(((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle);
    handle = GSmemGetHandleByPtr(lbl_8047A5A0);
    if ((u16)handle == 0) {
        __assert(lbl_802678D8, 0x252, &lbl_8047BF28);
    }
    fn_800E24B0(handle);
    GSmemFree(handle);
    lbl_8047A5A0 = NULL;
}
#pragma pop
#endif

#if defined(MENU_COLOSSEUM_BATTLE_INIT_ONLY)
/* RULE-EXCEPTION(title-path): peephole control and the local string-table
 * base preserve retail allocation. See docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
void menuColosseumBattleInit(void)
{
    extern void std104_soundSetBackgroundMusic(s32, s32, s32);
    extern u32 GSmemAlloc(u32 size, u32 alignment);
    extern void* std093_getCurrentGroupID(void);
    extern void* std112_modelGetResourceID(u32 size);
    extern void GSresGetResource(void* archive, void* destination);
    extern void cameraPlayAnime(s32 id, u32 data, s32 start, s32 loop);
    extern void GSscene_SetMode(s32 mode);
    extern u8 fn_800FF548(void);
    extern void toolentryDebugPokemonCreate(void);
    extern void menuColosseumBattleSetup(void* status);
    u8* strings;
    u32 heap;
    void* archive;

    strings = lbl_80267840;
    std104_soundSetBackgroundMusic(0x1E, 0, 0xFF);
    _flagSet(0x8AE, 0);

    if (lbl_8047A5A0 != NULL) {
        __assert(strings + 0x98, 0x20F, strings + 0x21C);
    }

    heap = GSmemAlloc(0x10F60, 0x20);
    if ((u16)heap == 0) {
        __assert(strings + 0x98, 0x212, &lbl_8047BF28);
    }
    lbl_8047A5A0 = GSmemLock(heap);
    if (lbl_8047A5A0 == NULL) {
        __assert(strings + 0x98, 0x213, strings + 0x22C);
    }

    archive = std093_getCurrentGroupID();
    ((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle =
        std112_modelGetResourceID(0x0FFE1000);
    GSresGetResource(
        archive, ((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle);
    cameraPlayAnime(0x531, 0x0FFF1800, 0, 1);
    GSscene_SetMode(4);
    if (fn_800FF548() == 0) {
        toolentryDebugPokemonCreate();
        menuColosseumBattleSetup(savedataGetStatus(0, 0xE));
    }
    if (menuColosseumBattleGetFlag() == 0) {
        ((ColosseumSaveWork*)savedataGetStatus(0, 0xE))->exitPending = 0;
    }
    menuOpen(0xD3, 0);
}
#pragma pop
#endif
