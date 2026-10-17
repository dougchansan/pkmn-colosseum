/**
 * @file menu_carde_matrix.c
 * @brief Card-E matrix display (0x8007C300-0x8007FD64)
 *
 * Address range: 0x8007C300 - 0x8007FD64
 * Total functions: 15
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void menuSpriteBiosGetPtr();
extern void cardeSaveLevelHasCard();
extern void cardeSaveGetCard();
extern void cardeSaveGetLevel();
extern void _threadSwitch();
extern void windowGetFreeWork();
extern void windowGetParam();
extern void windowGetActiveID();
extern void windowSearchID();
extern void windowGetKeyInfo();
extern void fn_801081F8();
extern void winSpriteGetDisp();
extern void winSpriteSetDisp();
extern void msgctrlSetValue();
extern void soundStartSE();
extern void __assert();
extern void* memset(void* dst, int val, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_8047A658;
extern const u32 lbl_8047C130[2];
extern const u32 lbl_8047C138[2];
extern u8 lbl_8047C140;
extern u8 lbl_8047C148;
extern u8 lbl_8047C14C;
extern u8 lbl_8047C150;
extern u8 lbl_8047C154;
extern u8 lbl_8047C158;
extern u8 lbl_8047C15C;
extern u8 lbl_8047C160;
extern u8 lbl_8047C168;
extern u8 lbl_8047C170;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EE868[];
/* lbl_80268B88: item-id tables for the three matrix item groups (each a
 * pair of 36-entry tables for the previous/current entry), then the two
 * sprite-id tables indexed by a row's layer byte. */
typedef struct MenuCardEMatrixTable {
    u16 ids[3][2][36];
    s32 sprites[2][8]; /* [0] cells, [1] row ends */
} MenuCardEMatrixTable;

#if defined(MENU_CARDE_MATRIX_TU)
/* The TU's own .rodata (0x80268B88): the item-id tables of the three
 * matrix item groups (a 36-entry row each for the previous and the
 * current entry), then the sprite ids for cells and row ends. Kept as
 * separate objects: retail addresses them off one pool base. */
static const u16 sCardEMatrixIds0[2][36] = {
    {
        0x07A0, 0x07A1, 0x07A2, 0x07A3, 0x102E, 0x102F, 0x1030, 0x1031, 0x1032, 0x1033, 0x1034, 0x1035,
        0x1036, 0x1037, 0x1038, 0x1039, 0x103A, 0x103B, 0x103C, 0x103D, 0x103E, 0x103F, 0x1040, 0x1041,
        0x1042, 0x1043, 0x1044, 0x1045, 0x1046, 0x1047, 0x1048, 0x1049, 0x104A, 0x104B, 0x104C, 0x104D,
    },
    {
        0x1127, 0x1128, 0x1129, 0x112A, 0x112B, 0x112C, 0x112D, 0x112E, 0x112F, 0x1130, 0x1131, 0x1132,
        0x1133, 0x1134, 0x1135, 0x1136, 0x1137, 0x1138, 0x1139, 0x113A, 0x113B, 0x113C, 0x113D, 0x113E,
        0x113F, 0x1140, 0x1141, 0x1142, 0x1143, 0x1144, 0x1145, 0x1146, 0x1147, 0x1148, 0x1149, 0x114A,
    },
};

static const u16 sCardEMatrixIds1[2][36] = {
    {
        0x104E, 0x104F, 0x1050, 0x1051, 0x1052, 0x1053, 0x1054, 0x1055, 0x1056, 0x1057, 0x1058, 0x1059,
        0x105A, 0x105B, 0x105C, 0x105D, 0x105E, 0x105F, 0x1060, 0x1061, 0x1062, 0x1063, 0x1064, 0x1065,
        0x1066, 0x1067, 0x1068, 0x1069, 0x106A, 0x106B, 0x106C, 0x106D, 0x106E, 0x106F, 0x1070, 0x1071,
    },
    {
        0x114B, 0x114C, 0x114D, 0x114E, 0x114F, 0x1150, 0x1151, 0x1152, 0x1153, 0x1154, 0x1155, 0x1156,
        0x1157, 0x1158, 0x1159, 0x115A, 0x115B, 0x115C, 0x115D, 0x115E, 0x115F, 0x1160, 0x1161, 0x1162,
        0x1163, 0x1164, 0x1165, 0x1166, 0x1167, 0x1168, 0x1169, 0x116A, 0x116B, 0x116C, 0x116D, 0x116E,
    },
};

static const u16 sCardEMatrixIds2[2][36] = {
    {
        0x1072, 0x1073, 0x1074, 0x1075, 0x1076, 0x1077, 0x1078, 0x1079, 0x107A, 0x107B, 0x107C, 0x107D,
        0x107E, 0x107F, 0x1080, 0x1081, 0x1082, 0x1083, 0x1084, 0x1085, 0x1086, 0x1087, 0x1088, 0x1089,
        0x108A, 0x108B, 0x108C, 0x108D, 0x108E, 0x108F, 0x1090, 0x1091, 0x1092, 0x1093, 0x1094, 0x1095,
    },
    {
        0x116F, 0x1170, 0x1171, 0x1172, 0x1173, 0x1174, 0x1175, 0x1176, 0x1177, 0x1178, 0x1179, 0x117A,
        0x117B, 0x117C, 0x117D, 0x117E, 0x117F, 0x1180, 0x1181, 0x1182, 0x1183, 0x1184, 0x1185, 0x1186,
        0x1187, 0x1188, 0x1189, 0x118A, 0x118B, 0x118C, 0x118D, 0x118E, 0x118F, 0x1190, 0x1191, 0x1192,
    },
};

static const s32 sCardEMatrixCellSprites[8] = {
    0x20A, 0x20C, 0x20E, 0x210, 0x212, 0x214, 0x216, 0x218,
};

static const s32 sCardEMatrixEndSprites[8] = {
    0x20B, 0x20D, 0x20F, 0x211, 0x213, 0x215, 0x217, 0x219,
};

#define CARDE_MATRIX_FILE "menuCardE_Matrix.c"
#define CARDE_MATRIX_SERIES_N "i < cem->m_seriesN"
#define CARDE_MATRIX_ANIMATING "!cem->m_isAnimating"
#define CARDE_MATRIX_ANIM_CUR "s[ANIM_cur]"
#define CARDE_MATRIX_IDS(group) ((u16*)sCardEMatrixIds##group[0])
#define CARDE_MATRIX_CELL_SPRITES sCardEMatrixCellSprites
#define CARDE_MATRIX_END_SPRITES sCardEMatrixEndSprites

extern void GScharLenCpy(void*, const void*, u32);
extern void qsort(void*, u32, u32, s32 (*)(u32, u32));
extern u16 GSmemAlloc(u32, u32);
extern void* GSmemLock(u16);
extern u16 GSmemGetHandleByPtr(void*);
extern void fn_800E24B0(u16);
extern void GSmemFree(u16);
extern void* windowSearchItemID(void*, s32);
extern s32 fn_80083BF8(void* arena);
extern struct MenuCardEEntry* fn_80083AF4(void* arena, s32 index);

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
    CARDE_CTX_S16(ctx, dst + 6) = item->h;
    CARDE_CTX_S16(ctx, dst + 4) = item->w;
}


s32 menuCardE_CompareEntryPtrs(u32 r3, u32 r4);

static inline void* menuCardE_Alloc(u32 size) {
    void* buf;
    u16 handle;

    handle = GSmemAlloc((size + 0x1F) & ~0x1F, 0x20);
    if (handle == 0) {
        __assert(CARDE_MATRIX_FILE, 0x1A2, &lbl_8047C140);
    }
    buf = GSmemLock(handle);
    memset(buf, 0, size);
    return buf;
}

