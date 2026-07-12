/**
 * @file fight_trainer_ai_waza_value.c
 * @brief game/pxdvs/app/fight/fightTrainerAiWazaValue.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x802405C0-0x8024E578, 104 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainerAiWazaValue.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
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
 * fightTrainerAiWazaValueKuroikiri - BattleOrchestrator
 *
 * Large battle orchestration function (0xA90 = 2704 bytes).
 *
 * Manages the complete flow of a battle encounter:
 *   1. Pre-battle: Load opponent data, validate teams
 *   2. Initiation: Call BattleSequenceStart with appropriate IDs
 *   3. Monitoring: Check battle status and handle interrupts
 *   4. Completion: Process results and update state
 *
 * This function acts as the bridge between the script system
 * (colosseum_script.c) and the battle engine (battle_main.c).
 * ========================================================================= */
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_80235A3C(void* ctx, u32 elem);
    extern u8 fn_802359D8(void* ctx, u32 elem);
    extern u8 fn_80235974(void* ctx, u32 elem);
    extern u8 fn_80235910(void* ctx, u32 elem);
    extern u8 fn_802358AC(void* ctx, u32 elem);
    extern u8 fn_802357CC(void* ctx, u32 elem);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(int a, void* ctx, u32* buf, int b, int c);
    extern int fn_80239984(int handle, void* ctx, int seq);
    extern u32 fightOutPokemonGetPokemonPtr(u32 v);
    extern void fn_80239EE8(int a, void* ctx, u32 v, int b, int c, u32 d, int e, int seq);
    u32 array1[10];
    u32 array2[8];
    u8 bufA[8];
    u8 bufB[8];
    u8 bufC[8];
    u8 bufD[8];
    u8 bufE[8];
    u8 bufF[8];
    u8 bufG[8];
    u8 bufH[8];
    u16 count1;
    u16 count2;
    int handle;
    u32 elem;
    u16 i;
    u8 j;

    handle = 0;
    count1 = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, array1, 1, 1);
    count2 = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, array2, 0, 1);

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count2; i++) {
            elem = array2[i];
            bufA[0] = fn_80235AA0(ctx, elem);
            bufA[1] = fn_80235A3C(ctx, elem);
            bufA[2] = fn_802359D8(ctx, elem);
            bufA[3] = fn_80235974(ctx, elem);
            bufA[4] = fn_80235910(ctx, elem);
            bufA[5] = fn_802358AC(ctx, elem);
            bufA[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufA[j] >= 8 && bufA[j] <= 9) { matched = 1; goto M0; }
            }
            matched = 0;
        M0:
            if (matched == 1) { found = 1; goto L0; }
        }
        found = 0;
    L0:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1b6);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b6);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count1; i++) {
            elem = array1[i];
            bufB[0] = fn_80235AA0(ctx, elem);
            bufB[1] = fn_80235A3C(ctx, elem);
            bufB[2] = fn_802359D8(ctx, elem);
            bufB[3] = fn_80235974(ctx, elem);
            bufB[4] = fn_80235910(ctx, elem);
            bufB[5] = fn_802358AC(ctx, elem);
            bufB[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufB[j] >= 3 && bufB[j] <= 4) { matched = 1; goto M1; }
            }
            matched = 0;
        M1:
            if (matched == 1) { found = 1; goto L1; }
        }
        found = 0;
    L1:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1b7);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b7);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count2; i++) {
            elem = array2[i];
            bufC[0] = fn_80235AA0(ctx, elem);
            bufC[1] = fn_80235A3C(ctx, elem);
            bufC[2] = fn_802359D8(ctx, elem);
            bufC[3] = fn_80235974(ctx, elem);
            bufC[4] = fn_80235910(ctx, elem);
            bufC[5] = fn_802358AC(ctx, elem);
            bufC[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufC[j] >= 10 && bufC[j] <= 12) { matched = 1; goto M2; }
            }
            matched = 0;
        M2:
            if (matched == 1) { found = 1; goto L2; }
        }
        found = 0;
    L2:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1b8);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b8);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count1; i++) {
            elem = array1[i];
            bufD[0] = fn_80235AA0(ctx, elem);
            bufD[1] = fn_80235A3C(ctx, elem);
            bufD[2] = fn_802359D8(ctx, elem);
            bufD[3] = fn_80235974(ctx, elem);
            bufD[4] = fn_80235910(ctx, elem);
            bufD[5] = fn_802358AC(ctx, elem);
            bufD[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if ((s32)(u8)bufD[j] >= 0 && (s32)(u8)bufD[j] <= 2) { matched = 1; goto M3; }
            }
            matched = 0;
        M3:
            if (matched == 1) { found = 1; goto L3; }
        }
        found = 0;
    L3:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1b9);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b9);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count1; i++) {
            elem = array1[i];
            bufE[0] = fn_80235AA0(ctx, elem);
            bufE[1] = fn_80235A3C(ctx, elem);
            bufE[2] = fn_802359D8(ctx, elem);
            bufE[3] = fn_80235974(ctx, elem);
            bufE[4] = fn_80235910(ctx, elem);
            bufE[5] = fn_802358AC(ctx, elem);
            bufE[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufE[j] >= 8 && bufE[j] <= 9) { matched = 1; goto M4; }
            }
            matched = 0;
        M4:
            if (matched == 1) { found = 1; goto L4; }
        }
        found = 0;
    L4:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1ba);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ba);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count2; i++) {
            elem = array2[i];
            bufF[0] = fn_80235AA0(ctx, elem);
            bufF[1] = fn_80235A3C(ctx, elem);
            bufF[2] = fn_802359D8(ctx, elem);
            bufF[3] = fn_80235974(ctx, elem);
            bufF[4] = fn_80235910(ctx, elem);
            bufF[5] = fn_802358AC(ctx, elem);
            bufF[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufF[j] >= 3 && bufF[j] <= 4) { matched = 1; goto M5; }
            }
            matched = 0;
        M5:
            if (matched == 1) { found = 1; goto L5; }
        }
        found = 0;
    L5:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1bb);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1bb);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count1; i++) {
            elem = array1[i];
            bufG[0] = fn_80235AA0(ctx, elem);
            bufG[1] = fn_80235A3C(ctx, elem);
            bufG[2] = fn_802359D8(ctx, elem);
            bufG[3] = fn_80235974(ctx, elem);
            bufG[4] = fn_80235910(ctx, elem);
            bufG[5] = fn_802358AC(ctx, elem);
            bufG[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if (bufG[j] >= 10 && bufG[j] <= 12) { matched = 1; goto M6; }
            }
            matched = 0;
        M6:
            if (matched == 1) { found = 1; goto L6; }
        }
        found = 0;
    L6:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1bc);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1bc);
        }
    }

    {
        u8 found;
        u8 matched;
        for (i = 0; i < count2; i++) {
            elem = array2[i];
            bufH[0] = fn_80235AA0(ctx, elem);
            bufH[1] = fn_80235A3C(ctx, elem);
            bufH[2] = fn_802359D8(ctx, elem);
            bufH[3] = fn_80235974(ctx, elem);
            bufH[4] = fn_80235910(ctx, elem);
            bufH[5] = fn_802358AC(ctx, elem);
            bufH[6] = fn_802357CC(ctx, elem);
            for (j = 0; j < 7; j++) {
                if ((s32)(u8)bufH[j] >= 0 && (s32)(u8)bufH[j] <= 2) { matched = 1; goto M7; }
            }
            matched = 0;
        M7:
            if (matched == 1) { found = 1; goto L7; }
        }
        found = 0;
    L7:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1bd);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1bd);
        }
    }
    return handle;
}

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

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 120 functions matched
 * =================================================================== */

/* Address: 0x8024E52C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaValueNull(void) { return 0; }

/* ===================================================================
 * EXPANDED FUNCTION COVERAGE
 * 560 additional functions for 0x80240000-0x80266360
 * =================================================================== */

/* -------------------------------------------------------------------
 * Battle Orchestration (0x80240000-0x8024D000)
 * 92 functions
 * ------------------------------------------------------------------- */

/* Address: 0x802400D8 | Size: 0x6C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueOomugaesi(void* ctx, u32 slot, u16 species, u32 extra) {
    extern u32 fn_8023CA9C();
    extern u16 fightTrainerAiCheckOumu();
    u16 currentSpecies;
    currentSpecies = fightTrainerAiCheckOumu(ctx);
    if (currentSpecies == species || currentSpecies == 0) {
        return 0;
    }
    return fn_8023CA9C(ctx, slot, currentSpecies, extra);
}

/* Address: 0x80240144 | Size: 0xAC */
void fightTrainerAiWazaValueRandamuSentaku(void* ctx, u32 param1, u32 param2) {
    extern void fn_800E0C54();
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValue();
    extern void fn_80239CCC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r6 = 0x0;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r3 = 0x0;
    r4 = 0x1cb;
    r5 = 0x3e;
    fightTrainerGetStatus();
    r31 = r3;
    fn_800E0C54();
    r5 = r3 & 0xFFFF;
    r4 = r31 + 0x1;
    r0 = (s32)r5 / (s32)r4;
    r3 = 0x0;
    r0 = r0 * r4;
    r30 = r5 - r0;
    r4 = r30;
    fightTrainerAiAddValue();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x1cb;
    fn_80239CCC();
    r3 = r31;
    return;
}

/* Address: 0x802401F0 | Size: 0x264 (612 bytes) */
void fightTrainerAiWazaValueHurahuradansu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightFloorGetStatus();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80237F74();
    extern void fn_8023831C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x50];
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

    r6 = 0x1;
    r7 = 0x1;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r5 = (u32)sp + 0x8;
    r4 = r27;
    r30 = 0x0;
    r3 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r31 = r3;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x18;
    r6 = 0x0;
    fightFloorGetStatus();
    r0 = r3 & 0xFFFF;
    if (r0 >= (u32)0x2) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x1c7;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1c7;
        fn_80239EE8();
    }
    r26 = (u32)sp + 0x8;
    r25 = r31 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r4 = *(u32*)(r26 + r0);
        if (r28 != (u32)r4) {
            r3 = r27;
            fn_8023831C();
            r0 = r3 & 0xFFFF;

            if (r0 == (u32)0x8 || r0 == (u32)0x9) {

                r3 = r30;
                r4 = r27;
                r5 = 0x1c8;
                fn_80239984();
                r0 = r3;
                r3 = r28;
                r30 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r27;
                r8 = r29;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1c8;
                fn_80239EE8();
                break;
        }
        }
        r24 = r24 + 0x1;

    }

    r25 = (u32)sp + 0x8;
    r26 = r31 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r4 = *(u32*)(r25 + r0);
        if (r28 != (u32)r4) {
            r3 = r27;
            r5 = 0x14;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r30;
                r4 = r27;
                r5 = 0x1c9;
                fn_80239984();
                r0 = r3;
                r3 = r28;
                r30 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r27;
                r8 = r29;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1c9;
                fn_80239EE8();
                break;
        }
        }
        r24 = r24 + 0x1;

    }

    r4 = (u32)sp + 0x8;
    r0 = r31 & 0xFFFF;
    r5 = 0x0;
    while (1) {
        r3 = r5 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u32*)(r4 + r3);
        if (r28 != (u32)r3) {
            r3 = r30;
            r4 = r27;
            r5 = 0x1ca;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1ca;
            fn_80239EE8();
            r3 = r30;
            return;
        }
        r5 = r5 + 0x1;

    }

    r3 = r30;
    return;
}

/* Address: 0x80240454 | Size: 0x16C (364 bytes) */
void fightTrainerAiWazaValueMakibisi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightTargetGetPtrAsNowFightType();
    extern void fightSideGetCountAsJoutaiDataId();
    extern void fightSideIsJoutaiDataId();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r3;
    r27 = r4;
    r31 = r5;
    r4 = r6;
    r29 = 0x0;
    r3 = 0x2;
    fightTargetGetPtrAsNowFightType();
    r4 = 0x4a;
    r28 = r3;
    fightSideIsJoutaiDataId();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = 0x4a;
        fightSideGetCountAsJoutaiDataId();
    } else {

        r3 = 0x0;
    }
    r0 = (s16)r3;
    if (r0 == (u32)0x1) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1c4;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1c4;
        fn_80239EE8();
        r3 = r29;
        return;
    }
    r0 = (s16)r3;
    if ((s32)r0 == (s32)0x1) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1c5;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1c5;
        fn_80239EE8();
        r3 = r29;
        return;
    }
    if ((s32)r0 != (s32)0x2) { r3 = r29; return; }
    r4 = r30;
    r3 = 0x0;
    r5 = 0x1c6;
    fn_80239984();
    r0 = r3;
    r3 = r27;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r30;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x1c6;
    fn_80239EE8();

    r3 = r29;
    return;
}

/* Address: 0x802405C0 | Size: 0x8C */
u32 fightTrainerAiWazaValueRisaikuru(void* ctx, u32 param1, u32 param2) {
#pragma optimize_for_size on
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u16 fn_80236B98(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80236B98(ctx) != 0) {
        u32 tmp = fn_80239984(0, ctx, 0x1c3);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1c3);
    }
    return handle;
}

