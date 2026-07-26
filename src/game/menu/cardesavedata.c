/**
 * @file gs_range_8007FD64.c
 * @brief gs-engine code, 0x8007FD64 - 0x80088428 (28 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

/* ===== External function declarations (fn_80084A8C only) ===== */
extern void fn_8005CF2C();
extern void fn_800776E4();
extern void fn_8008ABA0();
extern void fn_80092E38();
extern void fn_80092FC8();
extern void fn_80093160();
extern void fn_800932F0();
extern void fn_800934E4();
extern void fn_80093610();
extern void fn_80093698();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void menuCloseCustom();
extern s32 menuIsCheck();
extern void menuGetEnablePort();
extern s32 menuSetEnablePort();
extern void windowGetFreeWork();
extern void windowGetKeyInfo();
extern void winMsgOpen();
extern void winMsgClose();
extern void menuOpen();
extern void windowSearchID();
extern void _threadSwitch();

/* ===== SDA globals (fn_80084A8C only) ===== */
extern u8 lbl_80478950;
extern u8 lbl_80478954;
extern u8 lbl_8047C1A0;
extern u8 lbl_8047C1A8;
extern u8 lbl_8047C1AC;
extern u8 lbl_8047C1B0;
extern u8 lbl_8047C1B8;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEB78[];
extern u8 lbl_8026F2E8[];
extern u8 lbl_8026F488[];
extern f32 lbl_8047C1C8;
extern f32 lbl_8047C1CC;

typedef struct CardEPadState {
    /* 0x00 */ u8 pad00[4];
    /* 0x04 */ u16 trigger;
    /* 0x06 */ u16 repeat;
} CardEPadState;

typedef struct CardEModelAnim {
    /* 0x00 */ u32 modelId;
    /* 0x04 */ s16 anim;
    /* 0x06 */ s16 animAlt;
} CardEModelAnim;

typedef struct CardEGridTable {
    /* 0x00 */ u32 selectedIconModel[4];
    /* 0x10 */ u16 selectedIconAnim;
    /* 0x12 */ u8 pad12[2];
    /* 0x14 */ CardEModelAnim cell[3][3];
    /* 0x5C */ u32 cursorModel[4];
    /* 0x6C */ u16 cursorAnim;
} CardEGridTable;

typedef struct CardESelection {
    /* 0x00 */ u16 id[3];
} CardESelection;

extern void* fn_801054B8();
extern void* fn_800F92D4(u32);
extern void fn_800ECCA8(void*, s16);
extern void fn_800ECA78(void*, f32);
extern void fn_800EC9DC(void*, f32);
extern void fn_800EC990(void*);
extern void fn_800ECB74(void*, u32);
extern u8 fn_800EC960(void*);
extern void fn_80166A28();
extern s32 fn_801666BC();

extern void GScharLenCpy(void*, const void*, u32);
extern u16 fn_800E2C04(u32, u32);
extern void* fn_800E27B0(u16);
extern u16 fn_800E202C(void*);
extern void fn_800E24B0(u16);
extern void fn_800E209C(u16);
extern s32 fn_80083BF8(void*);
extern void* fn_80083AF4(void*, s32);
extern void* windowSearchItemID(void*, s32);
extern void qsort(void*, u32, u32, s32 (*)(u32, u32));
extern void __assert();
extern char lbl_80268B88[];
extern char lbl_8047C140[];
extern const u16 lbl_8047C190[4];

typedef struct MenuCardEItem {
    u8 pad0[0x50];
    s16 x;
    s16 y;
    s16 h;
    s16 w;
} MenuCardEItem;

#define CARDE_CTX_U32(ctx, off) (*(u32*)((u8*)(ctx) + (off)))
#define CARDE_CTX_S16(ctx, off) (*(s16*)((u8*)(ctx) + (off)))

static void menuCardE_SetItem(void* ctx, u32 off, void* window, s32 id) {
    CARDE_CTX_U32(ctx, off) = (u32)windowSearchItemID(window, id);
}

static void menuCardE_CopyRect(void* ctx, u32 dst, u32 itemOff) {
    MenuCardEItem* item = (MenuCardEItem*)CARDE_CTX_U32(ctx, itemOff);

    CARDE_CTX_S16(ctx, dst + 0) = item->x;
    CARDE_CTX_S16(ctx, dst + 2) = item->y;
    CARDE_CTX_S16(ctx, dst + 6) = item->w;
    CARDE_CTX_S16(ctx, dst + 4) = item->h;
}

/* 0x8007FD64 | size: 0x58
 * menuCardE_CompareEntryPtrs: qsort-style comparator for MenuCardEEntry*
 * elements.
 */
s32 menuCardE_CompareEntryPtrs(u32 r3, u32 r4) {
    u32 r0;
    u32 r5;

    r5 = *(u32*)((u8*)r3 + 0x0);
    r4 = *(u32*)((u8*)r4 + 0x0);
    r3 = *(u8*)((u8*)r5 + 0x1C);
    r0 = *(u8*)((u8*)r4 + 0x1C);
    r3 = (s8)r3;
    r0 = (s8)r0;
    if ((s32)r3 < (s32)r0) {
        return 0x1;
    }
    if ((s32)r3 > (s32)r0) {
        return -0x1;
    }
    r3 = *(u8*)((u8*)r5 + 0x1A);
    r0 = *(u8*)((u8*)r4 + 0x1A);
    if (r3 < r0) {
        return -0x1;
    }
    r0 = r0 - r3;
    r3 = (u32)r0 >> 31;
    return r3;
}

extern void GScharCpy(void* dst, const void* src);
extern const u8 lbl_80268DC0[];
extern const u8 lbl_80478948[];
extern s32 fn_8008102C(void** object_ref, const u32* descriptor, s32 index,
                       s32 value, const char* text, s32 subindex);

/* Decode and validate a packed card-e record. */
u32 fn_80080310(void* output, const u8* packed, void* auxiliary)
{
    u16 text0[256];
    u16 text1[256];
    u16 text2[256];
    u16 text3[256];
    u16 text4[256];
    void* object;
    const u8* input;
    void* aux;
    s32 bitPosition;
    const u32* descriptor;
    s32 recordIndex;
    s32 descriptorIndex;
    s32 groupIndex;
    s32 elementIndex;
    s32 width;
    s32 remaining;
    s32 endBit;
    s32 cursor;
    u16 value;
    u8 valid;
    u8 descriptorValid;
    u16* text;

#define READ_PACKED_BITS(bit_count, destination)                           \
    do {                                                                   \
        endBit = bitPosition + (bit_count);                                \
        value = 0;                                                         \
        cursor = bitPosition;                                              \
        while (cursor < endBit) {                                          \
            value = (u16)(value << 1);                                     \
            if ((packed[cursor >> 3] & lbl_80478948[cursor & 7]) != 0) {   \
                value |= 1;                                                \
            }                                                              \
            cursor++;                                                      \
        }                                                                  \
        bitPosition = endBit;                                              \
        (destination) = value;                                             \
    } while (0)

#define DECODE_ONE(desc_ptr, record, scratch, check_result)                 \
    do {                                                                   \
        descriptor = (desc_ptr);                                           \
        width = (s32)descriptor[1];                                        \
        descriptorValid = 1;                                               \
        if (width < 16) {                                                   \
            for (elementIndex = 0;                                         \
                 elementIndex < (s32)descriptor[2]; elementIndex++) {       \
                READ_PACKED_BITS(width, value);                             \
                if (!fn_8008102C(&object, descriptor, (record), value, 0,  \
                                 elementIndex)) {                           \
                    descriptorValid = 0;                                   \
                }                                                          \
            }                                                              \
        } else {                                                           \
            text = (scratch);                                              \
            remaining = width;                                             \
            while (remaining > 16) {                                       \
                READ_PACKED_BITS(16, value);                                \
                *text++ = value;                                           \
                remaining -= 16;                                           \
            }                                                              \
            if (remaining != 0) {                                          \
                READ_PACKED_BITS(remaining, value);                         \
                *text++ = value;                                           \
            }                                                              \
            *text = 0;                                                     \
            if (!fn_8008102C(&object, descriptor, (record), 0,             \
                             (const char*)(scratch), -1)) {                 \
                descriptorValid = 0;                                       \
            }                                                              \
        }                                                                  \
        if ((check_result) && !descriptorValid) {                           \
            valid = 0;                                                     \
        }                                                                  \
    } while (0)

    object = output;
    input = packed;
    aux = auxiliary;
    bitPosition = 0;
    (void)input;
    (void)aux;
    memset(output, 0, 0xB20);

    descriptor = (const u32*)lbl_80268DC0;
    valid = 1;
    DECODE_ONE(descriptor, -1, text4, 0);

    bitPosition = 0;
    switch (*(s32*)output) {
    case 0:
        for (descriptorIndex = 0; descriptorIndex < 40;
             descriptorIndex++) {
            DECODE_ONE((const u32*)lbl_80268DC0 + descriptorIndex * 3,
                       -1, text3, 1);
        }

        for (recordIndex = 0; recordIndex < 9; recordIndex++) {
            for (groupIndex = 0; groupIndex < 8; groupIndex++) {
                DECODE_ONE((const u32*)(lbl_80268DC0 + 0x1E0) +
                               groupIndex * 3,
                           recordIndex, text2, 1);
            }
        }

        for (recordIndex = 0; recordIndex < 36; recordIndex++) {
            for (groupIndex = 0; groupIndex < 24; groupIndex++) {
                DECODE_ONE((const u32*)(lbl_80268DC0 + 0x240) +
                               groupIndex * 3,
                           recordIndex, text1, 1);
            }
        }
        break;

    case 1:
        for (descriptorIndex = 0; descriptorIndex < 3;
             descriptorIndex++) {
            DECODE_ONE((const u32*)(lbl_80268DC0 + 0x360) +
                           descriptorIndex * 3,
                       -1, text0, 1);
        }
        break;

    default:
        valid = 0;
        break;
    }

#undef DECODE_ONE
#undef READ_PACKED_BITS
    return valid != 0;
}


