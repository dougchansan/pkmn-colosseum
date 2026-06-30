/**
 * @file gs_flag.c
 * @brief GSflag -- Game Flag System implementation.
 *
 * Decompiled from Address range: 0x8018FE30 - 0x80190E34 (9 functions).
 *
 * The flag system is used throughout Colosseum for tracking game state:
 *   - Story progression (which cutscenes have played, boss defeats)
 *   - Shadow Pokemon snagging state (which shadows have been caught)
 *   - Item pickup tracking (which overworld items have been collected)
 *   - Trainer defeat flags (which trainers have been beaten)
 *   - Map/floor state (door locks, puzzle states, NPC positions)
 *
 * Flags are packed into bitfield buffers at three persistence levels:
 *   Level 0: Permanent (saved to memory card)
 *   Level 1: Session (cleared on game over / soft reset)
 *   Level 2: Map-local (cleared on floor transition)
 *
 * Each flag definition is 8 bytes and specifies the storage level,
 * bit width, bit offset, and optional linked/cascade flag ID.
 *
 * The bitmask lookup table at lbl_8036C568 provides precomputed
 * masks: entry[n] = (1 << n) - 1, used to clip values to n bits.
 */

#include "game/gs_flag.h"

/* =========================================================================
 * External function declarations
 * ========================================================================= */

extern void fn_800DD970(const char* fmt, ...);   /* GSlog_Print */
extern void fn_801299C8(s32 slot, u16 eventID, s32 param1, s32 param2);  /* NPC event trigger 1 */
extern void fn_80129A78(s32 slot, u16 eventID, s32 param1, s32 param2);  /* NPC event trigger 2 */
extern void fn_8012F1FC(s32 slot);               /* Party/Pokemon update */
extern void fn_8012F40C(s32 slot);               /* Party slot refresh */
extern void fn_8012F150(s32 slot);               /* Party activate slot */
extern void fn_80129280(s32 slot, s32 count);    /* Party set active count */
extern void fn_8012A86C(s32 slot, u16 param);    /* Party event apply */
extern void fn_800FF56C(s32 floorID);            /* Floor/scene load */
extern void fn_80113778(s32 slot, f32 x, f32 y, f32 z);  /* Set world position */

/* =========================================================================
 * SDA global variables
 * ========================================================================= */

/**
 * These are Small Data Area (SDA) globals accessed via r0/r13 + offset.
 * They are defined in the .sbss section and initialized at runtime.
 */

/* lbl_80478EEC: Flag system runtime state pointer */
extern GSflagState* gFlagState;         /* .sbss:0x80478EEC */

/* lbl_80478F9C: Flag definition table pointer */
extern GSflagDef*   gFlagDefTable;      /* .sbss:0x80478F9C */

/* lbl_80478F98: Flag system configuration pointer */
extern GSflagConfig* gFlagConfig;       /* .sbss:0x80478F98 */

/* lbl_80478ED4: Event trigger table pointer */
extern void*        gFlagEventTbl;      /* .sbss:0x80478ED4 */

/* lbl_80478EF4: NPC event table pointer */
extern u8*          gFlagNPCTbl;        /* .sbss:0x80478EF4 */

/* lbl_80478EFC: Scene/floor trigger table pointer */
extern u8*          gFlagSceneTbl;      /* .sbss:0x80478EFC */

/* lbl_80478EE4: Party data pointer for flag events */
extern u8*          gFlagPartyPtr;      /* .sbss:0x80478EE4 */

/* =========================================================================
 * Rodata string constants
 * ========================================================================= */

/* lbl_802741F8 */
static const char sErrNotInit[] =
    "ERROR[GSflagSet]:Initialization has not finished.\n";

/* lbl_8027422C */
static const char sErrOverflow[] =
    "ERROR[GSflagSet]:Overflow BitValue FlagID=%d Value=%d "
    "(%d BitLength=%d)>(Limit=%dBit).\n";

/* lbl_80274284 */
static const char sErrGetNotInit[] =
    "ERROR[GSflagGet]:Initialization has not finished.\n";

/* lbl_802742B8+0x00 */
static const char sErrClearNotInit[] =
    "ERROR[GSflagClear]:Initialization has not finished.\n";

