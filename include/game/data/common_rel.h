/**
 * @file common_rel.h
 * @brief API for loading and accessing common_rel.fdat data tables.
 *
 * common_rel.fdat is the first file inside common.fsys and contains
 * the bulk of the game's data tables (Pokemon stats, moves, trainers,
 * items, natures, strings, etc.). The GoD Tool's CommonIndexes enum
 * (CMRelIndexes.swift) documents 108 index slots.
 *
 * The common_rel data is loaded at runtime into a heap allocation.
 * All table offsets are relative to the start of the loaded data.
 * Access is through an index table: each index slot stores a pointer
 * to a data table and (optionally) a count in a paired slot.
 *
 * Key offsets within common_rel.fdat:
 *   0x59890  - StringTable1
 *   0x92ED0  - Trainers (0x34 each, 819 entries)
 *   0x9FE28  - TrainerPokemonData (0x50 each, 5510 entries)
 *   0x11E048 - Moves (0x38 each, ~375 entries)
 *   0x12336C - PokemonStats (0x11C each, ~413 entries)
 *
 * The index table itself lives at a fixed offset near the start of
 * common_rel.fdat. Each index entry is a 32-bit pointer (relative
 * to the data base). Count entries are stored in the slot immediately
 * following the data pointer slot.
 */

#ifndef GAME_DATA_COMMON_REL_H
#define GAME_DATA_COMMON_REL_H

#include "dolphin/types.h"

/* ===================================================================
 * CommonIndexes -- index slots into the common_rel data tables.
 * Values from PekanMmd's GoD Tool (CMRelIndexes.swift).
 * =================================================================== */

typedef enum CommonRelIndex {
    COMMON_INDEX_LEGENDARY_POKEMON      =  2,
    COMMON_INDEX_LEGENDARY_COUNT        =  3,
    COMMON_INDEX_PEOPLE_IDS             =  6,
    COMMON_INDEX_ROOMS                  = 14,
    COMMON_INDEX_ROOMS_COUNT            = 15,
    COMMON_INDEX_TRAINER_CLASSES        = 24,
    COMMON_INDEX_TRAINER_CLASSES_COUNT  = 25,
    COMMON_INDEX_BATTLEFIELDS           = 28,
    COMMON_INDEX_BATTLEFIELDS_COUNT     = 29,
    COMMON_INDEX_BATTLE_TYPES           = 32,
    COMMON_INDEX_BATTLE_TYPES_COUNT     = 33,
    COMMON_INDEX_BATTLE_STYLES          = 42,
    COMMON_INDEX_BATTLE_STYLES_COUNT    = 43,
    COMMON_INDEX_TRAINERS               = 44,
    COMMON_INDEX_TRAINERS_COUNT         = 45,
    COMMON_INDEX_TRAINER_AI             = 46,
    COMMON_INDEX_TRAINER_AI_COUNT       = 47,
    COMMON_INDEX_TRAINER_POKEMON        = 48,
    COMMON_INDEX_TRAINER_POKEMON_COUNT  = 49,
    COMMON_INDEX_BATTLES                = 50,
    COMMON_INDEX_BATTLES_COUNT          = 51,
    COMMON_INDEX_SOUND_FILES            = 52,
    COMMON_INDEX_SOUND_FILES_COUNT      = 53,
    COMMON_INDEX_AI_WEIGHT_EFFECTS      = 56,
    COMMON_INDEX_AI_WEIGHT_EFFECTS_COUNT= 57,
    COMMON_INDEX_AI_POKEMON_ROLES       = 58,
    COMMON_INDEX_AI_POKEMON_ROLES_COUNT = 59,
    COMMON_INDEX_TREASURE_BOX           = 60,
    COMMON_INDEX_TREASURE_BOX_COUNT     = 61,
    COMMON_INDEX_MOVES                  = 62,
    COMMON_INDEX_MOVES_COUNT            = 63,
    COMMON_INDEX_NATURES                = 64,
    COMMON_INDEX_NATURES_COUNT          = 65,
    COMMON_INDEX_POKEMON_STATS          = 68,
    COMMON_INDEX_POKEMON_STATS_COUNT    = 69,
    COMMON_INDEX_NATURE_MULTIPLIERS     = 70,
    COMMON_INDEX_NATURE_MULTIPLIERS_COUNT= 71,
    COMMON_INDEX_CHARACTER_MODELS       = 72,
    COMMON_INDEX_CHARACTER_MODELS_COUNT = 73,
    COMMON_INDEX_SHADOW_DATA            = 80,
    COMMON_INDEX_SHADOW_DATA_COUNT      = 81,
    COMMON_INDEX_MET_LOCATIONS          = 82,
    COMMON_INDEX_MET_LOCATIONS_COUNT    = 83,
    COMMON_INDEX_INTERACTION_POINTS     = 86,
    COMMON_INDEX_INTERACTION_POINTS_COUNT= 87,
    COMMON_INDEX_STRING_TABLE_1         = 98,
    COMMON_INDEX_STRING_TABLE_2         = 99,
    COMMON_INDEX_STRING_TABLE_3         = 100,
    COMMON_INDEX_SCRIPT                 = 101,

    COMMON_INDEX_COUNT                  = 108
} CommonRelIndex;

