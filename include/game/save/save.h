#ifndef GAME_SAVE_H
#define GAME_SAVE_H

#include "dolphin/types.h"

/**
 * @file save.h
 * @brief Save file structures and declarations for Pokemon Colosseum.
 *
 * Colosseum save files use the GCI format (GameCube memory card image,
 * compatible with Dolphin emulator). The game stores save data on a
 * standard GameCube Memory Card via the Dolphin SDK CARD API.
 *
 * Save data integrity is verified using a SHA-1 hash (sha1Transform).
 *
 * The save system is initialized at boot from main.c:
 *   vtrInit - Card system init
 *   GSmovieInit - Save data init
 *   GSvtrRegisterGSgapp - Save system post-init
 *
 * Per-frame updates via:
 *   fn_801E0FB4 - Save/card per-frame update
 *   GSvtrLoadTexture - Card system tick
 *   vtrIsActive - Card system check pending
 *
 * Related source files:
 *   cardesavedata.c  - Card e-Reader save data (0x80082650 - 0x80083AF4)
 *   menuCB_SaveLoad.c - Save/Load menu callbacks
 *
 * Related FSYS archives on disc:
 *   save_menu.fsys    - Save menu UI assets
 *   prog_memcard.fsys - Memory card program assets
 */

/* =========================================================================
 * Constants
 * ========================================================================= */

/* GCI file header constants */
#define SAVE_GAME_CODE       "GC6E"    /* Game code for NTSC-U Colosseum */
#define SAVE_MAKER_CODE      "01"      /* Nintendo maker code */
#define SAVE_BANNER_FMT      0x02      /* CI8 banner format */
#define SAVE_ICON_FMT        0x02      /* CI8 icon format */
#define SAVE_ICON_SPEED      0x03      /* Icon animation speed */
#define SAVE_COMMENT_SIZE    64        /* Comment field size in GCI header */

/* Memory card block sizes */
#define CARD_BLOCK_SIZE      0x2000    /* 8192 bytes per block */
#define CARD_SECTOR_SIZE     0x200     /* 512 bytes per sector */

/* Save file layout */
#define SAVE_HEADER_SIZE     0x40      /* File header with magic and hash */
#define SAVE_DATA_OFFSET     0x40      /* Offset where save data begins */
#define SAVE_HASH_SIZE       20        /* SHA-1 hash output size (160 bits) */

/* Maximum party/box sizes */
#define SAVE_PARTY_SIZE      6         /* Max Pokemon in party */
#define SAVE_PC_BOX_COUNT    3         /* Number of PC boxes */
#define SAVE_PC_BOX_SLOTS    30        /* Slots per PC box */

/* Save Pokemon structure size (from community research / LibPkmGC) */
#define SAVE_POKEMON_SIZE    0x138     /* 312 bytes per Pokemon in save */

/* Item limits (from pokeconv.c assert: "cp->pc_items_num == 50 || cp->pc_items_num == 30") */
#define SAVE_BAG_ITEMS_MAX   30        /* Max items in bag */
#define SAVE_PC_ITEMS_MAX    50        /* Max items in PC storage */

/* Card e-Reader constants */
#define CARDE_CARDTYPE_TRAINER   0     /* e-Reader card type: trainer card */
#define CARDE_MAX_SERIES         8     /* Maximum number of card series */

/* Card e-Reader trainer states (from assert strings) */
#define CARDE_EX_TRAINER_STATE_APPEARING           0
#define CARDE_EX_TRAINER_STATE_ALREADY_BATTLED_WITH 1

/* =========================================================================
 * Save file header
 * ========================================================================= */

/**
 * Save file header, stored at offset 0x00 of save data.
 * The SHA-1 hash covers all data after the header.
 */
typedef struct SaveHeader {
    /* 0x00 */ u32  magic;               /* Magic identifier */
    /* 0x04 */ u32  version;             /* Save format version */
    /* 0x08 */ u32  saveCount;           /* Number of times saved (monotonic) */
    /* 0x0C */ u32  dataSize;            /* Size of data following header */
    /* 0x10 */ u8   sha1Hash[20];        /* SHA-1 hash of save data */
    /* 0x24 */ u8   reserved[28];        /* Padding to 0x40 */
} SaveHeader;

/* =========================================================================
 * Save file Pokemon structure (0x138 bytes)
 * ========================================================================= */

