/**
 * @file fight_trainer_ai_waza_value_exact_8024A664.c
 * @brief Byte-exact fightTrainerAiWazaValue.cpp range, 0x8024A664 - 0x8024B474.
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
