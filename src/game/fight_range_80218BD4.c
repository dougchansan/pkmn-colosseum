/**
 * @file fight_range_80218BD4.c
 * @brief Fight-sequence functions, 0x80218BD4 - 0x80218D24.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u32 lbl_8047B618;

extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
extern u8 fightOutPokemonIsPokemonJoutaiDataId();
extern void fightOutPokemonWritePokemonJoutaiDataId();
extern s8 fn_802025B8();
extern int fightCheckAccuracyRules();
extern u32 fightOutPokemonGetUseWazaDataId();

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimize_for_size on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size on
static inline u32 fightGetDefenderContext(void) {
    return fightTargetGetPtrAsNowFightType(0x12, 0);
}

void wsCmdAB(void) {
    u32 sub1 = fightTargetGetPtrAsNowFightType(
        2, fightTargetGetPtrAsNowFightType(0x11, 0));
    u32 ctx2 = fightGetDefenderContext();
    u32 sub2 = fightTargetGetPtrAsNowFightType(2, ctx2);

    if (fightOutPokemonIsPokemonJoutaiDataId(ctx2, 0x15) == 1 && sub1 != sub2 &&
        (lbl_8047B618 & 0x1000000) == 0) {
        lbl_8047B618 |= 0x40;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimize_for_size reset; optimize_for_size on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size reset

#pragma optimize_for_size on
void WS_MICHIDURE(void) {
    u32 context;
    u32 move;
    u32 target;
    u8 result;

    context = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = fightOutPokemonGetUseWazaDataId((void*)context);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    result = fightCheckAccuracyRules(move, context, target);
    if (result == 1) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        result = fn_802025B8(context, 0x15);
        if (result == 2) {
            fightOutPokemonWritePokemonJoutaiDataId(context, 0x15, 0);
        }
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}
#pragma optimize_for_size reset
