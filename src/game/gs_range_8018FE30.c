/**
 * @file gs_range_8018FE30.c
 * @brief gs-engine, 0x8018FE30 - 0x80190E34.
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

typedef struct FlagConfig {
    u32 count;
    s16 head;
} FlagConfig;

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

void fn_801903B0(s32 flagId);
void _flagSet(s32 flagId, u32 value);
extern void* fn_800FF56C(void);
extern void* savedataGetStatus(u8* data, u16 index);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern void fn_8012F1FC(s32 slot);
extern void fn_8012F40C(s32 slot);
extern s32 heroMoveDismissMember(s32 idx);
extern void heroBiosSetPokedoru(u16 value);
extern void floorChangePos(u32 arg0, void* data, f32 posX, f32 posY, f32 posZ);

void fn_8018FE30(s32 flagId)
{
    extern FlagDefinition* lbl_80478F9C;
    extern FlagConfig* lbl_80478F98;
    extern u32** lbl_80478ED4;
    extern u8* lbl_80478EE4;
    extern u16* lbl_80478EF4;
    extern FlagSceneEntry* lbl_80478EFC;
    FlagDefinition* definitions = lbl_80478F9C;
    FlagDefinition* definition;
    s32 current;
    u16* itemSwapTable;
    FlagSceneEntry* scene;
    u8* memberFlags;
    u32 value;
    s32 i;

    if (flagId < 0) {
        return;
    }

    for (current = flagId; current != -1; current = definitions[current].next) {
        if (definitions[current].initialValue != 0) {
            fn_801903B0(current);
        }
    }

    current = lbl_80478F98->head;
    while (current != flagId && current != -1) {
        definition = &definitions[current];

        if (definition->initialValue != 0) {
            value = lbl_80478ED4[definition->initialValue][0];
            _flagSet(current, value);
        }

        if (definition->itemSwap != 0) {
            itemSwapTable = &lbl_80478EF4[definition->itemSwap * 2];
            heroItemDecItemDataId(0, itemSwapTable[1], 1, -1);
            heroItemAddItemDataId(0, itemSwapTable[0], 1, -1);
        }
        current = definition->next;
    }

    definition = &definitions[flagId];
    if (definition->itemSwap != 0) {
        itemSwapTable = &lbl_80478EF4[definition->itemSwap * 2];
        heroItemDecItemDataId(0, itemSwapTable[1], 1, -1);
        heroItemAddItemDataId(0, itemSwapTable[0], 1, -1);
    }

    if (definition->event != 0) {
        scene = &lbl_80478EFC[definition->event];
        if (scene->memberFlags != 0) {
            fn_8012F1FC(0);
            fn_8012F40C(0);
            heroMoveDismissMember(1);

            memberFlags = lbl_80478EE4 + scene->memberFlags * 2;
            for (i = 0; i < 2; i++) {
                if (memberFlags[i] != 0) {
                    fn_8012F1FC(i);
                }
            }
        }

        savedataGetStatus(0, 2);
        heroBiosSetPokedoru(scene->pokedoru);
        if (scene->floorId == 0) {
            fn_800FF56C();
        }
        floorChangePos(0, scene, scene->posX, scene->posY, scene->posZ);
    }
}

void fn_801909A8(u32* buffer1, u32 count1, u32* buffer2, u32 count2,
                 u32* buffer3, u32 count3)
{
    extern FlagInitState* lbl_80478EEC;
    extern u8* lbl_80478F98;
    extern FlagDefinition* lbl_80478F9C;
    FlagInitState* state;
    u32 i;

    GSflagInitBitPos(lbl_80478F9C, *(u32*)lbl_80478F98, count1, count2,
                     count3);

    state = lbl_80478EEC;
    state->buffer1 = buffer1;
    state->buffer2 = buffer2;
    state->buffer3 = buffer3;
    state->count1 = count1;
    state->count2 = count2;
    state->count3 = count3;

    if (state->buffer1 == NULL) {
        GSlogWrite(lbl_802741F8 + 0x2C0);
    } else {
        for (i = 0; i < state->count1; i++) {
            state->buffer1[i] = 0;
        }
    }

    if (state->buffer2 == NULL) {
        GSlogWrite(lbl_802741F8 + 0x2C0);
    } else {
        for (i = 0; i < state->count2; i++) {
            state->buffer2[i] = 0;
        }
    }

    if (state->buffer3 == NULL) {
        GSlogWrite(lbl_802741F8 + 0x2C0);
    } else {
        for (i = 0; i < state->count3; i++) {
            state->buffer3[i] = 0;
        }
    }
}

void GSflagInitBitPos(FlagDefinition* definitions, u32 count, u32 capacity1,
                      u32 capacity2, u32 capacity3)
{
    const char* messages = lbl_802741F8;
    u16 next1 = 0;
    u16 next2 = 0;
    u16 next3 = 0;
    u32 i;

    for (i = 0; i < count; i++, definitions++) {
        u32 width = definitions->typeAndWidth & 0x3F;
        u32 type;

        if (width > 32) {
            GSlogWrite(messages + 0xF8, i, width, 32);
            definitions->typeAndWidth =
                (definitions->typeAndWidth & 0xC0) | 32;
        } else if (width == 0) {
            GSlogWrite(messages + 0x144, i, width, 32);
            definitions->typeAndWidth =
                (definitions->typeAndWidth & 0xC0) | 1;
        }

        type = (definitions->typeAndWidth >> 6) & 3;
        switch (type) {
        case 1:
            definitions->bitPosition = next1;
            next1 += definitions->typeAndWidth & 0x3F;
            break;
        case 2:
            definitions->bitPosition = next2;
            next2 += definitions->typeAndWidth & 0x3F;
            break;
        case 3:
            definitions->bitPosition = next3;
            next3 += definitions->typeAndWidth & 0x3F;
            break;
        }
    }

    if (capacity1 <= ((next1 + 31) >> 5)) {
        GSlogWrite(messages + 0x18C, (next1 + 31) >> 5, capacity1);
    }
    if (capacity2 <= ((next2 + 31) >> 5)) {
        GSlogWrite(messages + 0x1D8, (next2 + 31) >> 5, capacity2);
    }
    if (capacity3 <= ((next3 + 31) >> 5)) {
        GSlogWrite(messages + 0x224, (next3 + 31) >> 5, capacity3);
    }
}

u8 fn_801902E0(s32 flagId)
{
    extern u8* lbl_80478F9C;
    extern FlagStateEntry* lbl_80478EEC;
    extern u32 lbl_8036C568[];
    extern const char lbl_80274284[];
    extern void GSlogWrite(const char* fmt, ...);
    u32 typeAndWidth;
    u32* buffer;
    u8* definitions;
    u32 definitionOffset;
    FlagStateEntry* states;
    u32 bitWidth;
    u32 bitOffset;
    u32 wordIndex;
    u32 bitPosition;
    u32 lowWord;
    u32 highWord;
    u32 shiftAmount;
    u32 mask;
    u32 value;
    u8 result;

    definitionOffset = flagId << 3;
    definitions = lbl_80478F9C;
    states = lbl_80478EEC;
    typeAndWidth = definitions[definitionOffset];
    buffer = states[(typeAndWidth & 0xC0) >> 6].buffer;

    if (buffer == 0) {
        GSlogWrite(lbl_80274284);
        value = 0;
    } else {
        definitions += definitionOffset;
        bitWidth = typeAndWidth & 0x3F;
        bitOffset = *(u16*)(definitions + 4);
        wordIndex = bitOffset >> 5;
        bitPosition = bitOffset & 0x1F;

        if (bitWidth > 1) {
            lowWord = buffer[wordIndex];
            highWord = buffer[wordIndex + 1];
            shiftAmount = 32 - bitPosition;
            lowWord >>= bitPosition;
            highWord <<= shiftAmount;
            mask = lbl_8036C568[bitWidth];
            value = (highWord | lowWord) & mask;
        } else {
            value = buffer[wordIndex];
            value = (value >> bitPosition) & 1;
        }
    }

    if (value == 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
}

#define DEFINE_FLAG_SET(name, args, fixedValue)                               \
    void name args                                                             \
    {                                                                          \
        extern u8* lbl_80478F9C;                                               \
        extern FlagStateEntry* lbl_80478EEC;                                   \
        extern u32 lbl_8036C568[];                                             \
        u8* definition = lbl_80478F9C + (flagId << 3);                         \
        u32 typeAndWidth = definition[0];                                      \
        u32* buffer = lbl_80478EEC[(typeAndWidth & 0xC0) >> 6].buffer;         \
        u32 value = fixedValue;                                                \
        u32 bitWidth;                                                          \
        u32 bitOffset;                                                         \
        u32 wordIndex;                                                         \
        u32 bitPosition;                                                       \
        u32 mask;                                                              \
                                                                               \
        if (buffer == NULL) {                                                  \
            GSlogWrite(lbl_802741F8);                                          \
            return;                                                            \
        }                                                                      \
        bitWidth = typeAndWidth & 0x3F;                                        \
        bitOffset = *(u16*)(definition + 4);                                   \
        if (32 - __cntlzw(value) > bitWidth) {                                 \
            GSlogWrite(lbl_802741F8 + 0x34, flagId, value, value,              \
                       32 - __cntlzw(value), bitWidth);                         \
            value &= lbl_8036C568[bitWidth];                                   \
        }                                                                      \
        wordIndex = bitOffset >> 5;                                            \
        bitPosition = bitOffset & 0x1F;                                        \
        if (bitWidth > 1) {                                                    \
            mask = lbl_8036C568[bitWidth];                                     \
            buffer[wordIndex] = (buffer[wordIndex] & ~(mask << bitPosition))   \
                              | (value << bitPosition);                         \
            if (bitWidth + bitPosition >= 32) {                                \
                u32 remaining = bitWidth + bitPosition - 32;                   \
                mask = lbl_8036C568[remaining];                                \
                buffer[wordIndex + 1] = (buffer[wordIndex + 1] & ~mask)        \
                                      | (value >> (bitWidth - remaining));      \
            }                                                                  \
        } else if (value == 0) {                                               \
            buffer[wordIndex] &= ~(1u << bitPosition);                         \
        } else {                                                               \
            buffer[wordIndex] |= 1u << bitPosition;                            \
        }                                                                      \
    }

DEFINE_FLAG_SET(fn_801903B0, (s32 flagId), 0)
DEFINE_FLAG_SET(fn_80190528, (s32 flagId), 1)
DEFINE_FLAG_SET(_flagSet, (s32 flagId, u32 valueArg), valueArg)

#undef DEFINE_FLAG_SET

u32 fn_801906A0(s32 flagId)
{
    extern u8* lbl_80478F9C;
    extern FlagStateEntry* lbl_80478EEC;
    extern u32 lbl_8036C568[];
    extern const char lbl_80274284[];
    extern void GSlogWrite(const char* fmt, ...);
    u32 typeAndWidth;
    u32* buffer;
    u8* definitions;
    u32 definitionOffset;
    FlagStateEntry* states;
    u32 bitWidth;
    u32 bitOffset;
    u32 wordIndex;
    u32 bitPosition;
    u32 lowWord;
    u32 highWord;
    u32 shiftAmount;
    u32 mask;
    u32 value;

    definitionOffset = flagId << 3;
    definitions = lbl_80478F9C;
    states = lbl_80478EEC;
    typeAndWidth = definitions[definitionOffset];
    buffer = states[(typeAndWidth & 0xC0) >> 6].buffer;

    if (buffer == 0) {
        GSlogWrite(lbl_80274284);
        value = 0;
    } else {
        definitions += definitionOffset;
        bitWidth = typeAndWidth & 0x3F;
        bitOffset = *(u16*)(definitions + 4);
        wordIndex = bitOffset >> 5;
        bitPosition = bitOffset & 0x1F;

        if (bitWidth > 1) {
            lowWord = buffer[wordIndex];
            highWord = buffer[wordIndex + 1];
            shiftAmount = 32 - bitPosition;
            lowWord >>= bitPosition;
            highWord <<= shiftAmount;
            mask = lbl_8036C568[bitWidth];
            value = (highWord | lowWord) & mask;
        } else {
            value = buffer[wordIndex];
            value = (value >> bitPosition) & 1;
        }
    }

    return value;
}

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
