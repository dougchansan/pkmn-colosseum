/**
 * @file gs_range_8018FE30.c
 * @brief gs-engine, 0x8018FE30 - 0x801902E0, fn_801903B0/fn_80190528
 *        (0x801903B0 - 0x801906A0; see gs_range_8018FE30_suffix_801903B0.c)
 *        and _flagSet (0x8019075C - 0x801908D4; see
 *        gs_range_8018FE30_suffix_8019075C.c).
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

typedef struct FlagStateEntry {
    u32 unused;
    u32* buffer;
} FlagStateEntry;

typedef struct FlagDefinition {
    u8 typeAndWidth;
    u8 initialValue;
    u8 itemSwap;
    u8 event;
    u16 bitPosition;
    s16 next;
} FlagDefinition;

typedef struct FlagTypeAndWidth {
    u8 type : 2;
    u8 width : 6;
} FlagTypeAndWidth;

typedef struct FlagConfig {
    u32 count;
    s16 head;
} FlagConfig;

typedef struct FlagItemSwap {
    u16 addItem;
    u16 removeItem;
} FlagItemSwap;

typedef struct FlagSceneEntry {
    u8 memberFlags;
    u8 pad_01;
    u16 floorId;
    u16 pokedoru;
    u16 pad_06;
    f32 posX;
    f32 posY;
    f32 posZ;
    void* data;
} FlagSceneEntry;

extern const char lbl_802741F8[];
extern const char lbl_802742B8[];
extern void GSlogWrite(const char* fmt, ...);
void GSflagInitBitPos(FlagDefinition* definitions, u32 count, u32 capacity1,
                      u32 capacity2, u32 capacity3);

typedef struct FlagInitState {
    u32 _pad00[2];
    u32 count1;
    u32* buffer1;
    u32 count2;
    u32* buffer2;
    u32 count3;
    u32* buffer3;
} FlagInitState;

void std079_flagSetTrue(s32 flagId);
void _flagSet(s32 flagId, u32 value);
extern u32 std092_getCurrentRoomID(void);
extern void* savedataGetStatus(u8* data, u16 index);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern void partnerSetFollowing(s32 slot);
extern void partnerSetFollowState(s32 slot);
extern s32 heroMoveDismissMember(s32 idx);
extern void heroBiosSetPokedoru(u8* ptr, s32 value);
extern void floorChangePos(u32 floorId, s16 direction, f32 x, f32 y, f32 z);


/* GSflagClear, GSflagInit (XD GSflagInit) and GSflagInitBitPos, 0x801908D4 -
 * 0x80190E34, are linked from gs_flag_exact_801908D4.c. */

/* fn_801902E0 (XD GSflagTest) is linked from gs_flag_test_exact_801902E0.c. */

/* XD's _flagSet(buffer, defs, flagId, value) (GSflag.o, trevor403/xd-asm
 * @ b1087f18 FUN_801a0490), inlined here into each setter the way XD's
 * GSflagSet (FUN_801a03a4) calls it with the level's buffer.
 *
 * The multi-bit branch reads each word into `bits`, clears the field in
 * place and ORs in a separately named `shifted` value: that keeps the loaded
 * word in the register the andc writes back, puts the shifted value first in
 * the or, and declaring `word` before `bitPosition` gives retail's r6/r7
 * colouring of the bit position and word pointer. */
static inline u32 flagGetBitLength(u32 value)
{
    return 32 - __cntlzw(value);
}

static inline void flagSetValue(u32* buffer, FlagDefinition* defs, s32 flagId,
                                u32 value)
{
    extern u32 lbl_8036C568[];
    extern const char lbl_8027422C[];
    u32 bitWidth;
    u32 bitOffset;
    u32 length;
    u32 wordIndex;
    u32 end;
    u32* word;
    u32 bitPosition;

    if (buffer == NULL) {
        GSlogWrite(lbl_802741F8);
        return;
    }
    bitWidth = defs[flagId].typeAndWidth & 0x3F;
    bitOffset = defs[flagId].bitPosition;
    length = flagGetBitLength(value);
    if (length > bitWidth) {
        GSlogWrite(lbl_8027422C, flagId, value, value, length, bitWidth);
        value &= lbl_8036C568[bitWidth];
    }
    wordIndex = bitOffset >> 5;
    bitPosition = bitOffset & 0x1F;
    if (bitWidth > 1) {
        u32 bits;
        u32 shifted;
        word = &buffer[wordIndex];
        end = bitWidth + bitPosition;
        bits = word[0];
        bits &= ~(lbl_8036C568[bitWidth] << bitPosition);
        shifted = value << bitPosition;
        word[0] = shifted | bits;
        if (end >= 32) {
            u32 spill = end - 32;
            bits = word[1];
            bits &= ~lbl_8036C568[spill];
            shifted = value >> (bitWidth - spill);
            word[1] = shifted | bits;
        }
    } else if (value == 0) {
        buffer[wordIndex] &= ~(1 << bitPosition);
    } else {
        buffer[wordIndex] |= 1 << bitPosition;
    }
}

