/**
 * @file colosseum.h
 * @brief Colosseum/stadium battle flow, team management, and story scripting.
 *
 * This header covers the massive middle section of the uncovered gap,
 * spanning 0x80201800 - 0x80266360 (~400KB, ~1750 functions). This is
 * the heart of the game's unique Colosseum mode mechanics.
 *
 * Subsystem map:
 *   0x80201800 - 0x80212000 : Event handlers and colosseum setup (~100 funcs)
 *   0x80212000 - 0x80226000 : Story script interpreter and colosseum logic (~200 funcs)
 *   0x80226000 - 0x80240000 : Team management and validation (~120 funcs)
 *   0x80240000 - 0x80260000 : Pre/post battle flow and rewards (~300 funcs)
 *   0x80260000 - 0x80266360 : Utility functions and misc helpers (~50 funcs)
 */

#ifndef GAME_COLOSSEUM_H
#define GAME_COLOSSEUM_H

#include "dolphin/types.h"

/* =========================================================================
 * Constants
 * ========================================================================= */

/* Story script program counter location: lbl_8047B610 (SDA21) */
/* This global is incremented by +1 or +5 throughout the script system,
 * acting as a bytecode program counter for the event/story interpreter. */

/* Script command IDs (from the jumptable at jumptable_8039A220, 0x34 entries) */
/* These are the opcodes used by the story script interpreter in fightSeqSetTsuikaKouka. */
#define SCRIPT_CMD_NOP         0x00
#define SCRIPT_CMD_END         0x01
#define SCRIPT_CMD_JUMP        0x02
#define SCRIPT_CMD_CALL        0x03
#define SCRIPT_CMD_RETURN      0x04
#define SCRIPT_CMD_SET_FLAG    0x07  /* First valid opcode (0x07+0x00 = 0x07) */
#define SCRIPT_CMD_CHECK_FLAG  0x08
#define SCRIPT_CMD_BATTLE      0x09
#define SCRIPT_CMD_GIVE_ITEM   0x0A
#define SCRIPT_CMD_HEAL_PARTY  0x0B

/* Story event IDs */
#define EVENT_COLOSSEUM_MATCH   0x01
#define EVENT_STORY_BATTLE      0x02
#define EVENT_SHADOW_ENCOUNTER  0x03

/* Battle sequence IDs for pre/post battle flow */
#define BATTLE_SEQ_SETUP        0xF1
#define BATTLE_SEQ_EXECUTE      0xF2
#define BATTLE_SEQ_RESULT       0xF3
#define BATTLE_SEQ_REWARD       0xF4

/* =========================================================================
 * Script interpreter functions
 * ========================================================================= */

/**
 * fightSeqSetTsuikaKouka - StoryScriptExecute
 * The single largest function in the range (0x177C = 6012 bytes).
 *
 * This is the main story/event script interpreter. It reads bytecode-like
 * commands from a data stream (addressed via lbl_80478D78 SDA21) and
 * dispatches them through jumptable_8039A220 (0x34 entries).
 *
 * Setup:
 *   - Gets party count via PokemonGet(0,0,0x14,0)
 *   - Gets player party via fightFloorGetNowTenkouDataId(0,1)
 *   - Resolves active trainer slots via PokemonSlotLookupDefault(0x11,0) and (0x12,0)
 *   - Checks flags at lbl_80478D78+3 bit 6 to determine player/enemy perspective
 *   - Sets up active Pokemon via PokemonSet with fields 0x47 and 0x4B
 *
 * Main loop reads command bytes and dispatches through the jumptable.
 * Each command handler typically:
 *   1. Reads parameters from the command stream
 *   2. Calls TrainerDataGet/Set or PokemonGet/Set
 *   3. Updates the script program counter (lbl_8047B610)
 *
 * @param param1  Script context parameter
 * @param param2  Additional parameter
 */
void StoryScriptExecute(u32 param1, u32 param2);