/* lbl_802742B8+0x38 */
static const char sErrInitOverflowBitLen[] =
    "ERROR[GSflagInit]:Overflow BitLength FlagID=%d "
    "(Length=%d)>(Limit=%dBit).\n";

/* lbl_802742B8+0x7C */
static const char sErrInitZeroBitLen[] =
    "ERROR[GSflagInit]:Zero BitLength FlagID=%d "
    "(Length=%d)>(Limit=%dBit).\n";

/* lbl_802742B8+0xBC */
static const char sErrInitOverflowLevel0[] =
    "ERROR[GSflagInit]:Overflow FlagLevel0 "
    "(Use Bytes=%d)>=(Limit Bytes=%d).\n";

/* lbl_802742B8+0x108 */
static const char sErrInitOverflowLevel1[] =
    "ERROR[GSflagInit]:Overflow FlagLevel1 "
    "(Use Bytes=%d)>=(Limit Bytes=%d).\n";

/* lbl_802742B8+0x154 */
static const char sErrInitOverflowLevel2[] =
    "ERROR[GSflagInit]:Overflow FlagLevel2 "
    "(Use Bytes=%d)>=(Limit Bytes=%d).\n";

/* =========================================================================
 * Helper: get buffer pointer for a flag's storage level
 * ========================================================================= */

/**
 * Look up the bitfield buffer pointer for a flag's storage level.
 * The level is encoded in the upper 2 bits of the typeAndWidth byte.
 *
 * Level mapping:
 *   (typeAndWidth >> 5) & 0x3 yields 0,1,2,3
 *   Multiply by 8 (shift left 3) to get offset into GSflagState:
 *     0 -> offset 0x04 (bufLevel0)
 *     1 -> offset 0x0C (savePtr -- treated as level 1 alias)
 *     2 -> offset 0x14 (bufLevel1)
 *     3 -> offset 0x1C (bufLevel2)
 *
 * In practice the rlwinm instruction extracts bits 24:25 shifted left 2,
 * yielding a byte offset into gFlagState which is then added to +4.
 */
static u32* GSflag_GetBuffer(u8 typeAndWidth) {
    u32 levelOffset;
    u32* stateBase;

    stateBase = (u32*)gFlagState;
    /* rlwinm r0, r4, 29, 27, 28 -- extract bits [7:6], shift right 5, shift left 2 */
    levelOffset = ((u32)(typeAndWidth >> 5) & 0x3) << 2;

    /* Access stateBase[levelOffset/4 + 1] which is the buffer pointer */
    return *(u32**)((u8*)stateBase + levelOffset + 4);
}

/* =========================================================================
 * Helper: write a value into a flag's bit range
 * ========================================================================= */

/**
 * Write a value into the packed bitfield at the specified bit offset
 * with the specified bit width.
 *
 * For 1-bit flags (bitWidth == 1):
 *   If value == 0: clear the bit (AND with inverted mask)
 *   If value != 0: set the bit (OR with mask)
 *
 * For multi-bit flags (bitWidth > 1):
 *   Compute the word index and bit position within the word.
 *   Apply the bitmask to clear old bits, then OR in the new value.
 *   If the field spans two u32 words, handle the second word too.
 */
static void GSflag_WriteBits(u32* buffer, u16 bitOffset, u8 bitWidth, u32 value) {
    u32 wordIdx;
    u32 bitPos;
    u32 mask;
    u32 word;

    wordIdx = bitOffset >> 5;       /* bitOffset / 32 */
    bitPos  = bitOffset & 0x1F;     /* bitOffset % 32 */

    if (bitWidth <= 1) {
        /* Single-bit flag */
        if (value == 0) {
            /* Clear bit */
            word = buffer[wordIdx];
            word &= ~(1U << bitPos);
            buffer[wordIdx] = word;
        } else {
            /* Set bit */
            word = buffer[wordIdx];
            word |= (1U << bitPos);
            buffer[wordIdx] = word;
        }
    } else {
        /* Multi-bit flag */
        u32 totalBits = bitWidth + bitPos;
        mask = gFlagBitMasks[bitWidth];

        /* First word: shift value and mask into position */
        word = buffer[wordIdx];
        word &= ~(mask << bitPos);  /* Clear target bits */
        word |= (value << bitPos);  /* Write new value */
        buffer[wordIdx] = word;

        /* If the field spans into the next word */
        if (totalBits >= 32) {
            u32 remainBits = totalBits - 32;
            u32 word2 = buffer[wordIdx + 1];
            u32 mask2 = gFlagBitMasks[remainBits];
            u32 shiftedValue = value >> (bitWidth - remainBits);

            word2 &= ~mask2;       /* Clear target bits in word 2 */
            word2 |= shiftedValue; /* Write remaining bits */
            buffer[wordIdx + 1] = word2;
        }
    }
}