/* Apply one decoded card-e field and reject values outside its domain. */
s32 fn_8008102C(void** object_ref, const u32* descriptor, s32 index,
                s32 value, const char* text, s32 subindex)
{
    u8* object = (u8*)*object_ref;
    u32 field = descriptor[0];
    s32 i;
    u8* record;
    u16 half;

    switch (field) {
    case 0:
        *(s32*)object = value;
        return value >= 0 && value < 2;
    case 1:
        object[4] = (u8)value;
        return value > 0 && value < 6;
    case 2:
        object[5] = (u8)value;
        return value > 0 && value < 4;
    case 3:
        object[6] = (u8)value;
        return value > 0 && value < 10;
    case 4:
        object[7] = (u8)value;
        return value >= 0 && value <= 12;
    case 5:
        object[8] = (u8)value;
        return 1;
    case 6:
        GScharCpy(object + 0x0A, text);
        return 1;
    case 7:
        object[0x24] = (s8)(value - 1);
        return value >= 0 && value < 6;
    case 8:
        object[0x25] = (u8)value;
        return 1;
    case 9:
        object[0x26] = (s8)(value - 1);
        return (s8)object[0x26] >= 0 && (s8)object[0x26] < 5;
    case 10:
        GScharCpy(object + 0x28, text);
        return 1;
    case 11:
        GScharCpy(object + 0x38, text);
        return 1;
    case 12:
        GScharCpy(object + 0x48, text);
        return 1;
    case 13:
        object[0x58] = (s8)value;
        return value >= 1 && value <= 3;
    case 14:
        object[0x59] = (s8)value;
        return value >= 1 && value <= 6;
    case 15:
        object[0x5A] = (s8)value;
        return value >= 1 && value <= 5;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        object[0x5B + field - 16] = (s8)(value - 1);
        return value >= 0 && value <= 9;
    case 25:
    case 26:
    case 27:
        half = (u16)value;
        *(u16*)(object + 0x64 + (field - 25) * 2) = half;
        for (i = 0; i < 0x2F; i++) {
            if (((const u16*)(lbl_80268DC0 + 0x384))[i] == half) {
                return 1;
            }
        }
        return 0;
    case 28:
        object[0x6A] = (u8)value;
        return value >= 0 && value <= 0x24;
    case 29:
        object[0x6B] = (u8)value;
        return value >= 0 && value <= 0x24;
    case 30:
        object[0x6C] = (u8)value;
        return value >= 0 && value <= 0x24;
    case 31:
        GScharCpy(object + 0x6E, text);
        return 1;
    case 32:
        GScharCpy(object + 0x182, text);
        return 1;
    case 33:
        GScharCpy(object + 0x296, text);
        return 1;
    case 34:
        GScharCpy(object + 0xCA, text);
        return 1;
    case 35:
        GScharCpy(object + 0x1DE, text);
        return 1;
    case 36:
        GScharCpy(object + 0x2F2, text);
        return 1;
    case 37:
        GScharCpy(object + 0x126, text);
        return 1;
    case 38:
        GScharCpy(object + 0x23A, text);
        return 1;
    case 39:
        GScharCpy(object + 0x34E, text);
        return 1;
    case 40:
        GScharCpy(object + 0x3AC + index * 0x28, text);
        return 1;
    case 41:
        record = object + 0x3AC + index * 0x28;
        if (value == 2) {
            record[0x0C] = 1;
        } else if (value == 3) {
            record[0x0C] = 2;
        } else {
            record[0x0C] = 0;
        }
        return 1;
    case 42:
        record = object + 0x3AC + index * 0x28;
        record[0x0D + subindex] = (s8)value;
        return value >= 0 && value <= 0x24;
    case 43:
        record = object + 0x3AC + index * 0x28;
        half = (u16)value;
        *(u16*)(record + 0x12 + subindex * 2) = half;
        for (i = 0; i < 0x2F; i++) {
            if (((const u16*)(lbl_80268DC0 + 0x384))[i] == half) {
                return 1;
            }
        }
        return 0;
    case 44:
        *(u32*)(object + 0x3C8 + index * 0x28) = (u32)value;
        return 1;
    case 45:
        record = object + 0x3AC + index * 0x28;
        half = (u16)value;
        *(u16*)(record + 0x20) = half;
        for (i = 0; i < 0x13; i++) {
            if (((const u16*)(lbl_80268DC0 + 0x618))[i] == half) {
                return 1;
            }
        }
        return 0;
    case 46:
        record = object + 0x3AC + index * 0x28;
        *(u16*)(record + 0x22) = (u16)value;
        for (i = 0; i < 9; i++) {
            if ((s8)object[0x5B + i] == index) {
                return 1;
            }
        }
        return 0;
    default:
        return 0;
    }
}

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

typedef struct CardEGridMatrixCell {
    u8 pad00[0xC];
    u8 valid;
    u8 pad0D[3];
} CardEGridMatrixCell;

typedef struct CardEGridLayer {
    u8 pad00[0x76];
    CardEGridMatrixCell cells[1];
} CardEGridLayer;

static inline u32 CardEGridEntrySize(CardEGridEntry* entry)
{
    return 0x24 + entry->layers *
           (0x76 + ((entry->rows * entry->columns) << 4));
}

static inline void CardEGridValidate(CardEGridEntry* entry)
{
    if (entry->layers <= 3 && entry->rows <= 6 && entry->columns <= 5) {
        return;
    }
    entry->id = 0;
}

static inline s32 CardEGridLayerIsValid(CardEGridEntry* entry, s8 layer)
{
    return layer >= 0 && layer < entry->layers;
}

/* Clear an empty decoded card-e grid entry. */
#pragma push
#pragma peephole off
void fn_80082650(CardEGridEntry* entry)
{
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    extern char lbl_8047C188[] __attribute__((section(".sdata2")));
    CardEGridLayer* layer;
    s32 i;
    u8 valid;

    if (entry == NULL) {
        __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
    }
    if (!CardEGridLayerIsValid(entry, 0)) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    layer = (CardEGridLayer*)entry->data;
    if (layer == NULL) {
        __assert(lbl_8026F1C8, 0x1F1, lbl_8047C188);
    }

    for (i = entry->rows * entry->columns; i > 0; i--) {
        if (layer->cells[0].valid != 0) {
            valid = 1;
            goto scan_done;
        }
        layer = (CardEGridLayer*)((u8*)layer + sizeof(CardEGridMatrixCell));
    }
    valid = 0;
scan_done:
    if (!valid) {
        entry->id = 0;
    }
}
#pragma pop

/* Return the start of one layer in a decoded card-e grid entry. */
#pragma push
#pragma peephole off
void* fn_80082FE4(CardEGridEntry* entry, s8 layer)
{
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    u8* layerEntry;

    if (entry == NULL) {
        __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
    }
    if (!CardEGridLayerIsValid(entry, layer)) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    layerEntry = (u8*)entry;
    layerEntry += layer *
                  (0x76 + ((entry->rows * entry->columns) << 4));
    return layerEntry + 0x24;
}
#pragma pop

void* fn_800836AC(u8* arena, u8* descriptor, u8 create)
{
    extern void* savedataGetStatus(u32, u32);
    extern void fn_800CAA3C(void*, const void*);
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));

    CardEGridEntry* entry;
    CardEGridEntry* result;
    u8* base;
    u8* end;
    u8* next;
    u32 count;
    u32 i;
    u32 sliceSize;
    u32 entrySize;

    if (*(u32*)descriptor != 0) {
        return NULL;
    }
    if (arena != NULL) {
        base = arena;
    } else {
        base = savedataGetStatus(0, 0xD);
    }
    end = base + 0x4000;

    count = 0;
    entry = (CardEGridEntry*)base;
    while ((u8*)entry + 0x24 <= end && entry->id != 0) {
        CardEGridValidate(entry);
        count++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }

    entry = (CardEGridEntry*)base;
    result = NULL;
    for (i = 0; i < count; i++) {
        CardEGridValidate(entry);
        if (entry->key == descriptor[8]) {
            result = entry;
            break;
        }
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    if (result != NULL) {
        return result;
    }
    if (create == 0) {
        return NULL;
    }

    entry = (CardEGridEntry*)base;
    while ((u8*)entry + 0x24 <= end && entry->id != 0) {
        CardEGridValidate(entry);
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    sliceSize = 0x76 + (s8)descriptor[0x59] *
                         (s8)descriptor[0x5A] * 0x10;
    entrySize = 0x24 + (s8)descriptor[0x58] * sliceSize;
    next = (u8*)entry + entrySize;
    if (next > end) {
        return NULL;
    }

    memset(entry, 0, entrySize);
    fn_800CAA3C(entry, descriptor + 0x0A);
    entry->key = descriptor[8];
    entry->layers = descriptor[0x58];
    entry->rows = descriptor[0x59];
    entry->columns = descriptor[0x5A];
    for (i = 0; i < entry->layers; i++) {
        if (entry == NULL) {
            __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
        }
        if ((s32)i < 0 || (s32)i >= entry->layers) {
            __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
        }
        fn_800CAA3C((u8*)entry + 0x24 + i * sliceSize,
                    descriptor + 0x28 + i * 0x10);
    }
    return entry;
}

/* Return one well-formed record, or the terminating slot for a negative index. */
#pragma push
#pragma peephole off
static inline void CardEGridSetEntry(CardEGridEntry** entryOut,
                                     CardEGridEntry* entry)
{
    if (entryOut != NULL) {
        *entryOut = entry;
    }
}

void* fn_80083AF4(void* arena, s32 index)
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
s32 fn_80083BF8(void* arena)
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
#pragma pop


/* 0x8007FDBC | size: 0x554 */
void* fn_8007FDBC(void* window, const void* title) {
    char* table;
    u8* ctx;
    u16 handle;
    u16 listHandle;
    u16 oldHandle;
    s32 count;
    s32 i;
    s32 j;
    u32 bytes;
    u32* list;
    u16* ids0;
    u16* ids1;
    u16* ids2;
    u8* rowCtx;

    table = lbl_80268B88;

    handle = fn_800E2C04(0x500, 0x20);
    if (handle == 0) {
        __assert(table + 0x1F0, 0x1A2, lbl_8047C140);
    }

    ctx = fn_800E27B0(handle);
    memset(ctx, 0, 0x4E8);

    if (title != 0) {
        GScharLenCpy(ctx, title, 0x50);
        *(u16*)(ctx + 0x9E) = 0;
    } else {
        *(u16*)ctx = 0;
    }

    if (CARDE_CTX_U32(ctx, 0xB0) != 0) {
        oldHandle = fn_800E202C((void*)CARDE_CTX_U32(ctx, 0xB0));
        if (oldHandle == 0) {
            __assert(table + 0x1F0, 0x1AB, lbl_8047C140);
        }
        fn_800E24B0(oldHandle);
        fn_800E209C(oldHandle);
        CARDE_CTX_U32(ctx, 0xB0) = 0;
    }

    count = fn_80083BF8(0);
    CARDE_CTX_U32(ctx, 0xAC) = count;
    if (count != 0) {
        bytes = count * 4;
        listHandle = fn_800E2C04((bytes + 0x1F) & ~0x1F, 0x20);
        if (listHandle == 0) {
            __assert(table + 0x1F0, 0x1A2, lbl_8047C140);
        }
        list = fn_800E27B0(listHandle);
        memset(list, 0, bytes);
        CARDE_CTX_U32(ctx, 0xB0) = (u32)list;

        for (i = 0; i < count; i++) {
            list[i] = (u32)fn_80083AF4(0, i);
        }
        qsort(list, count, 4, menuCardE_CompareEntryPtrs);
    }

    CARDE_CTX_U32(ctx, 0xA4) = count != 0 ? 0 : -1;

    menuCardE_SetItem(ctx, 0x118, window, 0x79B);
    menuCardE_SetItem(ctx, 0x11C, window, 0x79C);
    menuCardE_SetItem(ctx, 0x120, window, 0x79D);
    menuCardE_SetItem(ctx, 0x124, window, 0x780);
    menuCardE_SetItem(ctx, 0x128, window, 0x781);
    menuCardE_SetItem(ctx, 0x12C, window, 0x782);
    menuCardE_SetItem(ctx, 0x130, window, 0x1193);
    menuCardE_SetItem(ctx, 0x134, window, 0x1195);
    menuCardE_SetItem(ctx, 0x138, window, 0x1194);
    menuCardE_SetItem(ctx, 0x13C, window, 0x796);
    menuCardE_SetItem(ctx, 0x140, window, 0x793);
    menuCardE_SetItem(ctx, 0x144, window, 0x797);
    menuCardE_SetItem(ctx, 0x148, window, 0x1196);
    menuCardE_SetItem(ctx, 0x14C, window, 0x792);
    menuCardE_SetItem(ctx, 0x150, window, 0x1126);
    menuCardE_SetItem(ctx, 0x154, window, 0x795);
    menuCardE_SetItem(ctx, 0x158, window, 0x791);
    menuCardE_SetItem(ctx, 0x15C, window, 0x1125);
    menuCardE_SetItem(ctx, 0x160, window, 0x799);
    menuCardE_SetItem(ctx, 0x164, window, 0x79A);
    menuCardE_SetItem(ctx, 0x168, window, 0x825);
    menuCardE_SetItem(ctx, 0x16C, window, 0x826);

    ids0 = (u16*)table;
    ids1 = (u16*)(table + 0x90);
    ids2 = (u16*)(table + 0x120);
    rowCtx = ctx;
    for (i = 0; i < 0x24; i++) {
        for (j = 0; j < 2; j++) {
            CARDE_CTX_U32(rowCtx, 0x170 + j * 0x90) =
                (u32)windowSearchItemID(window, ids0[j * 0x24]);
            CARDE_CTX_U32(rowCtx, 0x3B0 + j * 0x90) =
                (u32)windowSearchItemID(window, ids1[j * 0x24]);
            CARDE_CTX_U32(rowCtx, 0x290 + j * 0x90) =
                (u32)windowSearchItemID(window, ids2[j * 0x24]);
        }
        ids0++;
        ids1++;
        ids2++;
        rowCtx += 4;
    }

    menuCardE_SetItem(ctx, 0x4D0, window, 0x119A);
    menuCardE_SetItem(ctx, 0x4D4, window, 0x11C2);
    menuCardE_SetItem(ctx, 0x4D8, window, 0x790);
    menuCardE_SetItem(ctx, 0x4DC, window, 0x798);
    menuCardE_SetItem(ctx, 0x4E0, window, 0x78F);
    menuCardE_SetItem(ctx, 0x4E4, window, 0x794);

    menuCardE_CopyRect(ctx, 0xCE, 0x200);
    menuCardE_CopyRect(ctx, 0xD6, 0x440);
    menuCardE_CopyRect(ctx, 0xDE, 0x320);
    menuCardE_CopyRect(ctx, 0xE6, 0x118);
    menuCardE_CopyRect(ctx, 0xEE, 0x11C);
    menuCardE_CopyRect(ctx, 0xF6, 0x120);
    menuCardE_CopyRect(ctx, 0xFE, 0x15C);
    menuCardE_CopyRect(ctx, 0x106, 0x154);
    menuCardE_CopyRect(ctx, 0x10E, 0x14C);

    return ctx;
}


/* 0x80084034 | size: 0x4 */
void fn_80084034(void) {
}

#pragma peephole off
/* 0x80083CBC | size: 0x40 */
void fn_80083CBC(void* ptr) {
    memset(ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD), 0, 0x49CC);
}
#pragma peephole on

