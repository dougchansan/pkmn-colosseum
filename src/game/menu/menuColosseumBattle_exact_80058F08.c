/**
 * @file menuColosseumBattle_exact_80058F08.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x80058F08 - 0x80059034.
 */
#include "dolphin/types.h"

typedef struct ColosseumRosterRow {
    s32 mode;
    s32 menuId;
    s32 partyIndex;
    s32 action;
} ColosseumRosterRow;

extern u8 pokemonCheckValid(void*);
extern const ColosseumRosterRow lbl_802677D0[7];

/* Address: 0x80058F08 | Size: 0x38 */
s32 fn_80058F08(s32* partyIndex, s32 cursor) {
    const ColosseumRosterRow* row;

    if (cursor < 0 || cursor >= 7) {
        return 2;
    }

    row = &lbl_802677D0[cursor];
    *partyIndex = row->partyIndex;
    return row->menuId;
}

/* Address: 0x80058F40 | Size: 0xF4 */
#pragma push
#pragma peephole off
void* fn_80058F40(void* dflt, s32 dir, s32* state) {
    extern void* heroGetStatus(void*, s32, u16);
    void* hero;
    s32 i;

    if (state[0] != 0) {
        return dflt;
    }
    if (dir == 1) {
        for (i = state[1] - 1; i >= 0; i--) {
            u32 v;
            hero = heroGetStatus(NULL, 3, (u16)i);
            v = pokemonCheckValid(hero);
            if (v != 0) {
                break;
            }
        }
        if (i >= 0) {
            state[1] = i;
            return hero;
        }
    }
    if (dir == 2) {
        for (i = state[1] + 1; i < 6; i++) {
            u32 v;
            hero = heroGetStatus(NULL, 3, (u16)i);
            v = pokemonCheckValid(hero);
            if (v != 0) {
                break;
            }
        }
        if (i < 6) {
            state[1] = i;
            return hero;
        }
    }
    return dflt;
}
#pragma pop