/* =========================================================================
 * Helper: read a value from a flag's bit range
 * ========================================================================= */

/**
 * Read a value from the packed bitfield at the specified bit offset
 * with the specified bit width.
 */
static u32 GSflag_ReadBits(u32* buffer, u16 bitOffset, u8 bitWidth) {
    u32 wordIdx;
    u32 bitPos;
    u32 word;
    u32 mask;

    wordIdx = bitOffset >> 5;
    bitPos  = bitOffset & 0x1F;

    if (bitWidth <= 1) {
        /* Single-bit: extract and return 0 or 1 */
        word = buffer[wordIdx];
        return (word >> bitPos) & 1;
    } else {
        /* Multi-bit: may span two words */
        u32 lo = buffer[wordIdx];
        u32 hi = buffer[wordIdx + 1];
        u32 shiftAmt = 32 - bitPos;

        lo = lo >> bitPos;
        hi = hi << shiftAmt;
        mask = gFlagBitMasks[bitWidth];

        return (hi | lo) & mask;
    }
}

/* =========================================================================
 * GSflagSet (fn_8018FE30) -- 0x4B0 bytes
 * ========================================================================= */

/**
 * Set a flag by ID, following the linked flag chain.
 *
 * The function processes the flag in three phases:
 *
 * Phase 1: Process the immediate flag value (write bits to buffer).
 *          Follow any linked flags that chain from this flag
 *          (walking the -1-terminated linked list via bytes 6-7).
 *
 * Phase 2: Process the "cascade" linked flags from gFlagConfig.
 *          Starting from gFlagConfig->firstLinkedID, walk through
 *          linked flags until the chain reaches the original flagID
 *          or terminates at -1. Each linked flag in this second
 *          chain may also have event triggers.
 *
 * Phase 3: Fire event triggers if the flag or its chain flags
 *          have eventTrigType != 0 or eventTrigID != 0.
 *          This can trigger NPC events, party changes, floor
 *          transitions, and world position updates.
 */