/**
 * Pokemon as stored in the save file.
 * This is the persistent representation, distinct from the in-battle
 * BattlePokemon structure (0xE0 bytes) and the common_rel stats table.
 *
 * Documented from TuxSH's PkmGCTools / LibPkmGC.
 * Size: 0x138 (312) bytes.
 */
typedef struct SavePokemon {
    /* 0x00 */ u16  species;             /* Pokemon species index */
    /* 0x02 */ u16  unknown_02;          /* 0 on shadow Pokemon */
    /* 0x04 */ u32  personalityValue;    /* PID - Personality ID */
    /* 0x08 */ u32  versionInfo;         /* Game version metadata */
    /* 0x0C */ u16  locationCaught;      /* Met location ID */
    /* 0x0E */ u8   levelMet;            /* Level when caught */
    /* 0x0F */ u8   ballCaughtWith;      /* Poke Ball type */
    /* 0x10 */ u8   otGender;            /* Original Trainer gender */
    /* 0x11 */ u8   padding_11[3];
    /* 0x14 */ u16  secretID;            /* Secret ID */
    /* 0x16 */ u16  trainerID;           /* Trainer ID */
    /* 0x18 */ u8   otName[22];          /* Original Trainer name (Unicode) */
    /* 0x2E */ u8   pokemonName[22];     /* Nickname (Unicode) */
    /* 0x44 */ u8   padding_44[0x18];
    /* 0x5C */ u32  experience;          /* Current EXP */
    /* 0x60 */ u8   currentLevel;        /* Current level (derived from EXP) */
    /* 0x61 */ u8   padding_61[0x17];
    /* 0x78 */ u8   movesInfo[16];       /* 4 moves with PP data */
    /* 0x88 */ u16  heldItem;            /* Item index */
    /* 0x8A */ u16  currentHP;           /* Current hit points */
    /* 0x8C */ u16  stats[6];            /* HP/Atk/Def/SpA/SpD/Spe */
    /* 0x98 */ u16  evs[6];             /* Effort Values */
    /* 0xA4 */ u16  ivs[6];             /* Individual Values */
    /* 0xB0 */ u16  happiness;           /* Friendship value */
    /* 0xB2 */ u8   contestStats[5];     /* Cool/Beauty/Cute/Smart/Tough */
    /* 0xB7 */ u8   contestRibbons[5];   /* Contest ribbon data */
    /* 0xBC */ u8   contestLuster;       /* Luster/sheen value */
    /* 0xBD */ u8   specialRibbons[12];  /* Special ribbon flags */
    /* 0xC9 */ u8   padding_C9;
    /* 0xCA */ u8   pokerusStatus;       /* Infection status */
    /* 0xCB */ u8   flags[3];            /* Egg/ability/validity flags */
    /* 0xCE */ u8   padding_CE[0x0A];
    /* 0xD8 */ u16  shadowPokemonID;     /* Shadow ID (0 if not shadow) */
    /* 0xDA */ u16  padding_DA;
    /* 0xDC */ s32  purificationCounter; /* Shadow purification progress */
    /* 0xE0 */ u32  expStored;           /* Experience stored during shadow */
    /* 0xE4 */ u8   padding_E4[0x14];
    /* 0xF8 */ u8   obedient;            /* Obedience flag */
    /* 0xF9 */ u8   padding_F9[2];
    /* 0xFB */ u8   encounterType;       /* How the Pokemon was obtained */
    /* 0xFC */ u8   padding_FC[0x3C];
} SavePokemon;

/* =========================================================================
 * Save item structure
 * ========================================================================= */

typedef struct SaveItem {
    /* 0x00 */ u16  itemID;              /* Item index */
    /* 0x02 */ u16  quantity;            /* Stack count */
} SaveItem;

/* =========================================================================
 * Card e-Reader save data structures
 * ========================================================================= */

/**
 * Card e-Reader series definition.
 * Contains metadata about an e-Reader card series.
 */
typedef struct CardESeries {
    /* 0x00 */ u8   seriesID;            /* Series identifier */
    /* 0x01 */ u8   padding_01[7];
    /* 0x08 */ u8   seriesNumber;        /* Series number for validation */
    /* 0x09 */ u8   padding_09[0x17];
    /* 0x1A */ u8   levelMin;            /* Minimum level for series */
    /* 0x1B */ s8   levelMax;            /* Maximum level count */
    /* 0x1C */ u8   gridWidth;           /* Card matrix grid width */
    /* 0x1D */ u8   gridHeight;          /* Card matrix grid height */
    /* 0x1E */ u8   padding_1E[6];
    /* 0x24 */ u8   packMax;             /* Max packs in series */
    /* 0x25 */ u8   padding_25;
    /* 0x26 */ u8   trainerCardMax;      /* Max trainer cards */
} CardESeries;

