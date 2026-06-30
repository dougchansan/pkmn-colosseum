/**
 * @file pokemon_data.c
 * @brief Pokemon stat data accessors via common_rel.
 *
 * Provides higher-level accessor functions for reading Pokemon base stat
 * data from the common_rel data tables. These wrap the raw
 * CommonRel_GetPokemonStats() call with field extraction.
 *
 * The Pokemon stats table is at common_rel offset 0x12336C, with each
 * entry being 0x11C (284) bytes. The entries are indexed by species ID
 * (0 = Bulbasaur, etc. following the National Dex order).
 *
 * Address context:
 *   These accessor functions appear as small (0x20-0x40 byte) functions
 *   scattered through the battle logic (0x801C53BC - 0x801D7230) and
 *   world/Pokemon data init (0x80180000 - 0x80192000) regions. They
 *   follow the pattern:
 *
 *     1. Load common_rel base pointer from SDA
 *     2. Load Pokemon stats table pointer from index table
 *     3. mulli rN, rSpecies, 0x11C
 *     4. add rN, rTableBase, rN
 *     5. lbz/lhz at the desired field offset
 *
 *   The base stats use odd byte offsets with padding gaps:
 *     0x85 = HP, 0x87 = Attack, 0x89 = Defense,
 *     0x8B = Sp.Attack, 0x8D = Sp.Defense, 0x8F = Speed
 *
 *   This unusual alignment suggests the original struct had u16 fields
 *   with the high byte unused, or a union with larger types.
 */

#include "game/data/common_rel.h"

/* ===================================================================
 * Base stat accessors
 *
 * Each follows the pattern: get the PokemonStats entry for the given
 * species, then read the specific byte field.
 *
 * In the disassembly these are tiny leaf functions:
 *   mulli r0, r3, 0x11C
 *   add   r3, r4, r0
 *   lbz   r3, <offset>(r3)
 *   blr
 * =================================================================== */

u8 PokemonData_GetBaseHP(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseHP;
}

u8 PokemonData_GetBaseAttack(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseAttack;
}

u8 PokemonData_GetBaseDefense(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseDefense;
}

u8 PokemonData_GetBaseSpAttack(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseSpAttack;
}

u8 PokemonData_GetBaseSpDefense(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseSpDefense;
}

u8 PokemonData_GetBaseSpeed(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseSpeed;
}

/* ===================================================================
 * Type accessors
 * =================================================================== */

u8 PokemonData_GetType1(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->type1;
}

u8 PokemonData_GetType2(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->type2;
}

/* ===================================================================
 * Ability accessors
 * =================================================================== */

u8 PokemonData_GetAbility1(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->ability1;
}

u8 PokemonData_GetAbility2(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->ability2;
}

/**
 * Get the ability for a specific slot (0 or 1).
 * If the ability slot is 1 and no second ability exists (ability2 == 0),
 * falls back to ability1. This matches the Gen III behavior.
 *
 * @param species     Pokemon species index
 * @param abilitySlot 0 for primary ability, 1 for secondary
 * @return            Ability ID
 */
u8 PokemonData_GetAbilityBySlot(u16 species, u8 abilitySlot) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }

    if (abilitySlot == 0) {
        return stats->ability1;
    }

    /* Slot 1: use ability2 if it exists, otherwise fall back to ability1 */
    if (stats->ability2 != 0) {
        return stats->ability2;
    }
    return stats->ability1;
}

/* ===================================================================
 * Misc stat accessors
 * =================================================================== */

u8 PokemonData_GetCatchRate(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->catchRate;
}

u8 PokemonData_GetBaseExpReward(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseExpReward;
}

u8 PokemonData_GetGenderRatio(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->genderRatio;
}

u8 PokemonData_GetBaseHappiness(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->baseHappiness;
}

u8 PokemonData_GetLevelUpRate(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->levelUpRate;
}

u8 PokemonData_GetEggGroup1(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->eggGroup1;
}

u8 PokemonData_GetEggGroup2(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->eggGroup2;
}

