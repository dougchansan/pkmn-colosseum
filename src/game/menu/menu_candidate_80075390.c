/*
 * Linked carve of the menu range bucket: fn_80075390 .. menuToolBattleRun
 * (0x80075390-0x80075A34), the Pokemon preview model and battle-menu
 * driver. Owns its float pool (.sdata2 0x8047C098-0x8047C0C0: 180, 40,
 * 255, pi/25, 0, 1 and the two int->float biases).
 */
#define MENU_80075390_ONLY
#include "src/game/menu/menu_range_8007109C.c"