void GSflagSet(s32 flagID) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;
    s16         linkedID;
    s32         currentID;
    u32         value;

    if (flagID < 0) {
        return;
    }

    /* --- Phase 1: Process immediate flag and linked chain --- */
    currentID = flagID;
    defTable = gFlagDefTable;

    while (currentID != -1) {
        entry = &defTable[currentID];

        /* Check if flag is enabled */
        if (entry->enableByte != 0) {
            /* Get the buffer for this flag's storage level */
            typeAndWidth = entry->typeAndWidth;
            buffer = GSflag_GetBuffer(typeAndWidth);

            if (buffer == NULL) {
                fn_800DD970(sErrNotInit);
            } else {
                /* Extract bit width and offset */
                bitWidth  = typeAndWidth & 0x3F;
                bitOffset = entry->bitOffset;

                /* The "value" for Phase 1 is the enableByte itself */
                value = (u32)entry->enableByte;

                /* Validate bit width does not exceed maximum */
                {
                    u32 maxBits = 32 - __cntlzw(value);
                    if (maxBits > bitWidth) {
                        fn_800DD970(sErrOverflow, flagID, value, value, bitWidth, bitWidth);
                        value &= gFlagBitMasks[bitWidth];
                    }
                }

                /* Write the value into the bitfield */
                GSflag_WriteBits(buffer, bitOffset, bitWidth, value);
            }
        }

        /* Follow linked flag chain */
        linkedID = entry->linkedFlagID;
        currentID = linkedID;
    }

    /* --- Phase 2: Process cascade chain from config --- */
    currentID = gFlagConfig->firstLinkedID;

    while (currentID != flagID && currentID != -1) {
        entry = &defTable[currentID];

        /* Check if flag is enabled */
        if (entry->enableByte != 0) {
            /* Process this cascade flag similarly to Phase 1 */
            typeAndWidth = entry->typeAndWidth;

            /* Get event trigger buffer */
            {
                u32 evtIdx;
                u32 levelOffset;
                u32* evtBuffer;
                evtIdx = (u32)entry->enableByte;
                levelOffset = ((u32)(typeAndWidth >> 5) & 0x3) << 2;
                evtBuffer = *(u32**)((u8*)gFlagEventTbl + levelOffset);
                buffer = GSflag_GetBuffer(typeAndWidth);
            }

            if (buffer == NULL) {
                fn_800DD970(sErrNotInit);
            } else {
                bitWidth  = typeAndWidth & 0x3F;
                bitOffset = entry->bitOffset;
                value = 0;  /* Cascade sets always write from the event buffer */

                /* Validate and write */
                {
                    u32 maxBits = 32 - __cntlzw(value > 0 ? value : 1);
                    if (maxBits > bitWidth) {
                        fn_800DD970(sErrOverflow, flagID, value, value, bitWidth, bitWidth);
                        value &= gFlagBitMasks[bitWidth];
                    }
                }

                GSflag_WriteBits(buffer, bitOffset, bitWidth, value);

                /* Fire NPC event if trigger type is set */
                if (entry->eventTrigType != 0) {
                    u32  trigIdx = (u32)entry->eventTrigType << 2;
                    u16  npcEvtID;

                    npcEvtID = *(u16*)(gFlagNPCTbl + trigIdx + 2);
                    fn_801299C8(0, npcEvtID, 1, -1);

                    npcEvtID = *(u16*)(gFlagNPCTbl + trigIdx);
                    fn_80129A78(0, npcEvtID, 1, -1);
                }
            }
        }
        /* Follow linked flag chain */
        linkedID = entry->linkedFlagID;
        currentID = linkedID;
    }

    /* --- Phase 3: Process event triggers on the original flag --- */
    entry = &defTable[flagID];

    /* NPC event trigger on the original flag */
    if (entry->eventTrigType != 0) {
        u32  trigIdx = (u32)entry->eventTrigType << 2;
        u16  npcEvtID;

        npcEvtID = *(u16*)(gFlagNPCTbl + trigIdx + 2);
        fn_801299C8(0, npcEvtID, 1, -1);

        npcEvtID = *(u16*)(gFlagNPCTbl + trigIdx);
        fn_80129A78(0, npcEvtID, 1, -1);
    }

    /* Scene/floor trigger */
    if (entry->eventTrigID != 0) {
        GSflagSceneEntry* sceneEntry;
        u32  sceneIdx;
        u8   trigCount;
        u16  eventID;
        u16  sceneID;
        f32  posX;
        f32  posY;
        f32  posZ;

        sceneIdx = (u32)entry->eventTrigID * 0x18;
        trigCount = *(gFlagSceneTbl + sceneIdx);

        if (trigCount != 0) {
            s32 i;

            /* Initialize party for event */
            fn_8012F1FC(0);
            fn_8012F40C(0);
            fn_8012F150(1);

            /* Process each trigger (up to 2 party slots) */
            for (i = 0; i < 2; i++) {
                s32 slot;

                if (i == 0) {
                    slot = 0;
                } else if (i == 1) {
                    slot = 1;
                } else {
                    slot = -1;
                }

                if (slot >= 0) {
                    u8 trigByte;
                    trigByte = *(gFlagPartyPtr + slot + trigCount);
                    if (trigByte != 0) {
                        fn_8012F1FC(slot);
                    }
                }
            }

            /* Set party active count */
            fn_80129280(0, 2);

            /* Apply event */
            eventID = *(u16*)(gFlagSceneTbl + sceneIdx + 4);
            fn_8012A86C(0, eventID);

            /* Floor transition if sceneID is set */
            sceneID = *(u16*)(gFlagSceneTbl + sceneIdx + 2);
            if (sceneID == 0) {
                fn_800FF56C(sceneID);
            }

            /* Set world position */
            posX = *(f32*)(gFlagSceneTbl + sceneIdx + 0x08);
            posY = *(f32*)(gFlagSceneTbl + sceneIdx + 0x0C);
            posZ = *(f32*)(gFlagSceneTbl + sceneIdx + 0x10);
            fn_80113778(0, posX, posY, posZ);
        }
    }
}

