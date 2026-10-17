/**
 * @file menuColosseumBattle_exact_8005CEE8.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x8005CEE8 - 0x8005D184.
 */
#include "dolphin/types.h"
#include "game/data/rodata_80267398.h"
#include "game/menu/menu.h"

typedef struct ColosseumSaveWork {
    s32 battleKind;
    u8 pad_04[0x18];
    u8 exitPending;
} ColosseumSaveWork;

extern void fn_800347B8(void);
extern void fn_800347C4(void);
extern void fn_800347E8(s32);
extern void fn_8003480C(s32);
extern void menuColosseumBattleClearFlag();
extern void menuWaitFaceSprites();
extern void floorRequestChange(s32);
extern void soundStartSE();
extern void* savedataGetStatus();
extern void menuSetPosition();
extern void winMsgOpen();
extern void menuCloseCustom();
extern s32 menuOpen(s32, s32);
extern void menuClose();
extern void __assert();

extern char lbl_802678D8[];

/* Retail retains the otherwise-unreferenced callback entry points below. */
#pragma push
#pragma force_active on

/* Address: 0x8005CEE8 | Size: 0x44 */
void menuColosseumBattle(s32 battleKind) {
    ColosseumSaveWork* work;

    menuColosseumBattleClearFlag();
    work = (ColosseumSaveWork*)savedataGetStatus(0, 0xE);
    work->battleKind = battleKind;
    floorRequestChange(0x395);
}

/* Address: 0x8005CF2C | Size: 0x168 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8005CF2C(void* hero, s32 result) {
    s32 messageId;

    menuOpenCustom(MENU_ID(0xBE), 0, NULL, 0x10, NULL, 1, 0xF5);
    menuOpenCustom(MENU_ID(0xDA), 0, NULL, 0x10, NULL, 4,
                   hero, result, savedataBattleGetRulePtr(), 0);
    menuSetPosition(0xDA, 0, -0x28);

    messageId = fn_80076054(hero, savedataBattleGetRulePtr());
    if ((u16)messageId == 0) {
        __assert(lbl_802678D8, 0x1BB, lbl_80267A20);
    }

    soundStartSE(0x26);
    winMsgOpen(7, (u16)messageId, 1, 0);
    switch (result) {
    case 0:
        winMsgOpen(7, 0x440A, 1, 0);
        break;
    case 1:
    case 2:
    default:
        winMsgOpen(7, 0x3C4E, 1, 0);
        break;
    }

    menuCloseCustom(0xDA, 0, 0);
    menuCloseCustom(0xBE, 0, 1);
    menuCloseCustom(0xDA, 0, 1);
    menuWaitFaceSprites();
}
#pragma pop

/* Address: 0x8005D094 | Size: 0x24 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
s32 fn_8005D094(void) {
    fn_800347B8();
    return -1;
}
#pragma pop

/* Address: 0x8005D0B8 | Size: 0x24 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
s32 fn_8005D0B8(void) {
    fn_800347C4();
    return 0;
}
#pragma pop

/* Address: 0x8005D0DC | Size: 0x54 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8005D0DC(void) {
    s32 menuResult = menuOpen(2, 1);

    menuClose(2);
    if (menuResult >= 0) {
        fn_800347E8(menuResult);
    }
    return 0;
}
#pragma pop

/* Address: 0x8005D130 | Size: 0x54 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8005D130(void) {
    s32 menuResult = menuOpen(2, 1);

    menuClose(2);
    if (menuResult >= 0) {
        fn_8003480C(menuResult);
    }
    return 0;
}
#pragma pop
#pragma pop
