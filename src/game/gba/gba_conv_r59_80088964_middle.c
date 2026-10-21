/**
 * @file gba_conv_r59_80088964_middle.c
 * @brief gba_conv.c carve: menuMemcardTaskA and menuMemcardTaskB, 0x80088964 - 0x800889E4.
 *
 * Standalone: gba_conv.c's declarations plus only these bodies, copied
 * from gba_conv.c.
 */

#include "dolphin/types.h"
#include "game/gba/gba_conv.h"

/* ===== External function declarations ===== */
extern void fn_8001E074();
extern void fn_8005CF2C();
extern void fn_8006A76C();
extern void savedataBattleClearFlagC98B();
extern void savedataBattleSetFlagC98B();
extern void savedataBattleGetFlagC98B();
extern void savedataBattleSetCouponRun();
extern void savedataBattleGetCouponRun();
extern void savedataBattleGetSavedHeroKind();
extern void menuCommonFree();
extern void menuCommonAlloc();
extern void menuRuleCheckPartyAndRule();
extern void fn_8008ABA0();
extern void gbaCommStartTransferA();
extern void gbaCommStartTransferB();
extern void gbaCommStartTransferC();
extern void gbaCommStartTransferFile();
extern void gbaCommIsBusy();
extern void gbaCommGetResult();
extern void gbaCommEnd();
extern void SIProbe();
extern void GSgetFrameStep();
extern void GSgfxGetVsyncRate();
extern void GSmemGetHandleByPtr();
extern void GSmemFree();
extern void fn_800E24B0();
extern void GSmemLock();
extern void GSmemAlloc();
extern void GSmodelIsAnimating();
extern void GSmodelStartAnimation();
extern void GSmodelSetAnimRate();
extern void GSmodelSetAnimFrame();
extern void GSmodelSetAnimType();
extern void GSmodelSetAnimIndex();
extern void _threadSwitch();
extern void GSresFindById();
extern void GScharToGBA();
extern void GSmsgGetGSchar();
extern void std092_getCurrentRoomID();
extern void floorBattleEndTerminateScripts();
extern void std098_mapWarpToMenuRoom();
extern void menuClose();
extern void menuCloseCustom();
extern void menuIsCheck();
extern void menuOpen();
extern void menuGetEnablePort();
extern void menuSetEnablePort();
extern void windowGetFreeWork();
extern void windowSearchItemID();
extern void windowSearchID();
extern void windowGetPortKeyInfo();
extern void windowGetKeyInfo();
extern void winMsgCheck();
extern void winMsgClose();
extern void winMsgOpen();
extern void fn_801081F8();
extern void winSpriteSetDisp();
extern void floorGetPrevFloorID();
extern void wazaDataBiosGetDoc();
extern void wazaDataBiosGetPtr();
/* ... and 27 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478950;
extern u8 lbl_80478954;
extern u8 lbl_80478958;
extern u8 lbl_8047A660;
extern u8 lbl_8047A664;
extern u8 lbl_8047A668;
extern u8 lbl_8047A66C;
extern u8 lbl_8047C190;
extern u8 lbl_8047C198;
extern u8 lbl_8047C1A0;
extern u8 lbl_8047C1A8;
extern u8 lbl_8047C1AC;
extern u8 lbl_8047C1B0;
extern u8 lbl_8047C1B8;
extern u8 lbl_8047C1C0;
extern u8 lbl_8047C1C4;
extern u8 lbl_8047C1C8;
extern u8 lbl_8047C1CC;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEB78[];
extern u8 lbl_8026F2E8[];
extern u8 lbl_8026F488[];
extern u8 lbl_8026F4F8[];
extern u8 lbl_803FB2F8[];

/* ===== Forward declarations ===== */
void cardeArenaWalk(void);
void cardeArenaCountRecords(void);
void cardeArenaClear(void);
void cardeArenaGetPtr(void);
void cardeEncodePartyMoveText(void);
void cardeEncodeMoveText(void);
void cardeGBAMenuButton(void);
void menuGBACRun(void);
void cardeTransferUI(void);
void cardeGbaTransfer(void);
void cardeWaitMessage(void);
void cardeGridPrompt(void);
void fn_80088428(void);
void dnaKeyCodeCheck(void);
void menuSaveLoadReceiveGift(void);
s32 menuMemcardTaskA(void);
s32 menuMemcardTaskB(void);
void menuSaveLoadSaveGame(void);
void menuSaveLoadCommitA(void);
void menuSaveLoadCommitB(void);
void menuSaveLoadStorePosition(u8* p);
s32 fn_80088F58(void);
s32 fn_80088F74(void);
s32 fn_80088F88(void);
s32 dbgMenuGBAAddCoupon(void);
s32 dbgToolBattleDebugSetAGBConnectionMode(s32 a, s32 b);
s32 fn_80089028(void);
void fn_80089030(u8 x);

/* ===== Function implementations ===== */


/* 0x80088964 | size: 0x40 */
s32 menuMemcardTaskA(void) {
    extern s32 memcardWaitTask2(s32, s32, s32);

    if (memcardWaitTask2(0xd, 2, 0) == 4) {
        return 0;
    } else {
        return -1;
    }
}

/* 0x800889A4 | size: 0x40 */
s32 menuMemcardTaskB(void) {
    extern s32 memcardWaitTask2(s32, s32, s32);

    if (memcardWaitTask2(5, 2, 0) == 4) {
        return 0;
    } else {
        return -1;
    }
}