/* =========================================================================
 * GSflagGet (fn_801902E0) -- 0xD0 bytes
 * ========================================================================= */

/**
 * Read a flag and return its boolean value (0 or 1).
 *
 * Looks up the flag definition, retrieves the appropriate buffer,
 * and reads the value from the bitfield. For single-bit flags,
 * returns 0 or 1 directly. For multi-bit flags, reads the full
 * value but the final comparison converts to boolean.
 */
s32 GSflagGet(s32 flagID) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;
    u32         value;

    defTable = gFlagDefTable;
    entry = &defTable[flagID];
    typeAndWidth = entry->typeAndWidth;

    buffer = GSflag_GetBuffer(typeAndWidth);

    if (buffer == NULL) {
        fn_800DD970(sErrGetNotInit);
        return 0;
    }

    bitWidth  = typeAndWidth & 0x3F;
    bitOffset = entry->bitOffset;

    value = GSflag_ReadBits(buffer, bitOffset, bitWidth);

    if (value != 0) {
        return 1;
    }
    return 0;
}

/* =========================================================================
 * GSflagSet16 (fn_801903B0) -- 0x178 bytes
 * ========================================================================= */

/**
 * Clear a flag (write value 0 to its bit range).
 *
 * Uses the same bitfield manipulation as GSflagSet32 but
 * with a fixed value of 0.
 */
void GSflagSet16(s32 flagID) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;
    u32         value;

    value = 0;  /* li r29, 0x0 */
    defTable = gFlagDefTable;
    entry = &defTable[flagID];
    typeAndWidth = entry->typeAndWidth;

    buffer = GSflag_GetBuffer(typeAndWidth);

    if (buffer == NULL) {
        fn_800DD970(sErrNotInit);
        return;
    }

    bitWidth  = typeAndWidth & 0x3F;
    bitOffset = entry->bitOffset;

    /* Validate value against bit width */
    {
        u32 maxBits = 32 - __cntlzw(value);
        if (maxBits > bitWidth) {
            fn_800DD970(sErrOverflow, flagID, value, 0, 0, bitWidth);
            value &= gFlagBitMasks[bitWidth];
        }
    }

    GSflag_WriteBits(buffer, bitOffset, bitWidth, value);
}

/* =========================================================================
 * GSflagSet32 (fn_80190528) -- 0x178 bytes
 * ========================================================================= */

/**
 * Set a flag (write value 1 to its bit range).
 *
 * Uses the same bitfield manipulation as GSflagSet16 but
 * with a fixed value of 1.
 */
void GSflagSet32(s32 flagID) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;
    u32         value;

    value = 1;  /* li r29, 0x1 */
    defTable = gFlagDefTable;
    entry = &defTable[flagID];
    typeAndWidth = entry->typeAndWidth;

    buffer = GSflag_GetBuffer(typeAndWidth);

    if (buffer == NULL) {
        fn_800DD970(sErrNotInit);
        return;
    }

    bitWidth  = typeAndWidth & 0x3F;
    bitOffset = entry->bitOffset;

    /* Validate value against bit width */
    {
        u32 maxBits = 32 - __cntlzw(value);
        if (maxBits > bitWidth) {
            fn_800DD970(sErrOverflow, flagID, value, 1, 1, bitWidth);
            value &= gFlagBitMasks[bitWidth];
        }
    }

    GSflag_WriteBits(buffer, bitOffset, bitWidth, value);
}

/* =========================================================================
 * GSflagGet16 (fn_801906A0) -- 0xBC bytes
 * ========================================================================= */

/**
 * Read a flag as a boolean value.
 *
 * Similar to GSflagGet but returns the raw value instead of
 * clamping to 0/1 for multi-bit flags. For single-bit flags
 * the result is identical. The name "Get16" suggests this was
 * originally designed for 16-bit flag reads, but the actual
 * implementation handles arbitrary widths.
 */
