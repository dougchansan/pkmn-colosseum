/**
 * @file save_data.c
 * @brief Save data structures and serialization (cardesavedata.c).
 *
 * This file corresponds to the original cardesavedata.c source file,
 * confirmed by the string "cardesavedata.c" at lbl_8026F1C8 in rodata.
 *
 * Address range: 0x80082650 - 0x80083AF4 (11 functions)
 *
 * The card e-Reader system manages save data for scanned e-Reader cards.
 * Each card series has a grid of entries (gridWidth * gridHeight), with
 * each entry occupying 0x10 bytes. The grid is accessed at offset 0x24
 * from the base structure.
 *
 * Assert strings from rodata (lbl_8026F1D8):
 *   "0 <= level && level < series->level_max"
 *   "series->series_number == pCardE->series_number"
 *   "pack < series->pack_max"
 *   "card < series->trainer_card_max"
 *   "aex->state == CARDE_EX_TRAINER_STATE_APPEARING ||
 *    aex->state == CARDE_EX_TRAINER_STATE_ALREADY_BATTLED_WITH"
 *   "trainer->name[0]"
 *
 * SDA references used by these functions:
 *   lbl_8047C180 - NULL pointer assert string
 *   lbl_8047C188 - Grid pointer assert string
 */

#include "dolphin/types.h"
#include "game/save/save.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Assert function used throughout cardesavedata.c */
/* __assert */ extern void GS_Assert(const char* file, u32 line, const char* msg);

/* =========================================================================
 * Rodata string references
 * ========================================================================= */

/* File name string at lbl_8026F1C8 */
static const char sFile[] = "cardesavedata.c";

/* Assert condition strings at lbl_8026F1D8 (decoded from .4byte data):
 *
 * "0 <= level && level < series->level_max"
 * "series->series_number == pCardE->series_number"
 * "pack < series->pack_max"
 * "card < series->trainer_card_max"
 * "aex->state == CARDE_EX_TRAINER_STATE_APPEARING ||
 *  aex->state == CARDE_EX_TRAINER_STATE_ALREADY_BATTLED_WITH"
 * "trainer->name[0]"
 */

/* SDA string references for null-pointer asserts */
extern const char lbl_8047C180[];  /* NULL struct assert */
extern const char lbl_8047C188[];  /* NULL grid pointer assert */

/* =========================================================================
 * Card e-Reader data layout
 *
 * The card e-Reader data structure, reconstructed from disassembly:
 *
 * Offset  Size    Field
 * 0x00    0x02    species (u16) - species field, cleared on invalid level
 * 0x1A    0x01    seriesID (u8)
 * 0x1B    0x01    levelMax (s8) - must be > 0
 * 0x1C    0x01    gridWidth (u8)
 * 0x1D    0x01    gridHeight (u8)
 * 0x24    varies  gridData - card matrix grid entries (stride 0x10)
 *
 * Each grid entry:
 *   +0x00  u16   cardID
 *   +0x82  u8    isValid (0x82 from grid entry base, i.e., entry + 0x82 from base)
 *
 * Total grid entries = gridWidth * gridHeight
 * ========================================================================= */

/* =========================================================================
 * fn_80082650: cardesavedata_ValidateLevel
 * Address: 0x80082650, Size: 0xE8
 *
 * Validates card e-Reader level data. If the data pointer is NULL,
 * triggers an assert. Checks that levelMax > 0, and scans the card
 * grid for any entry with isValid != 0. If no valid entries are
 * found, clears the species field to 0.
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
void cardesavedata_ValidateLevel(void* pCardE) {
    u8* data = (u8*)pCardE;
    u8* grid;
    s8 levelMax;
    s8 width, height;
    s32 totalEntries;
    s32 i;
    u8 foundValid;

    /* Assert: pCardE != NULL (line 0x17F) */
    if (pCardE == NULL) {
        GS_Assert(sFile, 0x17F, lbl_8047C180);
    }

    /* Check levelMax > 0 (line 0x180) */
    levelMax = (s8)data[0x1B];
    if (levelMax <= 0) {
        GS_Assert(sFile, 0x180, "0 <= level && level < series->level_max");
    }

    /* Grid pointer at offset 0x24 */
    grid = data + 0x24;

    /* Assert: grid != NULL (line 0x1F1) */
    if (grid == NULL) {
        GS_Assert(sFile, 0x1F1, lbl_8047C188);
    }

    /* Compute total entries: width * height */
    width  = (s8)data[0x1C];
    height = (s8)data[0x1D];
    totalEntries = (s32)width * (s32)height;

    /* Scan grid for any valid entry */
    foundValid = 0;
    for (i = 0; i < totalEntries; i++) {
        /* Check isValid flag at offset 0x82 from each entry */
        if (grid[0x82] != 0) {
            foundValid = 1;
            break;
        }
        grid += 0x10; /* Next entry */
    }

    /* If no valid entries found, clear species to 0 */
    if (foundValid == 0) {
        *(u16*)(data + 0x00) = 0;
    }
}

