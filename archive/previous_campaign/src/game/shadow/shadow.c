/**
 * @file shadow.c
 * @brief Shadow Pokemon overworld management system.
 *
 * This file implements the non-battle Shadow Pokemon system:
 *   - Heart gauge tracking and purification progress
 *   - Purification methods (walking, cologne, calling, time decay, Relic Stone)
 *   - Shadow Pokemon registration (encounter/snag/purify state)
 *   - Snagging state tracking
 *
 * This is one of the original source files ("shadow.c" appears in rodata
 * at lbl_80275808 / 0x80275808). However, the "shadow.c" reference in
 * rodata is near the HSD library shadow rendering code (0x801B019C), which
 * is a *different* shadow.c for HSD's stencil shadow system. The game-level
 * Shadow Pokemon system lives in the game code area.
 *
 * The Shadow Pokemon data table is loaded from common_rel.fdat at
 * CommonIndexes slot 80 (data) / slot 81 (count). Each of the 48
 * Shadow Pokemon has an entry defining its species, owner trainer,
 * initial heart gauge, encounter location, and associated GSflags.
 *
 * Runtime state is persisted via the save system:
 *   - Per-Pokemon: shadowPokemonID, purificationCounter, expStored
 *     (in the SavePokemon structure at offsets 0xD8, 0xDC, 0xE0)
 *   - Global: ShadowRegistration array (one per shadow ID)
 *
 * Purification rates are modified by the Pokemon's nature, using the
 * first 5 bytes of the Nature data structure (battle/walk/call/daycare/cologne).
 *
 * Address context:
 *   The Shadow Pokemon game functions are scattered across the early
 *   game systems area (0x80005C3C - 0x80034280) and the field logic
 *   area (0x80114CA8 - 0x80130000). Key callers include:
 *     - The step counter in the field walk handler
 *     - The cologne item use handler in the menu system
 *     - The battle end handler
 *     - The Relic Stone interaction script
 *     - The save/load routines
 */

#include "game/shadow/shadow.h"
#include "game/gs_flag.h"

/* =========================================================================
 * External function declarations
 * ========================================================================= */

extern void fn_800DD970(const char* fmt, ...);   /* GSlog_Print */

/* Save Pokemon field access helpers */
extern u16  save_GetShadowID(void* pokemon);
extern s32  save_GetPurifyCounter(void* pokemon);
extern void save_SetPurifyCounter(void* pokemon, s32 value);
extern u32  save_GetStoredEXP(void* pokemon);
extern void save_SetStoredEXP(void* pokemon, u32 value);
extern u32  save_GetExperience(void* pokemon);
extern void save_SetExperience(void* pokemon, u32 value);
extern u8   save_GetNature(void* pokemon);
extern u16  save_GetSpecies(void* pokemon);
extern void save_ClearShadowID(void* pokemon);

/* Move replacement for purification */
extern void save_ReplaceMove(void* pokemon, u16 oldMoveID, u16 newMoveID);
extern u16  shadow_GetReplacementMove(u16 species, u8 level);

/* Nature data access */
extern void* nature_GetData(u8 natureID);

/* GSflag integration */
extern void GSflagSet32(s32 flagID);
extern s32  GSflagGet16(s32 flagID);

/* Scene/cutscene triggers */
extern void scene_TriggerCutscene(u32 cutsceneID);

/* Memory operations */
extern void* memset(void* dest, int val, u32 size);
extern void* memcpy(void* dest, const void* src, u32 size);

/* =========================================================================
 * Module-local state
 * ========================================================================= */

/**
 * Global Shadow Pokemon system state.
 * This is the single instance managing all 48 shadow Pokemon.
 */
static ShadowSystem sShadowSystem;

/* =========================================================================
 * Initialization
 * ========================================================================= */

/**
 * Initialize the Shadow Pokemon system.
 *
 * Loads the data table pointer and count, then initializes
 * the registration array to UNENCOUNTERED state with default
 * heart gauge values from the data table.
 */