/* Address: 0x8024064C | Size: 0x13C (316 bytes) */
u32 fightTrainerAiWazaValueSiroikiri(void* ctx, u32 param1, u32 param2, u32 param3) {
#pragma optimize_for_size on
    typedef void (*BattleScriptCallback)();
    extern BattleScriptCallback wazaGetStatus(u32, u16, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u16 fn_80236520(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern void fightTrainerAiWazaValueKusuguruDaun();
    extern void fightTrainerAiWazaValueKaihiDaun();
    extern void fightTrainerAiWazaValueMeityuuDaun();
    extern void fightTrainerAiWazaValueTokubouDaun();
    extern void fightTrainerAiWazaValueBougyoDaun();
    extern void fightTrainerAiWazaValueKougekiDaun();
    extern void fightTrainerAiWazaValueSubayasaDaun();
    extern void fightTrainerAiWazaValueNull();
    BattleScriptCallback callback;
    u32 setup;
    u16 species;

    setup = 0;
    species = fn_80236520(ctx, param3);
    if ((species != 0) && (species != 0xffff) && (species != 0x165) && (species != 0x163)) {
        callback = wazaGetStatus(0, species, 0x1c, 0);
        if (callback == NULL) {
            callback = fightTrainerAiWazaValueNull;
        }
        if ((callback == fightTrainerAiWazaValueSubayasaDaun) || (callback == fightTrainerAiWazaValueKougekiDaun) || (callback == fightTrainerAiWazaValueBougyoDaun)
            || (callback == fightTrainerAiWazaValueTokubouDaun) || (callback == fightTrainerAiWazaValueMeityuuDaun) || (callback == fightTrainerAiWazaValueKaihiDaun)
            || (callback == fightTrainerAiWazaValueKusuguruDaun)) {
            setup = fn_80239984(0, ctx, 0x1c2);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1c2);
        }
    }
    return setup;
}

/* Address: 0x80240788 | Size: 0x448 (1096 bytes) */
void fightTrainerAiWazaValueJikoanji(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802357CC();
    extern void fn_802358AC();
    extern void fn_80235910();
    extern void fn_80235974();
    extern void fn_802359D8();
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r6;
    r28 = r4;
    r27 = r3;
    r29 = r5;
    r4 = r30;
    r31 = 0x0;
    fn_80235AA0();
    *(u8*)(sp + 0x20) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    *(u8*)(sp + 0x21) = r3;
    r3 = r27;
    r4 = r30;
    fn_802359D8();
    *(u8*)(sp + 0x22) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235974();
    *(u8*)(sp + 0x23) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235910();
    *(u8*)(sp + 0x24) = r3;
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    *(u8*)(sp + 0x25) = r3;
    r3 = r27;
    r4 = r30;
    fn_802357CC();
    *(u8*)(sp + 0x26) = r3;
    r3 = (u32)sp + 0x20;
    r4 = 0x0;
    while (1) {
        r0 = r4 & 0xFF;
        if (r0 >= (u32)0x7) break;
        r0 = r4 & 0xFF;
        r0 = *(u8*)(r3 + r0);
        if (r0 >= (u32)0x8 || r0 > (u32)0x9) {

            r0 = 0x1;
            break;
        }
        r4 = r4 + 0x1;

    }
    r0 = 0x0;

    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x1be;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1be;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235AA0();
    *(u8*)(sp + 0x18) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    *(u8*)(sp + 0x19) = r3;
    r3 = r27;
    r4 = r30;
    fn_802359D8();
    *(u8*)(sp + 0x1A) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235974();
    *(u8*)(sp + 0x1B) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235910();
    *(u8*)(sp + 0x1C) = r3;
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    *(u8*)(sp + 0x1D) = r3;
    r3 = r27;
    r4 = r30;
    fn_802357CC();
    *(u8*)(sp + 0x1E) = r3;
    r3 = (u32)sp + 0x18;
    r4 = 0x0;
    while (1) {
        r0 = r4 & 0xFF;
        if (r0 >= (u32)0x7) break;
        r0 = r4 & 0xFF;
        r0 = *(u8*)(r3 + r0);
        if (r0 >= (u32)0xa || r0 > (u32)0xc) {

            r0 = 0x1;
            break;
        }
        r4 = r4 + 0x1;

    }
    r0 = 0x0;

    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1bf;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1bf;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235AA0();
    *(u8*)(sp + 0x10) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    *(u8*)(sp + 0x11) = r3;
    r3 = r27;
    r4 = r30;
    fn_802359D8();
    *(u8*)(sp + 0x12) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235974();
    *(u8*)(sp + 0x13) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235910();
    *(u8*)(sp + 0x14) = r3;
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    *(u8*)(sp + 0x15) = r3;
    r3 = r27;
    r4 = r30;
    fn_802357CC();
    *(u8*)(sp + 0x16) = r3;
    r3 = (u32)sp + 0x10;
    r4 = 0x0;
    while (1) {
        r0 = r4 & 0xFF;
        if (r0 >= (u32)0x7) break;
        r0 = r4 & 0xFF;
        r0 = *(u8*)(r3 + r0);
        if (r0 >= (u32)0x3 || r0 > (u32)0x4) {

            r0 = 0x1;
            break;
        }
        r4 = r4 + 0x1;

    }
    r0 = 0x0;

    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1c0;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1c0;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235AA0();
    *(u8*)(sp + 0x8) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    *(u8*)(sp + 0x9) = r3;
    r3 = r27;
    r4 = r30;
    fn_802359D8();
    *(u8*)(sp + 0xA) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235974();
    *(u8*)(sp + 0xB) = r3;
    r3 = r27;
    r4 = r30;
    fn_80235910();
    *(u8*)(sp + 0xC) = r3;
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    *(u8*)(sp + 0xD) = r3;
    r3 = r27;
    r4 = r30;
    fn_802357CC();
    *(u8*)(sp + 0xE) = r3;
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    while (1) {
        r0 = r4 & 0xFF;
        if (r0 >= (u32)0x7) break;
        r0 = r4 & 0xFF;
        r0 = *(u8*)(r3 + r0);
        if (r0 >= (u32)0x0 || r0 > (u32)0x2) {

            r0 = 0x1;
            break;
        }
        r4 = r4 + 0x1;

    }
    r0 = 0x0;

    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1c1;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1c1;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80241660 | Size: 0x510 (1296 bytes) */
void fightTrainerAiWazaValueTedasuke(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern void fightActionGetKindDataId();
    extern void fightActionCheckValid();
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fn_80202108();
    extern void fn_80202234();
    extern void fightOutPokemonGetUseWazaDataId();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f9 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r6 = 0x1;
    r7 = 0x1;
    r28 = r3;
    r29 = r4;
    r30 = r5;
    r5 = (u32)sp + 0x1c;
    r4 = r28;
    r31 = 0x0;
    r3 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r21 = 0x0;
    r22 = 0x0;
    while (1) {
        r3 = lbl_80478DF8;
        r4 = r22 & 0xFFFF;
        r0 = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= (u32)r0) break;
        r0 = r22 & 0xFFFF;
        if ((s32)r0 != (s32)0) {
            if (r0 != (u32)0x165) {
                if (r0 != (u32)0x163) {
                    r3 = r28;
                    r4 = r22;
                    r5 = 0x1;
                    fn_8023943C();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x163) {
                        r4 = r28;
                        r7 = r22;
                        r3 = 0x0;
                        r5 = 0x1;
                        r6 = 0x1;
                        r8 = 0x0;
                        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r21 = 0x1;
        }
        }
        }
        }
        }
        r22 = r22 + 0x1;

    }
    r0 = r21 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x1b0;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1b0;
        fn_80239EE8();
    }
    r27 = (u32)sp + 0x1c;
    r22 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r27 + r0);
        if (r29 != (u32)r4) {
            r3 = r28;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r25 = r3 & 0xFFFF;
            if (r29 != (u32)r4) {
                r23 = (u32)sp + 0x8;
                r21 = 0x0;
                while (1) {
                    r0 = r21 & 0xFFFF;
                    if (r0 >= (u32)r25) break;
                    r3 = r28;
                    r4 = *(u16*)(r23 + r0);
                    r5 = 0x1;
                    fn_8023943C();
                    r0 = r3 & 0xFF;
                    if (r29 != (u32)r4) {
                        r3 = r31;
                        r4 = r28;
                        r5 = 0x1b1;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r28;
                        r8 = r30;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x1b1;
                        fn_80239EE8();
                        break;
                    }
                    r21 = r21 + 0x1;

                }
        }
        }
        r24 = r24 + 0x1;

    }
    r27 = (u32)sp + 0x1c;
    r23 = r26 & 0xFFFF;
    r25 = 0x1;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r3 = *(u32*)(r27 + r22);
        if (r29 == (u32)r3 || r29 == (u32)r3 || r29 == (u32)r3 || r0 == (u32)0x13) {
            r4 = 0x0;
            r5 = 0xfe;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();

            fightActionCheckValid();
            r0 = r3 & 0xFF;

            r3 = r21;
            fightActionGetKindDataId();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x13) {
                r25 = 0x0;
                break;
            }
            r3 = *(u32*)(r27 + r22);
            fightOutPokemonGetUseWazaDataId();
            r0 = r3;
            r3 = r28;
            r4 = r0;
            r5 = 0x1;
            fn_8023943C();
            r0 = r3 & 0xFF;

            r25 = 0x0;
            break;
        }
        r24 = r24 + 0x1;

    }

    r0 = r25 & 0xFF;
    if (r0 == (u32)r23) {
        r3 = r31;
        r4 = r28;
        r5 = 0x1b2;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1b2;
        fn_80239EE8();
    }
    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r4 = *(u32*)(r25 + r0);
        if (r29 != (u32)r4) {
            r3 = r28;
            r5 = 0x12;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b3;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b3;
                fn_80239EE8();
                break;
        }
        }
        r22 = r22 + 0x1;

    }

    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = *(u32*)(r25 + r0);
        if (r29 != (u32)r3) {
            r4 = 0x0;
            r5 = 0xf9;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = r3 & 0xFF;
            if (r29 != (u32)r3) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b4;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b4;
                fn_80239EE8();
                break;
        }
        }
        r22 = r22 + 0x1;

    }

    r27 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r23 = *(u32*)(r27 + r0);
        if (r29 != (u32)r23) {
            r3 = r28;
            r4 = r23;
            r5 = 0x8;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r29 == (u32)r23) {
                r0 = -0x1;

            } else {
                r3 = r28;
                r4 = r23;
                r5 = 0x30;
                fn_80237F74();
                r0 = r3 & 0xFF;
                r3 = r23;
                r0 = 0x1 - r0;
                r4 = 0x8;
                r0 = __cntlzw(r0);
                r5 = (u32)r0 >> 5;
                r26 = r5 + 0x1;
                fn_80202108();
                r22 = r3 + r26;
                r3 = r23;
                r4 = 0x8;
                fn_80202234();
                r3 = (s8)r3;
                r0 = (s8)r22;
                if ((s32)r0 >= (s32)r3) {
                    r0 = 0x1;
                    goto L_80241AF4;
                }
                r0 = 0x0;
            }
        L_80241AF4:
            r0 = (s8)r0;
            if ((s32)r0 == (s32)r3) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b5;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b5;
                fn_80239EE8();
                r3 = r31;
                return;
        }
        }
        r25 = r25 + 0x1;

    }

    r3 = r31;
    return;
}

/* Address: 0x80241B70 | Size: 0x278 (632 bytes) */
u32 fightTrainerAiWazaValueKyouseikoutai(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u32 fightSideGetCountAsJoutaiDataId(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u8 fn_80235910(void*, u32);
    extern u8 fn_80235974(void*, u32);
    extern u8 fn_802359D8(void*, u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u8 stats[7];
    u32 handle;
    u32 mode;
    u8 found;
    u8 i;

    handle = 0;
    mode = fightTargetGetPtrAsNowFightType(2, param3);
    if (fightSideIsJoutaiDataId(mode, 0x4a) == 1) {
        mode = fightSideGetCountAsJoutaiDataId(mode, 0x4a);
    } else {
        mode = 0;
    }
    stats[0] = fn_80235AA0(ctx, param3);
    stats[1] = fn_80235A3C(ctx, param3);
    stats[2] = fn_802359D8(ctx, param3);
    stats[3] = fn_80235974(ctx, param3);
    stats[4] = fn_80235910(ctx, param3);
    stats[5] = fn_802358AC(ctx, param3);
    stats[6] = fn_802357CC(ctx, param3);

    i = 0;
    goto check_stats;
check_stat_value:
    if (stats[i] >= 8 && stats[i] <= 0xc) {
        found = 1;
        goto done_stats;
    }
    i++;
check_stats:
    if (i < 7) {
        goto check_stat_value;
    }
    found = 0;
done_stats:
    if (found == 1) {
        handle = fn_80239984(0, ctx, 0x1ac);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ac);
    }
    if ((s16)mode == 1) {
        handle = fn_80239984(handle, ctx, 0x1ad);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ad);
    } else if ((s16)mode == 2) {
        handle = fn_80239984(handle, ctx, 0x1ae);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ae);
    } else if ((s16)mode == 3) {
        handle = fn_80239984(handle, ctx, 0x1af);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1af);
    }
    return handle;
}