/* 0x80083CFC | size: 0x34 */
#pragma peephole off
void* fn_80083CFC(void* ptr) {
    return ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD);
}

/* Update the four-controller Card-e connection/status display. */
typedef struct CardEStatusWork {
    s32 state[4];
    s32 previousState[4];
    u8 refreshMessages;
    u8 initialized;
    u8 pad22[2];
    u32 field24;
    u32 field28;
    u32 pad2C;
    void* headerSprite[2];
    void* statusSprite[5];
    void* optionSprite[8][4];
} CardEStatusWork;

typedef struct CardEMessageEntry {
    u16 itemId;
    u16 initialMessage;
    u16 updatedMessage;
} CardEMessageEntry;

extern char lbl_8047C198[];

void fn_80084038(u8* window)
{
    const u32* stateFlags = (const u32*)lbl_8026F2E8;
    const CardEMessageEntry* messages =
        (const CardEMessageEntry*)(lbl_8026F2E8 + 0x70);
    const u8* portMasks = &lbl_80478950;
    CardEStatusWork* work;
    void* freeWork;
    void* sprite;
    u16 handle;
    u32 flags;
    s32 i;
    s32 row;
    u8 changed;
    u8 selected;

    if (window == 0) {
        ((void* (*)(u32))windowSearchID)(0xA6);
    }
    freeWork = ((void* (*)(u8*))windowGetFreeWork)(window);
    work = *(CardEStatusWork**)freeWork;

    if ((s8)window[1] == 0 && (s8)window[2] == 0) {
        handle = ((u16 (*)(u32, u32))fn_800E2C04)(0xE0, 0x20);
        if (handle == 0) {
            __assert(lbl_8026F2E8 + 0x184, 0xEA, lbl_8047C198);
        }
        work = ((CardEStatusWork* (*)(u16))fn_800E27B0)(handle);
        memset(work, 0, sizeof(CardEStatusWork));
        *(CardEStatusWork**)(((void* (*)(u8*))windowGetFreeWork)(window)) = work;

        work->refreshMessages = 1;
        work->initialized = 1;

#define FIND_STATUS_SPRITE(member, item) \
        work->member = ((void* (*)(u8*, u32))windowSearchItemID)(window, item)
        FIND_STATUS_SPRITE(headerSprite[0], 0x10F6);
        FIND_STATUS_SPRITE(headerSprite[1], 0x10F7);
        FIND_STATUS_SPRITE(statusSprite[0], 0x10D5);
        FIND_STATUS_SPRITE(statusSprite[1], 0x10DA);
        FIND_STATUS_SPRITE(statusSprite[2], 0x10E3);
        FIND_STATUS_SPRITE(statusSprite[3], 0x10F0);
        FIND_STATUS_SPRITE(statusSprite[4], 0x10F5);
        FIND_STATUS_SPRITE(optionSprite[0][0], 0x10D1);
        FIND_STATUS_SPRITE(optionSprite[1][0], 0x10D6);
        FIND_STATUS_SPRITE(optionSprite[2][0], 0x10DB);
        FIND_STATUS_SPRITE(optionSprite[3][0], 0x10DF);
        FIND_STATUS_SPRITE(optionSprite[4][0], 0x10E4);
        FIND_STATUS_SPRITE(optionSprite[5][0], 0x10E8);
        FIND_STATUS_SPRITE(optionSprite[6][0], 0x10EC);
        FIND_STATUS_SPRITE(optionSprite[7][0], 0x10F1);
        FIND_STATUS_SPRITE(optionSprite[0][1], 0x10D2);
        FIND_STATUS_SPRITE(optionSprite[1][1], 0x10D7);
        FIND_STATUS_SPRITE(optionSprite[2][1], 0x10DC);
        FIND_STATUS_SPRITE(optionSprite[3][1], 0x10E0);
        FIND_STATUS_SPRITE(optionSprite[4][1], 0x10E5);
        FIND_STATUS_SPRITE(optionSprite[5][1], 0x10E9);
        FIND_STATUS_SPRITE(optionSprite[6][1], 0x10ED);
        FIND_STATUS_SPRITE(optionSprite[7][1], 0x10F2);
        FIND_STATUS_SPRITE(optionSprite[0][2], 0x10D3);
        FIND_STATUS_SPRITE(optionSprite[1][2], 0x10D8);
        FIND_STATUS_SPRITE(optionSprite[2][2], 0x10DD);
        FIND_STATUS_SPRITE(optionSprite[3][2], 0x10E1);
        FIND_STATUS_SPRITE(optionSprite[4][2], 0x10E6);
        FIND_STATUS_SPRITE(optionSprite[5][2], 0x10EA);
        FIND_STATUS_SPRITE(optionSprite[6][2], 0x10EE);
        FIND_STATUS_SPRITE(optionSprite[7][2], 0x10F3);
        FIND_STATUS_SPRITE(optionSprite[0][3], 0x10D4);
        FIND_STATUS_SPRITE(optionSprite[1][3], 0x10D9);
        FIND_STATUS_SPRITE(optionSprite[2][3], 0x10DE);
        FIND_STATUS_SPRITE(optionSprite[3][3], 0x10E2);
        FIND_STATUS_SPRITE(optionSprite[4][3], 0x10E7);
        FIND_STATUS_SPRITE(optionSprite[5][3], 0x10EB);
        FIND_STATUS_SPRITE(optionSprite[6][3], 0x10EF);
        FIND_STATUS_SPRITE(optionSprite[7][3], 0x10F4);
#undef FIND_STATUS_SPRITE

        for (i = 0; i < 46; i++) {
            fn_801081F8(window, messages[i].itemId, messages[i].initialMessage);
        }
    } else if ((s8)window[1] == 3 && (s8)window[2] == 0) {
        for (i = 0; i < 46; i++) {
            fn_801081F8(window, messages[i].itemId, messages[i].updatedMessage);
        }
        window[2] = 1;
        work->refreshMessages = 1;
    } else if ((s8)window[1] == 5) {
        handle = ((u16 (*)(void*))fn_800E202C)(work);
        if (handle == 0) {
            __assert(lbl_8026F2E8 + 0x184, 0xF3, lbl_8047C198);
        }
        fn_800E24B0(handle);
        fn_800E209C(handle);
        return;
    }

    changed = 0;
    if ((s8)window[1] == 2 && work->refreshMessages != 0) {
        work->refreshMessages = 0;
        changed = 1;
    }

    for (i = 0; i < 4; i++) {
        if ((work->state[i] == 4 || work->state[i] == 5) &&
            !((u8 (*)(s32))fn_8008ABA0)(i + 1)) {
            menuSetEnablePort(((u8 (*)(void))menuGetEnablePort)() &
                              ~portMasks[i]);
            work->state[i] = 7;
            work->field28 = 8;
        }
    }

    for (i = 0; i < 4; i++) {
        if (work->previousState[i] != work->state[i]) {
            changed = 1;
        }
        work->previousState[i] = work->state[i];
    }
    if (!changed) {
        return;
    }

    if (work->headerSprite[0] != 0) {
        winSpriteSetDisp(work->headerSprite[0], 1);
    }
    if (work->headerSprite[1] != 0) {
        winSpriteSetDisp(work->headerSprite[1], 1);
    }

    flags = stateFlags[work->state[0]];
    for (row = 0; row < 5; row++) {
        if (work->statusSprite[row] != 0) {
            winSpriteSetDisp(work->statusSprite[row],
                             (flags & (0x100U << row)) != 0);
        }
    }

#define SET_STATUS_MESSAGE(item, active)                                      \
    do {                                                                       \
        sprite = (item);                                                       \
        if (sprite != 0) {                                                     \
            fn_801081F8(window, (u16)*(s16*)((u8*)sprite + 6),                 \
                          (active) ? 0x1BA : 0);                               \
            if (!(active)) {                                                   \
                *(s32*)((u8*)sprite + 0x64) = -1;                             \
            }                                                                  \
        }                                                                      \
    } while (0)

    if (work->refreshMessages == 0) {
        selected = work->state[0] == 9;
        SET_STATUS_MESSAGE(work->statusSprite[0], selected);
        SET_STATUS_MESSAGE(work->statusSprite[2], selected);
    }

    for (i = 0; i < 4; i++) {
        flags = stateFlags[work->state[i]];
        for (row = 0; row < 8; row++) {
            if (work->optionSprite[row][i] != 0) {
                winSpriteSetDisp(work->optionSprite[row][i],
                                 (flags & (1U << row)) != 0);
            }
        }

        if (work->state[i] == 6 || work->state[i] == 7 ||
            work->state[i] == 11) {
            if (work->headerSprite[0] != 0) {
                winSpriteSetDisp(work->headerSprite[0], 0);
            }
            if (work->headerSprite[1] != 0) {
                winSpriteSetDisp(work->headerSprite[1], 0);
            }
        }

        if (work->refreshMessages == 0) {
            selected = work->state[i] == 2;
            SET_STATUS_MESSAGE(work->optionSprite[0][i], selected);
            SET_STATUS_MESSAGE(work->optionSprite[3][i], selected);
            SET_STATUS_MESSAGE(work->optionSprite[4][i], selected);

            if ((flags & 4) != 0) {
                sprite = work->optionSprite[2][i];
                if (*(u32*)((u8*)sprite + 0x0C) == 0) {
                    fn_801081F8(window,
                                (u16)*(s16*)((u8*)sprite + 6),
                                lbl_8047C190[i]);
                }
            }
        }
    }
#undef SET_STATUS_MESSAGE
}

