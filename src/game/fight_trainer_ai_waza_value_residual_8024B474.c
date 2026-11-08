/**
 * @file fight_trainer_ai_waza_value_residual_8024B474.c
 * @brief Exact fightTrainerAiWazaValue.cpp residual, 0x8024B474 - 0x8024E578.
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
extern int  wazaSequenceSysInitEx();
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
u8 fightTrainerAiIsTokusei(void*, u32, u32);
u8 fightTrainerAiCheckWriteJoutai(void*, u32, u32);
u8 fightTrainerAiGetWazaTsuikaAvg(void*, u32);
u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
u16 fightTrainerAiGetPokemonDataId2(void*, u32);
u8 fightTrainerAiGetPokemonZokusei(void*, u32, u32);
u8 fightTrainerAiIsPokemonTokusei(void*, u32, u32);
u32 fightTrainerAiAddValueClamped(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fightTrainerAiLogValue(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);

#if !defined(FIGHT_AI_VALUE_EXACT_8024DE8C_ONLY)

#if !defined(FIGHT_AI_VALUE_EXACT_8024BA44_ONLY) && \
    !defined(FIGHT_AI_VALUE_CANDIDATE_8024BFC0_ONLY)

/* Address: 0x8024B474 | Size: 0x5D0 (1488 bytes) */
u32 fightTrainerAiWazaValueJisin(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    u32 fightOutPokemonGetPokemonPtr(u32);
    u16 fightTrainerAiGetLastUseWaza(void*, u32);
    u16 fightTrainerAiGetFightPokemonWazaAry(void*, u32, u16*, u32, u32);
    u16 fightTrainerAiGetHp(void*, u32);
    u16 fightTrainerAiGetAisyou(void*, u32, u32, u32);
    u32 fightTrainerAiGetWazaIryoku(void*, u32);
    u32 fightTrainerAiGetWazaTenkouValue(void*, u32, u32);
    u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    s32 fightTrainerAiGetWazaDamageValue(void*, u32, u32, u32, u32);
    void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 pokemon[27];
    u32 enemy[8];
    u32 own[8];
    u16 waza[10];
    u32 handle;
    u32 wazaKind;
    u16 enemyCount;
    u16 wazaCount;
    u16 i;
    u16 j;

    handle = 0;
    wazaKind = fightTrainerAiGetWazaTenkouValue(ctx, param2, param1);
    fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, enemy, 1, 1);
    fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, own, 0, 1);

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        if (fightTrainerAiGetAisyou(ctx, enemy[i], wazaKind, fightTrainerAiGetWazaIryoku(ctx, param2)) == 0x43) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa9);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa9);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        if (fightTrainerAiGetAisyou(ctx, enemy[i], wazaKind, fightTrainerAiGetWazaIryoku(ctx, param2)) == 0x42) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0xaa);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xaa);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        wazaCount = fightTrainerAiGetFightPokemonWazaAry(ctx, enemy[i], waza, 0, 1);
        if (wazaCount == 0) {
            continue;
        }
        for (j = 0; j < wazaCount; j++) {
            if (waza[j] == 0xb6 || waza[j] == 0xc5 || waza[j] == 0xcb) {
                handle = fightTrainerAiAddValueClamped(handle, ctx, 0xab);
                fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xab);
                break;
            }
        }
        if (j < wazaCount) {
            break;
        }
    }

    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xb6, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xac);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xac);
    } else if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xc5, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xac);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xac);
    } else if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xcb, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xac);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xac);
    }

    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xad);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xad);
    }

    for (i = 0; i < enemyCount; i++) {
        u16 kind;

        if (param1 == enemy[i]) {
            continue;
        }
        kind = fightTrainerAiGetLastUseWaza(ctx, enemy[i]);
        if (kind == 0xb6 || kind == 0xc5 || kind == 0xcb) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0xae);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xae);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        u16 damage;

        if (param1 == enemy[i]) {
            continue;
        }
        damage = fightTrainerAiGetHp(ctx, enemy[i]);
        if (damage > fightTrainerAiGetWazaDamageValue(ctx, param1, param2, enemy[i], 0)) {
            continue;
        }
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xaf);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xaf);
    }

    return handle;
}

