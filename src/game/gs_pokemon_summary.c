/**
 * @file gs_pokemon_summary.c
 * @brief Bag (item pocket) menu: pocket pages, item lists and cursor marquee.
 *
 * Address range: 0x80015000 - 0x800181C4 (~30 functions)
 *
 * Despite the file name, this is the bag menu, not a Pokemon summary screen:
 * the 6-entry page table lbl_80266918 has one entry per pocket (data source,
 * colours, drawPage callback, cursor/title message ids), the list pages read
 * the player's items through heroHizukiItemGetItemAryPtr, and the selected
 * row gets the same 45-dot chasing marquee as the shop menu
 * (menuPocketDrawMarqueeFrame / menuPocketDrawMarqueeDot). menuPocket2NumCtrl
 * follows this range.
 *
 * NOTE: the per-function roles in the list below were early guesses and are
 * mostly wrong (e.g. the "Summary" page names); prefer the names in
 * config/GC6E01/symbols.txt.
 *
 * Key functions:
 *   fn_8001501C  GSpokeSummary_DrawLabel      -- 0x34 bytes, render text label
 *   fn_80015050  GSpokeSummary_DrawHandler     -- 0x94 bytes, invoke page draw handler
 *   fn_800150E4  GSpokeSummary_DrawPage        -- 0x290 bytes, main page renderer
 *   menuPocketDrawNameList                    -- 0x23C bytes, names-only item-list page (pocket 1)
 *   fn_800155B0  GSpokeSummary_DrawStatsPage    -- 0x40C bytes, stat hexagon page
 *   fn_800159BC  GSpokeSummary_DrawRibbonPage   -- 0x480 bytes, ribbon collection
 *   menuPocketDrawItemList                    -- 0x374 bytes, item-list page draw (pockets 4/5)
 *   fn_800161B0  GSpokeSummary_PageTransition   -- 0x198 bytes, page flip animation
 *   fn_80016348  GSpokeSummary_UpdateCursor     -- 0x188 bytes, cursor movement
 *   fn_800164D0  GSpokeSummary_MoveDetail       -- 0x148 bytes, move detail popup
 *   fn_80016618  GSpokeSummary_GetPageCount     -- 0xA4 bytes, count available pages
 *   fn_800166BC  GSpokeSummary_SetupPage        -- 0x114 bytes, initialize page data
 *   menuPocketDrawMarqueeDot                  -- 0x2EC bytes, one dot of the cursor marquee
 *   menuPocketDrawMarqueeFrame                -- 0x458 bytes, cursor placement + 45-dot marquee
 *   fn_80016F14  GSpokeSummary_DrawExpBar       -- 0x114 bytes, draw EXP bar
 *   fn_80017028  GSpokeSummary_DrawHPBar        -- 0x73C bytes, draw HP bar with color
 *   fn_80017764  GSpokeSummary_GetBarColor      -- 0x2C bytes, HP color threshold
 *   fn_80017790  GSpokeSummary_FormatLevel      -- 0xD8 bytes, format "Lv.XX" string
 *   fn_80017868  GSpokeSummary_FormatHP         -- 0x84 bytes, format "HP/MaxHP"
 *   fn_800178EC  GSpokeSummary_GetGenderChar    -- 0x28 bytes, get gender symbol
 *   fn_80017914  GSpokeSummary_GetShinyIcon     -- 0x28 bytes, get shiny star icon
 *   fn_8001793C  GSpokeSummary_DrawShadowGauge  -- 0x54 bytes, draw heart gauge
 *   fn_80017990  GSpokeSummary_AnimateGauge     -- 0x7C bytes, gauge fill animation
 *   fn_80017A0C  GSpokeSummary_DrawPageDots     -- 0x2AC bytes, page indicator dots
 *   fn_80017CB8  GSpokeSummary_DrawBackground   -- 0x1D4 bytes, background gradient
 *   fn_80017E8C  GSpokeSummary_Init             -- 0x338 bytes, full initialization
 *
 * The page drawing system uses a function pointer table at sSummaryPageEntries.
 * Each page entry is 0x4C bytes and contains:
 *   +0x04: Pokemon index for data source (s32, -1 = use party default)
 *   +0x18: Draw handler function pointer
 *   +0x1C: Data source message ID (u16)
 *
 * fn_800150E4 (GSpokeSummary_DrawPage) iterates over the Pokemon's
 * data fields and renders them using heroItemGetItemKindToItemAryPtr (get Pokemon field data)
 * and fn_801429E8 (check if field is valid/non-empty). It calculates
 * scroll positions using floating point arithmetic with stick input
 * values from lbl_8047A2D0 (analog stick deflection).
 *
 * SDA globals:
 *   lbl_8047A2BC: Current summary screen mode (s32)
 *   lbl_8047A2C8: Analog stick integration value (s32)
 *   lbl_8047A2D0: Stick deflection (float)
 *   lbl_8047A2D8: Pokemon data cache pointer
 *   lbl_8047A2DC: Current label text resource
 *   lbl_8047A2F8: Pokemon data table pointer
 *
 * Rodata:
 *   sSummaryPageEntries: Page handler/data table (used by DrawHandler at +0x18)
 *   lbl_8047B748: Float constant 0.0f (stick deadzone)
 *   lbl_8047B8B8: Float constant for int-to-float conversion (0x43300000)
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Pokemon data access */
extern void* heroItemGetItemKindToItemAryPtr(void* hero, u8 kind, u16* count, u16* total, s32, s32);
extern void* heroHizukiItemGetItemAryPtr(void* hero, u16* count, u16* total, s32, s32);
extern u8    fn_801429E8(void* fieldData);    /* Check field validity */
extern u16   itemBiosGetNum(void* fieldData);    /* Get field value */
extern void* itemDataBiosGetPtr(u16 itemId);          /* Item data record */
extern u16   itemDataBiosGetPrice(void);               /* Get display field count */

/* Text rendering */
extern void  msgctrlSetValue(s32 paramId, s32 value);
extern u32   GSmsgGetRect(s32 resourceId);
extern void  fn_800FB680(s32 x, s32 y, s32 flags, u32 color);

/* Math/rendering helpers */
extern f64   sin(f64);
extern f64   cos(f64);
extern void  fn_800E0CA0(f32 angle);          /* Set camera rotation */
extern void  GSlerpGetLinearInterpolationVector(void* outVec, void* posA, void* posB); /* Vector subtract */
extern void  winMsgOpenWithSE(s32 p1, void* data, s32 p3, s32 p4, u8 p5);

/* =========================================================================
 * SDA globals
 * ========================================================================= */

extern s32   gSummaryMode;        /* lbl_8047A2BC */
extern void* gPokeDataCache;      /* lbl_8047A2D8 */
extern void* gPokeDataTable;      /* lbl_8047A2F8 */
extern s32   gSummaryLabel;       /* lbl_8047A2DC */
extern f32   gStickDeflection;    /* lbl_8047A2D0 */

/* ===== Phase 2 recovery stubs ===== */

/*
 * Source islands: a wrapper that defines one of these builds only that
 * address range, so the exact islands can link on their own.
 */
#if defined(GS_POKEMON_SUMMARY_80017CB8_ONLY) || \
    defined(GS_POKEMON_SUMMARY_800161B0_ONLY) || \
    defined(GS_POKEMON_SUMMARY_8001793C_ONLY) || \
    defined(GS_POKEMON_SUMMARY_80017E8C_ONLY) || \
    defined(GS_POKEMON_SUMMARY_800166BC_ONLY) || \
    defined(GS_POKEMON_SUMMARY_80016ABC_ONLY)
#define GS_POKEMON_SUMMARY_ISLAND
#endif

/* fn_8001501C - 0x8001501C | size: 0x34 */
extern u32 lbl_8047A2DC;
#if !defined(GS_POKEMON_SUMMARY_ISLAND)
#if 0
asm void fn_8001501C(void) {
#include "src/game/gs_pokemon_summary_fn_8001501C.inc"
}
#else
s32 fn_8001501C(void) {
    fn_800FB680(0, 0, -1, lbl_8047A2DC);
    return 0;
}
#endif
#endif /* !GS_POKEMON_SUMMARY_ISLAND */

