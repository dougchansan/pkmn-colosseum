/**
 * @file common_rel.c
 * @brief Runtime loader and index accessor for common_rel.fdat data.
 *
 * common_rel.fdat is the first file inside common.fsys. It contains
 * the game's core data tables: Pokemon stats, moves, trainers,
 * natures, type matchups, shadow data, string tables, and more.
 *
 * The data is structured as:
 *   1. A header with offsets to an index table
 *   2. An index table of 108 entries (each a 32-bit offset)
 *   3. Data tables at the indexed offsets
 *
 * The game loads common.fsys early during initialization via the FSYS
 * system, then resolves the index table to get pointers to each data
 * table. Count values for tables are stored in adjacent index slots
 * (e.g., index 44 = Trainers data, index 45 = Trainers count).
 *
 * Address context:
 *   The common_rel loading is triggered during GameMainLoop's
 *   subsystem initialization phase. The accessor functions are
 *   small utility functions scattered through the battle and world
 *   code that use index-based lookups with struct size multiplications.
 *
 * Key patterns found in the disassembly:
 *   mulli rN, rN, 0x11C  -- Pokemon stats lookup (species * 0x11C)
 *   mulli rN, rN, 0x38   -- Move data lookup (moveID * 0x38)
 *   mulli rN, rN, 0x34   -- Trainer data lookup (trainerID * 0x34)
 *   mulli rN, rN, 0x50   -- Trainer Pokemon lookup (index * 0x50)
 *
 * The index table resolution pattern:
 *   lwz   rN, indexTable(rBase)    ; load index table pointer
 *   slwi  rM, rIndex, 2           ; index * 4
 *   lwzx  rN, rN, rM              ; load offset from index table
 *   add   rN, rBase, rN           ; base + offset = data pointer
 */

#include "game/data/common_rel.h"

/* ===================================================================
 * External SDK / engine functions
 * =================================================================== */

extern void* memset(void* dst, int val, u32 size);

/* FSYS archive loading */
extern s32   FSYSLoadArchive(u32 fileHandle, u32 requestID);
extern s32   FSYSCheckFileLoaded(u32 fileHandle, u32 nameHash);
extern void* fn_800F9318(u32 fileHandle, u32 fileID); /* resolve loaded data ptr */

/* Debug output */
extern void  fn_800DD970(const char* fmt, ...); /* OSReport */

/* ===================================================================
 * Global state
 *
 * The common_rel data pointer lives in the small data area (.sbss).
 * It is set once during init and read everywhere else.
 * =================================================================== */

/**
 * Pointer to the loaded common_rel.fdat data base.
 * All index table offsets are relative to this address.
 * Set during CommonRel_Init after common.fsys is loaded.
 *
 * Likely at an SDA offset near lbl_8047B1xx or in BSS.
 */
static void* sCommonRelBase = NULL;

/**
 * Resolved pointers for each of the 108 index slots.
 * Computed once after the data is loaded by adding the
 * offset from the index table to sCommonRelBase.
 */
static void* sIndexPointers[COMMON_INDEX_COUNT];

/**
 * Whether the common_rel data has been fully loaded and resolved.
 */
static u8 sCommonRelReady = 0;

/* ===================================================================
 * fn_XXXXXXXX: CommonRel_Init
 *
 * Called during the game init sequence to trigger loading of
 * common.fsys and extract the common_rel.fdat data. The loading
 * is asynchronous via the FSYS system; this function initiates
 * the request and the game polls for completion.
 *
 * Once loaded, the index table at the start of the data is
 * resolved into direct pointers for fast access.
 * =================================================================== */
void CommonRel_Init(void) {
    sCommonRelBase = NULL;
    sCommonRelReady = 0;
    memset(sIndexPointers, 0, sizeof(sIndexPointers));
}

/* ===================================================================
 * CommonRel_ResolveIndexTable
 *
 * After common_rel.fdat is loaded into memory, this function walks
 * the index table and resolves each slot's offset to a direct pointer.
 *
 * The index table is located at a fixed offset within the file header.
 * Each entry is a 32-bit offset from the start of the file.
 *
 * Pattern from disassembly:
 *   lwz   r3, 0x10(rBase)     ; offset to index table
 *   add   r3, rBase, r3       ; indexTablePtr = base + offset
 *   lwz   r4, 0(r3)           ; first entry offset
 *   add   r4, rBase, r4       ; resolved pointer
 * =================================================================== */
static void CommonRel_ResolveIndexTable(void* base) {
    u32* header;
    u32  indexTableOffset;
    u32* indexTable;
    u32  i;

    if (base == NULL) {
        return;
    }

    sCommonRelBase = base;
    header = (u32*)base;

    /*
     * The common_rel header contains:
     *   +0x00: magic / identifier
     *   +0x04: version
     *   +0x08: number of index entries
     *   +0x0C: flags
     *   +0x10: offset to the index table
     *
     * Each index entry is a 32-bit offset. A zero offset means
     * the slot is unused.
     */
    indexTableOffset = header[4]; /* offset 0x10 */
    indexTable = (u32*)((u8*)base + indexTableOffset);

    for (i = 0; i < COMMON_INDEX_COUNT; i++) {
        u32 entryOffset = indexTable[i];

        if (entryOffset != 0) {
            sIndexPointers[i] = (void*)((u8*)base + entryOffset);
        } else {
            sIndexPointers[i] = NULL;
        }
    }

    sCommonRelReady = 1;
}

/* ===================================================================
 * CommonRel_SetData
 *
 * Called by the FSYS completion callback or polling loop once
 * common_rel.fdat has been loaded. Sets the base pointer and
 * resolves the index table.
 * =================================================================== */
void CommonRel_SetData(void* data) {
    if (data != NULL) {
        CommonRel_ResolveIndexTable(data);
    }
}