/* Address: 0x80241DE8 | Size: 0x1FC (508 bytes) */
s32 fightTrainerAiWazaValueYokodori(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fn_802364BC();
    extern u32 fn_80236520();
    extern u16 fn_802377E8();
    extern u8 fn_8023943C();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    u32 buf[8];
    s32 handle = 0;
    u32 r1v;
    u16 r2v;
    u16 count;
    u16 i;
    u32 r3v;

    r1v = fn_80236520(ctx, param1);
    r2v = fn_802364BC(ctx, param1);
    r3v = fn_80236520(ctx, param3);
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf, 0, 1);

    if ((u16)r3v != 0 && (u16)r3v != 0xffff && (u16)r3v != 0x165 && (u16)r3v != 0x163 &&
        fn_8023943C(ctx, r3v, 4) == 1) {
        handle = (s32)fn_80239984(0, ctx, 0x1a9);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a9);
    }

    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, buf[i]);
        if (v == 0x12e || v == 0xd4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1aa);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1aa);
            break;
        }
    }

    if ((u16)r1v == 0x121 || r2v == 0x121) {
        handle = (s32)fn_80239984(handle, ctx, 0x1ab);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ab);
    }
    return handle;
}

/* Address: 0x80241FE4 | Size: 0x2A8 (680 bytes) */
s32 fightTrainerAiWazaValueKonoyubitomare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fn_802377E8();
    extern u8 fn_8023785C();
    extern u16 fn_8023793C();
    extern u16 fn_80237CB8();
    extern s32 fn_80239500();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    u32 listA[8];
    u32 listB[8];
    u16 listC[2];
    s32 handle = 0;
    u16 countA;
    u16 countB;
    u16 countC;
    u8 found;
    u16 i;
    u16 j;
    u16 k;

    countA = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, listA, 1, 1);
    countB = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, listB, 0, 1);

    found = 0;
    for (i = 0; i < countB; i++) {
        countC = fn_80237CB8(ctx, listB[i], listC);
        for (j = 0; j < countC; j++) {
            for (k = 0; k < countA; k++) {
                if (param1 != listA[k] &&
                    fn_8023793C(ctx, listA[k], listC[j], fn_80239500(ctx, param2)) == 0x41) {
                    found = 1;
                }
            }
        }
    }

    if (found == 1) {
        handle = (s32)fn_80239984(0, ctx, 0x1A6);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A6);
    }

    for (i = 0; i < countA; i++) {
        if (param1 != listA[i] && fn_8023785C(ctx, listA[i]) == 2) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A7);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A7);
            break;
        }
    }

    for (i = 0; i < countB; i++) {
        u16 v = fn_802377E8(ctx, listB[i]);
        if (v == 0x12E || v == 0xD4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A8);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A8);
            break;
        }
    }

    return handle;
}

/* Address: 0x8024228C | Size: 0x3BC (956 bytes) */
u32 fightTrainerAiWazaValueKoukanKinsi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x19d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19d);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x19e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19e);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x19f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a0);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a1);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a2);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a3);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a3);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a4);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a5);
    }
    return handle;
}

/* Address: 0x80242648 | Size: 0xE8 (232 bytes) */
u32 fightTrainerAiWazaValueKanasibari(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fn_80236520(ctx, param3);
    if (fn_80236D60(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fn_8023943C(ctx, move, 1) == 1) {
                handle = fn_80239984(0, ctx, 0x19c);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19c);
            }
        }
    }
    return handle;
}

/* Address: 0x80242730 | Size: 0x170 (368 bytes) */
u32 fightTrainerAiWazaValueMitizure(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern const f32 lbl_8047E630;
    extern const f32 lbl_8047E638;
    extern const f32 lbl_8047E63C;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, param1, -1, lbl_8047E638) == 1) {
        handle = fn_80239984(0, ctx, 0x19b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19b);
    } else if (fn_802373B0(ctx, param1, -1, lbl_8047E63C) == 1) {
        handle = fn_80239984(0, ctx, 0x19a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19a);
    } else if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x199);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x199);
    }
    return handle;
}

/* Address: 0x802428A0 | Size: 0x134 (308 bytes) */
u32 fightTrainerAiWazaValueUrami(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_802391E0(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;
    u32 value;

    handle = 0;
    move = fn_80236520(ctx, param3);
    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163 && fn_80236D60(ctx, param1, param3) > 0) {
        value = fn_802391E0(ctx, move);
        if ((value & 0xFF) <= 5) {
            handle = fn_80239984(0, ctx, 0x197);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x197);
        } else if ((value & 0xFF) <= 0xa) {
            handle = fn_80239984(0, ctx, 0x198);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x198);
        }
    }
    return handle;
}

/* Address: 0x802429D4 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueOdareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fn_80236FFC(ctx, param3);
    statCurrent = fn_8023715C(ctx, param3);
    statNext = fn_802370AC(ctx, param3);
    if ((u16)statCurrent > (u16)statLimit) {
        handle = fn_80239984(0, ctx, 0x194);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x194);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fn_80239984(handle, ctx, 0x195);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x195);
    }
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        handle = fn_80239984(handle, ctx, 0x196);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x196);
    }
    return handle;
}

/* Address: 0x80242B50 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueIbaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fn_80236FFC(ctx, param3);
    statCurrent = fn_8023715C(ctx, param3);
    statNext = fn_802370AC(ctx, param3);
    if ((u16)statLimit > (u16)statCurrent) {
        handle = fn_80239984(0, ctx, 0x191);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x191);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fn_80239984(handle, ctx, 0x192);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x192);
    }
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        handle = fn_80239984(handle, ctx, 0x193);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x193);
    }
    return handle;
}

/* Address: 0x80242CCC | Size: 0xE4 (228 bytes) */
u32 fightTrainerAiWazaValueAnkooru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802364BC(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fn_802364BC(ctx, param3);
    if (fn_80236D60(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fn_8023943C(ctx, move, 1) == 0) {
                handle = fn_80239984(0, ctx, 0x190);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x190);
            }
        }
    }
    return handle;
}

/* Address: 0x80242DB0 | Size: 0x9C */
u32 fightTrainerAiWazaValueIkarinomaeba(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern const f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, extra, 1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x18f);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18f);
    }
    return handle;
}

/* Address: 0x80242E4C | Size: 0x1A0 (416 bytes) */
u32 fightTrainerAiWazaValueKusuguruDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u32 fn_80236F4C(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 currentAtk;
    u32 limitAtk;
    u32 currentDef;
    u32 limitDef;
    u32 handle;

    handle = 0;
    currentAtk = fn_8023715C(ctx, param3);
    limitAtk = fn_80236FFC(ctx, param3);
    currentDef = fn_802370AC(ctx, param3);
    limitDef = fn_80236F4C(ctx, param3);
    if ((u16)currentAtk > (u16)limitAtk) {
        handle = fn_80239984(0, ctx, 0x18c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18c);
    }
    if ((u16)currentDef > (u16)limitDef) {
        handle = fn_80239984(handle, ctx, 0x18d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18d);
    }
    if (fn_80235AA0(ctx, param3) <= 4 && fn_80235A3C(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0x18e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18e);
    }
    return handle;
}

/* Address: 0x80242FEC | Size: 0xF8 (248 bytes) */
u32 fightTrainerAiWazaValueKaihiDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_802357CC();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;

    if ((u8)fn_802357CC(ctx, param3) >= 7) {
        handle = fn_80239984(0, ctx, 0x18A);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18A);
    }
    if ((u8)fn_802357CC(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0x18B);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18B);
    }
    return handle;
}

/* Address: 0x802430E4 | Size: 0x94 */
u32 fightTrainerAiWazaValueMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;

    setup = 0;
    if (fn_802358AC(ctx, param3) <= 4U) {
        setup = fn_80239984(0, ctx, 0x189);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x189);
    }
    return setup;
}

/* Address: 0x80243178 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueTokubouDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235974();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count < (u16)limit) {
        handle = fn_80239984(0, arg0, 0x187);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x187);
    }
    if ((u8)fn_80235974(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x188);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x188);
    }
    return handle;
}

/* Address: 0x80243284 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueBougyoDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235A3C();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x185);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x185);
    }
    if ((u8)fn_80235A3C(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x186);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x186);
    }
    return handle;
}

/* Address: 0x80243390 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueKougekiDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_8023715C();
    extern u32 fn_80236FFC();
    extern u32 fn_80235AA0();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_8023715C(arg0, arg3);
    limit = fn_80236FFC(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x183);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x183);
    }
    if ((u8)fn_80235AA0(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x184);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x184);
    }
    return handle;
}

/* Address: 0x8024349C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[10];
    u32 setup;
    u16 count;
    u8 found;
    u16 index;

    found = 0;
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 1, 1);
    index = 0;
    while (index < count) {
        if (fn_80236D60(ctx, param3, entries[index]) > 0) {
            found = 1;
            break;
        }
        index++;
    }

    if (found == 1) {
        setup = fn_80239984(0, ctx, 0x181);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x181);
    } else {
        setup = fn_80239984(0, ctx, 0x182);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x182);
    }
    return setup;
}

/* Address: 0x802435D4 | Size: 0xB8 */
u32 fightTrainerAiWazaValueGamusyara(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802376EC(void*, u32);
    extern u32 fightTrainerAiAddValue(u32, u32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, u32);
    u32 baseHp;
    u32 handle;

    baseHp = fn_802376EC(ctx, param1);
    extra = (s32)(fn_802376EC(ctx, extra) & 0xFFFF) / (s32)(baseHp & 0xFFFF);
    extra *= fightTrainerGetStatus(0, 0x180, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, extra);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x180, extra);
    return handle;
}

/* Address: 0x8024368C | Size: 0x1AC (428 bytes) */
u32 fightTrainerAiWazaValueItamiwake(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fn_802387C8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 ownHp;
    s32 targetHp;
    u32 handle;

    handle = 0;
    ownHp = fn_802387C8(ctx, param1);
    targetHp = fn_802387C8(ctx, param3);
    if (ownHp * 3 <= targetHp) {
        handle = fn_80239984(0, ctx, 0x17d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17d);
    } else if (ownHp * 2 <= targetHp) {
        handle = fn_80239984(0, ctx, 0x17c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17c);
    } else if (ownHp >= targetHp * 3) {
        handle = fn_80239984(0, ctx, 0x17f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17f);
    } else if (ownHp >= targetHp * 2) {
        handle = fn_80239984(0, ctx, 0x17e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17e);
    }
    return handle;
}

/* Address: 0x80243838 | Size: 0x94 */
s32 fightTrainerAiWazaValueAkumu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern void* fightOutPokemonGetPokemonPtr();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    s32 ret = 0;

    if (fn_80236BFC(ctx, param3, 0x17) == 0) {
        ret = (s32)fn_80239984(0, ctx, 0x17b);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17b);
    }
    return ret;
}

/* Address: 0x802438CC | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueRokkuon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u16);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u16 fn_80201D84(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;
    u16 current;

    setup = 0;
    current = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
    if (fn_80236BFC(ctx, param3, 0x1d) == 0) {
        setup = fn_80239984(0, ctx, 0x179);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x179);
    }
    if (fn_80236BFC(ctx, param3, 0x1d) == 1) {
        if (current == fn_80201D84(param3, 0x1d)) {
            setup = fn_80239984(setup, ctx, 0x17a);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17a);
        }
    }
    return setup;
}

/* Address: 0x80243A0C | Size: 0x250 (592 bytes) */
u32 fightTrainerAiWazaValueNoroi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern f32 lbl_8047E640;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235714(void*, u32);
    extern u32 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80237DBC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_80237DBC(ctx, param1, 7);
    if ((u8)state == 1 && (u8)fn_802373B0(ctx, param1, 1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x174);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x174);
    } else if ((u8)state == 1 && (u8)fn_802373B0(ctx, param1, -1, lbl_8047E640) == 1) {
        handle = fn_80239984(0, ctx, 0x175);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x175);
    }
    if ((u8)state == 0 && (u8)fn_80235714(ctx, param1) == 0) {
        handle = fn_80239984(handle, ctx, 0x176);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x176);
    }
    if ((u8)state == 1) {
        handle = fn_80239984(handle, ctx, 0x177);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x177);
    }
    if ((u8)state == 0 && (u8)fn_80235714(ctx, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x178);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x178);
    }
    return handle;
}

/* Address: 0x80243C5C | Size: 0x7C | Pattern: field_accessor */
s32 fightTrainerAiWazaValueToriaezutukae(void* ctx, u32 slot, u32 param) {
    extern s32 fightOutPokemonGetPokemonPtr();
    extern s32 fn_80239984();
    extern void fn_80239EE8();
    s32 handle = fn_80239984(0, ctx, 0x173);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x173);
    return handle;
}

