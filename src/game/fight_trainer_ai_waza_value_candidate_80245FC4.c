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
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
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

/* =========================================================================
 * fightTrainerAiWazaValueHimitunotikara - MultiBattleSetup
 *
 * Handles multi-round battle encounters (0x1084 = 4228 bytes).
 *
 * The function shows a clear pattern of setting up multiple battle
 * rounds in sequence, checking after each round whether additional
 * rounds are needed:
 *
 *   for each potential round:
 *     if (CheckTrainerPokemonFlag(slot, slot2, roundCheckFlag)):
 *       PreBattleSetup(ctx, slot, roundSeqId)
 *       GetTrainerPokemonPtr(pokemonCtx) -> ptr
 *       BattleSequenceStart(0xEC64, slot, ptr, 0, 0, ctxHandle, 0, seqId)
 *
 * Round check flags: 0x18 (round 1), 0x1E (round 2), 0x07 (round 3)
 * ========================================================================= */
/* TODO: Decompile fightTrainerAiWazaValueHimitunotikara (4228 bytes) */
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void tikeiDataBiosGetFightKoukaId();
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetStatus();
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802358AC();
    extern void fn_80235910();
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_802384B4();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fightTrainerAiAddValue();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    extern void fightTrainerAiWazaValueTuikaHirumi();
    extern u8 jumptable_8039A5D8[];
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;

    r31 = r3;
    r29 = r4;
    r30 = r5;
    r28 = r6;
    r24 = 0x0;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0xf;
    r6 = 0x0;
    fightFloorGetStatus();
    r3 = r3 & 0xFFFF;
    tikeiDataBiosGetFightKoukaId();
    r0 = r3 & 0xFF;
    if (r0 > (u32)0x1b) { r3 = r24; return; }
    r3 = (u32)jumptable_8039A5D8;
    r0 = r0 << 2;
    r3 = (u32)jumptable_8039A5D8;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r25 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x113;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r25 = (s32)r25 / (s32)r3;
    r3 = 0x0;
    r4 = r25;
    fightTrainerAiAddValue();
    r26 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x113;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x114;
        fn_80239984();
        r26 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x114;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x11;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0x115;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r26;
        r4 = r24;
        fightTrainerAiAddValue();
        r26 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x115;
        fn_80239CCC();
            }
    r24 = r26;
    r3 = r24;
    return;
    r4 = r31;
    r5 = (u32)sp + 0x10;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r26 = r3;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x10d;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x10d;
    fn_80239CCC();
    r25 = (u32)sp + 0x10;
    r26 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r3 = r28;
        r4 = 0x0;
        r5 = 0xd5;
        r6 = 0x0;
        ((void(*)(void))pokemonGetStatus)();
        r4 = *(u32*)(r25 + r0);
        if (r3 != (u32)r4) {
            r3 = r31;
            r5 = 0x8;
            fn_802384B4();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r31;
                r4 = r30;
                fn_80239564();
                r24 = r3 & 0xFF;
                r3 = 0x0;
                r4 = 0x10e;
                r5 = 0x3e;
                r6 = 0x0;
                fightTrainerGetStatus();
                r24 = (s32)r24 / (s32)r3;
                r3 = r27;
                r4 = r24;
                fightTrainerAiAddValue();
                r27 = r3;
                r3 = r29;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r31;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x10e;
                fn_80239CCC();
                break;
        }
        }
        r24 = r24 + 0x1;

    }

    r3 = r31;
    r4 = r28;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r26) {
        r3 = r31;
        r4 = r28;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0x10f;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x10f;
        fn_80239CCC();
            }
    r24 = r27;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xdb;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xdb;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xdc;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xdc;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_802358AC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xdd;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xdd;
        fn_80239CCC();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r31;
                r4 = r28;
                r5 = 0x33;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
        }
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xde;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xde;
        fn_80239CCC();
                }
    r24 = r27;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xc3;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc3;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc4;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc4;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_80235A3C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xc5;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc5;
        fn_80239CCC();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xc6;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc6;
        fn_80239CCC();
            }
    r24 = r27;
    r3 = r24;
    return;
    r4 = r31;
    r3 = 0x0;
    r5 = 0xbf;
    fn_80239984();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xbf;
    fn_80239EE8();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc0;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc0;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_80235AA0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc1;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc1;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r31;
                r4 = r28;
                r5 = 0x34;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
        }
        }
        }
        r3 = r27;
        r4 = r31;
        r5 = 0xc2;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc2;
        fn_80239EE8();
                }
    r24 = r27;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xc7;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc7;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc8;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc8;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc9;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc9;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_80235910();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xca;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xca;
        fn_80239CCC();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xcb;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xcb;
        fn_80239CCC();
            }
    r24 = r27;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xe3;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r27 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xe3;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xe4;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe4;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xe5;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe5;
        fn_80239CCC();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x13;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xe6;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fightTrainerAiAddValue();
        r27 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe6;
        fn_80239CCC();
        }
    r24 = r27;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r29;
    r5 = r30;
    r6 = r28;
    fightTrainerAiWazaValueTuikaHirumi();
    r24 = r3;
    r3 = r24;
    return;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x104;
    r5 = 0x3e;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fightTrainerAiAddValue();
    r25 = r3;
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x104;
    fn_80239CCC();
    r4 = r31;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r25;
        r4 = r31;
        r5 = 0x105;
        fn_80239984();
        r25 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x105;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r28;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
        }
        }
        r3 = r31;
        r4 = r30;
        fn_80239564();
        r24 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0x106;
        r5 = 0x3e;
        r6 = 0x0;
        fightTrainerGetStatus();
        r24 = (s32)r24 / (s32)r3;
        r3 = r25;
        r4 = r24;
        fightTrainerAiAddValue();
        r25 = r3;
        r3 = r29;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x106;
        fn_80239CCC();
            }
    r24 = r25;

    r3 = r24;
    return;
}
