/**
 * @file fight_range_8021B910.c
 * @brief wsStatBuffChange, 0x8021B910 - 0x8021C034, and its jump table
 *        (.data 0x8039A048 - 0x8039A068).
 *
 * FIGHT_RANGE_8021B910_ONLY compiles only wsStatBuffChange from the shared body,
 * so the unit emits just that function and its own table.
 */
#define FIGHT_RANGE_8021B910_ONLY
#include "src/game/fight_range_80211A00.c"