#endif

#if defined(FIGHT_AI_VALUE_EXACT_8024BA44_ONLY)

/* Address: 0x8024BA44 | Size: 0x438 (1080 bytes) */
u32 fightTrainerAiWazaValueSokubaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetIrekaeFightPokemon(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightAiCheckTrapTokusei(void*, u32, u32, u32);
    extern u32 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fightAiCheckTrapTokusei(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fightTrainerGetIrekaeFightPokemon(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)status == 0) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x9f);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9f);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 3) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa0);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa0);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 4) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa1);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa1);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 6) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa2);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa2);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 9) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa4);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa4);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0xa) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa5);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa5);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x18) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa6);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa6);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1e) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa7);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa7);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1c) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa3);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa3);
    }
    if ((u8)status != 0) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0xa8);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa8);
    }
    return handle;
}
/* Address: 0x8024BE7C | Size: 0x144 (324 bytes) */
u32 fightTrainerAiWazaValueKorogaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern s32 fightAiGetHpPercent(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fightTrainerAiLogValue(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    s32 value;
    s32 quotient;

    handle = 0;
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x1a) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x9c);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9c);
    }
    value = fightAiGetHpPercent(ctx, param1);
    quotient = value / fightTrainerGetStatus(0, 0x9d, 0x3e, 0);
    handle = fightTrainerAiAddValue(handle, quotient);
    fightTrainerAiLogValue(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9d, quotient);
    handle = fightTrainerAiAddValueClamped(handle, ctx, 0x9e);
    fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9e);
    return handle;
}

#endif

#if defined(FIGHT_AI_VALUE_CANDIDATE_8024BFC0_ONLY)

#if !defined(FIGHT_AI_VALUE_EXACT_8024D818_ONLY) && \
    !defined(FIGHT_AI_VALUE_SUFFIX_8024DC7C_ONLY)

/* Address: 0x8024BFC0 | Size: 0x5FC (1532 bytes) */
u32 fightTrainerAiWazaValueJibaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    u32 fightOutPokemonGetPokemonPtr(u32);
    u16 fightTrainerAiGetLastUseWaza(void*, u32);
    u16 fightTrainerAiGetFightPokemonWazaAry(void*, u32, u16*, u32, u32);
    u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    u16 fightTrainerAiGetHp(void*, u32);
    u16 fightTrainerAiGetAisyou(void*, u32, u32, u32);
    u32 fightTrainerAiGetWazaIryoku(void*, u32);
    u32 fightTrainerAiGetWazaTenkouValue(void*, u32, u32);
    u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    s32 fightTrainerAiGetWazaDamageValue(void*, u32, u32, u32, u32);
    void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 pokemon[23];
    u32 enemy[8];
    u32 own[8];
    u16 waza[10];
    u32 handle;
    u32 wazaKind;
    u32 valueId;
    u16 enemyCount;
    u16 ownCount;
    u16 wazaCount;
    u16 i;
    u16 j;

    handle = 0;
    wazaKind = fightTrainerAiGetWazaTenkouValue(ctx, param2, param1);
    fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, enemy, 1, 1);
    ownCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, own, 0, 1);

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        if (fightTrainerAiGetAisyou(ctx, enemy[i], wazaKind, fightTrainerAiGetWazaIryoku(ctx, param2)) == 0x43) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0x94);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x94);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        if (fightTrainerAiGetAisyou(ctx, enemy[i], wazaKind, fightTrainerAiGetWazaIryoku(ctx, param2)) == 0x42) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0x95);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x95);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        if (param1 == enemy[i]) {
            continue;
        }
        wazaCount = fightTrainerAiGetFightPokemonWazaAry(ctx, enemy[i], waza, 0, 1);
        if (wazaCount == 0) {
            continue;
        }
        for (j = 0; j < wazaCount; j++) {
            if (waza[j] == 0xb6 || waza[j] == 0xc5 || waza[j] == 0xcb) {
                handle = fightTrainerAiAddValueClamped(handle, ctx, 0x96);
                fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x96);
                break;
            }
        }
        if (j < wazaCount) {
            break;
        }
    }

    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xb6, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x97);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x97);
    } else if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xc5, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x97);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x97);
    } else if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0xcb, 0) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x97);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x97);
    }

    for (i = 0; i < ownCount; i++) {
        if (fightTrainerAiIsPokemonJoutai(ctx, own[i], 0x14) == 1) {
            valueId = i == 0 ? 0x98 : 0x99;
            handle = fightTrainerAiAddValueClamped(handle, ctx, valueId);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, valueId);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        u16 kind;

        if (param1 == enemy[i]) {
            continue;
        }
        kind = fightTrainerAiGetLastUseWaza(ctx, enemy[i]);
        if (kind == 0xb6 || kind == 0xc5 || kind == 0xcb) {
            handle = fightTrainerAiAddValueClamped(handle, ctx, 0x9a);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9a);
        }
    }

    for (i = 0; i < enemyCount; i++) {
        u16 damage;

        if (param1 == enemy[i]) {
            continue;
        }
        damage = fightTrainerAiGetHp(ctx, enemy[i]);
        if (damage > fightTrainerAiGetWazaDamageValue(ctx, param1, param2, enemy[i], 0)) {
            continue;
        }
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x9b);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9b);
    }

    return handle;
}

