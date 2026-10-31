/** Candidate-only owner for 0x80089030 - 0x80089048. */
#include "dolphin/types.h"

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void gbaSetConnectionMode(u8 x) {
    extern u8 lbl_80478958;

    lbl_80478958 = (x == 0);
}
#pragma pop
