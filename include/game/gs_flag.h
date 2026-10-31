#ifndef GAME_GS_FLAG_H
#define GAME_GS_FLAG_H

#include "dolphin/types.h"

/**
 * @file gs_flag.h
 * @brief GSflag -- Game Flag System for Pokemon Colosseum.
 *
 * The GSflag system manages game-wide boolean and multi-bit flags used for
 * story progression, event completion, item collection, trainer defeat
 * tracking, Shadow Pokemon snagging state, and other persistent game state.
 *
 * Flags are stored as a packed bitfield array in save data. Each flag has
 * a definition entry in the flag definition table (pointed to by
 * gFlagDefTable / lbl_80478F9C) which specifies:
 *   - The storage level (which bitfield buffer the flag lives in)
 *   - The bit width (1 for boolean, up to 32 for multi-bit values)
 *   - The starting bit offset within that level's buffer
 *   - An optional linked flag ID for cascading sets
 *   - An optional event trigger ID
 *
 * Flag definition entry layout (8 bytes each):
 *   Byte 0:  [bits 7:6] = storage level (0-3)
 *            [bits 5:0] = bit width (1-32)
 *   Byte 1:  flag type / enable byte (0 = disabled)
 *   Byte 2:  event trigger type (0 = none)
 *   Byte 3:  event trigger ID (index into event table)
 *   Bytes 4-5: bit offset (u16) within the level's bitfield buffer
 *   Bytes 6-7: linked flag ID (s16, -1 = no link)
 *
 * Storage levels:
 *   Level 0 (type bits 7:6 = 00): Permanent save flags
 *   Level 1 (type bits 7:6 = 01): Session/temporary flags
 *   Level 2 (type bits 7:6 = 10): Floor/map-local flags
 *   Level 3 (type bits 7:6 = 11): Reserved
 *
 * The flag system uses three bitfield buffers (level 0, 1, 2) whose
 * pointers and sizes are stored in the GSflagState structure.
 *
 * Address range: 0x8018FE30 - 0x80190E34 (9 functions)
 *
 * Key SDA globals:
 *   lbl_80478EEC (gFlagState)    : GSflagState structure pointer
 *   lbl_80478F9C (gFlagDefTable) : Flag definition table pointer
 *   lbl_80478F98 (gFlagConfig)   : Flag system config pointer
 *   lbl_80478ED4 (gFlagEventTbl) : Event trigger table pointer
 *   lbl_80478EF4 (gFlagNPCTbl)   : NPC event table pointer
 *   lbl_80478EFC (gFlagSceneTbl) : Scene/floor trigger table pointer
 *   lbl_80478EE4 (gFlagPartyPtr) : Party data pointer for flag events
 */

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum bit width for a single flag value. */
#define GSFLAG_MAX_BITS         32

/** Flag definition entry size in bytes. */
#define GSFLAG_DEF_ENTRY_SIZE   8

/** Storage level codes (upper 2 bits of byte 0). */
#define GSFLAG_LEVEL_PERMANENT  0   /* Level 0: Permanent save flags */
#define GSFLAG_LEVEL_SESSION    1   /* Level 1: Session/temporary flags */
#define GSFLAG_LEVEL_MAP        2   /* Level 2: Floor/map-local flags */
#define GSFLAG_LEVEL_RESERVED   3   /* Level 3: Reserved */

/** Sentinel value for linked flag chain termination. */
#define GSFLAG_LINK_NONE        (-1)

/* =========================================================================
 * Bitmask lookup table
 * ========================================================================= */

/**
 * Bitmask table at lbl_8036C568 (34 entries).
 * Entry [n] = (1 << n) - 1, i.e. a mask of n low bits.
 * Used to clip values to the declared bit width of a flag.
 *
 *   [0]  = 0x00000000
 *   [1]  = 0x00000001
 *   [2]  = 0x00000003
 *   ...
 *   [32] = 0xFFFFFFFF
 *   [33] = 0x00000000  (sentinel)
 */
extern u32 gFlagBitMasks[];  /* lbl_8036C568 */

/* =========================================================================
 * Structures
 * ========================================================================= */