/* Address: 0x80243CD8 | Size: 0x640 (1600 bytes) */
void fightTrainerAiWazaValueKoraeru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern f32 lbl_8047E630;
    extern void wazaGetStatus();
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802373B0();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80237CB8();
    extern void fn_8023831C();
    extern void fn_80239500();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fightTrainerAiWazaValueJisin();
    extern void fightTrainerAiWazaValueJibaku();
    extern void fightTrainerAiWazaValueNull();
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r4;
    r28 = r5;
    r31 = r3;
    r16 = r6;
    r29 = 0x0;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r25 = r3;
    r4 = r31;
    r5 = (u32)sp + 0x20;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r15 = r3;
    r3 = r31;
    r4 = r16;
    r5 = (u32)sp + 0x8;
    fn_80237CB8();
    r16 = r3;
    r3 = r31;
    r4 = r30;
    fn_80236520();
    r27 = r3;
    r3 = r31;
    r4 = r30;
    fn_8023831C();
    r26 = r3;
    r17 = (u32)sp + 0x8;
    r16 = r16 & 0xFFFF;
    r18 = 0x0;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFFFF;
        if (r0 >= (u32)r16) break;
        r3 = r31;
        r4 = r28;
        fn_80239500();
        r6 = r3;
        r5 = *(u16*)(r17 + r0);
        r3 = r31;
        r4 = r30;
        fn_8023793C();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x41) {
            r18 = 0x1;
            break;
        }
        r19 = r19 + 0x1;

    }

    r0 = r18 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x16a;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16a;
        fn_80239EE8();
    }
    f1 = lbl_8047E630;
    r3 = r31;
    r4 = r30;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x16b;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16b;
        fn_80239EE8();
    }
    r16 = (u32)sp + 0x20;
    r21 = r15 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r4 = *(u32*)(r16 + r0);
        if (r30 != (u32)r4) {
            r3 = r31;
            r5 = (u32)sp + 0xc;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r17 = r3 & 0xFFFF;
            r24 = r3;
            if (r30 != (u32)r4) {
                r4 = (u32)fightTrainerAiWazaValueJisin;
                r3 = (u32)fightTrainerAiWazaValueJibaku;
                r5 = (u32)fightTrainerAiWazaValueNull;
                r15 = (u32)sp + 0xc;
                r19 = (u32)fightTrainerAiWazaValueJisin;
                r20 = (u32)fightTrainerAiWazaValueJibaku;
                r18 = (u32)fightTrainerAiWazaValueNull;
                r22 = 0x0;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r3 = 0x0;
                    r4 = *(u16*)(r15 + r0);
                    r5 = 0x1c;
                    r6 = 0x0;
                    wazaGetStatus();
                    if (r3 == (u32)0x0) {
                        r3 = r18;
                    }

                    if (r3 == (u32)r19 || r3 == (u32)r20) {

                        r3 = r29;
                        r4 = r31;
                        r5 = 0x16c;
                        fn_80239984();
                        r0 = r3;
                        r3 = r30;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r31;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x16c;
                        fn_80239EE8();
                        break;
                    }
                    r22 = r22 + 0x1;

                }

                r3 = r22 & 0xFFFF;
                r0 = r24 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r23 = r23 + 0x1;

    }

    r3 = (u32)fightTrainerAiWazaValueJisin;
    r15 = 0x0;
    r16 = (u32)fightTrainerAiWazaValueJisin;
    while (1) {
        r3 = lbl_80478DF8;
        r4 = r15 & 0xFFFF;
        r0 = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= (u32)r0) break;
        r4 = r15;
        r3 = 0x0;
        r5 = 0x1c;
        r6 = 0x0;
        wazaGetStatus();
        if (r3 == (u32)0x0) {
            r3 = (u32)fightTrainerAiWazaValueNull;
            r3 = (u32)fightTrainerAiWazaValueNull;
        }
        if (r3 != (u32)r16) {
            r4 = (u32)fightTrainerAiWazaValueJibaku;
            r0 = (u32)fightTrainerAiWazaValueJibaku;
            if (r3 == (u32)r0) {
            }
            r4 = r31;
            r7 = r15;
            r3 = 0x0;
            r5 = 0x1;
            r6 = 0x1;
            r8 = 0x0;
            fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r29;
                r4 = r31;
                r5 = 0x16d;
                fn_80239984();
                r0 = r3;
                r3 = r30;
                r29 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r31;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x16d;
                fn_80239EE8();
                break;
            }
            }
        r15 = r15 + 0x1;

    }

    r3 = r31;
    r4 = r30;
    r15 = 0x0;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r0 = r15 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x16e;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16e;
        fn_80239EE8();
    }
    r0 = r27 & 0xFFFF;
    if (r0 == (u32)0xcb) {
        r3 = r30;
        r4 = 0x0;
        r5 = 0xfc;
        r6 = 0x0;
        ((void(*)(void))pokemonGetStatus)();
        if ((s32)r3 != (s32)0x0) {
            r3 = r29;
            r4 = r31;
            r5 = 0x16f;
            fn_80239984();
            r0 = r3;
            r3 = r30;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r31;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x16f;
            fn_80239EE8();
    }
    }
    r0 = r26 & 0xFFFF;
    if ((r0 != (u32)0x11) && (r0 != (u32)0xf)) {

        r3 = r29;
        r4 = r31;
        r5 = 0x170;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x170;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    fn_802376EC();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x171;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x171;
        fn_80239EE8();
    }
    r0 = r25 & 0xFF;
    if (r0 != (u32)0x4) {
        if (r0 != (u32)0x3) { r3 = r29; return; }
    }
    r3 = r29;
    r4 = r31;
    r5 = 0x172;
    fn_80239984();
    r0 = r3;
    r3 = r30;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r28;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x172;
    fn_80239EE8();

    r3 = r29;
    return;
}

/* Address: 0x80244318 | Size: 0x160 (352 bytes) */
u32 fightTrainerAiWazaValueMiyaburu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802357CC(void*, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_80237DBC(ctx, param3, 7) == 1) {
        handle = fn_80239984(0, ctx, 0x167);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x167);
    }
    if ((fn_802357CC(ctx, param3) & 0xff) >= 8) {
        handle = fn_80239984(handle, ctx, 0x168);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x168);
    }
    if (fn_80236BFC(ctx, param3, 0x19) == 1) {
        handle = fn_80239984(handle, ctx, 0x169);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x169);
    }
    return handle;
}

/* Address: 0x80244478 | Size: 0x9C */
u32 fightTrainerAiWazaValueMajikkukooto(void* ctx, u32 param1, u32 param2) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((pokemonGetStatus(param1, 0, 0xed, 0) & 0xFFFF) != 0) {
        handle = fn_80239984(0, ctx, 0x166);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x166);
    }
    return handle;
}

/* Address: 0x80244514 | Size: 0x18C (396 bytes) */
u32 fightTrainerAiWazaValueMiraakooto(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8010C4A0(u32);
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_802395C8(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    u32 rawCount;
    u32 count;
    u32* entriesPtr;
    u32 move;
    u32 index;
    u32 current;
    u32 handle;

    handle = 0;
    rawCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 0, 1);
    move = fn_80236520(ctx, param3);
    entriesPtr = entries;
    count = rawCount & 0xffff;
    index = 0;
    while ((u16)index < count) {
        current = fn_8023715C(ctx, entriesPtr[(u16)index]);
        if ((u16)current < (u16)fn_80236FFC(ctx, entriesPtr[(u16)index])) {
            handle = fn_80239984(0, ctx, 0x164);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x164);
            break;
        }
        index++;
    }
    if ((u16)move != 0 && (u16)move != 0xffff && (u16)move != 0x165 && (u16)move != 0x163) {
        if ((u8)fn_8010C4A0(fn_802395C8(ctx, move, param3)) == 2) {
            handle = fn_80239984(handle, ctx, 0x165);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x165);
        }
    }
    return handle;
}

/* Address: 0x802446A0 | Size: 0x18C (396 bytes) */
u32 fightTrainerAiWazaValueKauntaa(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8010C4A0(u32);
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_802395C8(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    u32 rawCount;
    u32 count;
    u32* entriesPtr;
    u32 move;
    u32 index;
    u32 current;
    u32 handle;

    handle = 0;
    rawCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 0, 1);
    move = fn_80236520(ctx, param3);
    entriesPtr = entries;
    count = rawCount & 0xffff;
    index = 0;
    while ((u16)index < count) {
        current = fn_8023715C(ctx, entriesPtr[(u16)index]);
        if ((u16)current > (u16)fn_80236FFC(ctx, entriesPtr[(u16)index])) {
            handle = fn_80239984(0, ctx, 0x162);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x162);
            break;
        }
        index++;
    }
    if ((u16)move != 0 && (u16)move != 0xffff && (u16)move != 0x165 && (u16)move != 0x163) {
        if ((u8)fn_8010C4A0(fn_802395C8(ctx, move, param3)) == 1) {
            handle = fn_80239984(handle, ctx, 0x163);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x163);
        }
    }
    return handle;
}

/* Address: 0x8024482C | Size: 0xD4 (212 bytes) */
u32 fightTrainerAiWazaValueAroma(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80238748(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 list[23];
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32* listPtr;
    u32 handle = 0;
    u32 i;
    u32 count;

    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, battleCtx, list, 1, 1);
    listPtr = list;
    count &= 0xFFFF;
    for (i = 0; (u16)i < count; i++) {
        if (fn_80238748(battleCtx, listPtr[(u16)i]) == 0) {
            handle = fn_80239984(0, battleCtx, 0x161);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x161);
            break;
        }
    }
    return handle;
}

/* Address: 0x80244900 | Size: 0x8C */
u32 fightTrainerAiWazaValueRihuressyu(void* ctx, u32 param1, u32 param2) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80237310(ctx) == 0) {
        u32 tmp = fn_80239984(0, ctx, 0x160);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x160);
    }
    return handle;
}

/* Address: 0x8024498C | Size: 0x318 (792 bytes) */
void fightTrainerAiWazaValueSunaarasi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_802377E8();
    extern void fn_80238980();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0xC0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r26 = r4;
    r27 = r5;
    r25 = r3;
    r28 = 0x0;
    r4 = 0x0;
    r5 = 0x0;
    fn_80235B04();
    r29 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x28;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r30 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r0 = r29 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x3) {
        r4 = r25;
        r3 = 0x0;
        r5 = 0x15b;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x15b;
        fn_80239EE8();
    }
    r22 = (u32)sp + 0x28;
    r24 = r30 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = r25;
        r4 = *(u32*)(r22 + r23);
        r5 = 0x5;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r4 = *(u32*)(r22 + r23);
            r3 = r25;
            r5 = 0x4;
            fn_80238E30();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r4 = *(u32*)(r22 + r23);
                r3 = r25;
                r5 = 0x8;
                fn_80238E30();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                    r4 = *(u32*)(r22 + r23);
                    r3 = r25;
                    r5 = 0x8;
                    fn_80239058();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
            }
            }
            }
            r3 = r28;
            r4 = r25;
            r5 = 0x15c;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15c;
            fn_80239EE8();
            break;
                    }
        r21 = r21 + 0x1;

    }

    r24 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r25;
        r4 = *(u32*)(r24 + r0);
        fn_802377E8();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x12f) {
            r3 = r28;
            r4 = r25;
            r5 = 0x15d;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15d;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r31 = (u32)sp + 0x28;
    r30 = r30 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r30) break;
        r3 = r25;
        r4 = *(u32*)(r31 + r0);
        fn_80238980();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x181) {
            r3 = r28;
            r4 = r25;
            r5 = 0x15e;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15e;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r0 = r29 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = r28;
        r4 = r25;
        r5 = 0x15f;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x15f;
        fn_80239EE8();
    }
    r3 = r28;
    return;
}

/* Address: 0x80244CA4 | Size: 0x2C4 (708 bytes) */
u32 fightTrainerAiWazaValueArare(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u16 fn_802377E8(void*, u32);
    u32 pokemon[23];
    u32 outPokemon[8];
    u32 handle;
    u8 weather;
    u16 pokemonCount;
    u16 outPokemonCount;
    u16 i;

    handle = 0;
    weather = fn_80235B04(ctx, 0, 0);
    pokemonCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);
    outPokemonCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 0, 1);

    if (weather != 4) {
        handle = fn_80239984(0, ctx, 0x156);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x156);
    }

    for (i = 0; i < pokemonCount; i++) {
        if (fn_80238E30(ctx, pokemon[i], 0xf) == 1) {
            handle = fn_80239984(handle, ctx, 0x157);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x157);
            break;
        }
    }

    for (i = 0; i < outPokemonCount; i++) {
        if (fn_802377E8(ctx, outPokemon[i]) == 0x12f) {
            handle = fn_80239984(handle, ctx, 0x158);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x158);
            break;
        }
    }

    for (i = 0; i < pokemonCount; i++) {
        if (fn_80238980(ctx, pokemon[i]) == 0x181) {
            handle = fn_80239984(handle, ctx, 0x159);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x159);
            break;
        }
    }

    if (weather == 4) {
        handle = fn_80239984(handle, ctx, 0x15a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x15a);
    }
    return handle;
}

