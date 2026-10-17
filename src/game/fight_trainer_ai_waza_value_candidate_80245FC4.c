/**
 * @file fight_trainer_ai_waza_value_candidate_80245FC4.c
 * @brief fightTrainerAiWazaValueHimitunotikara (Secret Power), 0x80245FC4 -
 *        0x80247048, with its switch table (.data 0x8039A5D8 - 0x8039A648).
 *
 * Secret Power's effect depends on the terrain, so the AI switches on
 * tikeiDataBiosGetFightKoukaId() and scores the move like the move whose
 * added effect the terrain gives. Retail inlines those moves' own AI value
 * functions (TuikaDoku, TuikouMeityuuDaun, TuikouBougyoDaun, Oororabiimu,
 * TuikouSubayasaDaun, TuikouKonran, TuikaMahi; TuikaHirumi stays a call).
 * They are defined later in the TU, in other objects here, so this carve
 * carries static inline copies. Their locals are numbered per expansion,
 * which is what gives retail's per-case registers and lets each score
 * coalesce with the call result. Case 1 (ids 0x10D-0x10F) has no
 * standalone counterpart in the binary and is written the same way.
 * Plain GC/1.3 -O4,s, no pragmas.
 *
 * RULE-EXCEPTION(user-approved): static inline copies of the TU's global
 * AI value functions -- see docs/RULE_EXCEPTIONS.md
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus();
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
extern u32 fightFloorGetStatus(u32, u32, u32, u32);
extern u32 tikeiDataBiosGetFightKoukaId(u32);
u8 fightTrainerAiGetAbicntAverage(void*, u32);
u8 fightTrainerAiGetAbicntNimbleness(void*, u32);
u8 fightTrainerAiGetAbicntPhyDef(void*, u32);
u8 fightTrainerAiGetAbicntPhyAtk(void*, u32);
u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
u8 fightTrainerAiIsJoutaiNormal(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, void*, u32);
u8 fn_80239564(void*, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
u32 fightTrainerAiWazaValueTuikaHirumi(void*, u32, u32, u32);

static inline u32 fightTrainerAiWazaValueTuikaDoku(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0x113, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x113, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x114);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x114);
    }
    if (fightTrainerAiIsJoutaiNormal(ctx, param3) == 0 ||
        fn_80237F74(ctx, param3, 0x11) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x115, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x115, t);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueTuikouMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0xDB, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDB, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xDC);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDC);
    }
    if (fightTrainerAiGetAbicntAverage(ctx, param3) == 0) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xDD, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDD, t);
    }
    if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1 ||
        fn_80237F74(ctx, param3, 0x33) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xDE, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xDE, t);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueTuikouBougyoDaun(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0xC3, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC3, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xC4);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC4);
    }
    if (fightTrainerAiGetAbicntPhyDef(ctx, param3) == 0) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xC5, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC5, t);
    }
    if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xC6, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC6, t);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueOororabiimu(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    handle = fn_80239984(0, ctx, 0xBF);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xBF);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xC0);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC0);
    }
    if (fightTrainerAiGetAbicntPhyAtk(ctx, param3) == 0) {
        handle = fn_80239984(handle, ctx, 0xC1);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC1);
    }
    if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1 ||
        fn_80237F74(ctx, param3, 0x34) == 1) {
        handle = fn_80239984(handle, ctx, 0xC2);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC2);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueTuikouSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0xC7, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC7, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xC8);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC8);
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0xC9);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xC9);
    }
    if (fightTrainerAiGetAbicntNimbleness(ctx, param3) == 0) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xCA, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xCA, t);
    }
    if (fn_80237F74(ctx, param3, 0x1D) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x49) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xCB, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xCB, t);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueTuikouKonran(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0xE3, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE3, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0xE4);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE4);
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 9) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xE5, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE5, t);
    }
    if (fn_80237F74(ctx, param3, 0x13) == 1 ||
        fn_80237F74(ctx, param3, 0x14) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0xE6, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xE6, t);
    }
    return handle;
}

static inline u32 fightTrainerAiWazaValueTuikaMahi(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 handle;
    s32 t;

    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0x104, 0x3E, 0);
    handle = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x104, t);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10E, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x105);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x105);
    }
    if (fightTrainerAiIsJoutaiNormal(ctx, param3) == 0 ||
        fn_80237F74(ctx, param3, 0x07) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x106, 0x3E, 0);
        handle = fightTrainerAiAddValue(handle, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x106, t);
    }
    return handle;
}

/* Terrain case 1: ids 0x10D-0x10F, with a pass over the trainer's pokemon. */
static inline u32 himitunotikaraCase1(void* ctx, u32 param1, u32 param2, u32 param3)
{
    void* pokemonAry[24];
    s32 t;
    s32 score;
    u16 count;
    u16 i;

    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemonAry, 0, 1);
    t = fn_80239564(ctx, param2);
    t /= fightTrainerGetStatus(0, 0x10D, 0x3E, 0);
    score = fightTrainerAiAddValue(0, t);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10D, t);
    for (i = 0; i < count; i++) {
        if (pokemonGetStatus(param3, 0, 0xD5, 0) == pokemonAry[i]) {
            continue;
        }
        if (fn_802384B4(ctx, pokemonAry[i], 8) != 1) {
            continue;
        }
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x10E, 0x3E, 0);
        score = fightTrainerAiAddValue(score, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10E, t);
        break;
    }
    if (fightTrainerAiIsJoutaiNormal(ctx, param3) == 0 ||
        fn_80237F74(ctx, param3, 0x0F) == 1 ||
        fn_80237F74(ctx, param3, 0x13) == 1) {
        t = fn_80239564(ctx, param2);
        t /= fightTrainerGetStatus(0, 0x10F, 0x3E, 0);
        score = fightTrainerAiAddValue(score, t);
        fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x10F, t);
    }
    return score;
}

/* Address: 0x80245FC4 | Size: 0x1084 */
u32 fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3)
{
    u32 value;

    value = 0;
    switch ((u8)tikeiDataBiosGetFightKoukaId(fightFloorGetStatus(0, 0, 0xF, 0) & 0xFFFF)) {
    case 2:
        value = fightTrainerAiWazaValueTuikaDoku(ctx, param1, param2, param3);
        break;
    case 1:
        value = himitunotikaraCase1(ctx, param1, param2, param3);
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
