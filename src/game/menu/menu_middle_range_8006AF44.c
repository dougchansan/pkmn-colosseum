/**
 * @file menu_middle_range_8006AF44.c
 * @brief menu-middle unit, 0x8006AF44 - 0x8006AFC4 (savedataBattleSetSavedHero).
 *
 * Part of menuCB_Bios.c (0x8006A65C - 0x8006B6B4: the functions that use
 * its rodata, lbl_80267DD8 and the "menuCB_Bios.c" assert block at
 * 0x80267DE8). Its units build with one flag set, GC/1.3, -O4,p,
 * "-opt nopeephole", and the local peephole/scheduling pragmas that part of
 * menu_middle.c carried are gone. Evidence:
 * - every exact function in the range stays exact with no pragma:
 *   menuCBBattleReturnToMenuRoom - fn_8006A81C, menuCBBiosInitHero, menuCBBios_InitTrainer,
 *   fn_8006AC6C, savedataBattleSetCouponRun, savedataBattleGetCouponRun, fn_8006AEEC, savedataBattleGetSavedHero,
 *   savedataBattleGetEntryPtr, savedataBattleGetTeamPtr, savedataBattleGetEntryId - savedataBattleGetCourseCleared, savedataBattleUnlockCourse,
 *   savedataBattleIsCourseUnlocked, menuSetRuleType - fn_8006B5A8 (ten of them used to need
 *   "peephole off" and six "scheduling off");
 * - savedataBattleSetSavedHero (93.4) and savedataBattleGetSavedHeroKind (97.7) become exact, and every
 *   candidate gains or holds: menuCBBiosCopyHero 79.3 -> 83.6, menuCBBiosCreateTrainerHero
 *   67.2 -> 71.7, savedataBattleFindEntryByPlayer 90.5 -> 94.0, savedataBattleGetRulePtr 94.4 -> 94.7,
 *   menuColosseumBattleSetup 85.3 (GC/2.0) -> 93.9.
 */
#define MENU_MIDDLE_RESIDUAL_8006AF44_ONLY
#include "menu_middle.c"