static inline void menuCardE_Free(void* buf) {
    u16 handle;

    handle = GSmemGetHandleByPtr(buf);
    if (handle == 0) {
        __assert(CARDE_MATRIX_FILE, 0x1AB, &lbl_8047C140);
    }
    fn_800E24B0(handle);
    GSmemFree(handle);
}

/* Allocate, fill and sort the entry-pointer array at +0xB0. */
static inline void menuCardE_FillEntries(u8* ctx, s32 count) {
    s32 k;

    CARDE_CTX_U32(ctx, 0xB0) = (u32)menuCardE_Alloc(count * 4);

    for (k = 0; k < count; k++) {
        ((u32*)CARDE_CTX_U32(ctx, 0xB0))[k] =
            (u32)fn_80083AF4(0, k);
    }
    qsort((void*)CARDE_CTX_U32(ctx, 0xB0), count, 4,
          menuCardE_CompareEntryPtrs);
}

#else
extern MenuCardEMatrixTable lbl_80268B88;
extern u8 lbl_80268D78[];
extern u8 lbl_80268D8C[];
extern u8 lbl_80268DA0[];
extern u8 lbl_80268DB4[];
#define CARDE_MATRIX_FILE lbl_80268D78
#define CARDE_MATRIX_SERIES_N lbl_80268D8C
#define CARDE_MATRIX_ANIMATING lbl_80268DA0
#define CARDE_MATRIX_ANIM_CUR lbl_80268DB4
#define CARDE_MATRIX_IDS(group) (table->ids[group][0])
#define CARDE_MATRIX_CELL_SPRITES (table->sprites[0])
#define CARDE_MATRIX_END_SPRITES ((s32*)((u8*)table + 0x1D0))
#endif

/* ===== Forward declarations ===== */
void menuCardEMatrixSelectCard(u8 cardId, u8 subIndex);
void fn_8007C414(void);
void menuCardEMatrixSetTarget(u8 cardId, u8 subIndex, s8 row, s8 column, s32 state);
void fn_8007C634(void);
void fn_8007C764(u8 arg);
void fn_8007C7A8(u8 arg);
void fn_8007C7EC(void);
void fn_8007CAB0(void);
void fn_8007CB54(u32 arg);
struct MenuCardEMatrixItem;
void fn_8007CBB4(void*, struct MenuCardEMatrixItem*);
void fn_8007D4FC(void* window, u8* param);
void fn_8007D564(void* window, u8* param);
void fn_8007D79C(void* window, u8* param);
void fn_8007D89C(void* window, u8* param);
struct MenuCardEWindow;
void menuCardEMatrixWindowProc(struct MenuCardEWindow* window);

/*
 * Raw Card-E matrix offset map. Keep the function bodies below in raw
 * register-style form for matching; these names document the supported
 * MenuCardEMatrixContext overlay in include/game/menu/menu.h.
 *
 *   +0x0A0  prevEntryIndex
 *   +0x0A4  currentEntryIndex
 *   +0x0AC  entryCount
 *   +0x0B0  entries (MenuCardEEntry**)
 *   +0x0B4  prevSubIndex
 *   +0x0B5  currentSubIndex
 *   +0x0B6  transitionActive
 *   +0x0B8  transitionFrame
 *   +0x0BC  state/switch selector used by menuCardEMatrixWindowProc
 */

/* ===== Function implementations ===== */

typedef struct MenuCardEEntry {
    u8 unk_00[0x1A];
    u8 cardId;
    s8 layerCount;
    s8 sortGroup;
    s8 columns;
    u8 rowLayer[6];
} MenuCardEEntry;

typedef struct MenuCardEMatrixContext {
    u8 unk_000[0xA0];
    s32 prevEntryIndex;
    s32 currentEntryIndex;
    s16 prevCardId;
    s16 currentCardId;
    s32 entryCount;
    MenuCardEEntry** entries;
    u8 prevSubIndex;
    u8 currentSubIndex;
    u8 transitionActive;
    u8 unk_B7;
    s32 transitionFrame;
    s32 unk_BC;
    s32 gridIndex;
    s32 unk_C4;
    u8 unk_C8;
    u8 unk_C9;
} MenuCardEMatrixContext;

/* Fetch the Card-E context of window 0xA6 and wait for its transition to
 * finish; NULL when the window has no context. */
static inline MenuCardEMatrixContext* menuCardEWaitTransition(void) {
    extern void* windowSearchID(s32 id);
    extern MenuCardEMatrixContext** windowGetFreeWork(void* window);
    MenuCardEMatrixContext* context;

    context = *windowGetFreeWork(windowSearchID(0xA6));
    if (context == NULL) {
        return NULL;
    }
    *((u8*)context + 0xC8) = 0;
    while (context->transitionActive != 0) {
        _threadSwitch();
        context = *windowGetFreeWork(windowSearchID(0xA6));
        if (context == NULL) {
            return NULL;
        }
    }
    return context;
}

/*
 * 0x8007C300 | size: 0x114
 * Proposed role: set the current Card-E entry by entry+0x1A card id and copy
 * that target into both current/previous entry and sub-selection fields.
 */
#if defined(MENU_CARDE_R48_8007C300_PREFIX_ACTIVE)
/* 0x8007C300 - 0x8007C7EC are built with the peephole pass off. */
/* RULE-EXCEPTION(user-approved): unit-wide local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void menuCardEMatrixSelectCard(u8 cardId, u8 subIndex) {
    MenuCardEMatrixContext* context;
    s32 index;

    context = menuCardEWaitTransition();
    if (context == NULL) {
        return;
    }

    for (index = 0; index < context->entryCount; index++) {
        if (context->entries[index]->cardId == cardId) {
            break;
        }
    }
    if (index >= context->entryCount) {
        __assert(CARDE_MATRIX_FILE, 0x648, CARDE_MATRIX_SERIES_N);
    }
    context->currentEntryIndex = index;
    context->prevEntryIndex = index;
    context->currentSubIndex = subIndex;
    context->prevSubIndex = subIndex;
}

/* 0x8007C414 | size: 0x3C */
void fn_8007C414(void) {
    extern void* windowSearchID(u32 id);
    extern u32* windowGetFreeWork(void* obj);
    u32 p;

    p = *windowGetFreeWork(windowSearchID(0xa6));
    if (p != 0) {
        *(u32*)(p + 0xBC) = 0;
    }
    return;
}

/*
 * 0x8007C450 | size: 0x1E4
 * Proposed role: set a target Card-E selection, preserve the previous
 * entry/sub-selection, and arm the transition flag/frame when the target
 * differs.
 */
void menuCardEMatrixSetTarget(u8 cardId, u8 subIndex, s8 row, s8 column, s32 state) {
    MenuCardEMatrixContext* context;
    MenuCardEEntry* entry;
    s32 index;

    context = menuCardEWaitTransition();
    if (context == NULL) {
        return;
    }

    context->prevEntryIndex = context->currentEntryIndex;
    context->prevSubIndex = context->currentSubIndex;
    for (index = 0; index < context->entryCount; index++) {
        if (context->entries[index]->cardId == cardId) {
            break;
        }
    }
    if (index >= context->entryCount) {
        __assert(CARDE_MATRIX_FILE, 0x608, CARDE_MATRIX_SERIES_N);
    }

    context->currentEntryIndex = index;
    context->currentSubIndex = subIndex;
    entry = context->entries[index];
    context->unk_BC = state;
    if (column < 0) {
        context->gridIndex = entry->columns + row * 6;
    } else {
        context->gridIndex = column + row * 6;
    }
    context->unk_C4 = 0;
    if (context->currentEntryIndex == context->prevEntryIndex &&
        (s8)context->currentSubIndex == (s8)context->prevSubIndex) {
        return;
    }

    context->transitionActive = 1;
    context->transitionFrame = 0;
    menuCardEWaitTransition();
}