/* fn_80015050 - 0x80015050 | size: 0x94 */
extern u32 cursorBiosGetPos(u32 a);
extern const u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918

/* One option of a page's item menu (0x0C bytes). */
typedef struct SummaryMenuItem {
    u32 id;
    u32 arg;
    u16 flags;               /* 1 hidden, 2 needs battle use, 4 grey out, 8 not for 0x219 */
} SummaryMenuItem;

typedef struct SummaryMenuList {
    SummaryMenuItem* items;
    s32 count;
} SummaryMenuList;

typedef s32 (*SummaryPageDrawFn)(s32 x, s32 pageIndex, u16* messageParam);

typedef struct SummaryPageEntry {
    u8 displayColor[3];      /* 0x00, copied to output bytes 0x64-0x66 */
    u8 unk_03;
    s32 dataSource;          /* 0x04, -1 uses default party source */
    u8 unk_08[4];
    s32 colorMatchId;        /* 0x0C, used by fn_80017790 */
    s32 gaugeMatchId;        /* 0x10, compared with output species/id */
    SummaryPageDrawFn drawPage; /* 0x14, draws the page body at x */
    void* drawHandler;       /* 0x18 */
    u32 messageId;           /* 0x1C, cursor id of the page's list */
    u32 titleId;             /* 0x20, page title message */
    SummaryMenuList menus[5]; /* 0x24, indexed by summary mode */
} SummaryPageEntry;
#define SUMMARY_PAGES ((const SummaryPageEntry*)sSummaryPageEntries)
typedef char SummaryPageEntry_size_check[sizeof(SummaryPageEntry) == 0x4C ? 1 : -1];

typedef struct SummaryPageContext {
    u8 unk_00[0x95];
    s8 pageIndex;            /* 0x95, selected summary page/table slot */
} SummaryPageContext;

typedef struct SummaryDrawItem {
    u8 unk_00[0x06];
    s16 speciesId;           /* 0x06, compared against table ids */
    u8 unk_08[0x5C];
    u8 color[4];             /* 0x64-0x67, RGB + alpha */
} SummaryDrawItem;

#define SUMMARY_ENTRY_STRIDE 0x4C
#define SUMMARY_ENTRY_RAW(index) ((u8*)sSummaryPageEntries + (s32)(index) * SUMMARY_ENTRY_STRIDE)
#define SUMMARY_ENTRY_FIELD(entry) (*(s32*)((u8*)(entry) + 0x04))
#define SUMMARY_ENTRY_LABEL(entry) (*(u32*)((u8*)(entry) + 0x1C))
#define SUMMARY_ENTRY_PAGE_FN(entry) (*(SummaryPageDrawFn*)((u8*)(entry) + 0x14))
#define SUMMARY_ENTRY_ITEM_FN(entry) (*(DrawHandlerFn*)((u8*)(entry) + 0x18))
#define SUMMARY_ITEM_S16(item, offset) (*(s16*)((u8*)(item) + (offset)))
#define SUMMARY_ITEM_U16(item, offset) (*(u16*)((u8*)(item) + (offset)))
#define SUMMARY_ITEM_U8(item, offset) (*(u8*)((u8*)(item) + (offset)))
#define SUMMARY_CTX_S8(ctx, offset) ((s32)(s8)*(u8*)((u8*)(ctx) + (offset)))
#define SUMMARY_F32(label) (*(f32*)&(label))
#define SUMMARY_STORE_F32(label, value) (*(f32*)&(label) = (value))

#define SUMMARY_PAGE_INDEX(ctx) ((s32)((ctx)->pageIndex))
#define SUMMARY_PAGE_INDEX_VOLATILE(ctx) ((s32)*(volatile s8*)&(ctx)->pageIndex)
/*
 * Transitional accessors: direct SummaryPageEntry array indexing changes MWCC
 * register allocation for fn_80017990. Keep the byte-match spelling isolated
 * here until the rodata table can be typed without losing the match.
 */
#define SUMMARY_PAGE_ENTRY_AT(index) \
    (*(SummaryPageEntry*)((u8*)sSummaryPageEntries + (index) * sizeof(SummaryPageEntry)))
/*
 * A page's dataSource addressed from the table's first dataSource field.
 * Retail computes the field base before the row offset in some callers.
 */
/* RULE-EXCEPTION(user-approved): address-order macro chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
#define SUMMARY_PAGE_DATA_SOURCE(index) \
    (*(const s32*)((const u8*)&SUMMARY_PAGES->dataSource + (index) * sizeof(SummaryPageEntry)))
#define SUMMARY_PAGE_DISPLAY_COLOR(index, component) \
    (sSummaryPageEntries[(index) * sizeof(SummaryPageEntry) + (component)])

typedef s32 (*DrawHandlerFn)(u8*, u8*, u16*);
typedef s32 (*SummaryMenuCallback)(s32, s32, s32*);

extern u16 itemBiosGetItemDataId(void*);
extern u32 itemDataBiosGetName(void);
extern u32 itemDataBiosGetKinomiNo(u16);
extern u32 itemDataBiosGetWazaMachineNo(void* itemData);
extern u32 itemDataBiosGetHidenMachineNo(void* itemData);
extern u32 itemDataBiosGetWazaIDByWazaMachineNo(u32);
extern u32 itemDataBiosGetDoc(void* itemData);
extern u32 fn_80143F9C(void* itemData);
extern u32 itemDataBiosGetBattleUseFunc(void);
extern void* wazaDataBiosGetPtr(u16);
extern u32 wazaDataBiosGetName(void* wazaData);
extern u8* windowGetKeyInfo(void);
extern void cursorBiosSetPos(u32, u16*);
extern void fn_800FE38C(s32, s32, s32, s32);
extern void fn_800FE35C(void);
extern void fn_800FE6D0(s32, s32);
extern void spriteSetEnv(void);
extern void windowDrawSprite2(s32, s32, s32, s32, u32, s32, s32, s32);
extern void winSeqSetMenu(s32, s32);
extern void soundStart(s32, s32, s32, s32);
extern void winMsgOpen(s32, s32, s32, s32);
extern void* windowGetActiveID(void);
extern s32 menuOpenCustom(s32, void*, ...);
extern void menuClose(s32);
extern void menuCloseSync(s32, s32);
extern void winMsgClose(s32);
extern u32 pcboxGetItemCapacity(s32, u16);
extern void pcboxDelItem(s32, u16, u16);
extern void fn_8012959C(void*, u16, u16, s16);
extern void heroItemDecItemDataId(void*, u16, u16, s16);
extern void fn_80129514(void*, u16, u16);
extern void fn_80129948(void*, u8, u16, u16);
extern void* lbl_8047A2F8;

static inline const SummaryPageEntry* SummaryGetPage(s32 pageIndex) {
    const SummaryPageEntry* entry = SUMMARY_PAGES;

    entry += pageIndex;
    return entry;
}

/* Combined cursor index (scroll offset + row) of a page's list cursor. */
static inline s32 SummaryGetCursorIndex(s32 pageIndex) {
    SummaryPageEntry* entry = (SummaryPageEntry*)sSummaryPageEntries;
    u8 pos[2];
    entry += pageIndex;
    *(u16*)pos = (u16)(cursorBiosGetPos((u16)entry->messageId) >> 16);
    return (s32)(s8)pos[0] + (s32)(s8)pos[1];
}

/*
 * Item-pocket list helpers. `kind` is a page's dataSource: an item kind for
 * heroItemGetItemKindToItemAryPtr, or negative for the Hizuki list.
 */
static inline void* SummaryGetItemList(s32 kind, u16* count) {
    if (kind >= 0) {
        return heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, kind, count, 0, 0, 0);
    }
    return heroHizukiItemGetItemAryPtr(lbl_8047A2F8, count, 0, 0, 0);
}

/* Item id of the index-th valid entry in the list, or 0. */
static inline u16 SummaryListGetItemId(s32 kind, s32 index) {
    void* list;
    u16 count;
    s32 i;
    s32 visibleIndex;

    if (kind >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, kind, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }

    visibleIndex = -1;
    for (i = 0; i < count; i++, list = (u8*)list + 4) {
        if (fn_801429E8(list) != 0) {
            visibleIndex++;
            if (visibleIndex >= index) {
                return itemBiosGetItemDataId(list);
            }
        }
    }
    return 0;
}