/* ===================================================================
 * Struct sizes for key data tables
 * =================================================================== */

#define POKEMON_STATS_SIZE       0x11C   /* 284 bytes per Pokemon */
#define MOVE_DATA_SIZE           0x38    /* 56 bytes per move */
#define TRAINER_DATA_SIZE        0x34    /* 52 bytes per trainer */
#define TRAINER_POKEMON_SIZE     0x50    /* 80 bytes per trainer Pokemon */
#define NATURE_DATA_SIZE         0x28    /* 40 bytes per nature */
#define TRAINER_CLASS_SIZE       0x0C    /* 12 bytes per trainer class */

/* Maximum counts */
#define MAX_POKEMON_SPECIES      413
#define MAX_MOVES                375
#define MAX_TRAINERS             819
#define MAX_TRAINER_POKEMON      5510
#define MAX_NATURES              25

/* ===================================================================
 * Pokemon Stats Structure (0x11C bytes)
 *
 * Module 125 .data 0x121A04 (src/rel/common_rel/pokemon_data.c). Field
 * layout checked against the retail table (entry 1: catch rate 45,
 * Grass/Poison, Overgrow, base stats 45/49/49/65/65/45, evolves at 16).
 * =================================================================== */

typedef struct PokemonStatsEvolution {
    /* 0x0 */ u8   method;
    /* 0x1 */ u8   padding_1;
    /* 0x2 */ u16  condition;
    /* 0x4 */ u16  species;
} PokemonStatsEvolution; /* size: 0x6 */

typedef struct PokemonStatsLevelUpMove {
    /* 0x0 */ u8   level;
    /* 0x1 */ u8   padding_1;
    /* 0x2 */ u16  move;
} PokemonStatsLevelUpMove; /* size: 0x4 */

typedef struct PokemonStatsFace {
    /* 0x0 */ u8   unk0;
    /* 0x1 */ u8   unk1;
    /* 0x2 */ u16  species;
    /* 0x4 */ u32  resourceId;
} PokemonStatsFace; /* size: 0x8 */

typedef struct PokemonStats {
    /* 0x00 */ u8   levelUpRate;
    /* 0x01 */ u8   catchRate;
    /* 0x02 */ u8   genderRatio;
    /* 0x03 */ u8   unk3;
    /* 0x04 */ u8   unk4;
    /* 0x05 */ u8   unk5;
    /* 0x06 */ u16  baseExpReward;
    /* 0x08 */ u16  baseHappiness;
    /* 0x0A */ u16  unkA;
    /* 0x0C */ u16  unkC;
    /* 0x0E */ u16  unkE;
    /* 0x10 */ u16  unk10;
    /* 0x12 */ u16  unk12;
    /* 0x14 */ u16  unk14;
    /* 0x16 */ u16  eggCycles;
    /* 0x18 */ u32  nameStringID;
    /* 0x1C */ u32  unk1C;
    /* 0x20 */ u32  unk20;
    /* 0x24 */ u32  unk24;
    /* 0x28 */ u32  unk28;
    /* 0x2C */ u32  species;
    /* 0x30 */ u8   type1;
    /* 0x31 */ u8   type2;
    /* 0x32 */ u8   ability1;
    /* 0x33 */ u8   ability2;
    /* 0x34 */ u8   tmCompatibility[0x3A]; /* 50 TMs + 8 HMs, one byte each */
    /* 0x6E */ u8   eggGroup1;
    /* 0x6F */ u8   eggGroup2;
    /* 0x70 */ u16  wildItem1;
    /* 0x72 */ u16  wildItem2;
    /* 0x74 */ u16  eggMoves[8];
    /* 0x84 */ u16  baseStats[6];    /* HP/Atk/Def/SpA/SpD/Spe */
    /* 0x90 */ u16  evYield[6];      /* HP/Atk/Def/SpA/SpD/Spe */
    /* 0x9C */ PokemonStatsEvolution evolutions[5];
    /* 0xBA */ PokemonStatsLevelUpMove levelUpMoves[20];
    /* 0x10A*/ u16  unk10A;
    /* 0x10C*/ PokemonStatsFace faces[2];
} PokemonStats; /* size: 0x11C */