/* 0x8007C634 | size: 0x130 */
void fn_8007C634(void) {
    MenuCardEMatrixContext* context;

    context = menuCardEWaitTransition();
    if (context == NULL || context->entryCount <= 0) {
        return;
    }

    context->prevEntryIndex = context->currentEntryIndex;
    context->prevSubIndex = context->currentSubIndex;
    context->currentEntryIndex = 0;
    context->currentSubIndex = 0;
    if (context->currentEntryIndex == context->prevEntryIndex &&
        (s8)context->currentSubIndex == (s8)context->prevSubIndex) {
        return;
    }

    context->transitionActive = 1;
    context->transitionFrame = 0;
    menuCardEWaitTransition();
}

/* 0x8007C764 | size: 0x44 */
void fn_8007C764(u8 arg) {
    extern void* windowSearchID(u32 id);
    extern u32* windowGetFreeWork(void* obj);
    u32 p;

    p = *windowGetFreeWork(windowSearchID(0xa6));
    if (p != 0) {
        *(u8*)(p + 0xC9) = arg;
    }
    return;
}

/* 0x8007C7A8 | size: 0x44 */
void fn_8007C7A8(u8 arg) {
    extern void* windowSearchID(u32 id);
    extern u32* windowGetFreeWork(void* obj);
    u32 p;

    p = *windowGetFreeWork(windowSearchID(0xa6));
    if (p != 0) {
        *(u8*)(p + 0xC8) = arg;
    }
    return;
}
#pragma pop
#endif

/*
 * 0x8007C7EC | size: 0x2C4
 * Proposed role: rebuild and sort the Card-E entry pointer array, then
 * reselect the current entry using the saved card id at context+0xAA.
 */
#if defined(MENU_CARDE_R48_8007C7EC_O2_ACTIVE) && !defined(MENU_CARDE_MATRIX_TU)
/* Reselect the entry holding the saved card id, or -1. */
static inline void menuCardEMatrixFindCard(MenuCardEMatrixContext* context) {
    s32 i;

    context->currentEntryIndex = -1;
    for (i = 0; i < context->entryCount; i++) {
        if (context->entries[i]->cardId == context->currentCardId) {
            context->currentEntryIndex = i;
            break;
        }
    }
}

/* Allocate, fill and sort the context's entry-pointer array. */
static inline void menuCardEMatrixFillEntries(MenuCardEMatrixContext* context,
                                              s32 count) {
    extern MenuCardEEntry* fn_80083AF4(void* arena, s32 index);
    extern void qsort(void* base, u32 count, u32 size, s32 (*compare)(const void*, const void*));
    extern s32 menuCardE_CompareEntryPtrs(const void* a, const void* b);
    extern u16 GSmemAlloc(u32 size, u32 align);
    extern void* GSmemLock(u16 handle);
    MenuCardEEntry** entries;
    u32 size;
    u16 handle;
    s32 k;

    size = count * 4;
    handle = GSmemAlloc((size + 0x1F) & ~0x1F, 0x20);
    if (handle == 0) {
        __assert(CARDE_MATRIX_FILE, 0x1A2, &lbl_8047C140);
    }
    entries = GSmemLock(handle);
    memset(entries, 0, size);
    context->entries = entries;
    for (k = 0; k < count; k++) {
        context->entries[k] = fn_80083AF4(NULL, k);
    }
    qsort(context->entries, count, 4, menuCardE_CompareEntryPtrs);
}

#endif
#if defined(MENU_CARDE_R48_8007C7EC_O2_ACTIVE)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8007C7EC(void) {
    extern void* windowSearchID(s32 id);
    extern MenuCardEMatrixContext** windowGetFreeWork(void* window);
    extern s32 fn_80083BF8(void* arena);
    extern MenuCardEEntry* fn_80083AF4(void* arena, s32 index);
    extern u8 cardeSaveLevelHasCard(MenuCardEEntry* entry, u8 subIndex);
    extern void qsort(void* base, u32 count, u32 size, s32 (*compare)(const void*, const void*));
    extern s32 menuCardE_CompareEntryPtrs(const void* a, const void* b);
    extern u16 GSmemAlloc(u32 size, u32 align);
    extern void* GSmemLock(u16 handle);
    extern u16 GSmemGetHandleByPtr(void* ptr);
    extern void fn_800E24B0(u16 handle);
    extern void GSmemFree(u16 handle);
    MenuCardEMatrixContext* context;
    s32 savedIndex;
    u8 subIndex;
    MenuCardEEntry* entry;
#if !defined(MENU_CARDE_MATRIX_TU)
    u16 handle;
#endif
    s32 count;

    context = *windowGetFreeWork(windowSearchID(0xA6));
    if (context == NULL) {
        return;
    }
    if (context->transitionActive != 0) {
        __assert(CARDE_MATRIX_FILE, 0x594, CARDE_MATRIX_ANIMATING);
    }

    savedIndex = context->currentEntryIndex;
#if defined(MENU_CARDE_MATRIX_TU)
    if (context->entries != NULL) {
        menuCardE_Free(context->entries);
        context->entries = NULL;
    }

    count = context->entryCount = fn_80083BF8(NULL);
    if (count != 0) {
        menuCardE_FillEntries((u8*)context, count);
    }
#else
    if (context->entries != NULL) {
        handle = GSmemGetHandleByPtr(context->entries);
        if (handle == 0) {
            __assert(CARDE_MATRIX_FILE, 0x1AB, &lbl_8047C140);
        }
        fn_800E24B0(handle);
        GSmemFree(handle);
        context->entries = NULL;
    }

    count = context->entryCount = fn_80083BF8(NULL);
    if (count != 0) {
        menuCardEMatrixFillEntries(context, count);
    }

#endif

    menuCardEMatrixFindCard(context);
    if (context->currentEntryIndex < 0 && context->entryCount <= savedIndex) {
        context->currentEntryIndex = context->entryCount - 1;
    }
    context->prevEntryIndex = context->currentEntryIndex;

    if (context->entryCount <= 0 || context->currentEntryIndex < 0) {
        entry = NULL;
    } else {
        entry = context->entries[context->currentEntryIndex];
    }
    if (entry != NULL) {
        for (subIndex = context->currentSubIndex; (s8)subIndex > 0; subIndex--) {
            if (cardeSaveLevelHasCard(entry, subIndex) != 0) {
                break;
            }
        }
        context->currentSubIndex = subIndex;
    }

    if (context->currentEntryIndex == context->prevEntryIndex &&
        (s8)context->currentSubIndex == (s8)context->prevSubIndex) {
        return;
    }
    context->transitionActive = 1;
    context->transitionFrame = 0;
    menuCardEWaitTransition();
}
#pragma pop
#endif