/* Quantity of the index-th valid entry in the list, or 0. */
static inline u16 SummaryListGetNum(s32 kind, s32 index) {
    void* list;
    u16 count;
    s32 i;
    s32 visibleIndex;

    if (kind >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, kind, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }

    visibleIndex = -1;
    for (i = 0; i < count; i++, list = (u8*)list + 4) {
        if (fn_801429E8(list) != 0) {
            visibleIndex++;
            if (visibleIndex >= index) {
                return itemBiosGetNum(list);
            }
        }
    }
    return 0;
}

/* Number of valid entries in the list. */
static inline s32 SummaryListCount(s32 kind) {
    void* list;
    u16 count;
    s32 i;
    s32 validCount;

    if (kind >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, kind, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    for (i = 0; i < count; i++, list = (u8*)list + 4) {
        if (fn_801429E8(list) != 0) {
            validCount++;
        }
    }
    return validCount;
}

/* Total quantity held in an item pocket. */
static inline s32 SummaryPocketTotal(s32 kind) {
    u16 total;

    heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, kind, 0, &total, 0, 0);
    return total;
}

/* Item id under a page's list cursor index. */
static inline u16 SummaryGetItemDataId(s32 pageIndex, s32 index) {
    const SummaryPageEntry* entry = SUMMARY_PAGES;

    entry += pageIndex;
    return SummaryListGetItemId(entry->dataSource, index);
}

/* Number of valid entries in a page's item list. */
static inline s32 SummaryCountItems(s32 pageIndex) {
    const SummaryPageEntry* entry = SUMMARY_PAGES;

    entry += pageIndex;
    return SummaryListCount(entry->dataSource);
}

/*
 * A page's list entries, addressed by page index: the item id and quantity
 * of the index-th valid entry, and the pocket total. The page draws reach the
 * list helpers through these, one inline level down, which gives retail's
 * stack slots for the helpers' count locals.
 */
static inline u16 SummaryPageItemId(s32 pageIndex, s32 index) {
    return SummaryListGetItemId(SUMMARY_PAGES[pageIndex].dataSource, index);
}

static inline u16 SummaryPageItemNum(s32 pageIndex, s32 index) {
    return SummaryListGetNum(SUMMARY_PAGES[pageIndex].dataSource, index);
}

static inline s32 SummaryPagePocketTotal(s32 pageIndex) {
    return SummaryPocketTotal(SUMMARY_PAGES[pageIndex].dataSource);
}

#if !defined(GS_POKEMON_SUMMARY_ISLAND)
#if 0
asm void fn_80015050(void) {
#include "src/game/gs_pokemon_summary_fn_80015050.inc"
}
#else
s32 fn_80015050(u8* src, u8* param) {
    DrawHandlerFn fp;
    u16 tmp;
    u8* entry = (u8*)sSummaryPageEntries;
    entry += (s32)(s8)src[0x95] * 0x4C;
    fp = *(DrawHandlerFn*)(entry + 0x18);
    if (fp != NULL) {
        tmp = cursorBiosGetPos((u16)*(u32*)(entry + 0x1C)) >> 16;
        return fp(src, param, &tmp);
    }
    return 0;
}
#endif