/* =========================================================================
 * fn_80082738: cardesavedata_CheckSeries
 * Address: 0x80082738, Size: 0x228
 *
 * Validates a card e-Reader series. Checks that:
 *   1. Series number matches between the data and series definition
 *   2. Series index is within bounds (0 <= idx < levelMax)
 *   3. Grid dimensions are valid
 *
 * If seriesIdx == 0 and the series is empty (no cards scanned),
 * returns 1 (clear). Otherwise returns 0.
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
s32 cardesavedata_CheckSeries(void* pCardE, CardESeries* pSeries, s8 seriesIdx) {
    u8* data = (u8*)pCardE;
    u8* grid;
    s8 levelMax;
    u8 width, height;
    s32 totalEntries;
    s32 entrySize;
    s32 offset;
    s32 i;

    /* Assert: series->series_number == pCardE->series_number (line 0x225) */
    if (data[0x1A] != pSeries->seriesNumber) {
        GS_Assert(sFile, 0x225, "series->series_number == pCardE->series_number");
    }

    /* Assert: pCardE != NULL (line 0x17F) */
    if (pCardE == NULL) {
        GS_Assert(sFile, 0x17F, lbl_8047C180);
    }

    /* Save grid dimensions from the series definition */
    height = pSeries->trainerCardMax;
    width  = pSeries->packMax;

    /* Validate seriesIdx bounds (line 0x180) */
    {
        s32 idx = (s32)seriesIdx;
        BOOL inBounds = FALSE;

        if (idx >= 0) {
            levelMax = (s8)data[0x1B];
            if (idx < (s32)levelMax) {
                inBounds = TRUE;
            }
        }

        if (inBounds == 0) {
            GS_Assert(sFile, 0x180, "0 <= level && level < series->level_max");
        }
    }

    /* Compute offset into grid based on series index */
    {
        s32 w = (s32)(s8)data[0x1C];
        s32 h = (s32)(s8)data[0x1D];

        totalEntries = w * h;
        entrySize = (totalEntries << 4) + 0x76; /* totalEntries * 16 + 0x76 */
        offset = (s32)seriesIdx * entrySize;
    }

    /* Grid pointer */
    grid = data + offset + 0x24;

    /* Assert: grid != NULL (line 0x198) */
    if (grid == NULL) {
        GS_Assert(sFile, 0x198, lbl_8047C188);
    }

    /* Validate grid width (line 0x199) */
    if ((s32)(s8)width >= (s32)(s8)data[0x1C]) {
        GS_Assert(sFile, 0x199, "pack < series->pack_max");
    }

    /* Validate grid height (line 0x19A) */
    if ((s32)(s8)height >= (s32)(s8)data[0x1D]) {
        GS_Assert(sFile, 0x19A, "card < series->trainer_card_max");
    }

    /* Clear the target entry */
    {
        s32 w2 = (s32)(s8)data[0x1D];
        s32 entryOffset = ((s32)(s8)width * w2 + (s32)(s8)height);
        u8* entry;

        entryOffset = (entryOffset << 4) + 0x76;
        entry = grid + entryOffset;
        entry[0x0C] = 0; /* Clear isValid */
        *(u16*)(entry + 0x00) = 0; /* Clear cardID */
    }

    /* If seriesIdx == 0: check if series is empty */
    if ((s32)seriesIdx == 0) {
        u8* scanGrid;
        u8 anyValid;

        /* Assert: pCardE != NULL */
        if (pCardE == NULL) {
            GS_Assert(sFile, 0x17F, lbl_8047C180);
        }

        /* Check levelMax > 0 */
        levelMax = (s8)data[0x1B];
        if (levelMax <= 0) {
            GS_Assert(sFile, 0x180, "0 <= level && level < series->level_max");
        }

        scanGrid = data + 0x24;

        /* Assert: scanGrid != NULL */
        if (scanGrid == NULL) {
            GS_Assert(sFile, 0x1F1, lbl_8047C188);
        }

        /* Scan for any valid entry */
        {
            s32 w3 = (s32)(s8)data[0x1C];
            s32 h3 = (s32)(s8)data[0x1D];
            s32 total = w3 * h3;

            anyValid = 0;
            for (i = 0; i < total; i++) {
                if (scanGrid[0x82] != 0) {
                    anyValid = 1;
                    break;
                }
                scanGrid += 0x10;
            }
        }

        if (anyValid == 0) {
            return 1; /* Series is clear/empty */
        }
    }

    return 0;
}

/* =========================================================================
 * fn_80082960 - fn_800836AC: Additional card e-Reader save functions
 *
 * These 9 functions (Func3 through Func11) handle various operations
 * on the card e-Reader save data:
 *   - Reading/writing card entry fields
 *   - Clearing/resetting card grid entries
 *   - Serialization of card state for save file storage
 *   - Validation of card data integrity
 *
 * They all follow a similar pattern:
 *   1. Validate seriesNumber match
 *   2. Null-check the data pointer
 *   3. Validate seriesIdx bounds
 *   4. Compute grid offset
 *   5. Perform the specific operation
 *
 * The grid offset formula is consistent across all functions:
 *   entrySize = (gridWidth * gridHeight * 16) + 0x76
 *   offset    = seriesIdx * entrySize + 0x24
 *
 * Full decompilation of each would follow the same pattern as
 * cardesavedata_CheckSeries above, with different field operations
 * at the end.
 * ========================================================================= */

/* fn_80082960: Size 0x128 - Read a specific card entry field */
/* fn_80082A88: Size 0x11C - Read another card entry field */
/* fn_80082BA4: Size 0x14C - Write a card entry field */
/* fn_80082CF0: Size 0x1B4 - Complex card entry operation */
/* fn_80082EA4: Size 0x140 - Card entry validation */
/* fn_80082FE4: Size 0x0C0 - Small utility function */
/* fn_800830A4: Size 0x224 - Grid data serialization */
/* fn_800832C8: Size 0x3E4 - Large grid operation */
/* fn_800836AC: Size 0x448 - Largest function, full grid processing */