/* 0x8007CAB0 | size: 0xA4 */
#if defined(MENU_CARDE_R48_8007CAB0_SUFFIX_ACTIVE)
/* 0x8007CAB0 - 0x8007CBB4 are built with the peephole pass off. */
/* RULE-EXCEPTION(user-approved): unit-wide local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8007CAB0(void) {
    extern void* windowSearchID(s32 id);
    extern MenuCardEMatrixContext** windowGetFreeWork(void* window);
    MenuCardEMatrixContext* context;
    s32 i;
    s32 index;

    context = *windowGetFreeWork(windowSearchID(0xA6));
    if (context != NULL) {
        if (context->transitionActive == 0) {
            context->prevEntryIndex = context->currentEntryIndex;
            context->prevSubIndex = context->currentSubIndex;
        }
        /* Cache the prev/current entries' card ids at +0xA8 / +0xAA. */
        for (i = 0; i < 2; i++) {
            index = *(s32*)((u8*)context + 0xA0 + i * 4);
            if (index < 0) {
                *(s16*)((u8*)context + 0xA8 + i * 2) = -1;
            } else {
                *(u16*)((u8*)context + 0xA8 + i * 2) = context->entries[index]->cardId;
            }
        }
    }
}

/* 0x8007CB54 | size: 0x60 */
void fn_8007CB54(u32 arg) {
    extern void* windowSearchID(u32 id);
    extern u32* windowGetFreeWork(void* obj);
    extern void GScharLenCpy(u32 p, u32 arg, u32 size);
    u32 p;

    p = *windowGetFreeWork(windowSearchID(0xa6));
    if (p != 0) {
        GScharLenCpy(p, arg, 0x50);
        *(u16*)(p + 0x9E) = 0;
    }
    return;
}
#pragma pop
#endif

/* 0x8007CBB4 | size: 0x948 */
#if defined(MENU_CARDE_MATRIX_8007CBB4_ACTIVE)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#define menuCardEDigits ((u16*)&lbl_8047A658)
typedef struct MenuCardEMatrixItem {
    u8 unk_00[6];
    s16 id;
    u8 unk_08[0x44];
    u32 msgId;
    u8 unk_50[8];
    u32 spriteTexture;
    s16 spriteRect[4];
} MenuCardEMatrixItem;

typedef struct MenuCardEMatrixSprite {
    u8 unk_00[8];
    s16 rect[4];
    u32 texture;
} MenuCardEMatrixSprite;

typedef struct MenuCardEMatrixCell {
    u8 unk_00[0xC];
    u8 valid;
    u8 unk_0D;
    u16 value;
} MenuCardEMatrixCell;

static inline s32 menuCardEFindItemId(u16* ids, u16 id) {
    s32 i;

    for (i = 0; i < 36; i++) {
        if (ids[i] == id) {
            return i;
        }
    }
    return -1;
}

void fn_8007CBB4(void* window, MenuCardEMatrixItem* item) {
    extern void* windowSearchID(s32 id);
    extern MenuCardEMatrixContext** windowGetFreeWork(void* window);
    extern u8 winSpriteGetDisp(MenuCardEMatrixItem* item);
    extern MenuCardEMatrixSprite* menuSpriteBiosGetPtr(s32 id);
    extern void msgctrlSetValue(u32 id, void* value);
    extern u8* cardeSaveGetLevel(MenuCardEEntry* entry, u8 layer);
    extern MenuCardEMatrixCell* cardeSaveGetCard(MenuCardEEntry* entry, u8 layer, s8 row, s8 column);
    MenuCardEMatrixSprite* sprite;
#if !defined(MENU_CARDE_MATRIX_TU)
    MenuCardEMatrixTable* table;
#endif
    MenuCardEMatrixContext* context;
    s8 row;
    s32 group;
    s32 index;
    MenuCardEEntry* entry;
    u16* ids;
    s8 column;
    s32 which;
    u8 layer;
    u8* rows;
    MenuCardEMatrixCell* cell;
    u16 value;

#if !defined(MENU_CARDE_MATRIX_TU)
    table = &lbl_80268B88;
#endif
    if (window == NULL) {
        window = windowSearchID(0xA6);
    }
    context = *windowGetFreeWork(window);
    item->msgId = 0;
    sprite = NULL;
    if (context == NULL) {
        return;
    }
    if (winSpriteGetDisp(item) == 0) {
        return;
    }

    ids = CARDE_MATRIX_IDS(0);
    if ((index = menuCardEFindItemId(ids, item->id)) >= 0) {
        which = 0;
        group = 0;
    } else if ((index = menuCardEFindItemId(ids + 36, item->id)) >= 0) {
        which = 1;
        group = 0;
    } else if ((index = menuCardEFindItemId(ids = CARDE_MATRIX_IDS(2), item->id)) >= 0) {
        which = 0;
        group = 1;
    } else if ((index = menuCardEFindItemId(ids + 36, item->id)) >= 0) {
        which = 1;
        group = 1;
    } else if ((index = menuCardEFindItemId(ids = CARDE_MATRIX_IDS(1), item->id)) >= 0) {
        which = 0;
        group = 2;
    } else if ((index = menuCardEFindItemId(ids + 36, item->id)) >= 0) {
        which = 1;
        group = 2;
    } else {
        return;
    }

    if (context->entryCount <= 0 || (&context->prevEntryIndex)[which] < 0) {
        entry = NULL;
    } else {
        entry = context->entries[(&context->prevEntryIndex)[which]];
    }
    if (entry == NULL) {
        return;
    }

    row = index / 6;
    column = index % 6;
    layer = entry->rowLayer[row];
    if (row >= entry->sortGroup) {
        return;
    }

    if (column == entry->columns) {
        rows = cardeSaveGetLevel(entry, (&context->prevSubIndex)[which]);
        switch (group) {
        case 0:
            sprite = menuSpriteBiosGetPtr(CARDE_MATRIX_END_SPRITES[layer]);
            break;
        case 1:
            if (rows[row * 0xE + 0x1C] != 0) {
                item->msgId = 0xE5;
                msgctrlSetValue(0x37, rows + row * 0xE + 0x10);
            }
            break;
        case 2:
            item->msgId = 0x3CCF;
            break;
        }
    } else if (column < entry->columns) {
        cell = cardeSaveGetCard(entry, (&context->prevSubIndex)[which], row, column);
        if (cell->valid != 0) {
            msgctrlSetValue(0x37, cell);
        }
        switch (group) {
        case 0:
            sprite = menuSpriteBiosGetPtr(CARDE_MATRIX_CELL_SPRITES[layer]);
            break;
        case 1:
            if (cell->valid != 0) {
                item->msgId = 0xE5;
                msgctrlSetValue(0x37, cell);
            }
            break;
        case 2:
            if (cell->valid != 0) {
                item->msgId = 0xE5;
                value = cell->value;
                menuCardEDigits[3] = 0;
                menuCardEDigits[0] = '0' + (value / 100) % 10;
                menuCardEDigits[1] = '0' + (value / 10) % 10;
                menuCardEDigits[2] = '0' + value % 10;
                msgctrlSetValue(0x37, menuCardEDigits);
            }
            break;
        }
    } else {
        return;
    }

    if (sprite != NULL) {
        item->spriteTexture = sprite->texture;
        item->spriteRect[0] = sprite->rect[0];
        item->spriteRect[1] = sprite->rect[1];
        item->spriteRect[2] = sprite->rect[2];
        item->spriteRect[3] = sprite->rect[3];
    }
}
#pragma pop

/* 0x8007D4FC | size: 0x68 */
#endif

#if defined(MENU_CARDE_MATRIX_8007D4FC_GC20_ACTIVE)
/* 0x8007D4FC - 0x8007D978 are built with the peephole pass off. */
/* RULE-EXCEPTION(user-approved): unit-wide local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off

void fn_8007D4FC(void* window, u8* param) {
    extern void* windowSearchID(u32 id);
    extern u16** windowGetFreeWork(void* window);
    extern void msgctrlSetValue(u32 id, void* value);
    u16* work;

    if (window == 0) {
        window = windowSearchID(0xA6);
    }
    work = *windowGetFreeWork(window);
    if (*work != 0) {
        *(u32*)(param + 0x4C) = 0xE4;
        msgctrlSetValue(0x37, work);
    } else {
        *(u32*)(param + 0x4C) = 0;
    }
}

/* Card-E matrix work of a window, defaulting to window 0xA6. */
static inline u8* menuCardEGetWork(void* window) {
    extern void* windowSearchID(u32 id);
    extern u8** windowGetFreeWork(void* window);

    if (window == 0) {
        window = windowSearchID(0xA6);
    }
    return *windowGetFreeWork(window);
}