/**
 * fightOutPokemonDoSoubiItemCheck - ColosseumRoundExecute
 * Second largest function (0x1258 = 4696 bytes).
 * Manages a complete round in a Colosseum battle sequence.
 * Heavily uses TrainerDataGet, PokemonGet, and the battle sequence IDs.
 */
void ColosseumRoundExecute(u32 context, u32 param);

/**
 * wsPlayAnimation - ColosseumMatchSetup
 * Third largest function (0x100C = 4108 bytes).
 * Initializes a Colosseum match: sets up trainers, validates teams,
 * configures battle rules.
 */
void ColosseumMatchSetup(u32 param1, u32 param2);

/**
 * wsCmd49 - TeamValidation
 * Large function (0xCA4 = 3236 bytes).
 * Validates a team for Colosseum entry: checks species clauses,
 * item clauses, level restrictions, banned Pokemon/moves.
 */
u32 TeamValidation(u32 teamCtx, u32 rules);

/* =========================================================================
 * Battle flow helpers
 * ========================================================================= */

/**
 * fn_80239984 - PreBattleSetup
 * Called 491 times. Sets up the pre-battle state for a trainer encounter.
 *
 * @param context    Battle context
 * @param trainerSlot  Trainer to fight
 * @param sequenceId   Battle sequence ID (BATTLE_SEQ_*)
 * @return           Battle setup handle
 */
u32 PreBattleSetup(void* context, u32 trainerSlot, u16 sequenceId);

/**
 * fn_80239EE8 - BattleSequenceStart
 * Called 491 times. Launches a battle sequence with full parameters.
 *
 * @param battleId   Battle identifier
 * @param trainerSlot  Trainer slot
 * @param pokemonPtr   Pokemon data pointer
 * @param param4-7     Additional battle configuration
 * @param seqId        Sequence type (e.g., 0xF1, 0xF2, 0xF3)
 */
void BattleSequenceStart(u32 battleId, u32 trainerSlot, void* pokemonPtr,
                         u32 param4, u32 param5, u32 param6,
                         u32 param7, u16 seqId);

/**
 * fightTrainerAiGetWazaTenkouValue - BattleSequenceCheck
 * Called 98 times. Checks the status of a running battle sequence.
 *
 * @param trainerSlot  Trainer slot to check
 * @param sequenceId   Sequence being checked
 * @return             Status value
 */
u32 BattleSequenceCheck(u32 trainerSlot, u16 sequenceId);

/**
 * fightTrainerAiGetAisyou - BattleResultCheck
 * Called 98 times. Checks the result of a completed battle.
 *
 * @param context      Battle context
 * @param trainerSlot  Trainer slot
 * @param resultType   Type of result to query
 * @return             Result value (e.g., species ID 0x43 = caught?)
 */
u32 BattleResultCheck(void* context, u32 trainerSlot, u32 resultType);

/* =========================================================================
 * Event/flag query functions
 * ========================================================================= */

/**
 * fightOutPokemonIsPokemonJoutaiDataId - CheckEventFlag
 * Called 291 times. Checks whether an event flag is set on a
 * trainer/Pokemon context.
 *
 * @param context   Context pointer
 * @param flagId    Flag to check
 * @return          TRUE if flag is set
 */
BOOL CheckEventFlag(void* context, u32 flagId);

/**
 * fn_802025B8 - GetEventState
 * Called 59 times. Gets the current state value of an event.
 */
u32 GetEventState(void* context, u32 eventId);

/**
 * fightOutPokemonWritePokemonJoutaiDataId - SetEventState
 * Called 67 times. Sets the state value of an event.
 */
void SetEventState(void* context, u32 eventId, u32 value);

/**
 * wsRunScript - TriggerEvent
 * Called 121 times. Triggers an event on a trainer/scene context.
 *
 * @param context    Scene/trainer context
 * @param eventData  Event data pointer
 * @param param      Event parameter
 */
void TriggerEvent(void* context, void* eventData, u32 param);

/**
 * fightActionUseItemMain - EventCallback
 * Called from story script after battle completion to process results.
 */
void EventCallback(void* context, u32 param);

#endif /* GAME_COLOSSEUM_H */