void shadow_Init(ShadowPokemonData* dataPtr, u32 count) {
    u32 i;

    memset(&sShadowSystem, 0, sizeof(ShadowSystem));

    sShadowSystem.dataTable = dataPtr;
    sShadowSystem.dataCount = count;
    sShadowSystem.timeCounter = 0;
    sShadowSystem.walkCounter = 0;
    sShadowSystem.initialized = TRUE;

    /* Initialize each shadow Pokemon registration from the data table */
    for (i = 0; i < SHADOW_POKEMON_COUNT && i < count; i++) {
        ShadowRegistration* reg = &sShadowSystem.registry[i];
        ShadowPokemonData*  data = &dataPtr[i];

        reg->state            = SHADOW_STATE_UNENCOUNTERED;
        reg->trainerDefeated  = FALSE;
        reg->encounterCount   = 0;
        reg->currentGauge     = data->heartGaugeInitial;
        reg->maxGauge         = data->heartGaugeInitial;
        reg->speciesIndex     = data->species;
    }
}

/**
 * Reset the shadow registration state (new game).
 */
void shadow_Reset(void) {
    u32 i;
    ShadowPokemonData* dataPtr;

    if (sShadowSystem.initialized == 0) {
        return;
    }

    dataPtr = sShadowSystem.dataTable;
    sShadowSystem.timeCounter = 0;
    sShadowSystem.walkCounter = 0;

    for (i = 0; i < SHADOW_POKEMON_COUNT && i < sShadowSystem.dataCount; i++) {
        ShadowRegistration* reg = &sShadowSystem.registry[i];
        ShadowPokemonData*  data = &dataPtr[i];

        reg->state            = SHADOW_STATE_UNENCOUNTERED;
        reg->trainerDefeated  = FALSE;
        reg->encounterCount   = 0;
        reg->currentGauge     = data->heartGaugeInitial;
        reg->maxGauge         = data->heartGaugeInitial;
        reg->speciesIndex     = data->species;
    }
}

/* =========================================================================
 * Heart Gauge / Purification
 * ========================================================================= */

/**
 * Get the current heart gauge value for a shadow Pokemon.
 */
s32 shadow_GetHeartGauge(u16 shadowID) {
    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return -1;
    }
    return sShadowSystem.registry[shadowID - 1].currentGauge;
}

/**
 * Set the heart gauge value for a shadow Pokemon.
 * Clamps to the range [0, maxGauge].
 */
void shadow_SetHeartGauge(u16 shadowID, s32 value) {
    ShadowRegistration* reg;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return;
    }

    reg = &sShadowSystem.registry[shadowID - 1];

    if (value < 0) {
        value = 0;
    }
    if (value > reg->maxGauge) {
        value = reg->maxGauge;
    }

    reg->currentGauge = value;
}

/**
 * Reduce the heart gauge by the specified amount.
 *
 * The actual reduction is: amount * natureRate / 100
 * where natureRate is looked up from the Nature data table
 * based on the purification method.
 *
 * Nature data structure layout (first 5 bytes):
 *   [0] = battle rate
 *   [1] = walk rate
 *   [2] = call rate
 *   [3] = daycare rate (unused)
 *   [4] = cologne rate
 */
void shadow_ReduceHeartGauge(u16 shadowID, s32 amount, u8 method, u8 nature) {
    ShadowRegistration* reg;
    PurifyRates*        rates;
    s32                 adjustedAmount;
    u8                  rate;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return;
    }

    reg = &sShadowSystem.registry[shadowID - 1];

    /* Only reduce if still shadow (snagged but not purified) */
    if (reg->state != SHADOW_STATE_SNAGGED) {
        return;
    }

    /* Look up nature-based purification rate */
    rates = (PurifyRates*)nature_GetData(nature);
    if (rates == NULL) {
        rate = 100;  /* Default: 1x multiplier */
    } else {
        switch (method) {
        case PURIFY_METHOD_BATTLE:
            rate = rates->battleRate;
            break;
        case PURIFY_METHOD_WALK:
            rate = rates->walkRate;
            break;
        case PURIFY_METHOD_CALL:
            rate = rates->callRate;
            break;
        case PURIFY_METHOD_COLOGNE:
            rate = rates->cologneRate;
            break;
        case PURIFY_METHOD_TIME:
            rate = 100;  /* Time decay is not nature-dependent */
            break;
        default:
            rate = 100;
            break;
        }
    }

    /* Apply rate modifier: adjustedAmount = amount * rate / 100 */
    adjustedAmount = (amount * (s32)rate) / 100;
    if (adjustedAmount < 1 && amount > 0) {
        adjustedAmount = 1;  /* Minimum 1 if any reduction was requested */
    }

    /* Reduce gauge */
    reg->currentGauge -= adjustedAmount;
    if (reg->currentGauge < 0) {
        reg->currentGauge = 0;
    }
}