/* Entry lookup through the prev/current index pair at +0xA0 (which = 0/1). */
static inline void* menuCardEGetEntry(u8* work, s32 which) {
    s32 index;

    if (*(s32*)(work + 0xAC) <= 0 ||
        (index = ((s32*)(work + 0xA0))[which]) < 0) {
        return 0;
    }
    return *(void**)(*(u8**)(work + 0xB0) + index * 4);
}

/* 0x8007D564 | size: 0x238 */
void fn_8007D564(void* window, u8* param) {
    extern u8* cardeSaveGetLevel(void* entry, s8 sub_index);
    extern void msgctrlSetValue(u32 id, void* value);
    u8* work;
    u8* entry;
    u8* text;
    s8 sub;

    work = menuCardEGetWork(window);
    if (work == 0) {
        return;
    }

    /* Retail has no default: other message ids test an unset entry. */
    switch (*(s16*)(param + 6)) {
        case 0x1193:
            sub = 0;
            entry = menuCardEGetEntry(work, 0);
            break;
        case 0x1195:
            sub = 1;
            entry = menuCardEGetEntry(work, 0);
            break;
        case 0x1194:
            sub = 2;
            entry = menuCardEGetEntry(work, 0);
            break;
        case 0x796:
            sub = 0;
            entry = menuCardEGetEntry(work, 1);
            break;
        case 0x793:
            sub = 1;
            entry = menuCardEGetEntry(work, 1);
            break;
        case 0x797:
            sub = 2;
            entry = menuCardEGetEntry(work, 1);
            break;
    }

    if (entry != 0 && sub < (s8)entry[0x1B]) {
        text = cardeSaveGetLevel(entry, sub);
        *(u32*)(param + 0x4C) = 0xE4;
        msgctrlSetValue(0x37, text);
        return;
    }
    *(u32*)(param + 0x4C) = 0;
}

/* 0x8007D79C | size: 0x100 */
void fn_8007D79C(void* window, u8* param) {
    extern u8* cardeSaveGetLevel(void* entry, u32 sub_index);
    extern void msgctrlSetValue(u32 id, u32 value);
    u8* work;
    void* entry;
    s32 which;
    u8* result;

    work = menuCardEGetWork(window);
    *(u32*)(param + 0x4C) = 0;
    if (work == 0) {
        return;
    }

    switch (*(s16*)(param + 6)) {
        case 0x1126:
            which = 0;
            break;
        case 0x795:
            which = 1;
            break;
        default:
            return;
    }

    entry = menuCardEGetEntry(work, which);
    if (entry == 0) {
        return;
    }

    work += which;
    result = cardeSaveGetLevel(entry, *(u8*)(work + 0xB4));
    if (*(u8*)(result + 0x71) == 0) {
        return;
    }

    *(u32*)(param + 0x4C) = 0x3CBE;
    msgctrlSetValue(0x58, *(u8*)(result + 0x70));
    msgctrlSetValue(0x23, (u32)(result + 0x64));
}

/* 0x8007D89C | size: 0xDC */
void fn_8007D89C(void* window, u8* param) {
    extern void msgctrlSetValue(u32 id, void* value);
    u8* work;
    void* entry;
    s32 index;

    work = menuCardEGetWork(window);
    if (work == 0) {
        return;
    }

    if (*(s16*)(param + 6) == 0x791) {
        entry = menuCardEGetEntry(work, 0);
    } else {
        entry = menuCardEGetEntry(work, 1);
    }

    if (entry != 0) {
        *(u32*)(param + 0x4C) = 0xE3;
        msgctrlSetValue(0x37, entry);
    } else {
        *(u32*)(param + 0x4C) = 0;
    }
}

#pragma pop

/*
 * 0x8007D978 | size: 0x23EC
 * Proposed role: Card-E matrix main state machine. The switch over
 * jumptable_802EE868 has 10 cases and uses context+0xBC as its selector.
 */
#endif

#if defined(MENU_CARDE_MATRIX_8007D978_ACTIVE)
#pragma push
#pragma peephole off
typedef struct MenuCardEColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} MenuCardEColor;

typedef struct MenuCardESprite {
    struct MenuCardESprite* next;
    u8 unk_04;
    u8 flags;
    u8 unk_06[0x4A];
    s16 x;
    s16 y;
    u8 unk_54[0x10];
    u32 color;
} MenuCardESprite;

typedef struct MenuCardEWindow {
    u8 unk_00;
    s8 state;
    s8 subState;
    u8 unk_03;
    s32 id;
    u8 unk_08[0x14];
    MenuCardESprite* sprites;
} MenuCardEWindow;

typedef struct MenuCardEKeyInfo {
    u16 held;
    u8 unk_02[4];
    u16 trigger;
} MenuCardEKeyInfo;

/* The full Card-E matrix work area built by fn_8007FDBC. */
typedef struct MenuCardEMatrixWork {
    u8 title[0xA0];
    s32 entryIndex[2];          /* 0xA0 previous, 0xA4 current */
    s16 cardId[2];              /* 0xA8 */
    s32 entryCount;             /* 0xAC */
    MenuCardEEntry** entries;   /* 0xB0 */
    s8 subIndex[2];             /* 0xB4 previous, 0xB5 current */
    u8 transitionActive;        /* 0xB6 */
    u8 unk_B7;
    s32 transitionFrame;        /* 0xB8 */
    s32 state;                  /* 0xBC */
    s32 gridIndex;              /* 0xC0 */
    s32 stateFrame;             /* 0xC4 */
    u8 active;                  /* 0xC8 */
    u8 showArrows;              /* 0xC9 */
    u8 buzzed;                  /* 0xCA */
    u8 unk_CB;
    u16 lastKey;                /* 0xCC */
    s16 rect[9][4];             /* 0xCE */
    u8 unk_116[2];
    MenuCardESprite* subTab[3];         /* 0x118 */
    MenuCardESprite* layerTab[3];       /* 0x124 */
    MenuCardESprite* layerMark[2][3];   /* 0x130 */
    MenuCardESprite* frameA[2];         /* 0x148 */
    MenuCardESprite* frameB[2];         /* 0x150 */
    MenuCardESprite* scroll[2];         /* 0x158 */
    MenuCardESprite* arrowPrev[2];      /* 0x160 */
    MenuCardESprite* arrowNext[2];      /* 0x168 */
    MenuCardESprite* cell[2][36];       /* 0x170 */
    MenuCardESprite* cellBack[2][36];   /* 0x290 */
    MenuCardESprite* cellFront[2][36];  /* 0x3B0 */
    MenuCardESprite* cursor;            /* 0x4D0 */
    MenuCardESprite* empty;             /* 0x4D4 */
    MenuCardESprite* pageArrow[2];      /* 0x4D8 */
    MenuCardESprite* subArrow[2];       /* 0x4E0 */
} MenuCardEMatrixWork;