s32 GSflagGet16(s32 flagID) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;

    defTable = gFlagDefTable;
    entry = &defTable[flagID];
    typeAndWidth = entry->typeAndWidth;

    buffer = GSflag_GetBuffer(typeAndWidth);

    if (buffer == NULL) {
        fn_800DD970(sErrGetNotInit);
        return 0;
    }

    bitWidth  = typeAndWidth & 0x3F;
    bitOffset = entry->bitOffset;

    return (s32)GSflag_ReadBits(buffer, bitOffset, bitWidth);
}

/* =========================================================================
 * GSflagGet32 (fn_8019075C) -- 0x178 bytes
 * ========================================================================= */

/**
 * Write a multi-bit value to a flag.
 *
 * Despite the "Get32" name (which may be a misnomer from the original
 * source, or the names may have been swapped in the symbol proposal),
 * this function writes a value into the flag's bit range.
 *
 * The value parameter is clipped to the flag's declared bit width
 * before being written.
 */
void GSflagGet32(s32 flagID, s32 value) {
    GSflagDef*  defTable;
    GSflagDef*  entry;
    u32*        buffer;
    u8          typeAndWidth;
    u8          bitWidth;
    u16         bitOffset;

    defTable = gFlagDefTable;
    entry = &defTable[flagID];
    typeAndWidth = entry->typeAndWidth;

    buffer = GSflag_GetBuffer(typeAndWidth);

    if (buffer == NULL) {
        fn_800DD970(sErrNotInit);
        return;
    }

    bitWidth  = typeAndWidth & 0x3F;
    bitOffset = entry->bitOffset;

    /* Validate and clip value */
    {
        u32 maxBits = 32 - __cntlzw((u32)value);
        if (maxBits > bitWidth) {
            fn_800DD970(sErrOverflow, flagID, value, (u32)value, bitWidth, bitWidth);
            value &= (s32)gFlagBitMasks[bitWidth];
        }
    }

    GSflag_WriteBits(buffer, bitOffset, bitWidth, (u32)value);
}

/* =========================================================================
 * GSflagClear (fn_801908D4) -- 0xD4 bytes
 * ========================================================================= */

/**
 * Clear all bits in a flag level's buffer.
 *
 * Zeroes the entire u32-word buffer for the specified storage level.
 * Uses an unrolled loop that clears 8 words per iteration, then
 * handles the remainder one word at a time.
 *
 * The level parameter is used to index into GSflagState:
 *   level << 3 gives the byte offset into the state structure,
 *   from which the buffer pointer and word count are loaded.
 */
void GSflagClear(s32 level) {
    u32* stateBase;
    u32  offset;
    u32* buffer;
    u32  wordCount;
    u32  i;

    stateBase = (u32*)gFlagState;
    offset = (u32)level << 3;

    /* Load buffer pointer and word count */
    buffer    = *(u32**)((u8*)stateBase + offset + 4);
    wordCount = *(u32*)((u8*)stateBase + offset);

    if (buffer == NULL) {
        fn_800DD970(sErrClearNotInit);
        return;
    }

    if (wordCount == 0) {
        return;
    }

    /* Unrolled clear: 8 words at a time */
    i = 0;
    if (wordCount > 8) {
        u32 chunks = (wordCount - 8 + 7) >> 3;
        u32* ptr = buffer;
        u32  remaining = wordCount - 8;

        if (remaining > 0) {
            u32 c;
            for (c = 0; c < chunks; c++) {
                ptr[0] = 0;
                ptr[1] = 0;
                ptr[2] = 0;
                ptr[3] = 0;
                ptr[4] = 0;
                ptr[5] = 0;
                ptr[6] = 0;
                ptr[7] = 0;
                ptr += 8;
                i += 8;
            }
        }
    }

    /* Clear remaining words */
    {
        u32* ptr = buffer + i;
        u32  rem = wordCount - i;
        u32  j;
        for (j = 0; j < rem; j++) {
            ptr[j] = 0;
        }
    }
}

/* =========================================================================
 * GSflagInit (fn_801909A8) -- 0x2E8 bytes
 * ========================================================================= */