/**
 * Check if a shadow Pokemon's heart gauge has reached zero.
 */
BOOL shadow_IsPurifiable(u16 shadowID) {
    ShadowRegistration* reg;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return FALSE;
    }

    reg = &sShadowSystem.registry[shadowID - 1];

    /* Must be snagged (not unencountered, not already purified) */
    if (reg->state != SHADOW_STATE_SNAGGED) {
        return FALSE;
    }

    return (reg->currentGauge <= HEART_GAUGE_PURIFIED);
}

/**
 * Purify a shadow Pokemon at the Relic Stone.
 *
 * This is the culmination of the purification process:
 *   1. Mark the shadow as purified in the registry
 *   2. Convert stored EXP to real EXP on the Pokemon
 *   3. Replace Shadow Rush with a species-appropriate normal move
 *   4. Clear the shadow Pokemon ID from the save data
 *   5. Set the associated purification GSflag
 *   6. Trigger the purification celebration cutscene
 */
void shadow_Purify(u16 shadowID, void* pokemon) {
    ShadowRegistration* reg;
    ShadowPokemonData*  data;
    u32 storedEXP;
    u32 currentEXP;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return;
    }
    if (pokemon == NULL) {
        return;
    }

    reg = &sShadowSystem.registry[shadowID - 1];
    data = shadow_GetData(shadowID);

    if (reg->state != SHADOW_STATE_SNAGGED) {
        return;
    }
    if (reg->currentGauge > HEART_GAUGE_PURIFIED) {
        return;
    }

    /* 1. Update registry state */
    reg->state = SHADOW_STATE_PURIFIED;
    reg->currentGauge = 0;

    /* 2. Convert stored EXP */
    storedEXP  = save_GetStoredEXP(pokemon);
    currentEXP = save_GetExperience(pokemon);
    save_SetExperience(pokemon, currentEXP + storedEXP);
    save_SetStoredEXP(pokemon, 0);

    /* 3. Replace Shadow Rush with a normal move */
    {
        u16 species = save_GetSpecies(pokemon);
        u8  level   = 0; /* Will use current level internally */
        u16 newMove = shadow_GetReplacementMove(species, level);

        if (data != NULL) {
            save_ReplaceMove(pokemon, data->shadowMoveID, newMove);
        }
    }

    /* 4. Clear shadow Pokemon ID */
    save_ClearShadowID(pokemon);
    save_SetPurifyCounter(pokemon, 0);

    /* 5. Set purification GSflag */
    if (data != NULL && data->purifyBonusFlag != 0) {
        GSflagSet32((s32)data->purifyBonusFlag);
    }

    /* 6. Trigger purification cutscene */
    scene_TriggerCutscene(0x01);  /* Relic Stone purification animation */
}

/**
 * Apply walking purification.
 *
 * Called by the overworld step counter for each shadow Pokemon
 * in the active party. The walk counter tracks total steps,
 * and each step reduces the heart gauge by PURIFY_WALK_AMOUNT
 * scaled by the Pokemon's nature walk rate.
 */
void shadow_WalkPurify(void* pokemon) {
    u16 shadowID;
    u8  nature;

    if (pokemon == NULL) {
        return;
    }

    shadowID = save_GetShadowID(pokemon);
    if (shadowID == SHADOW_ID_NONE) {
        return;
    }

    nature = save_GetNature(pokemon);
    shadow_ReduceHeartGauge(shadowID, PURIFY_WALK_AMOUNT, PURIFY_METHOD_WALK, nature);

    /* Sync back to save data */
    {
        s32 gauge = shadow_GetHeartGauge(shadowID);
        save_SetPurifyCounter(pokemon, gauge);
    }
}

/**
 * Apply cologne item purification.
 *
 * The three cologne items have different purification strengths:
 *   Joy Scent    (0xDC): 300 base reduction
 *   Excite Scent (0xDD): 600 base reduction
 *   Vivid Scent  (0xDE): 900 base reduction
 *
 * All are scaled by the Pokemon's nature cologne rate.
 */
