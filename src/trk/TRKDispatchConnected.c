#include "dolphin/types.h"

extern u32 lbl_803FE7D0[];

void SetTRKConnected(u32 state) {
    lbl_803FE7D0[0] = state;
}

u32 GetTRKConnected(void) {
    return lbl_803FE7D0[0];
}
