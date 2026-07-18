/**
 * @file fight_trainer_ai_waza_value_prefix.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x802405C0 - 0x80241B70.
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