#if !defined(FIGHT_AI_VALUE_PREFIX_8024BFC0_ONLY)

/* Address: 0x8024C5BC | Size: 0x91C (2332 bytes) */
void fightTrainerAiWazaValueTobihaneru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiGetFightPokemonWazaAry();
    extern void fightTrainerAiIsPokemonJoutai();
    extern void fightTrainerAiIsFasterBasis();
    extern void fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
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
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x7e;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x4;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x7f;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x6;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x80;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x5;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x81;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x9;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x82;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0xa;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x83;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x18;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x84;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1e;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x85;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1c;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x86;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = r27;
    fightTrainerAiIsFasterBasis();
    if ((s32)r3 > (s32)0x0) {
        r3 = r26;
        r4 = r31;
        r5 = 0x87;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r26;
    r4 = r31;
    r5 = 0x88;
    fightTrainerAiAddValueClamped();
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
    fightTrainerAiAddValueLog2();
    r3 = r31;
    r4 = r30;
    r5 = 0x3;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x89;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x4;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8a;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x6;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8b;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x5;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8c;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x9;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8d;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0xa;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8e;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x18;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8f;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1e;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x90;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1c;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x91;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
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
        fightTrainerAiGetFightPokemonWazaAry();
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
                    fightTrainerAiAddValueClamped();
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
                    fightTrainerAiAddValueLog2();
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
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x93;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r27;
    return;
}
/* Address: 0x8024CED8 | Size: 0x940 (2368 bytes) */
void fightTrainerAiWazaValueSorawotobu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiGetFightPokemonWazaAry();
    extern void fightTrainerAiIsPokemonJoutai();
    extern void fightTrainerAiIsFasterBasis();
    extern void fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
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
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x69;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x4;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6a;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x6;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6b;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x5;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6c;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x9;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6d;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0xa;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6e;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x18;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6f;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1e;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x70;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1c;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x71;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = r25;
    fightTrainerAiIsFasterBasis();
    if ((s32)r3 > (s32)0x0) {
        r3 = r30;
        r4 = r31;
        r5 = 0x72;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x3;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x73;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x4;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x74;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x6;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x75;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x5;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x76;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x9;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x77;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0xa;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x78;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x18;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x79;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1e;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7a;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1c;
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7b;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
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
        fightTrainerAiGetFightPokemonWazaAry();
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
                    fightTrainerAiAddValueClamped();
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
                    fightTrainerAiAddValueLog2();
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
    fightTrainerAiIsPokemonJoutai();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7d;
        fightTrainerAiAddValueClamped();
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
        fightTrainerAiAddValueLog2();
    }
    r3 = r30;
    return;
}