/* Address: 0x80244F68 | Size: 0x258 (600 bytes) */
void fightTrainerAiWazaValueNihonbare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_80238980();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r27 = r4;
    r28 = r5;
    r26 = r3;
    r29 = 0x0;
    r4 = 0x0;
    r5 = 0x0;
    fn_80235B04();
    r30 = r3;
    r4 = r26;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r0 = r30 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x1) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x152;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x152;
        fn_80239EE8();
    }
    r23 = (u32)sp + 0x8;
    r25 = r31 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r26;
        r4 = *(u32*)(r23 + r24);
        r5 = 0xa;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r4 = *(u32*)(r23 + r24);
            r3 = r26;
            r5 = 0xc;
            fn_80238E30();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r4 = *(u32*)(r23 + r24);
                r3 = r26;
                r5 = 0x22;
                fn_80239058();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
            }
            }
            r3 = r29;
            r4 = r26;
            r5 = 0x153;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x153;
            fn_80239EE8();
            break;
                }
        r22 = r22 + 0x1;

    }

    r25 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r26;
        r4 = *(u32*)(r25 + r0);
        fn_80238980();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x181) {
            r3 = r29;
            r4 = r26;
            r5 = 0x154;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x154;
            fn_80239EE8();
            break;
        }
        r23 = r23 + 0x1;

    }

    r0 = r30 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r26;
        r5 = 0x155;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x155;
        fn_80239EE8();
    }
    r3 = r29;
    return;
}

/* Address: 0x802451C0 | Size: 0x258 (600 bytes) */
u32 fightTrainerAiWazaValueAmagoi(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 pokemon[23];
    u32 handle;
    u8 weather;
    u16 count;
    u16 i;

    handle = 0;
    weather = fn_80235B04(ctx, 0, 0);
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);

    if (weather != 2) {
        handle = fn_80239984(0, ctx, 0x14e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14e);
    }

    for (i = 0; i < count; i++) {
        if (fn_80238E30(ctx, pokemon[i], 0xb) == 1 ||
            fn_80239058(ctx, pokemon[i], 0x21) == 1 ||
            fn_80239058(ctx, pokemon[i], 0x2c) == 1) {
            handle = fn_80239984(handle, ctx, 0x14f);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14f);
            break;
        }
    }

    for (i = 0; i < count; i++) {
        if (fn_80238980(ctx, pokemon[i]) == 0x181) {
            handle = fn_80239984(handle, ctx, 0x150);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x150);
            break;
        }
    }

    if (weather == 2) {
        handle = fn_80239984(handle, ctx, 0x151);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x151);
    }
    return handle;
}

/* Address: 0x80245418 | Size: 0x160 (352 bytes) */
u32 fightTrainerAiWazaValueNemurare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_802384B4(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[26];
    u32 setup;
    u32 compareValue;
    u16 count;
    u16 index;

    setup = 0;
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, entries, 0, 1);
    if (fn_80237310(ctx, param3) == 1) {
        setup = fn_80239984(0, ctx, 0x14c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14c);
    }

    index = 0;
    while (index < count) {
        compareValue = pokemonGetStatus(param3, 0, 0xd5, 0);
        if (compareValue == entries[index]) {
            ;
        } else {
            if (fn_802384B4(ctx, entries[index], 8) == 1) {
                setup = fn_80239984(setup, ctx, 0x14d);
                fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14d);
                break;
            }
        }
        index++;
    }

    return setup;
}

/* Address: 0x80245578 | Size: 0x1A0 (416 bytes) */
u32 fightTrainerAiWazaValueNemuru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_8023831C(void*);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_8023831C(ctx);
    if ((state & 0xffff) == 3 || (state & 0xffff) == 9) {
        handle = fn_80239984(0, ctx, 0x148);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x148);
    }
    if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(handle, ctx, 0x149);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x149);
    }
    if (fn_80237310(ctx, param1) == 0) {
        handle = fn_80239984(handle, ctx, 0x14a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14a);
    }
    handle = fn_80239984(handle, ctx, 0x14b);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14b);
    return handle;
}

/* Address: 0x80245718 | Size: 0x98 */
u32 fightTrainerAiWazaValueKaihuku2(void* ctx, u32 param1, u32 param2) {
    extern const f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x147);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x147);
    }
    return handle;
}

/* Address: 0x802457B0 | Size: 0x168 (360 bytes) */
u32 fightTrainerAiWazaValueKaihuku1(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern u32 fn_80235B04();
    extern u8 fn_802373B0(void* a, u32 b, s32 c, f32 d);
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;
    u32 state;

    state = fn_80235B04(ctx, 0, 1);
    if ((u8)state == 1) {
        handle = fn_80239984(0, ctx, 0x144);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x144);
    }
    if ((u8)fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(handle, ctx, 0x145);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x145);
    }
    if ((u8)state == 2 || (u8)state == 4 || (u8)state == 3) {
        handle = fn_80239984(handle, ctx, 0x146);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x146);
    }
    return handle;
}

/* Address: 0x80245918 | Size: 0x98 */
u32 fightTrainerAiWazaValueKituke(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80236BFC(ctx, param3, 5) == 1) {
        u32 tmp = fn_80239984(0, ctx, 0x143);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x143);
    }
    return handle;
}

/* Address: 0x802459B0 | Size: 0x44C (1100 bytes) */
u32 fightTrainerAiWazaValueOiuti(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F8A18(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235BE4(void*, u32, u32, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fn_80235BE4(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fn_801F8A18(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x139);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x139);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x13a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13a);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x13b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13b);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x13c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13c);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x13d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13d);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x13e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13e);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x13f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x140);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x140);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x26) == 1) {
        handle = fn_80239984(handle, ctx, 0x141);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x141);
    }
    if ((u8)status != 0) {
        handle = fn_80239984(handle, ctx, 0x142);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x142);
    }
    return handle;
}

/* Address: 0x80245DFC | Size: 0x14C (332 bytes) */
u32 fightTrainerAiWazaValueUezaabooru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235B04(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_80235B04(ctx, 0, 1);
    if ((u8)state == 1 || (u8)state == 2) {
        handle = fn_80239984(0, ctx, 0x136);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x136);
    }
    if ((u8)state == 4) {
        handle = fn_80239984(handle, ctx, 0x137);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x137);
    }
    if ((u8)state == 3) {
        handle = fn_80239984(handle, ctx, 0x138);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x138);
    }
    return handle;
}

/* Address: 0x80245F48 | Size: 0x7C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueSizennotikara(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u16 tikeiDataBiosGetWazaId(u16);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fn_8023CA9C(void*, u32, u16, u32);
    u16 value;

    value = tikeiDataBiosGetWazaId((u16)fightFloorGetStatus(0, 0, 0xf, 0));
    if (value != (u16)param) {
        return fn_8023CA9C(ctx, slot, value, extra);
    }
    return 0;
}

/* Address: 0x80247048 | Size: 0x7C | Pattern: field_accessor */
s32 fightTrainerAiWazaValueItigekihissatu(void* ctx, u32 slot, u32 param) {
    extern s32 fightOutPokemonGetPokemonPtr();
    extern s32 fn_80239984();
    extern void fn_80239EE8();
    s32 handle = fn_80239984(0, ctx, 0x135);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x135);
    return handle;
}

/* Address: 0x802470C4 | Size: 0xEC (236 bytes) */
u32 fightTrainerAiWazaValueZennouryokuappu(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x133);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x133);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(handle, ctx, 0x134);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x134);
    }
    return handle;
}

/* Address: 0x802471B0 | Size: 0x128 (296 bytes) */
u32 fightTrainerAiWazaValueTuikaKougekiAppu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u8 fn_80239564(void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    u32 battleParam = param2;
    u32 trainerParam = param1;
    void* battleCtx = ctx;
    s32 quotient;
    u32 setupHandle;
    u32 pokemonPtr;
    u32 statusValue;

    statusValue = fn_80239564(battleCtx, battleParam);
    quotient = (s32)statusValue / fightTrainerGetStatus(0, 0x131, 0x3e, 0);
    setupHandle = fightTrainerAiAddValue(0, quotient);
    pokemonPtr = fightOutPokemonGetPokemonPtr(trainerParam);
    fn_80239CCC(0xEC64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x131, quotient);

    if (fn_80235AA0(battleCtx, trainerParam) >= 0xcU) {
        statusValue = fn_80239564(battleCtx, battleParam);
        quotient = (s32)statusValue / fightTrainerGetStatus(0, 0x132, 0x3e, 0);
        setupHandle = fightTrainerAiAddValue(setupHandle, quotient);
        pokemonPtr = fightOutPokemonGetPokemonPtr(trainerParam);
        fn_80239CCC(0xEC64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x132, quotient);
    }

    return setupHandle;
}

/* Address: 0x802472D8 | Size: 0xDC (220 bytes) */
u32 fightTrainerAiWazaValueHaganenotubasa(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x12f);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x12f);
    if (fn_80235A3C(ctx, param1) >= 0xc) {
        handle = fn_80239984(handle, ctx, 0x130);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x130);
    }
    return handle;
}

/* Address: 0x802473B4 | Size: 0x144 (324 bytes) */
u32 fn_802473B4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_8023831C(void*);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if ((fn_8023831C(battleCtx) & 0xFFFF) == 0x17) {
        handle = fn_80239984(0, battleCtx, 0x12c);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x12c);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10e, trainer) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x12d);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x12d);
    }
    handle = fn_80239984(handle, battleCtx, 0x12e);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x12e);
    return handle;
}

/* Address: 0x802474F8 | Size: 0x1A8 (424 bytes) */
u32 fightTrainerAiWazaValueOobaahiito(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_8023831C(void*);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((fn_8023831C(ctx) & 0xffff) == 0x17) {
        handle = fn_80239984(0, ctx, 0x128);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x128);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x129);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x129);
    }
    handle = fn_80239984(handle, ctx, 0x12a);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x12a);
    if (fn_80236BFC(ctx, param3, 7) == 1) {
        handle = fn_80239984(handle, ctx, 0x12b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x12b);
    }
    return handle;
}

/* Address: 0x802476A0 | Size: 0x110 (272 bytes) */
s32 fightTrainerAiWazaValueHatakiotosu(void* ctx, void* param1, u32 param2, u32 param3) {
    extern void* fightOutPokemonGetPokemonPtr();
    extern u8 fn_80236BFC();
    extern u8 fn_80237F74();
    extern u16 fn_802383A4();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    s32 handle = 0;

    if (fn_80236BFC(ctx, param3, 0x3d) == 0 && fn_802383A4(ctx, param3) != 0) {
        handle = (s32)fn_80239984(0, ctx, 0x126);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x126);
    }
    if (fn_80237F74(ctx, param3, 0x3c) == 1) {
        handle = (s32)fn_80239984(handle, ctx, 0x127);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x127);
    }
    return handle;
}

/* Address: 0x802477B0 | Size: 0x158 (344 bytes) */
u32 fightTrainerAiWazaValueDouguUbau(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80142984(u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80216048(u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_802383A4();
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 target = param3;
    u32 handle = 0;
    u8 canSteal;
    u32 targetItem;
    u32 ownItem;

    ownItem = fn_802383A4(battleCtx);
    targetItem = fn_802383A4(battleCtx, target);
    canSteal = 1;
    if (fn_80216048(trainer) == 0) {
        canSteal = 0;
    }
    if ((ownItem & 0xFFFF) != 0 || (targetItem & 0xFFFF) == 0xAF || (targetItem & 0xFFFF) == 0 ||
        fn_80142984(targetItem) == 0) {
        canSteal = 0;
    }
    if (canSteal == 1) {
        handle = fn_80239984(0, battleCtx, 0x124);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x124);
    }
    if (fn_80237F74(battleCtx, target, 0x3c) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x125);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x125);
    }
    return handle;
}

/* Address: 0x80247908 | Size: 0x1C0 (448 bytes) */
u32 fightTrainerAiWazaValueSooraabiimu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235B04(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 trainer;
    u32 sequenceArg;
    void* battleCtx;
    u32 handle;
    u32 state;

    trainer = param1;
    sequenceArg = param2;
    battleCtx = ctx;
    handle = 0;
    state = fn_80235B04(battleCtx, 0, 1);
    if ((u8)state == 1) {
        handle = fn_80239984(0, battleCtx, 0x120);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x120);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10e, trainer) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x121);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x121);
    }
    if ((u8)state == 2 || (u8)state == 4 || (u8)state == 3) {
        handle = fn_80239984(handle, battleCtx, 0x122);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x122);
    }
    if ((u8)state == 0) {
        handle = fn_80239984(handle, battleCtx, 0x123);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x123);
    }
    return handle;
}

/* Address: 0x80247AC8 | Size: 0x194 (404 bytes) */
u32 fightTrainerAiWazaValueToraiatakku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x11d);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11d);
    if (fn_80237310(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0x11e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11e);
    }
    if (fn_80237F74(ctx, param3, 0x13) == 1 || fn_80237F74(ctx, param3, 7) == 1 ||
        fn_80237F74(ctx, param3, 0x29) == 1 || fn_80237F74(ctx, param3, 0x28) == 1) {
        handle = fn_80239984(handle, ctx, 0x11f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11f);
    }
    return handle;
}

