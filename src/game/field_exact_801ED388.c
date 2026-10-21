/** Exact nursery callback registration, 0x801ED388 - 0x801ED3B8. */
#include "dolphin/types.h"

extern void darkPokemonStepCallback(void);
extern s32 heroMoveAddStepCallback(void (*callback)(void), s32 arg);
extern u32 lbl_8047B5B8;

void darkPokemonStepInit(void)
{
    lbl_8047B5B8 = heroMoveAddStepCallback(darkPokemonStepCallback, 0);
}