#endif

#endif

#if !defined(FIGHT_AI_VALUE_PREFIX_8024BFC0_ONLY) && \
    !defined(FIGHT_AI_VALUE_SUFFIX_8024DC7C_ONLY)

/* -------------------------------------------------------------------
 * Experience & Level Processing (0x8024D000-0x80254000)
 * 136 functions
 * ------------------------------------------------------------------- */

/* Address: 0x8024D818 | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueAbareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiIsTokusei();
    extern u16 fightTrainerAiGetSoubiItem();
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    s32 handle = 0;
    u16 v;

    if (fightTrainerAiIsTokusei(ctx, param1, 0x14) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x66);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x66);
    }
    v = fightTrainerAiGetSoubiItem(ctx, param1);
    if (v == 8 || v == 9) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x67);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x67);
    }
    handle = fightTrainerAiAddValueClamped(handle, ctx, 0x68);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x68);
    return handle;
}
/* Address: 0x8024D958 | Size: 0x1A4 (420 bytes) */
u32 fightTrainerAiWazaValueKyuusyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetHp();
    extern u32 fightTrainerAiIsTokusei(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fightTrainerAiLogValue(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    extern s32 fightTrainerAiGetWazaDamageValue(void*, u32, u32, u32, u32);
    u32 handle;
    s32 score;
    s32 scale;

    handle = fightTrainerAiGetHp();
    score = fightTrainerAiGetWazaDamageValue(ctx, param1, param2, param3, 1);
    scale = fightTrainerGetStatus(0, 0x63, 0x3e, 0);
    score = (((score / 2) * 100) / (u16)handle) / scale;
    {
        u32 nextHandle = fightTrainerAiAddValue(0, score);
        handle = nextHandle;
    }
    fightTrainerAiLogValue(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x63, score);
    if ((fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fightTrainerAiAddValueClamped(handle, ctx, 0x64);
            handle = nextHandle;
        }
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x64);
    }
    if ((fightTrainerAiIsTokusei(ctx, param3, 0x40) & 0xFF) == 1) {
        {
            u32 nextHandle = fightTrainerAiAddValueClamped(handle, ctx, 0x65);
            handle = nextHandle;
        }
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x65);
    }
    return handle;
}
/* Address: 0x8024DAFC | Size: 0xC0 */
u32 fightTrainerAiWazaValueRokettoZutuki(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fightTrainerAiAddValueClamped(0, ctx, 0x61);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x61);
    handle = fightTrainerAiAddValueClamped(handle, ctx, 0x62);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x62);
    return handle;
}
/* Address: 0x8024DBBC | Size: 0xC0 */
u32 fightTrainerAiWazaValueKamaitati(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fightTrainerAiAddValueClamped(0, ctx, 0x5f);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x5f);
    handle = fightTrainerAiAddValueClamped(handle, ctx, 0x60);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x60);
    return handle;
}

#endif

#if !defined(FIGHT_AI_VALUE_PREFIX_8024BFC0_ONLY) && \
    !defined(FIGHT_AI_VALUE_EXACT_8024D818_ONLY)