/**
 * Initialize the flag system.
 *
 * Sets up the flag state structure with buffer pointers and sizes
 * for all three storage levels. Validates the flag definition table
 * via GSflagSetBitValue, then zeroes all three bitfield buffers.
 *
 * Called during game initialization with pointers to save data
 * buffers and the flag definition table.
 *
 * Register mapping from disassembly:
 *   r3  = savePtr     (stored to gFlagState+0x0C)
 *   r4  = flagCount   (passed to GSflagSetBitValue)
 *   r5  = size0       (stored to gFlagState+0x08)
 *   r6  = size1       (stored to gFlagState+0x10)
 *   r7  = size2       (stored to gFlagState+0x18)
 *   r8  = reserved
 */
void GSflagInit(u32* savePtr, u32 flagCount, u32 size0,
                u32 size1, u32 size2, u32* buf1, u32* buf2, u32 reserved) {
    GSflagState*  state;
    GSflagDef*    defTable;
    GSflagConfig* config;
    u32           totalFlags;
    u32*          buffer;
    u32           wordCount;
    u32           i;

    /* Load global pointers */
    config   = gFlagConfig;
    defTable = gFlagDefTable;
    totalFlags = config->flagCount;

    /* Validate and prepare the flag table */
    GSflagSetBitValue(defTable, totalFlags, size0, size1, size2, reserved);

    /* Store configuration into gFlagState */
    state = gFlagState;
    state->savePtr   = savePtr;
    state->sizeLevel1 = size1;
    state->bufLevel2  = buf2;
    state->sizeLevel0 = size0;
    state->bufLevel1  = buf1;
    state->sizeLevel2 = size2;

    /* --- Clear level 0 buffer --- */
    buffer    = state->savePtr;
    wordCount = state->sizeLevel0;

    if (buffer == NULL) {
        fn_800DD970(sErrClearNotInit);
    } else if (wordCount > 0) {
        /* Unrolled clear: 8 words at a time */
        i = 0;
        if (wordCount > 8) {
            u32* ptr = buffer;
            u32  chunks = (wordCount - 8 + 7) >> 3;
            u32  remaining = wordCount - 8;
            if (remaining > 0) {
                u32 c;
                for (c = 0; c < chunks; c++) {
                    ptr[0] = 0; ptr[1] = 0; ptr[2] = 0; ptr[3] = 0;
                    ptr[4] = 0; ptr[5] = 0; ptr[6] = 0; ptr[7] = 0;
                    ptr += 8;
                    i += 8;
                }
            }
        }
        {
            u32* ptr = buffer + i;
            u32  rem = wordCount - i;
            u32  j;
            for (j = 0; j < rem; j++) {
                ptr[j] = 0;
            }
        }
    }

    /* --- Clear level 1 buffer --- */
    buffer    = state->bufLevel1;
    wordCount = state->sizeLevel1;

    if (buffer == NULL) {
        fn_800DD970(sErrClearNotInit);
    } else if (wordCount > 0) {
        i = 0;
        if (wordCount > 8) {
            u32* ptr = buffer;
            u32  chunks = (wordCount - 8 + 7) >> 3;
            u32  remaining = wordCount - 8;
            if (remaining > 0) {
                u32 c;
                for (c = 0; c < chunks; c++) {
                    ptr[0] = 0; ptr[1] = 0; ptr[2] = 0; ptr[3] = 0;
                    ptr[4] = 0; ptr[5] = 0; ptr[6] = 0; ptr[7] = 0;
                    ptr += 8;
                    i += 8;
                }
            }
        }
        {
            u32* ptr = buffer + i;
            u32  rem = wordCount - i;
            u32  j;
            for (j = 0; j < rem; j++) {
                ptr[j] = 0;
            }
        }
    }

    /* --- Clear level 2 buffer --- */
    buffer    = state->bufLevel2;
    wordCount = state->sizeLevel2;

    if (buffer == NULL) {
        fn_800DD970(sErrClearNotInit);
    } else if (wordCount > 0) {
        i = 0;
        if (wordCount > 8) {
            u32* ptr = buffer;
            u32  chunks = (wordCount - 8 + 7) >> 3;
            u32  remaining = wordCount - 8;
            if (remaining > 0) {
                u32 c;
                for (c = 0; c < chunks; c++) {
                    ptr[0] = 0; ptr[1] = 0; ptr[2] = 0; ptr[3] = 0;
                    ptr[4] = 0; ptr[5] = 0; ptr[6] = 0; ptr[7] = 0;
                    ptr += 8;
                    i += 8;
                }
            }
        }
        {
            u32* ptr = buffer + i;
            u32  rem = wordCount - i;
            u32  j;
            for (j = 0; j < rem; j++) {
                ptr[j] = 0;
            }
        }
    }
}

