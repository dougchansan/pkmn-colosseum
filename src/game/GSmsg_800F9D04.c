/*
 * Linked GSmsg object: .text 0x800F9D04-0x800FE35C (GScharMakeFromSJIS
 * through _msgGetSize), the TU's .bss 0x80401DE0-0x80402518 and its .sdata2
 * literal pool 0x8047CD00-0x8047CD50. GSmsgGetText (linked by itself) and
 * GScharToGBA/GScharFromGBA precede it in the TU. Built with GC/1.3.2 and
 * -opt nopeephole; see gs_msg.c for why 1.3.2.
 */
#define GS_MSG_POOL_OBJECT
#include "src/game/gs_msg.c"
