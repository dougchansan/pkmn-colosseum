#include "dolphin/types.h"

extern u8 lbl_803FED70[];

u8 TRKCommGetState(void) {
    return lbl_803FED70[0];
}

void TRKCommSetState(u8 state) {
    lbl_803FED70[0] = state;
}
