/** Candidate-only owner for 0x80089030 - 0x80089048. */
#include "dolphin/types.h"

#pragma push
#pragma peephole off
void fn_80089030(u8 x) {
    extern u8 lbl_80478958;

    lbl_80478958 = (x == 0);
}
#pragma pop