/**
 * Single flag definition entry (8 bytes).
 * Packed in the flag definition table.
 */
typedef struct GSflagDef {
    /* 0x00 */ u8   typeAndWidth;   /* [7:6]=level, [5:0]=bitWidth */
    /* 0x01 */ u8   enableByte;     /* non-zero if flag is active */
    /* 0x02 */ u8   eventTrigType;  /* event trigger type (0=none) */
    /* 0x03 */ u8   eventTrigID;    /* event trigger index */
    /* 0x04 */ u16  bitOffset;      /* starting bit position in buffer */
    /* 0x06 */ s16  linkedFlagID;   /* next flag in chain, -1 = end */
} GSflagDef;

/**
 * Runtime state for the flag system.
 * Stored at SDA global lbl_80478EEC (8 bytes visible, full struct ~0x20).
 *
 * Offsets match the lwz/stw patterns in GSflagInit (GSflagInit):
 *   +0x04 = bitfield buffer pointer for level 0 (permanent)
 *   +0x08 = buffer word count for level 0
 *   +0x0C = allocator/save pointer
 *   +0x10 = buffer word count for level 1
 *   +0x14 = bitfield buffer pointer for level 1 (session)
 *   +0x18 = buffer word count for level 2
 *   +0x1C = bitfield buffer pointer for level 2 (map)
 */
typedef struct GSflagState {
    /* 0x00 */ u32  padding_00;
    /* 0x04 */ u32* bufLevel0;      /* Permanent flag bitfield buffer */
    /* 0x08 */ u32  sizeLevel0;     /* Size in u32 words */
    /* 0x0C */ u32* savePtr;        /* Save data pointer */
    /* 0x10 */ u32  sizeLevel1;     /* Size in u32 words */
    /* 0x14 */ u32* bufLevel1;      /* Session flag bitfield buffer */
    /* 0x18 */ u32  sizeLevel2;     /* Size in u32 words */
    /* 0x1C */ u32* bufLevel2;      /* Map-local flag bitfield buffer */
} GSflagState;

/**
 * Flag system configuration.
 * Stored at SDA global lbl_80478F98.
 */
typedef struct GSflagConfig {
    /* 0x00 */ u32  flagCount;      /* Total number of flag definitions */
    /* 0x04 */ s16  firstLinkedID;  /* First linked flag ID in cascade chain */
} GSflagConfig;

/**
 * Scene/floor trigger table entry (0x18 bytes).
 * Stored at SDA global lbl_80478EFC.
 * When a flag is set, if it has a scene trigger, the game can
 * change floors, play cutscenes, spawn NPCs, etc.
 */
typedef struct GSflagSceneEntry {
    /* 0x00 */ u8   triggerCount;   /* Number of triggers */
    /* 0x01 */ u8   padding_01;
    /* 0x02 */ u16  sceneID;        /* Target scene/floor ID */
    /* 0x04 */ u16  eventID;        /* Event ID to fire */
    /* 0x06 */ u16  padding_06;
    /* 0x08 */ f32  posX;           /* World position X */
    /* 0x0C */ f32  posY;           /* World position Y */
    /* 0x10 */ f32  posZ;           /* World position Z */
    /* 0x14 */ u32  padding_14;
} GSflagSceneEntry;

/* =========================================================================
 * Function declarations
 * ========================================================================= */

/**
 * GSflagSet -- Set a flag by ID with cascading linked flags.
 * Address: 0x8018FE30, Size: 0x4B0
 *
 * Sets the specified flag and follows the linked flag chain.
 * For boolean (1-bit) flags, sets or clears the bit based on the
 * flag value. For multi-bit flags, writes the value clipped to
 * the declared bit width.
 *
 * If the flag has event triggers (eventTrigType != 0), those
 * are fired after the flag value is written. This can trigger
 * NPC spawning, scene transitions, and party changes.
 *
 * If flagID < 0, the function returns immediately.
 *
 * @param flagID  The flag index to set (>= 0).
 */
void GSflagSet(s32 flagID);

