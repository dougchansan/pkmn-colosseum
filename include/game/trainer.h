/**
 * @file trainer.h
 * @brief Trainer/party data access API for Pokemon Colosseum.
 *
 * The trainer data system provides a unified interface for querying trainer
 * and Pokemon party information. The core function TrainerDataGet (fightTrainerGetStatus)
 * is the single most called function from the target range at 883 call sites.
 *
 * Subsystem map (0x801FB1C0 - 0x80200A8C, ~180 functions):
 *   0x801FB1C0 - 0x801FB8E4 : TrainerDataGet dispatch (jumptable_803757D8, 0x56 entries)
 *   0x801FBFBC - 0x801FC658 : Trainer category handlers (5+ sub-dispatchers)
 *   0x801FC658 - 0x801FCA2C : Party member accessors
 *   0x801FCA2C - 0x801FCCAC : Team composition queries
 *   0x801FCCAC - 0x801FE000 : Trainer property accessors (~149 functions in 0x801FC*-0x801FD*)
 *   0x801FE000 - 0x80200A8C : Event/story flag integration
 *
 * The TrainerDataGet dispatch works in two phases:
 *   Phase 1: Determine the "category" from the field ID (r5) and load
 *            the appropriate sub-object pointer via one of:
 *            fightTrainerDataBiosGetPtr (fields 0x01-0x09, battle trainer)
 *            fightTrainerPokemonPartDataBiosGetPtr (fields 0x0A-0x0C, party config)
 *            fightTrainerPokemonDataBiosGetPtr (fields 0x0D-0x1D, team roster)
 *            fightTrainerAiDataBiosGetPtr (fields 0x1E-0x3C, story/event data)
 *            fightTrainerAiValueAddsubDataBiosGetPtr (fields 0x3D-0x41, misc attributes)
 *
 *   Phase 2: Use jumptable_803757D8 (86 entries) to call the specific
 *            getter for the requested field.
 *
 * Related functions called by TrainerDataGet:
 *   pokemonGetStatus (1769 calls total): Master data table lookup from common_rel
 *   fn_80142CF4 (169 calls): Secondary data accessor (unknown table)
 */

#ifndef GAME_TRAINER_H
#define GAME_TRAINER_H

#include "dolphin/types.h"

/* =========================================================================
 * Constants
 * ========================================================================= */

/* Trainer field ID ranges for TrainerDataGet (r5 parameter).
 * The maximum valid field ID is 0x5A (checked as cmplwi r0, 0x5b). */
#define TRAINER_FIELD_MAX              0x5A

/* Trainer category boundaries (Phase 1 dispatch) */
#define TRAINER_CAT_BATTLE_MIN         0x01
#define TRAINER_CAT_BATTLE_MAX         0x09
#define TRAINER_CAT_PARTY_MIN          0x0A
#define TRAINER_CAT_PARTY_MAX          0x0C
#define TRAINER_CAT_TEAM_MIN           0x0D
#define TRAINER_CAT_TEAM_MAX           0x1D
#define TRAINER_CAT_STORY_MIN          0x1E
#define TRAINER_CAT_STORY_MAX          0x3C
#define TRAINER_CAT_MISC_MIN           0x3D
#define TRAINER_CAT_MISC_MAX           0x41

/* Selected field IDs (identified from calling patterns) */
#define TRAINER_FIELD_NAME             0x01  /* Trainer name string */
#define TRAINER_FIELD_CLASS            0x02  /* Trainer class */
#define TRAINER_FIELD_POKEMON_COUNT    0x14  /* Number of Pokemon in party */
#define TRAINER_FIELD_PARTY_POKEMON    0x36  /* Get party Pokemon pointer */
#define TRAINER_FIELD_TEAM_ID          0x42  /* Team/battle configuration */
#define TRAINER_FIELD_POKE_PTR         0x43  /* Pokemon data pointer */
#define TRAINER_FIELD_MONEY            0x44  /* Prize money */
#define TRAINER_FIELD_AI_FLAGS         0x45  /* AI behavior flags */

/* =========================================================================
 * TrainerDataGet-related functions
 * ========================================================================= */

/**
 * fightTrainerGetStatus - TrainerDataGet
 * Read a field from a trainer or their party.
 *
 * @param pokemon  Pokemon context pointer (r3, can be from fightTargetGetPtrAsNowFightType)
 * @param slot     Trainer slot/index (r4)
 * @param field    Trainer field ID (r5)
 * @param extra    Extra parameter for sub-indexed fields (r6)
 * @return         Field value (type varies by field)
 *
 * Uses jumptable_803757D8 (86 entries). 883 call sites in the codebase.
 */
u32 TrainerDataGet(void* pokemon, u32 slot, u16 field, u32 extra);

/**
 * fightTrainerSetStatus - TrainerDataSet
 * Write a field to a trainer or their party.
 *
 * @param pokemon  Pokemon context pointer (r3)
 * @param slot     Trainer slot/index (r4)
 * @param field    Trainer field ID (r5)
 * @param extra    Extra parameter (r6)
 * @param value    Value to write (r7)
 *
 * 169 call sites.
 */
u32 TrainerDataSet(void* pokemon, u32 slot, u16 field, u32 extra, u32 value);

/**
 * fightOutPokemonGetPokemonPtr / fightPokemonGetPokemonPtr - trainer -> party -> Pokemon pointer navigation
 * (two-hop and single-hop). Implemented and matched at 100% in
 * colosseum_event.c, not in this file.
 */

/**
 * fightTrainerAiIsPokemonJoutai - CheckTrainerPokemonFlag
 * Query a boolean flag on a specific Pokemon owned by a trainer.
 * Calls TrainerDataGet three times to resolve trainer -> party -> Pokemon,
 * then checks a flag via fightOutPokemonIsPokemonJoutaiDataId.
 * 272 call sites. Not yet decompiled (no definition in this TU); the
 * only prior "caller" was fictional and has been removed.
 */

#endif /* GAME_TRAINER_H */
