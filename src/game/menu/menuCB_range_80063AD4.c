/* Residual menu callback, 0x80063AD4 - 0x80063D10. */
#include "dolphin/types.h"

/* The two gradient colours menuTaisenDrawTextBox copies: the first objects of its
 * .sdata2 pool (0x8047BFC8, 0x8047BFCC).
 * RULE-EXCEPTION(user-approved): defined non-const in .sdata2 ahead of the
 * code, so MWCC places them first and the code loads them (a const of known
 * value would become immediates) - see docs/RULE_EXCEPTIONS.md */
#pragma section ".sdata2"
__declspec(section ".sdata2") u32 lbl_8047BFC8 = 0x213A44F2;
__declspec(section ".sdata2") u32 lbl_8047BFCC = 0x11272BF2;

#define MENUCB_RANGE_HEAD_SUFFIX_ONLY
#include "menuCB_range_80062948.c"