/**
 * Card e-Reader matrix entry.
 * Each entry in the card grid tracks whether a card has been scanned.
 * Stride: 0x10 bytes per entry, with the valid flag at offset 0x82
 * from the base of the grid data.
 */
typedef struct CardEMatrixEntry {
    /* 0x00 */ u16  cardID;              /* Card identifier */
    /* 0x02 */ u8   padding_02[0x0A];
    /* 0x0C */ u8   isValid;             /* Non-zero if card is scanned */
    /* 0x0D */ u8   padding_0D[3];
} CardEMatrixEntry;

/* =========================================================================
 * Function declarations - Save file management
 * ========================================================================= */

/**
 * vtrInit: Initialize the card/save system.
 * Called from GameInit() during boot.
 * Sets up memory card communication and file handles.
 */
void save_CardSystemInit(void);

/**
 * GSmovieInit: Initialize save data structures.
 * Called from GameInit() after card system init.
 * Allocates and zeroes save data buffers.
 */
void save_DataInit(void);

/**
 * GSvtrRegisterGSgapp: Post-initialization for save system.
 * Called from GameInit() after task registration.
 * Finalizes card system state.
 */
void save_PostInit(void);

/**
 * fn_801E0FB4: Per-frame save/card update.
 * Called from TaskVBlank with mode, flags.
 * Processes pending card operations.
 *
 * @param mode    Operation mode (0x10 normal, -1 if lbl_80478820 is set)
 * @param param1  Always 1
 * @param param2  Always 1
 */
void save_CardUpdate(s32 mode, u32 param1, u32 param2);

/**
 * GSvtrLoadTexture: Card system tick.
 * Called from GameMainLoop per frame.
 * Advances card state machine.
 */
void save_CardTick(void);

/**
 * vtrIsActive: Check if card operations are pending.
 * @return Non-zero if operations are in progress
 */
u32 save_CardCheckPending(void);

/**
 * vtrGetMode: Get current card system state.
 * @return State code
 */
u32 save_CardGetState(void);

/* =========================================================================
 * Function declarations - SHA-1 integrity verification
 * ========================================================================= */

/**
 * sha1Transform: SHA-1 hash computation.
 * Confirmed by 0x5A827999 constants in disassembly.
 * Size: 0x1784 bytes (heavily unrolled).
 *
 * Uses a static work buffer at lbl_804670E8 (0x40 bytes, SHA-1 state).
 *
 * @param ctx  Pointer to SHA-1 context / input data
 */
void save_SHA1Process(void* ctx);

/* =========================================================================
 * Function declarations - Card e-Reader save data (cardesavedata.c)
 * ========================================================================= */

/**
 * fn_80082650: Validate card e-Reader level data.
 * Checks that the card series data pointer is non-null and that
 * the level count is positive. Scans the card matrix grid for
 * any entry with a valid flag set at offset 0x82.
 * If no valid entries found and the caller passes 0, clears the
 * species field to 0.
 *
 * @param pCardE  Pointer to card e-Reader data structure
 */
void cardesavedata_ValidateLevel(void* pCardE);

/**
 * cardeSaveRemoveCard: Check and validate a card e-Reader series.
 * Validates series number matches, level bounds, and grid dimensions.
 * If the caller passes a series index of 0 (first series with no cards
 * scanned), returns 1 indicating the series is "clear" (empty).
 *
 * @param pCardE    Pointer to card e-Reader data
 * @param pSeries   Pointer to series definition
 * @param seriesIdx Series index to check
 * @return 1 if series is valid/clear, 0 otherwise
 */
s32 cardesavedata_CheckSeries(void* pCardE, CardESeries* pSeries, s8 seriesIdx);

/**
 * cardeSaveClearSummary - fn_800836AC: Additional card e-Reader save functions.
 * These handle reading/writing specific card data fields,
 * clearing entries, and serialization of card grid state.
 * Total: 9 functions (Func3 through Func11).
 */
void cardesavedata_Func3(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func4(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func5(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func6(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func7(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func8(void* pCardE, void* pSeries);
void cardesavedata_Func9(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func10(void* pCardE, void* pSeries, s8 idx);
void cardesavedata_Func11(void* pCardE, void* pSeries, s8 idx);

#endif /* GAME_SAVE_H */