/* Address: 0x8024DC7C | Size: 0x210 (528 bytes) */
u32 fightTrainerAiWazaValueKiaipanti(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetLastUseWaza(void*, u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
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
    if ((fightTrainerAiGetLastUseWaza(battleCtx, trainer) & 0xFFFF) == 0x117) {
        handle = fightTrainerAiAddValueClamped(0, battleCtx, 0x5B);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5B);
    }

    entriesPtr = entries;
    count = rawCount;
    index = 0;
    while ((u16)index < count) {
        if (fightTrainerAiIsPokemonJoutai(battleCtx, entriesPtr[(u16)index], 8) == 1) {
            handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x5C);
            fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5C);
            break;
        }
        index++;
    }

    entriesPtr = entries;
    rawCount = rawCount & 0xFFFF;
    index = 0;
    while ((u16)index < rawCount) {
        if (fightTrainerAiIsPokemonJoutai(battleCtx, entriesPtr[(u16)index], 7) == 1) {
            handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x5D);
            fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5D);
            break;
        }
        index++;
    }

    handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x5E);
    fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5E);
    return handle;
}

#endif

#endif

#else

/* Address: 0x8024DE8C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueAteminage(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetAbicntAvoid(void*, u32);
    extern u8 fightTrainerAiGetAbicntAverage(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fightTrainerAiGetAbicntAvoid(battleCtx, param3) > 6U) {
        handle = fightTrainerAiAddValueClamped(0, battleCtx, 0x58);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x58);
    }
    if (fightTrainerAiGetAbicntAverage(battleCtx, trainer) < 6U) {
        handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x59);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x59);
    }
    handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x5a);
    fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5a);
    return handle;
}
/* Address: 0x8024DFC4 | Size: 0x108 (264 bytes) */
u32 fightTrainerAiWazaValueRibenji(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetLastUseWaza(void*, u32);
    extern u8 fightTrainerAiIsWazaTypeId(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;
    u32 move = fightTrainerAiGetLastUseWaza(battleCtx, param3);

    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163) {
        if (fightTrainerAiIsWazaTypeId(battleCtx, move, 1) == 1) {
            handle = fightTrainerAiAddValueClamped(0, battleCtx, 0x56);
            fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x56);
        }
    }
    handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x57);
    fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x57);
    return handle;
}
/* Address: 0x8024E0CC | Size: 0x7C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueTokubetuYuusen(void* ctx, u32 slot, u32 param) {
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    u32 handle;

    handle = fightTrainerAiAddValueClamped(0, ctx, 0x55);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x55);
    return handle;
}
/* Address: 0x8024E148 | Size: 0xEC (236 bytes) */
u32 fightTrainerAiWazaValueKouPuraioritii(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fightTrainerAiAddValueClamped(0, ctx, 0x53);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x53);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x54);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x54);
    }
    return handle;
}
/* Address: 0x8024E234 | Size: 0xEC (236 bytes) */
u32 fightTrainerAiWazaValueKaihukuAddsub(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fightTrainerAiAddValueClamped(0, ctx, 0x51);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x51);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x52);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x52);
    }
    return handle;
}
/* Address: 0x8024E320 | Size: 0x164 (356 bytes) */
u32 fightTrainerAiWazaValueHittyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetAbicntAvoid(void*, u32);
    extern u8 fightTrainerAiGetAbicntAverage(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fightTrainerAiGetAbicntAvoid(battleCtx, param3) > 6U) {
        handle = fightTrainerAiAddValueClamped(0, battleCtx, 0x4E);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4E);
    }
    if (fightTrainerAiGetAbicntAverage(battleCtx, trainer) < 6U) {
        handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x4F);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4F);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10E, trainer) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, battleCtx, 0x50);
        fightTrainerAiAddValueLog2(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x50);
    }
    return handle;
}
/* Address: 0x8024E484 | Size: 0xA8 */
u32 fightTrainerAiWazaValueTuujouIryoku(void* ctx, u32 slot, u32 param) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x4d);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x4d);
    }
    return handle;
}
/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 120 functions matched
 * =================================================================== */

/* Address: 0x8024E52C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaValueNull(void) { return 0; }
/* Address: 0x8024E534 | Size: 0x44 | Pattern: field_accessor */
u32 fightTrainerAiSelectItem(void* ctx, u32 slot, u32 param) {
    extern u32 fightTrainerGetStatus();
    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return 0;
}

#endif
