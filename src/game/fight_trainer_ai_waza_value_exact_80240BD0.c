/**
 * @file fight_trainer_ai_waza_value_exact_80240BD0.c
 * @brief Exact fightTrainerAiWazaValue.cpp range, 0x80240BD0 - 0x80241660.
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


#pragma force_active on
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
            {
                /* Preserve MWCC's unsigned lower-bound comparison. */
                u32 min_status = param3 - param3;
                u32 max_status = min_status + 2;
                for (j = 0; j < 7; j++) {
                    if ((u32)bufD[j] >= min_status && (u32)bufD[j] <= max_status) {
                        matched = 1;
                        goto M3;
                    }
                }
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
        u32 block_elem;
        for (elem = 0; (u16)elem < count1; elem++) {
            block_elem = array1[(u16)elem];
            bufG[0] = fn_80235AA0(ctx, block_elem);
            bufG[1] = fn_80235A3C(ctx, block_elem);
            bufG[2] = fn_802359D8(ctx, block_elem);
            bufG[3] = fn_80235974(ctx, block_elem);
            bufG[4] = fn_80235910(ctx, block_elem);
            bufG[5] = fn_802358AC(ctx, block_elem);
            bufG[6] = fn_802357CC(ctx, block_elem);
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
        u32 block_elem;
        u16 block_i;
        for (block_i = 0; block_i < count2; block_i++) {
            block_elem = array2[block_i];
            bufH[0] = fn_80235AA0(ctx, block_elem);
            bufH[1] = fn_80235A3C(ctx, block_elem);
            bufH[2] = fn_802359D8(ctx, block_elem);
            bufH[3] = fn_80235974(ctx, block_elem);
            bufH[4] = fn_80235910(ctx, block_elem);
            bufH[5] = fn_802358AC(ctx, block_elem);
            bufH[6] = fn_802357CC(ctx, block_elem);
            {
                /* Preserve MWCC's unsigned lower-bound comparison. */
                u32 min_status = param3 - param3;
                u32 max_status = min_status + 2;
                for (j = 0; j < 7; j++) {
                    if ((u32)bufH[j] >= min_status && (u32)bufH[j] <= max_status) {
                        matched = 1;
                        goto M7;
                    }
                }
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
#pragma force_active off