void shadow_ColognePurify(void* pokemon, u16 itemID) {
    u16 shadowID;
    u8  nature;
    s32 amount;

    if (pokemon == NULL) {
        return;
    }

    shadowID = save_GetShadowID(pokemon);
    if (shadowID == SHADOW_ID_NONE) {
        return;
    }

    /* Determine reduction amount based on cologne type */
    switch (itemID) {
    case ITEM_JOY_SCENT:
        amount = PURIFY_JOY_SCENT_AMT;
        break;
    case ITEM_EXCITE_SCENT:
        amount = PURIFY_EXCITE_SCENT_AMT;
        break;
    case ITEM_VIVID_SCENT:
        amount = PURIFY_VIVID_SCENT_AMT;
        break;
    default:
        return;  /* Not a cologne item */
    }

    nature = save_GetNature(pokemon);
    shadow_ReduceHeartGauge(shadowID, amount, PURIFY_METHOD_COLOGNE, nature);

    /* Sync back to save data */
    {
        s32 gauge = shadow_GetHeartGauge(shadowID);
        save_SetPurifyCounter(pokemon, gauge);
    }
}

/**
 * Apply time-based passive purification.
 *
 * Called every frame. Internally maintains a frame counter and only
 * applies reduction every PURIFY_TIME_INTERVAL frames (30 seconds
 * at 60fps). All snagged shadow Pokemon in the party receive the
 * time reduction.
 *
 * Time purification is NOT modified by nature.
 */
void shadow_TimePurify(void) {
    u32 i;

    if (sShadowSystem.initialized == 0) {
        return;
    }

    sShadowSystem.timeCounter++;

    if (sShadowSystem.timeCounter < PURIFY_TIME_INTERVAL) {
        return;
    }

    /* Reset counter */
    sShadowSystem.timeCounter = 0;

    /* Apply time reduction to all snagged shadows */
    for (i = 0; i < SHADOW_POKEMON_COUNT; i++) {
        ShadowRegistration* reg = &sShadowSystem.registry[i];

        if (reg->state == SHADOW_STATE_SNAGGED && reg->currentGauge > 0) {
            reg->currentGauge -= PURIFY_TIME_AMOUNT;
            if (reg->currentGauge < 0) {
                reg->currentGauge = 0;
            }
        }
    }
}

/**
 * Apply battle purification.
 *
 * Called at the end of a battle for each shadow Pokemon that
 * participated. Reduces the heart gauge by PURIFY_BATTLE_AMOUNT,
 * scaled by the Pokemon's nature battle purification rate.
 */
void shadow_BattlePurify(void* pokemon) {
    u16 shadowID;
    u8  nature;

    if (pokemon == NULL) {
        return;
    }

    shadowID = save_GetShadowID(pokemon);
    if (shadowID == SHADOW_ID_NONE) {
        return;
    }

    nature = save_GetNature(pokemon);
    shadow_ReduceHeartGauge(shadowID, PURIFY_BATTLE_AMOUNT, PURIFY_METHOD_BATTLE, nature);

    /* Sync back to save data */
    {
        s32 gauge = shadow_GetHeartGauge(shadowID);
        save_SetPurifyCounter(pokemon, gauge);
    }
}

/* =========================================================================
 * Registration / Snagging
 * ========================================================================= */

/**
 * Register a shadow Pokemon encounter.
 *
 * Called when the player enters battle with a trainer who has a
 * shadow Pokemon. If the shadow has not been snagged yet, its state
 * is set to ENCOUNTERED and the encounter GSflag is set.
 */
void shadow_RegisterEncounter(u16 shadowID) {
    ShadowRegistration* reg;
    ShadowPokemonData*  data;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return;
    }

    reg = &sShadowSystem.registry[shadowID - 1];

    /* Don't overwrite snagged/purified state */
    if (reg->state >= SHADOW_STATE_SNAGGED) {
        return;
    }

    reg->state = SHADOW_STATE_ENCOUNTERED;
    reg->encounterCount++;

    /* Set the encounter GSflag */
    data = shadow_GetData(shadowID);
    if (data != NULL && data->encounterFlag != 0) {
        GSflagSet32((s32)data->encounterFlag);
    }
}

/**
 * Register that a shadow Pokemon has been snagged.
 *
 * Called after a successful snag. Sets the state to SNAGGED,
 * initializes the heart gauge tracking from the data table,
 * and fires the snag GSflag.
 */