static inline u32 menuCardELerpColor(u32 to, u32 from, f32 t) {
    u32 result;

    ((MenuCardEColor*)&result)->r =
        (((MenuCardEColor*)&to)->r - ((MenuCardEColor*)&from)->r) * t + ((MenuCardEColor*)&from)->r;
    ((MenuCardEColor*)&result)->g =
        (((MenuCardEColor*)&to)->g - ((MenuCardEColor*)&from)->g) * t + ((MenuCardEColor*)&from)->g;
    ((MenuCardEColor*)&result)->b =
        (((MenuCardEColor*)&to)->b - ((MenuCardEColor*)&from)->b) * t + ((MenuCardEColor*)&from)->b;
    ((MenuCardEColor*)&result)->a =
        (((MenuCardEColor*)&to)->a - ((MenuCardEColor*)&from)->a) * t + ((MenuCardEColor*)&from)->a;
    return result;
}

static inline void menuCardEBuzz(MenuCardEMatrixWork* work) {
    extern void soundStartSE(s32 id);

    if (work->buzzed == 0) {
        soundStartSE(0x26);
    }
    work->buzzed = 1;
}

static inline MenuCardEEntry* menuCardEWorkEntry(MenuCardEMatrixWork* work, s32 which) {
    if (work->entryCount <= 0 || work->entryIndex[which] < 0) {
        return NULL;
    }
    return work->entries[work->entryIndex[which]];
}

static inline void menuCardEFreeHandle(void* buf) {
    extern u16 GSmemGetHandleByPtr(void* ptr);
    extern void fn_800E24B0(u16 handle);
    extern void GSmemFree(u16 handle);
    u16 handle;

    handle = GSmemGetHandleByPtr(buf);
    if (handle == 0) {
        __assert(CARDE_MATRIX_FILE, 0x1AB, &lbl_8047C140);
    }
    fn_800E24B0(handle);
    GSmemFree(handle);
}

/* 0x8007D978 | size: 0x23EC
 * Card-E matrix window procedure: builds/frees the work area, handles
 * the entry/layer cursor keys and animates the grid, tabs and cursor. */