/* fn_800150E4 - 0x800150E4 | size: 0x290 */
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_800150E4(void) {
#include "src/game/gs_pokemon_summary_fn_800150E4.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800150E4(s32 x, s32 pageIndex, u16* packedRange) {
    s32 validCount;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleIndex;
    s32 lineLimit;
    s32 scroll;
    s32 y;
    s32 textX;

    displayLine = 0;
    lineLimit = 8;
    scroll = 0;
    validCount = SummaryCountItems(pageIndex);
    itemIndex = ((s8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            lineLimit = 9;
        }
        scroll = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    for (visibleIndex = displayLine; visibleIndex < lineLimit && itemIndex < validCount;
         displayLine++, visibleIndex++, itemIndex++) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scroll;
            textX = x;
            if (itemIndex + 1 < 10) {
                msgctrlSetValue(0x34, 0);
                textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            }
            msgctrlSetValue(0x34, itemIndex + 1);
            fn_800FB680(textX, y, -1, 0xCA);
            textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            itemDataBiosGetPtr((u16)SummaryPageItemId(pageIndex, itemIndex));
            fn_800FB680(textX + 10, y, -1, itemDataBiosGetName());
        }
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scroll, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* menuPocketDrawNameList - 0x80015374 | size: 0x23c */
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_80015374(void) {
#include "src/game/gs_pokemon_summary_fn_80015374.inc"
}
#else
#pragma push
#pragma peephole off
s32 menuPocketDrawNameList(s32 x, s32 pageIndex, u16* packedRange) {
    s32 validCount;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleIndex;
    s32 lineLimit;
    s32 scroll;
    s32 y;
    u32 name;

    displayLine = 0;
    lineLimit = 8;
    scroll = 0;
    validCount = SummaryCountItems(pageIndex);
    itemIndex = ((s8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            lineLimit = 9;
        }
        scroll = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    for (visibleIndex = displayLine; visibleIndex < lineLimit && itemIndex < validCount;
         displayLine++, visibleIndex++, itemIndex++) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scroll;
            itemDataBiosGetPtr((u16)SummaryPageItemId(pageIndex, itemIndex));
            name = itemDataBiosGetName();
            if (name != 0) {
                fn_800FB680(x, y, -1, name);
            }
        }
    }

    if (visibleIndex < lineLimit) {
        fn_800FB680(x, visibleIndex * 0x1F - scroll, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800155B0 - 0x800155B0 | size: 0x40c */
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_800155B0(void) {
#include "src/game/gs_pokemon_summary_fn_800155B0.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800155B0(s32 x, s32 pageIndex, u16* packedRange) {
    s32 validCount;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleIndex;
    s32 lineLimit;
    s32 scroll;
    s32 y;
    s32 textX;
    u16 itemId;
    u8 berryNo;
    s16 width;

    displayLine = 0;
    lineLimit = 8;
    scroll = 0;
    validCount = SummaryCountItems(pageIndex);
    itemIndex = ((s8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            lineLimit = 9;
        }
        scroll = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    for (visibleIndex = displayLine; visibleIndex < lineLimit && itemIndex < validCount;
         displayLine++, visibleIndex++, itemIndex++) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scroll;
            fn_800FB680(x, y, -1, 0x2AFF);
            textX = x + (u16)(GSmsgGetRect(0x2AFF) >> 16);

            itemId = SummaryPageItemId(pageIndex, itemIndex);
            berryNo = itemDataBiosGetKinomiNo(itemId) + 1;
            if (berryNo < 10) {
                msgctrlSetValue(0x34, 0);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            }
            msgctrlSetValue(0x34, berryNo);
            fn_800FB680(textX, y, -1, 0xCA);
            textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            itemDataBiosGetPtr((u16)itemId);
            fn_800FB680(textX + 9, y, -1, itemDataBiosGetName());

            msgctrlSetValue(0x34, SummaryPagePocketTotal(pageIndex));
            width = (s16)(GSmsgGetRect(0xCA) >> 16);
            width += (s16)(GSmsgGetRect(0x12E) >> 16);
            fn_800FB680(x + 0x11A - width, y, -1, 0x12E);

            msgctrlSetValue(0x34, SummaryPageItemNum(pageIndex, itemIndex));
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
        }
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scroll, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800159BC - 0x800159BC | size: 0x480 */
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_800159BC(void) {
#include "src/game/gs_pokemon_summary_fn_800159BC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800159BC(s32 x, s32 pageIndex, u16* packedRange) {
    s32 validCount;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleIndex;
    s32 lineLimit;
    s32 scroll;
    s32 y;
    s32 textX;
    void* itemData;
    u8 machineNo;
    s32 tmNo;
    u32 hidenNo;
    u8 hmNo;
    u32 labelId;
    s16 width;

    displayLine = 0;
    lineLimit = 8;
    scroll = 0;
    validCount = SummaryCountItems(pageIndex);
    itemIndex = ((s8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            lineLimit = 9;
        }
        scroll = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    for (visibleIndex = displayLine; visibleIndex < lineLimit && itemIndex < validCount;
         displayLine++, visibleIndex++, itemIndex++) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scroll;
            itemData = itemDataBiosGetPtr(SummaryPageItemId(pageIndex, itemIndex));
            machineNo = itemDataBiosGetWazaMachineNo(itemData);
            tmNo = machineNo + 1;
            hidenNo = itemDataBiosGetHidenMachineNo(itemData);
            hmNo = hidenNo + 1;
            if ((u8)hidenNo != 0xFF) {
                labelId = 0x2B00;
            } else {
                labelId = 0x2AFF;
            }
            fn_800FB680(x, y, -1, labelId);
            textX = x + (u16)(GSmsgGetRect(labelId) >> 16);
            if ((u8)hidenNo != 0xFF) {
                msgctrlSetValue(0x34, hmNo);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            } else {
                if (tmNo < 10) {
                    msgctrlSetValue(0x34, 0);
                    fn_800FB680(textX, y, -1, 0xCA);
                    textX += (u16)(GSmsgGetRect(0xCA) >> 16);
                }
                msgctrlSetValue(0x34, tmNo);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (u16)(GSmsgGetRect(0xCA) >> 16);
            }
            fn_800FB680(textX + 9, y, -1,
                        wazaDataBiosGetName(wazaDataBiosGetPtr(itemDataBiosGetWazaIDByWazaMachineNo(machineNo))));

            msgctrlSetValue(0x34, SummaryPagePocketTotal(pageIndex));
            width = (s16)(GSmsgGetRect(0xCA) >> 16);
            width += (s16)(GSmsgGetRect(0x12E) >> 16);
            fn_800FB680(x + 0x11A - width, y, -1, 0x12E);

            msgctrlSetValue(0x34, SummaryPageItemNum(pageIndex, itemIndex));
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
        }
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scroll, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* menuPocketDrawItemList - 0x80015E3C | size: 0x374 */
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_80015E3C(void) {
#include "src/game/gs_pokemon_summary_fn_80015E3C.inc"
}
#else
#pragma push
#pragma peephole off
s32 menuPocketDrawItemList(s32 x, s32 pageIndex, u16* packedRange) {
    s32 validCount;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleIndex;
    s32 lineLimit;
    s32 scroll;
    s32 y;
    u32 name;
    s16 width;

    displayLine = 0;
    lineLimit = 8;
    scroll = 0;
    validCount = SummaryCountItems(pageIndex);
    itemIndex = ((s8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            lineLimit = 9;
        }
        scroll = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    for (visibleIndex = displayLine; visibleIndex < lineLimit && itemIndex < validCount;
         displayLine++, visibleIndex++, itemIndex++) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scroll;
            itemDataBiosGetPtr((u16)SummaryPageItemId(pageIndex, itemIndex));
            name = itemDataBiosGetName();
            if (name != 0) {
                fn_800FB680(x, y, -1, name);
            }

            msgctrlSetValue(0x34, SummaryPagePocketTotal(pageIndex));
            width = (s16)(GSmsgGetRect(0xCA) >> 16);
            width += (s16)(GSmsgGetRect(0x12E) >> 16);
            fn_800FB680(x + 0x11A - width, y, -1, 0x12E);

            msgctrlSetValue(0x34, SummaryPageItemNum(pageIndex, itemIndex));
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
        }
    }

    if (visibleIndex < lineLimit) {
        fn_800FB680(x, visibleIndex * 0x1F - scroll, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

#endif /* !GS_POKEMON_SUMMARY_ISLAND */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_800161B0_ONLY)
/* fn_800161B0 - 0x800161B0 | size: 0x198 */
extern u8 lbl_802EF0A8[];

/* Screen layout record: 0x1C-byte parts, then the summary window rect. */
typedef struct SummaryLayoutPart {
    s16 unk_00;
    s16 x;
    s16 y;
    u8 unk_06[0x16];
} SummaryLayoutPart;

typedef struct SummaryLayout {
    SummaryLayoutPart parts[0x222];
    u8 unk_3BB8[2];
    s16 originX;   /* 0x3BBA */
    s16 originY;   /* 0x3BBC */
    s16 width;     /* 0x3BBE */
    s16 height;    /* 0x3BC0 */
} SummaryLayout;
extern u32 lbl_8047A2D4;
extern const f32 lbl_8047B748;
extern const f32 lbl_8047B750;
#if 0
asm void fn_800161B0(void) {
#include "src/game/gs_pokemon_summary_fn_800161B0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_800161B0(SummaryPageContext* ctx, u8* item) {
    SummaryPageDrawFn drawPage;
    s32 neighborIndex;
    f32 neighborX;
    u16 pos;

    {
        SummaryLayout* layout = (SummaryLayout*)lbl_802EF0A8;
        fn_800FE38C(layout->originX - layout->parts[*(s16*)(item + 0x06)].x,
                    layout->originY - layout->parts[*(s16*)(item + 0x06)].y,
                    layout->width, layout->height);
    }

    pos = cursorBiosGetPos((u16)SummaryGetPage(ctx->pageIndex)->messageId) >> 16;
    drawPage = SummaryGetPage(SUMMARY_CTX_S8(ctx, 0x95))->drawPage;
    if (drawPage != NULL) {
        drawPage((s32)SUMMARY_F32(lbl_8047A2D4), SUMMARY_CTX_S8(ctx, 0x95), &pos);
    }

    if (lbl_8047B748 != SUMMARY_F32(lbl_8047A2D4)) {
        if (SUMMARY_F32(lbl_8047A2D4) > lbl_8047B748) {
            neighborIndex = ctx->pageIndex + 1;
            if (neighborIndex >= 6) {
                neighborIndex = 1;
            }
            neighborX = SUMMARY_F32(lbl_8047A2D4) - lbl_8047B750;
        } else {
            neighborIndex = ctx->pageIndex - 1;
            if (neighborIndex < 1) {
                neighborIndex = 5;
            }
            neighborX = lbl_8047B750 + SUMMARY_F32(lbl_8047A2D4);
        }

        drawPage = SUMMARY_PAGES[neighborIndex].drawPage;
        if (drawPage != NULL) {
            pos = cursorBiosGetPos((u16)SUMMARY_PAGES[neighborIndex].messageId) >> 16;
            drawPage((s32)neighborX, neighborIndex, &pos);
        }
    }

    fn_800FE35C();
    return 0;
}
#pragma pop
#endif

/* fn_80016348 - 0x80016348 | size: 0x188 */
extern u32 lbl_8047A2D4;
extern const f32 lbl_8047B748;
extern const f32 lbl_8047B750;
#if 0
asm void fn_80016348(void) {
#include "src/game/gs_pokemon_summary_fn_80016348.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80016348(SummaryPageContext* ctx, u8* item) {
    s32 halfWidth;
    u32 titleId;
    s32 halfText;
    s32 neighborIndex;
    f32 neighborX;

    fn_800FE38C(0, 0, *(s16*)(item + 0x54), *(s16*)(item + 0x56));

    halfWidth = *(s16*)(item + 0x54) / 2;
    titleId = SummaryGetPage(ctx->pageIndex)->titleId;
    halfText = (s16)(GSmsgGetRect(titleId) >> 16) / 2;
    fn_800FB680((s32)SUMMARY_F32(lbl_8047A2D4) + (halfWidth - halfText), 0, -1, titleId);

    if (lbl_8047B748 != SUMMARY_F32(lbl_8047A2D4)) {
        if (SUMMARY_F32(lbl_8047A2D4) > lbl_8047B748) {
            neighborIndex = ctx->pageIndex + 1;
            if (neighborIndex >= 6) {
                neighborIndex = 1;
            }
            neighborX = SUMMARY_F32(lbl_8047A2D4) - lbl_8047B750;
        } else {
            neighborIndex = ctx->pageIndex - 1;
            if (neighborIndex < 1) {
                neighborIndex = 5;
            }
            neighborX = lbl_8047B750 + SUMMARY_F32(lbl_8047A2D4);
        }

        titleId = SummaryGetPage(neighborIndex)->titleId;
        halfText = (s16)(GSmsgGetRect(titleId) >> 16) / 2;
        fn_800FB680((s32)neighborX + (halfWidth - halfText), 0, -1, titleId);
    }

    fn_800FE35C();
    return 0;
}
#pragma pop
#endif

/* fn_800164D0 - 0x800164D0 | size: 0x148 */
extern void* lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern const f32 lbl_8047B744;
extern u32 lbl_8047A2C4;
extern const f32 lbl_8047B740;
#if 0
asm void fn_800164D0(void) {
#include "src/game/gs_pokemon_summary_fn_800164D0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_800164D0(SummaryPageContext* ctx, u8* item) {
    s8 pos[2];
    s32 lastRow;

    *(u16*)pos = cursorBiosGetPos((u16)SummaryGetPage(ctx->pageIndex)->messageId) >> 16;
    lastRow = pos[0] + 8;
    if (lastRow < SummaryListCount(SUMMARY_PAGE_DATA_SOURCE(ctx->pageIndex)) + 1 &&
        (s32)lbl_8047A2D8 == -1) {
        item[0x67] = lbl_8047B740 * (lbl_8047B744 - SUMMARY_F32(lbl_8047A2C4));
    } else {
        item[0x67] = 0;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80016618 - 0x80016618 | size: 0xa4 */
extern u32 lbl_8047A2D8;
extern const f32 lbl_8047B744;
extern u32 lbl_8047A2C4;
extern const f32 lbl_8047B740;
#if 0
asm void fn_80016618(void) {
#include "src/game/gs_pokemon_summary_fn_80016618.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80016618(u8* src, u8* dst) {
    u8* entry;
    u16 tmp;
    entry = (u8*)sSummaryPageEntries;
    entry = entry + (s32)(SUMMARY_CTX_S8(src, 0x95)) * SUMMARY_ENTRY_STRIDE;
    tmp = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
    if ((s32)(s8)*(u8*)&tmp > 0 && (s32)lbl_8047A2D8 == -1) {
        dst[0x67] = *(f32*)&lbl_8047B740 * (*(f32*)&lbl_8047B744 - *(f32*)&lbl_8047A2C4);
    } else {
        dst[0x67] = 0;
    }
    return 0;
}
#pragma pop
#endif

#endif /* 0x800161B0 - 0x800166BC */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_800166BC_ONLY)
/* fn_800166BC - 0x800166BC | size: 0x114 */
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2D0;
extern const f32 lbl_8047B768;
extern const f32 lbl_8047B754;
extern const f32 lbl_8047B758;
extern const f32 lbl_8047B75C;
extern const f32 lbl_8047B760;
#if 0
asm void fn_800166BC(void) {
#include "src/game/gs_pokemon_summary_fn_800166BC.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_800166BC(SummaryPageContext* ctx, u8* item) {
    s8 pos[2];
    s32 row;
    f32 angle;

    *(u16*)pos = cursorBiosGetPos((u16)SummaryGetPage(ctx->pageIndex)->messageId) >> 16;
    row = pos[1];
    *(s16*)(item + 0x52) = row * 0x1F + 0x95;
    if ((s32)lbl_8047A2C8 == 0) {
        *(s16*)(item + 0x52) += (s16)SUMMARY_F32(lbl_8047A2D0);
    }

    angle = lbl_8047B754 * (f32)((s32)SUMMARY_F32(lbl_8047A2D0) + (row + pos[0]) * 0x1F);
    while (angle > 3.14159274f) {
        angle -= 6.28318548f;
    }
    while (angle < -3.14159274f) {
        angle += 6.28318548f;
    }
    *(f32*)(item + 0x70) = angle;
    return 0;
}
#pragma pop
#endif

#endif /* 0x800166BC - 0x800167D0 */

#if !defined(GS_POKEMON_SUMMARY_ISLAND)
/* menuPocketDrawMarqueeDot - 0x800167D0 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern const f32 lbl_8047B748;
extern const f32 lbl_8047B744;
extern const f32 lbl_8047B768;
extern const f32 lbl_8047B75C;
extern const f32 lbl_8047B770;
extern const f32 lbl_8047B774;
extern const f32 lbl_8047B778;
#if 0
asm void fn_800167D0(void) {
#include "src/game/gs_pokemon_summary_fn_800167D0.inc"
}
#else
#pragma push
#pragma peephole off
void menuPocketDrawMarqueeDot(u8* owner, u8* item, f32 phase, u16 iconId, u8 alpha) {
    f32 stops[5];
    f32 width;
    f32 height;
    f32 radius;
    f32 total;
    f32 t;
    f32 angle;
    s32 segment;
    s32 x;
    s32 y;

    width = *(s16*)(item + 0x54);
    height = *(s16*)(item + 0x56);
    radius = SUMMARY_F32(lbl_8047B75C) * height * SUMMARY_F32(lbl_8047B770);
    total = SUMMARY_F32(lbl_8047B774) * (width + radius);
    stops[0] = SUMMARY_F32(lbl_8047B748);
    stops[1] = width / total;
    stops[2] = (width + radius) / total;
    stops[3] = (lbl_8047B774 * width + radius) / total;
    stops[4] = SUMMARY_F32(lbl_8047B744);

    for (segment = 0; segment < 4; segment++) {
        if (stops[segment] <= phase && stops[segment + 1] > phase) {
            break;
        }
    }

    t = (phase - stops[segment]) / (stops[segment + 1] - stops[segment]);
    if (segment == 0) {
        x = t * width;
        y = 0;
    }
    if (segment == 1) {
        angle = SUMMARY_F32(lbl_8047B75C) * t - SUMMARY_F32(lbl_8047B778);
        x = (height - SUMMARY_F32(lbl_8047B774)) * (f32)cos(angle) * SUMMARY_F32(lbl_8047B770) + width;
        y = height * SUMMARY_F32(lbl_8047B770) + (height - SUMMARY_F32(lbl_8047B774)) * (f32)sin(angle) * SUMMARY_F32(lbl_8047B770);
    }
    if (segment == 2) {
        x = height - SUMMARY_F32(lbl_8047B774);
        y = (lbl_8047B744 - t) * width;
    }
    if (segment == 3) {
        angle = SUMMARY_F32(lbl_8047B75C) * t + SUMMARY_F32(lbl_8047B778);
        x = (height - SUMMARY_F32(lbl_8047B774)) * (f32)cos(angle) * SUMMARY_F32(lbl_8047B770);
        y = height * SUMMARY_F32(lbl_8047B770) + (height - SUMMARY_F32(lbl_8047B774)) * (f32)sin(angle) * SUMMARY_F32(lbl_8047B770);
    }

    windowDrawSprite2(x, y, 2, 2, alpha | 0xFFFFFF00, (s32)owner, iconId, 0);
}
#pragma pop
#endif

#endif /* !GS_POKEMON_SUMMARY_ISLAND */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_80016ABC_ONLY)
/* Defined in the preceding region, which the 80016ABC carve leaves out. */
void menuPocketDrawMarqueeDot(u8* owner, u8* item, f32 phase, u16 iconId, u8 alpha);
/* menuPocketDrawMarqueeFrame - 0x80016ABC | size: 0x458 */
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2CC;
/* Move the list cursor sprite to the selected (or held) row and fade it at the edges. */
static inline void SummaryPlaceCursor(SummaryPageContext* ctx, u8* item) {
    s8 pos[2];
    s32 y;
    u8 alpha;

    *(u16*)pos = cursorBiosGetPos((u16)SummaryGetPage(ctx->pageIndex)->messageId) >> 16;
    if ((s32)lbl_8047A2E8 >= 0) {
        y = ((s32)lbl_8047A2E8 - pos[0]) * 0x1F + 0x95;
        if ((s32)lbl_8047A2C8 != 0) {
            y -= (s32)SUMMARY_F32(lbl_8047A2D0);
        }
        if (y + *(s16*)(item + 0x56) < 0x95 || y >= 0x18D) {
            alpha = 0;
        } else {
            alpha = 0xFF;
        }
    } else {
        y = pos[1] * 0x1F + 0x95;
        if ((s32)lbl_8047A2C8 == 0) {
            y += (s32)SUMMARY_F32(lbl_8047A2D0);
        }
        if ((s32)lbl_8047A2D8 == -1) {
            alpha = 0x72;
        } else {
            alpha = 0xFF;
        }
    }
    *(s16*)(item + 0x52) = y;
    item[0x67] = alpha;
}

/*
 * The gauge code uses plain float literals. Retail shares each one with the
 * rest of the summary TU's .sdata2 pool (lbl_8047B740..lbl_8047B784), so the
 * 80016ABC carve imports them (constant_import in configure.py). Literals, not
 * the pool labels, keep MWCC from CSE-ing the 1.0f wrap constant into an
 * early temporary, which is what gives retail's float register order.
 */
static inline f32 SummaryGaugePhase(f32 offset) {
    f32 phase = offset + SUMMARY_F32(lbl_8047A2CC);
    if (phase > 1.0f) {
        phase -= 1.0f;
    }
    return phase;
}

/* Draw one ring of 45 gauge segments starting at `phase`, fading in. */
static inline void SummaryDrawGaugeRing(SummaryPageContext* ctx, u8* item, f32 phase, f32 step) {
    s32 i;

    for (i = 0; i < 0x2D; i++) {
        menuPocketDrawMarqueeDot((u8*)ctx, item, phase, 0xD1, 255.0f * ((f32)i / 45.0f));
        phase += step;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}

/* Built with -opt nopeephole and -fp_contract off for the whole carve (configure.py). */
s32 menuPocketDrawMarqueeFrame(SummaryPageContext* ctx, u8* item) {
    f32 step;

    if ((s32)lbl_8047A2D8 != -1) {
        return 0;
    }
    if ((s32)lbl_8047A2E8 >= 0) {
        return 0;
    }

    SummaryPlaceCursor(ctx, item);
    fn_800FE6D0((s16)(*(s16*)((u8*)ctx + 0x84) + *(s16*)(item + 0x50)),
                (s16)(*(s16*)((u8*)ctx + 0x86) + *(s16*)(item + 0x52)));
    spriteSetEnv();

    /* 2 over the perimeter of the gauge's stadium outline (2 * width + pi * height). */
    step = 2.0f / (2.0f * ((f32)*(s16*)(item + 0x54) + 3.1415927f * (f32)*(s16*)(item + 0x56) / 2.0f));

    SummaryDrawGaugeRing(ctx, item, SUMMARY_F32(lbl_8047A2CC), step);

    SummaryDrawGaugeRing(ctx, item, SummaryGaugePhase(0.25f), step);

    SummaryDrawGaugeRing(ctx, item, SummaryGaugePhase(0.5f), step);

    SummaryDrawGaugeRing(ctx, item, SummaryGaugePhase(0.75f), step);

    return 0;
}

/* fn_80016F14 - 0x80016F14 | size: 0x114 */
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2D8;
#if 0
asm void fn_80016F14(void) {
#include "src/game/gs_pokemon_summary_fn_80016F14.inc"
}
#else
s32 fn_80016F14(SummaryPageContext* ctx, u8* item) {
    SummaryPlaceCursor(ctx, item);
    return 0;
}
#endif

/* fn_80017028 - 0x80017028 | size: 0x73c */
extern u8 lbl_80266B88[];
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2D4;
extern u32 lbl_8047A2D0;
extern void* lbl_8047A2F8;
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2C8;
extern const f32 lbl_8047B788;
extern const f32 lbl_8047B78C;
extern const f32 lbl_8047B750;
extern const f32 lbl_8047B790;
extern u32 lbl_8047A2DC;
#if 0
asm void fn_80017028(void) {
#include "src/game/gs_pokemon_summary_fn_80017028.inc"
}
#else
/* RULE-EXCEPTION(user-approved): retained mutation masks and reused constant
 * temporaries shape MWCC code generation; see docs/RULE_EXCEPTIONS.md. */
s32 fn_80017028(SummaryPageContext* ctx) {
  u32 fallbackLabels[5];
  s8 pos[2];
  s8 newPos[2];
  u8 *input;
  u8 *labelBase;
  u8 *kindBase;
  s32 cursorIndex;
  s32 validCount;
  s32 moved;
  u32 soundId;
  u32 label;
  s32 i;
  input = windowGetKeyInfo();
  moved = 0;
  soundId = 0;
  for (i = 0; i < 5; i++)
  {
    fallbackLabels[i] = ((u32 *) lbl_80266B88)[i];
  }

  if (((*((f32 *) (&lbl_8047B748))) != (*((f32 *) (&lbl_8047A2D4)))) || ((*((f32 *) (&lbl_8047B748))) != (*((f32 *) (&lbl_8047A2D0)))))
  {
    return 0;
  }
  labelBase = ((u8 *) ((const SummaryPageEntry *) lbl_80266918)) + 0x1C;
  *((u16 *) pos) = cursorBiosGetPos(((*((u32 *) (labelBase + (ctx->pageIndex * 0x4C)))) & 0xffff)) >> 16;
  kindBase = ((u8 *) ((const SummaryPageEntry *) lbl_80266918)) + 0x04;
  SummaryListCount(*((s32 *) (kindBase + (ctx->pageIndex * 0x4C))));
  i = 1;
  cursorIndex = pos[0] + pos[i];
  if (((s32) lbl_8047A2E8) < 0)
  {
    if ((((((*((u16 *) (input + 0x04))) & 0xC0) != 0) && (((s32) lbl_8047A2E0) != 3)) && (((s32) lbl_8047A2E0) != 4)) && ((*((s32 *) (((u8 *) ((const SummaryPageEntry *) lbl_80266918)->unk_08) + (ctx->pageIndex * 0x4C)))) != 0))
    {
      if (SummaryListGetItemId(*((s32 *) (kindBase + (ctx->pageIndex * 0x4C))), cursorIndex) != 0)
      {
        lbl_8047A2E8 = cursorIndex;
        soundId = 0x24;
      }
    }
  }
  else
    if (((*((u16 *) (((u8 *) input) + 0x04))) & 0xD0) != 0)
  {
    if (SummaryListGetItemId(*((s32 *) (kindBase + (ctx->pageIndex * 0x4C))), cursorIndex) != 0)
    {
      s32 kind = SummaryGetPage(ctx->pageIndex)->dataSource;
      s32 from = lbl_8047A2E8;
      if (kind == (-i))
      {
        label = (u16) from;
        fn_80129514(lbl_8047A2F8, label, (u16) cursorIndex);
      }
      else
      {
        fn_80129948(lbl_8047A2F8, ((kind) & 0xff), (u16) from, (u16) cursorIndex);
      }
      soundId = 0x24;
    }
    else
    {
      soundId = 0x25;
    }
    lbl_8047A2E8 = -i;
  }
  else
    if (((*((u16 *) (((u8 *) input) + 0x04))) & 0x20) != 0)
  {
    lbl_8047A2E8 = -i;
    soundId = 0x25;
  }
  *((u16 *) pos) = cursorBiosGetPos((u16) ((((((((((((*((u32 *) (labelBase + (ctx->pageIndex * 0x4C)))) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu)) >> 16;
  validCount = SummaryListCount(*((s32 *) (kindBase + (ctx->pageIndex * 0x4C)))) + 1;
  if ((((*((u16 *) (((u8 *) input) + 0x04))) | (*((u16 *) (((u8 *) input) + 0x08)))) & 2) != 0)
  {
    pos[i]++;
    if ((pos[i] + pos[0]) >= validCount)
    {
      pos[1]--;
    }
    else
    {
      if (pos[i] >= 8)
      {
        pos[0]++;
        pos[1]--;
        soundId = 0x23;
        lbl_8047A2C8 = i;
      }
      else
      {
        soundId = 0x23;
        lbl_8047A2C8 = 0;
      }
      *((f32 *) (&lbl_8047A2D0)) = *((f32 *) (&lbl_8047B788));
    }
    moved = i;
  }
  if ((((*((u16 *) (((u8 *) input) + 0x04))) | (*((u16 *) (((u8 *) input) + 0x08)))) & i) != 0)
  {
    if ((pos[1] > 0) || (pos[0] > 0))
    {
      if ((--pos[i]) < 0)
      {
        pos[1] = 0;
        pos[0]--;
        soundId = 0x23;
        lbl_8047A2C8 = i;
      }
      else
      {
        soundId = 0x23;
        lbl_8047A2C8 = 0;
      }
      *((f32 *) (&lbl_8047A2D0)) = *((f32 *) (&lbl_8047B78C));
    }
    moved = 1;
  }
  *((u16 *) newPos) = *((u16 *) pos);
  cursorBiosSetPos((u16) (*((u32 *) (labelBase + (ctx->pageIndex * 0x4C)))), (u16 *) newPos);
  if ((((s32) lbl_8047A2E8) < 0) && (moved == 0))
  {
    if (((*((u16 *) (((u8 *) input) + 0x06))) & 8) != 0)
    {
      if ((--ctx->pageIndex) < 1)
      {
        ctx->pageIndex = 5;
      }
      *((f32 *) (&lbl_8047A2D4)) = *((f32 *) (&lbl_8047B750));
    }
    if (((*((u16 *) (((u8 *) input) + 0x06))) & 4) != 0)
    {
      if ((++ctx->pageIndex) >= 6)
      {
        ctx->pageIndex = i;
      }
      *((f32 *) (&lbl_8047A2D4)) = *((f32 *) (&lbl_8047B790));
    }
  }
  if (soundId != 0)
  {
    soundStart(soundId, 0, 0xFF, 0);
  }
  if (((s32) lbl_8047A2E8) >= 0)
  {
    label = 0x2B2B;
  }
  else
  {
    u32 itemId;
    *((u16 *) pos) = cursorBiosGetPos((u16) (*((u32 *) (labelBase + (ctx->pageIndex * 0x4C))))) >> 16;
    cursorIndex = pos[0] + pos[1];
    itemId = SummaryListGetItemId(*((s32 *) (kindBase + (ctx->pageIndex * 0x4C))), cursorIndex);
    if (itemId != 0)
    {
      label = itemDataBiosGetDoc(itemDataBiosGetPtr(itemId));
    }
    else
    {
      label = fallbackLabels[lbl_8047A2E0];
    }
  }
  lbl_8047A2DC = label;
  return 0;
}
#endif

/* fn_80017764 - 0x80017764 | size: 0x2c */
extern void menuButtonNormal(void);
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2E8;

void fn_80017764(void) {
    if ((s32)lbl_8047A2E8 < 0) {
        menuButtonNormal();
    }
}

/* fn_80017790 - 0x80017790 | size: 0xd8 */
#if 0
asm void fn_80017790(void) {
#include "src/game/gs_pokemon_summary_fn_80017790.inc"
}
#else
s32 fn_80017790(u8* unused, SummaryDrawItem* item) {
    SummaryPageEntry* entry;
    s32 i;

    entry = (SummaryPageEntry*)sSummaryPageEntries;
    for (i = 0; i < 6; i++) {
        if ((s32)item->speciesId == entry->colorMatchId) {
            break;
        }
        entry++;
    }
    if (i >= 6) {
        return 0;
    }

    item->color[0] = SUMMARY_PAGE_DISPLAY_COLOR(i, 0);
    item->color[1] = SUMMARY_PAGE_DISPLAY_COLOR(i, 1);
    item->color[2] = SUMMARY_PAGE_DISPLAY_COLOR(i, 2);
    if (SUMMARY_PAGE_ENTRY_AT(i).dataSource == -1) {
        item->color[3] = 0;
    }

    return 0;
}
#endif

/* fn_80017868 - 0x80017868 | size: 0x84 */
extern u32 heroGetStatus(u32 a, s32 b, s32 c);
extern void* lbl_8047A2F8;
extern u32 lbl_8047A2E0;
#if 0
asm void fn_80017868(void) {
#include "src/game/gs_pokemon_summary_fn_80017868.inc"
}
#else
s32 fn_80017868(u32 unused, u8* ctx) {
    u32 r;
    r = heroGetStatus((u32)lbl_8047A2F8, 0xC, 0);
    if ((s32)lbl_8047A2E0 != 3) return 0;
    msgctrlSetValue(0x50, (s32)r);
    fn_800FB680((s32)*(s16*)(ctx + 0x54) - (s32)(s16)(GSmsgGetRect(0x151) >> 16), 0, -1, 0x151);
    return 0;
}
#endif

#endif /* 0x80016ABC - 0x800178EC */

#if !defined(GS_POKEMON_SUMMARY_ISLAND)
/* fn_800178EC - 0x800178EC | size: 0x28 */
extern u32 lbl_8047A2E0;
s32 fn_800178EC(u32 unused, u8* ptr) {
    if ((s32)lbl_8047A2E0 != 3) {
        *(u8*)(ptr + 0x67) = 0;
    } else {
        *(u8*)(ptr + 0x67) = 0xCC;
    }
    return 0;
}

/* fn_80017914 - 0x80017914 | size: 0x28 */
s32 fn_80017914(u32 unused, u8* ptr) {
    if ((s32)lbl_8047A2E0 != 3) {
        *(u8*)(ptr + 0x67) = 0;
    } else {
        *(u8*)(ptr + 0x67) = 0xFF;
    }
    return 0;
}

#endif /* !GS_POKEMON_SUMMARY_ISLAND */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_8001793C_ONLY)
/* fn_8001793C - 0x8001793C | size: 0x54 */
#if 0
asm void fn_8001793C(void) {
#include "src/game/gs_pokemon_summary_fn_8001793C.inc"
}
#else
s32 fn_8001793C(SummaryPageContext* ctx, SummaryDrawItem* item) {
    item->color[0] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 0);
    item->color[1] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 1);
    item->color[2] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 2);
    return 0;
}
#endif

/* fn_80017990 - 0x80017990 | size: 0x7c */
#if 0
asm void fn_80017990(void) {
#include "src/game/gs_pokemon_summary_fn_80017990.inc"
}
#else
s32 fn_80017990(SummaryPageContext* ctx, SummaryDrawItem* item) {
    s32 idx = SUMMARY_PAGE_INDEX(ctx);
    if ((s32)item->speciesId == SUMMARY_PAGE_ENTRY_AT(idx).gaugeMatchId) {
        item->color[0] = SUMMARY_PAGE_DISPLAY_COLOR(idx, 0);
        item->color[1] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 1);
        item->color[2] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 2);
        item->color[3] = 0xFF;
    } else {
        item->color[3] = 0;
    }
    return 0;
}
#endif

/* fn_80017A0C - 0x80017A0C | size: 0x2ac */
extern u32 lbl_8047A2D8;
extern void* lbl_8047A2F8;
extern const f32 lbl_8047B748;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2D4;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2C4;
extern u32 lbl_8047A2CC;
extern const f32 lbl_8047B794;
extern const f32 lbl_8047B798;
extern const f32 lbl_8047B74C;
extern const f32 lbl_8047B744;
extern const f32 lbl_8047B79C;
#if 0
asm void fn_80017A0C(void) {
#include "src/game/gs_pokemon_summary_fn_80017A0C.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80017A0C(u8* ctx) {
    s32 cursorIndex;

    switch (SUMMARY_CTX_S8(ctx, 0x01)) {
    case 0:
        if (SUMMARY_CTX_S8(ctx, 0x02) == 0) {
            winSeqSetMenu(0x59, 0x5E);

            cursorIndex = SummaryGetCursorIndex((s32)lbl_8047A2D8);
            lbl_8047A2DC = itemDataBiosGetDoc(
                itemDataBiosGetPtr((u16)SummaryListGetItemId(SUMMARY_PAGE_DATA_SOURCE(SUMMARY_CTX_S8(ctx, 0x95)), cursorIndex)));
            SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047B748));
            SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B748));
            lbl_8047A2C8 = 0;
            SUMMARY_STORE_F32(lbl_8047A2C4, SUMMARY_F32(lbl_8047B748));
            SUMMARY_STORE_F32(lbl_8047A2CC, SUMMARY_F32(lbl_8047B748));
            ctx[0x02] = 1;
        }
        break;

    case 2:
        if (SUMMARY_F32(lbl_8047A2D4) > SUMMARY_F32(lbl_8047B748)) {
            SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047A2D4) - SUMMARY_F32(lbl_8047B794));
            if (SUMMARY_F32(lbl_8047A2D4) < SUMMARY_F32(lbl_8047B748)) {
                SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047B748));
            }
        }
        if (SUMMARY_F32(lbl_8047A2D4) < SUMMARY_F32(lbl_8047B748)) {
            SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047A2D4) + SUMMARY_F32(lbl_8047B794));
            if (SUMMARY_F32(lbl_8047A2D4) > SUMMARY_F32(lbl_8047B748)) {
                SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047B748));
            }
        }
        if (SUMMARY_F32(lbl_8047A2D0) > SUMMARY_F32(lbl_8047B748)) {
            SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047A2D0) - SUMMARY_F32(lbl_8047B798));
            if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B748));
            }
        }
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047A2D0) + SUMMARY_F32(lbl_8047B798));
            if (SUMMARY_F32(lbl_8047A2D0) > SUMMARY_F32(lbl_8047B748)) {
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B748));
            }
        }

        SUMMARY_STORE_F32(lbl_8047A2C4, SUMMARY_F32(lbl_8047A2C4) + SUMMARY_F32(lbl_8047B74C));
        if (SUMMARY_F32(lbl_8047A2C4) > SUMMARY_F32(lbl_8047B744)) {
            SUMMARY_STORE_F32(lbl_8047A2C4, SUMMARY_F32(lbl_8047B748));
        }
        SUMMARY_F32(lbl_8047A2CC) += SUMMARY_F32(lbl_8047B79C);
        if (SUMMARY_F32(lbl_8047A2CC) >= lbl_8047B744) {
            SUMMARY_F32(lbl_8047A2CC) -= SUMMARY_F32(lbl_8047B744);
        }
        break;

    case 3:
        if (SUMMARY_CTX_S8(ctx, 0x02) == 0) {
            winSeqSetMenu(0x59, 0x62);
            ctx[0x02] = 1;
        }
        break;
    }

    return 0;
}
#pragma pop
#endif

