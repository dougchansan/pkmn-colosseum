/** Exact standalone owner for 0x80014118 - 0x80014198. */
#include "dolphin/types.h"

extern s32 fightTrainer_GetFightPokemonPtr(u32, u16);
extern s32 fightPokemonGetPokemonPtr(void);
extern s32 heroGetStatus(u32, s32, u16);
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F4;
extern u32 lbl_8047A2F8;

void fn_80014118(s32 arg, s32* out1, s32* out2) {
    s32 val;
    s32 other;

    if ((s32)lbl_8047A2E0 == 1) {
        other = fightTrainer_GetFightPokemonPtr(lbl_8047A2F4, (u16)arg);
        val = fightPokemonGetPokemonPtr();
    } else {
        val = heroGetStatus(lbl_8047A2F8, 3, (u16)arg);
        other = 0;
    }
    *out1 = val;
    *out2 = other;
}