#pragma peephole on

#pragma push
#pragma peephole off
/* Run the Card-e transfer UI while temporarily reserving controller port 1. */
s32 fn_800849B4(s32 mode, s32 command, void* input, void* output)
{
    extern void fn_80093698(s32);
    extern u8 fn_80084A8C();
    extern void menuCloseCustom(s32, s32, s32);
    extern u8 menuIsCheck(s32);
    extern u8 menuSetEnablePort(u8);
    extern void winMsgClose(s32);
    u8 previousPort;
    u8 succeeded;
    s32 port;

    previousPort = menuSetEnablePort(1);
    succeeded = fn_80084A8C(mode, command, input, output);
    winMsgClose(0);
    if (menuIsCheck(0xE4) != 0) {
        menuCloseCustom(0xE4, 0, 1);
    }
    menuSetEnablePort(previousPort);
    for (port = 0; port < 3; port++) {
        fn_80093698(port);
    }

    if (succeeded != 0) {
        return 0;
    }
    return -1;
}
#pragma pop

/* 0x80084A8C | size: 0x305C */
void fn_80084A8C(void) {
    extern void fn_80087AE8();
    extern void fn_80128E04();
    extern void fn_80128E24();
    extern void savedataGetStatus();
    extern void heroInit();
    extern void heroBiosCopy();
    extern void msgctrlSetValue();
    extern void gamedataAttestBiosGetLangareaId();
    extern void gamedataBiosGetGamedataAtttestPtr();
    extern void fn_80166A28();
    extern void __assert();
    extern u8 jumptable_802EEB78[];
    u8 sp[0xBF0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r3 = (u32)&lbl_8026F2E8;
    r15 = 0x0;
    tmp = (u32)&lbl_8026F2E8;
    *(u32*)(sp + 0xC34) = tmp;
    r3 = 0x1;
    ((void(*)(void))fn_80093698)();
    while (1) {
        r5 = 0x0;
        r4 = r3 + 0x190;
        r3 = 0x1;
        ((void(*)(void))fn_800932F0)();
        if ((s32)r3 != 0) break;
        ((void(*)(void))_threadSwitch)();


    }
    r3 = 0xe4;
    r4 = 0x0;
    ((void(*)(void))menuOpen)();
    r3 = 0xe4;
    ((void(*)(void))windowSearchID)();
    r16 = r3;
    if (r16 == 0) {
        r4 = 0x1f4;
        r5 = (u32)&lbl_8047C1A0;
        r3 = r3 + 0x184;
        __assert();
    }
    if (r16 == 0) {
        r3 = 0xa6;
        ((void(*)(void))windowSearchID)();
        r16 = r3;
    }
    r3 = r16;
    ((void(*)(void))windowGetFreeWork)();
    r5 = 0x0;
    r16 = *(u32*)((u8*)r3 + 0x0);
    r30 = (u32)&lbl_80478954;
    r4 = tmp << 4;
    *(u32*)((u8*)r16 + 0x24) = tmp;
    r29 = r3 + 0x30;
    r29 = r29 + r4;
    *(u32*)((u8*)r16 + 0x2C) = tmp;
    tmp = *(u8*)&lbl_80478954;
    *(u8*)((u8*)r16 + 0x21) = tmp;
    tmp = 0x5;
    r3 = *(u8*)((u8*)r30 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r30 + 0x1;
    r5 = 0x1;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r4 + 0x1;
    r5 = 0x2;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r4 = r4 + 0x1;
    r5 = 0x3;
    r3 = *(u8*)((u8*)r4 + 0x0);
    r3 = (s8)r3;
    r5 = r3 << 2;
    r3 = *(u32*)(r29 + r5);
    *(u32*)(r16 + r5) = r3;
    if ((s32)r5 < 0) {
        r3 = *(u32*)(r16 + r5);
        if ((s32)r3 == 1) {
            *(u32*)(r16 + r5) = tmp;
    }
    }
    r24 = r16;
    tmp = tmp & 0x00000010;
    if (tmp != 0) {
        if ((s32)tmp != 0) {
        }
        if ((s32)tmp == 2) {
            }
        r3 = 0x0;
        r4 = 0x2;
        savedataGetStatus();
        r4 = 0x0;
        *(u8*)((u8*)r16 + 0x21) = r4;
        r4 = 0x8;
        tmp = tmp & 0x00000002;
        r18 = r3;
        *(u32*)((u8*)r16 + 0x0) = r4;
        if (tmp != 0) {
            r3 = 0x2f;
            r4 = 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x3d88;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            f27 = *(f32*)&lbl_8047C1A8;
            f31 = *(f64*)&lbl_8047C1B0;
            r17 = 0x43300000;
            f29 = *(f64*)&lbl_8047C1B8;
            f28 = *(f32*)&lbl_8047C1AC;
            while (f27 < f28) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0xC14) = tmp;
                f30 = f0 - f31;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;

            }
            r3 = r18;
            ((void(*)(void))fn_800776E4)();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r3 = 0xe4;
                r4 = 0x0;
                r5 = 0x1;
                ((void(*)(void))menuCloseCustom)();
                r3 = r18;
                r4 = 0x0;
                ((void(*)(void))fn_8005CF2C)();
                r3 = 0x0;
                return;
        }
        }
        if (tmp != 0) {
            r3 = tmp;
            r3 = *(u32*)((u8*)r3 + 0x0);
            if (r3 != 0) {
                r4 = r18;
                heroBiosCopy();
        }
        }
        tmp = 0xa;
        *(u32*)((u8*)r16 + 0x0) = tmp;
        }
    r25 = 0x0;
    tmp = tmp & 0x00000040;
    *(u32*)(sp + 0xC2C) = tmp;
    tmp = tmp & 0x1;
    *(u32*)(sp + 0xC28) = tmp;
    tmp = tmp & 0x00000002;
    *(u32*)(sp + 0xC24) = tmp;
    tmp = tmp & 0x00000008;
    *(u32*)(sp + 0xC20) = tmp;
    r31 = tmp & 0x00000020;
    do {
        r23 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r24 + 0x21) = r23;
        if (tmp != 0) {
            tmp = (s8)r23;
            tmp = tmp << 2;
            tmp = *(u32*)(r3 + tmp);
            if (tmp != 0) {
                r22 = tmp;
                goto L_80084E14;
            }
        }
        r22 = (u32)sp + 0xf4;
    L_80084E14:
        if (tmp != 0) {
            tmp = (s8)r23;
            if ((s32)tmp == 1) {
                goto L_80084E38;
            }
        }
        r21 = (u32)sp + 0x1c;
    L_80084E38:
        r28 = (s8)r23;
        r26 = r28 + 0x1;
        r27 = r28 << 2;
    L_80084E44:
        ((void(*)(void))menuGetEnablePort)();
        r4 = (u32)&lbl_80478950;
        tmp = *(u8*)(r4 + r28);
        tmp = r3 & ~tmp;
        r3 = tmp & 0xFF;
        ((void(*)(void))menuSetEnablePort)();
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp != 4) {
            tmp = 0x2;
            r4 = r26;
            *(u32*)(r27 + r24) = tmp;
            r3 = 0x2f;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x3c42;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            r3 = r24;
            r4 = 0x6;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r15 = 0x0;
            L_80084EB4:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))menuGetEnablePort)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))menuSetEnablePort)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_80084EB4;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_80085010;
                }
                }
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                msgctrlSetValue();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_80085010:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
            }
        }
        r4 = r26;
        r3 = 0x2f;
        msgctrlSetValue();
        r3 = 0x7;
        r4 = 0x3c43;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))winMsgOpen)();
        tmp = r15 & 0xFF;
        r3 = 0x3;
        *(u32*)(r27 + r24) = r3;
        if (tmp == 0) {
            tmp = 0x0;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_80085114;
        L_800850BC:
            r3 = 0x10c;
            ((void(*)(void))menuIsCheck)();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                ((void(*)(void))_threadSwitch)();
                goto L_80085114;
            }
            ((void(*)(void))windowGetKeyInfo)();
            tmp = *(u16*)((u8*)r3 + 0x4);
            tmp = tmp & 0x00000020;
            if ((s32)tmp != 0) {
                tmp = 0x2;
                r3 = 0xe;
                *(u32*)((u8*)r24 + 0x28) = tmp;
                goto L_8008513C;
            }
            tmp = *(u32*)((u8*)r24 + 0x28);
            if (tmp == 8) {
                r3 = 0xe;
                goto L_8008513C;
            }
            ((void(*)(void))_threadSwitch)();
        L_80085114:
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_800934E4)();
            if ((s32)r3 == 0) goto L_800850BC;
            tmp = 0x0;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093610)();
        L_8008513C:
            if ((s32)r3 == 0xe) {
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r15 = 0x0;
            L_80085150:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))menuGetEnablePort)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))menuSetEnablePort)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_80085150;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_800852AC;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                msgctrlSetValue();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_800852AC:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
            }
            r15 = 0x1;
        }
        r3 = r28;
        r4 = 0x0;
        ((void(*)(void))fn_80093160)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_8008538C;
    L_80085334:
        r3 = 0x10c;
        ((void(*)(void))menuIsCheck)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_8008538C;
        }
        ((void(*)(void))windowGetKeyInfo)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r3 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_800853B4;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r3 = 0xe;
            goto L_800853B4;
        }
        ((void(*)(void))_threadSwitch)();
    L_8008538C:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_80085334;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
    L_800853B4:
        if ((s32)r3 != 0xe) {
        do {
            if ((s32)r3 < 0xe) {
                if ((s32)r3 != 2) {
                    break;
                }
                r4 = 0x20000;
                tmp = r4 + 0x2;
                if ((s32)r3 == (s32)tmp) break;
                break;
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r15 = 0x0;
        L_800853EC:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800853EC;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085548;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085548:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        } while (0);
            r20 = 0x0;
        L_800855B8:
            r19 = 0x0;
            r17 = r24;
            r18 = r19;
            r16 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r17 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r18 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r16 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r17 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r17 + 0x0);
                if ((s32)tmp == 7) {
                    r19 = 0x1;
                }
                r17 = r17 + 0x4;
                r16 = r16 + 0x1;
                r18 = r18 + 0x1;
            } while ((s32)r18 <= 3);
            tmp = r19 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_800855B8;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085714;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085714:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x3c47;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
                }
        tmp = 0x4;
        r16 = 0x0;
        *(u32*)(r27 + r24) = tmp;
        do {
            r3 = r26;
            ((void(*)(void))fn_8008ABA0)();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                ((void(*)(void))menuGetEnablePort)();
                r4 = (u32)&lbl_80478950;
                tmp = *(u8*)(r4 + r28);
                tmp = tmp | r3;
                r3 = tmp & 0xFF;
                ((void(*)(void))menuSetEnablePort)();
                break;
            }
            ((void(*)(void))_threadSwitch)();
            r16 = r16 + 0x1;
        } while ((s32)r16 < 0x12c);

        if (tmp == 0) {
            r4 = r26;
            r3 = 0x2f;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x3c4d;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            f27 = *(f32*)&lbl_8047C1A8;
            f28 = *(f64*)&lbl_8047C1B0;
            r16 = 0x43300000;
            f30 = *(f64*)&lbl_8047C1B8;
            f31 = *(f32*)&lbl_8047C1AC;
            while (f27 < f31) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0xC1C) = tmp;
                f29 = f0 - f28;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;

            }
        }
        r3 = r22;
        heroInit();
        tmp = 0x0;
        r3 = r28;
        *(u32*)(sp + 0x18) = tmp;
        r4 = r22;
        r5 = (u32)sp + 0x18;
        ((void(*)(void))fn_80092FC8)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_80085934;
    L_800858DC:
        r3 = 0x10c;
        ((void(*)(void))menuIsCheck)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_80085934;
        }
        ((void(*)(void))windowGetKeyInfo)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r16 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_80085960;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r16 = 0xe;
            goto L_80085960;
        }
        ((void(*)(void))_threadSwitch)();
    L_80085934:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_800858DC;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
        r16 = r3;
    L_80085960:
        if ((s32)r16 != 0xe) {
            goto L_80085B40;
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r15 = 0x0;
    L_80085978:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))menuGetEnablePort)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))menuSetEnablePort)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_80085978;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80085AD4;
            }
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        msgctrlSetValue();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80085AD4:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_80085B40:
        /* extrwi tmp, r3, 2, 22 */;
        if (tmp != 0) {
            r20 = 0x0;
        L_80085B54:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80085B54;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085CB0;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085CB0:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x3c49;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        /* extrwi tmp, r3, 4, 24 */;
        if (tmp <= 7) {
            r3 = (u32)jumptable_802EEB78;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEB78;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            r17 = 0x1;


        } else {
        tmp = 0x0;
        goto L_80085E1C;
        }
        fn_80128E24();
        if (r3 != 0) {
            fn_80128E04();
            if (r3 != 0) {
                gamedataBiosGetGamedataAtttestPtr();
                if (r3 != 0) {
                    gamedataAttestBiosGetLangareaId();
                    r3 = r3 & 0xFF;
                    tmp = r17 & 0xFF;
                    if (r3 == tmp) {
                        tmp = 0x1;
                        goto L_80085E1C;
            }
            }
            }
        }
        tmp = 0x0;
    L_80085E1C:
        tmp = tmp & 0xFF;
        if (tmp == 0) {
            r20 = 0x0;
        L_80085E2C:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80085E2C;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80085F88;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80085F88:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x44f0;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        if ((s32)r16 != 4) {

        } else {
        tmp = r3 & 0x00000002;
        if (tmp != 0) goto L_8008629C;
        }
        r20 = 0x0;
    L_80086074:
        r18 = 0x0;
        r16 = r24;
        r19 = r18;
        r17 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r19 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))menuGetEnablePort)();
                    tmp = *(u8*)((u8*)r17 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))menuSetEnablePort)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r16 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp == 7) {
                r18 = 0x1;
            }
            r16 = r16 + 0x4;
            r17 = r17 + 0x1;
            r19 = r19 + 0x1;
        } while ((s32)r19 <= 3);
        tmp = r18 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r20 = r20 + 0x1;
            if ((s32)r20 < 0xf) goto L_80086074;
        }
        r3 = 0x26;
        fn_80166A28();
        r16 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r16 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r16 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r16 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r16 = 0x4;
        }
        }
        }
        }
        if ((s32)r16 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_800861D0;
            }
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r16 + 0x1;
        r3 = 0x2f;
        msgctrlSetValue();
        if ((s32)r16 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_800861D0:
    do {
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            tmp = 0x0;
            break;
        }
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = 0x6;
        r3 = 0x2f;
        tmp = (s8)tmp;
        tmp = tmp << 2;
        *(u32*)(r24 + tmp) = r4;
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = (s8)tmp;
        r4 = r4 + 0x1;
        msgctrlSetValue();
        r3 = 0x7;
        r4 = 0x3c49;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))winMsgOpen)();
        tmp = *(u32*)((u8*)r24 + 0x24);
        tmp = tmp & 0x00000008;
        if (tmp == 0) {
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();

        } else {
        r3 = r24;
        r4 = 0x7;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = 0x1;
            break;
        }
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        tmp = 0x0;
    } while (0);
        tmp = tmp & 0xFF;
        if (tmp != 0) goto L_80084E44;
        r3 = 0x0;
        return;
    L_8008629C:
        if (tmp != 0) {
            tmp = r3 & 0x00000004;
            r4 = 0x1;
            if (tmp != 0) {
                tmp = r3 & 0x00000008;
                if (tmp == 0) {
                    r4 = 0x0;
                }

            } else {
            tmp = r3 & 0x1;
            if (tmp == 0) {
                r4 = 0x0;
        }
            }
            tmp = r4 & 0xFF;
        }
        if (tmp == 0) {
            r20 = 0x0;
        L_800862EC:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_800862EC;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80086448;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80086448:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x44c3;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x0;
            return;
        }
        do {
            if (tmp == 0) break;
            tmp = r3 & 0x1;
            if (tmp == 0) {
                r20 = 0x0;
            L_80086530:
                r18 = 0x0;
                r16 = r24;
                r19 = r18;
                r17 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r16 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r19 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))menuGetEnablePort)();
                            tmp = *(u8*)((u8*)r17 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))menuSetEnablePort)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r16 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r16 + 0x0);
                    if ((s32)tmp == 7) {
                        r18 = 0x1;
                    }
                    r16 = r16 + 0x4;
                    r17 = r17 + 0x1;
                    r19 = r19 + 0x1;
                } while ((s32)r19 <= 3);
                tmp = r18 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r20 = r20 + 0x1;
                    if ((s32)r20 < 0xf) goto L_80086530;
                }
                r3 = 0x26;
                fn_80166A28();
                r16 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r16 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r16 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r16 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r16 = 0x4;
                }
                }
                }
                }
                if ((s32)r16 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_8008668C;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r4 = r16 + 0x1;
                r3 = 0x2f;
                msgctrlSetValue();
                if ((s32)r16 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_8008668C:
            do {
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    tmp = *(u32*)((u8*)r24 + 0x24);
                    tmp = tmp & 0x00000008;
                    if (tmp != 0) {
                        tmp = 0x1;
                        break;
                    }
                    tmp = 0x0;
                    break;
                }
                tmp = *(u8*)((u8*)r24 + 0x21);
                r4 = 0x6;
                r3 = 0x2f;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r4;
                tmp = *(u8*)((u8*)r24 + 0x21);
                r4 = (s8)tmp;
                r4 = r4 + 0x1;
                msgctrlSetValue();
                r3 = 0x7;
                r4 = 0x44c3;
                r5 = 0x0;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = r24;
                    r4 = 0x1;
                    fn_80087AE8();

                } else {
                r3 = r24;
                r4 = 0x7;
                fn_80087AE8();
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                tmp = 0x0;
            } while (0);
                tmp = tmp & 0xFF;
                if (tmp != 0) goto L_80084E44;
                r3 = 0x0;
                return;
            }
            r3 = r22;
            ((void(*)(void))fn_800776E4)();
            tmp = r3 & 0xFF;
            if (tmp != 0) break;
            r3 = 0xe4;
            r4 = 0x0;
            r5 = 0x1;
            ((void(*)(void))menuCloseCustom)();
            r3 = r22;
            r4 = 0x1;
            ((void(*)(void))fn_8005CF2C)();
            if (tmp == 0) {
                r3 = 0x0;
                return;
            }
            r3 = 0xe4;
            r4 = 0x0;
            ((void(*)(void))menuOpen)();
            r3 = 0xe4;
            ((void(*)(void))windowSearchID)();
            r16 = r3;
            if (r16 == 0) {
                r4 = 0x1f4;
                r5 = (u32)&lbl_8047C1A0;
                r3 = r3 + 0x184;
                __assert();
            }
            if (r16 == 0) {
                r3 = 0xa6;
                ((void(*)(void))windowSearchID)();
                r16 = r3;
            }
            r3 = r16;
            ((void(*)(void))windowGetFreeWork)();
            r5 = *(u32*)((u8*)r3 + 0x0);
            r4 = 0x0;
            *(u32*)((u8*)r5 + 0x24) = tmp;
            *(u32*)((u8*)r5 + 0x2C) = tmp;
            tmp = *(u8*)((u8*)r3 + 0x0);
            *(u8*)((u8*)r5 + 0x21) = tmp;
            tmp = 0x5;
            r3 = *(u8*)((u8*)r30 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r30 + 0x1;
            r4 = 0x1;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r6 + 0x1;
            r4 = 0x2;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            r6 = r6 + 0x1;
            r4 = 0x3;
            r3 = *(u8*)((u8*)r6 + 0x0);
            r3 = (s8)r3;
            r4 = r3 << 2;
            r3 = *(u32*)(r29 + r4);
            *(u32*)(r5 + r4) = r3;
            if ((s32)r4 < (s32)r25) {
                r3 = *(u32*)(r5 + r4);
                if ((s32)r3 == 1) {
                    *(u32*)(r5 + r4) = tmp;
            }
            }
            tmp = 0x6;
            r24 = r5;
            *(u32*)(r27 + r5) = tmp;
            goto L_80084E44;
        } while (0);
        if (tmp != 0) {
            tmp = (s8)r23;
            if ((s32)tmp != 1) {
            }
            if (r31 == 0) goto L_80086DBC;
            }
        r3 = r28;
        r4 = r21;
        ((void(*)(void))fn_80092E38)();
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        goto L_8008697C;
    L_80086924:
        r3 = 0x10c;
        ((void(*)(void))menuIsCheck)();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            ((void(*)(void))_threadSwitch)();
            goto L_8008697C;
        }
        ((void(*)(void))windowGetKeyInfo)();
        tmp = *(u16*)((u8*)r3 + 0x4);
        tmp = tmp & 0x00000020;
        if ((s32)tmp != 0) {
            tmp = 0x2;
            r3 = 0xe;
            *(u32*)((u8*)r24 + 0x28) = tmp;
            goto L_800869A4;
        }
        tmp = *(u32*)((u8*)r24 + 0x28);
        if (tmp == 8) {
            r3 = 0xe;
            goto L_800869A4;
        }
        ((void(*)(void))_threadSwitch)();
    L_8008697C:
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_800934E4)();
        if ((s32)r3 == 0) goto L_80086924;
        tmp = 0x0;
        *(u32*)((u8*)r24 + 0x28) = tmp;
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093610)();
    L_800869A4:
        if ((s32)r3 != 0xe) {
            if ((s32)r3 < 0xe) {
                if ((s32)r3 == 0xb) goto L_80086DBC;
            }
            goto L_80086B90;
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r15 = 0x0;
    L_800869C8:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))menuGetEnablePort)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))menuSetEnablePort)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_800869C8;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80086B24;
            }
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        msgctrlSetValue();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80086B24:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_80086B90:
        r20 = 0x0;
    L_80086B94:
        r18 = 0x0;
        r16 = r24;
        r19 = r18;
        r17 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r19 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))menuGetEnablePort)();
                    tmp = *(u8*)((u8*)r17 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))menuSetEnablePort)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r16 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r16 + 0x0);
            if ((s32)tmp == 7) {
                r18 = 0x1;
            }
            r16 = r16 + 0x4;
            r17 = r17 + 0x1;
            r19 = r19 + 0x1;
        } while ((s32)r19 <= 3);
        tmp = r18 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r20 = r20 + 0x1;
            if ((s32)r20 < 0xf) goto L_80086B94;
        }
        r3 = 0x26;
        fn_80166A28();
        r16 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r16 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r16 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r16 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r16 = 0x4;
        }
        }
        }
        }
        if ((s32)r16 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80086CF0;
            }
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r16 + 0x1;
        r3 = 0x2f;
        msgctrlSetValue();
        if ((s32)r16 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80086CF0:
    do {
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            tmp = 0x0;
            break;
        }
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = 0x6;
        r3 = 0x2f;
        tmp = (s8)tmp;
        tmp = tmp << 2;
        *(u32*)(r24 + tmp) = r4;
        tmp = *(u8*)((u8*)r24 + 0x21);
        r4 = (s8)tmp;
        r4 = r4 + 0x1;
        msgctrlSetValue();
        r3 = 0x7;
        r4 = 0x3c47;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))winMsgOpen)();
        tmp = *(u32*)((u8*)r24 + 0x24);
        tmp = tmp & 0x00000008;
        if (tmp == 0) {
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();

        } else {
        r3 = r24;
        r4 = 0x7;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = 0x1;
            break;
        }
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        tmp = 0x0;
    } while (0);
        tmp = tmp & 0xFF;
        if (tmp != 0) goto L_80084E44;
        r3 = 0x0;
        return;
    L_80086DBC:
        if (r31 != 0) {
            tmp = *(u32*)((u8*)r21 + 0x8);
            tmp = tmp & 0x00000010;
        }
        if (tmp == 0) {
            r20 = 0x0;
        L_80086DD8:
            r18 = 0x0;
            r16 = r24;
            r19 = r18;
            r17 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r19 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r17 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r16 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r16 + 0x0);
                if ((s32)tmp == 7) {
                    r18 = 0x1;
                }
                r16 = r16 + 0x4;
                r17 = r17 + 0x1;
                r19 = r19 + 0x1;
            } while ((s32)r19 <= 3);
            tmp = r18 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r20 = r20 + 0x1;
                if ((s32)r20 < 0xf) goto L_80086DD8;
            }
            r3 = 0x26;
            fn_80166A28();
            r16 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r16 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r16 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r16 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r16 = 0x4;
            }
            }
            }
            }
            if ((s32)r16 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80086F34;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r16 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r16 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80086F34:
        do {
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = *(u32*)((u8*)r24 + 0x24);
                tmp = tmp & 0x00000008;
                if (tmp != 0) {
                    tmp = 0x1;
                    break;
                }
                tmp = 0x0;
                break;
            }
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = 0x6;
            r3 = 0x2f;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r4;
            tmp = *(u8*)((u8*)r24 + 0x21);
            r4 = (s8)tmp;
            r4 = r4 + 0x1;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x4417;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            tmp = *(u32*)((u8*)r24 + 0x24);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = r24;
                r4 = 0x1;
                fn_80087AE8();

            } else {
            r3 = r24;
            r4 = 0x7;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            tmp = 0x0;
        } while (0);
            tmp = tmp & 0xFF;
            if (tmp != 0) goto L_80084E44;
            r3 = 0x2f;
            r4 = 0x0;
            msgctrlSetValue();
            r3 = 0x7;
            r4 = 0x44cf;
            r5 = 0x0;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
            r3 = r24;
            r4 = 0x1;
            fn_80087AE8();
            r3 = 0x0;
            return;
        }
    do {
        r3 = r28;
        ((void(*)(void))fn_80093698)();
        r3 = 0x3cc;
        fn_80166A28();
        tmp = 0x5;
        *(u32*)(r24 + r27) = tmp;
        ((void(*)(void))menuGetEnablePort)();
        r4 = (u32)&lbl_80478950;
        tmp = *(u8*)(r4 + r28);
        tmp = tmp | r3;
        r3 = tmp & 0xFF;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r26;
        r3 = 0x2f;
        msgctrlSetValue();
        r3 = 0x7;
        r4 = 0x3c4b;
        r5 = 0x0;
        r6 = 0x0;
        ((void(*)(void))winMsgOpen)();
        if ((s32)tmp != 2) {
            if ((s32)tmp < 2) {
                if ((s32)tmp != 0) {
                    if ((s32)tmp < 0) {
                        break;
                    }
                    if ((s32)tmp >= 4) break;
                    goto L_8008769C;
                    }
                r3 = r24;
                r4 = 0x3;
                fn_80087AE8();
                tmp = r3 & 0xFF;
                if (tmp != 0) {
                    r3 = 0x1;
                    return;
                }
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r15 = 0x0;
            L_800870D4:
                r16 = 0x0;
                r18 = r24;
                r17 = r16;
                r19 = (u32)&lbl_80478950;
                do {
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp != 5) {
                        if ((s32)tmp == 4) {
                        }
                        r3 = r17 + 0x1;
                        ((void(*)(void))fn_8008ABA0)();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            ((void(*)(void))menuGetEnablePort)();
                            tmp = *(u8*)((u8*)r19 + 0x0);
                            tmp = r3 & ~tmp;
                            r3 = tmp & 0xFF;
                            ((void(*)(void))menuSetEnablePort)();
                            r3 = 0x7;
                            tmp = 0x8;
                            *(u32*)((u8*)r18 + 0x0) = r3;
                            *(u32*)((u8*)r24 + 0x28) = tmp;
                        }
                        }
                    tmp = *(u32*)((u8*)r18 + 0x0);
                    if ((s32)tmp == 7) {
                        r16 = 0x1;
                    }
                    r18 = r18 + 0x4;
                    r19 = r19 + 0x1;
                    r17 = r17 + 0x1;
                } while ((s32)r17 <= 3);
                tmp = r16 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))_threadSwitch)();
                    r15 = r15 + 0x1;
                    if ((s32)r15 < 0xf) goto L_800870D4;
                }
                r3 = 0x26;
                fn_80166A28();
                r15 = 0x0;
                tmp = *(u32*)((u8*)r24 + 0x0);
                if ((s32)tmp != 7) {
                    r15 = 0x1;
                    tmp = *(u32*)((u8*)r24 + 0x4);
                    if ((s32)tmp != 7) {
                        r15 = 0x2;
                        tmp = *(u32*)((u8*)r24 + 0x8);
                        if ((s32)tmp != 7) {
                            r15 = 0x3;
                            tmp = *(u32*)((u8*)r24 + 0xC);
                            if ((s32)tmp != 7) {
                                r15 = 0x4;
                }
                }
                }
                }
                if ((s32)r15 > 3) {
                    tmp = *(u32*)((u8*)r24 + 0x28);
                    tmp = tmp & 0x00000008;
                    if (tmp == 0) {
                        r3 = 0x0;
                        goto L_80087230;
                    }
                }
                r3 = 0x1;
                ((void(*)(void))menuSetEnablePort)();
                r4 = r15 + 0x1;
                r3 = 0x2f;
                msgctrlSetValue();
                if ((s32)r15 == 0) {
                    r3 = 0x7;
                    r4 = 0x44c0;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44b8;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                tmp = 0x8;
                r3 = 0x1;
                *(u32*)((u8*)r24 + 0x28) = tmp;
            L_80087230:
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    tmp = *(u8*)((u8*)r24 + 0x21);
                    r3 = 0x6;
                    tmp = (s8)tmp;
                    tmp = tmp << 2;
                    *(u32*)(r24 + tmp) = r3;
                }
                tmp = *(u32*)((u8*)r24 + 0x2C);
                if ((s32)tmp == 3) {
                    r3 = 0x7;
                    r4 = 0x44e7;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                } else {

                    r3 = 0x7;
                    r4 = 0x44e6;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))winMsgOpen)();
                }
                r3 = *(u8*)((u8*)r24 + 0x21);
                r3 = (s8)r3;
                ((void(*)(void))fn_80093698)();
                r3 = 0x0;
                return;
                    }
            tmp = (s8)r23;
            if ((s32)tmp != 2) break;
            r3 = r24;
            r4 = 0x3;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = 0x1;
                return;
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r15 = 0x0;
        L_800872D4:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800872D4;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087430;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087430:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
        tmp = (s8)r23;
        if ((s32)tmp != 3) break;
        r3 = r24;
        r4 = 0x3;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0x1;
            return;
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r15 = 0x0;
    L_800874D4:
        r16 = 0x0;
        r18 = r24;
        r17 = r16;
        r19 = (u32)&lbl_80478950;
        do {
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp != 5) {
                if ((s32)tmp == 4) {
                }
                r3 = r17 + 0x1;
                ((void(*)(void))fn_8008ABA0)();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    ((void(*)(void))menuGetEnablePort)();
                    tmp = *(u8*)((u8*)r19 + 0x0);
                    tmp = r3 & ~tmp;
                    r3 = tmp & 0xFF;
                    ((void(*)(void))menuSetEnablePort)();
                    r3 = 0x7;
                    tmp = 0x8;
                    *(u32*)((u8*)r18 + 0x0) = r3;
                    *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                }
            tmp = *(u32*)((u8*)r18 + 0x0);
            if ((s32)tmp == 7) {
                r16 = 0x1;
            }
            r18 = r18 + 0x4;
            r19 = r19 + 0x1;
            r17 = r17 + 0x1;
        } while ((s32)r17 <= 3);
        tmp = r16 & 0xFF;
        if (tmp == 0) {
            ((void(*)(void))_threadSwitch)();
            r15 = r15 + 0x1;
            if ((s32)r15 < 0xf) goto L_800874D4;
        }
        r3 = 0x26;
        fn_80166A28();
        r15 = 0x0;
        tmp = *(u32*)((u8*)r24 + 0x0);
        if ((s32)tmp != 7) {
            r15 = 0x1;
            tmp = *(u32*)((u8*)r24 + 0x4);
            if ((s32)tmp != 7) {
                r15 = 0x2;
                tmp = *(u32*)((u8*)r24 + 0x8);
                if ((s32)tmp != 7) {
                    r15 = 0x3;
                    tmp = *(u32*)((u8*)r24 + 0xC);
                    if ((s32)tmp != 7) {
                        r15 = 0x4;
        }
        }
        }
        }
        if ((s32)r15 > 3) {
            tmp = *(u32*)((u8*)r24 + 0x28);
            tmp = tmp & 0x00000008;
            if (tmp == 0) {
                r3 = 0x0;
                goto L_80087630;
            }
        }
        r3 = 0x1;
        ((void(*)(void))menuSetEnablePort)();
        r4 = r15 + 0x1;
        r3 = 0x2f;
        msgctrlSetValue();
        if ((s32)r15 == 0) {
            r3 = 0x7;
            r4 = 0x44c0;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44b8;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        tmp = 0x8;
        r3 = 0x1;
        *(u32*)((u8*)r24 + 0x28) = tmp;
    L_80087630:
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            tmp = *(u8*)((u8*)r24 + 0x21);
            r3 = 0x6;
            tmp = (s8)tmp;
            tmp = tmp << 2;
            *(u32*)(r24 + tmp) = r3;
        }
        tmp = *(u32*)((u8*)r24 + 0x2C);
        if ((s32)tmp == 3) {
            r3 = 0x7;
            r4 = 0x44e7;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        } else {

            r3 = 0x7;
            r4 = 0x44e6;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))winMsgOpen)();
        }
        r3 = *(u8*)((u8*)r24 + 0x21);
        r3 = (s8)r3;
        ((void(*)(void))fn_80093698)();
        r3 = 0x0;
        return;
    L_8008769C:
        tmp = (s8)r23;
        if ((s32)tmp == 0) {
            r3 = r24;
            r4 = 0x3;
            fn_80087AE8();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = 0x1;
                return;
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r15 = 0x0;
        L_800876D4:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800876D4;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087830;
            }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087830:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
    } while (0);
        r4 = 0x7;
        tmp = *(u8*)((u8*)r3 + 0x1);
        r3 = r24;
        *(u8*)((u8*)r24 + 0x21) = tmp;
        fn_80087AE8();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r15 = 0x0;
        L_800878CC:
            r16 = 0x0;
            r18 = r24;
            r17 = r16;
            r19 = (u32)&lbl_80478950;
            do {
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp != 5) {
                    if ((s32)tmp == 4) {
                    }
                    r3 = r17 + 0x1;
                    ((void(*)(void))fn_8008ABA0)();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        ((void(*)(void))menuGetEnablePort)();
                        tmp = *(u8*)((u8*)r19 + 0x0);
                        tmp = r3 & ~tmp;
                        r3 = tmp & 0xFF;
                        ((void(*)(void))menuSetEnablePort)();
                        r3 = 0x7;
                        tmp = 0x8;
                        *(u32*)((u8*)r18 + 0x0) = r3;
                        *(u32*)((u8*)r24 + 0x28) = tmp;
                    }
                    }
                tmp = *(u32*)((u8*)r18 + 0x0);
                if ((s32)tmp == 7) {
                    r16 = 0x1;
                }
                r18 = r18 + 0x4;
                r19 = r19 + 0x1;
                r17 = r17 + 0x1;
            } while ((s32)r17 <= 3);
            tmp = r16 & 0xFF;
            if (tmp == 0) {
                ((void(*)(void))_threadSwitch)();
                r15 = r15 + 0x1;
                if ((s32)r15 < 0xf) goto L_800878CC;
            }
            r3 = 0x26;
            fn_80166A28();
            r15 = 0x0;
            tmp = *(u32*)((u8*)r24 + 0x0);
            if ((s32)tmp != 7) {
                r15 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x4);
                if ((s32)tmp != 7) {
                    r15 = 0x2;
                    tmp = *(u32*)((u8*)r24 + 0x8);
                    if ((s32)tmp != 7) {
                        r15 = 0x3;
                        tmp = *(u32*)((u8*)r24 + 0xC);
                        if ((s32)tmp != 7) {
                            r15 = 0x4;
            }
            }
            }
            }
            if ((s32)r15 > 3) {
                tmp = *(u32*)((u8*)r24 + 0x28);
                tmp = tmp & 0x00000008;
                if (tmp == 0) {
                    r3 = 0x0;
                    goto L_80087A28;
                }
            }
            r3 = 0x1;
            ((void(*)(void))menuSetEnablePort)();
            r4 = r15 + 0x1;
            r3 = 0x2f;
            msgctrlSetValue();
            if ((s32)r15 == 0) {
                r3 = 0x7;
                r4 = 0x44c0;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44b8;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            tmp = 0x8;
            r3 = 0x1;
            *(u32*)((u8*)r24 + 0x28) = tmp;
        L_80087A28:
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                tmp = *(u8*)((u8*)r24 + 0x21);
                r3 = 0x6;
                tmp = (s8)tmp;
                tmp = tmp << 2;
                *(u32*)(r24 + tmp) = r3;
            }
            tmp = *(u32*)((u8*)r24 + 0x2C);
            if ((s32)tmp == 3) {
                r3 = 0x7;
                r4 = 0x44e7;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            } else {

                r3 = 0x7;
                r4 = 0x44e6;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))winMsgOpen)();
            }
            r3 = *(u8*)((u8*)r24 + 0x21);
            r3 = (s8)r3;
            ((void(*)(void))fn_80093698)();
            r3 = 0x0;
            return;
        }
        r25 = r25 + 0x1;
        r4 = r4 + 0x1;
    } while (r25 < 4);

    return;
}

