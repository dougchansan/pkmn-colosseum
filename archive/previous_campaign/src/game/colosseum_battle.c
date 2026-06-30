/**
 * @file colosseum_battle.c
 * @brief Pre/post battle flow, rewards, and experience for Colosseum mode.
 *
 * =========================================================================
 * SUBSYSTEM ANALYSIS
 * =========================================================================
 *
 * Address range: 0x80240000 - 0x80266360
 * Total functions: ~520
 * Total code size: ~156KB
 *
 * This module handles everything that happens around battles in the
 * Colosseum mode -- the setup before battles, processing after battles,
 * experience/reward distribution, and progression tracking.
 *
 * The code in this range heavily uses the CheckTrainerPokemonFlag
 * (fn_80236BFC, 272 calls) and BattleSequenceStart (fn_80239EE8, 491 calls)
 * functions, confirming its role as the battle orchestration layer.
 *
 * BATTLE ORCHESTRATION (0x80240000-0x8024D000):
 *
 *   fn_80240BD0 (0xA90 bytes): Large battle orchestration function.
 *     Manages the high-level flow of a battle encounter including
 *     pre-battle dialogue, party selection, and battle initiation.
 *
 *   fn_80245FC4 (0x1084 bytes): Second-largest in this region.
 *     Complex battle setup that handles multi-round encounters.
 *     The code shows repeated patterns of:
 *       CheckTrainerPokemonFlag -> PreBattleSetup -> BattleSequenceStart
 *     with different sequence IDs (0xF1, 0xF2, 0xF3, 0xF4).
 *
 *   fn_8024E690 (0x1224 bytes): Third-largest. Appears to handle
 *     post-battle processing including experience distribution and
 *     potential Shadow Pokemon purification progress.
 *
 * BATTLE SEQUENCE PATTERN:
 *   The calling convention throughout this module follows a strict pattern
 *   visible at 0x80249000:
 *
 *     1. GetTrainerPokemonPtr(ctx) -> pokemonPtr
 *     2. PreBattleSetup(ctx, trainerSlot, seqId) -> setupHandle
 *     3. CheckTrainerPokemonFlag(trainerSlot, partySlot, flagId)
 *     4. If flag set:
 *        BattleSequenceStart(0xEC64, trainerSlot, pokemonPtr,
 *                            0, 0, contextHandle, 0, seqId)
 *
 *   The constant 0xEC64 is constructed as `lis r6, 1; subi r3, r6, 0x139C`
 *   and appears to be a standard battle configuration code.
 *
 *   Sequence IDs observed:
 *     0xF1 = First round / initial encounter
 *     0xF2 = Second round / continuation
 *     0xF3 = Third round / continuation
 *     0xF4 = Final round
 *
 *   After each sequence:
 *     CheckTrainerPokemonFlag with flagId 0x18, 0x1E, 0x07 to determine
 *     if additional rounds are needed.
 *
 * REWARD/EXPERIENCE PROCESSING (0x8024D000-0x80260000):
 *
 *   This is the densest code region, with many functions in the
 *   0x80250000-0x80260000 range (500+ functions in 64KB). The
 *   function density suggests these are individual reward/stat/check
 *   handlers, each dealing with a specific aspect of post-battle
 *   processing.
 *
 *   Key address clusters:
 *     0x80250000-0x80254000: ~170 functions, likely experience calculation
 *       and level-up processing for each Pokemon
 *     0x80254000-0x80258000: ~100 functions, likely item rewards and
 *       Poke Coupon processing
 *     0x80258000-0x8025C000: ~80 functions, likely team state updates
 *       (PP restoration, status healing)
 *     0x8025C000-0x80260000: ~80 functions, likely shadow Pokemon
 *       purification gauge updates
 *
 * UTILITY REGION (0x80260000-0x80266360):
 *
 *   fn_8026316C (0xA5C bytes): Large utility function near the end.
 *     This is one of the last significant functions before the .ctors
 *     section at 0x80266360.
 *
 *   The remaining functions (0x80260000-0x80266360) appear to be
 *   miscellaneous helpers: string formatting, data validation,
 *   debug helpers, and cleanup functions.
 *
 * lbl_8047B610 USAGE (script PC):
 *   Throughout this module, lbl_8047B610 is incremented heavily:
 *     lwz r3, lbl_8047B610@sda21(r0)
 *     addi r0, r3, 1
 *     stw r0, lbl_8047B610@sda21(r0)
 *
 *   or with skip:
 *     lwz r3, lbl_8047B610@sda21(r0)
 *     addi r0, r3, 5
 *     stw r0, lbl_8047B610@sda21(r0)
 *
 *   The +5 skips happen after successful condition checks, jumping
 *   over what would be a 4-byte operand + 1-byte opcode.
 *
 *   Sometimes the PC is set to a loaded value:
 *     lwz r3, lbl_8047B610@sda21(r0)
 *     lwz r0, 0x1(r3)    ; read next 4 bytes as target
 *     stw r0, lbl_8047B610@sda21(r0)  ; absolute jump
 *
 *   This confirms a bytecode format where:
 *     Byte 0: opcode
 *     Bytes 1-4: optional 32-bit operand (jump target, value, etc.)
 *
 * =========================================================================
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* fn_8012640C();
extern u32   fn_801254B4();

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
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void fn_8025F2FC(int r3);
void fn_8025F524(int r3);
void fn_8025F618(int r3);
void fn_8025F7E8(int r3);
u32  fn_8025F81C(int r3);
u32  fn_8025F9AC(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void fn_800AE7E0();
extern void fn_800E01F4();
extern int  fn_800E0C04();
extern u32  fn_8011F520();
extern u32  fn_8011F5B0();
extern u16  fn_8011F5C8();
extern u32  fn_80129280();
extern int  fn_801C40F0();
extern int  fn_801C41C8();
extern int  fn_801DAC90();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 fn_802600E4();
u32 fn_802600E4();
int fn_80240BD0(void* ctx, u32 param1, u32 param2, u32 param3);
void fn_80245FC4(void* ctx, u32 param1, u32 param2, u32 param3);
void fn_8024E690(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fn_8025A290(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
void fn_8026316C(void* ctx, u32 param1, u32 param2, u32 param3);

/* =========================================================================
 * fn_80240BD0 - BattleOrchestrator
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
int fn_80240BD0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_80235A3C(void* ctx, u32 elem);
    extern u8 fn_802359D8(void* ctx, u32 elem);
    extern u8 fn_80235974(void* ctx, u32 elem);
    extern u8 fn_80235910(void* ctx, u32 elem);
    extern u8 fn_802358AC(void* ctx, u32 elem);
    extern u8 fn_802357CC(void* ctx, u32 elem);
    extern u16 fn_801F1C18(int a, void* ctx, u32* buf, int b, int c);
    extern int fn_80239984(int handle, void* ctx, int seq);
    extern u32 fn_80205B8C(u32 v);
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
    count1 = fn_801F1C18(0, ctx, array1, 1, 1);
    count2 = fn_801F1C18(0, ctx, array2, 0, 1);

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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1b6);
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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1b7);
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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1b8);
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
                if (bufD[j] >= 0 && bufD[j] <= 2) { matched = 1; goto M3; }
            }
            matched = 0;
        M3:
            if (matched == 1) { found = 1; goto L3; }
        }
        found = 0;
    L3:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1b9);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1b9);
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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1ba);
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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1bb);
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
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1bc);
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
                if (bufH[j] >= 0 && bufH[j] <= 2) { matched = 1; goto M7; }
            }
            matched = 0;
        M7:
            if (matched == 1) { found = 1; goto L7; }
        }
        found = 0;
    L7:
        if (found == 1) {
            handle = fn_80239984(handle, ctx, 0x1bd);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1bd);
        }
    }
    return handle;
}

/* =========================================================================
 * fn_80245FC4 - MultiBattleSetup
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
/* TODO: Decompile fn_80245FC4 (4228 bytes) */
void fn_80245FC4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80136468();
    extern void fn_801F1990();
    extern void fn_801F1A6C();
    extern void fn_801F54A4();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
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
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    extern void fn_8024AFC4();
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
    fn_801F54A4();
    r3 = r3 & 0xFFFF;
    fn_80136468();
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
    fn_801FB1C0();
    r25 = (s32)r25 / (s32)r3;
    r3 = 0x0;
    r4 = r25;
    fn_802399FC();
    r26 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x114;
        fn_80239984();
        r26 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r26;
        r4 = r24;
        fn_802399FC();
        r26 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_801F1A6C();
    r26 = r3;
    r3 = r31;
    r4 = r30;
    fn_80239564();
    r24 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x10d;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
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
        ((void(*)(void))fn_8012640C)();
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
                fn_801FB1C0();
                r24 = (s32)r24 / (s32)r3;
                r3 = r27;
                r4 = r24;
                fn_802399FC();
                r27 = r3;
                r3 = r29;
                fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xdc;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc4;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc0;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xc8;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0xe4;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r27;
        r4 = r24;
        fn_802399FC();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
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
    fn_8024AFC4();
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
    fn_801FB1C0();
    r24 = (s32)r24 / (s32)r3;
    r3 = 0x0;
    r4 = r24;
    fn_802399FC();
    r25 = r3;
    r3 = r29;
    fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r25;
        r4 = r31;
        r5 = 0x105;
        fn_80239984();
        r25 = r3;
        r3 = r29;
        fn_80205B8C();
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
        fn_801FB1C0();
        r24 = (s32)r24 / (s32)r3;
        r3 = r25;
        r4 = r24;
        fn_802399FC();
        r25 = r3;
        r3 = r29;
        fn_80205B8C();
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

/* =========================================================================
 * fn_8024E690 - PostBattleProcessing
 *
 * Large post-battle handler (0x1224 = 4644 bytes).
 *
 * Processes the results of a completed battle:
 *   - Experience calculation and distribution
 *   - Shadow Pokemon purification progress
 *   - Prize money / Poke Coupon rewards
 *   - Story flag updates
 *   - Team state restoration (PP, status)
 * ========================================================================= */
/* TODO: Decompile fn_8024E690 (4644 bytes) */
void fn_8024E690(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A434[];
    extern u32 lbl_80478B38;
    extern void fn_8000815C();
    extern void fn_800E0C54();
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_801F4460();
    extern void fn_801F87CC();
    extern void fn_801F8C00();
    extern void fn_801FB1C0();
    extern void fn_801FCEC4();
    extern void fn_80204DE4();
    extern void fn_80205BE8();
    extern void fn_802062FC();
    extern void fn_802068C8();
    extern void fn_80235B04();
    extern void fn_802367CC();
    extern void fn_802369B8();
    extern void fn_8023793C();
    extern void fn_80238538();
    extern void fn_80238600();
    extern void fn_802386C8();
    extern void fn_8023881C();
    extern void fn_802389D4();
    extern void fn_80238B0C();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_8023943C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_802397B8();
    extern void fn_802398E4();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C530();
    extern void fn_8024FE80();
    u8 sp[0x860];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
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
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    f32 f8 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = 0x0;
    r5 = 0x43;
    r16 = r6;
    r15 = r3;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r21 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r21;
    r5 = 0x38;
    r6 = 0x0;
    fn_801FB1C0();
    r5 = (u32)lbl_8027A434;
    r0 = r3 & 0xFF;
    r5 = (u32)lbl_8027A434;
    r4 = 0x3;
    *(u8*)(sp + 0x7F0) = r0;
    r6 = (u32)sp + 0x14;
    r14 = 0x0;
    ctr_fn = (void(*)(void))r4;
    do {
        r3 = *(u32*)((u8*)r5 + 0x4);
        r0 = *(u32*)((u8*)r5 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r5 + 0x4);
    r3 = r15;
    r4 = 0x0;
    r5 = 0x1;
    *(u32*)((u8*)r6 + 0x4) = r0;
    fn_80235B04();
    r3 = (u32)sp + 0x68;
    r5 = 0x0;
    r4 = 0x0;
    while (1) {
        r0 = r5 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r5 = r5 + 0x1;
        *(u32*)(r3 + r0) = r4;

    }
    r4 = r16;
    r3 = (u32)sp + 0x110;
    fn_801FCEC4();
    r3 = r16;
    fn_802062FC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r16;
        r4 = 0xe2;
        r5 = 0x0;
        fn_80204DE4();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r14 = 0x1;
    }
    }
    r3 = r15;
    r4 = (u32)sp + 0x98;
    fn_801F87CC();
    r20 = r3;
    r4 = r15;
    r5 = (u32)sp + 0x34;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    r4 = r15;
    r5 = (u32)sp + 0xb0;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1A6C();
    r0 = r20 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r3 = -0x1;
        return;
    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x1f;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        r5 = (0x1 << 16);
        *(u32*)(sp + 0x8) = r0;
        r6 = r15;
        *(u32*)(sp + 0xC) = r0;
        r7 = 0x0;
        r8 = 0x0;
        *(u32*)(sp + 0x10) = r0;
        r9 = 0x0;
        r10 = 0x0;
        fn_8023A118();
        fn_800E0C54();
        r5 = r3 & 0xFFFF;
        r4 = r20 & 0xFFFF;
        r0 = (s32)r5 / (s32)r4;
        r3 = (u32)sp + 0x98;
        r0 = r0 * r4;
        r0 = r5 - r0;
        r17 = *(u32*)(r3 + r0);
        if (r17 != (u32)0x0) {
            r4 = r17;
            r3 = 0x0;
            fn_801F4460();
            r0 = r3;
            r3 = r17;
            r14 = r0;
            fn_80205BE8();
            r8 = 0x0;
            r5 = (0x1 << 16);
            r0 = 0x228;
            r7 = r3;
            r6 = r14;
            *(u32*)(sp + 0xC) = r0;
            r8 = 0x0;
            r9 = 0x0;
            r10 = 0x0;
            fn_8023A118();
            r3 = r17;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r3 = (s16)r3;
            return;
    }
    }
    r0 = r14 & 0xFF;
    if (r0 == (u32)0x1) {
        r14 = (u32)sp + 0x98;
        r17 = (u32)sp + 0x80;
        r18 = (u32)sp + 0x18;
        r19 = 0x0;
        r26 = 0x0;
        while (1) {
            r0 = r26 & 0xFFFF;
            if (r0 >= (u32)0xe) break;
            r4 = 0x0;
            r0 = r3 + 0x2;
            r23 = *(s16*)(r18 + r3);
            r24 = *(s16*)(r18 + r0);
            r3 = r4;
            while (1) {
                r0 = r4 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r4 = r4 + 0x1;
                *(u32*)(r17 + r0) = r3;

            }
            r22 = 0x0;
            r25 = r22;
            while (1) {
                r0 = r25 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r19 = *(u32*)(r14 + r0);
                if (r19 != (u32)0x0 || (s32)r23 != (s32)r0 || (s32)r24 != (s32)r0) {
                    r0 = (s16)r23;
                    if (r19 >= (u32)0x0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238600();
                        r0 = r3 & 0xFF;

                    }
                    r0 = (s16)r24;
                    if ((s32)r23 >= (s32)r0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238538();
                        r0 = r3 & 0xFF;

                    }
                    r3 = (u32)sp + 0x80;
                    *(u32*)(r3 + r0) = r19;
                    r22 = r22 + 0x1;
                }
                r25 = r25 + 0x1;

            }
            r0 = r22 & 0xFFFF;
            r19 = r22;
            if (r0 != (u32)0x6) break;
            r26 = r26 + 0x2;

        }

        r0 = r19 & 0xFFFF;
        if (r0 != (u32)0xe) {
            fn_800E0C54();
            r5 = r3 & 0xFFFF;
            r4 = r19 & 0xFFFF;
            r0 = (s32)r5 / (s32)r4;
            r3 = (u32)sp + 0x80;
            r0 = r0 * r4;
            r0 = r5 - r0;
            r14 = *(u32*)(r3 + r0);
            if (r14 != (u32)0x0) {
                r4 = r14;
                r3 = 0x0;
                fn_801F4460();
                r15 = r3;
                r3 = r14;
                fn_80205BE8();
                r8 = 0x0;
                r5 = (0x1 << 16);
                r0 = 0x228;
                r7 = r3;
                r6 = r15;
                *(u32*)(sp + 0xC) = r0;
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r14;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                r3 = (s16)r3;
                return;
    }
    }
    }
    r4 = (0xffff << 16);
    r3 = (0x1 << 16);
    r18 = r4 + 0x1;
    r22 = (u32)sp + 0x98;
    r17 = r20 & 0xFFFF;
    r19 = 0x0;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r17) break;
        r24 = *(u32*)(r22 + r0);
        if (r24 != (u32)0x0) {
            r3 = r24;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r0 = (s16)r3;
            if (r24 >= (u32)0x0) {
                r3 = r15;
                r4 = r24;
                fn_802386C8();
                r0 = r3;
                r3 = r15;
                r25 = r0;
                r4 = r24;
                fn_802389D4();
                r0 = r3;
                r3 = r24;
                r24 = r0;
                fn_80205BE8();
                r4 = 0x0;
                r5 = 0xc9;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                r3 = r3 & 0xFFFF;
                if (r19 < r25) {
                    r19 = r25;
                }
                if ((s32)r18 < (s32)r24) {
                    r18 = r24;
                }
                r0 = r14 & 0xFFFF;
                if (r0 > r3) {
                    r14 = r3;
        }
        }
        }
        r23 = r23 + 0x1;

    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x21;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r22 = (u32)sp + 0x98;
        r17 = r20 & 0xFFFF;
        r23 = 0x0;
        while (1) {
            r0 = r23 & 0xFFFF;
            if (r0 >= (u32)r17) break;
            r24 = *(u32*)(r22 + r0);
            if (r24 != (u32)0x0) {
                r3 = r24;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                r0 = (s16)r3;
                if (r24 >= (u32)0x0) {
                    r4 = r24;
                    r3 = 0x0;
                    fn_801F4460();
                    r3 = r24;
                    fn_80205BE8();
                    r4 = 0x0;
                    r5 = 0xc9;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                    r3 = r3 & 0xFFFF;
                    r0 = r14 & 0xFFFF;
                    if (r0 >= (u32)r3) {
                        r4 = r24;
                        r3 = 0x0;
                        fn_801F4460();
                        r14 = r3;
                        r3 = r24;
                        fn_80205BE8();
                        r8 = 0x0;
                        r5 = (0x1 << 16);
                        r0 = 0x228;
                        r7 = r3;
                        r6 = r14;
                        *(u32*)(sp + 0xC) = r0;
                        r8 = 0x0;
                        r9 = 0x0;
                        r10 = 0x0;
                        fn_8023A118();
                        r3 = r24;
                        r4 = 0x0;
                        r5 = 0xce;
                        r6 = 0x0;
                        ((void(*)(void))fn_8012640C)();
                        r3 = (s16)r3;
                        return;
            }
            }
            }
            r23 = r23 + 0x1;

        }
    }
    r0 = *(u8*)(sp + 0x7F0);
    r28 = 0x0;
    r0 = r3 + 0x1;
    *(u32*)(sp + 0x7F4) = r0;
    r0 = r20 & 0xFFFF;
    *(u32*)(sp + 0x808) = r0;
    while (1) {
        r0 = *(u32*)(sp + 0x808);
        r3 = r28 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = (u32)sp + 0x98;
        r27 = *(u32*)(r3 + r30);
        if (r27 != (u32)0x0) {
            r3 = r27;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r0 = (s16)r3;
            if (r27 >= (u32)0x0) {
                r4 = r27;
                r3 = 0x0;
                fn_801F4460();
                r26 = r3;
                r3 = r16;
                r4 = r27;
                r5 = 0x0;
                fn_802068C8();
                r3 = r15;
                r4 = r16;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802367CC();
                r0 = *(u32*)(sp + 0x7FC);
                r23 = r3;
                r31 = (u32)sp + 0x34;
                r14 = 0x0;
                r17 = r0 & 0xFFFF;
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r29 = *(u32*)(r31 + r0);
                    if (r29 != (u32)0x0) {
                        r22 = r23 & 0xFFFF;
                        r24 = 0x0;
                        while (1) {
                            r0 = r24 & 0xFFFF;
                            if (r0 >= (u32)r22) break;
                            r3 = (u32)sp + 0x54;
                            r5 = *(u16*)(r3 + r0);
                            if (r5 == (u32)0x0 || r5 == (u32)0x165 || r0 == (u32)0x1) {

                                r3 = r15;
                                r4 = r16;
                                r6 = r29;
                                fn_8023C530();
                                r0 = r3 & 0xFF;

                                r14 = 0x1;
                                break;
                            }
                            r24 = r24 + 0x1;

                        }

                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) break;
                    }
                    r25 = r25 + 0x1;

                }

                r3 = r16;
                r4 = (u32)sp + 0x110;
                fn_801FCEC4();
                r3 = r15;
                r4 = r27;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802369B8();
                r3 = r27;
                fn_80205BE8();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x227;
                r7 = r3;
                r29 = (u32)sp + 0x68;
                *(u32*)(sp + 0xC) = r0;
                r6 = r26;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r15;
                r4 = r27;
                fn_80238600();
                r17 = r3;
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1a;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1a;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1b;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1b;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1c;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1c;
                    fn_80239EE8();
                }
                r17 = 0x0;
                while (1) {
                    r0 = r17 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r4 = r21;
                    r6 = r17;
                    r3 = 0x0;
                    r5 = 0x39;
                    fn_801FB1C0();
                    r5 = r3 & 0xFFFF;
                    if (r5 != (u32)0x9) {
                        r3 = r15;
                        r4 = r27;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r4 = r21;
                            r6 = r17;
                            r3 = 0x0;
                            r5 = 0x3a;
                            fn_801FB1C0();
                            r22 = r3 & 0xFF;
                            r3 = *(u32*)(r29 + r30);
                            r4 = r22;
                            r5 = r15;
                            r6 = 0x1d;
                            fn_802398E4();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fn_80205BE8();
                            r6 = (0x1 << 16);
                            r5 = r3;
                            r4 = r15;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x1d;
                            fn_80239A40();
                    }
                    }
                    r17 = r17 + 0x1;

                }
                r3 = r15;
                r4 = r27;
                fn_8023881C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1e;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1e;
                    fn_80239EE8();
                }
                r0 = r14 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1f;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1f;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802386C8();
                if (r19 <= (u32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x21;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x21;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802389D4();
                if ((s32)r18 <= (s32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x20;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x20;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7FC);
                r24 = 0x0;
                r14 = r0 & 0xFFFF;
                while (1) {
                    r0 = r24 & 0xFFFF;
                    if (r0 >= (u32)r14) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r0 = *(u32*)(sp + 0x804);
                        r25 = 0x0;
                        r31 = r0 & 0xFFFF;
                        while (1) {
                            r0 = r25 & 0xFFFF;
                            if (r0 >= (u32)r31) break;
                            r3 = (u32)sp + 0x54;
                            r22 = *(u16*)(r3 + r0);
                            if (r22 != (u32)0x0) {
                                r3 = r15;
                                r4 = r22;
                                r5 = r16;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r17 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r22;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r22;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r23;
                                        r5 = r17;
                                        fn_8023793C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x22;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fn_80205BE8();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x22;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r25 = r25 + 0x1;

                        }
                    }
                    r24 = r24 + 0x1;

                }
                r0 = *(u32*)(sp + 0x7FC);
                r22 = 0x0;
                r31 = r0 & 0xFFFF;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r31) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r3 = r15;
                        r4 = r23;
                        r5 = (u32)sp + 0x54;
                        r6 = 0x0;
                        r7 = 0x0;
                        fn_802367CC();
                        r14 = r3 & 0xFFFF;
                        r17 = 0x0;
                        while (1) {
                            r0 = r17 & 0xFFFF;
                            if (r0 >= (u32)r14) break;
                            r3 = (u32)sp + 0x54;
                            r24 = *(u16*)(r3 + r0);
                            if (r24 != (u32)0x0) {
                                r3 = r15;
                                r4 = r24;
                                r5 = r23;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r25 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r24;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r24;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r27;
                                        r5 = r25;
                                        fn_80238B0C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x23;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fn_80205BE8();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x23;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r17 = r17 + 0x1;

                        }
                    }
                    r22 = r22 + 0x1;

                }
                r4 = r21;
                r3 = 0x0;
                r5 = 0x20;
                r6 = 0x0;
                fn_801FB1C0();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r17 = 0x0;
                    while (1) {
                        r0 = lbl_80478B38;
                        r3 = r17 & 0xFFFF;
                        if (r3 >= (u32)r0) break;
                        r0 = r17 & 0xFFFF;
                        if (r0 != (u32)0x9) {
                            r3 = r15;
                            r4 = r27;
                            r5 = r17;
                            r6 = 0x1;
                            fn_80238B0C();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0x41) {
                                r0 = *(u32*)(sp + 0x800);
                                r14 = (u32)sp + 0xb0;
                                r22 = 0x0;
                                r23 = r0 & 0xFFFF;
                                while (1) {
                                    r0 = r22 & 0xFFFF;
                                    if (r0 >= (u32)r23) break;
                                    r24 = *(u32*)(r14 + r0);
                                    if (r24 != (u32)0x0) {
                                        r4 = r24;
                                        r3 = 0x0;
                                        fn_801F4460();
                                        if (r3 != (u32)0x0) {
                                            r4 = r24;
                                            fn_801F8C00();
                                            r0 = r3 & 0xFF;
                                    }
                                    }
                                    if (r0 != (u32)0x1 || r0 != (u32)0x2 && r0 != (u32)0x3 || r0 != (u32)0x2 && r0 != (u32)0x3) {

                                        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

                                            r3 = r15;
                                            r4 = r24;
                                            r5 = r17;
                                            r6 = 0x1;
                                            fn_80238B0C();
                                            r0 = r3 & 0xFFFF;
                                            if (r0 == (u32)0x41) {
                                                r3 = *(u32*)(r29 + r30);
                                                r4 = r15;
                                                r5 = 0x24;
                                                fn_80239984();
                                                *(u32*)(r29 + r30) = r3;
                                                r3 = r27;
                                                fn_80205BE8();
                                                r7 = (0x1 << 16);
                                                r5 = r3;
                                                r4 = r26;
                                                r6 = 0x0;
                                                r7 = 0x0;
                                                r8 = 0x0;
                                                r9 = 0x0;
                                                r10 = 0x24;
                                                fn_80239EE8();
                            }
                                        }
                                    }
                                    r22 = r22 + 0x1;

                                }
                    }
                        }
                        r17 = r17 + 0x1;

                    }
                }
                r3 = r15;
                r4 = r27;
                fn_8024FE80();
                r0 = r3 & 0xFFFF;
                r14 = r3;
                if (r3 != (u32)r0) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = r14;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r10 = r14;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7F8);
                r0 = r0 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = r15;
                    r4 = r27;
                    r5 = 0x21;
                    fn_80239058();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x2c;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) goto L_8024F710;
                    }
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x29;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x29;
                    fn_80239EE8();

                } else {
                    if (r0 == (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x22;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2a;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fn_80205BE8();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2a;
                            fn_80239EE8();
                        }
                    } else {
                    if (r0 == (u32)0x3) {
                        r3 = r15;
                        r4 = r27;
                        r14 = 0x0;
                        r5 = 0x8;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r14 = 0x1;
                        }
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x8;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r15;
                            r4 = r27;
                            r5 = 0x5;
                            fn_80238E30();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x1) {
                                r3 = r15;
                                r4 = r27;
                                r5 = 0x4;
                                fn_80238E30();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                        }
                            }
                            r14 = 0x1;
                                }
                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2b;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fn_80205BE8();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2b;
                            fn_80239EE8();
                        }
                        goto L_8024F710;
                    }
                    }
                    if (r0 == (u32)0x4) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0xf;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2c;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fn_80205BE8();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2c;
                            fn_80239EE8();
                }
                    }
                }
            L_8024F710:
                fn_8000815C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    fn_800E0C54();
                    r0 = *(u32*)(sp + 0x7F4);
                    r5 = r3 & 0xFFFF;
                    r3 = *(u32*)(r29 + r30);
                    r4 = (s32)r5 / (s32)r0;
                    r0 = r4 * r0;
                    r4 = r5 - r0;
                    r0 = *(u8*)(sp + 0x7F0);
                    r14 = r4 - r0;
                    r4 = r14;
                    fn_802399FC();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fn_80205BE8();
                    r0 = 0x0;
                    r5 = (0x1 << 16);
                    *(u32*)(sp + 0x8) = r0;
                    r0 = 0x225;
                    r7 = r3;
                    r6 = r26;
                    *(u32*)(sp + 0xC) = r0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x0;
                    fn_8023A118();
                }
                r3 = r27;
                fn_80205BE8();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x226;
                r7 = r3;
                r6 = r26;
                *(u32*)(sp + 0xC) = r0;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                fn_8023A118();
            }
        }
        r28 = r28 + 0x1;

    }
    r4 = r20;
    r3 = (u32)sp + 0x68;
    r5 = 0x1;
    fn_802397B8();
    if ((s32)r3 < (s32)0x0) {
        r3 = -0x1;
        return;
    }
    r14 = r3 << 2;
    r3 = (u32)sp + 0x98;
    r15 = *(u32*)(r3 + r14);
    if (r15 == (u32)0x0) {
        r3 = -0x1;
        return;
    }
    r4 = r15;
    r3 = 0x0;
    fn_801F4460();
    r16 = r3;
    r3 = r15;
    fn_80205BE8();
    r0 = 0x0;
    r4 = (u32)sp + 0x68;
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x228;
    r5 = (0x1 << 16);
    r7 = r3;
    *(u32*)(sp + 0xC) = r0;
    r6 = r16;
    r8 = 0x0;
    r0 = *(u32*)(r4 + r14);
    r9 = 0x0;
    *(u32*)(sp + 0x10) = r0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r15;
    r4 = 0x0;
    r5 = 0xce;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r3 = (s16)r3;

    return;
}

/* =========================================================================
 * fn_8025A254 - ShadowPokemonCheck
 *
 * Small helper (0x3C bytes) that checks if a Pokemon at a given
 * party index is a Shadow Pokemon. Demonstrates the data access pattern:
 *
 *   PokemonSlotLookupDefault(0x02) -> Pokemon pointer
 *   fn_801F6E44(pointer, 0x4C) -> shadow status
 *   return (status != 2)
 *
 * This is one of the few functions small enough to fully reconstruct:
 * ========================================================================= */
BOOL ShadowPokemonCheck(void) {
    void* pokemon;
    u8 status;

    pokemon = (void*)PokemonSlotLookupDefault(0x02, 0);
    /* fn_801F6E44 reads a field at offset 0x4C, returning shadow status */
    /* status = fn_801F6E44(pokemon, 0x4C); */
    /* return (status != 2); */
    return FALSE; /* Placeholder */
}

/* =========================================================================
 * fn_8025A220 - CheckBattleCondition
 *
 * Small helper (0x34 bytes) that checks a specific battle condition
 * via CheckTrainerPokemonFlag with flagId 0x0F.
 *
 * return (!CheckTrainerPokemonFlag(r3, r4, 0x0F));
 * ========================================================================= */
BOOL CheckBattleCondition(void* context, u32 slot) {
    BOOL flagSet = CheckTrainerPokemonFlag(context, slot, 0x0F);
    return !flagSet;
}

/* =========================================================================
 * fn_8025A290 - ProcessBattleResult
 *
 * Medium helper (0xB0 bytes) that processes the result of a battle.
 * Calls BattleSequenceCheck and BattleResultCheck to determine
 * the outcome, then calls fn_80236C80 to update state.
 *
 * @param trainerCtx    Trainer context
 * @param trainerSlot   Slot index
 * @param resultSlot    Result query slot
 * @param resultType    Type of result to process
 * ========================================================================= */
/* fn_8025A290 | size: 0xB0 */
u32 fn_8025A290(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType) {
    extern u16 fn_8023793C();
    extern u32 fn_80239500();
    extern u32 fn_802395C8();
    extern u32 fn_8025C264();
    s32 resultVal;
    u32 statusVal;
    statusVal = fn_802395C8(trainerCtx, resultSlot, trainerSlot);
    resultVal = fn_8025C264(trainerCtx, trainerSlot, resultSlot, resultType, 0);
    if (fn_8023793C(trainerCtx, resultType, statusVal, fn_80239500(trainerCtx, resultSlot)) == 0x43) {
        resultVal = 0;
    }
    if (resultVal == 0) {
        return 0;
    }
    if (resultVal == -1) {
        return 1;
    }
    return 1;
}

/* =========================================================================
 * fn_8026316C - FinalCleanup
 *
 * Large function near the end of the range (0xA5C = 2652 bytes).
 * One of the last significant functions before .ctors at 0x80266360.
 *
 * Likely performs comprehensive cleanup after a Colosseum session:
 *   - Reset temporary state
 *   - Save progress
 *   - Release loaded resources
 *   - Restore overworld state
 * ========================================================================= */
/* TODO: Decompile fn_8026316C (2652 bytes) */
void fn_8026316C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_800119A8();
    extern void fn_80011A1C();
    extern void fn_80011D9C();
    extern void fn_80102568();
    extern void fn_80102620();
    extern void fn_801026A4();
    extern void fn_801EF634();
    extern void fn_801F0134();
    extern void fn_801F0204();
    extern void fn_801F0234();
    extern void fn_801F02AC();
    extern void fn_801F1700();
    extern void fn_801F18DC();
    extern void fn_801F1918();
    extern void fightSideGetStatus();
    extern void fn_801F7E60();
    extern void fn_801F9130();
    extern void fn_801F9790();
    extern void fn_801F981C();
    extern void fn_80205C24();
    extern void fn_80207760();
    extern void fn_8020E1A4();
    extern void fn_8020E204();
    extern void fn_80263BC8();
    extern void fn_80265924();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f3 = 0.0f;
    f32 f5 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r4;
    r29 = r3;
    r25 = r5;
    r3 = r31;
    fn_8020E204();
    fn_8020E1A4();
    r27 = r3 & 0xFF;
    r3 = 0x0;
    fn_801F18DC();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0) {
    do {
        r4 = r29;
        r5 = r31;
        r24 = 0x100;
        r3 = 0x2;
        fn_801F02AC();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0x5;
        r6 = 0x0;
        fightSideGetStatus();
        r26 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_801F0134();
        r0 = r3 & 0xFFFF;
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        fn_801F0234();
        fn_801F0204();
        if ((s32)r3 < (s32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = r26;
        r6 = r3 & 0xFFFF;
        r3 = 0x0;
        r5 = 0x2;
        fightSideGetStatus();
    } while (0);
    do {
        if ((s32)r3 != (s32)0xf3) {
            if ((s32)r3 < (s32)0xf3) {
                if ((s32)r3 != (s32)0xf1) {
                    if ((s32)r3 < (s32)0xf1) {
                        break;
                    }
                    if ((s32)r3 >= (s32)0xf5) break;
                    continue;
                    }
                r24 = 0x100;
                break;
                    }
            r24 = 0x101;
            break;
        }
        r24 = 0x102;
        break;

        r24 = 0x103;
    } while (0);
        r3 = r24;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        fn_801026A4();
    }
    r30 = 0x0;
    r26 = 0x0;
while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 < r27) continue;
    break;
    do {
        r3 = r29;
        r4 = r30;
        fn_801F981C();
        if ((s32)r3 == (s32)0xf5) {
            r26 = r30;
            break;
        }
        r4 = 0x1;
        fn_80205C24();
        r0 = r3 & 0xFF;
        if ((s32)r3 == (s32)0xf5) {
            r26 = r30;
            break;
        }
        fn_801EF634();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x1) {
    while (1) {
        r3 = r29;
        fn_801F9790();
        break;
        }
        r3 = 0x0;
        fn_801F1700();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            fn_80265924();
            r0 = r3 & 0xFF;
        }
        if (r0 == (u32)0x1) {
    while (1) {
        r3 = r28;
        fn_80207760();
        r3 = r29;
        r4 = r28;
        r5 = r31;
        fn_801F9130();
        r26 = r30;
        break;
        }
        r3 = 0x0;
        fn_801F18DC();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
        do {
            r4 = r28;
            r5 = r31;
            r3 = 0x2;
            fn_801F02AC();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r24 = r3 & 0xFFFF;
            r3 = r28;
            r4 = r31;
            fn_801F0134();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fn_801F0234();
            fn_801F0204();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r24;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x3;
            fightSideGetStatus();
        } while (0);
            r4 = 0x1;
            fn_80011D9C();
        }
        r0 = 0x0;
        *(u32*)(sp + 0x8) = r0;
    while (1) {
            r3 = r28;
            fn_80207760();
            r4 = r28;
            r5 = (u32)sp + 0xc;
            r3 = 0x0;
            fn_801F1918();
            r3 = 0x0;
            fn_801F18DC();
            *(u8*)(sp + 0x23) = r3;
            r3 = (u32)sp + 0xc;
            r4 = (u32)sp + 0x8;
            r5 = 0x1;
            fn_80011A1C();
            r24 = r3;
            fn_801EF634();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fn_801F02AC();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fn_801F0134();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fn_801F0234();
                    fn_801F0204();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    fn_80011D9C();
                }
                r3 = 0x1;
                fn_800119A8();
                continue;
            }
            r3 = 0x0;
            fn_801F1700();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                fn_80265924();
                r0 = r3 & 0xFF;
            }
            if (r0 == (u32)0x1 || (s32)r24 >= (s32)0x0) {

                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fn_801F02AC();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fn_801F0134();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fn_801F0234();
                    fn_801F0204();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    fn_80011D9C();
                }
                r3 = 0x1;
                fn_800119A8();
                continue;
            }
            if ((s32)r24 >= (s32)0x0) {
                r3 = 0x1;
                fn_800119A8();
            }
            r3 = r29;
            r4 = r28;
            r5 = r31;
            r6 = r24;
            fn_80263BC8();
            r24 = r3;
            fn_801EF634();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fn_801F02AC();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fn_801F0134();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fn_801F0234();
                    fn_801F0204();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    fn_80011D9C();
                }
                r3 = 0x1;
                fn_800119A8();
    }
            }
            r3 = 0x0;
            fn_801F1700();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                fn_80265924();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r0 = r24 & 0xFF;
            }
            }
            if (r0 != (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fn_801F02AC();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fn_801F0134();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fn_801F0234();
                    fn_801F0204();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    fn_80011D9C();
                }
                r3 = 0x1;
                fn_800119A8();
    }
            }
            r0 = r24 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r29;
                fn_801F7E60();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) continue;
                r3 = r30 & 0xFFFF;
                if (r0 != (u32)0x1) {
                    r30 = r26;
                    r3 = 0x0;
                    fn_801F18DC();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                    do {
                        r4 = r28;
                        r5 = r31;
                        r3 = 0x2;
                        fn_801F02AC();
                        if (r3 == (u32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        r4 = 0x0;
                        r5 = 0x5;
                        r6 = 0x0;
                        fightSideGetStatus();
                        r24 = r3 & 0xFFFF;
                        r3 = r28;
                        r4 = r31;
                        fn_801F0134();
                        r0 = r3 & 0xFFFF;
                        if (r3 == (u32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        fn_801F0234();
                        fn_801F0204();
                        if ((s32)r3 < (s32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        r4 = r24;
                        r6 = r3 & 0xFFFF;
                        r3 = 0x0;
                        r5 = 0x3;
                        fightSideGetStatus();
                    } while (0);
                        r4 = 0x0;
                        fn_80011D9C();
                    }
                    r3 = 0x1;
                    fn_800119A8();
                    continue;
                }
                r0 = r25 & 0xFF;
                if (r0 != (u32)0x1 || r3 != (u32)0x0) continue;

                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fn_801F02AC();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fn_801F0134();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fn_801F0234();
                    fn_801F0204();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    fn_80011D9C();
                }
                r3 = 0x1;
                fn_800119A8();
                r24 = 0x0;
                while (1) {
                    r0 = r24 & 0xFFFF;
                    if (r0 >= (u32)r27) break;
                    r3 = r29;
                    r4 = r24;
                    fn_801F981C();
                    if (r3 != (u32)0x0) {
                        r4 = 0x0;
                        r5 = 0x120;
                        r6 = 0x0;
                        r7 = 0x0;
                        ((void(*)(void))fn_801254B4)();
                    }
                    r24 = r24 + 0x1;

                }
                r3 = 0x0;
                return;
            }
            if (r0 == (u32)0x2) continue;
        break;
    }
        r3 = 0x0;
        fn_801F18DC();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
        do {
            r4 = r28;
            r5 = r31;
            r3 = 0x2;
            fn_801F02AC();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r24 = r3 & 0xFFFF;
            r3 = r28;
            r4 = r31;
            fn_801F0134();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fn_801F0234();
            fn_801F0204();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r24;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x3;
            fightSideGetStatus();
        } while (0);
            r4 = 0x0;
            fn_80011D9C();
        }
        r26 = r30;
    } while (0);
        r30 = r30 + 0x1;
        r0 = r30 & 0xFFFF;
        if (r0 < r27) continue;
    break;
}

    r3 = 0x0;
    fn_801F18DC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r27) {
    do {
        r4 = r29;
        r5 = r31;
        r24 = 0x100;
        r3 = 0x2;
        fn_801F02AC();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0x5;
        r6 = 0x0;
        fightSideGetStatus();
        r25 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_801F0134();
        r0 = r3 & 0xFFFF;
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        fn_801F0234();
        fn_801F0204();
        if ((s32)r3 < (s32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = r25;
        r6 = r3 & 0xFFFF;
        r3 = 0x0;
        r5 = 0x2;
        fightSideGetStatus();
    } while (0);
    do {
        if ((s32)r3 != (s32)0xf3) {
            if ((s32)r3 < (s32)0xf3) {
                if ((s32)r3 != (s32)0xf1) {
                    if ((s32)r3 < (s32)0xf1) {
                        break;
                    }
                    if ((s32)r3 >= (s32)0xf5) break;
                    continue;
                    }
                r24 = 0x100;
                break;
                    }
            r24 = 0x101;
            break;
        }
        r24 = 0x102;
        break;

        r24 = 0x103;
    } while (0);
        r3 = r24;
        fn_80102620();
        r0 = r3 & 0xFF;
        if ((s32)r3 != (s32)0xf5) {
        do {
            r4 = r29;
            r5 = r31;
            r24 = 0x100;
            r3 = 0x2;
            fn_801F02AC();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r25 = r3 & 0xFFFF;
            r3 = r29;
            r4 = r31;
            fn_801F0134();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fn_801F0234();
            fn_801F0204();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r25;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x2;
            fightSideGetStatus();
        } while (0);
        do {
            if ((s32)r3 != (s32)0xf3) {
                if ((s32)r3 < (s32)0xf3) {
                    if ((s32)r3 != (s32)0xf1) {
                        if ((s32)r3 < (s32)0xf1) {
                            break;
                        }
                        if ((s32)r3 >= (s32)0xf5) break;
                        continue;
                        }
                    r24 = 0x100;
                    break;
                        }
                r24 = 0x101;
                break;
            }
            r24 = 0x102;
            break;

            r24 = 0x103;
        } while (0);
            r3 = r24;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
    }
    }
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r27) break;
        r3 = r29;
        r4 = r24;
        fn_801F981C();
        if (r3 != (u32)0x0) {
            r4 = 0x0;
            r5 = 0x120;
            r6 = 0x0;
            r7 = 0x0;
            ((void(*)(void))fn_801254B4)();
        }
        r24 = r24 + 0x1;

    }
    r3 = 0x1;

    return;
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 120 functions matched
 * =================================================================== */

/* Address: 0x8024E52C | Size: 0x8 | Pattern: return_constant */
u32 fn_8024E52C(void) { return 0; }

/* Address: 0x80250980 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250980(void) { return 0; }

/* Address: 0x80250988 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250988(void) { return 0; }

/* Address: 0x80250990 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250990(void) { return 0; }

/* Address: 0x80250998 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250998(void) { return 0; }

/* Address: 0x80250A24 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250A24(void) { return 0; }

/* Address: 0x80250AB0 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250AB0(void) { return 0; }

/* Address: 0x80250AB8 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250AB8(void) { return 0; }

/* Address: 0x80250CE8 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250CE8(void) { return 0; }

/* Address: 0x80250D74 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250D74(void) { return 0; }

/* Address: 0x80250F54 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250F54(void) { return 0; }

/* Address: 0x80250F5C | Size: 0x8 | Pattern: return_constant */
u32 fn_80250F5C(void) { return 0; }

/* Address: 0x80250F64 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250F64(void) { return 0; }

/* Address: 0x80250F6C | Size: 0x8 | Pattern: return_constant */
u32 fn_80250F6C(void) { return 0; }

/* Address: 0x80250F74 | Size: 0x8 | Pattern: return_constant */
u32 fn_80250F74(void) { return 0; }

/* Address: 0x80251150 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251150(void) { return 0; }

/* Address: 0x802511D0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802511D0(void) { return 0; }

/* Address: 0x802511D8 | Size: 0x8 | Pattern: return_constant */
u32 fn_802511D8(void) { return 0; }

/* Address: 0x80251264 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251264(void) { return 0; }

/* Address: 0x8025126C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025126C(void) { return 0; }

/* Address: 0x80251274 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251274(void) { return 0; }

/* Address: 0x8025127C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025127C(void) { return 0; }

/* Address: 0x80251284 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251284(void) { return 0; }

/* Address: 0x8025128C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025128C(void) { return 0; }

/* Address: 0x80251294 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251294(void) { return 0; }

/* Address: 0x8025129C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025129C(void) { return 0; }

/* Address: 0x80251350 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251350(void) { return 0; }

/* Address: 0x802514C4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802514C4(void) { return 0; }

/* Address: 0x802514CC | Size: 0x8 | Pattern: return_constant */
u32 fn_802514CC(void) { return 0; }

/* Address: 0x802514D4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802514D4(void) { return 0; }

/* Address: 0x802514DC | Size: 0x8 | Pattern: return_constant */
u32 fn_802514DC(void) { return 0; }

/* Address: 0x802514E4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802514E4(void) { return 0; }

/* Address: 0x8025160C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025160C(void) { return 0; }

/* Address: 0x80251650 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251650(void) { return 0; }

/* Address: 0x80251680 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251680(void) { return 0; }

/* Address: 0x80251798 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251798(void) { return 0; }

/* Address: 0x80251B38 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251B38(void) { return 0; }

/* Address: 0x80251B40 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251B40(void) { return 0; }

/* Address: 0x80251B48 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251B48(void) { return 0; }

/* Address: 0x80251CDC | Size: 0x8 | Pattern: return_constant */
u32 fn_80251CDC(void) { return 0; }

/* Address: 0x80251CE4 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251CE4(void) { return 0; }

/* Address: 0x80251F64 | Size: 0x8 | Pattern: return_constant */
u32 fn_80251F64(void) { return 20; }

/* Address: 0x80252030 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252030(void) { return 0; }

/* Address: 0x80252140 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252140(void) { return 0; }

/* Address: 0x8025234C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025234C(void) { return 0; }

/* Address: 0x80252390 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252390(void) { return 0; }

/* Address: 0x80252468 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252468(void) { return 0; }

/* Address: 0x80252470 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252470(void) { return 0; }

/* Address: 0x80252478 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252478(void) { return 0; }

/* Address: 0x80252480 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252480(void) { return 0; }

/* Address: 0x80252488 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252488(void) { return 0; }

/* Address: 0x80252490 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252490(void) { return 0; }

/* Address: 0x80252498 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252498(void) { return 0; }

/* Address: 0x802524A0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802524A0(void) { return 0; }

/* Address: 0x802524A8 | Size: 0x8 | Pattern: return_constant */
u32 fn_802524A8(void) { return 0; }

/* Address: 0x802524B0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802524B0(void) { return 0; }

/* Address: 0x80252740 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252740(void) { return 0; }

/* Address: 0x802527BC | Size: 0x8 | Pattern: return_constant */
u32 fn_802527BC(void) { return 5; }

/* Address: 0x802528BC | Size: 0x8 | Pattern: return_constant */
u32 fn_802528BC(void) { return 0; }

/* Address: 0x802528C4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802528C4(void) { return 0; }

/* Address: 0x802528CC | Size: 0x8 | Pattern: return_constant */
u32 fn_802528CC(void) { return 0; }

/* Address: 0x802528D4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802528D4(void) { return 0; }

/* Address: 0x8025296C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025296C(void) { return 0; }

/* Address: 0x80252974 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252974(void) { return 0; }

/* Address: 0x802529C4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802529C4(void) { return 0; }

/* Address: 0x802529CC | Size: 0x8 | Pattern: return_constant */
u32 fn_802529CC(void) { return 0; }

/* Address: 0x802529D4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802529D4(void) { return 0; }

/* Address: 0x802529DC | Size: 0x8 | Pattern: return_constant */
u32 fn_802529DC(void) { return 0; }

/* Address: 0x802529E4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802529E4(void) { return 0; }

/* Address: 0x802529EC | Size: 0x8 | Pattern: return_constant */
u32 fn_802529EC(void) { return 0; }

/* Address: 0x80252A78 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252A78(void) { return 0; }

/* Address: 0x80252F1C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F1C(void) { return 0; }

/* Address: 0x80252F24 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F24(void) { return 0; }

/* Address: 0x80252F2C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F2C(void) { return 0; }

/* Address: 0x80252F34 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F34(void) { return 0; }

/* Address: 0x80252F3C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F3C(void) { return 0; }

/* Address: 0x80252F44 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F44(void) { return 0; }

/* Address: 0x80252F4C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F4C(void) { return 0; }

/* Address: 0x80252F54 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F54(void) { return 0; }

/* Address: 0x80252F5C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F5C(void) { return 0; }

/* Address: 0x80252F64 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F64(void) { return 0; }

/* Address: 0x80252F6C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F6C(void) { return 0; }

/* Address: 0x80252F74 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F74(void) { return 0; }

/* Address: 0x80252F7C | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F7C(void) { return 0; }

/* Address: 0x80252F84 | Size: 0x8 | Pattern: return_constant */
u32 fn_80252F84(void) { return 0; }

/* Address: 0x80253010 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253010(void) { return 0; }

/* Address: 0x80253018 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253018(void) { return 0; }

/* Address: 0x802531F0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802531F0(void) { return 40; }

/* Address: 0x80253344 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253344(void) { return 0; }

/* Address: 0x802533D0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802533D0(void) { return 0; }

/* Address: 0x80253484 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253484(void) { return 0; }

/* Address: 0x802534CC | Size: 0x8 | Pattern: return_constant */
u32 fn_802534CC(void) { return 0; }

/* Address: 0x80253510 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253510(void) { return 0; }

/* Address: 0x80253518 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253518(void) { return 0; }

/* Address: 0x80253520 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253520(void) { return 0; }

/* Address: 0x80253528 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253528(void) { return 0; }

/* Address: 0x80253530 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253530(void) { return 0; }

/* Address: 0x80253538 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253538(void) { return 0; }

/* Address: 0x80253540 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253540(void) { return 0; }

/* Address: 0x802535CC | Size: 0x8 | Pattern: return_constant */
u32 fn_802535CC(void) { return 0; }

/* Address: 0x802535D4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802535D4(void) { return 0; }

/* Address: 0x802535DC | Size: 0x8 | Pattern: return_constant */
u32 fn_802535DC(void) { return 0; }

/* Address: 0x802535E4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802535E4(void) { return 0; }

/* Address: 0x802535EC | Size: 0x8 | Pattern: return_constant */
u32 fn_802535EC(void) { return 0; }

/* Address: 0x802538B8 | Size: 0x8 | Pattern: return_constant */
u32 fn_802538B8(void) { return 0; }

/* Address: 0x80253948 | Size: 0x8 | Pattern: return_constant */
u32 fn_80253948(void) { return 0; }

/* Address: 0x80254678 | Size: 0x8 | Pattern: return_constant */
u32 fn_80254678(void) { return 1; }

/* Address: 0x80254E2C | Size: 0x8 | Pattern: return_constant */
u32 fn_80254E2C(void) { return 1; }

/* Address: 0x80255218 | Size: 0x8 | Pattern: return_constant */
u32 fn_80255218(void) { return 1; }

/* Address: 0x802552C8 | Size: 0x8 | Pattern: return_constant */
u32 fn_802552C8(void) { return 1; }

/* Address: 0x80255EE4 | Size: 0x8 | Pattern: return_constant */
u32 fn_80255EE4(void) { return 0; }

/* Address: 0x802564C0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802564C0(void) { return 1; }

/* Address: 0x8025746C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025746C(void) { return 1; }

/* Address: 0x802575C0 | Size: 0x8 | Pattern: return_constant */
u32 fn_802575C0(void) { return 1; }

/* Address: 0x80258134 | Size: 0x8 | Pattern: return_constant */
u32 fn_80258134(void) { return 1; }

/* Address: 0x802586F4 | Size: 0x8 | Pattern: return_constant */
u32 fn_802586F4(void) { return 1; }

/* Address: 0x802587B8 | Size: 0x8 | Pattern: return_constant */
u32 fn_802587B8(void) { return 1; }

/* Address: 0x8025B11C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025B11C(void) { return 1; }

/* Address: 0x8025C25C | Size: 0x8 | Pattern: return_constant */
u32 fn_8025C25C(void) { return 1; }

/* Address: 0x80262D34 | Size: 0x8 | Pattern: return_constant */
u32 fn_80262D34(void) { return 0; }

/* ===================================================================
 * EXPANDED FUNCTION COVERAGE
 * 560 additional functions for 0x80240000-0x80266360
 * =================================================================== */

/* -------------------------------------------------------------------
 * Battle Orchestration (0x80240000-0x8024D000)
 * 92 functions
 * ------------------------------------------------------------------- */

/* Address: 0x802400D8 | Size: 0x6C | Pattern: field_accessor */
u32 fn_802400D8(void* ctx, u32 slot, u16 species, u32 extra) {
    extern u32 fn_8023CA9C();
    extern u16 fn_8025CB3C();
    u16 currentSpecies;
    currentSpecies = fn_8025CB3C(ctx);
    if (currentSpecies == species || currentSpecies == 0) {
        return 0;
    }
    return fn_8023CA9C(ctx, slot, currentSpecies, extra);
}

/* Address: 0x80240144 | Size: 0xAC */
void fn_80240144(void* ctx, u32 param1, u32 param2) {
    extern void fn_800E0C54();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_802399FC();
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
    fn_801FB1C0();
    r31 = r3;
    fn_800E0C54();
    r5 = r3 & 0xFFFF;
    r4 = r31 + 0x1;
    r0 = (s32)r5 / (s32)r4;
    r3 = 0x0;
    r0 = r0 * r4;
    r30 = r5 - r0;
    r4 = r30;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
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
void fn_802401F0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_801F54A4();
    extern void fn_80205B8C();
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
    fn_801F1C18();
    r31 = r3;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x18;
    r6 = 0x0;
    fn_801F54A4();
    r0 = r3 & 0xFFFF;
    if (r0 >= (u32)0x2) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x1c7;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
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
                fn_80205B8C();
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
                fn_80205B8C();
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
            fn_80205B8C();
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
void fn_80240454(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6D9C();
    extern void fn_801F6E98();
    extern void fn_80205B8C();
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
    fn_801F025C();
    r4 = 0x4a;
    r28 = r3;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = 0x4a;
        fn_801F6D9C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_80205B8C();
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
u32 fn_802405C0(void* ctx, u32 param1, u32 param2) {
#pragma optimize_for_size on
    extern u32 fn_80205B8C(u32);
    extern u16 fn_80236B98(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80236B98(ctx) != 0) {
        u32 tmp = fn_80239984(0, ctx, 0x1c3);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c3);
    }
    return handle;
}

/* Address: 0x8024064C | Size: 0x13C (316 bytes) */
u32 fn_8024064C(void* ctx, u32 param1, u32 param2, u32 param3) {
#pragma optimize_for_size on
    typedef void (*BattleScriptCallback)();
    extern BattleScriptCallback fn_8011BEB4(u32, u16, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u16 fn_80236520(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern void fn_80242E4C();
    extern void fn_80242FEC();
    extern void fn_802430E4();
    extern void fn_80243178();
    extern void fn_80243284();
    extern void fn_80243390();
    extern void fn_8024349C();
    extern void fn_8024E52C();
    BattleScriptCallback callback;
    u32 setup;
    u16 species;

    setup = 0;
    species = fn_80236520(ctx, param3);
    if ((species != 0) && (species != 0xffff) && (species != 0x165) && (species != 0x163)) {
        callback = fn_8011BEB4(0, species, 0x1c, 0);
        if (callback == NULL) {
            callback = fn_8024E52C;
        }
        if ((callback == fn_8024349C) || (callback == fn_80243390) || (callback == fn_80243284)
            || (callback == fn_80243178) || (callback == fn_802430E4) || (callback == fn_80242FEC)
            || (callback == fn_80242E4C)) {
            setup = fn_80239984(0, ctx, 0x1c2);
            fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c2);
        }
    }
    return setup;
}

/* Address: 0x80240788 | Size: 0x448 (1096 bytes) */
void fn_80240788(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
void fn_80241660(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern void fn_801F0898();
    extern void fn_801F1170();
    extern void fn_801F1990();
    extern void fn_801F1C18();
    extern void fn_80202108();
    extern void fn_80202234();
    extern void fn_80205184();
    extern void fn_80205B8C();
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
    fn_801F1C18();
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
                        fn_801F1990();
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
        fn_80205B8C();
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
                        fn_80205B8C();
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
            ((void(*)(void))fn_8012640C)();

            fn_801F1170();
            r0 = r3 & 0xFF;

            r3 = r21;
            fn_801F0898();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x13) {
                r25 = 0x0;
                break;
            }
            r3 = *(u32*)(r27 + r22);
            fn_80205184();
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
        fn_80205B8C();
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
                fn_80205B8C();
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
            ((void(*)(void))fn_8012640C)();
            r0 = r3 & 0xFF;
            if (r29 != (u32)r3) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b4;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fn_80205B8C();
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
                fn_80205B8C();
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
void fn_80241B70(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6D9C();
    extern void fn_801F6E98();
    extern void fn_80205B8C();
    extern void fn_802357CC();
    extern void fn_802358AC();
    extern void fn_80235910();
    extern void fn_80235974();
    extern void fn_802359D8();
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r28 = r3;
    r26 = r6;
    r29 = r4;
    r30 = r5;
    r31 = 0x0;
    r4 = r26;
    r3 = 0x2;
    fn_801F025C();
    r4 = 0x4a;
    r27 = r3;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = 0x4a;
        fn_801F6D9C();
        r27 = r3;
    } else {

        r27 = 0x0;
    }
    r3 = r28;
    r4 = r26;
    fn_80235AA0();
    *(u8*)(sp + 0x8) = r3;
    r3 = r28;
    r4 = r26;
    fn_80235A3C();
    *(u8*)(sp + 0x9) = r3;
    r3 = r28;
    r4 = r26;
    fn_802359D8();
    *(u8*)(sp + 0xA) = r3;
    r3 = r28;
    r4 = r26;
    fn_80235974();
    *(u8*)(sp + 0xB) = r3;
    r3 = r28;
    r4 = r26;
    fn_80235910();
    *(u8*)(sp + 0xC) = r3;
    r3 = r28;
    r4 = r26;
    fn_802358AC();
    *(u8*)(sp + 0xD) = r3;
    r3 = r28;
    r4 = r26;
    fn_802357CC();
    *(u8*)(sp + 0xE) = r3;
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    while (1) {
        r0 = r4 & 0xFF;
        if (r0 >= (u32)0x7) break;
        r0 = r4 & 0xFF;
        r0 = *(u8*)(r3 + r0);
        if (r0 >= (u32)0x8 || r0 > (u32)0xc) {

            r0 = 0x1;
            break;
        }
        r4 = r4 + 0x1;

    }
    r0 = 0x0;

    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x1ac;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1ac;
        fn_80239EE8();
    }
    r0 = (s16)r27;
    if ((s32)r0 == (s32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x1ad;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1ad;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    if ((s32)r0 == (s32)0x2) {
        r3 = r31;
        r4 = r28;
        r5 = 0x1ae;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1ae;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    if ((s32)r0 != (s32)0x3) { r3 = r31; return; }
    r3 = r31;
    r4 = r28;
    r5 = 0x1af;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x1af;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x80241DE8 | Size: 0x1FC (508 bytes) */
s32 fn_80241DE8(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fn_801F1C18();
    extern void* fn_80205B8C();
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
    count = fn_801F1C18(0, ctx, buf, 0, 1);

    if ((u16)r3v != 0 && (u16)r3v != 0xffff && (u16)r3v != 0x165 && (u16)r3v != 0x163 &&
        fn_8023943C(ctx, r3v, 4) == 1) {
        handle = (s32)fn_80239984(0, ctx, 0x1a9);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1a9);
    }

    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, buf[i]);
        if (v == 0x12e || v == 0xd4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1aa);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1aa);
            break;
        }
    }

    if ((u16)r1v == 0x121 || r2v == 0x121) {
        handle = (s32)fn_80239984(handle, ctx, 0x1ab);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1ab);
    }
    return handle;
}

/* Address: 0x80241FE4 | Size: 0x2A8 (680 bytes) */
s32 fn_80241FE4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_801F1C18();
    extern void* fn_80205B8C();
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

    countA = fn_801F1C18(0, ctx, listA, 1, 1);
    countB = fn_801F1C18(0, ctx, listB, 0, 1);

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
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1A6);
    }

    for (i = 0; i < countA; i++) {
        if (param1 != listA[i] && fn_8023785C(ctx, listA[i]) == 2) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A7);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1A7);
            break;
        }
    }

    for (i = 0; i < countB; i++) {
        u16 v = fn_802377E8(ctx, listB[i]);
        if (v == 0x12E || v == 0xD4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A8);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1A8);
            break;
        }
    }

    return handle;
}

/* Address: 0x8024228C | Size: 0x3BC (956 bytes) */
void fn_8024228C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236BFC();
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

    r29 = r5;
    r30 = r6;
    r28 = r4;
    r27 = r3;
    r31 = 0x0;
    r4 = r30;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x19d;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x19d;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x19e;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x19e;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x19f;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x19f;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a0;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a0;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a1;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a1;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a2;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a2;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a3;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a3;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a4;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a4;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x1a5;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1a5;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80242648 | Size: 0xE8 (232 bytes) */
void fn_80242648(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_80236D60();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r6;
    r27 = r4;
    r26 = r3;
    r28 = r5;
    r4 = r29;
    r31 = 0x0;
    fn_80236520();
    r0 = r3;
    r3 = r26;
    r30 = r0;
    r4 = r27;
    r5 = r29;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r0 = r30 & 0xFFFF;
        if ((s32)r3 != (s32)0x0) {
            if (r0 != (u32)0xffff) {
                if (r0 != (u32)0x165) {
                    if (r0 != (u32)0x163) {
                        r3 = r26;
                        r4 = r30;
                        r5 = 0x1;
                        fn_8023943C();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r4 = r26;
                            r3 = 0x0;
                            r5 = 0x19c;
                            fn_80239984();
                            r0 = r3;
                            r3 = r27;
                            r31 = r0;
                            fn_80205B8C();
                            r6 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r8 = r28;
                            r6 = 0x0;
                            r7 = 0x0;
                            r9 = 0x0;
                            r10 = 0x19c;
                            fn_80239EE8();
    }
    }
    }
    }
    }
    }
    r3 = r31;
    return;
}

/* Address: 0x80242730 | Size: 0x170 (368 bytes) */
void fn_80242730(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern f32 lbl_8047E638;
    extern f32 lbl_8047E63C;
    extern void fn_80205B8C();
    extern void fn_802373B0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    f1 = lbl_8047E638;
    r30 = r5;
    r28 = r3;
    r29 = r4;
    r31 = 0x0;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x19b;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x19b;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    f1 = lbl_8047E63C;
    r3 = r28;
    r4 = r29;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x19a;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x19a;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    f1 = lbl_8047E630;
    r3 = r28;
    r4 = r29;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = r31; return; }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x199;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x199;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x802428A0 | Size: 0x134 (308 bytes) */
void fn_802428A0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_80236D60();
    extern void fn_802391E0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r6;
    r27 = r4;
    r26 = r3;
    r28 = r5;
    r4 = r29;
    r31 = 0x0;
    fn_80236520();
    r0 = r3 & 0xFFFF;
    r30 = r3;
    if ((s32)r0 == (s32)0) { r3 = r31; return; }
    if (r0 == (u32)0xffff) { r3 = r31; return; }
    if (r0 == (u32)0x165) { r3 = r31; return; }
    if (r0 == (u32)0x163) { r3 = r31; return; }
    r3 = r26;
    r4 = r27;
    r5 = r29;
    fn_80236D60();
    if ((s32)r3 <= (s32)0x0) { r3 = r31; return; }
    r3 = r26;
    r4 = r30;
    fn_802391E0();
    r0 = r3 & 0xFF;
    if (r0 <= (u32)0x5) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x197;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x197;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    if (r0 > (u32)0xa) { r3 = r31; return; }
    r4 = r26;
    r3 = 0x0;
    r5 = 0x198;
    fn_80239984();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r26;
    r8 = r28;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x198;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x802429D4 | Size: 0x17C (380 bytes) */
void fn_802429D4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236FFC();
    extern void fn_802370AC();
    extern void fn_8023715C();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r25 = r6;
    r30 = r4;
    r29 = r3;
    r31 = r5;
    r4 = r25;
    r26 = 0x0;
    fn_80236FFC();
    r28 = r3;
    r3 = r29;
    r4 = r25;
    fn_8023715C();
    r27 = r3;
    r3 = r29;
    r4 = r25;
    fn_802370AC();
    r4 = r27 & 0xFFFF;
    r0 = r28 & 0xFFFF;
    r28 = r3;
    if (r4 > r0) {
        r4 = r29;
        r3 = 0x0;
        r5 = 0x194;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x194;
        fn_80239EE8();
    }
    r3 = r27 & 0xFFFF;
    r0 = r28 & 0xFFFF;
    if (r3 > r0) {
        r3 = r26;
        r4 = r29;
        r5 = 0x195;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x195;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r25;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r29;
        r5 = 0x196;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x196;
        fn_80239EE8();
    }
    r3 = r26;
    return;
}

/* Address: 0x80242B50 | Size: 0x17C (380 bytes) */
void fn_80242B50(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236FFC();
    extern void fn_802370AC();
    extern void fn_8023715C();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r25 = r6;
    r30 = r4;
    r29 = r3;
    r31 = r5;
    r4 = r25;
    r26 = 0x0;
    fn_80236FFC();
    r28 = r3;
    r3 = r29;
    r4 = r25;
    fn_8023715C();
    r27 = r3;
    r3 = r29;
    r4 = r25;
    fn_802370AC();
    r4 = r28 & 0xFFFF;
    r0 = r27 & 0xFFFF;
    r28 = r3;
    if (r4 > r0) {
        r4 = r29;
        r3 = 0x0;
        r5 = 0x191;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x191;
        fn_80239EE8();
    }
    r3 = r27 & 0xFFFF;
    r0 = r28 & 0xFFFF;
    if (r3 > r0) {
        r3 = r26;
        r4 = r29;
        r5 = 0x192;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x192;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r25;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r29;
        r5 = 0x193;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x193;
        fn_80239EE8();
    }
    r3 = r26;
    return;
}

/* Address: 0x80242CCC | Size: 0xE4 (228 bytes) */
void fn_80242CCC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_802364BC();
    extern void fn_80236D60();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r6;
    r27 = r4;
    r26 = r3;
    r28 = r5;
    r4 = r29;
    r31 = 0x0;
    fn_802364BC();
    r0 = r3;
    r3 = r26;
    r30 = r0;
    r4 = r27;
    r5 = r29;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r0 = r30 & 0xFFFF;
        if ((s32)r3 != (s32)0x0) {
            if (r0 != (u32)0xffff) {
                if (r0 != (u32)0x165) {
                    if (r0 != (u32)0x163) {
                        r3 = r26;
                        r4 = r30;
                        r5 = 0x1;
                        fn_8023943C();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x163) {
                            r4 = r26;
                            r3 = 0x0;
                            r5 = 0x190;
                            fn_80239984();
                            r0 = r3;
                            r3 = r27;
                            r31 = r0;
                            fn_80205B8C();
                            r6 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r8 = r28;
                            r6 = 0x0;
                            r7 = 0x0;
                            r9 = 0x0;
                            r10 = 0x190;
                            fn_80239EE8();
    }
    }
    }
    }
    }
    }
    r3 = r31;
    return;
}

/* Address: 0x80242DB0 | Size: 0x9C */
void fn_80242DB0(void* ctx, u32 param1, u32 param2) {
    extern f32 lbl_8047E630;
    extern void fn_80205B8C();
    extern void fn_802373B0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;

    f1 = lbl_8047E630;
    r29 = r4;
    r30 = r5;
    r28 = r3;
    r4 = r6;
    r31 = 0x0;
    r5 = 0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x18f;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x18f;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80242E4C | Size: 0x1A0 (416 bytes) */
void fn_80242E4C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_80236F4C();
    extern void fn_80236FFC();
    extern void fn_802370AC();
    extern void fn_8023715C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
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
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r6;
    r28 = r4;
    r27 = r3;
    r29 = r5;
    r4 = r30;
    r31 = 0x0;
    fn_8023715C();
    r24 = r3;
    r3 = r27;
    r4 = r30;
    fn_80236FFC();
    r26 = r3;
    r3 = r27;
    r4 = r30;
    fn_802370AC();
    r25 = r3;
    r3 = r27;
    r4 = r30;
    fn_80236F4C();
    r4 = r24 & 0xFFFF;
    r0 = r26 & 0xFFFF;
    r26 = r3;
    if (r4 > r0) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x18c;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x18c;
        fn_80239EE8();
    }
    r3 = r25 & 0xFFFF;
    r0 = r26 & 0xFFFF;
    if (r3 > r0) {
        r3 = r31;
        r4 = r27;
        r5 = 0x18d;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x18d;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235AA0();
    r0 = r3 & 0xFF;
    if (r0 <= (u32)0x4) {
        r3 = r27;
        r4 = r30;
        fn_80235A3C();
        r0 = r3 & 0xFF;
        if (r0 <= (u32)0x4) {
            r3 = r31;
            r4 = r27;
            r5 = 0x18e;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x18e;
            fn_80239EE8();
    }
    }
    r3 = r31;
    return;
}

/* Address: 0x80242FEC | Size: 0xF8 (248 bytes) */
u32 fn_80242FEC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_802357CC();
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle = 0;

    if ((u8)fn_802357CC(ctx, param3) >= 7) {
        handle = fn_80239984(0, ctx, 0x18A);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x18A);
    }
    if ((u8)fn_802357CC(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0x18B);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x18B);
    }
    return handle;
}

/* Address: 0x802430E4 | Size: 0x94 */
u32 fn_802430E4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80205B8C(u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;

    setup = 0;
    if (fn_802358AC(ctx, param3) <= 4U) {
        setup = fn_80239984(0, ctx, 0x189);
        fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x189);
    }
    return setup;
}

/* Address: 0x80243178 | Size: 0x10C (268 bytes) */
u32 fn_80243178(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235974();
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count < (u16)limit) {
        handle = fn_80239984(0, arg0, 0x187);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x187);
    }
    if ((u8)fn_80235974(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x188);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x188);
    }
    return handle;
}

/* Address: 0x80243284 | Size: 0x10C (268 bytes) */
u32 fn_80243284(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235A3C();
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x185);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x185);
    }
    if ((u8)fn_80235A3C(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x186);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x186);
    }
    return handle;
}

/* Address: 0x80243390 | Size: 0x10C (268 bytes) */
u32 fn_80243390(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_8023715C();
    extern u32 fn_80236FFC();
    extern u32 fn_80235AA0();
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_8023715C(arg0, arg3);
    limit = fn_80236FFC(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x183);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x183);
    }
    if ((u8)fn_80235AA0(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x184);
        fn_80239EE8(0xEC64, arg0, fn_80205B8C(arg1), 0, 0, arg2, 0, 0x184);
    }
    return handle;
}

/* Address: 0x8024349C | Size: 0x138 (312 bytes) */
u32 fn_8024349C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_801F1C18(u32, void*, u32*, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[10];
    u32 setup;
    u16 count;
    u8 found;
    u16 index;

    found = 0;
    count = fn_801F1C18(0, ctx, entries, 1, 1);
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
        fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x181);
    } else {
        setup = fn_80239984(0, ctx, 0x182);
        fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x182);
    }
    return setup;
}

/* Address: 0x802435D4 | Size: 0xB8 */
void fn_802435D4(void* ctx, u32 param1, u32 param2) {
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_802376EC();
    extern void fn_802399FC();
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

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    fn_802376EC();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    fn_802376EC();
    r3 = r3 & 0xFFFF;
    r0 = r31 & 0xFFFF;
    r30 = (s32)r3 / (s32)r0;
    r3 = 0x0;
    r4 = 0x180;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r30 = r30 * r3;
    r3 = 0x0;
    r4 = r30;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x180;
    fn_80239CCC();
    r3 = r31;
    return;
}

/* Address: 0x8024368C | Size: 0x1AC (428 bytes) */
void fn_8024368C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_802387C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r3;
    r26 = r4;
    r31 = r5;
    r27 = r6;
    r28 = 0x0;
    fn_802387C8();
    r29 = r3;
    r3 = r30;
    r4 = r27;
    fn_802387C8();
    r0 = r29 * 0x3;
    if ((s32)r0 <= (s32)r3) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x17d;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x17d;
        fn_80239EE8();
        r3 = r28;
        return;
    }
    r0 = r29 << 1;
    if ((s32)r0 <= (s32)r3) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x17c;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x17c;
        fn_80239EE8();
        r3 = r28;
        return;
    }
    r0 = r3 * 0x3;
    if ((s32)r29 >= (s32)r0) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x17f;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x17f;
        fn_80239EE8();
        r3 = r28;
        return;
    }
    r0 = r3 << 1;
    if ((s32)r29 < (s32)r0) { r3 = r28; return; }
    r4 = r30;
    r3 = 0x0;
    r5 = 0x17e;
    fn_80239984();
    r0 = r3;
    r3 = r26;
    r28 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r30;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x17e;
    fn_80239EE8();

    r3 = r28;
    return;
}

/* Address: 0x80243838 | Size: 0x94 */
s32 fn_80243838(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern void* fn_80205B8C();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    s32 ret = 0;

    if (fn_80236BFC(ctx, param3, 0x17) == 0) {
        ret = (s32)fn_80239984(0, ctx, 0x17b);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x17b);
    }
    return ret;
}

/* Address: 0x802438CC | Size: 0x140 (320 bytes) */
u32 fn_802438CC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_801F0134(u32, u16);
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u16 fn_80201D84(u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;
    u16 current;

    setup = 0;
    current = fn_801F0134(param1, (u16)fn_801F54A4(0, 0, 0x14, 0));
    if (fn_80236BFC(ctx, param3, 0x1d) == 0) {
        setup = fn_80239984(0, ctx, 0x179);
        fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x179);
    }
    if (fn_80236BFC(ctx, param3, 0x1d) == 1) {
        if (current == fn_80201D84(param3, 0x1d)) {
            setup = fn_80239984(setup, ctx, 0x17a);
            fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x17a);
        }
    }
    return setup;
}

/* Address: 0x80243A0C | Size: 0x250 (592 bytes) */
void fn_80243A0C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern f32 lbl_8047E640;
    extern void fn_80205B8C();
    extern void fn_80235714();
    extern void fn_802373B0();
    extern void fn_80237DBC();
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
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r27 = r3;
    r28 = r4;
    r30 = 0x0;
    r5 = 0x7;
    fn_80237DBC();
    r31 = r3;
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        f1 = lbl_8047E630;
        r3 = r27;
        r4 = r28;
        r5 = 0x1;
        fn_802373B0();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r4 = r27;
            r3 = 0x0;
            r5 = 0x174;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x174;
            fn_80239EE8();
            goto L_80243B24;
    }
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x1) {
        f1 = lbl_8047E640;
        r3 = r27;
        r4 = r28;
        r5 = -0x1;
        fn_802373B0();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r4 = r27;
            r3 = 0x0;
            r5 = 0x175;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x175;
            fn_80239EE8();
    }
    }
L_80243B24:
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r28;
        fn_80235714();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r30;
            r4 = r27;
            r5 = 0x176;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x176;
            fn_80239EE8();
    }
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r27;
        r5 = 0x177;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x177;
        fn_80239EE8();
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r28;
        fn_80235714();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r30;
            r4 = r27;
            r5 = 0x178;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x178;
            fn_80239EE8();
    }
    }
    r3 = r30;
    return;
}

/* Address: 0x80243C5C | Size: 0x7C | Pattern: field_accessor */
s32 fn_80243C5C(void* ctx, u32 slot, u32 param) {
    extern s32 fn_80205B8C();
    extern s32 fn_80239984();
    extern void fn_80239EE8();
    s32 handle = fn_80239984(0, ctx, 0x173);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(slot), 0, 0, param, 0, 0x173);
    return handle;
}

/* Address: 0x80243CD8 | Size: 0x640 (1600 bytes) */
void fn_80243CD8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern f32 lbl_8047E630;
    extern void fn_8011BEB4();
    extern void fn_801F1990();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    extern void fn_8024B474();
    extern void fn_8024BFC0();
    extern void fn_8024E52C();
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
    fn_801F1C18();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
                r4 = (u32)fn_8024B474;
                r3 = (u32)fn_8024BFC0;
                r5 = (u32)fn_8024E52C;
                r15 = (u32)sp + 0xc;
                r19 = (u32)fn_8024B474;
                r20 = (u32)fn_8024BFC0;
                r18 = (u32)fn_8024E52C;
                r22 = 0x0;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r3 = 0x0;
                    r4 = *(u16*)(r15 + r0);
                    r5 = 0x1c;
                    r6 = 0x0;
                    fn_8011BEB4();
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
                        fn_80205B8C();
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

    r3 = (u32)fn_8024B474;
    r15 = 0x0;
    r16 = (u32)fn_8024B474;
    while (1) {
        r3 = lbl_80478DF8;
        r4 = r15 & 0xFFFF;
        r0 = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= (u32)r0) break;
        r4 = r15;
        r3 = 0x0;
        r5 = 0x1c;
        r6 = 0x0;
        fn_8011BEB4();
        if (r3 == (u32)0x0) {
            r3 = (u32)fn_8024E52C;
            r3 = (u32)fn_8024E52C;
        }
        if (r3 != (u32)r16) {
            r4 = (u32)fn_8024BFC0;
            r0 = (u32)fn_8024BFC0;
            if (r3 == (u32)r0) {
            }
            r4 = r31;
            r7 = r15;
            r3 = 0x0;
            r5 = 0x1;
            r6 = 0x1;
            r8 = 0x0;
            fn_801F1990();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r29;
                r4 = r31;
                r5 = 0x16d;
                fn_80239984();
                r0 = r3;
                r3 = r30;
                r29 = r0;
                fn_80205B8C();
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
        fn_80205B8C();
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
        ((void(*)(void))fn_8012640C)();
        if ((s32)r3 != (s32)0x0) {
            r3 = r29;
            r4 = r31;
            r5 = 0x16f;
            fn_80239984();
            r0 = r3;
            r3 = r30;
            r29 = r0;
            fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_80205B8C();
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
void fn_80244318(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_802357CC();
    extern void fn_80236BFC();
    extern void fn_80237DBC();
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

    r29 = r5;
    r30 = r6;
    r28 = r4;
    r27 = r3;
    r31 = 0x0;
    r4 = r30;
    r5 = 0x7;
    fn_80237DBC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x167;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x167;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_802357CC();
    r0 = r3 & 0xFF;
    if (r0 >= (u32)0x8) {
        r3 = r31;
        r4 = r27;
        r5 = 0x168;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x168;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x19;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x169;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x169;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80244478 | Size: 0x9C */
void fn_80244478(void* ctx, u32 param1, u32 param2) {
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r6 = 0x0;
    r29 = r4;
    r28 = r3;
    r30 = r5;
    r31 = 0x0;
    r3 = r29;
    r4 = 0x0;
    r5 = 0xed;
    ((void(*)(void))fn_8012640C)();
    r0 = r3 & 0xFFFF;
    if ((s32)r0 != (s32)0) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x166;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x166;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80244514 | Size: 0x18C (396 bytes) */
void fn_80244514(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8010C4A0();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_80236FFC();
    extern void fn_8023715C();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x50];
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

    r7 = 0x1;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r5 = (u32)sp + 0x8;
    r31 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fn_801F1C18();
    r25 = r3;
    r3 = r27;
    r4 = r30;
    fn_80236520();
    r24 = (u32)sp + 0x8;
    r23 = r3;
    r26 = r25 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r3 = r27;
        r4 = *(u32*)(r24 + r25);
        fn_8023715C();
        r4 = *(u32*)(r24 + r25);
        r25 = r3;
        r3 = r27;
        fn_80236FFC();
        r4 = r25 & 0xFFFF;
        r0 = r3 & 0xFFFF;
        if (r4 < r0) {
            r4 = r27;
            r3 = 0x0;
            r5 = 0x164;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x164;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r0 = r23 & 0xFFFF;
    if (r0 != (u32)r26) {
        if (r0 != (u32)0xffff) {
            if (r0 != (u32)0x165) {
                if (r0 != (u32)0x163) {
                    r3 = r27;
                    r4 = r23;
                    r5 = r30;
                    fn_802395C8();
                    fn_8010C4A0();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x2) {
                        r3 = r31;
                        r4 = r27;
                        r5 = 0x165;
                        fn_80239984();
                        r0 = r3;
                        r3 = r28;
                        r31 = r0;
                        fn_80205B8C();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r27;
                        r8 = r29;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x165;
                        fn_80239EE8();
    }
    }
    }
    }
    }
    r3 = r31;
    return;
}

/* Address: 0x802446A0 | Size: 0x18C (396 bytes) */
void fn_802446A0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8010C4A0();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_80236FFC();
    extern void fn_8023715C();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x50];
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

    r7 = 0x1;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r5 = (u32)sp + 0x8;
    r31 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fn_801F1C18();
    r25 = r3;
    r3 = r27;
    r4 = r30;
    fn_80236520();
    r24 = (u32)sp + 0x8;
    r23 = r3;
    r26 = r25 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r3 = r27;
        r4 = *(u32*)(r24 + r25);
        fn_8023715C();
        r4 = *(u32*)(r24 + r25);
        r25 = r3;
        r3 = r27;
        fn_80236FFC();
        r4 = r25 & 0xFFFF;
        r0 = r3 & 0xFFFF;
        if (r4 > r0) {
            r4 = r27;
            r3 = 0x0;
            r5 = 0x162;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x162;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r0 = r23 & 0xFFFF;
    if (r0 != (u32)r26) {
        if (r0 != (u32)0xffff) {
            if (r0 != (u32)0x165) {
                if (r0 != (u32)0x163) {
                    r3 = r27;
                    r4 = r23;
                    r5 = r30;
                    fn_802395C8();
                    fn_8010C4A0();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r3 = r31;
                        r4 = r27;
                        r5 = 0x163;
                        fn_80239984();
                        r0 = r3;
                        r3 = r28;
                        r31 = r0;
                        fn_80205B8C();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r27;
                        r8 = r29;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x163;
                        fn_80239EE8();
    }
    }
    }
    }
    }
    r3 = r31;
    return;
}

/* Address: 0x8024482C | Size: 0xD4 (212 bytes) */
void fn_8024482C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_80205B8C();
    extern void fn_80238748();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    r25 = r3;
    r26 = r4;
    r27 = r5;
    r5 = (u32)sp + 0x8;
    r4 = r25;
    r29 = 0x0;
    r3 = 0x0;
    fn_801F1A6C();
    r30 = (u32)sp + 0x8;
    r31 = r3 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r25;
        r4 = *(u32*)(r30 + r0);
        fn_80238748();
        r0 = r3 & 0xFF;
        if ((s32)r0 == (s32)0) {
            r4 = r25;
            r3 = 0x0;
            r5 = 0x161;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r29 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x161;
            fn_80239EE8();
            r3 = r29;
            return;
        }
        r28 = r28 + 0x1;

    }

    r3 = r29;
    return;
}

/* Address: 0x80244900 | Size: 0x8C */
u32 fn_80244900(void* ctx, u32 param1, u32 param2) {
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80237310(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80237310(ctx) == 0) {
        u32 tmp = fn_80239984(0, ctx, 0x160);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x160);
    }
    return handle;
}

/* Address: 0x8024498C | Size: 0x318 (792 bytes) */
void fn_8024498C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    fn_801F1A6C();
    r30 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
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
        fn_80205B8C();
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
            fn_80205B8C();
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
            fn_80205B8C();
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
            fn_80205B8C();
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
        fn_80205B8C();
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
void fn_80244CA4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_80235B04();
    extern void fn_802377E8();
    extern void fn_80238980();
    extern void fn_80238E30();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0xB0];
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
    fn_801F1A6C();
    r30 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    r0 = r29 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x4) {
        r4 = r25;
        r3 = 0x0;
        r5 = 0x156;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x156;
        fn_80239EE8();
    }
    r23 = (u32)sp + 0x28;
    r24 = r30 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = r25;
        r4 = *(u32*)(r23 + r0);
        r5 = 0xf;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r28;
            r4 = r25;
            r5 = 0x157;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x157;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r24 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r25;
        r4 = *(u32*)(r24 + r0);
        fn_802377E8();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x12f) {
            r3 = r28;
            r4 = r25;
            r5 = 0x158;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x158;
            fn_80239EE8();
            break;
        }
        r23 = r23 + 0x1;

    }

    r31 = (u32)sp + 0x28;
    r30 = r30 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r30) break;
        r3 = r25;
        r4 = *(u32*)(r31 + r0);
        fn_80238980();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x181) {
            r3 = r28;
            r4 = r25;
            r5 = 0x159;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x159;
            fn_80239EE8();
            break;
        }
        r23 = r23 + 0x1;

    }

    r0 = r29 & 0xFF;
    if (r0 == (u32)0x4) {
        r3 = r28;
        r4 = r25;
        r5 = 0x15a;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x15a;
        fn_80239EE8();
    }
    r3 = r28;
    return;
}

/* Address: 0x80244F68 | Size: 0x258 (600 bytes) */
void fn_80244F68(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_80205B8C();
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
    fn_801F1A6C();
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
        fn_80205B8C();
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
            fn_80205B8C();
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
            fn_80205B8C();
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
        fn_80205B8C();
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
void fn_802451C0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_80205B8C();
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
    fn_801F1A6C();
    r0 = r30 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x2) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x14e;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x14e;
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
        r5 = 0xb;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r4 = *(u32*)(r23 + r24);
            r3 = r26;
            r5 = 0x21;
            fn_80239058();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r4 = *(u32*)(r23 + r24);
                r3 = r26;
                r5 = 0x2c;
                fn_80239058();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
            }
            }
            r3 = r29;
            r4 = r26;
            r5 = 0x14f;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x14f;
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
            r5 = 0x150;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x150;
            fn_80239EE8();
            break;
        }
        r23 = r23 + 0x1;

    }

    r0 = r30 & 0xFF;
    if (r0 == (u32)0x2) {
        r3 = r29;
        r4 = r26;
        r5 = 0x151;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x151;
        fn_80239EE8();
    }
    r3 = r29;
    return;
}

/* Address: 0x80245418 | Size: 0x160 (352 bytes) */
u32 fn_80245418(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8012640C(u32, u32, u32, u32);
    extern u16 fn_801F1A6C(u32, void*, u32*, u32, u32);
    extern u32 fn_80205B8C(u32);
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
    count = fn_801F1A6C(0, ctx, entries, 0, 1);
    if (fn_80237310(ctx, param3) == 1) {
        setup = fn_80239984(0, ctx, 0x14c);
        fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x14c);
    }

    index = 0;
    while (index < count) {
        compareValue = fn_8012640C(param3, 0, 0xd5, 0);
        if (compareValue == entries[index]) {
            ;
        } else {
            if (fn_802384B4(ctx, entries[index], 8) == 1) {
                setup = fn_80239984(setup, ctx, 0x14d);
                fn_80239EE8(0xec64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x14d);
                break;
            }
        }
        index++;
    }

    return setup;
}

/* Address: 0x80245578 | Size: 0x1A0 (416 bytes) */
void fn_80245578(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_802373B0();
    extern void fn_8023831C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r3;
    r28 = r4;
    r31 = r5;
    r29 = 0x0;
    fn_8023831C();
    r0 = r3 & 0xFFFF;

    if (r0 == (u32)0x3 || r0 == (u32)0x9) {

        r4 = r30;
        r3 = 0x0;
        r5 = 0x148;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x148;
        fn_80239EE8();
    }
    f1 = lbl_8047E630;
    r3 = r30;
    r4 = r28;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r30;
        r5 = 0x149;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x149;
        fn_80239EE8();
    }
    r3 = r30;
    r4 = r28;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r30;
        r5 = 0x14a;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x14a;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x14b;
    fn_80239984();
    r29 = r3;
    r3 = r28;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r30;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x14b;
    fn_80239EE8();
    r3 = r29;
    return;
}

/* Address: 0x80245718 | Size: 0x98 */
void fn_80245718(void* ctx, u32 param1, u32 param2) {
    extern f32 lbl_8047E630;
    extern void fn_80205B8C();
    extern void fn_802373B0();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;

    f1 = lbl_8047E630;
    r30 = r5;
    r28 = r3;
    r29 = r4;
    r31 = 0x0;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x147;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x147;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x802457B0 | Size: 0x168 (360 bytes) */
u32 fn_802457B0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern u32 fn_80235B04();
    extern u8 fn_802373B0(void* a, u32 b, s32 c, f32 d);
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle = 0;
    u32 state;

    state = fn_80235B04(ctx, 0, 1);
    if ((u8)state == 1) {
        handle = fn_80239984(0, ctx, 0x144);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x144);
    }
    if ((u8)fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(handle, ctx, 0x145);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x145);
    }
    if ((u8)state == 2 || (u8)state == 4 || (u8)state == 3) {
        handle = fn_80239984(handle, ctx, 0x146);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x146);
    }
    return handle;
}

/* Address: 0x80245918 | Size: 0x98 */
u32 fn_80245918(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80236BFC(ctx, param3, 5) == 1) {
        u32 tmp = fn_80239984(0, ctx, 0x143);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x143);
    }
    return handle;
}

/* Address: 0x802459B0 | Size: 0x44C (1100 bytes) */
void fn_802459B0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F4354();
    extern void fn_801F8A18();
    extern void fn_80205B8C();
    extern void fn_80235BE4();
    extern void fn_80236BFC();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r29 = r6;
    r27 = r4;
    r28 = r5;
    r26 = r3;
    r5 = r29;
    r31 = 0x0;
    r4 = 0x0;
    r6 = 0x0;
    fn_80235BE4();
    r30 = r3;
    r4 = r29;
    r3 = 0x0;
    fn_801F4354();
    r0 = 0x0;
    r4 = (u32)sp + 0x8;
    *(u16*)(sp + 0x8) = r0;
    fn_801F8A18();
    if (r3 == (u32)0x0) {
        r30 = 0x1;
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x139;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x139;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13a;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13a;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13b;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13b;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13c;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13c;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13d;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13d;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13e;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13e;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x13f;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x13f;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x140;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x140;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x26;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x141;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x141;
        fn_80239EE8();
    }
    r0 = r30 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0x142;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x142;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80245DFC | Size: 0x14C (332 bytes) */
void fn_80245DFC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80235B04();
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

    r28 = r4;
    r29 = r5;
    r27 = r3;
    r30 = 0x0;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r31 = r3;
    r0 = r3 & 0xFF;

    if (r0 == (u32)0x1 || r0 == (u32)0x2) {

        r4 = r27;
        r3 = 0x0;
        r5 = 0x136;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x136;
        fn_80239EE8();
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x4) {
        r3 = r30;
        r4 = r27;
        r5 = 0x137;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x137;
        fn_80239EE8();
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = r30;
        r4 = r27;
        r5 = 0x138;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x138;
        fn_80239EE8();
    }
    r3 = r30;
    return;
}

/* Address: 0x80245F48 | Size: 0x7C | Pattern: field_accessor */
u32 fn_80245F48(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u16 fn_801363E8(u16);
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_8023CA9C(void*, u32, u16, u32);
    u16 value;

    value = fn_801363E8((u16)fn_801F54A4(0, 0, 0xf, 0));
    if (value != (u16)param) {
        return fn_8023CA9C(ctx, slot, value, extra);
    }
    return 0;
}

/* Address: 0x80247048 | Size: 0x7C | Pattern: field_accessor */
s32 fn_80247048(void* ctx, u32 slot, u32 param) {
    extern s32 fn_80205B8C();
    extern s32 fn_80239984();
    extern void fn_80239EE8();
    s32 handle = fn_80239984(0, ctx, 0x135);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(slot), 0, 0, param, 0, 0x135);
    return handle;
}

/* Address: 0x802470C4 | Size: 0xEC (236 bytes) */
void fn_802470C4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x133;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x133;
    fn_80239EE8();
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x134;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x134;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x802471B0 | Size: 0x128 (296 bytes) */
u32 fn_802471B0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u8 fn_80239564(void*, u32);
    extern u32 fn_802399FC(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    u32 battleParam = param2;
    u32 trainerParam = param1;
    void* battleCtx = ctx;
    s32 quotient;
    u32 setupHandle;
    u32 pokemonPtr;
    u32 statusValue;

    statusValue = fn_80239564(battleCtx, battleParam);
    quotient = (s32)statusValue / fn_801FB1C0(0, 0x131, 0x3e, 0);
    setupHandle = fn_802399FC(0, quotient);
    pokemonPtr = fn_80205B8C(trainerParam);
    fn_80239CCC(0xEC64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x131, quotient);

    if (fn_80235AA0(battleCtx, trainerParam) >= 0xcU) {
        statusValue = fn_80239564(battleCtx, battleParam);
        quotient = (s32)statusValue / fn_801FB1C0(0, 0x132, 0x3e, 0);
        setupHandle = fn_802399FC(setupHandle, quotient);
        pokemonPtr = fn_80205B8C(trainerParam);
        fn_80239CCC(0xEC64, battleCtx, pokemonPtr, 0, 0, battleParam, 0, 0x132, quotient);
    }

    return setupHandle;
}

/* Address: 0x802472D8 | Size: 0xDC (220 bytes) */
void fn_802472D8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80235A3C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x12f;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x12f;
    fn_80239EE8();
    r3 = r28;
    r4 = r29;
    fn_80235A3C();
    r0 = r3 & 0xFF;
    if (r0 >= (u32)0xc) {
        r3 = r31;
        r4 = r28;
        r5 = 0x130;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x130;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x802473B4 | Size: 0x144 (324 bytes) */
void fn_802473B4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_8023831C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r31 = 0x0;
    fn_8023831C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x17) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x12c;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x12c;
        fn_80239EE8();
    }
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x12d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x12d;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x12e;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x12e;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x802474F8 | Size: 0x1A8 (424 bytes) */
void fn_802474F8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_8023831C();
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

    r29 = r3;
    r30 = r4;
    r31 = r5;
    r27 = r6;
    r28 = 0x0;
    fn_8023831C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x17) {
        r4 = r29;
        r3 = 0x0;
        r5 = 0x128;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x128;
        fn_80239EE8();
    }
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = r29;
        r5 = 0x129;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x129;
        fn_80239EE8();
    }
    r3 = r28;
    r4 = r29;
    r5 = 0x12a;
    fn_80239984();
    r28 = r3;
    r3 = r30;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x12a;
    fn_80239EE8();
    r3 = r29;
    r4 = r27;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = r29;
        r5 = 0x12b;
        fn_80239984();
        r28 = r3;
        r3 = r30;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x12b;
        fn_80239EE8();
    }
    r3 = r28;
    return;
}

/* Address: 0x802476A0 | Size: 0x110 (272 bytes) */
s32 fn_802476A0(void* ctx, void* param1, u32 param2, u32 param3) {
    extern void* fn_80205B8C();
    extern u8 fn_80236BFC();
    extern u8 fn_80237F74();
    extern u16 fn_802383A4();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    s32 handle = 0;

    if (fn_80236BFC(ctx, param3, 0x3d) == 0 && fn_802383A4(ctx, param3) != 0) {
        handle = (s32)fn_80239984(0, ctx, 0x126);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x126);
    }
    if (fn_80237F74(ctx, param3, 0x3c) == 1) {
        handle = (s32)fn_80239984(handle, ctx, 0x127);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x127);
    }
    return handle;
}

/* Address: 0x802477B0 | Size: 0x158 (344 bytes) */
void fn_802477B0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80142984();
    extern void fn_80205B8C();
    extern void fn_80216048();
    extern void fn_80237F74();
    extern void fn_802383A4();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
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
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    r24 = r6;
    r27 = 0x0;
    fn_802383A4();
    r28 = r3;
    r3 = r29;
    r4 = r24;
    fn_802383A4();
    r0 = r3;
    r3 = r30;
    r25 = r0;
    r26 = 0x1;
    fn_80216048();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r26 = 0x0;
    }
    r0 = r28 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r0 = r25 & 0xFFFF;
        if (r0 != (u32)0xaf && r0 != (u32)0x0) {

            r3 = r25;
            fn_80142984();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
        }
        }
        r26 = 0x0;
            }
    r0 = r26 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r29;
        r3 = 0x0;
        r5 = 0x124;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x124;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r24;
    r5 = 0x3c;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        r5 = 0x125;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x125;
        fn_80239EE8();
    }
    r3 = r27;
    return;
}

/* Address: 0x80247908 | Size: 0x1C0 (448 bytes) */
void fn_80247908(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80235B04();
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

    r30 = r4;
    r31 = r5;
    r29 = r3;
    r27 = 0x0;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r28 = r3;
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r29;
        r3 = 0x0;
        r5 = 0x120;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x120;
        fn_80239EE8();
    }
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        r5 = 0x121;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x121;
        fn_80239EE8();
    }
    r0 = r28 & 0xFF;
    if (r0 != (u32)0x2 && r0 != (u32)0x4) {

        if (r0 == (u32)0x3) {
        }
        r3 = r27;
        r4 = r29;
        r5 = 0x122;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x122;
        fn_80239EE8();
        }
    r0 = r28 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = r27;
        r4 = r29;
        r5 = 0x123;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x123;
        fn_80239EE8();
    }
    r3 = r27;
    return;
}

/* Address: 0x80247AC8 | Size: 0x194 (404 bytes) */
void fn_80247AC8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
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

    r31 = r3;
    r27 = r4;
    r28 = r5;
    r29 = r6;
    r4 = r31;
    r3 = 0x0;
    r5 = 0x11d;
    fn_80239984();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r28;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x11d;
    fn_80239EE8();
    r3 = r31;
    r4 = r29;
    fn_80237310();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = r30;
        r4 = r31;
        r5 = 0x11e;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x11e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x13;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r29;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r4 = r29;
            r5 = 0x29;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r31;
                r4 = r29;
                r5 = 0x28;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) { r3 = r30; return; }
    }
    }
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x11f;
    fn_80239984();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r28;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x11f;
    fn_80239EE8();

    r3 = r30;
    return;
}

/* Address: 0x80247C5C | Size: 0x184 (388 bytes) */
void fn_80247C5C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
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

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r3 = 0x0;
    r5 = 0x11a;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x11a;
    fn_80239EE8();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x11b;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x11b;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x11;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x11c;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x11c;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x80247DE0 | Size: 0x1C0 (448 bytes) */
void fn_80247DE0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
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

    r29 = r3;
    r30 = r4;
    r31 = r5;
    r27 = r6;
    r4 = r29;
    r3 = 0x0;
    r5 = 0x116;
    fn_80239984();
    r0 = r3;
    r3 = r30;
    r28 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x116;
    fn_80239EE8();
    r3 = r28;
    r4 = r29;
    r5 = 0x117;
    fn_80239984();
    r28 = r3;
    r3 = r30;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x117;
    fn_80239EE8();
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = r29;
        r5 = 0x118;
        fn_80239984();
        r28 = r3;
        r3 = r30;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x118;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r27;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r29;
        r4 = r27;
        r5 = 0x11;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r29;
            r4 = r27;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r28; return; }
    }
    }
    r3 = r28;
    r4 = r29;
    r5 = 0x119;
    fn_80239984();
    r28 = r3;
    r3 = r30;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x119;
    fn_80239EE8();

    r3 = r28;
    return;
}

/* Address: 0x80247FA0 | Size: 0x1D0 (464 bytes) */
void fn_80247FA0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r5;
    r30 = r4;
    r29 = r3;
    r26 = r6;
    r4 = r31;
    fn_80239564();
    r27 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x113;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s32)r27 / (s32)r3;
    r3 = 0x0;
    r4 = r28;
    fn_802399FC();
    r0 = r3;
    r3 = r30;
    r27 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x113;
    fn_80239CCC();
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        r5 = 0x114;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x114;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r26;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r29;
        r4 = r26;
        r5 = 0x11;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r29;
            r4 = r26;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r27; return; }
    }
    }
    r3 = r29;
    r4 = r31;
    fn_80239564();
    r28 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x115;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s32)r28 / (s32)r3;
    r3 = r27;
    r4 = r28;
    fn_802399FC();
    r0 = r3;
    r3 = r30;
    r27 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x115;
    fn_80239CCC();

    r3 = r27;
    return;
}

/* Address: 0x80248170 | Size: 0x150 (336 bytes) */
u32 fn_80248170(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_801F1990(u32, void*, u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if ((fn_801F1990(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(0, ctx, 0x110);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x110);
    }
    if ((fn_80237F74(ctx, param1, 0x36) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x111);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x111);
    }
    handle = fn_80239984(handle, ctx, 0x112);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x112);
    return handle;
}

/* Address: 0x802482C0 | Size: 0x1A0 (416 bytes) */
s32 fn_802482C0(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fn_801F54A4();
    extern u32 fn_801F0134();
    extern u8 fn_80236BFC();
    extern u16 fn_80201D84();
    extern u8 fn_80237F74();
    extern u32 fn_80239984();
    extern void* fn_80205B8C();
    extern void fn_80239EE8();
    s32 h = 0;
    u32 r28v;

    r28v = fn_801F0134(param1, fn_801F54A4(0, 0, 0x14, 0));
    if (fn_80236BFC(ctx, param3, 0x1d) == 1 &&
        (u16)r28v == (u16)fn_80201D84(param3, 0x1d)) {
        h = fn_80239984(0, ctx, 0x10a);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x10a);
    }
    r28v = fn_80239984(h, ctx, 0x10b);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x10b);
    if (fn_80236BFC(ctx, param3, 9) == 1 || fn_80237F74(ctx, param3, 0x14) == 1) {
        r28v = fn_80239984(r28v, ctx, 0x10c);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x10c);
    }
    return r28v;
}

/* Address: 0x80248460 | Size: 0x22C (556 bytes) */
s32 fn_80248460(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_801F0134();
    extern u16 fn_801F1C18();
    extern u16 fn_801F54A4();
    extern u16 fn_80201D84();
    extern void* fn_80205B8C();
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

    someVal = fn_801F0134(param1, fn_801F54A4(0, 0, 0x14, 0));
    count = fn_801F1C18(0, ctx, buf, 1, 1);

    for (i = 0; i < count; i++) {
        if (buf[i] != 0 && fn_80236D60(ctx, param3, buf[i]) > 0) {
            break;
        }
    }

    if (i < count) {
        handle = (s32)fn_80239984(0, ctx, 0x107);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x107);
    }

    if (fn_80236BFC(ctx, param3, 0x1D) == 1 && someVal == fn_80201D84(param3, 0x1D)) {
        handle = (s32)fn_80239984(handle, ctx, 0x108);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x108);
    }

    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 7) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        handle = (s32)fn_80239984(handle, ctx, 0x109);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x109);
    }

    return handle;
}

/* Address: 0x8024868C | Size: 0x1D0 (464 bytes) */
void fn_8024868C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r5;
    r30 = r4;
    r29 = r3;
    r26 = r6;
    r4 = r31;
    fn_80239564();
    r27 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x104;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s32)r27 / (s32)r3;
    r3 = 0x0;
    r4 = r28;
    fn_802399FC();
    r0 = r3;
    r3 = r30;
    r27 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x104;
    fn_80239CCC();
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        r5 = 0x105;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r27 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x105;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r26;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r29;
        r4 = r26;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r29;
            r4 = r26;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r27; return; }
    }
    }
    r3 = r29;
    r4 = r31;
    fn_80239564();
    r28 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x106;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s32)r28 / (s32)r3;
    r3 = r27;
    r4 = r28;
    fn_802399FC();
    r0 = r3;
    r3 = r30;
    r27 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x106;
    fn_80239CCC();

    r3 = r27;
    return;
}

/* Address: 0x8024885C | Size: 0x2C0 (704 bytes) */
void fn_8024885C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801F1A6C();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_802384B4();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
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

    r7 = 0x1;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r5 = (u32)sp + 0x10;
    r3 = 0x0;
    r6 = 0x0;
    fn_801F1A6C();
    r26 = r3;
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r25 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x100;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r25 = (s32)r25 / (s32)r3;
    r3 = 0x0;
    r4 = r25;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x100;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x101;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x101;
        fn_80239EE8();
    }
    r25 = (u32)sp + 0x10;
    r26 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r3 = r30;
        r4 = 0x0;
        r5 = 0xd5;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r4 = *(u32*)(r25 + r0);
        if (r3 != (u32)r4) {
            r3 = r27;
            r5 = 0x7;
            fn_802384B4();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r27;
                r4 = r29;
                fn_80239564();
                r25 = r3 & 0xFF;
                r3 = 0x0;
                r4 = 0x102;
                r5 = 0x3e;
                r6 = 0x0;
                fn_801FB1C0();
                r25 = (s32)r25 / (s32)r3;
                r3 = r31;
                r4 = r25;
                fn_802399FC();
                r0 = r3;
                r3 = r28;
                r31 = r0;
                fn_80205B8C();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r27;
                r8 = r29;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x102;
                fn_80239CCC();
                break;
        }
        }
        r24 = r24 + 0x1;

    }

    r3 = r27;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r26) {
        r3 = r27;
        r4 = r30;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r25 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0x103;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r25 = (s32)r25 / (s32)r3;
    r3 = r31;
    r4 = r25;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x103;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x80248B1C | Size: 0x220 (544 bytes) */
void fn_80248B1C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_80237F74();
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

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r31 = r6;
    r4 = r28;
    r3 = 0x0;
    r5 = 0xfb;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r27 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xfb;
    fn_80239EE8();
    r3 = r27;
    r4 = r28;
    r5 = 0xfc;
    fn_80239984();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xfc;
    fn_80239EE8();
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r28;
        r5 = 0xfd;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xfd;
        fn_80239EE8();
    }
    r3 = r28;
    r4 = r31;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r28;
        r5 = 0xfe;
        fn_80239984();
        r27 = r3;
        r3 = r29;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xfe;
        fn_80239EE8();
    }
    r3 = r28;
    r4 = r31;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r28;
        r4 = r31;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r28;
            r4 = r31;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r27; return; }
    }
    }
    r3 = r27;
    r4 = r28;
    r5 = 0xff;
    fn_80239984();
    r27 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xff;
    fn_80239EE8();

    r3 = r27;
    return;
}

/* Address: 0x80248D3C | Size: 0x288 (648 bytes) */
void fn_80248D3C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r27 = r3;
    r28 = r4;
    r30 = r6;
    r31 = 0x0;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0xf6;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf6;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xf7;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r31 = r3;
    r3 = r28;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xf7;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xf8;
        fn_80239984();
        r31 = r3;
        r3 = r28;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf8;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xf9;
        fn_80239984();
        r31 = r3;
        r3 = r28;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf9;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x13;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xfa;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r30 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r30;
    fn_802399FC();
    r31 = r3;
    r3 = r28;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xfa;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x80248FC4 | Size: 0x49C (1180 bytes) */
void fn_80248FC4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F4354();
    extern void fn_801F8A18();
    extern void fn_80205B8C();
    extern void fn_80235BE4();
    extern void fn_80236BFC();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r6;
    r26 = r4;
    r27 = r5;
    r31 = r3;
    r5 = r28;
    r30 = 0x0;
    r4 = 0x0;
    r6 = 0x0;
    fn_80235BE4();
    r29 = r3;
    r4 = r28;
    r3 = 0x0;
    fn_801F4354();
    r0 = 0x0;
    r4 = (u32)sp + 0x8;
    *(u16*)(sp + 0x8) = r0;
    fn_801F8A18();
    if (r3 == (u32)0x0) {
        r29 = 0x1;
    }
    r0 = r29 & 0xFF;
    if (r3 == (u32)0x0) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0xeb;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xeb;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xec;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xec;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xed;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xed;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xee;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xee;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xef;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xef;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf0;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf0;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf1;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf1;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf2;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf2;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf3;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf3;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf4;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf4;
        fn_80239EE8();
    }
    r0 = r29 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0xf5;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xf5;
        fn_80239EE8();
    }
    r3 = r30;
    return;
}

/* Address: 0x80249460 | Size: 0x218 (536 bytes) */
void fn_80249460(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xe7;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xe7;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xe8;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe8;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x7;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xe9;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe9;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xea;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r30 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r30;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xea;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x80249678 | Size: 0x248 (584 bytes) */
void fn_80249678(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xe3;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xe3;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xe4;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe4;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xe5;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = r31;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe5;
        fn_80239CCC();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x13;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xe6;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xe6;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x802498C0 | Size: 0x1F4 (500 bytes) */
void fn_802498C0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_802358AC();
    extern void fn_80237F74();
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

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r3 = 0x0;
    r5 = 0xdf;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xdf;
    fn_80239EE8();
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = r31;
        r4 = r27;
        r5 = 0xe0;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe0;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r27;
                r4 = r30;
                r5 = 0x33;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
        }
        }
        }
        r3 = r31;
        r4 = r27;
        r5 = 0xe1;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe1;
        fn_80239EE8();
                }
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    r0 = r3 & 0xFF;
    if (r0 <= (u32)0x4) {
        r3 = r31;
        r4 = r27;
        r5 = 0xe2;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xe2;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x80249AB4 | Size: 0x278 (632 bytes) */
void fn_80249AB4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_802358AC();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xdb;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xdb;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xdc;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xdc;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_802358AC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xdd;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = r31;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xdd;
        fn_80239CCC();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r27;
                r4 = r30;
                r5 = 0x33;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xde;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xde;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x80249D2C | Size: 0x25C (604 bytes) */
void fn_80249D2C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80235974();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xd7;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xd7;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xd8;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd8;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235974();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xd9;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = r31;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd9;
        fn_80239CCC();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xda;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xda;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x80249F88 | Size: 0x1E8 (488 bytes) */
void fn_80249F88(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_802359D8();
    extern void fn_80237F74();
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

    r29 = r3;
    r30 = r4;
    r31 = r5;
    r27 = r6;
    r4 = r29;
    r3 = 0x0;
    r5 = 0xd3;
    fn_80239984();
    r0 = r3;
    r3 = r30;
    r28 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xd3;
    fn_80239EE8();
    r4 = r29;
    r8 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = r29;
        r5 = 0xd4;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd4;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r27;
    fn_802359D8();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = r29;
        r5 = 0xd5;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r28 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r29;
        r8 = r31;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd5;
        fn_80239EE8();
    }
    r3 = r29;
    r4 = r27;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r29;
        r4 = r27;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r29;
            r4 = r27;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r28; return; }
    }
    }
    r3 = r28;
    r4 = r29;
    r5 = 0xd6;
    fn_80239984();
    r0 = r3;
    r3 = r30;
    r28 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r29;
    r8 = r31;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xd6;
    fn_80239EE8();

    r3 = r28;
    return;
}

/* Address: 0x8024A170 | Size: 0x2B8 (696 bytes) */
void fn_8024A170(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    fn_801F1C18();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
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
                        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_80205B8C();
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
void fn_8024A428(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    fn_801F1C18();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
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
                        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_80205B8C();
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
void fn_8024A664(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80235910();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xc7;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc7;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xc8;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc8;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xc9;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc9;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235910();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xca;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = r31;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xca;
        fn_80239CCC();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xcb;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xcb;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x8024A924 | Size: 0x25C (604 bytes) */
void fn_8024A924(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80235A3C();
    extern void fn_80237F74();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r5;
    r28 = r4;
    r27 = r3;
    r30 = r6;
    r4 = r29;
    fn_80239564();
    r26 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xc3;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r26 / (s32)r3;
    r3 = 0x0;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc3;
    fn_80239CCC();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xc4;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc4;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xc5;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = r31;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc5;
        fn_80239CCC();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    r3 = r27;
    r4 = r29;
    fn_80239564();
    r30 = r3 & 0xFF;
    r3 = 0x0;
    r4 = 0xc6;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s32)r30 / (s32)r3;
    r3 = r31;
    r4 = r26;
    fn_802399FC();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc6;
    fn_80239CCC();

    r3 = r31;
    return;
}

/* Address: 0x8024AB80 | Size: 0x204 (516 bytes) */
void fn_8024AB80(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80235AA0();
    extern void fn_80237F74();
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

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r4 = r27;
    r3 = 0x0;
    r5 = 0xbf;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xbf;
    fn_80239EE8();
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xc0;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc0;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    fn_80235AA0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xc1;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xc1;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r27;
                r4 = r30;
                r5 = 0x34;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) { r3 = r31; return; }
    }
    }
    }
    r3 = r31;
    r4 = r27;
    r5 = 0xc2;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xc2;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x8024AD84 | Size: 0x16C (364 bytes) */
void fn_8024AD84(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80237F74();
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

    r28 = r4;
    r27 = r3;
    r29 = r5;
    r30 = r6;
    r3 = r28;
    r31 = 0x0;
    r4 = 0x0;
    r5 = 0xed;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r0 = r3 & 0xFFFF;
    if ((s32)r0 != (s32)0) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0xbd;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xbd;
        fn_80239EE8();
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x27;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xbe;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xbe;
        fn_80239EE8();
        r3 = r31;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x13;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = r31; return; }
    r3 = r31;
    r4 = r27;
    r5 = 0xbe;
    fn_80239984();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xbe;
    fn_80239EE8();

    r3 = r31;
    return;
}

/* Address: 0x8024AEF0 | Size: 0xD4 (212 bytes) */
void fn_8024AEF0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r5;
    r28 = r3;
    r29 = r4;
    r5 = r6;
    r31 = 0x0;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0xbb;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xbb;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0xbc;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xbc;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x8024AFC4 | Size: 0x4B0 (1200 bytes) */
void fn_8024AFC4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239564();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r6;
    r29 = r5;
    r27 = r3;
    r28 = r4;
    r5 = r30;
    r31 = 0x0;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r3 = r27;
        r4 = r29;
        fn_80239564();
        r26 = r3 & 0xFF;
        r3 = 0x0;
        r4 = 0xb0;
        r5 = 0x3e;
        r6 = 0x0;
        fn_801FB1C0();
        r26 = (s32)r26 / (s32)r3;
        r3 = 0x0;
        r4 = r26;
        fn_802399FC();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xb0;
        fn_80239CCC();
        r3 = r27;
        r4 = r30;
        r5 = 0x3;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb1;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb1;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x4;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb2;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb2;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x6;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb3;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb3;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x1c;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb4;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb4;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x18;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb5;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb5;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x5;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb6;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb6;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x9;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb7;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb7;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0xa;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb8;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb8;
            fn_80239EE8();
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x1e;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r27;
            r5 = 0xb9;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r31 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xb9;
            fn_80239EE8();
    }
    }
    r4 = r27;
    r8 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0xba;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xba;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x8024B474 | Size: 0x5D0 (1488 bytes) */
void fn_8024B474(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    fn_801F1A6C();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1C18();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
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
                fn_80205B8C();
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
                fn_80205B8C();
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
                        fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xac;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
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
        fn_801F1990();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fn_80205B8C();
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
        fn_801F1990();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xad;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
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
                fn_80205B8C();
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
                fn_80205B8C();
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
void fn_8024BA44(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F4354();
    extern void fn_801F8A18();
    extern void fn_80205B8C();
    extern void fn_80235BE4();
    extern void fn_80236BFC();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r29 = r6;
    r27 = r4;
    r28 = r5;
    r26 = r3;
    r5 = r29;
    r31 = 0x0;
    r4 = 0x0;
    r6 = 0x0;
    fn_80235BE4();
    r30 = r3;
    r4 = r29;
    r3 = 0x0;
    fn_801F4354();
    r0 = 0x0;
    r4 = (u32)sp + 0x8;
    *(u16*)(sp + 0x8) = r0;
    fn_801F8A18();
    if (r3 == (u32)0x0) {
        r30 = 0x1;
    }
    r0 = r30 & 0xFF;
    if (r3 == (u32)0x0) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x9f;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x9f;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa0;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa0;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa1;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa1;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa2;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa2;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa4;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa4;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa5;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa5;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa6;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa6;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa7;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa7;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa3;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa3;
        fn_80239EE8();
    }
    r0 = r30 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r31;
        r4 = r26;
        r5 = 0xa8;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xa8;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x8024BE7C | Size: 0x144 (324 bytes) */
void fn_8024BE7C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236BFC();
    extern void fn_802387C8();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239CCC();
    extern void fn_80239EE8();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r5;
    r31 = r3;
    r26 = r4;
    r29 = 0x0;
    r5 = 0x1a;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x9c;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r29 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x9c;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r26;
    fn_802387C8();
    r30 = r3;
    r3 = 0x0;
    r4 = 0x9d;
    r5 = 0x3e;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s32)r30 / (s32)r3;
    r3 = r29;
    r4 = r28;
    fn_802399FC();
    r30 = r3;
    r3 = r26;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r27;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x9d;
    fn_80239CCC();
    r3 = r30;
    r4 = r31;
    r5 = 0x9e;
    fn_80239984();
    r30 = r3;
    r3 = r26;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r27;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x9e;
    fn_80239EE8();
    r3 = r30;
    return;
}

/* Address: 0x8024BFC0 | Size: 0x5FC (1532 bytes) */
void fn_8024BFC0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
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
    fn_801F1A6C();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1C18();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
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
                fn_80205B8C();
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
                fn_80205B8C();
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
                        fn_80205B8C();
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
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0x97;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
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
        fn_801F1990();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fn_80205B8C();
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
        fn_801F1990();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fn_80205B8C();
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
            fn_80205B8C();
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
                fn_80205B8C();
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
                fn_80205B8C();
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
void fn_8024C5BC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fn_8025C5A4();
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
    fn_801F1C18();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
    fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
                fn_8025C5A4();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r27;
                    r4 = r31;
                    r5 = 0x92;
                    fn_80239984();
                    r27 = r3;
                    r3 = r30;
                    fn_80205B8C();
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
        fn_80205B8C();
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
void fn_8024CED8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fn_8025C5A4();
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
    fn_801F1C18();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
        fn_80205B8C();
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
                fn_8025C5A4();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r30;
                    r4 = r31;
                    r5 = 0x7c;
                    fn_80239984();
                    r0 = r3;
                    r3 = r29;
                    r30 = r0;
                    fn_80205B8C();
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
        fn_80205B8C();
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
u32 fn_8024D818(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74();
    extern u16 fn_8023831C();
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    s32 handle = 0;
    u16 v;

    if (fn_80237F74(ctx, param1, 0x14) == 1) {
        handle = fn_80239984(0, ctx, 0x66);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x66);
    }
    v = fn_8023831C(ctx, param1);
    if (v == 8 || v == 9) {
        handle = fn_80239984(handle, ctx, 0x67);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x67);
    }
    handle = fn_80239984(handle, ctx, 0x68);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x68);
    return handle;
}

/* Address: 0x8024D958 | Size: 0x1A4 (420 bytes) */
u32 fn_8024D958(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_801F1990(u32, void*, u32, u32, u32, u32);
    extern s32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u32 fn_802376EC();
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_802399FC(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern s32 fn_8023C370(void*, u32, u32, u32, u32);
    u32 handle;
    s32 score;
    s32 scale;

    handle = fn_802376EC();
    score = fn_8023C370(ctx, param1, param2, param3, 1);
    scale = fn_801FB1C0(0, 0x63, 0x3e, 0);
    score = (((score / 2) * 100) / (u16)handle) / scale;
    {
        u32 nextHandle = fn_802399FC(0, score);
        handle = nextHandle;
    }
    fn_80239CCC(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x63, score);
    if ((fn_801F1990(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x64);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x64);
    }
    if ((fn_80237F74(ctx, param3, 0x40) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x65);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x65);
    }
    return handle;
}

/* Address: 0x8024DAFC | Size: 0xC0 */
void fn_8024DAFC(void* ctx, u32 param1, u32 param2) {
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x61;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x61;
    fn_80239EE8();
    r3 = r31;
    r4 = r28;
    r5 = 0x62;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x62;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x8024DBBC | Size: 0xC0 */
void fn_8024DBBC(void* ctx, u32 param1, u32 param2) {
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x5f;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x5f;
    fn_80239EE8();
    r3 = r31;
    r4 = r28;
    r5 = 0x60;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x60;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x8024DC7C | Size: 0x210 (528 bytes) */
void fn_8024DC7C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_80236BFC();
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

    r6 = 0x0;
    r7 = 0x1;
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r5 = (u32)sp + 0x8;
    r4 = r27;
    r30 = 0x0;
    r3 = 0x0;
    fn_801F1C18();
    r31 = r3;
    r3 = r27;
    r4 = r28;
    fn_80236520();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x117) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x5b;
        fn_80239984();
        r0 = r3;
        r3 = r28;
        r30 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r27;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x5b;
        fn_80239EE8();
    }
    r25 = (u32)sp + 0x8;
    r26 = r31 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r26) break;
        r3 = r27;
        r4 = *(u32*)(r25 + r0);
        r5 = 0x8;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r30;
            r4 = r27;
            r5 = 0x5c;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x5c;
            fn_80239EE8();
            break;
        }
        r24 = r24 + 0x1;

    }

    r26 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r27;
        r4 = *(u32*)(r26 + r0);
        r5 = 0x7;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r30;
            r4 = r27;
            r5 = 0x5d;
            fn_80239984();
            r0 = r3;
            r3 = r28;
            r30 = r0;
            fn_80205B8C();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r27;
            r8 = r29;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x5d;
            fn_80239EE8();
            break;
        }
        r25 = r25 + 0x1;

    }

    r3 = r30;
    r4 = r27;
    r5 = 0x5e;
    fn_80239984();
    r25 = r3;
    r3 = r28;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r27;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x5e;
    fn_80239EE8();
    r3 = r25;
    return;
}

/* Address: 0x8024DE8C | Size: 0x138 (312 bytes) */
void fn_8024DE8C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_802357CC();
    extern void fn_802358AC();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r3;
    r30 = r5;
    r4 = r6;
    r31 = 0x0;
    fn_802357CC();
    r0 = r3 & 0xFF;
    if (r0 > (u32)0x6) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x58;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x58;
        fn_80239EE8();
    }
    r3 = r28;
    r4 = r29;
    fn_802358AC();
    r0 = r3 & 0xFF;
    if (r0 < (u32)0x6) {
        r3 = r31;
        r4 = r28;
        r5 = 0x59;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x59;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x5a;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x5a;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x8024DFC4 | Size: 0x108 (264 bytes) */
void fn_8024DFC4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r3;
    r30 = r5;
    r4 = r6;
    r31 = 0x0;
    fn_80236520();
    r0 = r3 & 0xFFFF;
    r4 = r3;
    if ((s32)r0 != (s32)0) {
        if (r0 != (u32)0xffff) {
            if (r0 != (u32)0x165) {
                if (r0 != (u32)0x163) {
                    r3 = r28;
                    r5 = 0x1;
                    fn_8023943C();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r4 = r28;
                        r3 = 0x0;
                        r5 = 0x56;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fn_80205B8C();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r28;
                        r8 = r30;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x56;
                        fn_80239EE8();
    }
    }
    }
    }
    }
    r3 = r31;
    r4 = r28;
    r5 = 0x57;
    fn_80239984();
    r31 = r3;
    r3 = r29;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x57;
    fn_80239EE8();
    r3 = r31;
    return;
}

/* Address: 0x8024E0CC | Size: 0x7C | Pattern: field_accessor */
u32 fn_8024E0CC(void* ctx, u32 slot, u32 param) {
    extern s32 fn_80239984();
    extern u32 fn_80205B8C();
    extern void fn_80239EE8();
    u32 handle;

    handle = fn_80239984(0, ctx, 0x55);
    fn_80239EE8(0xEC64, ctx, fn_80205B8C(slot), 0, 0, param, 0, 0x55);
    return handle;
}

/* Address: 0x8024E148 | Size: 0xEC (236 bytes) */
void fn_8024E148(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x53;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x53;
    fn_80239EE8();
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x54;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x54;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x8024E234 | Size: 0xEC (236 bytes) */
void fn_8024E234(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    r3 = 0x0;
    r4 = r28;
    r5 = 0x51;
    fn_80239984();
    r0 = r3;
    r3 = r29;
    r31 = r0;
    fn_80205B8C();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r28;
    r8 = r30;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x51;
    fn_80239EE8();
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r28;
        r5 = 0x52;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x52;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x8024E320 | Size: 0x164 (356 bytes) */
u32 fn_8024E320(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_801F1990(u32, void*, u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
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
        fn_80239EE8(0xEC64, battleCtx, fn_80205B8C(trainer), 0, 0, sequenceArg, 0, 0x4E);
    }
    if (fn_802358AC(battleCtx, trainer) < 6U) {
        handle = fn_80239984(handle, battleCtx, 0x4F);
        fn_80239EE8(0xEC64, battleCtx, fn_80205B8C(trainer), 0, 0, sequenceArg, 0, 0x4F);
    }
    if (fn_801F1990(0, battleCtx, 1, 1, 0x10E, trainer) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x50);
        fn_80239EE8(0xEC64, battleCtx, fn_80205B8C(trainer), 0, 0, sequenceArg, 0, 0x50);
    }
    return handle;
}

/* Address: 0x8024E484 | Size: 0xA8 */
void fn_8024E484(void* ctx, u32 param1, u32 param2) {
    extern void fn_801F1990();
    extern void fn_80205B8C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r6 = 0x1;
    r7 = 0x10e;
    r28 = r3;
    r29 = r4;
    r30 = r5;
    r31 = 0x0;
    r4 = r28;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    fn_801F1990();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x4d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x4d;
        fn_80239EE8();
    }
    r3 = r31;
    return;
}

/* Address: 0x8024E534 | Size: 0x44 | Pattern: field_accessor */
u32 fn_8024E534(void* ctx, u32 slot, u32 param) {
    extern u32 fn_801FB1C0();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return 0;
}

/* Address: 0x8024E578 | Size: 0x118 (280 bytes) */
void fn_8024E578(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D30[];
    extern void fn_801F4354();
    extern void fn_801FB1C0();
    extern void fn_8020505C();
    extern void fn_80205B8C();
    extern void fn_8023A118();
    extern void fn_8024E690();
    extern void fn_8024F8B4();
    u8 sp[0x30];
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

    r6 = 0x0;
    r28 = r4;
    r29 = r5;
    r27 = r3;
    r4 = 0x0;
    r5 = 0x43;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r28;
    r3 = 0x0;
    fn_801F4354();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r28;
    r5 = r29;
    fn_8024F8B4();
    if ((s32)r0 <= (s32)0) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    fn_80205B8C();
    r0 = 0x0;
    r5 = (0x1 << 16);
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x228;
    r7 = r3;
    r6 = r30;
    *(u32*)(sp + 0xC) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r27;
    r4 = r29;
    r6 = r28;
    r5 = 0x1;
    fn_8024E690();
    r5 = r3;
    r0 = (s16)r5;
    if ((s32)r0 < (s32)0) {
        r3 = 0x0;
        return;
    }
    r4 = (u32)lbl_80375D30;
    r8 = (s16)r5;
    r7 = (u32)lbl_80375D30;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8020505C();
    r3 = 0x1;

    return;
}

/* Address: 0x8024F8B4 | Size: 0x5CC (1484 bytes) */
void fn_8024F8B4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_801F4354();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_8023565C();
    extern void fn_80235714();
    extern void fn_802367CC();
    extern void fn_80236E9C();
    extern void fn_80237288();
    extern void fn_8023753C();
    extern void fn_802376EC();
    extern void fn_8023785C();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C370();
    extern void fn_8023C530();
    extern s32 fn_80250070();
    u8 sp[0xA0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
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
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r5 = 0x43;
    r6 = 0x0;
    r16 = r4;
    r15 = r3;
    r4 = 0x0;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r16;
    r24 = 0x0;
    r3 = 0x0;
    fn_801F4354();
    r0 = r3;
    r3 = r15;
    r20 = r0;
    r4 = r16;
    fn_80236E9C();
    r26 = r3;
    r3 = r15;
    r4 = r16;
    r5 = (u32)sp + 0x38;
    r6 = 0x0;
    r7 = 0x1;
    fn_802367CC();
    r27 = r3;
    r4 = r15;
    r5 = (u32)sp + 0x18;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    r28 = r3;
    r14 = (u32)sp + 0x18;
    r31 = r3 & 0xFFFF;
    r18 = 0x0;
    r17 = 0x0;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r19 = *(u32*)(r14 + r0);
        if (r19 != (u32)0x0) {
            r3 = r15;
            r4 = r19;
            fn_802376EC();
            r25 = r3 & 0xFFFF;
            r30 = r27 & 0xFFFF;
            r21 = 0x0;
            while (1) {
                r0 = r21 & 0xFFFF;
                if (r0 >= (u32)r30) break;
                r3 = (u32)sp + 0x38;
                r22 = *(u16*)(r3 + r0);
                if (r22 != (u32)0x0) {
                    if (r22 != (u32)0x165) {
                        r3 = r15;
                        r4 = r16;
                        r5 = r22;
                        r6 = r19;
                        fn_8023C530();
                        r29 = r3;
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r17 = 0x1;
                        }
                        r3 = r15;
                        r4 = r19;
                        fn_80237288();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r17 = 0x1;
                        }
                        r3 = r15;
                        r4 = r22;
                        r5 = 0x1;
                        fn_8023943C();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r15;
                            r4 = r16;
                            r5 = r22;
                            r6 = r19;
                            r7 = 0x0;
                            fn_8023C370();
                            if ((s32)r25 < (s32)r3) {
                                r0 = r29 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r18 = 0x1;
                }
                }
                }
                }
                }
                r21 = r21 + 0x1;

            }
        }
        r23 = r23 + 0x1;

    }
    r3 = r16;
    fn_80205B8C();
    r8 = 0x0;
    r5 = (0x1 << 16);
    r0 = 0x227;
    r7 = r3;
    r6 = r20;
    *(u32*)(sp + 0xC) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r15;
    r4 = r16;
    fn_8023785C();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r31) {
        r4 = r15;
        r3 = 0x0;
        r5 = 0x1;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x1;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023753C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)r31) {
        r3 = r24;
        r4 = r15;
        r5 = 0x2;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x2;
        fn_80239EE8();

    } else {
        r3 = r15;
        r4 = r16;
        fn_8023785C();
        r0 = r3 & 0xFF;
        if (r0 != (u32)r31) {
            r3 = r24;
            r4 = r15;
            r5 = 0x3;
            fn_80239984();
            r0 = r3;
            r3 = r16;
            r24 = r0;
            fn_80205B8C();
            r7 = (0x1 << 16);
            r5 = r3;
            r4 = r20;
            r6 = 0x0;
            r7 = 0x0;
            r8 = 0x0;
            r9 = 0x0;
            r10 = 0x3;
            fn_80239EE8();
        }
    }
    r3 = r15;
    r4 = r16;
    fn_80235714();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x4;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x4;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023565C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x5;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x5;
        fn_80239EE8();
    }
    r0 = r17 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x6;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x6;
        fn_80239EE8();
    }
    r0 = r18 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x7;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x7;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023753C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r17 = (u32)sp + 0x18;
        r14 = r28 & 0xFFFF;
        r18 = 0x0;
        while (1) {
            r0 = r18 & 0xFFFF;
            if (r0 >= (u32)r14) break;
            r4 = *(u32*)(r17 + r0);
            if (r4 != (u32)0x0) {
                r3 = r15;
                fn_80236E9C();
                r3 = r3 & 0xFFFF;
                r0 = r26 & 0xFFFF;
                if (r3 > r0) {
                    r3 = r24;
                    r4 = r15;
                    r5 = 0x8;
                    fn_80239984();
                    r0 = r3;
                    r3 = r16;
                    r24 = r0;
                    fn_80205B8C();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r20;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x8;
                    fn_80239EE8();
            }
            }
            r18 = r18 + 0x1;

        }
    }
    r3 = r15;
    r4 = r16;
    fn_80250070();
    r0 = r3 & 0xFFFF;
    r14 = r3;
    if (r0 != (u32)r14) {
        r3 = r24;
        r4 = r15;
        r5 = r14;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fn_80205B8C();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r10 = r14;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        fn_80239EE8();
    }
    r3 = r16;
    fn_80205B8C();
    r0 = 0x0;
    r5 = (0x1 << 16);
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x226;
    r7 = r3;
    r6 = r20;
    *(u32*)(sp + 0xC) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r24;
    return;
}

/* Address: 0x8024FE80 | Size: 0x1F0 (496 bytes) */
void fn_8024FE80(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A420[];
    extern void fn_801F1C18();
    extern void fn_80235B04();
    extern void fn_80236FFC();
    extern void fn_8023715C();
    extern void fn_80237F74();
    extern void fn_80239058();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
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

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r5 = 0x1;
    r30 = r4;
    r29 = r3;
    r4 = 0x0;
    fn_80235B04();
    r4 = (u32)lbl_8027A420;
    r0 = 0x2;
    r4 = (u32)lbl_8027A420;
    r31 = r3;
    r6 = (u32)sp + 0x4;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r4 = r29;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    *(u32*)((u8*)r6 + 0x4) = r0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    r28 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x16;
    fn_80239058();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r25 = (u32)sp + 0x1c;
        r26 = r28 & 0xFFFF;
        r24 = 0x0;
        while (1) {
            r0 = r24 & 0xFFFF;
            if (r0 >= (u32)r26) break;
            r3 = r29;
            r4 = *(u32*)(r25 + r27);
            fn_8023715C();
            r4 = *(u32*)(r25 + r27);
            r27 = r3;
            r3 = r29;
            fn_80236FFC();
            r4 = r27 & 0xFFFF;
            r0 = r3 & 0xFFFF;
            if (r4 >= (u32)r0) {
                r3 = 0x25;
                return;
            }
            r24 = r24 + 0x1;

        }
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x24;
    fn_80239058();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r27 = (u32)sp + 0x8;
        r25 = (u32)sp + 0x1c;
        r28 = r28 & 0xFFFF;
        r24 = 0x0;
        while (1) {
            r0 = r24 & 0xFFFF;
            if (r0 >= (u32)r28) break;
            r22 = 0x0;
            r23 = 0x0;
            while (1) {
                r0 = r23 & 0xFFFF;
                if (r0 >= (u32)0xa) break;
                r4 = *(u32*)(r25 + r26);
                r5 = *(u16*)(r27 + r0);
                r3 = r29;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r22 = 0x1;
                    break;
                }
                r23 = r23 + 0x1;

            }

            r0 = r22 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = 0x26;
                return;
            }
            r24 = r24 + 0x1;

        }
    }
    r0 = r31 & 0xFF;
    if (r0 != (u32)r28) {
        r3 = r29;
        r4 = r30;
        r5 = 0x4d;
        fn_80239058();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x27;
            return;
        }
        r3 = r29;
        r4 = r30;
        r5 = 0xd;
        fn_80239058();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x28;
            return;
    }
    }
    r3 = 0x0;

    return;
}

/* Address: 0x80250070 | Size: 0x27C (636 bytes) */
s32 fn_80250070(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern u8 fn_8023753C();

    if (fn_80236BFC(ctx, param1, 0x9) == 1) {
        return 0x9;
    }
    if (fn_80236BFC(ctx, param1, 0xA) == 1) {
        return 0xA;
    }
    if (fn_80236BFC(ctx, param1, 0x1E) == 1 && fn_8023753C(ctx, param1) == 0) {
        return 0xB;
    }
    if (fn_80236BFC(ctx, param1, 0xE) == 1) {
        return 0xC;
    }
    if (fn_80236BFC(ctx, param1, 0x17) == 1) {
        return 0xD;
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        return 0xE;
    }
    if (fn_80236BFC(ctx, param1, 0x19) == 1) {
        return 0xF;
    }
    if (fn_80236BFC(ctx, param1, 0x1B) == 1) {
        return 0x10;
    }
    if (fn_80236BFC(ctx, param1, 0x1C) == 1) {
        return 0x11;
    }
    if (fn_80236BFC(ctx, param1, 0x1D) == 1) {
        return 0x12;
    }
    if (fn_80236BFC(ctx, param1, 0x26) == 1) {
        return 0x13;
    }
    if (fn_80236BFC(ctx, param1, 0x27) == 1) {
        return 0x14;
    }
    if (fn_80236BFC(ctx, param1, 0x28) == 1) {
        return 0x15;
    }
    if (fn_80236BFC(ctx, param1, 0x29) == 1) {
        return 0x16;
    }
    if (fn_80236BFC(ctx, param1, 0x2A) == 1) {
        return 0x17;
    }
    return fn_80236BFC(ctx, param1, 0x30) == 1 ? 0x18 : 0;
}

/* Address: 0x802502EC | Size: 0x694 (1684 bytes) */
void fn_802502EC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D70[];
    extern void fn_8011FC74();
    extern void fn_80142984();
    extern void fn_801440F0();
    extern void fn_801F0134();
    extern void fn_801F1A6C();
    extern void fn_801F1C18();
    extern void fn_801F7C54();
    extern void fn_801FB1C0();
    extern void fn_80204CE0();
    extern void fn_80205B8C();
    extern void fn_802062FC();
    extern void fn_80206608();
    extern void fn_802126C4();
    extern void fn_80235714();
    extern void fn_80236C80();
    extern void fn_80237310();
    extern void fn_8023753C();
    extern void fn_8023785C();
    extern void fn_802397B8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    u8 sp[0x250];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r6 = 0x0;
    r29 = r4;
    r22 = r5;
    r28 = r3;
    r4 = 0x0;
    r5 = 0x43;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r3 = r28;
    r4 = (u32)sp + 0x38;
    r30 = 0x0;
    r5 = 0x14;
    r6 = 0x1;
    fn_801F7C54();
    r0 = r3 & 0xFFFF;
    r31 = r3;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r4 = r28;
    r5 = (u32)sp + 0x18;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1C18();
    r17 = r3;
    r4 = r28;
    r5 = (u32)sp + 0x60;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1A6C();
    r26 = r3;
    r18 = (u32)sp + 0x18;
    r17 = r17 & 0xFFFF;
    r20 = 0x0;
    while (1) {
        r0 = r20 & 0xFFFF;
        if (r0 >= (u32)r17) break;
        r19 = *(u32*)(r18 + r0);
        if (r19 != (u32)0x0) {
            r3 = r19;
            fn_802062FC();
            r0 = r3 & 0xFF;
            if (r19 != (u32)0x0) {
                r3 = r28;
                r4 = r19;
                fn_80235714();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r30 = 0x0;
        }
        }
        }
        r20 = r20 + 0x1;

    }
    r3 = (u32)sp + 0xc0;
    r5 = 0x0;
    r4 = 0x0;
    while (1) {
        r0 = r5 & 0xFFFF;
        if (r0 >= (u32)0x14) break;
        r5 = r5 + 0x1;
        *(u32*)(r3 + r0) = r4;

    }
    r20 = (u32)sp + 0x38;
    r27 = r31 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r27) break;
        r25 = r24 & 0xFFFF;
        r23 = *(u16*)(r20 + r0);
        if (r23 != (u32)0x0) {
            r3 = r23;
            fn_80142984();
            r0 = r3 & 0xFF;
            if (r23 != (u32)0x0) {
                r3 = r23;
                r4 = r29;
                fn_802126C4();
                r21 = r3;
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x7) {
                    r3 = r29;
                    r4 = 0x0;
                    r5 = 0xd5;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                    r5 = r3;
                    r6 = r23;
                    r3 = (u32)sp + 0x110;
                    r4 = 0x0;
                    r7 = 0x0;
                    fn_801440F0();
                    r3 = (s16)r3;
                    r0 = -r3;
                    r0 = r0 & ~r3;
                    if (r0 != (u32)0x7) {
                        r0 = r21 & 0xFF;

                        if (r0 == (u32)0x2 || r0 == (u32)0x1) {

                            r3 = r28;
                            r4 = r29;
                            fn_8023753C();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r3 = r29;
                                fn_80205B8C();
                                fn_8011FC74();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r17 = r25 << 2;
                                    r18 = (u32)sp + 0xc0;
                                    r3 = *(u32*)(r18 + r17);
                                    r4 = r28;
                                    r5 = 0x2e;
                                    fn_80239984();
                                    *(u32*)(r18 + r17) = r3;
                                    r3 = r29;
                                    fn_80205B8C();
                                    r6 = (0x1 << 16);
                                    r5 = r3;
                                    r4 = r28;
                                    r9 = r23;
                                    r6 = 0x0;
                                    r7 = 0x0;
                                    r8 = 0x0;
                                    r10 = 0x2e;
                                    fn_80239EE8();
        }
            }
                        }
                        r0 = r21 & 0xFF;

                        if (r0 == (u32)0x3 || r0 == (u32)0x1) {

                            r3 = r28;
                            r4 = r29;
                            fn_80237310();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r3 = r29;
                                fn_80205B8C();
                                fn_8011FC74();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r17 = r25 << 2;
                                    r18 = (u32)sp + 0xc0;
                                    r3 = *(u32*)(r18 + r17);
                                    r4 = r28;
                                    r5 = 0x2f;
                                    fn_80239984();
                                    *(u32*)(r18 + r17) = r3;
                                    r3 = r29;
                                    fn_80205B8C();
                                    r6 = (0x1 << 16);
                                    r5 = r3;
                                    r4 = r28;
                                    r9 = r23;
                                    r6 = 0x0;
                                    r7 = 0x0;
                                    r8 = 0x0;
                                    r10 = 0x2f;
                                    fn_80239EE8();
        }
            }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x5) {
                            r3 = r28;
                            r4 = r29;
                            fn_80235714();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x5) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x30;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fn_80205B8C();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x30;
                                fn_80239EE8();
                        }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x4) {
                            r3 = r28;
                            r4 = r29;
                            fn_80236C80();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x2) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x31;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fn_80205B8C();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x31;
                                fn_80239EE8();
                        }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x6) {
                            r0 = r30 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x32;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fn_80205B8C();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x32;
                                fn_80239EE8();
                        }
                        }
                        r3 = r28;
                        r4 = r29;
                        fn_8023785C();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x2) {
                            r3 = r28;
                            r4 = r29;
                            fn_8023785C();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x3) {
                                r19 = (u32)sp + 0x60;
                                r17 = r26 & 0xFFFF;
                                r18 = 0x0;
                                while (1) {
                                    r0 = r18 & 0xFFFF;
                                    if (r0 >= (u32)r17) break;
                                    r3 = r29;
                                    r4 = 0x0;
                                    r5 = 0xd5;
                                    r6 = 0x0;
                                    ((void(*)(void))fn_8012640C)();
                                    r0 = *(u32*)(r19 + r21);
                                    if (r3 != (u32)r0) {
                                        r3 = r28;
                                        r4 = r29;
                                        fn_8023785C();
                                        r0 = r3 & 0xFF;
                                        if (r0 != (u32)0x2) {
                                            r3 = r28;
                                            r4 = r29;
                                            fn_8023785C();
                                            r0 = r3 & 0xFF;
                                            if (r0 == (u32)0x3) {
                                            }
                                            r3 = *(u32*)(r19 + r21);
                                            fn_80206608();
                                            r0 = r3 & 0xFF;
                                            if (r0 == (u32)0x1) {
                                                r17 = r25 << 2;
                                                r18 = (u32)sp + 0xc0;
                                                r3 = *(u32*)(r18 + r17);
                                                r4 = r28;
                                                r5 = 0x33;
                                                fn_80239984();
                                                *(u32*)(r18 + r17) = r3;
                                                r3 = r29;
                                                fn_80205B8C();
                                                r6 = (0x1 << 16);
                                                r5 = r3;
                                                r4 = r28;
                                                r9 = r23;
                                                r6 = 0x0;
                                                r7 = 0x0;
                                                r8 = 0x0;
                                                r10 = 0x33;
                                                fn_80239EE8();
                                                break;
                    }
                                        }
                                            }
                                    r18 = r18 + 0x1;

                                }
        }
                        }
                        r3 = r29;
                        fn_80205B8C();
                        r0 = 0x226;
                        r4 = (u32)sp + 0xc0;
                        r5 = (0x1 << 16);
                        *(u32*)(sp + 0xC) = r0;
                        r0 = r25 << 2;
                        r7 = r3;
                        r6 = r28;
                        r0 = *(u32*)(r4 + r0);
                        *(u32*)(sp + 0x10) = r0;
                        r8 = 0x0;
                        r9 = 0x0;
                        r10 = 0x0;
                        fn_8023A118();
        }
        }
        }
        }
        r24 = r24 + 0x1;

    }
    r4 = (u32)sp + 0xc0;
    r0 = r31 & 0xFFFF;
    r17 = 0x0;
    while (1) {
        r3 = r17 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u32*)(r4 + r3);
        if ((s32)r3 > (s32)0x0) break;
        r17 = r17 + 0x1;

    }

    r3 = r17 & 0xFFFF;
    r0 = r31 & 0xFFFF;
    if (r3 >= (u32)r0) {
        r3 = 0x0;
        return;
    }
    r4 = r31;
    r3 = (u32)sp + 0xc0;
    r5 = 0x1;
    fn_802397B8();
    if (r3 < r0) {
        r3 = 0x0;
        return;
    }
    r3 = (u32)sp + 0x38;
    r17 = *(u16*)(r3 + r0);
    if (r17 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = r29;
    fn_80205B8C();
    r0 = 0x228;
    r4 = (u32)sp + 0xc0;
    r5 = (0x1 << 16);
    *(u32*)(sp + 0xC) = r0;
    r0 = r18 << 2;
    r7 = r3;
    r6 = r28;
    r0 = *(u32*)(r4 + r0);
    *(u32*)(sp + 0x10) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r29;
    r4 = r22;
    fn_801F0134();
    r0 = 0x0;
    r4 = (u32)lbl_80375D70;
    *(u32*)(sp + 0x8) = r0;
    r9 = r3;
    r7 = (u32)lbl_80375D70;
    r3 = r29;
    r8 = r17;
    r4 = 0x0;
    r5 = 0x12;
    r6 = 0x0;
    r10 = -0x1;
    fn_80204CE0();
    r3 = 0x1;

    return;
}

/* Address: 0x802509A0 | Size: 0x84 | Pattern: field_accessor */
void fn_802509A0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250A2C | Size: 0x84 | Pattern: field_accessor */
void fn_80250A2C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250AC0 | Size: 0x84 | Pattern: field_accessor */
void fn_80250AC0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250B44 | Size: 0x78 | Pattern: field_accessor */
void fn_80250B44(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80235B04();
    extern void fn_80250BBC();
    u8 sp[0x20];
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
    u32 r4 = slot;
    u32 r5 = param;

    r28 = r4;
    r29 = r5;
    r27 = r3;
    r30 = r6;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r4 = (u32)fn_80250BBC;
    r31 = r3;
    r9 = (u32)fn_80250BBC;
    r3 = r27;
    r4 = r29;
    r5 = r28;
    r6 = r30;
    r7 = 0x0;
    r8 = 0x0;
    r10 = 0x0;
    fn_80211170();
    r0 = r31 & 0xFF;
    if ((s32)r0 != (s32)0) {
        r3 = r3 << 1;
    }
    return;
}

/* Address: 0x80250BBC | Size: 0xA8 */
void fn_80250BBC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BBD8();
    extern void fn_80235B04();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r4 = 0x0;
    r30 = r5;
    r5 = 0x1;
    fn_80235B04();
    r31 = r3;
    r3 = r30;
    r4 = 0x0;
    r5 = 0xd9;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x2) {
        r0 = 0xb;

    } else if (r0 == (u32)0x3) {
        r0 = 0x5;

    } else if (r0 == (u32)0x1) {
        r0 = 0xa;

    } else if (r0 == (u32)0x4) {
        r0 = 0xf;

    } else {
        r0 = 0x0;
    }
    r7 = r0 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x30;
    r6 = 0x0;
    fn_8011BBD8();
    return;
}

/* Address: 0x80250C64 | Size: 0x84 | Pattern: field_accessor */
void fn_80250C64(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250CF0 | Size: 0x84 | Pattern: field_accessor */
void fn_80250CF0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250D7C | Size: 0x84 | Pattern: field_accessor */
void fn_80250D7C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250E00 | Size: 0x84 | Pattern: field_accessor */
void fn_80250E00(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80250E84 | Size: 0x40 | Ghidra import */
u32 fn_80250E84(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80250EC4();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80250EC4,0);
}


/* Address: 0x80250EC4 | Size: 0x90 */
void fn_80250EC4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BBD8(u32, u32, u32, u32, u32);
    extern u32 fn_802152A8(u32);
    extern u32 fn_802377E8(void*, u32);
    u32 sourceValue;
    u32 convertedValue;
    u32 scaledValue;

    sourceValue = (u32)fn_8012640C(param2, 0, 0xd9, 0);
    convertedValue = fn_802377E8(ctx, param2);
    scaledValue = fn_802152A8((u32)fn_8012640C(0, convertedValue, 0x5f, 0) & 0xffff);
    fn_8011BBD8(sourceValue, 0, 0x2f, 0, scaledValue & 0xffff);
}

/* Address: 0x80250F7C | Size: 0x40 | Ghidra import */
u32 fn_80250F7C(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80250FBC();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80250FBC,0);
}


/* Address: 0x80250FBC | Size: 0xB4 */
void fn_80250FBC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BBD8();
    extern void fn_8011BEB4();
    extern void fn_80237664();
    extern void fn_802376EC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r4 = 0x0;
    r6 = 0x0;
    r31 = r5;
    r28 = r3;
    r5 = 0xd9;
    r3 = r31;
    ((void(*)(void))fn_8012640C)();
    r0 = r3;
    r3 = r28;
    r29 = r0;
    r4 = r31;
    fn_802376EC();
    r30 = r3;
    r3 = r28;
    r4 = r31;
    fn_80237664();
    r31 = r3;
    r3 = r29;
    r4 = 0x0;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFFFF;
    r0 = r30 & 0xFFFF;
    r3 = r3 * r0;
    r0 = r31 & 0xFFFF;
    r0 = (s32)r3 / (s32)r0;
    r0 = r0 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r0 = 0x1;
    }
    r3 = r29;
    r7 = r0 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BBD8();
    return;
}

/* Address: 0x80251070 | Size: 0x5C | Pattern: field_accessor */
void fn_80251070(void* ctx, u32 slot, u32 param) {
    extern void fn_802376EC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;

    r29 = r3;
    r30 = r6;
    fn_802376EC();
    r31 = r3 & 0xFFFF;
    r3 = r29;
    r4 = r30;
    fn_802376EC();
    r0 = r3 & 0xFFFF;
    r3 = r31 - r0;
    r0 = r0 - r31;
    r3 = r3 + r4;
    r3 = r3 - r3; /* -borrow */;
    r3 = r0 & ~r3;
    return;
}

/* Address: 0x802510CC | Size: 0x84 | Pattern: field_accessor */
void fn_802510CC(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251158 | Size: 0x3C | Pattern: simple_wrapper */
extern u32 fn_80211170(void* ctx, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7);
u32 fn_80251158(void* ctx, u32 param1, u32 param2, u32 param3) {
    return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x80251194 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80251194(void* ctx, u32 param1, u32 param2, u32 param3) {
    return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x802511E0 | Size: 0x84 | Pattern: field_accessor */
void fn_802511E0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802512A4 | Size: 0xAC */
void fn_802512A4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BEB4();
    extern void fn_801363E8();
    extern void fn_801F54A4();
    extern void fn_8023C370();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r27 = r3;
    r28 = r4;
    r30 = r5;
    r29 = r6;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0xf;
    r6 = 0x0;
    fn_801F54A4();
    r3 = r3 & 0xFFFF;
    fn_801363E8();
    r4 = r30;
    r30 = r3;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8011BEB4();
    r31 = r3 & 0xFFFF;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8011BEB4();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)r31) {
        r3 = r27;
        r4 = r28;
        r5 = r30;
        r6 = r29;
        r7 = 0x1;
        fn_8023C370();
    } else {

        r3 = 0x0;
    }
    return;
}

/* Address: 0x80251358 | Size: 0x78 | Pattern: field_accessor */
void fn_80251358(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r4 = r5;
    r9 = 0x0;
    r30 = r3;
    r31 = r6;
    r5 = r0;
    r10 = 0x0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r5 = 0x5;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x802513D0 | Size: 0x84 | Pattern: field_accessor */
void fn_802513D0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251454 | Size: 0x70 | Pattern: field_accessor */
void fn_80251454(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_8023720C();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    r31 = r4;
    r4 = r5;
    r30 = r3;
    r5 = r31;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r31 = r0;
    fn_8023720C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x802514EC | Size: 0x98 */
void fn_802514EC(void* ctx, u32 param1, u32 param2) {
    extern void fn_80119DD0();
    extern void fn_80202360();
    extern void fn_80236BFC();
    extern void fn_80237664();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r5 = 0x2d;
    r29 = r3;
    r30 = r4;
    r31 = 0x1;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = 0x2d;
        fn_80202360();
        r31 = r3;
    }
    r3 = 0x2d;
    fn_80119DD0();
    r0 = r3 & 0xFF;
    r0 = r0 - r31;
    r0 = (s16)r0;
    if (r0 < (u32)0x1) {
        r0 = 0x0;
    }
    r0 = (s16)r0;
    r3 = 0x1;
    r0 = r3 << r0;
    r3 = r29;
    r4 = r30;
    r31 = (s16)r0;
    fn_80237664();
    r0 = r3 & 0xFFFF;
    r0 = (s32)r0 / (s32)r31;
    r3 = -r0;
    return;
}

/* Address: 0x80251584 | Size: 0x88 */
s32 fn_80251584(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s16 fn_80202360(u32, u32);
    extern s32 fn_80211170(void*, u32, u32, u32, u32, u32, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    s16 multiplier;

    multiplier = 1;
    if (fn_80236BFC(ctx, param1, 0x2d) == 1) {
        multiplier = fn_80202360(param1, 0x2d);
    }
    return multiplier * fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x80251614 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80251614(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251658 | Size: 0x28 | Ghidra import */
int fn_80251658(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 >> 1 & 0x7fff);
}
/* Address: 0x80251688 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80251688(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x802516C4 | Size: 0xD4 (212 bytes) */
u32 fn_802516C4(void* ctx, u32 unused, u32 param2, u32 param3) {
    extern u16 fn_801F1A6C(u32, void*, u32*, u32, u32);
    extern u32 fn_80216CF8(u32, u32, u8, u32, u8);
    extern u32 fn_80237774();
    extern u32 fn_802377E8();
    extern u32 fn_8023892C();
    extern u32 fn_80238980();
    u32 entries[24];
    u32 total;
    u16 count;
    u16 index;
    u32 mappedEntry;
    u32 entryType;
    u32 convertedParam;
    u32 convertedState;

    total = 0;
    count = fn_801F1A6C(0, ctx, entries, 1, 1);
    index = 0;
    while (index < count) {
        mappedEntry = fn_80238980(ctx, entries[index]);
        entryType = fn_8023892C(ctx, entries[index]);
        convertedParam = fn_802377E8(ctx, param3);
        convertedState = fn_80237774(ctx, param3);
        total += fn_80216CF8(param2, mappedEntry, (u8)entryType, convertedParam, (u8)convertedState);
        index++;
    }
    return total;
}

/* Address: 0x802517A0 | Size: 0x84 | Pattern: field_accessor */
void fn_802517A0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251824 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80251824(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251860 | Size: 0x78 | Pattern: field_accessor */
void fn_80251860(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r4 = r5;
    r9 = 0x0;
    r30 = r3;
    r31 = r6;
    r5 = r0;
    r10 = 0x0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r5 = 0x23;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x802518D8 | Size: 0x78 | Pattern: field_accessor */
void fn_802518D8(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r4 = r5;
    r9 = 0x0;
    r30 = r3;
    r31 = r6;
    r5 = r0;
    r10 = 0x0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r5 = 0x1f;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x80251950 | Size: 0xBC */
void fn_80251950(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BEB4();
    extern void fn_801F025C();
    extern void fn_802026E4();
    extern void fn_80232110();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r3 = 0x0;
    r28 = r5;
    r27 = r4;
    r29 = r6;
    r4 = r28;
    r5 = 0x7;
    r6 = 0x0;
    fn_8011BEB4();
    r31 = r3 & 0xFFFF;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x3;
    r6 = 0x0;
    fn_8011BEB4();
    r30 = r3 & 0xFFFF;
    r4 = r29;
    r3 = 0x2;
    fn_801F025C();
    r0 = r3;
    r3 = r27;
    r5 = r0;
    r4 = r29;
    r6 = r28;
    r7 = r31;
    r8 = r30;
    fn_80232110();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = 0x32;
    fn_802026E4();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31 * 0xf;
        r0 = 0xa;
        r31 = (s32)r3 / (s32)r0;
    }
    r3 = r31;
    return;
}

/* Address: 0x80251A0C | Size: 0x78 | Pattern: field_accessor */
void fn_80251A0C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r4 = r5;
    r9 = 0x0;
    r30 = r3;
    r31 = r6;
    r5 = r0;
    r10 = 0x0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r5 = 0x20;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x80251A84 | Size: 0x78 | Pattern: field_accessor */
void fn_80251A84(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r4 = r5;
    r9 = 0x0;
    r30 = r3;
    r31 = r6;
    r5 = r0;
    r10 = 0x0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r30;
    r5 = 0x1f;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r31 = r31 << 1;
    }
    r3 = r31;
    return;
}

/* Address: 0x80251AFC | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80251AFC(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251B50 | Size: 0x84 | Pattern: field_accessor */
void fn_80251B50(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251BD4 | Size: 0x84 | Pattern: field_accessor */
void fn_80251BD4(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251C58 | Size: 0x84 | Pattern: field_accessor */
void fn_80251C58(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251CEC | Size: 0x40 | Ghidra import */
u32 fn_80251CEC(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80251D2C();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80251D2C,0);
}


/* Address: 0x80251D2C | Size: 0x88 */
void fn_80251D2C(void* ctx, u32 param1, u32 param2) {
    extern void* fn_80205B8C();
    extern void fn_80120B00();
    extern void fn_8011BBD8();
    void* handle;
    u16 var_a;
    u16 var_8;

    handle = fn_8012640C(param2, 0, 0xd9, 0);
    fn_80120B00(fn_80205B8C(param2), &var_a, &var_8);
    fn_8011BBD8(handle, 0, 0x2f, 0, var_a);
    fn_8011BBD8(handle, 0, 0x30, 0, var_8);
}

/* Address: 0x80251DB4 | Size: 0x90 */
int fn_80251DB4(void* ctx, u32 param1, u32 param2) {
    extern u8 fn_80235B04(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fn_80235B04(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251E44 | Size: 0x90 */
int fn_80251E44(void* ctx, u32 param1, u32 param2) {
    extern u8 fn_80235B04(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fn_80235B04(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251ED4 | Size: 0x90 */
int fn_80251ED4(void* ctx, u32 param1, u32 param2) {
    extern u8 fn_80235B04(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fn_80235B04(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251F6C | Size: 0x84 | Pattern: field_accessor */
void fn_80251F6C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80251FF0 | Size: 0x40 | Ghidra import */
u32 fn_80251FF0(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    int iVar1;
  iVar1 = fn_80211170(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}


/* Address: 0x80252038 | Size: 0x40 | Ghidra import */
u32 fn_80252038(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80252078();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80252078,0);
}


/* Address: 0x80252078 | Size: 0x44 | Pattern: field_accessor */
u32 fn_80252078(void* ctx, u32 slot, u32 param) {
    extern u32 fn_8011BBD8();
    u32 val = (u32)fn_8012640C((void*)param, 0, 0xd9, 0);
    return (u32)fn_8011BBD8(val, 0, 0x2f, 0, 0x46);
}

/* Address: 0x802520BC | Size: 0x84 | Pattern: field_accessor */
void fn_802520BC(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252148 | Size: 0x40 | Ghidra import */
u32 fn_80252148(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80252188();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80252188,0);
}


/* Address: 0x80252188 | Size: 0x80 | Pattern: field_accessor */
void fn_80252188(void* ctx, u32 slot, u32 param) {
    extern void fn_8011BBD8();
    extern void fn_8011BEB4();
    extern void fn_80217BD0();
    extern void fn_8023842C();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r4 = 0x0;
    r6 = 0x0;
    r30 = r5;
    r29 = r3;
    r5 = 0xd9;
    r3 = r30;
    ((void(*)(void))fn_8012640C)();
    r4 = 0x0;
    r31 = r3;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r29;
    r4 = r30;
    fn_8023842C();
    fn_80217BD0();
    r0 = r3;
    r3 = r31;
    r7 = r0 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BBD8();
    return;
}

/* Address: 0x80252208 | Size: 0x40 | Ghidra import */
u32 fn_80252208(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80252248();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80252248,0);
}


/* Address: 0x80252248 | Size: 0x44 | Pattern: field_accessor */
u32 fn_80252248(void* ctx, u32 slot, u32 param) {
    extern u32 fn_8011BBD8();
    u32 val = (u32)fn_8012640C((void*)param, 0, 0xd9, 0);
    return (u32)fn_8011BBD8(val, 0, 0x2f, 0, 0x28);
}

/* Address: 0x8025228C | Size: 0x40 | Ghidra import */
u32 fn_8025228C(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_802522CC();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_802522CC,0);
}


/* Address: 0x802522CC | Size: 0x80 | Pattern: field_accessor */
void fn_802522CC(void* ctx, u32 slot, u32 param) {
    extern void fn_8011BBD8();
    extern void fn_8011BEB4();
    extern void fn_80217BEC();
    extern void fn_8023842C();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r4 = 0x0;
    r6 = 0x0;
    r30 = r5;
    r29 = r3;
    r5 = 0xd9;
    r3 = r30;
    ((void(*)(void))fn_8012640C)();
    r4 = 0x0;
    r31 = r3;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r29;
    r4 = r30;
    fn_8023842C();
    fn_80217BEC();
    r0 = r3;
    r3 = r31;
    r7 = r0 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BBD8();
    return;
}

/* Address: 0x80252354 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80252354(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252398 | Size: 0x40 | Ghidra import */
u32 fn_80252398(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_802523D8();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_802523D8,0);
}


/* Address: 0x802523D8 | Size: 0x90 */
void fn_802523D8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BBD8();
    extern void fn_8011BEB4();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r4 = 0x0;
    r6 = 0x0;
    r29 = r5;
    r28 = r3;
    r5 = 0xd9;
    r3 = r29;
    ((void(*)(void))fn_8012640C)();
    r4 = 0x0;
    r31 = r3;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BEB4();
    r30 = r3 & 0xFFFF;
    r3 = r28;
    r4 = r29;
    r5 = 0x1a;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
    }
    r3 = r31;
    r7 = r30 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BBD8();
    return;
}

/* Address: 0x802524B8 | Size: 0x84 | Pattern: field_accessor */
void fn_802524B8(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x8025253C | Size: 0xB4 */
void fn_8025253C(void* ctx, u32 param1, u32 param2) {
    extern void fn_80211170();
    extern void fn_802525F0();
    extern void fn_80252634();
    extern void fn_80252678();
    u8 sp[0x20];
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

    r7 = (u32)fn_80252678;
    r8 = 0x0;
    r9 = (u32)fn_80252678;
    r7 = 0x0;
    r10 = 0x0;
    r28 = r4;
    r29 = r5;
    r27 = r3;
    r30 = r6;
    r5 = r28;
    r4 = r29;
    fn_80211170();
    r4 = (u32)fn_80252634;
    r31 = r3;
    r9 = (u32)fn_80252634;
    r3 = r27;
    r4 = r29;
    r5 = r28;
    r6 = r30;
    r7 = 0x0;
    r8 = 0x0;
    r10 = 0x0;
    fn_80211170();
    r4 = (u32)fn_802525F0;
    r31 = r31 + r3;
    r9 = (u32)fn_802525F0;
    r3 = r27;
    r4 = r29;
    r5 = r28;
    r6 = r30;
    r7 = 0x0;
    r8 = 0x0;
    r10 = 0x0;
    fn_80211170();
    r31 = r31 + r3;
    r3 = r31;
    return;
}

/* Address: 0x802525F0 | Size: 0x44 | Pattern: field_accessor */
u32 fn_802525F0(void* ctx, u32 slot, u32 param) {
    extern u32 fn_8011BBD8();
    u32 val = (u32)fn_8012640C((void*)param, 0, 0xd9, 0);
    return (u32)fn_8011BBD8(val, 0, 0x2f, 0, 0x1e);
}

/* Address: 0x80252634 | Size: 0x44 | Pattern: field_accessor */
u32 fn_80252634(void* ctx, u32 slot, u32 param) {
    extern u32 fn_8011BBD8();
    u32 val = (u32)fn_8012640C((void*)param, 0, 0xd9, 0);
    return (u32)fn_8011BBD8(val, 0, 0x2f, 0, 0x14);
}

/* Address: 0x80252678 | Size: 0x44 | Pattern: field_accessor */
u32 fn_80252678(void* ctx, u32 slot, u32 param) {
    extern u32 fn_8011BBD8();
    u32 val = (u32)fn_8012640C((void*)param, 0, 0xd9, 0);
    return (u32)fn_8011BBD8(val, 0, 0x2f, 0, 0xa);
}

/* Address: 0x802526BC | Size: 0x84 | Pattern: field_accessor */
void fn_802526BC(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252748 | Size: 0x74 | Pattern: field_accessor */
void fn_80252748(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_802376EC();
    u8 sp[0x20];
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
    u32 r4 = slot;
    u32 r5 = param;

    r30 = r6;
    r28 = r4;
    r27 = r3;
    r29 = r5;
    r4 = r30;
    fn_802376EC();
    r31 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r28;
    r6 = r30;
    r7 = 0x0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_80211170();
    r4 = r31 & 0xFFFF;
    if ((s32)r4 <= (s32)r3) {
    }
    return;
}

/* Address: 0x802527C4 | Size: 0x40 | Ghidra import */
u32 fn_802527C4(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    extern void fn_80252804();
  return fn_80211170(r3,r5,r4,r6,0,0,(u32)fn_80252804,0);
}


/* Address: 0x80252804 | Size: 0x90 */
void fn_80252804(void* ctx, u32 param1, u32 param2) {
    extern void* fn_8012640C();
    extern u32 fn_802376EC();
    extern s32 fn_80237664();
    extern u8 fn_80218B6C();
    extern void fn_8011BBD8();
    void* a;
    u32 b;
    s32 t;

    a = fn_8012640C(param2, 0, 0xD9, 0);
    b = fn_802376EC(ctx, param2);
    t = fn_80237664(ctx, param2);
    fn_8011BBD8(a, 0, 0x2F, 0, fn_80218B6C(b, t));
}

/* Address: 0x80252894 | Size: 0x28 | Pattern: call_return_u16 */
extern u32 fn_802376EC(void*, u32, u32);
u16 fn_80252894(void* ctx, u32 p1, u32 p2, u32 p3) { return (u16)fn_802376EC(ctx, p3, p2); }

/* Address: 0x802528DC | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_802528DC(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252918 | Size: 0x54 | Pattern: field_accessor */
u32 fn_80252918(void* ctx, u32 slot, u32 arg2, u32 param) {
    extern u32 fn_802376EC();
    u32 val1, val2, avg;
    val1 = fn_802376EC(ctx, slot) & 0xFFFF;
    val2 = fn_802376EC(ctx, param) & 0xFFFF;
    avg = (s32)(val1 + val2) / 2;
    return val2 - avg;
}

/* Address: 0x8025297C | Size: 0x24 | Pattern: call_return_u8 */
extern u32 fn_80237774(void*);
u8 fn_8025297C(void* ctx) { return (u8)fn_80237774(ctx); }

/* Address: 0x802529A0 | Size: 0x24 | Pattern: call_return_u8 */
u8 fn_802529A0(void* ctx) { return (u8)fn_80237774(ctx); }

/* Address: 0x802529F4 | Size: 0x84 | Pattern: field_accessor */
void fn_802529F4(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252A80 | Size: 0x84 | Pattern: field_accessor */
void fn_80252A80(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252B04 | Size: 0x40 | Ghidra import */
u32 fn_80252B04(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    int iVar1;
  iVar1 = fn_80211170(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}


/* Address: 0x80252B44 | Size: 0x84 | Pattern: field_accessor */
void fn_80252B44(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252BC8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80252BC8(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252C04 | Size: 0x84 | Pattern: field_accessor */
void fn_80252C04(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252C88 | Size: 0x84 | Pattern: field_accessor */
void fn_80252C88(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252D0C | Size: 0x84 | Pattern: field_accessor */
void fn_80252D0C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252D90 | Size: 0x84 | Pattern: field_accessor */
void fn_80252D90(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252E14 | Size: 0x84 | Pattern: field_accessor */
void fn_80252E14(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252E98 | Size: 0x84 | Pattern: field_accessor */
void fn_80252E98(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80252F8C | Size: 0x84 | Pattern: field_accessor */
void fn_80252F8C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80253020 | Size: 0x84 | Pattern: field_accessor */
void fn_80253020(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802530A4 | Size: 0x40 | Ghidra import */
u32 fn_802530A4(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    int iVar1;
  iVar1 = fn_80211170(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}


/* Address: 0x802530E4 | Size: 0x84 | Pattern: field_accessor */
void fn_802530E4(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80253168 | Size: 0x88 */
void fn_80253168(void* ctx, u32 param1, u32 param2) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r30 = r5;
    r29 = r3;
    r31 = r6;
    r4 = r30;
    r5 = r0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r29;
    r5 = 0x21;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r30 & 0xFFFF;
        if (r0 == (u32)0xfa) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802531F8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_802531F8(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253234 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80253234(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253270 | Size: 0x28 | Pattern: call_return_u16 */
u16 fn_80253270(void* ctx, u32 p1, u32 p2, u32 p3) { return (u16)fn_802376EC(ctx, p3, p2); }

/* Address: 0x80253298 | Size: 0x28 | Ghidra import */
int fn_80253298(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 & 0xffff);
}
/* Address: 0x802532C0 | Size: 0x84 | Pattern: field_accessor */
void fn_802532C0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x8025334C | Size: 0x84 | Pattern: field_accessor */
void fn_8025334C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802533D8 | Size: 0x28 | Ghidra import */
int fn_802533D8(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 >> 1 & 0x7fff);
}
/* Address: 0x80253400 | Size: 0x84 | Pattern: field_accessor */
void fn_80253400(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x8025348C | Size: 0x40 | Ghidra import */
u32 fn_8025348C(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80211170();
    int iVar1;
  iVar1 = fn_80211170(r3,r5,r4,r6,0,0,0,0);
  return iVar1 * 3;
}


/* Address: 0x802534D4 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_802534D4(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253548 | Size: 0x84 | Pattern: field_accessor */
void fn_80253548(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802535F4 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_802535F4(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253630 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_80253630(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x8025366C | Size: 0x84 | Pattern: field_accessor */
void fn_8025366C(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802536F0 | Size: 0x84 | Pattern: field_accessor */
void fn_802536F0(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80253774 | Size: 0x84 | Pattern: field_accessor */
void fn_80253774(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802537F8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fn_802537F8(void* ctx, u32 param1, u32 param2, u32 param3) { return fn_80211170(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253834 | Size: 0x84 | Pattern: field_accessor */
void fn_80253834(void* ctx, u32 slot, u32 param) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r29 = r5;
    r28 = r3;
    r30 = r6;
    r4 = r29;
    r5 = r0;
    fn_80211170();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    r5 = 0x21;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x802538C0 | Size: 0x88 */
void fn_802538C0(void* ctx, u32 param1, u32 param2) {
    extern void fn_80211170();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r7 = 0x0;
    r8 = 0x0;
    r0 = r4;
    r9 = 0x0;
    r10 = 0x0;
    r30 = r5;
    r29 = r3;
    r31 = r6;
    r4 = r30;
    r5 = r0;
    fn_80211170();
    r0 = r3;
    r4 = r31;
    r3 = r29;
    r5 = 0x21;
    r31 = r0;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r30 & 0xFFFF;
        if (r0 == (u32)0x39) {
            r31 = r31 << 1;
        }
    }
    r3 = r31;
    return;
}

/* Address: 0x80253950 | Size: 0x6C | Pattern: field_accessor */
void fn_80253950(void* ctx, u32 slot, u32 param) {
    extern void fn_80136428();
    extern void fn_801F54A4();
    extern void fn_80237DBC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r5 = 0xf;
    r6 = 0x0;
    r30 = r3;
    r31 = r4;
    r3 = 0x0;
    r4 = 0x0;
    fn_801F54A4();
    r3 = r3 & 0xFFFF;
    fn_80136428();
    r0 = r3;
    r3 = r30;
    r4 = r31;
    r5 = r0 & 0xFF;
    fn_80237DBC();
    r0 = r3 & 0xFF;
    r3 = 0x1 - r0;
    r3 = r3 - r0; /* -borrow */;
    return;
}

/* Address: 0x802539BC | Size: 0xC4 (196 bytes) */
u32 fn_802539BC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235910(void*, u32);
    extern u32 fn_80235AA0(void);
    extern u8 fn_8025C808(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235AA0();
    secondStatus = fn_80235910(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 1, 0x41) & 0xff) == 0) {
        if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 3, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

/* Address: 0x80253A80 | Size: 0xC4 (196 bytes) */
u32 fn_80253A80(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235974(void*, u32);
    extern u32 fn_802359D8(void*, u32);
    extern u8 fn_8025C808(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_802359D8(ctx, param1);
    secondStatus = fn_80235974(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 4, 0x41) & 0xff) == 0) {
        if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 5, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

/* Address: 0x80253B44 | Size: 0x34 | Ghidra import */
int fn_80253B44(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x39);
  return (uVar1 & 0xFF) != 1;
}
/* Address: 0x80253B78 | Size: 0xB4 */
/* Address: 0x80253B78 | Size: 0xB4 */
void fn_80253B78(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80253C2C | Size: 0xC4 (196 bytes) */
u32 fn_80253C2C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235A3C(void*, u32);
    extern u32 fn_80235AA0(void*, u32);
    extern u8 fn_8025C808(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235AA0(ctx, param1);
    secondStatus = fn_80235A3C(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 1, 0x41) & 0xff) == 0) {
        if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 2, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

/* Address: 0x80253CF0 | Size: 0xB4 */
void fn_80253CF0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80253DA4 | Size: 0xC4 (196 bytes) */
u32 fn_80253DA4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235974(void*, u32);
    extern u32 fn_80235A3C(void*, u32);
    extern u8 fn_8025C808(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235A3C(ctx, param1);
    secondStatus = fn_80235974(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 2, 0x41) & 0xff) == 0) {
        if ((fn_8025C808(ctx, param1, param3, param2, 0x10, 5, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

/* Address: 0x80253E68 | Size: 0xC0 */
void fn_80253E68(void* ctx, u32 param1, u32 param2) {
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r6;
    r28 = r4;
    r27 = r3;
    r29 = r5;
    r4 = r30;
    fn_80235AA0();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    fn_80235A3C();
    r0 = r31 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r0 = r3 & 0xFF;
        if ((s32)r0 == (s32)0) {
            r3 = 0x0;
            return;
    }
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x1;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = r27;
        r4 = r28;
        r5 = r30;
        r6 = r29;
        r7 = 0x90;
        r8 = 0x2;
        r9 = 0x1;
        fn_8025C808();
        r0 = r3 & 0xFF;
        if ((s32)r0 == (s32)0) {
            r3 = 0x0;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80253F28 | Size: 0xB4 */
void fn_80253F28(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80253FDC | Size: 0xF0 (240 bytes) */
void fn_80253FDC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80235B04();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x2) {
        r30 = 0xb;

    } else if (r0 == (u32)0x3) {
        r30 = 0x5;

    } else if (r0 == (u32)0x1) {
        r30 = 0xa;

    } else if (r0 == (u32)0x4) {
        r30 = 0xf;

    } else {
        r30 = 0x0;
    }
    r3 = r27;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* -------------------------------------------------------------------
 * Item Rewards & Poke Coupon (0x80254000-0x80258000)
 * 95 functions
 * ------------------------------------------------------------------- */

/* Address: 0x802540CC | Size: 0xB4 */
void fn_802540CC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254180 | Size: 0x34 | Ghidra import */
int fn_80254180(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x38);
  return (uVar1 & 0xFF) != 1;
}
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
/* Address: 0x802541B4 | Size: 0xB4 */
void fn_802541B4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254268 | Size: 0x1F8 (504 bytes) */
void fn_80254268(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r31 = r6;
    r28 = r4;
    r29 = r5;
    r3 = 0x2;
    r4 = r31;
    fn_801F025C();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x0;
            goto L_802543A0;
        }
        r3 = r27;
        r4 = r31;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x48;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0xc;
        fn_80237F74();
    }
    r0 = 0x1;
L_802543A0:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = r29;
    r6 = r31;
    r7 = 0x0;
    fn_8025C264();
    r31 = r3;
    r3 = r30;
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254460 | Size: 0xB4 */
void fn_80254460(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254514 | Size: 0xB4 */
void fn_80254514(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802545C8 | Size: 0xB0 */
void fn_802545C8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80254680 | Size: 0x38 | Ghidra import */
int fn_80254680(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern u32 fn_80236BFC();
  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_80236BFC(r3,r6,0x28);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x802546B8 | Size: 0x30 | Ghidra import */
u32 fn_802546B8(void)

{
    extern u32 fn_8023720C();
  u32 uVar1;
  
  uVar1 = fn_8023720C();
  uVar1 = __cntlzw(1 - (uVar1 & 0xff));
  return uVar1 >> 5 & 0xff;
}
/* Address: 0x802546E8 | Size: 0x128 (296 bytes) */
void fn_802546E8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1C18();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    u8 sp[0x70];
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

    r6 = 0x0;
    r7 = 0x0;
    r5 = (u32)sp + 0x1c;
    r29 = r3;
    r27 = r4;
    fn_802367CC();
    r31 = r3;
    r4 = r29;
    r5 = (u32)sp + 0x30;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    r28 = r3;
    r3 = r29;
    r4 = r27;
    r5 = 0x27;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r27 = (u32)sp + 0x30;
    r28 = r28 & 0xFFFF;
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)r28) break;
        r4 = *(u32*)(r27 + r0);
        if (r4 != (u32)0x0) {
            r3 = r29;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x0;
            fn_802367CC();
            r5 = (u32)sp + 0x1c;
            r4 = r31 & 0xFFFF;
            r8 = (u32)sp + 0x8;
            r0 = r3 & 0xFFFF;
            r10 = 0x0;
            while (1) {
                r3 = r10 & 0xFFFF;
                if (r3 >= (u32)r0) break;
                r9 = 0x0;
                while (1) {
                    r3 = r9 & 0xFFFF;
                    if (r3 >= (u32)r4) break;
                    r6 = *(u16*)(r8 + r7);
                    r3 = *(u16*)(r5 + r3);
                    if (r6 == (u32)r3) {
                        r3 = 0x1;
                        return;
                    }
                    r9 = r9 + 0x1;

                }
                r10 = r10 + 0x1;

            }
        }
        r30 = r30 + 0x1;

    }
    r3 = 0x0;

    return;
}

/* Address: 0x80254810 | Size: 0xC8 (200 bytes) */
void fn_80254810(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80229934();
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r7 = (0x1 << 16);
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r28;
    r5 = r30;
    fn_80229934();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = 0x19;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x19;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802548D8 | Size: 0xB4 */
void fn_802548D8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025498C | Size: 0xE4 (228 bytes) */
void fn_8025498C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r4;
    r28 = r5;
    r26 = r3;
    r29 = r6;
    r5 = r27;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r26;
    r30 = r0;
    r4 = r27;
    fn_802376EC();
    r31 = r3;
    r3 = r26;
    r4 = r29;
    fn_802376EC();
    r3 = r3 & 0xFFFF;
    r0 = r31 & 0xFFFF;
    if (r3 <= (u32)r0) {
        r3 = 0x0;
        return;
    }
    r3 = r26;
    r4 = r27;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r26;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r26;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254A70 | Size: 0xB4 */
void fn_80254A70(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254B24 | Size: 0x178 (376 bytes) */
void fn_80254B24(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80229704();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_80237F74();
    extern void fn_8025C264();
    extern void fn_8025CC90();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r5;
    r30 = r6;
    r29 = r4;
    r28 = r3;
    r5 = 0x48;
    r4 = r30;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r30;
    r5 = 0xf;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r30;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r4 = r30;
    r3 = 0x2;
    fn_801F025C();
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r29;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_8025C264();
    r31 = r3;
    r3 = r28;
    r4 = r30;
    fn_8025CC90();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r30;
    r5 = 0x26;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r4 = r30;
    r3 = 0x8;
    fn_80229704();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254C9C | Size: 0xB0 */
void fn_80254C9C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80254D4C | Size: 0xB4 */
void fn_80254D4C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254E00 | Size: 0x2c | Ghidra import */
u32 fn_80254E00(void)

{
    extern u16 fn_80236B98();
  u16 sVar1;
  
  sVar1 = fn_80236B98();
  return sVar1 != 0;
}
/* Address: 0x80254E34 | Size: 0xB4 */
void fn_80254E34(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80254EE8 | Size: 0x34 | Ghidra import */
int fn_80254EE8(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x25);
  return (uVar1 & 0xFF) != 1;
}
/* Address: 0x80254F1C | Size: 0x38 | Ghidra import */
/* Address: 0x80254F1C | Size: 0x38 | Ghidra import */
u32 fn_80254F1C(u32 r3, u32 r4)
{
    extern u32 fn_80215008();
    u8 auStack_38[0x38];

    u32 uVar1 = fn_80215008(r3, auStack_38, 0x18, r4);
    return (-uVar1 & ~uVar1) >> 0x1f;
}
/* Address: 0x80254F54 | Size: 0x34 | Ghidra import */
int fn_80254F54(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x35);
  return (uVar1 & 0xFF) != 1;
}
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
/* Address: 0x80254F88 | Size: 0x78 | Pattern: field_accessor */
void fn_80254F88(void* ctx, u32 slot, u32 param) {
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = (0x1 << 16);
    r29 = r3;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x19;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80255000 | Size: 0xF0 (240 bytes) */
void fn_80255000(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80142984();
    extern void fn_80216048();
    extern void fn_80237F74();
    extern void fn_802383A4();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r3;
    r30 = r4;
    r29 = r6;
    fn_802383A4();
    r0 = r3;
    r3 = r28;
    r31 = r0;
    r4 = r29;
    fn_802383A4();
    r0 = r3;
    r3 = r30;
    r30 = r0;
    fn_80216048();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r0 = r31 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r0 = r30 & 0xFFFF;
        if ((s32)r0 == (s32)0) { r3 = 0x0; return; }
    }
    r3 = r31 & 0xFFFF;
    if (r3 == (u32)0xaf) { r3 = 0x0; return; }
    r0 = r30 & 0xFFFF;
    if (r0 == (u32)0xaf) { r3 = 0x0; return; }
    if (r3 != (u32)0x0) {
        r3 = r31;
        fn_80142984();
        r0 = r3 & 0xFF;
        if (r3 == (u32)0x0) { r3 = 0x0; return; }
    }
    r0 = r30 & 0xFFFF;
    if (r3 != (u32)0x0 || r3 != (u32)0x0) {
        r3 = r30;
        fn_80142984();
        r0 = r3 & 0xFF;

        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = r29;
    r5 = 0x3c;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802550F0 | Size: 0xB4 */
void fn_802550F0(void* ctx, u32 param1, u32 param2) {
    extern void fn_801F025C();
    extern void fn_801F54A4();
    extern void fn_802062FC();
    extern void fn_80236BFC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r29 = r3;
    r30 = r4;
    r3 = 0xe;
    fn_801F025C();
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x19;
    r6 = 0x0;
    fn_801F54A4();
    r0 = r3 & 0xFFFF;
    if (r0 < (u32)0x2) { r3 = 0x0; return; }
    r3 = r31;
    fn_802062FC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x0; return; }
    r3 = r29;
    r4 = r30;
    r5 = 0x32;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x0; return; }
    r3 = r29;
    r4 = r31;
    r5 = 0x32;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) { r3 = 0x1; return; }

    r3 = 0x0;
    return;

    r3 = 0x1;

    return;
}

/* Address: 0x802551A4 | Size: 0x74 | Pattern: field_accessor */
u32 fn_802551A4(void* ctx, u32 slot, u32 param, u32 param3) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_8025C264(void*, u32, u32, u32, u32);
    u32 result;

    result = fn_8025C264(ctx, slot, param, param3, 0);
    if ((fn_80236BFC(ctx, param3, 0x30) & 0xff) == 1) {
        return 0;
    }
    if ((s32)result == 0) {
        return 0;
    }
    if ((s32)result == -1) {
        return 1;
    }
    return 1;
}

/* Address: 0x80255220 | Size: 0xA8 */
void fn_80255220(void* ctx, u32 param1, u32 param2) {
    extern void fn_8011BEB4();
    extern void fn_801363E8();
    extern void fn_801F54A4();
    extern void fn_8023C530();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r27 = r3;
    r28 = r4;
    r30 = r5;
    r29 = r6;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0xf;
    r6 = 0x0;
    fn_801F54A4();
    r3 = r3 & 0xFFFF;
    fn_801363E8();
    r4 = r30;
    r30 = r3;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8011BEB4();
    r31 = r3 & 0xFFFF;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8011BEB4();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)r31) {
        r3 = r27;
        r4 = r28;
        r5 = r30;
        r6 = r29;
        fn_8023C530();
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802552D0 | Size: 0xB4 */
void fn_802552D0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80255384 | Size: 0xB4 */
void fn_80255384(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80255438 | Size: 0xB4 */
void fn_80255438(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802554EC | Size: 0x10C (268 bytes) */
void fn_802554EC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_802359D8();
    extern void fn_80235AA0();
    extern void fn_80236BFC();
    extern void fn_8025C808();
    extern void fn_8025CAA8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r6;
    r27 = r4;
    r26 = r3;
    r28 = r5;
    r4 = r29;
    fn_80235AA0();
    r30 = r3;
    r3 = r26;
    r4 = r29;
    fn_802359D8();
    r31 = r3;
    r3 = r26;
    r4 = r29;
    r5 = r28;
    fn_8025CAA8();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r0 = r30 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r31 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
    }
    }
    r3 = r26;
    r4 = r27;
    r5 = r29;
    r6 = r28;
    r7 = 0xa0;
    r8 = 0x1;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r27;
        r5 = r29;
        r6 = r28;
        r7 = 0xa0;
        r8 = 0x4;
        r9 = 0x1;
        fn_8025C808();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
    }
    }
    r3 = r26;
    r4 = r29;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802555F8 | Size: 0x228 (552 bytes) */
u32 fn_802555F8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern u8 fn_80237DBC();
    extern u8 fn_80237F74();
    extern u8 fn_80237310();
    extern s32 fn_8025C264();
    extern s32 fn_801F025C();
    extern u8 fn_801F6E98();
    s32 handle;
    u8 flag;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) return 0;
    if (fn_80236BFC(ctx, param3, 0x6) == 1) return 0;
    if (fn_80237DBC(ctx, param3, 0xA) == 1) return 0;
    if (fn_80237F74(ctx, param3, 0x29) == 1) return 0;

    if (fn_80237F74(ctx, param3, 0x11) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x14) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x7) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0xF) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x48) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x29) == 1) {
        flag = 0;
        goto check;
    }
    if (fn_80237F74(ctx, param3, 0x28) != 1) {
        fn_80237F74(ctx, param3, 0xC);
    }
flag1:
    flag = 1;
check:
    if (flag == 0) return 0;

    if (fn_80237310(ctx, param3) == 0) return 0;

    handle = fn_8025C264(ctx, param1, param2, param3, 0);
    if (fn_801F6E98(fn_801F025C(2, param3), 0x4B) == 1) return 0;
    if (handle == 0) return 0;
    if (handle == -1) return 1;
    return 1;
}

/* Address: 0x80255820 | Size: 0x218 (536 bytes) */
void fn_80255820(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80235910();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r3;
    r27 = r6;
    r25 = r4;
    r26 = r5;
    r3 = 0x2;
    r4 = r27;
    fn_801F025C();
    r0 = r3;
    r3 = r31;
    r28 = r0;
    r4 = r27;
    fn_80235910();
    r29 = r3;
    r3 = r31;
    r4 = r25;
    r5 = r26;
    r6 = r27;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r31;
    r4 = r27;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFF;
        if (r0 >= (u32)0xc) {
            r3 = r31;
            r4 = r25;
            r5 = r27;
            r6 = r26;
            r7 = 0x10;
            r8 = 0x4;
            r9 = 0x1;
            fn_8025C808();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0xc) {
                r3 = 0x0;
                return;
    }
    }
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x0;
            goto L_802559DC;
        }
        r3 = r31;
        r4 = r27;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x48;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0xc;
        fn_80237F74();
    }
    r0 = 0x1;
L_802559DC:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80255A38 | Size: 0x74 | Pattern: field_accessor */
u32 fn_80255A38(void* ctx, u32 slot, u32 param, u32 param3) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_8025C264(void*, u32, u32, u32, u32);
    u32 result;

    result = fn_8025C264(ctx, slot, param, param3, 0);
    if ((fn_80236BFC(ctx, param3, 0x1b) & 0xff) == 1) {
        return 0;
    }
    if ((s32)result == 0) {
        return 0;
    }
    if ((s32)result == -1) {
        return 1;
    }
    return 1;
}

/* Address: 0x80255AAC | Size: 0x38 | Ghidra import */
int fn_80255AAC(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_80235B04(r3,0,0);
  iVar2 = -(uVar1 & 0xff) + 4;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff) + 3);
}
/* Address: 0x80255AE4 | Size: 0x68 | Pattern: field_accessor */
void fn_80255AE4(void* ctx, u32 slot, u32 param) {
    extern f32 lbl_8047E648;
    extern void fn_80236BFC();
    extern void fn_802373B0();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = slot;
    u32 r5 = param;

    r5 = 0x2d;
    r30 = r3;
    r31 = r4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
    } else {

        f1 = lbl_8047E648;
        r3 = r30;
        r4 = r31;
        r5 = 0x0;
        fn_802373B0();
        r0 = r3 & 0xFF;
        r3 = 0x1 - r0;
        r3 = r3 - r0; /* -borrow */;
    }
    return;
}

/* Address: 0x80255B4C | Size: 0xCC (204 bytes) */
void fn_80255B4C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80236BFC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r4;
    r28 = r5;
    r26 = r3;
    r29 = r6;
    r5 = r27;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r26;
    r30 = r0;
    r4 = r27;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r26;
    r31 = r0;
    r4 = r27;
    r5 = 0x2d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r31 = 0x0;
    }
    r3 = r26;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r26;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80255C18 | Size: 0x74 | Pattern: field_accessor */
u32 fn_80255C18(void* ctx, u32 slot, u32 param) {
    extern u8 fn_80236BFC(void* ctx, u32 slot, u32 param);
    extern u32 fn_80202360(u32 slot, u32 param);
    extern u8 fn_80119DD0(u32 param);
    u32 result;

    result = slot;
    if (fn_80236BFC(ctx, slot, 0x2d) == 0) {
        result = 0;
    } else {
        result = fn_80202360(result, 0x2d);
    }
    if ((s16)result >= (s32)fn_80119DD0(0x2d)) {
        return 0;
    }
    return 1;
}

/* Address: 0x80255C8C | Size: 0xB0 */
void fn_80255C8C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80255D3C | Size: 0x40 | Ghidra import */
u32 fn_80255D3C(u32 r3_in, u32 r4)
{
    extern u32 fn_8012640C();
  u32 uVar1;
  u8 result;
  uVar1 = fn_8012640C(r4, 0, 0xed, 0) & 0xFFFF;
  result = uVar1 != 0;
  return result;
}
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
/* Address: 0x80255D7C | Size: 0x38 | Ghidra import */
int fn_80255D7C(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_802373B0();
    extern f32 lbl_8047E648;

  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_802373B0((double)lbl_8047E648,r3,r4,0);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x80255DB4 | Size: 0x44 | Pattern: field_accessor */
u32 fn_80255DB4(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u32 fn_8025C808();
    u32 result = fn_8025C808(ctx, slot, arg3, param, 0x10, 0x2, 0x41) & 0xFF;
    return (result != 0) ? 1 : 0;
}

/* Address: 0x80255DF8 | Size: 0xB0 */
void fn_80255DF8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80255EA8 | Size: 0x3c | Ghidra import */
u32 fn_80255EA8(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern int fn_8025C264();
  int iVar1;
  iVar1 = fn_8025C264(r3, r4, r5, r6, 0);
  if (iVar1 == 0) {
    return 0;
  }
  if (iVar1 == -1) {
    return 1;
  }
  return 1;
}
/* Address: 0x80255EEC | Size: 0xB4 */
void fn_80255EEC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80255FA0 | Size: 0xB4 */
void fn_80255FA0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256054 | Size: 0xB4 */
void fn_80256054(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256108 | Size: 0xB4 */
void fn_80256108(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802561BC | Size: 0x38 | Ghidra import */
int fn_802561BC(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern u32 fn_80236BFC();
  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_80236BFC(r3,r6,0x34);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x802561F4 | Size: 0xB4 */
void fn_802561F4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802562A8 | Size: 0xB4 */
void fn_802562A8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025635C | Size: 0xB4 */
void fn_8025635C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256410 | Size: 0xB0 */
void fn_80256410(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802564C8 | Size: 0x64 | Pattern: field_accessor */
u32 fn_802564C8(void* ctx, u32 slot, u32 param) {
    extern f32 lbl_8047E64C;
    extern u32 fn_80235AA0(void);
    extern u32 fn_802373B0(void*, u32, s32, f32);

    if ((fn_80235AA0() & 0xFF) >= 0xC) {
        goto ret0;
    }
    if ((fn_802373B0(ctx, slot, -1, lbl_8047E64C) & 0xFF) == 0) {
        goto ret1;
    }
ret0:
    return 0;
ret1:
    return 1;
}

/* Address: 0x8025652C | Size: 0xB4 */
void fn_8025652C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802565E0 | Size: 0xB4 */
void fn_802565E0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256694 | Size: 0xB4 */
void fn_80256694(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256748 | Size: 0x38 | Ghidra import */
int fn_80256748(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;
  
  uVar1 = fn_80235B04(r3,0,0);
  return (uVar1 & 0xff) != 1;
}
/* Address: 0x80256780 | Size: 0x38 | Ghidra import */
int fn_80256780(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_80235B04(r3,0,0);
  iVar2 = -(uVar1 & 0xff) + 2;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff) + 1);
}
/* Address: 0x802567B8 | Size: 0xA4 */
void fn_802567B8(void* ctx, u32 param1, u32 param2) {
    extern void fn_80120B00();
    extern void fn_80205B8C();
    extern void fn_8023793C();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;

    r29 = r4;
    r28 = r3;
    r31 = r5;
    r30 = r6;
    r3 = r29;
    fn_80205B8C();
    r4 = (u32)sp + 0xa;
    r5 = (u32)sp + 0x8;
    fn_80120B00();
    r3 = r28;
    r4 = r29;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_8025C264();
    r0 = *(u16*)(sp + 0xA);
    r31 = r3;
    r5 = *(u16*)(sp + 0x8);
    r3 = r28;
    r4 = r30;
    r6 = (s16)r0;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025685C | Size: 0x38 | Ghidra import */
int fn_8025685C(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_802373B0();
    extern f32 lbl_8047E648;

  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_802373B0((double)lbl_8047E648,r3,r4,0);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x80256894 | Size: 0x38 | Ghidra import */
int fn_80256894(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_802373B0();
    extern f32 lbl_8047E648;

  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_802373B0((double)lbl_8047E648,r3,r4,0);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x802568CC | Size: 0x38 | Ghidra import */
int fn_802568CC(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_802373B0();
    extern f32 lbl_8047E648;

  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_802373B0((double)lbl_8047E648,r3,r4,0);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x80256904 | Size: 0xB0 */
void fn_80256904(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802569B4 | Size: 0xB4 */
void fn_802569B4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256A68 | Size: 0x78 | Pattern: field_accessor */
u32 fn_80256A68(void* ctx, u32 a, u32 b, u32 c) {
    extern s32 fn_802395C8();
    extern s32 fn_80239500();
    extern s32 fn_8023793C();
    s32 res1;

    res1 = fn_802395C8(ctx, b, a);
    return !((u16)fn_8023793C(ctx, c, res1, fn_80239500(ctx, b)) == 0x43);
}

/* Address: 0x80256AE0 | Size: 0x38 | Ghidra import */
u32 fn_80256AE0(void)
{
    extern int fn_801F8A18();
  u32 r3;
  int iVar1;
  u16 local_8[4];

  local_8[0] = 0;
  iVar1 = fn_801F8A18(r3, local_8);
  return (-iVar1 != 0) ? 1 : 0;
}
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
/* Address: 0x80256B18 | Size: 0xB4 */
void fn_80256B18(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256BCC | Size: 0xB4 */
void fn_80256BCC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256C80 | Size: 0x3c | Ghidra import */
u32 fn_80256C80(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F6E44();
  u32 uVar1;
  u32 uVar2;
  
  uVar1 = fn_801F025C(2);
  uVar2 = fn_801F6E44(uVar1,0x4b);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}
/* Address: 0x80256CBC | Size: 0xB4 */
void fn_80256CBC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80256D70 | Size: 0xB0 */
void fn_80256D70(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80256E20 | Size: 0xB0 */
void fn_80256E20(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80256ED0 | Size: 0x200 (512 bytes) */
void fn_80256ED0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_802016A4();
    extern void fn_80236BFC();
    extern void fn_80237288();
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r26 = r4;
    r27 = r3;
    r29 = r5;
    r28 = r6;
    r3 = r26;
    fn_802016A4();
    r30 = r3;
    r3 = r28;
    fn_802016A4();
    r31 = r3;
    r3 = r27;
    r4 = r26;
    r5 = r29;
    r6 = r28;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r29 = r0;
    r4 = r28;
    r5 = 0xc;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r29 = 0x0;
    }
    r3 = r27;
    r4 = r28;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r28;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r28;
            r5 = 0x7;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r27;
                r4 = r28;
                r5 = 0xf;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                    r3 = r27;
                    r4 = r28;
                    r5 = 0x48;
                    fn_80237F74();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                        r3 = r27;
                        r4 = r28;
                        r5 = 0x29;
                        fn_80237F74();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r27;
                            r4 = r28;
                            r5 = 0x28;
                            fn_80237F74();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x1) {
                                r3 = r27;
                                r4 = r28;
                                r5 = 0xc;
                                fn_80237F74();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r0 = 0x0;
                                    goto L_80257034;
    }
    }
    }
    }
    }
    }
    }
    }
    r0 = 0x1;
L_80257034:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r30 & 0xFF;
    r0 = r31 & 0xFF;
    if (r3 != (u32)r0) {
        r3 = r27;
        r4 = r28;
        fn_80237288();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r28;
            r5 = 0xa;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r0 = r30 & 0xFF;
                if (r0 != (u32)0x2) {
                    r0 = r31 & 0xFF;
                    if (r0 == (u32)0x2) {
        }
        }
        }
        }
        r29 = 0x0;
                    }
    if ((s32)r29 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802570D0 | Size: 0xB0 */
void fn_802570D0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80257180 | Size: 0x23C (572 bytes) */
void fn_80257180(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80235AA0();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r3;
    r27 = r6;
    r25 = r4;
    r26 = r5;
    r3 = 0x2;
    r4 = r27;
    fn_801F025C();
    r0 = r3;
    r3 = r31;
    r28 = r0;
    r4 = r27;
    fn_80235AA0();
    r29 = r3;
    r3 = r31;
    r4 = r27;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r31;
    r4 = r25;
    r5 = r26;
    r6 = r27;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r31;
    r4 = r27;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFF;
        if (r0 >= (u32)0xc) {
            r3 = r31;
            r4 = r25;
            r5 = r27;
            r6 = r26;
            r7 = 0x20;
            r8 = 0x1;
            r9 = 0x1;
            fn_8025C808();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0xc) {
                r3 = 0x0;
                return;
    }
    }
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1) {
        r3 = r31;
        r4 = r27;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x0;
            goto L_80257360;
        }
        r3 = r31;
        r4 = r27;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x48;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r31;
        r4 = r27;
        r5 = 0xc;
        fn_80237F74();
    }
    r0 = 0x1;
L_80257360:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r28;
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802573BC | Size: 0xB0 */
void fn_802573BC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80257474 | Size: 0x38 | Ghidra import */
int fn_80257474(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_80235B04(r3,0,0);
  iVar2 = -(uVar1 & 0xff) + 3;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff) + 2);
}
/* Address: 0x802574AC | Size: 0x5C | Pattern: field_accessor */
u32 fn_802574AC(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u32 fn_80229934();
    extern u32 fn_8025C674();

    if ((u8)fn_80229934(param, slot, arg3) == 1) {
        return 0;
    }
    return (u16)fn_8025C674(ctx) != 0;
}

/* Address: 0x80257508 | Size: 0x3c | Ghidra import */
u32 fn_80257508(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern int fn_8025C264();
  int iVar1;
  iVar1 = fn_8025C264(r3, r4, r5, r6, 0);
  if (iVar1 == 0) {
    return 0;
  }
  if (iVar1 == -1) {
    return 1;
  }
  return 1;
}
/* Address: 0x80257544 | Size: 0x7C | Pattern: field_accessor */
void fn_80257544(void* ctx, u32 slot, u32 param) {
    extern void fn_80119DD0();
    extern void fn_801F025C();
    extern void fn_801F6D9C();
    extern void fn_801F6E98();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;

    r3 = 0x3;
    fn_801F025C();
    r30 = 0x0;
    r31 = r3;
    r4 = 0x4a;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = 0x4a;
        fn_801F6D9C();
        r30 = r3;
    }
    r3 = 0x4a;
    fn_80119DD0();
    r3 = r3 & 0xFF;
    r0 = (s16)r30;
    if ((s32)r0 >= (s32)r3) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802575C8 | Size: 0x188 (392 bytes) */
void fn_802575C8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80235910();
    extern void fn_80235A3C();
    extern void fn_80235AA0();
    extern void fn_80236BFC();
    extern void fn_80237288();
    extern void fn_80237DBC();
    extern void fn_8025C808();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r28 = r6;
    r26 = r4;
    r25 = r3;
    r27 = r5;
    r4 = r28;
    fn_80235AA0();
    r29 = r3;
    r3 = r25;
    r4 = r28;
    fn_80235A3C();
    r30 = r3;
    r3 = r25;
    r4 = r28;
    fn_80235910();
    r31 = r3;
    r3 = r25;
    r4 = r26;
    r5 = 0x7;
    fn_80237DBC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r25;
        r4 = r28;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r25;
        r4 = r28;
        r5 = 0x18;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) { r3 = 0x0; return; }
        r3 = r25;
        r4 = r28;
        fn_80237288();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) { r3 = 0x1; return; }

        r3 = 0x0;
        return;
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r29 & 0xFF;
        if (r0 >= (u32)0xc) {
            r0 = r30 & 0xFF;
            if (r0 >= (u32)0xc) {
                r3 = 0x0;
                return;
    }
    }
    }
    r3 = r25;
    r4 = r26;
    r5 = r28;
    r6 = r27;
    r7 = 0x90;
    r8 = 0x3;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0xc) { r3 = 0x1; return; }
    r3 = r25;
    r4 = r26;
    r5 = r28;
    r6 = r27;
    r7 = 0x10;
    r8 = 0x1;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0xc) { r3 = 0x1; return; }
    r3 = r25;
    r4 = r26;
    r5 = r28;
    r6 = r27;
    r7 = 0x10;
    r8 = 0x2;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0xc) { r3 = 0x1; return; }
    r3 = 0x0;
    return;

    r3 = 0x1;

    return;
}

/* Address: 0x80257750 | Size: 0xE8 (232 bytes) */
void fn_80257750(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x10;
    r8 = 0x7;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257838 | Size: 0x8C */
void fn_80257838(void* ctx, u32 param1, u32 param2) {
    extern void fn_80236BFC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r5 = 0x14;
    r31 = r6;
    r30 = r3;
    r4 = r31;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x17;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x8;
    fn_80236BFC();
    r3 = r3 & 0xFF;
    r3 = r3 - r0; /* -borrow */;

    return;
}

/* Address: 0x802578C4 | Size: 0x78 | Pattern: field_accessor */
void fn_802578C4(void* ctx, u32 slot, u32 param) {
    extern void fn_80236BFC();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = (0x1 << 16);
    r29 = r3;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x16;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025793C | Size: 0xB4 */
void fn_8025793C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802579F0 | Size: 0xB0 */
void fn_802579F0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80257AA0 | Size: 0xB4 */
void fn_80257AA0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257B54 | Size: 0xCC (204 bytes) */
void fn_80257B54(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_80238748();
    extern void fn_80239058();
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
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
    r25 = r3;
    r26 = r5;
    r5 = (u32)sp + 0x8;
    r4 = r25;
    r3 = 0x0;
    fn_801F1A6C();
    r28 = (u32)sp + 0x8;
    r31 = r3 & 0xFFFF;
    r30 = r26 & 0xFFFF;
    r26 = 0x0;
    r27 = 0x0;
    while (1) {
        r0 = r27 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r4 = *(u32*)(r28 + r29);
        if (r4 == (u32)0x0 || r0 == (u32)0x1 || r0 == (u32)0x1) {
            if (r30 == (u32)0xd7) {
                r3 = r25;
                r5 = 0x2b;
                fn_80239058();
                r0 = r3 & 0xFF;

            }
            r4 = *(u32*)(r28 + r29);
            r3 = r25;
            fn_80238748();
            r0 = r3 & 0xFF;

            r26 = 0x1;
            break;
        }
        r27 = r27 + 0x1;

    }

    r0 = r26 & 0xFF;
    if (r0 == (u32)r31) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80257C20 | Size: 0xB4 */
void fn_80257C20(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257CD4 | Size: 0x124 (292 bytes) */
void fn_80257CD4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_802364BC();
    extern void fn_802367CC();
    extern void fn_80237288();
    extern void fn_802381C4();
    extern void fn_8025C264();
    u8 sp[0x50];
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

    r7 = (0x1 << 16);
    r30 = r3;
    r31 = r6;
    fn_8025C264();
    r27 = r3;
    r3 = r30;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r6 = (u32)sp + 0x8;
    r7 = 0x0;
    fn_802367CC();
    r28 = r3;
    r3 = r30;
    r4 = r31;
    fn_802364BC();
    r0 = r3 & 0xFFFF;
    r29 = r3;
    if ((s32)r0 == (s32)0) { r3 = 0x0; return; }
    if (r0 == (u32)0x165) { r3 = 0x0; return; }
    if (r0 == (u32)0xffff) { r3 = 0x0; return; }
    r3 = r30;
    r4 = r31;
    fn_80237288();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {

        r3 = 0x0;
        return;
    }
    r7 = r28 & 0xFFFF;
    r6 = (u32)sp + 0x8;
    r5 = r29 & 0xFFFF;
    r4 = (u32)sp + 0x1c;
    r3 = 0x0;
    r10 = 0x0;
    while (1) {
        r0 = r10 & 0xFF;
        if ((s32)r0 >= (s32)r7) break;
        r9 = *(s16*)(r6 + r8);
        r0 = (s16)r9;
        if (r0 >= (u32)0x1) {
            r0 = *(u16*)(r4 + r8);
            if (r5 == (u32)r0) {
                r3 = r30;
                r4 = r31;
                r5 = r9 & 0xFF;
                fn_802381C4();
                break;
        }
        }
        r10 = r10 + 0x1;

    }

    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)r7) {
        r3 = 0x0;
        return;
    }
    if ((s32)r27 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257DF8 | Size: 0xB4 */
void fn_80257DF8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257EAC | Size: 0x38 | Ghidra import */
int fn_80257EAC(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    extern u32 fn_80229934();

    return (u8)fn_80229934(arg2, arg1, arg3) != 1;
}
/* Address: 0x80257EE4 | Size: 0xE4 (228 bytes) */
void fn_80257EE4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80218FDC();
    extern void fn_8021901C();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r6 = 0x0;
    r7 = 0x1;
    r5 = (u32)sp + 0x8;
    r28 = r3;
    r27 = r4;
    fn_802367CC();
    r29 = r3;
    r3 = r28;
    r4 = r27;
    r5 = 0x8;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r30 = (u32)sp + 0x8;
    r31 = r29 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r27 = *(u16*)(r30 + r0);
        if (r27 != (u32)0x165 && r27 != (u32)0x163) {

            r3 = r27;
            fn_80218FDC();
            r0 = r3 & 0xFF;
            if (r27 == (u32)0x163 && r27 != (u32)0x108 && r27 != (u32)0xfd) {

                r3 = r27;
                fn_8021901C();
                r0 = r3 & 0xFF;
                if (r27 == (u32)0xfd) break;
        }
        }
        r28 = r28 + 0x1;

    }

    r3 = r28 & 0xFFFF;
    r0 = r29 & 0xFFFF;
    if (r3 >= (u32)r0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80257FC8 | Size: 0xF4 (244 bytes) */
void fn_80257FC8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80219270();
    extern void fn_80236458();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r7 = 0x0;
    r5 = (u32)sp + 0x8;
    r29 = r6;
    r27 = r3;
    r28 = r4;
    r6 = 0x0;
    fn_802367CC();
    r31 = r3;
    r3 = r27;
    r4 = r29;
    fn_80236458();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r29;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = 0x10;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) { r3 = 0x0; return; }
    r3 = r30;
    fn_80219270();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {

        r3 = 0x0;
        return;
    }
    r5 = r30 & 0xFFFF;
    r4 = (u32)sp + 0x8;
    r0 = r31 & 0xFFFF;
    r6 = 0x0;
    while (1) {
        r3 = r6 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u16*)(r4 + r3);
        if (r5 == (u32)r3) {
            r3 = 0x0;
            return;
        }
        r6 = r6 + 0x1;

    }
    r3 = 0x1;

    return;
}

/* -------------------------------------------------------------------
 * Team State Updates (0x80258000-0x8025C000)
 * 78 functions
 * ------------------------------------------------------------------- */

/* Address: 0x802580BC | Size: 0x78 | Pattern: field_accessor */
void fn_802580BC(void* ctx, u32 slot, u32 param) {
    extern void fn_80236BFC();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = (0x1 << 16);
    r29 = r3;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025813C | Size: 0xD0 (208 bytes) */
void fn_8025813C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80236BFC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = 0x8;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025820C | Size: 0x78 | Pattern: field_accessor */
void fn_8025820C(void* ctx, u32 slot, u32 param) {
    extern void fn_80236BFC();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;
    u32 r5 = param;

    r7 = (0x1 << 16);
    r29 = r3;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258284 | Size: 0x140 (320 bytes) */
void fn_80258284(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80219804();
    extern void fn_802364BC();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802381C4();
    extern void fn_8025C264();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r7 = 0x0;
    r31 = r6;
    r26 = r4;
    r29 = r5;
    r30 = r3;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r6 = (u32)sp + 0x8;
    fn_802367CC();
    r28 = r3;
    r3 = r30;
    r4 = r31;
    fn_802364BC();
    r0 = r3;
    r3 = r30;
    r27 = r0;
    r4 = r26;
    r5 = r29;
    r6 = r31;
    r7 = 0x0;
    fn_8025C264();
    r29 = r3;
    r3 = r27;
    fn_80219804();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r7 = (u32)sp + 0x8;
    r6 = r27 & 0xFFFF;
    r5 = (u32)sp + 0x1c;
    r0 = r28 & 0xFFFF;
    r3 = 0x0;
    r10 = 0x0;
    while (1) {
        r4 = r10 & 0xFFFF;
        if (r4 >= (u32)r0) break;
        r9 = *(s16*)(r7 + r8);
        r4 = (s16)r9;
        if (r0 >= (u32)0x1) {
            r4 = *(u16*)(r5 + r8);
            if (r6 == (u32)r4) {
                r3 = r30;
                r4 = r31;
                r5 = r9 & 0xFF;
                fn_802381C4();
                break;
        }
        }
        r10 = r10 + 0x1;

    }

    r0 = r3 & 0xFF;
    if (r4 == (u32)r0) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x2a;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r29 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802583C4 | Size: 0xB0 */
void fn_802583C4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80258474 | Size: 0xB0 */
void fn_80258474(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80258524 | Size: 0xB0 */
void fn_80258524(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802585D4 | Size: 0x120 (288 bytes) */
void fn_802585D4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_802364BC();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802381C4();
    extern void fn_8025C264();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r7 = 0x0;
    r31 = r6;
    r26 = r4;
    r29 = r5;
    r30 = r3;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r6 = (u32)sp + 0x8;
    fn_802367CC();
    r27 = r3;
    r3 = r30;
    r4 = r31;
    fn_802364BC();
    r28 = r3;
    r3 = r30;
    r4 = r26;
    r5 = r29;
    r6 = r31;
    r7 = 0x0;
    fn_8025C264();
    r29 = r3;
    r7 = (u32)sp + 0x8;
    r6 = r28 & 0xFFFF;
    r5 = (u32)sp + 0x1c;
    r0 = r27 & 0xFFFF;
    r3 = 0x0;
    r10 = 0x0;
    while (1) {
        r4 = r10 & 0xFFFF;
        if (r4 >= (u32)r0) break;
        r9 = *(s16*)(r7 + r8);
        r4 = (s16)r9;
        if ((s32)r0 >= (s32)0) {
            r4 = *(u16*)(r5 + r8);
            if (r6 == (u32)r4) {
                r3 = r30;
                r4 = r31;
                r5 = r9 & 0xFF;
                fn_802381C4();
                break;
        }
        }
        r10 = r10 + 0x1;

    }

    r0 = r3 & 0xFF;
    if (r4 == (u32)r0) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x29;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r29 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802586FC | Size: 0xBC */
void fn_802586FC(void* ctx, u32 param1, u32 param2) {
    extern void fn_80236BFC();
    extern void fn_80237DBC();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r7 = 0x0;
    r29 = r3;
    r30 = r6;
    fn_8025C264();
    r31 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0xc;
    fn_80237DBC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802587C0 | Size: 0x144 (324 bytes) */
void fn_802587C0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021A2C0();
    extern void fn_802364BC();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
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

    r7 = 0x0;
    r28 = r5;
    r31 = r6;
    r26 = r3;
    r27 = r4;
    r5 = (u32)sp + 0x8;
    r6 = 0x0;
    fn_802367CC();
    r30 = r3;
    r3 = r26;
    r4 = r31;
    fn_802364BC();
    r0 = r3;
    r3 = r26;
    r29 = r0;
    r4 = r31;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r7 = (0x1 << 16);
    r3 = r26;
    r4 = r27;
    r5 = r28;
    r6 = r31;
    fn_8025C264();
    r31 = r3;
    r3 = r26;
    r4 = r27;
    r5 = 0x10;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) { r3 = 0x0; return; }
    r3 = r29;
    fn_8021A2C0();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x0; return; }
    r5 = r29 & 0xFFFF;
    if (r0 == (u32)0x1) { r3 = 0x0; return; }
    if (r5 == (u32)0xffff) { r3 = 0x0; return; }
    if (r5 == (u32)0x165) { r3 = 0x0; return; }
    if (r5 == (u32)0x163) {

        r3 = 0x0;
        return;
    }
    r4 = (u32)sp + 0x8;
    r0 = r30 & 0xFFFF;
    r6 = 0x0;
    while (1) {
        r3 = r6 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u16*)(r4 + r3);
        if (r5 == (u32)r3) {
            r3 = 0x0;
            return;
        }
        r6 = r6 + 0x1;

    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258904 | Size: 0xB0 */
void fn_80258904(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x802589B4 | Size: 0xB0 */
void fn_802589B4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x80258A64 | Size: 0x6C | Pattern: field_accessor */
u32 fn_80258A64(void* ctx, u32 slot, u32 param) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_802373B0(void*, u32, s32, f32);
    extern f32 lbl_8047E650;
    u32 result;

    if ((fn_80236BFC(ctx, slot, 0x14) & 0xff) == 1) {
        return 0;
    }

    result = fn_802373B0(ctx, slot, -1, lbl_8047E650) & 0xff;
    return result != 1;
}

/* Address: 0x80258AD0 | Size: 0xB4 */
void fn_80258AD0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258B84 | Size: 0xB4 */
void fn_80258B84(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258C38 | Size: 0xB4 */
void fn_80258C38(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258CEC | Size: 0xB4 */
void fn_80258CEC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258DA0 | Size: 0xB4 */
void fn_80258DA0(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258E54 | Size: 0xB4 */
void fn_80258E54(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258F08 | Size: 0xB4 */
void fn_80258F08(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80258FBC | Size: 0xB4 */
void fn_80258FBC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259070 | Size: 0xB4 */
void fn_80259070(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259124 | Size: 0xB4 */
void fn_80259124(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802591D8 | Size: 0x260 (608 bytes) */
void fn_802591D8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_8023793C();
    extern void fn_80237F74();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r30 = r6;
    r28 = r4;
    r29 = r5;
    r3 = 0x2;
    r4 = r30;
    fn_801F025C();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r29;
    r5 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r26 = r0;
    r4 = r30;
    r5 = 0x7;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r30;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x0;
            goto L_80259328;
        }
        r3 = r27;
        r4 = r30;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r30;
        r5 = 0x48;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r30;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r30;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r30;
        r5 = 0xc;
        fn_80237F74();
    }
    r0 = 0x1;
L_80259328:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r29;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r26;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    fn_80237310();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = r29;
    r6 = r30;
    r7 = 0x0;
    fn_8025C264();
    r27 = r3;
    r3 = r31;
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r27 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259438 | Size: 0x270 (624 bytes) */
u8 fn_80259438(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u8 fn_80237310(void*, u32);
    extern s32 fn_8025C264(void*, u32, u32, u32, u32);
    extern u32 fn_801F025C(u32, u32);
    extern u8 fn_801F6E98(u32, u32);
    s32 r;
    u8 flag;

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x4) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x8) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
        return 0;
    }

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        flag = 0;
    } else {
        if (fn_80237F74(ctx, param3, 0x14) != 1) {
            if (fn_80237F74(ctx, param3, 0x7) != 1) {
                if (fn_80237F74(ctx, param3, 0xf) != 1) {
                    if (fn_80237F74(ctx, param3, 0x48) != 1) {
                        if (fn_80237F74(ctx, param3, 0x29) != 1) {
                            if (fn_80237F74(ctx, param3, 0x28) != 1) {
                                fn_80237F74(ctx, param3, 0xc);
                            }
                        }
                    }
                }
            }
        }
        flag = 1;
    }
    if (flag == 0) {
        return 0;
    }

    r = fn_8025C264(ctx, param1, param2, param3, 0);
    if (fn_801F6E98(fn_801F025C(2, param3), 0x4b) == 1) {
        return 0;
    }
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        return 1;
    }
    return 1;
}

/* Address: 0x802596A8 | Size: 0x3c | Ghidra import */
u32 fn_802596A8(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F6E44();
  u32 uVar1;
  u32 uVar2;
  
  uVar1 = fn_801F025C(2);
  uVar2 = fn_801F6E44(uVar1,0x48);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}
/* Address: 0x802596E4 | Size: 0xE8 (232 bytes) */
void fn_802596E4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0xa0;
    r8 = 0x5;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802597CC | Size: 0xE8 (232 bytes) */
void fn_802597CC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0xa0;
    r8 = 0x3;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x802598B4 | Size: 0xE8 (232 bytes) */
void fn_802598B4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0xa0;
    r8 = 0x2;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025999C | Size: 0xE8 (232 bytes) */
void fn_8025999C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0xa0;
    r8 = 0x1;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0xa0;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259A84 | Size: 0x68 | Pattern: field_accessor */
int fn_80259A84(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237288(void*, u32);

    if ((u8)fn_80236BFC(ctx, arg3, 0x10) == 1) {
        goto fail;
    }
    if ((u8)fn_80237288(ctx, arg3) != 1) {
        goto success;
    }
fail:
    return 0;
success:
    return 1;
}

/* Address: 0x80259AEC | Size: 0xE8 (232 bytes) */
void fn_80259AEC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x20;
    r8 = 0x5;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259BD4 | Size: 0xE8 (232 bytes) */
void fn_80259BD4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x20;
    r8 = 0x4;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259CBC | Size: 0xE8 (232 bytes) */
void fn_80259CBC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x20;
    r8 = 0x3;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259DA4 | Size: 0xE8 (232 bytes) */
void fn_80259DA4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x20;
    r8 = 0x2;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259E8C | Size: 0xE8 (232 bytes) */
void fn_80259E8C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x20;
    r8 = 0x1;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x20;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x80259F74 | Size: 0x1F8 (504 bytes) */
void fn_80259F74(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r31 = r6;
    r28 = r4;
    r29 = r5;
    r3 = 0x2;
    r4 = r31;
    fn_801F025C();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = 0x14;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1 && r0 != (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x0;
            goto L_8025A0F4;
        }
        r3 = r27;
        r4 = r31;
        r5 = 0x7;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0xf;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x48;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x29;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0x28;
        fn_80237F74();
        r0 = r3 & 0xFF;

        r3 = r27;
        r4 = r31;
        r5 = 0xc;
        fn_80237F74();
    }
    r0 = 0x1;
L_8025A0F4:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = r29;
    r6 = r31;
    r7 = 0x0;
    fn_8025C264();
    r31 = r3;
    r3 = r30;
    r4 = 0x4b;
    fn_801F6E98();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A16C | Size: 0xB4 */
void fn_8025A16C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A220 | Size: 0x34 | Ghidra import */
int fn_8025A220(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  return (fn_80236BFC(r3, r4, 0xf) & 0xFF) != 1;
}
/* Address: 0x8025A254 | Size: 0x3c | Ghidra import */
/* Address: 0x8025A254 | Size: 0x3c | Ghidra import */
u32 fn_8025A254(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F6E44();
  u32 uVar1;
  u32 uVar2;
  
  uVar1 = fn_801F025C(2);
  uVar2 = fn_801F6E44(uVar1,0x4c);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}
/* Address: 0x8025A340 | Size: 0xB4 */
void fn_8025A340(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A3F4 | Size: 0xB4 */
void fn_8025A3F4(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A4A8 | Size: 0xB4 */
void fn_8025A4A8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A55C | Size: 0xB0 */
void fn_8025A55C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025A60C | Size: 0xB0 */
void fn_8025A60C(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025A6BC | Size: 0xB4 */
void fn_8025A6BC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A770 | Size: 0x100 (256 bytes) */
void fn_8025A770(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80237774();
    extern void fn_8023793C();
    extern void fn_80237F74();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r25 = r3;
    r29 = r4;
    r26 = r5;
    r27 = r6;
    fn_80237774();
    r30 = r3;
    r3 = r25;
    r4 = r27;
    fn_80237774();
    r31 = r3;
    r3 = r25;
    r4 = r26;
    r5 = r29;
    fn_802395C8();
    r7 = (0x1 << 16);
    r28 = r3;
    r3 = r25;
    r4 = r29;
    r5 = r26;
    r6 = r27;
    fn_8025C264();
    r0 = r3;
    r3 = r25;
    r29 = r0;
    r4 = r26;
    fn_80239500();
    r6 = r3;
    r3 = r25;
    r4 = r27;
    r5 = r28;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r29 = 0x0;
    }
    r3 = r25;
    r4 = r27;
    r5 = 0x5;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r29 = 0x0;
    }
    r3 = r30 & 0xFF;
    r0 = r31 & 0xFF;
    if (r3 < r0) {
        r29 = 0x0;
    }
    if ((s32)r29 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025A870 | Size: 0x8C */
void fn_8025A870(void* ctx, u32 param1, u32 param2) {
    extern f32 lbl_8047E648;
    extern void fn_80236BFC();
    extern void fn_802373B0();
    extern void fn_8025CC90();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;

    r5 = 0x8;
    r30 = r3;
    r31 = r4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = r31;
    fn_8025CC90();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    f1 = lbl_8047E648;
    r3 = r30;
    r4 = r31;
    r5 = 0x0;
    fn_802373B0();
    r0 = r3 & 0xFF;
    r3 = 0x1 - r0;
    r3 = r3 - r0; /* -borrow */;

    return;
}

/* Address: 0x8025A8FC | Size: 0xB4 */
void fn_8025A8FC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025A9B0 | Size: 0x3c | Ghidra import */
u32 fn_8025A9B0(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F6E44();
  u32 uVar1;
  u32 uVar2;
  
  uVar1 = fn_801F025C(2);
  uVar2 = fn_801F6E44(uVar1,0x49);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}
/* Address: 0x8025A9EC | Size: 0xB4 */
void fn_8025A9EC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025AAA0 | Size: 0x270 (624 bytes) */
u8 fn_8025AAA0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u8 fn_80237310(void*, u32);
    extern s32 fn_8025C264(void*, u32, u32, u32, u32);
    extern u32 fn_801F025C(u32, u32);
    extern u8 fn_801F6E98(u32, u32);
    s32 r;
    u8 flag;

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x4) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x8) == 1) {
        return 0;
    }

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        flag = 0;
    } else {
        if (fn_80237F74(ctx, param3, 0x14) != 1) {
            if (fn_80237F74(ctx, param3, 0x7) != 1) {
                if (fn_80237F74(ctx, param3, 0xf) != 1) {
                    if (fn_80237F74(ctx, param3, 0x48) != 1) {
                        if (fn_80237F74(ctx, param3, 0x29) != 1) {
                            if (fn_80237F74(ctx, param3, 0x28) != 1) {
                                fn_80237F74(ctx, param3, 0xc);
                            }
                        }
                    }
                }
            }
        }
        flag = 1;
    }
    if (flag == 0) {
        return 0;
    }

    r = fn_8025C264(ctx, param1, param2, param3, 0);
    if (fn_801F6E98(fn_801F025C(2, param3), 0x4b) == 1) {
        return 0;
    }
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        return 1;
    }
    return 1;
}

/* Address: 0x8025AD10 | Size: 0x38 | Ghidra import */
int fn_8025AD10(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_802373B0();
    extern f32 lbl_8047E648;

  u32 uVar1;
  int iVar2;
  
  uVar1 = fn_802373B0((double)lbl_8047E648,r3,r4,0);
  iVar2 = -(uVar1 & 0xff) + 1;
  return iVar2 - ((u32)(iVar2 == 0) + -(uVar1 & 0xff));
}
/* Address: 0x8025AD48 | Size: 0xB4 */
void fn_8025AD48(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025ADFC | Size: 0x2c | Ghidra import */
u32 fn_8025ADFC(void)

{
    extern int fn_8025C770();
  u8 cVar1;
  
  cVar1 = fn_8025C770();
  return cVar1 != 0;
}
/* Address: 0x8025AE28 | Size: 0xB0 */
void fn_8025AE28(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025AED8 | Size: 0xE0 (224 bytes) */
void fn_8025AED8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F4354();
    extern void fn_801F87CC();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8025C264();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r30 = r6;
    r28 = r4;
    r29 = r5;
    r3 = 0x0;
    r4 = r30;
    fn_801F4354();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r30;
    r5 = 0x15;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r30;
    r5 = 0x25;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r28;
    r5 = r29;
    r6 = r30;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r31;
    r31 = r0;
    r4 = (u32)sp + 0x8;
    fn_801F87CC();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025AFB8 | Size: 0xB0 */
void fn_8025AFB8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025B068 | Size: 0xB4 */
void fn_8025B068(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B124 | Size: 0xE8 (232 bytes) */
void fn_8025B124(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x7;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B20C | Size: 0xE8 (232 bytes) */
void fn_8025B20C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x6;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B2F4 | Size: 0xE8 (232 bytes) */
void fn_8025B2F4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x3;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B3DC | Size: 0xE8 (232 bytes) */
void fn_8025B3DC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x2;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B4C4 | Size: 0xE8 (232 bytes) */
void fn_8025B4C4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x90;
    r8 = 0x1;
    r9 = 0x1;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x90;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B5AC | Size: 0xB4 */
void fn_8025B5AC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B660 | Size: 0xE8 (232 bytes) */
void fn_8025B660(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x10;
    r8 = 0x7;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B748 | Size: 0xE8 (232 bytes) */
void fn_8025B748(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x10;
    r8 = 0x4;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B830 | Size: 0xE8 (232 bytes) */
void fn_8025B830(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x10;
    r8 = 0x2;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025B918 | Size: 0xE8 (232 bytes) */
void fn_8025B918(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8021C034();
    extern void fn_80236BFC();
    extern void fn_8025C264();
    extern void fn_8025C808();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = 0x0;
        fn_8025C264();
        r31 = r3;
    }
    r3 = r27;
    r4 = r28;
    r5 = r30;
    r6 = r29;
    r7 = 0x10;
    r8 = 0x1;
    r9 = 0x41;
    fn_8025C808();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = 0x10;
    fn_8021C034();
    r0 = (s8)r3;
    if (r0 < (u32)0x1) {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == (s32)-0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BA00 | Size: 0x2c | Ghidra import */
u32 fn_8025BA00(void)

{
    extern u16 fn_8025CB3C();
  u16 sVar1;
  
  sVar1 = fn_8025CB3C();
  return sVar1 != 0;
}
/* Address: 0x8025BA2C | Size: 0xF4 (244 bytes) */
void fn_8025BA2C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_80236BFC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r29;
    r5 = 0x14;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r29;
    r5 = 0x8;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r27;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r31 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r3 = 0x0;
        return;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BB20 | Size: 0x108 (264 bytes) */
void fn_8025BB20(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F1A6C();
    extern void fn_8022967C();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    extern void fn_8025CBE8();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
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
    r29 = r6;
    r5 = r27;
    r4 = r28;
    fn_802395C8();
    r30 = r3;
    r4 = r26;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fn_801F1A6C();
    r31 = r3;
    r3 = r28;
    fn_8022967C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r31 & 0xFFFF;
        if (r0 <= (u32)0x1) {
            r3 = 0x0;
            return;
    }
    }
    r3 = r26;
    fn_8025CBE8();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r26;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r26;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r3 = 0x0;
        return;
    }
    r3 = r26;
    r4 = r27;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    if ((s32)r3 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BC28 | Size: 0xB4 */
void fn_8025BC28(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BCDC | Size: 0xB4 */
void fn_8025BCDC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BD90 | Size: 0xB4 */
void fn_8025BD90(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BE44 | Size: 0xB4 */
void fn_8025BE44(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BEF8 | Size: 0xB4 */
void fn_8025BEF8(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r30 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r30;
    r4 = r28;
    fn_802395C8();
    r31 = r3;
    r3 = r27;
    r4 = r30;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r30 = r3;
    r3 = r27;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r31;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r30 = 0x0;
    }
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    if ((s32)r30 == (s32)-0x1) {
        r3 = 0x1;
        return;
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025BFAC | Size: 0x200 (512 bytes) */
u32 fn_8025BFAC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern u8 fn_8025CC90(void* ctx, u32 a);
    extern u8 fn_80237310(void* ctx, u32 a);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    extern s32 fn_8025C264(void* ctx, u32 p1, u32 p2, u32 p3, u32 z);
    extern u32 fn_801F025C(s32 a, u32 b);
    extern u8 fn_801F6E98(u32 a, u32 b);
    s32 result;
    u8 ok;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) return 0;
    if (fn_80236BFC(ctx, param3, 0x8) == 1) return 0;
    if (fn_8025CC90(ctx, param3) == 1) return 0;
    if (fn_80237310(ctx, param3) == 0) return 0;
    if (fn_80237F74(ctx, param3, 0x11) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x14) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x7) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0xf) == 1 || fn_80237F74(ctx, param3, 0x48) == 1) {
        ok = 0;
        goto check;
    }
    if (fn_80237F74(ctx, param3, 0x29) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x28) == 1) goto ok1;
    fn_80237F74(ctx, param3, 0xc);
ok1:
    ok = 1;
check:;
    if (ok == 0) return 0;
    result = fn_8025C264(ctx, param1, param2, param3, 0);
    if (fn_801F6E98(fn_801F025C(2, param3), 0x4b) == 1) return 0;
    if (result == 0) return 0;
    if (result == -1) return 1;
    return 1;
}

/* -------------------------------------------------------------------
 * Shadow Pokemon & Purification (0x8025C000-0x80260000)
 * 89 functions
 * ------------------------------------------------------------------- */

/* Address: 0x8025C1AC | Size: 0xB0 */
void fn_8025C1AC(void* ctx, u32 param1, u32 param2) {
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_8025C264();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r31 = r4;
    r28 = r5;
    r27 = r3;
    r29 = r6;
    r5 = r31;
    r4 = r28;
    fn_802395C8();
    r0 = r3;
    r3 = r27;
    r30 = r0;
    r4 = r31;
    r5 = r28;
    r6 = r29;
    r7 = 0x0;
    fn_8025C264();
    r0 = r3;
    r3 = r27;
    r31 = r0;
    r4 = r28;
    fn_80239500();
    r6 = r3;
    r3 = r27;
    r4 = r29;
    r5 = r30;
    fn_8023793C();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x43) {
        r31 = 0x0;
    }
    if ((s32)r31 == (s32)0x0) {
        r3 = 0x0;
    } else {

        r3 = 0x1;
    }
    return;
}

/* Address: 0x8025C264 | Size: 0x340 (832 bytes) */
void fn_8025C264(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BEB4();
    extern void fn_801F0134();
    extern void fn_801F1D5C();
    extern void fn_801F3BB4();
    extern void fn_801F54A4();
    extern void fn_80201D84();
    extern void fn_80229934();
    extern void fn_80229B70();
    extern void fn_80229BD8();
    extern void fn_80235B04();
    extern void fn_80236BFC();
    extern void fn_80237288();
    extern void fn_8025CAA8();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
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

    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fn_801F54A4();
    r4 = r3 & 0xFFFF;
    r3 = r26;
    fn_801F0134();
    r31 = r3;
    r3 = r25;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r4 = r27;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    fn_8011BEB4();
    r30 = r3 & 0xFFFF;
    if (r28 == (u32)0x0) {
        r3 = 0x1;
        return;
    }
    r7 = (u32)sp + 0x8;
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    fn_801F1D5C();
    r24 = r3;
    r4 = (u32)sp + 0x8;
    r3 = 0x0;
    r5 = 0x8;
    r6 = 0x0;
    fn_801F3BB4();
    r4 = (u32)sp + 0x8;
    r0 = r24 & 0xFFFF;
    r6 = 0x0;
    r7 = 0x0;
    r5 = 0x0;
    while (1) {
        r3 = r5 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u32*)(r4 + r3);
        if (r3 != (u32)0x0) {
            if (r26 == (u32)r3) {
                r6 = r5;
            }
            if (r28 == (u32)r3) {
                r7 = r5;
        }
        }
        r5 = r5 + 0x1;

    }
    r3 = r6 & 0xFFFF;
    r0 = r7 & 0xFFFF;
    r0 = r3 - r0;
    r24 = (u32)r0 >> 31;

    if (r29 == (u32)0xffff || r29 == (u32)0xfffe) {

        if (r29 == (u32)0xffff) {
            r3 = r25;
            r4 = r28;
            r5 = 0x1d;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r28;
                r4 = 0x1d;
                fn_80201D84();
                r3 = r3 & 0xFFFF;
                r0 = r31 & 0xFFFF;
                if (r0 == (u32)r3) {
                    r3 = -0x1;
                    return;
        }
        }
        }
        r3 = r25;
        r4 = r26;
        fn_80237288();
        r0 = r3 & 0xFF;
        if ((r0 == (u32)0x1) && (r24 == (u32)0x1)) {

            r3 = 0x0;
            return;
        }
        r3 = r25;
        r4 = r28;
        r5 = r27;
        fn_8025CAA8();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = 0x1;
        return;
    }
    r3 = r27;
    r4 = r26;
    r5 = r28;
    fn_80229934();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r25;
    r4 = r28;
    r5 = r27;
    fn_8025CAA8();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r25;
    r4 = r28;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = 0x1d;
        fn_80201D84();
        r3 = r3 & 0xFFFF;
        r0 = r31 & 0xFFFF;
        if (r3 == (u32)r0) {
            r3 = -0x1;
            return;
    }
    }
    if (r24 == (u32)0x1 && r0 == (u32)0x1 && r0 != (u32)0x39 && r0 != (u32)0xfa) {
        r3 = r25;
        r4 = r28;
        r5 = 0x1f;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            if (r30 != (u32)0x92) {
                if (r30 != (u32)0x95) {
                    if (r30 != (u32)0x98) {
                        if (r30 != (u32)0xcf) {
                            r3 = 0x0;
                            return;
        }
        }
        }
        }
        }
        r3 = r25;
        r4 = r28;
        r5 = 0x20;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if ((r0 == (u32)0x1) && (r30 != (u32)0x93)) {

            r3 = 0x0;
            return;
        }
        r3 = r25;
        r4 = r28;
        r5 = 0x21;
        fn_80236BFC();
        r0 = r3 & 0xFF;

        r0 = r27 & 0xFFFF;

        r3 = 0x0;
        return;
    }
    r3 = r27;
    fn_80229B70();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = -0x1;
        return;
    }
    r3 = r27;
    fn_80229BD8();
    r0 = r3 & 0xFF;
    r3 = 0x1;
    if (r0 != (u32)0x1) return;
    r3 = -0x1;

    return;
}

/* Address: 0x8025C5A4 | Size: 0xD0 (208 bytes) */
s32 fn_8025C5A4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_8011BEB4();
    u32 val = fn_8011BEB4(0, param2, 9, 0) & 0xFFFF;

    if ((u16)param1 == 0x1F &&
        (val == 0x92 || val == 0x95 || val == 0x98 || val == 0xCF)) {
        return 1;
    }
    if ((u16)param1 == 0x20 && val == 0x93) {
        return 1;
    }
    if ((u16)param1 == 0x21 && ((u16)param2 == 0x39 || (u16)param2 == 0xFA)) {
        return 1;
    }
    if (val == 0x5E) {
        return 1;
    }
    return 0;
}

/* Address: 0x8025C674 | Size: 0x48 | Pattern: field_accessor */
u32 fn_8025C674(void* ctx, u32 slot, u32 param) {
    extern u32 fn_801F37B0();
    extern void _fightTrainerAiCheckHorobinoutaSub();
    u32 result[2];
    result[0] = (u32)ctx;
    result[1] = 0;
    fn_801F37B0(0, (u32)_fightTrainerAiCheckHorobinoutaSub, (u32)result, 0);
    return result[1] & 0xFFFF;
}

/* Address: 0x8025C6BC | Size: 0xB4 */
void fn_8025C6BC(void* ctx, u32 param1, u32 param2) {
    extern void fn_801F8424();
    extern void fn_802062FC();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r29 = r5;
    r28 = r4;
    r31 = r3;
    r30 = *(u32*)((u8*)r5 + 0x0);
    fn_802062FC();
    r0 = r3 & 0xFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x1;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = r28;
    fn_801F8424();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x1;
        return;
    }
    r3 = r30;
    r4 = r31;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x2b;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = *(u32*)((u8*)r29 + 0x4);
            r0 = r3 + 0x1;
            *(u32*)((u8*)r29 + 0x4) = r0;
    }
    }
    r3 = 0x1;

    return;
}

/* Address: 0x8025C770 | Size: 0x98 */
void fn_8025C770(void* ctx, u32 param1, u32 param2) {
    extern void fn_801FB1C0();
    extern void fn_8021B364();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;

    r5 = 0x43;
    r6 = 0x0;
    r31 = r4;
    r4 = 0x0;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2a;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = (u32)sp + 0x8;
        fn_8021B364();
        r3 = r3 & 0xFF;
        r0 = 0x1;
        r0 = r3 - r0;
        r0 = -0x1;
        r3 = r0 & 0xFF;
    } else {

        r3 = 0x1;
    }
    r31 = *(u32*)(sp + 0x1C);
    return;
}

/* Address: 0x8025C808 | Size: 0x2A0 (672 bytes) */
void fn_8025C808(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F025C();
    extern void fn_801F6E98();
    extern void fn_801FB1C0();
    extern void fn_8021C034();
    extern void fn_8021C090();
    extern void fn_80229C28();
    extern void fn_80237F74();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
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
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r0 = r9 & 0x00000040;
    r21 = r9;
    r22 = r3;
    r23 = r5;
    r24 = r6;
    r25 = r7;
    r26 = r8;
    r28 = 0x0;
    r27 = 0x0;
    if ((s32)r0 != (s32)0) {
        r30 = r4;
    } else {

        r30 = r23;
    }
    r4 = r30;
    r3 = 0x2;
    fn_801F025C();
    r31 = r21 & 0xFF;
    r21 = r3;
    r0 = r31 & 0xbf;
    r0 = r0 & 0x00000080;
    if ((s32)r0 != (s32)0) {
        r28 = 0x1;
    }
    r0 = r31 & 0x00000020;
    if ((s32)r0 != (s32)0) {
        r27 = 0x1;
    }
    r3 = r26;
    fn_8021C090();
    r0 = r3;
    r3 = r30;
    r29 = r0;
    r4 = 0x0;
    r5 = r29;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r26 = (s8)r3;
    r3 = r25;
    fn_8021C034();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0) {
        r3 = r21;
        r4 = 0x4c;
        fn_801F6E98();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r28 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = r24 & 0xFFFF;
                if (r0 != (u32)0xae) {
                    r3 = 0x0;
                    return;
        }
        }
        }
        r0 = r24 & 0xFFFF;
        if (r0 == (u32)0xae || r0 == (u32)0x1 || r0 == (u32)0x1) {
            r0 = r27 & 0xFF;

            r3 = r22;
            r4 = 0x0;
            r5 = 0x43;
            r6 = 0x0;
            fn_801FB1C0();
            r4 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x2;
            r6 = 0x0;
            fn_801FB1C0();
            r4 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x24;
            r6 = 0x0;
            fn_801FB1C0();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r23;
                r4 = r24;
                fn_80229C28();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r0 = 0x1;
                    goto L_8025C96C;
            }
            }
            r0 = 0x0;
        L_8025C96C:
            r0 = r0 & 0xFF;

            r3 = 0x0;
            return;
        }
        r3 = r22;
        r4 = r30;
        r5 = 0x1d;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r22;
            r4 = r30;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1 || r0 != (u32)0x1 || r0 != (u32)0xae) {
            }
            r0 = r28 & 0xFF;

            r0 = r24 & 0xFFFF;

            r3 = 0x0;
            return;
            }
        r3 = r22;
        r4 = r30;
        r5 = 0x33;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r28 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = r29 & 0xFFFF;
                if (r0 == (u32)0xeb) {
                    r3 = 0x0;
                    return;
        }
        }
        }
        r3 = r22;
        r4 = r30;
        r5 = 0x34;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r28 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = r29 & 0xFFFF;
                if (r0 == (u32)0xe6) {
                    r3 = 0x0;
                    return;
        }
        }
        }
        r3 = r22;
        r4 = r30;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r31 & 0x1F;
            if (r0 == (u32)0x1) {
                r3 = 0x0;
                return;
        }
        }
        r0 = (s8)r26;
        if (r0 > (u32)0x1) { r3 = 0x1; return; }
        r3 = 0x0;
        return;
    }
    if ((s32)r26 < (s32)0xc) { r3 = 0x1; return; }
    r3 = 0x0;
    return;

    r3 = 0x1;

    return;
}

/* Address: 0x8025CAA8 | Size: 0x94 */
void fn_8025CAA8(void* ctx, u32 param1, u32 param2) {
    extern void fn_801FB1C0();
    extern void fn_80229C28();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    r6 = 0x0;
    r30 = r4;
    r31 = r5;
    r4 = 0x0;
    r5 = 0x43;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x24;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        fn_80229C28();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x1;
            return;
    }
    }
    r3 = 0x0;

    return;
}

/* Address: 0x8025CB3C | Size: 0xAC */
u16 fn_8025CB3C(void* ctx, u32 param1, u32 param2) {
    extern u32 fn_800E0C54(void);
    extern u32 fn_8012640C(u32, u32, u32, u32);
    extern u8 fn_80201248(u32, u16*);
    u16 choices[4];
    u32 species;
    s32 random;
    s32 index;
    u8 count;

    species = fn_8012640C(param1, 0, 0xF7, 0) & 0xFFFF;
    if ((species != 0) && (species != 0x165) && (species != 0xFFFF)) {
        return species;
    }
    count = fn_80201248(param1, choices);
    if (count != 0) {
        random = fn_800E0C54() & 0xFFFF;
        index = random % (s32)(u8)count;
        species = choices[(u8)index];
        if ((species != 0) && (species != 0x165)) {
            return species;
        }
    }
    return 0;
}

/* Address: 0x8025CBE8 | Size: 0x48 | Pattern: field_accessor */
u32 fn_8025CBE8(void* ctx, u32 slot, u32 param) {
    extern u32 fn_801F37B0();
    extern void fn_8025CC30();
    u32 buf[2];
    u32 r;
    u32 s;
    buf[0] = (u32)ctx;
    r = fn_801F37B0(0, (u32)fn_8025CC30, (u32)buf, 0) & 0xFF;
    s = 1 - r;
    return (s != 0) ? 1 : 0;
}

/* Address: 0x8025CC30 | Size: 0x60 | Pattern: field_accessor */
u32 fn_8025CC30(void* ctx, u32 slot, u32* param) {
    extern u32 fn_802062FC(void);
    extern u32 fn_80237F74(u32, void*, u32);
    u32 r31;
    u32 r30;
    u32 result;

    r30 = (u32)ctx;
    r31 = *param;
    if ((fn_802062FC() & 0xFF) == 0) {
        return 1;
    }
    result = fn_80237F74(r31, (void*)r30, 6) & 0xFF;
    return result != 1;
}

/* Address: 0x8025CC90 | Size: 0x50 | Pattern: field_accessor */
u32 fn_8025CC90(void* ctx, u32 slot, u32 param) {
    extern u32 fn_801F37B0();
    extern u32 fn_8025CCE0();
    u32 buf[2];
    u32 r;
    buf[0] = (u32)ctx;
    buf[1] = slot;
    r = fn_801F37B0(0, (u32)fn_8025CCE0, (u32)buf, 0) & 0xFF;
    return r != 1;
}

/* Address: 0x8025CCE0 | Size: 0x84 | Pattern: field_accessor */
u32 fn_8025CCE0(void* ctx, u32 slot, u32 param) {
    extern u32 fn_802062FC(void);
    extern u32 fn_80236BFC(u32, void*, u32);
    extern u32 fn_80237F74(u32, u32, u32);
    u32* args;
    u32 a;
    u32 b;

    args = (u32*)param;
    a = args[0];
    b = args[1];
    if ((fn_802062FC() & 0xFF) == 0) {
        return 1;
    }
    if (((fn_80236BFC(a, ctx, 0xB) & 0xFF) == 1) &&
        ((fn_80237F74(a, b, 0x2B) & 0xFF) == 0)) {
        return 0;
    }
    return 1;
}

/* Address: 0x8025CD64 | Size: 0x54 | Pattern: field_accessor */
void fn_8025CD64(void* ctx, u32 slot, u32 param) {
    extern u32 lbl_8047B650;
    extern u32 fn_800E202C();
    extern void fn_800E209C();
    extern void fn_800E24B0();
    u32 handle;
    u32 result;
    handle = lbl_8047B650;
    if (handle != 0) {
        result = fn_800E202C(handle);
        if ((result & 0xFFFF) != 0) {
            fn_800E24B0(result);
            fn_800E209C(result);
        }
        lbl_8047B650 = 0;
    }
}

/* Address: 0x8025CDB8 | Size: 0x2B4 (692 bytes) */
void fn_8025CDB8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478D98;
    extern u32 lbl_8047B650;
    extern u32 lbl_8047B654;
    extern void fn_8006B09C();
    extern void fn_800E202C();
    extern void fn_800E209C();
    extern void fn_800E24B0();
    extern void fn_800E27B0();
    extern void fn_800E2C04();
    extern void fn_800FA280();
    extern void fn_8012086C();
    extern void fn_80123EF0();
    extern void fn_801240C4();
    extern void fn_80124A60();
    extern void fn_80129280();
    extern void fn_8012A248();
    extern void fn_8012AC08();
    extern void fn_80135938();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = 0x0;
    r4 = 0x20;
    r0 = lbl_80478D98;
    lbl_8047B654 = r3;
    r3 = r0 * 0x138;
    r0 = r3 + 0x1f;
    fn_800E2C04();
    r0 = r3 & 0xFFFF;
    if ((s32)r0 != (s32)0) {
        fn_800E27B0();
    } else {

        r3 = 0x0;
    }
    lbl_8047B654 = r3;
    r30 = r3;
    r31 = 0x0;
    while (1) {
        r0 = lbl_80478D98;
        if ((s32)r31 >= (s32)r0) break;
        r3 = 0x0;
        r4 = 0x1;
        fn_80135938();
        r0 = r31 + 0x1;
        r6 = r3;
        r3 = r30;
        r5 = 0xa;
        r4 = r0 & 0xFFFF;
        fn_801240C4();
        r3 = r31 + 0x1004;
        fn_800FA280();
        r9 = r3;
        r3 = r30;
        r4 = 0x0;
        r5 = 0x8;
        r6 = 0x1;
        r7 = 0x0;
        r8 = 0x0;
        fn_80123EF0();
        r3 = r30;
        fn_8012086C();
        r30 = r30 + 0x138;
        r31 = r31 + 0x1;

    }
    r3 = lbl_8047B650;
    if (r3 != (u32)0x0) {
        fn_800E202C();
        r0 = r3 & 0xFFFF;
        r30 = r3;
        if (r3 != (u32)0x0) {
            fn_800E24B0();
            r3 = r30;
            fn_800E209C();
        }
        r0 = 0x0;
        lbl_8047B650 = r0;
    }
    r3 = 0x80;
    r4 = 0x20;
    fn_800E2C04();
    r0 = r3 & 0xFFFF;
    if (r3 != (u32)0x0) {
        fn_800E27B0();
    } else {

        r3 = 0x0;
    }
    lbl_8047B650 = r3;
    r31 = 0x0;
    do {
        r3 = r31;
        fn_8006B09C();
        r3 = r3 + 0xb44;
        fn_8012A248();
        r30 = 0x0;
        do {
            r3 = r31;
            fn_8006B09C();
            r4 = r30 & 0xFFFF;
            r3 = r3 + 0xb44;
            fn_8012AC08();
            fn_80124A60();
            r30 = r30 + 0x1;
        } while ((s32)r30 < (s32)0x6);
        r31 = r31 + 0x1;
    } while ((s32)r31 < (s32)0x4);
    r30 = 0x0;
    do {
        r3 = r30;
        fn_8006B09C();
        r3 = r3 + 0x2c;
        fn_8012A248();
        r31 = 0x0;
        do {
            r3 = r30;
            fn_8006B09C();
            r4 = r31 & 0xFFFF;
            r3 = r3 + 0x2c;
            fn_8012AC08();
            fn_80124A60();
            r31 = r31 + 0x1;
        } while ((s32)r31 < (s32)0x6);
        r30 = r30 + 0x1;
    } while ((s32)r30 < (s32)0x4);
    r0 = 0x6;
    ctr_fn = (void(*)(void))r0;
    do {
    } while (--ctr != 0);
    r3 = 0x0;
    r4 = 0x2;
    fn_80129280();
    r30 = r3;
    r3 = 0x0;
    fn_8006B09C();
    r3 = r3 + 0xb44;
    if (r30 != (u32)0x0) {
        r4 = r30;
        r5 = 0xb18;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r29 = 0x0;
    r31 = 0x0;
    do {
        if ((s32)r31 == (s32)0x0) {
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            r30 = r3;
            r3 = r31;
            fn_8006B09C();
            r3 = r3 + 0xb44;
            if (r30 != (u32)0x0) {
                r4 = r30;
                r5 = 0xb18;
                memcpy((void*)r3, (const void*)r4, (u32)r5);
            }

        } else {
            r28 = 0x0;
            do {
                r0 = lbl_8047B654;
                r3 = r31;
                r30 = r0 + r29;
                fn_8006B09C();
                r4 = r28 & 0xFFFF;
                r3 = r3 + 0xb44;
                fn_8012AC08();
                if (r3 != (u32)0x0) {
                    r4 = r30;
                    r5 = 0x138;
                    memcpy((void*)r3, (const void*)r4, (u32)r5);
                }
                r28 = r28 + 0x1;
                r29 = r29 + 0x138;
            } while ((s32)r28 < (s32)0x6);
        }
        r31 = r31 + 0x1;
    } while ((s32)r31 < (s32)0x4);
    r3 = lbl_8047B654;
    if (r3 != (u32)0x0) {
        fn_800E202C();
        r0 = r3 & 0xFFFF;
        r30 = r3;
        if (r3 != (u32)0x0) {
            fn_800E24B0();
            r3 = r30;
            fn_800E209C();
        }
        r0 = 0x0;
        lbl_8047B654 = r0;
    }
    return;
}

/* Address: 0x8025D06C | Size: 0x3c | Ghidra import */
u32 fn_8025D06C(void)
{
    extern u32 fn_8006ADEC();
    extern void fn_8006AFC4();
    extern void* fn_8006B5A8();
    extern void fn_801293FC();
    u32 uVar1;
  fn_8006B5A8();
  fn_8006AFC4();
  uVar1 = fn_8006ADEC();
  fn_801293FC(0,uVar1);
  return 0;
}


/* Address: 0x8025D0A8 | Size: 0xBC */
void fn_8025D0A8(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478EAC;
    extern f32 lbl_8047E658;
    extern f32 lbl_8047E65C;
    extern void fn_8011F5C8();
    extern void fn_80123FBC();
    extern void fn_80129280();
    extern void fn_8012AC08();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = 0x0;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        r4 = 0x2;
        fn_80129280();
        r28 = r3;
    }
    r31 = 0x0;
    do {
        r3 = r28;
        r4 = r31 & 0xFFFF;
        fn_8012AC08();
        r29 = r3;
        fn_80123FBC();
        r0 = r3 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r29;
            fn_8011F5C8();
            r4 = lbl_80478EAC;
            r0 = r3 & 0xFFFF;
            r3 = 0x0;
            do {
                r5 = *(u16*)(r4 + r3);
                if (r5 == (u32)0x0) break;
                if (r0 == (u32)r5) {
                    r30 = r30 + 0x1;
                }
                r3 = r3 + 0x2;
            } while (1);
        }
        r31 = r31 + 0x1;
    } while ((s32)r31 < (s32)0x6);
    f1 = lbl_8047E658;
    f0 = lbl_8047E65C;
    ctr_fn = (void(*)(void))r30;
    if ((s32)r30 > (s32)0x0) {
        do {
            f1 = f1 * f0;
        } while (--ctr != 0);
    }
    return;
}

/* Address: 0x8025D164 | Size: 0x128 (296 bytes) */
void fn_8025D164(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8039A648[];
    extern u8 lbl_8039A664[];
    extern u32 lbl_80478EAC;
    extern f32 lbl_8047E658;
    extern f32 lbl_8047E65C;
    extern void fn_8006B09C();
    extern void fn_8006B5A8();
    extern void fn_8011F5C8();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    u32 r4 = param1;
    u32 r5 = param2;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r28 = 0x0;
    fn_8006B5A8();
    r27 = *(u32*)((u8*)r3 + 0xC);
    fn_8006B5A8();
    r26 = *(u32*)((u8*)r3 + 0x0);
    fn_8006B5A8();
    r30 = *(u32*)((u8*)r3 + 0x14);
    r29 = 0x0;
    do {
        r3 = 0x0;
        fn_8006B09C();
        r4 = r29 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        r31 = r3;
        fn_80123FBC();
        r0 = r3 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r31;
            fn_8011F5C8();
            r4 = lbl_80478EAC;
            r0 = r3 & 0xFFFF;
            r3 = 0x0;
            do {
                r5 = *(u16*)(r4 + r3);
                if (r5 == (u32)0x0) break;
                if (r0 == (u32)r5) {
                    r28 = r28 + 0x1;
                }
                r3 = r3 + 0x2;
            } while (1);
        }
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x6);
    f1 = lbl_8047E658;
    f0 = lbl_8047E65C;
    ctr_fn = (void(*)(void))r28;
    if ((s32)r28 > (s32)0x0) {
        do {
            f1 = f1 * f0;
        } while (--ctr != 0);
    }
    if ((s32)r26 == (s32)0x1) {
        r3 = r30 + 0x1;
        r0 = 0xa;
        r0 = (s32)r3 / (s32)r0;
        if ((s32)r0 > (s32)0xa) {
            r0 = 0xa;
        }
        r3 = (u32)lbl_8039A664;
        r0 = r0 << 2;
        r3 = (u32)lbl_8039A664;
        f0 = *(f32*)(void*)(r3 + r0);
        f1 = f1 * f0;
    } else {

        if ((s32)r27 >= (s32)0x6) {
            r27 = 0x6;
        }
        r3 = (u32)lbl_8039A648;
        r0 = r27 << 2;
        r3 = (u32)lbl_8039A648;
        f0 = *(f32*)(void*)(r3 + r0);
        f1 = f1 * f0;
    }
    f0 = (f64)(s32)f1;
    *(f64*)(void*)(sp + 0x8) = f0;
    r3 = *(u32*)(sp + 0xC);
    return;
}

/* Address: 0x8025D28C | Size: 0x24 | Pattern: null_check_getter */
extern void* fn_8006B09C(void*);
u16 fn_8025D28C(void* ctx) { return *(u16*)fn_8006B09C(ctx); }

/* Address: 0x8025D2B0 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025D2B0(void* ctx) { return *(u32*)((u8*)fn_8006B09C(ctx) + 0x24); }

/* Address: 0x8025D2D4 | Size: 0x90 */
u32 fn_8025D2D4(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478E04;
    extern void* fn_8006B09C(void*);
    extern u32 fn_801FCBA4(void);
    extern void fn_801FCCC4(u32);
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    id = *(u16*)fn_8006B09C(ctx);
    if (id == 0) {
        return 0;
    }
    fn_801FCCC4(id);
    offset = fn_801FCBA4();
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        ret = entry[3];
        if (ret == 0) {
            return 0xf941200;
        }
        return ret;
    }
    ret = entry[4];
    if (ret == 0) {
        return 0xf8f1200;
    }
    return ret;
}

/* Address: 0x8025D364 | Size: 0x90 */
u32 fn_8025D364(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478E04;
    extern void* fn_8006B09C(void*);
    extern u32 fn_801FCBA4(void);
    extern void fn_801FCCC4(u32);
    u16 id16;
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    if ((s32)ctx != 0) {
        id16 = *(u16*)fn_8006B09C(ctx);
    } else {
        id16 = *(u16*)fn_8006B09C(ctx);
    }
    id = id16;
    if (id == 0) {
        return 0;
    }
    fn_801FCCC4(id);
    offset = fn_801FCBA4();
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        return entry[1];
    }
    ret = entry[2];
    if (ret == 0) {
        return 0xf991200;
    }
    return ret;
}

/* Address: 0x8025D3F4 | Size: 0x16C (364 bytes) */
void fn_8025D3F4(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8006B09C();
    extern void fn_8006B1D4();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r27 = r3;
    fn_8006B1D4();
    r30 = 0x0;
    r31 = r27;
    r29 = r30;
    r28 = r3 & 0xFFFF;
    do {
        r3 = r27;
        fn_8006B09C();
        r4 = r29 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        fn_80123FBC();
        r3 = r3 & 0xFF;
        r0 = r3 - r0; /* -borrow */;
        r0 = r0 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r30 & 0xFFFF;
            r0 = r3 + 0x1;
            r30 = r0 & 0xFFFF;
        }
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x6);
    r0 = r30 & 0xFFFF;
    if (r0 < r28) {
        r28 = r30;
    }
    r3 = r31;
    r30 = r28 & 0xFFFF;
    fn_8006B09C();
    r28 = r3;
    r3 = r31;
    fn_8006B09C();
    r0 = 0x163;
    r5 = r28 + 0x28;
    r4 = r3 + 0xb40;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r27 = 0x0;
    r29 = 0x0;
    while ((s32)r27 < (s32)r30) {

        r3 = r31;
        fn_8006B09C();
        r0 = r29 + 0x8;
        r28 = *(u32*)(r3 + r0);
        r3 = r31;
        fn_8006B09C();
        r4 = r28 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        r28 = r3;
        r3 = r31;
        fn_8006B09C();
        r4 = r27 & 0xFFFF;
        r3 = r3 + 0x2c;
        fn_8012AC08();
        r3 = r31;
        fn_8006B09C();
        r4 = r27 & 0xFFFF;
        r3 = r3 + 0x2c;
        fn_8012AC08();
        if ((r3 != (u32)0x0) && (r28 != (u32)0x0)) {

            r0 = 0x27;
            ctr_fn = (void(*)(void))r0;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                r0 = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = r0;
            } while (--ctr != 0);
        }
        r27 = r27 + 0x1;
        r29 = r29 + 0x4;

    }
    return;
}

/* Address: 0x8025D560 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025D560(void* ctx) { return *(u32*)((u8*)fn_8006B09C(ctx) + 0x20); }

/* Address: 0x8025D584 | Size: 0x5C | Pattern: field_accessor */
void fn_8025D584(void* ctx, u32 slot, u32 param) {
    extern void fn_8006B09C();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r4 = slot;
    u32 r5 = param;

    fn_8006B09C();
    r4 = *(u32*)((u8*)r3 + 0x20);
    if ((s32)r0 < (s32)0) { r3 = 0x0; return; }
    if ((s32)r0 > (s32)0x6) {

        r3 = 0x0;
        return;
    }
    r0 = r0 << 2;
    r5 = -0x1;
    r4 = r3 + r0;
    *(u32*)((u8*)r4 + 0x8) = r5;
    r4 = *(u32*)((u8*)r3 + 0x20);
    *(u32*)((u8*)r3 + 0x20) = r0;
    r3 = *(u32*)((u8*)r3 + 0x20);

    return;
}

/* Address: 0x8025D5E0 | Size: 0x64 | Pattern: field_accessor */
u32 fn_8025D5E0(void* ctx, s32 count, u32* src) {
    typedef struct BattleFieldAccessor {
        u8 unk_00[8];
        u32 values[6];
        u32 count;
    } BattleFieldAccessor;
    extern BattleFieldAccessor* fn_8006B09C(void*);
    BattleFieldAccessor* dst;
    s32 i;

    dst = fn_8006B09C(ctx);
    for (i = 0; i < count; i++) {
        dst->values[i] = src[i];
    }
    dst->count = i;
    return i;
}

/* Address: 0x8025D644 | Size: 0x100 (256 bytes) */
void fn_8025D644(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8006B09C();
    extern void fn_8006B1D4();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r29 = r4;
    r25 = r3;
    fn_8006B09C();
    r31 = r3;
    r30 = *(u32*)((u8*)r3 + 0x20);
    fn_8006B1D4();
    r26 = 0x0;
    r28 = r3 & 0xFFFF;
    r27 = r26;
    do {
        r3 = r25;
        fn_8006B09C();
        r4 = r27 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        fn_80123FBC();
        r3 = r3 & 0xFF;
        r0 = r3 - r0; /* -borrow */;
        r0 = r0 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r26 & 0xFFFF;
            r0 = r3 + 0x1;
            r26 = r0 & 0xFFFF;
        }
        r27 = r27 + 0x1;
    } while ((s32)r27 < (s32)0x6);
    r0 = r26 & 0xFFFF;
    r3 = *(u32*)((u8*)r31 + 0x20);
    if (r0 < r28) {
        r28 = r26;
    }
    r0 = r28 & 0xFFFF;
    if ((s32)r3 >= (s32)r0) {
        r3 = -0x1;
        return;
    }
    r3 = 0x0;
    ctr_fn = (void(*)(void))r30;
    if ((s32)r30 > (s32)0x0) {
        do {
            r0 = r3 + 0x8;
            r0 = *(u32*)(r31 + r0);
            if ((s32)r0 == (s32)r29) {
                r3 = -0x1;
                return;
            }
            r3 = r3 + 0x4;
        } while (--ctr != 0);
    }
    r0 = r30 << 2;
    r3 = r30;
    r4 = r31 + r0;
    *(u32*)((u8*)r4 + 0x8) = r29;
    r4 = *(u32*)((u8*)r31 + 0x20);
    r0 = r4 + 0x1;
    *(u32*)((u8*)r31 + 0x20) = r0;

    return;
}

/* Address: 0x8025D744 | Size: 0x44 | Pattern: field_accessor */
u32 fn_8025D744(void* ctx, u32 slot, u32 param) {
    extern void* fn_8006B09C();
    u8* base;
    u32 i;
    base = (u8*)fn_8006B09C(ctx);
    *(u32*)(base + 0x20) = 0;
    for (i = 0; i < 6; i++) {
        *(u32*)(base + 0x8 + i * 4) = (u32)-1;
    }
    return (u32)base;
}

/* Address: 0x8025D788 | Size: 0x80 | Pattern: field_accessor */
void fn_8025D788(void* ctx, u32 slot, u32 param) {
    typedef struct {
        u32 words[0x2C6];
    } BattleCopyBlock;
    BattleCopyBlock* src;
    s32 i;
    BattleCopyBlock* dst;

    i = 0;
    do {
        src = (BattleCopyBlock*)((u8*)fn_8006B09C((void*)i) + 0xb44);
        dst = (BattleCopyBlock*)((u8*)fn_8006B09C((void*)i) + 0x2c);
        if ((src != NULL) && (dst != NULL)) {
            *dst = *src;
        }
        i++;
    } while ((s32)i < 4);
}

/* Address: 0x8025D808 | Size: 0x94 */
void fn_8025D808(void* ctx, u32 param1, u32 param2) {
    extern void fn_8006B09C();
    extern void fn_8006B1D4();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;

    r28 = r3;
    fn_8006B1D4();
    r30 = 0x0;
    r29 = r28;
    r31 = r30;
    r28 = r3 & 0xFFFF;
    do {
        r3 = r29;
        fn_8006B09C();
        r4 = r31 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        fn_80123FBC();
        r3 = r3 & 0xFF;
        r0 = r3 - r0; /* -borrow */;
        r0 = r0 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r30 & 0xFFFF;
            r0 = r3 + 0x1;
            r30 = r0 & 0xFFFF;
        }
        r31 = r31 + 0x1;
    } while ((s32)r31 < (s32)0x6);
    r0 = r30 & 0xFFFF;
    r3 = r28;
    if (r0 < r28) {
        r3 = r30;
    }
    return;
}

/* Address: 0x8025D89C | Size: 0x78 | Pattern: field_accessor */
void fn_8025D89C(void* ctx, u32 slot, u32 param) {
    extern void fn_8006B09C();
    extern void fn_80123FBC();
    extern void fn_8012AC08();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = slot;

    r30 = 0x0;
    r31 = r3;
    r29 = 0x0;
    do {
        r3 = r31;
        fn_8006B09C();
        r4 = r29 & 0xFFFF;
        r3 = r3 + 0xb44;
        fn_8012AC08();
        fn_80123FBC();
        r3 = r3 & 0xFF;
        r0 = r3 - r0; /* -borrow */;
        r0 = r0 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r30 & 0xFFFF;
            r0 = r3 + 0x1;
            r30 = r0 & 0xFFFF;
        }
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x6);
    r3 = r30;
    return;
}

/* Address: 0x8025D914 | Size: 0x24 | Pattern: null_check_getter */
void* fn_8025D914(void* ctx) { return (u8*)fn_8006B09C(ctx) + 0xb44; }

/* Address: 0x8025D938 | Size: 0x38 | Ghidra import */
u32 fn_8025D938(u32 r3, u16 r4)
{
    extern void* fn_8006B09C();
    extern void fn_8012AC08();
    int iVar1;
  iVar1 = (int)fn_8006B09C();
  fn_8012AC08(iVar1 + 0x2c, r4);
}


/* Address: 0x8025D970 | Size: 0x38 | Ghidra import */
u32 fn_8025D970(u32 r3, u16 r4)
{
    extern void* fn_8006B09C();
    extern void fn_8012AC08();
    int iVar1;
  iVar1 = (int)fn_8006B09C();
  fn_8012AC08(iVar1 + 0xb44, r4);
}


/* Address: 0x8025D9A8 | Size: 0x24 | Pattern: null_check_getter */
extern void* fn_8006B5A8(void*);
u32 fn_8025D9A8(void* ctx) { return *(u32*)fn_8006B5A8(ctx); }

/* Address: 0x8025D9CC | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025D9CC(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x10); }

/* Address: 0x8025D9F0 | Size: 0x28 | Ghidra import */
u32 fn_8025D9F0(void)
{
    extern u32 fn_8006A7E8();
    extern void* fn_8006B09C();
    u16 uVar1;
  fn_8006B09C();
  uVar1 = fn_8006A7E8();
  return uVar1;
}


/* Address: 0x8025DA18 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DA18(void* ctx) { if (ctx == NULL) return 0; return 0; /* stub */ }

/* fn_8025DA3C | Size: 0x4C | Get battle party size based on mode */
u32 fn_8025DA3C(void) {
    s32 mode;
    u32 res;
    extern void* fn_8006B5A8(void);
    void* result = fn_8006B5A8();
    res = 2;
    mode = *(s32*)((u8*)result + 0x4);
    switch (mode) {
        case 0:
        case 1:
            res = 2;
            break;
        case 2:
            res = 4;
            break;
    }
    return res;
}

/* Address: 0x8025DA88 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DA88(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x4); }

/* Address: 0x8025DAAC | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DAAC(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0xc); }

/* Address: 0x8025DAD0 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DAD0(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x8); }

/* Address: 0x8025DAF4 | Size: 0x38 | Ghidra import */
u32 fn_8025DAF4(void)
{
    extern void* fn_8006B5A8();
    u32 uVar1;
    uVar1 = (u32)fn_8006B5A8();
    if (*(u32 *)(uVar1 + 0x18) != 0) {
        *(u32 *)(uVar1 + 0x18) = *(u32 *)(uVar1 + 0x18) - 1;
    }
    return *(u32 *)(uVar1 + 0x18);
}


/* Address: 0x8025DB2C | Size: 0x30 | Ghidra import */
u32 fn_8025DB2C(void)
{
    extern void* fn_8006B5A8();
    int iVar1;
  iVar1 = (int)fn_8006B5A8();
  *(int *)(iVar1 + 0x18) = *(int *)(iVar1 + 0x18) + 1;
  return *(u32 *)(iVar1 + 0x18);
}


/* Address: 0x8025DB5C | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DB5C(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x18); }

/* Address: 0x8025DB80 | Size: 0x30 | Ghidra import */
u32 fn_8025DB80(void)
{
    extern void* fn_8006B5A8();
    int iVar1;
  iVar1 = (int)fn_8006B5A8();
  *(int *)(iVar1 + 0x14) = *(int *)(iVar1 + 0x14) + 1;
  return *(u32 *)(iVar1 + 0x14);
}


/* Address: 0x8025DBB0 | Size: 0x24 | Pattern: null_check_getter */
u32 fn_8025DBB0(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x14); }

/* Address: 0x8025DBD4 | Size: 0x58 | Pattern: field_accessor */
u32 tableResBiosGetResPtr(u32 idx) {
    extern u8 lbl_8027A450[];
    extern u8 lbl_8039A690[];
    extern u32 *lbl_80478E08;
    extern u32 *lbl_80478E0C;
    extern void fn_800DD970(const char* fmt, ...);

    u32 count;
    u32* table;
    count = *lbl_80478E08;
    if (idx >= count) {
        fn_800DD970((const char*)lbl_8027A450, lbl_8039A690);
        return 0;
    }
    table = lbl_80478E0C;
    return table[idx];
}

/* Address: 0x8025DCBC | Size: 0x58 | Ghidra import */
void fn_8025DCBC(int *param)
{
    u32 *r3 = (u32 *)param;
    if (*r3 != 0) {
        fn_80165A20(*r3, 0x32, 0xff);
    }
    if (r3[1] != 0) {
        fn_801659FC(r3[1], 0x32, 0xff);
    }
}

/* Address: 0x8025DD14 | Size: 0x98 | Ghidra import */
#pragma push
#pragma use_lmw_stmw off
#pragma optimize_for_size off
void fn_8025DD14(int *r3)
{
    extern int fn_801653BC();
    extern int fn_801653C4();
    extern int fn_801656D8();
    extern void fn_80165A20();
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;

    iVar1 = fn_801653C4();
    if (iVar1 != 0) {
        iVar2 = fn_801656D8();
        fn_80165A20(1, 0x32, 0xff);
    } else {
        iVar2 = 0;
    }
    iVar3 = fn_801653BC();
    if (iVar3 != 0) {
        iVar4 = fn_801656D8();
    } else {
        iVar4 = 0;
    }
    *r3 = iVar1;
    r3[1] = iVar3;
    r3[2] = iVar2;
    r3[3] = iVar4;
}
#pragma pop

/* Address: 0x8025DDAC | Size: 0x48 | Ghidra import */
void fn_8025DDAC(u32 *r3,u32 r4)
{
    extern u32 fn_800E4170();
    extern u32 fn_801DAC3C(u32);
  u32 iVar1;
  r3 = (u32*)*r3;
  if (r3 == 0) return;
  iVar1 = fn_801DAC3C((u32)r3);
  if (iVar1 == 0) return;
  fn_800E4170(iVar1,r4);
}

/* Address: 0x8025DDF4 | Size: 0x18 | Ghidra import */
void fn_8025DDF4(u32 *r3)

{
  if (*r3 == 0) {
    return;
  }
  *r3 = 0;
  return;
}

/* Address: 0x8025DE0C | Size: 0x48 | Ghidra import */
void fn_8025DE0C(u32 *r3,u32 r4)
{
    extern u32 fn_800E43A4();
    extern u32 fn_801DAC3C(u32);
  u32 iVar1;
  r3 = (u32*)*r3;
  if (r3 == 0) return;
  iVar1 = fn_801DAC3C((u32)r3);
  if (iVar1 == 0) return;
  fn_800E43A4(iVar1,r4);
}

/* Address: 0x8025DE54 | Size: 0xE4 | Ghidra import */
void fn_8025DE54(u32 *r3,u16 *r4,int r5,int r6,int r7,
                 int r8)

{
    extern int _threadSwitch();
    extern int fn_801C41C8();
    extern int fn_801DA4E8();
    extern int fn_801DA8C4();
    extern int fn_801DA914();
    extern u8 fn_801DA94C();
    extern int fn_801DA9E8();
    extern int fn_801DB088();
    extern f32 lbl_8047E670;

  u16 *p;
  int iVar2;

  if (r7 == 1) {
    fn_801C41C8((double)lbl_8047E670,2);
  }
  fn_801DA4E8(*r3,1);
  p = r4;
  for (iVar2 = 0; iVar2 < r5; iVar2 = iVar2 + 1) {
    if (r6 == 0) {
      fn_801DA914(*r3,*p,p[1]);
    }
    fn_801DA9E8(*r3,*p,p[1]);
    while (fn_801DA94C(*r3,*p,p[1]) != 0) {
      fn_801DB088();
      _threadSwitch();
    }
    if (r8 == 1) {
      fn_801DA4E8(*r3,0);
    }
    fn_801DA8C4(*r3,*p,p[1]);
    p = p + 2;
  }
  return;
}

/* Address: 0x8025DF38 | Size: 0x178 | Ghidra import */
u32 fn_8025DF38(int *r3,u32 r4,u16 *r5,int r6)

{
    extern u32 fn_800E0C04();
    extern int fn_800E3C94();
    extern int fn_800E8FE8();
    extern int fn_800E900C();
    extern int fn_800E90C8();
    extern int fn_800E9108();
    extern u32 fn_800FF56C();
    extern int fn_80113F6C();
    extern int fn_80115280();
    extern u32 fn_8011538C();
    extern int fn_80115BD8();
    extern u32 fn_8018F470();
    extern s8 fn_801DDD28();
    extern int fn_801DE190();
  BOOL bVar1;
  u32 uVar2;
  int iVar3;
  s8 cVar8;
  int iVar4;
  int iVar5;
  u32 uVar6;
  u32 uVar7;
  int iVar9;
  int iVar10;
  int local_68 [17];
  
  uVar2 = fn_800E0C04(0xffffffff);
  iVar3 = fn_801DE190(r4,uVar2,0);
  *r3 = iVar3;
  if (iVar3 == 0) {
    uVar2 = 0;
  }
  else {
    for (iVar3 = 0; iVar3 < r6; iVar3 = iVar3 + 1) {
      cVar8 = fn_801DDD28(*r3,*r5,r5[1],0);
      if (cVar8 == '\0') {
        return 0;
      }
      r5 = r5 + 2;
    }
    if (((r3 != (int *)0x0) && (*r3 != 0)) && (iVar3 = fn_801DAC3C(), iVar3 != 0)) {
      fn_800E90C8(iVar3,1);
      iVar4 = fn_80115BD8();
      if (iVar4 != 0) {
        uVar2 = fn_8018F470(1);
        iVar5 = fn_80115280(iVar4);
        iVar9 = 0;
        iVar10 = 0;
        if (iVar5 == 1) {
          bVar1 = 0;
          while (!bVar1) {
            uVar6 = fn_8011538C(iVar4,0);
            uVar7 = fn_800FF56C();
            iVar5 = fn_80113F6C(uVar7,uVar6);
            if (iVar5 != 0) {
              *(int *)((int)local_68 + iVar10) = iVar5;
              iVar9 = iVar9 + 1;
              iVar10 = iVar10 + 4;
            }
            bVar1 = 1;
          }
          fn_800E9108(iVar3,1);
          fn_800E8FE8(iVar3,uVar2);
          fn_800E900C(iVar3,iVar9,local_68);
          fn_800E3C94(iVar3,1);
        }
      }
    }
    uVar2 = 1;
  }
  return uVar2;
}

/* Address: 0x8025E0B0 | Size: 0x10C | Ghidra import */
u32 fn_8025E0B0(int *r3,u32 r4,u16 *r5,int r6)

{
    extern u32 fn_8011F5B0();
  u32 uVar1;
  u32 uVar2;
  u16 sVar4;
  u16 sVar6;
  u32 iResult;
  int iVar3;
  u8 cVar5;

  sVar4 = (int)fn_8012640C(r4,0,0x6e,0);
  if (sVar4 == 0) {
    uVar1 = 0;
  }
  else {
    sVar6 = (int)fn_8012640C(0,sVar4,0x66,0);
    if (sVar6 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = fn_8011F5B0(r4);
      uVar2 = (int)fn_8012640C(r4,0,0xc1,0);
      uVar2 = (-uVar2 | uVar2) >> 0x1f;
      iResult = fn_801DE190(sVar6,uVar1,uVar2);
      *r3 = iResult;
      if (iResult == 0) {
        uVar1 = 0;
      }
      else {
        u16 *p = r5;
        for (iVar3 = 0; iVar3 < r6; iVar3 = iVar3 + 1) {
          cVar5 = fn_801DDD28(*r3,*p,p[1],0);
          if (cVar5 == '\0') {
            return 0;
          }
          p = p + 2;
        }
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025E1BC(void) {
#include "src/game/colosseum_battle_fn_8025E1BC.inc"
}
#endif
#pragma pop
static inline u32 *validptr_8025E1BC(u32 *p) {

  u32 *q = p;

  if (q != (u32 *)0 && *q != 0) return q;

  return (u32 *)0;

}

#pragma push
#pragma optimize_for_size off
void fn_8025E1BC(float f1, float f2, float f3, float f4, float f5, float f6) {
  extern u32 lbl_8047B658;
  extern u16 lbl_80478DA0;
  extern f32 lbl_8047E678;
  extern f32 lbl_8047E67C;
  extern void fn_8025DDF4();
  extern u8 fn_8025DF38();
  extern void fn_8025DE0C();
  extern void fn_8025DDAC();
  extern void fn_8025DE54();
  extern void fn_800E01F4();
  u32 *r31;
  u32 *r30;
  float out_buf[3];
  float scale_buf[3];
  float sv1, sv2, sv3, sv4, sv5, sv6;
  float fscale;
  sv1 = f1; sv2 = f2; sv3 = f3;
  sv4 = f4; sv5 = f5; sv6 = f6;
  fn_801DADC0(1);
  fn_8025DDF4(&lbl_8047B658);
  if ((u8)(fn_8025DF38(&lbl_8047B658, 0x3f, &lbl_80478DA0, 1) == 1) == 1) {
    r31 = validptr_8025E1BC(&lbl_8047B658);
    r30 = validptr_8025E1BC(&lbl_8047B658);
    if (r30 != (u32 *)0 && *r30 != 0) {
      fn_800E01F4(sv1, out_buf, sv2, sv3);
      fn_8025DE0C(r30, (u32)out_buf);
    }
    {
      u32 *r3;
      r3 = validptr_8025E1BC(&lbl_8047B658);
      if (r3 != (u32 *)0 && *r3 != 0) {
        fscale = lbl_8047E678;
        scale_buf[0] = fscale * sv4;
        scale_buf[1] = fscale * sv5;
        scale_buf[2] = fscale * sv6;
        fn_8025DDAC(r3, (u32)scale_buf);
      }
    }
    if (r31 != (u32 *)0 && *r31 != 0) {
      fn_8025DE54(r31, &lbl_80478DA0, 1, 0, 0, 1);
      fn_801C41C8(5, lbl_8047E67C);
      fn_801C40F0(1);
    }
  }
  fn_801DAC90();
}
#pragma pop

/* Address: 0x8025E534 | Size: 0xC0 | Ghidra import */
void fn_8025E534(void)
{
    extern u32 lbl_8027A478[];
    extern u32 fn_800F9318();
    extern u32 fn_80113F48();
    extern int fn_80118A68();
    extern int fn_80118F04();
    extern u32 fn_801190DC();
    extern int fn_8025DC2C();
    extern u32 lbl_8047B660;
    extern u32 lbl_8047B664;
    extern f32 lbl_8047E684;
    float fVar1;
    u32 uVar2;
    u32 state;
    u32 local_buf[3];

    local_buf[0] = lbl_8027A478[0];
    local_buf[1] = lbl_8027A478[1];
    local_buf[2] = lbl_8027A478[2];
    state = lbl_8047B664;
    if ((s32)state != 1) goto skip_cleanup;
    if ((s32)state != 1) goto skip_cleanup;
    fn_80118A68(lbl_8047B660, 1);
    lbl_8047B660 = 0;
    lbl_8047B664 = 0;
skip_cleanup:
    uVar2 = fn_80113F48();
    uVar2 = fn_800F9318(uVar2, 0x108a1400);
    uVar2 = fn_801190DC(uVar2, 0, 0);
    lbl_8047B660 = uVar2;
    fn_80118F04(uVar2, local_buf);
    fVar1 = lbl_8047E684;
    lbl_8047B664 = 1;
    fn_8025DC2C((double)fVar1);
    if ((s32)lbl_8047B664 == 1) {
        fn_80118A68(lbl_8047B660, 1);
        lbl_8047B660 = 0;
        lbl_8047B664 = 0;
    }
}

/* Address: 0x8025E5F4 | Size: 0x4C | Ghidra import */
void fn_8025E5F4(void)
{
    extern int GSthreadCreate();
    extern u32 fn_800FF560();
    extern void fn_8025E534();
    extern u32 fn_801CAF0C();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801CAF0C();
  if ((uVar1 & 0xFFFF) != 0) {
    uVar2 = fn_800FF560();
    GSthreadCreate(1, uVar2, 0x4000, 1, 1, (u32)fn_8025E534);
  }
}

/* Address: 0x8025E640 | Size: 0x37C | Ghidra import */
u32 fn_8025E640(u32 r3,int r4)

{
    extern int fn_80105C68();
    extern int fn_80105E7C();
    extern int fn_801067E8();
    extern int fn_8011DE98();
    extern u32 fn_8011F4A8();
    extern int fn_80122370();
    extern u32 fn_801229F4();
    extern u32 fn_80123090();
    extern u32 fn_801236F8();
    extern s8 fn_80123B5C();
    extern int fn_80123D58();
    extern int fn_8012546C();
    extern int fn_80132A38();
    extern int fn_80165668();
    /* fn_802600E4 forward-declared at file scope */
  u8 bVar11;
  int iVar1;
  u32 uVar2;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  u32 uVar3;
  s8 cVar12;
  u32 uVar4;
  u32 uVar13;
  char local_58;
  u8 local_57 [3];
  u8 local_54 [2];
  short local_52;
  short local_50;
  short local_4e;
  short local_4c;
  short local_4a;
  short local_48;
  u8 local_44 [2];
  short local_42;
  short local_40;
  short local_3e;
  short local_3c;
  short local_3a;
  short local_38;
  
  uVar13 = 0;
  local_58 = '\0';
  fn_8012640C(r3,0,0x7a,0);
  while (1) {
    if (r4 == 0) {
      fn_801254B4(r3,0,0xc6,0,0);
      return uVar13;
    }
    bVar11 = (int)fn_8012640C(r3,0,0x7a,0);
    if (99 < bVar11) break;
    iVar1 = (int)fn_8012640C(r3,0,0x79,0);
    uVar2 = fn_801229F4(r3,bVar11 + 1);
    sVar5 = (int)fn_8012640C(r3,0,0x87,0);
    sVar6 = (int)fn_8012640C(r3,0,0x88,0);
    sVar7 = (int)fn_8012640C(r3,0,0x89,0);
    sVar8 = (int)fn_8012640C(r3,0,0x8a,0);
    sVar9 = (int)fn_8012640C(r3,0,0x8b,0);
    sVar10 = (int)fn_8012640C(r3,0,0x8c,0);
    if ((u32)(iVar1 + r4) < uVar2) {
      r4 = 0;
      fn_801254B4(r3,0,0x79,0);
    }
    else {
      r4 = (iVar1 + r4) - uVar2;
      uVar13 = 1;
      fn_8011DE98(r3,uVar2);
      fn_8012546C(r3);
      uVar3 = fn_80123090(r3);
      fn_80122370(r3,uVar3,0);
      uVar2 = fn_8011F4A8(r3);
      fn_80165668(0x4ca,0,0xff);
      fn_80132A38(0x2f,uVar2 & 0xff);
      fn_801067E8(0x44ce,1,0);
      local_52 = (int)fn_8012640C(r3,0,0x87,0);
      local_50 = (int)fn_8012640C(r3,0,0x88,0);
      local_4e = (int)fn_8012640C(r3,0,0x89,0);
      local_4a = (int)fn_8012640C(r3,0,0x8a,0);
      local_48 = (int)fn_8012640C(r3,0,0x8b,0);
      local_4c = (int)fn_8012640C(r3,0,0x8c,0);
      local_3c = local_4c - sVar10;
      local_42 = local_52 - sVar5;
      local_40 = local_50 - sVar6;
      local_3e = local_4e - sVar7;
      local_3a = local_4a - sVar8;
      local_38 = local_48 - sVar9;
      local_44[0] = 1;
      fn_80105E7C(local_44,1);
      local_54[0] = 0;
      fn_80105E7C(local_54,1);
      fn_80105C68(1);
      local_58 = '\0';
      while (uVar4 = fn_801236F8(r3,uVar2,&local_58), (uVar4 & 0xffff) != 0) {
        cVar12 = fn_80123B5C(r3,uVar4);
        if ((cVar12 == -1) &&
           (iVar1 = fn_802600E4(r3,uVar4,local_57,0,0x8025e3b0,0), iVar1 != 0)) {
          fn_80123D58(r3,local_57[0],uVar4);
        }
        local_58 = local_58 + '\x01';
      }
    }
  }
  return uVar13;
}

/* Address: 0x8025EF58 | Size: 0x354 | Ghidra import */

void fn_8025EF58(void)

{
    extern u32 _DAT_804782bc;
    extern u32 _DAT_804782c0;
    extern u32 _DAT_804782c4;
    extern u32 fn_8001BDF4();
    extern int fn_800FF730();
    extern int fn_801065B8();
    extern int fn_8011288C();
    extern int fn_8011EE40();
    extern s8 fn_80129B2C();
    extern u32 fn_8012A5B0();
    extern int fn_801C40F0();
    extern s8 fn_801EEC74();
    extern u32 lbl_8047B660;
    extern u32 lbl_8047B664;
    extern u32 lbl_8047B668;
    extern u32 lbl_8047B66C;
    extern f32 lbl_8047E680;
    extern f32 lbl_8047E68C;
  int iVar1;

  u32 uVar2;
  u32 uVar3;
  short sVar5;
  s8 cVar7;
  short sVar6;
  u32 uVar4;

  u16 uVar8;
  u8 auStack_28 [24];
  
  if ((int)lbl_8047B668 != -1) {
    sVar6 = 0;
    fn_80129280(0,0);
    uVar2 = fn_80129280(0,2);
    uVar8 = 0;
    while (1) {
      if (5 < uVar8) break;
      uVar3 = fn_8012AC08(uVar2,uVar8);
      cVar7 = fn_80123FBC();
      if (cVar7 != '\0') {
        cVar7 = fn_8011FC74(uVar3);
        if (cVar7 == '\x01') {
          fn_8011EE40(uVar3);
          cVar7 = fn_801EEC74();
          if (cVar7 == '\0') {
            sVar6 = sVar6 + 1;
          }
        }
      }
      uVar8 = uVar8 + 1;
    }
    sVar5 = fn_801CAF0C();
    fn_80129280(0,0);
    fn_80129280(0,2);
    cVar7 = fn_80129B2C(0,0x219);
    if (cVar7 == '\x01') {
      if (sVar6 == 0) {
        iVar1 = 3;
      }
      else if (sVar5 == 0) {
        iVar1 = 1;
      }
      else {
        iVar1 = 2;
      }
    }
    else if (sVar5 == 0) {
      iVar1 = 5;
    }
    else {
      iVar1 = 4;
    }
    if (lbl_8047B668 == 0) {
      if (iVar1 == 1) {
        iVar1 = 5;
      }
      else if (iVar1 == 2) {
        iVar1 = 4;
      }
      else if (iVar1 == 3) {
        iVar1 = 5;
      }
    }
    else if (lbl_8047B668 == 1) {
      iVar1 = 2;
    }
    if (iVar1 == 2) {
      fn_8025DD14((int*)auStack_28);
      fn_80165668(0x3c8,0,0xff);
      uVar2 = fn_8012A5B0(0,3,lbl_8047B66C & 0xffff);
      _DAT_804782c0 = (int)fn_8012640C(uVar2,0,0x6e,0);
      _DAT_804782bc = 1;
      _DAT_804782c4 = lbl_8047B66C;
      fn_801C41C8((double)lbl_8047E680,3);
      fn_801C40F0(1);
      fn_800FF730(0x385);
      fn_8011288C(0,0);
      _threadSwitch();
      fn_801C41C8((double)lbl_8047E68C,2);
      fn_801C40F0(1);
      fn_8025DCBC((int*)auStack_28);
    }
    else if (iVar1 == 4) {
      if (lbl_8047B664 == 1) {
        fn_80118A68(lbl_8047B660,1);
        lbl_8047B660 = 0;
        lbl_8047B664 = 0;
      }
      sVar6 = fn_801CAF0C();
      if (sVar6 == 0) {
        lbl_8047B668 = 0xffffffff;
        lbl_8047B66C = 0xffffffff;
      }
      else {
        fn_801067E8(0x3b0f,1,0);
        fn_801065B8(1);
        uVar4 = fn_8001BDF4(7,0,0);
        if (uVar4 != 0xffffffff) {
          uVar2 = fn_8012A5B0(0,3,uVar4 & 0xffff);
          _DAT_804782c0 = (int)fn_8012640C(uVar2,0,0x6e,0);
          _DAT_804782bc = 0;
          _DAT_804782c4 = uVar4;
          fn_801C41C8((double)lbl_8047E680,3);
          fn_801C40F0(1);
          fn_8025DD14((int*)auStack_28);
          fn_800FF730(0x385);
          fn_8011288C(0,0);
          _threadSwitch();
          fn_801C41C8((double)lbl_8047E68C,2);
          fn_801C40F0(1);
          fn_8025DCBC((int*)auStack_28);
        }
      }
    }
    else {
      lbl_8047B668 = 0xffffffff;
      lbl_8047B66C = 0xffffffff;
    }
  }
  return;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F2AC(void) {
#include "src/game/colosseum_battle_fn_8025F2AC.inc"
}
#else
#pragma optimization_level 4
void fn_8025F2AC(void) {
  u32 *p = &lbl_8047B668;
  p[0] = 0xFFFFFFFF;
  p[1] = 0xFFFFFFFF;
}
#endif
#pragma pop

/* Address: 0x8025F2C0 | Size: 0x3C | Ghidra import */
void fn_8025F2C0(int r3,u32 r4)

{
    extern u32 lbl_8047B668;
    u32 *p = &lbl_8047B668;
    p[0] = r3;
    p[1] = r4;
  if ((r3 == 0) || (r3 == 1)) {
    fn_8025EF58();
  }
  return;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F2FC(void) {
#include "src/game/colosseum_battle_fn_8025F2FC.inc"
}
#endif
#pragma pop
void fn_8025F2FC(int r3) {
  u8 *entry;
  u8 b7;
  entry = lbl_804783E0 + (r3 * 0x100);
  if (*(s32 *)(entry + 0x20) != 0) return;
  if ((*(u8 *)(entry + 0x5) != 0) || (*(u8 *)(entry + 0x6) != 4)) {
    *(s32 *)(entry + 0x20) = 1;
    return;
  }
  b7 = *(u8 *)(entry + 0x7);
  *(u8 *)(*(u32 *)(entry + 0x14)) = b7 & 0x3a;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F350(void) {
#include "src/game/colosseum_battle_fn_8025F350.inc"
}
#endif
#pragma pop
void fn_8025F350(void) {
  int i;
  u32 r0;
  u32 r30;
  u8 *r29;
  u8 *r28;
  r0 = *(u32 *)0x800000F8;
  r0 = r0 >> 2;
  r0 = (u32)(((u64)0x431BDE83UL * r0) >> 32);
  r0 = r0 >> 15;
  r0 = r0 * 60;
  r30 = r0 >> 3;
  r29 = lbl_804783E0;
  r28 = lbl_804782E0;
  for (i = 0; i < 4; i++) {
    *(u32 *)(r29 + 0x34) = r30;
    *(u32 *)(r29 + 0x30) = 0;
    OSInitThreadQueue((void *)(r29 + 0x24));
    *(u32 *)(r29 + 0xF8) = (u32)r28;
    r29 += 0x100;
    r28 += 0x40;
  }
  OSInitAlarm();
  fn_800AE7E0();
  lbl_8047B670 = 0;
  OSRegisterResetFunction(lbl_8039A6B8);
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F3F4(void) {
#include "src/game/colosseum_battle_fn_8025F3F4.inc"
}
#endif
#pragma pop
u32 fn_8025F3F4(int r3, u32 r4) {
  int idx;
  u8 *entry;
  u32 result;
  idx = r3;
  entry = lbl_804783E0 + idx * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    result = 2;
  } else {
    *(u8 *)(entry + 0x0) = 0;
    *(u32 *)(entry + 0x14) = r4;
    *(u32 *)(entry + 0x1C) = (u32)fn_8025F7E8;
    result = fn_8025F9AC(idx, 1, 3, (u32)fn_8025F2FC);
  }
  if (result == 0) {
    result = fn_8025F81C(idx);
  }
  return result;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F484(void) {
#include "src/game/colosseum_battle_fn_8025F484.inc"
}
#endif
#pragma pop
u32 fn_8025F484(int r3, u32 r4) {
  int idx;
  u8 *entry;
  u32 result;
  idx = r3;
  entry = lbl_804783E0 + idx * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    result = 2;
  } else {
    *(u8 *)(entry + 0x0) = 0xFF;
    *(u32 *)(entry + 0x14) = r4;
    *(u32 *)(entry + 0x1C) = (u32)fn_8025F7E8;
    result = fn_8025F9AC(idx, 1, 3, (u32)fn_8025F2FC);
  }
  if (result == 0) {
    result = fn_8025F81C(idx);
  }
  return result;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F514(void) {
#include "src/game/colosseum_battle_fn_8025F514.inc"
}
#endif
#pragma pop
#pragma scheduling off
u32 fn_8025F514(void) {
  lbl_8047B670 = 1;
  return 1;
}
#pragma scheduling on

/* Address: 0x8025F524 | Size: 0x60 | Ghidra import */
void fn_8025F524(int r3)

{
    extern int fn_800054F4();
  r3 = r3 * 0x100;
  if (*(int *)(r3 + -0x7fb87c00) == 0) {
    fn_800054F4(*(u32 *)(r3 + -0x7fb87c08),r3 + -0x7fb87c1b,4);
    **(u8 **)(r3 + -0x7fb87c0c) = *(u8 *)(r3 + -0x7fb87c17) & 0x3a;
  }
  return;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F584(void) {
#include "src/game/colosseum_battle_fn_8025F584.inc"
}
#endif
#pragma pop
u32 fn_8025F584(int r3, u32 r4, u32 r5) {
  int idx;
  u8 *entry;
  u32 result;
  idx = r3;
  entry = lbl_804783E0 + idx * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    result = 2;
  } else {
    *(u8 *)(entry + 0x0) = 0x14;
    *(u32 *)(entry + 0x18) = r4;
    *(u32 *)(entry + 0x14) = r5;
    *(u32 *)(entry + 0x1C) = (u32)fn_8025F7E8;
    result = fn_8025F9AC(idx, 1, 5, (u32)fn_8025F524);
  }
  if (result == 0) {
    result = fn_8025F81C(idx);
  }
  return result;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F618(void) {
#include "src/game/colosseum_battle_fn_8025F618.inc"
}
#endif
#pragma pop
void fn_8025F618(int r3) {
  u8 *entry;
  entry = lbl_804783E0 + r3 * 0x100;
  if (*(s32 *)(entry + 0x20) != 0) return;
  *(u8 *)(*(u32 *)(entry + 0x14)) = *(u8 *)(entry + 0x5) & 0x3A;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F648(void) {
#include "src/game/colosseum_battle_fn_8025F648.inc"
}
#endif
#pragma pop
u32 fn_8025F648(int r3, u32 r4, u32 r5) {
  int idx;
  u32 r29;
  u32 r30;
  u8 *entry;
  u32 result;
  idx = r3;
  r29 = r4;
  r30 = r5;
  entry = lbl_804783E0 + idx * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    result = 2;
  } else {
    *(u8 *)(entry + 0x0) = 0x15;
    memcpy(entry + 1, (void *)r29, 4);
    *(u32 *)(entry + 0x18) = r29;
    *(u32 *)(entry + 0x14) = r30;
    *(u32 *)(entry + 0x1C) = (u32)fn_8025F7E8;
    result = fn_8025F9AC(idx, 5, 1, (u32)fn_8025F618);
  }
  if (result == 0) {
    result = fn_8025F81C(idx);
  }
  return result;
}

/* Address: 0x8025F70C | Size: 0xDC | Ghidra import */
void fn_8025F70C(int r3,u32 r4,u32 r5)

{
    extern int fn_8009BB68();
    extern int fn_8009BD60();
    extern u32 lbl_8047B670;
  int iVar1;
  void *pcVar2;

  u8 auStack_2e0 [724];
  
  iVar1 = r3 * 0x100;
  if (lbl_8047B670 == 0) {
    if ((r4 & 0xf) == 0) {
      *(u32 *)(iVar1 + -0x7fb87c00) = 0;
    }
    else {
      *(u32 *)(iVar1 + -0x7fb87c00) = 1;
    }
    pcVar2 = *(void **)(iVar1 + -0x7fb87be8);
    if (pcVar2 != (void *)0x0) {
      *(u32 *)(iVar1 + -0x7fb87be8) = 0;
      ((void (*)())pcVar2)(r3);
    }
    if (*(int *)(iVar1 + -0x7fb87c04) != 0) {
      fn_8009BD60((int*)auStack_2e0);
      fn_8009BB68((int*)auStack_2e0);
      pcVar2 = *(void **)(iVar1 + -0x7fb87c04);
      *(u32 *)(iVar1 + -0x7fb87c04) = 0;
      ((void (*)())pcVar2)(r3,*(u32 *)(iVar1 + -0x7fb87c00));
      fn_8009BD60((int*)auStack_2e0);
      fn_8009BB68(r5);
    }
  }
  return;
}

/* Address: 0x8025F7E8 | Size: 0x34 | Ghidra import */
void fn_8025F7E8(int r3)

{
    extern int fn_800A2478();
  fn_800A2478(r3 * 0x100 + -0x7fb87bfc);
  return;
}

/* Address: 0x8025F81C | Size: 0x6C | Ghidra import */
u32 fn_8025F81C(int r3)

{
    extern u32 fn_8009DF3C();
    extern int fn_8009DF64();
    extern int fn_800A238C();
  u32 uVar1;
  u32 uVar2;
  
  r3 = r3 * 0x100;
  uVar1 = fn_8009DF3C();
  while (*(int *)(r3 + -0x7fb87c04) != 0) {
    fn_800A238C(r3 + -0x7fb87bfc);
  }
  uVar2 = *(u32 *)(r3 + -0x7fb87c00);
  fn_8009DF64(uVar1);
  return uVar2;
}

/* Address: 0x8025F888 | Size: 0x124 | Ghidra import */
void fn_8025F888(int r3,u32 r4)

{
    extern u32 DAT_80478410;
    extern u32 DAT_80478414;
    extern u32 fn_8009BBC4();
    extern int fn_800A1960();
    extern int fn_800D06F4();
    extern u32 lbl_8047B670;
  int iVar1;
  int iVar2;
  u32 uVar3;
  void *pcVar4;

  u8 auStack_2e0 [724];
  
  iVar1 = r3 * 0x100;
  if (lbl_8047B670 == 0) {
    if (((r4 & 0xff) == 0) && ((r4 & 0xffff0000) == 0x40000)) {
      iVar2 = fn_800D06F4(r3,iVar1 + -0x7fb87c20,*(u32 *)(iVar1 + -0x7fb87c14),
                           iVar1 + -0x7fb87c1b,*(u32 *)(iVar1 + -0x7fb87c10),0x8025f70c,
                           *(u32 *)(&DAT_80478410 + iVar1),
                           *(u32 *)(&DAT_80478414 + iVar1));
      if (iVar2 != 0) {
        return;
      }
      *(u32 *)(iVar1 + -0x7fb87c00) = 2;
    }
    else {
      *(u32 *)(iVar1 + -0x7fb87c00) = 1;
    }
    pcVar4 = *(void **)(iVar1 + -0x7fb87be8);
    if (pcVar4 != (void *)0x0) {
      *(u32 *)(iVar1 + -0x7fb87be8) = 0;
      ((void (*)())pcVar4)(r3);
    }
    if (*(int *)(iVar1 + -0x7fb87c04) != 0) {
      uVar3 = fn_8009BBC4();
      fn_8009BD60((int*)auStack_2e0);
      fn_8009BB68((int*)auStack_2e0);
      pcVar4 = *(void **)(iVar1 + -0x7fb87c04);
      *(u32 *)(iVar1 + -0x7fb87c04) = 0;
      ((void (*)())pcVar4)(r3,*(u32 *)(iVar1 + -0x7fb87c00));
      fn_8009BD60((int*)auStack_2e0);
      fn_8009BB68(uVar3);
      fn_800A1960();
    }
  }
  return;
}

/* Address: 0x8025F9AC | Size: 0x74 | Ghidra import */
u32 fn_8025F9AC(int r3,u32 r4,u32 r5,u32 r6)

{
    extern int fn_800D0CBC();
  int iVar1;
  u32 uVar2;
  
  iVar1 = r3 * 0x100;
  uVar2 = fn_8009DF3C();
  *(u32 *)(iVar1 + -0x7fb87be8) = r6;
  *(u32 *)(iVar1 + -0x7fb87c14) = r4;
  *(u32 *)(iVar1 + -0x7fb87c10) = r5;
  fn_800D0CBC(r3,0x8025f888);
  fn_8009DF64(uVar2);
  return 0;
}

/* Address: 0x8025FA20 | Size: 0x1AC | Ghidra import */
void fn_8025FA20(u16 r3,u32 *r4,u32 *r5)

{
    extern u32 lbl_8047E690;
    extern u32 lbl_8047E694;
    extern u32 lbl_8047E698;
    extern u32 lbl_8047E69C;
    extern u32 lbl_8047E6A0;
    extern u32 lbl_8047E6A4;
    extern u32 lbl_8047E6A8;
    extern u32 lbl_8047E6AC;
  u32 uVar1;

  uVar1 = lbl_8047E690;
  if (r3 == 0x92) {
    uVar1 = lbl_8047E6A4;
  }
  else if (r3 < 0x92) {
    if (r3 == 0x4a) {
      uVar1 = lbl_8047E698;
    }
    else if (r3 < 0x4a) {
      if (r3 == 0x26) {
        uVar1 = lbl_8047E694;
      }
      else if (r3 < 0x26) {
        if (r3 == 0x1a) {
          uVar1 = lbl_8047E6A0;
        }
        else if ((r3 < 0x1a) && (r3 == 6)) {
          uVar1 = lbl_8047E6A0;
        }
      }
      else if (r3 == 0x44) {
        uVar1 = lbl_8047E6A8;
      }
    }
    else if (r3 == 0x8e) {
      uVar1 = lbl_8047E69C;
    }
    else if (r3 < 0x8e) {
      if (r3 == 0x85) {
        uVar1 = lbl_8047E698;
      }
      else if ((r3 < 0x85) && (r3 < 0x4c)) {
        uVar1 = lbl_8047E698;
      }
    }
    else if (r3 == 0x90) {
      uVar1 = lbl_8047E6A4;
    }
    else if (0x8f < r3) {
      uVar1 = lbl_8047E69C;
    }
  }
  else if (r3 == 0x136) {
    uVar1 = lbl_8047E69C;
  }
  else if (r3 < 0x136) {
    if (r3 == 0xfa) {
      uVar1 = lbl_8047E698;
    }
    else if (r3 < 0xfa) {
      if (r3 == 0xe2) {
        uVar1 = lbl_8047E69C;
      }
      else if ((r3 < 0xe2) && (r3 == 0xd9)) {
        uVar1 = lbl_8047E69C;
      }
    }
    else if (r3 == 300) {
      uVar1 = lbl_8047E6A0;
    }
  }
  else if (r3 == 0x16b) {
    uVar1 = lbl_8047E698;
  }
  else if (r3 < 0x16b) {
    if (r3 == 0x14b) {
      uVar1 = lbl_8047E6A8;
    }
  }
  else if (r3 == 0x198) {
    uVar1 = lbl_8047E6A4;
  }
  else if ((r3 < 0x198) && (0x196 < r3)) {
    uVar1 = lbl_8047E6A4;
  }
  if (r5 != (u32 *)0x0) {
    *r5 = lbl_8047E6AC;
  }
  if (r4 != (u32 *)0x0) {
    *r4 = uVar1;
    return;
  }
  return;
}

/* Address: 0x8025FBCC | Size: 0x168 | Ghidra import */
void fn_8025FBCC(int r3)

{
  u16 *puVar1;
  u32 uVar2;
  u32 uVar3;
  u16 uVar4;
  
  if (r3 == 0) {
    fn_80129280(0,0xc);
  }
  for (uVar4 = 1; uVar4 < 0xfc; uVar4 = uVar4 + 1) {
    puVar1 = (u16 *)fn_80129280(0,0xc);
    for (uVar3 = 0; (uVar3 & 0xffff) < (u32)*puVar1; uVar3 = uVar3 + 1) {
      if ((puVar1[(uVar3 & 0xffff) * 6 + 2] & 0x3fff) != uVar4) {
        }
        puVar1[(u32)*puVar1 * 6 + 2] = uVar4 | 0x8000;
        uVar2 = fn_800E0C04(0xffffffff);
        *(u32 *)(puVar1 + (u32)*puVar1 * 6 + 6) = uVar2;
        *puVar1 = *puVar1 + 1;
      }
  }
  uVar4 = 0x115;
  do {
    if (0x19b < uVar4) {
      return;
    }
    puVar1 = (u16 *)fn_80129280(0,0xc);
    for (uVar3 = 0; (uVar3 & 0xffff) < (u32)*puVar1; uVar3 = uVar3 + 1) {
      if ((puVar1[(uVar3 & 0xffff) * 6 + 2] & 0x3fff) != uVar4) {
        }
        puVar1[(u32)*puVar1 * 6 + 2] = uVar4 | 0x8000;
        uVar2 = fn_800E0C04(0xffffffff);
        *(u32 *)(puVar1 + (u32)*puVar1 * 6 + 6) = uVar2;
        *puVar1 = *puVar1 + 1;
      }
    uVar4 = uVar4 + 1;
  } while (1);
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025FD34(void) {
#include "src/game/colosseum_battle_fn_8025FD34.inc"
}
#endif
#pragma pop
u32 fn_8025FD34(u16 *r3, u16 r4) {
  u16 *queue;
  u16 count;
  u32 lookup;
  u16 i;
  u32 r5;
  u32 r0;
  queue = r3;
  lookup = (u32)r4;
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  count = *queue;
  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
    r5 = (u32)(u16)i * 12;
    r0 = (u32)*(u16 *)((u8 *)queue + r5 + 4) & 0x3FFF;
    if (r0 == lookup) {
      return *(u32 *)((u8 *)queue + r5 + 8);
    }
  }
  return 0;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025FDDC(void) {
#include "src/game/colosseum_battle_fn_8025FDDC.inc"
}
#endif
#pragma pop
u32 fn_8025FDDC(u16 *r3, u16 r4) {
  u16 *queue;
  u16 count;
  u32 lookup;
  u16 i;
  u32 r5;
  u32 r0;
  queue = r3;
  lookup = (u32)r4;
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  count = *queue;
  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
    r5 = (u32)(u16)i * 12;
    r0 = (u32)*(u16 *)((u8 *)queue + r5 + 4) & 0x3FFF;
    if (r0 == lookup) {
      return *(u32 *)((u8 *)queue + r5 + 0xC);
    }
  }
  return 0;
}

/* Address: 0x8025FE84 | Size: 0x60 | Ghidra import */
u16 fn_8025FE84(u16 *r3, u32 r4)
{
    u16 res;
    extern void *fn_80129280();
    if (r3 == (u16 *)0) {
        r3 = (u16 *)fn_80129280(0, 0xc);
    }
    if (*r3 != 0) {
        r3 = (u16 *)((u8 *)r3 + (r4 & 0xFFFF) * 12);
        res = *(u16 *)((u8 *)r3 + 4);
    } else {
        res = 0;
    }
    return res;
}

/* Address: 0x8025FEE4 | Size: 0x34 | Ghidra import */
u16 fn_8025FEE4(u16 *r3)

{
  if (r3 == (u16 *)0x0) {
    r3 = (u16 *)fn_80129280(0,0xc);
  }
  return *r3;
}

/* Address: 0x8025FF18 | Size: 0x84 | Ghidra import */
u32 fn_8025FF18(u16 *r3)

{
  u32 uVar1;
  u16 uVar2;
  u32 uVar3;
  
  uVar3 = 0;
  if (r3 == (u16 *)0x0) {
    r3 = (u16 *)fn_80129280(0,0xc);
  }
  uVar2 = 0;
  while (1) {
    if (*r3 <= uVar2) break;
    uVar1 = (u32)uVar2;
    if ((r3[uVar1 * 6 + 2] & 0x8000) != 0) {
      uVar3 = 1;
    }
    uVar2 = uVar2 + 1;
    r3[uVar1 * 6 + 2] = r3[uVar1 * 6 + 2] & 0x3fff;
  }
  return uVar3;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025FF9C(void) {
#include "src/game/colosseum_battle_fn_8025FF9C.inc"
}
#endif
#pragma pop
void fn_8025FF9C(u16 *r3, u32 r4) {
  u16 *queue;
  u32 r30;
  u16 count;
  u32 lookup;
  u32 r5;
  u16 i;
  u32 r0;
  u32 new_entry_off;
  u8 *new_entry;
  queue = r3;
  r30 = r4;
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  lookup = (u32)(u16)fn_8011F5C8(r30);
  count = *queue;
  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
    r5 = (u32)(u16)i * 12;
    r0 = (u32)*(u16 *)((u8 *)queue + r5 + 4) & 0x3FFF;
    if (r0 == lookup) {
      return;
    }
  }
  new_entry_off = (u32)count * 12;
  new_entry = (u8 *)queue + new_entry_off;
  *(u16 *)(new_entry + 4) = (u16)(lookup | 0x8000);
  r5 = fn_8011F5B0(r30);
  new_entry_off = (u32)*queue * 12;
  new_entry = (u8 *)queue + new_entry_off;
  *(u32 *)(new_entry + 0xC) = r5;
  r5 = fn_8011F520(r30);
  new_entry_off = (u32)*queue * 12;
  new_entry = (u8 *)queue + new_entry_off;
  *(u32 *)(new_entry + 0x8) = r5;
  *queue = (u16)(*queue + 1);
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80260070(void) {
#include "src/game/colosseum_battle_fn_80260070.inc"
}
#endif
#pragma pop
void fn_80260070(u16 *r3) {
  u16 *queue;
  int i;
  u32 off;
  u8 *entry;
  queue = r3;
  if (queue == (u16 *)0) {
    queue = (u16 *)fn_80129280(0, 0xC);
  }
  *queue = 0;
  off = 0;
  for (i = 0; i < 0x1F4; i++) {
    entry = (u8 *)queue + off;
    *(u16 *)(entry + 4) = 0;
    *(u32 *)(entry + 0xC) = fn_800E0C04(-1);
    off += 12;
  }
}

/* Address: 0x8026045C | Size: 0x27C | Ghidra import */
int fn_8026045C(u32 r3,u32 r4,int r5)

{
    extern u32 DAT_8027a488;
    extern int fn_80097A38();
    extern int GScameraGetPerspective();
    extern u32 GScameraGetActiveCamera();
    extern int fn_800ECB74();
    extern int fn_801766A8();
    extern int GSscene_GetCameraRotationVector();
    extern int GSscene_SetCameraRotationVector();
    extern int GSscene_GetCameraDirectionVector();
    extern int GSscene_SetCameraDirectionVector();
    extern int GSscene_GetCameraPositionVector();
    extern int GSscene_SetCameraPositionVector();
    extern int GSscene_GetCameraViewVector();
    extern int GSscene_SetCameraViewVector();
    extern int GSscene_SetMode();
    extern int fn_801DAC90();
    extern int fn_801DADC0();
    extern f32 lbl_8047E6C0;

  u32 uVar1;
  int iVar2;
  short sVar4;
  u32 uVar3;
  s8 cVar5;
  u16 *puVar6;
  int iVar7;
  int iVar8;
  float local_98;
  u8 auStack_94 [4];
  u8 auStack_90 [4];
  u8 auStack_8c [4];
  u32 local_88;
  u32 local_84;
  u32 local_80;
  u32 local_7c;
  u32 local_78;
  u32 local_74;
  u32 local_70;
  u32 local_6c;
  u32 local_68;
  u32 local_64;
  u32 local_60;
  u32 local_5c;
  u32 local_58;
  u32 local_54;
  u32 local_50;
  u32 local_4c;
  u32 local_48;
  u32 local_44;
  u32 local_40;
  u32 local_3c;
  u32 local_38;
  u32 local_34;
  u32 local_30;
  u32 local_2c;
  float local_28;
  
  fn_801C41C8((double)lbl_8047E6C0,3);
  fn_801C40F0(1);
  GSscene_GetCameraDirectionVector(&local_88);
  GSscene_GetCameraRotationVector(&local_7c);
  GSscene_GetCameraPositionVector(&local_70);
  GSscene_GetCameraViewVector(&local_64);
  uVar1 = GScameraGetActiveCamera();
  GScameraGetPerspective(uVar1,&local_98,auStack_94,auStack_90,auStack_8c);
  local_58 = local_88;
  local_54 = local_84;
  local_50 = local_80;
  local_4c = local_7c;
  local_48 = local_78;
  local_44 = local_74;
  local_40 = local_70;
  local_3c = local_6c;
  local_38 = local_68;
  local_34 = local_64;
  local_30 = local_60;
  local_2c = local_5c;
  local_28 = local_98;
  fn_801DAC90();
  iVar2 = fn_80097A38(r3,r4);
  if (3 < iVar2) {
    iVar2 = -1;
  }
  fn_801DADC0(2);
  sVar4 = (int)fn_8012640C(r3,0,0x6e,0);
  if (sVar4 == 0) {
    sVar4 = -1;
  }
  else {
    sVar4 = (int)fn_8012640C(0,sVar4,0x66,0);
    if (sVar4 == 0) {
      sVar4 = -1;
    }
  }
  if (sVar4 == -1) {
    iVar7 = 0;
  }
  else {
    uVar1 = fn_8011F5B0(r3);
    uVar3 = (int)fn_8012640C(r3,0,0xc1,0);
    iVar7 = fn_801DE190(sVar4,uVar1,(-uVar3 | uVar3) >> 0x1f);
    if (iVar7 == 0) {
      iVar7 = 0;
    }
    else {
      iVar8 = 0;
      puVar6 = (u16 *)&DAT_8027a488;
      do {
        if ((*(int *)(puVar6 + 2) == 1) && (cVar5 = fn_801DDD28(iVar7,*puVar6,4,0), cVar5 == '\0'))
        break;
        iVar8 = iVar8 + 1;
        puVar6 = puVar6 + 4;
      } while (iVar8 < 5);
      if (iVar8 < 5) {
        iVar7 = 0;
      }
    }
  }
  if (iVar7 != 0) {
    *(int *)(r5 + 4) = iVar7;
  }
  uVar1 = fn_801DAC3C(*(u32 *)(r5 + 4));
  fn_800ECB74(uVar1,1);
  fn_801DA4E8(*(u32 *)(r5 + 4),1);
  GSscene_SetMode(2);
  GSscene_SetCameraDirectionVector(&local_58);
  GSscene_SetCameraRotationVector(&local_4c);
  GSscene_SetCameraPositionVector(&local_40);
  GSscene_SetCameraViewVector(&local_34);
  fn_801766A8((double)local_28);
  fn_801C41C8((double)lbl_8047E6C0,2);
  fn_801C40F0(1);
  return (int)(char)iVar2;
}

/* Address: 0x802606D8 | Size: 0x238 | Ghidra import */
int fn_802606D8(u32 *r3,int r4,int r5,u32 r6)

{
    extern u32 DAT_8027a488;
    extern u32 DAT_8027a48c;
    extern int fn_800D3088();
    extern u32 fn_800F7AF0();
    extern u32 fn_800F7BC4();
    extern f32 lbl_8047E6C0;
  u16 uVar1;
  BOOL bVar2;

  s8 cVar7;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  int iVar8;
  u32 uVar9;
  float local_a8;
  u8 auStack_a4 [4];
  u8 auStack_a0 [4];
  u8 auStack_9c [4];
  u32 local_98;
  u32 local_94;
  u32 local_90;
  u32 local_8c;
  u32 local_88;
  u32 local_84;
  u32 local_80;
  u32 local_7c;
  u32 local_78;
  u32 local_74;
  u32 local_70;
  u32 local_6c;
  u32 local_68;
  u32 local_64;
  u32 local_60;
  u32 local_5c;
  u32 local_58;
  u32 local_54;
  u32 local_50;
  u32 local_4c;
  u32 local_48;
  u32 local_44;
  u32 local_40;
  u32 local_3c;
  float local_38;
  
  iVar8 = 0;
  bVar2 = 0;
  if ((r4 < 0) || (4 < r4)) {
    iVar8 = 0;
  }
  else {
    uVar1 = *(u16 *)(&DAT_8027a488 + r4 * 8);
    if (*(int *)(&DAT_8027a48c + r4 * 8) == 0) {
      uVar9 = *r3;
    }
    else {
      uVar9 = r3[1];
    }
    fn_801DA9E8(uVar9,uVar1,4);
    while (!bVar2) {
      fn_801DB088();
      if ((r6 & 10) != 0) {
        uVar6 = 4;
        if ((r6 & 2) != 0) {
          uVar6 = 2;
        }
        fn_801C41C8((double)lbl_8047E6C0,uVar6);
        r6 = r6 & 0xfffffff5;
      }
      cVar7 = fn_801DA94C(uVar9,uVar1,4);
      if (cVar7 == '\0') break;
      _threadSwitch();
      GSscene_GetCameraDirectionVector(&local_98);
      GSscene_GetCameraRotationVector(&local_8c);
      GSscene_GetCameraPositionVector(&local_80);
      GSscene_GetCameraViewVector(&local_74);
      uVar6 = GScameraGetActiveCamera();
      GScameraGetPerspective(uVar6,&local_a8,auStack_a4,auStack_a0,auStack_9c);
      local_68 = local_98;
      local_64 = local_94;
      local_60 = local_90;
      local_5c = local_8c;
      local_58 = local_88;
      local_54 = local_84;
      local_50 = local_80;
      local_4c = local_7c;
      local_48 = local_78;
      local_44 = local_74;
      local_40 = local_70;
      local_3c = local_6c;
      local_38 = local_a8;
      iVar3 = fn_800D3088();
      iVar8 = iVar8 + iVar3;
      if (r5 != 0) {
        uVar4 = fn_800F7AF0(1);
        uVar5 = fn_800F7BC4(1);
        if ((uVar5 & uVar4 & 0x200) != 0) {
          bVar2 = 1;
        }
      }
    }
    GSscene_SetMode(2);
    GSscene_SetCameraDirectionVector(&local_68);
    GSscene_SetCameraRotationVector(&local_5c);
    GSscene_SetCameraPositionVector(&local_50);
    GSscene_SetCameraViewVector(&local_44);
    fn_801766A8((double)local_38);
    if ((r6 & 0x14) != 0) {
      uVar6 = 5;
      if ((r6 & 4) != 0) {
        uVar6 = 3;
      }
      fn_801C41C8((double)lbl_8047E6C0,uVar6);
      fn_801C40F0(1);
    }
    fn_801DA8C4(uVar9,uVar1,4);
    if (bVar2 == 0) {
      iVar8 = -1;
    }
  }
  return iVar8;
}

/* Address: 0x80260EBC | Size: 0x414 | Ghidra import */
u32
fn_80260EBC(u32 r3,u32 r4,u32 r5,u16 *r6,
            int r7,u8 *r8)

{
    extern u32 DAT_8027a488;
    extern int fn_8011F5FC();
    extern int fn_801657D0();
    extern int fn_80260910();
    extern f32 lbl_8047E6C0;
  u16 uVar1;
  BOOL bVar2;

  short sVar7;
  s8 cVar8;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  int iVar6;
  int iVar9;
  u16 *puVar10;
  u8 local_188 [4];
  int local_184;
  int local_180;
  int local_178;
  int local_174;
  u32 local_170;
  u32 local_16c;
  u8 auStack_168 [320];
  
  fn_801DADC0(2);
  local_184 = 0;
  local_180 = 0;
  sVar7 = (int)fn_8012640C(r3,0,0x6e,0);
  if (sVar7 == 0) {
    sVar7 = -1;
  }
  else {
    sVar7 = (int)fn_8012640C(0,sVar7,0x66,0);
    if (sVar7 == 0) {
      sVar7 = -1;
    }
  }
  if (sVar7 == -1) {
    iVar9 = 0;
  }
  else {
    uVar3 = fn_8011F5B0(r3);
    uVar4 = (int)fn_8012640C(r3,0,0xc1,0);
    iVar9 = fn_801DE190(sVar7,uVar3,(-uVar4 | uVar4) >> 0x1f);
    if (iVar9 == 0) {
      iVar9 = 0;
    }
    else {
      iVar6 = 0;
      puVar10 = (u16 *)&DAT_8027a488;
      do {
        if ((*(int *)(puVar10 + 2) == 0) &&
           (cVar8 = fn_801DDD28(iVar9,*puVar10,4,0), cVar8 == '\0')) break;
        iVar6 = iVar6 + 1;
        puVar10 = puVar10 + 4;
      } while (iVar6 < 5);
      if (iVar6 < 5) {
        iVar9 = 0;
      }
    }
  }
  if (iVar9 != 0) {
    local_184 = iVar9;
    sVar7 = (int)fn_8012640C(r4,0,0x6e,0);
    if (sVar7 == 0) {
      sVar7 = -1;
    }
    else {
      sVar7 = (int)fn_8012640C(0,sVar7,0x66,0);
      if (sVar7 == 0) {
        sVar7 = -1;
      }
    }
    if (sVar7 == -1) {
      iVar9 = 0;
    }
    else {
      uVar3 = fn_8011F5B0(r4);
      uVar4 = (int)fn_8012640C(r4,0,0xc1,0);
      iVar9 = fn_801DE190(sVar7,uVar3,(-uVar4 | uVar4) >> 0x1f);
      if (iVar9 == 0) {
        iVar9 = 0;
      }
      else {
        iVar6 = 0;
        puVar10 = (u16 *)&DAT_8027a488;
        do {
          if ((*(int *)(puVar10 + 2) == 1) &&
             (cVar8 = fn_801DDD28(iVar9,*puVar10,4,0), cVar8 == '\0')) break;
          iVar6 = iVar6 + 1;
          puVar10 = puVar10 + 4;
        } while (iVar6 < 5);
        if (iVar6 < 5) {
          iVar9 = 0;
        }
      }
    }
    if (iVar9 != 0) {
      bVar2 = 1;
      local_180 = iVar9;
      goto LAB_0025e130;
    }
  }
  fn_801DAC90();
  bVar2 = 0;
LAB_0025e130:
  if (bVar2) {
    iVar9 = fn_801653C4();
    if (iVar9 == 0) {
      uVar4 = 0;
    }
    else {
      uVar4 = fn_801656D8();
      fn_80165A20(1,0x32,0xff);
    }
    local_174 = fn_801653BC();
    if (local_174 == 0) {
      uVar5 = 0;
    }
    else {
      uVar5 = fn_801656D8();
      fn_801657D0(0x32);
    }
    local_178 = iVar9;
    local_170 = uVar4;
    local_16c = uVar5;
    iVar9 = fn_80260910(&local_184,r5,r3,r4);
    if (local_178 != 0) {
      fn_80165A20(local_178,0x32,local_170 & 0xff);
    }
    if (local_174 != 0) {
      fn_801659FC(local_174,0x32,local_16c & 0xff);
    }
    if (iVar9 == 0) {
      bVar2 = 0;
    }
    else {
      fn_8011F5FC((int*)auStack_168,r4);
      for (iVar9 = 0; iVar9 < r7; iVar9 = iVar9 + 1) {
        uVar1 = *r6;
        iVar6 = fn_802600E4((int*)auStack_168,uVar1,local_188,0,0x8026045c,&local_184);
        if (iVar6 == 0) {
          local_188[0] = 0xff;
        }
        else {
          fn_80123D58((int*)auStack_168,local_188[0],uVar1);
        }
        r6 = r6 + 1;
        *r8 = local_188[0];
        r8 = r8 + 1;
      }
      fn_801C41C8((double)lbl_8047E6C0,3);
      fn_801C40F0(1);
      bVar2 = 1;
    }
    fn_801DAC90();
    if (bVar2) {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
  }
  else {
    uVar3 = 2;
  }
  return uVar3;
}

/* Address: 0x802613D4 | Size: 0x70 | Ghidra import */
#pragma peephole off
u32 fn_802613D4(u32 idx) {
    extern void* lbl_80478F80;
    extern void* lbl_80478F84;
    extern void fn_800DD970(char*, char*, ...);
    extern char lbl_8027A4C8[];
    extern char lbl_8039A6C8[];
    u8* entry;
    u16 i = (u16)idx;

    if (i >= *(u32*)lbl_80478F80) {
        fn_800DD970(lbl_8027A4C8, lbl_8039A6C8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478F84 + i * 8;
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u32*)(entry + 4);
}
#pragma peephole on

/* Address: 0x802614B4 | Size: 0x88 | Ghidra import */
int fn_802614B4(void)

{
    typedef struct BattleScanContext {
        u8 collectEntries;
        u8 consumeEntries;
        u16 count;
        u8 *entries;
        u8 *nextEntry;
    } BattleScanContext;
    typedef u32 (*BattleScanCallback)(u32, u32, char *);
    extern void fn_801F2B5C(u32, BattleScanCallback, void *, u32);
    extern void fn_801F37B0(u32, BattleScanCallback, void *, u32);
    extern u32 fn_80261708(u32, u32, char *);
    extern u32 fn_8026184C(u32, u32, char *);
    BattleScanContext scan;
    u16 firstCount;
    int total;

    scan.collectEntries = 0;
    scan.consumeEntries = 0;
    scan.count = 0;
    scan.nextEntry = 0;
    fn_801F2B5C(0, fn_8026184C, &scan, 0);
    firstCount = scan.count;
    scan.count = 0;
    fn_801F37B0(0, fn_80261708, &scan, 0);
    total = firstCount * 0x78;
    total += scan.count * 0x7C;
    return total + 0x48;
}

/* Address: 0x8026153C | Size: 0xB8 | Ghidra import */
void fn_8026153C(void *rawOut)
{
#pragma optimize_for_size on
    typedef struct BattleScanOutput {
        u16 firstCount;
        u16 secondCount;
        u8 entries[0x44];
    } BattleScanOutput;
    typedef struct BattleScanContext {
        u8 collectEntries;
        u8 consumeEntries;
        u16 count;
        u8 *entries;
        u8 *nextEntry;
    } BattleScanContext;
    typedef u32 (*BattleScanCallback)(u32, u32, char *);
    extern void *fn_801C3108(void);
    extern void fn_801F2B5C(u32, BattleScanCallback, void *, u32);
    extern void fn_801F37B0(u32, BattleScanCallback, void *, u32);
    extern u32 fn_80261708(u32, u32, char *);
    extern u32 fn_8026184C(u32, u32, char *);
    BattleScanOutput *out;
    BattleScanContext scan;
    u16 firstCount;
    u16 secondCount;
    u8 *entries;

    out = rawOut;
    entries = out->entries;
    memcpy(entries, fn_801C3108(), 0x44);
    scan.collectEntries = 1;
    scan.consumeEntries = 1;
    scan.count = 0;
    scan.entries = entries;
    scan.nextEntry = entries + 0x44;
    fn_801F2B5C(0, fn_8026184C, &scan, 0);
    firstCount = scan.count;
    scan.count = 0;
    fn_801F37B0(0, fn_80261708, &scan, 0);
    secondCount = scan.count;
    out->firstCount = firstCount;
    out->secondCount = secondCount;
    fn_801EF8F4(1);
    fn_801C3114();
    fn_801DAC90();
}

/* Address: 0x802615F4 | Size: 0x114 | Ghidra import */
void fn_802615F4(short *r3)

{
    extern u32 fn_800E3D08();
    extern int fn_800E9B2C();
    extern int fn_801C3430();
    extern int fn_801DA224();
    extern int fn_801DAEF8();
    extern u32 fn_801DE418();
    extern int fn_801F198C();
  BOOL bVar1;
  u32 uVar2;
  u32 uVar3;
  short sVar4;
  short sVar6;
  int *piVar5;
  
  fn_801DAEF8(10);
  sVar6 = *r3;
  piVar5 = (int *)(r3 + 0x24);
  sVar4 = r3[1];
  while (bVar1 = sVar6 != 0, sVar6 = sVar6 + -1, bVar1) {
    uVar3 = fn_801DE418(*(u16 *)(piVar5 + 1));
    *(u32 *)(*piVar5 + 0x27c0) = uVar3;
    *(u32 *)piVar5[2] = uVar3;
    uVar2 = fn_801DAC3C();
    fn_800E9B2C(uVar2,piVar5 + 3);
    fn_801DA224(uVar3,*(u8 *)((int)piVar5 + 6));
    piVar5 = piVar5 + 0x1e;
  }
  while (bVar1 = sVar4 != 0, sVar4 = sVar4 + -1, bVar1) {
    uVar3 = fn_801DE190(*(u16 *)(piVar5 + 1),piVar5[2],*(u8 *)((int)piVar5 + 7));
    *(u32 *)(*piVar5 + 0x600) = uVar3;
    *(u32 *)piVar5[3] = uVar3;
    uVar2 = fn_801DAC3C();
    fn_800E9B2C(uVar2,piVar5 + 4);
    fn_801DA224(uVar3,*(u8 *)((int)piVar5 + 6));
    uVar2 = fn_800E3D08(uVar2);
    fn_801DA4E8(uVar3,uVar2);
    piVar5 = piVar5 + 0x1f;
  }
  uVar3 = fn_801C3108();
  fn_800054F4(uVar3,r3 + 2,0x44);
  fn_801EF8F4(1);
  fn_801C3430();
  fn_801F198C();
  return;
}

/* Address: 0x80261708 | Size: 0x144 | Ghidra import */
u32 fn_80261708(u32 r3,u32 r4,char *r5)

{
    extern int fn_800E9C6C();
    extern u8 fn_801D9E1C();
    extern u8 fn_801DA354();
    extern short fn_801DAC78();
    extern int fn_801DB100();
    extern u32 fn_801DE164();
  int iVar1;
  int iVar2;
  short sVar5;
  u8 uVar6;
  u32 uVar3;
  int iVar4;
  int iVar7;
  int iVar8;
  u32 *puVar9;
  int iVar10;
  
  iVar2 = (int)fn_8012640C(r3,0,0xee,0);
  if ((iVar2 != 0) && (sVar5 = fn_801DAC78(), sVar5 != 0)) {
    if (*r5 != '\0') {
      puVar9 = *(u32 **)(r5 + 8);
      *puVar9 = r3;
      *(short *)(puVar9 + 1) = sVar5;
      uVar6 = fn_801DA354(iVar2);
      *(u8 *)((int)puVar9 + 6) = uVar6;
      uVar6 = fn_801D9E1C(iVar2);
      *(u8 *)((int)puVar9 + 7) = uVar6;
      uVar3 = fn_801DE164(iVar2);
      puVar9[2] = uVar3;
      if (iVar2 == 0) {
        iVar1 = 0;
      }
      else {
        iVar8 = 0;
        iVar4 = 0;
        do {
          iVar7 = 0;
          iVar10 = 2;
          do {
            iVar1 = *(int *)(r5 + 4) + iVar4 + iVar7;
            if (*(int *)(iVar1 + 4) == iVar2) {
              iVar1 = iVar1 + 4;
              goto LAB_0025e7f4;
            }
            iVar7 = iVar7 + 4;
            iVar10 = iVar10 + -1;
          } while (iVar10 != 0);
          iVar8 = iVar8 + 1;
          iVar4 = iVar4 + 0x10;
        } while (iVar8 < 4);
        iVar1 = 0;
      }
LAB_0025e7f4:
      puVar9[3] = iVar1;
      uVar3 = fn_801DAC3C(iVar2);
      fn_800E9C6C(uVar3,puVar9 + 4);
      *(int *)(r5 + 8) = *(int *)(r5 + 8) + 0x7c;
    }
    if (r5[1] != '\0') {
      fn_801DB100(iVar2);
    }
    *(short *)(r5 + 2) = *(short *)(r5 + 2) + 1;
  }
  return 1;
}

/* Address: 0x8026184C | Size: 0x108 | Ghidra import */
u32 fn_8026184C(u32 r3,u32 r4,char *r5)

{
  int iVar1;
  short sVar4;
  u8 uVar5;
  int iVar2;
  u32 uVar3;
  int *piVar6;
  u32 *puVar7;
  int iVar8;
  
  iVar1 = fn_801FB1C0(r3,0,0x4c,0);
  if ((iVar1 != 0) && (sVar4 = fn_801DAC78(), sVar4 != 0)) {
    if (*r5 != '\0') {
      puVar7 = *(u32 **)(r5 + 8);
      *puVar7 = r3;
      *(short *)(puVar7 + 1) = sVar4;
      uVar5 = fn_801DA354(iVar1);
      *(u8 *)((int)puVar7 + 6) = uVar5;
      if (iVar1 == 0) {
        piVar6 = (int *)0x0;
      }
      else {
        iVar2 = 0;
        iVar8 = 4;
        do {
          piVar6 = (int *)(*(int *)(r5 + 4) + iVar2);
          if (*piVar6 == iVar1) goto LAB_0025e8fc;
          iVar2 = iVar2 + 0x10;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
        piVar6 = (int *)0x0;
      }
LAB_0025e8fc:
      puVar7[2] = (u32)piVar6;
      uVar3 = fn_801DAC3C(iVar1);
      fn_800E9C6C(uVar3,puVar7 + 3);
      *(int *)(r5 + 8) = *(int *)(r5 + 8) + 0x78;
    }
    if (r5[1] != '\0') {
      fn_801DB100(iVar1);
    }
    *(short *)(r5 + 2) = *(short *)(r5 + 2) + 1;
  }
  return 1;
}

/* Address: 0x80261BEC | Size: 0xD0 | Ghidra import */
u32 fn_80261BEC(u32 r3,u32 r4,u8 *r5)

{
  u32 iVar1;
  u32 uVar2;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  u8 cVar6;

  iVar1 = fn_801F02AC(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fn_801F0134(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fn_801F0234();
      uVar3 = fn_801F0204();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
      }
    }
  }
  cVar6 = fn_80102620(uVar2);
  if ((cVar6 == '\x01') && (r5 != (u8 *)0x0)) {
    *r5 = 0;
  }
  return 1;
}

/* Address: 0x80261CBC | Size: 0xD0 | Ghidra import */
u32 fn_80261CBC(u32 r3,u32 r4)

{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u8 cVar5;
  u32 uVar6;

  iVar1 = fn_801F02AC(2,r3,r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1,0,5,0);
    sVar4 = fn_801F0134(r3,r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fn_801F0234();
      uVar2 = fn_801F0204();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0,uVar3,2,uVar2 & 0xffff);
      }
    }
  }
  cVar5 = fn_80102620(uVar6);
  if (cVar5 != '\0') {
    fn_80102568(uVar6,0,0);
  }
  return 1;
}

/* Address: 0x80261D8C | Size: 0xF0 | Ghidra import */
u32 fn_80261D8C(u32 r3, u32 r4)
{
    extern int fn_801F7954();
    extern void fn_801026A4(u32, u32, u32, u32, u32, u32, ...);
    u32 iVar1;
    u16 uVar3;
    u16 sVar4;
    u32 uVar2;
    u32 uVar5;
    u32 local_28;
    u16 local_24;
    u32 local_20;
    u16 local_1c;

    iVar1 = fn_801F02AC(2, r3, r4);
    if (iVar1 == 0) {
        uVar5 = 0;
    } else {
        uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
        sVar4 = fn_801F0134(r3, r4);
        if (sVar4 == 0) {
            uVar5 = 0;
        } else {
            fn_801F0234();
            uVar2 = fn_801F0204();
            if ((int)uVar2 < 0) {
                uVar5 = 0;
            } else {
                uVar5 = fightSideGetStatus(0, uVar3, 2, uVar2 & 0xffff);
            }
        }
    }
    fn_801F7954(r3, &local_28);
    local_20 = local_28;
    local_1c = local_24;
    fn_801026A4(uVar5, 0, 0, 0, 0, 1, &local_20);
    return 1;
}

/* Address: 0x80261EF8 | Size: 0xBC | Ghidra import */
int fn_80261EF8(u32 r3,u32 r4)

{
  u32 iVar1;
  u32 uVar2;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  
  iVar1 = fn_801F02AC(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fn_801F0134(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fn_801F0234();
      uVar3 = fn_801F0204();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
      }
    }
  }
  uVar3 = fn_80102620(uVar2);
  iVar1 = -(uVar3 & 0xff) + 1;
  return iVar1 - ((u32)(iVar1 == 0) + -(uVar3 & 0xff));
}

/* Address: 0x80261FB4 | Size: 0xD0 | Ghidra import */
u32 fn_80261FB4(u32 r3,u32 r4)

{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u8 cVar5;
  u32 uVar6;

  iVar1 = fn_801F02AC(2,r3,r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1,0,5,0);
    sVar4 = fn_801F0134(r3,r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fn_801F0234();
      uVar2 = fn_801F0204();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0,uVar3,3,uVar2 & 0xffff);
      }
    }
  }
  cVar5 = fn_80102620(uVar6);
  if (cVar5 != '\0') {
    fn_80102568(uVar6,0,0);
  }
  return 1;
}

/* Address: 0x80262084 | Size: 0x140 | Ghidra import */
u32 fn_80262084(u32 r3,u32 r4,char *r5)

{
    extern int fn_801FE168();
  u32 *puVar1;
  u32 uVar2;
  u8 cVar7;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 *puVar8;
  u32 *puVar9;
  u8 cVar10;
  u32 uVar11;
  u32 uStack_7c;
  u32 local_78 [10];
  u8 local_4f;
  u32 uStack_4c;
  u32 local_48 [14];
  
  if (r5 == (char *)0x0) {
    cVar10 = '\x01';
  }
  else {
    cVar10 = *r5;
  }
  cVar7 = fn_802062FC(r3);
  if (cVar7 != '\0') {
    iVar3 = fn_801F02AC(2,r3,r4);
    if (iVar3 == 0) {
      uVar11 = 0;
    }
    else {
      uVar5 = fightSideGetStatus(iVar3,0,5,0);
      sVar6 = fn_801F0134(r3,r4);
      if (sVar6 == 0) {
        uVar11 = 0;
      }
      else {
        fn_801F0234();
        uVar4 = fn_801F0204();
        if ((int)uVar4 < 0) {
          uVar11 = 0;
        }
        else {
          uVar11 = fightSideGetStatus(0,uVar5,3,uVar4 & 0xffff);
        }
      }
    }
    fn_801FE168(r3,local_78);
    if (cVar10 == '\0') {
      local_4f = 0;
    }
    puVar9 = &uStack_4c;
    puVar8 = &uStack_7c;
    iVar3 = 6;
    do {
      puVar1 = puVar8 + 1;
      puVar8 = puVar8 + 2;
      uVar2 = *puVar8;
      puVar9[1] = *puVar1;
      puVar9 = puVar9 + 2;
      *puVar9 = uVar2;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    fn_801026A4(uVar11,0xffffffff,0,0,0,1,local_48);
  }
  return 1;
}

/* Address: 0x80264ADC | Size: 0x27C | Ghidra import */
u32 fn_80264ADC(u32 r3,u32 r4,u32 r5)

{
    extern int fn_8001120C();
    extern int fn_80011288();
    extern int fn_80106080();
    extern int fn_80106394();
    extern int fn_801906A0();
    extern int fn_801F000C();
  u32 uVar1;
  s8 cVar6;
  int iVar2;
  u16 uVar4;
  short sVar5;
  u32 uVar3;
  u8 auStack_38 [4];
  u32 local_34;
  u32 local_2c;
  u32 local_24;
  u32 local_1c;
  u8 local_18;
  u8 local_17;
  
LAB_00261af4:
  uVar1 = fn_801F02AC(0xf,r3,r5);
  cVar6 = fn_802062FC();
  if (cVar6 == '\x01') {
    iVar2 = fn_801F02AC(2,uVar1,r5);
    if (iVar2 == 0) {
      local_34 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(iVar2,0,5,0);
      sVar5 = fn_801F0134(uVar1,r5);
      if (sVar5 == 0) {
        local_34 = 0;
      }
      else {
        fn_801F0234();
        uVar3 = fn_801F0204();
        if ((int)uVar3 < 0) {
          local_34 = 0;
        }
        else {
          local_34 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
        }
      }
    }
  }
  else {
    local_34 = 0;
  }
  uVar1 = fn_801F02AC(0x10,r3,r5);
  cVar6 = fn_802062FC();
  if (cVar6 == '\x01') {
    iVar2 = fn_801F02AC(2,uVar1,r5);
    if (iVar2 == 0) {
      local_2c = 0;
    }
    else {
      uVar4 = fightSideGetStatus(iVar2,0,5,0);
      sVar5 = fn_801F0134(uVar1,r5);
      if (sVar5 == 0) {
        local_2c = 0;
      }
      else {
        fn_801F0234();
        uVar3 = fn_801F0204();
        if ((int)uVar3 < 0) {
          local_2c = 0;
        }
        else {
          local_2c = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
        }
      }
    }
  }
  else {
    local_2c = 0;
  }
  local_24 = 0;
  local_1c = 0;
  local_18 = 2;
  local_17 = fn_801F18DC(0);
  iVar2 = fn_80011288((int*)auStack_38,0,1);
  if (iVar2 < 0) {
    fn_8001120C(1);
    return 0;
  }
  if (iVar2 != 0) goto LAB_00261cb0;
  uVar1 = fn_801F02AC(0xf,r3,r5);
  goto LAB_00261ccc;
LAB_00261cb0:
  if (iVar2 == 1) {
    uVar1 = fn_801F02AC(0x10,r3,r5);
LAB_00261ccc:
    cVar6 = fn_802062FC(uVar1);
    if (cVar6 != '\0') {
      fn_80205B8C(uVar1);
      cVar6 = fn_8011FC74();
      if (cVar6 != '\0') {
        fn_8001120C(1);
        return uVar1;
      }
      iVar2 = fn_801906A0(0x99f);
      if (iVar2 == 0) {
        fn_80106394(0x7716,1,1);
      }
      else {
        fn_80106394(0x7702,1,1);
      }
      fn_801F000C(0x40);
      fn_80106080(0);
    }
  }
  goto LAB_00261af4;
}

/* Address: 0x8026503C | Size: 0x2F0 | Ghidra import */
u32 fn_8026503C(u32 r3,u32 r4,u32 r5)

{
    extern u8 lbl_8047B678;
  u32 uVar1;
  s8 cVar6;
  int iVar2;
  u16 uVar4;
  short sVar5;
  u32 uVar3;

  u8 auStack_38 [4];
  u32 local_34;
  u32 local_2c;
  u32 local_24;
  u32 local_1c;
  u8 local_18;
  u8 local_17;
  
LAB_00262054:
  do {
    uVar1 = fn_801F02AC(0xf,r3,r5);
    cVar6 = fn_802062FC();
    if (cVar6 == '\x01') {
      iVar2 = fn_801F02AC(2,uVar1,r5);
      if (iVar2 == 0) {
        local_34 = 0;
      }
      else {
        uVar4 = fightSideGetStatus(iVar2,0,5,0);
        sVar5 = fn_801F0134(uVar1,r5);
        if (sVar5 == 0) {
          local_34 = 0;
        }
        else {
          fn_801F0234();
          uVar3 = fn_801F0204();
          if ((int)uVar3 < 0) {
            local_34 = 0;
          }
          else {
            local_34 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      local_34 = 0;
    }
    uVar1 = fn_801F02AC(0x10,r3,r5);
    cVar6 = fn_802062FC();
    if (cVar6 == '\x01') {
      iVar2 = fn_801F02AC(2,uVar1,r5);
      if (iVar2 == 0) {
        local_2c = 0;
      }
      else {
        uVar4 = fightSideGetStatus(iVar2,0,5,0);
        sVar5 = fn_801F0134(uVar1,r5);
        if (sVar5 == 0) {
          local_2c = 0;
        }
        else {
          fn_801F0234();
          uVar3 = fn_801F0204();
          if ((int)uVar3 < 0) {
            local_2c = 0;
          }
          else {
            local_2c = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      local_2c = 0;
    }
    uVar1 = fn_801F02AC(0xe,r3,r5);
    cVar6 = fn_802062FC();
    if (cVar6 == '\x01') {
      iVar2 = fn_801F02AC(2,uVar1,r5);
      if (iVar2 == 0) {
        local_24 = 0;
      }
      else {
        uVar4 = fightSideGetStatus(iVar2,0,5,0);
        sVar5 = fn_801F0134(uVar1,r5);
        if (sVar5 == 0) {
          local_24 = 0;
        }
        else {
          fn_801F0234();
          uVar3 = fn_801F0204();
          if ((int)uVar3 < 0) {
            local_24 = 0;
          }
          else {
            local_24 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      local_24 = 0;
    }
    local_1c = 0;
    local_18 = 3;
    local_17 = fn_801F18DC(0);
    iVar2 = fn_80011288((int*)auStack_38,0,1);
    lbl_8047B678 = 1;
    if (iVar2 < 0) {
      fn_8001120C(1);
      return 0;
    }
    if (iVar2 == 0) {
      uVar1 = fn_801F02AC(0xf,r3,r5);
    }
    else if (iVar2 == 1) {
      uVar1 = fn_801F02AC(0x10,r3,r5);
    }
    else {
      if (iVar2 != 2) goto LAB_00262054;
      uVar1 = fn_801F02AC(0xe,r3,r5);
    }
    cVar6 = fn_802062FC(uVar1);
    if (cVar6 != '\0') {
      fn_8001120C(1);
      return uVar1;
    }
  } while (1);
}

/* Address: 0x80265754 | Size: 0x174 | Ghidra import */
void fn_80265754(u32 r3,u32 r4)

{
  typedef struct BattleStatusPair {
      u32 unk0;
      u16 unk4;
  } BattleStatusPair;
  extern void fn_801026A4(u32, u32, u32, u32, u32, u32, ...);
  u32 iVar1;
  u32 uVar2;
  u8 cVar6;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  BattleStatusPair local_20;
  BattleStatusPair local_28;
  
  iVar1 = fn_801F02AC(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fn_801F0134(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fn_801F0234();
      uVar3 = fn_801F0204();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
      }
    }
  }
  cVar6 = fn_80102620(uVar2);
  if (cVar6 != '\0') {
    iVar1 = fn_801F02AC(2,r3,r4);
    if (iVar1 == 0) {
      uVar2 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(iVar1,0,5,0);
      sVar5 = fn_801F0134(r3,r4);
      if (sVar5 == 0) {
        uVar2 = 0;
      }
      else {
        fn_801F0234();
        uVar3 = fn_801F0204();
        if ((int)uVar3 < 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
        }
      }
    }
    fn_801F7954(r3,&local_28);
    local_20.unk0 = local_28.unk0;
    local_20.unk4 = local_28.unk4;
    fn_801026A4(uVar2,0,0,0,0,1,&local_20);
  }
  return;
}

/* Address: 0x8026595C | Size: 0x48 | Ghidra import */

double fn_8026595C(void)

{
    extern u32 DAT_80478801;
    extern u32 _DAT_80478804;
    extern u32 _DAT_80478808;
    extern u32 _DAT_8047880c;
    extern f32 lbl_8047E6D8;
    extern f32 lbl_8047E6DC;
    extern f32 lbl_8047E6E8;

  if (DAT_80478801 == '\x01') {
    return (double)lbl_8047E6E8;
  }
  if (_DAT_8047880c == 0) {
    return (double)lbl_8047E6D8;
  }
  return (double)(_DAT_80478808 - _DAT_80478804 / lbl_8047E6DC);
}

/* Address: 0x80265B74 | Size: 0x48 | Ghidra import */

double fn_80265B74(void)

{
    extern u32 DAT_80478811;
    extern u32 _DAT_80478814;
    extern u32 _DAT_80478818;
    extern u32 _DAT_8047881c;
    extern f32 lbl_8047E6D8;
    extern f32 lbl_8047E6DC;
    extern f32 lbl_8047E6E8;

  if (DAT_80478811 == '\x01') {
    return (double)lbl_8047E6E8;
  }
  if (_DAT_8047881c == 0) {
    return (double)lbl_8047E6D8;
  }
  return (double)(_DAT_80478818 - _DAT_80478814 / lbl_8047E6DC);
}

/* Address: 0x80265E34 | Size: 0x48 | Ghidra import */
u32 fn_80265E34(u32 r3)

{
    extern int fn_8011D5D4();
  int iVar1;
  
  fn_8011D5D4(r3,1);
  iVar1 = fn_80129280(0,0x10);
  if (*(char *)(iVar1 + 5) == '\0') {
    *(u8 *)(iVar1 + 5) = 0x2d;
  }
  return 1;
}

/* Address: 0x80265E7C | Size: 0x48 | Ghidra import */
u32 fn_80265E7C(u32 r3)

{
    extern int fn_8011D5F8();
  int iVar1;
  
  fn_8011D5F8(r3,1);
  iVar1 = fn_80129280(0,0x10);
  if (*(char *)(iVar1 + 4) == '\0') {
    *(u8 *)(iVar1 + 4) = 0x2c;
  }
  return 1;
}

/* Address: 0x80265F94 | Size: 0x2BC | Ghidra import */
void fn_80265F94(int r3)

{
    extern s8 fn_80106934();
    extern u16 fn_8011E15C();
    extern int fn_8011E778();
    extern int fn_801666BC();
    extern int fn_80166A28();
    extern u16 lbl_8047E6F8;
    extern f32 lbl_8047E6FC;
  u16 uVar1;
  u32 uVar2;

  short sVar6;
  short sVar7;
  u32 uVar3;
  s8 cVar9;
  int iVar4;
  u16 uVar8;
  u32 uVar5;
  
  sVar6 = (int)fn_8012640C(0,0xfa,0x66,0);
  sVar7 = -1;
  if (sVar6 != 0) {
    sVar7 = sVar6;
  }
  uVar3 = fn_801DE190(sVar7,0,0);
  fn_801DDD28(uVar3,lbl_8047E6F8,4,0);
  fn_801DA4E8(uVar3,1);
  uVar1 = lbl_8047E6F8;
  fn_801DA9E8(uVar3,uVar1,4);
  fn_801C41C8((double)lbl_8047E6FC,2);
  while (cVar9 = fn_801C40F0(0), cVar9 != '\0') {
    fn_801DB088();
    cVar9 = fn_801DA94C(uVar3,uVar1,4);
    if (cVar9 == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  iVar4 = fn_8011E778(0xfa);
  if (iVar4 == 0) {
    uVar8 = 0;
  }
  else {
    uVar8 = fn_8011E15C();
    fn_80166A28(uVar8);
  }
  while (iVar4 = fn_801666BC(uVar8), iVar4 == 2) {
    fn_801DB088();
    cVar9 = fn_801DA94C(uVar3,uVar1,4);
    if (cVar9 == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  fn_801067E8(0x44ba,0,0);
  while (1) {
    cVar9 = fn_80106934();
    uVar2 = __cntlzw(1 - cVar9);
    if ((uVar2 >> 5 & 0xff) == 0) break;
    fn_801DB088();
    cVar9 = fn_801DA94C(uVar3,uVar1,4);
    if (cVar9 == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  fn_801065B8(1);
  if (r3 == 1) {
    fn_80132A38(0x5d,0x3d2);
    uVar5 = 0x44bb;
  }
  else if ((r3 < 1) && (-1 < r3)) {
    fn_80132A38(0x5d,0x3d2);
    uVar5 = 0x44bc;
  }
  else {
    uVar5 = 0x44b9;
  }
  fn_801067E8(uVar5,0,0);
  while (1) {
    cVar9 = fn_80106934();
    uVar2 = __cntlzw(1 - cVar9);
    if ((uVar2 >> 5 & 0xff) == 0) break;
    fn_801DB088();
    cVar9 = fn_801DA94C(uVar3,uVar1,4);
    if (cVar9 == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  fn_801065B8(1);
  fn_801C41C8((double)lbl_8047E6FC,3);
  while (cVar9 = fn_801C40F0(0), cVar9 != '\0') {
    fn_801DB088();
    cVar9 = fn_801DA94C(uVar3,uVar1,4);
    if (cVar9 == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  return;
}

/* Address: 0x80266250 | Size: 0xD0 | Ghidra import */
void fn_80266250(void)
{
    extern int fn_800FF660();
    extern u32 lbl_8047B680;
    extern int fn_801653BC();
    extern int fn_801653C4();
    extern u32 fn_801656D8();
    extern void fn_80165A20();
    extern void fn_801657D0();
    extern void fn_801659FC();
    extern void fn_801DADC0();
    extern void fn_801DAC90();
    extern void fn_80265F94();
    u32 iVar1;
    u32 uVar3;
    u32 iVar2;
    u32 uVar4;
    u32 uVar5;

    uVar5 = lbl_8047B680;
    iVar1 = fn_801653C4();
    if (iVar1 != 0) {
        uVar3 = fn_801656D8();
        fn_80165A20(1, 0x32, 0xff);
    } else {
        uVar3 = 0;
    }
    iVar2 = fn_801653BC();
    if (iVar2 != 0) {
        uVar4 = fn_801656D8();
        fn_801657D0(0x32);
    } else {
        uVar4 = 0;
    }
    fn_801DADC0(1);
    fn_80265F94(uVar5);
    fn_801DAC90();
    if (iVar1 != 0) {
        fn_80165A20(iVar1, 0x32, uVar3);
    }
    if (iVar2 != 0) {
        fn_801659FC(iVar2, 0x32, uVar4);
    }
    fn_800FF660();
    fn_8011288C(0, 0x5960008);
}
/* Address: 0x8025E3B0 | Size: 0x184 | Ghidra import */

int fn_8025E3B0(u32 r3,u32 r4)

{
    extern u32 _DAT_80478288;
    extern u32 _DAT_8047828c;
    extern u32 _DAT_80478290;
    extern u32 _DAT_80478294;
    extern u32 _DAT_80478298;
    extern u32 _DAT_8047829c;
    extern u32 _DAT_804782a0;
    extern u32 _DAT_804782a4;
    extern u32 _DAT_804782a8;
    extern u32 _DAT_804782ac;
    extern u32 _DAT_804782b0;
    extern u32 _DAT_804782b4;
    extern u32 _DAT_804782b8;
    extern u32 lbl_80478DB0;
    extern f32 lbl_8047E680;

  u32 uVar1;
  int iVar2;
  int iVar3;

  u8 auStack_58 [4];
  u32 local_54;
  u8 auStack_50 [4];
  u8 auStack_4c [4];
  u8 auStack_48 [4];
  u32 local_44;
  u32 local_40;
  u32 local_3c;
  u32 local_38;
  u32 local_34;
  u32 local_30;
  u32 local_2c;
  u32 local_28;
  u32 local_24;
  u32 local_20;
  u32 local_1c;
  u32 local_18;
  
  fn_801C41C8((double)lbl_8047E680,3);
  fn_801C40F0(1);
  GSscene_GetCameraDirectionVector(&local_44);
  GSscene_GetCameraRotationVector(&local_38);
  GSscene_GetCameraPositionVector(&local_2c);
  GSscene_GetCameraViewVector(&local_20);
  uVar1 = GScameraGetActiveCamera();
  GScameraGetPerspective(uVar1,&local_54,auStack_50,auStack_4c,auStack_48);
  _DAT_80478288 = local_44;
  _DAT_8047828c = local_40;
  _DAT_80478290 = local_3c;
  _DAT_80478294 = local_38;
  _DAT_80478298 = local_34;
  _DAT_8047829c = local_30;
  _DAT_804782a0 = local_2c;
  _DAT_804782a4 = local_28;
  _DAT_804782a8 = local_24;
  _DAT_804782ac = local_20;
  _DAT_804782b0 = local_1c;
  _DAT_804782b4 = local_18;
  _DAT_804782b8 = local_54;
  fn_801DAC90();
  iVar2 = fn_80097A38(r3,r4);
  if (3 < iVar2) {
    iVar2 = -1;
  }
  fn_8012640C(r3,0,0x6e,0);
  fn_801DADC0(1);
  iVar3 = fn_8025E0B0((int*)auStack_58,r3,(u16*)lbl_80478DB0,1);
  if (iVar3 == 1) {
    fn_801C41C8((double)lbl_8047E680,2);
    fn_8025DE54((u32*)auStack_58,(u16*)lbl_80478DB0,1,1,1,0);
  }
  return (int)(char)iVar2;
}
/* Address: 0x8025E9BC | Size: 0x390 | Ghidra import */
void fn_8025E9BC(u32 r3)

{
    extern s8 fn_8001E184();
    extern int fn_80029660();
    extern int fn_8011D904();
    extern int fn_8011DE68();
    extern u16 fn_8011EDF8();
    extern u32 fn_8011EE10();
    extern u16 fn_8011EE58();
    extern u16 fn_8011F228();
    extern u32 fn_8011F4F0();
    extern int fn_8011FBCC();
    extern u8 fn_80121ADC();
    extern int fn_80121B4C();
    extern int fn_8012805C();
    extern u32 fn_80128A64();
    extern u8 fn_80123FBC();
    extern int fn_801EECD8();
    extern u32 lbl_80478DB0;
    extern f32 lbl_8047E680;
    extern f32 lbl_8047E688;

  u32 uVar1;
  s8 cVar7;
  u8 uVar8;
  u32 uVar2;
  u16 sVar5;
  u16 sVar6;
  u16 sVar7;
  int iVar3;
  u32 uVar4;

  u8 auStack_18 [8];
  u8 auStack_1c [4];
  u8 auStack_20 [4];
  u8 auStack_24 [4];
  u16 local_28 [2];

  local_28[0] = 0;
  uVar1 = fn_8012A5B0(0,3,r3 & 0xffff);
  uVar8 = fn_80123FBC();
  if (uVar8 == 1) {
    fn_8012640C(uVar1,0,0x6e,0);
    uVar2 = fn_8011EE40(uVar1);
    fn_801EECD8(uVar2,1);
    uVar8 = fn_80121ADC(uVar1,0x3e);
    if (uVar8 != 0) {
      fn_80121B4C(uVar1,0x3e);
    }
    fn_8011FBCC((double)lbl_8047E688,uVar1);
    fn_80165668(0x3f7,0,0xff);
    sVar5 = fn_8011F228(uVar1,0);
    if (sVar5 != 0) {
      uVar2 = fn_8011F4F0(uVar1);
      fn_80132A38(0x32,uVar2);
      fn_80132A38(0x39,sVar5);
      fn_801067E8(0x3b10,1,0);
    }
    uVar2 = fn_8011EE10(uVar1);
    fn_80132A38(0x2f,uVar2);
    fn_801067E8(0x3b0b,1,0);
    fn_801065B8(1);
    sVar7 = fn_8011EE58(uVar1);
    sVar6 = fn_8011EDF8(uVar1);
    fn_8011D904(uVar1,(u16)((sVar6 + 0x46) + sVar7));
    fn_8011DE68(uVar1,0);
    iVar3 = fn_8025E640(uVar1,uVar2);
    if (iVar3 == 1) {
      uVar4 = fn_80128A64(uVar1,0,0,local_28,auStack_18);
      if (((uVar4 & 0xffff) != 0) && ((uVar4 & 0xffff) != 0xffff)) {
        fn_801C41C8((double)lbl_8047E680,3);
        fn_801C40F0(1);
        fn_801DAC90();
        iVar3 = fn_8012805C(uVar1,uVar4,local_28[0],auStack_18,0,1,1,0);
        if (iVar3 == 0) {
          fn_801DADC0(1);
          iVar3 = fn_8025E0B0((int*)auStack_1c,uVar1,(u16*)&lbl_80478DB0,1);
          if (iVar3 == 1) {
            fn_801C41C8((double)lbl_8047E680,2);
            fn_8025DE54((u32*)auStack_1c,(u16*)&lbl_80478DB0,1,1,1,0);
          }
          uVar2 = fn_8011F4F0(uVar1);
          fn_80132A38(0x32,uVar2);
        }
        else {
          uVar2 = fn_8011F4F0(uVar1);
          fn_80132A38(0x32,uVar2);
          fn_801DADC0(1);
          iVar3 = fn_8025E0B0((int*)auStack_20,uVar1,(u16*)&lbl_80478DB0,1);
          if (iVar3 == 1) {
            fn_801C41C8((double)lbl_8047E680,2);
            fn_8025DE54((u32*)auStack_20,(u16*)&lbl_80478DB0,1,1,1,0);
          }
        }
      }
    }
    fn_80165668(0x3ca,0,0xff);
    fn_80265E7C(uVar1);
    fn_801067E8(0x3b0c,1,0);
    fn_801067E8(0x3b0d,1,1);
    cVar7 = fn_8001E184();
    if (cVar7 == '\0') {
      fn_801065B8(1);
      fn_801C41C8((double)lbl_8047E680,3);
      fn_801C40F0(1);
      fn_801DAC90();
      fn_80029660(2,r3);
      fn_801DADC0(1);
      iVar3 = fn_8025E0B0((int*)auStack_24,uVar1,(u16*)&lbl_80478DB0,1);
      if (iVar3 == 1) {
        fn_801C41C8((double)lbl_8047E680,2);
        fn_8025DE54((u32*)auStack_24,(u16*)&lbl_80478DB0,1,1,1,0);
      }
    }
    else {
      fn_801065B8(1);
    }
  }
  return;
}

/* Address: 0x8025ED4C | Ghidra import */

void fn_8025ED4C(void)

{
    extern u32 _DAT_804782bc;
    extern u32 _DAT_804782c0;
    extern u32 _DAT_804782c4;
    extern u32 lbl_80478DA8;
    extern u32 lbl_8047B668;
    extern u32 lbl_8047B66C;
    extern f32 lbl_8047E680;
  BOOL bVar1;

  u32 uVar2;
  s8 cVar4;
  int iVar3;

  u8 auStack_18 [16];
  
  if ((_DAT_804782bc == 0) || (_DAT_804782bc == 1)) {
    fn_801DADC0(1);
    if (_DAT_804782bc == 0) {
      uVar2 = fn_8012A5B0(0,3,_DAT_804782c4 & 0xffff);
      cVar4 = fn_80123FBC();
      if (cVar4 == '\x01') {
        fn_8012640C(uVar2,0,0x6e,0);
        iVar3 = fn_8025E0B0((int*)auStack_18,uVar2,(u16*)lbl_80478DA8,2);
        if (iVar3 == 1) {
          fn_8025DE54((u32*)auStack_18,(u16*)lbl_80478DA8,2,1,1,0);
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
      else {
        bVar1 = 0;
      }
      if (bVar1) {
        fn_8025E9BC(_DAT_804782c4);
      }
    }
    else if (_DAT_804782bc == 1) {
      uVar2 = fn_8012A5B0(0,3,_DAT_804782c4 & 0xffff);
      cVar4 = fn_80123FBC();
      if (cVar4 == '\x01') {
        fn_801254B4(uVar2,0,0xc5,0,0);
        fn_8012640C(uVar2,0,0x6e,0);
        iVar3 = fn_8025E0B0((int*)auStack_18,uVar2,(u16*)0x8039a6a8,4);
        if (iVar3 == 1) {
          fn_8025DE54((u32*)auStack_18,(u16*)0x8039a6a8,4,1,1,0);
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
      else {
        bVar1 = 0;
      }
      if (bVar1) {
        fn_8025E9BC(_DAT_804782c4);
      }
    }
    fn_801C41C8((double)lbl_8047E680,3);
    fn_801C40F0(1);
    fn_801DAC90();
    _DAT_804782bc = -1;
    lbl_8047B668 = 0xffffffff;
    lbl_8047B66C = 0xffffffff;
    _DAT_804782c0 = 0;
    _DAT_804782c4 = 0;
    fn_800FF660();
    fn_8011288C(0,0);
    _threadSwitch();
  }
  return;
}

/* Address: 0x80264D58 | Size: 0x2E4 | Ghidra import */
u32 fn_80264D58(u32 r3,u32 r4,u32 r5)

{
    extern int menuFightCloseWaza();
    extern u32 fn_800117BC();
    extern int fn_801F4C14();
    extern int fn_801FE3F8();
    extern s8 fn_801FFEC8();
    extern u16 fn_802040E8();
    extern int fn_80204F6C();
    extern int fn_8022B2CC();
    extern u32 lbl_80478DF8;
    extern u8 lbl_8047B678;
    u32 saved_r27 = 0;
  u8 bVar8;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar9;
  u16 uVar7;
  u32 uVar4;
  int iVar5;
  int iVar6;

  u16 local_70 [2];
  u8 auStack_6c [68];
  u8 local_28;
  
  fn_8020E204(r5);
  bVar8 = fn_8020E1A4();
  fn_801FE3F8(r4,auStack_6c);
  local_28 = fn_801F18DC(0);
  uVar1 = fn_80205B8C(r4);
  do {
    do {
      while (1) {
        uVar2 = (int)fn_8012640C(r4,0,0x101,0);
        uVar3 = fn_800117BC((int*)auStack_6c,uVar2,1);
        if ((int)uVar3 < 0) {
          menuFightCloseWaza(1);
          return 0;
        }
        cVar9 = fn_801FFEC8(r4,uVar3 & 0xffff,1,local_70);
        uVar7 = (int)fn_8012640C(uVar1,0,0x7f,uVar3 & 0xffff);
        if (cVar9 != '\0') {
          fn_80132A38(0x11,r4);
          fn_8011BEB4(0,uVar7,1,0);
          uVar2 = fn_800FA280();
          fn_80132A38(0x28,uVar2);
          uVar7 = fn_802040E8(r4);
          fn_801F4C14(0,0,0x56,0,uVar7);
        }
        if (cVar9 == '\x06') {
          saved_r27 = 0x7661;
        }
        else if (cVar9 == '\x05') {
          fn_8011BEB4(0,local_70[0],1,0);
          uVar2 = fn_800FA280();
          fn_80132A38(0x28,uVar2);
          saved_r27 = 0x76bb;
        }
        else if (cVar9 == '\x04') {
          saved_r27 = 0x7600;
        }
        else if (cVar9 == '\x03') {
          saved_r27 = 0x75ff;
        }
        else if (cVar9 == '\x02') {
          saved_r27 = 0x75fe;
        }
        else if (cVar9 == '\x01') {
          saved_r27 = 0x75fd;
        }
        if (cVar9 == '\0') break;
        if (saved_r27 != 0) {
          fn_80106394(saved_r27,1,1);
        }
        fn_801F000C(0x40);
        fn_80106080(0);
      }
      uVar4 = (int)fn_8012640C(uVar1,0,0x7f,uVar3 & 0xffff);
      uVar4 = uVar4 & 0xffff;
    } while (((uVar4 == 0) || (lbl_80478DF8 <= uVar4)) || (uVar4 == 0x165));
    lbl_8047B678 = 0;
    iVar5 = fn_8022B2CC(r4,uVar4,r5,0x8026503c,1,0,0xffffffff);
    cVar9 = fn_801F18DC(0);
  } while ((((cVar9 == '\x01') && (lbl_8047B678 == '\0')) &&
           ((1 < bVar8 && (iVar6 = fn_8026503C(r4,uVar4,r5), iVar6 == 0)))) ||
          (iVar5 == 0));
  uVar1 = fn_801F0134(iVar5,r5);
  menuFightCloseWaza(1);
  fn_80204F6C(r4,0,0x13,0,0x80375ca8,uVar4,uVar1,(int)(char)uVar3,0);
  return 1;
}

/* Address: 0x802612D0 | Size: 0x5C | Ghidra import */

void fn_802612D0(void)
{
    extern u32 lbl_804787E0[];
    extern u32 fn_80260EBC();
    extern void fn_800FF660();
    extern void fn_8011288C();
    u32 *base = lbl_804787E0;
    base[6] = fn_80260EBC(base[0], base[1], base[2], (u16*)base[4], base[3], (u8*)base[5]);
    fn_800FF660();
    fn_8011288C(0, 0);
}

/* Address: 0x802600E4 | Size: 0x378 | Ghidra import */
u32
fn_802600E4(u32 r3,u32 r4,u8 *r5,int r6,void *r7,
            u32 r8)

{
    extern s8 fn_8001E074();
    extern int fn_801069FC();
    extern int fn_80106D3C();
  int iVar1;
  short sVar3;
  u32 uVar2;
  s8 cVar5;
  u16 uVar4;
  u32 uVar6;
  
  uVar6 = 0;
  do {
    sVar3 = fn_8011F228(r3,uVar6 & 0xffff);
    if (sVar3 == 0) break;
    uVar6 = uVar6 + 1;
  } while ((int)uVar6 < 4);
  if ((int)uVar6 < 4) {
LAB_0025d3c4:
    fn_80165668(0x4ca,0,0xff);
    uVar2 = fn_8011F4F0(r3);
    fn_80132A38(0x32,uVar2);
    fn_80132A38(0x39,r4 & 0xffff);
    if (r6 == 0) {
      fn_801067E8(0x423d,1,0);
      fn_801065B8(1);
    }
    else {
      fn_80106D3C(2,0x423d,1,0);
      fn_801069FC(1);
    }
    *r5 = (char)uVar6;
    uVar2 = 1;
  }
  else {
    uVar2 = fn_8011F4F0(r3);
    fn_80132A38(0x32,uVar2);
    fn_80132A38(0x39,r4 & 0xffff);
    do {
      if (r6 == 0) {
        fn_801067E8(0x4243,1,0);
        cVar5 = fn_8001E184();
        fn_801065B8(1);
      }
      else {
        fn_80106D3C(2,0x4243,1,0);
        cVar5 = fn_8001E074(0,0xffffffff,0xffffffff,0);
        fn_801069FC(1);
      }
      if (cVar5 == '\x01') {
        iVar1 = 1;
      }
      else if ((cVar5 < '\x01') && (-1 < cVar5)) {
        iVar1 = 0;
      }
      else {
        iVar1 = 2;
      }
      if (iVar1 == 0) {
        if (r7 == (void *)0x0) {
          uVar6 = 0;
        }
        else {
          cVar5 = ((s8 (*)())r7)(r3,r4,r8);
          uVar6 = (u32)cVar5;
        }
        if (-1 < (int)uVar6) {
          uVar2 = fn_8011F4F0(r3);
          fn_80132A38(0x32,uVar2);
          fn_80132A38(0x5d,0x468);
          uVar4 = fn_8011F228(r3,uVar6 & 0xffff);
          fn_80132A38(0x39,uVar4);
          if (r6 == 0) {
            fn_801067E8(0x4248,1,0);
          }
          else {
            fn_80106D3C(2,0x4248,1,0);
          }
          goto LAB_0025d3c4;
        }
      }
      fn_80132A38(0x32,uVar2);
      fn_80132A38(0x39,r4 & 0xffff);
      if (r6 == 0) {
        fn_801067E8(0x4242,1,0);
        cVar5 = fn_8001E184();
        fn_801065B8(1);
      }
      else {
        fn_80106D3C(2,0x4242,1,0);
        cVar5 = fn_8001E074(0,0xffffffff,0xffffffff,0);
        fn_801069FC(1);
      }
      if (cVar5 == '\x01') {
        iVar1 = 1;
      }
      else if ((cVar5 < '\x01') && (-1 < cVar5)) {
        iVar1 = 0;
      }
      else {
        iVar1 = 2;
      }
    } while (iVar1 != 0);
    if (r6 == 0) {
      fn_801067E8(0x4241,1,0);
      fn_801065B8(1);
    }
    else {
      fn_80106D3C(2,0x4241,1,0);
      fn_801069FC(1);
    }
    uVar2 = 0;
  }
  return uVar2;
}

/* Address: 0x8025DC2C | Size: 0x90 | Ghidra import (PSQ removed) */


void fn_8025DC2C(double r3)

{
    extern f32 lbl_8047E660;
    extern f32 lbl_8047E664;
    extern f64 lbl_8047E668;
  float fVar1;

  u32 uVar2;

  u64 f29;
  double dVar3;
  u64 f30;
  double dVar4;
  u64 f31;
  double dVar5;
  u8 auStack_28 [16];
  u8 auStack_18 [16];
  u8 auStack_8 [8];

  dVar4 = (double)(float)((double)lbl_8047E664 * r3);
  dVar5 = lbl_8047E668;
  fVar1 = lbl_8047E660;
  while (dVar3 = (double)fVar1, dVar3 < dVar4) {
    _threadSwitch();
    uVar2 = fn_800D3088();
    fVar1 = (float)(dVar3 + (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uVar2)) - dVar5));
  }

  return;
}

/* Address: 0x80260910 | Size: 0x5AC | Ghidra import (PSQ removed) */


u32 fn_80260910(u32 *r3,int r4,int r5,int r6)

{
    extern u32 DAT_8027a488;
    extern u32 DAT_8027a48c;
    extern u32 _DAT_8027a488;
    extern u32 _DAT_8027a48c;
    extern u32 _DAT_8027a4b0;
    extern u32 _DAT_8027a4b8;
    extern f32 lbl_8047E6B0;
    extern f32 lbl_8047E6B4;
    extern f64 lbl_8047E6B8;
    extern f32 lbl_8047E6C0;
    extern f32 lbl_8047E6C4;
    u32 saved_r29 = 0;
  float fVar1;
  u16 uVar2;
  BOOL bVar3;
  BOOL bVar4;

  u32 uVar5;
  int iVar6;
  u32 uVar7;
  u32 uVar8;
  s8 cVar10;
  u16 uVar9;
  u32 uVar11;
  u32 uVar12;

  u64 f29;
  double dVar13;
  u64 f30;
  double dVar14;
  u64 f31;
  double dVar15;
  u8 auStack_28 [16];
  u8 auStack_18 [16];
  u8 auStack_8 [8];

  fn_801DA4E8(*r3,1);
  uVar5 = fn_8011F4F0(r5);
  fn_80132A38(0x32,uVar5);
  fn_801067E8(0x4401,1,0);
  if (r5 == 0) {
    iVar6 = 0;
  }
  else {
    cVar10 = fn_80123FBC(r5);
    if (cVar10 == '\0') {
      iVar6 = 0;
    }
    else {
      fn_8011F5C8(r5);
      iVar6 = fn_8011E778();
    }
  }
  if (iVar6 == 0) {
    uVar9 = 0;
  }
  else {
    uVar9 = fn_8011E15C();
    fn_80166A28(uVar9);
  }
  if (_DAT_8027a48c == 0) {
    uVar5 = *r3;
  }
  else {
    uVar5 = r3[1];
  }
  fn_801DA9E8(uVar5,_DAT_8027a488,4);
  fn_801DB088();
  r3[2] = 0;
  bVar3 = 1;
  bVar4 = 0;
  uVar11 = 0;
  uVar12 = 0;
  do {
    if (0x239 < uVar12) {
LAB_0025db60:
      fn_801C41C8((double)lbl_8047E6C0,5);
      while (cVar10 = fn_801C40F0(0), cVar10 != '\0') {
        if (*(int *)(&DAT_8027a48c + r3[2] * 8) == 0) {
          uVar5 = *r3;
        }
        else {
          uVar5 = r3[1];
        }
        uVar9 = *(u16 *)(&DAT_8027a488 + r3[2] * 8);
        fn_801DB088();
        fn_801DA94C(uVar5,uVar9,4);
        _threadSwitch();
      }
      if (*(int *)(&DAT_8027a48c + r3[2] * 8) == 0) {
        uVar5 = *r3;
      }
      else {
        uVar5 = r3[1];
      }
      fn_801DA8C4(uVar5,*(u16 *)(&DAT_8027a488 + r3[2] * 8),4);
      dVar15 = lbl_8047E6B8;
      dVar14 = (double)lbl_8047E6B4;
      fVar1 = lbl_8047E6B0;
      while (dVar13 = (double)fVar1, dVar13 < dVar14) {
        _threadSwitch();
        uVar5 = fn_800D3088();
        fVar1 = (float)(dVar13 + (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uVar5)) - dVar15));
      }
      if (bVar4) {
        fn_801065B8(1);
        soundStop(0x3d4,0x32);
        iVar6 = 0;
        if ((_DAT_8027a4b0 < uVar12) && (iVar6 = 1, _DAT_8027a4b8 < uVar12)) {
          iVar6 = 2;
        }
        fn_802606D8(r3,*(u32 *)(iVar6 * 8 + -0x7fd85b4c),0,8);
        if (r5 == 0) {
          iVar6 = 0;
        }
        else {
          cVar10 = fn_80123FBC(r5);
          if (cVar10 == '\0') {
            iVar6 = 0;
          }
          else {
            fn_8011F5C8(r5);
            iVar6 = fn_8011E778();
          }
        }
        if (iVar6 == 0) {
          uVar9 = 0;
        }
        else {
          uVar9 = fn_8011E15C();
          fn_80166A28(uVar9);
        }
        while (iVar6 = fn_801666BC(uVar9), iVar6 == 2) {
          _threadSwitch();
        }
        uVar5 = fn_8011F4F0(r5);
        fn_80132A38(0x32,uVar5);
        fn_801067E8(0x43ff,1,0);
        fn_801065B8(1);
        fn_801C41C8((double)lbl_8047E6C0,3);
        fn_801C40F0(1);
        uVar5 = 0;
      }
      else {
        fn_801DA4E8(*r3,0);
        fn_801DA4E8(r3[1],1);
        fn_802606D8(r3,1,0,8);
        fn_801065B8(1);
        soundStop(0x3d4,0x32);
        if (r6 == 0) {
          iVar6 = 0;
        }
        else {
          cVar10 = fn_80123FBC(r6);
          if (cVar10 == '\0') {
            iVar6 = 0;
          }
          else {
            fn_8011F5C8(r6);
            iVar6 = fn_8011E778();
          }
        }
        if (iVar6 == 0) {
          uVar9 = 0;
        }
        else {
          uVar9 = fn_8011E15C();
          fn_80166A28(uVar9);
        }
        while (iVar6 = fn_801666BC(uVar9), iVar6 == 2) {
          _threadSwitch();
        }
        uVar5 = fn_8011F4F0(r5);
        fn_80132A38(0x32,uVar5);
        uVar9 = fn_8011F5C8(r6);
        fn_80132A38(0x4e,uVar9);
        fn_80132A38(0x5d,0x3d2);
        fn_801067E8(0x4400,1,0);
        fn_801065B8(1);
        dVar14 = lbl_8047E6B8;
        dVar15 = (double)lbl_8047E6C4;
        fVar1 = lbl_8047E6B0;
        while (dVar13 = (double)fVar1, dVar13 < dVar15) {
          _threadSwitch();
          uVar5 = fn_800D3088();
          fVar1 = (float)(dVar13 + (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uVar5)) - dVar14));
        }
        uVar5 = 1;
      }

      return uVar5;
    }
    if (((r4 != 0) && (0x77 < uVar12)) && (1 < uVar11)) {
      uVar7 = fn_800F7AF0(1);
      uVar8 = fn_800F7BC4(1);
      if ((uVar8 & uVar7 & 0x200) != 0) {
        bVar4 = 1;
        goto LAB_0025db60;
      }
    }
    if (*(int *)(&DAT_8027a48c + r3[2] * 8) == 0) {
      uVar5 = *r3;
    }
    else {
      uVar5 = r3[1];
    }
    uVar2 = *(u16 *)(&DAT_8027a488 + r3[2] * 8);
    fn_801DB088();
    uVar7 = fn_801DA94C(uVar5,uVar2,4);
    if (-1 < (int)-(uVar7 & 0xff)) goto LAB_0025db60;
    if (uVar11 == 1) {
      iVar6 = fn_801666BC(0x3d3);
      if (iVar6 != 2) {
        iVar6 = fn_800D3088();
        saved_r29 = saved_r29 + iVar6;
        if (0x1d < saved_r29) {
          fn_80165A20(0x3d4,0,0xff);
          uVar11 = 2;
        }
      }
    }
    else if ((uVar11 == 0) && (iVar6 = fn_801666BC(uVar9), iVar6 != 2)) {
      fn_80165A20(0x3d3,0,0xff);
      saved_r29 = 0;
      uVar11 = 1;
    }
    if (bVar3) {
      fn_801C41C8((double)lbl_8047E6C0,2);
      bVar3 = 0;
    }
    _threadSwitch();
    iVar6 = fn_800D3088();
    uVar12 = uVar12 + iVar6;
  } while (1);
}

/* Address: 0x80264488 | Size: 0x654 | Ghidra import (PSQ removed) */


u32 fn_80264488(u32 r3,int r4,u32 r5)

{
    extern s8 fn_8000DD0C();
    extern int fn_8000DD30();
    extern int fn_8000DD5C();
    extern s8 fn_8000DD98();
    extern int fn_8000DDBC();
    extern int fn_8000DDE8();
    extern u32 fn_80018F88();
    extern u8 fn_80019064();
    extern u32 fn_800D37CC();
    extern int fn_80102038();
    extern int fn_8010206C();
    extern s8 fn_80142CF4();
    extern int fn_801DA36C();
    extern int fn_801EFFC4();
    extern s8 fn_801F1758();
    extern s8 fn_801F7EF0();
    extern u16 fn_801F85B0();
    extern short fn_801F8638();
    extern int fn_8022FF90();
    extern f32 lbl_8047E6CC;
    extern f64 lbl_8047E6D0;

  int iVar1;
  u32 uVar2;
  short sVar4;
  s8 cVar6;
  u8 uVar7;
  int iVar3;
  u16 uVar5;
  u16 uVar8;
  u32 uVar9;
  u32 uVar10;

  u64 f30;
  double dVar11;
  u64 f31;
  double dVar12;
  u8 local_80;
  char local_7f [3];
  char local_7c [4];
  char local_78 [4];
  char local_74 [4];
  u32 local_70 [2];
  u32 local_68;
  u32 uStack_64;
  u8 auStack_18 [16];
  u8 auStack_8 [8];

  local_70[0] = 0;
  for (uVar8 = 0; uVar8 < 2; uVar8 = uVar8 + 1) {
    local_74[uVar8] = '\0';
    local_78[uVar8] = '\0';
    local_7c[uVar8] = '\0';
  }
  for (uVar9 = 0; (uVar9 & 0xffff) < 2; uVar9 = uVar9 + 1) {
    iVar1 = fn_801FB1C0(r3,0,0x46,uVar9);
    if (iVar1 != 0) {
      uVar5 = fn_801F85B0(r3,iVar1);
      fn_801FB1C0(r3,0,0x45,uVar5);
      cVar6 = fn_80206608();
      uVar10 = uVar9 & 0xffff;
      local_74[uVar10] = cVar6;
      cVar6 = fn_802026E4(iVar1,8);
      local_78[uVar10] = cVar6;
      cVar6 = fn_802026E4(iVar1,7);
      local_7c[uVar10] = cVar6;
    }
  }
  iVar1 = fn_801FB1C0(r3,0,0x44,0);
  if (iVar1 != 0) {
    uVar2 = (int)fn_8012640C(r4,0,0xd6,0);
    sVar4 = fn_801F8638(r3,uVar2);
    local_70[0] = (u32)sVar4;
    dVar11 = lbl_8047E6D0;
    dVar12 = (double)lbl_8047E6CC;
LAB_0026160c:
    fn_801F2B5C(0,0x80261cbc,0,0);
    fn_801F37B0(0,0x80261fb4,0,0);
    cVar6 = fn_801F1700(0);
    if (cVar6 == '\x01') {
      fn_8000DDBC();
    }
    cVar6 = fn_801F1758(0);
    if (cVar6 == '\x01') {
      fn_8000DD30();
    }
    fn_801F2B5C(0,0x80261cbc,0,0);
    while (1) {
      local_7f[0] = '\x01';
      fn_801F2B5C(0,0x80261bec,local_7f,0);
      if (local_7f[0] == '\x01') break;
      _threadSwitch();
    }
    fn_801F37B0(0,0x80261fb4,0,0);
    while (cVar6 = fn_801F37B0(0,0x80261ef8,0,0), cVar6 != '\x01') {
      _threadSwitch();
    }
    cVar6 = fn_801F1700(0);
    if (cVar6 == '\x01') {
      while (cVar6 = fn_8000DD98(), cVar6 != '\0') {
        _threadSwitch();
      }
    }
    cVar6 = fn_801F1758(0);
    if (cVar6 == '\x01') {
      while (cVar6 = fn_8000DD0C(), cVar6 != '\0') {
        _threadSwitch();
      }
    }
    uStack_64 = fn_800D37CC();
    uStack_64 = uStack_64 ^ 0x80000000;
    local_68 = 0x43300000;
    fn_8010206C((double)(float)(dVar12 / (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uStack_64)) -
                                                         dVar11)));
    uVar9 = fn_80018F88(1,local_70,r3);
    fn_801EFFC4(10);
    uVar7 = fn_80019064();
    if (((uVar9 & 0xffff) != 0) && (cVar6 = fn_80142CF4(0,uVar9,2,0), cVar6 == '\x02')) {
      for (uVar10 = 0; (uVar10 & 0xffff) < 2; uVar10 = uVar10 + 1) {
        iVar1 = fn_801FB1C0(r3,0,0x46,uVar10);
        if ((iVar1 != 0) && (iVar3 = (int)fn_8012640C(iVar1,0,0xee,0), iVar3 != 0)) {
          if ((local_78[uVar10 & 0xffff] == '\x01') &&
             (cVar6 = fn_802026E4(iVar1,8), cVar6 == '\0')) {
            fn_801DA36C(iVar3,1);
          }
          if ((local_7c[uVar10 & 0xffff] == '\x01') &&
             (cVar6 = fn_802026E4(iVar1,7), cVar6 == '\0')) {
            fn_801DA36C(iVar3,2);
          }
        }
      }
    }
    fn_801F2B5C(0,0x80261d8c,0,0);
    local_80 = 1;
    fn_801F37B0(0,0x80262084,&local_80,0);
    cVar6 = fn_801F1700(0);
    if (cVar6 == '\x01') {
      fn_8000DDE8();
    }
    cVar6 = fn_801F1758(0);
    if (cVar6 == '\x01') {
      fn_8000DD5C();
    }
    if ((r4 != 0) && (cVar6 = fn_801F18DC(0), cVar6 != '\x01')) {
      iVar1 = fn_801F02AC(2,r4,r5);
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar5 = fightSideGetStatus(iVar1,0,5,0);
        sVar4 = fn_801F0134(r4,r5);
        if (sVar4 == 0) {
          uVar2 = 0;
        }
        else {
          fn_801F0234();
          uVar10 = fn_801F0204();
          if ((int)uVar10 < 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = fightSideGetStatus(0,uVar5,3,uVar10 & 0xffff);
          }
        }
      }
      fn_80011D9C(uVar2,1);
    }
    uStack_64 = fn_800D37CC();
    uStack_64 = uStack_64 ^ 0x80000000;
    local_68 = 0x43300000;
    fn_80102038((double)(float)(dVar12 / (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uStack_64)) -
                                                         dVar11)));
    fn_801EFFC4(10);
    if ((uVar9 & 0xffff) != 0) {
      cVar6 = fn_80142CF4(0,uVar9,2,0);
      if (cVar6 == '\x01') goto code_r0x002619ac;
      cVar6 = fn_80142CF4(0,uVar9,2,0);
      if (cVar6 == '\x02') {
        fn_8022FF90();
        for (uVar10 = 0; (uVar10 & 0xffff) < 2; uVar10 = uVar10 + 1) {
          iVar1 = fn_801FB1C0(r3,0,0x46,uVar10);
          if (((iVar1 != 0) && (local_74[uVar10 & 0xffff] == '\0')) &&
             (cVar6 = fn_802062FC(), cVar6 == '\x01')) {
            fn_801254B4(iVar1,0,0x120,0,1);
          }
        }
      }
      uVar2 = 1;
      uVar10 = local_70[0] & 0xffff;
      goto LAB_00261a8c;
    }
    uVar2 = 0;
    goto LAB_00261ab8;
  }
  uVar2 = 0;
LAB_00261ab8:

  return uVar2;
code_r0x002619ac:
  cVar6 = fn_801F7EF0(r3);
  if ((cVar6 == '\x01') || (iVar1 = fn_80264ADC(r4,uVar9,r5), iVar1 == 0))
  goto LAB_0026160c;
  uVar10 = fn_801F0134(iVar1,r5);
  uVar2 = 0;
LAB_00261a8c:
  fn_80204CE0(r4,0,0x12,0,0x80375d70,uVar9 & 0xffff,uVar10,uVar7,uVar2);
  uVar2 = 1;
  goto LAB_00261ab8;
}

/* Address: 0x80265DB0 | Size: 0x84 | Ghidra import (PSQ removed) */
void fn_80265DB0(u8 *r3)

{
    extern u32 lbl_8047E6D8;
    extern f32 lbl_8047E6DC;
    extern f64 lbl_8047E6E0;

  u32 uVar1;

  u64 f30;
  double dVar2;
  u64 f31;
  double dVar3;
  u8 auStack_18 [16];
  u8 auStack_8 [8];

  dVar2 = lbl_8047E6E0;
  *(u32 *)(r3 + 4) = lbl_8047E6D8;
  dVar3 = (double)lbl_8047E6DC;
  while (*(float *)(r3 + 4) < (float)(dVar3 * (double)*(float *)(r3 + 8))) {
    _threadSwitch();
    uVar1 = fn_800D3088();
    *(float *)(r3 + 4) =
         *(float *)(r3 + 4) + (float)((double)(((u64)(0x43300000) << 32) | (u32)(uVar1)) - dVar2);
  }
  *r3 = 1;
  do {
    _threadSwitch();
  } while (1);
}