extern FlagDefinition* lbl_80478F9C;
extern FlagStateEntry* lbl_80478EEC;

#if !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
void std079_flagSetTrue(s32 flagId)
{
    flagSetValue(
        lbl_80478EEC[(lbl_80478F9C[flagId].typeAndWidth & 0xC0) >> 6].buffer,
        lbl_80478F9C, flagId, 0);
}

void std078_flagSetFalse(s32 flagId)
{
    flagSetValue(
        lbl_80478EEC[(lbl_80478F9C[flagId].typeAndWidth & 0xC0) >> 6].buffer,
        lbl_80478F9C, flagId, 1);
}
#endif
#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY)
void _flagSet(s32 flagId, u32 value)
{
    flagSetValue(
        lbl_80478EEC[(lbl_80478F9C[flagId].typeAndWidth & 0xC0) >> 6].buffer,
        lbl_80478F9C, flagId, value);
}
#endif

#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY) && !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
static inline s32 flagMemberSlot(s32 i)
{
    switch (i) {
    case 0:
        return 0;
    case 1:
        return 1;
    }
    return -1;
}

void dbgFlagApplyPreset(int flagId)
{
    extern FlagConfig* lbl_80478F98;
    extern u32* lbl_80478ED4;
    extern FlagItemSwap* lbl_80478EF4;
    extern u8 (*lbl_80478EE4)[2];
    extern FlagSceneEntry* lbl_80478EFC;
    int current;
    u32 event;
    u8 members;
    s32 i;
    s32 slot;
    u8 itemSwap;
    u32 floorId;

    if (flagId < 0) {
        return;
    }

    for (current = flagId; current != -1;
         current = lbl_80478F9C[current].next) {
        if (lbl_80478F9C[current].initialValue != 0) {
            std079_flagSetTrue(current);
        }
    }

    current = lbl_80478F98->head;
    while (current != flagId && current != -1) {
        if (lbl_80478F9C[current].initialValue != 0) {
            _flagSet(current, lbl_80478ED4[lbl_80478F9C[current].initialValue]);
        }
        itemSwap = lbl_80478F9C[current].itemSwap;
        if (itemSwap != 0) {
            heroItemDecItemDataId(0, lbl_80478EF4[itemSwap].removeItem, 1, -1);
            heroItemAddItemDataId(0, lbl_80478EF4[itemSwap].addItem, 1, -1);
        }
        current = lbl_80478F9C[current].next;
    }

    itemSwap = lbl_80478F9C[flagId].itemSwap;
    if (itemSwap != 0) {
        heroItemDecItemDataId(0, lbl_80478EF4[itemSwap].removeItem, 1, -1);
        heroItemAddItemDataId(0, lbl_80478EF4[itemSwap].addItem, 1, -1);
    }

    if (lbl_80478F9C[current].event != 0) {
        event = lbl_80478F9C[flagId].event;
        members = lbl_80478EFC[event].memberFlags;
        if (members != 0) {
            partnerSetFollowing(0);
            partnerSetFollowState(0);
            heroMoveDismissMember(1);
            for (i = 0; i < 2; i++) {
                slot = flagMemberSlot(i);
                if (slot >= 0 && lbl_80478EE4[members][i] != 0) {
                    partnerSetFollowing(slot);
                }
            }
        }

        heroBiosSetPokedoru(savedataGetStatus(0, 2), lbl_80478EFC[event].pokedoru);
        floorId = lbl_80478EFC[event].floorId;
        if (floorId == 0) {
            floorId = std092_getCurrentRoomID();
        }
        floorChangePos(floorId, 0, lbl_80478EFC[event].posX,
                       lbl_80478EFC[event].posY, lbl_80478EFC[event].posZ);
    }
}
#endif

/* fn_801906A0 (0x801906A0, XD GSflagGet with _flagGet inlined) is linked
 * from gs_flag_get_exact_801906A0.c. _flagSet (0x8019075C) is scored from
 * this file through gs_range_8018FE30_suffix_8019075C.c. */

#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY) && !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
void GSflagClear(s32 level)
{
    extern FlagStateEntry* lbl_80478EEC;
    extern const char lbl_802742B8[];
    extern void GSlogWrite(const char* fmt, ...);
    FlagStateEntry* states;
    u32* buffer;
    u32 wordCount;
    u32 i;

    states = lbl_80478EEC;
    buffer = states[level].buffer;
    if (buffer == 0) {
        GSlogWrite(lbl_802742B8);
    } else {
        wordCount = states[level].unused;
        for (i = 0; i < wordCount; i++) {
            buffer[i] = 0;
        }
    }
}
#endif