#endif /* 0x8001793C - 0x80017CB8 */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_80017CB8_ONLY)
/* fn_80017CB8 - 0x80017CB8 | size: 0x1d4 */
extern void* lbl_8047A2F8;
extern u32 lbl_8047A2E0;
#if 0
asm void fn_80017CB8(void) {
#include "src/game/gs_pokemon_summary_fn_80017CB8.inc"
}
#else
/* The GS_POKEMON_SUMMARY_80017CB8_ONLY unit builds with -opt nopeephole and
   scheduling on (configure.py), the state these pragmas give the function. */
#if !defined(GS_POKEMON_SUMMARY_80017CB8_ONLY)
#pragma push
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local scheduling pragma — see docs/RULE_EXCEPTIONS.md */
#pragma scheduling on
#endif
s32 fn_80017CB8(SummaryMenuItem* out, s32 maxEntries, s32 pageIndex, s32 selectedIndex) {
    SummaryMenuItem* src;
    s32 i;
    s32 outCount;
    s32 count;
    s32 usable;
    s32 notEgg;
    u16 itemId;

    itemId = SummaryGetItemDataId(pageIndex, selectedIndex);
    itemDataBiosGetPtr(itemId);
    if (itemDataBiosGetBattleUseFunc() != 0) {
        usable = 1;
    } else {
        usable = 0;
    }
    if (itemId == 0x219) {
        notEgg = 0;
    } else {
        notEgg = 1;
    }

    outCount = 0;
    count = SUMMARY_PAGES[pageIndex].menus[(s32)lbl_8047A2E0].count;
    src = SUMMARY_PAGES[pageIndex].menus[(s32)lbl_8047A2E0].items;
    for (i = 0; i < count; i++) {
        if ((src->flags & 1) == 0 &&
            ((src->flags & 2) == 0 || usable != 0) &&
            ((src->flags & 8) == 0 || notEgg != 0)) {
            out->id = src->id;
            out->arg = src->arg;
            out->flags = 0;
            if (usable == 0 && (src->flags & 4) != 0) {
                out->flags |= 1;
            }
            outCount++;
            out++;
            if (outCount >= maxEntries) {
                break;
            }
        }
        src++;
    }

    return outCount;
}
#if !defined(GS_POKEMON_SUMMARY_80017CB8_ONLY)
#pragma pop
#endif
#endif

