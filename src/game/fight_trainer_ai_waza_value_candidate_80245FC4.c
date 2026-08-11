/* Score instrumentation only; not evidence of a retail TU boundary. */
/**
 * @file fight_trainer_ai_waza_value_candidate_80245FC4.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x80245FC4 - 0x80247048.
 *
 * Physically split from fight_trainer_ai_waza_value.c so this
 * translation unit owns only the functions in the stated range.
 */
#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void fn_801EF8F4();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void fn_80165A20();     /* Fade out music */
extern void fn_801659FC();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
/* Declared with a full-word return: retail keeps the count unmasked in a
 * register (mr) and only narrows it with a clrlwi right before the loop.
 * A u16 return type would sink that mask into the assignment instead. */
u32 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fn_80237310(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, u32, u32);
u8 fn_80239564(void*, u32);
u8 fn_80235B04(void*, u32, u32);
u16 fn_80238980(void*, u32);
u8 fn_80238E30(void*, u32, u32);
u8 fn_80239058(void*, u32, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);

/*
 * fightTrainerAiWazaValueHimitunotikara - 0x80245FC4, size 0x1084.
 *
 * Secret Power: the move's effect depends on the terrain, so the whole body is
 * a switch on tikeiDataBiosGetFightKoukaId() over the 28-entry table at
 * jumptable_8039A5D8. Each arm scores one terrain effect the same way:
 *
 *   v     = fn_80239564(ctx, param2) / fightTrainerGetStatus(0, id, 0x3E, 0)
 *   score = fightTrainerAiAddValue(score, v)   + fn_80239CCC(..., id, v)
 *
 * or, where there is no weight to divide by, the flat
 *
 *   score = fn_80239984(score, ctx, id)        + fn_80239EE8(..., id)
 *
 * The two emitters differ only in that fn_80239CCC takes the divided delta as
 * a ninth (stack) argument.
 */
/* Per-function codegen settings: this range was built with the scheduler and
 * the peephole pass on at -O4.  Measured on the finished body: no pragmas
 * 81.48, +scheduling 96.98, +peephole 97.67, +optimize_for_size off 97.83.
 * Wrapped in push/pop so nothing leaks into the functions that follow. */
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole on
#pragma optimize_for_size off
u32 fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 tikeiDataBiosGetFightKoukaId(u32);
    extern u8 fn_802358AC(void*, u32);
    extern u8 fn_80235910(void*, u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_8024AFC4(void*, u32, u32, u32);
    u32 pokemonAry[24];
    /* Local set and declaration order are both load-bearing.  Six locals with
     * non-overlapping lifetimes still colour into the eight callee-saved
     * registers retail uses (r24-r31, frame 0x90); folding the scratch `t` and
     * the loop counter `i` back into `value` costs 0.8pp, and declaring `value`
     * last is what drops the result into r24 instead of r30 (+0.15pp). */
    s32 t;
    s32 score;
    u32* p;
    s32 i;
    u32 count;
    s32 value;

    value = 0;
    switch ((u8)tikeiDataBiosGetFightKoukaId(
                fightFloorGetStatus(0, 0, 0xF, 0) & 0xFFFF)) {
    case 2:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x113, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x113, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0x114);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x114);
        }
        if (fn_80237310(ctx, param3) == 0 ||
            fn_80237F74(ctx, param3, 0x11) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0x115, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x115,
                        t);
        }
        value = score;
        break;

    case 1:
        count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemonAry, 0, 1);
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x10D, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10D, t);

        p = pokemonAry;
        count &= 0xFFFF;
        for (i = 0; (u16)i < count; i++) {
            /* Call through the file-scope `extern void* pokemonGetStatus();`
             * directly: casting it to a function-pointer type first makes MWCC
             * hoist the address into a register and call it via mtctr/bctrl,
             * which costs a callee-saved register on top of the wrong opcode. */
            if ((u32)pokemonGetStatus(param3, 0, 0xD5, 0) == p[(u16)i]) {
                continue;
            }
            if (fn_802384B4(ctx, p[(u16)i], 8) != 1) {
                continue;
            }
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0x10E, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10E,
                        t);
            break;
        }

        if (fn_80237310(ctx, param3) == 0 ||
            fn_80237F74(ctx, param3, 0x0F) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0x10F, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10F,
                        t);
        }
        value = score;
        break;

    case 27:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xDB, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDB, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0xDC);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDC);
        }
        if (fn_802358AC(ctx, param3) == 0) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xDD, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDD,
                        t);
        }
        if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1 ||
            fn_80237F74(ctx, param3, 0x49) == 1 ||
            fn_80237F74(ctx, param3, 0x33) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xDE, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDE,
                        t);
        }
        value = score;
        break;

    case 23:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xC3, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC3, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0xC4);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC4);
        }
        if (fn_80235A3C(ctx, param3) == 0) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xC5, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC5,
                        t);
        }
        if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1 ||
            fn_80237F74(ctx, param3, 0x49) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xC6, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC6,
                        t);
        }
        value = score;
        break;

    case 22:
        score = fn_80239984(0, ctx, 0xBF);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xBF);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0xC0);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC0);
        }
        if (fn_80235AA0(ctx, param3) == 0) {
            score = fn_80239984(score, ctx, 0xC1);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC1);
        }
        if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1 ||
            fn_80237F74(ctx, param3, 0x49) == 1 ||
            fn_80237F74(ctx, param3, 0x34) == 1) {
            score = fn_80239984(score, ctx, 0xC2);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC2);
        }
        value = score;
        break;

    case 24:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xC7, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC7, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0xC8);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC8);
        }
        if (fn_80236BFC(ctx, param3, 5) == 1) {
            score = fn_80239984(score, ctx, 0xC9);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC9);
        }
        if (fn_80235910(ctx, param3) == 0) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xCA, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xCA,
                        t);
        }
        if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1 ||
            fn_80237F74(ctx, param3, 0x49) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xCB, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xCB,
                        t);
        }
        value = score;
        break;

    case 7:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xE3, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE3, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0xE4);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE4);
        }
        if (fn_80236BFC(ctx, param3, 9) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xE5, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE5,
                        t);
        }
        if (fn_80237F74(ctx, param3, 0x13) == 1 ||
            fn_80237F74(ctx, param3, 0x14) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0xE6, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE6,
                        t);
        }
        value = score;
        break;

    case 8:
        value = fn_8024AFC4(ctx, param1, param2, param3);
        break;

    case 5:
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x104, 0x3E, 0);
        score = fightTrainerAiAddValue(0, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x104, t);
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, ctx, 1, 1, 0x10E, param1) == 1) {
            score = fn_80239984(score, ctx, 0x105);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x105);
        }
        if (fn_80237310(ctx, param3) == 0 ||
            fn_80237F74(ctx, param3, 0x07) == 1 ||
            fn_80237F74(ctx, param3, 0x13) == 1) {
            t = fn_80239564(ctx, param2);
            t /= fightTrainerGetStatus(0, 0x106, 0x3E, 0);
            score = fightTrainerAiAddValue(score, t);
            fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x106,
                        t);
        }
        value = score;
        break;
    }

    return value;
}
#pragma pop
