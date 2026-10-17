/**
 * @file fight_trainer_ai_waza_value.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x8024A170 - 0x8024A664.
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
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

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
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fightTrainerAiIsJoutaiNormal(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, u32, u32);
u8 fn_80239564(void*, u32);
u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
u16 fightTrainerAiGetPokemonDataId2(void*, u32);
u8 fightTrainerAiGetPokemonZokusei(void*, u32, u32);
u8 fn_80239058(void*, u32, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);

/* Address: 0x8024A170 | Size: 0x2B8 (696 bytes) */
u32 fightTrainerAiWazaValueKogoerukaze(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u32 fightOutPokemonGetPokemonPtr(u32);
    s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    u8 fn_80237F74(void*, u32, u32);
    u32 fn_80239984(u32, void*, u32);
    u16 j;
    void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 enemy[8];
    u32 own[8];
    u32 handle;
    u32 ownPokemon;
    u16 enemyCount;
    u16 ownCount;
    u16 i;

    handle = 0;
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, enemy, 1, 1);
    ownCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, own, 0, 1);

    for (i = 0; i < ownCount; i++) {
        ownPokemon = own[i];
        if (ownPokemon == 0) {
            continue;
        }
        for (j = 0; j < enemyCount; j++) {
            if (enemy[j] != 0 && fightTrainerAiIsFasterBasis(ctx, ownPokemon, enemy[j]) > 0) {
                handle = fn_80239984(handle, ctx, 0xcf);
                fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xcf);
                break;
            }
        }
    }

    if (ownCount >= 2) {
        handle = fn_80239984(handle, ctx, 0xd0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd0);
    }

    for (i = 0; i < enemyCount; i++) {
        if (enemy[i] != 0 && fightTrainerAiIsFasterBasis(ctx, param3, enemy[i]) < 0) {
            break;
        }
    }
    if (i < enemyCount) {
        handle = fn_80239984(handle, ctx, 0xd1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd1);
    }

    if (fn_80237F74(ctx, param3, 0x1d) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1) {
        handle = fn_80239984(handle, ctx, 0xd2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xd2);
    }
    return handle;
}
/* Address: 0x8024A428 | Size: 0x23C (572 bytes) */
u32 fightTrainerAiWazaValueKanarazuSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u32 fightOutPokemonGetPokemonPtr(u32);
    u8 fightTrainerAiGetAbicntNimbleness(void*, u32);
    s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    u8 fn_80237F74(void*, u32, u32);
    u32 fn_80239984(u32, void*, u32);
    void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 enemy[8];
    u32 own[8];
    u32 handle;
    u16 enemyCount;
    u16 ownCount;
    u16 i;
    u16 j;
    u32 ownPokemon;
    u8 found;

    handle = 0;
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, enemy, 1, 1);
    ownCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, own, 0, 1);

    for (i = 0; i < ownCount; i++) {
        ownPokemon = own[i];
        if (ownPokemon == 0) {
            continue;
        }
        found = 0;
        for (j = 0; j < enemyCount; j++) {
            if (enemy[j] != 0 && fightTrainerAiIsFasterBasis(ctx, ownPokemon, enemy[j]) > 0) {
                handle = fn_80239984(handle, ctx, 0xcc);
                fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xcc);
                found = 1;
                break;
            }
        }
        if (found == 1) {
            break;
        }
    }

    if (fightTrainerAiGetAbicntNimbleness(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0xcd);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xcd);
    }

    if (fn_80237F74(ctx, param3, 0x1d) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1) {
        handle = fn_80239984(handle, ctx, 0xce);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xce);
    }
    return handle;
}