/* ===================================================================
 * CommonRel_GetBase
 *
 * Returns the base pointer to the loaded common_rel.fdat data.
 * =================================================================== */
void* CommonRel_GetBase(void) {
    return sCommonRelBase;
}

/* ===================================================================
 * CommonRel_GetDataTable
 *
 * Returns a pointer to the data table for the given index.
 * The index value must be one of the CommonRelIndex enum values.
 *
 * This is the fundamental accessor used by all game systems to
 * reach into the common_rel data. The disassembly shows this as
 * a simple array dereference:
 *
 *   slwi  r0, rIndex, 2
 *   lwzx  r3, rTableBase, r0
 * =================================================================== */
void* CommonRel_GetDataTable(s32 index) {
    if (sCommonRelReady == 0) {
        return NULL;
    }
    if (index < 0 || index >= COMMON_INDEX_COUNT) {
        return NULL;
    }
    return sIndexPointers[index];
}

/* ===================================================================
 * CommonRel_GetTableCount
 *
 * Returns the entry count stored at the given index slot.
 * Count slots are stored as 32-bit values in the index table.
 * The convention is that the count for table at index N is at
 * index N+1 (e.g., Trainers at 44, Trainers count at 45).
 * =================================================================== */
u32 CommonRel_GetTableCount(s32 countIndex) {
    void* countPtr;

    if (sCommonRelReady == 0) {
        return 0;
    }
    if (countIndex < 0 || countIndex >= COMMON_INDEX_COUNT) {
        return 0;
    }

    countPtr = sIndexPointers[countIndex];
    if (countPtr == NULL) {
        return 0;
    }

    /* The count entry stores the count value directly as a u32
     * at the resolved address. */
    return *(u32*)countPtr;
}

/* ===================================================================
 * CommonRel_GetEntry
 *
 * Generic entry accessor. Computes:
 *   entry_ptr = table_base + (entryIndex * entrySize)
 *
 * This pattern appears frequently in the disassembly as:
 *   mulli rN, rIndex, <size>
 *   add   rN, rTableBase, rN
 *
 * Examples:
 *   mulli r0, r3, 0x11C   ; Pokemon stats
 *   mulli r0, r3, 0x38    ; Move data
 *   mulli r0, r3, 0x34    ; Trainer data
 *   mulli r0, r3, 0x50    ; Trainer Pokemon
 * =================================================================== */
void* CommonRel_GetEntry(s32 tableIndex, s32 entryIndex, u32 entrySize) {
    void* tableBase;

    tableBase = CommonRel_GetDataTable(tableIndex);
    if (tableBase == NULL) {
        return NULL;
    }

    if (entryIndex < 0) {
        return NULL;
    }

    return (void*)((u8*)tableBase + ((u32)entryIndex * entrySize));
}

/* ===================================================================
 * Pokemon Stats accessors
 *
 * Pattern: mulli rN, rSpecies, 0x11C
 *          add   rN, rPokemonStatsBase, rN
 * =================================================================== */

PokemonStats* CommonRel_GetPokemonStats(u16 species) {
    return (PokemonStats*)CommonRel_GetEntry(
        COMMON_INDEX_POKEMON_STATS,
        (s32)species,
        POKEMON_STATS_SIZE
    );
}

u32 CommonRel_GetPokemonStatsCount(void) {
    return CommonRel_GetTableCount(COMMON_INDEX_POKEMON_STATS_COUNT);
}

/* ===================================================================
 * Move Data accessors
 *
 * Pattern: mulli rN, rMoveID, 0x38
 *          add   rN, rMoveDataBase, rN
 * =================================================================== */

CommonMoveData* CommonRel_GetMoveData(u16 moveID) {
    return (CommonMoveData*)CommonRel_GetEntry(
        COMMON_INDEX_MOVES,
        (s32)moveID,
        MOVE_DATA_SIZE
    );
}

u32 CommonRel_GetMoveCount(void) {
    return CommonRel_GetTableCount(COMMON_INDEX_MOVES_COUNT);
}

/* ===================================================================
 * Trainer Data accessors
 *
 * Pattern: mulli rN, rTrainerID, 0x34
 *          add   rN, rTrainerDataBase, rN
 * =================================================================== */

CommonTrainerData* CommonRel_GetTrainerData(u16 trainerID) {
    return (CommonTrainerData*)CommonRel_GetEntry(
        COMMON_INDEX_TRAINERS,
        (s32)trainerID,
        TRAINER_DATA_SIZE
    );
}

u32 CommonRel_GetTrainerCount(void) {
    return CommonRel_GetTableCount(COMMON_INDEX_TRAINERS_COUNT);
}

/* ===================================================================
 * Trainer Pokemon accessors
 *
 * Pattern: mulli rN, rIndex, 0x50
 *          add   rN, rTrainerPokemonBase, rN
 * =================================================================== */

CommonTrainerPokemon* CommonRel_GetTrainerPokemon(u16 index) {
    return (CommonTrainerPokemon*)CommonRel_GetEntry(
        COMMON_INDEX_TRAINER_POKEMON,
        (s32)index,
        TRAINER_POKEMON_SIZE
    );
}

u32 CommonRel_GetTrainerPokemonCount(void) {
    return CommonRel_GetTableCount(COMMON_INDEX_TRAINER_POKEMON_COUNT);
}

/* ===================================================================
 * Nature Data accessors
 * =================================================================== */

CommonNatureData* CommonRel_GetNatureData(u8 nature) {
    if (nature >= MAX_NATURES) {
        return NULL;
    }
    return (CommonNatureData*)CommonRel_GetEntry(
        COMMON_INDEX_NATURES,
        (s32)nature,
        NATURE_DATA_SIZE
    );
}