#endif /* 0x80017CB8 - 0x80017E8C */

#if !defined(GS_POKEMON_SUMMARY_ISLAND) || defined(GS_POKEMON_SUMMARY_80017E8C_ONLY)
/* fn_80017E8C - 0x80017E8C | size: 0x338 */
/* Argument block for the quantity picker menus 0x5B / 0x5C. */
typedef struct SummaryQuantityMenuArg {
    u8 color[3];
    s32 menuId;
    s32 max;
    s32 step;
    s32 unk_10;
} SummaryQuantityMenuArg;

extern void* lbl_8047A2F8;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2FC;
#if 0
asm void fn_80017E8C(void) {
#include "src/game/gs_pokemon_summary_fn_80017E8C.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local peephole-off pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80017E8C(s32 pageIndex, u16 species, s32 slotIndex) {
    SummaryQuantityMenuArg arg;
    s32 digits;
    s32 menuId;
    s32 mode;
    s32 quantity;
    s32 result;
    s32 menuResult;
    s32 removed;
    s32 held;
    s32 kind;
    const s32* src;
    s32 offset;

    if ((s32)(u8)fn_80143F9C(itemDataBiosGetPtr((u16)species)) == 0) {
        msgctrlSetValue(0x2D, species);
        winMsgOpen(2, 0x426C, 1, 0);
        winMsgClose(1);
        return 0;
    }

    /*
     * RULE-EXCEPTION(user-approved): row address walked to dataSource with a
     * pointer pre-increment, and the row offset named, to reproduce retail's
     * registers — see docs/RULE_EXCEPTIONS.md
     */
    offset = pageIndex * 0x4C;
    src = (const s32*)((const u8*)SUMMARY_PAGES + offset);
    kind = *++src;
    if (kind != 5 && kind != -1) {
        lbl_8047A2DC = 0x2B28;
        if (SummaryPocketTotal(kind) > 100) {
            mode = 3;
        } else {
            mode = 2;
        }

        held = SummaryListGetNum(*src, slotIndex);
        if (held < 1) {
            result = 0;
        } else {
            if (mode == 2) {
                digits = 1;
                menuId = 0x5B;
            } else {
                digits = 2;
                menuId = 0x5C;
            }
            arg.menuId = menuId;
            {
                const SummaryPageEntry* entry = SUMMARY_PAGES;
                entry = (const SummaryPageEntry*)((const u8*)entry + offset);
                arg.color[0] = entry->displayColor[0];
                arg.color[1] = entry->displayColor[1];
                arg.color[2] = entry->displayColor[2];
            }
            arg.step = 1;
            arg.max = held;
            arg.unk_10 = 0;
            lbl_8047A2FC = 1;
            menuResult = menuOpenCustom(menuId, windowGetActiveID(), &digits, 0, 1, 1, &arg);
            menuClose(menuId);
            menuCloseSync(menuId, 1);
            if (menuResult == -1) {
                result = -1;
            } else {
                result = lbl_8047A2FC;
            }
        }
        quantity = result;
    } else {
        quantity = 1;
    }

    if (quantity < 0) {
        return 0;
    }

    if ((u16)pcboxGetItemCapacity(0, species) < quantity) {
        removed = 0;
    } else {
        pcboxDelItem(0, species, quantity);
        removed = 1;
    }
    if (removed == 0) {
        msgctrlSetValue(0x2D, species);
        winMsgOpen(2, 0x2B49, 1, 0);
        winMsgClose(1);
        return 0;
    }

    if (*src == -1) {
        fn_8012959C(lbl_8047A2F8, species, quantity, slotIndex);
    } else {
        heroItemDecItemDataId(lbl_8047A2F8, species, quantity, slotIndex);
    }

    msgctrlSetValue(0x2D, species);
    msgctrlSetValue(0x2F, quantity);
    winMsgOpen(2, 0x4266, 1, 0);
    winMsgClose(1);
    return 1;
}
#pragma pop
#endif
#endif /* !GS_POKEMON_SUMMARY_ISLAND || GS_POKEMON_SUMMARY_80017E8C_ONLY */
