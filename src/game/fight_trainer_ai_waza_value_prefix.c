/**
 * @file fight_trainer_ai_waza_value_prefix.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x802405C0 - 0x80240BD0.
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

/* Address: 0x802405C0 | Size: 0x8C */
u32 fightTrainerAiWazaValueRisaikuru(void* ctx, u32 param1, u32 param2) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u16 fightAiGetUsedItem(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fightAiGetUsedItem(ctx) != 0) {
        u32 tmp = fn_80239984(0, ctx, 0x1c3);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1c3);
    }
    return handle;
}
/* Address: 0x8024064C | Size: 0x13C (316 bytes) */
u32 fightTrainerAiWazaValueSiroikiri(void* ctx, u32 param1, u32 param2, u32 param3) {
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
/* The target inlines this seven-status scan once for each scoring range. */
static inline u8 jikoanjiStatusInRange(void* ctx, u32 target, u8 low, u8 high) {
    u8 fightTrainerAiGetAbicntAvoid(void*, u32);
    u8 fightTrainerAiGetAbicntAverage(void*, u32);
    u8 fightTrainerAiGetAbicntNimbleness(void*, u32);
    u8 fn_80235974(void*, u32);
    u8 fn_802359D8(void*, u32);
    u8 fightTrainerAiGetAbicntPhyDef(void*, u32);
    u8 fightTrainerAiGetAbicntPhyAtk(void*, u32);
    u8 status[7];
    u8 i;

    status[0] = fightTrainerAiGetAbicntPhyAtk(ctx, target);
    status[1] = fightTrainerAiGetAbicntPhyDef(ctx, target);
    status[2] = fn_802359D8(ctx, target);
    status[3] = fn_80235974(ctx, target);
    status[4] = fightTrainerAiGetAbicntNimbleness(ctx, target);
    status[5] = fightTrainerAiGetAbicntAverage(ctx, target);
    status[6] = fightTrainerAiGetAbicntAvoid(ctx, target);

    for (i = 0; i < 7; i++) {
        if (status[i] >= low && status[i] <= high) {
            return 1;
        }
    }
    return 0;
}

u32 fightTrainerAiWazaValueJikoanji(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 fightOutPokemonGetPokemonPtr(u32);
    u32 fn_80239984(u32, void*, u32);
    void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;

    if (jikoanjiStatusInRange(ctx, param3, 8, 9) == 1) {
        handle = fn_80239984(0, ctx, 0x1be);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1be);
    }

    if (jikoanjiStatusInRange(ctx, param3, 0xa, 0xc) == 1) {
        handle = fn_80239984(handle, ctx, 0x1bf);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1bf);
    }

    if (jikoanjiStatusInRange(ctx, param3, 3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x1c0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1c0);
    }

    if (jikoanjiStatusInRange(ctx, param3, 0, 2) == 1) {
        handle = fn_80239984(handle, ctx, 0x1c1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1c1);
    }
    return handle;
}