/* =========================================================================
 * GSflagSetBitValue (GSflagInitBitPos) -- 0x1A4 bytes
 * ========================================================================= */

/**
 * Validate flag definitions and compute bit offsets.
 *
 * Iterates through every flag definition entry. For each entry:
 *   1. Validates that the bit width is > 0 and <= GSFLAG_MAX_BITS.
 *      If invalid, clamps to a valid range and logs an error.
 *   2. Computes the starting bit offset within the appropriate
 *      storage level's buffer and stores it in the entry's bitOffset.
 *   3. Tracks the total bits used per level.
 *
 * After processing all entries, validates that the total bits
 * used per level do not exceed the allocated buffer sizes.
 *
 * The level is determined by bits [7:6] of each entry's typeAndWidth:
 *   00 (level 1) -> totalBits0
 *   01 (level 2) -> totalBits1
 *   10 (level 3) -> totalBits2
 */
void GSflagSetBitValue(GSflagDef* defTable, u32 flagCount,
                       u32 maxBits0, u32 maxBits1, u32 maxBits2, u32 reserved) {
    u32         i;
    GSflagDef*  entry;
    u16         totalBits0;   /* Bits used in level 0 (permanent) */
    u16         totalBits1;   /* Bits used in level 1 (session) */
    u16         totalBits2;   /* Bits used in level 2 (map) */
    u8          bitWidth;
    u8          levelCode;

    totalBits0 = 0;
    totalBits1 = 0;
    totalBits2 = 0;
    entry = defTable;

    for (i = 0; i < flagCount; i++) {
        bitWidth = entry->typeAndWidth & 0x3F;

        /* Validate bit width */
        if (bitWidth > GSFLAG_MAX_BITS) {
            fn_800DD970(sErrInitOverflowBitLen, i, bitWidth, GSFLAG_MAX_BITS);
            /* Clamp to maximum */
            entry->typeAndWidth = (entry->typeAndWidth & 0xC0) | GSFLAG_MAX_BITS;
        } else if (bitWidth == 0) {
            fn_800DD970(sErrInitZeroBitLen, i, bitWidth, GSFLAG_MAX_BITS);
            /* Set to 1-bit minimum */
            entry->typeAndWidth = (entry->typeAndWidth & 0xC0) | 1;
        }

        bitWidth = entry->typeAndWidth & 0x3F;

        /* Determine storage level from upper 2 bits */
        levelCode = (entry->typeAndWidth >> 6) & 0x3;

        switch (levelCode) {
        case 1:
            /* Level 0: Permanent flags */
            entry->bitOffset = totalBits0;
            totalBits0 += bitWidth;
            break;
        case 2:
            /* Level 1: Session flags */
            entry->bitOffset = totalBits1;
            totalBits1 += bitWidth;
            break;
        case 3:
            /* Level 2: Map-local flags */
            entry->bitOffset = totalBits2;
            totalBits2 += bitWidth;
            break;
        default:
            /* Level 0 or invalid -- skip offset assignment */
            break;
        }

        entry++;
    }

    /* Validate total bits used vs buffer sizes */
    {
        u32 neededWords0 = ((u32)totalBits0 + 31) / 32;
        if (neededWords0 >= maxBits0) {
            fn_800DD970(sErrInitOverflowLevel0, neededWords0, maxBits0);
        }
    }

    {
        u32 neededWords1 = ((u32)totalBits1 + 31) / 32;
        if (neededWords1 >= maxBits1) {
            fn_800DD970(sErrInitOverflowLevel1, neededWords1, maxBits1);
        }
    }

    {
        u32 neededWords2 = ((u32)totalBits2 + 31) / 32;
        if (neededWords2 >= maxBits2) {
            fn_800DD970(sErrInitOverflowLevel2, neededWords2, maxBits2);
        }
    }
}