#define CARDE_GRID_TABLE ((CardEGridTable*)lbl_8026F488)
#define CARDE_SHOW_MODEL(model_id, anim_id)                                      \
    do {                                                                         \
        void* model_;                                                            \
        model_ = fn_800F92D4((model_id));                                        \
        if (model_ != 0) {                                                       \
            fn_800ECCA8(model_, (anim_id));                                      \
            fn_800ECA78(model_, lbl_8047C1CC);                                   \
            fn_800EC9DC(model_, lbl_8047C1C8);                                   \
            fn_800EC990(model_);                                                 \
        }                                                                        \
    } while (0)

/* Run the card-e three-tile grid prompt.  A zero return means that three
 * cells were accepted; one means the player backed out before completing it. */
typedef struct CardGridKeyInfo {
    u8 _00[4];
    u16 buttons;
    u16 repeat;
} CardGridKeyInfo;

typedef struct CardGridCell {
    u32 modelId;
    s16 selectAnim;
    s16 resetAnim;
} CardGridCell;

typedef struct CardGridData {
    u16 tileId[3][3];
    u16 _12;
    CardGridCell cell[3][3];
    s16 cursorAnim[3][3];
    u16 _6E;
} CardGridData;

typedef struct CardGridChoice {
    s32 row;
    s32 column;
} CardGridChoice;