/* ===================================================================
 * Move Data Structure (0x38 bytes)
 *
 * Located at common_rel offset 0x11E048.
 * =================================================================== */

typedef struct CommonMoveData {
    /* 0x00 */ u8   priority;
    /* 0x01 */ u8   basePP;
    /* 0x02 */ u8   type;
    /* 0x03 */ u8   targets;
    /* 0x04 */ u8   accuracy;
    /* 0x05 */ u8   effectAccuracy;
    /* 0x06 */ u8   makesContact;
    /* 0x07 */ u8   blockedByProtect;
    /* 0x08 */ u8   magicCoatReflects;
    /* 0x09 */ u8   snatchSteals;
    /* 0x0A */ u8   mirrorMoveCopies;
    /* 0x0B */ u8   kingsRockFlinch;
    /* 0x0C */ u8   unk0C[4];
    /* 0x10 */ u8   soundBased;
    /* 0x11 */ u8   unk11;
    /* 0x12 */ u8   hmFlag;
    /* 0x13 */ u8   recoil;
    /* 0x14 */ u8   unk14[3];
    /* 0x17 */ u8   basePower;
    /* 0x18 */ u8   padding_18[3];
    /* 0x1B */ u8   effect;
    /* 0x1C */ u16  effectID;
    /* 0x1E */ u8   padding_1E[4];
    /* 0x22 */ u16  nameStringID;
    /* 0x24 */ u8   unk24[0x0A];
    /* 0x2E */ u16  descriptionID;
    /* 0x30 */ u8   padding_30[2];
    /* 0x32 */ u16  animationID;
    /* 0x34 */ u8   unk34[4];
} CommonMoveData; /* size: 0x38 */

/* ===================================================================
 * Trainer Data Structure (0x34 bytes)
 *
 * Located at common_rel offset 0x92ED0.
 * =================================================================== */

typedef struct CommonTrainerData {
    /* 0x00 */ u8   gender;
    /* 0x01 */ u8   padding_01;
    /* 0x02 */ u8   padding_02;
    /* 0x03 */ u8   trainerClass;     /* low byte; paired with 0x02 for u16 */
    /* 0x04 */ u16  firstPokemonIndex;
    /* 0x06 */ u16  aiValue;
    /* 0x08 */ u32  nameStringID;
    /* 0x0C */ u32  openingAnimIndex;
    /* 0x10 */ u32  modelIndex;
    /* 0x14 */ u16  items[8];         /* 8 held item slots */
    /* 0x24 */ u32  preBattleString;
    /* 0x28 */ u32  winString;
    /* 0x2C */ u32  loseString;
    /* 0x30 */ u32  altLoseString;
} CommonTrainerData; /* size: 0x34 */

/* ===================================================================
 * Trainer Pokemon Structure (0x50 bytes)
 *
 * Located at common_rel offset 0x9FE28.
 * =================================================================== */

typedef struct CommonTrainerPokemon {
    /* 0x00 */ u8   abilitySlot;
    /* 0x01 */ u8   gender;
    /* 0x02 */ u8   nature;
    /* 0x03 */ u8   shadowID;
    /* 0x04 */ u8   level;
    /* 0x05 */ u8   sendOutPriority;
    /* 0x06 */ u8   unk06[4];
    /* 0x0A */ u16  species;
    /* 0x0C */ u16  pokeBall;
    /* 0x0E */ u8   padding_0E[2];
    /* 0x10 */ u16  itemFlag;
    /* 0x12 */ u16  heldItem;
    /* 0x14 */ u32  nicknameStringID;
    /* 0x18 */ u8   unk18[4];
    /* 0x1C */ u8   ivHP;
    /* 0x1D */ u8   ivAtk;
    /* 0x1E */ u8   ivDef;
    /* 0x1F */ u8   ivSpAtk;
    /* 0x20 */ u8   ivSpDef;
    /* 0x21 */ u8   ivSpe;
    /* 0x22 */ u16  evHP;
    /* 0x24 */ u16  evAtk;
    /* 0x26 */ u16  evDef;
    /* 0x28 */ u16  evSpAtk;
    /* 0x2A */ u16  evSpDef;
    /* 0x2C */ u16  evSpe;
    /* 0x2E */ u8   padding_2E[2];
    /* 0x30 */ u8   move1PPBonuses;
    /* 0x31 */ u8   unk31[5];
    /* 0x36 */ u16  move1;
    /* 0x38 */ u8   move2PPBonuses;
    /* 0x39 */ u8   unk39[5];
    /* 0x3E */ u16  move2;
    /* 0x40 */ u8   move3PPBonuses;
    /* 0x41 */ u8   unk41[5];
    /* 0x46 */ u16  move3;
    /* 0x48 */ u8   move4PPBonuses;
    /* 0x49 */ u8   unk49[5];
    /* 0x4E */ u16  move4;
} CommonTrainerPokemon; /* size: 0x50 */