/* Address: 0x80247C5C | Size: 0x184 (388 bytes) */
u32 fightTrainerAiWazaValueDokudokunokiba(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x11a);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11a);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x11b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11b);
    }
    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 0x11) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = fn_80239984(handle, ctx, 0x11c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x11c);
    }
    return handle;
}

/* Address: 0x80247DE0 | Size: 0x1C0 (448 bytes) */
u32 fightTrainerAiWazaValuePoizunteeru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x116);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x116);
    handle = fn_80239984(handle, ctx, 0x117);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x117);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x118);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x118);
    }
    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 0x11) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = fn_80239984(handle, ctx, 0x119);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x119);
    }
    return handle;
}

/* Address: 0x80247FA0 | Size: 0x1D0 (464 bytes) */
u32 fightTrainerAiWazaValueTuikaDoku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 battleParam = param2;
    u32 trainerParam = param1;
    void* battleCtx = ctx;
    u32 target = param3;
    u32 handle;
    s32 quotient;
    u32 pokemonPtr;
    u32 statusValue;

    statusValue = fn_80239564(battleCtx, battleParam);
    quotient = ((s32)statusValue) / fightTrainerGetStatus(0, 0x113, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, quotient);
    ;
    fn_80239CCC(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainerParam), 0, 0, battleParam, 0, 0x113, quotient);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10e, trainerParam) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x114);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainerParam), 0, 0, battleParam, 0, 0x114);
    }
    if (((fn_80237310(battleCtx, target) == 0) || (fn_80237F74(battleCtx, target, 0x11) == 1)) ||
        (fn_80237F74(battleCtx, target, 0x13) == 1)) {
        statusValue = fn_80239564(battleCtx, battleParam);
        quotient = ((s32)statusValue) / fightTrainerGetStatus(0, 0x115, 0x3e, 0);
        handle = fightTrainerAiAddValue(handle, quotient);
        pokemonPtr = fightOutPokemonGetPokemonPtr(trainerParam);
        fn_80239CCC(0xec64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x115, quotient);
    }
    return handle;
}

/* Address: 0x80248170 | Size: 0x150 (336 bytes) */
u32 fightTrainerAiWazaValueHandou(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if ((fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(0, ctx, 0x110);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x110);
    }
    if ((fn_80237F74(ctx, param1, 0x36) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x111);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x111);
    }
    handle = fn_80239984(handle, ctx, 0x112);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x112);
    return handle;
}

/* Address: 0x802482C0 | Size: 0x1A0 (416 bytes) */
s32 fightTrainerAiWazaValueBakuretuPanti(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u8 fn_80236BFC();
    extern u16 fn_80201D84();
    extern u8 fn_80237F74();
    extern u32 fn_80239984();
    extern void* fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 h = 0;
    u32 r28v;

    r28v = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, fightFloorGetStatus(0, 0, 0x14, 0));
    if (fn_80236BFC(ctx, param3, 0x1d) == 1 &&
        (u16)r28v == (u16)fn_80201D84(param3, 0x1d)) {
        h = fn_80239984(0, ctx, 0x10a);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10a);
    }
    r28v = fn_80239984(h, ctx, 0x10b);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10b);
    if (fn_80236BFC(ctx, param3, 9) == 1 || fn_80237F74(ctx, param3, 0x14) == 1) {
        r28v = fn_80239984(r28v, ctx, 0x10c);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10c);
    }
    return r28v;
}

/* Address: 0x80248460 | Size: 0x22C (556 bytes) */
s32 fightTrainerAiWazaValueDenjihou(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u16 fightFloorGetStatus();
    extern u16 fn_80201D84();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u8 fn_80236BFC();
    extern s32 fn_80236D60();
    extern u8 fn_80237310();
    extern u8 fn_80237F74();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    u32 buf[8];
    s32 handle = 0;
    u16 someVal;
    u16 count;
    u16 i;

    someVal = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, fightFloorGetStatus(0, 0, 0x14, 0));
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf, 1, 1);

    for (i = 0; i < count; i++) {
        if (buf[i] != 0 && fn_80236D60(ctx, param3, buf[i]) > 0) {
            break;
        }
    }

    if (i < count) {
        handle = (s32)fn_80239984(0, ctx, 0x107);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x107);
    }

    if (fn_80236BFC(ctx, param3, 0x1D) == 1 && someVal == fn_80201D84(param3, 0x1D)) {
        handle = (s32)fn_80239984(handle, ctx, 0x108);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x108);
    }

    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 7) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = (s32)fn_80239984(handle, ctx, 0x109);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x109);
    }

    return handle;
}

/* Address: 0x8024868C | Size: 0x1D0 (464 bytes) */
u32 fightTrainerAiWazaValueTuikaMahi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 battleParam = param2;
    u32 trainerParam = param1;
    void* battleCtx = ctx;
    u32 target = param3;
    s32 quotient;
    u32 handle;
    u32 pokemonPtr;
    u32 statusValue;

    statusValue = fn_80239564(battleCtx, battleParam);
    quotient = (s32)statusValue / fightTrainerGetStatus(0, 0x104, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, quotient);
    pokemonPtr = fightOutPokemonGetPokemonPtr(trainerParam);
    fn_80239CCC(0xec64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x104, quotient);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10e, trainerParam) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x105);
        fn_80239EE8(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainerParam), 0, 0, battleParam, 0, 0x105);
    }
    if (fn_80237310(battleCtx, target) == 0 || fn_80237F74(battleCtx, target, 7) == 1 ||
        fn_80237F74(battleCtx, target, 0x13) == 1) {
        statusValue = fn_80239564(battleCtx, battleParam);
        quotient = (s32)statusValue / fightTrainerGetStatus(0, 0x106, 0x3e, 0);
        handle = fightTrainerAiAddValue(handle, quotient);
        pokemonPtr = fightOutPokemonGetPokemonPtr(trainerParam);
        fn_80239CCC(0xec64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x106, quotient);
    }
    return handle;
}

/* Address: 0x8024885C | Size: 0x2C0 (704 bytes) */
u32 fightTrainerAiWazaValueTuikaKoori(void* ctx, u32 param1, u32 param2, u32 param3) {
    typedef union PokemonRef {
        u32 value;
        void* ptr;
    } PokemonRef;
    PokemonRef pokemon[23];
    u32 handle;
    u32 value;
    u16 count;
    u16 i;

    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 0, 1);
    value = fn_80239564(ctx, param2);
    value = (s32)value / fightTrainerGetStatus(0, 0x100, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, value);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x100, value);

    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x101);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x101);
    }

    for (i = 0; i < count; i++) {
        if (pokemonGetStatus(param3, 0, 0xd5, 0) != pokemon[i].ptr && fn_802384B4(ctx, pokemon[i].value, 7) == 1) {
            value = fn_80239564(ctx, param2);
            value = (s32)value / fightTrainerGetStatus(0, 0x102, 0x3e, 0);
            handle = fightTrainerAiAddValue(handle, value);
            fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x102, value);
            break;
        }
    }

    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 0x28) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        value = fn_80239564(ctx, param2);
        value = (s32)value / fightTrainerGetStatus(0, 0x103, 0x3e, 0);
        handle = fightTrainerAiAddValue(handle, value);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x103, value);
    }
    return handle;
}

/* Address: 0x80248B1C | Size: 0x220 (544 bytes) */
u32 fightTrainerAiWazaValueBureizukikku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80237310(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0xfb);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xfb);
    handle = fn_80239984(handle, ctx, 0xfc);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xfc);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xfd);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xfd);
    }
    if ((u8)fn_80236BFC(ctx, param3, 7) == 1) {
        handle = fn_80239984(handle, ctx, 0xfe);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xfe);
    }
    if ((u8)fn_80237310(ctx, param3) == 0 || (u8)fn_80237F74(ctx, param3, 0x29) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = fn_80239984(handle, ctx, 0xff);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xff);
    }
    return handle;
}

/* Address: 0x80248D3C | Size: 0x288 (648 bytes) */
u32 fightTrainerAiWazaValueJikokaitou(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80237310(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    u32 handle;

    handle = 0;
    if ((u8)fn_80236BFC(ctx, param1, 7) == 1) {
        handle = fn_80239984(0, ctx, 0xf6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf6);
    }
    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xf7, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(handle, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf7, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xf8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf8);
    }
    if ((u8)fn_80236BFC(ctx, param3, 7) == 1) {
        handle = fn_80239984(handle, ctx, 0xf9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf9);
    }
    if ((u8)fn_80237310(ctx, param3) != 0) {
        if ((u8)fn_80237F74(ctx, param3, 0x29) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
                goto done;
            }
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xfa, 0x3e, 0);
    quotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xfa, quotient);
done:
    return handle;
}

/* Address: 0x80248FC4 | Size: 0x49C (1180 bytes) */
u32 fightTrainerAiWazaValueHonoonouzu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F8A18(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235BE4(void*, u32, u32, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fn_80235BE4(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fn_801F8A18(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)status == 0) {
        handle = fn_80239984(0, ctx, 0xeb);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xeb);
    }
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(handle, ctx, 0xec);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xec);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0xed);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xed);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0xee);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xee);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0xef);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xef);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0xf0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf0);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0xf1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf1);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0xf2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf2);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0xf3);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf3);
    }
    if ((u8)fn_80236BFC(ctx, param3, 7) == 1) {
        handle = fn_80239984(handle, ctx, 0xf4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf4);
    }
    if ((u8)status != 0) {
        handle = fn_80239984(handle, ctx, 0xf5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xf5);
    }
    return handle;
}

/* Address: 0x80249460 | Size: 0x218 (536 bytes) */
u32 fightTrainerAiWazaValueIryokuHonoo(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80237310(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    s32 quotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xe7, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe7, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xe8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe8);
    }
    if ((u8)fn_80236BFC(ctx, param3, 7) == 1) {
        handle = fn_80239984(handle, ctx, 0xe9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe9);
    }
    if ((u8)fn_80237310(ctx, param3) == 0 || (u8)fn_80237F74(ctx, param3, 0x29) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xea, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xea, quotient);
    }
    return handle;
}

/* Address: 0x80249678 | Size: 0x248 (584 bytes) */
u32 fightTrainerAiWazaValueTuikouKonran(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    s32 finalQuotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xe3, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe3, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xe4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe4);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xe5, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe5, quotient);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x14) != 1) {
            goto done;
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xe6, 0x3e, 0);
    finalQuotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, finalQuotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe6, finalQuotient);
done:
    return handle;
}

/* Address: 0x802498C0 | Size: 0x1F4 (500 bytes) */
u32 fightTrainerAiWazaValueDorokake(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802358AC(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0xdf);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xdf);
    if ((u8)fn_802358AC(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0xe0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe0);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) == 1 || (u8)fn_80237F74(ctx, param3, 0x13) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x49) == 1 || (u8)fn_80237F74(ctx, param3, 0x33) == 1) {
        handle = fn_80239984(handle, ctx, 0xe1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe1);
    }
    if ((u8)fn_802358AC(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0xe2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe2);
    }
    return handle;
}

/* Address: 0x80249AB4 | Size: 0x278 (632 bytes) */
u32 fightTrainerAiWazaValueTuikouMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802358AC(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    s32 finalQuotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xdb, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xdb, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xdc);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xdc);
    }
    if ((u8)fn_802358AC(ctx, param3) == 0) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xdd, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xdd, quotient);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x49) != 1) {
                if ((u8)fn_80237F74(ctx, param3, 0x33) != 1) {
                    goto done;
                }
            }
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xde, 0x3e, 0);
    finalQuotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, finalQuotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xde, finalQuotient);
done:
    return handle;
}

/* Address: 0x80249D2C | Size: 0x25C (604 bytes) */
u32 fightTrainerAiWazaValueTuikouTokubouDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235974(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    s32 finalQuotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xd7, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd7, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xd8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd8);
    }
    if ((u8)fn_80235974(ctx, param3) == 0) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xd9, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd9, quotient);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x49) != 1) {
                goto done;
            }
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xda, 0x3e, 0);
    finalQuotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, finalQuotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xda, finalQuotient);
done:
    return handle;
}

/* Address: 0x80249F88 | Size: 0x1E8 (488 bytes) */
u32 fightTrainerAiWazaValueTuikouTokukouDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802359D8(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0xd3);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd3);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xd4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd4);
    }
    if ((u8)fn_802359D8(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0xd5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd5);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x49) != 1) {
                goto done;
            }
        }
    }
    handle = fn_80239984(handle, ctx, 0xd6);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd6);
done:
    return handle;
}

