/**
 * @file cardesavedata_r51_80083AF4_suffix.c
 * @brief cardeArenaWalk / cardeArenaCountRecords (0x80083AF4 - 0x80083CBC): walk the Card-e
 *        save-data arena's grid records, returning the record at an index
 *        (or the terminating slot for a negative index) or the record count.
 *
 * Function-boundary carve of the Card-e save-data TU (see cardesavedata.c),
 * text only, GC/1.3 -O4,p with the TU's unit-wide -opt nopeephole, no
 * pragmas. The same bodies are in cardesavedata.c.
 *
 * RULE-EXCEPTION(user-approved): single-use inline helpers - see
 * docs/RULE_EXCEPTIONS.md. Retail writes the result and the count through
 * a NULL-checked out-pointer (addi r0,r1,8; cmplwi r0,0; beq; stw), and the
 * result lives in that stack slot across the loop; the two out-pointer
 * setters reproduce it. Plain locals give 81.4%/90.6%.
 */
#include "dolphin/types.h"

typedef struct CardEGridEntry {
    u16 id;
    u8 pad02[0x18];
    u8 key;
    s8 layers;
    s8 rows;
    s8 columns;
    u8 pad1E[6];
    u8 data[1];
} CardEGridEntry;

static inline u32 CardEGridEntrySize(CardEGridEntry* entry)
{
    return 0x24 + entry->layers *
           (0x76 + ((entry->rows * entry->columns) << 4));
}

/* RULE-EXCEPTION(user-approved): single-use out-pointer setters - see docs/RULE_EXCEPTIONS.md */
static inline void CardEGridSetEntry(CardEGridEntry** entryOut,
                                     CardEGridEntry* entry)
{
    if (entryOut != NULL) {
        *entryOut = entry;
    }
}

void* cardeArenaWalk(void* arena, s32 index)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    CardEGridEntry* result;
    u8* end;
    s32 currentIndex;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    CardEGridSetEntry(&result, NULL);
    currentIndex = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        if (currentIndex == index) {
            result = entry;
        }
        currentIndex++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    if (index < 0) {
        result = entry;
    }
    return result;
}

static inline void CardEGridSetCount(s32* countOut, s32 count)
{
    if (countOut != NULL) {
        *countOut = count;
    }
}

/* Count well-formed records in the Card-e save-data arena. */
s32 cardeArenaCountRecords(void* arena)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    u8* end;
    s32 count;
    s32 currentCount;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    currentCount = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        currentCount++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    CardEGridSetCount(&count, currentCount);
    return count;
}
