/**
 * @file gba_conv_candidate_80088F58.c
 * @brief gba_conv.c carve: fn_80088F58 .. dbgToolBattleDebugSetAGBConnectionMode, 0x80088F58 - 0x80089028.
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
extern void fn_80093610();
extern void gbaCommEnd();
extern void SIProbe();
extern void msgGetCharsPerFrame();
extern void fn_800D37CC();
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
void fn_80083AF4(void);
void fn_80083BF8(void);
void fn_80083CBC(void);
void fn_80083CFC(void);
void fn_80083D30(void);
void fn_80083ECC(void);
void fn_80084034(void);
void fn_80084038(void);
void fn_800849B4(void);
void cardeGbaTransfer(void);
void fn_80087AE8(void);
void fn_80087C64(void);
void fn_80088428(void);
void dnaKeyCodeCheck(void);
void menuSaveLoadReceiveGift(void);
s32 fn_80088964(void);
s32 fn_800889A4(void);
void menuSaveLoadSaveGame(void);
void menuSaveLoadCommitA(void);
void fn_80088D84(void);
void menuSaveLoadStorePosition(u8* p);
s32 fn_80088F58(void);
s32 fn_80088F74(void);
s32 fn_80088F88(void);
s32 dbgMenuGBAAddCoupon(void);
s32 dbgToolBattleDebugSetAGBConnectionMode(s32 a, s32 b);
s32 fn_80089028(void);
void fn_80089030(u8 x);

/* ===== Function implementations ===== */


/* 0x80088F58 | size: 0x1C */
s32 fn_80088F58(void) {
    u32 count;

    count = *(volatile u32*)&lbl_8047A66C;
    count++;
    *(volatile u32*)&lbl_8047A66C = count;
    *(volatile u32*)&lbl_8047A668 = 0;
    return 0;
}

/* 0x80088F74 | size: 0x14 */
s32 fn_80088F74(void) {
    *(u32*)&lbl_8047A668 += 1;
    return 0;
}

/* 0x80088F88 | size: 0x1C */
s32 fn_80088F88(void) {
    u32 count;

    count = *(volatile u32*)&lbl_8047A664;
    count++;
    *(volatile u32*)&lbl_8047A664 = count;
    *(volatile u32*)&lbl_8047A660 = 0;
    return 0;
}

/* 0x80088FA4 | size: 0x54 */
s32 dbgMenuGBAAddCoupon(void) {
    /* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
    #pragma peephole off
    extern s32 menuOpen(s32, s32);
    extern void menuClose(s32);
    s32 r31;

    r31 = menuOpen(2, 1);
    menuClose(2);
    if (r31 >= 0) {
        *(u32*)&lbl_8047A660 += r31;
    }
    return 0;
}

/* 0x80088FF8 | size: 0x30 */
s32 dbgToolBattleDebugSetAGBConnectionMode(s32 a, s32 b) {
    fn_80089030(b == 0);
    return 0;
}

