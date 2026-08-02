/** Exact standalone owner for 0x801D142C - 0x801D1470. */
#include "game/battle/battle_waza_types.h"

u32 fn_801D142C(s32 idx) {
    WazaEntry* entry;

    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) {
        return 0xFF;
    }
    return *(u8*)((u8*)entry + 1);
}
