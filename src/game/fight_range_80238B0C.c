/**
 * @file fight_range_80238B0C.c
 * @brief fightTrainerAiGetPokemonAisyou, 0x80238B0C - 0x80238E30: the AI's type-effectiveness
 *        lookup for a move type against a pokemon (zokuseiGetAisyouDualType), with the
 *        tokusei 0x1A / 0x19 overrides (0x43).
 *
 * Exact: written with the fight AI accessor inlines. The per-slot type getter
 * (aiGetType) is the body retail expands in fightTrainerAiGetPokemonAisyouScore's loop and twice in
 * fightTrainerAiGetPokemonZokusei; keeping it a nested inline under aiGetTypeList numbers the
 * first tokusei check's trainer/pokemon webs between the list's count and
 * species, which gives retail's r26.
 */
#include "dolphin/types.h"
extern u32 pokemonGetStatus(u32, u32, u32, u8);
extern u32 fightFloorGetStatus();
extern u32 fightTrainerGetStatus();
extern u32 fightPokemonGetPokemonPtr();
extern u32 pokemonGetTokuseiDataId();
extern u8 fightTrainerIsAllyFightTargetPtr();
extern u32 zokuseiGetAisyouDualType();

static inline u32 aiGetTokusei(u32 ctx, u32 pokemon)
{
    u16 trainer;
    u32 p;

    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
    fightFloorGetStatus(0, 0, 0x14, 0);
    trainer = fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
    p = fightPokemonGetPokemonPtr(pokemon);
    if ((u8)fightTrainerGetStatus(0, trainer, 0x2B, 0) == 1) {
        return pokemonGetTokuseiDataId(p);
    }
    return 0;
}

static inline u8 aiIsTokusei(u32 ctx, u32 pokemon, u16 tokusei)
{
    return (u16)aiGetTokusei(ctx, pokemon) == tokusei;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (one call here; the
 * same body is expanded in fightTrainerAiGetPokemonAisyouScore and twice in fightTrainerAiGetPokemonZokusei), kept as a
 * nested inline for register numbering -- see docs/RULE_EXCEPTIONS.md. */
static inline u16 aiGetType(u32 ctx, u32 pokemon, u32 i)
{
    u16 floor;
    u16 trainer;
    u16 species;
    u16 type;

    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    trainer = fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
    species = pokemonGetStatus(fightPokemonGetPokemonPtr(pokemon), 0, 0x6E, 0);
    if ((u8)fightTrainerGetStatus(0, trainer, 0x2A, 0) == 1) {
        if (fightTrainerIsAllyFightTargetPtr(ctx, pokemon, floor) == 0) {
            type = pokemonGetStatus(0, species, 0x16, i);
        } else {
            type = pokemonGetStatus(0, species, 0x16, i);
        }
    } else {
        type = 9;
    }
    return type;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper chosen for register
 * numbering (written in place: 99.05%) -- see docs/RULE_EXCEPTIONS.md. */
static inline u32 aiGetTypeList(u32 ctx, u32 pokemon, u16* types)
{
    u16 type;
    u32 count;
    u32 i;

    count = 0;
    for (i = count; (u8)i < 2; i++) {
        type = aiGetType(ctx, pokemon, i);
        if (type != 9) {
            types[(u16)count] = type;
            count++;
        }
    }
    return count;
}

u32 fightTrainerAiGetPokemonAisyou(u32 ctx, u32 pokemon, u32 moveType, s16 power)
{
    u16 types[2];
    u32 count;
    u32 result;

    if ((u16)moveType == 9) {
        return 0x3F;
    }
    if (aiIsTokusei(ctx, pokemon, 0x1A) == 1 && (u16)moveType == 4) {
        return 0x43;
    }
    count = aiGetTypeList(ctx, pokemon, types);
    if ((u16)count == 0) {
        return 0x3F;
    }
    result = zokuseiGetAisyouDualType(moveType, types, count);
    if (aiIsTokusei(ctx, pokemon, 0x19) == 1 && (u16)result != 0x41 && power > 0) {
        return 0x43;
    }
    return result;
}