extern const u32 lbl_8047C1C0;
extern const u32 lbl_8047C1C4;
extern CardGridKeyInfo* windowGetPortKeyInfo(u32 port);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelStartAnimation(void* model);
extern void GSmodelSetAnimType(void* model, u32 type);
extern u8 GSmodelIsAnimating(void* model);

u32 fn_80087C64(const u16* expected)
{
    u32 occupied[3][3];
    CardGridChoice choices[3];
    const CardGridData* data = (const CardGridData*)lbl_8026F488;
    s32 row = 1;
    s32 column = 1;
    s32 count = 0;
    u32 i;

    windowGetPortKeyInfo(1);
    for (i = 0; i < 9; i++) {
        ((u32*)occupied)[i] = 0;
    }

    while (count < 3) {
        CardGridKeyInfo* key = windowGetPortKeyInfo(1);
        u32 handled = 0;

        if (key->repeat & 0x10) {
            u32 accepted = 0;
            if (row >= 0 && row < 3 && column >= 0 && column < 3 &&
                occupied[column][row] == 0) {
                CardGridCell cells[3][3];
                void* model;
                memcpy(cells, data->cell, sizeof(cells));
                model = fn_800F92D4(cells[column][row].modelId);
                if (model != 0) {
                    if (model != 0) {
                        GSmodelSetAnimIndex(model, cells[column][row].selectAnim);
                        GSmodelSetAnimFrame(model, lbl_8047C1CC);
                        GSmodelSetAnimRate(model, lbl_8047C1C8);
                        GSmodelStartAnimation(model);
                    }
                }
                occupied[column][row] = 1;
                accepted = 1;
            }

            if (accepted) {
                s16 counterAnim[4];
                void* model;
                choices[count].row = row;
                choices[count].column = column;
                count++;
                ((u32*)counterAnim)[0] = lbl_8047C1C0;
                ((u32*)counterAnim)[1] = lbl_8047C1C4;
                if (count >= 0 && count < 4) {
                    model = fn_800F92D4(0x107E100B);
                    if (model != 0) {
                        if (model != 0) {
                            GSmodelSetAnimIndex(model, counterAnim[count]);
                            GSmodelSetAnimFrame(model, lbl_8047C1CC);
                            GSmodelSetAnimRate(model, lbl_8047C1C8);
                            GSmodelStartAnimation(model);
                        }
                    }
                }

                if (count >= 3) {
                    u32 matches = 0;
                    s32 gridRow;
                    u16 tiles[3][3];
                    memcpy(tiles, data->tileId, sizeof(tiles));
                    for (gridRow = 0; gridRow < 3; gridRow++) {
                        s32 gridColumn;
                        for (gridColumn = 0; gridColumn < 3; gridColumn++) {
                            if (occupied[gridRow][gridColumn] != 0) {
                                u16 tile = tiles[gridRow][gridColumn];
                                if (tile == expected[0] || tile == expected[1] ||
                                    tile == expected[2]) {
                                    matches++;
                                }
                            }
                        }
                    }

                    if (matches < 3) {
                        s32 clearRow;
                        fn_80166A28(0x26);
                        while (fn_801666BC(0x26) == 2) {
                            _threadSwitch();
                        }
                        for (clearRow = 0; clearRow < 3; clearRow++) {
                            s32 clearColumn;
                            for (clearColumn = 0; clearColumn < 3; clearColumn++) {
                                if (occupied[clearRow][clearColumn] != 0) {
                                    CardGridCell cells[3][3];
                                    void* clearModel;
                                    memcpy(cells, data->cell, sizeof(cells));
                                    clearModel = fn_800F92D4(cells[clearColumn][clearRow].modelId);
                                    if (clearModel != 0) {
                                        if (clearModel != 0) {
                                            GSmodelSetAnimIndex(clearModel,
                                                cells[clearColumn][clearRow].resetAnim);
                                            GSmodelSetAnimFrame(clearModel, lbl_8047C1CC);
                                            GSmodelSetAnimRate(clearModel, lbl_8047C1C8);
                                            GSmodelStartAnimation(clearModel);
                                        }
                                    }
                                    occupied[clearRow][clearColumn] = 0;
                                }
                            }
                        }
                        {
                            s16 counterAnim[4];
                            void* counterModel;
                            ((u32*)counterAnim)[0] = lbl_8047C1C0;
                            ((u32*)counterAnim)[1] = lbl_8047C1C4;
                            count = 0;
                            counterModel = fn_800F92D4(0x107E100B);
                            if (counterModel != 0) {
                                if (counterModel != 0) {
                                    GSmodelSetAnimIndex(counterModel, counterAnim[0]);
                                    GSmodelSetAnimFrame(counterModel, lbl_8047C1CC);
                                    GSmodelSetAnimRate(counterModel, lbl_8047C1C8);
                                    GSmodelStartAnimation(counterModel);
                                }
                            }
                        }
                    } else {
                        fn_80166A28(0x4A1);
                        while (fn_801666BC(0x4A1) == 2) {
                            _threadSwitch();
                        }
                    }
                } else {
                    fn_80166A28(0x3C6);
                }
                handled = 1;
            }
        }

        if ((key->buttons & 0x20) && !handled) {
            CardGridCell cells[3][3];
            s16 counterAnim[4];
            CardGridChoice* choice;
            void* model;
            count--;
            if (count < 0) {
                break;
            }

            fn_80166A28(0x3C7);
            choice = &choices[count];
            memcpy(cells, data->cell, sizeof(cells));
            if (choice->row >= 0 && choice->row < 3 &&
                choice->column >= 0 && choice->column < 3) {
                model = fn_800F92D4(cells[choice->column][choice->row].modelId);
                if (model != 0) {
                    if (model != 0) {
                        GSmodelSetAnimIndex(model,
                            cells[choice->column][choice->row].resetAnim);
                        GSmodelSetAnimFrame(model, lbl_8047C1CC);
                        GSmodelSetAnimRate(model, lbl_8047C1C8);
                        GSmodelStartAnimation(model);
                    }
                }
            }

            ((u32*)counterAnim)[0] = lbl_8047C1C0;
            ((u32*)counterAnim)[1] = lbl_8047C1C4;
            if (count >= 0 && count < 4) {
                model = fn_800F92D4(0x107E100B);
                if (model != 0) {
                    if (model != 0) {
                        GSmodelSetAnimIndex(model, counterAnim[count]);
                        GSmodelSetAnimFrame(model, lbl_8047C1CC);
                        GSmodelSetAnimRate(model, lbl_8047C1C8);
                        GSmodelStartAnimation(model);
                    }
                }
            }
            occupied[choice->column][choice->row] = 0;
            handled = 1;
        }

        if (!handled) {
            s32 nextRow = row;
            s32 nextColumn = column;
            u32 moved = 0;
            if ((key->repeat & 1) && column > 0) {
                nextColumn = column - 1;
                moved = 1;
            }
            if ((key->repeat & 2) && nextColumn < 2) {
                nextColumn++;
                moved = 1;
            }
            if ((key->repeat & 4) && row > 0) {
                nextRow = row - 1;
                moved = 1;
            }
            if ((key->repeat & 8) && nextRow < 2) {
                nextRow++;
                moved = 1;
            }

            if (moved) {
                s16 cursorAnim[3][3];
                void* cursorModel;
                memcpy(cursorAnim, data->cursorAnim, sizeof(cursorAnim));
                if (nextRow >= 0 && nextRow < 3 &&
                    nextColumn >= 0 && nextColumn < 3) {
                    cursorModel = fn_800F92D4(0x107E1009);
                    if (cursorModel != 0) {
                        if (cursorModel != 0) {
                            GSmodelSetAnimIndex(cursorModel,
                                cursorAnim[nextColumn][nextRow]);
                            GSmodelSetAnimFrame(cursorModel, lbl_8047C1CC);
                            GSmodelSetAnimRate(cursorModel, lbl_8047C1C8);
                            GSmodelStartAnimation(cursorModel);
                        }
                        GSmodelSetAnimType(cursorModel, 0);
                        while (GSmodelIsAnimating(cursorModel) != 0) {
                            _threadSwitch();
                        }
                    }
                }
                row = nextRow;
                column = nextColumn;
            }
        }
        _threadSwitch();
    }

    return count < 0 ? 1 : 0;
}

