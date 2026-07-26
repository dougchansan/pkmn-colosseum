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

/* Address: 0x80059BDC | Size: 0x30F4 */
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void menuColosseumBattleMain(void) {
    extern s32 fn_8007162C(void);
    extern s32 fn_80071344(void);
    extern s32 fn_80071398();
    extern s32 fn_800714C8(void);
    extern s32 fn_80071160(void);
    extern s32 menuOpenCustom(s32, s32, void*, s32, s32, s32, ...);
    extern s32 menuIsCheck(s32);
    extern s32 fn_800F7EF8(s32);
    extern s32 fn_8008ABA0(s32);
    extern u8* fn_8006AFC4(void*);
    extern s32 fn_8006A7E8(void*);
    extern void* fn_8006A7C8(void*);
    extern s32 fn_8006A7BC(void*);
    extern s32 fn_8006A76C(void*);
    extern u8* fn_8006B420(void);
    extern u8* fn_8006B51C(s32);
    extern u8* fn_8006B09C(s32);
    extern u8* fn_8006AFE4(s32);
    extern s32 fn_8006ADEC(void);
    extern u32 fn_800FF540(void);
    extern u32 fn_801906A0(s32);
    extern u8* fn_80104704(u32);
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
    extern s32 fn_80088C60(void);
    extern s32 fn_80088964(void);
    extern s32 fn_80088D84(void);
    extern s32 fn_800889A4(void);
    extern s32 fn_800776E4(void*);
    extern s32 fn_80077EA4(void*, void*);
    extern s32 fn_801D04E8(void);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void floorChangePos(u32, s32, f32, f32, f32);
    typedef struct SaveImage { u32 data[0x330B]; } SaveImage;
#define WORKP ((u8*)savedataGetStatus(0, 0xE))
    u8* dat = (u8*)lbl_80267840;
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
        __assert((char*)(dat + 0x98), 0x267, (char*)(dat + 0xB0));
    }
    if (fn_801906A0(0x8AE) != 0) {
        __assert((char*)(dat + 0x98), 0x268, (char*)(dat + 0xD0));
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
            cmd = (*(s32(*)[6])(dat + 0))[v];
            switch (v) {
            case 1:
                floorId = 1;
                break;
            }
            break;
        }
        case 0xA8: {
            s32 t;
            switch (fn_80071344()) {
            case 0:
                cmd = 0xAC;
                t = 0;
                *(s32*)(WORKP + 4) = t;
                t = 0;
                *(s32*)(WORKP + 0x10) = t;
                t = 0;
                *(s32*)(WORKP + 0) = t;
                fn_8006A7E0(WORKP + 0x59A8, 0);
                break;
            case 1:
                cmd = 0xB3;
                t = 2;
                *(s32*)(WORKP + 4) = t;
                t = 4;
                *(s32*)(WORKP + 0x10) = t;
                t = 2;
                *(s32*)(WORKP + 0) = t;
                break;
            case 3:
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0xAC: {
            fn_8006B4AC(0);
            {
                s32 v = fn_80071344();
                if (v < 0) {
                    cmd = -1;
                    break;
                }
                prevCmd = (*(s32(*)[6])(dat + 0x18))[v];
            }
            if (fn_8006AFC4(WORKP) == 0 && (prevCmd == 0xAE || prevCmd == 0xAF)) {
                winMsgOpen(2, 0x3BFE, 1, 0);
                winMsgClose(1);
                break;
            }
            switch (prevCmd) {
            case 0xAE: {
                s32 t = 0;
                *(s32*)(WORKP + 0) = t;
                cmd = prevCmd;
                break;
            }
            case 0xAF: {
                s32 t;
                t = 1;
                *(s32*)(WORKP + 0) = t;
                t = 6;
                *(s32*)(WORKP + 0xC) = t;
                t = 2;
                *(s32*)(WORKP + 8) = t;
                cmd = prevCmd;
                break;
            }
            case 0xAD:
                if ((u8)fn_8006A7BC(WORKP) == 0 || fn_8006AFC4(WORKP) == 0) {
                    winMsgOpen(2, 0x4415, 1, 0);
                    winMsgClose(1);
                    break;
                }
                cmd = prevCmd;
                break;
            default:
                cmd = prevCmd;
                break;
            }
            break;
        }
        case 0xAD: {
            s32 ok = 0;
            u8* st;
            s32 r;
            if ((u8)fn_8006A7BC(WORKP) != 0) {
                if (fn_8006AFC4(WORKP) != 0) {
                    ok = 1;
                }
            }
            if (ok == 0) {
                __assert((char*)(dat + 0x98), 0x30F, (char*)(dat + 0x10C));
            }
            cmd = (s32)(WORKP + 0xC98C);
            st = fn_8006AFC4(WORKP);
            if ((u8)menuIsCheck(0xC8)) {
                menuClose(0xC8);
                while ((u8)menuIsCheck(0xC8)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(st);
            menuOpenCustom(0xC8, 0, NULL, 0x10, 0, 4, fn_8006A7C8(st), r, 0, cmd);
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
                fn_8006E0CC();
                cmd = -1;
                break;
            }
            fn_8006A7F0(fn_8006AFE4(1), fn_8006AFC4(WORKP));
            if ((u8)fn_8006A76C(WORKP) != 0) {
                fn_8006A79C(WORKP);
            } else {
                s32 wasOpen = fn_8006A7BC(WORKP);
                fn_8006A79C(WORKP);
                winMsgOpen(2, 0x44D9, 1, 0);
                winMsgClose(1);
                if (fn_80088C60() < 0) {
                    if ((u8)wasOpen != 0) {
                        fn_8006A7AC(WORKP);
                    }
                    menuCloseCustom(0xC8, 0, 0);
                    fn_8006E0CC();
                    cmd = -1;
                    break;
                }
            }
            if (*(s32*)(WORKP + 0xC98C) != 1) {
                u8* q = WORKP + 0x10000;
                s32 t;
                t = *(s32*)(q - 0x3674);
                *(s32*)(WORKP + 0) = t;
                t = *(s32*)(q - 0x3670);
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(q - 0x366C);
                *(s32*)(WORKP + 8) = t;
                t = *(s32*)(q - 0x3668);
                *(s32*)(WORKP + 0xC) = t;
                t = *(s32*)(q - 0x3664);
                *(s32*)(WORKP + 0x10) = t;
                t = *(s32*)(q - 0x3660);
                *(s32*)(WORKP + 0x14) = t;
                t = *(s32*)(q - 0x365C);
                *(s32*)(WORKP + 0x18) = t;
                if (*(u32*)(q - 0x3660) == 0) {
                    fn_8006ADB4(0);
                }
                if (*(s32*)(WORKP + 0) != 0) {
                    __assert((char*)(dat + 0x98), 0xAB, (char*)(dat + 0x158));
                }
                fn_8019075C(0x8AE, (*(s32*)(q - 0x3670) == 0) ? 1 : 2);
                fn_80069C0C(WORKP);
                cmd = 0xD1;
            } else {
                u8* q = WORKP + 0x10000;
                s32 t;
                t = *(s32*)(q - 0x3674);
                *(s32*)(WORKP + 0) = t;
                t = *(s32*)(q - 0x3670);
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(q - 0x366C);
                *(s32*)(WORKP + 8) = t;
                t = *(s32*)(q - 0x3668);
                *(s32*)(WORKP + 0xC) = t;
                t = *(s32*)(q - 0x3664);
                *(s32*)(WORKP + 0x10) = t;
                t = *(s32*)(q - 0x3660);
                *(s32*)(WORKP + 0x14) = t;
                t = *(s32*)(q - 0x365C);
                *(s32*)(WORKP + 0x18) = t;
                if (*(u32*)(q - 0x3660) == 0) {
                    fn_8006ADB4(0);
                }
                if (*(s32*)(WORKP + 0xC) != 6) {
                    __assert((char*)(dat + 0x98), 0x81, (char*)(dat + 0x184));
                }
                if (*(s32*)(WORKP + 0) != 1) {
                    __assert((char*)(dat + 0x98), 0x82, (char*)(dat + 0x1B0));
                }
                fn_8019075C(0x8AE, (*(s32*)(q - 0x3670) == 0) ? 1 : 2);
                fn_8019075C(0xB59, *(s32*)(WORKP + 0x14));
                fn_8019075C(0xAFC, 0);
                fn_8019075C(0xB11, 0);
                fn_8019075C(0xDE1, 0);
                heroMoveSyncWithHero();
                fn_8019075C(0xAFC, *(s32*)(q - 0x3634));
                fn_8019075C(0xB11, *(s32*)(q - 0x362C));
                fn_8019075C(0xDE1, *(s32*)(q - 0x3628));
                floorId = *(s32*)(q - 0x3654);
                posX = *(f32*)(WORKP + 0xC9B4);
                posY = *(f32*)(WORKP + 0xC9B8);
                posZ = *(f32*)(WORKP + 0xC9BC);
                warpArg = (s32)(lbl_8047BF18 * *(f32*)(WORKP + 0xC9C4));
                posFlag = 1;
                cmd = 0x105;
            }
            menuCloseCustom(0xC8, 0, 0);
            fn_8006E0CC();
            break;
        }
        case 0xAE: {
            s32 t;
            s32 v;
            t = 2;
            *(s32*)(WORKP + 8) = t;
            t = 0;
            *(s32*)(WORKP + 0) = t;
            v = fn_80071344();
            switch (v) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                *(s32*)(WORKP + 0xC) = v;
                fn_8006B4AC((*(s32(*)[7])(dat + 0x30))[v]);
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
                u8* npc;
                u8* mem;
                s32 r;
                prevCmd = (u8)(v == 1);
                npc = fn_80104704((u16)fn_8007162C());
                if (npc != NULL) {
                    mem = *(u8**)(npc + 0x20);
                } else {
                    mem = NULL;
                }
                fn_80108518(mem + 0xC, 0x1CE);
                r = menuOpenCustom(0xBC, 0, NULL, 0x10, 1, 1, prevCmd);
                switch (r) {
                case 0:
                case 1:
                case 2:
                case 3: {
                    s32 t;
                    *(s32*)(WORKP + 0x10) = r;
                    t = 0;
                    *(s32*)(WORKP + 8) = t;
                    t = 0;
                    *(s32*)(WORKP + 0xC) = t;
                    cmd = 0xCC;
                    menuCloseCustom(0xBC, 0, 0);
                    break;
                }
                default:
                    menuCloseCustom(0xBC, 0, 1);
                    fn_80108518(mem + 0xC, 0x1CA);
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
            __assert((char*)(dat + 0x98), 0x3C4, (char*)&lbl_8047BF1C);
            break;
        case 0xB0:
            switch (fn_80071344()) {
            case 0:
                if ((u8)fn_801D04E8() == 0) {
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
            prevCmd = (s32)(lbl_8047A5A0 + 0x1660);
            fn_8006B4AC(2);
            if ((u8)fn_800776E4((u8*)prevCmd) == 0) {
                s32 m;
                menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xF5);
                menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, prevCmd, 0, fn_8006B420(), 0);
                menuSetPosition(0xDA, 0, -0x28);
                m = fn_80076054((u8*)prevCmd, fn_8006B420());
                if ((u16)m == 0) {
                    __assert((char*)(dat + 0x98), 0x1BB, (char*)(dat + 0x1E0));
                }
                fn_80166A28(0x26);
                winMsgOpen(7, (u16)m, 1, 0);
                winMsgOpen(7, 0x440A, 1, 0);
                menuCloseCustom(0xDA, 0, 0);
                menuCloseCustom(0xBE, 0, 1);
                menuCloseCustom(0xDA, 0, 1);
                fn_8006E0CC();
                fn_8006B4AC(0);
                winMsgClose(1);
                fn_800714C8();
                cmd = -1;
                break;
            }
            fn_8006B4AC(0);
            heroBiosSetHomePlace((u8*)prevCmd, 0);
            fn_8006AC28(lbl_8047A5A0, 0);
            fn_8006A824(lbl_8047A5A0, (u8*)prevCmd);
            fn_800714C8();
            cmd = 0xB2;
            break;
        }
        case 0xED: {
            u32 i = 0;
            s32 r;
            u8* arr[4];
            prevCmd = i;
            for (; i < 4; i++) {
                memset(lbl_8047A5A0 + 0x1660 + prevCmd, 0, 0xB18);
                heroInit(lbl_8047A5A0 + 0x1660 + prevCmd);
                prevCmd += 0xB18;
            }
            arr[0] = NULL;
            arr[1] = lbl_8047A5A0 + 0x1660;
            arr[2] = NULL;
            arr[3] = NULL;
            fn_8006B4AC(2);
            r = fn_800849B4(0, 2, arr, NULL);
            fn_8006B4AC(0);
            if (r < 0) {
                fn_800714C8();
                cmd = -1;
                break;
            }
            fn_8006AC28(lbl_8047A5A0, 0);
            fn_8006A824(lbl_8047A5A0, lbl_8047A5A0 + 0x1660);
            fn_800714C8();
            cmd = 0xB2;
            break;
        }
        case 0xB2: {
            u8* sv = lbl_8047A5A0;
            s32 r;
            if ((u8)menuIsCheck(0xDA)) {
                menuClose(0xDA);
                while ((u8)menuIsCheck(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(sv);
            menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(sv), r, 0, 0);
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
                if (fn_8006AFC4(WORKP) != 0) {
                    u8* p;
                    menuCloseCustom(0xDA, 0, 0);
                    fn_8006E0CC();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xEB);
                    p = fn_8006AFC4(WORKP);
                    if ((u8)menuIsCheck(0xDA)) {
                        menuClose(0xDA);
                        while ((u8)menuIsCheck(0xDA)) {
                            _threadSwitch();
                        }
                    }
                    r = fn_8006A7E8(p);
                    menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(p), r, 0, 0);
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
                    if ((u8)fn_8006A7BC(WORKP) != 0) {
                        winMsgOpen(2, 0x44C2, 1, 0);
                        prevCmd = menuSubOpenYesNo(0, 0x3C, 0x9E, 1);
                        winMsgClose(1);
                        if (prevCmd != 0) {
                            goto b2_cancel;
                        }
                    }
                    menuCloseCustom(0xDA, 0, 0);
                    fn_8006E0CC();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, first);
                    {
                        u8* sv2 = lbl_8047A5A0;
                        if ((u8)menuIsCheck(0xDA)) {
                            menuClose(0xDA);
                            while ((u8)menuIsCheck(0xDA)) {
                                _threadSwitch();
                            }
                        }
                        r = fn_8006A7E8(sv2);
                        menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(sv2), r, 0, 0);
                    }
                }
                memcpy(lbl_8047A5A0 + 0x4318, savedataGetStatus(0, 0xE), 0xCC2C);
                fn_8006AF44(savedataGetStatus(0, 0xE), lbl_8047A5A0);
                if ((u8)fn_801D04E8() == 0) {
                    fn_80166A28(0x26);
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
                fn_8006E0CC();
                fn_800714C8();
                winMsgClose(1);
                cmd = -1;
                break;
            }
        b2_cancel:
            if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                winMsgOpen(2, 0x3D55, 1, 0);
                menuCloseCustom(0xDA, 0, 0);
                fn_8006E0CC();
                winMsgClose(1);
            }
            cmd = -1;
            break;
        }
        case 0xCC: {
            u8* p;
            s32 r;
            fn_80069C0C(savedataGetStatus(0, 0xE));
            p = fn_8006B09C(0);
            if ((u8)menuIsCheck(0xDA)) {
                menuClose(0xDA);
                while ((u8)menuIsCheck(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(p);
            menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(p), r, 0, 0);
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
            fn_8006E0CC();
            if (r != 0) {
                cmd = -1;
                break;
            }
            cmd = 0xD1;
            break;
        }
        case 0xB1: {
            s32 r;
            s32 m;
            u8* wm;
            cmd = (s32)fn_8006AFC4(WORKP);
            if (*(s32*)(WORKP + 4) == 2) {
                __assert((char*)(dat + 0x98), 0x4B9, (char*)(dat + 0x1F4));
            }
            if ((u32)cmd == 0) {
                __assert((char*)(dat + 0x98), 0x4BA, (char*)&lbl_8047BF20);
            }
            wm = fn_8006B420();
            if ((u8)menuIsCheck(0xDA)) {
                menuClose(0xDA);
                while ((u8)menuIsCheck(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8((void*)cmd);
            menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)cmd), r, wm, 0);
            m = fn_80076054((u8*)cmd + 0xB44, fn_8006B420());
            if ((u16)m != 0) {
                winMsgOpen(1, (u16)m, 1, 0);
                winMsgClose(1);
                menuCloseCustom(0xDA, 0, 0);
                fn_8006E0CC();
                fn_800714C8();
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
                fn_8006E0CC();
                cmd = -1;
                break;
            }
            {
                u8* dst = lbl_8047A5A0 + 0x4318;
                SaveImage* dstSaveImage = (SaveImage*)dst;
                SaveImage* srcSaveImage = (SaveImage*)savedataGetStatus(0, 0xE);

                *dstSaveImage = *srcSaveImage;
                prevCmd = (s32)fn_8006AFC4(dst);
                if ((u8)fn_8006A7BC(dst) != 0 && (u32)prevCmd != 0) {
                    menuCloseCustom(0xDA, 0, 0);
                    fn_8006E0CC();
                    menuCloseCustom(0xBE, 0, 1);
                    menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, 0xD7);
                    if ((u8)menuIsCheck(0xC8)) {
                        menuClose(0xC8);
                        while ((u8)menuIsCheck(0xC8)) {
                            _threadSwitch();
                        }
                    }
                    r = fn_8006A7E8((void*)prevCmd);
                    menuOpenCustom(0xC8, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)prevCmd), r, 0, dst + 0xC98C);
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
                    fn_8006E0CC();
                    menuCloseCustom(0xBE, 0, 1);
                    if (r != 0) {
                        cmd = -1;
                        break;
                    }
                    if ((u8)fn_8006A76C(WORKP) == 0) {
                        u8* wm2;
                        menuOpenCustom(0xBE, 0, NULL, 0x10, 0, 1, first);
                        wm2 = fn_8006B420();
                        if ((u8)menuIsCheck(0xDA)) {
                            menuClose(0xDA);
                            while ((u8)menuIsCheck(0xDA)) {
                                _threadSwitch();
                            }
                        }
                        r = fn_8006A7E8((void*)cmd);
                        menuOpenCustom(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)cmd), r, wm2, 0);
                    }
                }
            }
            if ((u8)fn_8006A76C(WORKP) != 0) {
                fn_8006A79C(WORKP);
            } else {
                s32 wasOpen = fn_8006A7BC(WORKP);
                fn_8006A79C(WORKP);
                if (fn_80088C60() < 0) {
                    if ((u8)wasOpen != 0) {
                        fn_8006A7AC(WORKP);
                    }
                    menuCloseCustom(0xDA, 0, 0);
                    fn_8006E0CC();
                    cmd = -1;
                    break;
                }
            }
            {
                s32 v3;
                s32 v2;
                s32 v1;
                s32 v0;
                s32 buf[20];
                s32 t;
                prevCmd = *(s32*)(WORKP + 0x10);
                v3 = *(s32*)(WORKP + 0xC);
                v2 = *(s32*)(WORKP + 8);
                v1 = *(s32*)(WORKP + 4);
                v0 = *(s32*)(WORKP + 0);
                memset(buf, 0, 0x50);
                buf[0] = v0;
                buf[1] = v1;
                buf[2] = v2;
                buf[3] = v3;
                buf[4] = prevCmd;
                buf[5] = 0;
                buf[6] = 5;
                if (*(s32*)(WORKP + 0) == 1) {
                    t = buf[0];
                    *(s32*)(WORKP + 0) = t;
                    t = buf[1];
                    *(s32*)(WORKP + 4) = t;
                    t = buf[2];
                    *(s32*)(WORKP + 8) = t;
                    t = buf[3];
                    *(s32*)(WORKP + 0xC) = t;
                    t = buf[4];
                    *(s32*)(WORKP + 0x10) = t;
                    t = buf[5];
                    *(s32*)(WORKP + 0x14) = t;
                    t = buf[6];
                    *(s32*)(WORKP + 0x18) = t;
                    if ((u32)buf[5] == 0) {
                        fn_8006ADB4(0);
                    }
                    if (*(s32*)(WORKP + 0xC) != 6) {
                        __assert((char*)(dat + 0x98), 0x81, (char*)(dat + 0x184));
                    }
                    if (*(s32*)(WORKP + 0) != 1) {
                        __assert((char*)(dat + 0x98), 0x82, (char*)(dat + 0x1B0));
                    }
                    fn_8019075C(0x8AE, (buf[1] == 0) ? 1 : 2);
                    fn_8019075C(0xB59, *(s32*)(WORKP + 0x14));
                    fn_8019075C(0xAFC, 0);
                    fn_8019075C(0xB11, 0);
                    fn_8019075C(0xDE1, 0);
                    heroMoveSyncWithHero();
                    floorId = 0x4C;
                    cmd = 0x105;
                } else {
                    t = buf[0];
                    *(s32*)(WORKP + 0) = t;
                    t = buf[1];
                    *(s32*)(WORKP + 4) = t;
                    t = buf[2];
                    *(s32*)(WORKP + 8) = t;
                    t = buf[3];
                    *(s32*)(WORKP + 0xC) = t;
                    t = buf[4];
                    *(s32*)(WORKP + 0x10) = t;
                    t = buf[5];
                    *(s32*)(WORKP + 0x14) = t;
                    t = buf[6];
                    *(s32*)(WORKP + 0x18) = t;
                    if ((u32)buf[5] == 0) {
                        fn_8006ADB4(0);
                    }
                    if (*(s32*)(WORKP + 0) != 0) {
                        __assert((char*)(dat + 0x98), 0xAB, (char*)(dat + 0x158));
                    }
                    fn_8019075C(0x8AE, (buf[1] == 0) ? 1 : 2);
                    fn_80069C0C(WORKP);
                    cmd = 0xD1;
                }
            }
            menuCloseCustom(0xDA, 0, 0);
            fn_8006E0CC();
            break;
        }
        case 0xB3: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            if (v < 4) {
                u8* e = dat + 0x4C;
                s32* st;
                s32 t;
                e += v << 4;
                st = (s32*)(e + 8);
                if (*st == 0) {
                    if ((u8)fn_801D04E8() == 0) {
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
                t = *(s32*)e;
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(e + 4);
                *(s32*)(WORKP + 0xC) = t;
                fn_8006A7E0(WORKP + 0x59A8, *st);
                fn_8006A81C(WORKP + 0x59A8, (s8)e[0xC]);
                {
                    u8* p = e + 1;
                    u32 i = 1;
                    prevCmd = 0x1660;
                    for (; i < 4; i++) {
                        fn_8006A7E0(WORKP + (prevCmd + 0x59A8), 1);
                        fn_8006A81C(WORKP + (prevCmd + 0x59A8), (s8)p[0xC]);
                        prevCmd += 0x1660;
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
                fn_80077E80(lbl_8047A5A0 + 0x42C0, fn_8006B420());
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
                    fn_800714C8();
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
            s32 r = menuOpenCustom(fn_8007162C(), 0, NULL, 0x10, 1, 1, lbl_8047A5A0 + 0x42C0);
            if (r < 0) {
                cmd = -1;
                break;
            }
            switch (r) {
            case 5:
                fn_800714C8();
                cmd = 0xC0;
                break;
            case 6:
                if (menuGetCursorItemID(fn_8007162C()) == 0x9FC) {
                    fn_80077E80(lbl_8047A5A0 + 0x42C0, fn_8006B51C(0));
                    *(s16*)(lbl_8047A5A0 + 0x42C6) = 6;
                    break;
                }
                goto c1_apply;
            case 7:
            c1_apply:
                if ((u8)fn_80077EA4(lbl_8047A5A0 + 0x42C0, fn_8006B420()) == 0) {
                    s32 ofs = *(s32*)(WORKP + 8) * 0x54;
                    fn_80077E80(WORKP + (ofs + 0xC9DC), lbl_8047A5A0 + 0x42C0);
                    for (;;) {
                        if ((u8)fn_801D04E8() == 0) {
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
            break;
        }
        case 0xB4:
            __assert((char*)(dat + 0x98), 0x5F5, (char*)&lbl_8047BF1C);
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
                    cmd = fn_80071398(0xB3);
                } else {
                    cmd = fn_80071398(0xAF);
                }
                break;
            }
            break;
        }
        case 0xC2:
            switch (fn_80071344()) {
            case 0:
                fn_800714C8();
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
                cmd = fn_80071398(0xB3);
                break;
            }
            break;
        case 0xE4: {
            u32 off;
            s32 mode2;
            u8* arr[4];
            s32 r;
            prevCmd = 0;
            off = prevCmd;
            for (; (u32)prevCmd < 4; prevCmd++) {
                memset(lbl_8047A5A0 + 0x1660 + off, 0, 0xB18);
                heroInit(lbl_8047A5A0 + 0x1660 + off);
                off += 0xB18;
            }
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
                cmd = fn_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 0x59AC) == 0) {
                heroBiosSetHomePlace(lbl_8047A5A0 + 0x1660, 0);
            }
            switch (*(s32*)(WORKP + 4)) {
            case 0:
            case 1:
                fn_8006A824(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                fn_8006A824(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
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
                fn_8006A824(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                fn_8006A824(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
                fn_8006A824(WORKP + 0x8668, lbl_8047A5A0 + 0x2C90);
                fn_8006A824(WORKP + 0x9CC8, lbl_8047A5A0 + 0x37A8);
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
                u32 i = 0;
                u32 zb;
                prevCmd = 0;
                zb = prevCmd;
                for (; i < 4; i++) {
                    *(u8*)(WORKP + (prevCmd + 0x7005)) = zb;
                    prevCmd += 0x1660;
                }
            }
            v = fn_80071344();
            if (v < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        winMsgOpen(2, 0x4445, 1, 0);
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            r = fn_8010264C(0xD0, 1);
            menuClose(0xD0);
            if (r < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        winMsgOpen(2, 0x4445, 1, 0);
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 4) != 2) {
                fn_80069C0C(WORKP);
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
                u8* e = dat + 0x8C;
                s32 t;
                e += v * 4;
                t = (s8)e[0];
                *(s32*)(WORKP + 0x59D0) = t;
                t = (s8)e[1];
                *(s32*)(WORKP + 0x7030) = t;
                t = (s8)e[2];
                *(s32*)(WORKP + 0x8690) = t;
                t = (s8)e[3];
                *(s32*)(WORKP + 0x9CF0) = t;
                fn_80069C0C(WORKP);
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
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        winMsgOpen(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    msgctrlSetValue(0x2F, e);
                    winMsgOpen(2, 0x44B8, 1, 0);
                    break;
                }
                winMsgClose(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    winMsgClose(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            }
        b5_done:
            break;
        }
        case 0xD1:
            fn_8006B8FC();
            floorId = 0x397;
            break;
        case 0xB8:
            savedataGetStatus(0, 0xE);
            fn_80062948();
            cmd = fn_80071398();
            break;
        case 0xB9: {
            s32 r = fn_8010264C(0xB9, 1);
            menuCloseCustom(0xB9, 0, 0);
            menuCloseCustom(0xBE, 0, 1);
            switch (r) {
            case 0:
                if ((u8)fn_801D04E8() == 0) {
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
                    while ((u8)fn_800F7EF8(1) == 0) {
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
            u32 coins;
            s32 st;
            if ((u8)fn_8006A7BC(WORKP) == 0) {
                coins = fn_8006ADEC();
                cmd = (s32)fn_8006AFC4(WORKP);
                if ((u32)cmd == 0) {
                    __assert((char*)(dat + 0x98), 0x72C, (char*)&lbl_8047BF24);
                }
                if (coins == 0) {
                    goto quit105;
                }
                fn_8006ADB4(0);
                if (fn_8006A7E8((void*)cmd) == 0) {
                    if ((u8)fn_8006A76C(WORKP) != 0) {
                        goto quit105;
                    }
                    prevCmd = coins;
                    fn_801293FC(0, coins);
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
                        fn_80129384(0, prevCmd);
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
                        u8* nm;
                        arr2[0] = NULL;
                        arr2[1] = lbl_8047A5A0 + 0x1660;
                        arr2[2] = NULL;
                        arr2[3] = NULL;
                        r = fn_800849B4(0, 0x40, arr2, money);
                        if (r < 0) {
                            continue;
                        }
                        h = heroBiosGetRnd((u8*)cmd + 0xB44);
                        if (h != heroBiosGetRnd(lbl_8047A5A0 + 0x1660)) {
                            goto mismatch105;
                        }
                        nm = heroBiosGetNamePtr((u8*)cmd + 0xB44);
                        if (GScharCmp(heroBiosGetNamePtr(lbl_8047A5A0 + 0x1660), nm) != 0) {
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
                        fn_80092C90(1, money, 0);
                        if (fn_80093574(1) == 0xC) {
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
                    } while (fn_80088C60() < 0);
                }
            quit105:
                winMsgClose(1);
            }
            st = *(s32*)(WORKP + 0);
            switch (st) {
            case 0:
                cmd = fn_80071398(0xAE);
                break;
            case 1:
            default:
                cmd = fn_80071398(0xAC);
                break;
            }
            break;
        }
        default:
            break;
        }
        if ((u8)fn_800F7EF8(1) == 0 && (u8)fn_8008ABA0(1) == 0) {
            winMsgOpen(2, 0x3C4F, 1, 0);
            while ((u8)fn_800F7EF8(1) == 0) {
                _threadSwitch();
            }
            winMsgClose(1);
        }
        prevCmd = first;
        if (cmd < 0) {
            s32 r = fn_800714C8();
            menuCloseCustom(0xBE, 0, 1);
            if (r < 0) {
                goto done;
            }
        } else if (cmd != fn_8007162C()) {
            s32 r = fn_8007162C();
            if (r == windowGetActiveID()) {
                menuCloseCustom(fn_8007162C(), 0, 0);
            }
            menuCloseCustom(0xBE, 0, 1);
            fn_800715BC(cmd);
        }
        if (floorId != 0) {
            s32 r = fn_8007162C();
            if (r != windowGetActiveID()) {
                goto done;
            }
            menuCloseCustom(fn_8007162C(), 0, 0);
            menuCloseCustom(0xBE, 0, 1);
            goto done;
        }
    }
done:
    if (posFlag != 0) {
        fn_8006B8FC();
        floorChangePos(floorId, warpArg, posX, posY, posZ);
    } else {
        if (floorId == 0) {
            floorId = 0x3A1;
        }
        if (floorId == 0x3A1) {
            fn_8006B8F0();
            if ((u8)fn_800F7EF8(1) == 0) {
                winMsgOpen(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                while ((u8)fn_800F7EF8(1) == 0) {
                    _threadSwitch();
                }
                winMsgClose(1);
            }
        } else {
            fn_8006B8FC();
        }
        floorLink(floorId, 0);
    }
#undef WORKP
}
#pragma pop
/* Address: 0x8005CCD0 | Size: 0xB8 */
#pragma push
#pragma scheduling off
#pragma peephole off
void menuColosseumBattleExit(void) {
    u32 handle;

    fadeCheck(1);
    if (fn_8006B8E8() == 0) {
        ColosseumSaveWork* work;

        handle = 0;
        work = (ColosseumSaveWork*)savedataGetStatus(0, 0xE);
        work->exitPending = handle;
        _flagSet(0x8AE, handle);
    }

    menuClose(0xD3);
    toolentryTaisenFreePokemonData();
    fn_801CB9D8(((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle);
    handle = fn_800E202C(lbl_8047A5A0);
    if ((u16)handle == 0) {
        __assert(lbl_802678D8, 0x252, &lbl_8047BF28);
    }
    fn_800E24B0(handle);
    fn_800E209C(handle);
    lbl_8047A5A0 = NULL;
}
#pragma pop

void menuColosseumBattleInit(void)
{
    extern void fn_80165A20(s32, s32, s32);
    extern u32 fn_800E2C04(u32 size, u32 alignment);
    extern void* fn_80113F48(void);
    extern void* fn_801CBA0C(u32 size);
    extern void GSresGetResource(void* archive, void* destination);
    extern void cameraPlayAnime(s32 id, u32 data, s32 start, s32 loop);
    extern void GSscene_SetMode(s32 mode);
    extern u8 fn_800FF548(void);
    extern void toolentryDebugPokemonCreate(void);
    extern void fn_8006B5D0(void* status);
    u32 heap;
    void* archive;

    fn_80165A20(0x1E, 0, 0xFF);
    _flagSet(0x8AE, 0);

    if (lbl_8047A5A0 != NULL) {
        __assert(lbl_80267840 + 0x98, 0x20F, lbl_80267840 + 0x21C);
    }

    heap = fn_800E2C04(0x10F60, 0x20);
    if ((u16)heap == 0) {
        __assert(lbl_80267840 + 0x98, 0x212, &lbl_8047BF28);
    }
    lbl_8047A5A0 = fn_800E27B0(heap);
    if (lbl_8047A5A0 == NULL) {
        __assert(lbl_80267840 + 0x98, 0x213, lbl_80267840 + 0x22C);
    }

    archive = fn_80113F48();
    ((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle =
        fn_801CBA0C(0x0FFE1000);
    GSresGetResource(
        archive, ((ColosseumMenuHeap*)lbl_8047A5A0)->resourceHandle);
    cameraPlayAnime(0x531, 0x0FFF1800, 0, 1);
    GSscene_SetMode(4);
    if (fn_800FF548() == 0) {
        toolentryDebugPokemonCreate();
        fn_8006B5D0(savedataGetStatus(0, 0xE));
    }
    if (fn_8006B8E8() == 0) {
        ((ColosseumSaveWork*)savedataGetStatus(0, 0xE))->exitPending = 0;
    }
    menuOpen(0xD3, 0);
}