/* ===================================================================
 * Nature Data Structure (0x28 bytes)
 * =================================================================== */

typedef struct CommonNatureData {
    /* 0x00 */ u8   purifyBattle;
    /* 0x01 */ u8   purifyWalk;
    /* 0x02 */ u8   purifyCall;
    /* 0x03 */ u8   purifyDayCare;
    /* 0x04 */ u8   purifyCologne;
    /* 0x05 */ s8   attackMod;
    /* 0x06 */ s8   defenseMod;
    /* 0x07 */ s8   spAttackMod;
    /* 0x08 */ s8   spDefenseMod;
    /* 0x09 */ s8   speedMod;
    /* 0x0A */ u8   unk0A[0x0C];
    /* 0x16 */ u16  nameStringID;
    /* 0x18 */ u8   unk18[0x10];
} CommonNatureData; /* size: 0x28 */

/* ===================================================================
 * Runtime common_rel state
 *
 * The loaded common_rel data is stored in a global pointer. The index
 * table at the start of the data provides offsets to each data table.
 * =================================================================== */

/**
 * Common rel data manager -- holds the base pointer to the loaded
 * common_rel.fdat data and the resolved index table pointers.
 */
typedef struct CommonRelData {
    void*  base;                         /* base pointer to loaded data */
    void*  indexTable[COMMON_INDEX_COUNT]; /* resolved pointers for each index */
} CommonRelData;

/* ===================================================================
 * Function declarations
 * =================================================================== */

/**
 * Initialize the common_rel data system.
 * Triggers loading of common.fsys and extracts common_rel.fdat.
 * Called during GameMainLoop initialization.
 */
void CommonRel_Init(void);

/**
 * Get the base pointer to the loaded common_rel data.
 * Returns NULL if common_rel has not been loaded yet.
 */
void* CommonRel_GetBase(void);

/**
 * Get a pointer to a data table by CommonRelIndex.
 * The returned pointer points directly into the loaded common_rel data.
 *
 * @param index  CommonRelIndex value (0-107)
 * @return       Pointer to the data table, or NULL if not loaded
 */
void* CommonRel_GetDataTable(s32 index);

/**
 * Get the entry count for a data table.
 * Many tables store their count in the index slot immediately after
 * the data pointer slot (e.g., index 44 = Trainers data, 45 = count).
 *
 * @param countIndex  The index slot that holds the count
 * @return            Number of entries, or 0 if not available
 */
u32 CommonRel_GetTableCount(s32 countIndex);

/**
 * Get a specific entry from a data table by index.
 * Computes: base_ptr + (entryIndex * entrySize).
 *
 * @param tableIndex  CommonRelIndex for the data table
 * @param entryIndex  Index of the entry within the table
 * @param entrySize   Size of each entry in bytes
 * @return            Pointer to the entry, or NULL
 */
void* CommonRel_GetEntry(s32 tableIndex, s32 entryIndex, u32 entrySize);

/* ===================================================================
 * Pokemon Stats accessors
 * =================================================================== */

PokemonStats* CommonRel_GetPokemonStats(u16 species);
u32           CommonRel_GetPokemonStatsCount(void);

/* ===================================================================
 * Move Data accessors
 * =================================================================== */

CommonMoveData* CommonRel_GetMoveData(u16 moveID);
u32             CommonRel_GetMoveCount(void);

/* ===================================================================
 * Trainer Data accessors
 * =================================================================== */

CommonTrainerData*    CommonRel_GetTrainerData(u16 trainerID);
u32                   CommonRel_GetTrainerCount(void);
CommonTrainerPokemon* CommonRel_GetTrainerPokemon(u16 index);
u32                   CommonRel_GetTrainerPokemonCount(void);

/* ===================================================================
 * Nature Data accessors
 * =================================================================== */

CommonNatureData* CommonRel_GetNatureData(u8 nature);

#endif /* GAME_DATA_COMMON_REL_H */