#undef CARDE_SHOW_MODEL
#undef CARDE_GRID_TABLE

typedef struct CardEPageLayout {
    u8 field_00[0x10];
    u8 summary[0x66];
    u8 cells[1][0x10];
} CardEPageLayout;

static inline s32 cardEPageSize(const u8* card)
{
    return (s8)card[0x1C] * (s8)card[0x1D] * 0x10 + 0x76;
}

static inline u8* cardEGetPage(u8* card, s8 pageIndex)
{
    return card + 0x24 + pageIndex * cardEPageSize(card);
}

static inline u8* cardEGetCell(u8* card, s8 pageIndex, s8 row, s8 column)
{
    u8* page = cardEGetPage(card, pageIndex);
    s32 index = row * (s8)card[0x1D] + column;

    return page + 0x76 + index * 0x10;
}

u32 fn_80080ED8(u16* destination, const u8* source)
{
    const u8* cursor;
    u32 length = 0;

    if (source == NULL) {
        return 0;
    }

    if (destination == NULL) {
        for (cursor = source; *cursor != 0; length++) {
            if ((*cursor >= 0x81 && *cursor <= 0x9F) ||
                (*cursor >= 0xE0 && *cursor <= 0xFC)) {
                cursor += 2;
                length++;
            } else {
                cursor++;
            }
        }
        return length;
    }

    cursor = source;
    while (*cursor != 0) {
        u16 character = *cursor++;

        if (character >= 0xA1 && character <= 0xDF) {
            character = (u16)(character + 0xFEC0);
        }
        *destination++ = character;
        length++;
    }
    *destination = 0;
    return length;
}