void menuCardEMatrixWindowProc(MenuCardEWindow* window) {
    extern void* windowSearchID(s32 id);
    extern MenuCardEMatrixWork** windowGetFreeWork(void* window);
    extern void* windowGetParam(void* window, s32 index);
    extern s32 windowGetActiveID(void);
    extern MenuCardEKeyInfo* windowGetKeyInfo(void);
    extern MenuCardEMatrixWork* fn_8007FDBC(void* window, void* title);
    extern void fn_801081F8(void* window, s32 itemId, s32 arg);
    extern void winSpriteSetDisp(MenuCardESprite* sprite, s32 disp);
    extern void soundStartSE(s32 id);
    extern u8 cardeSaveLevelHasCard(MenuCardEEntry* entry, s8 layer);
    extern u8* cardeSaveGetLevel(MenuCardEEntry* entry, s8 layer);
    extern u8* cardeSaveGetCard(MenuCardEEntry* entry, s8 layer, s8 row, s8 column);
    MenuCardEMatrixWork* work;
    MenuCardEEntry* entries[3];
    MenuCardEEntry** entryp;
    MenuCardEEntry* entry;
    MenuCardEWindow* other;
    MenuCardESprite* sprite;
    u8* layer;
    f32 phase;
    f32 slideX;
    f32 slideY;
    f32 slideXr;
    f32 slideYr;
    f32 rt;
    f32 t;
    u32 from;
    u32 to;
    u32 color;
    s32 which;
    s32 j;
    s32 i;
    u32 k;
    u32 gridFrom;
    u32 gridTo;
    u32 tabFrom;
    u32 tabTo;
    s32 index;
    s32 dir;
    s16 baseX;
    s16 baseY;
    s16 stepX;
    s16 stepY;
    s8 row;
    s8 column;
    s8 count;
    s32 held;

    work = *windowGetFreeWork(window == NULL ? windowSearchID(0xA6) : window);
    switch (window->state) {
    case 0:
        if (window->subState == 0) {
            work = fn_8007FDBC(window, windowGetParam(window, 0));
            *windowGetFreeWork(window) = work;
            fn_801081F8(window, 0x79A, 0x40);
            fn_801081F8(window, 0x826, 0x48);
        }
        break;
    case 3:
        if (window->subState == 0) {
            window->subState = 1;
        }
        break;
    case 5:
        if (work->entries != NULL) {
            menuCardEFreeHandle(work->entries);
            work->entries = NULL;
        }
        menuCardEFreeHandle(work);
        return;
    }

    for (i = 0; i < 3; i++) {
        winSpriteSetDisp(work->subTab[i], 0);
        winSpriteSetDisp(work->layerTab[i], 0);
        for (j = 0; j < 2; j++) {
            winSpriteSetDisp(work->layerMark[j][i], 0);
        }
    }
    for (i = 0; i < 2; i++) {
        winSpriteSetDisp(work->frameA[i], 0);
        winSpriteSetDisp(work->frameB[i], 0);
        winSpriteSetDisp(work->scroll[i], 0);
    }
    winSpriteSetDisp(work->arrowPrev[0], 0);
    winSpriteSetDisp(work->arrowPrev[1], 0);
    winSpriteSetDisp(work->arrowNext[0], 0);
    winSpriteSetDisp(work->arrowNext[1], 0);
    for (i = 0; i < 36; i++) {
        for (j = 0; j < 2; j++) {
            winSpriteSetDisp(work->cell[j][i], 0);
            winSpriteSetDisp(work->cellBack[j][i], 0);
            winSpriteSetDisp(work->cellFront[j][i], 0);
        }
    }
    for (k = 0; k < 2; k++) {
        winSpriteSetDisp(work->pageArrow[k], 0);
    }
    for (k = 0; k < 2; k++) {
        winSpriteSetDisp(work->subArrow[k], 0);
    }
    winSpriteSetDisp(work->cursor, 0);
    winSpriteSetDisp(work->empty, 0);

    if (window->id == windowGetActiveID() && work->active != 0) {
        if (work->showArrows != 0) {
            for (k = 0; k < 2; k++) {
                winSpriteSetDisp(work->pageArrow[k], 1);
            }
        }
        for (k = 0; k < 2; k++) {
            winSpriteSetDisp(work->subArrow[k], 1);
        }
    }

    held = windowGetKeyInfo()->held;
    if ((work->lastKey & 0xF) != ((u16)held & 0xF)) {
        work->buzzed = 0;
    }
    work->lastKey = held;

    entries[1] = menuCardEWorkEntry(work, 1);
    entries[2] = NULL;
    if (entries[1] == NULL) {
        winSpriteSetDisp(work->empty, 1);
        if (window->id == windowGetActiveID() && work->active != 0 &&
            (windowGetKeyInfo()->trigger & 0xF) != 0) {
            menuCardEBuzz(work);
        }
        return;
    }

    if (work->transitionActive != 0) {
        work->transitionFrame++;
        if (work->transitionFrame == 16) {
            work->transitionFrame = 0;
            t = 1.0f;
            work->transitionActive = 0;
        } else {
            t = work->transitionFrame * 0.0625f;
        }
    } else {
        work->entryIndex[0] = work->entryIndex[1];
        work->subIndex[0] = work->subIndex[1];
        if (window->id == windowGetActiveID() && work->active != 0) {
            if (windowGetKeyInfo()->trigger & 1) {
                if (work->entryIndex[1] == 0) {
                    menuCardEBuzz(work);
                } else {
                    soundStartSE(0x3C5);
                    work->entryIndex[1]--;
                    work->transitionActive = 1;
                }
            } else if (windowGetKeyInfo()->trigger & 2) {
                if (work->entryIndex[1] == work->entryCount - 1) {
                    menuCardEBuzz(work);
                } else {
                    soundStartSE(0x3C5);
                    work->entryIndex[1]++;
                    work->transitionActive = 1;
                }
            } else if (windowGetKeyInfo()->trigger & 4) {
                if (work->subIndex[1] == 0) {
                    menuCardEBuzz(work);
                } else {
                    soundStartSE(0x3C5);
                    work->subIndex[1]--;
                    work->transitionActive = 1;
                }
            } else if (windowGetKeyInfo()->trigger & 8) {
                if (entries[1] == NULL) {
                    __assert(CARDE_MATRIX_FILE, 0x28E, CARDE_MATRIX_ANIM_CUR);
                }
                if (work->subIndex[1] == entries[1]->layerCount - 1 ||
                    cardeSaveLevelHasCard(entries[1], work->subIndex[1] + 1) == 0) {
                    menuCardEBuzz(work);
                } else {
                    soundStartSE(0x3C5);
                    work->subIndex[1]++;
                    work->transitionActive = 1;
                }
            }
        }
        if (work->transitionActive != 0) {
            entries[1] = menuCardEWorkEntry(work, 1);
            work->state = 0;
            work->transitionFrame = 0;
            if (entries[1]->layerCount <= work->subIndex[1]) {
                work->subIndex[1] = entries[1]->layerCount - 1;
            }
            entry = entries[1];
            while (cardeSaveLevelHasCard(entry, work->subIndex[1]) == 0) {
                work->subIndex[1]--;
            }
            t = 0.0f;
        } else {
            t = 1.0f;
        }
    }

    entryp = entries;
    entries[0] = menuCardEWorkEntry(work, 0);
    entries[1] = menuCardEWorkEntry(work, 1);
    rt = 1.0f - t;
    slideX = 48.0f * t;
    slideY = 32.0f * t;
    slideXr = 48.0f * rt;
    slideYr = 32.0f * rt;

    /* Grid cells of the previous (fading out) and current entry. */
    for (which = 0; which < 2; which++) {
        gridFrom = 0xFFFFFFFF;
        gridTo = 0xFFFFFFFF;
        stepY = work->rect[0][2];
        stepX = work->rect[0][3];
        if (entryp[which] != NULL && (work->transitionActive != 0 || which != 0)) {
            baseX = work->rect[0][0] + (stepX * (5 - entryp[which]->columns)) / 2;
            baseY = work->rect[0][1] + (stepY * (6 - entryp[which]->sortGroup)) / 2;
            switch (which) {
            case 0:
                if (work->subIndex[0] < work->subIndex[1]) {
                    baseX = baseX - slideX;
                } else if (work->subIndex[1] < work->subIndex[0]) {
                    baseX = baseX + slideX;
                }
                if (work->entryIndex[0] < work->entryIndex[1]) {
                    baseY = baseY - slideY;
                } else if (work->entryIndex[1] < work->entryIndex[0]) {
                    baseY = baseY + slideY;
                }
                gridTo = 0xFFFFFF00;
                break;
            case 1:
                if (work->subIndex[0] < work->subIndex[1]) {
                    baseX = baseX + slideXr;
                } else if (work->subIndex[1] < work->subIndex[0]) {
                    baseX = baseX - slideXr;
                }
                if (work->entryIndex[0] < work->entryIndex[1]) {
                    baseY = baseY + slideYr;
                } else if (work->entryIndex[1] < work->entryIndex[0]) {
                    baseY = baseY - slideYr;
                }
                gridFrom = 0xFFFFFF00;
                break;
            }
            color = menuCardELerpColor(gridTo, gridFrom, t);
            layer = cardeSaveGetLevel(entryp[which], work->subIndex[which]);
            for (row = 0; row < entryp[which]->sortGroup; row++) {
                for (column = 0; column < 6; column++) {
                    work->cell[which][row * 6 + column]->x = baseX + column * stepX;
                    work->cell[which][row * 6 + column]->y = baseY + row * stepY;
                    if ((column == entryp[which]->columns && layer[row * 0xE + 0x1C] != 0) ||
                        (column < entryp[which]->columns &&
                         cardeSaveGetCard(entryp[which], work->subIndex[which], row, column)[0xC] != 0)) {
                        if (work->state == 0 || work->state == 6 ||
                            work->gridIndex != row * 6 + column || work->transitionActive == 0) {
                            winSpriteSetDisp(work->cell[which][row * 6 + column], 1);
                            winSpriteSetDisp(work->cellFront[which][row * 6 + column], 1);
                            winSpriteSetDisp(work->cellBack[which][row * 6 + column], 1);
                            work->cell[which][row * 6 + column]->color = color;
                            work->cellFront[which][row * 6 + column]->x =
                                work->rect[1][0] + (baseX + column * stepX) - work->rect[0][0];
                            work->cellFront[which][row * 6 + column]->y =
                                work->rect[1][1] + (baseY + row * stepY) - work->rect[0][1];
                            work->cellFront[which][row * 6 + column]->color = color;
                            work->cellBack[which][row * 6 + column]->x =
                                work->rect[2][0] + (baseX + column * stepX) - work->rect[0][0];
                            work->cellBack[which][row * 6 + column]->y =
                                work->rect[2][1] + (baseY + row * stepY) - work->rect[0][1];
                            work->cellBack[which][row * 6 + column]->color = color;
                        }
                    }
                }
            }
        }
    }

    /* Layer marks of both entries. */
    for (which = 0; which < 2; which++) {
        if ((work->transitionActive != 0 || which != 0) && entryp[which] != NULL) {
            for (i = 0; i < entryp[which]->layerCount; i++) {
                if (i == work->subIndex[which]) {
                    from = 0xFFFFFFFF;
                } else {
                    from = 0x024F84FF;
                }
                switch (which) {
                case 0:
                    to = from & 0xFFFFFF00;
                    break;
                case 1:
                    to = from;
                    from &= 0xFFFFFF00;
                    break;
                }
                winSpriteSetDisp(work->layerMark[which][i], 1);
                work->layerMark[which][i]->color = menuCardELerpColor(to, from, t);
            }
        }
    }

    /* Layer tabs. */
    if (work->transitionActive != 0) {
        count = entries[1]->layerCount;
        if (entries[0] != NULL && count < entries[0]->layerCount) {
            count = entries[0]->layerCount;
        }
        for (row = 0; row < count; row++) {
            tabFrom = 0xFFFFFFFF;
            tabTo = 0xFFFFFFFF;
            winSpriteSetDisp(work->layerTab[row], 1);
            if (entries[0] == NULL || entries[0]->layerCount <= row) {
                tabFrom = 0xFFFFFF00;
            }
            if (entries[1]->layerCount <= row) {
                tabTo = 0xFFFFFF00;
            }
            work->layerTab[row]->color = menuCardELerpColor(tabTo, tabFrom, t);
        }
    } else {
        for (row = 0; row < entries[1]->layerCount; row++) {
            winSpriteSetDisp(work->layerTab[row], 1);
            work->layerTab[row]->color = 0xFFFFFFFF;
        }
    }

    /* Layer tab cursor. */
    winSpriteSetDisp(work->subTab[0], 1);
    if (entries[0] != NULL) {
        work->subTab[0]->x = (work->rect[3 + work->subIndex[1]][0] -
                              work->rect[3 + work->subIndex[0]][0]) * t +
                             work->rect[3 + work->subIndex[0]][0];
    } else {
        work->subTab[0]->x = work->rect[3 + work->subIndex[1]][0];
    }

    /* Entry scroll. */
    if (work->transitionActive != 0 && work->entryIndex[1] != work->entryIndex[0]) {
        if (work->entryIndex[0] < work->entryIndex[1]) {
            dir = 1;
        } else {
            dir = -1;
        }
        winSpriteSetDisp(work->scroll[0], 1);
        work->scroll[0]->color = menuCardELerpColor(0xFFFFFF00, 0xFFFFFFFF, t);
        work->scroll[0]->y = work->rect[6][1] - 10.0f * t * dir;
        winSpriteSetDisp(work->scroll[1], 1);
        work->scroll[1]->color = menuCardELerpColor(0xFFFFFFFF, 0xFFFFFF00, t);
        work->scroll[1]->y = work->rect[6][1] + 10.0f * rt * dir;
    } else {
        winSpriteSetDisp(work->scroll[1], 1);
        work->scroll[1]->color = 0xFFFFFFFF;
        work->scroll[1]->y = work->rect[6][1];
    }

    /* Layer frames. */
    for (which = 0; which < 2; which++, entryp++) {
        if (*entryp != NULL && cardeSaveGetLevel(*entryp, work->subIndex[which])[0x71] != 0) {
            winSpriteSetDisp(work->frameB[which], 1);
            work->frameB[which]->color =
                menuCardELerpColor(lbl_8047C138[which], lbl_8047C130[which], t);
            winSpriteSetDisp(work->frameA[which], 1);
            work->frameA[which]->color =
                menuCardELerpColor(lbl_8047C138[which], lbl_8047C130[which], t);
        }
    }

    if (work->entryIndex[1] > 0) {
        winSpriteSetDisp(work->arrowPrev[0], 1);
        winSpriteSetDisp(work->arrowPrev[1], 1);
    }
    if (work->entryIndex[1] >= 0 && work->entryIndex[1] < work->entryCount - 1) {
        winSpriteSetDisp(work->arrowNext[0], 1);
        winSpriteSetDisp(work->arrowNext[1], 1);
    }

    /* Selected-cell animation. */
    if (work->transitionActive == 0) {
        dir = -1;
        index = work->gridIndex;
        work->stateFrame++;
        switch (work->state) {
        case 1:
            work->cell[1][index]->color =
                menuCardELerpColor(0xFFFFFFFF, 0xFFFFFF00, work->stateFrame * 0.0625f);
            winSpriteSetDisp(work->cellFront[1][index], 0);
            winSpriteSetDisp(work->cellBack[1][index], 0);
            if (work->stateFrame == 16) {
                work->stateFrame = 0;
                work->state = 2;
            }
            break;
        case 2:
            dir = 1;
        case 3:
            winSpriteSetDisp(work->cellFront[1][index], 0);
            winSpriteSetDisp(work->cellBack[1][index], 0);
            goto blink;
        case 4:
            dir = 1;
        case 5:
        blink:
            winSpriteSetDisp(work->cursor, 1);
            work->cursor->x = work->cell[1][index]->x;
            work->cursor->y = work->cell[1][index]->y;
            if (dir == 1) {
                work->cursor->color =
                    menuCardELerpColor(0xFFFFFF77, 0xFFFFFF00, work->stateFrame * 0.0625f);
            } else {
                work->cursor->color =
                    menuCardELerpColor(0xFFFFFF00, 0xFFFFFF77, work->stateFrame * 0.0625f);
            }
            if (work->stateFrame == 16) {
                work->stateFrame = 0;
                work->state += dir;
            }
            break;
        case 6:
            row = index / 6;
            entry = entries[1];
            for (column = 0; column < entry->columns; column++) {
                if (cardeSaveGetCard(entry, work->subIndex[1], row, column)[0xC] == 0) {
                    winSpriteSetDisp(work->cell[1][row * 6 + column], 1);
                    phase = work->stateFrame * 0.03125f;
                    if (phase < 1.0f) {
                        work->cell[1][row * 6 + column]->color =
                            menuCardELerpColor(0xFFFFFF77, 0xFFFFFF00, phase);
                    } else {
                        phase -= 1.0f;
                        work->cell[1][row * 6 + column]->color =
                            menuCardELerpColor(0xFFFFFF00, 0xFFFFFF77, phase);
                    }
                }
            }
            if (work->stateFrame == 0x40) {
                work->stateFrame = 0;
            }
            break;
        case 7:
            winSpriteSetDisp(work->cellFront[1][index], 0);
            winSpriteSetDisp(work->cellBack[1][index], 0);
        case 8:
            work->cell[1][index]->color =
                menuCardELerpColor(0xFFFFFF00, 0xFFFFFFFF, work->stateFrame * 0.0625f);
            if (work->stateFrame == 16) {
                work->state = 9;
            }
            break;
        case 9:
            winSpriteSetDisp(work->cellFront[1][index], 0);
            winSpriteSetDisp(work->cellBack[1][index], 0);
            winSpriteSetDisp(work->cell[1][index], 0);
            break;
        }
    }

    other = windowSearchID(0xE8);
    if (other != NULL) {
        for (sprite = other->sprites; sprite != NULL; sprite = sprite->next) {
            if (sprite->flags & 1) {
                if (work->transitionActive == 0 && work->state != 0) {
                    sprite->color = 0xFFFFFFAA;
                } else {
                    sprite->color = 0xFFFFFFFF;
                }
            }
        }
    }
}
#pragma pop
#endif

