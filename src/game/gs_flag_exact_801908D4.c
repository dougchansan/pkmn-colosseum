/**
 * @file gs_flag_exact_801908D4.c
 * @brief GS flag module tail, .text 0x801908D4 - 0x80190E34: GSflagClear,
 *        GSflagInit (GSflagInit) and GSflagInitBitPos.
 *
 * GSflagInit is XD's GSflagInit (GSflag.o, 0x801A06C4 in the GXXE01 map,
 * StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af; trevor403/xd-asm @ b1087f18
 * FUN_801a06c4): it sets up the bit positions, stores the three buffers and
 * their word counts, then clears levels 1-3. XD calls GSflagClear
 * (FUN_801a05f0) three times; Colosseum inlines it, so the three zero-fill
 * loops are GSflagClear's body with constant levels. The inline needs
 * GSflagClear in the same object, which is why this unit covers all three
 * functions (formerly gs_flag_exact_801908D4 plus the
 * gs_flag_candidate_801909A8 residual).
 *
 * RULE-EXCEPTION(title-path): extern named stand-ins for this TU's .rodata
 * strings (lbl_802741F8, lbl_802742B8 stay in rodata_802741F8.c with the
 * rest of the GS flag module's messages) -- see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

typedef struct FlagStateEntry {
    u32 wordCount;
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

extern FlagStateEntry* lbl_80478EEC;
extern u8* lbl_80478F98;
extern FlagDefinition* lbl_80478F9C;
extern const char lbl_802741F8[];
extern const char lbl_802742B8[];
extern void GSlogWrite(const char* format, ...);
void GSflagInitBitPos(FlagDefinition* definitions, u32 count, u32 capacity1,
                      u32 capacity2, u32 capacity3);

void GSflagClear(s32 level)
{
    FlagStateEntry* states;
    u32* buffer;
    u32 wordCount;
    u32 index;

    states = lbl_80478EEC;
    buffer = states[level].buffer;
    if (buffer == NULL) {
        GSlogWrite(lbl_802742B8);
    } else {
        wordCount = states[level].wordCount;
        for (index = 0; index < wordCount; index++) {
            buffer[index] = 0;
        }
    }
}

void GSflagInit(u32* buffer1, u32 count1, u32* buffer2, u32 count2,
                 u32* buffer3, u32 count3)
{
    GSflagInitBitPos(lbl_80478F9C, *(u32*)lbl_80478F98, count1, count2,
                     count3);

    lbl_80478EEC[1].buffer = buffer1;
    lbl_80478EEC[2].buffer = buffer2;
    lbl_80478EEC[3].buffer = buffer3;
    lbl_80478EEC[1].wordCount = count1;
    lbl_80478EEC[2].wordCount = count2;
    lbl_80478EEC[3].wordCount = count3;
    GSflagClear(1);
    GSflagClear(2);
    GSflagClear(3);
}

void GSflagInitBitPos(FlagDefinition* definitions, u32 count, u32 capacity1,
                      u32 capacity2, u32 capacity3)
{
    const char* messages = lbl_802741F8;
    FlagDefinition* current = definitions;
    u32 next1 = 0;
    u32 next2 = 0;
    u32 next3 = 0;
    u32 i;

    for (i = 0; i < count; current++, i++) {
        u32 width = current->typeAndWidth & 0x3F;
        u32 type;

        if (width > 32) {
            GSlogWrite(messages + 0xF8, i, width, 32);
            ((FlagTypeAndWidth*)&current->typeAndWidth)->width = 32;
        } else if (width == 0) {
            GSlogWrite(messages + 0x144, i, width, 32);
            ((FlagTypeAndWidth*)&current->typeAndWidth)->width = 1;
        }

        type = (current->typeAndWidth >> 6) & 3;
        switch (type) {
        case 1:
            current->bitPosition = next1;
            next1 += current->typeAndWidth & 0x3F;
            break;
        case 2:
            current->bitPosition = next2;
            next2 += current->typeAndWidth & 0x3F;
            break;
        case 3:
            current->bitPosition = next3;
            next3 += current->typeAndWidth & 0x3F;
            break;
        }
    }

    if (capacity1 <= (((u16)next1 + 31) / 32)) {
        GSlogWrite(messages + 0x18C, ((u16)next1 + 31) / 32, capacity1);
    }
    if (capacity2 <= (((u16)next2 + 31) / 32)) {
        GSlogWrite(messages + 0x1D8, ((u16)next2 + 31) / 32, capacity2);
    }
    if (capacity3 <= (((u16)next3 + 31) / 32)) {
        GSlogWrite(messages + 0x224, ((u16)next3 + 31) / 32, capacity3);
    }
}