/* ===================================================================
 * Evolution data accessor
 *
 * Each Pokemon can have up to 2 evolution paths. Each path is 6 bytes:
 *   u16 method, u16 condition, u16 evolvedSpecies
 *
 * Located at offsets 0x9C (evo 1) and 0xA2 (evo 2) within the
 * PokemonStats entry.
 * =================================================================== */

/**
 * Get evolution data for a species.
 *
 * @param species   Pokemon species index
 * @param evoSlot   0 or 1 (first or second evolution path)
 * @param outMethod   Output: evolution method ID
 * @param outCondition Output: condition value (level, item, etc.)
 * @param outSpecies   Output: evolved species index
 * @return          TRUE if the evolution path is valid (method != 0)
 */
BOOL PokemonData_GetEvolution(u16 species, u8 evoSlot,
                               u16* outMethod, u16* outCondition,
                               u16* outSpecies) {
    PokemonStats* stats;
    u8* evoData;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return FALSE;
    }

    if (evoSlot == 0) {
        evoData = stats->evolution1;
    } else {
        evoData = stats->evolution2;
    }

    *outMethod    = (u16)((evoData[0] << 8) | evoData[1]);
    *outCondition = (u16)((evoData[2] << 8) | evoData[3]);
    *outSpecies   = (u16)((evoData[4] << 8) | evoData[5]);

    return (*outMethod != 0) ? TRUE : FALSE;
}

/* ===================================================================
 * Level-up move list accessor
 *
 * Each Pokemon has up to 20 level-up moves stored at offset 0xBA.
 * Each entry is 4 bytes: Level(1) + unused(1) + MoveID(2).
 * The list is terminated by a MoveID of 0.
 * =================================================================== */

/**
 * Get a level-up move entry for a species.
 *
 * @param species   Pokemon species index
 * @param moveSlot  Index into the level-up move list (0-19)
 * @param outLevel  Output: level at which the move is learned
 * @param outMoveID Output: move ID
 * @return          TRUE if the entry is valid (moveID != 0)
 */
BOOL PokemonData_GetLevelUpMove(u16 species, u8 moveSlot,
                                 u8* outLevel, u16* outMoveID) {
    PokemonStats* stats;
    u8* moveEntry;

    if (moveSlot >= 20) {
        return FALSE;
    }

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return FALSE;
    }

    moveEntry = &stats->levelUpMoves[moveSlot * 4];
    *outLevel  = moveEntry[0];
    *outMoveID = (u16)((moveEntry[2] << 8) | moveEntry[3]);

    return (*outMoveID != 0) ? TRUE : FALSE;
}

/* ===================================================================
 * String ID accessors
 * =================================================================== */

u16 PokemonData_GetNameStringID(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->nameStringID;
}

u16 PokemonData_GetSpeciesNameID(u16 species) {
    PokemonStats* stats;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return 0;
    }
    return stats->speciesNameID;
}

/* ===================================================================
 * TM Compatibility check
 *
 * TM compatibility is stored as a bitfield starting at offset 0x34
 * within the PokemonStats entry. Each bit corresponds to a TM/HM.
 *
 * Bit N corresponds to TM (N+1). The bitfield spans bytes 0x34-0x6D
 * (58 bytes = 464 bits, more than enough for 50 TMs + 8 HMs = 58).
 * =================================================================== */

/**
 * Check if a Pokemon can learn a specific TM/HM.
 *
 * @param species  Pokemon species index
 * @param tmIndex  TM index (0-based: TM01 = 0, HM01 = 50, etc.)
 * @return         TRUE if compatible, FALSE otherwise
 */
BOOL PokemonData_CanLearnTM(u16 species, u8 tmIndex) {
    PokemonStats* stats;
    u8 byteIndex;
    u8 bitIndex;

    stats = CommonRel_GetPokemonStats(species);
    if (stats == NULL) {
        return FALSE;
    }

    byteIndex = tmIndex / 8;
    bitIndex  = tmIndex % 8;

    if (byteIndex >= 0x3A) {
        return FALSE;
    }

    return (stats->tmCompatibility[byteIndex] & (1 << bitIndex)) ? TRUE : FALSE;
}