#if defined(MENU_CARDE_MATRIX_TU)
/* 0x8007FD64 | size: 0x58
 * qsort comparator for MenuCardEEntry* elements. */
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

/* 0x8007FDBC | size: 0x554 */
#pragma push
/* Both allocations assert on the same source line, so they share a helper. */
void* fn_8007FDBC(void* window, const void* title) {
    u8* ctx;
    u16* rowIds0;
    u8* rowItems;
    u16* rowIds1;
    u16* rowIds2;
    u16* ids0;
    u8* rowCtx;
    u16* ids1;
    u16* ids2;
    s32 count;
    s32 i;
    s32 j;

    ctx = menuCardE_Alloc(0x4E8);

    if (title != 0) {
        GScharLenCpy(ctx, title, 0x50);
        *(u16*)(ctx + 0x9E) = 0;
    } else {
        *(u16*)ctx = 0;
    }

    if (CARDE_CTX_U32(ctx, 0xB0) != 0) {
        menuCardE_Free((void*)CARDE_CTX_U32(ctx, 0xB0));
        CARDE_CTX_U32(ctx, 0xB0) = 0;
    }

    count = CARDE_CTX_U32(ctx, 0xAC) = fn_80083BF8(0);
    if (count != 0) {
        menuCardE_FillEntries(ctx, count);
    }

    CARDE_CTX_U32(ctx, 0xA4) = (s32)CARDE_CTX_U32(ctx, 0xAC) != 0 ? 0 : -1;

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

    rowCtx = ctx;
    ids0 = CARDE_MATRIX_IDS(0);
    ids1 = CARDE_MATRIX_IDS(1);
    ids2 = CARDE_MATRIX_IDS(2);
    for (i = 0; i < 0x24; i++) {
        rowIds0 = ids0;
        rowItems = rowCtx;
        rowIds1 = ids1;
        rowIds2 = ids2;

        for (j = 0; j < 2; j++) {
            CARDE_CTX_U32(rowItems, 0x170) =
                (u32)windowSearchItemID(window, *rowIds0);
            CARDE_CTX_U32(rowItems, 0x3B0) =
                (u32)windowSearchItemID(window, *rowIds1);
            CARDE_CTX_U32(rowItems, 0x290) =
                (u32)windowSearchItemID(window, *rowIds2);
            rowIds0 += 0x24;
            rowItems += 0x90;
            rowIds1 += 0x24;
            rowIds2 += 0x24;
        }
        ids0++;
        rowCtx += 4;
        ids1++;
        ids2++;
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
#pragma pop
#endif