/**
 * fn_801902E0 -- Read a boolean flag.
 * Address: 0x801902E0, Size: 0xD0
 *
 * Reads a single flag and returns 0 or 1.
 * For multi-bit flags this reads the full value but the caller
 * typically uses it as a boolean test.
 *
 * @param flagID  The flag index to read.
 * @return        0 if the flag is clear, 1 if set.
 */
u8 std081_flagCheckTrue(s32 flagID);

/**
 * GSflagSet16 -- Set a flag to 0 (clear).
 * Address: 0x801903B0, Size: 0x178
 *
 * Identical structure to fn_80190528 but writes value = 0.
 * Used to explicitly clear a multi-bit flag to zero.
 *
 * @param flagID  The flag index to clear.
 */
void GSflagSet16(s32 flagID);

/**
 * fn_80190528 -- Set a flag to 1 (assert).
 * Address: 0x80190528, Size: 0x178
 *
 * Identical structure to GSflagSet16 but writes value = 1.
 * Used to explicitly set a multi-bit flag to one.
 *
 * @param flagID  The flag index to set.
 */
void std078_flagSetFalse(s32 flagID);

/**
 * GSflagGet16 -- Read a boolean flag value.
 * Address: 0x801906A0, Size: 0xBC
 *
 * Reads a flag and returns its boolean value (0 or 1).
 * For single-bit flags, returns the bit directly.
 * For multi-bit flags, extracts and returns the full value
 * but callers interpret as boolean.
 *
 * @param flagID  The flag index to read.
 * @return        The flag's boolean value.
 */
s32 GSflagGet16(s32 flagID);

/**
 * GSflagGet32 -- Read/write a multi-bit flag value.
 * Address: 0x8019075C, Size: 0x178
 *
 * Writes a multi-bit value into the flag's bit range.
 * The value is clipped to the flag's declared bit width
 * using the bitmask table.
 *
 * @param flagID  The flag index.
 * @param value   The value to write (clipped to bit width).
 */
void GSflagGet32(s32 flagID, s32 value);

/**
 * GSflagClear -- Clear all bits in a flag level's buffer.
 * Address: 0x801908D4, Size: 0xD4
 *
 * Zeroes the entire bitfield buffer for the specified level.
 * Uses an unrolled 8-word-at-a-time memset loop.
 *
 * @param level  The storage level index (0-2).
 */
void GSflagClear(s32 level);

/**
 * GSflagInit -- Initialize the flag system.
 * Address: 0x801909A8, Size: 0x2E8
 *
 * Sets up the flag system with the definition table, buffer
 * pointers, and buffer sizes for all three storage levels.
 * Zeroes all three bitfield buffers.
 *
 * Called during game initialization.
 *
 * @param savePtr       Save data pointer for level 0 buffer.
 * @param flagCount     Total number of flag definitions.
 * @param size0         Word count for level 0 (permanent) buffer.
 * @param size1         Word count for level 1 (session) buffer.
 * @param size2         Word count for level 2 (map) buffer.
 * @param buf1          Pointer to level 1 buffer.
 * @param buf2          Pointer to level 2 buffer.
 * @param reserved      Reserved parameter.
 */
void GSflagInit(u32* savePtr, u32 flagCount, u32 size0,
                u32 size1, u32 size2, u32* buf1, u32* buf2, u32 reserved);

/**
 * GSflagSetBitValue -- Low-level bit value setter with validation.
 * Address: 0x80190C90, Size: 0x1A4
 *
 * Iterates through the flag definition table, computing bit
 * offsets and verifying that each flag's bit width does not
 * exceed the maximum (32 bits) and that buffer sizes are
 * sufficient. Stores the computed bit offsets into each
 * flag definition entry.
 *
 * Called during GSflagInit to validate and prepare the table.
 *
 * @param defTable   Pointer to flag definition table.
 * @param flagCount  Number of flag definitions.
 * @param maxBits0   Maximum bit count for level 0.
 * @param maxBits1   Maximum bit count for level 1.
 * @param maxBits2   Maximum bit count for level 2.
 * @param reserved   Reserved parameter.
 */
void GSflagSetBitValue(GSflagDef* defTable, u32 flagCount,
                       u32 maxBits0, u32 maxBits1, u32 maxBits2, u32 reserved);

#endif /* GAME_GS_FLAG_H */