void shadow_RegisterSnag(u16 shadowID) {
    ShadowRegistration* reg;
    ShadowPokemonData*  data;

    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return;
    }

    reg  = &sShadowSystem.registry[shadowID - 1];
    data = shadow_GetData(shadowID);

    reg->state = SHADOW_STATE_SNAGGED;

    /* Initialize heart gauge from data table */
    if (data != NULL) {
        reg->currentGauge = data->heartGaugeInitial;
        reg->maxGauge     = data->heartGaugeInitial;

        /* Set the snag GSflag */
        if (data->snagFlag != 0) {
            GSflagSet32((s32)data->snagFlag);
        }
    }
}

/**
 * Check if a shadow Pokemon has been snagged.
 */
BOOL shadow_IsSnagged(u16 shadowID) {
    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return FALSE;
    }
    return (sShadowSystem.registry[shadowID - 1].state >= SHADOW_STATE_SNAGGED);
}

/**
 * Check if a shadow Pokemon has been purified.
 */
BOOL shadow_IsPurified(u16 shadowID) {
    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return FALSE;
    }
    return (sShadowSystem.registry[shadowID - 1].state == SHADOW_STATE_PURIFIED);
}

/**
 * Get the shadow state for a given shadow ID.
 */
u8 shadow_GetState(u16 shadowID) {
    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return SHADOW_STATE_UNENCOUNTERED;
    }
    return sShadowSystem.registry[shadowID - 1].state;
}

/**
 * Get the shadow Pokemon data entry for a given shadow ID.
 * Shadow IDs are 1-based, so we subtract 1 for the array index.
 */
ShadowPokemonData* shadow_GetData(u16 shadowID) {
    if (shadowID < SHADOW_ID_MIN || shadowID > SHADOW_ID_MAX) {
        return NULL;
    }
    if (sShadowSystem.dataTable == NULL) {
        return NULL;
    }
    if ((u32)(shadowID - 1) >= sShadowSystem.dataCount) {
        return NULL;
    }
    return &sShadowSystem.dataTable[shadowID - 1];
}

/**
 * Get the total number of shadow Pokemon snagged so far.
 */
u32 shadow_GetSnagCount(void) {
    u32 count = 0;
    u32 i;

    for (i = 0; i < SHADOW_POKEMON_COUNT; i++) {
        if (sShadowSystem.registry[i].state >= SHADOW_STATE_SNAGGED) {
            count++;
        }
    }

    return count;
}

/**
 * Get the total number of shadow Pokemon purified so far.
 */
u32 shadow_GetPurifyCount(void) {
    u32 count = 0;
    u32 i;

    for (i = 0; i < SHADOW_POKEMON_COUNT; i++) {
        if (sShadowSystem.registry[i].state == SHADOW_STATE_PURIFIED) {
            count++;
        }
    }

    return count;
}

/**
 * Check if all 48 shadow Pokemon have been snagged.
 */
BOOL shadow_AllSnagged(void) {
    return (shadow_GetSnagCount() >= SHADOW_POKEMON_COUNT);
}

/**
 * Check if all 48 shadow Pokemon have been purified.
 * This is the condition for the Mt. Battle Ho-Oh reward.
 */
BOOL shadow_AllPurified(void) {
    return (shadow_GetPurifyCount() >= SHADOW_POKEMON_COUNT);
}

/* =========================================================================
 * Save / Load
 * ========================================================================= */

/**
 * Save shadow registration state to save data.
 *
 * Serializes the entire ShadowRegistration array into the
 * provided save buffer. The save system calls this during
 * the save-to-card operation.
 */
void shadow_SaveState(void* saveBuffer) {
    if (saveBuffer == NULL || !sShadowSystem.initialized) {
        return;
    }

    memcpy(saveBuffer, sShadowSystem.registry,
           sizeof(ShadowRegistration) * SHADOW_POKEMON_COUNT);
}

/**
 * Load shadow registration state from save data.
 *
 * Deserializes the ShadowRegistration array from the save buffer.
 * Called during the load-from-card operation.
 */
void shadow_LoadState(void* saveBuffer) {
    if (saveBuffer == NULL || !sShadowSystem.initialized) {
        return;
    }

    memcpy(sShadowSystem.registry, saveBuffer,
           sizeof(ShadowRegistration) * SHADOW_POKEMON_COUNT);
}