/* Address: 0x8024A170 | Size: 0x2B8 (696 bytes) */
void fightTrainerAiWazaValueKogoerukaze(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236D60();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r7 = 0x1;
    r24 = r3;
    r25 = r4;
    r26 = r5;
    r27 = r6;
    r4 = r24;
    r5 = (u32)sp + 0x28;
    r29 = 0x0;
    r3 = 0x0;
    r6 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r31 = (u32)sp + 0x8;
    r23 = r3 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r19 = *(u32*)(r31 + r0);
        if (r19 != (u32)0x0) {
            r21 = (u32)sp + 0x28;
            r22 = r30 & 0xFFFF;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFFFF;
            if (r0 >= (u32)r22) break;
                r5 = *(u32*)(r21 + r0);
                if (r5 != (u32)0x0) {
                    r3 = r24;
                    r4 = r19;
                    fn_80236D60();
                    if ((s32)r3 > (s32)0x0) {
                        r3 = r29;
                        r4 = r24;
                        r5 = 0xcf;
                        fn_80239984();
                        r0 = r3;
                        r3 = r25;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r24;
                        r8 = r26;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xcf;
                        fn_80239EE8();
                        break;
                }
                }
                r20 = r20 + 0x1;

            }
        }
        r28 = r28 + 0x1;

    }
    if (r23 >= (u32)0x2) {
        r3 = r29;
        r4 = r24;
        r5 = 0xd0;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd0;
        fn_80239EE8();
    }
    r31 = (u32)sp + 0x28;
    r28 = r30 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r28) break;
        r5 = *(u32*)(r31 + r0);
        if (r5 != (u32)0x0) {
            r3 = r24;
            r4 = r27;
            fn_80236D60();
            if ((s32)r3 < (s32)0x0) break;
        }
        r21 = r21 + 0x1;

    }

    r3 = r21 & 0xFFFF;
    r0 = r30 & 0xFFFF;
    if (r3 < r0) {
        r3 = r29;
        r4 = r24;
        r5 = 0xd1;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd1;
        fn_80239EE8();
    }
    r3 = r24;
    r4 = r27;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r24;
        r4 = r27;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r24;
            r4 = r27;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r29; return; }
    }
    }
    r3 = r29;
    r4 = r24;
    r5 = 0xd2;
    fn_80239984();
    r0 = r3;
    r3 = r25;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r24;
    r8 = r26;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xd2;
    fn_80239EE8();

    r3 = r29;
    return;
}

/* Address: 0x8024A428 | Size: 0x23C (572 bytes) */
void fightTrainerAiWazaValueKanarazuSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235910();
    extern void fn_80236D60();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r7 = 0x1;
    r24 = r3;
    r25 = r4;
    r26 = r5;
    r27 = r6;
    r4 = r24;
    r5 = (u32)sp + 0x28;
    r29 = 0x0;
    r3 = 0x0;
    r6 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r31 = (u32)sp + 0x8;
    r23 = r3 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r19 = *(u32*)(r31 + r0);
        if (r19 != (u32)0x0) {
            r21 = (u32)sp + 0x28;
            r22 = r30 & 0xFFFF;
            r18 = 0x0;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFFFF;
                if (r0 >= (u32)r22) break;
                r5 = *(u32*)(r21 + r0);
                if (r5 != (u32)0x0) {
                    r3 = r24;
                    r4 = r19;
                    fn_80236D60();
                    if ((s32)r3 > (s32)0x0) {
                        r3 = r29;
                        r4 = r24;
                        r5 = 0xcc;
                        fn_80239984();
                        r0 = r3;
                        r3 = r25;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r24;
                        r8 = r26;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xcc;
                        fn_80239EE8();
                        r18 = 0x1;
                        break;
                }
                }
                r20 = r20 + 0x1;

            }

            r0 = r18 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r28 = r28 + 0x1;

    }

    r3 = r24;
    r4 = r27;
    fn_80235910();
    r0 = r3 & 0xFF;
    if (r0 == (u32)r23) {
        r3 = r29;
        r4 = r24;
        r5 = 0xcd;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xcd;
        fn_80239EE8();
    }
    r3 = r24;
    r4 = r27;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r24;
        r4 = r27;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r24;
            r4 = r27;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r29; return; }
    }
    }
    r3 = r29;
    r4 = r24;
    r5 = 0xce;
    fn_80239984();
    r0 = r3;
    r3 = r25;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r24;
    r8 = r26;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xce;
    fn_80239EE8();

    r3 = r29;
    return;
}

/* Address: 0x8024A664 | Size: 0x2C0 (704 bytes) */
u32 fightTrainerAiWazaValueTuikouSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235910(void*, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    s32 finalQuotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xc7, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc7, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xc8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc8);
    }
    if ((u8)fn_80236BFC(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0xc9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc9);
    }
    if ((u8)fn_80235910(ctx, param3) == 0) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xca, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xca, quotient);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x49) != 1) {
                goto done;
            }
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xcb, 0x3e, 0);
    finalQuotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, finalQuotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xcb, finalQuotient);
done:
    return handle;
}

/* Address: 0x8024A924 | Size: 0x25C (604 bytes) */
u32 fightTrainerAiWazaValueTuikouBougyoDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235A3C(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    u8 numerator;
    s32 quotient;
    s32 finalQuotient;
    u32 handle;

    quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xc3, 0x3e, 0);
    quotient = quotient / denom;
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc3, quotient);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xc4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc4);
    }
    if ((u8)fn_80235A3C(ctx, param3) == 0) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xc5, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc5, quotient);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) != 1) {
        if ((u8)fn_80237F74(ctx, param3, 0x13) != 1) {
            if ((u8)fn_80237F74(ctx, param3, 0x49) != 1) {
                goto done;
            }
        }
    }
    numerator = (s32)(fn_80239564(ctx, param2) & 0xff);
    denom = fightTrainerGetStatus(0, 0xc6, 0x3e, 0);
    finalQuotient = numerator / denom;
    handle = fightTrainerAiAddValue(handle, finalQuotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc6, finalQuotient);
done:
    return handle;
}

/* Address: 0x8024AB80 | Size: 0x204 (516 bytes) */
u32 fightTrainerAiWazaValueOororabiimu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235AA0(void*, u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0xbf);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbf);
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xc0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc0);
    }
    if ((u8)fn_80235AA0(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0xc1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc1);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x1d) == 1 || (u8)fn_80237F74(ctx, param3, 0x13) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x49) == 1 || (u8)fn_80237F74(ctx, param3, 0x34) == 1) {
        handle = fn_80239984(handle, ctx, 0xc2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc2);
    }
    return handle;
}

/* Address: 0x8024AD84 | Size: 0x16C (364 bytes) */
u32 fightTrainerAiWazaValueNekodamasi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((pokemonGetStatus(param1, 0, 0xed, 0) & 0xffff) != 0) {
        handle = fn_80239984(0, ctx, 0xbd);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbd);
    }
    if (fn_80237F74(ctx, param3, 0x27) == 1) {
        handle = fn_80239984(handle, ctx, 0xbe);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbe);
        goto done;
    }
    if (fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = fn_80239984(handle, ctx, 0xbe);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbe);
    }
done:
    return handle;
}

/* Address: 0x8024AEF0 | Size: 0xD4 (212 bytes) */
u32 fightTrainerAiWazaValueGoddobaado(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_80236D60(ctx, param1, param3) > 0) {
        handle = fn_80239984(0, ctx, 0xbb);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbb);
    }
    handle = fn_80239984(handle, ctx, 0xbc);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xbc);
    return handle;
}

/* Address: 0x8024AFC4 | Size: 0x4B0 (1200 bytes) */
u32 fightTrainerAiWazaValueTuikaHirumi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239564(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 denom;
    s32 quotient;
    u32 handle;

    handle = 0;
    if (fn_80236D60(ctx, param1, param3) > 0) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xb0, 0x3e, 0);
        quotient = quotient / denom;
        handle = fightTrainerAiAddValue(0, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb0, quotient);
        if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
            handle = fn_80239984(handle, ctx, 0xb1);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb1);
        }
        if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
            handle = fn_80239984(handle, ctx, 0xb2);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb2);
        }
        if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
            handle = fn_80239984(handle, ctx, 0xb3);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb3);
        }
        if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
            handle = fn_80239984(handle, ctx, 0xb4);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb4);
        }
        if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
            handle = fn_80239984(handle, ctx, 0xb5);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb5);
        }
        if ((u8)fn_80236BFC(ctx, param3, 5) == 1) {
            handle = fn_80239984(handle, ctx, 0xb6);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb6);
        }
        if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
            handle = fn_80239984(handle, ctx, 0xb7);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb7);
        }
        if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
            handle = fn_80239984(handle, ctx, 0xb8);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb8);
        }
        if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
            handle = fn_80239984(handle, ctx, 0xb9);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xb9);
        }
    }
    if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xba);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xba);
    }
    return handle;
}

/* Address: 0x8024B474 | Size: 0x5D0 (1488 bytes) */
void fightTrainerAiWazaValueJisin(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023C370();
    u8 sp[0xF0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r5;
    r30 = r3;
    r31 = 0x0;
    r5 = r29;
    r4 = r28;
    fn_802395C8();
    r24 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x5c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r0 = *(u32*)(r25 + r23);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r25 + r23);
            r6 = r3;
            r3 = r30;
            r5 = r24;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x43) {
                r3 = r31;
                r4 = r30;
                r5 = 0xa9;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xa9;
                fn_80239EE8();
        }
        }
        r26 = r26 + 0x1;

    }
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r0 = *(u32*)(r25 + r23);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r25 + r23);
            r6 = r3;
            r3 = r30;
            r5 = r24;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x42) {
                r3 = r31;
                r4 = r30;
                r5 = 0xaa;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xaa;
                fn_80239EE8();
        }
        }
        r26 = r26 + 0x1;

    }
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r25 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r4 = r3 & 0xFFFF;
            r26 = r3;
            if (r29 != (u32)r4) {
                r3 = (u32)sp + 0x8;
                r23 = 0x0;
                while (1) {
                    r0 = r23 & 0xFFFF;
                    if (r0 >= (u32)r4) break;
                    r0 = *(u16*)(r3 + r0);
                    if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                        if (r0 == (u32)0xcb) {
                        }
                        r3 = r31;
                        r4 = r30;
                        r5 = 0xab;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r30;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xab;
                        fn_80239EE8();
                        break;
                        }
                    r23 = r23 + 0x1;

                }

                r3 = r23 & 0xFFFF;
                r0 = r26 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r24 = r24 + 0x1;

    }

    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0xb6;
    r8 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xac;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xac;
        fn_80239EE8();

    } else {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xc5;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xac;
            fn_80239EE8();
            goto L_8024B878;
        }
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xcb;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xac;
            fn_80239EE8();
        }
    }
L_8024B878:
    r4 = r30;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xad;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xad;
        fn_80239EE8();
    }
    r24 = (u32)sp + 0x3c;
    r23 = r27 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r4 = *(u32*)(r24 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_80236520();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                if (r0 == (u32)0xcb) {
                }
                r3 = r31;
                r4 = r30;
                r5 = 0xae;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xae;
                fn_80239EE8();
            }
                }
        r22 = r22 + 0x1;

    }
    r22 = (u32)sp + 0x3c;
    r23 = r27 & 0xFFFF;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r4 = *(u32*)(r22 + r24);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_802376EC();
            r6 = *(u32*)(r22 + r24);
            r24 = r3;
            r3 = r30;
            r4 = r29;
            r5 = r28;
            r7 = 0x0;
            fn_8023C370();
            r0 = r24 & 0xFFFF;
            if ((s32)r0 <= (s32)r3) {
                r3 = r31;
                r4 = r30;
                r5 = 0xaf;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xaf;
                fn_80239EE8();
        }
        }
        r25 = r25 + 0x1;

    }
    r3 = r31;
    return;
}

/* Address: 0x8024BA44 | Size: 0x438 (1080 bytes) */
u32 fightTrainerAiWazaValueSokubaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F8A18(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235BE4(void*, u32, u32, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fn_80235BE4(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fn_801F8A18(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)status == 0) {
        handle = fn_80239984(0, ctx, 0x9f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(handle, ctx, 0xa0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa0);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0xa1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa1);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0xa2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa2);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0xa4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa4);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0xa5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa5);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0xa6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa6);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0xa7);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa7);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0xa3);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa3);
    }
    if ((u8)status != 0) {
        handle = fn_80239984(handle, ctx, 0xa8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa8);
    }
    return handle;
}

/* Address: 0x8024BE7C | Size: 0x144 (324 bytes) */
u32 fightTrainerAiWazaValueKorogaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 fn_802387C8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    s32 value;
    s32 quotient;

    handle = 0;
    if (fn_80236BFC(ctx, param1, 0x1a) == 1) {
        handle = fn_80239984(0, ctx, 0x9c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9c);
    }
    value = fn_802387C8(ctx, param1);
    quotient = value / fightTrainerGetStatus(0, 0x9d, 0x3e, 0);
    handle = fightTrainerAiAddValue(handle, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9d, quotient);
    handle = fn_80239984(handle, ctx, 0x9e);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9e);
    return handle;
}