u32 fn_80082738(u8* card, const u8* window, s8 pageIndex)
{
    u8* page;
    u8* cell;
    s32 count;
    s32 i;

    page = cardEGetPage(card, pageIndex);
    cell = page + 0x76 +
        ((s8)window[0x24] * (s8)card[0x1D] + (s8)window[0x26]) * 0x10;
    *(u16*)cell = 0;
    cell[0x0C] = 0;

    if (pageIndex != 0) {
        return 0;
    }

    page = cardEGetPage(card, 0);
    count = (s8)card[0x1C] * (s8)card[0x1D];
    for (i = 0; i < count; i++) {
        if (page[0x82 + i * 0x10] != 0) {
            return 0;
        }
    }
    return 1;
}

void fn_80082960(u8* card, const u8* window, s8 pageIndex)
{
    u8* page = cardEGetPage(card, pageIndex);
    u8* entry = page + 0x10 + (s8)window[0x24] * 0x0E;

    *(u16*)entry = 0;
    entry[0x0C] = 0;
}

u32 fn_80082A88(u8* card, s8 pageIndex)
{
    u8* page = cardEGetPage(card, pageIndex);
    s32 count = (s8)card[0x1C] * (s8)card[0x1D];
    s32 i;

    for (i = 0; i < count; i++) {
        if (page[0x82 + i * 0x10] != 0) {
            return 1;
        }
    }
    return 0;
}

u8* fn_80082BA4(u8* card, const u8* window, s8 pageIndex)
{
    extern void fn_800CAA3C(void*, const void*);
    u8* page = cardEGetPage(card, pageIndex);
    u8* entry = page + 0x10 + (s8)window[0x24] * 0x0E;
    const u8* descriptor =
        window + 0x3AC + (s8)window[0x5E + pageIndex] * 0x28;

    fn_800CAA3C(entry, descriptor);
    entry[0x0C] = 1;
    return page;
}

u8* fn_80082CF0(u8* card, const u8* window, s8 pageIndex)
{
    extern void fn_800CAA3C(void*, const void*);
    s32 pageSize;
    u8* page;
    s32 index;
    s8 row = (s8)window[0x24];
    s8 column = (s8)window[0x26];
    u8* cell;
    const u8* descriptor =
        window + 0x3AC + (s8)window[0x5B + pageIndex] * 0x28;

    pageSize = (s8)card[0x1C] * (s8)card[0x1D] * 0x10 + 0x76;
    page = card + 0x24 + pageIndex * pageSize;
    index = row * (s8)card[0x1D] + column;
    cell = page + 0x76 + index * 0x10;

    fn_800CAA3C(cell, descriptor);
    cell[0x0C] = 1;
    *(u16*)(cell + 0x0E) = *(const u16*)(descriptor + 0x22);
    card[0x1E + row] = window[0x25];
    return cell;
}

u8* fn_80082EA4(u8* card, s8 pageIndex, s8 row, s8 column)
{
    s32 pageSize =
        (s8)card[0x1C] * (s8)card[0x1D] * 0x10 + 0x76;
    u8* page = card + 0x24 + pageIndex * pageSize;
    s32 index = row * (s8)card[0x1D] + column;

    return page + 0x76 + index * 0x10;
}
