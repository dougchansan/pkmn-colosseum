/* Score instrumentation only; not evidence of a retail TU boundary. */
/**
 * @file fight_trainer_ai_waza_value_candidate_80245FC4.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x80245FC4 - 0x80247048.
 *
 * Physically split from fight_trainer_ai_waza_value.c so this
 * translation unit owns only the functions in the stated range.
 *
 * Secret Power (Himitunotikara) dispatches on the terrain effect id and
 * scores the move as the terrain's equivalent effect.  In the retail TU
 * every one of those equivalents except TuikaHirumi is small enough that
 * MWCC's deferred auto-inliner expanded it into the switch, even though
 * the definitions follow this function in the file (their out-of-line
 * copies live at 0x80247FA0 - 0x8024AB80).  The definitions are repeated
 * below, verbatim from the exact units that own them, as compile-only
 * same-TU context so the inline decision is reproduced; this object is
 * never linked, so nothing here is claimed twice.
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

/* Same-TU move valuations Secret Power dispatches to.  Defined after the
 * dispatcher in the retail file; TuikaHirumi (0x4B0 bytes) stays a call. */
u32 fightTrainerAiWazaValueTuikaDoku(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikaMahi(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikouKonran(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikouMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikouSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikouBougyoDaun(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueOororabiimu(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaValueTuikaHirumi(void* ctx, u32 param1, u32 param2, u32 param3);
static u32 fightTrainerAiWazaValueTuikaNemuri(void* ctx, u32 param1, u32 param2, u32 param3);

/* Address: 0x80245FC4 | Size: 0x1084 (4228 bytes) */
u32 fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 tikeiDataBiosGetFightKoukaId(u32);
    u32 value;

    value = 0;
    switch ((u8)tikeiDataBiosGetFightKoukaId(fightFloorGetStatus(0, 0, 0xf, 0) & 0xffff)) {
    case 2:
        value = fightTrainerAiWazaValueTuikaDoku(ctx, param1, param2, param3);
        break;
    case 1:
        value = fightTrainerAiWazaValueTuikaNemuri(ctx, param1, param2, param3);
        break;
    case 27:
        value = fightTrainerAiWazaValueTuikouMeityuuDaun(ctx, param1, param2, param3);
        break;
    case 23:
        value = fightTrainerAiWazaValueTuikouBougyoDaun(ctx, param1, param2, param3);
        break;
    case 22:
        value = fightTrainerAiWazaValueOororabiimu(ctx, param1, param2, param3);
        break;
    case 24:
        value = fightTrainerAiWazaValueTuikouSubayasaDaun(ctx, param1, param2, param3);
        break;
    case 7:
        value = fightTrainerAiWazaValueTuikouKonran(ctx, param1, param2, param3);
        break;
    case 8:
        value = fightTrainerAiWazaValueTuikaHirumi(ctx, param1, param2, param3);
        break;
    case 5:
        value = fightTrainerAiWazaValueTuikaMahi(ctx, param1, param2, param3);
        break;
    }
    return value;
}

/* Sleep-terrain valuation (effect ids 0x10d - 0x10f), the same shape as
 * TuikaDoku/TuikaMahi with Nemurare's party scan in the middle.  Retail has
 * no out-of-line copy anywhere in the DOL: Secret Power is its only caller,
 * the inliner expanded it there, and nothing else referenced the static. */
static u32 fightTrainerAiWazaValueTuikaNemuri(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_802384B4(void*, u32, u32);
    extern u8 fn_80239564(void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    u32 entries[24];
    u32 handle;
    u32 compareValue;
    s32 quotient;
    u16 count;
    u16 index;

    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, entries, 0, 1);
    quotient = fn_80239564(ctx, param2);
    quotient /= fightTrainerGetStatus(0, 0x10d, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10d, quotient);
    index = 0;
    while (index < count) {
        compareValue = pokemonGetStatus(param3, 0, 0xd5, 0);
        if (compareValue != entries[index]) {
            if (fn_802384B4(ctx, entries[index], 8) == 1) {
                quotient = fn_80239564(ctx, param2);
                quotient /= fightTrainerGetStatus(0, 0x10e, 0x3e, 0);
                handle = fightTrainerAiAddValue(handle, quotient);
                fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10e, quotient);
                break;
            }
        }
        index++;
    }
    if (fn_80237310(ctx, param3) == 0 || fn_80237F74(ctx, param3, 0xf) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        quotient = fn_80239564(ctx, param2);
        quotient /= fightTrainerGetStatus(0, 0x10f, 0x3e, 0);
        handle = fightTrainerAiAddValue(handle, quotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10f, quotient);
    }
    return handle;
}

/* =========================================================================
 * Compile-only same-TU context.  These definitions are owned and linked by
 * fight_trainer_ai_waza_value_exact_80247048.c and
 * fight_trainer_ai_waza_value_exact_8024A664.c; they are repeated here,
 * unchanged, only so the deferred auto-inliner sees the same bodies the
 * retail TU had after the dispatcher.
 * ========================================================================= */
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
    u32 handle;
    u32 pokemonPtr;
    u32 statusValue;
    s32 quotient;

    statusValue = fn_80239564(battleCtx, battleParam);
    quotient = (s32)statusValue / fightTrainerGetStatus(0, 0x104, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, quotient);
    fn_80239CCC(0xec64, battleCtx, fightOutPokemonGetPokemonPtr(trainerParam), 0, 0, battleParam, 0, 0x104, quotient);
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
    u32 handle;
    s32 quotient;
    s32 denom;
    s32 finalQuotient;

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
    if ((u8)fn_80237F74(ctx, param3, 0x13) == 1 || (u8)fn_80237F74(ctx, param3, 0x14) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xe6, 0x3e, 0);
        finalQuotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, finalQuotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xe6, finalQuotient);
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
    u32 handle;
    s32 quotient;
    s32 denom;
    s32 finalQuotient;

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
    if ((u8)fn_80237F74(ctx, param3, 0x1d) == 1 || (u8)fn_80237F74(ctx, param3, 0x13) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x49) == 1 || (u8)fn_80237F74(ctx, param3, 0x33) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xde, 0x3e, 0);
        finalQuotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, finalQuotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xde, finalQuotient);
    }
    return handle;
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
    u32 handle;
    s32 quotient;
    s32 denom;
    s32 finalQuotient;

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
    if ((u8)fn_80237F74(ctx, param3, 0x1d) == 1 || (u8)fn_80237F74(ctx, param3, 0x13) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x49) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xcb, 0x3e, 0);
        finalQuotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, finalQuotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xcb, finalQuotient);
    }
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
    u32 handle;
    s32 quotient;
    s32 denom;
    s32 finalQuotient;

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
    if ((u8)fn_80237F74(ctx, param3, 0x1d) == 1 || (u8)fn_80237F74(ctx, param3, 0x13) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x49) == 1) {
        quotient = (s32)(fn_80239564(ctx, param2) & 0xff);
        denom = fightTrainerGetStatus(0, 0xc6, 0x3e, 0);
        finalQuotient = quotient / denom;
        handle = fightTrainerAiAddValue(handle, finalQuotient);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xc6, finalQuotient);
    }
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