/* Address: 0x8024BFC0 | Size: 0x5FC (1532 bytes) */
void fightTrainerAiWazaValueJibaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023C370();
    u8 sp[0xF0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r5;
    r30 = r3;
    r31 = 0x0;
    r5 = r29;
    r4 = r28;
    fn_802395C8();
    r25 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x5c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r0 = *(u32*)(r24 + r22);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r24 + r22);
            r6 = r3;
            r3 = r30;
            r5 = r25;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x43) {
                r3 = r31;
                r4 = r30;
                r5 = 0x94;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x94;
                fn_80239EE8();
        }
        }
        r23 = r23 + 0x1;

    }
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r0 = *(u32*)(r24 + r22);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r24 + r22);
            r6 = r3;
            r3 = r30;
            r5 = r25;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x42) {
                r3 = r31;
                r4 = r30;
                r5 = 0x95;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x95;
                fn_80239EE8();
        }
        }
        r23 = r23 + 0x1;

    }
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r4 = *(u32*)(r24 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r4 = r3 & 0xFFFF;
            r25 = r3;
            if (r29 != (u32)r4) {
                r3 = (u32)sp + 0x8;
                r22 = 0x0;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r4) break;
                    r0 = *(u16*)(r3 + r0);
                    if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                        if (r0 == (u32)0xcb) {
                        }
                        r3 = r31;
                        r4 = r30;
                        r5 = 0x96;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r30;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x96;
                        fn_80239EE8();
                        break;
                        }
                    r22 = r22 + 0x1;

                }

                r3 = r22 & 0xFFFF;
                r0 = r25 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r23 = r23 + 0x1;

    }

    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0xb6;
    r8 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0x97;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x97;
        fn_80239EE8();

    } else {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xc5;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x97;
            fn_80239EE8();
            goto L_8024C3C8;
        }
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xcb;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x97;
            fn_80239EE8();
        }
    }
L_8024C3C8:
    r25 = (u32)sp + 0x1c;
    r22 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r3 = r30;
        r4 = *(u32*)(r25 + r0);
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r24 & 0xFFFF;
            r3 = r31;
            r4 = r30;
            r5 = r0 - r0; /* -borrow */;
            r23 = r5 + 0x99;
            r5 = r23;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r10 = r23;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            fn_80239EE8();
        }
        r24 = r24 + 0x1;

    }
    r23 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r23 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_80236520();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                if (r0 == (u32)0xcb) {
                }
                r3 = r31;
                r4 = r30;
                r5 = 0x9a;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x9a;
                fn_80239EE8();
            }
                }
        r21 = r21 + 0x1;

    }
    r21 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r21 + r23);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_802376EC();
            r6 = *(u32*)(r21 + r23);
            r23 = r3;
            r3 = r30;
            r4 = r29;
            r5 = r28;
            r7 = 0x0;
            fn_8023C370();
            r0 = r23 & 0xFFFF;
            if ((s32)r0 <= (s32)r3) {
                r3 = r31;
                r4 = r30;
                r5 = 0x9b;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x9b;
                fn_80239EE8();
        }
        }
        r24 = r24 + 0x1;

    }
    r3 = r31;
    return;
}

/* Address: 0x8024C5BC | Size: 0x91C (2332 bytes) */
void fightTrainerAiWazaValueTobihaneru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r7 = 0x1;
    r31 = r3;
    r30 = r4;
    r29 = r5;
    r27 = r6;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r26 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r28 = r3;
    r3 = r31;
    r4 = r27;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x7e;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x7f;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x80;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x80;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x81;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x81;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x82;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x82;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x83;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x83;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x84;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x84;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x85;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x85;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x86;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x86;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = r27;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r3 = r26;
        r4 = r31;
        r5 = 0x87;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x87;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r31;
    r5 = 0x88;
    fn_80239984();
    r27 = r3;
    r3 = r30;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x88;
    fn_80239EE8();
    r3 = r31;
    r4 = r30;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x89;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x89;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8a;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8b;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8b;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8c;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8c;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8d;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8d;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8e;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8f;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x90;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x90;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x91;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x91;
        fn_80239EE8();
    }
    r26 = (u32)sp + 0x1c;
    r25 = r28 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r31;
        r4 = *(u32*)(r26 + r0);
        r5 = (u32)sp + 0x8;
        r6 = 0x0;
        r7 = 0x0;
        fn_802367CC();
        r23 = r3 & 0xFFFF;
        if (r0 != (u32)0x1) {
            r24 = (u32)sp + 0x8;
            r21 = 0x0;
            r22 = 0x0;
            while (1) {
                r0 = r22 & 0xFFFF;
                if (r0 >= (u32)r23) break;
                r3 = r31;
                r5 = *(u16*)(r24 + r0);
                r4 = 0x1f;
                fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r27;
                    r4 = r31;
                    r5 = 0x92;
                    fn_80239984();
                    r27 = r3;
                    r3 = r30;
                    fightOutPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r31;
                    r8 = r29;
                    r6 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x92;
                    fn_80239EE8();
                    r21 = 0x1;
                    break;
                }
                r22 = r22 + 0x1;

            }

            r0 = r21 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r28 = r28 + 0x1;

    }

    r3 = r31;
    r4 = r30;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x93;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x93;
        fn_80239EE8();
    }
    r3 = r27;
    return;
}

/* Address: 0x8024CED8 | Size: 0x940 (2368 bytes) */
void fightTrainerAiWazaValueSorawotobu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
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

    r7 = 0x1;
    r31 = r3;
    r29 = r4;
    r28 = r5;
    r25 = r6;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r30 = 0x0;
    r27 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r3 = r31;
    r4 = r25;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x69;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x69;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6a;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6b;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6b;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6c;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6c;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6d;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6e;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6f;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x70;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x70;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x71;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x71;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = r25;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r3 = r30;
        r4 = r31;
        r5 = 0x72;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x72;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x73;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x73;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x74;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x74;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x75;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x75;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x76;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x76;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x77;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x77;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x78;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x78;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x79;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x79;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7a;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7b;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7b;
        fn_80239EE8();
    }
    r0 = r28 & 0xFFFF;

    if (r0 == (u32)0x13 || r0 == (u32)0x154) {

        r27 = 0x1f;

    } else {
        if (r0 == (u32)0x5b) {
            r27 = 0x20;
            goto L_8024D6BC;
        }
        if (r0 == (u32)0x123) {
            r27 = 0x21;
        }
    }
L_8024D6BC:
    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = r31;
        r4 = *(u32*)(r25 + r0);
        r5 = (u32)sp + 0x8;
        r6 = 0x0;
        r7 = 0x0;
        fn_802367CC();
        r22 = r3 & 0xFFFF;
        if (r0 != (u32)0x123) {
            r23 = (u32)sp + 0x8;
            r20 = 0x0;
            r21 = 0x0;
            while (1) {
                r0 = r21 & 0xFFFF;
                if (r0 >= (u32)r22) break;
                r3 = r31;
                r5 = *(u16*)(r23 + r0);
                r4 = r27;
                fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r30;
                    r4 = r31;
                    r5 = 0x7c;
                    fn_80239984();
                    r0 = r3;
                    r3 = r29;
                    r30 = r0;
                    fightOutPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r31;
                    r8 = r28;
                    r6 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x7c;
                    fn_80239EE8();
                    r20 = 0x1;
                    break;
                }
                r21 = r21 + 0x1;

            }

            r0 = r20 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r26 = r26 + 0x1;

    }

    r3 = r31;
    r4 = r29;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7d;
        fn_80239EE8();
    }
    r3 = r30;
    return;
}

/* -------------------------------------------------------------------
 * Experience & Level Processing (0x8024D000-0x80254000)
 * 136 functions
 * ------------------------------------------------------------------- */

/* Address: 0x8024D818 | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueAbareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74();
    extern u16 fn_8023831C();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;
    u16 v;

    if (fn_80237F74(ctx, param1, 0x14) == 1) {
        handle = fn_80239984(0, ctx, 0x66);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x66);
    }
    v = fn_8023831C(ctx, param1);
    if (v == 8 || v == 9) {
        handle = fn_80239984(handle, ctx, 0x67);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x67);
    }
    handle = fn_80239984(handle, ctx, 0x68);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x68);
    return handle;
}

/* Address: 0x8024D958 | Size: 0x1A4 (420 bytes) */
u32 fightTrainerAiWazaValueKyuusyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802376EC();
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern s32 fn_8023C370(void*, u32, u32, u32, u32);
    u32 handle;
    s32 score;
    s32 scale;

    handle = fn_802376EC();
    score = fn_8023C370(ctx, param1, param2, param3, 1);
    scale = fightTrainerGetStatus(0, 0x63, 0x3e, 0);
    score = (((score / 2) * 100) / (u16)handle) / scale;
    {
        u32 nextHandle = fightTrainerAiAddValue(0, score);
        handle = nextHandle;
    }
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x63, score);
    if ((fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x64);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x64);
    }
    if ((fn_80237F74(ctx, param3, 0x40) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x65);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x65);
    }
    return handle;
}

/* Address: 0x8024DAFC | Size: 0xC0 */
u32 fightTrainerAiWazaValueRokettoZutuki(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x61);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x61);
    handle = fn_80239984(handle, ctx, 0x62);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x62);
    return handle;
}

/* Address: 0x8024DBBC | Size: 0xC0 */
u32 fightTrainerAiWazaValueKamaitati(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x5f);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x5f);
    handle = fn_80239984(handle, ctx, 0x60);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x60);
    return handle;
}

/* Address: 0x8024DC7C | Size: 0x210 (528 bytes) */
u32 fightTrainerAiWazaValueKiaipanti(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    void* battleCtx;
    u32 trainer;
    u32 sequenceArg;
    u32 handle;
    u32 rawCount;
    u32* entriesPtr;
    u16 count;
    u32 index;

    battleCtx = ctx;
    trainer = param1;
    sequenceArg = param2;
    handle = 0;
    rawCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, battleCtx, entries, 0, 1);
    if ((fn_80236520(battleCtx, trainer) & 0xFFFF) == 0x117) {
        handle = fn_80239984(0, battleCtx, 0x5B);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5B);
    }

    entriesPtr = entries;
    count = rawCount;
    index = 0;
    while ((u16)index < count) {
        if (fn_80236BFC(battleCtx, entriesPtr[(u16)index], 8) == 1) {
            handle = fn_80239984(handle, battleCtx, 0x5C);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5C);
            break;
        }
        index++;
    }

    entriesPtr = entries;
    rawCount = rawCount & 0xFFFF;
    index = 0;
    while ((u16)index < rawCount) {
        if (fn_80236BFC(battleCtx, entriesPtr[(u16)index], 7) == 1) {
            handle = fn_80239984(handle, battleCtx, 0x5D);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5D);
            break;
        }
        index++;
    }

    handle = fn_80239984(handle, battleCtx, 0x5E);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5E);
    return handle;
}

/* Address: 0x8024DE8C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueAteminage(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fn_802357CC(battleCtx, param3) > 6U) {
        handle = fn_80239984(0, battleCtx, 0x58);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x58);
    }
    if (fn_802358AC(battleCtx, trainer) < 6U) {
        handle = fn_80239984(handle, battleCtx, 0x59);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x59);
    }
    handle = fn_80239984(handle, battleCtx, 0x5a);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5a);
    return handle;
}

/* Address: 0x8024DFC4 | Size: 0x108 (264 bytes) */
u32 fightTrainerAiWazaValueRibenji(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;
    u32 move = fn_80236520(battleCtx, param3);

    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163) {
        if (fn_8023943C(battleCtx, move, 1) == 1) {
            handle = fn_80239984(0, battleCtx, 0x56);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x56);
        }
    }
    handle = fn_80239984(handle, battleCtx, 0x57);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x57);
    return handle;
}

/* Address: 0x8024E0CC | Size: 0x7C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueTokubetuYuusen(void* ctx, u32 slot, u32 param) {
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    u32 handle;

    handle = fn_80239984(0, ctx, 0x55);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x55);
    return handle;
}

/* Address: 0x8024E148 | Size: 0xEC (236 bytes) */
u32 fightTrainerAiWazaValueKouPuraioritii(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x53);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x53);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(handle, ctx, 0x54);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x54);
    }
    return handle;
}

/* Address: 0x8024E234 | Size: 0xEC (236 bytes) */
u32 fn_8024E234(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x51);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x51);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(handle, ctx, 0x52);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x52);
    }
    return handle;
}

/* Address: 0x8024E320 | Size: 0x164 (356 bytes) */
u32 fightTrainerAiWazaValueHittyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fn_802357CC(battleCtx, param3) > 6U) {
        handle = fn_80239984(0, battleCtx, 0x4E);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4E);
    }
    if (fn_802358AC(battleCtx, trainer) < 6U) {
        handle = fn_80239984(handle, battleCtx, 0x4F);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4F);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10E, trainer) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x50);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x50);
    }
    return handle;
}

/* Address: 0x8024E484 | Size: 0xA8 */
u32 fightTrainerAiWazaValueTuujouIryoku(void* ctx, u32 slot, u32 param) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(0, ctx, 0x4d);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x4d);
    }
    return handle;
}

/* Address: 0x8024E534 | Size: 0x44 | Pattern: field_accessor */
u32 fn_8024E534(void* ctx, u32 slot, u32 param) {
    extern u32 fightTrainerGetStatus();
    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return 0;
}
